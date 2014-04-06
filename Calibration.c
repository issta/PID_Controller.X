#include "Calibration.h"
#include "Orientation.h"
#include "PID_Algorithm.h"

//******************************************************************************
//Public Variable Declarations
//******************************************************************************
INT16 Calibration_Gyro_Offset_X = 0;
INT16 Calibration_Gyro_Offset_Y = 0;
INT16 Calibration_Gyro_Offset_Z = 0;

float Calibration_Yaw_Zero_Point = 0;

//******************************************************************************
//Public Function Definitions
//******************************************************************************
void Calibration_sensorsOffsets()
{
    Calibration_Gyro_Offset_X = L3G4200D_XAngularRate_Raw_Avg;
    Calibration_Gyro_Offset_Y = L3G4200D_YAngularRate_Raw_Avg;
    Calibration_Gyro_Offset_Z = L3G4200D_ZAngularRate_Raw_Avg;
}

void Calibration_zeroYaw()
{
    Calibration_Yaw_Zero_Point = scaledOrientationYaw;
}
