/* ========================================
 *
 * Copyright Eduard Kraemer, 2019
 *
 * ========================================
*/

#include "OS_EventManager.h"

#include "DR_Regulation.h"
#include "DR_Measure.h"
#include "DR_ErrorDetection.h"

#include "Aom_Measure.h"


/****************************************** Defines ******************************************************/
/****************************************** Variables ****************************************************/
#if (WITHOUT_REGULATION == false)
static u8 ucDigitsPerVoltageStep = 0;
static u8 ucDigitsCurrentLimit = 0;
#endif


/****************************************** Function prototypes ******************************************/
/****************************************** loacl functiones *********************************************/
/****************************************** External visible functiones **********************************/

#if (WITHOUT_REGULATION == false)
//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Writes actual ADC values into the structure.
\return     none
\param      uiAdcVal - The new ADC value from the sequenced channel
\param      eChannel - Type of measurement channel
\param      ucChannelIdx - The number of the measurement channel 
***********************************************************************************/
void Aom_Measure_SetActualAdcValues(u16 uiAdcVal, teMeasureType eChannel, u8 ucChannelIdx)
{
    tRegulationValues* psRegVal = Aom_GetRegulationSettings();
    
    /* Check for valid channel */
    switch(eChannel)
    {
        case eMeasureChVoltage:
        {
            if(ucChannelIdx < DRIVE_OUTPUTS)
                psRegVal->sLedValue[ucChannelIdx].uiIsVoltageAdc = uiAdcVal;
            break;
        }
        
        case eMeasureChCurrent:
        {
            if(ucChannelIdx < DRIVE_OUTPUTS)
                psRegVal->sLedValue[ucChannelIdx].uiIsCurrentAdc = uiAdcVal;
            break;
        }
        
        case eMeasureChTemp:
        {
            if(ucChannelIdx < DRIVE_OUTPUTS)
                psRegVal->uiNtcAdcValue[ucChannelIdx] = uiAdcVal;        
            break;
        }
            
        case eMeasureChInvalid:
        default:
            break;
    }    
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       22.10.2019
\brief      Returns the actual ADC value
\return     uiIsAdcVal - The actual measured ADC value
\param      eChan - The channel which should be returned 
\param      ucOutputIdx - The output channel which shall be returned
***********************************************************************************/
u16 Aom_Measure_GetAdcIsValue(teMeasureType eChan, u8 ucOutputIdx)
{
    u16 uiAdcValue = 0xFFFF;
    
    const tRegulationValues* psRegVal = Aom_GetRegulationSettings();
    
    /* Check for valid pointer and valid output index */
    if(psRegVal && (ucOutputIdx < DRIVE_OUTPUTS))
    {    
        switch(eChan)
        {
            case eMeasureChVoltage:
                uiAdcValue = psRegVal->sLedValue[ucOutputIdx].uiIsVoltageAdc;
                break;
            
            case eMeasureChCurrent:
                uiAdcValue = psRegVal->sLedValue[ucOutputIdx].uiIsCurrentAdc;
                break;
            
            case eMeasureChTemp:
                uiAdcValue = psRegVal->uiNtcAdcValue[ucOutputIdx];
                break;
                
            case eMeasureChInvalid:
            default:
                break;
        }
    }
    return uiAdcValue;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       03.04.2019
\brief      Returns the requested ADC value.
\return     uiReqAdcVal - The requested value from the user
\param      ucOutputIdx - The ADC value from the given output index
***********************************************************************************/
u16 Aom_Measure_GetAdcRequestedValue(u8 ucOutputIdx)
{
    tLedValue* psLedVal = Aom_GetOutputsSettingsEntry(ucOutputIdx);    
    return psLedVal->uiReqVoltageAdc;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       20.10.2019
\brief      Returns the measured current ADC value.
\return     uiIsCurrentAdc - The measured current ADC value
\param      ucOutputIdx - The ADC value from the given output index
***********************************************************************************/
u16 Aom_Measure_GetMeasuredCurrentAdcValue(u8 ucOutputIdx)
{
    tLedValue* psLedVal = Aom_GetOutputsSettingsEntry(ucOutputIdx);  
    return psLedVal->uiIsCurrentAdc;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       22.10.2019
\fn         Aom_GetAdcVoltageStepValue
\brief      Returns the calculated amount of digits per defined voltage step.
\return     ucDigitsPerVoltageStep - The amount of digits per voltage step
\param      none
***********************************************************************************/
u16 Aom_Measure_GetAdcVoltageStepValue(void)
{
    if(ucDigitsPerVoltageStep == 0)
    {
        ucDigitsPerVoltageStep = DR_Measure_CalculateAdcValue(VOLTAGE_STEP, 0);
    }
    
    return ucDigitsPerVoltageStep;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       22.10.2019
\fn         Aom_GetAdcCurrentLimitValue
\brief      Returns the calculated amount of digits for the maximum defined current
\return     ucDigitsCurrentLimit - The amount of digits for the current limit
\param      none
***********************************************************************************/
u16 Aom_Measure_GetAdcCurrentLimitValue(void)
{
    if(ucDigitsCurrentLimit == 0)
    {
        ucDigitsCurrentLimit = DR_Measure_CalculateAdcValue(0, CURRENT_MAX_LIMIT);
    }        
    
    return ucDigitsCurrentLimit; 
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       08.11.2019
\fn         Aom_GetMeasuredValues
\brief      Returns the calculated values like temperature, current and voltage.
\return     none
\param      puiVoltage - Pointer where the voltage shall be saved
\param      puiCurrent - Pointer where the current shall be saved
\param      psiTemperature - Pointer where the temperature shall be saved
\param      ucOutputIdx - The requested output
***********************************************************************************/
void Aom_Measure_GetMeasuredValues(u32* pulVoltage, u16* puiCurrent, s16* psiTemperature, u8 ucOutputIdx)
{
    tsConvertedMeasurement* psCvMeasure = Aom_GetConvertedMeasurementPointer();
    
    if(pulVoltage)
    {
        *pulVoltage = psCvMeasure->sOutput[ucOutputIdx].ulMilliVolt;
    }
    
    if(puiCurrent)
    {
        *puiCurrent = psCvMeasure->sOutput[ucOutputIdx].uiMilliAmp;
    }
    
    if(psiTemperature)
    {
        *psiTemperature = psCvMeasure->sOutput[ucOutputIdx].uiTemp;
    }
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       18.11.2019
\fn         Aom_SetMeasuredValues
\brief      Calculates and saves the actual converted measurement values into AOM
\return     none
\param      bVoltage - Set true to save the actual voltage measurement
\param      bCurrent - Set true to save the actual current measurement
\param      bTemperature - Set true to save the temperature
***********************************************************************************/
void Aom_Measure_SetMeasuredValues(bool bVoltage, bool bCurrent, bool bTemperature)
{
    tsConvertedMeasurement* psCvMeasure = Aom_GetConvertedMeasurementPointer();
    const tRegulationValues* psRegVal = Aom_GetRegulationSettings();
    
    u8 ucOutputIdx;
    for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        if(bVoltage)
        {
            psCvMeasure->sOutput[ucOutputIdx].ulMilliVolt = DR_Measure_CalculateVoltageValue(psRegVal->sLedValue[ucOutputIdx].uiIsVoltageAdc);
        }
        
        if(bCurrent)
        {
            psCvMeasure->sOutput[ucOutputIdx].uiMilliAmp = DR_Measure_CalculateCurrentValue(psRegVal->sLedValue[ucOutputIdx].uiIsCurrentAdc);
        }
        
        if(bTemperature)
        {
            psCvMeasure->sOutput[ucOutputIdx].uiTemp = DR_Measure_CalculateTemperatureValue(psRegVal->uiNtcAdcValue[ucOutputIdx]);
        }
    }
}
#endif



//********************************************************************************
/*!
\author     Kraemer E.
\date       20.12.2020
\brief      Checks if system voltage was already calculated.
\return     bSystemVoltageCalculated - True when already calculated otherwise false
\param      none
***********************************************************************************/
bool Aom_Measure_SystemVoltageCalculated(void)
{   
    return (DR_Measure_GetSystemVoltage() == 0) ? false : true;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       10.11.2019
\brief      Wrapper function for better modularity
\return     u32 - System voltage in millivolt
\param      uiAdcValue - The ADC value of a voltage measurement
***********************************************************************************/
u32 Aom_Measure_CalculateSystemVoltage(void)
{    
    u32 ulAvgSystemVoltage = 0;
    
    #ifndef TARGET_SYSTEM_VOLTAGE
        u32 ulSystemVoltage[DRIVE_OUTPUTS];
    
        /* PWM-Modules shall be disabled but the system voltage should be enabled */
        u8 ucOutputIdx;
        for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
        {
            sPwmMap[ucOutputIdx].pfnStop();
        }
        Actors_SetOutputStatus(eVoltageEnable, true);
        
        /* Wait for a few milliseconds until some measurement data was collected */
        CyDelay(200);
        
        /* First step get measured voltage from each output by calling the measure tick */
        u16 uiAdcOutput[DRIVE_OUTPUTS];    
        for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
        {
            /* The voltage on the coil is equal to the system voltage when the PWM-Modules are disabled */
            uiAdcOutput[ucOutputIdx] = Measure_GetAveragedAdcValue(ucOutputIdx);
            
            /* Now the calculated values */
            ulSystemVoltage[ucOutputIdx] = Measure_CalculateVoltageValue(uiAdcOutput[ucOutputIdx]);
            
            /* Add each system voltage values */
            ulAvgSystemVoltage += ulSystemVoltage[ucOutputIdx];
        }
        
        /* Get the average value */
        ulAvgSystemVoltage /= DRIVE_OUTPUTS;
    #else
        ulAvgSystemVoltage = TARGET_SYSTEM_VOLTAGE;
    #endif
    /* Save the new system voltage */
    DR_Measure_SetSystemVoltage(ulAvgSystemVoltage);
    
    return ulAvgSystemVoltage;
}