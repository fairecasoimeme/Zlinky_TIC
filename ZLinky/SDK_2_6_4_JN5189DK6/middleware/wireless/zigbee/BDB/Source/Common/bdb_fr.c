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
# COMPONENT:   bdb_failure_recovery.c
#
# DESCRIPTION: BDB failure recovery implementation 
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "bdb_api.h"
#include "bdb_start.h"
#include "dbg.h"
#include "PDM.h"
#include <string.h>
#include <stdlib.h>
#if (defined BDB_SUPPORT_NWK_STEERING)
#include "bdb_ns.h"
#endif
#include "bdb_fr.h"

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

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
teFR_State eFR_State = E_FR_IDLE;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_vFrStateMachine
 *
 * DESCRIPTION:
 * Upon Poll Failure on ZED or Leave with Rejoin on ZED/ZR, stack will
 * kick off a rejoin internally.
 * 1. If this rejoin was successful BDB generates BDB_EVENT_REJOIN_SUCCESS
 *    event to application.
 * 2. If this rejoin was failure BDB enters into failure recovery state
 *    and generates BDB_EVENT_FAILURE_RECOVERY_FOR_REJOIN event to application.
 *    In failure recovery state, BDB performs rejoin cycles same as in
 *    BDB_vStart.
 *    Number of rejoin cycles are controlled by BDBC_IMP_MAX_REJOIN_CYCLES
 *    macro defined in bdb_options.h file in application.
 *
 *    a. If the rejoin was success BDB_EVENT_REJOIN_SUCCESS is generated.
 *    b. If the rejoin was unsuccessful BDB_EVENT_REJOIN_FAILURE is generated.
 *
 * RETURNS:
 *
 * NOTES:
 *
 ****************************************************************************/
PUBLIC void BDB_vFrStateMachine(BDB_tsZpsAfEvent *psZpsAfEvent)
{
    BDB_tsBdbEvent sBdbEvent;

    if((eNS_State != E_NS_IDLE) || (eInitState != E_INIT_IDLE))
    {
        return;
    }

    switch(eFR_State)
    {
        case E_FR_IDLE:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_NWK_FAILED_TO_JOIN:
                    DBG_vPrintf(TRACE_BDB,"BDB: FR Starting rejoin cycles!\n");
                    eFR_State = E_FR_RECOVERY;
                    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;

                    sBdbEvent.eEventType = BDB_EVENT_FAILURE_RECOVERY_FOR_REJOIN;
                    APP_vBdbCallback(&sBdbEvent);

                    BDB_vRejoinCycle(TRUE);
                    break;

                case ZPS_EVENT_NWK_JOINED_AS_ROUTER:
                case ZPS_EVENT_NWK_JOINED_AS_ENDDEVICE:
                    DBG_vPrintf(TRACE_BDB,"BDB: FR Rejoined --- \n");
                    BDB_vRejoinSuccess();
                    break;
                default:
                    break;
            }
            break;

        case E_FR_RECOVERY:
            switch(psZpsAfEvent->sStackEvent.eType)
            {
                case ZPS_EVENT_NWK_FAILED_TO_JOIN:
                    BDB_vRejoinCycle(TRUE);
                    break;
                case ZPS_EVENT_NWK_JOINED_AS_ENDDEVICE:
                case ZPS_EVENT_NWK_JOINED_AS_ROUTER:
                    DBG_vPrintf(TRACE_BDB,"BDB: FR Rejoined --- \n");
                    BDB_vRejoinSuccess();
                    break;
                default:
                    break;
            }
            break;

        case E_FR_GIVE_UP:
            break;

        default:
            break;
    }
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
