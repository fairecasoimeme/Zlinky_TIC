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


/*****************************************************************************
 *
 * MODULE:             WWAH Cluster
 *
 * COMPONENT:          WWAH.c
 *
 * DESCRIPTION:        WWAH cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "WWAH.h"
#include "WWAH_internal.h"
#include "appZpsBeaconHandler.h"

#include "dbg.h"

#ifdef DEBUG_CLD_WWAH
#define TRACE_WWAH    TRUE
#else
#define TRACE_WWAH    FALSE
#endif

#define BEACON_FILTER_LQI_VALUE          30
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_WWAH) && !defined(WWAH_SERVER) && !defined(WWAH_CLIENT)
#error You Must Have either WWAH_SERVER and/or WWAH_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_WWAH
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_WWAHClusterCommandDefinitions[] = {

#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_ENABLE_APS_LINK_KEY_AUTHORIZATION,              E_ZCL_CF_RX},/* Mandatory */
#else
    {E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE,      E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_DISABLE_APS_LINK_KEY_AUTHORIZATION,             E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_POWERING_OFF_NOTIFICATION,                      E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY,               E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_POWERING_ON_NOTIFICATION,                       E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_REQUEST_NEW_APS_LINK_KEY,                       E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_SHORT_ADDRESS_CHANGE,                           E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM,          E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY_RESPONSE,              E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM,         E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_POWER_DESCRIPTOR_CHANGE,                        E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_REQUEST_TIME,                                   E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_NEW_DEBUG_REPORT_NOTIFICATION,                  E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_ENABLE_WWAH_REJOIN_ALGORITHM,                   E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY_RESPONSE,                    E_ZCL_CF_TX},
#endif
#ifdef WWAH_SERVER
    {E_CLD_WWAH_CMD_DISABLE_WWAH_REJOIN_ALGORITHM,                  E_ZCL_CF_RX},
#else
    {E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE, E_ZCL_CF_TX},
#endif
    {E_CLD_WWAH_CMD_SET_IAS_ZONE_ENROLLMENT_METHOD,                 E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_CLEAR_BINDING_TABLE,                            E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_ENABLE_PERIODIC_ROUTER_CHECK_INS,               E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DISABLE_PERIODIC_ROUTER_CHECK_INS,              E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_SET_MAC_POLL_CCA_WAIT_TIME,                     E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_SET_PENDING_NETWORK_UPDATE,                     E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_REQUIRE_APS_ACKS_ON_UNICAST,                    E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_REMOVE_APS_ACKS_ON_UNICAST,                     E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY,                       E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY,                             E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_SURVEY_BEACONS,                                 E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DISABLE_OTA_DOWNGRADES,                         E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DISBALE_MGMT_LEAVE_WITHOUT_REJOIN,              E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_ENABLE_WWAH_PARENT_CLASSIFICATION,              E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DISABLE_WWAH_PARENT_CLASSIFICATION,             E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION,         E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_ENABLE_WWAH_BAD_PARENT_RECOVERY,                E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DISABLE_WWAH_BAD_PARENT_RECOVERY,               E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_ENABLE_CONFIGURATION_MODE,                      E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_DISABLE_CONFIGURATION_MODE,                     E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_USE_TRUST_CENTER_FOR_CLUSTER_SERVER,            E_ZCL_CF_RX},
    {E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY,          E_ZCL_CF_RX},
    };
#endif

const tsZCL_AttributeDefinition asCLD_WWAHClusterAttributeDefinitions[] = {
#ifdef WWAH_SERVER
    {E_CLD_WWAH_ATTR_ID_VERSION,                                    (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT16,       (uint32)(&((tsCLD_WWAH*)(0))->u16Version),0},                           /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_DISABLE_OTA_DOWNGRADE,                      (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bDisableOtaDowngrades),0},                /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED,          (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bMgmtLeaveWithoutRejoinEnabled),0},       /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_NWK_RETRY_COUNT,                            (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT8,        (uint32)(&((tsCLD_WWAH*)(0))->u8NwkRetryCount),0},                      /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_MAC_RETRY_COUNT,                            (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT8,        (uint32)(&((tsCLD_WWAH*)(0))->u8MACRetryCount),0},                      /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_ROUTER_CHECKIN_ENABLED,                     (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bRouterCheckInEnabled),0},                /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_TOUCHLINK_INTERPAN_ENABLED,                 (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bTouchlinkInterpanEnabled),0},            /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_WWAH_PARENT_CLASSIFICATION_ENABLED,         (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bWWAHParentClassificationEnabled),0},     /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_WWAH_APP_EVENT_RETRY_ENABLED,               (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bWWAHAppEventRetryEnabled),0},            /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_WWAH_APP_EVENT_RETRY_QUEUE_SIZE,            (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT8,        (uint32)(&((tsCLD_WWAH*)(0))->u8WWAHAppEventRetryQueueSize),0},         /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_WWAH_REJOIN_ENABLED,                        (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bWWAHRejoinEnabled),0},                   /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_MAC_POLL_CCA_WAIT_TIME,                     (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT8,        (uint32)(&((tsCLD_WWAH*)(0))->u8MACPollCCAWaitTime),0},                 /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_CONFIGURATION_MODE_ENABLED,                 (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bConfigurationModeEnabled),0},            /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_CURRENT_DEBUG_REPORT_ID,                    (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT8,        (uint32)(&((tsCLD_WWAH*)(0))->u8CurrentDebugReportID),0},               /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED,   (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,          (uint32)(&((tsCLD_WWAH*)(0))->bTCSecurityOnNwkKeyRotationEnabled),0},  /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_WWAH_BAD_PARENT_RECOVERY_ENABLED,           (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_BOOL,         (uint32)(&((tsCLD_WWAH*)(0))->bWWAHBadParentRecoveryEnabled),0},        /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_PENDING_NETWORK_UPDATE_CHANNEL,             (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT8,        (uint32)(&((tsCLD_WWAH*)(0))->u8PendingNetworkUpdateChannel),0},        /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_PENDING_NETWORK_UPDATE_PANID,               (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT16,       (uint32)(&((tsCLD_WWAH*)(0))->u16PendingNetworkUpdatePANID),0},         /* Mandatory */

    {E_CLD_WWAH_ATTR_ID_OTA_MAX_OFFLINE_DURATION,                   (E_ZCL_AF_RD|E_ZCL_AF_MS),                E_ZCL_UINT16,       (uint32)(&((tsCLD_WWAH*)(0))->u16OtaMaxOfflineDuration),0},             /* Mandatory */
#endif

 };

tsZCL_ClusterDefinition sCLD_WWAH = {
    GENERAL_CLUSTER_ID_WWAH,
    FALSE,
    E_ZCL_SECURITY_APPLINK,
    (sizeof(asCLD_WWAHClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
    (tsZCL_AttributeDefinition*)asCLD_WWAHClusterAttributeDefinitions,
    NULL,
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    (sizeof(asCLD_WWAHClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
    (tsZCL_CommandDefinition*)asCLD_WWAHClusterCommandDefinitions
#endif
};

uint8 au8WWAHAttributeControlBits[(sizeof(asCLD_WWAHClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
tsBeaconFilterType  sBeaconFilter;

#ifdef WWAH_CLIENT
uint8 au8WWAHClientAttributeControlBits[(sizeof(asCLD_WWAHClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCreateWWAH
 **
 ** DESCRIPTION:
 ** Creates a WWAH cluster
 **
 ** PARAMETERS:                 Name                                Usage
 ** tsZCL_ClusterInstance      *psClusterInstance                   Cluster structure
 ** bool_t                      bIsServer                           Server Client flag
 ** tsZCL_ClusterDefinition    *psClusterDefinition                 Cluster Definition
 ** void                       *pvEndPointSharedStructPtr           EndPoint Shared Structure
 ** uint8                      *pu8AttributeControlBits             Attribute Control Bits
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCreateWWAH(
        tsZCL_ClusterInstance                   *psClusterInstance,
        bool_t                                  bIsServer,
        tsZCL_ClusterDefinition                 *psClusterDefinition,
        void                                    *pvEndPointSharedStructPtr,
        uint8                                   *pu8AttributeControlBits,
        tsCLD_WWAH_CustomDataStructure          *psCustomDataStructure)
{
    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
           if((psClusterInstance==NULL) || (psClusterDefinition==NULL) || (psCustomDataStructure==NULL) )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    // cluster data
    vZCL_InitializeClusterInstance(
                       psClusterInstance,
                       bIsServer,
                       psClusterDefinition,
                       pvEndPointSharedStructPtr,
                       pu8AttributeControlBits,
                       psCustomDataStructure,
                       eCLD_WWAHCommandHandler);

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance;

    if(psClusterInstance->pvEndPointSharedStructPtr != NULL)
    {
#ifdef WWAH_SERVER
        /* Set attribute default values */
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u16Version = 0;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bDisableOtaDowngrades = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bMgmtLeaveWithoutRejoinEnabled = TRUE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u8NwkRetryCount = 0;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u8MACRetryCount = 0;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bRouterCheckInEnabled = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bTouchlinkInterpanEnabled = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bWWAHParentClassificationEnabled = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bWWAHAppEventRetryEnabled = TRUE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u8WWAHAppEventRetryQueueSize = 0x0A;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bWWAHRejoinEnabled = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u8MACPollCCAWaitTime = 0x03;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bConfigurationModeEnabled = TRUE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u8CurrentDebugReportID = 0;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bTCSecurityOnNwkKeyRotationEnabled = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->bWWAHBadParentRecoveryEnabled = FALSE;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u8PendingNetworkUpdateChannel = 0xFF;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u16PendingNetworkUpdatePANID = 0xFFFF;
        ((tsCLD_WWAH*)pvEndPointSharedStructPtr)->u16OtaMaxOfflineDuration = 0;

#endif /* WWAH_SERVER */
    }

    return E_ZCL_SUCCESS;

}

#ifdef WWAH_SERVER
/****************************************************************************
 **
 ** NAME:       vCLD_WWAHHandleSurveyBeacons
 **
 ** DESCRIPTION:
 ** Handles the Survey Beacon request and send a response back
 **
 ** PARAMETERS:                 Name                                Usage
 ** tsZCL_ClusterInstance      *psClusterInstance                   Cluster structure
 ** bool_t                      bIsServer                           Server Client flag
 ** tsZCL_ClusterDefinition    *psClusterDefinition                 Cluster Definition
 ** void                       *pvEndPointSharedStructPtr           EndPoint Shared Structure
 ** uint8                      *pu8AttributeControlBits             Attribute Control Bits
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PUBLIC  void vCLD_WWAHHandleSurveyBeacons(uint8 u8SourceEndPointId)
{
    uint16 i;
    int j = 0;
    uint16 u16MaxPayloadSize = 0;
    ZPS_tsNwkNib *psNib = ZPS_psAplZdoGetNib();
    tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal sResponse = {0};
    uint8 au8BeaconSurvey[(CLD_WWAH_MAX_SURVEY_BEACONS * 4)] = {0};
    tsZCL_Address sZCL_Address;
    uint8 u8TransactionSeqNumber = 0;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_WWAH_CustomDataStructure *psCommon;

    /* Find pointers to cluster */
    eZCL_FindCluster(GENERAL_CLUSTER_ID_WWAH, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);

    u8TransactionSeqNumber = psCommon->u8BeaconSurveyTransSeqNum;

    sResponse.pu8BeaconSurvey = &au8BeaconSurvey[0];
    
    u16MaxPayloadSize = u16ZCL_GetTxPayloadSize(ZPS_u16AplZdoLookupAddr(ZPS_eAplAibGetApsTrustCenterAddress()));

    for (i = 0; i < psNib->sTblSize.u8NtDisc; i++)
    {
        if((u16MaxPayloadSize < (j + 3)) || (sResponse.u8NumberOfBeacons > CLD_WWAH_MAX_SURVEY_BEACONS))
        {
            break;
        }
        ZPS_tsNwkDiscNtEntry *psDiscNtEntry = &psNib->sTbl.psNtDisc[i];
        if(psDiscNtEntry->uAncAttrs.bfBitfields.u1Used)
        {
            au8BeaconSurvey[j++] = (uint8)(psDiscNtEntry->u16NwkAddr);
            au8BeaconSurvey[j++] = (uint8)(psDiscNtEntry->u16NwkAddr >> 8);
            au8BeaconSurvey[j++] = psDiscNtEntry->u8LinkQuality;
            au8BeaconSurvey[j++] = psDiscNtEntry->uAncAttrs.bfBitfields.u2PriorityParent;
            sResponse.u8NumberOfBeacons++;
        }
    }

    sZCL_Address.eAddressMode = E_ZCL_AM_BOUND_NON_BLOCKING;
    
    eCLD_WWAHCommandSurveyBeaconsResponseSend(
                    u8SourceEndPointId,
                    0,
                    &sZCL_Address,
                    &u8TransactionSeqNumber,
                    &sResponse);
                    
    psCommon->u8BeaconSurveyTransSeqNum = 0;
}    

 /****************************************************************************
 *
 * NAME: vCLD_WWAHSetParentClassificationFilter
 *
 * DESCRIPTION:
 *  Sets Rejoin filters as per WWAH Parent Clasification
 *      a. Sets up an LQI filter to reject distant beacons
 *      b. filter on the Permit Join status to only find potential parents and
 *         networks that are accepting association requests
 *      d. filter on Router/End Device Capacity
 *      e. Filter on Parent Priority
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vCLD_WWAHSetParentClassificationFilter(bool_t   bWWAHParentClassificationEnabled)
{
    sBeaconFilter.pu64ExtendPanIdList = NULL;
    sBeaconFilter.u8ListSize = 0;
    sBeaconFilter.u8Lqi = BEACON_FILTER_LQI_VALUE;

    switch(ZPS_eAplZdoGetDeviceType())
    {
        case ZPS_ZDO_DEVICE_ENDDEVICE:
            sBeaconFilter.u16FilterMap = BF_BITMAP_CAP_ENDDEVICE | \
                                        BF_BITMAP_PERMIT_JOIN |\
                                        BF_BITMAP_LQI;
        break;
        
        case ZPS_ZDO_DEVICE_ROUTER:
            sBeaconFilter.u16FilterMap = BF_BITMAP_CAP_ROUTER | \
                                        BF_BITMAP_PERMIT_JOIN |\
                                        BF_BITMAP_LQI;
            break;

        default:
            //impossible!
            break;
    }
    
    if(bWWAHParentClassificationEnabled)
    {
       sBeaconFilter.u16FilterMap |= BF_BITMAP_PRIORITY_PARENT;
    }
    else
    {
       sBeaconFilter.u16FilterMap &= ~BF_BITMAP_PRIORITY_PARENT;
    }

    ZPS_bAppAddBeaconFilter(&sBeaconFilter);
}
#endif   /* # define WWAH_SERVER */
#endif   /* #define CLD_WWAH */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

