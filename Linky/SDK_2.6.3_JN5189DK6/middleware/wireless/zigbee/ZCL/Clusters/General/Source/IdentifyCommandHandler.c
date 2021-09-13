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


/****************************************************************************
 *
 * MODULE:             Identify Cluster
 *
 * DESCRIPTION:
 * Message event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
//#include "zps_apl_aib.h"
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"

#include "Identify.h"
#include "Identify_internal.h"

#ifdef CLD_GROUPS
#include "Groups_internal.h"
#endif

#include "zcl_options.h"

#include "dbg.h"

#ifdef DEBUG_CLD_IDENTIFY
#define TRACE_IDENTIFY    TRUE
#else
#define TRACE_IDENTIFY    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_IDENTIFY) && !defined(IDENTIFY_SERVER) && !defined(IDENTIFY_CLIENT)
#error You Must Have either IDENTIFY_SERVER and/or IDENTIFY_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
                        
    
#ifdef IDENTIFY_SERVER
PRIVATE teZCL_Status eCLD_IdentifyHandleIdentifyRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
                            
#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
PRIVATE teZCL_Status eCLD_IdentifyHandleUpdateCommissionStateCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
#endif 

PRIVATE teZCL_Status eCLD_IdentifyHandleIdentifyQueryRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            bool_t                      bDisableDefaultResponse);
                            
#ifdef  CLD_IDENTIFY_CMD_TRIGGER_EFFECT
PRIVATE teZCL_Status eCLD_IdentifyHandleIdentifyTriggerEffectRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

#endif
#endif

#ifdef IDENTIFY_CLIENT
PRIVATE teZCL_Status eCLD_IdentifyHandleIdentifyQueryResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif

#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE
PRIVATE teZCL_Status eCLD_IdentifyHandleEZModeInvokeCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
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
 ** NAME:       eCLD_IdentifyCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Message Cluster custom commands
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
PUBLIC  teZCL_Status eCLD_IdentifyCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_IdentifyCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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

#ifdef IDENTIFY_SERVER
        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_IDENTIFY_CMD_IDENTIFY):
            {
                eCLD_IdentifyHandleIdentifyRequest(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_IDENTIFY_CMD_IDENTIFY_QUERY):
            {
                eCLD_IdentifyHandleIdentifyQueryRequest(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.bDisableDefaultResponse);
                break;
            }

#ifdef  CLD_IDENTIFY_CMD_TRIGGER_EFFECT
            case E_CLD_IDENTIFY_CMD_TRIGGER_EFFECT:
                eCLD_IdentifyHandleIdentifyTriggerEffectRequest(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
#endif

#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE
            case E_CLD_IDENTIFY_CMD_EZ_MODE_INVOKE:
                eCLD_IdentifyHandleEZModeInvokeCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
#endif
#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
            case E_CLD_IDENTIFY_CMD_UPDATE_COMMISSION_STATE:
                eCLD_IdentifyHandleUpdateCommissionStateCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
#endif
            default:
            {
                // unlock
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif

    }
    else
    {

#ifdef IDENTIFY_CLIENT
        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_IDENTIFY_CMD_IDENTIFY):
            {
                eCLD_IdentifyHandleIdentifyQueryResponse(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
            
#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE
            case E_CLD_IDENTIFY_CMD_EZ_MODE_INVOKE:
                eCLD_IdentifyHandleEZModeInvokeCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
            break;
#endif
            default:
            {
                // unlock
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif

    }

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
#ifdef IDENTIFY_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyHandleIdentifyRequest
 **
 ** DESCRIPTION:
 ** Handles identify request
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
PRIVATE  teZCL_Status eCLD_IdentifyHandleIdentifyRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_Identify_IdentifyRequestPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IdentifyCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IdentifyCommandIdentifyRequestReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IDENTIFY, "eCLD_IdentifyCommandIdentifyRequestReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psIdentifyRequestPayload = &sPayload;

    /* Update attribute */
    ((tsCLD_Identify*)psClusterInstance->pvEndPointSharedStructPtr)->u16IdentifyTime = sPayload.u16IdentifyTime;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

#ifdef CLD_GROUPS
    if(sPayload.u16IdentifyTime != 0)
        eCLD_GroupsSetIdentifying(psEndPointDefinition->u8EndPointNumber, TRUE);
#endif

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyHandleIdentifyQueryRequest
 **
 ** DESCRIPTION:
 ** Handles Identify Query request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** bool_t                    bDisableDefaultResponse   Disable Default Response flag
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_IdentifyHandleIdentifyQueryRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            bool_t                      bDisableDefaultResponse)
{

    teZCL_Status eStatus;
    tsCLD_Identify_IdentifyQueryResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IdentifyCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;


    /* Send a default response on error, further default response
     * processes after handling the command
     * If sending a query response, then do not send default response
     * if not sending query response, then send default response Success if enabeled as it has been disabled from the receive command
     */

    /* Receive the command */
    eStatus = eCLD_IdentifyCommandIdentifyQueryRequestReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IDENTIFY, "eCLD_IdentifyCommandIdentifyQueryRequestReceive Error: %d\r\n", eStatus);
        return eStatus;
    }


    /* No Message data for callback */

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* If we're currently identifying, send a response */
    if(((tsCLD_Identify*)psClusterInstance->pvEndPointSharedStructPtr)->u16IdentifyTime > 0)
    {

        /* Build response */
        sResponse.u16Timeout = ((tsCLD_Identify*)psClusterInstance->pvEndPointSharedStructPtr)->u16IdentifyTime;

        // build address structure
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

        eStatus = eCLD_IdentifyCommandIdentifyQueryResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                         pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                         &sZCL_Address,
                                                         &u8TransactionSequenceNumber,
                                                         &sResponse);

    } else {
        /* lpsw2722
         * not sending a query response and default responses not disabled
         * then send a default response
         */
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
    }

    return eStatus;
}

#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyHandleUpdateCommissionStateCommand
 **
 ** DESCRIPTION:
 ** Handles Update Commission State command
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
PRIVATE  teZCL_Status eCLD_IdentifyHandleUpdateCommissionStateCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_Identify_UpdateCommissionStatePayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IdentifyCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IdentifyUpdateCommissionStateCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IDENTIFY, "eCLD_IdentifyUpdateCommissionStateCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psUpdateCommissionStatePayload = &sPayload;

    switch(sPayload.u8Action)
    {
        case 0:
            break;
        case 1:
            /* Update attribute */
            ((tsCLD_Identify*)psClusterInstance->pvEndPointSharedStructPtr)->u8CommissionState |= sPayload.u8CommissionStateMask;
        break;
        case 2:
            /* Update attribute */
            ((tsCLD_Identify*)psClusterInstance->pvEndPointSharedStructPtr)->u8CommissionState &= ~(sPayload.u8CommissionStateMask);
        break;
        default:
            return E_ZCL_ERR_INVALID_VALUE;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}
#endif

#ifdef CLD_IDENTIFY_CMD_TRIGGER_EFFECT
PRIVATE  teZCL_Status eCLD_IdentifyHandleIdentifyTriggerEffectRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_Identify_TriggerEffectRequestPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IdentifyCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IdentifyCommandTriggerEffectRequestReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IDENTIFY, "eCLD_IdentifyCommandTriggerEffectRequestReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psTriggerEffectRequestPayload = &sPayload;




    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* For reserved eEffectId, send DefaultResponse with Invalid Value. */
    switch(sPayload.eEffectId)
    {
        case E_CLD_IDENTIFY_EFFECT_BLINK:
        case E_CLD_IDENTIFY_EFFECT_BREATHE:
        case E_CLD_IDENTIFY_EFFECT_OKAY:
        case E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE:
        case E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT:
        case E_CLD_IDENTIFY_EFFECT_STOP_EFFECT:
            /* Valid mode so continue */
            break;
        default:
            DBG_vPrintf(TRACE_IDENTIFY, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
            break;
    }

    /* For reserved u8EffectVarient, send DefaultResponse with Invalid Value. */
    switch(sPayload.u8EffectVarient)
    {
        case 0x00: // Default
            break;
        default:
            DBG_vPrintf(TRACE_IDENTIFY, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            /* Doing the default Effect Varient */
            sPayload.u8EffectVarient = 0x00;
            break;
    }

    return E_ZCL_SUCCESS;
}
#endif
#endif /* IDENTIFY_SERVER */

#ifdef IDENTIFY_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyHandleIdentifyQueryResponse
 **
 ** DESCRIPTION:
 ** Handles identify request
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
PRIVATE  teZCL_Status eCLD_IdentifyHandleIdentifyQueryResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_Identify_IdentifyQueryResponsePayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IdentifyCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IdentifyCommandIdentifyQueryResponseReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IDENTIFY, "eCLD_IdentifyCommandIdentifyQueryResponseReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psIdentifyQueryResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}
#endif

#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyHandleEZModeInvokeCommand
 **
 ** DESCRIPTION:
 ** Handles EZ mode invoke request
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
PRIVATE  teZCL_Status eCLD_IdentifyHandleEZModeInvokeCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_Identify_EZModeInvokePayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IdentifyCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IdentifyCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IdentifyEZModeInvokeCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IDENTIFY, "eCLD_IdentifyEZModeInvokeCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEZModeInvokePayload = &sPayload;

    switch(sPayload.u8Action)
    {
        case IDENTIFY_CMD_EZMODE_INVOKE_ACTION_FACTOTY_FRESH:
        break;
        case IDENTIFY_CMD_EZMODE_INVOKE_ACTION_NETWORK_STEERING:
        break;
        case IDENTIFY_CMD_EZMODE_INVOKE_ACTION_FINDING_AND_BINDING:
        break;
        default:
            return E_ZCL_ERR_INVALID_VALUE;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
