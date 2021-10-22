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
 * MODULE:             WWAH Cluster
 *
 * COMPONENT:          WWAHCommandHandler.c
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
#include "zcl_common.h"
#include "WWAH.h"
#include "WWAH_internal.h"

#include "zcl_options.h"

#include "dbg.h"
#include "zps_nwk_sec.h"
#include "zps_apl_aib.h"

#ifdef DEBUG_CLD_WWAH
#define TRACE_WWAH TRUE
#else
#define TRACE_WWAH FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_WWAH) && !defined(WWAH_SERVER) && !defined(WWAH_CLIENT)
#error You Must Have either WWAH_SERVER and/or WWAH_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

#ifdef WWAH_SERVER

PRIVATE teZCL_Status eCLD_WWAHHandleEnableOrDisableAPSLinkKeyAuthorizationRequest(
        ZPS_tsAfEvent              *pZPSevent,
        tsZCL_EndPointDefinition   *psEndPointDefinition,
        tsZCL_ClusterInstance      *psClusterInstance,
        uint8                      u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleRequestNewAPSLinkKeyRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnableWWAHAppEventRetryAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableWWAHAppEventRetryAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleRequestTimeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnableWWAHRejoinAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableWWAHRejoinAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleSetIASZoneEnrollmentMethodRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleClearBindingTableRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnablePeriodicRouterCheckInsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisablePeriodicRouterCheckInsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleSetMACPollCCAWaitTimeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleSetPendingNetworkUpdateRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleRequireAPSACKsOnUnicastsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleRemoveAPSACKsOnUnicastsRequirementRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);        
PRIVATE teZCL_Status eCLD_WWAHHandleAPSACKEnablementQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDebugReportQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleSurveyBeaconsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);        
PRIVATE teZCL_Status eCLD_WWAHHandleDisableOTADowngradesRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableTouchlinkInterpanMessageRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableParentClassificationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnableBadParentRecoveryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnableConfigurationModeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleUseTrustCenterForClusterServerRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableMGMTLeaveWithoutRejoinRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnableWWAHParentClassificationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleEnableTCSecurityonNwkKeyRotationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableWWAHBadParentRecoveryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDisableConfigurationModeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleTrustCenterForClusterServerQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);

#endif /* WWAH_SERVER */

#ifdef WWAH_CLIENT
PRIVATE teZCL_Status eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandlePoweringOnOffNotification(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleShortAddressChange(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleAPSACKEnablementQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleHandleNewDebugReportNotification(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandlPowerDescriptorChangeQuery(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleDebugReportQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleTrustCenterForClusterQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);
PRIVATE teZCL_Status eCLD_WWAHHandleSurveyBeaconsResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier);

#endif /* WWAH_CLIENT */

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
 ** NAME:       eCLD_WWAHCommandHandler
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

PUBLIC teZCL_Status eCLD_WWAHCommandHandler(
        ZPS_tsAfEvent *pZPSevent,
        tsZCL_EndPointDefinition *psEndPointDefinition,
        tsZCL_ClusterInstance *psClusterInstance)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Store return address details */
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psCommon->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(
            pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
            &sZCL_HeaderParams);

    // get EP mutex
#ifndef COOPERATIVE
    eZCL_GetMutex(psEndPointDefinition);
#endif

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;
    
#ifdef WWAH_SERVER
    // SERVER
    if (psClusterInstance->bIsServer == TRUE)
    {
        
        /* Shweta ToDo : Check if this is correct or not */
        if((pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr != 0x0000) 
            && (ZPS_u64AplZdoLookupIeeeAddr(pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr) != ZPS_eAplAibGetApsTrustCenterAddress()))
        {
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
            return E_ZCL_SUCCESS;            
        }
        else
        {
            switch (sZCL_HeaderParams.u8CommandIdentifier)
            {
            case (E_CLD_WWAH_CMD_ENABLE_APS_LINK_KEY_AUTHORIZATION):
            case (E_CLD_WWAH_CMD_DISABLE_APS_LINK_KEY_AUTHORIZATION):
                eStatus = eCLD_WWAHHandleEnableOrDisableAPSLinkKeyAuthorizationRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case (E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY):
                eStatus = eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_REQUEST_NEW_APS_LINK_KEY):
                eStatus = eCLD_WWAHHandleRequestNewAPSLinkKeyRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case (E_CLD_WWAH_CMD_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM):
                eStatus = eCLD_WWAHHandleEnableWWAHAppEventRetryAlgorithmRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case (E_CLD_WWAH_CMD_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM):
                eStatus = eCLD_WWAHHandleDisableWWAHAppEventRetryAlgorithmRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case (E_CLD_WWAH_CMD_REQUEST_TIME):
                eStatus = eCLD_WWAHHandleRequestTimeRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_ENABLE_WWAH_REJOIN_ALGORITHM):
                eStatus = eCLD_WWAHHandleEnableWWAHRejoinAlgorithmRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISABLE_WWAH_REJOIN_ALGORITHM):
                eStatus = eCLD_WWAHHandleDisableWWAHRejoinAlgorithmRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_SET_IAS_ZONE_ENROLLMENT_METHOD):
                eStatus = eCLD_WWAHHandleSetIASZoneEnrollmentMethodRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_CLEAR_BINDING_TABLE):
                eStatus = eCLD_WWAHHandleClearBindingTableRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_ENABLE_PERIODIC_ROUTER_CHECK_INS):
                eStatus = eCLD_WWAHHandleEnablePeriodicRouterCheckInsRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISABLE_PERIODIC_ROUTER_CHECK_INS):
                eStatus = eCLD_WWAHHandleDisablePeriodicRouterCheckInsRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_SET_MAC_POLL_CCA_WAIT_TIME):
                eStatus = eCLD_WWAHHandleSetMACPollCCAWaitTimeRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_SET_PENDING_NETWORK_UPDATE):
                eStatus = eCLD_WWAHHandleSetPendingNetworkUpdateRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_REQUIRE_APS_ACKS_ON_UNICAST):
                eStatus = eCLD_WWAHHandleRequireAPSACKsOnUnicastsRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;            
            
            case(E_CLD_WWAH_CMD_REMOVE_APS_ACKS_ON_UNICAST):
                eStatus = eCLD_WWAHHandleRemoveAPSACKsOnUnicastsRequirementRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY):
                eStatus = eCLD_WWAHHandleAPSACKEnablementQueryRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY):
                eStatus = eCLD_WWAHHandleDebugReportQueryRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_USE_TRUST_CENTER_FOR_CLUSTER_SERVER):
                eStatus = eCLD_WWAHHandleUseTrustCenterForClusterServerRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_SURVEY_BEACONS):
                eStatus = eCLD_WWAHHandleSurveyBeaconsRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;
                        
            case(E_CLD_WWAH_CMD_DISABLE_OTA_DOWNGRADES):
                eStatus = eCLD_WWAHHandleDisableOTADowngradesRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISBALE_MGMT_LEAVE_WITHOUT_REJOIN):
                eStatus = eCLD_WWAHHandleDisableMGMTLeaveWithoutRejoinRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISABLE_TOUCHLINK_INTERPAN_MESSAGE):
                eStatus = eCLD_WWAHHandleDisableTouchlinkInterpanMessageRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_ENABLE_WWAH_PARENT_CLASSIFICATION):
                eStatus = eCLD_WWAHHandleEnableWWAHParentClassificationRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISABLE_WWAH_PARENT_CLASSIFICATION):
                eStatus = eCLD_WWAHHandleDisableParentClassificationRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION):
                eStatus = eCLD_WWAHHandleEnableTCSecurityonNwkKeyRotationRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_ENABLE_WWAH_BAD_PARENT_RECOVERY):
                eStatus = eCLD_WWAHHandleEnableBadParentRecoveryRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISABLE_WWAH_BAD_PARENT_RECOVERY):
                eStatus = eCLD_WWAHHandleDisableWWAHBadParentRecoveryRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_ENABLE_CONFIGURATION_MODE):
                eStatus = eCLD_WWAHHandleEnableConfigurationModeRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_DISABLE_CONFIGURATION_MODE):
                eStatus = eCLD_WWAHHandleDisableConfigurationModeRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            case(E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY):
                eStatus = eCLD_WWAHHandleTrustCenterForClusterServerQueryRequest(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
            break;

            default:
    #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
            return (E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
            }
        }
    }
#endif

#ifdef WWAH_CLIENT
    // CLIENT

    if(psClusterInstance->bIsServer == FALSE)
    {
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {
        case (E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE):
            eStatus = eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryResponse(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case (E_CLD_WWAH_CMD_POWERING_OFF_NOTIFICATION):
        case (E_CLD_WWAH_CMD_POWERING_ON_NOTIFICATION):
            eStatus = eCLD_WWAHHandlePoweringOnOffNotification(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case (E_CLD_WWAH_CMD_SHORT_ADDRESS_CHANGE):
            eStatus = eCLD_WWAHHandleShortAddressChange(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case (E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY_RESPONSE):
            eStatus = eCLD_WWAHHandleAPSACKEnablementQueryResponse(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case (E_CLD_WWAH_CMD_POWER_DESCRIPTOR_CHANGE):
            eStatus = eCLD_WWAHHandlPowerDescriptorChangeQuery(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case(E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY_RESPONSE):
            eStatus = eCLD_WWAHHandleDebugReportQueryResponse(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case (E_CLD_WWAH_CMD_NEW_DEBUG_REPORT_NOTIFICATION):
            eStatus = eCLD_WWAHHandleHandleNewDebugReportNotification(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case(E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE):
                eStatus = eCLD_WWAHHandleTrustCenterForClusterQueryResponse(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        case(E_CLD_WWAH_CMD_SURVEY_BEACON_RESPONSE):
                eStatus = eCLD_WWAHHandleSurveyBeaconsResponse(pZPSevent,psEndPointDefinition,psClusterInstance,sZCL_HeaderParams.u8CommandIdentifier);
        break;

        default:
#ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
#endif
        return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
        }
    }
#endif

    /* Added the check to make sure the default response with status success is centralized */
    if((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse))
       eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);

    // unlock
#ifndef COOPERATIVE
    eZCL_ReleaseMutex(psEndPointDefinition);
#endif
    // delete the i/p buffer on return
    return (E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

#ifdef WWAH_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableOrDisableAPSLinkKeyAuthorizationRequest
 **
 ** DESCRIPTION:
 ** Handles an Enable APS Link Key Authorization Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableOrDisableAPSLinkKeyAuthorizationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload  sRequest  = { 0 };
    bool_t bIsClusterExempted = FALSE;
    int i = 0, j = 0;
    uint16 au16ClusterId[CLD_WWAH_NO_OF_CLUSTERS];

    sRequest.pu16ClusterId=(uint16*)au16ClusterId;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableOrDisableAPSLinkKeyAuthorizationRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psEnableOrDisableAPSLinkKeyAuthorizationRequestPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

	
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    /* Make the whole application security to be APS */
    eZCL_SetSupportedSecurity(E_ZCL_SECURITY_APPLINK);
    
    for(i = 0; i < psEndPointDefinition->u16NumberOfClusters; i++)
    {
        for(j = 0; j < sRequest.u8NumberOfClustersToExempt; j++)
        {
            if(psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum == sRequest.pu16ClusterId[j])
            {
                bIsClusterExempted = TRUE;
                break;
            }
        }
        /* Shweta ToDo Confirm this Always Exempt the WWAH Cluster as it should always be at APS security */
        if((bIsClusterExempted == FALSE) &&
                 (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum != GENERAL_CLUSTER_ID_WWAH))
        {
            psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u8ClusterControlFlags &= 0xF0;
            if(u8CommandIdentifier == E_CLD_WWAH_CMD_ENABLE_APS_LINK_KEY_AUTHORIZATION)
                psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u8ClusterControlFlags |= E_ZCL_SECURITY_APPLINK;
            else
                psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u8ClusterControlFlags |= E_ZCL_SECURITY_NETWORK;
        }
        bIsClusterExempted = FALSE;
        j = 0;
    }

    if(ZPS_u8AplAibGetDeviceApsKeyType(ZPS_eAplAibGetApsTrustCenterAddress()) != ZPS_APS_TCLK_LINK_KEY)
    {
        /* Send a Default response */
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_ACTION_DENIED);
        eStatus = E_ZCL_FAIL;
    }
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryRequest
 **
 ** DESCRIPTION:
 ** Handles an APS Link Key Authorization Query Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload  sRequest  = { 0 };
    tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload sResponse = { 0 };
    tsZCL_Address sZCL_Address;
    int i;
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    sResponse.u16ClusterID = sRequest.u16ClusterID;
    
    for(i = 0; i < psEndPointDefinition->u16NumberOfClusters; i++)
    {
        if((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum == sRequest.u16ClusterID) &&
             (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u8ClusterControlFlags & E_ZCL_SECURITY_APPLINK))
        {
            sResponse.bAPSLinkKeyAuthorizationStatus = TRUE;
            break;
        }
        else
        {
            sResponse.bAPSLinkKeyAuthorizationStatus = FALSE;
        }
    }
    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psAPSLinkKeyAuthorizationQueryRequestPayload = &sRequest;
    CallBackMessage.uRespMessage.psAPSLinkKeyAuthorizationQueryResponsePayload = &sResponse;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* build address structure */
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address,&psCommon->sReceiveEventAddress);

    eStatus = eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseSend(
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &u8TransactionSequenceNumber,
                    &sResponse);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleRequestNewAPSLinkKeyRequest
 **
 ** DESCRIPTION:
 ** Handles a Request New APS Link Key Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleRequestNewAPSLinkKeyRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    /* Receive the command */
    eStatus = eCLD_WWAHCommandRequestNewAPSLinkKeyRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Shweta ToDo Check if it is only for TCLK as of now */
    ZPS_eAplZdoRequestKeyReq(4, 0);

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableWWAHAppEventRetryAlgorithmRequest
 **
 ** DESCRIPTION:
 ** Handles an Enable WWAH App Event Retry Algorithm Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableWWAHAppEventRetryAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    if((sRequest.u8FirstBackoffTime <= 0) ||
        (sRequest.u8BackoffSequenceCommonRatio <= 0) ||
            (sRequest.u32MaxBackoffTime <= sRequest.u8FirstBackoffTime) || 
                (sRequest.u32MaxBackoffTime >= 86400) ||
                    (sRequest.u8MaxReDeliveryAttempts <= 0))
    {
        /* Send a Default response */
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return eStatus;
    }                    
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHAppEventRetryEnabled = TRUE;
    
    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psEnableWWAHAppEventRetryAlgorithmRequestPayload = &sRequest;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableWWAHAppEventRetryAlgorithmRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable WWAH App Event Retry Algorithm Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableWWAHAppEventRetryAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHAppEventRetryEnabled = FALSE;

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
	
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleRequestTimeRequest
 **
 ** DESCRIPTION:
 ** Handles a Request Time Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleRequestTimeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandRequestTimeRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
	
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableWWAHRejoinAlgorithmRequest
 **
 ** DESCRIPTION:
 ** Handles an Enable WWAH Rejoin Algorithm Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableWWAHRejoinAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableWWAHRejoinAlgorithmRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psEnableWWAHRejoinAlgorithmRequestPayload = &sRequest;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
	
    /* Set the Rejoin Enabled attribute as TRUE */
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHRejoinEnabled = TRUE;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableWWAHRejoinAlgorithmRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable WWAH App Event Retry Algorithm Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableWWAHRejoinAlgorithmRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHRejoinEnabled = FALSE;

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleSetIASZoneEnrollmentMethodRequest
 **
 ** DESCRIPTION:
 ** Handles a Set IAS Zone Enrollment Method Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleSetIASZoneEnrollmentMethodRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psSetIASZoneEnrollmentMethodRequestPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    /* If Sensor is already enrolled , just send a default status of NOT authorized */
    if(sRequest.eEnrollmentMode == 0xFF)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
        eStatus = E_ZCL_FAIL;
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleClearBindingTableRequest
 **
 ** DESCRIPTION:
 ** Handles a Clear Binding Table Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleClearBindingTableRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandClearBindingTableRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    ZPS_ePurgeBindTable();
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnablePeriodicRouterCheckInsRequest
 **
 ** DESCRIPTION:
 ** Handles an Enable Periodic Router CheckIns Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnablePeriodicRouterCheckInsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bRouterCheckInEnabled = TRUE;


     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psEnablePeriodicRouterCheckInsRequestPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
  
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisablePeriodicRouterCheckInsRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable Periodic Router Check-Ins Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisablePeriodicRouterCheckInsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bRouterCheckInEnabled = FALSE;

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
  
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleSetMACPollCCAWaitTimeRequest
 **
 ** DESCRIPTION:
 ** Handles a Set MAC Poll CCA Wait Time Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleSetMACPollCCAWaitTimeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->u8MACPollCCAWaitTime = sRequest.u8WaitTime;

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psSetMACPollCCAWaitTimeRequestPayload = &sRequest;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleSetPendingNetworkUpdateRequest
 **
 ** DESCRIPTION:
 ** Handles a Set Pending Network Update Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                      u8CommandIdentifier       Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleSetPendingNetworkUpdateRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandSetPendingNetworkUpdateRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);

    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->u8PendingNetworkUpdateChannel = sRequest.u8Channel;
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->u16PendingNetworkUpdatePANID = sRequest.u16PANID;

    /* Message data for callback */
     psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psSetPendingNetworkUpdateRequestPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleRequireAPSACKsOnUnicastsRequest
 **
 ** DESCRIPTION:
 ** Handles a Require APS ACKs on Unicasts Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleRequireAPSACKsOnUnicastsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload  sRequest  = { 0 };
    int8 i = 0, j = 0;
    uint16 au16ClusterId[CLD_WWAH_NO_OF_CLUSTERS];
    bool_t bIsClusterExempted = FALSE;
    
    sRequest.pu16ClusterId=(uint16*)au16ClusterId;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* ToDo: Shweta need to do it for all endpoint */
    for(i = 0; i < psEndPointDefinition->u16NumberOfClusters; i++)
    {
        for(j = 0; j < sRequest.u8NumberOfClustersToExempt; j++)
        {
            /* Shweta ToDo Confirm this Always Exempt the WWAH Cluster */
            if(psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum == sRequest.pu16ClusterId[j])
            {
                bIsClusterExempted = TRUE;
                break;
            }
        }
        if(bIsClusterExempted == FALSE)
        {
            eStatus = eZCL_SetAPSACKClusterControlFlagsBit(psEndPointDefinition->u8EndPointNumber,
                                     psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum,
                                     psEndPointDefinition->psClusterInstance[i].bIsServer,
                                     TRUE);
        }
        bIsClusterExempted = FALSE;
        j = 0;
    }
    
    
    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psRequireAPSACKsOnUnicastsRequestPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleRemoveAPSACKsOnUnicastsRequirementRequest
 **
 ** DESCRIPTION:
 ** Handles a Remove APS ACKs on Unicasts Requirement Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleRemoveAPSACKsOnUnicastsRequirementRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    int8 i = 0;
    uint8 u8ClusterControlFlags = 0;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);                  
                    
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
            
    /* ToDo: Shweta need to do it for all endpoint */
    for(i = 0; i < psEndPointDefinition->u16NumberOfClusters; i++)
    {
        u8ClusterControlFlags = u8ZCL_GetClusterControlFlags(psEndPointDefinition->u8EndPointNumber,
                                                            psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum,
                                                            psEndPointDefinition->psClusterInstance[i].bIsServer);


        if((u8ClusterControlFlags != 0xFF) && (u8ClusterControlFlags & CLUSTER_APS_ACK_ENABLE_MASK))
        {
            eStatus = eZCL_SetAPSACKClusterControlFlagsBit(psEndPointDefinition->u8EndPointNumber,
                         psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum,
                         psEndPointDefinition->psClusterInstance[i].bIsServer,
                         FALSE);
        }
    }
       
    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleAPSACKEnablementQueryRequest
 **
 ** DESCRIPTION:
 ** Handles an APS ACK Enablement Query Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleAPSACKEnablementQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_APSACKEnablementQueryResponsePayload sResponse = { 0 };
    tsZCL_Address sZCL_Address;
    uint8 u8ClusterControlFlags = 0;
    int i = 0;
    uint16 au16ClusterId[CLD_WWAH_NO_OF_CLUSTERS];
    
    sResponse.pu16ClusterId = (uint16*)au16ClusterId;
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandAPSACKEnablementQueryRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    /* ToDo: Shweta need to do it for all endpoint */

    for(i = 0; i < psEndPointDefinition->u16NumberOfClusters; i++)
    {
        u8ClusterControlFlags = u8ZCL_GetClusterControlFlags(psEndPointDefinition->u8EndPointNumber,
                                                            psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum,
                                                            psEndPointDefinition->psClusterInstance[i].bIsServer);


        if(((u8ClusterControlFlags != 0xFF) && !(u8ClusterControlFlags & CLUSTER_APS_ACK_ENABLE_MASK)) || psZCL_Common->bDisableAPSACK)
        {
            sResponse.pu16ClusterId[sResponse.u8NumberOfClustersToExempt++] = psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum;
        }
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uRespMessage.psAPSACKEnablementQueryResponsePayload = &sResponse;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_WWAHCommandAPSACKEnablementQueryResponseSend(
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &u8TransactionSequenceNumber,
                    &sResponse);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDebugReportQueryRequest
 **
 ** DESCRIPTION:
 ** Handles a Debug Report Query Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDebugReportQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_DebugReportQueryRequestPayload  sRequest  = { 0 };
    tsCLD_WWAH_DebugReportQueryResponsePayload sResponse = { 0 };
    tsZCL_Address sZCL_Address;

    uint8 au8ReportData[CLD_WWAH_REPORT_DATA_LENGTH];
    sResponse.pu8ReportData = (uint8*)au8ReportData;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandDebugReportQueryRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psDebugReportQueryRequestPayload = &sRequest;
    CallBackMessage.uRespMessage.psDebugReportQueryResponsePayload = &sResponse;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    if(((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->u8CurrentDebugReportID != sRequest.u8DebugReportID)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        eStatus = E_ZCL_FAIL;
    }
    else
    {
        // build address structure
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

        eStatus = eCLD_WWAHCommandDebugReportQueryResponseSend(
                        pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                        pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                        &sZCL_Address,
                        &u8TransactionSequenceNumber,
                        &sResponse);
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleUseTrustCenterForClusterServerRequest
 **
 ** DESCRIPTION:
 ** Handles a Use Trust Center For OTA Server Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleUseTrustCenterForClusterServerRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_UseTCForClusterServerPayload  sRequest  = { 0 };
    uint16 au16ClusterId[CLD_WWAH_NO_OF_CLUSTERS];
    uint32 i = 0;
    int j = 0;
    uint16 u16NwkAddress = 0;
    uint64 u64IEEEAdress = 0;
    ZPS_tsAplAib * psAplAib  = ZPS_psAplAibGetAib();
    
    sRequest.pu16ClusterId=(uint16*)au16ClusterId;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandUseTrustCenterForClusterServerRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psUseTCForClusterServerPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    /* Copy the cluster list into Custom Data Structure */
    if(sRequest.u8NumberOfClusters <= CLD_WWAH_NO_OF_CLUSTERS)
    {
        psCommon->u8NumberOfTCForServerCluster = sRequest.u8NumberOfClusters;
        memcpy(&psCommon->au16TCForClusterServer[0],&au16ClusterId[0],(sRequest.u8NumberOfClusters * sizeof(uint16)));
    }
    
    /* In case any bindings were created remove it and recreate the bidning for trust center */
    for( i = 0 ; i < psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable ; i++ )
    {
        for(j = 0 ; j < psCommon->u8NumberOfTCForServerCluster; j++)
        {
            if ( (psCommon->au16TCForClusterServer[j] == psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[i].u16ClusterId) &&
                     (psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[i].u8DstAddrMode == E_ZCL_AM_IEEE))
            {
                u64IEEEAdress = ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(), psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[i].u16AddrOrLkUp);
                u16NwkAddress = ZPS_u16AplZdoLookupAddr(u64IEEEAdress);

                if(u64IEEEAdress != ZPS_eAplAibGetApsTrustCenterAddress())
                {
                    ZPS_eAplZdoUnbind(psCommon->au16TCForClusterServer[j],
                                     pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     u16NwkAddress,
                                     u64IEEEAdress,
                                     psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[i].u8DestinationEndPoint);


                    /* Use Trust Center Values for binding */
                    
                    u64IEEEAdress = ZPS_eAplAibGetApsTrustCenterAddress();
                    u16NwkAddress = ZPS_u16AplZdoLookupAddr(u64IEEEAdress);                               
                    ZPS_eAplZdoBind(psCommon->au16TCForClusterServer[j],
                                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                    u16NwkAddress,
                                    u64IEEEAdress,
                                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint);    
                }                                    
                break;
            }
        }
        j = 0;

    }
                  
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleSurveyBeaconsRequest
 **
 ** DESCRIPTION:
 ** Handles a Survey Beacons Command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleSurveyBeaconsRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_SurveyBeaconsPayload  sRequest  = { 0 };

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandSurveyBeaconsReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sRequest);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    psCommon->u8BeaconSurveyTransSeqNum  = u8TransactionSequenceNumber;
    
    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uReqMessage.psSurveyBeaconsPayload = &sRequest;

    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Extended Beacon not supported return from here */
    if(sRequest.bStandardBeacons == FALSE)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return (E_ZCL_FAIL);
    }

    /* Todo Shweta: Need to store Beacons somewhere */
    ZPS_eAplZdoSurveyBeacons(0x07FFF800);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableOTADowngradesRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable OTA Downgrades Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableOTADowngradesRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisableOTADowngradesRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bDisableOtaDowngrades = TRUE;

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableMGMTLeaveWithoutRejoinRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable MGMT Leave Without Rejoin Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableMGMTLeaveWithoutRejoinRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    /* Shweta ToDo Check what should be value of this ?? */
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bMgmtLeaveWithoutRejoinEnabled = FALSE;

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableTouchlinkInterpanMessageRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable Touchlink Interpan message Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableTouchlinkInterpanMessageRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bTouchlinkInterpanEnabled = FALSE;

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableWWAHParentClassificationRequest
 **
 ** DESCRIPTION:
 ** Handles an Enable WWAH Parent Classification Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableWWAHParentClassificationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableWWAHParentClassificationRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHParentClassificationEnabled = TRUE;
    vCLD_WWAHSetParentClassificationFilter(TRUE);

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableParentClassificationRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable Parent Classification Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableParentClassificationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandDisableParentClassificationRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHParentClassificationEnabled = FALSE;
    vCLD_WWAHSetParentClassificationFilter(FALSE);

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableTCSecurityonNwkKeyRotationRequest
 **
 ** DESCRIPTION:
 ** Handles an Enable TC Security on Ntwk Key Rotation Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableTCSecurityonNwkKeyRotationRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    uint8 u8BitMaskSet = 0;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableTCSecurityonNwkKeyRotationRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bTCSecurityOnNwkKeyRotationEnabled = TRUE;
    
    /* Set the Bit mask for Configuration mode so that the commands are disabled */
    u8BitMaskSet = ZPS_u8AplExtdedAibGetWWAH();
    u8BitMaskSet |= ZPS_APL_WWAH_KEY_ROTATION;
    ZPS_vAplExtdedAibSetWWAH(u8BitMaskSet);

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableBadParentRecoveryRequest
 **
 ** DESCRIPTION:
 ** Handles a Enable Bad Parent Recovery Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableBadParentRecoveryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableBadParentRecoveryRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHBadParentRecoveryEnabled = TRUE;

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableWWAHBadParentRecoveryRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable WWAH Bad Parent Recovery Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableWWAHBadParentRecoveryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHDisableWWAHBadParentRecoveryRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bWWAHBadParentRecoveryEnabled = FALSE;

    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleEnableConfigurationModeRequest
 **
 ** DESCRIPTION:
 ** Handles a Enable Configuration Mode Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleEnableConfigurationModeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    uint8 u8BitMaskSet = 0;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;  
 
    /* Receive the command */
    eStatus = eCLD_WWAHCommandEnableConfigurationModeRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bConfigurationModeEnabled = TRUE;
    
    /* Clear the Bit mask for Configuration mode so that the commands are disabled */
    u8BitMaskSet = ZPS_u8AplExtdedAibGetWWAH();
    u8BitMaskSet &= ~ZPS_APL_WWAH_CONFIG_MODE;
    ZPS_vAplExtdedAibSetWWAH(u8BitMaskSet);

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDisableConfigurationModeRequest
 **
 ** DESCRIPTION:
 ** Handles a Disable Configuration Mode Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleDisableConfigurationModeRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    uint8 u8BitMaskSet = 0;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_WWAHDisableConfigurationModeRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }
    
    ((tsCLD_WWAH *)psClusterInstance->pvEndPointSharedStructPtr)->bConfigurationModeEnabled = FALSE;

    /* Set the Bit mask for Configuration mode so that the commands are disabled */
    u8BitMaskSet = ZPS_u8AplExtdedAibGetWWAH();
    u8BitMaskSet |= ZPS_APL_WWAH_CONFIG_MODE;
    ZPS_vAplExtdedAibSetWWAH(u8BitMaskSet);

     /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleTrustCenterForClusterServerQueryRequest
 **
 ** DESCRIPTION:
 ** Handles a Trust Center for Cluster Server Query Request command
 **
 ** PARAMETERS:               Name                       Usage
 ** ZPS_tsAfEvent             *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition  *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance     *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier        Command ID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eCLD_WWAHHandleTrustCenterForClusterServerQueryRequest(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_TCForClusterServerQueryResponsePayload sResponse = { 0 };
    tsZCL_Address sZCL_Address;
    uint16 au16ClusterId[CLD_WWAH_NO_OF_CLUSTERS];
    
    sResponse.pu16ClusterId =(uint16*)au16ClusterId;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber);
                    
    if (eStatus != E_ZCL_SUCCESS) {
        DBG_vPrintf(TRACE_WWAH, "Error: %d", eStatus);
        return (E_ZCL_FAIL);
    }

    sResponse.u8NumberOfClusters = psCommon->u8NumberOfTCForServerCluster;
    memcpy(&sResponse.pu16ClusterId[0],&psCommon->au16TCForClusterServer[0],(sResponse.u8NumberOfClusters * sizeof(uint16)));
    
    /* Message data for callback */
    psCommon->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_WWAH; 

    tsCLD_WWAHCallBackMessage CallBackMessage;
    tsCLD_WWAHCallBackMessage *psCallBackMessage = &CallBackMessage;
    CallBackMessage.u8CommandId = u8CommandIdentifier;
    CallBackMessage.uRespMessage.psTCForClusterServerQueryResponsePayload = &sResponse;
	
    psCommon->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData=(void *)psCallBackMessage;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_WWAHCommandTrustCenterForClusterServerQueryResponseSend(
                    pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                    &sZCL_Address,
                    &u8TransactionSequenceNumber,
                    &sResponse);


    return eStatus;
}

#endif /* WWAH_SERVER */

#ifdef WWAH_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryResponse
 **
 ** DESCRIPTION:
 ** Handles an APS Link Key Authorization Query  Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandleAPSLinkKeyAuthorizationQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload sResponse = {0};

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psAPSLinkKeyAuthorizationQueryResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandlePoweringOnOffNotification
 **
 ** DESCRIPTION:
 ** Handles a Powering On Notification Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandlePoweringOnOffNotification(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_PoweringOnOffNotificationPayload sResponse = {0};
    uint8 au8Data[CLD_WWAH_MAX_ZB_CMD_PAYLOAD_LENGTH] = {0};
    sResponse.pu8ManuPowNotfReason = (uint8*)&au8Data[0];

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandPoweringOnOffNotificationReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
                    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandPoweringOnOffNotificationReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psPoweringOnOffNotificationPayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleShortAddressChange
 **
 ** DESCRIPTION:
 ** Handles a Short Address Change Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandleShortAddressChange(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_ShortAddressChangePayload sResponse;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandShortAddressChangeReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandShortAddressChangeReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psShortAddressChangePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleAPSACKEnablementQueryResponse
 **
 ** DESCRIPTION:
 ** Handles an APS ACK Enablement Query Change Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandleAPSACKEnablementQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_APSACKEnablementQueryResponsePayload sResponse = {0};
    uint16 au16ClusterId[CLD_WWAH_NO_OF_CLUSTERS];

    sResponse.pu16ClusterId = (uint16*)au16ClusterId;
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandAPSACKEnablementQueryResponseReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandAPSACKEnablementQueryResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psAPSACKEnablementQueryResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandlPowerDescriptorChangeQuery
 **
 ** DESCRIPTION:
 ** Handles a Power Descriptor Change Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandlPowerDescriptorChangeQuery(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_PowerDescriptorChangePayload sResponse;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandPowerDescriptorChangeReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
                    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandPowerDescriptorChangeReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psPowerDescriptorChangePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleDebugReportQueryResponse
 **
 ** DESCRIPTION:
 ** Handles a Debug Report Query Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandleDebugReportQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_DebugReportQueryResponsePayload sResponse = {0};

    uint8 au8ReportData[CLD_WWAH_REPORT_DATA_LENGTH];
    sResponse.pu8ReportData = (uint8*)au8ReportData;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandDebugReportQueryResponseReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
                    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandDebugReportQueryResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psDebugReportQueryResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleHandleNewDebugReportNotification
 **
 ** DESCRIPTION:
 ** Handles an New Debug Report Notification command
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
PRIVATE teZCL_Status eCLD_WWAHHandleHandleNewDebugReportNotification(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAHNewDebugReportNotificationPayload sResponse;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHNewDebugReportNotificationReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);
                    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHNewDebugReportNotificationReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psNewDebugReportNotificationPayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleTrustCenterForClusterQueryResponse
 **
 ** DESCRIPTION:
 ** Handle Trust Center for Cluster Server Query Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandleTrustCenterForClusterQueryResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_TCForClusterServerQueryResponsePayload sResponse = {0};

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandTrustCenterForClusterQueryResponseReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponse);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandTrustCenterForClusterQueryResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psTCForClusterServerQueryResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHHandleSurveyBeaconsResponse
 **
 ** DESCRIPTION:
 ** Handle Survey Beacons Response command
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
PRIVATE teZCL_Status eCLD_WWAHHandleSurveyBeaconsResponse(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    int i = 0, j = 0;
    tsCLD_WWAH_CustomDataStructure *psCommon;
    tsCLD_WWAH_SurveyBeaconsResponsePayload sResponse = {0};
    tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal sResponseInternal = {0};
    uint8 au8BeaconSurvey[(CLD_WWAH_MAX_SURVEY_BEACONS * 4)] = {0};
    tsCLD_WWAH_BeaconSurvey ausBeaconSurvey[CLD_WWAH_MAX_SURVEY_BEACONS];
    
    
    sResponseInternal.pu8BeaconSurvey = &au8BeaconSurvey[0];
    sResponse.psBeaconSurvey = &ausBeaconSurvey[0];
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_WWAH_CustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_WWAHCommandSurveyBeaconsResponseReceive(
                    pZPSevent,
                    &u8TransactionSequenceNumber,
                    &sResponseInternal);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WWAH, "\nErr: eCLD_WWAHCommandSurveyBeaconsResponseReceive:%d", eStatus);
        return eStatus;
    }

    sResponse.u8NumberOfBeacons = sResponseInternal.u8NumberOfBeacons;
    for(i = 0; i < sResponse.u8NumberOfBeacons ; i++)
    {
        memcpy(&sResponse.psBeaconSurvey[i].u16ShortAddress,&au8BeaconSurvey[j],sizeof(uint16));
        j += 2;
        sResponse.psBeaconSurvey[i].u8RSSI = au8BeaconSurvey[j++];
        sResponse.psBeaconSurvey[i].u8ClassifictionMask = au8BeaconSurvey[j++];
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psSurveyBeaconsResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;

}
#endif /* WWAH_CLIENT */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
