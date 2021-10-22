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
# COMPONENT:   bdb_ns.h
#
# DESCRIPTION: BDB NetworkSteering API 
#              
#
###############################################################################*/

#ifndef BDB_NS_INCLUDED
#define BDB_NS_INCLUDED

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
    E_NS_IDLE,
    E_NS_WAIT_DISCOVERY,
    E_NS_WAIT_JOIN,
    E_NS_TCLK_WAIT_NODE_DESC_RES,
    E_NS_WAIT_AFTER_NWK_JOIN,
    E_NS_TCLK_WAIT_SEND_REQ_KEY,
    E_NS_TCLK_WAIT_SEND_VER_KEY,
    E_NS_TCLK_WAIT_LEAVE_CFM,
}teNS_State;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC BDB_teStatus BDB_eNsStartNwkSteering(void);
PUBLIC void BDB_vNsStateMachine(BDB_tsZpsAfEvent *psZpsAfEvent);
PUBLIC void BDB_vNsTimerCb(void *pvParam);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC teNS_State eNS_State;
extern PUBLIC uint8 u8TimerBdbNs;

#if defined __cplusplus
}
#endif

#endif  /* BDB_NS_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/






