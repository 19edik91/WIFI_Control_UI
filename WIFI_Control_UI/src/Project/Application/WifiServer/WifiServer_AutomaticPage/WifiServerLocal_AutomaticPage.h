//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/


#ifndef _WIFISERVERLOC_AUTOMATICPAGE_H_
#define _WIFISERVERLOC_AUTOMATICPAGE_H_

#include "BaseTypes.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "WebSocketsServer.h"

void WifiServerLocal_AutoPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket);
tCStateDefinition* WifiServerLocal_AutoPage_Linker(void);

#ifdef __cplusplus
extern "C"
{
#endif

/* C-Functions */

#ifdef __cplusplus
}
#endif

#endif // _WIFISERVERLOC_AUTOMATICPAGE_H_
