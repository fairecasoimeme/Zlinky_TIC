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

#include "IASZone.h"
#include "IASZone_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"
#include "dbg.h"

#ifdef DEBUG_CLD_IASZONE
#define TRACE_IASZONE   TRUE
#else
#define TRACE_IASZONE   FALSE
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
#ifdef IASZONE_SERVER
PRIVATE teZCL_Status eCLD_IASZoneHandleZoneEnrollResp(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#ifdef CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE
PRIVATE teZCL_Status eCLD_IASZoneHandleInitiateNormalModeOpRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif
#ifdef CLD_IASZONE_CMD_INITIATE_TEST_MODE
PRIVATE teZCL_Status eCLD_IASZoneHandleInitiateTestModeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif        
#endif
#ifdef IASZONE_CLIENT        
PRIVATE teZCL_Status eCLD_IASZoneHandleZoneEnrollRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId); 
PRIVATE teZCL_Status eCLD_IASZoneHandleZoneStatusChangeNotification(
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
 ** NAME:       eCLD_IASZoneCommandHandler
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
PUBLIC teZCL_Status eCLD_IASZoneCommandHandler(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus = E_ZCL_FAIL;
	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_IASZone_CustomDataStructure *psCommon;
    #ifdef IASZONE_SERVER
        tsCLD_IASZone *psSharedStruct;
        /* Get a pointer to the shared structure */
        psSharedStruct = (tsCLD_IASZone*)psClusterInstance->pvEndPointSharedStructPtr;
    #endif
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASZone_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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
    
#ifdef IASZONE_SERVER
    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer)
    {
        uint64 u64SrcIEEEAddress;
        /*Check if the client is a CIE with the address in the attribute */
        if(pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode == E_ZCL_AM_IEEE)
        {
            u64SrcIEEEAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr;
        }
        else
        {
            u64SrcIEEEAddress = ZPS_u64AplZdoLookupIeeeAddr(pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
            if(u64SrcIEEEAddress == ZPS_NWK_NULL_EXT_ADDR)
            {
                u64SrcIEEEAddress = (uint64)0xffffffffffffffffLL;
            }
        }
        
        if(u64SrcIEEEAddress == psSharedStruct->u64IASCIEAddress )
        {
            // SERVER
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {

                case E_CLD_IASZONE_CMD_ZONE_ENROLL_RESP:
                    eStatus = eCLD_IASZoneHandleZoneEnrollResp(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
                break;
                
                #ifdef CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE
                    case E_CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE:
                        eStatus = eCLD_IASZoneHandleInitiateNormalModeOpRequest(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
                    break;
                #endif
    
                #ifdef  CLD_IASZONE_CMD_INITIATE_TEST_MODE   
                    case E_CLD_IASZONE_CMD_INITIATE_TEST_MODE:
                        eStatus = eCLD_IASZoneHandleInitiateTestModeRequest(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
                    break;
                #endif
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

        }

    }
#endif

#ifdef IASZONE_CLIENT
    if(!psClusterInstance->bIsServer)
    {
        // Client
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

        case E_CLD_IASZONE_CMD_ZONE_STATUS_CHANGE_NOTIFICATION:
            eStatus = eCLD_IASZoneHandleZoneStatusChangeNotification(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_IASZONE_CMD_ZONE_ENROLL_REQUEST:
            eStatus = eCLD_IASZoneHandleZoneEnrollRequest(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
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
    }
#endif
    
    /* Added the check to make sure the default reponse with status success is centralized */
    if((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse))
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
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

#ifdef IASZONE_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneHandleZoneEnrollResp
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
PRIVATE teZCL_Status eCLD_IASZoneHandleZoneEnrollResp(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASZone_CustomDataStructure *psCommon;
    tsCLD_IASZone_EnrollResponsePayload sPayload;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASZone_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASZONE, "\eCLD_IASZoneHandleZoneEnrollResp");

    /* Receive the command */
    eStatus = eCLD_IASZoneEnrollRespReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASZONE, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psZoneEnrollResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);  
    
    /* Update the attributes if the response code is success */
    if (!sPayload.e8EnrollResponseCode)
    {
        eCLD_IASZoneUpdateZoneState(psEndPointDefinition->u8EndPointNumber,E_CLD_IASZONE_STATE_ENROLLED);
        eCLD_IASZoneUpdateZoneID(psEndPointDefinition->u8EndPointNumber,sPayload.u8ZoneID);
    }    
    return E_ZCL_SUCCESS;
}

#ifdef CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneHandleInitiateNormalModeOpRequest
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
PRIVATE teZCL_Status eCLD_IASZoneHandleInitiateNormalModeOpRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASZone_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASZone_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASZONE, "\eCLD_IASZoneHandleZoneEnrollResp");

    /* Receive the command */
    eStatus = eCLD_IASZoneNormalOperationModeReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASZONE, " Error: %d", eStatus);
        return eStatus;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);  
    
    eCLD_IASZoneUpdateZoneStatus(psEndPointDefinition->u8EndPointNumber,CLD_IASZONE_STATUS_MASK_TEST,CLD_IASZONE_STATUS_MASK_RESET);
    psCommon->sTestMode.u8TestModeDuration=0;
    
    eStatus = eZCL_WriteLocalAttributeValue(
                     psEndPointDefinition->u8EndPointNumber,                   //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,                    //uint16                     u16ClusterId,
                     TRUE,                                                     //bool_t                       bIsServerClusterInstance,
                     FALSE,                                                    //bool_t                       bManufacturerSpecific,
                     FALSE,                                                    //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_CURRENT_ZONE_SENSITIVITY_LEVEL,     //uint16                     u16AttributeId,
                     &psCommon->sTestMode.u8CurrentZoneSensitivityLevel        //void                      *pvAttributeValue
                     );   

    return eStatus;
}
#endif

#ifdef CLD_IASZONE_CMD_INITIATE_TEST_MODE
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneHandleInitiateTestModeRequest
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
PRIVATE teZCL_Status eCLD_IASZoneHandleInitiateTestModeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASZone_CustomDataStructure *psCommon;
    tsCLD_IASZone_InitiateTestModeRequestPayload sPayload;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASZone_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASZONE, "\eCLD_IASZoneHandleZoneEnrollResp");

    /* Receive the command */
    eStatus = eCLD_IASZoneTestModeReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASZONE, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psZoneInitiateTestModeRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);  
    
    eCLD_IASZoneUpdateZoneStatus(psEndPointDefinition->u8EndPointNumber,CLD_IASZONE_STATUS_MASK_TEST,CLD_IASZONE_STATUS_MASK_SET);

    psCommon->sTestMode.u8TestModeDuration = sPayload.u8TestModeDuration;
    
    eStatus = eZCL_ReadLocalAttributeValue(
                     psEndPointDefinition->u8EndPointNumber,                   //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,                    //uint16                     u16ClusterId,
                     TRUE,                                                     //bool_t                       bIsServerClusterInstance,
                     FALSE,                                                    //bool_t                       bManufacturerSpecific,
                     FALSE,                                                    //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_CURRENT_ZONE_SENSITIVITY_LEVEL,     //uint16                     u16AttributeId,
                     &psCommon->sTestMode.u8CurrentZoneSensitivityLevel        //void                      *pvAttributeValue
                     );
    
    eStatus = eZCL_WriteLocalAttributeValue(
                     psEndPointDefinition->u8EndPointNumber,                   //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,                    //uint16                     u16ClusterId,
                     TRUE,                                                     //bool_t                       bIsServerClusterInstance,
                     FALSE,                                                    //bool_t                       bManufacturerSpecific,
                     FALSE,                                                    //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_CURRENT_ZONE_SENSITIVITY_LEVEL,     //uint16                     u16AttributeId,
                     &sPayload.u8CurrentZoneSensitivityLevel                   //void                      *pvAttributeValue
                     );
    
    return E_ZCL_SUCCESS;
}
#endif

#endif

#ifdef IASZONE_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneHandleZoneStatusChangeNotification
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
PRIVATE teZCL_Status eCLD_IASZoneHandleZoneStatusChangeNotification(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASZone_CustomDataStructure *psCommon;
    tsCLD_IASZone_StatusChangeNotificationPayload sPayload;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASZone_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASZONE, "\eCLD_IASZoneHandleZoneEnrollResp");

    /* Receive the command */
    eStatus = eCLD_IASZoneStatusChangeNotificationReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASZONE, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psZoneStatusNotificationPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);  

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneHandleZoneEnrollRequest
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
PRIVATE teZCL_Status eCLD_IASZoneHandleZoneEnrollRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_IASZone_CustomDataStructure *psCommon;
    tsCLD_IASZone_EnrollRequestPayload sPayload;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASZone_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_IASZONE, "\eCLD_IASZoneHandleZoneEnrollResp");

    /* Receive the command */
    eStatus = eCLD_IASZoneEnrollReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASZONE, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.sZoneEnrollRequestCallbackPayload.psZoneEnrollRequestPayload = &sPayload;
    psCommon->sCallBackMessage.uMessage.sZoneEnrollRequestCallbackPayload.sZoneEnrollResponsePayload.e8EnrollResponseCode = E_CLD_IASZONE_ENROLL_RESP_NOT_SUPPORTED;
    psCommon->sCallBackMessage.uMessage.sZoneEnrollRequestCallbackPayload.sZoneEnrollResponsePayload.u8ZoneID = 0;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);  
    
    /*Send a response out */
    
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_IASZoneEnrollRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &(psCommon->sCallBackMessage.uMessage.sZoneEnrollRequestCallbackPayload.sZoneEnrollResponsePayload));

    DBG_vPrintf(TRACE_IASZONE, " Send: %d", eStatus);    
    
    return eStatus;
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
