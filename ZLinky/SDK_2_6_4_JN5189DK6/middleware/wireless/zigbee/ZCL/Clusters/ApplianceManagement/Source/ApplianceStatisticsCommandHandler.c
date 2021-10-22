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
 * MODULE:             Appilance Statistics Cluster
 *
 * COMPONENT:          ApplianceStatisticsCommandHandler.c
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

#include "ApplianceStatistics.h"
#include "ApplianceStatistics_internal.h"

#include "zcl_options.h"

#include "dbg.h"



#ifdef DEBUG_CLD_APPLIANCE_STATISTICS
#define TRACE_APPLIANCE_STATISTICS    TRUE
#else
#define TRACE_APPLIANCE_STATISTICS    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_APPLIANCE_STATISTICS) && !defined(APPLIANCE_STATISTICS_SERVER) && !defined(APPLIANCE_STATISTICS_CLIENT)
#error You Must Have either APPLIANCE_STATISTICS_SERVER and/or APPLIANCE_STATISTICS_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef APPLIANCE_STATISTICS_CLIENT
PRIVATE teZCL_Status eCLD_ASCHandleLogNotificationORLogResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ASCHandleLogQueueResponseORStatisticsAvailable(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                        u8CommandId);
#endif

#ifdef APPLIANCE_STATISTICS_SERVER
PRIVATE teZCL_Status eCLD_ASCHandleLogRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eCLD_ASCHandleLogQueueRequest(
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
 ** NAME:       eCLD_ApplianceStatisticsCommandHandler
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
PUBLIC  teZCL_Status eCLD_ApplianceStatisticsCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_ApplianceStatisticsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ApplianceStatisticsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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

#ifdef APPLIANCE_STATISTICS_SERVER
    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == TRUE)
    {

        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier) 
        {

            case (E_CLD_APPLIANCE_STATISTICS_CMD_LOG_REQUEST):
            {
                eCLD_ASCHandleLogRequest(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_REQUEST):
            {
                eCLD_ASCHandleLogQueueRequest(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

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

    }
#endif

#ifdef APPLIANCE_STATISTICS_CLIENT

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == FALSE)
    {

        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_APPLIANCE_STATISTICS_CMD_LOG_NOTIFICATION):
            case (E_CLD_APPLIANCE_STATISTICS_CMD_LOG_RESPONSE):
            {
                eCLD_ASCHandleLogNotificationORLogResponse(pZPSevent, psEndPointDefinition, psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
                break;
            }

            case (E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_RESPONSE):
            case (E_CLD_APPLIANCE_STATISTICS_CMD_STATISTICS_AVAILABLE):
            {
                eCLD_ASCHandleLogQueueResponseORStatisticsAvailable(pZPSevent, psEndPointDefinition, psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier); 
                break;
            }

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
    }
#endif

    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

#ifdef APPLIANCE_STATISTICS_SERVER
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_ASCHandleLogRequest
 **
 ** DESCRIPTION:
 ** Handles Log request
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

PRIVATE  teZCL_Status eCLD_ASCHandleLogRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_ASC_LogRequestPayload sPayload;
    tsCLD_ASC_LogNotificationORLogResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    tsCLD_ApplianceStatisticsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;
    tsCLD_LogTable *psLogTable;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ApplianceStatisticsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_ASCLogRequestReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "\nErr:eCLD_ASCLogRequestReceive:%d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "\nAdd Log:%04x ", sPayload.u32LogId);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psLogRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    if(eCLD_ASCGetLogEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u32LogId,&psLogTable) == E_ZCL_CMDS_SUCCESS)
    {
        sResponse.utctTime = psLogTable->utctTime;
        sResponse.u32LogId = psLogTable->u32LogID;
        sResponse.u32LogLength = psLogTable->u8LogLength;
        sResponse.pu8LogData = psLogTable->pu8LogData;
    }
    else
    {
#ifdef DEBUG_CLD_APPLIANCE_STATISTICS        
        DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "\nErr\n");
#endif
        return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
    }


    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_ASCLogNotificationORLogResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_APPLIANCE_STATISTICS_CMD_LOG_RESPONSE,
                                                     &sResponse);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_ASCHandleLogQueueRequest
 **
 ** DESCRIPTION:
 ** Handles Log Queue request
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

PRIVATE  teZCL_Status eCLD_ASCHandleLogQueueRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_ApplianceStatisticsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ApplianceStatisticsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_ASCLogQueueRequestReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "\nErr:eCLD_ASCLogQueueRequestReceive:%d", eStatus);
        return eStatus;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_ASCLogQueueResponseORStatisticsAvailableSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_RESPONSE);
    return eStatus;
}
#endif

#ifdef APPLIANCE_STATISTICS_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_ASCHandleLogNotificationORLogResponse
 **
 ** DESCRIPTION:
 ** Handles Log Notification
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ASCHandleLogNotificationORLogResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    tsCLD_ASC_LogNotificationORLogResponsePayload sPayload;
    
    uint8   u8TransactionSequenceNumber;
    tsCLD_ApplianceStatisticsCustomDataStructure *psCommon;

    uint8 au8LogData[CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE];
    sPayload.pu8LogData=(uint8*)&au8LogData[0];
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ApplianceStatisticsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_ASCLogNotificationORLogResponseReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "\nErr:eCLD_ASCLogNotificationORLogResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psLogNotificationORLogResponsePayload = &sPayload;

    psCommon->sCallBackMessage.u8CommandId = u8CommandId;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_ASCHandleLogQueueResponseORStatisticsAvailable
 **
 ** DESCRIPTION:
 ** Handles Log Notification
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ASCHandleLogQueueResponseORStatisticsAvailable(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    tsCLD_ASC_LogQueueResponseORStatisticsAvailablePayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_ApplianceStatisticsCustomDataStructure *psCommon;
    
    uint32 au32LogId[CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE];
    sPayload.pu32LogId=(uint32*)&au32LogId[0];
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ApplianceStatisticsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_ASCLogQueueResponseORStatisticsAvailableReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_APPLIANCE_STATISTICS, "\nErr:eCLD_ASCLogQueueResponseORStatisticsAvailableReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psLogQueueResponseORStatisticsAvailabePayload = &sPayload;

    psCommon->sCallBackMessage.u8CommandId = u8CommandId;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
