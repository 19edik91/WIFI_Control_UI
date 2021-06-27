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

#ifndef _AOM_FLASH_H_
#define _AOM_FLASH_H_

#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"
#include "Aom.h"

void Aom_Flash_WriteUserSettingsInFlash(void);
void Aom_Flash_ReadUserSettingsFromFlash(void);
void Aom_Flash_WriteSystemSettingsInFlash(void);
u8   Aom_Flash_ReadSystemSettingsFromFlash(void);

#ifdef __cplusplus
}
#endif    

#endif //_AOM_FLASH_H_