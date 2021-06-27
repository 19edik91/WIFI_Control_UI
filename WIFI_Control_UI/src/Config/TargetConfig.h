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
#ifndef _TARGET_CONFIG_H_
#define _TARGET_CONFIG_H_
    
#ifdef __cplusplus
extern "C"
{
#endif   

/* Choose target config */
//#define CYPRESS_PSOC4100S_PLUS

#define ESPRESSIF_ESP8266

#if defined(CYPRESS_PSOC4100S_PLUS) && defined(ESPRESSIF_ESP8266)
    #error ONLY ONE TARGET CAN BE USED!
#endif


#ifdef __cplusplus
}
#endif    

#endif //_TARGET_CONFIG_H_

/* [] END OF FILE */
