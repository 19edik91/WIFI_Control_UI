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
#ifndef _HAL_Config_H_
#define _HAL_Config_H_
    
#ifdef __cplusplus
extern "C"
{
#endif   

#include "BaseTypes.h"

/****************************************** Defines ******************************************************/
#define HAL_IO_ERRORCALLBACK        //Define a callback routine for errors in the HAL-IO-Module

/****************************************** ADC-Specific defines ******************************************************/
#define ADC_MAX_VAL                 1024u   //10bit ADC resolution
#define ADC_REF_MILLIVOLT           1000u   //1000mV reference Voltage
#define ADC_INPUT_CHANNEL0          A0
#define ADC_CHANNELS                1


/****************************************** Watchdog-Specific defines ******************************************************/


/****************************************** GPIO-Specific defines ******************************************************/



#ifdef __cplusplus
}
#endif    

#endif //_HAL_Config_H_

/* [] END OF FILE */
