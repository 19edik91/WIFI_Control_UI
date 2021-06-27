//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019

\file       MessageHandler.h
\brief      Handler for the serial communication messages

***********************************************************************************/

#ifndef _RESPONSEDENIEDHANDLER_H_
#define _RESPONSEDENIEDHANDLER_H_

#ifdef __cplusplus
extern "C"
{
#endif    
  
#include "OS_Messages.h"
teMessageType ResDeniedMsg_Handler(tsMessageFrame* psMsgFrame);

#ifdef __cplusplus
}
#endif    

#endif //_RESPONSEDENIEDHANDLER_H_
