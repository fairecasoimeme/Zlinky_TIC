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
 * MODULE:             Door Lock Cluster
 *
 * COMPONENT:          WWAH_internal.h
 *
 * DESCRIPTION:        The internal API for the Door Lock Cluster
 *
 *****************************************************************************/

#ifndef  WWAH_INTERNAL_H_INCLUDED
#define  WWAH_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "WWAH.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
    zuint8                          u8NumberOfBeacons;
    zuint8                          *pu8BeaconSurvey;
}tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_WWAHCommandHandler(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance);

#ifdef WWAH_CLIENT
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload      *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandPoweringOnOffNotificationReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandShortAddressChangeReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_ShortAddressChangePayload                *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandPowerDescriptorChangeReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PowerDescriptorChangePayload             *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandDebugReportQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryResponsePayload                  *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHNewDebugReportNotificationReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAHNewDebugReportNotificationPayload                 *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_TCForClusterServerQueryResponsePayload           *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal             *psPayload);

#endif /* WWAH_CLIENT */

#ifdef WWAH_SERVER
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableOrDisableAPSLinkKeyAuthorizationRequestReceive(
        ZPS_tsAfEvent                                                     *pZPSevent,
        uint8                                                             *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload   *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload       *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandRequestNewAPSLinkKeyRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload   *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandRequestTimeRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHRejoinAlgorithmRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload          *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload         *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandClearBindingTableRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload       *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload              *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandSetPendingNetworkUpdateRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload            *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload           *psPayload);        
PUBLIC  teZCL_Status eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandDebugReportQueryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryRequestPayload                   *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandUseTrustCenterForClusterServerRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_UseTCForClusterServerPayload                     *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHParentClassificationRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableTCSecurityonNwkKeyRotationRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHDisableWWAHBadParentRecoveryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHDisableConfigurationModeRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsPayload                             *psPayload);  
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableOTADowngradesRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableParentClassificationRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableBadParentRecoveryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableConfigurationModeRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC teZCL_Status eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber);
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload      *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandDebugReportQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryResponsePayload                  *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandNewDebugReportNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAHNewDebugReportNotificationPayload                 *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterServerQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_TCForClusterServerQueryResponsePayload       *psPayload);
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal             *psPayload);        
#endif /* WWAH_SERVER */

#endif /* WWAH_INTERNAL_H_INCLUDED*/
