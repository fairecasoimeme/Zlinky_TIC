/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "EmbeddedTypes.h"
#include "dbg.h"
#include <string.h>
#include "ota.h"
#include "app_ota_server.h"
#include "app_common.h"
#include "zps_gen.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_APP_OTA
    #define TRACE_APP_OTA               TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
bool_t bImagePresent = FALSE;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vAppInitOTA
 *
 * DESCRIPTION:
 * Initialises application OTA client related data structures and calls
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

void vAppInitOTA(void)
{
    tsNvmDefs sNvmDefs;
    teZCL_Status eZCL_Status;

    sNvmDefs.u8FlashDeviceType = E_FL_CHIP_INTERNAL;
    sNvmDefs.u32SectorSize = 512; /* Sector Size = 512 bytes*/
    /* Each Page on JN518x is 512 bytes , Flash size is 632K. Taking into account 31.5K for PDM (start page 1153) and 24K for customer data
     * usable for image is 576K
     * Split it into 2 sections to support OTA, so 288K becomes Max image size
     * Each 288K section would be 576 pages. This could be represented as 32K sectors to keep in line with legacy devices.*/
    uint8_t u8StartSector[1] = {9}; /* So next image starts at 9*32*1024 = 288K offset*/
    uint8_t u8MaxSectorPerImage = 9 ;  /* 9 *32* 1024 = 288K is the maximum size of the image */

    vOTA_FlashInit(NULL,&sNvmDefs);

    eZCL_Status = eOTA_AllocateEndpointOTASpace(APP_u8GetDeviceEndpoint(), u8StartSector, 1, u8MaxSectorPerImage, TRUE,NULL);
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_APP_OTA, "eAllocateEndpointOTASpace returned error 0x%x\n", eZCL_Status);
    }

    eZCL_Status = eOTA_NewImageLoaded(APP_u8GetDeviceEndpoint(),FALSE,NULL);
    DBG_vPrintf(TRACE_APP_OTA, "\neOTA_NewImageLoaded status = %d\n", eZCL_Status);
    if(eZCL_Status == E_ZCL_SUCCESS)
    {
    	bImagePresent = TRUE;
    }
    eZCL_Status = eOTA_SetServerAuthorisation(APP_u8GetDeviceEndpoint(), E_CLD_OTA_STATE_ALLOW_ALL,NULL, 0);
    if (eZCL_Status != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_APP_OTA, "eSetServerAuthorisation returned error 0x%x\n", eZCL_Status);
    }

    if(bImagePresent)
    {
        APP_vSendImageNotification();
    }
}


void APP_vSendImageNotification(void)
{

    tsOTA_ImageNotifyCommand   sImageNotifyCommand;
    tsZCL_Address              sDestinationAddress;
    sDestinationAddress.eAddressMode = E_ZCL_AM_BROADCAST;
    sDestinationAddress.uAddress.u16DestinationAddress = 0xffff;
    sImageNotifyCommand.ePayloadType = E_CLD_OTA_QUERY_JITTER;
    sImageNotifyCommand.u8QueryJitter=100;
    if(APP_eGetCurrentApplicationState() == E_RUNNING  && bImagePresent)
    {
        eOTA_ServerImageNotify(APP_u8GetDeviceEndpoint(),0xff,&sDestinationAddress,&sImageNotifyCommand);
    }
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
