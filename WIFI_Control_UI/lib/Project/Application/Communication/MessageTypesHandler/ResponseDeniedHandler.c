//********************************************************************************
/*!
\author     Kraemer E
\date       30.01.2019

\file       MessageHandler.c
\brief      Handler for the serial communication messages

***********************************************************************************/
#include "BaseTypes.h"
#include "MessageHandler.h"
#include "OS_Messages.h"
#include "OS_Serial_UART.h"
#include "OS_ErrorDebouncer.h"
#include "OS_Communication.h"

//#include "Version\Version.h"
/****************************************** Defines ******************************************************/

/****************************************** Function prototypes ******************************************/
/****************************************** local functions *********************************************/

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
teMessageType ResDeniedMsg_Handler(tsMessageFrame* psMsgFrame)
{
    /* Get payload */    
    const teMessageId eMessageId = OS_Communication_GetObject(psMsgFrame);   
    teMessageType eResponse = eNoType;
    
    //For other handling
    switch(eMessageId)
    {
        
        default:
            break;
    }
    
    return eResponse;
}

