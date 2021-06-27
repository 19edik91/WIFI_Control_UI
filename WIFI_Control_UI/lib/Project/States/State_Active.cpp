


/********************************* includes **********************************/
#include "OS_State_Reset.h"
#include "OS_StateManager.h"
#include "OS_SoftwareTimer.h"
#include "OS_Serial_UART.h"
#include "OS_ErrorDebouncer.h"
#include "OS_RealTimeClock.h"

#include "MessageHandler.h"
#include "Aom.h"
#include "WiFiServerLocal.h"
#include "WiFiManager.h"

/***************************** defines / macros ******************************/
#define RESET_CTRL_TIMEOUT      3000   //3 sec for reset timeout

/************************ local data type definitions ************************/

/************************* local function prototypes *************************/

/************************* local data (const and var) ************************/
static u8 ucActiveOutputs = 0;
static bool bModulesInit = false;

static u8 ucSW_Timer_10ms = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_5001ms = INVALID_TIMER_INDEX;
static u8 ucSW_Timer_FlashWrite = INVALID_TIMER_INDEX;


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
    //TODO: Use AOM to save user settings
    //Aom_Flash_WriteUserSettingsInFlash();
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

    /********************** Use Wifi-Manager to change WIFI ***********************/
    WiFiManager WiFiManager;    //Create new object
    WiFiManager.autoConnect("ESP_G", "19Affe91");

    /********************** Initialize the modules ********************************/
    system_update_cpu_freq(SYS_CPU_160MHZ);    
    WifiServerLocal_Init();
         
    Aom_InitStructuresWithDefaultValues();

    /* Create necessary software timer */      
    OS_SW_Timer_CreateTimer(&ucSW_Timer_10ms, SW_TIMER_10MS, eSwTimer_CreatePeriodic);
    OS_SW_Timer_CreateTimer(&ucSW_Timer_5001ms, SW_TIMER_5001MS, eSwTimer_CreatePeriodic); 

    /* Create async timer */
    OS_SW_Timer_CreateAsyncTimer(&ucSW_Timer_FlashWrite, SAVE_IN_FLASH_TIMEOUT, eSwTimer_CreateSuspended, TimeoutFlashUserSettings);
    
    if(bModulesInit == false)
    {     
        MessageHandler_Init();                
        bModulesInit = true;
    }
    
    //Send message about started system
    MessageHandler_SendSystemStarted();

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
            /******* 10ms-Tick **********/
            if(ulParam2 == EVT_SW_TIMER_10MS)
            {
                WifiServerLocal_Handler();

                //Check for received messages
                OS_Serial_UART_PollRxData();

                /* Handle the received message */
                MessageHandler_HandleSerialCommEvent();  
            }
            
            /******* 1001ms-Tick **********/
            else if(ulParam2 == EVT_SW_TIMER_1001MS)
            {                        
                /* Update time client for WIFI time */
                WifiServerLocal_UpdateTimeClient();
                WifiServerLocal_UpdateWebSocketValues();

                /* Check if system is initialized with the
                *  settings from the master. Otherwise send
                *  the message to the master again.
                */
                if(Aom_SettingsInitDone() == false)
                {
                    //Send message about started system
                    //#warning test
                    //Aom_SettingsMsgReceived(true, true);
                    MessageHandler_SendSystemStarted();
                }
            }
    
            /******* 1001ms-Tick **********/
            else if(ulParam2 == EVT_SW_TIMER_5001MS)
            {
                if(Aom_SettingsInitDone() == true)
                {
                    //Send actual time message to the master
                    MessageHandler_SendActualTime();

                    /* A Heartbeat is necessary */
                    for(u8 ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
                    {
                        MessageHandler_SendHeartBeatOutput(ucOutputIdx);
                    }
                }
            }
            break;
        }//eEvtSoftwareTimer


        case eEvtNewRegulationValue:
        {
            /* Doesn't get into this event.. don't know why.. debugging is hard */
            #if 0
            /* Check if event is new regulation value received */
            if(sNewEvent.param1 == eEvtParam_NewRegulationValueReceived)
            {
                /* Send message with new values to system master */
                MessageHandler_SendOutputStatus();
            }
            #endif
            break;
        }
                        
        case eEvtSendError:
        {
            /* Send error message to the POD controller */ 
            MessageHandler_SendFaultMessage(uiParam1);
            break;
        }        
        
        case eEvtManualInitHardware:
        {
            if(uiParam1 == eEvtParam_ManualInitSetMinValue)
            {
                MessageHandler_SendManualInitValue(false, true, ulParam2);
            }
            else if(uiParam1 == eEvtParam_ManualInitSetMaxValue)
            {
                MessageHandler_SendManualInitValue(true, false, ulParam2);
            }
            else if(uiParam1 == eEvtParam_ManualInitDone)
            {
                MessageHandler_SendManualInitDone();
            }                
            break;
        }

        case eEvtAutoInitHardware:
        {
            if(uiParam1 == eEvtParam_AutoInitDone)
            {
                WifiServerLocal_AutoInitDone();
            }
            else if(uiParam1 == eEvtParam_AutoInitStart)
            {
                MessageHandler_SendAutoInitValue();
            }                
            break;
        }

        case eEvtUserTimerReceived:
        {
            /* Copy timer values from parameter into array */
            u8 ucTimeArray[4];
            memcpy(&ucTimeArray[0], &ulParam2 ,sizeof(ucTimeArray));

            /* 0: StartHour | 1: StartMin | 2: StopHour | 3: StopMin */
            WifiServerLocal_SetSavedUserTimerValues(ucTimeArray[0],ucTimeArray[2],ucTimeArray[1],ucTimeArray[3],uiParam1);
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
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_10ms) != eSwTimer_StatusInvalid)
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_10ms);
            
    if(OS_SW_Timer_GetTimerState(ucSW_Timer_5001ms) != eSwTimer_StatusInvalid)
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_5001ms);

    if(OS_SW_Timer_GetTimerState(ucSW_Timer_FlashWrite) != eSwTimer_StatusInvalid)
        OS_SW_Timer_DeleteTimer(&ucSW_Timer_FlashWrite);
            
    /* Switch state to root state */
    OS_StateManager_CurrentStateReached();

    return ucReturn;
}


