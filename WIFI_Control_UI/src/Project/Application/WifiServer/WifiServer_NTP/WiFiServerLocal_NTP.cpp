//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServerLocal_NTP.c
\brief      NTP-functions

***********************************************************************************/
#include <WiFiServer.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "Aom.h"
#include <time.h>
#include <TimeLib.h>
#include "Timezone.h"

//Set to true when the regulation isn't required
#define USE_IOT_WITHOUT_REGULATION  false
#define GTM_OFFSET  0   //0 to get UTC time

//Create UDP server structure
WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
//NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", GTM_OFFSET*60*60, 60000);

/* Change the time rule */
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; //Central european summer time
TimeChangeRule CET  = {"CET", Last, Sun, Oct, 3, 60};   //Central european standard time

/* Set new time zone */
Timezone CE(CEST, CET);

/* Variable to hold the time */
//String in formatted time hh:mm:ss
static String ActualTime;

//********************************************************************************
/*!
\author     Kraemer E
\date       26.09.2020
\fn         GetDateTimeStructure
\brief      Calculates readable time format from epoch time
\return     tm - Timestructure
\param      ulEpochTime - The epoch time is the tick in seconds since the UNIX-start
***********************************************************************************/
//static tm GetDateTimeStructure(u32_t ulEpochTime)
//{
//    /* Create new time structure */
//    struct tm* psTimeStr;
//    const time_t time = ulEpochTime;
//
//    /* Fill the time structure */
//    psTimeStr = localtime(&time);
//
//    return *psTimeStr;   
//}

//********************************************************************************
/*!
\author     Kraemer E
\date       26.09.2020
\fn         WifiServerLocal_NTP_Init
\brief      Initializes the the Web-page functionalities.
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_NTP_Init(void)
{    
    //Start with time client
    timeClient.begin();
}

//********************************************************************************
/*!
\author     Kraemer E
\date       26.09.2020
\fn         WifiServerLocal_NTP_UpdateTimeClient
\brief      Updates the time client. 
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_NTP_UpdateTimeClient(void)
{
    //Update time client
    timeClient.update();

    //Get epoch time
    u32_t epochTime = timeClient.getEpochTime();

    //Set new system time from epoch time
    setTime((time_t)epochTime);

    //Adjust UTC-epoch time into local time with the system time   
    time_t locTime = CE.toLocal(now());

    //Create time structure with the local time
    struct tm* psTimeStruct = localtime(&locTime);

    /* Create new structure for holding date values */
    tsDate sDate;
    sDate.ulEpochTick = epochTime;
    sDate.uiYear  = psTimeStruct->tm_year + 1900;    //Year begins at 1900
    sDate.ucMonth = psTimeStruct->tm_mon + 1;        //Months start at 0 (=Jan)
    sDate.ucDay   = psTimeStruct->tm_mday;
    sDate.ucHour  = psTimeStruct->tm_hour;
    sDate.ucMinute= psTimeStruct->tm_min;
    sDate.ucSecond= psTimeStruct->tm_sec;
    sDate.bDST    = (psTimeStruct->tm_isdst == 0) ? false : true;

    Aom_SetDate(&sDate);
}