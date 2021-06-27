//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/
#ifdef USE_CHARTS
#include <WifiServerLocal.h>
#include <WiFiServer.h>
#include "ChartPage.h"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "Aom.h"
#include "MessageHandler.h"
#include "EventManager.h"
#include "Standby.h"
#include "WifiServerLocal_Chart.h"

/***************************** defines / macros ******************************/
#define ENABLE_SERIAL_DEBUG         false

//Set to true when the regulation isn't required
#define USE_IOT_WITHOUT_REGULATION  false


/************************ local data type definitions ************************/
/************************* local function prototypes *************************/
/************************* local data (const and var) ************************/
static tCStateDefinition sChartPageState;

static String szValues = ""; //Contains all values separated


/****************************** local functions ******************************/
//********************************************************************************
/*!
\author     Kraemer E
\date       27.01.2019
\fn         PageHandler
\brief      Callback for the chart page of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void PageHandler(void)
{
    //Read saved HTML content
    String sHtmlPage = CHART_PAGE;

    //Send web page
    sChartPageState.psWebServer->send(200, "text/html", sHtmlPage);
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
    //No slider used
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
    String sButtonName = sChartPageState.psWebServer->arg("submit");

    /* Check for sub page enter */
    if(sButtonName == "Home")
    {        
        WifiServerLocal_PageRequest(ePage_Root);
        //return;
    }

    sChartPageState.psWebServer->sendHeader("Location", "/");
    sChartPageState.psWebServer->send(302, "text/plain", "Updated-- Press Back Button");
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
    switch (eWsType)
    {
        case WStype_TEXT:
        {
            //Nothing to do
            break;
        }

        case WStype_DISCONNECTED:
        {
            //TODO: Web page disconnected
            break;
        }

        case WStype_CONNECTED:
        {
            /* When page is freshly connected, send values to web page */
            sChartPageState.psWebSocketServer->sendTXT(ucNum, szValues);
            break;
        }
        
        default:
            break;
    }
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
void WifiServerLocal_ChartPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket)
{    
    /* Initialize state handler */
    sChartPageState.pFctButtonHandler = ButtonHandler;
    sChartPageState.pFctSliderHandler = SliderHandler;
    sChartPageState.pFctPageHandler = PageHandler;
    sChartPageState.pFctWebSocketHandler = WebSocketHandler;
    sChartPageState.psWebServer = psWebServer;
    sChartPageState.psWebSocketServer = psWebSocket;
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         WifiServerLocal_ChartPage_Linker
\brief      Function to get the page state structure
\return     tCStateDefinition* - Pointer to the page state structure
\param      none
***********************************************************************************/
tCStateDefinition* WifiServerLocal_ChartPage_Linker(void)
{
    return &sChartPageState;
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         WifiServerLocal_RootPage_SendWebsocketValue
\brief      Function to get the values and create a websocket message for the web-page
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_ChartPage_SendWebsocketValue(void)
{
    tsAdcValues sAdcValues;
    if(Aom_GetAdcValues(&sAdcValues))
    {
        szValues = "V" + String(sAdcValues.uiVoltageAdc) + "v";
        szValues += "C" + String(sAdcValues.uiCurrentVal) + "c";
        szValues += "R" + String(sAdcValues.uiRegulationVal) + "r";

        sChartPageState.psWebSocketServer->broadcastTXT(szValues);
    }
}

#endif //USE_CHARTS