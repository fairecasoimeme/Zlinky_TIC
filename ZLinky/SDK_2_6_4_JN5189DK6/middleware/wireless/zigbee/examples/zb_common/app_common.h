/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#include "EmbeddedTypes.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define NETWORK_RESTART_TIME    ZTIMER_TIME_MSEC(1000)
#define POLL_TIME               ZTIMER_TIME_MSEC(5000)
#define POLL_TIME_FAST          ZTIMER_TIME_MSEC(250)
#define GP_ZCL_TICK_TIME        ZTIMER_TIME_MSEC(1)


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
	APP_E_EVENT_NONE,
	APP_E_EVENT_BUTTON_DOWN,
	APP_E_EVENT_BUTTON_UP,
	APP_E_EVENT_SERIAL_TOGGLE,
	APP_E_EVENT_SERIAL_NWK_STEER,
	APP_E_EVENT_SERIAL_FIND_BIND_START,
	APP_E_EVENT_SERIAL_FORM_NETWORK
} teAppEvents;

typedef enum
{
    E_STARTUP,
    E_RUNNING
} teNodeState;

typedef struct
{
    teNodeState     eNodeState;
}tsDeviceDesc;

typedef struct
{
    uint8_t u8Button;
    uint32_t u32DIOState;
} APP_tsEventButton;

typedef struct
{
	teAppEvents eType;
    union
    {
        APP_tsEventButton            sButton;
    }uEvent;
} APP_tsEvent;

/* Out Of Band Commissioning */
#define ZB_OOB_KEY_SIZE 16

PACKED_STRUCT dev_info
{
    uint64_t addr;
    uint8_t instCode[ZB_OOB_KEY_SIZE];
    uint16_t crc;
};

PACKED_STRUCT oob_info {
    uint8_t key[ZB_OOB_KEY_SIZE];
    uint64_t tcAddress;
    uint64_t panId;
    uint16_t shortPanId;
    uint8_t activeKeySeq;
    uint8_t channel;
};

PACKED_STRUCT oob_info_enc {
    struct oob_info info;
    uint32_t mic;
};

PACKED_STRUCT bdb_oob_enc {
    uint64_t addr;
    uint8_t key[ZB_OOB_KEY_SIZE];
    uint32_t mic;
    uint64_t tcAddress;
    uint8_t activeKeySeq;
    uint8_t channel;
    uint16_t shortPanId;
    uint64_t panId;
};

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void APP_GetInstallCode(uint8_t instCode[ZB_OOB_KEY_SIZE]);
void APP_GetDeviceInfo(struct dev_info* info);

bool_t APP_GetOOBInfo(struct dev_info* info, struct oob_info_enc* oob);
bool_t APP_SetOOBInfo(struct oob_info_enc* oob, bool_t enc);
void APP_ClearOOBInfo();

bool_t APP_Start_BDB_OOB();

/* Must be called before zps_eAplAfInit() */
void APP_SetHighTxPowerMode();

/* Must be called after zps_eAplAfInit() */
void APP_SetMaxTxPower();

#undef HIGH_TX_PWR_LIMIT
#define HIGH_TX_PWR_LIMIT 15	/* dBm */

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
uint8_t APP_u8GetDeviceEndpoint(void);
teNodeState APP_eGetCurrentApplicationState (void);
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_COMMON_H_*/
