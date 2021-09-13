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

#include "app_sram_cmd.h"
#include "FunctionLib.h"

/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
//#include "fsl_debug_console.h"
#define APP_SRAM_CMD_LOG(...) //PRINTF("[APP] "); PRINTF(__VA_ARGS__);

/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/


/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/

void app_sram_cmd_send(sAppSramCmdToSend_t *pCmdToSend, 
                        uint8_t *pSramBuffer, 
                        uint8_t sramBufferSize,
                        uint8_t *pAppCmdState, 
                        bool_t (*sramWriteFunction)(uint8_t *, uint32_t))
{
    APP_SRAM_CMD_LOG("%s\r\n", __FUNCTION__);
    uint8_t lenToWrite = 0;
    /* Do we need to send a response ? */
    if (pCmdToSend->cmdLenSent < pCmdToSend->cmdLen)
    {
        *pAppCmdState = E_APP_SRAM_CMD_STATE_SENDING_CMD;
        FLib_MemSet(pSramBuffer, 0x0, sramBufferSize);
        if (pCmdToSend->cmdLen > sramBufferSize)
        {
            if(pCmdToSend->cmdLen-pCmdToSend->cmdLenSent >= sramBufferSize)
            {
                lenToWrite = sramBufferSize;
            }
            else
            {
                lenToWrite = pCmdToSend->cmdLen-pCmdToSend->cmdLenSent;
            }
        }
        else
        {
            lenToWrite = pCmdToSend->cmdLen;
        }
        /* Is it our first write of this cmd ? */
        if (pCmdToSend->cmdLenSent == 0)
        {
            /* Put the tlv tag and the data len in the sram buffer */
            pSramBuffer[0] = pCmdToSend->tlvTag;
            pSramBuffer[1] = pCmdToSend->cmdLen-TLV_TAG_AND_TLV_LEN_SIZE;
            FLib_MemCpy(&pSramBuffer[2], pCmdToSend->pData, lenToWrite-TLV_TAG_AND_TLV_LEN_SIZE);
        }
        else
        {
            FLib_MemCpy(&pSramBuffer[0], pCmdToSend->pData+pCmdToSend->cmdLenSent-TLV_TAG_AND_TLV_LEN_SIZE, lenToWrite);
        }
        pCmdToSend->cmdLenSent += lenToWrite;
        APP_SRAM_CMD_LOG("Sending ... lenToWrite = %d, cmdLenSent = %d cmdLen=%d\r\n", lenToWrite, pCmdToSend->cmdLenSent, pCmdToSend->cmdLen);
        sramWriteFunction(&pSramBuffer[0], sramBufferSize);

    }
    else
    {
        APP_SRAM_CMD_LOG("move to E_APP_SRAM_CMD_STATE_WAITING_CMD\r\n");
        *pAppCmdState = E_APP_SRAM_CMD_STATE_WAITING_CMD;
    }
}

void app_sram_cmd_receive(sAppSramCmdToReceive_t **ppCmdToReceive, 
                            getCmdToReceiveContextCallback appCallback, 
                            uint8_t *pSramBuffer, 
                            uint8_t sramBufferSize,
                            uint8_t *pAppCmdState, 
                            bool_t (*sramReadFunction)(uint8_t *, uint32_t))
{
    APP_SRAM_CMD_LOG("%s\r\n", __FUNCTION__);
    switch (*pAppCmdState)
    {
        case E_APP_SRAM_CMD_STATE_WAITING_CMD:
            /* Is it a know command ? */
            *ppCmdToReceive = appCallback(pSramBuffer[0], pSramBuffer[1]);
            if (*ppCmdToReceive != NULL)
            {
                /* Do we need to wait for more bytes ? */
                if ((*ppCmdToReceive)->cmdLen > sramBufferSize)
                {
                    (*ppCmdToReceive)->cmdLenReceived += sramBufferSize;
                    (*ppCmdToReceive)->process(pSramBuffer+TLV_TAG_AND_TLV_LEN_SIZE, sramBufferSize-TLV_TAG_AND_TLV_LEN_SIZE, FALSE);
                    *pAppCmdState = E_APP_SRAM_CMD_STATE_RECEIVING_CMD;
                    sramReadFunction(pSramBuffer, sramBufferSize);
                }
                else
                {
                    (*ppCmdToReceive)->cmdLenReceived = (*ppCmdToReceive)->cmdLen;
                    /* Process the command */
                    (*ppCmdToReceive)->process(pSramBuffer+TLV_TAG_AND_TLV_LEN_SIZE, (*ppCmdToReceive)->cmdLen-TLV_TAG_AND_TLV_LEN_SIZE, TRUE);
                    (*ppCmdToReceive) = NULL;
                }
            }
            else
            {
                /* Drop it and trigger a new read*/
                sramReadFunction(pSramBuffer, sramBufferSize);
            }
            break;
        case E_APP_SRAM_CMD_STATE_RECEIVING_CMD:
            /* Did we receive the full command ? */
            if ((*ppCmdToReceive)->cmdLenReceived + sramBufferSize >= (*ppCmdToReceive)->cmdLen)
            {
                /* Process the command */
                (*ppCmdToReceive)->process(pSramBuffer, (*ppCmdToReceive)->cmdLen-(*ppCmdToReceive)->cmdLenReceived, TRUE);
                (*ppCmdToReceive) = NULL;
            }
            else
            {
                (*ppCmdToReceive)->process(pSramBuffer, sramBufferSize, FALSE);
                sramReadFunction(pSramBuffer, sramBufferSize);
            }
            break;
        default:
            break;
    }
}