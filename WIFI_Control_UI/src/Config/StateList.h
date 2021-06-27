/*
 * StateList.h
 *
 *  Created on: 07.04.2021
 *      Author: kraemere
 */

#ifndef STATE_LIST_H_
#define STATE_LIST_H_

#include "State_Active.h"
#include "State_Standby.h"
   

 //Use of X-Macros for defining User-relevant states
/*              State name           |       State-Entry-Fn      |           State-Root-Fn          |         State-Exit-Fn          */
#define USER_STATE_LIST \
   STATE(   eSM_State_Active         ,    State_Active_Entry      ,       State_Active_Root          ,      State_Active_Exit          )\
   STATE(   eSM_State_Standby        ,    State_Standby_Entry     ,       State_Standby_Root         ,      State_Standby_Exit         )
 
#endif /* STATE_LIST_H_ */
