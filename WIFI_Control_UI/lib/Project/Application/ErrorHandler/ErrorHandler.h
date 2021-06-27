

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include "Faults.h"
#include "Aom.h"

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
void ErrorHandler_HandleActualError(u8 eFaultCode, bool bSetError);
bool ErrorHandler_SendErrorMessage(void);


#ifdef __cplusplus
}
#endif

#endif /* ERRORHANDLER_H_ */
