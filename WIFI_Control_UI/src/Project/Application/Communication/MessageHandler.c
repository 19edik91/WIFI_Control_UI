//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019

\file       MessageHandler.c
\brief      Handler for the serial communication messages

***********************************************************************************/
#include "BaseTypes.h"
#include "MessageHandler.h"
#include "OS_Messages.h"
#include "OS_Serial_UART.h"
#include "OS_ErrorDebouncer.h"
#include "OS_Communication.h"
#include "RequestResponseHandler.h"
#include "ResponseDeniedHandler.h"
#include "Aom.h"

//#include "Version\Version.h"
/****************************************** Defines ******************************************************/
#define INVALID_MESSAGES_MAX    10

/****************************************** Variables ****************************************************/
/****************************************** Function prototypes ******************************************/

/****************************************** local functions *********************************************/
//********************************************************************************
/*!
\author     KraemerE    
\date       30.01.2019  
\brief      Message handler which handles each message it got.
\return     void 
\param      pMessage - pointer to message
\param      ucSize   - sizeof whole message
***********************************************************************************/
void MessageHandler_HandleMessage(void* pvMsg)
{
    tsMessageFrame* psMsgFrame = (tsMessageFrame*)pvMsg;
    
    bool bValidMessage = true;    
    static u8 ucInvalidMessageCounter = 0;    

    /* Get payload */    
    const teMessageId eMessageId = OS_Communication_GetObject(psMsgFrame);
    const teMessageType eMsgType = OS_Communication_GetMessageType(psMsgFrame);
    
    teMessageType eResponse = eNoType;
    
    /* Check for valid message address */
    if(OS_Communication_ValidateMessageAddresses(psMsgFrame))
    {    
        /* Check if the message is a response or a request */
        switch(eMsgType)
        {           
            case eTypeAck:
            {
                //Clear message from buffer
                OS_Communication_ResponseReceived(psMsgFrame);
                break;
            }
            
            case eTypeDenied:
            {
                eResponse = ResDeniedMsg_Handler(psMsgFrame);
                break;
            }
            
            case eTypeResponse:
            case eTypeRequest:
            {
                eResponse = ReqResMsg_Handler(psMsgFrame);                
                break;
            }

            case eNoType:
            case eTypeUnknown:
            default:
                //invalid message
                break;
        }
    }
    else
    {   
        /* Invalid message received. Maybe UART TX & RX shorted */
        bValidMessage = false;
        
        /* Wrong address response with denied */
        eResponse = eTypeDenied;
    }
       
    /* Send response message */
    if(eResponse != eNoType)
    {
        OS_Communication_SendAcknowledge(eMessageId, psMsgFrame->sPayload.ucQueryID, eResponse);
    }

    /* Check for invalid messages */
    if(bValidMessage)
    {
        ucInvalidMessageCounter = 0;
    }
    else
    {
        if(++ucInvalidMessageCounter > INVALID_MESSAGES_MAX)
        {
            ucInvalidMessageCounter = INVALID_MESSAGES_MAX;
            OS_ErrorDebouncer_PutErrorInQueue(eCommunicationFault);
        }
    }     
    return;
}


/****************************************** External visible functiones **********************************/

//********************************************************************************
/*!
\author     KraemerE    
\date       30.01.2019  
\brief      Creates a message with the fault code and sends it further
\return     none 
\param      uiErrorCode - The error code which should be send
***********************************************************************************/
void MessageHandler_SendFaultMessage(const u16 uiErrorCode)
{
    /* Create structure */
    tMsgFaultMessage sMsgFault;

    /* Clear the structures */
    memset(&sMsgFault, 0, sizeof(sMsgFault));

    /* Fill them */
    sMsgFault.uiErrorCode = uiErrorCode;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgErrorCode, &sMsgFault, sizeof(tMsgFaultMessage), eCmdSet);
}


//********************************************************************************
/*!
\author     KraemerE    
\date       18.03.2019  
\brief      Creates a message with the output status and sends it to the master.
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendRequestOutputStatus(u8 ucOutputIndex)
{
    /* Send output state only when the settings have been received. */
    if(Aom_SettingsInitDone())
    {  
        /* Create structure */
        tMsgRequestOutputState sMsgOutput;

        /* Clear the structures */
        memset(&sMsgOutput, 0, sizeof(sMsgOutput));

        /* Get output status values */
        const tRegulationValues* psRegValues = Aom_GetCustomValue();

        /* Put the values into the message */
        sMsgOutput.ucBrightness = psRegValues->sLedValue[ucOutputIndex].ucPercentValue;
        sMsgOutput.ucLedStatus = psRegValues->sLedValue[ucOutputIndex].bStatus;
        sMsgOutput.ucInitMenuActive = !psRegValues->bMainPageActive;
        sMsgOutput.ucOutputIndex = ucOutputIndex;

        /* Start to send the packet */
        OS_Communication_SendRequestMessage(eMsgRequestOutputStatus, &sMsgOutput, sizeof(tMsgRequestOutputState), eCmdSet);
    }
}

//********************************************************************************
/*!
\author     KraemerE    
\date       18.03.2019  
\brief      Creates a message with the output status and sends it to the master.
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendHeartBeatOutput(u8 ucOutputIndex)
{
    /* Send output state only when the settings have been received. */
    if(Aom_SettingsInitDone())
    {  
        /* Create structure */
        tMsgHeartBeatOutput sMsgHeartBeatOutput;

        /* Clear the structures */
        memset(&sMsgHeartBeatOutput, 0, sizeof(tMsgHeartBeatOutput));

        /* Get output status values */
        const tRegulationValues* psRegValues = Aom_GetCustomValue();

        /* Put the values into the message */
        sMsgHeartBeatOutput.ucBrightness = psRegValues->sLedValue[ucOutputIndex].ucPercentValue;
        sMsgHeartBeatOutput.ucLedStatus = psRegValues->sLedValue[ucOutputIndex].bStatus;
        sMsgHeartBeatOutput.ucOutputIndex = ucOutputIndex;

        /* Start to send the packet */
        OS_Communication_SendRequestMessage(eMsgHeartBeatOutput, &sMsgHeartBeatOutput, sizeof(tMsgHeartBeatOutput), eNoCmd);
    }
}


//********************************************************************************
/*!
\author     KraemerE    
\date       22.05.2019  
\brief      Creates a message which requests the sleep or wake up mode
\return     none 
\param      bSleep - True to send a sleep request otherwise sends a wake up request
***********************************************************************************/
void MessageHandler_SendSleepOrWakeUpMessage(bool bSleep)
{
    /* Start to send the packet */
    teMessageId eMsgID = bSleep ? eMsgSleep : eMsgWakeUp;
    OS_Communication_SendRequestMessage(eMsgID, NULL, 0, eCmdSet);
}


//********************************************************************************
/*!
\author     KraemerE    
\date       03.11.2019  
\brief      Creates a message where the master shall save either the min values
            or the max values of the current brightness step
\return     none 
\param      bMaxValue - True to save the current values as the max values
\param      bMinValue - True to save the current values as the min values
***********************************************************************************/
void MessageHandler_SendManualInitValue(bool bMaxValue, bool bMinValue, u8 ucOutputIdx)
{
    /* Create structure */
    tMsgManualInit sMsgManualInit;
    
    /* Clear the structures */
    memset(&sMsgManualInit, 0, sizeof(sMsgManualInit));

    sMsgManualInit.ucSetMinValue = bMinValue;
    sMsgManualInit.ucSetMaxValue = bMaxValue;
    sMsgManualInit.ucOutputIndex = ucOutputIdx;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgManualInitHardware, &sMsgManualInit, sizeof(tMsgManualInit), eCmdSet);
}

//********************************************************************************
/*!
\author     KraemerE    
\date       03.11.2019  
\brief      Creates a message where the master shall initialize 
            the values automatically
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendAutoInitValue(void)
{
    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgAutoInitHardware, NULL, 0, eCmdSet);
}

//********************************************************************************
/*!
\author     KraemerE    
\date       10.11.2019  
\brief      Creates a message where the master gets a notification about the 
            finished initialization status.
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendManualInitDone(void)
{
    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgManualInitHwDone, NULL, 0, eCmdSet);
}

//********************************************************************************
/*!
\author     KraemerE    
\date       08.12.2019  
\brief      Creates a message where the master gets a notification about the 
            finished system start status.
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendSystemStarted(void)
{
    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgSystemStarted, NULL, 0, eCmdSet);
}

//********************************************************************************
/*!
\author     KraemerE    
\date       08.12.2019  
\brief      Creates a message with the new automatic mode timer settings
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendNewUserTimerValues(u8 ucStartH, u8 ucStopH, u8 ucStartM, u8 ucStopM, u8 ucTimerIdx)
{
    /* Create structure */
    tMsgUserTimer sMsgUserTimer;
    
    /* Clear the structures */
    memset(&sMsgUserTimer, 0, sizeof(sMsgUserTimer));

    /* Fill them */
    sMsgUserTimer.ucStartHour = ucStartH; 
    sMsgUserTimer.ucStopHour = ucStopH;
    sMsgUserTimer.ucStartMin = ucStartM;
    sMsgUserTimer.ucStopMin = ucStopM;
    sMsgUserTimer.ucTimerIdx = ucTimerIdx;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgUserTimer, &sMsgUserTimer, sizeof(tMsgUserTimer), eCmdSet);
}

//********************************************************************************
/*!
\author     KraemerE    
\date       08.12.2019  
\fn         MessageHandler_SendNewUserTimerValues 
\brief      Creates a message with the new automatic mode timer settings
\return     none 
\param      none
***********************************************************************************/
void MessageHandler_SendActualTime(void)
{
    /* Create structure */
    tMsgCurrentTime sMsgTime;
    
    /* Clear the structures */
    memset(&sMsgTime, 0, sizeof(sMsgTime));

    /* Correct message fill because of packed structure (pointer needs correct alignment ) */
    u8 ucHour = 0xFF;
    u8 ucMinutes = 0xFF;
    u32 ulTicks = 0;
    Aom_GetActualTime(&ucHour, &ucMinutes, &ulTicks);

    /* Copy data */
    sMsgTime.ucHour = ucHour;
    sMsgTime.ucMinutes = ucMinutes;
    sMsgTime.ulTicks = ulTicks;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgCurrentTime, &sMsgTime, sizeof(tMsgCurrentTime), eCmdSet);
}

//********************************************************************************
/*!
\author     KraemereE   
\date       26.10.2020 
\fn         SendStillAliveMessage()
\brief      Sends a message to the slave to check if its still alive
\param      bRequest - true when a request otherwise send a response
\return     none 
***********************************************************************************/
void MessageHandler_SendStillAliveMessage(bool bRequest)
{    
    /* Create structure */
    tMsgStillAlive sMsgStillAlive;
        
    /* Clear the structures */
    memset(&sMsgStillAlive, 0, sizeof(sMsgStillAlive));  
    
    /* Fill them */
    sMsgStillAlive.bRequest  = (bRequest == true) ? 0xFF : 0;
    sMsgStillAlive.bResponse = (bRequest == false) ? 0xFF : 0;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgStillAlive, &sMsgStillAlive, sizeof(tMsgStillAlive), eCmdSet);
}

//********************************************************************************
/*!
\author     Kraemer E
\date       05.05.2021
\fn         MessageHandler_Init
\brief      Links the message handler function with the OS-Communication module
\return     void 
***********************************************************************************/
void MessageHandler_Init(void)
{    
    OS_Communication_Init(MessageHandler_HandleMessage);
}


//********************************************************************************
/*!
\author     Kraemer E
\date       12.07.2021
\brief      Just a wrapper function
\return     void 
***********************************************************************************/
void MessageHandler_HandleSerialCommEvent(void)
{
    OS_Communication_HandleSerialCommEvent();
}


//********************************************************************************
/*!
\author     KraemereE   
\date       20.04.2022 
\brief      Sends a message with the change of the night mode status
\param      none
\return     none 
***********************************************************************************/
void MessageHandler_SendNightModeStatus(void)
{    
    /* Create structure */
    tsMsgEnableNightMode sMsgEnableNightMode;
        
    /* Clear the structures */
    memset(&sMsgEnableNightMode, 0, sizeof(tsMsgEnableNightMode));  
    
    /* Get output status values */
    const tRegulationValues* psRegValues = Aom_GetCustomValue();

    /* Fill them */
    sMsgEnableNightMode.ucNightModeStatus = psRegValues->bAutomaticModeStatus;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgEnableNightMode, &sMsgEnableNightMode, sizeof(tsMsgEnableNightMode), eCmdSet);
}

//********************************************************************************
/*!
\author     KraemereE   
\date       20.04.2022 
\brief      Sends a message with the change of the automatic mode status
\param      none
\return     none 
***********************************************************************************/
void MessageHandler_SendAutomaticModeStatus(void)
{    
    /* Create structure */
    tsMsgEnableAutomaticMode sMsgEnableAutomaticMode;
        
    /* Clear the structures */
    memset(&sMsgEnableAutomaticMode, 0, sizeof(tsMsgEnableAutomaticMode));  
    
    /* Get output status values */
    const tRegulationValues* psRegValues = Aom_GetCustomValue();

    /* Fill them */
    sMsgEnableAutomaticMode.ucAutomaticModeStatus = psRegValues->bAutomaticModeStatus;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgEnableAutomaticMode, &sMsgEnableAutomaticMode, sizeof(tsMsgEnableAutomaticMode), eCmdSet);
}

//********************************************************************************
/*!
\author     KraemereE   
\date       20.04.2022 
\brief      Sends a message with the change of the motion detection mode status
\param      none
\return     none 
***********************************************************************************/
void MessageHandler_SendMotionDetectionModeStatus(void)
{    
    /* Create structure */
    tsMsgEnableMotionDetectStatus sMsgMotionDetect;
        
    /* Clear the structures */
    memset(&sMsgMotionDetect, 0, sizeof(tsMsgEnableMotionDetectStatus));  
    
    /* Get output status values */
    const tRegulationValues* psRegValues = Aom_GetCustomValue();

    /* Fill them */
    sMsgMotionDetect.ucMotionDetectStatus = psRegValues->bMotionDetectionOnOff;
    sMsgMotionDetect.ucBurnTime = psRegValues->ucBurnTime;

    /* Start to send the packet */
    OS_Communication_SendRequestMessage(eMsgEnableMotionDetect, &sMsgMotionDetect, sizeof(tsMsgEnableMotionDetectStatus), eCmdSet);
}