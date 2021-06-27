//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/


#ifndef _WIFISERVERLOC_H_
#define _WIFISERVERLOC_H_

#include "BaseTypes.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*pFunction)(void);
typedef void (*pFunctionWs)(u8 ucNum, WStype_t eWsType, u8* pucPayload, size_t length);

typedef struct _tCStateDefinition
{
    pFunction   pFctButtonHandler;     /**< Pointer to the Entry function. This function will be called if the state is entered */
    pFunction   pFctSliderHandler;         /**< Pointer to the Execution function. This function will be called for every message if the state is active or a child of this state is active. */
    pFunction   pFctPageHandler;      /**< Pointer to the second Exit function of the state. This function is called if the state machine changes to an other state. */
    pFunctionWs pFctWebSocketHandler; /**< Handler for web socket events */
    ESP8266WebServer* psWebServer;
    WebSocketsServer* psWebSocketServer;
}tCStateDefinition;

typedef enum
{
    ePage_Root,
    ePage_Automatic,
    ePage_Initialization,
    ePage_Chart
}tePageSelect;


void WifiServerLocal_Init(void);
void WifiServerLocal_Handler(void);
void WifiServerLocal_AutoInitDone(void);
void WifiServerLocal_NewMeasureValues(u16 uiVolt, u16 uiCurr, s16 siTemp);
void WifiServerLocal_UpdateTimeClient(void);
void WifiServerLocal_SetSavedUserTimerValues(u8 ucStartH, u8 ucStopH, u8 ucStartM, u8 ucStopM, u8 ucTimerIdx);
void WifiServerLocal_PageRequest(tePageSelect ePageSelect);
void WifiServerLocal_UpdateWebSocketValues(void);

#ifdef __cplusplus
}
#endif

#endif // _WIFISERVERLOC_H_
