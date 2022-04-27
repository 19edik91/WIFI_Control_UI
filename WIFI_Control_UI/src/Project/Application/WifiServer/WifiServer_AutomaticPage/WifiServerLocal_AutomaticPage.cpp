//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/
#include <WifiServerLocal.h>
#include <WiFiServer.h>
#include "AutomaticPage.h"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "Aom.h"
#include "MessageHandler.h"
#include "OS_EventManager.h"

/***************************** defines / macros ******************************/
#define ENABLE_SERIAL_DEBUG         false

//Set to true when the regulation isn't required
#define USE_IOT_WITHOUT_REGULATION  false

/************************ local data type definitions ************************/
/************************* local function prototypes *************************/
/************************* local data (const and var) ************************/
static tCStateDefinition sAutoPageState;
static tsAutomaticValues* psAutomaticVal = nullptr;


/****************************** local functions ******************************/
//********************************************************************************
/*!
\author     Kraemer E
\date       01.12.2019
\fn         GetValueFromString
\brief      Returns the time value as a byte back
\return     ucVal - Return value as a byte
\param      s - String which should be checked.
***********************************************************************************/
static u8 GetValueFromString(String s)
{
    bool bZeroFound = false;
    u8 ucVal = 0xFF;
    
    if(s[0] == '0')
    {
        bZeroFound = true;      
    }

    if(bZeroFound)
    {
        s = s.substring(1,2);
        ucVal = s.toInt();
    }
    else
    {
        ucVal = s.toInt();
    }

    return ucVal;
}



//********************************************************************************
/*!
\author     Kraemer E
\date       01.12.2019
\fn         AutomaticHandler
\brief      Callback for the automatic menu page of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void PageHandler(void)
{
    //Read saved HTML content
    String sHtmlPage = AUTOMATIC_page;    

    //Replace variables from the HTML data with real values
    String sNightMode = (psAutomaticVal->bNightModeOnOff == true) ? "ON" : "OFF";
    sHtmlPage.replace("@@A1@@", sNightMode);

    String sMotionMode = (psAutomaticVal->bMotionDetectOnOff == true) ? "ON" : "OFF";
    sHtmlPage.replace("@@B1@@", sMotionMode);
       
    String sAutomaticMode = (psAutomaticVal->bAutomaticOnOff == true) ? "ON" : "OFF";
    sHtmlPage.replace("@@C1@@", sAutomaticMode);

    //Change slider position and description
    sHtmlPage.replace("@@L3@@", String(psAutomaticVal->ucEndurance));
    sHtmlPage.replace("@@SLIDERVAL@@", String(psAutomaticVal->ucEndurance));   

    //Replace values for start and stop time
    sHtmlPage.replace("ValStartTimeHours", psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime);
    sHtmlPage.replace("ValStartTimeMin", psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].sTime);
    sHtmlPage.replace("ValStopTimeHours", psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].sTime);
    sHtmlPage.replace("ValStopTimeMin", psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].sTime);
    sHtmlPage.replace("TimerIdx", String(psAutomaticVal->ucTimerIndex));

    //Send sub web page
    sAutoPageState.psWebServer->send(200, "text/html", sHtmlPage);
}

//********************************************************************************
/*!
\author     Kraemer E
\date       27.01.2019
\fn         SliderHandler
\brief      Callback for the Slider functionality of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void SliderHandler(void)
{
    String sSliderValue = sAutoPageState.psWebServer->arg("SliderValue");
    psAutomaticVal->ucEndurance = sSliderValue.toInt();
    PageHandler();
    return;
}

//********************************************************************************
/*!
\author     Kraemer E
\date       27.01.2019
\fn         ButtonHandler
\brief      Callback for the button functionality of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void ButtonHandler(void)
{ 
    //Get pushed button argument
    String sButtonName = sAutoPageState.psWebServer->arg("submit");

    if(sButtonName == "TimeSet")
    {
        sAutoPageState.psWebServer->arg("startTimeHours").toCharArray(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime, sizeof(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime));
        psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].ucTime = GetValueFromString(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime);

        sAutoPageState.psWebServer->arg("startTimeMinutes").toCharArray(psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].sTime, sizeof(psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].sTime));
        psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].ucTime = GetValueFromString(psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].sTime);
        
        sAutoPageState.psWebServer->arg("stopTimeHours").toCharArray(psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].sTime, sizeof(psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].sTime));
        psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].ucTime = GetValueFromString(psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].sTime);
        
        sAutoPageState.psWebServer->arg("stopTimeMinutes").toCharArray(psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].sTime, sizeof(psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].sTime));
        psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].ucTime = GetValueFromString(psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].sTime);

        /* Correct times */
        if(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].ucTime >= 24)
        {
            String st = "23";
            st.toCharArray(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime, st.length());
            psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].ucTime = 23;
        }

        if(psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].ucTime >= 24)
        {
            String st = "23";
            st.toCharArray(psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].sTime, st.length());
            psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].ucTime = 23;
        }

        if(psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].ucTime >= 60)
        {
            String st = "59";
            st.toCharArray(psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].sTime, st.length());
            psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].ucTime = 59;
        }

        if(psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].ucTime >= 60)
        {
            String st = "59";
            st.toCharArray(psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].sTime, st.length());
            psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].ucTime = 59;
        }

        //Send new values to the Master
        MessageHandler_SendNewUserTimerValues(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].ucTime,
                                                psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].ucTime,
                                                psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].ucTime,
                                                psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].ucTime,
                                                psAutomaticVal->ucTimerIndex);
        psAutomaticVal->ucTimerAmountSet |= (0x01 << psAutomaticVal->ucTimerIndex);
        
        PageHandler();
        return;
    }

    if(sButtonName == "nextBtn")
    {
            //Increment the timer index
        if(psAutomaticVal->ucTimerIndex < USER_TIMER_AMOUNT)
        {
            ++psAutomaticVal->ucTimerIndex;
        }
    
        //Reload automatic mode side
        PageHandler();
        return;
    }

    if(sButtonName == "previousBtn")
    {
            //Increment the timer index
        if(psAutomaticVal->ucTimerIndex > 0)
        {
            --psAutomaticVal->ucTimerIndex;
        }
    
        //Reload automatic mode side
        PageHandler();
        return;
    }

    if(sButtonName == "HomeBtn" || sButtonName == "HomeBtnAuto")
    {
        WifiServerLocal_PageRequest(ePage_Root);
    }  

    if(sButtonName == "NightON")
    {
        psAutomaticVal->bNightModeOnOff = true;
        PageHandler();
        return;
    }
    else if(sButtonName == "NightOFF")
    {
        psAutomaticVal->bNightModeOnOff = false;
        PageHandler();
        return;
    }

    if(sButtonName == "PirON")
    {
        psAutomaticVal->bMotionDetectOnOff = true;
        PageHandler();
        return;
    }
    else if(sButtonName == "PirOFF")
    {
        psAutomaticVal->bMotionDetectOnOff = false;
        PageHandler();
        return;
    }
    if(sButtonName == "AutoON")
    {
        psAutomaticVal->bAutomaticOnOff = true;
        PageHandler();
        return;
    }
    else if(sButtonName == "AutoOFF")
    {
        psAutomaticVal->bAutomaticOnOff = false;
        PageHandler();
        return;
    }

    sAutoPageState.psWebServer->sendHeader("Location", "/");
    sAutoPageState.psWebServer->send(302, "text/plain", "Updated-- Press Back Button");
    delay(500);   
}

//********************************************************************************
/*!
\author     Kraemer E
\date       20.09.2020
\fn         WebSocketHandler
\brief      Callback for the websocket events of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void WebSocketHandler(u8 ucNum, WStype_t eWsType, u8* pucPayload, size_t length)
{

}

//********************************************************************************
/*!
\author     Kraemer E
\date       31.03.2020
\fn         WifiServerLocal_RootPage_Init
\brief      Initializes the function pointer of this page
\return     none
\param      psWebServer - The Webserver which should be used for this page
***********************************************************************************/
void WifiServerLocal_AutoPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket)
{    
    /* Initialize state handler */
    sAutoPageState.pFctButtonHandler = ButtonHandler;
    sAutoPageState.pFctSliderHandler = SliderHandler;
    sAutoPageState.pFctPageHandler = PageHandler;
    sAutoPageState.pFctWebSocketHandler = WebSocketHandler;
    sAutoPageState.psWebServer = psWebServer;
    sAutoPageState.psWebSocketServer = psWebSocket;
    
    psAutomaticVal = Aom_AutomaticPointer();
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         void WifiServerLocal_AutoPage_Linker(tCStateDefinition* psPageState)
\brief      Function to get the page state structure
\return     tCStateDefinition* - Pointer to the page state structure
\param      none
***********************************************************************************/
tCStateDefinition* WifiServerLocal_AutoPage_Linker(void)
{
    return &sAutoPageState;
}