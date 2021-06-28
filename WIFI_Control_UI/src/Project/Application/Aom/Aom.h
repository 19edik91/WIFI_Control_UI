/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef _AOM_H_
#define _AOM_H_

#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"
#include "Project_Config.h"

typedef struct
{
    u8 ucPercentValue;
    bool bStatus;
}tLedValue;

typedef struct
{
    tLedValue sLedValue[DRIVE_OUTPUTS];
    u8   ucBurnTime;
    bool bMainPageActive;
    bool bAutomaticModeStatus;
    bool bNightModeOnOff;
    bool bMotionDetectionOnOff;
}tRegulationValues;

typedef struct
{
    u8 ucMinValue;
    u8 ucMaxValue;
    u8 ucSliderValue;
    bool bOnOff;
}tsBrightnessValue;
 
typedef struct
{
    tsBrightnessValue sBrightnessValue[DRIVE_OUTPUTS];
}tsOutputValues;


typedef struct
{
    char sTime[3];
    u8 ucTime;
}tsTime;

typedef struct
{
    tsTime sStartHour[USER_TIMER_AMOUNT];
    tsTime sStartMin[USER_TIMER_AMOUNT];
    tsTime sStopHour[USER_TIMER_AMOUNT];
    tsTime sStopMin[USER_TIMER_AMOUNT];
    u8 ucTimerIndex;
    u8 ucTimerAmountSet; //Bitwise set 
    u8 ucEndurance; //Leuchtdauer
    bool bNightModeOnOff;
    bool bMotionDetectOnOff;
    bool bAutomaticOnOff;
}tsAutomaticValues;

typedef struct
{
    u32 ulEpochTick;
    u16 uiYear;
    u8 ucMonth;
    u8 ucDay;
    u8 ucHour;
    u8 ucMinute;
    u8 ucSecond;
    bool bDST;
}tsDate;

typedef struct
{
    u16 uiVoltageAdc;
    u16 uiCurrentVal;
    u16 uiRegulationVal;
}tsAdcValues;

void Aom_InitStructuresWithDefaultValues(void);
tsOutputValues* Aom_OutputValuePointer(void);
tsAutomaticValues* Aom_AutomaticPointer(void);
u8 Aom_SetCustomValue(u8 ucBrightnessValue, u8 ucOutputIndex, bool bLedStatus, bool bRootPageActive);
void Aom_UpdateOutputStatus(u8 ucBrightnessVal, u8 ucOutputIndex, bool bLedStatus, bool bNightModeOnOff, bool bMotionDetected, s32 slRemainBurnTime);
bool Aom_SetNightModeValue(bool bNightModeOnOff);
bool Aom_SetMotionDetectionModeValue(bool bMotionDetectOnOff, u8 ucBurnTime);
bool Aom_SetAutomaticModeValue(bool bAutomaticModeOnOff);
const tRegulationValues* Aom_GetCustomValue(void);
void Aom_SetNewMeasureValues(u8 ucOutputIndex, u32 ulVolt, u16 uiCurr, s16* psiTemp);
void Aom_GetMeasureValues(u8 ucOutputIndex, u32* pulVolt, u16* puiCurrent, s16* psiTemp, u32* pulPower);
bool Aom_SettingsInitDone(void);
void Aom_SetSavedUserSettings(u8 ucOutputIndex, u8 ucBrightness, bool bOnOff, bool bAutomaticModeOnOff, bool bNightModeOnOff, bool bMotionDetectOnOff, u8 ucBurnTime );
void Aom_SettingsMsgReceived(bool bTimerSettings, bool bOutputSettings, u8 ucOutputIdx);
bool Aom_GetSettingsMsgReceived(void);
void Aom_SetSettingsInitDone(void);
//bool Aom_IsStandbyActive(void);
void Aom_SetActualTime(u8 ucHours, u8 ucMinutes, u32 ulLocTicks);
void Aom_GetActualTime(u8* pucHours, u8* pucMinutes, u32* pulLocTicks);
void Aom_SetDate(tsDate* psDate);
const tsDate* Aom_GetDate(void);
void Aom_GetBurningTimeValues(s32* pslRemainingTime, s32* pslInitialTime);
void Aom_SetAdcValues(tsAdcValues* psAdcValue);
bool Aom_GetAdcValues(tsAdcValues* psAdcValue);

#ifdef __cplusplus
}
#endif    

#endif //_AOM_H_
