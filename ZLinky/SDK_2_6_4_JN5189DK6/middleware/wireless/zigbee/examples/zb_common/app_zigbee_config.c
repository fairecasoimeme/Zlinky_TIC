/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "zigbee_config.h"
#include "ZQueue.h"
#include "bdb_api.h"
#include "ZTimer.h"
#include "zps_apl_af.h"
#include "pdum_gen.h"
#include "app_zcl_task.h"
#include "app_common.h"
#include "fsl_rng.h"
#include "bdb_DeviceCommissioning.h"

uint8_t u8TimerZCL;

tszQueue APP_msgBdbEvents;


/****************************************************************************
*
* NAME: APP_vStopZigbeeTimers
*
* DESCRIPTION:
*
*
* RETURNS:
* Never
*
****************************************************************************/
void APP_vStopZigbeeTimers(void)
{
    ZTIMER_eStop(u8TimerZCL);
}


/****************************************************************************
 *
 * NAME: APP_vRunZigbee
 *
 * DESCRIPTION:
 * Main  execution loop
 *
 * RETURNS:
 * Never
 *
 ****************************************************************************/
void APP_vRunZigbee(void)
{
		zps_taskZPS();
        bdb_taskBDB();        
}

/****************************************************************************
 *
 * NAME: APP_vInitZigbeeResources
 *
 * DESCRIPTION:
 * Main  execution loop
 *
 * RETURNS:
 * Never
 *
 ****************************************************************************/
void APP_vInitZigbeeResources(void)
{
    ZTIMER_eOpen(&u8TimerZCL,           APP_cbTimerZclTick ,    NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZQ_vQueueCreate(&APP_msgBdbEvents,        BDB_QUEUE_SIZE,          sizeof(BDB_tsZpsAfEvent),    NULL);    
    ZQ_vQueueCreate(&zps_msgMlmeDcfmInd,      MLME_QUEQUE_SIZE,        sizeof(MAC_tsMlmeVsDcfmInd), NULL);
    ZQ_vQueueCreate(&zps_msgMcpsDcfmInd,      MCPS_QUEUE_SIZE,         sizeof(MAC_tsMcpsVsDcfmInd), NULL);
    ZQ_vQueueCreate(&zps_msgMcpsDcfm,         MCPS_DCFM_QUEUE_SIZE,    sizeof(MAC_tsMcpsVsCfmData), NULL);
    ZQ_vQueueCreate(&zps_TimeEvents,          TIMER_QUEUE_SIZE,        sizeof(zps_tsTimeEvent),     NULL);
    PDUM_vInit();
}

/* Out Of Band Commissioning */
static struct dev_info app_dev_info;
static bool_t valid_dev_info;

static BDB_tsOobWriteDataToCommission bdb_oob;
static uint8_t nwk_key[ZB_OOB_KEY_SIZE];
static bool_t valid_oob_info;

bool bGetInstallCode(uint8 install_code[16]);

static void APP_dev_info_init()
{
    if (!valid_dev_info)
    {
        app_dev_info.addr = ZPS_u64AplZdoGetIeeeAddr();
        if (!bGetInstallCode(app_dev_info.instCode))
        {
            trng_config_t trng_config;

            /* Generate random install code */
            TRNG_GetDefaultConfig(&trng_config);
            TRNG_Init(RNG, &trng_config);
            TRNG_GetRandomData(RNG, app_dev_info.instCode, ZB_OOB_KEY_SIZE);
            TRNG_Deinit(RNG);
        }
        app_dev_info.crc = ZPS_u16crc(app_dev_info.instCode, ZB_OOB_KEY_SIZE);
        valid_dev_info = TRUE;
    }
}

void APP_GetInstallCode(uint8_t instCode[ZB_OOB_KEY_SIZE])
{
	APP_dev_info_init();
    memcpy(instCode, app_dev_info.instCode, ZB_OOB_KEY_SIZE);
}

void APP_GetDeviceInfo(struct dev_info* info)
{
	if (!info)
	    return;

	APP_dev_info_init();
    info->addr = app_dev_info.addr;
    APP_GetInstallCode(info->instCode);
    info->crc = app_dev_info.crc;
}

bool_t APP_GetOOBInfo(struct dev_info* info, struct oob_info_enc* oob)
{
    if (!sBDB.sAttrib.bbdbNodeIsOnANetwork)
        return FALSE;

    if (!oob)
        return FALSE;

    if (info)
    {
    	BDB_tsOobWriteDataToAuthenticate auth;
    	struct bdb_oob_enc tmp;
    	uint16_t size;
    	uint16_t crc = ZPS_u16crc(info->instCode, ZB_OOB_KEY_SIZE);

        if (crc != info->crc)
            return FALSE;

        auth.u64ExtAddr = info->addr;
        auth.pu8InstallCode = info->instCode;

        if (BDB_eOutOfBandCommissionGetDataEncrypted(&auth, (uint8*)&tmp, &size)
            != BDB_E_SUCCESS)
            return FALSE;

        memcpy(oob->info.key, tmp.key, ZB_OOB_KEY_SIZE);
        oob->info.tcAddress = tmp.tcAddress;
        oob->info.panId = tmp.panId;
        oob->info.shortPanId = tmp.shortPanId;
        oob->info.activeKeySeq = tmp.activeKeySeq;
        oob->info.channel = tmp.channel;
        oob->mic = tmp.mic;
    }
    else
    {
    	BDB_tsOobReadDataToAuthenticate tmp;

        BDB_vOutOfBandCommissionGetData(&tmp);

        memcpy(oob->info.key, tmp.au8Key, ZB_OOB_KEY_SIZE);
        oob->info.tcAddress = tmp.u64TcAddress;
        oob->info.panId = tmp.u64PanId;
        oob->info.shortPanId = tmp.u16ShortPanId;
        oob->info.activeKeySeq = tmp.u8ActiveKeySeq;
        oob->info.channel = tmp.u8Channel;
    }
    return TRUE;
}

bool_t APP_SetOOBInfo(struct oob_info_enc* oob, bool_t enc)
{
    if (sBDB.sAttrib.bbdbNodeIsOnANetwork)
        return FALSE;

    if (!oob)
        return FALSE;

    if (enc)
    {
        APP_dev_info_init();
    	if (!BDB_bOutOfBandCommissionGetKey(app_dev_info.instCode,
            oob->info.key, app_dev_info.addr, nwk_key, (uint8*)&oob->mic))
            return FALSE;
    }
    else
    {
        memcpy(nwk_key, oob->info.key, ZB_OOB_KEY_SIZE);
    }

    bdb_oob.u64PanId = oob->info.panId;
    bdb_oob.u64TrustCenterAddress = oob->info.tcAddress;
    bdb_oob.pu8NwkKey = nwk_key;
    bdb_oob.u16PanId = oob->info.shortPanId;
    bdb_oob.u8ActiveKeySqNum = oob->info.activeKeySeq;
    bdb_oob.u8Channel = oob->info.channel;
    bdb_oob.pu8InstallCode = (uint8*)NULL;
    bdb_oob.u16ShortAddress = 0xFFFF;
    bdb_oob.u8DeviceType = ZPS_eAplZdoGetDeviceType();
    bdb_oob.u8NwkUpdateId = 1;

    if (bdb_oob.u8DeviceType == ZPS_ZDO_DEVICE_ENDDEVICE)
    {
        /* Use ED settings */
    	bdb_oob.bRejoin          = TRUE;
    	bdb_oob.u8RxOnWhenIdle   = FALSE;
    }
    else
    {
        /* Use router settings */
    	bdb_oob.bRejoin          = FALSE;
    	bdb_oob.u8RxOnWhenIdle   = TRUE;
    }

    valid_oob_info = TRUE;
    return TRUE;
}

void APP_ClearOOBInfo()
{
    valid_oob_info = FALSE;
}

bool_t APP_Start_BDB_OOB()
{
    if (!valid_oob_info)
        return FALSE;

    BDB_u8OutOfBandCommissionStartDevice(&bdb_oob);
    return TRUE;
}

/* High Tx power */
void APP_SetHighTxPowerMode()
{
	if (CHIP_IS_HITXPOWER_CAPABLE())
		vMMAC_SetTxPowerMode(TRUE);
}

void APP_SetMaxTxPower()
{
	if (CHIP_IS_HITXPOWER_CAPABLE())
		ZPS_eMacPlmeSet(PHY_PIB_ATTR_TX_POWER, HIGH_TX_PWR_LIMIT);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
