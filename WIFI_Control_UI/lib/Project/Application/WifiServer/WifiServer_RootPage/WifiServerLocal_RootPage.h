//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/


#ifndef _WIFISERVERLOC_ROOTPAGE_H_
#define _WIFISERVERLOC_ROOTPAGE_H_

#include "BaseTypes.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "WebSocketsServer.h"

void WifiServerLocal_RootPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket);
tCStateDefinition* WifiServerLocal_RootPage_Linker(void);
void WifiServerLocal_RootPage_SendWebsocketValue(void);
void WifiServerLocal_RootPage_SendCurrentClock(void);

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif // _WIFISERVERLOC_ROOTPAGE_H_
