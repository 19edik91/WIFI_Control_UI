//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019

\file       MessageHandler.h
\brief      Handler for the serial communication messages

***********************************************************************************/

#ifndef _REQUESTRESPONSEHANDLER_H_
#define _REQUESTRESPONSEHANDLER_H_

//#include <Project.h>

#ifdef __cplusplus
extern "C"
{
#endif    

#include "OS_Messages.h"
  
teMessageType ReqResMsg_Handler(tsMessageFrame* psMsgFrame);

#ifdef __cplusplus
}
#endif    

#endif //_REQUESTRESPONSEHANDLER_H_
