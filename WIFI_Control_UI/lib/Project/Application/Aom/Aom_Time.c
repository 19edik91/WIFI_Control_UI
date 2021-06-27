/* ========================================
 *
 * Copyright Eduard Kraemer, 2019
 *
 * ========================================
*/
#include "Aom_Time.h"
#include "OS_EventManager.h"

/****************************************** Defines ******************************************************/
/****************************************** Variables ****************************************************/
/****************************************** Function prototypes ******************************************/
/****************************************** loacl functiones *********************************************/
/****************************************** External visible functiones **********************************/
//********************************************************************************
/*!
\author     Kraemer E.
\date       27.05.2020
\brief      Saves the time from the ESP
\return     none
\param      ucHour - The time value in hours
\param      ucMin - The time value in minutes
***********************************************************************************/
void Aom_Time_SetReceivedTime(u8 ucHour, u8 ucMin, u32 ulTicks)
{
    tsCurrentTime* psCurrentTime = Aom_GetCurrentTimePointer();
    psCurrentTime->ucHours = ucHour;
    psCurrentTime->ucMinutes = ucMin;
    psCurrentTime->ulTicks = ulTicks;
    
    OS_EVT_PostEvent(eEvtTimeReceived, eEvtParam_TimeFromNtpClient ,0);
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       07.10.2020
\brief      Saves the time from the RTC
\return     none
\param      ucHour - The time value in hours
\param      ucMin - The time value in minutes
***********************************************************************************/
void Aom_Time_SetRealTimeClockTime(u8 ucHour, u8 ucMin, u32 ulTicks)
{
    tsCurrentTime* psCurrentTime = Aom_GetCurrentTimePointer();
    psCurrentTime->ucHours = ucHour;
    psCurrentTime->ucHours = ucHour;
    psCurrentTime->ucMinutes = ucMin;
    psCurrentTime->ulTicks = ulTicks;
    
    OS_EVT_PostEvent(eEvtTimeReceived, eEvtParam_TimeFromRtc, 0);
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       27.05.2020
\brief      Gets the saved time structure as a pointer
\return     tsCurrentTime* - Address to the structure
***********************************************************************************/
const tsCurrentTime* Aom_Time_GetCurrentTime(void)
{
    return Aom_GetCurrentTimePointer();
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       08.12.2019
\brief      Saves the new received timer settings into structure
\return     none
\param      psUserTimerSettings - Pointer to the time setting structure
\param      ucTimerIdx - The time index which shall be overwritten
***********************************************************************************/
void Aom_Time_SetUserTimerSettings(tsTimeFormat* psUserTimerSettings, u8 ucTimerIdx)
{
    bool bTimerSettingsChanged = false;
    
    /* Get access to the regulation value */
    tRegulationValues* psRegulationValues = Aom_GetRegulationSettings();
    
    if(psUserTimerSettings)
    {
        if(psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucHourSet != psUserTimerSettings->ucHourSet)
        {
            psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucHourSet = psUserTimerSettings->ucHourSet;
            bTimerSettingsChanged = true;
        }
        
        if(psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucHourClear != psUserTimerSettings->ucHourClear)
        {
            psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucHourClear = psUserTimerSettings->ucHourClear;
            bTimerSettingsChanged = true;
        }
        
        if(psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucMinClear != psUserTimerSettings->ucMinClear)
        {
            psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucMinClear = psUserTimerSettings->ucMinClear;
            bTimerSettingsChanged = true;
        }
        
        if(psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucMinSet != psUserTimerSettings->ucMinSet)
        {
            psRegulationValues->sUserTimerSettings.sTimer[ucTimerIdx].ucMinSet = psUserTimerSettings->ucMinSet;
            bTimerSettingsChanged = true;
        }
    }
    
    if(bTimerSettingsChanged)
    {
        psRegulationValues->sUserTimerSettings.ucSetTimerBinary |= 0x01 << ucTimerIdx;
        
        /* Start with event */
        OS_EVT_PostEvent(eEvtNewRegulationValue, eEvtParam_RegulationValueStartTimer, 0);
    }    
}

