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
        OS_Communication_SendResponseMessage(eMessageId, eResponse);
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
    tsMessageFrame sMsgFrame;  
    tMsgFaultMessage sMsgFault;

    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));
    memset(&sMsgFault, 0, sizeof(sMsgFault));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgErrorCode;
    sMsgFault.uiErrorCode = uiErrorCode;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    //    sMsgVersion.uiCrc = (u16)SelfTest_FlashCRCRead(ST_FLASH_SEGIDX_S1);
    memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgFault, sizeof(sMsgFault));

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
        tsMessageFrame sMsgFrame;  
        tMsgRequestOutputState sMsgOutput;

        /* Clear the structures */
        memset(&sMsgFrame, 0, sizeof(sMsgFrame));
        memset(&sMsgOutput, 0, sizeof(sMsgOutput));

        /* Fill them */
        sMsgFrame.sPayload.ucCommand = eCmdSet;
        sMsgFrame.sPayload.ucMsgId = eMsgRequestOutputStatus;
        sMsgFrame.sHeader.ucMsgType = eTypeRequest;


        /* Get output status values */
        tRegulationValues* psRegValues = Aom_GetCustomValue();

        /* Put the values into the message */
        sMsgOutput.b7Brightness = psRegValues->sLedValue[ucOutputIndex].ucPercentValue;
        sMsgOutput.bLedStatus = psRegValues->sLedValue[ucOutputIndex].bStatus;
        sMsgOutput.bInitMenuActiveInv = !psRegValues->bMainPageActive;
        sMsgOutput.bInitMenuActive = psRegValues->bMainPageActive;
        sMsgOutput.bAutomaticModeActive = psRegValues->bAutomaticModeStatus;
        sMsgOutput.bMotionDetectionOnOff = psRegValues->bMotionDetectionOnOff;
        sMsgOutput.bNightModeOnOff = psRegValues->bNightModeOnOff;
        sMsgOutput.ucBurnTime = psRegValues->ucBurnTime;
        sMsgOutput.b3OutputIndex = ucOutputIndex;

        /* Save the message in frame */
        memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgOutput, sizeof(sMsgOutput));

        /* Fill header and checksum */
        HF_CreateMessageFrame(&sMsgFrame);

        /* Start to send the packet */
        HF_SendMessage(&sMsgFrame);
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
        tsMessageFrame sMsgFrame;  
        tMsgHeartBeatOutput sMsgHeartBeatOutput;

        /* Clear the structures */
        memset(&sMsgFrame, 0, sizeof(tsMessageFrame));
        memset(&sMsgHeartBeatOutput, 0, sizeof(tMsgHeartBeatOutput));

        /* Fill them */
        sMsgFrame.sPayload.ucCommand = eCmdSet;
        sMsgFrame.sPayload.ucMsgId = eMsgHeartBeatOutput;
        sMsgFrame.sHeader.ucMsgType = eTypeRequest;

        /* Get output status values */
        const tRegulationValues* psRegValues = Aom_GetCustomValue();

        /* Put the values into the message */
        sMsgHeartBeatOutput.ucBrightness = psRegValues->sLedValue[ucOutputIndex].ucPercentValue;
        sMsgHeartBeatOutput.ucLedStatus = psRegValues->sLedValue[ucOutputIndex].bStatus;
        sMsgHeartBeatOutput.ucOutputIndex = ucOutputIndex;

        /* Save the message in frame */
        memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgHeartBeatOutput, sizeof(tMsgHeartBeatOutput));

        /* Fill header and checksum */
        HF_CreateMessageFrame(&sMsgFrame);

        /* Start to send the packet */
        HF_SendMessage(&sMsgFrame);
    }
}

//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019
\brief      Is called whenever an message was received. Used by the Event-Handler
\return     void 
***********************************************************************************/
void MessageHandler_HandleSerialCommEvent(void)
{
    //Buffer is bigger than message frame because a fault could lead to a
    //bigger message size and with this also to an error.
//    u8  ucRxBuffer[sizeof(tsMessageFrame)+sizeof(u32)];
    u8 ucRxBuffer[50];
    u8  ucRxCount = sizeof(ucRxBuffer);
    u32 ulCalcCrc32 = INITIAL_CRC_VALUE;    
    
    /* Clear buffer first */
    memset(&ucRxBuffer[0], 0, ucRxCount);
    
    /* Get message from the buffer */
    if(Serial_GetPacket(&ucRxBuffer[0], &ucRxCount))
    {
        /* Get the whole message first and cast it into the message frame */
        tsMessageFrame* psMsgFrame = (tsMessageFrame*)ucRxBuffer;
                
        /* Calculate the CRC for this message */
        ulCalcCrc32 = HF_CreateMessageCrc(&ucRxBuffer[0], HF_GetMessageSizeWithoutCrc(psMsgFrame));
        
        /* Check for correct CRC */
        if(psMsgFrame->ulCrc32 == ulCalcCrc32)
        {        
            /* Handle message */
            HandleMessage(psMsgFrame);
        }
        else
        {           
            /* Put into error queue */
            ErrorDebouncer_PutErrorInQueue(eMessageCrcFault);
        }
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
    /* Create structure */
    tsMessageFrame sMsgFrame;  

    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = bSleep ? eMsgSleep : eMsgWakeUp;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    tsMessageFrame sMsgFrame;  
    tMsgManualInit sMsgManualInit;
    
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));
    memset(&sMsgManualInit, 0, sizeof(sMsgManualInit));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgManualInitHardware;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    sMsgManualInit.bSetMinValue = bMinValue;
    sMsgManualInit.bSetMaxValue = bMaxValue;
    sMsgManualInit.ucOutputIndex = ucOutputIdx;

    /* Save the message in frame */
    memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgManualInit, sizeof(sMsgManualInit));

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    /* Create structure */
    tsMessageFrame sMsgFrame;  
    
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgAutoInitHardware;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    /* Create structure */
    tsMessageFrame sMsgFrame;  
    
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgManualInitHwDone;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    /* Create structure */
    tsMessageFrame sMsgFrame;  
    
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgSystemStarted;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    tsMessageFrame sMsgFrame;  
    tMsgUserTimer sMsgUserTimer;
    
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));
    memset(&sMsgUserTimer, 0, sizeof(sMsgUserTimer));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgUserTimer;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    sMsgUserTimer.ucStartHour = ucStartH; 
    sMsgUserTimer.ucStopHour = ucStopH;
    sMsgUserTimer.ucStartMin = ucStartM;
    sMsgUserTimer.ucStopMin = ucStopM;
    sMsgUserTimer.b7TimerIdx = ucTimerIdx;

    /* Save the message in frame */
    memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgUserTimer, sizeof(sMsgUserTimer));

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    tsMessageFrame sMsgFrame;  
    tMsgCurrentTime sMsgTime;
    
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));
    memset(&sMsgTime, 0, sizeof(sMsgTime));

    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgCurrentTime;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    /* Correct message fill because of packed structure (pointer needs correct alignment ) */
    u8 ucHour = 0xFF;
    u8 ucMinutes = 0xFF;
    u32 ulTicks = 0;
    Aom_GetActualTime(&ucHour, &ucMinutes, &ulTicks);

    /* Copy data */
    sMsgTime.ucHour = ucHour;
    sMsgTime.ucMinutes = ucMinutes;
    sMsgTime.ulTicks = ulTicks;

    /* Save the message in frame */
    memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgTime, sizeof(tMsgCurrentTime));

    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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
    tsMessageFrame sMsgFrame;  
    tMsgStillAlive sMsgStillAlive;
        
    /* Clear the structures */
    memset(&sMsgFrame, 0, sizeof(sMsgFrame));
    memset(&sMsgStillAlive, 0, sizeof(sMsgStillAlive));  
    
    /* Fill them */
    sMsgFrame.sPayload.ucCommand = eCmdSet;
    sMsgFrame.sPayload.ucMsgId = eMsgStillAlive;
    sMsgStillAlive.bRequest  = (bRequest == true) ? 0xFF : 0;
    sMsgStillAlive.bResponse = (bRequest == false) ? 0xFF : 0;
    sMsgFrame.sHeader.ucMsgType = eTypeRequest;

    /* Fill frame payload */
    memcpy(&sMsgFrame.sPayload.ucData[0], &sMsgStillAlive, sizeof(tMsgStillAlive));
    
    /* Fill header and checksum */
    HF_CreateMessageFrame(&sMsgFrame);

    /* Start to send the packet */
    HF_SendMessage(&sMsgFrame);
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