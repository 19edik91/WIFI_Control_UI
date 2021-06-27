


/********************************* includes **********************************/
#include "OS_State_Reset.h"
#include "OS_StateManager.h"
#include "OS_SoftwareTimer.h"
#include "OS_Serial_UART.h"
#include "OS_ErrorDebouncer.h"
#include "OS_RealTimeClock.h"

#include <project.h>

#include "DR_ErrorDetection.h"
#include "DR_Measure.h"
#include "DR_Regulation.h"

#include "MessageHandler.h"

#include "Aom_Flash.h"
#include "Aom_System.h"
#include "Aom_Regulation.h"
#include "Aom_Measure.h"
#include "Aom_Time.h"

#include "AutomaticMode.h"
#include "State_Standby.h"

/***************************** defines / macros ******************************/
#define NIGHT_MODE_START        22
#define NIGHT_MODE_STOP         5

#define STDBY_MSG_TIMEOUT      3000   //3 sec for reset timeout
#define RESET_CTRL_TIMEOUT     3000     // 3 sec timeout for ESP reset

/************************ local data type definitions ************************/

/************************* local function prototypes *************************/

/************************* local data (const and var) ************************/
static bool bStandbyAllowed = true;
static bool bSlaveReseted = false;

static u8 ucSW_Timer_MsgRxTimeout = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_EspResetTimeout = INVALID_TIMER_INDEX;
/************************ export data (const and var) ************************/


/****************************** local functions ******************************/
//***************************************************************************
/*!
\author     KraemerE
\date       07.05.2021
\brief      Allow to enter standby state again.
\return     none
\param      none
******************************************************************************/
static void UnblockStandbyState(void)
{
    bStandbyAllowed = true;
}

//***************************************************************************
/*!
\author     KraemerE
\date       07.05.2021
\brief      Timeout for slave reset.
\return     none
\param      none
******************************************************************************/
static void ResetSlaveByTimeout(void)
{
    /* unleash the reset state */
    DR_Regulation_SetEspResetStatus(false);
    
    /* Reset AOM-System status */
    Aom_System_SetSystemStarted(false);
    
    bSlaveReseted = true;
}

//********************************************************************************
/*!
\author  KraemerE
\date    18.02.2019
\brief   In this state the CPU shall only wait for a message from the UIM,
         do some self-tests and maybe go to sleep.
\param   none
\return  none
***********************************************************************************/
static void EnterSleepMode(void)
{    
    /* Check if transmision is done */
    if(OS_Serial_UART_TransmitStatus() == true)
    {
        /* Enable wake-up sources before critical section is entered */
        DR_Regulation_SetWakeupInterrupts();
        
        /* Enter critical section */
        u8 ucInterruptStatus = EnterCritical();
        
        /* Enable wake up sources from deep sleep or set UART in sleep mode */
        if(OS_Serial_UART_EnableUartWakeupInSleep())
        {                
            /* Put modules like Timers, ADC and so on in sleep mode */
            DR_Regulation_ModulesSleep();
            
            DR_Regulation_EnterDeepSleepMode();
                           
            /* CPU woke up here - Disable wake up sources / Enable UART again */
            OS_Serial_UART_DisableUartWakeupInSleep();
            
            /* Wake up modules like Timers, ADC and so from sleep mode */
            DR_Regulation_ModulesWakeup(); 
        }
        
        /* Enter critical section */
        LeaveCritical(ucInterruptStatus);
        
        /* Disable wake-up sources after critical section is left */
        DR_Regulation_DeleteWakeupInterrupts();        
    }
}

/************************ externally visible functions ***********************/
//***************************************************************************
/*!
\author     KraemerE
\date       30.04.2021
\brief      Entry state of this state.
\return     u8
\param      eEventID - Event which shall be handled while in this state.
\param      uiParam1 - Event parameter from the received event
\param      ulParam2 - Second event parameter from the received event
******************************************************************************/
u8 State_Standby_Entry(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    /* No events expected in this state */
    u8 ucReturn = EVT_PROCESSED;

    /* Make compiler happy */
    (void)uiParam1;
    (void)ulParam2;
    
    switch(eEventID)
    {
        default:
            ucReturn = EVT_NOT_PROCESSED;
            break;
    }
    
    /* Send sleep message */
    MessageHandler_SendSleepOrWakeUpMessage(true);
    bSlaveReseted = false;
    
    /* Creat async timer. When a message was received the standby state shall be blocked for a 
       dependent time. Maybe the standby state shall be left */
    OS_SW_Timer_CreateAsyncTimer(&ucSW_Timer_MsgRxTimeout, STDBY_MSG_TIMEOUT, eSwTimer_CreateSuspended, UnblockStandbyState);
    OS_SW_Timer_CreateAsyncTimer(&ucSW_Timer_EspResetTimeout, RESET_CTRL_TIMEOUT, eSwTimer_CreateSuspended, ResetSlaveByTimeout);
    
    /* Disable measurement module */
    //Warning when SelfTest_ADC is enabled. This would leave to an stopped CPU.
    DR_Measure_Stop();    
    
    /* Unblock standby state from previous state */
    UnblockStandbyState();
    
    /* Switch state to root state */
    OS_StateManager_CurrentStateReached();

    return ucReturn;
}


//***************************************************************************
/*!
\author     KraemerE
\date       30.04.2021
\brief      Entry state of this state.
\return     u8
\param      eEventID - Event which shall be handled while in this state.
\param      uiParam1 - Event parameter from the received event
\param      ulParam2 - Second event parameter from the received event
******************************************************************************/
u8 State_Standby_Root(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_PROCESSED;
    
    switch(eEventID)
    {       
        case eEvtStandby_RxToggled:
        {
            //Message in standby state received. Set timeout until standby can be re-entered again.
            bStandbyAllowed = false;
            OS_SW_Timer_SetTimerState(ucSW_Timer_MsgRxTimeout, eSwTimer_StatusRunning);
            break;
        }
                                        
        case eEvtAutomaticMode_ResetBurningTimeout:
        {
            /* Motion sensor has detected motion */
            AutomaticMode_ResetBurningTimeout();
            
            /* Check if standby mode can be left */
            if(AutomaticMode_LeaveStandbyMode())
            {
                bStandbyAllowed = false;
                OS_EVT_PostEvent(eEvtState_Request, eSM_State_Active, 0);
            }
            break;
        }
        
        case eEvtStandby_WakeUpReceived:
        {
            /* Send a wake-up-message */
            MessageHandler_SendSleepOrWakeUpMessage(false);
            bStandbyAllowed = false;
            OS_EVT_PostEvent(eEvtState_Request, eSM_State_Active, 0);
            break;
        }
        
        case eEvtNewRegulationValue:
        case eEvtTimeReceived:
        case eEvtSerialMsgReceived:
        {
            /* Send sleep message */
            MessageHandler_SendSleepOrWakeUpMessage(true);
            
            /* Request state change*/
            //OS_EVT_PostEvent(eEvtState_Request, eSM_State_Active, 0);
            break;
        }
        
        default:
            ucReturn = EVT_NOT_PROCESSED;
            break;
    }    
    
    /* Check if deep sleep mode can be entered */
    if(bStandbyAllowed)
    {
        EnterSleepMode();
    }    
    
    OS_StateManager_CurrentStateReached();

    return ucReturn;
}


//***************************************************************************
/*!
\author     KraemerE
\date       30.04.2021
\brief      Exit state of this state.
\return     u8
\param      eEventID - Event which shall be handled while in this state.
\param      uiParam1 - Event parameter from the received event
\param      ulParam2 - Second event parameter from the received event
******************************************************************************/
u8 State_Standby_Exit(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_PROCESSED;
    
    /* Make compiler happy */
    (void)eEventID;
    (void)uiParam1;
    (void)ulParam2;
    
    /* Enable measurement module */
    DR_Measure_Start();
        
    /* Leave standby state only when the slave was reseted */
    if(bSlaveReseted == false)
    {
        /* Set slave into reset mode */
        DR_Regulation_SetEspResetStatus(true);
        
        /* Check if timer is already running */
        if(OS_SW_Timer_GetTimerState(ucSW_Timer_EspResetTimeout) == eSwTimer_StatusSuspended)
        {
            /* Start reset timeout. After this timeout the standby state can be left */
            OS_SW_Timer_SetTimerState(ucSW_Timer_EspResetTimeout, eSwTimer_StatusRunning);
        }
    }
    else
    {    
        /* Delete software timer which are related to this state */   
        if(OS_SW_Timer_GetTimerState(ucSW_Timer_MsgRxTimeout) != eSwTimer_StatusInvalid)
            OS_SW_Timer_DeleteTimer(&ucSW_Timer_MsgRxTimeout);
            
        if(OS_SW_Timer_GetTimerState(ucSW_Timer_EspResetTimeout) != eSwTimer_StatusInvalid)
            OS_SW_Timer_DeleteTimer(&ucSW_Timer_EspResetTimeout);

        /* Switch state next state */
        OS_StateManager_CurrentStateReached();
    }
    return ucReturn;
}


