/**
* @file    ErrorHandler.c
*
* @brief   TODO<Brief description of contents of file ErrorHandler.c>
*
* @author  $Author: kraemere $
*
* @date    $Date: 2019-01-17 11:49:38 +0100 (Do, 17 Jan 2019) $
*
* @version $Rev: 913 $
*
*/

/********************************* includes **********************************/
#include "ErrorHandler.h"
#include "OS_EventManager.h"

/***************************** defines / macros ******************************/

/************************ local data type definitions ************************/
/************************* local function prototypes *************************/
/************************* local data (const and var) ************************/
/************************ export data (const and var) ************************/
/****************************** local functions ******************************/
/************************ externally visible functions ***********************/




//***************************************************************************
/*!
\author     KraemerE
\date       04.05.2021
\brief      Handles explicit user specific errors in dependency of the fault code
\return     bErrorHandled - True when handled specific fault. Default case
                            will use OS-default handling.
\param      eFaultCode - Faultcode name which shall be handled
\param      bSetError - True when the fault shall be handled and send. False
                        when the fault shall only be send.
******************************************************************************/
bool ErrorHandler_HandleActualError(u8 eFaultCode, bool bSetError)
{    
    bool bErrorHandled = true;

    if(bSetError)
    {        
        switch(eFaultCode)
        {           
            default:
            {
                /* When not handled in here, use OS-default handling */
                bErrorHandled = false;
            
                break;
            }
        }
    }
           
    return bErrorHandled;
}
