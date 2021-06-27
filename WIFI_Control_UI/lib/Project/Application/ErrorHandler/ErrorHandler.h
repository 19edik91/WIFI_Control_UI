

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include "OS_Faults.h"

#ifdef __cplusplus
extern "C"
{
#endif

/********************************* includes **********************************/


/***************************** defines / macros ******************************/


/****************************** type definitions *****************************/
typedef struct
{
    u8 eFaultCode;
    u8 ucSendTimeout;
    bool bHandlingOnLV;
}tSendErrorDelay;

/***************************** global variables ******************************/


/************************ externally visible functions ***********************/
bool ErrorHandler_HandleActualError(u8 eFaultCode, bool bSetError);

#ifdef __cplusplus
}
#endif

#endif /* ERRORHANDLER_H_ */
