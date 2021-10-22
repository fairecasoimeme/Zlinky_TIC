/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _APP_SRAM_CMD_H_
#define _APP_SRAM_CMD_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"

/*! *********************************************************************************
*************************************************************************************
* Public Macros
*************************************************************************************
********************************************************************************** */
#define TLV_TAG_AND_TLV_LEN_SIZE 2

/*! *********************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
********************************************************************************** */


typedef enum
{
    E_TLV_TAG_CMD_GET_DEVICE_NAME_REQUEST = 0xa0,
    E_TLV_TAG_CMD_PUT_PROVISIONING_DATA_REQUEST = 0xa1,
} eTlvCmdTag;

typedef enum
{
    E_APP_SRAM_CMD_STATE_WAITING_PRESENCE,
    E_APP_SRAM_CMD_STATE_WAITING_CMD,
    E_APP_SRAM_CMD_STATE_RECEIVING_CMD,
    E_APP_SRAM_CMD_STATE_SENDING_CMD   
} eAppSramCmdStateType;

typedef struct
{
    eTlvCmdTag tlvTag;
    uint8_t cmdLen;
    uint8_t cmdLenSent;
    const uint8_t *pData;
} sAppSramCmdToSend_t;

typedef struct
{
    eTlvCmdTag tlvTag;
    uint8_t cmdLen;
    uint8_t cmdLenReceived;
    void (*process)(const uint8_t*, uint8_t, bool_t);
} sAppSramCmdToReceive_t;

typedef sAppSramCmdToReceive_t* (*getCmdToReceiveContextCallback)(uint8_t tlvTag, uint8_t tlvLen);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

void app_sram_cmd_send(sAppSramCmdToSend_t *pCmdToSend, 
                        uint8_t *pSramBuffer, 
                        uint8_t sramBufferSize,
                        uint8_t *pAppCmdState, 
                        bool_t (*sramWriteFunction)(uint8_t *, uint32_t));

void app_sram_cmd_receive(sAppSramCmdToReceive_t **ppCmdToReceive, 
                            getCmdToReceiveContextCallback appCallback, 
                            uint8_t *pSramBuffer, 
                            uint8_t sramBufferSize,
                            uint8_t *pAppCmdState, 
                            bool_t (*sramReadFunction)(uint8_t *, uint32_t));
#endif /* _APP_SRAM_CMD_H_ */
