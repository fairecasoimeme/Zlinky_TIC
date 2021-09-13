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
# COMPONENT:   bdb_fb_target.c
#
# DESCRIPTION: BDB Find & Bind Target Implementation
#              
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "bdb_fb_api.h"
#include "dbg.h"
#include <string.h>
#include "Identify.h"
#include "bdb_api.h"
#include "Groups_internal.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_FB_TARGET
#define TRACE_FB_TARGET FALSE
#else
#define TRACE_FB_TARGET TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE BDB_teStatus eFbStartFbTarget(uint8 u8EndPoint);
                                       
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE teFB_State aeFBState[ZCL_NUMBER_OF_ENDPOINTS];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: BDB_eFbTriggerAsTarget
 *
 * DESCRIPTION:
 *  Starts the finding & binding procedure for target endpoint supplied.
 *  This is done by putting the device into identify mode for
 *  bdbcMinCommissioningTime(BDBC_MIN_COMMISSIONING_TIME). During this time,
 *  this Find & Bind target device will generate responses to identify query
 *  and other find and bind commands.
 *
 *  Find & Bind initiator node may ask to stop the Find & Bind target process
 *  on this node by sending Identify cluster's StopIdenitify command.
 *  Hence, when local attribute u16IdentifyTime of IdentifySeverCluser becomes
 *  zero, application must call BDB_vZclEventHandler with tsBDB_ZCLEvent
 *  eType as BDB_E_ZCL_EVENT_IDENTIFY and pointer to tsZCL_CallBackEvent.
 *  This will help Base device to terminate the Find & Bind process as Target.
 *
 * PARAMETERS:  Name                Usage
 *              u8Endpoint          Local Endpoint for F&B Target
 * RETURNS:
 *  BDB_E_FAILURE,          if endpoint supplied is invalid
 *                          if could not write Identify cluster, IdentifyTime
 *                          attribute locally.
 *  BDB_E_SUCCESS, if F&B process as target EP is ongoing
 ****************************************************************************/
PUBLIC BDB_teStatus BDB_eFbTriggerAsTarget(uint8 u8EndPoint)
{
    if( u8EndPoint > BDB_FB_NUMBER_OF_ENDPOINTS )
    {
        DBG_vPrintf(TRACE_FB_TARGET, "BDB_eFbTriggerAsTarget E_ZCL_ERR_EP_RANGE \n");
        return BDB_E_FAILURE;
    }

    return eFbStartFbTarget(u8EndPoint);
}

/****************************************************************************
 *
 * NAME:       BDB_vFbHandleStopIdentification
 *
 * DESCRIPTION:
 *  This function is used to stop the Find and Bind procedure as target when
 *  identify time on local node reaches zero.
 *
 * PARAMETERS:                 Name               Usage
 * tsZCL_CallBackEvent         pCallBackEvent     ZCL call back structure
 *
 * RETURN:
 * None
 *
 ****************************************************************************/
PUBLIC void BDB_vFbHandleStopIdentification(tsZCL_CallBackEvent *pCallBackEvent)
{
    BDB_tsBdbEvent sBdbEvent = {0};

    DBG_vPrintf(TRACE_FB_TARGET, "BDB_vFbHandleStopIdentification \r\n");
    
    if(E_FB_IN_PROGRESS_STATE == aeFBState[pCallBackEvent->u8EndPoint - 1])
    {
        /*Step 4: Change BDB status into Success */
        sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;  
        /* clear state variables */
        aeFBState[pCallBackEvent->u8EndPoint - 1] =  E_FB_IDLE_STATE;

        /* callback to app */
        sBdbEvent.eEventType = BDB_EVENT_FB_OVER_AT_TARGET;
        APP_vBdbCallback(&sBdbEvent);
    }
}

/****************************************************************************
 *
 * NAME: BDB_vFbExitAsTarget
 *
 * DESCRIPTION:
 *  This function explicitly stops Find and Bind process as target node.
 *  Classic example would be upon application Find and Bind button release,
 *  this API can be used to stop on going Find & Bind process as target.
 *
 * PARAMETERS:  Name                            Usage
 *              u8SourceEndpoint                Local end point for F&B Target.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void BDB_vFbExitAsTarget(uint8 u8SourceEndpoint)
{
    DBG_vPrintf(TRACE_FB_TARGET, "BDB_FB_vExit  \n");
    /* clear state variables */
    uint16 u16IdentifyTime = 0;
    DBG_vPrintf(TRACE_FB_TARGET, "vEZ_Exit  \n");

    eZCL_WriteLocalAttributeValue(
                        u8SourceEndpoint,
                        GENERAL_CLUSTER_ID_IDENTIFY,
                        TRUE,
                        FALSE,
                        FALSE,
                        E_CLD_IDENTIFY_ATTR_ID_IDENTIFY_TIME,
                        &u16IdentifyTime);

    /* clear state variables */
    aeFBState[u8SourceEndpoint - 1] = E_FB_IDLE_STATE;
        
    /*Step 4: Change BDB status into Success */
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_SUCCESS;  
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eFbStartFbTarget
 *
 * DESCRIPTION:
 * Function to write Identify Cluster, IdentifyTime attribute locally.
 *
 * PARAMETERS:             Name                Usage
 * uint8                   u8EndPoint  Find and Bind target EndPoint.
 *
 * RETURN:
 *  BDB_E_FAILURE,                  if couldn't write local IdentifyTime.
 *  BDB_E_SUCCESS
 *
 ****************************************************************************/
PRIVATE BDB_teStatus eFbStartFbTarget(uint8 u8EndPoint)
{
    uint16 u16FBCommissioningTime = BDBC_MIN_COMMISSIONING_TIME;
    /* clear state variables */
    aeFBState[u8EndPoint - 1] = E_FB_IDLE_STATE;
    
    /* Step 1 of 8.5: Change BDB status into progress */
    sBDB.sAttrib.ebdbCommissioningStatus = E_BDB_COMMISSIONING_STATUS_IN_PROGRESS;  
    
    /* Step 2 of 8.5 : put in identify mode */
    ZPS_teStatus eZCLStatus =  eZCL_WriteLocalAttributeValue(
                                    u8EndPoint,
                                    GENERAL_CLUSTER_ID_IDENTIFY,
                                    TRUE,
                                    FALSE,
                                    FALSE,
                                    E_CLD_IDENTIFY_ATTR_ID_IDENTIFY_TIME,
                                    &u16FBCommissioningTime);
                
    if(eZCLStatus)
    {
        DBG_vPrintf(TRACE_FB_TARGET, "eEZ_HandleFindAndBind eZCL_WriteLocalAttributeValue %d \n",eZCLStatus);
        return BDB_E_FAILURE;
    }

    eCLD_GroupsSetIdentifying(u8EndPoint, TRUE);
    
    aeFBState[u8EndPoint - 1] = E_FB_IN_PROGRESS_STATE;
    
    return BDB_E_SUCCESS;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
