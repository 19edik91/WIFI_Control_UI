

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

/***************************** global variables ******************************/


/************************ externally visible functions ***********************/
bool ErrorHandler_HandleActualError(u8 eFaultCode, bool bSetError);

#ifdef __cplusplus
}
#endif

#endif /* ERRORHANDLER_H_ */
