/* ========================================
 *
 * Copyright Eduard Kraemer, 2019
 *
 * ========================================
*/


#include "OS_Flash.h"
#include "OS_EventManager.h"

#include "DR_Measure.h"

#include "Aom_Flash.h"
/****************************************** Defines ******************************************************/
/****************************************** Variables ****************************************************/
/****************************************** Function prototypes ******************************************/
/****************************************** loacl functiones *********************************************/
/****************************************** External visible functiones **********************************/
  
//********************************************************************************
/*!
\author     Kraemer E.
\date       10.11.2019
\fn         Aom_WriteSystemSettingsInFlash
\brief      Save actual system settings in flash memory.
\return     none
\param      none
***********************************************************************************/
void Aom_Flash_WriteSystemSettingsInFlash(void)
{       
    /* Calculate the desired flash data size first */
    u8 ucFlashDataSize = sizeof(tsSystemSettings) * DRIVE_OUTPUTS;
    
    /* Initialize the flash data array */
    u8 ucFlashData[ucFlashDataSize];    
    memset(&ucFlashData[0], 0, ucFlashDataSize);
    
    /* Copy system settings into flash data array */
    u8 ucDataOffset = 0;
    u8 ucOutputIdx;
    for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        /* Get address of the entry */
        tsSystemSettings* psSystemSettings = Aom_GetSystemSettingsEntry(ucOutputIdx);        
        memcpy(&ucFlashData[ucDataOffset], psSystemSettings, sizeof(tsSystemSettings));
        
        ucDataOffset += sizeof(tsSystemSettings);        
    }
    
    /* When writing was successfull, change regulation values */
    if(OS_Flash_WriteSystemSettings(&ucFlashData[0], ucFlashDataSize))
    {
        u8 ucFlashCompareData[ucFlashDataSize];
        memset(&ucFlashCompareData[0], 0, ucFlashDataSize);
        
        OS_Flash_GetSystemSettings(&ucFlashCompareData[0], ucFlashDataSize);
        
        /* Check if flash access was successful. Memcmp returns 0 when both buffers contains the same data */
        if(memcmp(&ucFlashData[0], &ucFlashCompareData[0], ucFlashDataSize))
        {
            //Faulty wait for WDT
            while(1);
        }
        
        for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
        {
            /* Get address of the entry */
            tsSystemSettings* psSystemSettings = Aom_GetSystemSettingsEntry(ucOutputIdx);        
        
            /* Set new regulation limits to for voltage calculation */
            u32 ulMinVoltageLimit = DR_Measure_CalculateVoltageValue(psSystemSettings->uiMinAdcVoltage);
            u32 ulMaxVoltageLimit = DR_Measure_CalculateVoltageValue(psSystemSettings->uiMaxAdcVoltage);
            
            DR_Measure_SetNewVoltageLimits(ulMinVoltageLimit, ulMaxVoltageLimit, ucOutputIdx);
        }
    }
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\fn         Aom_ReadSystemSettingsFromFlash
\brief      Read the system settings from the flash memory.
\return     none
\param      none
***********************************************************************************/
u8 Aom_Flash_ReadSystemSettingsFromFlash(void)
{   
    u8 ucRead = 0;
    
    /* Calculate the desired flash data size first */
    u8 ucFlashDataSize = sizeof(tsSystemSettings) * DRIVE_OUTPUTS;
    
    /* Initialize the flash data array */
    u8 ucFlashData[ucFlashDataSize];    
    memset(&ucFlashData[0], 0, ucFlashDataSize);
    
    /* Read user settings and if there is nothing saved, wait for answer from ESP */
    if(OS_Flash_GetSystemSettings(&ucFlashData[0], ucFlashDataSize))
    {
        u8 ucOutputIdx;
        u8 ucDataOffset = 0;
        for(ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
        {
            /* Get address of the entry */
            tsSystemSettings* psSystemSettings = Aom_GetSystemSettingsEntry(ucOutputIdx);        
            memcpy(psSystemSettings, &ucFlashData[ucDataOffset], sizeof(tsSystemSettings));
            ucDataOffset += sizeof(tsSystemSettings);  
        
            if(psSystemSettings->uiMaxAdcVoltage || psSystemSettings->uiMaxAdcCurrent)
            {
                ucRead = 0x01 << ucOutputIdx;
                
                /* Set new regulation limits to for voltage calculation */
                u32 ulMinVoltageLimit = DR_Measure_CalculateVoltageValue(psSystemSettings->uiMinAdcVoltage);
                u32 ulMaxVoltageLimit = DR_Measure_CalculateVoltageValue(psSystemSettings->uiMaxAdcVoltage);
                DR_Measure_SetNewVoltageLimits(ulMinVoltageLimit, ulMaxVoltageLimit, ucOutputIdx);
            }
        }
    }
    
    return ucRead;
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       20.01.2019
\brief      Save actual user settings in flash memory. Function is called by a
            software timer
\return     none
\param      none
***********************************************************************************/
void Aom_Flash_WriteUserSettingsInFlash(void)
{    
    OS_Flash_WriteUserSettings(Aom_GetRegulationSettings(), sizeof(tRegulationValues));
}


//********************************************************************************
/*!
\author     Kraemer E.
\date       15.02.2020
\brief      Read actual user settings from flash memory.
\return     none
\param      none
***********************************************************************************/
void Aom_Flash_ReadUserSettingsFromFlash(void)
{   
    /* Read user settings and if there is nothing saved, wait for answer from ESP */
    if(OS_Flash_GetUserSettings(Aom_GetRegulationSettings(), sizeof(tRegulationValues)) == false)
    {       
        /* No config found -> Set default values */
        tRegulationValues* psRegulationVal = Aom_GetRegulationSettings();
        
        psRegulationVal->sUserTimerSettings.sTimer[0].ucHourSet = 6;
        psRegulationVal->sUserTimerSettings.sTimer[0].ucHourClear = 21;
        psRegulationVal->sUserTimerSettings.sTimer[0].ucMinSet = 0;
        psRegulationVal->sUserTimerSettings.sTimer[0].ucMinClear = 0;
        psRegulationVal->sUserTimerSettings.ucSetTimerBinary |= 0x01 << 0;
        
        /* Read last saved brightness value, in this case the LED should always be OFF */
        //Aom_SetCustomValue(sRegulationValues.sLedValue.ucPercentValue, false);
    }
}
