//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019

\file       MessageHandler.c
\brief      Handler for the serial communication messages

***********************************************************************************/
#include "OS_Messages.h"
#include "OS_Serial_UART.h"
#include "OS_ErrorDebouncer.h"
#include "OS_Communication.h"

#include "RequestResponseHandler.h"
#include "BaseTypes.h"
#include "MessageHandler.h"

#include "Aom.h"

//#include "Version\Version.h"
/****************************************** Defines ******************************************************/

/****************************************** Function prototypes ******************************************/
/****************************************** local functions *********************************************/
//********************************************************************************
/*!
\author     Kraemer E
\date       08.12.2019
\brief      Sends a settings done message to the slave.
\return     void 
***********************************************************************************/
static void SendUserSettingsDone(void)
{
    /* Start to send the packet */
    OS_Communication_SendResponseMessage(eMsgInitDone, NULL, 0, eCmdSet);
}

/****************************************** External visible functiones **********************************/
//********************************************************************************
/*!
\author     KraemerE    
\date       30.01.2019  

\fn         HandleMessage 

\brief      Message handler which handles each message it got.

\return     void 

\param      pMessage - pointer to message
\param      ucSize   - sizeof whole message

***********************************************************************************/
teMessageType ReqResMsg_Handler(tsMessageFrame* psMsgFrame)
{
    /* Get payload */    
    const teMessageId eMessageId = OS_Communication_GetObject(psMsgFrame);   
    teMessageType eResponse = eTypeAck;
    
    /* Check if standby is active */
    //if(Aom_IsStandbyActive())
    //{
        /* Message should only occur when when standby is left. Therfore request exit standby */
    //    OS_EVT_PostEvent(eEvtStandby, eEvtParam_ExitStandby, 0);
    //}

    /* Switch to message ID */
    switch(eMessageId)
    {   
        case eMsgWakeUp:
        {
            /* Stop standby timeout when its counting */
            OS_EVT_PostEvent(eEvtStandby, eEvtParam_ExitStandby, 0);                
            break;
        }
        
        case eMsgSleep:
        {
            //Check if clients are connected
            //TODO: cant get currently the amount of clients
            //      allow always to get into sleep mode
            //Serial_Print("MsgSleep"); 
            OS_EVT_PostEvent(eEvtStandby, eEvtParam_EnterStandby, 0);    
            break;
        }

        case eMsgAutoInitHardware:
        {
            OS_EVT_PostEvent(eEvtAutoInitHardware, eEvtParam_AutoInitDone, 0);
            break;
        }

        case eMsgUserTimer:
        {
            //Check if any user timer has been set
            if(psMsgFrame->sPayload.ucPayloadLen > 0)
            {
                /* Cast payload first */
                tMsgUserTimer* psMsgUserTimer = (tMsgUserTimer*)psMsgFrame->sPayload.pucData;

                /* Set timer values into an array */
                u8 ucTimeArray[4];
                ucTimeArray[0] = psMsgUserTimer->ucStartHour;
                ucTimeArray[1] = psMsgUserTimer->ucStartMin;
                ucTimeArray[2] = psMsgUserTimer->ucStopHour;
                ucTimeArray[3] = psMsgUserTimer->ucStopMin;

                /* Copy the array into an u32 parameter */
                ulEventParam2 ulTimeParam2 = 0;
                memcpy(&ulTimeParam2, &ucTimeArray[0], sizeof(ucTimeArray));

                /* Post timer values for event handler */
                OS_EVT_PostEvent(eEvtUserTimerReceived, psMsgUserTimer->ucTimerIdx, ulTimeParam2);
            }

            Aom_SettingsMsgReceived(true, false, 0);
            break; 
        }

        case eMsgInitOutputStatus:
        {
            /* Cast payload first */
            tMsgInitOutputState* psMsgInitOutputState = (tMsgInitOutputState*)psMsgFrame->sPayload.pucData;

            for(int outputIdx = 0; outputIdx < DRIVE_OUTPUTS; outputIdx++)
            {
                Aom_SetSavedUserSettings(psMsgInitOutputState->asOutputs[outputIdx].ucOutputIndex,
                                            psMsgInitOutputState->asOutputs[outputIdx].ucBrightness,
                                            psMsgInitOutputState->asOutputs[outputIdx].ucLedStatus,
                                            psMsgInitOutputState->ucAutomaticModeActive,
                                            psMsgInitOutputState->ucNightModeOnOff,
                                            psMsgInitOutputState->ucMotionDetectionOnOff,
                                            psMsgInitOutputState->ucBurnTime);
                                            
                Aom_SettingsMsgReceived(false, true, psMsgInitOutputState->asOutputs[outputIdx].ucOutputIndex);
            }
            break;
        }

        case eMsgInitDone:
        {
            if(Aom_GetSettingsMsgReceived() == true)
            {
                Aom_SetSettingsInitDone();

                //Set info that user settings are done
                SendUserSettingsDone();
            }
            break;
        }

        case eMsgOutputState:
        {
            /* Cast payload */
            tMsgOutputState* psMsgOutputState = (tMsgOutputState*)psMsgFrame->sPayload.pucData;                        
            Aom_SetNewMeasureValues(psMsgOutputState->ucOutputIndex, psMsgOutputState->ulVoltage, psMsgOutputState->uiCurrent, &psMsgOutputState->siTemperature);
            break;
        }

        case eMsgUpdateOutputStatus:
        {
            /* Cast payload */
            tMsgUpdateOutputState* psUpdateOutput = (tMsgUpdateOutputState*)psMsgFrame->sPayload.pucData;
            Aom_UpdateOutputStatus(psUpdateOutput->ucBrightness,
                                    psUpdateOutput->ucOutputIndex,
                                    psUpdateOutput->ucLedStatus,
                                    psUpdateOutput->ucNightModeOnOff,
                                    psUpdateOutput->ucMotionDetectionOnOff,
                                    psUpdateOutput->slRemainingBurnTime);

            break;
        }

        case eMsgStillAlive:
        {
            /* Cast payload */
            tMsgStillAlive* psStillAlive = (tMsgStillAlive*)psMsgFrame->sPayload.pucData;
            if(psStillAlive->bRequest == 0xFF && psStillAlive->bResponse == 0x00)
            {
                MessageHandler_SendStillAliveMessage(false);
            }
            break;
        }

        default:
            eResponse = eTypeDenied;
            break;
    }
    return eResponse;
}

