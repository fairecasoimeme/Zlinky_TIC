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

#include "app_ntag.h"
#include "fsl_os_abstraction.h"
#include "EmbeddedTypes.h"
#include "fsl_debug_console.h"
#include "MemManager.h"
#include "Messaging.h"
#include "Panic.h"
#include "FunctionLib.h"
#include "PWR_Interface.h"

#include "app_ntag_core_state.h"
#include "app_ntag_smart_poster.h"
/* NTAG driver includes */
#include "HAL_I2C_driver.h"
#include "ntag_driver.h"
#include "ntag_defines.h"

#define SRAM_DATA_TRANSFERT_DEMO
#ifdef SRAM_DATA_TRANSFERT_DEMO
#include "app_sram_cmd.h"
#endif

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define APP_NTAG_LOG(...) PRINTF("[APP] "); PRINTF(__VA_ARGS__);
#define APP_NTAG_LOG_EVENT(...) //PRINTF("[APP_NTAG_EVENT] "); PRINTF(__VA_ARGS__);

#define APP_NTAG_DEVICE_NAME {'n','t','a','g','_','i','2','c'}
#define APP_NTAG_CMD_OK_RESPONSE {'O','K'}


/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/
typedef enum
{
    E_APP_NTAG_NO_ERROR,
    E_APP_NTAG_I2C_ERROR,
    E_APP_NTAG_READ_ERROR,
    E_APP_NTAG_WRITE_ERROR
} eAppNtagError;

typedef struct
{
#ifdef SRAM_DATA_TRANSFERT_DEMO
    eAppSramCmdStateType sramCmdState;
    uint8_t sramBuffer[NTAG_MEM_SRAM_SIZE];
    sAppSramCmdToReceive_t *pCmdToReceive;
    sAppSramCmdToSend_t cmdToSend;
#endif
    NTAG_HANDLE_T ntagDriverHandleInstance;
} sAppNtagHandle_t;
/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/
static void app_ntag_postEvent(AppProcessEventCallback appCallback, sAppNtagCoreStateEvent_t *pEvent);
static void app_ntag_processEvent(void *pPram);
static eAppNtagError app_ntag_initEepromSmartPoster(void);
static bool_t app_ntag_isEepromSmartPosterConfigured(eAppNtagError *pNtagError);
static eAppNtagError app_ntag_eepromWriteSmartPoster(void);
static eAppNtagError app_ntag_unlockWriteAccess(void);
static eAppNtagError app_ntag_lockWriteAccess(void);

#ifdef SRAM_DATA_TRANSFERT_DEMO
static void app_ntag_sramSendResponse();
static void app_ntag_sramProcessCmdGetDeviceNameRequest(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived);
static void app_ntag_sramProcessCmdPutProvisioningDataRequest(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived);
static sAppSramCmdToReceive_t* app_ntag_sramGetCmdToReceiveContext(uint8_t tlvTag, uint8_t len);
#endif
/************************************************************************************
 *************************************************************************************
 * Extern function
 *************************************************************************************
 ************************************************************************************/
extern void app_queue_ntag_msg(sAppNtagMsg_t *pMsg);

/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/
static sAppNtagHandle_t appNtagHandle;

#ifdef SRAM_DATA_TRANSFERT_DEMO
static sAppSramCmdToReceive_t ntagSramCmdsToReceive[] = {{E_TLV_TAG_CMD_GET_DEVICE_NAME_REQUEST, 0, 0, app_ntag_sramProcessCmdGetDeviceNameRequest}, 
                                                     {E_TLV_TAG_CMD_PUT_PROVISIONING_DATA_REQUEST, 0, 0, app_ntag_sramProcessCmdPutProvisioningDataRequest}};
static const uint8_t appNtagDeviceName[] = APP_NTAG_DEVICE_NAME;
static const uint8_t appNtagCmdOkResponse[] = APP_NTAG_CMD_OK_RESPONSE;
#endif

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/

bool_t app_ntag_init(void)
{
    bool_t result = FALSE;
    do
    {
        /* Create the Ntag driver instance */
        appNtagHandle.ntagDriverHandleInstance = NTAG_InitDevice((NTAG_ID_T)0, I2C2);
        if (appNtagHandle.ntagDriverHandleInstance == NULL)
            break;
        if (app_ntag_initEepromSmartPoster() != E_APP_NTAG_NO_ERROR)
            break;
#ifdef SRAM_DATA_TRANSFERT_DEMO
        appNtagHandle.sramCmdState = E_APP_SRAM_CMD_STATE_WAITING_PRESENCE;
        appNtagHandle.pCmdToReceive = NULL;
#endif
        app_ntag_core_state_init(app_ntag_postEvent, app_ntag_processEvent, appNtagHandle.ntagDriverHandleInstance); 
        result = TRUE;
    } while (0);
    return result;
}

void app_ntag_runStateMachine()
{
    app_ntag_core_state_runStateMachine();
}

/************************************************************************************
 *************************************************************************************
 * Private function functions
 *************************************************************************************
 ************************************************************************************/

static void app_ntag_postEvent(AppProcessEventCallback appCallback, sAppNtagCoreStateEvent_t *pEvent)
{
    uint8_t *pMsg = MEM_BufferAlloc(sizeof(sAppNtagMsg_t) + sizeof(sAppNtagCoreStateEvent_t));
    sAppNtagMsg_t *pMsgStruct = NULL;
    sAppNtagCoreStateEvent_t *pEventTosave = NULL;

    if (pMsg != NULL)
    {
        pMsgStruct = (sAppNtagMsg_t *) pMsg;
        pEventTosave = (sAppNtagCoreStateEvent_t *) (pMsg + sizeof(sAppNtagMsg_t));
        pMsgStruct->processMsg = appCallback;
        pMsgStruct->pMsgParm = (void *) pEventTosave;
        FLib_MemCpy(pEventTosave, pEvent, sizeof(sAppNtagCoreStateEvent_t));
        app_queue_ntag_msg(pMsgStruct);
    }
}

static void app_ntag_processEvent(void *pPram)
{
    APP_NTAG_LOG_EVENT("%s \r\n", __FUNCTION__);
    sAppNtagCoreStateEvent_t *pEvent = (sAppNtagCoreStateEvent_t *) pPram;
    bool_t ntagInfieldNewValue = FALSE;
    OSA_InterruptDisable();
    if (pEvent != NULL)
    {
        switch (pEvent->eventType)
        {
            case E_APP_NTAG_CORE_STATE_EVENT_FD:
                APP_NTAG_LOG_EVENT("E_APP_NTAG_CORE_STATE_EVENT_FD\r\n");
                /* Get the field status */
                ntagInfieldNewValue = app_ntag_core_state_isInField();
                (void) ntagInfieldNewValue;
#ifdef SRAM_DATA_TRANSFERT_DEMO
                if (ntagInfieldNewValue && appNtagHandle.sramCmdState == E_APP_SRAM_CMD_STATE_WAITING_PRESENCE)
                {
                    appNtagHandle.sramCmdState = E_APP_SRAM_CMD_STATE_WAITING_CMD;
                    APP_NTAG_LOG_EVENT("PRESENT\r\n");
                    PWR_DisallowDeviceToSleep();
                    HAL_I2C_InitDevice(HAL_I2C_INIT_DEFAULT, kCLOCK_Fro32M, I2C2);
                    app_ntag_core_state_readFromSram(appNtagHandle.sramBuffer, sizeof(appNtagHandle.sramBuffer));
                } 
                else if (!ntagInfieldNewValue && appNtagHandle.sramCmdState != E_APP_SRAM_CMD_STATE_WAITING_PRESENCE)
                {
                    appNtagHandle.sramCmdState = E_APP_SRAM_CMD_STATE_WAITING_PRESENCE;
                    APP_NTAG_LOG_EVENT("ABSENT\r\n");
                    HAL_I2C_CloseDevice(I2C2);
                    PWR_AllowDeviceToSleep();
                }
#endif
                break;
#ifdef SRAM_DATA_TRANSFERT_DEMO
            case E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_FAIL:
                APP_NTAG_LOG_EVENT("E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_FAIL\r\n");
                break;
            case E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_OK:
                APP_NTAG_LOG_EVENT("E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_OK\r\n");
                app_sram_cmd_receive(&appNtagHandle.pCmdToReceive, 
                            app_ntag_sramGetCmdToReceiveContext, 
                            appNtagHandle.sramBuffer,
                            sizeof(appNtagHandle.sramBuffer),
                            &appNtagHandle.sramCmdState,
                            app_ntag_core_state_readFromSram);
                break;
            case E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_FAIL:
                APP_NTAG_LOG_EVENT("E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_FAIL\r\n");
                break;
            case E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_OK:
                APP_NTAG_LOG_EVENT("E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_OK\r\n");
                app_ntag_sramSendResponse();
                if (appNtagHandle.sramCmdState == E_APP_SRAM_CMD_STATE_WAITING_CMD)
                    app_ntag_core_state_readFromSram(appNtagHandle.sramBuffer, sizeof(appNtagHandle.sramBuffer));
                break;
#endif
            default:
                break;
        }
    }
    OSA_InterruptEnable();
}

#ifdef SRAM_DATA_TRANSFERT_DEMO
static void app_ntag_sramSendResponse()
{
    app_sram_cmd_send(&appNtagHandle.cmdToSend,
                        appNtagHandle.sramBuffer,
                        sizeof(appNtagHandle.sramBuffer),
                        &appNtagHandle.sramCmdState,
                        app_ntag_core_state_writeToSram);
}

static sAppSramCmdToReceive_t* app_ntag_sramGetCmdToReceiveContext(uint8_t tlvTag, uint8_t len)
{
    uint32_t i;
    sAppSramCmdToReceive_t* pResult = NULL;
    for (i=0; i<(sizeof(ntagSramCmdsToReceive)/sizeof(sAppSramCmdToReceive_t)); i++)
    {
        if (ntagSramCmdsToReceive[i].tlvTag == tlvTag)
        {
            ntagSramCmdsToReceive[i].cmdLen = len+TLV_TAG_AND_TLV_LEN_SIZE;
            ntagSramCmdsToReceive[i].cmdLenReceived = 0;
            pResult = &ntagSramCmdsToReceive[i];
            break;
        }
    }
    return pResult;
}

static void app_ntag_sramProcessCmdGetDeviceNameRequest(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived)
{
    uint32_t i;
    APP_NTAG_LOG("Get Device Name request content [ ");
    for (i=0; i<dataLen; i++)
    {
        PRINTF("0x%x ", pData[i]);
    }
    PRINTF("]\n");

    if (allDataReceived)
    {
        appNtagHandle.cmdToSend.tlvTag = E_TLV_TAG_CMD_GET_DEVICE_NAME_REQUEST;
        appNtagHandle.cmdToSend.cmdLen = TLV_TAG_AND_TLV_LEN_SIZE + sizeof(appNtagDeviceName);
        appNtagHandle.cmdToSend.cmdLenSent = 0; 
        appNtagHandle.cmdToSend.pData = &appNtagDeviceName[0];
        app_ntag_sramSendResponse();
    }
}

static void app_ntag_sramProcessCmdPutProvisioningDataRequest(const uint8_t *pData, uint8_t dataLen, bool_t allDataReceived)
{
    uint32_t i;
    APP_NTAG_LOG("Put Provisioning data request content [");
    for (i=0; i<dataLen; i++)
    {
        PRINTF("0x%x ", pData[i]);
    }
    PRINTF("]\n");

    if (allDataReceived)
    {
        appNtagHandle.cmdToSend.tlvTag = E_TLV_TAG_CMD_PUT_PROVISIONING_DATA_REQUEST;
        appNtagHandle.cmdToSend.cmdLen = TLV_TAG_AND_TLV_LEN_SIZE + sizeof(appNtagCmdOkResponse);
        appNtagHandle.cmdToSend.cmdLenSent = 0; 
        appNtagHandle.cmdToSend.pData = &appNtagCmdOkResponse[0];
        app_ntag_sramSendResponse();
    }
}
#endif

static eAppNtagError app_ntag_initEepromSmartPoster(void)
{
    eAppNtagError ntagErr = E_APP_NTAG_NO_ERROR;
    bool_t isConfigured = FALSE;
    uint8_t byte0 = 0;
    uint8_t i = 0;
    bool_t i2cAddrFound = FALSE;
    do 
    {
        HAL_I2C_InitDevice(HAL_I2C_INIT_DEFAULT, kCLOCK_Fro32M, I2C2);
        /* Try to access the device at default I2C address */
        if (NTAG_ReadBytes(appNtagHandle.ntagDriverHandleInstance, 0, &byte0, sizeof(byte0)))
        {
            /* Try now with the 0x02 I2C address */
            NTAG_SetNtagI2cAddress(appNtagHandle.ntagDriverHandleInstance, 0x2);
            if (!NTAG_ReadBytes(appNtagHandle.ntagDriverHandleInstance, 0, &byte0, sizeof(byte0)))
            {
                i2cAddrFound = TRUE;
            }
            else
            {
                /* Loop to try to find a valid i2c address */
                for (i=0;i<0xFF; i++)
                {
                    if (i==2 || i ==0x55) /* Skip default and 0x02 I2C address */
                        continue;
                    NTAG_SetNtagI2cAddress(appNtagHandle.ntagDriverHandleInstance, i);
                    if (!NTAG_ReadBytes(appNtagHandle.ntagDriverHandleInstance, 0, &byte0, sizeof(byte0)))
                    {
                        i2cAddrFound = TRUE;
                        break;
                    }
                }
            }
            if (!i2cAddrFound)
            {
                ntagErr = E_APP_NTAG_READ_ERROR;
                break;
            }
        }
        isConfigured = app_ntag_isEepromSmartPosterConfigured(&ntagErr);
        if (ntagErr != E_APP_NTAG_NO_ERROR)
            break;
        if (!isConfigured)
        {
            ntagErr = app_ntag_eepromWriteSmartPoster();
        }
    } while (0);
    /* Stop I2C */
    HAL_I2C_CloseDevice(I2C2);
    return ntagErr;
}

static bool_t app_ntag_isEepromSmartPosterConfigured(eAppNtagError *pNtagError)
{
    bool_t result = FALSE;
    uint32_t addrToRead = NTAG_I2C_BLOCK_SIZE;
    uint8_t buf[4];
    uint16_t smartPosterNdefSize = app_ntag_smart_poster_getSize();
    if (!NTAG_ReadBytes(appNtagHandle.ntagDriverHandleInstance, addrToRead, buf, sizeof(buf)))
    {
        *pNtagError = E_APP_NTAG_NO_ERROR;
        if (smartPosterNdefSize < 0xFF)
        {
            result = (buf[1] == smartPosterNdefSize);
        }
        else
        {
            result = (buf[1] == 0xff 
                        && buf[2] == (uint8_t) ((smartPosterNdefSize & 0x0000FF00) >> 8)
                        && buf[3] == (uint8_t)(smartPosterNdefSize & 0x000000FF));
        }
    }
    else
    {
        *pNtagError = E_APP_NTAG_READ_ERROR; 
    }
    return result;
}

static eAppNtagError app_ntag_eepromWriteSmartPoster(void)
{
    eAppNtagError ntagErr = E_APP_NTAG_NO_ERROR;
    do
    {
        /* Unlock write access */
        ntagErr = app_ntag_unlockWriteAccess();
        if (ntagErr != E_APP_NTAG_NO_ERROR)
            break;
        /* If necessary set the configuration registers
        * here by calling NTAG_WriteConfiguration 
        */
        /* Write the smart poster value */
        if (!app_ntag_smart_poster_eepromWrite(appNtagHandle.ntagDriverHandleInstance))
        {
            ntagErr = E_APP_NTAG_WRITE_ERROR;
            break;
        }
        /* Lock write access */
        ntagErr = app_ntag_lockWriteAccess();
    } while(0);
    return ntagErr;
}

static eAppNtagError app_ntag_unlockWriteAccess(void)
{
    eAppNtagError ntagErr = E_APP_NTAG_NO_ERROR;
    uint8_t bytes[NTAG_I2C_BLOCK_SIZE];
    FLib_MemSet(bytes, 0x0, sizeof(bytes));
    
    do
    {
        /* Try to read the block 0 */
        if (NTAG_ReadBytes(appNtagHandle.ntagDriverHandleInstance, 0, bytes, sizeof(bytes)))
        {
            ntagErr = E_APP_NTAG_READ_ERROR;
            break;
        }
        /* Set the Capability Container (CC) */
        bytes[12] = 0xE1; /* Indicates that NDEF data is present inside the tag */
        bytes[13] = 0x10; /* Indicates to support the version 1.0 */
        bytes[14] = 0xE9; /* Indicates 1864 bytes of memory size assigned to the data area */
        bytes[15] = 0x00; /* Indicates read and write access granted without any security */
        if (NTAG_WriteBytes(appNtagHandle.ntagDriverHandleInstance, 0, bytes, sizeof(bytes)))
        {
            ntagErr = E_APP_NTAG_WRITE_ERROR;
        }
    } while (0);
    

    return ntagErr;
}

static eAppNtagError app_ntag_lockWriteAccess(void)
{
    eAppNtagError ntagErr = E_APP_NTAG_NO_ERROR;
    uint8_t bytes[NTAG_I2C_BLOCK_SIZE];
    FLib_MemSet(bytes, 0x0, sizeof(bytes));

    do
    {
        /* Try to read the block 0 */
        if (NTAG_ReadBytes(appNtagHandle.ntagDriverHandleInstance, 0, bytes, sizeof(bytes)))
        {
            ntagErr = E_APP_NTAG_READ_ERROR;
            break;
        }
        /* Set the Capability Container (CC) */
        bytes[3] = 0xE1; /* Indicates that NDEF data is present inside the tag */
        bytes[4] = 0x10; /* Indicates to support the version 1.0 */
        bytes[5] = 0xE9; /* Indicates 1864 bytes of memory size assigned to the data area */
        bytes[6] = 0x0F; /* Indicates read only access granted */
        if (NTAG_WriteBytes(appNtagHandle.ntagDriverHandleInstance, 0, bytes, sizeof(bytes)))
        {
            ntagErr = E_APP_NTAG_WRITE_ERROR;
        }
    } while (0);
    return ntagErr;
}
