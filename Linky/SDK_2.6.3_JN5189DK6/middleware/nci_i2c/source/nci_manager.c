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
#include "nci_manager.h"
#include "nci.h"
#include "nfc.h"
/************************************************************************************
*************************************************************************************
* Private Macros
*************************************************************************************
************************************************************************************/
//#include "fsl_debug_console.h"
#define NCI_MANAGER_LOG(...) //PRINTF("[NCI_MANAGER] "); PRINTF(__VA_ARGS__);

#define NFC_BLOCK_SIZE 16

/************************************************************************************
*************************************************************************************
* Private definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
    E_NCI_INIT_STATE_READ_HEADER,
    E_NCI_INIT_STATE_READ_VERSION,
    E_NCI_INIT_STATE_POLL_I2C_LOCKED_OFF,
    E_NCI_INIT_STATE_READ_LAST_BLOCK,
} eNciManagerInitState;

typedef enum
{
    E_NCI_SRAM_WRITE_STATE_POLL_PTHRU_ON_DIR_OUT,
    E_NCI_SRAM_WRITE_STATE_DATA_PUSHED,
    E_NCI_SRAM_WRITE_STATE_POLL_SRAM_I2C_READY_OFF,
} eNciManagerWriteSramState;

typedef struct
{
    eNciManagerWriteSramState stateType;
    uint8_t *pDataToWrite;
    uint32_t dataLen;
} sNciManagerWriteSramState_t;


typedef enum
{
    E_NCI_SRAM_READ_STATE_POLL_SRAM_RF_READY_ON,
    E_NCI_SRAM_READ_STATE_DATA_READ
} eNciManagerReadSramState;

typedef struct
{
    eNciManagerReadSramState stateType;
    uint8_t *pDataRead;
    uint32_t dataLen;
} sNciManagerReadSramState_t;

typedef enum
{
    E_NCI_STATE_INIT,
    E_NCI_STATE_WRITE_SRAM,
    E_NCI_STATE_READ_SRAM,
    E_NCI_STATE_IDLE

} eNciManagerState;

typedef struct 
{
    eNciManagerState stateType;
    union{
        eNciManagerInitState initState;
        sNciManagerReadSramState_t readSramState;
        sNciManagerWriteSramState_t writeSramState;
    } stateDetail;
} sNciManagerState_t;

typedef struct 
{
    bool_t ntagInField;
    sNciManagerState_t nciManagerState;
    uint8_t nfcBuffer[NFC_HEADER_SIZE];
    NCI_ManagerAppPostEventCallback appPostEventCallback;
    NCI_ManagerAppProcessEventCallback appProcessEventCallback;
    sNciManagerEvent_t appEvent;
} sNciManagerHandle_t;


/************************************************************************************
*************************************************************************************
* Private function declarations
*************************************************************************************
************************************************************************************/
static void NCI_ManagerEventHandler(teNciEvent  eNciEvent, uint32_t u32Address, uint32_t u32Length, uint8_t *pu8Data);
static void NCI_ManagerProcessInitStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data);
static void NCI_ManagerProcessWriteSramStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data);
static void NCI_ManagerProcessReadSramStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data);
static void NCI_ManagerProcessIdleStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data);
static void NCI_ManagerProcessReadOkEventInitState(uint8_t *pu8Data, uint32_t u32Length);
static void NCI_ManagerSetHandleAppEvent(eNciManagerAppEventType eventType, uint8_t *pdata, uint32_t len);
static void NCI_ManagerProcessEventPresent(void);
static void NCI_ManagerProcessEventAbsent(void);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static sNciManagerHandle_t nciManagerHandle;

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

void NCI_ManagerInit(NCI_ManagerAppPostEventCallback appPostEventCallback, NCI_ManagerAppProcessEventCallback appProcessEventCallback)
{
    NCI_vRegCbEvent(NCI_ManagerEventHandler);
    /* Init the nci manager states */
    NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_NO_EVENT, NULL, 0);
    nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_INIT;
    nciManagerHandle.appPostEventCallback = appPostEventCallback;
    nciManagerHandle.appProcessEventCallback = appProcessEventCallback;
    nciManagerHandle.ntagInField = FALSE;
}

void NCI_ManagerRunStateMachine(uint32_t tickMs)
{
    if (nciManagerHandle.appEvent.eventType != E_NCI_MANAGER_NO_EVENT)
    {
        /* Post an event to the app */
        nciManagerHandle.appPostEventCallback(nciManagerHandle.appProcessEventCallback, &nciManagerHandle.appEvent);
        NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_NO_EVENT, NULL, 0);
    }
    else if (nciManagerHandle.nciManagerState.stateType == E_NCI_STATE_WRITE_SRAM
        && nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType == E_NCI_SRAM_WRITE_STATE_POLL_PTHRU_ON_DIR_OUT)
    {
        /* In case of SRAM Write state start the first step */
        NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
    }
    else if (nciManagerHandle.nciManagerState.stateType == E_NCI_STATE_READ_SRAM
        && nciManagerHandle.nciManagerState.stateDetail.readSramState.stateType == E_NCI_SRAM_READ_STATE_POLL_SRAM_RF_READY_ON)
    {
        /* In case of SRAM Read state start the first step */
        NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
    }
    else if (nciManagerHandle.nciManagerState.stateType == E_NCI_STATE_IDLE)
    {
        NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
    }
    /* Run the NCI core state machine */
    NCI_vTick(tickMs);
}

bool_t NCI_ManagerWriteToSram(uint8_t *pData, uint32_t dataLen)
{
    bool_t result = FALSE;
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    do
    {
        if (dataLen > NFC_SRAM_SIZE || pData == NULL)
            break;
        if (nciManagerHandle.nciManagerState.stateType != E_NCI_STATE_IDLE)
            break;
        nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_WRITE_SRAM;
        nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType = E_NCI_SRAM_WRITE_STATE_POLL_PTHRU_ON_DIR_OUT;
        nciManagerHandle.nciManagerState.stateDetail.writeSramState.pDataToWrite = pData;
        nciManagerHandle.nciManagerState.stateDetail.writeSramState.dataLen = dataLen;
        result = TRUE;
    } while (0);

    return result;
}

bool_t NCI_ManagerReadFromSram(uint8_t *pData, uint32_t dataLen)
{
    bool_t result = FALSE;
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    do
    {
        if (dataLen > NFC_SRAM_SIZE || pData == NULL)
            break;
        if (nciManagerHandle.nciManagerState.stateType != E_NCI_STATE_IDLE)
            break;
        nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_READ_SRAM;
        nciManagerHandle.nciManagerState.stateDetail.readSramState.stateType = E_NCI_SRAM_READ_STATE_POLL_SRAM_RF_READY_ON;
        nciManagerHandle.nciManagerState.stateDetail.readSramState.pDataRead = pData;
        nciManagerHandle.nciManagerState.stateDetail.readSramState.dataLen = dataLen;
        result = TRUE;
    } while (0);

    return result;
}

bool_t NCI_ManagerStop(void)
{
    return NCI_bEnd();
}

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

static void NCI_ManagerEventHandler(teNciEvent  eNciEvent, uint32_t u32Address, uint32_t u32Length, uint8_t *pu8Data)
{
    switch (nciManagerHandle.nciManagerState.stateType)
    {
        case E_NCI_STATE_INIT:
            NCI_ManagerProcessInitStateMachine(eNciEvent, u32Length, pu8Data);
            break;
        case E_NCI_STATE_WRITE_SRAM:
            NCI_ManagerProcessWriteSramStateMachine(eNciEvent, u32Length, pu8Data);
            break;
        case E_NCI_STATE_READ_SRAM:
            NCI_ManagerProcessReadSramStateMachine(eNciEvent, u32Length, pu8Data);
            break;
        case E_NCI_STATE_IDLE:
            NCI_ManagerProcessIdleStateMachine(eNciEvent, u32Length, pu8Data);
            break;
        default:
            break;
    }
}

static void NCI_ManagerProcessInitStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    switch (eNciEvent)
    {
        case E_NCI_EVENT_PRESENT:
        {
            NCI_ManagerProcessEventPresent();
            NCI_bRead(0, NFC_HEADER_SIZE, nciManagerHandle.nfcBuffer);
            NCI_MANAGER_LOG("E_NCI_EVENT_PRESENT\r\n");
            break;
        }
        case E_NCI_EVENT_READ_OK:
        {
            NCI_ManagerProcessReadOkEventInitState(pu8Data, u32Length);
            break;
        }
        case E_NCI_EVENT_WRITE_FAIL:
        case E_NCI_EVENT_READ_FAIL:
        {
            NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_INIT_FAIL, NULL, 0);
            NCI_MANAGER_LOG("Event = %d\r\n", eNciEvent);
            break;
        }
        case E_NCI_EVENT_ABSENT:
        {
            NCI_ManagerProcessEventAbsent();
            break;
        }
        default:
            break;
    }  
}

static void NCI_ManagerProcessReadOkEventInitState(uint8_t *pu8Data, uint32_t u32Length)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    if (u32Length > 0)
    {
        switch(nciManagerHandle.nciManagerState.stateDetail.initState)
        {
            case E_NCI_INIT_STATE_READ_HEADER:
            {
                /* Is this an NXP tag */
                if (pu8Data[0] == 0x04)
                {
                    nciManagerHandle.nciManagerState.stateDetail.initState = E_NCI_INIT_STATE_READ_VERSION;
                    NCI_bReadVersion(NFC_VERSION_SIZE, nciManagerHandle.nfcBuffer);
                    NCI_MANAGER_LOG("E_NCI_INIT_STATE_READ_HEADER NXP tag\r\n");
                }
                else
                {
                    nciManagerHandle.nciManagerState.stateDetail.initState = E_NCI_INIT_STATE_READ_HEADER;
                    NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_INIT_FAIL, NULL, 0);
                    NCI_MANAGER_LOG("E_NCI_INIT_STATE_READ_HEADER not an NXP tag\r\n");
                }
                break;
            }
            case E_NCI_INIT_STATE_READ_VERSION:
            {
                /* Known NTAG type ? */
                if (NCI_u32Ntag() != 0)
                {
                    nciManagerHandle.nciManagerState.stateDetail.initState = E_NCI_INIT_STATE_POLL_I2C_LOCKED_OFF;
                    NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
                    NCI_MANAGER_LOG("E_NCI_INIT_STATE_READ_VERSION known tag\r\n");
                }
                else
                {
                    nciManagerHandle.nciManagerState.stateDetail.initState = E_NCI_INIT_STATE_READ_HEADER;
                    NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_INIT_FAIL, NULL, 0);
                    NCI_MANAGER_LOG("E_NCI_INIT_STATE_READ_VERSION unknown tag\r\n");
                }
                break;
            }
            case E_NCI_INIT_STATE_POLL_I2C_LOCKED_OFF:
            {
                /* Is the interface not locked for I2C use ? */
                if ((nciManagerHandle.nfcBuffer[NFC_OFFSET_NS_REG] & NFC_MASK_NS_REG_I2C_LOCKED) == 0)
                {
                    nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_IDLE;
                    NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_INIT_OK, NULL, 0);
                    NCI_MANAGER_LOG("E_NCI_INIT_STATE_POLL_I2C_LOCKED_OFF i2c not locked\r\n");
                }
                else
                {
                    /* Retry to read the NS reg */
                    NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
                    NCI_MANAGER_LOG("E_NCI_INIT_STATE_POLL_I2C_LOCKED_OFF i2c locked => retry\r\n");
                }
                break;
            }
            default:
                break;
        }
    }
}

static void NCI_ManagerProcessWriteSramStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    switch (eNciEvent)
    {
        case E_NCI_EVENT_READ_OK:
        {
            if (nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType 
                    == E_NCI_SRAM_WRITE_STATE_POLL_PTHRU_ON_DIR_OUT)
            {
                /* In pass through out mode ? */
                if ((nciManagerHandle.nfcBuffer[NFC_OFFSET_NC_REG] & NFC_MASK_NC_REG_PTHRU_ON_OFF) != 0
                &&  (nciManagerHandle.nfcBuffer[NFC_OFFSET_NC_REG] & NFC_MASK_NC_REG_PTHRU_DIR)    != 0)
                {
                    if (NCI_bWrite(NCI_u32Sram(), nciManagerHandle.nciManagerState.stateDetail.writeSramState.dataLen, nciManagerHandle.nciManagerState.stateDetail.writeSramState.pDataToWrite))
                    {
                        nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType = E_NCI_SRAM_WRITE_STATE_DATA_PUSHED;
                        NCI_MANAGER_LOG("E_NCI_SRAM_WRITE_STATE_POLL_PTHRU_ON_DIR_OUT enabled\r\n");
                    }
                    else
                    {
                        NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_WRITE_FAIL, NULL, 0);
                    }
                }
                else
                {
                    NCI_MANAGER_LOG("E_NCI_SRAM_WRITE_STATE_POLL_PTHRU_ON_DIR_OUT disabled => retry\r\n");
                }
                
                                
            }
            else if (nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType 
                    == E_NCI_SRAM_WRITE_STATE_POLL_SRAM_I2C_READY_OFF)
            {
                /* Is SRAM I2C ready off ? */
                if ((nciManagerHandle.nfcBuffer[NFC_OFFSET_NS_REG] & NFC_MASK_NS_REG_SRAM_I2C_READY) == 0)
                {
                    nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_IDLE;
                    NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_WRITE_OK, nciManagerHandle.nciManagerState.stateDetail.writeSramState.pDataToWrite, nciManagerHandle.nciManagerState.stateDetail.writeSramState.dataLen);
                    NCI_MANAGER_LOG("E_NCI_SRAM_WRITE_STATE_POLL_SRAM_I2C_READY_OFF ready off\r\n");
                }
                else
                {
                    /* Re-try to read the I2C ready off */
                    NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
                    NCI_MANAGER_LOG("E_NCI_SRAM_WRITE_STATE_POLL_SRAM_I2C_READY_OFF not ready => retry\r\n");
                }
                
            }
            break;
        }
        case E_NCI_EVENT_WRITE_OK:
        {
            if (nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType 
                    == E_NCI_SRAM_WRITE_STATE_DATA_PUSHED)
            {
                nciManagerHandle.nciManagerState.stateDetail.writeSramState.stateType = E_NCI_SRAM_WRITE_STATE_POLL_SRAM_I2C_READY_OFF;
                NCI_bRead(NCI_u32Session(), NFC_BLOCK_SIZE, nciManagerHandle.nfcBuffer);
                NCI_MANAGER_LOG("E_NCI_SRAM_WRITE_STATE_DATA_PUSHED\r\n");
            }
            break;
        }
        case E_NCI_EVENT_WRITE_FAIL:
        case E_NCI_EVENT_READ_FAIL:
        {
            NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_WRITE_FAIL, NULL, 0);
            NCI_MANAGER_LOG("Event = %d\r\n", eNciEvent);
            break;
        }
        case E_NCI_EVENT_ABSENT:
        {
            NCI_ManagerProcessEventAbsent();
            break;
        }
        default:
            break;
    }
}

static void NCI_ManagerProcessReadSramStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    switch (eNciEvent)
    {
        case E_NCI_EVENT_READ_OK:
        {
            if (nciManagerHandle.nciManagerState.stateDetail.readSramState.stateType 
                    == E_NCI_SRAM_READ_STATE_POLL_SRAM_RF_READY_ON)
            {
                /* Is SRAM RF ready on ? */
                if ((nciManagerHandle.nfcBuffer[NFC_OFFSET_NS_REG] & NFC_MASK_NS_REG_SRAM_RF_READY) != 0)
                {
                    nciManagerHandle.nciManagerState.stateDetail.readSramState.stateType = E_NCI_SRAM_READ_STATE_DATA_READ;
                    NCI_bRead(NCI_u32Sram(), nciManagerHandle.nciManagerState.stateDetail.readSramState.dataLen, nciManagerHandle.nciManagerState.stateDetail.readSramState.pDataRead);
                    NCI_MANAGER_LOG("E_NCI_SRAM_READ_STATE_POLL_SRAM_RF_READY_ON ready on\r\n");
                }
                else
                {
                    NCI_MANAGER_LOG("E_NCI_SRAM_READ_STATE_POLL_SRAM_RF_READY_ON not ready on => retry\r\n");
                }
                
            }
            else if (nciManagerHandle.nciManagerState.stateDetail.readSramState.stateType 
                    == E_NCI_SRAM_READ_STATE_DATA_READ)
            {
                nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_IDLE;
                NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_READ_OK, nciManagerHandle.nciManagerState.stateDetail.readSramState.pDataRead, nciManagerHandle.nciManagerState.stateDetail.readSramState.dataLen);
                NCI_MANAGER_LOG("E_NCI_SRAM_READ_STATE_DATA_READ\r\n");
            }
            break;
        }
        case E_NCI_EVENT_WRITE_FAIL:
        case E_NCI_EVENT_READ_FAIL:
        {
            NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_READ_FAIL, NULL, 0);
            NCI_MANAGER_LOG("Event = %d\r\n", eNciEvent);
            break;
        }
        case E_NCI_EVENT_ABSENT:
        {
            NCI_ManagerProcessEventAbsent();
            break;
        }
        default:
            break;
    }
}

static void NCI_ManagerProcessIdleStateMachine(teNciEvent eNciEvent, uint32_t u32Length, uint8_t *pu8Data)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    switch (eNciEvent)
    {
        case E_NCI_EVENT_ABSENT:
        {
            NCI_ManagerProcessEventAbsent();
            NCI_MANAGER_LOG("E_NCI_EVENT_ABSENT\r\n");
            break;
        }
        case E_NCI_EVENT_READ_FAIL: 
        {
            NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_READ_FAIL, NULL, 0);
            break;
        }
        default:
            NCI_MANAGER_LOG("Event = %d\r\n", eNciEvent);
            break;
    }    
}

static void NCI_ManagerSetHandleAppEvent(eNciManagerAppEventType eventType, uint8_t *pdata, uint32_t len)
{
    nciManagerHandle.appEvent.eventType = eventType;
    nciManagerHandle.appEvent.pData = pdata;
    nciManagerHandle.appEvent.dataLen = len;    
}

static void NCI_ManagerProcessEventPresent(void)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    if (!nciManagerHandle.ntagInField)
    {
        nciManagerHandle.ntagInField = TRUE;
        nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_INIT;
        nciManagerHandle.nciManagerState.stateDetail.initState = E_NCI_INIT_STATE_READ_HEADER;
        NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_EVENT_PRESENT, NULL, 0);
        /* Post a synchrous event to the app */
        nciManagerHandle.appProcessEventCallback(&nciManagerHandle.appEvent);
        NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_NO_EVENT, NULL, 0);
        NCI_MANAGER_LOG("E_NCI_EVENT_PRESENT\r\n");
    }
}

static void NCI_ManagerProcessEventAbsent(void)
{
    NCI_MANAGER_LOG("%s\r\n", __FUNCTION__);
    if (nciManagerHandle.ntagInField)
    {
        nciManagerHandle.ntagInField = FALSE;
        nciManagerHandle.nciManagerState.stateType = E_NCI_STATE_INIT;
        nciManagerHandle.nciManagerState.stateDetail.initState = E_NCI_INIT_STATE_READ_HEADER;
        NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_EVENT_ABSENT, NULL, 0);
        /* Post a synchrous event to the app */
        nciManagerHandle.appProcessEventCallback(&nciManagerHandle.appEvent);
        NCI_ManagerSetHandleAppEvent(E_NCI_MANAGER_NO_EVENT, NULL, 0);
        NCI_MANAGER_LOG("E_NCI_EVENT_ABSENT\r\n");
    }
}