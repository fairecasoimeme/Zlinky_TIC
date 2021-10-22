/*! *********************************************************************************
* Copyright 2020 NXP
* All rights reserved.
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "app_ntag_core_state.h"
/* Drivers includes */
#include "fsl_ntag.h"
#include "ntag_driver.h"
#include "ntag_bridge.h"

/************************************************************************************
*************************************************************************************
* Private Macros
*************************************************************************************
************************************************************************************/
//#include "fsl_debug_console.h"
#define APP_NTAG_CORE_STATE_LOG(...) //PRINTF("[APP_NTAG_CORE] "); PRINTF(__VA_ARGS__);

/************************************************************************************
*************************************************************************************
* Private definitions
*************************************************************************************
************************************************************************************/

typedef enum
{
    E_NTAG_SRAM_WRITE_STATE_PUSH_DATA,
    E_NTAG_SRAM_WRITE_STATE_POLL_SRAM_RF_READY_OFF,
} eAppNtagWriteSramState;

typedef struct
{
    eAppNtagWriteSramState stateType;
    uint8_t *pDataToWrite;
    uint32_t dataLen;
} sAppNtagCoreWriteSramState_t;


typedef enum
{
    E_NTAG_SRAM_READ_STATE_SET_TRANSFERT_DIR,
    E_NTAG_SRAM_READ_STATE_POLL_SRAM_I2C_READY_ON,
    E_NTAG_SRAM_READ_STATE_DATA_READ
} eAppNtagReadSramState;

typedef struct
{
    eAppNtagReadSramState stateType;
    uint8_t *pDataRead;
    uint32_t dataLen;
} sAppNtagCoreReadSramState_t;

typedef enum
{
    E_NTAG_STATE_WRITE_SRAM,
    E_NTAG_STATE_READ_SRAM,
    E_NTAG_STATE_IDLE
} eAppNtagState;

typedef struct 
{
    eAppNtagState stateType;
    union{
        sAppNtagCoreReadSramState_t readSramState;
        sAppNtagCoreWriteSramState_t writeSramState;
    } stateDetail;
} sAppNtagCoreState_t;

typedef struct 
{
    bool_t inField;
    sAppNtagCoreState_t ntagCoreState;
    NTAG_HANDLE_T ntagDriverHandle;
    AppPostEventCallback appPostEventCallback;
    AppProcessEventCallback appProcessEventCallback;
} sAppNtagCoreHandle_t;


/************************************************************************************
*************************************************************************************
* Private function declarations
*************************************************************************************
************************************************************************************/
static void app_ntag_core_state_fieldDetectCb(ntag_field_detect_t fd, void *userData);
static void app_ntag_core_state_processWriteSramStateMachine();
static void app_ntag_core_state_processReadSramStateMachine();
static void app_ntag_core_state_postAppEvent(eAppNtagCoreStateEventType eventType, uint8_t *pdata, uint32_t len);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static sAppNtagCoreHandle_t ntagCoreStateHandle;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

void app_ntag_core_state_init(AppPostEventCallback appPostEventCallback, 
                        AppProcessEventCallback appProcessEventCallback,
                        NTAG_HANDLE_T ntagDriverHandle)
{
    ntag_config_t ntagConfig;
    NTAG_GetDefaultConfig(&ntagConfig);
    ntagConfig.userData = NULL;
    ntagConfig.callback = app_ntag_core_state_fieldDetectCb;

    /* Init NTAG driver: pads, FD handling */
    NTAG_Init(&ntagConfig);

    /* Init the local instance */
    ntagCoreStateHandle.inField = FALSE;
    ntagCoreStateHandle.ntagCoreState.stateType = E_NTAG_STATE_IDLE;
    ntagCoreStateHandle.ntagDriverHandle = ntagDriverHandle;
    ntagCoreStateHandle.appPostEventCallback = appPostEventCallback;
    ntagCoreStateHandle.appProcessEventCallback = appProcessEventCallback;
}

void app_ntag_core_state_runStateMachine()
{
    switch (ntagCoreStateHandle.ntagCoreState.stateType)
    {
        case E_NTAG_STATE_WRITE_SRAM:
            app_ntag_core_state_processWriteSramStateMachine();
            break;
        case E_NTAG_STATE_READ_SRAM:
            app_ntag_core_state_processReadSramStateMachine();
            break;
        case E_NTAG_STATE_IDLE:
            break;
        default:
            break;
    }

}

bool_t app_ntag_core_state_isInField()
{
    return ntagCoreStateHandle.inField;
}

bool_t app_ntag_core_state_writeToSram(uint8_t *pData, uint32_t dataLen)
{
    bool_t result = FALSE;
    APP_NTAG_CORE_STATE_LOG("%s\r\n", __FUNCTION__);
    do
    {
        if (dataLen > NTAG_MEM_SRAM_SIZE || pData == NULL)
            break;
        if (ntagCoreStateHandle.ntagCoreState.stateType != E_NTAG_STATE_IDLE)
            break;
        ntagCoreStateHandle.ntagCoreState.stateType = E_NTAG_STATE_WRITE_SRAM;
        ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.stateType = E_NTAG_SRAM_WRITE_STATE_PUSH_DATA;
        ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.pDataToWrite = pData;
        ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.dataLen = dataLen;
        result = TRUE;
    } while (0);

    return result;  
}

bool_t app_ntag_core_state_readFromSram(uint8_t *pData, uint32_t dataLen)
{
    bool_t result = FALSE;
    APP_NTAG_CORE_STATE_LOG("%s\r\n", __FUNCTION__);
    do
    {
        if (dataLen > NTAG_MEM_SRAM_SIZE || pData == NULL)
            break;
        if (ntagCoreStateHandle.ntagCoreState.stateType != E_NTAG_STATE_IDLE)
            break;
        ntagCoreStateHandle.ntagCoreState.stateType = E_NTAG_STATE_READ_SRAM;
        ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.stateType = E_NTAG_SRAM_READ_STATE_SET_TRANSFERT_DIR;
        ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.pDataRead = pData;
        ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.dataLen = dataLen;
        result = TRUE;
    } while (0);

    return result;  
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

static void app_ntag_core_state_fieldDetectCb(ntag_field_detect_t fd, void *userData)
{
    switch (fd)
    {
        case kNTAG_FieldDetectIn:
            APP_NTAG_CORE_STATE_LOG("kNTAG_FieldDetectIn (%d)\r\n", ntagCoreStateHandle.inField);
            if (!ntagCoreStateHandle.inField)
            {
                ntagCoreStateHandle.inField = TRUE;
                app_ntag_core_state_postAppEvent(E_APP_NTAG_CORE_STATE_EVENT_FD, NULL, 0);
            }
            break;
        case kNTAG_FieldDetectOut:
            APP_NTAG_CORE_STATE_LOG("kNTAG_FieldDetectOut (%d) state = %d\r\n", ntagCoreStateHandle.inField, ntagCoreStateHandle.ntagCoreState.stateType);
            if (ntagCoreStateHandle.inField)
            {
                ntagCoreStateHandle.inField = FALSE;
                if (ntagCoreStateHandle.ntagCoreState.stateType == E_NTAG_STATE_READ_SRAM)
                {
                    app_ntag_core_state_postAppEvent(E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_FAIL, ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.pDataRead, ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.dataLen);
                }
                else if (ntagCoreStateHandle.ntagCoreState.stateType == E_NTAG_STATE_WRITE_SRAM)
                {
                    app_ntag_core_state_postAppEvent(E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_FAIL, ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.pDataToWrite, ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.dataLen);
                }
                /* Move to idle state */
                ntagCoreStateHandle.ntagCoreState.stateType = E_NTAG_STATE_IDLE;
                app_ntag_core_state_postAppEvent(E_APP_NTAG_CORE_STATE_EVENT_FD, NULL, 0);
            }
            break;
        default:
            break;
    }
}

static void app_ntag_core_state_processWriteSramStateMachine()
{
    uint8_t reg = 0;
    switch (ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.stateType)
    {
        case E_NTAG_SRAM_WRITE_STATE_PUSH_DATA:
            // Enable Passthrough Mode I2C->RF
            NTAG_SetTransferDir(ntagCoreStateHandle.ntagDriverHandle, I2C_TO_RF);
            NTAG_SetPthruOnOff(ntagCoreStateHandle.ntagDriverHandle, TRUE);
            NTAG_WriteBytes(ntagCoreStateHandle.ntagDriverHandle, NTAG_MEM_ADDR_START_SRAM,
                                ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.pDataToWrite,
                                ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.dataLen);
            ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.stateType = E_NTAG_SRAM_WRITE_STATE_POLL_SRAM_RF_READY_OFF;
            break;
        case E_NTAG_SRAM_WRITE_STATE_POLL_SRAM_RF_READY_OFF:
            /* Read NS register */
            NTAG_ReadRegister(ntagCoreStateHandle.ntagDriverHandle, NTAG_MEM_OFFSET_NS_REG, &reg);
            if (!(reg & NTAG_NS_REG_MASK_SRAM_RF_READY))
            {
                app_ntag_core_state_postAppEvent(E_APP_NTAG_CORE_STATE_EVENT_WRITE_SRAM_OK, ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.pDataToWrite, ntagCoreStateHandle.ntagCoreState.stateDetail.writeSramState.dataLen);
                /* Move to idle state */
                ntagCoreStateHandle.ntagCoreState.stateType = E_NTAG_STATE_IDLE;        
            }
            break;
        default:
            break;
    }
}

static void app_ntag_core_state_processReadSramStateMachine()
{
    uint8_t reg = 0;
    switch(ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.stateType)
    {
        case E_NTAG_SRAM_READ_STATE_SET_TRANSFERT_DIR:
            // Enable Passthrough Mode RF->I2C
            NTAG_SetTransferDir(ntagCoreStateHandle.ntagDriverHandle, RF_TO_I2C);
            NTAG_SetPthruOnOff(ntagCoreStateHandle.ntagDriverHandle, TRUE);
            ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.stateType = E_NTAG_SRAM_READ_STATE_POLL_SRAM_I2C_READY_ON;
            break;
        case E_NTAG_SRAM_READ_STATE_POLL_SRAM_I2C_READY_ON:
            /* Read NS register */
            NTAG_ReadRegister(ntagCoreStateHandle.ntagDriverHandle, NTAG_MEM_OFFSET_NS_REG, &reg);
            if (reg & NTAG_NS_REG_MASK_SRAM_I2C_READY)
            {
                ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.stateType = E_NTAG_SRAM_READ_STATE_DATA_READ;
            }
            break;
        case E_NTAG_SRAM_READ_STATE_DATA_READ:
            NTAG_ReadBytes(ntagCoreStateHandle.ntagDriverHandle, NTAG_MEM_ADDR_START_SRAM, ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.pDataRead, ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.dataLen);
            app_ntag_core_state_postAppEvent(E_APP_NTAG_CORE_STATE_EVENT_READ_SRAM_OK, ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.pDataRead, ntagCoreStateHandle.ntagCoreState.stateDetail.readSramState.dataLen);               
            /* Move to idle state */
            ntagCoreStateHandle.ntagCoreState.stateType = E_NTAG_STATE_IDLE;  
            break;
        default:
            break;
    }
}

static void app_ntag_core_state_postAppEvent(eAppNtagCoreStateEventType eventType, uint8_t *pdata, uint32_t len)
{
    sAppNtagCoreStateEvent_t event;
    event.eventType = eventType;
    event.pData = pdata;
    event.dataLen = len;
    /* Post an event to the app */
    ntagCoreStateHandle.appPostEventCallback(ntagCoreStateHandle.appProcessEventCallback, &event);
}