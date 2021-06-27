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

#ifndef _AOM_TIME_H_
#define _AOM_TIME_H_

#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"
#include "Aom.h"

const tsCurrentTime* Aom_Time_GetCurrentTime(void);

void Aom_Time_SetReceivedTime(u8 ucHour, u8 ucMin, u32 ulTicks);
void Aom_Time_SetRealTimeClockTime(u8 ucHour, u8 ucMin, u32 ulTicks);
void Aom_Time_SetUserTimerSettings(tsTimeFormat* psUserTimerSettings, u8 ucTimerIdx);

#ifdef __cplusplus
}
#endif    

#endif //_AOM_TIME_H_