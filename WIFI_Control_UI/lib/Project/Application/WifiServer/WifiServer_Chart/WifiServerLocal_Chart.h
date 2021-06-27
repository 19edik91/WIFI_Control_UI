//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/


#ifndef _WIFISERVERLOC_CHARTPAGE_H_
#define _WIFISERVERLOC_CHARTPAGE_H_

#ifdef USE_CHARTS

#include "BaseTypes.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "WebSocketsServer.h"

void WifiServerLocal_ChartPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket);
tCStateDefinition* WifiServerLocal_ChartPage_Linker(void);
void WifiServerLocal_ChartPage_SendWebsocketValue(void);

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif
#endif //USE_CHARTS
#endif // _WIFISERVERLOC_CHARTPAGE_H_
