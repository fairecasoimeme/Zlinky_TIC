/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/

#include "app_nci.h"
#include "EmbeddedTypes.h"
#include "fsl_debug_console.h"
#include "fsl_os_abstraction.h"
#include "MemManager.h"
#include "Messaging.h"
#include "Panic.h"
#include "FunctionLib.h"

#include "nci_manager.h"
#include "nci.h"

#include "app_sram_cmd.h"

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define APP_NCI_LOG(...) PRINTF("[APP] "); PRINTF(__VA_ARGS__);
#define APP_NCI_LOG_EVENT(...)
#define SRAM_BUFFER_SIZE 64

/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/

typedef struct
{
    eAppSramCmdStateType currentState;
    uint8_t sramBuffer[SRAM_BUFFER_SIZE];
    sAppSramCmdToReceive_t *pCmdToReceive;
    sAppSramCmdToSend_t *pCmdToSend;
    uint32_t savedTick;
} sAppNciHandle_t;

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/
static void app_nci_postEvent(NCI_ManagerAppProcessEventCallback appCallback, sNciManagerEvent_t *pEvent);
static void app_nci_processEvent(void *pPram);
static sAppSramCmdToReceive_t* app_nci_getCmdToReceiveContext(uint8_t tlvTag, uint8_t len);
static void app_nci_processCmdGetDeviceNameResponse(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived);
static void app_nci_processCmdPutProvisioningDataResponse(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived);

/************************************************************************************
 *************************************************************************************
 * Extern function
 *************************************************************************************
 ************************************************************************************/
extern void app_queue_nci_msg(sAppNciMsg_t *pMsg);

/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/
static const uint8_t provisioningData[] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                            0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                                            0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
                                            0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                                            0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
                                            0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
                                            0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
                                            0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                                            0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09};

static sAppSramCmdToSend_t nciCmdsToSend[] = {{E_TLV_TAG_CMD_GET_DEVICE_NAME_REQUEST, TLV_TAG_AND_TLV_LEN_SIZE, 0, NULL}, 
                                              {E_TLV_TAG_CMD_PUT_PROVISIONING_DATA_REQUEST, sizeof(provisioningData)+TLV_TAG_AND_TLV_LEN_SIZE, 0, &provisioningData[0]}};
static sAppSramCmdToReceive_t nciCmdsToReceive[] = {{E_TLV_TAG_CMD_GET_DEVICE_NAME_REQUEST, 0, 0, app_nci_processCmdGetDeviceNameResponse}, 
                                                     {E_TLV_TAG_CMD_PUT_PROVISIONING_DATA_REQUEST, 0, 0, app_nci_processCmdPutProvisioningDataResponse}};

static sAppNciHandle_t appNciHandle;

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/

void app_nci_init()
{
    appNciHandle.savedTick = 0;
    /* Init the nci lib */
    NCI_vInitialise(0xFF, 10, 400000, 7, 18);
    /* Init the nci_manager component */
    NCI_ManagerInit(app_nci_postEvent, app_nci_processEvent);
}

void app_nci_runStateMachine ()
{
    uint32_t currentTime = OSA_TimeGetMsec();
    uint32_t diffTime = OSA_TimeGetMsec()-appNciHandle.savedTick;
    if (diffTime >= 5)
    {
        NCI_ManagerRunStateMachine(diffTime);
        appNciHandle.savedTick = currentTime;
    }
}

/************************************************************************************
 *************************************************************************************
 * Private function functions
 *************************************************************************************
 ************************************************************************************/

static void app_nci_postEvent(NCI_ManagerAppProcessEventCallback appCallback, sNciManagerEvent_t *pEvent)
{
    uint8_t *pMsg = MEM_BufferAlloc(sizeof(sAppNciMsg_t) + sizeof(sNciManagerEvent_t));
    sAppNciMsg_t *pMsgStruct = NULL;
    sNciManagerEvent_t *pEventTosave = NULL;

    if (pMsg != NULL)
    {
        pMsgStruct = (sAppNciMsg_t *) pMsg;
        pEventTosave = (sNciManagerEvent_t *) (pMsg + sizeof(sAppNciMsg_t));
        pMsgStruct->processMsg = appCallback;
        pMsgStruct->pMsgParm = (void *) pEventTosave;
        FLib_MemCpy(pEventTosave, pEvent, sizeof(sNciManagerEvent_t));
        app_queue_nci_msg(pMsgStruct);
    }
}

static void app_nci_processEvent(void *pParam)
{
    APP_NCI_LOG_EVENT("%s \r\n", __FUNCTION__);
    sNciManagerEvent_t *pEvent = (sNciManagerEvent_t *) pParam;
    if (pEvent != NULL)
    {
        switch(pEvent->eventType)
        {
            case E_NCI_MANAGER_INIT_FAIL:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_INIT_FAIL\r\n");
                NCI_ManagerStop();
                break;
            case E_NCI_MANAGER_INIT_OK:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_INIT_OK\r\n");
                /* NCI sends the first cmd */
                appNciHandle.currentState = E_APP_SRAM_CMD_STATE_SENDING_CMD;
                appNciHandle.pCmdToSend = &nciCmdsToSend[0];
                appNciHandle.pCmdToSend->cmdLenSent = 0;
                appNciHandle.pCmdToReceive = NULL;
                app_sram_cmd_send(appNciHandle.pCmdToSend,
                        appNciHandle.sramBuffer,
                        sizeof(appNciHandle.sramBuffer),
                        &appNciHandle.currentState,
                        NCI_ManagerWriteToSram);
                break;
            case E_NCI_MANAGER_READ_FAIL:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_READ_FAIL\r\n");
                NCI_ManagerStop();
                break;
            case E_NCI_MANAGER_READ_OK:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_READ_OK\r\n");
                app_sram_cmd_receive(&appNciHandle.pCmdToReceive, 
                        app_nci_getCmdToReceiveContext, 
                        appNciHandle.sramBuffer,
                        sizeof(appNciHandle.sramBuffer),
                        &appNciHandle.currentState,
                        NCI_ManagerReadFromSram);
                break;
            case E_NCI_MANAGER_WRITE_FAIL:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_WRITE_FAIL\r\n");
                NCI_ManagerStop();
                break;
            case E_NCI_MANAGER_WRITE_OK:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_WRITE_OK\r\n");
                app_sram_cmd_send(appNciHandle.pCmdToSend,
                        appNciHandle.sramBuffer,
                        sizeof(appNciHandle.sramBuffer),
                        &appNciHandle.currentState,
                        NCI_ManagerWriteToSram);
                if (appNciHandle.currentState == E_APP_SRAM_CMD_STATE_WAITING_CMD)
                    NCI_ManagerReadFromSram(appNciHandle.sramBuffer, sizeof(appNciHandle.sramBuffer));
                break;
            case E_NCI_MANAGER_EVENT_PRESENT:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_EVENT_PRESENT\r\n");
                break;
            case E_NCI_MANAGER_EVENT_ABSENT:
                APP_NCI_LOG_EVENT("E_NCI_MANAGER_EVENT_ABSENT\r\n");
                break;
            default:
                APP_NCI_LOG_EVENT("event = %d \r\n", pEvent->eventType);
                break;
        }
    }
}

static sAppSramCmdToReceive_t* app_nci_getCmdToReceiveContext(uint8_t tlvTag, uint8_t len)
{
    uint32_t i;
    sAppSramCmdToReceive_t* pResult = NULL;
    for (i=0; i<(sizeof(nciCmdsToReceive)/sizeof(sAppSramCmdToReceive_t)); i++)
    {
        if (nciCmdsToReceive[i].tlvTag == tlvTag)
        {
            nciCmdsToReceive[i].cmdLen = len+TLV_TAG_AND_TLV_LEN_SIZE;
            nciCmdsToReceive[i].cmdLenReceived = 0;
            pResult = &nciCmdsToReceive[i];
            break;
        }
    }
    return pResult;
}

static void app_nci_processCmdGetDeviceNameResponse(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived)
{
    uint32_t i;
    APP_NCI_LOG("Get Device Name response content [");
    for (i=0; i<dataLen; i++)
    {
        PRINTF("%c ", pData[i]);
    }
    PRINTF("]\n");

    if (allDataReceived)
    {
        /* Send the next cmd */
        appNciHandle.currentState = E_APP_SRAM_CMD_STATE_SENDING_CMD;
        appNciHandle.pCmdToSend = &nciCmdsToSend[1];
        appNciHandle.pCmdToSend->cmdLenSent = 0;
        appNciHandle.pCmdToReceive = NULL;
        app_sram_cmd_send(appNciHandle.pCmdToSend,
                appNciHandle.sramBuffer,
                sizeof(appNciHandle.sramBuffer),
                &appNciHandle.currentState,
                NCI_ManagerWriteToSram);
    }
}

static void app_nci_processCmdPutProvisioningDataResponse(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived)
{
    uint32_t i;
    APP_NCI_LOG("Put Provisioning data response content [");
    for (i=0; i<dataLen; i++)
    {
        PRINTF("%c ", pData[i]);
    }
    PRINTF("]\n");
}