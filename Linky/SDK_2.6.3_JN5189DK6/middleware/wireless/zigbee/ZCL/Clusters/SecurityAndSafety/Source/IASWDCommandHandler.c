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


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

#include "IASWD.h"
#include "IASWD_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"
#include "dbg.h"

#ifdef DEBUG_CLD_IASWD
#define TRACE_IASWD   TRUE
#else
#define TRACE_IASWD   FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef IASWD_SERVER
PRIVATE teZCL_Status eCLD_IASWDHandleStartWarningReq(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId); 
PRIVATE teZCL_Status eCLD_IASWDHandleSquawkReq(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_IASWDCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASWDCommandHandler(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance)
{
    #ifdef IASWD_SERVER 
        teZCL_Status eStatus = E_ZCL_FAIL;
    #endif
	tsZCL_HeaderParams sZCL_HeaderParams;

    tsCLD_IASWD_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASWD_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Store return address details */
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psCommon->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer)
    {
#ifdef IASWD_SERVER    
        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

        case E_CLD_IASWD_CMD_START_WARNING:
            eStatus = eCLD_IASWDHandleStartWarningReq(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_IASWD_CMD_SQUAWK:
            eStatus = eCLD_IASWDHandleSquawkReq(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        default:
            /* Unsupported command */
            // unlock
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            /*As the command handler for this command is available but it does NOT handle the commands hence return */
            return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
            break;

        }
#endif
    }
    else
    {
#ifdef IASWD_CLIENT    
        // Client
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {
        
        default:
            /* Unsupported command */
            // unlock
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            /*As the command handler for this command is available but it does NOT handle the commands hence return */
            return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
            break;

        }
#endif        
    }
    
#ifdef IASWD_SERVER 
    /* Added the check to make sure the default reponse with status success is centralized */
    if((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse))
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
#endif        
    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef IASWD_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_IASWDHandleStartWarningReq
 **
 ** DESCRIPTION:
 ** Handles move to hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_IASWDHandleStartWarningReq(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASWD_CustomDataStructure *psCommon;
    tsCLD_IASWD_StartWarningReqPayload sPayload;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASWD_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASWD, "\neCLD_IASWDHandleStartWarningReq");

    /* Receive the command */
    eStatus = eCLD_IASWDStartWarningReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASWD, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psWDStartWarningReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent); 

    /*Update The custom storage for the Warning Req*/ 
    psCommon->sWarning = sPayload;  
    /*Update the the warning duration for transition in 100MS*/
    psCommon->u32WarningDurationRemainingIn100MS = psCommon->sWarning.u16WarningDuration*10;

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASWDHandleSquawkReq
 **
 ** DESCRIPTION:
 ** Handles move to hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_IASWDHandleSquawkReq(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASWD_CustomDataStructure *psCommon;
    tsCLD_IASWD_SquawkReqPayload sPayload;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASWD_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASWD, "\neCLD_IASWDHandleSquawkReq");

    /* Receive the command */
    eStatus = eCLD_IASWDSquawkReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASWD, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psWDSquawkReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);  

    /*Update The custom storage for the Warning Req*/ 
    psCommon->sSquawk = sPayload;  

    return E_ZCL_SUCCESS;
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
