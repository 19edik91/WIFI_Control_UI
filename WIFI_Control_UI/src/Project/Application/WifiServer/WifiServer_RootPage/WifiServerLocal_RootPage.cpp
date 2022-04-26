//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/
#include <WifiServerLocal.h>
#include <WiFiServer.h>
#include "RootPage.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "Aom.h"
#include "MessageHandler.h"
#include "OS_EventManager.h"
#include "WifiServerLocal_RootPage.h"

/***************************** defines / macros ******************************/
#define ENABLE_SERIAL_DEBUG         false

//Set to true when the regulation isn't required
#define USE_IOT_WITHOUT_REGULATION  false


/************************ local data type definitions ************************/
/************************* local function prototypes *************************/
/************************* local data (const and var) ************************/
static tCStateDefinition sRootPageState;

//Brightness variables to hold the min max values
//Values are used for min max scaling
static tsOutputValues* psOutputValues = nullptr;

static String sValues = "";
static String sBurningTime = "";
static String szOutputValues = "";  //String for updating the output values by websockets
/****************************** local functions ******************************/
//********************************************************************************
/*!
\author     Kraemer E
\date       27.01.2019
\fn         RootHandler
\brief      Callback for the root page of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void PageHandler(void)
{
    //Read saved HTML content
    #if USE_IOT_WITHOUT_REGULATION
        //TODO: New page
    #else
        String sHtmlPage = ROOT_PAGE;
    #endif    

    for(u8 ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        String szLedStatus = (psOutputValues->sBrightnessValue[ucOutputIdx].bOnOff == true) ? "ON" : "OFF";
        String szButtonIdx = "@@L" + String(ucOutputIdx + 1) + "@@";
        String szSliderIdx = "@@BR" + String(ucOutputIdx + 1) + "@@";
        String szSliderVal = "@@SLIDERVAL" + String(ucOutputIdx + 1) + "@@";

        //Replace variables from the HTML data with real values
        sHtmlPage.replace(szButtonIdx, szLedStatus);

        #if USE_IOT_WITHOUT_REGULATION
        #else
        //Change slider position and description
        sHtmlPage.replace(szSliderIdx, String(psOutputValues->sBrightnessValue[ucOutputIdx].ucSliderValue));
        sHtmlPage.replace(szSliderVal, String(psOutputValues->sBrightnessValue[ucOutputIdx].ucSliderValue));   
        #endif
    }
    //Send web page
    sRootPageState.psWebServer->send(200, "text/html", sHtmlPage);
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
    //Empty used Websockets instead
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
    String sButtonName = sRootPageState.psWebServer->arg("submit");

    /* Check for sub page enter */
    if(sButtonName == "InitMenu")
    {        
        WifiServerLocal_PageRequest(ePage_Initialization);
        //return;
    }
    else if(sButtonName == "AutoMenu")
    {        
        WifiServerLocal_PageRequest(ePage_Automatic);
        //return;
    }
    else if(sButtonName == "ChartMenu")
    {
        WifiServerLocal_PageRequest(ePage_Chart);
    }

    sRootPageState.psWebServer->sendHeader("Location", "/");
    sRootPageState.psWebServer->send(302, "text/plain", "Updated-- Press Back Button");
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
    switch (eWsType)
    {
        case WStype_TEXT:
        {
            String sWsText = String((char*)&pucPayload[0]);

            //Brightness was changed with a slider
            if(sWsText.startsWith("B"))
            {
                String szSliderValue = sWsText.substring(sWsText.indexOf("B")+1, sWsText.indexOf("b"));
                String szSliderIndex = sWsText.substring(sWsText.indexOf("I")+1, sWsText.indexOf("i"));

                /* Set new brightness value */
                psOutputValues->sBrightnessValue[(szSliderIndex.toInt()-1)].ucSliderValue =  szSliderValue.toInt();

                /* Broadcast new slider value to all clients */
                sRootPageState.psWebSocketServer->broadcastTXT(sWsText);
            }
            //LED button has been clicked
            else if(sWsText.startsWith("L"))
            {
                String szButtonIndex = sWsText.substring(sWsText.indexOf("L")+1, sWsText.indexOf("l"));
                String szButtonState = sWsText.substring(sWsText.indexOf("S")+1, sWsText.indexOf("s"));

                /* Set new button state */
                psOutputValues->sBrightnessValue[(szButtonIndex.toInt()-1)].bOnOff = szButtonState.toInt();

                /* Broadcast new button state to all clients */
                sRootPageState.psWebSocketServer->broadcastTXT(sWsText);
            }
            break;
        }

        case WStype_DISCONNECTED:
        {
            //TODO: Web page disconnected
            
            /* Disconnect client */
            sRootPageState.psWebSocketServer->disconnect(ucNum);
            break;
        }

        case WStype_CONNECTED:
        {
            /* When page is freshly connected, send values to web page */
            sRootPageState.psWebSocketServer->sendTXT(ucNum, sValues);
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
void WifiServerLocal_RootPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket)
{    
    /* Initialize state handler */
    sRootPageState.pFctButtonHandler = ButtonHandler;
    sRootPageState.pFctSliderHandler = SliderHandler;
    sRootPageState.pFctPageHandler = PageHandler;
    sRootPageState.pFctWebSocketHandler = WebSocketHandler;
    sRootPageState.psWebServer = psWebServer;
    sRootPageState.psWebSocketServer = psWebSocket;

    /* Initialize pointer to the Brightness structure */
    psOutputValues = Aom_OutputValuePointer();
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         WifiServerLocal_RootPage_Linker
\brief      Function to get the page state structure
\return     tCStateDefinition* - Pointer to the page state structure
\param      none
***********************************************************************************/
tCStateDefinition* WifiServerLocal_RootPage_Linker(void)
{
    return &sRootPageState;
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
void WifiServerLocal_RootPage_SendWebsocketValue(void)
{
    s16 siNtcTemp = 0; 
    Aom_GetMeasureValues(0xFF, NULL, NULL, &siNtcTemp, NULL); 

    sValues += "K" + String(siNtcTemp) + "k";
    sRootPageState.psWebSocketServer->broadcastTXT(sValues);

    /* Show the remaining and initial burning time on web page */
    s32 slRemain = 0;
    s32 slInit = 0;
    Aom_GetBurningTimeValues(&slRemain, &slInit);

    sBurningTime = "T" + String(slRemain) + "t";
    sBurningTime += "O" + String(slInit) + "o";
    sRootPageState.psWebSocketServer->broadcastTXT(sBurningTime);

    /* Checkout a change of the buttons and slider from another UI or the master */
    for(u8 ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        szOutputValues = "B" + String(psOutputValues->sBrightnessValue[ucOutputIdx].ucSliderValue) + "b";
        szOutputValues += "I" + String(ucOutputIdx + 1) + "i";
        sRootPageState.psWebSocketServer->broadcastTXT(szOutputValues);         
    }
}

//********************************************************************************
/*!
\author     Kraemer E
\date       24.09.2020
\fn         WifiServerLocal_RootPage_SendCurrentClock
\brief      Function to get current clock values and send them as a websocket message
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_RootPage_SendCurrentClock(void)
{
    u8 ucHours = 0;
    u8 ucMinutes = 0;
    String sClock = "";

    Aom_GetActualTime(&ucHours, &ucMinutes, NULL);

    if(ucHours < 10)
    {
        sClock = "H" + String(0) + String(ucHours) + "h";
    }
    else
    {
        sClock = "H" + String(ucHours) + "h";
    }

    if(ucMinutes < 10)
    {
        sClock += "M" + String(0) + String(ucMinutes) + "m"; 
    }
    else
    {
        sClock += "M" + String(ucMinutes) + "m"; 
    }

    sRootPageState.psWebSocketServer->broadcastTXT(sClock);
}
