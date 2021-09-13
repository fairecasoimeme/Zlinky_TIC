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
 * MODULE:             Door Lock Cluster
 *
 * COMPONENT:          DoorLockCommandHandler.c
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

#include "zcl.h"
#include "zcl_customcommand.h"

#include "DoorLock.h"
#include "DoorLock_internal.h"

#include "zcl_options.h"


#include "dbg.h"
#ifdef DEBUG_CLD_DOOR_LOCK
#define TRACE_DOOR_LOCK TRUE
#else
#define TRACE_DOOR_LOCK FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_DOOR_LOCK) && !defined(DOOR_LOCK_SERVER) && !defined(DOOR_LOCK_CLIENT)
#error You Must Have either DOOR_LOCK_SERVER and/or DOOR_LOCK_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

#ifdef DOOR_LOCK_SERVER
PRIVATE teZCL_Status eCLD_DoorLockHandleLockUnlockRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif

#ifdef DOOR_LOCK_CLIENT
PRIVATE teZCL_Status eCLD_DoorLockHandleLockUnlockResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
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
 ** NAME:       eCLD_DoorLockCommandHandler
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

PUBLIC  teZCL_Status eCLD_DoorLockCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    DBG_vPrintf(TRACE_DOOR_LOCK, "\nDOOR LOCK: ");

    // further error checking can only be done once we have interrogated the ZCL payload
   	u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


#ifdef DOOR_LOCK_SERVER
    // SERVER
    if(psClusterInstance->bIsServer == TRUE)
    {
        if((sZCL_HeaderParams.u8CommandIdentifier == E_CLD_DOOR_LOCK_CMD_LOCK) || (sZCL_HeaderParams.u8CommandIdentifier == E_CLD_DOOR_LOCK_CMD_UNLOCK)){
            eCLD_DoorLockHandleLockUnlockRequest(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        }else{
            // unlock mutex
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
        }
    }
#endif

#ifdef DOOR_LOCK_CLIENT
    // CLIENT
    if(psClusterInstance->bIsServer == FALSE)
    {
        if((sZCL_HeaderParams.u8CommandIdentifier == E_CLD_DOOR_LOCK_CMD_LOCK) || (sZCL_HeaderParams.u8CommandIdentifier == E_CLD_DOOR_LOCK_CMD_UNLOCK)){
            eCLD_DoorLockHandleLockUnlockResponse(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        }else{
            // unlock
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
        }
    }
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

#ifdef DOOR_LOCK_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_DoorLockHandleLockUnlockRequest
 **
 ** DESCRIPTION:
 ** Handles a  Lock/Unlock Request command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_DoorLockHandleLockUnlockRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_DoorLock_LockUnlockResponsePayload   sResponse = {0};
    tsZCL_CallBackEvent sDoorLockCustomCallBackEvent;
    tsZCL_ReceiveEventAddress       sReceiveEventAddress;
    tsCLD_DoorLockCallBackMessage sDoorLockCallBackMessage;
    tsZCL_Address sZCL_Address;

    DBG_vPrintf(TRACE_DOOR_LOCK, "Lock: ");

    /* Receive the command */
    eStatus = eCLD_DoorLockCommandLockUnlockRequestReceive(pZPSevent,
                                    &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_DOOR_LOCK, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    eZCL_SetReceiveEventAddressStructure(pZPSevent, &sReceiveEventAddress);
    
    /* Generate a custom command event */
    eZCL_SetCustomCallBackEvent(&sDoorLockCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    sDoorLockCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    sDoorLockCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
    sDoorLockCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&sDoorLockCallBackMessage;
    sDoorLockCustomCallBackEvent.psClusterInstance = psClusterInstance;

    /* Fill in message */
    sDoorLockCallBackMessage.u8CommandId = u8CommandIdentifier;
    sDoorLockCallBackMessage.uMessage.psLockUnlockResponsePayload = &sResponse;
    // call callback
    psEndPointDefinition->pCallBackFunctions(&sDoorLockCustomCallBackEvent);
    
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &sReceiveEventAddress);
    
    eStatus = eCLD_DoorLockCommandLockUnlockResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     (teCLD_DoorLock_CommandID)u8CommandIdentifier,
                                                     &sResponse);
    return eStatus;
}
#endif

#ifdef DOOR_LOCK_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_DoorLockHandleLockUnlockResponse
 **
 ** DESCRIPTION:
 ** Handles a Lock/Unlock Response command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_DoorLockHandleLockUnlockResponse(
                    ZPS_tsAfEvent                   *pZPSevent,
                    tsZCL_EndPointDefinition        *psEndPointDefinition,
                    tsZCL_ClusterInstance           *psClusterInstance,
                    uint8                           u8CommandIdentifier)
{

    teZCL_Status eStatus;
    tsCLD_DoorLock_LockUnlockResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    tsZCL_CallBackEvent sDoorLockCustomCallBackEvent;
    tsCLD_DoorLockCallBackMessage sDoorLockCallBackMessage;
    
    /* Receive the command */
    eStatus = eCLD_DoorLockCommandLockUnlockResponseReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sResponse);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_DOOR_LOCK, "\nErr: eCLD_DoorLockCommandLockUnlockResponseReceive:%d", eStatus);
        return eStatus;
    }

    eZCL_SetCustomCallBackEvent(&sDoorLockCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    sDoorLockCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    sDoorLockCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
    sDoorLockCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&sDoorLockCallBackMessage;
    sDoorLockCustomCallBackEvent.psClusterInstance = psClusterInstance;

    /* Fill in message */
    sDoorLockCallBackMessage.u8CommandId = u8CommandIdentifier;
    sDoorLockCallBackMessage.uMessage.psLockUnlockResponsePayload = &sResponse;
    // call callback
    psEndPointDefinition->pCallBackFunctions(&sDoorLockCustomCallBackEvent);
    return E_ZCL_SUCCESS;
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
