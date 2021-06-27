//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/
#include <WifiServerLocal.h>
#include <WiFiServer.h>
#include "InitializationPage.h"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "Aom.h"
#include "MessageHandler.h"
#include "OS_EventManager.h"
#include "WifiServerLocal_InitPage.h"

/***************************** defines / macros ******************************/
#define ENABLE_SERIAL_DEBUG         false

//Set to true when the regulation isn't required
#define USE_IOT_WITHOUT_REGULATION  false

/************************ local data type definitions ************************/
/************************* local function prototypes *************************/
/************************* local data (const and var) ************************/
static tCStateDefinition sInitPageState;

/* Variables for the process status */
static bool bManualInitProgress = OFF;
static bool bAutoInitProgress = OFF;
static bool bInitProcessStatus = OFF;

static tsOutputValues* psOutputVal = nullptr;
static u8 ucInitOutputIndex = 0;
/****************************** local functions ******************************/
//********************************************************************************
/*!
\author     Kraemer E
\date       03.11.2019
\fn         SubHandler
\brief      Callback for the sub page of the Web-page.
\return     none
\param      none
***********************************************************************************/
static void PageHandler(void)
{
    u32 ulVoltage;
    u16 uiCurrent;
    s16 siTemp;
    Aom_GetMeasureValues(ucInitOutputIndex, &ulVoltage, &uiCurrent, &siTemp, NULL);

    //Read saved HTML content
    String sHtmlPage = INIT_PAGE;      

    sHtmlPage.replace("@@OutputIdx@@", String(ucInitOutputIndex));
    sHtmlPage.replace("@@OutputCnt@@", String(DRIVE_OUTPUTS));

    sHtmlPage.replace("@@T1@@", String(siTemp));
    sHtmlPage.replace("@@V@@", String(ulVoltage));
    sHtmlPage.replace("@@A@@", String(uiCurrent));
    
    //Replace variables from the HTML data with real values
    sHtmlPage.replace("@@MinBR@@", String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMinValue));
    sHtmlPage.replace("@@MaxBR@@", String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMaxValue));
    
    //Change slider position and description
    sHtmlPage.replace("@@BR@@", String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue));
    sHtmlPage.replace("@@SLIDERVAL@@", String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue));

    //Change progress text while in initialization mode
    sHtmlPage.replace("@@AISTATE@@", (bAutoInitProgress ? "ON" : "OFF"));
    sHtmlPage.replace("@@MISTATE@@", (bManualInitProgress ? "ON" : "OFF"));
    
    //Send sub web page
    sInitPageState.psWebServer->send(200, "text/html", sHtmlPage);
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
    //Nothing to do here
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
    String sButtonName = sInitPageState.psWebServer->arg("submit");

    if(sButtonName == "HomeBtn" || sButtonName == "HomeBtnAuto")
    {
        WifiServerLocal_PageRequest(ePage_Root);
    }
    
    sInitPageState.psWebServer->sendHeader("Location", "/");
    sInitPageState.psWebServer->send(302, "text/plain", "Updated-- Press Back Button");
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

                /* Set new brightness value */
                psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue =  szSliderValue.toInt();

                /* Broadcast new slider value to all clients */
                sInitPageState.psWebSocketServer->broadcastTXT(sWsText);
            }
            //Button has been clicked
            else if(sWsText.startsWith("L"))
            {
                String szButton = sWsText.substring(sWsText.indexOf("L")+1, sWsText.indexOf("l"));

                if(szButton == "AION")
                {
                    if(bInitProcessStatus == OFF)
                    {
                        bInitProcessStatus = ON;  
                        bAutoInitProgress = ON;
                        
                        /* Post event */
                        OS_EVT_PostEvent(eEvtAutoInitHardware, eEvtParam_AutoInitStart, 0);        
                    }
                }
                else if(szButton == "AIOFF")
                {
                    //TODO: Check for OFF state
                }
                else if(szButton == "MION")
                {
                    /* Enable manual initialization only when nothing is in progress */
                    if(bInitProcessStatus == OFF && bManualInitProgress == OFF)
                    {
                        bInitProcessStatus = ON;  
                        bManualInitProgress = ON;
                    }
                }
                else if(szButton == "MIOFF")
                {
                    /* Disable manual initialization only when initialization is in progress */
                    if(bInitProcessStatus == ON && bManualInitProgress == ON)
                    {
                        bInitProcessStatus = OFF;
                        bManualInitProgress = OFF;

                        /* Post event */
                        OS_EVT_PostEvent(eEvtManualInitHardware, eEvtParam_ManualInitDone, 0);             
                    }
                }
                else if(szButton == "Prev")
                {
                    /* Decrement index */
                    if(ucInitOutputIndex)
                    {
                        --ucInitOutputIndex;
                    }
                }
                else if(szButton == "Next")
                {
                    /* Increment index */
                    if(ucInitOutputIndex < (DRIVE_OUTPUTS-1))
                    {
                        ++ucInitOutputIndex;
                    }
                }
                else if(szButton == "MinSet")
                {
                    /* Min-Value can only be set when manual init is in progress */
                    if(bInitProcessStatus == ON && bManualInitProgress == ON)
                    {
                        /* Validate correct min values. New value has to be smaller than the set max values */
                        if(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue < psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMaxValue)
                        {
                            //Replace the min value with the slider value
                            psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMinValue = psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue;
                            
                            /* Post event */
                            OS_EVT_PostEvent(eEvtManualInitHardware, eEvtParam_ManualInitSetMinValue, ucInitOutputIndex); 
                        }
                    }
                }
                else if(szButton == "MaxSet")
                {
                    /* Max-Value can only be set when manual init is in progress */
                    if(bInitProcessStatus == ON && bManualInitProgress == ON)
                    {
                        /* Validate correct max values. New value has to be greater than the set min values */
                        if(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue > psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMinValue)
                        {
                            //Replace the min value with the slider value
                            psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMaxValue = psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue; 

                            /* Post event */
                            OS_EVT_PostEvent(eEvtManualInitHardware, eEvtParam_ManualInitSetMaxValue, ucInitOutputIndex); 
                        }
                    }
                }

                /* Broadcast new button state to all clients */
                //sRootPageState.psWebSocketServer->broadcastTXT(sWsText);
                WifiServerLocal_InitPage_SendWebsocketValue();
            }
            break;
        }

        case WStype_DISCONNECTED:
        {
            //TODO: Web page disconnected
            
            /* Disconnect client */
            sInitPageState.psWebSocketServer->disconnect(ucNum);
            break;
        }

        case WStype_CONNECTED:
        {
            /* When page is freshly connected, send values to web page */
            //sInitPageState.psWebSocketServer->sendTXT(ucNum, sValues);
            WifiServerLocal_InitPage_SendWebsocketValue();
            break;
        }
        
        default:
            break;
    }
}

//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\brief      Function to send new values to the websocket client
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_InitPage_SendWebsocketValue(void)
{
    String szValues = "";

    //Send output index and brightness first
    szValues += "O" + String(ucInitOutputIndex + 1) + "o";  //Output index
    szValues += "N" + String(DRIVE_OUTPUTS) + "n";      //Output count
    szValues += "U" + String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMaxValue) + "u";    //Max value as "upper limit"
    szValues += "L" + String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucMinValue) + "l";    //Min value as "lower limit"
    szValues += "B" + String(psOutputVal->sBrightnessValue[ucInitOutputIndex].ucSliderValue) + "b"; //Brightness value

    //Set automatic-initialazing status
    szValues += "A"; 
    szValues += (bAutoInitProgress == ON ? "ON" : "OFF");
    szValues += "a";

    //Set manual-initialazing status
    szValues += "M"; 
    szValues += (bManualInitProgress == ON ? "ON" : "OFF");
    szValues += "m";

    //Send first message
    sInitPageState.psWebSocketServer->broadcastTXT(szValues);


    //Clear szValues
    szValues = "";

    u32 ulVoltage;
    u16 uiCurrent;
    s16 siTemp;
    Aom_GetMeasureValues(ucInitOutputIndex, &ulVoltage, &uiCurrent, &siTemp, NULL);

    szValues += "V" + String(ulVoltage) + "v";  //Voltage value
    szValues += "C" + String(uiCurrent) + "c";  //Current value
    szValues += "K" + String(siTemp) + "k";     //Temperature value

    sInitPageState.psWebSocketServer->broadcastTXT(szValues);
}


//********************************************************************************
/*!
\author     Kraemer E
\date       08.11.2019
\fn         WifiServer_AutoInitDone
\brief      When the auto initialization has finished on the master this function
            is called to stop the progress
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_InitPage_AutoInitDone(void)
{ 
    //Change process status only to off when it's truly in Auto init state
    if(bInitProcessStatus == ON && bAutoInitProgress == ON)
    {
        bInitProcessStatus = OFF;  
        bAutoInitProgress = OFF;
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
void WifiServerLocal_InitPage_Init(ESP8266WebServer* psWebServer, WebSocketsServer* psWebSocket)
{    
    /* Initialize state handler */
    sInitPageState.pFctButtonHandler = ButtonHandler;
    sInitPageState.pFctSliderHandler = SliderHandler;
    sInitPageState.pFctPageHandler = PageHandler;
    sInitPageState.pFctWebSocketHandler = WebSocketHandler;
    sInitPageState.psWebServer = psWebServer;
    sInitPageState.psWebSocketServer = psWebSocket;
    
    /* Initialize pointer to the Brightness structure */
    psOutputVal = Aom_OutputValuePointer();
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         void WifiServerLocal_InitPage_Linker(tCStateDefinition* psPageState)
\brief      Function to get the page state structure
\return     tCStateDefinition* - Pointer to the page state structure
\param      none
***********************************************************************************/
tCStateDefinition* WifiServerLocal_InitPage_Linker(void)
{
    return &sInitPageState;
}