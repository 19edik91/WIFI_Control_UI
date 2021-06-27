/* ========================================
 *
 * Copyright Eduard Kraemer, 2019
 *
 * ========================================
*/
#include "Aom.h"

/****************************************** Defines ******************************************************/
/****************************************** Variables ****************************************************/
static tRegulationValues sRegulationValues;
/****************************************** Function prototypes ******************************************/
bool ValidatePercentValue(u8* pucValue);

typedef struct 
{
    u32 ulPower;        //Power in milli watt
    u32 ulVoltage;      //Voltage in milli volt
    u16 uiCurrent;      //Current in milli ampere 
}tsElectricData;

typedef struct 
{
    tsElectricData sElectricData[DRIVE_OUTPUTS];
    s16 siNtcTemp;
}tsSystemData;

static tsSystemData sSystemData;

//Boolen variable. Is set to true when the message from the master is received
static bool bTimerSettingsInitDone = false;
static bool bOutputSettingsInitDone[DRIVE_OUTPUTS] = {false, false, false};
static bool bSettingsReceivedDone = false;

static tsOutputValues sOutputValues;
static tsAutomaticValues sAutomaticValues;
static s32 slRemaingBurnTime = 0;

static bool bNewAdcValues = false;
static tsAdcValues sAdcValues;

static tsDate sDateNow;
/****************************************** loacl functiones *********************************************/

//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
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

/****************************************** External visible functiones **********************************/
//********************************************************************************
/*!
\author     Kraemer E.
\date       01.01.2021
\brief      Initializes the structures
\return     none
\param      none
***********************************************************************************/
void Aom_InitStructuresWithDefaultValues(void)
{       
    for(u8 ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        sOutputValues.sBrightnessValue[ucOutputIdx].ucMinValue = PERCENT_LOW;
        sOutputValues.sBrightnessValue[ucOutputIdx].ucMaxValue = PERCENT_HIGH;
        sOutputValues.sBrightnessValue[ucOutputIdx].bOnOff = OFF;
        sOutputValues.sBrightnessValue[ucOutputIdx].ucSliderValue = PERCENT_LOW;
    }

    for(u8 ucTimerEntry = 0; ucTimerEntry < USER_TIMER_AMOUNT; ucTimerEntry++)
    {
        sAutomaticValues.sStartHour[ucTimerEntry].ucTime = 6;
        sAutomaticValues.sStopHour[ucTimerEntry].ucTime = 21;
    }
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Writes new requested values from customer into AOM.
\return     ucValuesChanged - Returns the output of which the values has changed
\param      ucBrightnessValue - Value with the saved brightness percentage
\param      ucOutputIndex - The output which request a new setting
\param      bLedStatus - The state of the LED (On or Off)
\param      bRootPageActive - True when the current page is the root page
***********************************************************************************/
u8 Aom_SetCustomValue(u8 ucBrightnessValue, u8 ucOutputIndex, bool bLedStatus, bool bRootPageActive)
{       
    u8 ucValuesChanged = 0xFF;

    /* Check for valid output */
    if(ucOutputIndex < DRIVE_OUTPUTS)
    {
        /* Use pointer for easier handling */
        tLedValue* psLedVal = &sRegulationValues.sLedValue[ucOutputIndex];

        /* Check first if values have changed */
        if(ucBrightnessValue != psLedVal->ucPercentValue 
            || bLedStatus != psLedVal->bStatus 
            || sRegulationValues.bMainPageActive != bRootPageActive)
        {
            /* Validate brightness value */
            ValidatePercentValue(&ucBrightnessValue);

            /* Set customised percent value */
            psLedVal->ucPercentValue = ucBrightnessValue;

            /* Save LED status */
            psLedVal->bStatus = bLedStatus;   

            /* Save new page status */
            sRegulationValues.bMainPageActive = bRootPageActive;

            ucValuesChanged = ucOutputIndex;
        }
    }
    return ucValuesChanged;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       31.05.2020
\brief      Writes updated values from Master
\return     none
\param      ucBrightnessValue - Value with the updated brightness percentage
\param      ucOutputIndex - The output which returns the current settings
\param      bLedStatus - The actual LED status
\param      bNightModeOnOff - The actual night mode status
\param      bMotionDetected - The status about a detected motion 
\param      ucRemainBurnTime - The amount of the remaining time, where the LED will
                                be on.
***********************************************************************************/
void Aom_UpdateOutputStatus(u8 ucBrightnessVal, u8 ucOutputIndex, bool bLedStatus, bool bNightModeOnOff, bool bMotionDetected, s32 slRemainBurnTime)
{
    if(ucOutputIndex < DRIVE_OUTPUTS)
    {
        /* Overwrite the output values with the new one from the master */
        sOutputValues.sBrightnessValue[ucOutputIndex].bOnOff = bLedStatus;
        sOutputValues.sBrightnessValue[ucOutputIndex].ucSliderValue = ucBrightnessVal;

        slRemaingBurnTime = slRemainBurnTime;
        /* TODO:
            NightModeOnOff - Display if the night mode is active (in time slot or not)
            MotionDetected - Display if the PIR has detected a motion
        */
    }
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       13.05.2020
\fn         Aom_SetNightModeValue
\brief      Writes new requested values from customer into AOM.
\return     none
\param      bNightModeOnOff - True when the night mode is enabled other false
***********************************************************************************/
bool Aom_SetNightModeValue(bool bNightModeOnOff)
{       
    bool bValuesChanged = false;
    
    /* Check first if values have changed */
    if(bNightModeOnOff != sRegulationValues.bNightModeOnOff)
    {
        sRegulationValues.bNightModeOnOff = bNightModeOnOff;
        bValuesChanged = true;
    }

    return bValuesChanged;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       13.05.2020
\fn         Aom_SetMotionDetectionModeValue
\brief      Writes new requested values from customer into AOM.
\return     none
\param      bMotionDetectOnOff - True when the motion detection mode is enabled
***********************************************************************************/
bool Aom_SetMotionDetectionModeValue(bool bMotionDetectOnOff, u8 ucBurnTime)
{       
    bool bValuesChanged = false;
    
    /* Check first if values have changed */
    if(bMotionDetectOnOff != sRegulationValues.bMotionDetectionOnOff
        || ucBurnTime != sRegulationValues.ucBurnTime)
    {
        sRegulationValues.bMotionDetectionOnOff = bMotionDetectOnOff;
        sRegulationValues.ucBurnTime = ucBurnTime;
        bValuesChanged = true;
    }

    return bValuesChanged;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       13.05.2020
\fn         Aom_SetMotionDetectionModeValue
\brief      Writes new requested values from customer into AOM.
\return     none
\param      bMotionDetectOnOff - True when the motion detection mode is enabled
***********************************************************************************/
bool Aom_SetAutomaticModeValue(bool bAutomaticModeOnOff)
{       
    bool bValuesChanged = false;
    
    /* Check first if values have changed */
    if(bAutomaticModeOnOff != sRegulationValues.bAutomaticModeStatus)
    {
        sRegulationValues.bAutomaticModeStatus = bAutomaticModeOnOff;
        bValuesChanged = true;
    }

    return bValuesChanged;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       18.03.2019
\fn         Aom_GetCustomValue
\brief      Returns the saved regulation values
\return     none
\param      psRegValues - Pointer to the given regulation values
***********************************************************************************/
const tRegulationValues* Aom_GetCustomValue(void)
{
    #if 0
    if(psRegValues)
    {
        /* Use memcpy instead 
        psRegValues->sLedValue.ucPercentValue = sRegulationValues.sLedValue.ucPercentValue;
        psRegValues->sLedValue.bStatus = sRegulationValues.sLedValue.bStatus;
        psRegValues->bMainPageActive = sRegulationValues.bMainPageActive;

        psRegValues->bAutomaticModeStatus = sRegulationValues.bAutomaticModeStatus;
        psRegValues->bMotionDetectionOnOff = sRegulationValues.bMotionDetectionOnOff;
        psRegValues->ucBurnTime = sRegulationValues.ucBurnTime;
        psRegValues->bNightModeOnOff = sRegulationValues.bNightModeOnOff;
        */
        memcpy(psRegValues, &sRegulationValues, sizeof(tRegulationValues));
    }
    #else
    return &sRegulationValues;
    #endif
}

//********************************************************************************
/*!
\author     Kraemer E
\date       08.11.2019
\fn         Aom_SetNewMeasureValues
\brief      Puts new measured values into variables
\return     none
\param      ulVolt - Voltage value in millivolt
\param      uiCurr - Current value in milliampere
\siTemp     psiTemp - Tempereature in double accuracy (25.00°C)
***********************************************************************************/
void Aom_SetNewMeasureValues(u8 ucOutputIndex, u32 ulVolt, u16 uiCurr, s16* psiTemp)
{
    if(ucOutputIndex < DRIVE_OUTPUTS)
    {
        sSystemData.sElectricData[ucOutputIndex].ulVoltage = ulVolt;
        sSystemData.sElectricData[ucOutputIndex].uiCurrent = uiCurr;
        sSystemData.sElectricData[ucOutputIndex].ulPower = (ulVolt * uiCurr) ;
    }

    if(psiTemp)
        sSystemData.siNtcTemp = *psiTemp;
}

//********************************************************************************
/*!
\author     Kraemer E
\date       08.12.2019
\fn         Aom_GetMeasureValues
\brief      Gets measured values from variables
\return     none
\param      
***********************************************************************************/
void Aom_GetMeasureValues(u8 ucOutputIndex, u32* pulVolt, u16* puiCurrent, s16* psiTemp, u32* pulPower)
{
    if(psiTemp)
    {
        *psiTemp = sSystemData.siNtcTemp;
    }

    if(ucOutputIndex < DRIVE_OUTPUTS)
    {
        if(pulVolt)
        {
            *pulVolt = sSystemData.sElectricData[ucOutputIndex].ulVoltage;
        }

        if(puiCurrent)
        {
            *puiCurrent = sSystemData.sElectricData[ucOutputIndex].uiCurrent;
        }

        if(pulPower)
        {
            *pulPower = sSystemData.sElectricData[ucOutputIndex].ulPower;
        }
    }
}

//********************************************************************************
/*!
\author     Kraemer E
\date       13.05.2020
\fn         Aom_SetSavedUserSettings
\brief      Sets the values which were saved in the flash of the master.
\return     none
\param      ucBrightness - Slider value for the brightness setting
\param      bOnOff - Booelan for the output enabled state
\param      bAutomaticModeOnOff - True when the automatic timer mode is enabled
\param      bNightModeOnOff - True when the night mode is enabled
\param      bMotionDetectOnOff - True when the Motion detection is enabled (PIR) 
***********************************************************************************/
void Aom_SetSavedUserSettings(u8 ucOutputIndex, u8 ucBrightness, bool bOnOff, bool bAutomaticModeOnOff, bool bNightModeOnOff, bool bMotionDetectOnOff, u8 ucBurnTime )
{
    if(ucOutputIndex < DRIVE_OUTPUTS)
    {
        sOutputValues.sBrightnessValue[ucOutputIndex].bOnOff = bOnOff;
        sOutputValues.sBrightnessValue[ucOutputIndex].ucSliderValue = ucBrightness;
    }

    sAutomaticValues.bAutomaticOnOff = bAutomaticModeOnOff;
    sAutomaticValues.bMotionDetectOnOff = bMotionDetectOnOff;
    sAutomaticValues.bNightModeOnOff = bNightModeOnOff;
    sAutomaticValues.ucEndurance = ucBurnTime;
}

//********************************************************************************
/*!
\author     Kraemer E
\date       13.05.2020
\fn         Aom_SettingsMsgReceived
\brief      Sets a boolean variable to true when received.
            Both init done bools have to be true for sending the output state 
            message.
\return     none
\param      bTimerSettings - True when timer settings message was received
\param      bOutputSettings - True when output settings message was received
***********************************************************************************/
void Aom_SettingsMsgReceived(bool bTimerSettings, bool bOutputSettings, u8 ucOutputIdx)
{
    if(bTimerSettings)
    {
        bTimerSettingsInitDone = true;
    }

    if(bOutputSettings)
    {
        if(ucOutputIdx < DRIVE_OUTPUTS)
        {
            bOutputSettingsInitDone[ucOutputIdx] = true;
        }
    }
}

//********************************************************************************
/*!
\author     Kraemer E
\date       13.05.2020
\brief      Returns the settings message received status.
\return     none
\param      none
***********************************************************************************/
bool Aom_GetSettingsMsgReceived(void)
{
    bool bOutputInitDone = false;

    for(u8 ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        if(bOutputSettingsInitDone[ucOutputIdx])
        {
            bOutputInitDone = true;
        }
    }

    return (bTimerSettingsInitDone && bOutputInitDone);
}


//********************************************************************************
/*!
\author     Kraemer E
\date       13.05.2020
\brief      Returns the settings message received status.
\return     none
\param      none
***********************************************************************************/
void Aom_SetSettingsInitDone(void)
{
    bSettingsReceivedDone = true;
}


//********************************************************************************
/*!
\author     Kraemer E
\date       10.12.2019
\fn         Aom_SettingsInitDone
\brief      Returns true or false according to the settings done state
\return     Boolean
\param      none
***********************************************************************************/
bool Aom_SettingsInitDone(void)
{
    return bSettingsReceivedDone;
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         Aom_BrightnessPointer
\brief      Returns pointer to the brightness value structure
\return     tsBrightnessValue - Pointer to the brighntess value structure
\param      none
***********************************************************************************/
tsOutputValues* Aom_OutputValuePointer(void)
{
    return &sOutputValues;
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         Aom_AutomaticPointer
\brief      Returns pointer to the automatic value structure
\return     tsAutomaticValues - Pointer to the brighntess value structure
\param      none
***********************************************************************************/
tsAutomaticValues* Aom_AutomaticPointer(void)
{
    return &sAutomaticValues;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       31.05.2020
\fn         Aom_GetActualTime
\brief      Get the saved time
\return     none
\param      none
***********************************************************************************/
void Aom_GetActualTime(u8* pucHours, u8* pucMinutes, u32* pulLocTicks)
{   
    if(pucHours)
        *pucHours = sDateNow.ucHour;

    if(pucMinutes)
        *pucMinutes = sDateNow.ucMinute;
    
    if(pulLocTicks)
        *pulLocTicks = sDateNow.ulEpochTick;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       26.09.2020
\fn         Aom_SetDate
\brief      Saves the date values from an NTP server
\return     none
\param      sDate - The date structure
***********************************************************************************/
void Aom_SetDate(tsDate* psDate)
{
    memcpy(&sDateNow, psDate, sizeof(tsDate));
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       26.09.2020
\fn         Aom_GetDate
\brief      Returns the date values
\return     sDate - The date structure
\param      none
***********************************************************************************/
const tsDate* Aom_GetDate(void)
{
    return &sDateNow;
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       27.09.2020
\fn         Aom_GetBurningTimeValues
\brief      Returns the burning time values
\return     sDate - The date structure
\param      pucRemainingTime - Pointer to the variable which should hold the remaining time
\param      pucInitialTime - Pointer to the variable which should hold the initial set time
***********************************************************************************/
void Aom_GetBurningTimeValues(s32* pslRemainingTime, s32* pslInitialTime)
{
    if(pslRemainingTime)
        *pslRemainingTime = slRemaingBurnTime;

    if(pslInitialTime)
    {
        /* Convert minutes value into milliseconds value (x 60.000 or bitshift by 16 (=65.536) */
        *pslInitialTime = sAutomaticValues.ucEndurance << 16;
        pslInitialTime -= 5536;
    }
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       01.11.2020
\brief      Sets the received ADC values
\return     none
\param      tsAdcValues - Structure with the new adc values
***********************************************************************************/
void Aom_SetAdcValues(tsAdcValues* psAdcValue)
{
    if(psAdcValue)
    {
        memcpy(&sAdcValues, psAdcValue, sizeof(tsAdcValues));
        bNewAdcValues = true;
    }
}

//********************************************************************************
/*!
\author     Kraemer E.
\date       01.11.2020
\brief      Gets the received ADC values
\return     bNewAdcValues - True when values are new otherwise the same values
                            where already read
\param      tsAdcValues - Structure with the adc values
***********************************************************************************/
bool Aom_GetAdcValues(tsAdcValues* psAdcValue)
{
    bool bLocalCopy = false;

    if(psAdcValue)
    {
        memcpy(psAdcValue, &sAdcValues, sizeof(tsAdcValues));
        bLocalCopy = bNewAdcValues;
        bNewAdcValues = false;
    }

    return bLocalCopy;
}