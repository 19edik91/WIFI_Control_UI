/*
 * OS_SoftwareTimer_Config.h
 *
 *  Created on: 07.04.2021
 *      Author: kraemere
 */

#ifndef OS_CONFIG_H_
#define OS_CONFIG_H_
    
#define MAX_EVENT_TIMER 10

#define SW_TIMER_2MS        2u
#define SW_TIMER_10MS       10u
#define SW_TIMER_51MS       51u
#define SW_TIMER_251MS      251u
#define SW_TIMER_1001MS     1001u
#define SW_TIMER_5001MS     5001u
    
    
#define EVT_SW_TIMER_2MS        (TIMER_TICK_OFFSET + SW_TIMER_2MS)
#define EVT_SW_TIMER_10MS       (TIMER_TICK_OFFSET + SW_TIMER_10MS)
#define EVT_SW_TIMER_51MS       (TIMER_TICK_OFFSET + SW_TIMER_51MS)
#define EVT_SW_TIMER_251MS      (TIMER_TICK_OFFSET + SW_TIMER_251MS)
#define EVT_SW_TIMER_1001MS     (TIMER_TICK_OFFSET + SW_TIMER_1001MS)
#define EVT_SW_TIMER_5001MS     (TIMER_TICK_OFFSET + SW_TIMER_5001MS)
    
#ifdef CY_PROJECT_NAME
#define EnterCritical()   CyEnterCriticalSection()
#define LeaveCritical(x)  CyExitCriticalSection(x)
#else
#define EnterCritical()   3; noInterrupts()
#define LeaveCritical(x)  interrupts()
#endif
    
//#define TMGR_PostEvent(param1,param2) EVT_PostEvent(eEvtTimer,(uiEventParam1)param1,(ulEventParam2)param2)

#endif /* OS_CONFIG_H_ */
