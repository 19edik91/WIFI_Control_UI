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

#ifndef _AOM_REGULATION_H_
#define _AOM_REGULATION_H_

#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"
#include "Aom.h"

const tRegulationValues* Aom_Regulation_GetRegulationValuesPointer(void);

void Aom_Regulation_SetCustomValue(u8 ucBrightnessValue, bool bLedStatus, bool bInitMenuActive, bool bAutomaticModeStatus, u8 ucOutputIdx);
void Aom_Regulation_CheckRequestValues(u8 ucBrightnessValue, bool bLedStatus, bool bNightModeOnOff, bool bMotionDetectionOnOff,
                           u8 ucBurnTime, bool bInitMenuActive, bool bAutomaticModeStatus, u8 ucOutputIdx);
bool Aom_Regulation_CompareCustomValue(u8 ucBrightnessValue, bool bLedStatus, u8 ucOutputIdx);
void Aom_Regulation_SetCalculatedVoltageValue(tRegulationValues* psRegulationValues);
void Aom_Regulation_GetRegulationValues(tRegulationValues* psRegulationValues);
              
void Aom_Regulation_SetMinSystemSettings(u16 uiCurrentAdc, u16 uiVoltageAdc, u16 uiCompVal, u8 ucOutputIdx);
void Aom_Regulation_SetMaxSystemSettings(u16 uiCurrentAdc, u16 uiVoltageAdc, u16 uiCompVal, u8 ucOutputIdx);
              

#ifdef __cplusplus
}
#endif    

#endif //_AOM_REGULATION_H_