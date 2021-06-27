
//********************************************************************************
/*!
\author     Kraemer E.
\date       04.05.2021

\file       State_Active.h
\brief      Active state of the system

***********************************************************************************/
#ifndef _STATE_ACTIVE_H_
#define _STATE_ACTIVE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/********************************* includes **********************************/
#include "BaseTypes.h"
#include "OS_EventManager.h"

/***************************** defines / macros ******************************/

/************************ externally visible functions ***********************/
u8 State_Active_Entry(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2);
u8 State_Active_Root(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2);
u8 State_Active_Exit(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2);

#ifdef __cplusplus
}
#endif

#endif // _OS_STATE_RESET_H_

/* [] END OF FILE */
