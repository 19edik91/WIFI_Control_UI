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
#define WDT_MAX_INTERVAL        CY_SYS_WDT_MATCH_MASK /* Is 0xFFFF -> 1.638s */
#define LFCLK_WCO_CYCLES_PER_SECOND (32768u)
#define LFCLK_ILO_CYCLES_PER_SECOND (CY_SYS_CLK_ILO_DESIRED_FREQ_HZ)

//The maximum time interval which can be used (in milliseconds)
#define WDT_TIME_LIMIT ((CY_SYS_WDT_MATCH_MASK * 1000)/LFCLK_ILO_CYCLES_PER_SECOND)

//Use of X-Macros for defining system timers
/*      Systemtimer name   |  SystemTimer Cnt    |   SystemTimer Mask   | SystemTimer Int   | SystemTimer Reset  |  Callback Fctn   */
#define WDT_LIST \
   SYST( eSysTimer0         ,    CY_SYS_TIMER0   , CY_SYS_TIMER0_MASK   , CY_SYS_TIMER0_INT , CY_SYS_TIMER0_RESET,  NULL            )\
   SYST( eSysTimer1         ,    CY_SYS_TIMER1   , CY_SYS_TIMER1_MASK   , CY_SYS_TIMER1_INT , CY_SYS_TIMER1_RESET,  NULL            )\
   SYST( eSysTimer2         ,    CY_SYS_TIMER2   , CY_SYS_TIMER2_MASK   , CY_SYS_TIMER2_INT , CY_SYS_TIMER2_RESET,  NULL            )


/****************************************** GPIO-Specific defines ******************************************************/

// Macro derived form PsoC generated source code
#define CY_PIN_MAPPING(name, check) {name ## __PS, name ## __DR, name ## __PC, name ## __SHIFT, name ## __PORT, check}
//Definitions: PS - PortPinStatusRegister | DR - PortOutputDataRegister | PC - PortConfigurationRegister

/* PWM_Definition                   Start_Fn          |     Stop_Fn            |       WriteCmp_Fn             |   ReadCmp_Fn                 |     ReadPeriod_Fn eriod value   |   WritePeriod_Fn                  | PWM_PWM_PERIOD_BUF_VALUE              |   PWM_0_PWM_COMPARE_BUF_VALUE */
#define CY_PWM_MAPPING(index) {PWM_ ## index ## _Start,  PWM_ ## index ## _Stop, PWM_ ## index ## _WriteCompare, PWM_ ## index ## _ReadCompare,     PWM_ ## index ## _ReadPeriod,   PWM_ ## index ## _WritePeriod   , PWM_ ## index ## _PWM_PERIOD_BUF_VALUE,   PWM_ ## index ## _PWM_COMPARE_BUF_VALUE  }
//    P_MAP(    ePWM_0    ,   PWM_0_Start      ,  PWM_0_Stop      ,       PWM_0_WriteCompare  ,   PWM_0_ReadCompare   ,     PWM_0_Init)

#define COUNT_OF_SENSELINES     eInvalidSense
#define COUNT_OF_OUTPUTS        eInvalidOutput
#define COUNT_OF_PWM            eInvalidPWM
#define COUNT_OF_PORT_ISR       eInvalidPort
#define NUMBER_OF_PORTS          7u
#define NUMBER_OF_MAX_PORT_PINS  8u
#define OUTPUT_FAULT_COUNT       10u

//Xmacro for sense lines
    /*      Connector name  |       Pin mapping   |                       Check Pin   */
#define SENSE_MAP\
    S_MAP(    eSenseVoltage_0       ,   CY_PIN_MAPPING(Pin_VoltageMeasure__0, false))\
    S_MAP(    eSenseVoltage_1       ,   CY_PIN_MAPPING(Pin_VoltageMeasure__1, false))\
    S_MAP(    eSenseVoltage_2       ,   CY_PIN_MAPPING(Pin_VoltageMeasure__2, false))\
    S_MAP(    eSenseVoltage_3       ,   CY_PIN_MAPPING(Pin_VoltageMeasure__3, false))\
    S_MAP(    eSenseCurrent_0       ,   CY_PIN_MAPPING(Pin_CurrentMeasure__0, false))\
    S_MAP(    eSenseCurrent_1       ,   CY_PIN_MAPPING(Pin_CurrentMeasure__1, false))\
    S_MAP(    eSenseCurrent_2       ,   CY_PIN_MAPPING(Pin_CurrentMeasure__2, false))\
    S_MAP(    eSenseCurrent_3       ,   CY_PIN_MAPPING(Pin_CurrentMeasure__3, false))\
    S_MAP(    eSenseTemperature_0   ,   CY_PIN_MAPPING(Pin_NTC_IN__0,         false))\
    S_MAP(    eSenseTemperature_1   ,   CY_PIN_MAPPING(Pin_NTC_IN__1,         false))\
    S_MAP(    eSenseTemperature_2   ,   CY_PIN_MAPPING(Pin_NTC_IN__2,         false))\
    S_MAP(    eSenseTemperature_3   ,   CY_PIN_MAPPING(Pin_NTC_IN__3,         false))\
    S_MAP(    eSensePIR             ,   CY_PIN_MAPPING(Pin_PIR__0,            false))

#define OUTPUT_MAP\
    O_MAP(    ePin_PwmOut_0         ,   CY_PIN_MAPPING(Pin_PwmOut_0__0,         false))\
    O_MAP(    ePin_PwmOut_1         ,   CY_PIN_MAPPING(Pin_PwmOut_1__0,         false))\
    O_MAP(    ePin_PwmOut_2         ,   CY_PIN_MAPPING(Pin_PwmOut_2__0,         false))\
    O_MAP(    ePin_PwmOut_3         ,   CY_PIN_MAPPING(Pin_PwmOut_3__0,         false))\
    O_MAP(    ePin_VoltEn_0         ,   CY_PIN_MAPPING(Pin_VOLT_EN__0,          true) )\
    O_MAP(    ePin_VoltEn_1         ,   CY_PIN_MAPPING(Pin_VOLT_EN__1,          true) )\
    O_MAP(    ePin_VoltEn_2         ,   CY_PIN_MAPPING(Pin_VOLT_EN__2,          true) )\
    O_MAP(    ePin_VoltEn_3         ,   CY_PIN_MAPPING(Pin_VOLT_EN__3,          true) )\
    O_MAP(    ePin_PwmEn_0          ,   CY_PIN_MAPPING(Pin_PWM_EN__0,           true) )\
    O_MAP(    ePin_PwmEn_1          ,   CY_PIN_MAPPING(Pin_PWM_EN__1,           true) )\
    O_MAP(    ePin_PwmEn_2          ,   CY_PIN_MAPPING(Pin_PWM_EN__2,           true) )\
    O_MAP(    ePin_PwmEn_3          ,   CY_PIN_MAPPING(Pin_PWM_EN__3,           true) )\
    O_MAP(    ePin_LedGreen         ,   CY_PIN_MAPPING(Pin_LED_G__0,            true) )\
    O_MAP(    ePin_LedRed           ,   CY_PIN_MAPPING(Pin_LED_R__0,            true) )\
    O_MAP(    ePin_EspResetPin      ,   CY_PIN_MAPPING(Pin_ESP_Reset__0,        false))\
    O_MAP(    ePin_EspResetCopy     ,   CY_PIN_MAPPING(Pin_ESP_COPY__0,         false))

    /* PWM-X-Macro table*/    
#define PWM_MAP\
    P_MAP(ePWM_0,   CY_PWM_MAPPING(0))\
    P_MAP(ePWM_1,   CY_PWM_MAPPING(1))\
    P_MAP(ePWM_2,   CY_PWM_MAPPING(2))\
    P_MAP(ePWM_3,   CY_PWM_MAPPING(3))

/*              PORT        |   Pin_0-Callback  |   Pin_1-Callback  |   Pin_2-Callback  |   Pin_3-Callback  |   Pin_4-Callback  |   Pin_5-Callback  |   Pin_6-Callback  |   Pin_7-Callback  |*/
#define ISR_IO_MAP\
    ISR_MAP(    ePort_0     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_1     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_2     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_3     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_4     ,       DR_Regulation_RxInterruptOnSleep        ,       NULL        ,       NULL        , DR_Regulation_CheckSensorForMotion  ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_5     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_6     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )\
    ISR_MAP(    ePort_7     ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        ,       NULL        )


#ifdef __cplusplus
}
#endif    

#endif //_HAL_Config_H_

/* [] END OF FILE */
