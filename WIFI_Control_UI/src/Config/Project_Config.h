/*
 * Project config.h
 *
 *  Created on: 07.04.2021
 *      Author: kraemere
 */

#ifndef PROJECT_CONFIG_H_
#define PROJECT_CONFIG_H_

//Set this define to true when the system shall work without the
//regulation handling.
#define WITHOUT_REGULATION      0
    
#define TARGET_SYSTEM_VOLTAGE   12000   //Voltage in milli volt
    
#ifndef DRIVE_OUTPUTS
    #define DRIVE_OUTPUTS           1       //Amount of the outputs used in the system
#else 
    #error Drive outputs arent set
#endif

#define PERCENT_LOW              10      //5% is lowest value 
#define PERCENT_HIGH             100    //100% is highest value
#define PERCENT_STEPS            10      //Each step is done in 5%

//#define CURRENT_LOW_LIMIT        20  //20mA should be the low limit
#define CURRENT_MAX_LIMIT        2000   //2000mA is the maximum limit
#define VOLTAGE_STEP             10     //10mV voltage steps

#define SAVE_IN_FLASH_TIMEOUT    30000  //Timeout until the new value is saved in the flash (in ms)

#define ENABLE_FAST_STANDBY     false
#if ENABLE_FAST_STANDBY
    #warning FAST_STANDBY_ENABLED
#endif

#if ENABLE_FAST_STANDBY
    #define TIMEOUT_ENTER_STANDBY    10000  //Timeout in milliseconds until the standby is entered
#else
    #define TIMEOUT_ENTER_STANDBY    60000  //Timeout in milliseconds until the standby is entered
#endif
    
#define USER_TIMER_AMOUNT        4      //Amount of possible user timers

/****   Defines for error detection module ****************************************************************************/
#define PWM_TEST_RETRY 10
#define MAX_MILLI_CURRENT_VALUE 2000    //Maximum current value in mA
#define MAX_AMBIENT_TEMPERATURE 650     //65.0°C

/********************************************************************************/

//Use of X-Macros for defining errors
/*              Error name                          |  Fault Code    |   Priority   |         Debouncer Count          */
#define USER_ERROR_LIST \
   ERROR(   eOutputVoltageFault_0                    ,    0xA001      ,       3     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOutputVoltageFault_1                    ,    0xA002      ,       3     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOutputVoltageFault_2                    ,    0xA003      ,       3     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOutputVoltageFault_3                    ,    0xA004      ,       3     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverCurrentFault_0                      ,    0xA005      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverCurrentFault_1                      ,    0xA006      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverCurrentFault_2                      ,    0xA007      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverCurrentFault_3                      ,    0xA008      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eLoadMissingFault_0                      ,    0xA009      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eLoadMissingFault_1                      ,    0xA00A      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eLoadMissingFault_2                      ,    0xA00B      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eLoadMissingFault_3                      ,    0xA00C      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverTemperatureFault_0                  ,    0xA010      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverTemperatureFault_1                  ,    0xA011      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverTemperatureFault_2                  ,    0xA012      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eOverTemperatureFault_3                  ,    0xA013      ,       2     ,      FAULTS_DEBOUNCE_CNT_DEFAULT )\
   ERROR(   eCommunicationTimeoutFault               ,    0xA014      ,       2     ,      1                           )\
   ERROR(   eMuxInvalid                              ,    0xA015      ,       1     ,      1                           )

#define USER_EVENT_LIST \
    eEvtAutomaticMode_ResetBurningTimeout,\
    eEvtCommTimeout,\
    eEvtTimeReceived,\
    eEvtStandby,\
    eEvtNewRegulationValue,\
    eEvtInitRegulationValue,\
    eEvtStandby_WakeUpReceived,\
    eEvtStandby_RxToggled,\
    eEvtManualInitHardware,\
    eEvtAutoInitHardware,\
    eEvtUserTimerReceived,\
    
#define USER_EVENTPARAM_LIST \
    eEvtParam_ManualInitSetMinValue,\
    eEvtParam_ManualInitSetMaxValue,\
    eEvtParam_ManualInitDone,\
    eEvtParam_AutoInitDone,\
    eEvtParam_AutoInitStart,\
    
#endif /* PROJECT_CONFIG_H_ */
