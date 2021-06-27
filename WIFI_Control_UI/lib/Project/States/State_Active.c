


/********************************* includes **********************************/
#include "OS_State_Reset.h"
#include "OS_StateManager.h"
#include "OS_SoftwareTimer.h"
#include "OS_Serial_UART.h"
#include "OS_ErrorDebouncer.h"
#include "OS_RealTimeClock.h"




#include "MessageHandler.h"

#include "Aom_System.h"
#include "Aom_Regulation.h"
#include "Aom_Measure.h"
#include "Aom_Time.h"
#include "Aom_Flash.h"

#include "AutomaticMode.h"


/***************************** defines / macros ******************************/
#define RESET_CTRL_TIMEOUT      3000   //3 sec for reset timeout

/************************ local data type definitions ************************/

/************************* local function prototypes *************************/

/************************* local data (const and var) ************************/
static u8 ucActiveOutputs = 0;
static bool bModulesInit = false;

static u8 ucSW_Timer_2ms = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_10ms = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_FlashWrite = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_EnterStandby = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_EspReset = INVALID_TIMER_INDEX;

/************************ export data (const and var) ************************/


/****************************** local functions ******************************/
//***************************************************************************
/*!
\author     KraemerE
\date       04.05.2021
\brief      Timeout callback for async timer. Saves the current user settings
            into the flash.
\return     none
\param      none
******************************************************************************/
void TimeoutFlashUserSettings(void)
{
    Aom_Flash_WriteUserSettingsInFlash();
}


//***************************************************************************
/*!
\author     KraemerE
\date       04.05.2021
\brief      Requests a state change to the standby state.
\return     none
\param      none
******************************************************************************/
void RequestStandbyState(void)
{
    OS_EVT_PostEvent(eEvtState_Request, eSM_State_Standby, 0);
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       26.10.2020
\brief      Resets slave after a communication fault.
\param      bReset - true for activating reset otherwise false
\return     none
***********************************************************************************/
static void ResetSlaveByTimeout(bool bReset)
{
    if(bReset == true)
    {
        /* Check if timer is already running */
        if(OS_SW_Timer_GetTimerState(ucSW_Timer_EspReset) == eSwTimer_StatusSuspended)
        {
            //Start timeout
            OS_SW_Timer_SetTimerState(ucSW_Timer_EspReset, eSwTimer_StatusRunning);
        }
    }
    
    DR_Regulation_SetEspResetStatus(bReset);
    MessageHandler_ClearAllTimeouts();
    Aom_System_SetSlaveResetState(bReset);
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       06.05.2021
\brief      Timeout callback function which enables the slave again.
\param      none
\return     none
***********************************************************************************/
static void EnableSlaveTimeout(void)
{
    ResetSlaveByTimeout(false);
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
u8 State_Active_Entry(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
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

    /* Create necessary software timer */      
    OS_SW_Timer_CreateTimer(&ucSW_Timer_2ms, SW_TIMER_2MS, eSwTimer_CreatePeriodic);
    OS_SW_Timer_CreateTimer(&ucSW_Timer_10ms, SW_TIMER_10MS, eSwTimer_CreatePeriodic);
    
    /* Create async timer */
    OS_SW_Timer_CreateAsyncTimer(&ucSW_Timer_EnterStandby, TIMEOUT_ENTER_STANDBY, eSwTimer_CreateSuspended, RequestStandbyState);
    OS_SW_Timer_CreateAsyncTimer(&ucSW_Timer_FlashWrite, SAVE_IN_FLASH_TIMEOUT, eSwTimer_CreateSuspended, TimeoutFlashUserSettings);
    OS_SW_Timer_CreateAsyncTimer(&ucSW_Timer_EspReset, RESET_CTRL_TIMEOUT, eSwTimer_CreateSuspended, EnableSlaveTimeout);
    
    if(bModulesInit == false)
    {
        if(Aom_Measure_SystemVoltageCalculated() == false)
        {
            Aom_Measure_CalculateSystemVoltage();
        }
        
        MessageHandler_Init();
        DR_Measure_Init();
        DR_Regulation_Init();
                
        bModulesInit = true;
    }
    
    /* Switch on system */    
    //const tRegulationValues* psRegVal = Aom_Regulation_GetRegulationValuesPointer();
    //u8 ucOutputIdx;
    //for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    //{              
    //    OS_EVT_PostEvent(eEvtNewRegulationValue, eEvtParam_RegulationStart, ucOutputIdx);
    //}
    
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
u8 State_Active_Root(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_PROCESSED;
    
    switch(eEventID)
    {
        case eEvtSoftwareTimer:
        {
            /******* 2ms-Tick **********/
            if(ulParam2 == EVT_SW_TIMER_2MS)
            {
                DR_Measure_Tick();
                ucActiveOutputs = DR_Regulation_Handler();
            }
            
            /******* 10ms-Tick **********/
            else if(ulParam2 == EVT_SW_TIMER_10MS)
            {
                /* Get standby-timer state */
                teSW_TimerStatus eTimerState = OS_SW_Timer_GetTimerState(ucSW_Timer_EnterStandby);
                
                /* Start the timeout for the standby when all regulation states are off */
                if(ucActiveOutputs == 0)
                {
                    if(eTimerState == eSwTimer_StatusSuspended && Aom_System_StandbyAllowed())
                    {
                        /* Start the timeout for the standby timeout */
                        OS_SW_Timer_SetTimerState(ucSW_Timer_EnterStandby, eSwTimer_StatusRunning);
                    }
                }
                else if(eTimerState == eSwTimer_StatusRunning)
                {
                    /* Stop standby timeout when its counting */
                    OS_SW_Timer_SetTimerState(ucSW_Timer_EnterStandby, eSwTimer_StatusSuspended);
                }
            }
            
            /******* 51ms-Tick **********/
            else if(ulParam2 == EVT_SW_TIMER_51MS)
            {
                /* Check for over-current faults */
                DR_ErrorDetection_CheckCurrentValue();
                
                /* Check for over-temperature faults */
                DR_ErrorDetection_CheckAmbientTemperature();
                
                /* Handle message in the retry buffer */
                MessageHandler_Tick(SW_TIMER_51MS);
            }
            
            /******* 251ms-Tick **********/
            else if(ulParam2 == EVT_SW_TIMER_251MS)
            {
                DR_Regulation_CheckSensorForMotion();
                AutomaticMode_Handler();
            }
            
            /******* 1001ms-Tick **********/
            else if(ulParam2 == EVT_SW_TIMER_1001MS)
            {                        
                AutomaticMode_Tick(SW_TIMER_1001MS);
                                
                /* Toggle LED to show a living CPU */
                DR_Regulation_ToggleHeartBeatLED();
                
                /* Toggle error LED when an error is in timeout */
                DR_Regulation_ToggleErrorLED();

                /* Calculate voltage, current and temperature and send them afterwards to the slave */
                Aom_Measure_SetMeasuredValues(true, true, true);
                
                /* Check first if the slave is active before sending a request */
                if(Aom_System_GetSystemStarted())
                {
                    MessageHandler_SendOutputState();
                }
            }            
            break;
        }//eEvtSoftwareTimer
        
        case eEvtNewRegulationValue:
        {
            /* Start the timeout for saving the requested values into flash */
            if(uiParam1 == eEvtParam_RegulationValueStartTimer)
            {
                /* Restart the flash timeout */
                OS_SW_Timer_SetTimerState(ucSW_Timer_FlashWrite, eSwTimer_StatusRunning);
            }
            else if(uiParam1 == eEvtParam_RegulationStart)
            {
                DR_Regulation_ChangeState(eStateActiveR, (u8)ulParam2);
            }
            else if(uiParam1 == eEvtParam_RegulationStop)
            {
                DR_Regulation_ChangeState(eStateOff, (u8)ulParam2);
            }
            break;
        }
        
        case eEvtTimeReceived:
        {
            /* Get pointer to the regulation structure (read-only) */
            const tsCurrentTime* psTime = Aom_Time_GetCurrentTime();
            
            /* Update RTC time with the received epoch time from NTP-Client */                        
            if(uiParam1 == eEvtParam_TimeFromNtpClient)
            {
                OS_RealTimeClock_SetTime(psTime->ulTicks);
            }
            
            /* Check for handling in automatic mode */
            AutomaticMode_TimeUpdated();
            break;
        }
                
        case eEvtSendError:
        {
            /* Send error message to the POD controller */ 
            MessageHandler_SendFaultMessage(uiParam1);
            break;
        }        
        
        case eEvtCommTimeout:
        {
            /* Check if reset timer is suspended and a communication timout is pending */
            if(OS_SW_Timer_GetTimerState(ucSW_Timer_EspReset) == eSwTimer_StatusSuspended
                && MessageHandler_GetCommunicationTimeoutStatus() == true)
            {
                /* When the reset pin is in high state, put it to low and reset the slave */
                //bool bEspResetStatus = DR_Regulation_GetEspResetStatus();
                ResetSlaveByTimeout(true);
                
                Aom_System_SetSystemStarted(false);
            }
            break;
        }
        
        
        case eEvtAutomaticMode_ResetBurningTimeout:
        {
            AutomaticMode_ResetBurningTimeout();
            break;
        }
        
        default:
            ucReturn = EVT_NOT_PROCESSED;
            break;
    }
    
    
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
u8 State_Active_Exit(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_NOT_PROCESSED;
    
    /* Make compiler happy */
    (void)eEventID;
    (void)uiParam1;
    (void)ulParam2;
    
    /* Delete software timer which are related to this state */ 
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_2ms) != eSwTimer_StatusInvalid)
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_2ms);
        
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_10ms) != eSwTimer_StatusInvalid)
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_10ms);
        
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_FlashWrite) != eSwTimer_StatusInvalid)
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_FlashWrite);
        
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_EnterStandby) != eSwTimer_StatusInvalid)    
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_EnterStandby);
        
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_EspReset) != eSwTimer_StatusInvalid)        
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_EspReset);
    
    /* Switch state to root state */
    OS_StateManager_CurrentStateReached();

    return ucReturn;
}


