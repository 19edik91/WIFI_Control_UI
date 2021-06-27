/* ========================================
 *
 * Copyright Eduard Kraemer, 2019
 *
 * ========================================
*/



#include "OS_EventManager.h"
#include "OS_ErrorHandler.h"

#include "DR_Regulation.h"
#include "DR_ErrorDetection.h"
#include "DR_Measure.h"

#include "Aom_Regulation.h"
#include "AutomaticMode.h"

/****************************************** Defines ******************************************************/
/****************************************** Variables ****************************************************/

/****************************************** Function prototypes ******************************************/
bool ValidatePercentValue(u8* pucValue);


/****************************************** loacl functiones *********************************************/
#if (WITHOUT_REGULATION == false)
//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\fn         ValidatePercentValue()
\brief      Checks given value for a valid range (1..100%).
\return     bValid - true when valid value was set.
\param      pucValue - Pointer to the value which should be checked.
***********************************************************************************/
bool ValidatePercentValue(u8* pucValue)
{
    bool bValid = false;    
    if(pucValue)
    {
        /* Check if value is smaller or equal zero */
        if(*pucValue < PERCENT_LOW)
        {
            *pucValue = PERCENT_LOW;
        }
        
        /* Check if request is greater than 100% */
        if(*pucValue > PERCENT_HIGH)
        {
            *pucValue = PERCENT_HIGH;
        }
        
        bValid = true;
    }    
    return bValid;
}
#endif


/****************************************** External visible functiones **********************************/
//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Writes new requested values from customer into AOM.
\return     none
\param      ucBrightnessValue - Value with the saved brightness percentage
***********************************************************************************/
void Aom_Regulation_SetCustomValue(u8 ucBrightnessValue, bool bLedStatus, bool bInitMenuActive, bool bAutomaticModeStatus, u8 ucOutputIdx)
{   
    /* Check first if output index is valid */
    if(ucOutputIdx < DRIVE_OUTPUTS)
    {        
        tRegulationValues* psRegVal = Aom_GetRegulationSettings();
        tLedValue* psLedVal = Aom_GetOutputsSettingsEntry(ucOutputIdx);
        
        /* Save LED status */
        //teRegulationState eActualState = DR_Regulation_GetActualState(ucOutputIdx);
        
        //if(DR_Regulation_GetHardwareEnabledStatus(ucOutputIdx) != bLedStatus)
        {
            psLedVal->bStatus = bLedStatus;
            
            //Get the parameter in dependency of the led status
            teEventParam eParam = (bLedStatus == OFF) ? eEvtParam_RegulationStop : eEvtParam_RegulationStart;

            /* Request a regulation state-change */
            OS_EVT_PostEvent(eEvtNewRegulationValue, eParam, ucOutputIdx);
        }
    
        /* Check first if values are new values */
        if(ucBrightnessValue != psLedVal->ucPercentValue)
        {
            if(ValidatePercentValue(&ucBrightnessValue))
            {
                /* Set customised percent value */
                psLedVal->ucPercentValue = ucBrightnessValue;
                        
                /* Calculate requested voltage value */
                u16 uiReqVoltage = DR_Measure_CalculateVoltageFromPercent(ucBrightnessValue, bInitMenuActive, ucOutputIdx);
                
                /* Calculate requested ADC value */
                psLedVal->uiReqVoltageAdc = DR_Measure_CalculateAdcValue(uiReqVoltage,0);
                            
                /* Start with event */
                OS_EVT_PostEvent(eEvtNewRegulationValue, eEvtParam_RegulationValueStartTimer, ucOutputIdx);
            }
        }

        /* Check if automatic mode has been enabled or disabled */
        if(bAutomaticModeStatus != psRegVal->sUserTimerSettings.bAutomaticModeActive)
        {
            psRegVal->sUserTimerSettings.bAutomaticModeActive = bAutomaticModeStatus;

            /* Start with event */
            OS_EVT_PostEvent(eEvtNewRegulationValue, eEvtParam_RegulationValueStartTimer, ucOutputIdx);
        }
    }
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Compares received values with already saved values
\return     none
\param      ucBrightnessValue - Value with the saved brightness percentage
***********************************************************************************/
bool Aom_Regulation_CompareCustomValue(u8 ucBrightnessValue, bool bLedStatus, u8 ucOutputIdx)
{   
    bool bDifferentValue = false;
    
    /* Check first if output index is valid */
    if(ucOutputIdx < DRIVE_OUTPUTS)
    {        
        const tLedValue* psLedVal = Aom_GetOutputsSettingsEntry(ucOutputIdx);
        
        /* Compare values between heart beat and saved values */
        if(ucBrightnessValue != psLedVal->ucPercentValue
            || bLedStatus != psLedVal->bStatus)
        {
            bDifferentValue = true;
        }
    }
    
    return bDifferentValue;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Writes new requested values from customer into AOM.
\return     none
\param      ucBrightnessValue - Value with the saved brightness percentage
***********************************************************************************/
void Aom_Regulation_CheckRequestValues(u8 ucBrightnessValue, bool bLedStatus, bool bNightModeOnOff, bool bMotionDetectionOnOff,
                            u8 ucBurnTime, bool bInitMenuActive, bool bAutomaticModeStatus, u8 ucOutputIdx)
{   
    tsAutomaticModeValues* psAutomaticModeVal = Aom_GetAutomaticModeSettingsPointer();
    tRegulationValues* psRegVal = Aom_GetRegulationSettings();
    
    /* Check for over-current */
    //if(OS_ErrorHandler_GetErrorTimeout())
    //{
    //    /* Overcurrent was detected. Reduce requested brighntess value by half */
    //    ucBrightnessValue /= 2;
    //}
    
    /* Get new automatic mode state according to the set modes */
    teAutomaticState eAutoState = eStateDisabled;    
    if(bMotionDetectionOnOff == ON && bAutomaticModeStatus == ON)
    {
        eAutoState = eStateAutomaticMode_2;
    }
    else if(bMotionDetectionOnOff == ON && bAutomaticModeStatus == OFF)
    {
        eAutoState = eStateAutomaticMode_3;
    }
    else if(bMotionDetectionOnOff == OFF && bAutomaticModeStatus == ON)
    {
        eAutoState = eStateAutomaticMode_1;
    }
    AutomaticMode_ChangeState(eAutoState);
    
    /* Get automatic mode LED status dependent of the automatic mode */
    bool bAutomaticLedState = AutomaticMode_Handler();
    
    /* Overwrite user settings only when automatic mode is active */
    if(eAutoState != eStateDisabled)
    {
        bLedStatus = bAutomaticLedState;
    }
    
    /* Reduce brightness when night mode time slot is active */
    ucBrightnessValue = (psAutomaticModeVal->bInNightModeTimeSlot == true) ? PERCENT_LOW : ucBrightnessValue;
        
    /* Save requested night mode */
    psRegVal->bNightModeOnOff = bNightModeOnOff;    
    
    /* Save motion detection status */
    psRegVal->sUserTimerSettings.bMotionDetectOnOff = bMotionDetectionOnOff;
    psRegVal->sUserTimerSettings.ucBurningTime = ucBurnTime;
    
    /* Set changed customer values */
    Aom_Regulation_SetCustomValue(ucBrightnessValue, bLedStatus, bInitMenuActive, bAutomaticModeStatus, ucOutputIdx);
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Writes requested values from AOM into pointed structure.
\return     none
\param      psRegulationValues - Pointer to the structure.
***********************************************************************************/
void Aom_Regulation_GetRegulationValues(tRegulationValues* psRegulationValues)
{
    if(psRegulationValues)
    {
        memcpy(psRegulationValues, Aom_GetRegulationSettings(), sizeof(tRegulationValues));
    }
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       27.05.2020
\brief      Returns a pointer to the regualtion values.
\return     const tRegulationValues pointer
***********************************************************************************/
const tRegulationValues* Aom_Regulation_GetRegulationValuesPointer(void)
{
    return Aom_GetRegulationSettings();
}

#if (WITHOUT_REGULATION == false)
//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\fn         Aom_SetCalculatedVoltageValue()
\brief      Writes new requested values from customer into AOM.
\return     none
\param      psRegulationValues - Pointer with the calculated values
***********************************************************************************/
//void Aom_Regulation_SetCalculatedVoltageValue(tRegulationValues* psRegulationValues)
//{
//    if(psRegulationValues)
//    {
//        tRegulationValues* psRegVal = Aom_GetRegulationSettings();
//        psRegVal->sLedValue.uiIsVoltage = psRegulationValues->sLedValue.uiIsVoltage;        
//    }
//}


//********************************************************************************
/*!
\author     Kraemer E.
\date       22.10.2019
\brief      Sets the minimum values of the system
\return     none
***********************************************************************************/
void Aom_Regulation_SetMinSystemSettings(u16 uiCurrentAdc, u16 uiVoltageAdc, u16 uiCompVal, u8 ucOutputIdx)
{
    tsSystemSettings* psSystemSettings = Aom_GetSystemSettingsEntry(ucOutputIdx);    
    psSystemSettings->uiMinAdcCurrent = uiCurrentAdc;
    psSystemSettings->uiMinAdcVoltage = uiVoltageAdc;
    psSystemSettings->uiMinCompVal = uiCompVal;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       22.10.2019
\brief      Sets the maximum values of the system
\return     none
***********************************************************************************/
void Aom_Regulation_SetMaxSystemSettings(u16 uiCurrentAdc, u16 uiVoltageAdc, u16 uiCompVal, u8 ucOutputIdx)
{
    tsSystemSettings* psSystemSettings = Aom_GetSystemSettingsEntry(ucOutputIdx);
    psSystemSettings->uiMaxAdcCurrent = uiCurrentAdc;
    psSystemSettings->uiMaxAdcVoltage = uiVoltageAdc;
    psSystemSettings->uiMaxCompVal = uiCompVal;
}

#endif

