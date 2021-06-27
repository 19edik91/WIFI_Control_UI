
//********************************************************************************
/*!
\author     Kraemer E.
\date       06.05.2021

\file       State_Standby.h
\brief      Standby state of the system

***********************************************************************************/
#ifndef _STATE_STANDBY_H_
#define _STATE_STANDBY_H_

#ifdef __cplusplus
extern "C"
{
#endif

/********************************* includes **********************************/
#include "BaseTypes.h"
#include "OS_EventManager.h"

/***************************** defines / macros ******************************/

/************************ externally visible functions ***********************/
u8 State_Standby_Entry(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2);
u8 State_Standby_Root(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2);
u8 State_Standby_Exit(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2);

#ifdef __cplusplus
}
#endif

#endif // _STATE_STANDBY_H_

/* [] END OF FILE */
