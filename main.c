/*******************************************************************************
 * Programmer: Ryan Moniz and Michael Taylor                                   *
 * Class: EE 416                                                               *
 * Lab Project: Motor Controller for PID Controller for Senior Design          *
 * Last Update: March 5, 2014                                                  *
 * Description: This program is designed to run the control of the motors for  *
 *              Team Naan's senior design project, which is to impelement a PID*
 *              algorithm to control four motors (quadcopter).                 *
 ******************************************************************************/
#include "RcRx.h"
#include "MotorPID.h"
#include "MotorCtrl.h"
#include "CommunicationLoop.h"
#include "ADXL362.h"
#include "L3G4200D.h"
#include "FIFOSPI2.h"
#include "HMC5883L.h"
#include "OrientationLoop.h"
#include "setup.h"
#include "Calibration.h"
#include <xc.h>


// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FMIIEN = ON              // Ethernet RMII/MII Enable (MII Enabled)
#pragma config FETHIO = ON              // Ethernet I/O Pin Select (Default Ethernet I/O)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)
// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_12        // USB PLL Input Divider (12x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = ON              // Watchdog Timer Enable (WDT Enabled)
// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)



int main (void)
{
    //Configures the board for operation
    Setup_initializeBoard();

    //Blocking Call// waits for RC controller to zero
    RcRx_zeroController();

    //Starts the various communication protocols
    Setup_communications();

    //Calibrates the on-board sensors
    Calibration_sensorsOffsets();

    //Blocking Call// waits for a startup sequnce on the RC controller
    RcRx_enableProps();

    //Starts the PID Roll/Pitch/Yaw calculation and the PID based on it
    OrientationLoop_start();

    //Starts the motors
    MotorCtrl_startupMotors();

    while (1)
    {
        if (RcRx_disableProps())
        {
            OC1RS = OC_KILL;
            OC2RS = OC_KILL;
            OC3RS = OC_KILL;
            OC4RS = OC_KILL;

            MotorCtrl_OC_ONE = OC_MIN;
            MotorCtrl_OC_TWO = OC_MIN;
            MotorCtrl_OC_THREE = OC_MIN;
            MotorCtrl_OC_FOUR = OC_MIN;

            T5CONCLR = 0x8000;

            RcRx_enableProps();

            T5CONSET = 0x8000;

            yawFlag = 0;
            MotorPID_determineZeroYaw();

            MotorCtrl_startupMotors();
        }
        
        //Update OCxRS registers with newly calculated values
        OC1RS = MotorCtrl_OC_ONE;     //Front-Left Motor
        OC2RS = MotorCtrl_OC_TWO;     //Front-Right Motor
        OC3RS = MotorCtrl_OC_THREE;   //Back-Right Motor
        OC4RS = MotorCtrl_OC_FOUR;    //Back-Left Motor

        /*OC_ONE_PC = 100.0 * (float)(OC_ONE - OC_KILL) / (float)(OC_MAX - OC_KILL);
        OC_TWO_PC = 100.0 * (float)(OC_TWO - OC_KILL) / (float)(OC_MAX - OC_KILL);
        OC_THREE_PC = 100.0 * (float)(OC_THREE - OC_KILL) / (float)(OC_MAX - OC_KILL);
        OC_FOUR_PC = 100.0 * (float)(OC_FOUR - OC_KILL) / (float)(OC_MAX - OC_KILL);*/
        MotorCtrl_OC_ONE_PC = MotorCtrl_OC_ONE;
        MotorCtrl_OC_TWO_PC = MotorCtrl_OC_TWO;
        MotorCtrl_OC_THREE_PC = MotorCtrl_OC_THREE;
        MotorCtrl_OC_FOUR_PC = MotorCtrl_OC_FOUR;
    }

    return 0;
}
