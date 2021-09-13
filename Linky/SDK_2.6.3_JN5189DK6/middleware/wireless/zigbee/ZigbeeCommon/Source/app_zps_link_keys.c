/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/


/*###############################################################################
#
# MODULE:      ZigbeeCommon
#
# COMPONENT:   app_zps_link_keys.c
#
# DESCRIPTION: application link keys definitions 
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "zps_apl_af.h"
#include "zps_apl_aib.h"
#include <string.h>
#include <stdlib.h>
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/* Distributed security global link key */
PUBLIC uint8 au8DistributedLinkKey[16]  = {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
                                           0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf};


/* Touchlink preconfigured link keys*/
#ifndef BDB_APPLICATION_DEFINED_TL_MASTER_KEY
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
PUBLIC PUBLIC tsReg128 sTLMasterKey = {0x11223344, 0x55667788, 0x99aabbcc, 0xddeeff00 };
#else
PUBLIC uint8 au8TLMasterKey[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00};
#endif
#else
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
extern PUBLIC tsReg128 sTLMasterKey;
#else
extern PUBLIC uint8 au8TLMasterKey[16];
#endif
#endif
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
PUBLIC tsReg128 sTLCertKey = {0xc0c1c2c3, 0xc4c5c6c7,0xc8c9cacb,0xcccdcecf};
#else
PUBLIC uint8 au8TLCertKey[16] = {0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf};
#endif


PUBLIC uint8* ZPS_pau8GetBdbDistributedLinkKey (void)
{
    return &au8DistributedLinkKey[0];
}
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
