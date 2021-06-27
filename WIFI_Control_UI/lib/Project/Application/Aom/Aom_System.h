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

#ifndef _AOM_SYSTEM_H_
#define _AOM_SYSTEM_H_

#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"
#include "Aom.h"

tsAutomaticModeValues* Aom_System_GetAutomaticModeValuesStruct(void);

bool Aom_System_GetSystemStarted(void);
bool Aom_System_StandbyAllowed(void);
bool Aom_System_GetSlaveResetState(void);

void Aom_System_SetSystemStarted(bool bStarted);
void Aom_System_SetSlaveResetState(bool bResetActive);

#ifdef __cplusplus
}
#endif    

#endif //_AOM_SYSTEM_H_