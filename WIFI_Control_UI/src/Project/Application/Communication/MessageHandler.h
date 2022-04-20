//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019

\file       MessageHandler.h
\brief      Handler for the serial communication messages

***********************************************************************************/

#ifndef _MESSAGEHANDLER_H_
#define _MESSAGEHANDLER_H_


#ifdef __cplusplus
extern "C"
{
#endif    
  
#include "OS_Messages.h"

void MessageHandler_Init(void);
extern void MessageHandler_HandleMessage(void* pvMsg);
void MessageHandler_HandleSerialCommEvent(void);
void MessageHandler_SendRequestOutputStatus(u8 ucOutputIndex);
void MessageHandler_SendHeartBeatOutput(u8 ucOutputIndex);
void MessageHandler_SendFaultMessage(const u16 uiErrorCode);
bool MessageHandler_GetActorsConfigurationStatus(void);
void MessageHandler_SendSleepOrWakeUpMessage(bool bSleep);
void MessageHandler_SendManualInitValue(bool bMaxValue, bool bMinValue, u8 ucOutputIdx);
void MessageHandler_SendAutoInitValue(void);
void MessageHandler_SendManualInitDone(void);
void MessageHandler_SendSystemStarted(void);
void MessageHandler_SendNewUserTimerValues(u8 ucStartH, u8 ucStopH, u8 ucStartM, u8 ucStopM, u8 ucTimerIdx);
void MessageHandler_SendActualTime(void);
void MessageHandler_SendStillAliveMessage(bool bRequest);
void MessageHandler_SendNightModeStatus(void);
void MessageHandler_SendAutomaticModeStatus(void);
void MessageHandler_SendMotionDetectionModeStatus(void);


#ifdef __cplusplus
}
#endif    

#endif //_MESSAGEHANDLER_H_
