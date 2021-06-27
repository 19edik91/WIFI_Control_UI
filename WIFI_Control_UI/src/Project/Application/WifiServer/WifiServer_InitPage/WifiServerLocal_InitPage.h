//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/


#ifndef _WIFISERVERLOC_INITIALIZATION_PAGE_H_
#define _WIFISERVERLOC_INITIALIZATION_PAGE_H_

#include "BaseTypes.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "WebSocketsServer.h"


void WifiServerLocal_InitPage_SendWebsocketValue(void);
void WifiServerLocal_InitPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket);
tCStateDefinition* WifiServerLocal_InitPage_Linker(void);
void WifiServerLocal_InitPage_AutoInitDone(void);
#ifdef __cplusplus
extern "C"
{
#endif

/* C-Functions */


#ifdef __cplusplus
}
#endif


#endif // _WIFISERVERLOC_INITIALIZATION_PAGE_H_
