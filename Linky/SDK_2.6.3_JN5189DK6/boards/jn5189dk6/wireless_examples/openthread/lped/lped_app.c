/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*!=================================================================================================
\file       lped_app.c
\brief      This is a public source file for the low power end device demo application.
==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
/* General Includes */
#include "EmbeddedTypes.h"
#include <string.h>

/* NXP Framework */
#include "Keyboard.h"
#include "RNG_Interface.h"
#include "TimersManager.h"
#include "FunctionLib.h"
#include "PWR_Interface.h"

/* Application */
#include "lped_app.h"
#include "app_init.h"
#include "app_led.h"
#include "network_utils.h"
#include "app_latency_measure.h"

#if gEnableOTAClient_d
#include "app_ota.h"
#endif

#include <openthread/thread.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/joiner.h>
#include <openthread/coap.h>

/*==================================================================================================
Private macros
==================================================================================================*/
#if APP_AUTOSTART
#define gAppFactoryResetTimeoutMin_c            10000
#define gAppFactoryResetTimeoutMax_c            20000
#endif

#define gAppJoinTimeout_c                       800    /* miliseconds */

#define APP_LED_URI_PATH                        "led"
#define APP_TEMP_URI_PATH                       "temp"
#define APP_SINK_URI_PATH                       "sink"
#if LARGE_NETWORK
#define APP_RESET_TO_FACTORY_URI_PATH           "reset"
#endif

#define APP_DEFAULT_DEST_ADDR                   in6addr_realmlocal_allthreadnodes

/*==================================================================================================
Private type definitions
==================================================================================================*/

/*==================================================================================================
Private global variables declarations
==================================================================================================*/
static bool_t mFirstPushButtonPressed = FALSE;
static bool_t mJoiningIsAppInitiated = FALSE;
bool_t bSleepAllowed = TRUE;
/*==================================================================================================
Private prototypes
==================================================================================================*/

static void APP_OtHandleScanResult(otActiveScanResult *aResult, void *aContext);
static void App_HandleKeyboard(void);
static void APP_InitCoapDemo(void);
static void APP_ReportTemp(void);

#if gKBD_KeysCount_c > 2
static bool_t APP_SendDataSinkCommand(uint8_t *pCommand, uint8_t dataLen);
static void APP_SendDataSinkCreate(void);
static void APP_SendDataSinkRelease(void);
#endif
static void APP_SendLedOn(void);
static void APP_SendLedOff(void);
static void APP_SendLedFlash(void);
static void APP_LocalDataSinkRelease(void);
static void APP_ProcessLedCmd(uint8_t *pCommand, uint16_t dataLen);
static void APP_CoapGenericCallback(void *pCtx, otMessage *pMsg, const otMessageInfo *pMsgInfo);
static void APP_CoapLedCb(void *pCtx, otMessage *pMsg, const otMessageInfo *pMsgInfo);
static void APP_CoapTempCb(void *pCtx, otMessage *pMsg, const otMessageInfo *pMsgInfo);
static void APP_CoapSinkCb(void *pCtx, otMessage *pMsg, const otMessageInfo *pMsgInfo);
#if LARGE_NETWORK
static void APP_CoapResetToFactoryDefaultsCb(void *pCtx, otMessage *pMsg, const otMessageInfo *pMsgInfo);
static void APP_SendResetToFactoryCommand(void);
#endif
#if APP_AUTOSTART
static void APP_AutoStart(void);
static void APP_AutoStartCb(void *param);
#endif

static void APP_OtNwkJoin();
/*==================================================================================================
Public global variables declarations
==================================================================================================*/
otCoapResource gAPP_LED_URI_PATH = {.mUriPath = APP_LED_URI_PATH, .mHandler = APP_CoapLedCb, .mContext = NULL, .mNext = NULL };
otCoapResource gAPP_TEMP_URI_PATH = {.mUriPath = APP_TEMP_URI_PATH, .mHandler = APP_CoapTempCb, .mContext = NULL, .mNext = NULL };
otCoapResource gAPP_SINK_URI_PATH = {.mUriPath = APP_SINK_URI_PATH, .mHandler = APP_CoapSinkCb, .mContext = NULL, .mNext = NULL };
#if LARGE_NETWORK
otCoapResource gAPP_RESET_URI_PATH = {.mUriPath = APP_RESET_TO_FACTORY_URI_PATH, .mHandler = APP_CoapResetToFactoryDefaultsCb, .mContext = NULL, .mNext = NULL };
#endif

/* Application state/mode */
appDeviceState_t gAppDeviceState;
appDeviceMode_t gAppDeviceMode;

/* Flag used to stop the attaching retries */
bool_t gbRetryInterrupt = TRUE;

/* Destination address for CoAP commands */
otIp6Address gCoapDestAddress = IN6ADDR_ANY_INIT;

/* Application timer Id */
tmrTimerID_t mAppTimerId = gTmrInvalidTimerID_c;

#if APP_AUTOSTART
tmrTimerID_t tmrStartApp = gTmrInvalidTimerID_c;
#endif

bool_t gNwkFound = FALSE;
bool_t gAttachFinished = FALSE;

/* Pointer to OpenThread Instance */
extern otInstance *gpOpenThreadInstance;

/* Event mask for button presses */
extern uint32_t eventMask;

/*==================================================================================================
Public functions
==================================================================================================*/
/*!*************************************************************************************************
\fn     void APP_Init(void)
\brief  This function is used to initialize application.
***************************************************************************************************/
void APP_Init
(
    void
)
{
    /* Set default device mode/state */
    APP_SetMode(gDeviceMode_Configuration_c);
    App_UpdateStateLeds(gDeviceState_FactoryDefault_c);
    gNwkFound = FALSE;
    gAttachFinished = FALSE;

    if(gpOpenThreadInstance)
    {
        /* Initialize CoAP demo */
        APP_InitCoapDemo();

#if gEnableLatencyMeasure_d
        LatencyMeasure_Init(gpOpenThreadInstance);
#endif

#if gEnableOTAClient_d
        OtaClientInit(gpOpenThreadInstance);
#endif

#if APP_AUTOSTART
        tmrStartApp = TMR_AllocateTimer();

        if(tmrStartApp != gTmrInvalidTimerID_c)
        {
            uint32_t jitterTime = NWKU_GetRandomNoFromInterval(gAppFactoryResetTimeoutMin_c,
                                                               gAppFactoryResetTimeoutMax_c);
            TMR_StartSingleShotTimer(tmrStartApp, jitterTime, APP_AutoStartCb, NULL);
        }
#endif
    }
}

/*!*************************************************************************************************
\fn     void App_Handler(void)
\brief  Application Handler. In this configuration is called on the task with the lowest priority
***************************************************************************************************/
void APP_Handler
(
    void
)
{
    App_HandleKeyboard();
}

/*!*************************************************************************************************
\fn     void Stack_to_APP_Handler(void *param)
\brief  This function is used to handle stack events in asynchronous mode.

\param  [in]    param    Pointer to stack event
***************************************************************************************************/
void Stack_to_APP_Handler
(
    uint32_t flags,
    void *param
)
{
    if((flags & OT_CHANGED_THREAD_ROLE) != 0)
    {
        otDeviceRole devRole = otThreadGetDeviceRole(gpOpenThreadInstance);

        switch(devRole)
        {
            case OT_DEVICE_ROLE_CHILD:
                /* Attached to a parent in a found network */
                mFirstPushButtonPressed = FALSE;
                gAttachFinished = TRUE;
                APP_SetMode(gDeviceMode_Application_c);
                App_UpdateStateLeds(gDeviceState_NwkConnected_c);
                PWR_AllowDeviceToSleep();
                break;

            case OT_DEVICE_ROLE_DETACHED:
                if(gAttachFinished == TRUE)
                {
                    APP_SetMode(gDeviceMode_Configuration_c);
                    App_UpdateStateLeds(gDeviceState_NwkFailure_c);
                    gAttachFinished = FALSE;
                }
                break;

            case OT_DEVICE_ROLE_DISABLED:
                APP_SetMode(gDeviceMode_Configuration_c);
                App_UpdateStateLeds(gDeviceState_FactoryDefault_c);
                break;

            default:
                break;
        }
    }

    if((flags & OT_CHANGED_THREAD_PANID) != 0)
    {
        if(!otThreadIsDiscoverInProgress(gpOpenThreadInstance))
        {
            gPanId = otLinkGetPanId(gpOpenThreadInstance);
        }
    }

    if((flags & OT_CHANGED_THREAD_CHANNEL) != 0)
    {
        gChannel = otLinkGetChannel(gpOpenThreadInstance);
        gScanMask = 1 << gChannel;
    }

    if((flags & OT_CHANGED_THREAD_NETWORK_NAME) != 0)
    {

    }

    if((flags & OT_CHANGED_THREAD_EXT_PANID) != 0)
    {

    }

    if((flags & OT_CHANGED_SUPPORTED_CHANNEL_MASK) != 0)
    {

    }

    if((flags & OT_CHANGED_IP6_MULTICAST_SUBSCRIBED) != 0)
    {
        APP_OtSetMulticastAddresses(gpOpenThreadInstance);

        /* Set application CoAP destination to all nodes on connected network */
        if(!IP_IsAddrEqual(&in6addr_realmlocal_allthreadnodes.mFields, &in6addr_any.mFields))
        {
            FLib_MemCpy(&gCoapDestAddress, &in6addr_realmlocal_allthreadnodes, sizeof(otIp6Address));
        }
    }
}

/*!*************************************************************************************************
\fn     void App_AllowDeviceToSleep()
\brief  This function is used to handle radio allow device to sleep events
***************************************************************************************************/
void App_AllowDeviceToSleep()
{
    bSleepAllowed = TRUE;
}
/*!*************************************************************************************************
\fn     void App_DisallowDeviceToSleep()
\brief  This function is used to handle radio disallow device to sleep events
***************************************************************************************************/
void App_DisallowDeviceToSleep()
{
    bSleepAllowed = FALSE;
}

/*==================================================================================================
Private functions
==================================================================================================*/
/*!*************************************************************************************************
\private
\fn     static void APP_InitCoapDemo(void)
\brief  Initialize CoAP demo.
***************************************************************************************************/
static void APP_InitCoapDemo
(
    void
)
{
    otCoapStart(gpOpenThreadInstance, OT_DEFAULT_COAP_PORT);
    otCoapSetDefaultHandler(gpOpenThreadInstance, APP_CoapGenericCallback, NULL);

    gAPP_LED_URI_PATH.mContext = gpOpenThreadInstance;
    otCoapAddResource(gpOpenThreadInstance, &gAPP_LED_URI_PATH);

    gAPP_TEMP_URI_PATH.mContext = gpOpenThreadInstance;
    otCoapAddResource(gpOpenThreadInstance, &gAPP_TEMP_URI_PATH);

    gAPP_SINK_URI_PATH.mContext = gpOpenThreadInstance;
    otCoapAddResource(gpOpenThreadInstance, &gAPP_SINK_URI_PATH);
}

/*!*************************************************************************************************
\private
\fn     static void APP_OtStartDiscovery(uint8_t *param)
\brief  Start the discovery procedure.

\param  [in]    param    Not used
***************************************************************************************************/
static void APP_OtStartDiscovery
(
    void
)
{
    otThreadDiscover(gpOpenThreadInstance, gScanMask, OT_PANID_BROADCAST, FALSE, FALSE, APP_OtHandleScanResult, NULL);
}

/*!*************************************************************************************************
\private
\fn     static void APP_OtStartJoin(uint8_t *param)
\brief  Start the discovery procedure.

\param  [in]    param    Not used
***************************************************************************************************/
static void APP_OtStartJoin
(
    void
)
{
    otIp6SetEnabled(gpOpenThreadInstance, TRUE);

    if((gPanId != 0xFFFF) && (gChannel != 0))
    {
        APP_OtNwkJoin();
    }
    else
    {
        APP_OtStartDiscovery();
    }
}

/*!*************************************************************************************************
\private
\fn     static void App_JoinTimerCallback(void *param)
\brief  Join timer callback.

\param  [in]    param    Not used
***************************************************************************************************/
static void App_JoinTimerCallback
(
    void *param
)
{
    if(mFirstPushButtonPressed)
    {
        mJoiningIsAppInitiated = TRUE;
        TMR_FreeTimer(mAppTimerId);
        mAppTimerId = gTmrInvalidTimerID_c;
        APP_OtStartJoin();
    }
}

void APP_JoinerCallback(otError aError, void *aCtx)
{
    switch (aError)
    {
        case OT_ERROR_NONE:
            otThreadSetEnabled(gpOpenThreadInstance, TRUE);
            mJoiningIsAppInitiated = FALSE;
            break;

        default:
            /* failed join */
            break;
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_OtNwkJoin()
\brief  This is a handler for  KBD module - short press events. Device is in configuration mode.
***************************************************************************************************/
static void APP_OtNwkJoin()
{

    otThreadSetChildTimeout(gpOpenThreadInstance, OT_POLL_RATE/1000 + 4);
    otIp6SetEnabled(gpOpenThreadInstance, TRUE);

    if ((isOutOfBand == FALSE) && (FALSE == otDatasetIsCommissioned(gpOpenThreadInstance)))
    {
        otJoinerStart(gpOpenThreadInstance, (const char *)&gPskd, NULL, NULL,
                      NULL, NULL, NULL, APP_JoinerCallback, NULL);
    }
    else
    {
        otThreadSetEnabled(gpOpenThreadInstance, TRUE);
        mJoiningIsAppInitiated = FALSE;
    }
}

static void APP_OtHandleScanResult(otActiveScanResult *aResult, void *aContext)
{
    if(aResult == NULL)
    {
        if(!otThreadIsDiscoverInProgress(gpOpenThreadInstance))
        {
            if((gPanId != 0xFFFF) && (gChannel != 0))
            {
                APP_OtNwkJoin();
            }
            else
            {
                /* Search for Thread nwk failed, retry */
                if(mJoiningIsAppInitiated)
                {
                    if(gbRetryInterrupt)
                    {
                        mJoiningIsAppInitiated = TRUE;

                        /* Retry to join the network */
                        APP_OtStartJoin();
                        return;
                    }
                    mJoiningIsAppInitiated = FALSE;
                }
            }
        }
    }
    else
    {
        if(gNwkFound == FALSE)
        {
            gPanId = aResult->mPanId;
            gChannel = aResult->mChannel;

            otLinkSetPanId(gpOpenThreadInstance, gPanId);
            otLinkSetChannel(gpOpenThreadInstance, gChannel);

            gNwkFound = TRUE;
        }
    }
}

static void APP_ConfigModeSwShortPressHandler
(
    uint32_t keyEvent
)
{
    (void)keyEvent;

    if((APP_GetState() == gDeviceState_FactoryDefault_c) ||
       (APP_GetState() == gDeviceState_NwkFailure_c) ||
       (APP_GetState() == gDeviceState_NwkOperationPending_c))
    {
       App_UpdateStateLeds(gDeviceState_JoiningOrAttaching_c);
       mFirstPushButtonPressed = TRUE;

       if(mAppTimerId == gTmrInvalidTimerID_c)
       {
           mAppTimerId = TMR_AllocateTimer();
       }

       /* Validate application timer Id */
       if(mAppTimerId != gTmrInvalidTimerID_c)
       {
           /* Start the application timer. Wait gAppJoinTimeout_c to start the joining procedure */
           TMR_StartSingleShotTimer(mAppTimerId, gAppJoinTimeout_c, App_JoinTimerCallback, NULL);
       }
       else
       {
           mJoiningIsAppInitiated = TRUE;
           /* No timer available - try to join the network */
           APP_OtStartJoin();
       }
    }
    /* Double press */
    else if(mFirstPushButtonPressed)
    {
        /* Reset */
        mFirstPushButtonPressed = FALSE;
    }
}
/*!*************************************************************************************************
\private
\fn     static void APP_ConfigModeHandleKeyboard(uint32_t keyEvent)
\brief  This is a handler for KBD module events. Device is in configuration mode.

\param  [in]    keyEvent   The keyboard module event
***************************************************************************************************/
static void APP_ConfigModeHandleKeyboard
(
    uint32_t keyEvent
)
{
    switch(keyEvent)
    {
        case gKBD_EventPB1_c:
        case gKBD_EventPB2_c:
        case gKBD_EventPB3_c:
            APP_ConfigModeSwShortPressHandler(keyEvent);
            break;

        case gKBD_EventLongPB1_c:
        case gKBD_EventLongPB2_c:
        case gKBD_EventLongPB3_c:
            break;

        /* Factory reset */
        case gKBD_EventVeryLongPB1_c:
        case gKBD_EventVeryLongPB2_c:
        case gKBD_EventVeryLongPB3_c:
            APP_OtFactoryReset();
            break;

        default:
            break;
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_AppModeHandleKeyboard(uint32_t keyEvent)
\brief  This is a handler for KBD module events. Device is in application mode.

\param  [in]    keyEvent    The keyboard module event
***************************************************************************************************/
static void APP_AppModeHandleKeyboard
(
    uint32_t keyEvent
)
{
    switch(keyEvent)
    {
        case gKBD_EventPB1_c:
            /* Remote led - on */
            APP_SendLedOn();
            break;
        case gKBD_EventPB2_c:
            /* Remote led - off */
            APP_SendLedOff();
            break;

#if gKBD_KeysCount_c > 2
        case gKBD_EventPB3_c:
            /* Data sink create */
            APP_SendDataSinkCreate();
            break;
#endif
        case gKBD_EventLongPB1_c:
            /* Remote led flash */
            APP_SendLedFlash();
            break;
        case gKBD_EventLongPB2_c:
            /* Report temperature */
            APP_ReportTemp();
            break;

#if gKBD_KeysCount_c > 2
        case gKBD_EventLongPB3_c:
            /* Remote data sink release */
            APP_SendDataSinkRelease();

            /* Local data sink release */
            APP_LocalDataSinkRelease();

            break;
#endif
        case gKBD_EventVeryLongPB1_c:
            /* Factory reset */
            APP_OtFactoryReset();
            break;
        case gKBD_EventVeryLongPB2_c:
#if LARGE_NETWORK
            /* OTA factory reset */
            APP_SendResetToFactoryCommand();
            break;
#endif

#if gKBD_KeysCount_c > 2
        case gKBD_EventVeryLongPB3_c:
            /* Factory reset */
            APP_OtFactoryReset();
            break;
#endif

        default:
            break;
    }
}

/*!*************************************************************************************************
\private
\fn     static void App_HandleKeyboard(uint8_t *param)
\brief  This is a handler for KBD module events.

\param  [in]    param    The keyboard module event
***************************************************************************************************/
static void App_HandleKeyboard
(
    void
)
{
    uint8_t keyEvent = 0xFF;
    uint8_t pos = 0;

    while(eventMask)
    {
        for(pos = 0; pos < (8 * sizeof(eventMask)); pos++)
        {
            if(eventMask & (1 << pos))
            {
                keyEvent = pos;
                eventMask = eventMask & ~(1 << pos);
                break;
            }
        }

        if(APP_GetMode() == gDeviceMode_Configuration_c)
        {
            /* Device is in configuration mode */
            APP_ConfigModeHandleKeyboard(keyEvent);
        }
        else
        {
            /* Device is in application mode */
            APP_AppModeHandleKeyboard(keyEvent);
        }

        /* Allow sleep after processing the keyboard press */
        PWR_PreventEnterLowPower(FALSE);
    }
}

/*==================================================================================================
  Coap Demo functions:
==================================================================================================*/
/*!*************************************************************************************************
\private
\fn     static void APP_CoapGenericCallback(sessionStatus sessionStatus, uint8_t *pData,
                                            coapSession_t *pSession, uint32_t dataLen)
\brief  This function is the generic callback function for CoAP message.

\param  [in]    sessionStatus   Status for CoAP session
\param  [in]    pData           Pointer to CoAP message payload
\param  [in]    pSession        Pointer to CoAP session
\param  [in]    dataLen         Length of CoAP payload
***************************************************************************************************/
static void APP_CoapGenericCallback(void *pCtx, otMessage *pMsg, const otMessageInfo *pMsgInfo)
{
    /* If no ACK was received, try again */
    /*if(sessionStatus == gCoapFailure_c)
    {
        if(FLib_MemCmp(pSession->pUriPath->pUriPath, (coapUriPath_t *)&gAPP_TEMP_URI_PATH.pUriPath,
                       pSession->pUriPath->length))
        {
            APP_ReportTemp();
        }
    }*/
    /* Process data, if any */
    /*else
    {
        if ( otCoapMessageGetType(pMsg) == OT_COAP_TYPE_NON_CONFIRMABLE)
        {
            //Process data
        }
        else if (otCoapMessageGetType(pMsg) == OT_COAP_TYPE_CONFIRMABLE)
        {
            //Process data
        }
    }*/
}

static void APP_GetBoardTemperatureString(uint8_t *pString)
{
    int32_t tempInt = 0;
    uint8_t *pIndex = NULL;
    uint8_t sTemp[] = "Temp:";

    tempInt = BOARD_GetTemperature();
    pIndex = pString;
    FLib_MemCpy(pIndex, sTemp, sizeof(sTemp) - 1);
    pIndex += (sizeof(sTemp) - 1);
    NWKU_PrintDec(tempInt, pIndex, 2, TRUE);
}

/*!*************************************************************************************************
\private
\fn     static void APP_ReportTemp(uint8_t *pParam)
\brief  This open a socket and report the temperature to gCoapDestAddress.

\param  [in]    pParam    Not used
***************************************************************************************************/
static void APP_ReportTemp
(
    void
)
{
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    otCoapCode coapCode = OT_COAP_CODE_POST;
    otError error = OT_ERROR_NONE;
    uint8_t string[11] = {0};

    /* Get Temperature String */
    APP_GetBoardTemperatureString(&string[0]);

    if(!otIp6IsAddressEqual(otThreadGetMeshLocalEid(gpOpenThreadInstance), &gCoapDestAddress))
    {
        otMessageInfo messageInfo;
        otMessage *pMsg = otCoapNewMessage(gpOpenThreadInstance, NULL);

        if(pMsg)
        {
            if(!IP6_IsMulticastAddr(&gCoapDestAddress.mFields))
            {
                coapType = OT_COAP_TYPE_CONFIRMABLE;
            }

            otCoapMessageInit(pMsg, coapType, coapCode);
            otCoapMessageGenerateToken(pMsg, 4);
            error = otCoapMessageAppendUriPathOptions(pMsg, APP_TEMP_URI_PATH);
            error = otCoapMessageSetPayloadMarker(pMsg);

            FLib_MemSet(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mPeerAddr = gCoapDestAddress;
            messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

            error = otMessageAppend(pMsg, (const void *)&string, sizeof(string) - 1);
            error = otCoapSendRequest(gpOpenThreadInstance, pMsg, &messageInfo, NULL, NULL);

            if ((error != OT_ERROR_NONE) && (pMsg != NULL))
            {
                otMessageFree(pMsg);
            }
        }
    }
}

#if gKBD_KeysCount_c > 2
/*!*************************************************************************************************
\private
\fn     static nwkStatus_t APP_SendDataSinkCommand(uint8_t *pCommand, uint8_t dataLen)
\brief  This function is used to send a Data Sink command to APP_DEFAULT_DEST_ADDR.

\param  [in]    pCommand       Pointer to command data
\param  [in]    dataLen        Data length

\return         nwkStatus_t    Status of the command
***************************************************************************************************/
static bool_t APP_SendDataSinkCommand
(
    uint8_t *pCommand,
    uint8_t dataLen
)
{
    bool_t status = TRUE;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    otCoapCode coapCode = OT_COAP_CODE_POST;
    otError error = OT_ERROR_NONE;
    otIp6Address coapDestAddress = APP_DEFAULT_DEST_ADDR;
    otMessageInfo messageInfo;
    otMessage *pMsg = otCoapNewMessage(gpOpenThreadInstance, NULL);

    FLib_MemSet(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = coapDestAddress;
    messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    if(pMsg != NULL)
    {
        otCoapMessageInit(pMsg, coapType, coapCode);
        otCoapMessageGenerateToken(pMsg, 4);
        error = otCoapMessageAppendUriPathOptions(pMsg, APP_SINK_URI_PATH);
        error = otCoapMessageSetPayloadMarker(pMsg);
        error = otMessageAppend(pMsg, (const void *)pCommand, dataLen);
        error = otCoapSendRequest(gpOpenThreadInstance, pMsg, &messageInfo, NULL, NULL);

        if ((error != OT_ERROR_NONE) && (pMsg != NULL))
        {
            otMessageFree(pMsg);
        }
    }

    if(error != OT_ERROR_NONE)
    {
        status = FALSE;
    }

    return status;
}

/*!*************************************************************************************************
\private
\fn     static void APP_SendDataSinkCreate(uint8_t *pParam)
\brief  This function is used to send a Data Sink Create command to APP_DEFAULT_DEST_ADDR.

\param  [in]    pParam    Not used
***************************************************************************************************/
static void APP_SendDataSinkCreate
(
    void
)
{
    uint8_t aCommand[] = {"create"};

    /* Send command over the air */
    if(APP_SendDataSinkCommand(aCommand, sizeof(aCommand)))
    {
        /* Local data sink create */
        FLib_MemCpy(&gCoapDestAddress, otThreadGetMeshLocalEid(gpOpenThreadInstance), sizeof(otIp6Address));
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_SendDataSinkRelease(uint8_t *pParam)
\brief  This function is used to send a Data Sink Release command to APP_DEFAULT_DEST_ADDR.

\param  [in]    pParam    Pointer to stack event
***************************************************************************************************/
static void APP_SendDataSinkRelease
(
    void
)
{
    uint8_t aCommand[] = {"release"};

    /* Send command over the air */
    if(APP_SendDataSinkCommand(aCommand, sizeof(aCommand)))
    {
        /* Local data sink release */
        APP_LocalDataSinkRelease();
    }
}
#endif
/*!*************************************************************************************************
\private
\fn     static void APP_SendLedCommand(uint8_t *pCommand, uint8_t dataLen)
\brief  This function is used to send a Led command to gCoapDestAddress.

\param  [in]    pCommand    Pointer to command data
\param  [in]    dataLen     Data length
***************************************************************************************************/
static void APP_SendLedCommand
(
    uint8_t *pCommand,
    uint16_t dataLen
)
{
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    otCoapCode coapCode = OT_COAP_CODE_POST;
    otError error = OT_ERROR_NONE;

    if(!otIp6IsAddressEqual(otThreadGetMeshLocalEid(gpOpenThreadInstance), &gCoapDestAddress))
    {
        otMessageInfo messageInfo;
        otMessage *pMsg = otCoapNewMessage(gpOpenThreadInstance, NULL);

        if(pMsg)
        {
            if(!IP6_IsMulticastAddr(&gCoapDestAddress.mFields))
            {
                coapType = OT_COAP_TYPE_CONFIRMABLE;
            }
            else
            {
                APP_ProcessLedCmd(pCommand, dataLen);
            }

            otCoapMessageInit(pMsg, coapType, coapCode);
            otCoapMessageGenerateToken(pMsg, 4);

            error = otCoapMessageAppendUriPathOptions(pMsg, APP_LED_URI_PATH);
            error = otCoapMessageSetPayloadMarker(pMsg);
            error = otMessageAppend(pMsg, (const void *)pCommand, dataLen - 1);

            FLib_MemSet(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mPeerAddr = gCoapDestAddress;
            messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

            error = otCoapSendRequest(gpOpenThreadInstance, pMsg, &messageInfo, NULL, NULL);

            if ((error != OT_ERROR_NONE) && (pMsg != NULL))
            {
                otMessageFree(pMsg);
            }
        }
    }
    else
    {
        APP_ProcessLedCmd(pCommand, dataLen);
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_SendLedOn(uint8_t *pParam)
\brief  This function is used to send a Led RGB On command over the air.

\param  [in]    pParam    Not used
***************************************************************************************************/
static void APP_SendLedOn
(
    void
)
{
    uint8_t aCommand[] = {"on"};

    APP_SendLedCommand(aCommand, sizeof(aCommand));
}

/*!*************************************************************************************************
\private
\fn     static void APP_SendLedOff(uint8_t *pParam)
\brief  This function is used to send a Led RGB Off command over the air.

\param  [in]    pParam    Not used
***************************************************************************************************/
static void APP_SendLedOff
(
    void
)
{
    uint8_t aCommand[] = {"off"};

    APP_SendLedCommand(aCommand, sizeof(aCommand));
}

/*!*************************************************************************************************
\private
\fn     static void APP_SendLedFlash(uint8_t *pParam)
\brief  This function is used to send a Led flash command over the air.

\param  [in]    pParam    Not used
***************************************************************************************************/
static void APP_SendLedFlash
(
    void
)
{
    uint8_t aCommand[] = {"flash"};

    APP_SendLedCommand(aCommand, sizeof(aCommand));
}

/*!*************************************************************************************************
\private
\fn     static void APP_LocalDataSinkRelease(uint8_t *pParam)
\brief  This function is used to restore the default destination address for CoAP messages.

\param  [in]    pParam    Pointer to stack event
***************************************************************************************************/
static void APP_LocalDataSinkRelease
(
    void
)
{
    otIp6Address defaultDestAddress = APP_DEFAULT_DEST_ADDR;

    FLib_MemCpy(&gCoapDestAddress, &defaultDestAddress, sizeof(otIp6Address));
}

/*!*************************************************************************************************
\private
\fn     static void APP_CoapLedCb(coapSessionStatus_t sessionStatus, uint8_t *pData,
                                  coapSession_t *pSession, uint32_t dataLen)
\brief  This function is the callback function for CoAP LED message.
\brief  It performs the required operations and sends back a CoAP ACK message.

\param  [in]    sessionStatus   Status for CoAP session
\param  [in]    pData           Pointer to CoAP message payload
\param  [in]    pSession        Pointer to CoAP session
\param  [in]    dataLen         Length of CoAP payload
***************************************************************************************************/
static void APP_CoapLedCb
(
    void *pCtx,
    otMessage *pMsg,
    const otMessageInfo *pMsgInfo
)
{
    otCoapCode msgCode = otCoapMessageGetCode(pMsg);
    otCoapType msgType = otCoapMessageGetType(pMsg);
    uint8_t pData[20] = {0};
    otError error = OT_ERROR_NONE;
    uint16_t dataLen = otMessageGetLength(pMsg) - otMessageGetOffset(pMsg);

    otMessageRead(pMsg, otMessageGetOffset(pMsg), (void *)pData, dataLen);
    /* Process the command only if it is a POST method */
    if(msgCode == OT_COAP_CODE_POST)
    {
        APP_ProcessLedCmd((uint8_t *)pData, dataLen);
    }

    /* Send the reply */
    if(OT_COAP_TYPE_CONFIRMABLE == msgType)
    {
        otMessage *responseMessage = NULL;
        responseMessage = otCoapNewMessage(gpOpenThreadInstance, NULL);

        if(responseMessage != NULL)
        {
            otCoapMessageInitResponse(responseMessage, pMsg, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);

            error = otCoapSendResponse(gpOpenThreadInstance, responseMessage, pMsgInfo);

            if((error != OT_ERROR_NONE) && (responseMessage != NULL))
            {
                otMessageFree(responseMessage);
            }
        }
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_ProcessLedCmd(uint8_t *pCommand, uint16_t dataLen)
\brief  This function is used to process a LED command (on, off, flash, toggle, rgb, color wheel).

\param  [in]    pCommand    Pointer to command data
\param  [in]    dataLen     Data length
***************************************************************************************************/
static void APP_ProcessLedCmd
(
    uint8_t *pCommand,
    uint16_t dataLen
)
{
    /* Set mode state */
    APP_SetMode(gDeviceMode_Application_c);
    mFirstPushButtonPressed  = FALSE;

    /* Process command */
    if(FLib_MemCmp(pCommand, "on", MIN(dataLen, strlen("on"))))
    {
        App_UpdateStateLeds(gDeviceState_AppLedOn_c);
    }
    else if(FLib_MemCmp(pCommand, "off", MIN(dataLen, strlen("off"))))
    {
        App_UpdateStateLeds(gDeviceState_AppLedOff_c);
    }
    else if(FLib_MemCmp(pCommand, "toggle", MIN(dataLen, strlen("toggle"))))
    {
        App_UpdateStateLeds(gDeviceState_AppLedToggle_c);
    }
    else if(FLib_MemCmp(pCommand, "flash", MIN(dataLen, strlen("flash"))))
    {
        App_UpdateStateLeds(gDeviceState_AppLedFlash_c);
    }
    else if(FLib_MemCmp(pCommand, "rgb", MIN(dataLen, strlen("rgb"))))
    {
        char* p = (char *)pCommand + strlen("rgb");
        uint8_t redValue = 0, greenValue = 0, blueValue = 0;
        appDeviceState_t appState = gDeviceState_AppLedRgb_c;

        dataLen -= strlen("rgb");

        while(dataLen != 0)
        {
            if(*p == 'r')
            {
                p++;
                dataLen--;
                redValue = NWKU_atoi(p);
            }

            if(*p == 'g')
            {
                p++;
                dataLen--;
                greenValue = NWKU_atoi(p);
            }

            if(*p == 'b')
            {
                p++;
                dataLen--;
                blueValue = NWKU_atoi(p);
            }

            dataLen--;
            p++;
        }

        /* Update RGB values */
        appState = gDeviceState_AppLedOff_c;

        if(redValue || greenValue || blueValue)
        {
            appState = gDeviceState_AppLedOn_c;
        }

        App_UpdateStateLeds(appState);
    }
    else if(FLib_MemCmp(pCommand, "color wheel", MIN(dataLen, strlen("color wheel"))))
    {
        App_UpdateStateLeds(gDeviceState_AppLedFlash_c);
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_CoapTempCb(coapSessionStatus_t sessionStatus, uint8_t *pData,
                                   coapSession_t *pSession, uint32_t dataLen)
\brief  This function is the callback function for CoAP temperature message.
\brief  It sends the temperature value in a CoAP ACK message.

\param  [in]    sessionStatus   Status for CoAP session
\param  [in]    pData           Pointer to CoAP message payload
\param  [in]    pSession        Pointer to CoAP session
\param  [in]    dataLen         Length of CoAP payload
***************************************************************************************************/
static void APP_CoapTempCb
(
    void *pCtx,
    otMessage *pMsg,
    const otMessageInfo *pMsgInfo
)
{
    otCoapCode msgCode = otCoapMessageGetCode(pMsg);
    otCoapType msgType = otCoapMessageGetType(pMsg);
    uint8_t string[11] = {0};

    if(msgCode == OT_COAP_CODE_GET)
    {
        /* Get Temperature String */
        APP_GetBoardTemperatureString(&string[0]);
    }
    else if(msgCode == OT_COAP_CODE_POST)
    {
        uint8_t temp[10];
        uint32_t maxDisplayedString = 10;
        uint16_t dataLen = otMessageGetLength(pMsg) - otMessageGetOffset(pMsg);

        if(dataLen != 0)
        {
            otMessageRead(pMsg, otMessageGetOffset(pMsg), (void *)temp, dataLen);
            /* Prevent from buffer overload */
            (dataLen >= maxDisplayedString) ? (dataLen = (maxDisplayedString - 1)) : (dataLen);
        }
    }

    if(OT_COAP_TYPE_CONFIRMABLE == msgType)
    {
        otError error = OT_ERROR_NONE;
        otMessage *responseMessage = NULL;
        responseMessage = otCoapNewMessage(gpOpenThreadInstance, NULL);

        if(responseMessage != NULL)
        {
            if(OT_COAP_CODE_GET == msgCode)
            {
                otCoapMessageInitResponse(responseMessage, pMsg, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CONTENT);
            }
            else
            {
                otCoapMessageInitResponse(responseMessage, pMsg, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);
            }

            error = otCoapMessageAppendUriPathOptions(responseMessage, APP_TEMP_URI_PATH);

            if(OT_COAP_CODE_GET == msgCode)
            {
                error = otCoapMessageSetPayloadMarker(responseMessage);
                error = otMessageAppend(responseMessage, (const void *)&string, sizeof(string));
            }

            error = otCoapSendResponse(gpOpenThreadInstance, responseMessage, pMsgInfo);

            if((error != OT_ERROR_NONE) && (responseMessage != NULL))
            {
                otMessageFree(responseMessage);
            }
        }
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_CoapSinkCb(coapSessionStatus_t sessionStatus, uint8_t *pData,
                                   coapSession_t *pSession, uint32_t dataLen)
\brief  This function is the callback function for CoAP sink message.

\param  [in]    sessionStatus   Status for CoAP session
\param  [in]    pData           Pointer to CoAP message payload
\param  [in]    pSession        Pointer to CoAP session
\param  [in]    dataLen         Length of CoAP payload
***************************************************************************************************/
static void APP_CoapSinkCb
(
    void *pCtx,
    otMessage *pMsg,
    const otMessageInfo *pMsgInfo
)
{
    uint16_t dataLen = otMessageGetLength(pMsg) - otMessageGetOffset(pMsg);
    uint8_t pData[7] = {0};

    otMessageRead(pMsg, otMessageGetOffset(pMsg), (void *)pData, dataLen);
    /* Process command */
    if(FLib_MemCmp(pData, "create", 6))
    {
        /* Data sink create */
        FLib_MemCpy(&gCoapDestAddress, &pMsgInfo->mPeerAddr, sizeof(otIp6Address));
    }

    if(FLib_MemCmp(pData, "release", 7))
    {
        /* Data sink release */
        APP_LocalDataSinkRelease();
    }

    if(otCoapMessageGetType(pMsg) == OT_COAP_TYPE_CONFIRMABLE)
    {
        /* Send CoAP ACK */
        otError error = OT_ERROR_NONE;
        otMessage *responseMessage = NULL;
        responseMessage = otCoapNewMessage(gpOpenThreadInstance, NULL);

        if(responseMessage != NULL)
        {
            otCoapMessageInitResponse(responseMessage, pMsg, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);

            error = otCoapSendResponse(gpOpenThreadInstance, responseMessage, pMsgInfo);

            if((error != OT_ERROR_NONE) && (responseMessage != NULL))
            {
                otMessageFree(responseMessage);
            }
        }
    }
}

#if LARGE_NETWORK
/*!*************************************************************************************************
\private
\fn     static void APP_SendResetToFactoryCommand()
\brief  This function is used to send a Factory Reset command to APP_DEFAULT_DEST_ADDR.

\param  [in]    pParam    Pointer to stack event
***************************************************************************************************/
static void APP_SendResetToFactoryCommand
(
    void
)
{
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    otCoapCode coapCode = OT_COAP_CODE_POST;
    otError error = OT_ERROR_NONE;
    otMessageInfo messageInfo;
    otMessage *pMsg = otCoapNewMessage(gpOpenThreadInstance, NULL);

    if(pMsg)
    {
        FLib_MemSet(&messageInfo, 0, sizeof(messageInfo));
        messageInfo.mPeerAddr = APP_DEFAULT_DEST_ADDR;
        messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

        otCoapMessageInit(pMsg, coapType, coapCode);
        otCoapMessageGenerateToken(pMsg, 4);
        error = otCoapMessageAppendUriPathOptions(pMsg, APP_RESET_TO_FACTORY_URI_PATH);
        error = otCoapSendRequest(gpOpenThreadInstance, pMsg, &messageInfo, NULL, NULL);

        if ((error != OT_ERROR_NONE) && (pMsg != NULL))
        {
            otMessageFree(pMsg);
        }
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_CoapResetToFactoryDefaultsCb(coapSessionStatus_t sessionStatus, uint8_t *pData,
                                                     coapSession_t *pSession, uint32_t dataLen)
\brief  This function is the callback function for CoAP factory reset message.

\param  [in]    sessionStatus   Status for CoAP session
\param  [in]    pData           Pointer to CoAP message payload
\param  [in]    pSession        Pointer to CoAP session
\param  [in]    dataLen         Length of CoAP payload
***************************************************************************************************/
static void APP_CoapResetToFactoryDefaultsCb
(
    void *pCtx,
    otMessage *pMsg,
    const otMessageInfo *pMsgInfo
)
{
    APP_OtFactoryReset();
}
#endif

#if APP_AUTOSTART
/*!*************************************************************************************************
\private
\fn     static void APP_AutoStart(void)
\brief  This is the autostart function, used to start the network joining.

\param  [in]    param    Not used
***************************************************************************************************/
static void APP_AutoStart
(
    void
)
{
    /* If device is disconnected */
    if(otThreadGetDeviceRole(gpOpenThreadInstance) < OT_DEVICE_ROLE_CHILD)
    {
        mJoiningIsAppInitiated = TRUE;

        APP_OtStartJoin();
    }
}

/*!*************************************************************************************************
\private
\fn     static void APP_AutoStartCb(void)
\brief  This is the autostart callback function.

\param  [in]    param    Not used
***************************************************************************************************/
static void APP_AutoStartCb
(
    void *param
)
{
    APP_AutoStart();
}
#endif

/*==================================================================================================
Private debug functions
==================================================================================================*/
