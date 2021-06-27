


/********************************* includes **********************************/
#include "OS_StateManager.h"

#include "State_Standby.h"
#include "HAL_System.h"
/***************************** defines / macros ******************************/

/************************ local data type definitions ************************/

/************************* local function prototypes *************************/

/************************* local data (const and var) ************************/

/************************ export data (const and var) ************************/


/****************************** local functions ******************************/

//********************************************************************************
/*!
\author  KraemerE
\date    18.02.2019
\brief   In this state the CPU shall only wait for a message from the UIM,
         do some self-tests and maybe go to sleep.
\param   none
\return  none
***********************************************************************************/
static void EnterSleepMode(void)
{    
    /* Go back to  deep sleep. Sleep is leaved with the next interrupt from a 
        wake up source (Watchdog-Timer or UART changes) */
    //wifi_set_sleep_type(LIGHT_SLEEP_T);

    /* Enter deep sleep forever. Device is waked up per reset from master */
    HAL_System_EnterDeepsleep();
    HAL_System_Delay(100);
}

/************************ externally visible functions ***********************/
//***************************************************************************
/*!
\author     KraemerE
\date       30.04.2021
\brief      Entry state of this state.
\return     u8
\param      eEventID - Event which shall be handled while in this state.
\param      uiParam1 - Event parameter from the received event
\param      ulParam2 - Second event parameter from the received event
******************************************************************************/
u8 State_Standby_Entry(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_PROCESSED;
    
    /* Make compiler happy */
    (void)eEventID;
    (void)uiParam1;
    (void)ulParam2;  
        
    /* Switch state to root state */
    OS_StateManager_CurrentStateReached();

    return ucReturn;
}


//***************************************************************************
/*!
\author     KraemerE
\date       30.04.2021
\brief      Entry state of this state.
\return     u8
\param      eEventID - Event which shall be handled while in this state.
\param      uiParam1 - Event parameter from the received event
\param      ulParam2 - Second event parameter from the received event
******************************************************************************/
u8 State_Standby_Root(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_PROCESSED;
    
    /* Make compiler happy */
    (void)eEventID;
    (void)uiParam1;
    (void)ulParam2;  
    
    /* Enter deep sleep mode */
    EnterSleepMode();
    
    OS_StateManager_CurrentStateReached();

    return ucReturn;
}


//***************************************************************************
/*!
\author     KraemerE
\date       30.04.2021
\brief      Exit state of this state.
\return     u8
\param      eEventID - Event which shall be handled while in this state.
\param      uiParam1 - Event parameter from the received event
\param      ulParam2 - Second event parameter from the received event
******************************************************************************/
u8 State_Standby_Exit(teEventID eEventID, uiEventParam1 uiParam1, ulEventParam2 ulParam2)
{
    u8 ucReturn = EVT_PROCESSED;
    
    /* Make compiler happy */
    (void)eEventID;
    (void)uiParam1;
    (void)ulParam2;    

    /* Switch state next state */
    OS_StateManager_CurrentStateReached();
    return ucReturn;
}


