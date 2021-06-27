//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/


#ifndef _WIFISERVERLOC_NTP_H_
#define _WIFISERVERLOC_NTP_H_

#include "BaseTypes.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>

void WifiServerLocal_NTP_Init(void);
void WifiServerLocal_NTP_UpdateTimeClient(void);

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

#endif // _WIFISERVERLOC_NTP_H_
