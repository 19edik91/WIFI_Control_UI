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

#ifndef _AOM_MEASURE_H_
#define _AOM_MEASURE_H_

#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"
#include "Aom.h"

u32 Aom_Measure_CalculateSystemVoltage(void);
bool Aom_Measure_SystemVoltageCalculated(void);

void Aom_Measure_GetMeasuredValues(u32* pulVoltage, u16* puiCurrent, s16* psiTemperature, u8 ucOutputIdx);
void Aom_Measure_SetMeasuredValues(bool bVoltage, bool bCurrent, bool bTemperature);
void Aom_Measure_SetActualAdcValues(u16 uiAdcVal, teMeasureType eChannel, u8 ucChannelIdx);

u16 Aom_Measure_GetAdcRequestedValue(u8 ucOutputIdx);
u16 Aom_Measure_GetAdcIsValue(teMeasureType eChan, u8 ucOutputIdx);
u16 Aom_Measure_GetMeasuredCurrentAdcValue(u8 ucOutputIdx);
u16 Aom_Measure_GetAdcVoltageStepValue(void);
u16 Aom_Measure_GetAdcCurrentLimitValue(void);


#ifdef __cplusplus
}
#endif    

#endif //_AOM_MEASURE_H_