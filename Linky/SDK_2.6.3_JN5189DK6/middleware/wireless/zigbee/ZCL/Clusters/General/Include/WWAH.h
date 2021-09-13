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
 * COMPONENT:          WWAH.h
 *
 * DESCRIPTION:        Header for WWAH Cluster
 *
 *****************************************************************************/

#ifndef WWAH_H
#define WWAH_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_WWAH                        0xFC57

#ifndef CLD_WWAH_NO_OF_CLUSTERS
    #define CLD_WWAH_NO_OF_CLUSTERS                     10
#endif

#ifndef CLD_WWAH_REPORT_DATA_LENGTH
    #define CLD_WWAH_REPORT_DATA_LENGTH                 64
#endif

#ifndef CLD_WWAH_MAX_ZB_CMD_PAYLOAD_LENGTH
    #define CLD_WWAH_MAX_ZB_CMD_PAYLOAD_LENGTH          65
#endif    

#ifndef CLD_WWAH_MAX_SURVEY_BEACONS
    #define CLD_WWAH_MAX_SURVEY_BEACONS                 20
#endif


#define WWAH_NODE_POWER_DESCRIPTOR_BITMASK_CURRENT_POWER_MODE               (1 << 0)
#define WWAH_NODE_POWER_DESCRIPTOR_BITMASK_AVAILABLE_POWER_SOURCE           (1 << 4)
#define WWAH_NODE_POWER_DESCRIPTOR_BITMASK_CURRENT_POWER_SOURCE             (1 << 8)
#define WWAH_NODE_POWER_DESCRIPTOR_BITMASK_CURRENT_POWER_SOURCE_LEVEL       (1 << 12)
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* WWAH Command - Payload */
typedef enum
{
    E_CLD_WWAH_CMD_ENABLE_APS_LINK_KEY_AUTHORIZATION                              = 0x0,        /* Mandatory */
    E_CLD_WWAH_CMD_DISABLE_APS_LINK_KEY_AUTHORIZATION,
    E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY,
    E_CLD_WWAH_CMD_REQUEST_NEW_APS_LINK_KEY,
    E_CLD_WWAH_CMD_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM,
    E_CLD_WWAH_CMD_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM,
    E_CLD_WWAH_CMD_REQUEST_TIME,
    E_CLD_WWAH_CMD_ENABLE_WWAH_REJOIN_ALGORITHM,
    E_CLD_WWAH_CMD_DISABLE_WWAH_REJOIN_ALGORITHM,
    E_CLD_WWAH_CMD_SET_IAS_ZONE_ENROLLMENT_METHOD,
    E_CLD_WWAH_CMD_CLEAR_BINDING_TABLE,
    E_CLD_WWAH_CMD_ENABLE_PERIODIC_ROUTER_CHECK_INS,
    E_CLD_WWAH_CMD_DISABLE_PERIODIC_ROUTER_CHECK_INS,
    E_CLD_WWAH_CMD_SET_MAC_POLL_CCA_WAIT_TIME,
    E_CLD_WWAH_CMD_SET_PENDING_NETWORK_UPDATE,
    E_CLD_WWAH_CMD_REQUIRE_APS_ACKS_ON_UNICAST,
    E_CLD_WWAH_CMD_REMOVE_APS_ACKS_ON_UNICAST,
    E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY,
    E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY,
    E_CLD_WWAH_CMD_SURVEY_BEACONS,
    E_CLD_WWAH_CMD_DISABLE_OTA_DOWNGRADES,
    E_CLD_WWAH_CMD_DISBALE_MGMT_LEAVE_WITHOUT_REJOIN,
    E_CLD_WWAH_CMD_DISABLE_TOUCHLINK_INTERPAN_MESSAGE,
    E_CLD_WWAH_CMD_ENABLE_WWAH_PARENT_CLASSIFICATION,
    E_CLD_WWAH_CMD_DISABLE_WWAH_PARENT_CLASSIFICATION,
    E_CLD_WWAH_CMD_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION,
    E_CLD_WWAH_CMD_ENABLE_WWAH_BAD_PARENT_RECOVERY,
    E_CLD_WWAH_CMD_DISABLE_WWAH_BAD_PARENT_RECOVERY,
    E_CLD_WWAH_CMD_ENABLE_CONFIGURATION_MODE,
    E_CLD_WWAH_CMD_DISABLE_CONFIGURATION_MODE,
    E_CLD_WWAH_CMD_USE_TRUST_CENTER_FOR_CLUSTER_SERVER,
    E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY
} teCLD_WWAH_Request;

typedef enum
{
    E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE,
    E_CLD_WWAH_CMD_POWERING_OFF_NOTIFICATION,
    E_CLD_WWAH_CMD_POWERING_ON_NOTIFICATION,
    E_CLD_WWAH_CMD_SHORT_ADDRESS_CHANGE,
    E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY_RESPONSE,
    E_CLD_WWAH_CMD_POWER_DESCRIPTOR_CHANGE,
    E_CLD_WWAH_CMD_NEW_DEBUG_REPORT_NOTIFICATION,
    E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY_RESPONSE,
    E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE,
    E_CLD_WWAH_CMD_SURVEY_BEACON_RESPONSE,
}teCLD_WWAH_Response;

typedef enum
{
    /* WWAH attribute set attribute ID's */
    E_CLD_WWAH_ATTR_ID_VERSION                                  = 0x0000,       /* 0.Mandatory */
    E_CLD_WWAH_ATTR_ID_DISABLE_OTA_DOWNGRADE                    = 0x0002,       /* 2.Mandatory */
    E_CLD_WWAH_ATTR_ID_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED,                       /* 3.Mandatory */
    E_CLD_WWAH_ATTR_ID_NWK_RETRY_COUNT,                                         /* 4.Mandatory*/
    E_CLD_WWAH_ATTR_ID_MAC_RETRY_COUNT,                                         /* 5.Mandatory*/
    E_CLD_WWAH_ATTR_ID_ROUTER_CHECKIN_ENABLED,                                  /* 6.Mandatory*/
    E_CLD_WWAH_ATTR_ID_TOUCHLINK_INTERPAN_ENABLED,                              /* 7.Mandatory*/
    E_CLD_WWAH_ATTR_ID_WWAH_PARENT_CLASSIFICATION_ENABLED,                      /* 8.Mandatory*/
    E_CLD_WWAH_ATTR_ID_WWAH_APP_EVENT_RETRY_ENABLED,                            /* 9.Mandatory*/
    E_CLD_WWAH_ATTR_ID_WWAH_APP_EVENT_RETRY_QUEUE_SIZE,                         /* 10.Mandatory*/
    E_CLD_WWAH_ATTR_ID_WWAH_REJOIN_ENABLED,                                     /* 11.Mandatory*/
    E_CLD_WWAH_ATTR_ID_MAC_POLL_CCA_WAIT_TIME,                                  /* 12.Mandatory*/
    E_CLD_WWAH_ATTR_ID_CONFIGURATION_MODE_ENABLED,                              /* 13.Mandatory*/
    E_CLD_WWAH_ATTR_ID_CURRENT_DEBUG_REPORT_ID,                                 /* 14.Mandatory*/
    E_CLD_WWAH_ATTR_ID_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED,                 /* 15.Mandatory*/
    E_CLD_WWAH_ATTR_ID_WWAH_BAD_PARENT_RECOVERY_ENABLED,                        /* 16.Mandatory*/
    E_CLD_WWAH_ATTR_ID_PENDING_NETWORK_UPDATE_CHANNEL,                          /* 17.Mandatory*/
    E_CLD_WWAH_ATTR_ID_PENDING_NETWORK_UPDATE_PANID,                            /* 18.Mandatory*/
    E_CLD_WWAH_ATTR_ID_OTA_MAX_OFFLINE_DURATION,                                /* 19.Mandatory*/
} teCLD_WWAH_Cluster_AttrID;

typedef  enum
{
    E_CLD_WWAH_MODE_TRIP_TO_PAIR,
    E_CLD_WWAH_MODE_AUTO_ENROLL_RESPONSE,
    E_CLD_WWAH_MODE_AUTO_ENROLL_REQUEST
}teCLD_WWAH_EnrollmentMode;

typedef enum
{
    E_CLD_WWAH_REASON_UNKNOWN,
    E_CLD_WWAH_REASON_BATTERY,
    E_CLD_WWAH_REASON_BROWNOUT,
    E_CLD_WWAH_REASON_WATCHDOG,
    E_CLD_WWAH_REASON_RESETPIN,
    E_CLD_WWAH_REASON_MEMORY_HW_FAULT,
    E_CLD_WWAH_REASON_SW_EXCEPTION,
    E_CLD_WWAH_REASON_OTA_BOOTLOAD_SUCCESS,
    E_CLD_WWAH_REASON_SW_RESET,
    E_CLD_WWAH_REASON_POWER_BUTTON,
    E_CLD_WWAH_REASON_TEMPERATURE,
    E_CLD_WWAH_REASON_BOOTLOAD_FAILURE
}teCLD_WWAH_PowerNotfReason;

/* WWAH cluster */
typedef struct
{
#ifdef WWAH_SERVER
    zuint16                  u16Version;
    zbool                    bDisableOtaDowngrades;
    zbool                    bMgmtLeaveWithoutRejoinEnabled;
    zuint8                   u8NwkRetryCount;
    zuint8                   u8MACRetryCount;
    zbool                    bRouterCheckInEnabled;
    zbool                    bTouchlinkInterpanEnabled;
    zbool                    bWWAHParentClassificationEnabled;
    zbool                    bWWAHAppEventRetryEnabled;
    zuint8                   u8WWAHAppEventRetryQueueSize;
    zbool                    bWWAHRejoinEnabled;
    zuint8                   u8MACPollCCAWaitTime;
    zbool                    bConfigurationModeEnabled;
    zuint8                   u8CurrentDebugReportID;
    zbool                    bTCSecurityOnNwkKeyRotationEnabled;
    zbool                    bWWAHBadParentRecoveryEnabled;
    zuint8                   u8PendingNetworkUpdateChannel;
    zuint16                  u16PendingNetworkUpdatePANID;
    zuint16                  u16OtaMaxOfflineDuration;
#endif
} tsCLD_WWAH;

/* Commands Received by WWAH Server Cluster */
typedef struct
{
    zuint8              u8NumberOfClustersToExempt;
    zuint16             *pu16ClusterId;
}tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload;

typedef struct
{
    zuint16             u16ClusterID;
}tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload;

typedef struct
{
    zuint8              u8FirstBackoffTime;
    zuint8              u8BackoffSequenceCommonRatio;
    zuint32             u32MaxBackoffTime;
    zuint8              u8MaxReDeliveryAttempts;
}tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload;

typedef struct
{
    zuint16             u16FastRejoinTimeout;
    zuint16             u16DurationBetweenEachRejoin;
    zuint16             u16FastRejoinFirstBackoff;
    zuint16             u16MaxBackoffTime;
    zuint16             u16MaxBackoffIterations;
}tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload;

typedef struct
{
    zenum8              eEnrollmentMode;
}tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload;

typedef struct
{
    zuint16             u16CheckInInterval;
}tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload;

typedef struct
{
    zuint8              u8WaitTime;
}tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload;

typedef struct
{
    zuint8              u8Channel;
    zuint16             u16PANID;
}tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload;

typedef struct
{
    zuint8              u8NumberOfClustersToExempt;
    zuint16             *pu16ClusterId;
}tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload;

typedef struct
{
    zbool               bStandardBeacons;
}tsCLD_WWAH_SurveyBeaconsPayload;

typedef struct
{
    zuint8              u8DebugReportID;
}tsCLD_WWAH_DebugReportQueryRequestPayload;

typedef struct
{
    zuint8              u8NumberOfClusters;
    zuint16             *pu16ClusterId;
}tsCLD_WWAH_UseTCForClusterServerPayload;


/* Commands Generated by WWAH Server Cluster */
typedef struct
{
    zuint16             u16ClusterID;
    zbool               bAPSLinkKeyAuthorizationStatus;
}tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload;

typedef struct
{
    zenum8              ePowNotiReason;
    zuint16             u16ManuID;
    zuint8              u8ManuPowNotfReasonLen;
    uint8               *pu8ManuPowNotfReason;
}tsCLD_WWAH_PoweringOnOffNotificationPayload;

typedef struct
{
    zieeeaddress        u64DeviceEUI64;
    zuint16             u16DeviceShort;
}tsCLD_WWAH_ShortAddressChangePayload;

typedef struct
{
    zuint8              u8NumberOfClustersToExempt;
    zuint16             *pu16ClusterId;
}tsCLD_WWAH_APSACKEnablementQueryResponsePayload;

typedef struct
{
    zbmap16              b16NodeDescriptor;
}tsCLD_WWAH_PowerDescriptorChangePayload;

typedef struct
{
    zuint8              u8DebugReportID;
    zuint8              *pu8ReportData;
}tsCLD_WWAH_DebugReportQueryResponsePayload;

typedef struct
{
    zuint8              u8DebugReportID;
    zuint32             u32SizeOfReport;
}tsCLD_WWAHNewDebugReportNotificationPayload;

typedef struct
{
    zuint8              u8NumberOfClusters;
    zuint16             *pu16ClusterId;
}tsCLD_WWAH_TCForClusterServerQueryResponsePayload;

typedef struct
{
    zuint16             u16ShortAddress;
    zuint8              u8RSSI;
    uint8               u8ClassifictionMask;
}tsCLD_WWAH_BeaconSurvey;

typedef struct
{
    zuint8                          u8NumberOfBeacons;
    tsCLD_WWAH_BeaconSurvey         *psBeaconSurvey;
}tsCLD_WWAH_SurveyBeaconsResponsePayload;

/* Definition of WWAH Callback Event Structure */
typedef struct
{
    uint8               u8CommandId;
    union
    {
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload     *psEnableOrDisableAPSLinkKeyAuthorizationRequestPayload;
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload               *psAPSLinkKeyAuthorizationQueryRequestPayload;
        tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload           *psEnableWWAHAppEventRetryAlgorithmRequestPayload;
        tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload                  *psEnableWWAHRejoinAlgorithmRequestPayload;
        tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload                 *psSetIASZoneEnrollmentMethodRequestPayload;
        tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload               *psEnablePeriodicRouterCheckInsRequestPayload;
        tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload                      *psSetMACPollCCAWaitTimeRequestPayload;
        tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload                    *psSetPendingNetworkUpdateRequestPayload;
        tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload                   *psRequireAPSACKsOnUnicastsRequestPayload;
        tsCLD_WWAH_DebugReportQueryRequestPayload                           *psDebugReportQueryRequestPayload;
        tsCLD_WWAH_SurveyBeaconsPayload                                     *psSurveyBeaconsPayload;
        tsCLD_WWAH_UseTCForClusterServerPayload                             *psUseTCForClusterServerPayload;
    } uReqMessage;
    union
    {
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload              *psAPSLinkKeyAuthorizationQueryResponsePayload;
        tsCLD_WWAH_PoweringOnOffNotificationPayload                         *psPoweringOnOffNotificationPayload;
        tsCLD_WWAH_ShortAddressChangePayload                                *psShortAddressChangePayload;
        tsCLD_WWAH_APSACKEnablementQueryResponsePayload                     *psAPSACKEnablementQueryResponsePayload;
        tsCLD_WWAH_PowerDescriptorChangePayload                             *psPowerDescriptorChangePayload;
        tsCLD_WWAH_DebugReportQueryResponsePayload                          *psDebugReportQueryResponsePayload;
        tsCLD_WWAHNewDebugReportNotificationPayload                         *psNewDebugReportNotificationPayload;
        tsCLD_WWAH_TCForClusterServerQueryResponsePayload                   *psTCForClusterServerQueryResponsePayload;
        tsCLD_WWAH_SurveyBeaconsResponsePayload                             *psSurveyBeaconsResponsePayload;
    } uRespMessage;
} tsCLD_WWAHCallBackMessage;

/* Custom data structure */
typedef struct
{
    tsZCL_ReceiveEventAddress     sReceiveEventAddress;
    tsZCL_CallBackEvent           sCustomCallBackEvent;
    tsCLD_WWAHCallBackMessage     sCallBackMessage;
#if (defined CLD_WWAH) && (defined WWAH_SERVER)    
    uint8                         u8NumberOfTCForServerCluster;
    uint8                         u8BeaconSurveyTransSeqNum;
    uint16                        au16TCForClusterServer[CLD_WWAH_NO_OF_CLUSTERS];
#endif    
} tsCLD_WWAH_CustomDataStructure;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_WWAHCreateWWAH(
        tsZCL_ClusterInstance                       *psClusterInstance,
        bool_t                                      bIsServer,
        tsZCL_ClusterDefinition                     *psClusterDefinition,
        void                                        *pvEndPointSharedStructPtr,
        uint8                                       *pu8AttributeControlBits,
        tsCLD_WWAH_CustomDataStructure              *psCustomDataStructure);

# ifdef WWAH_CLIENT
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableAPSLinkKeyAuthorizationRequestSend(
        uint8                                                            u8SourceEndPointId,
        uint8                                                            u8DestinationEndPointId,
        tsZCL_Address                                                    *psDestinationAddress,
        uint8                                                            *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload  *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableAPSLinkKeyAuthorizationRequestSend(
        uint8                                                            u8SourceEndPointId,
        uint8                                                            u8DestinationEndPointId,
        tsZCL_Address                                                    *psDestinationAddress,
        uint8                                                            *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload  *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload       *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandRequestNewAPSLinkKeyRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload   *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandRequestTimeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHRejoinAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload          *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload         *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandClearBindingTableRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload       *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload              *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandSetPendingNetworkUpdateRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload            *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload   *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommanddDebugReportQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryRequestPayload                   *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandUseTrustCenterForClusterServerRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_UseTCForClusterServerPayload                     *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHParentClassificationRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnableTCSecurityonNwkKeyRotationRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHBadParentRecoveryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableConfigurationModeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);
        
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsPayload                             *psPayload);       

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableOTADowngradesRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandDisableParentClassificationRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnableBadParentRecoveryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);

PUBLIC  teZCL_Status eCLD_WWAHCommandEnableConfigurationModeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber);
#endif /* WWAH_CLIENT */

# ifdef WWAH_SERVER
PUBLIC  teZCL_Status eCLD_WWAHCommandPoweringOnNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandShortAddressChangeSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_ShortAddressChangePayload                        *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandPowerDescriptorChangeSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PowerDescriptorChangePayload                     *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandNewDebugReportNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAHNewDebugReportNotificationPayload                 *psPayload);

PUBLIC  teZCL_Status eCLD_WWAHCommandPoweringOffNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload);
        
PUBLIC  void vCLD_WWAHSetParentClassificationFilter(bool_t   bWWAHParentClassificationEnabled);
PUBLIC  void vCLD_WWAHHandleSurveyBeacons(uint8 u8SourceEndPointId);
#endif /* WWAH_SERVER */

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_WWAH;
extern const tsZCL_AttributeDefinition asCLD_WWAHClusterAttributeDefinitions[];
extern uint8 au8WWAHAttributeControlBits[];
#ifdef WWAH_CLIENT
extern uint8 au8WWAHClientAttributeControlBits[];
#endif
#endif /* WWAH_H */
