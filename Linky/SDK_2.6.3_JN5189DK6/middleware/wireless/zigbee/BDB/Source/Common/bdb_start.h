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
# MODULE:      BDB
#
# COMPONENT:   bdb_start.h
#
# DESCRIPTION: BDB Network Initialisation API
#
#
###############################################################################*/

#ifndef BDB_START_INCLUDED
#define BDB_START_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "bdb_api.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_INIT_IDLE,
    E_INIT_WAIT_REJOIN,
}teInitState;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void BDB_vInit(BDB_tsInitArgs *psInitArgs);
PUBLIC bool_t BDB_bIsBaseIdle(void);
PUBLIC void BDB_vStart(void);
PUBLIC void BDB_vInitStateMachine(BDB_tsZpsAfEvent *psZpsAfEvent);
PUBLIC uint8 BDB_u8PickChannel(uint32 u32ChannelMask);
PUBLIC void BDB_vRejoinCycle(bool_t bSkipDirectJoin);
PUBLIC void BDB_vRejoinSuccess(void);
PUBLIC void BDB_vRejoinTimerCb(void *pvParam);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC uint8 au8DefaultTCLinkKey[16];
extern PUBLIC uint8 au8DistributedLinkKey[16];
extern PUBLIC uint8 au8PreConfgLinkKey[16];
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
extern PUBLIC tsReg128 sTLMasterKey;
extern PUBLIC tsReg128 sTLCertKey;
#else
extern PUBLIC uint8 au8TLMasterKey[16];
extern PUBLIC uint8 au8TLCertKey[16];
#endif

extern PUBLIC teInitState eInitState;
extern PUBLIC bool_t bAssociationJoin;
extern PUBLIC uint8 u8RejoinCycles;
extern PUBLIC uint8 u8TimerBdbRejoin;

#if defined __cplusplus
}
#endif

#endif  /* BDB_START_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/






