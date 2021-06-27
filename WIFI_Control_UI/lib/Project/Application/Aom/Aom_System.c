/* ========================================
 *
 * Copyright Eduard Kraemer, 2019
 *
 * ========================================
*/

#include "AutomaticMode.h"
#include "OS_StateManager.h"
#include "Aom_System.h"
/****************************************** Defines ******************************************************/



/****************************************** Variables ****************************************************/
static bool bIsSystemStarted = false;
static bool bSlaveInResetState = false;     //Bool to get the current "Reset pin" state
    

/****************************************** Function prototypes ******************************************/
/****************************************** loacl functiones *********************************************/
/****************************************** External visible functiones **********************************/

//********************************************************************************
/*!
\author     Kraemer E.
\date       21.08.2020
\fn         Aom_GetAutomaticModeValuesStruct
\brief      Returns the pointer reference to the automatic mode values structure.
\return     tsAutomaticModeValues - pointer reference
\param      void
***********************************************************************************/
tsAutomaticModeValues* Aom_System_GetAutomaticModeValuesStruct(void)
{
    return Aom_GetAutomaticModeSettingsPointer();
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       04.09.2020
\fn         Aom_SetSystemStarted
\brief      Sets the boolean for the system started information
\return     none
\param      void
***********************************************************************************/
void Aom_System_SetSystemStarted(bool bStarted)
{
    bIsSystemStarted = bStarted;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       04.09.2020
\fn         Aom_GetSystemStarted
\brief      Gets the system started information
\return     bIsSystemStarted - True when request-update message was received or
                                system started message was received
\param      void
***********************************************************************************/
bool Aom_System_GetSystemStarted(void)
{
    return bIsSystemStarted;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       26.09.2020
\fn         Aom_StandbyAllowed
\brief      Checks if system is allowed to switch into the standby state.
            Only in automatic state 2 and 3 allowe because there the PIR sensor is 
            enabled
\return     bStandbyAllowed - True when PIR sensor is enabled, otherwise false
\param      void
***********************************************************************************/
bool Aom_System_StandbyAllowed(void)
{
    bool bStandbyAllowed = false;
    
    teAutomaticState eState = AutomaticMode_GetAutomaticState();
    
    if(eState == eStateAutomaticMode_2 || eState == eStateAutomaticMode_3)
    {
        bStandbyAllowed = true;
    }
    
    return bStandbyAllowed;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       26.10.2020
\fn         Aom_SetSlaveResetState
\brief      Sets the boolean for the slave in reset status
\return     none
\param      bResetActive - True when slave is in reset state
***********************************************************************************/
void Aom_System_SetSlaveResetState(bool bResetActive)
{
    bSlaveInResetState = bResetActive;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       26.10.2020
\fn         Aom_GetSlaveResetState
\brief      Gets the slave reset status
\return     bSlaveInResetState - True when slave is in reset state otherwise false
\param      void
***********************************************************************************/
bool Aom_System_GetSlaveResetState(void)
{
    return bSlaveInResetState;
}
