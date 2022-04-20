//********************************************************************************
/*!
\author     Kraemer E.
\date       05.02.2019

\file       WifiServer.c
\brief      Wifi Server functions

***********************************************************************************/
#include <WiFiServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include "WebSocketsServer.h"
#include <WifiServerLocal.h>

#include "Aom.h"
#include "MessageHandler.h"
#include "OS_EventManager.h"
#include "WifiServerLocal_AutomaticPage.h"
#include "WifiServerLocal_InitPage.h"
#include "WifiServerLocal_RootPage.h"
#include "WifiServerLocal_NTP.h"
//#include "WifiServerLocal_Chart.h"

#define ENABLE_SERIAL_DEBUG         false

//Set to true when the regulation isn't required
#define USE_IOT_WITHOUT_REGULATION  false

//SSID and password of your WIFI-Router
const char* ssid0 = "FRITZ!Box 6340 Cable";
const char* password0 = "55163310210586764215";
int RSSI0 = 0;
const char* ssid1 = "DotHomeGK";
const char* password1 = "Jo25082017Ed";
int RSSI1 = 0;

const char* szMDNS = "esp0";

//Create server with default HTTP port
ESP8266WebServer server(80);

//Create websocket at port 81
WebSocketsServer webSocket = WebSocketsServer(81);

/* Variable to hold the time */
//String in formatted time hh:mm:ss
static String ActualTime;

static tCStateDefinition* psWebPage = nullptr; 
static tsOutputValues* psOutputValues = nullptr;
static tsAutomaticValues* psAutomaticVal = nullptr;

static tePageSelect eCurrentPage = ePage_Root;

//********************************************************************************
/*!
\author     Kraemer E
\date       04.04.2020
\fn         IsCurrentTimeInActiveTimeSlot
\brief      Just a linker function to satisfy the structure
***********************************************************************************/
void PageHandler(void)
{
    psWebPage->pFctPageHandler();
}

//********************************************************************************
/*!
\author     Kraemer E
\date       04.04.2020
\fn         ButtonHandler
\brief      Just a linker function to satisfy the structure
***********************************************************************************/
void ButtonHandler(void)
{
    psWebPage->pFctButtonHandler();
}

//********************************************************************************
/*!
\author     Kraemer E
\date       04.04.2020
\fn         SliderHandler
\brief      Just a linker function to satisfy the structure
***********************************************************************************/
void SliderHandler(void)
{
    psWebPage->pFctSliderHandler();
}

//********************************************************************************
/*!
\author     Kraemer E
\date       04.04.2020
\fn         SliderHandler
\brief      Just a linker function to satisfy the structure
***********************************************************************************/
void WebSocketHandler(u8 ucNum, WStype_t eWsType, u8* pucPayload, size_t length)
{
    psWebPage->pFctWebSocketHandler(ucNum, eWsType, pucPayload, length);
}


//********************************************************************************
/*!
\author     Kraemer E
\date       04.04.2020
\fn         ConnectToNetwork
\brief      Function to connect with a network
***********************************************************************************/
void ConnectToNetwork(void)
{
    u8 ucConnectingTimeout = 60;
    bool bSSID0_Used = false;
    bool bSSID1_Used = false;

    //Scan network for available access
    u8 ucFoundNetworks = WiFi.scanNetworks();

    //Find the RSSI of the pre-set networks
    for(u8 ucNetworkIdx = 0; ucNetworkIdx < ucFoundNetworks; ucNetworkIdx++)
    {
        if(WiFi.SSID(ucNetworkIdx) == ssid0)
        {
            RSSI0 = WiFi.RSSI(ucNetworkIdx);
        }
        else if(WiFi.SSID(ucNetworkIdx) == ssid1)
        {
            RSSI1 = WiFi.RSSI(ucNetworkIdx);
        }
    }

    //Choose the connection with the higher RSSI
    if(RSSI0 > RSSI1)
    {
        //Start WiFi-Connection
        WiFi.begin(ssid0, password0);
        bSSID0_Used = true;
    }
    else
    {
        //Start WiFi-Connection
        WiFi.begin(ssid1, password1);
        bSSID1_Used = true;
    }    

    //Wait for the connection
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        
        if(bSSID0_Used == false || bSSID1_Used == false)
        {
            if(ucConnectingTimeout == 0)
            {
                WiFi.disconnect(false);

                if(bSSID0_Used)
                {
                    WiFi.begin(ssid1, password1);
                    bSSID1_Used = true;
                }
                else
                {
                    WiFi.begin(ssid0, password0);
                    bSSID0_Used = true;
                }
            }
            else
            {
                ucConnectingTimeout--;
            }
        }
    }
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
void WifiServerLocal_AutoInitDone(void)
{ 
    WifiServerLocal_InitPage_AutoInitDone();
    psWebPage->pFctPageHandler();
}


//********************************************************************************
/*!
\author     Kraemer E
\date       27.01.2019
\fn         WifiServer_Init
\brief      Initializes the the Web-page functionalities.
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_Init(void)
{    
    /* Init pointer to the structure from AOM */
    psOutputValues = Aom_OutputValuePointer();
    psAutomaticVal = Aom_AutomaticPointer();

    /* Init function pointer to the root page */
    WifiServerLocal_PageRequest(ePage_Root);

    /* Disable flash writing access every time the "wifi.begin" command is set */
    WiFi.persistent(ON);
    //Disable AP-Mode
    WiFi.enableAP(false);

    //Settings of the WIFI
    #if 0
    WiFi.hostname("Linas Lampe");
    IPAddress ip(192, 168, 178, 40);
    IPAddress dns(192, 168, 178, 1);
    IPAddress gateway(192, 168, 178, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet, dns);
    #else
    WiFi.hostname("Julius-Lampe");
    IPAddress ucLocIP = WiFi.localIP();
    IPAddress ip(ucLocIP[0], ucLocIP[1], ucLocIP[2], 41);
    WiFi.config(ip, WiFi.gatewayIP(), WiFi.subnetMask(), WiFi.dnsIP());
    #endif

    //Use WIFI-Manager instead!
    //ConnectToNetwork();

    //Create callback for root location
    server.on("/", PageHandler);

    //Create callbacks for sub-routines as per <a href="ledOn"> set.
    server.on("/form", ButtonHandler);

    //Create callback for slider
    server.on("/slider", SliderHandler);

    //Create callback for websocket events
    webSocket.begin();
    webSocket.onEvent(WebSocketHandler);

    //Start server
    server.begin();

    WifiServerLocal_NTP_Init();

    /* Initialize the pages */
    WifiServerLocal_AutoPage_Init(&server, &webSocket);
    WifiServerLocal_InitPage_Init(&server, &webSocket);
    WifiServerLocal_RootPage_Init(&server, &webSocket);
    //WifiServerLocal_ChartPage_Init(&server, &webSocket);
}


//********************************************************************************
/*!
\author     Kraemer E
\date       27.01.2019
\fn         WifiServer_Handler
\brief      Handler function for the Web-page requests.
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_Handler(void)
{ 
    //Handle client requests    
    webSocket.loop(); 
    server.handleClient();

    for(u8 ucOutputIdx = 0; ucOutputIdx < DRIVE_OUTPUTS; ucOutputIdx++)
    {
        bool bRootPageActive = (eCurrentPage == ePage_Root) ? true : false;

        /* Check for changes in the page handling and save values in separate structure in AOM */
        bool bValueChanged = Aom_SetMotionDetectionModeValue(psAutomaticVal->bMotionDetectOnOff, psAutomaticVal->ucEndurance);

        //Check if the night mode status has changed
        if(Aom_SetNightModeValue(psAutomaticVal->bNightModeOnOff))
        {
            MessageHandler_SendNightModeStatus();
        }

        //Check if the automatic mode has been enabled or disabled
        if(Aom_SetAutomaticModeValue(psAutomaticVal->bAutomaticOnOff))
        {
            MessageHandler_SendAutomaticModeStatus();
        }

        u8 ucValueChanged  = Aom_SetCustomValue(psOutputValues->sBrightnessValue[ucOutputIdx].ucSliderValue,
                                                ucOutputIdx,
                                                psOutputValues->sBrightnessValue[ucOutputIdx].bOnOff,
                                                bRootPageActive);

        if(bValueChanged || ucValueChanged != 0xFF)
        {                   
            /* When the system is in standby mode. Send a wake-up message to UIM*/
            //if(Aom_IsStandbyActive())
            //{
                //MessageHandler_SendSleepOrWakeUpMessage(false);
            //    OS_EVT_PostEvent(eEvtStandby, eEvtParam_ExitStandby, 0);
            //}

            
            /* Send message with new values to system master */
            MessageHandler_SendRequestOutputStatus(ucValueChanged);
        }
    }
}

//********************************************************************************
/*!
\author     Kraemer E
\date       26.11.2019
\fn         WifiServer_UpdateTimeClient
\brief      Updates the time client. 
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_UpdateTimeClient(void)
{
    WifiServerLocal_NTP_UpdateTimeClient();

    if(eCurrentPage == ePage_Root)
    {
        WifiServerLocal_RootPage_SendCurrentClock();
    }
    else if(eCurrentPage == ePage_Initialization)
    {
        WifiServerLocal_InitPage_SendWebsocketValue();
    }
}


//********************************************************************************
/*!
\author     Kraemer E
\date       26.11.2019
\brief      Sends new websocket values to the clients
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_UpdateWebSocketValues(void)
{
    if(eCurrentPage == ePage_Root)
    {
        WifiServerLocal_RootPage_SendWebsocketValue();        
    }
    //else if(eCurrentPage == ePage_Chart)
    //{
    //    WifiServerLocal_ChartPage_SendWebsocketValue();
    //}
}


//********************************************************************************
/*!
\author     Kraemer E
\date       08.12.2019
\fn         WifiServer_SetSavedUserTimerValues
\brief      Updates the user timer values with the pre-saved values from the master
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_SetSavedUserTimerValues(u8 ucStartH, u8 ucStopH, u8 ucStartM, u8 ucStopM, u8 ucTimerIdx)
{   
    if(ucTimerIdx < USER_TIMER_AMOUNT)
    {
        psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].ucTime = ucStartH;
        psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].ucTime = ucStartM;
        psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].ucTime = ucStopH;
        psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].ucTime = ucStopM;
        
        String st = String(ucStartH);
        st.toCharArray(&psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime[0], sizeof(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime));

        st = String(ucStartM);
        st.toCharArray(&psAutomaticVal->sStartMin[psAutomaticVal->ucTimerIndex].sTime[0], sizeof(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime));

        st = String(ucStopH);
        st.toCharArray(&psAutomaticVal->sStopHour[psAutomaticVal->ucTimerIndex].sTime[0], sizeof(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime));

        st = String(ucStopM);
        st.toCharArray(&psAutomaticVal->sStopMin[psAutomaticVal->ucTimerIndex].sTime[0], sizeof(psAutomaticVal->sStartHour[psAutomaticVal->ucTimerIndex].sTime)); 
    
        psAutomaticVal->ucTimerAmountSet |= (0x01 << ucTimerIdx);
    }
}


//********************************************************************************
/*!
\author     Kraemer E
\date       02.04.2020
\fn         WifiServerLocal_PageRequest
\brief      Function to change the web page according to the request.
\return     none
\param      none
***********************************************************************************/
void WifiServerLocal_PageRequest(tePageSelect ePageSelect)
{
    switch (ePageSelect)
    {
        case ePage_Initialization:
        {
            psWebPage = WifiServerLocal_InitPage_Linker();
            break;
        }

        case ePage_Automatic:
        {
            psWebPage = WifiServerLocal_AutoPage_Linker();
            break;
        }

        //case ePage_Chart:
        //{
        //    psWebPage = WifiServerLocal_ChartPage_Linker();
        //    break;
        //}
    
        case ePage_Root:
        default:
        {
            psWebPage = WifiServerLocal_RootPage_Linker();
            break;
        }
    }

    eCurrentPage = ePageSelect;
}