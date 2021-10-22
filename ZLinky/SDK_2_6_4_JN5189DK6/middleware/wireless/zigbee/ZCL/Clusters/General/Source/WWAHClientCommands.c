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
 * MODULE:             WWAH Lock Cluster
 *
 * COMPONENT:          WWAHClientCommands.c
 *
 * DESCRIPTION:        Send a WWAH cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "WWAH.h"
#include "WWAH_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"
#include "ZTimer.h"

//#include "app_main.h"

#ifdef DEBUG_CLD_WWAH
#else
#define TRACE_GROUPS    FALSE
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

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

#ifdef WWAH_CLIENT

/* Definitions of WWAH Client Cluster Request Send Command */

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableAPSLinkKeyAuthorizationRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enable APS Link Key Authorization Request command.
 **
 ** PARAMETERS:                                                         Name                            Usage
 ** uint8                                                               u8SourceEndPointId              Source EP Id
 ** uint8                                                               u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                                       *psDestinationAddress           Destination Address
 ** uint8                                                               *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload     *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableAPSLinkKeyAuthorizationRequestSend(
        uint8                                                               u8SourceEndPointId,
        uint8                                                               u8DestinationEndPointId,
        tsZCL_Address                                                       *psDestinationAddress,
        uint8                                                               *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload     *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,                                      E_ZCL_UINT8,            &psPayload->u8NumberOfClustersToExempt  },
            {   psPayload->u8NumberOfClustersToExempt,  E_ZCL_UINT16,           psPayload->pu16ClusterId                }
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_APS_LINK_KEY_AUTHORIZATION,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));
}

/***********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableAPSLinkKeyAuthorizationRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable APS Link Key Authorization Request command.
 **
 ** PARAMETERS:                                                         Name                            Usage
 ** uint8                                                               u8SourceEndPointId              Source EP Id
 ** uint8                                                               u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                                       *psDestinationAddress           Destination Address
 ** uint8                                                               *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload     *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableAPSLinkKeyAuthorizationRequestSend(
        uint8                                                               u8SourceEndPointId,
        uint8                                                               u8DestinationEndPointId,
        tsZCL_Address                                                       *psDestinationAddress,
        uint8                                                               *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload     *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,                                          E_ZCL_UINT8,            &psPayload->u8NumberOfClustersToExempt  },
            {   psPayload->u8NumberOfClustersToExempt,      E_ZCL_UINT16,           psPayload->pu16ClusterId                },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_APS_LINK_KEY_AUTHORIZATION,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an APS Link Key Authorization Query Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload       *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload       *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,              E_ZCL_UINT16,           &psPayload->u16ClusterID    }

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRequestNewAPSLinkKeyRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Request New Link Key Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRequestNewAPSLinkKeyRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_REQUEST_NEW_APS_LINK_KEY,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enable WWAH App Event Retry Algorithm Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload   *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload   *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,              E_ZCL_UINT8,            &psPayload->u8FirstBackoffTime              },
            {   1,              E_ZCL_UINT8,            &psPayload->u8BackoffSequenceCommonRatio    },
            {   1,              E_ZCL_UINT32,           &psPayload->u32MaxBackoffTime               },
            {   1,              E_ZCL_UINT8,            &psPayload->u8MaxReDeliveryAttempts         },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable WWAH App Event Retry Algorithm Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRequestTimeRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Request Time Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRequestTimeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_REQUEST_TIME,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}
/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableWWAHRejoinAlgorithmRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enable WWAH App Event Retry Algorithm Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload          *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHRejoinAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload          *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,              E_ZCL_UINT16,           &psPayload->u16FastRejoinTimeout            },
            {   1,              E_ZCL_UINT16,           &psPayload->u16DurationBetweenEachRejoin    },
            {   1,              E_ZCL_UINT16,           &psPayload->u16FastRejoinFirstBackoff       },
            {   1,              E_ZCL_UINT16,           &psPayload->u16MaxBackoffTime               },
            {   1,              E_ZCL_UINT16,           &psPayload->u16MaxBackoffIterations         },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_WWAH_REJOIN_ALGORITHM,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable WWAH App Event Retry Algorithm Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_WWAH_REJOIN_ALGORITHM,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Set IAS Zone Enrollment Method Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload         *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload         *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
             {  1,              E_ZCL_ENUM8,            &psPayload->eEnrollmentMode         },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_SET_IAS_ZONE_ENROLLMENT_METHOD,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandClearBindingTableRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Clear Binding Table Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandClearBindingTableRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_CLEAR_BINDING_TABLE,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enable Periodic Router Check-In Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload       *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload       *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
             {  1,              E_ZCL_UINT16,           &psPayload->u16CheckInInterval          },

     };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_PERIODIC_ROUTER_CHECK_INS,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable Periodic Router Check-Ins Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_PERIODIC_ROUTER_CHECK_INS,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Set MAC Poll CCA Wait Time Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload              *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload              *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
         {  1,              E_ZCL_UINT8,            &psPayload->u8WaitTime          },
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_SET_MAC_POLL_CCA_WAIT_TIME,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSetPendingNetworkUpdateRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Set Pending Network Update Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload            *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSetPendingNetworkUpdateRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload            *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,              E_ZCL_UINT8,            &psPayload->u8Channel           },
            {   1,              E_ZCL_UINT16,           &psPayload->u16PANID            },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_SET_PENDING_NETWORK_UPDATE,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/***********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Require APS ACKs on Unicasts Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload           *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload           *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,                                          E_ZCL_UINT8,            &psPayload->u8NumberOfClustersToExempt  },
            {   psPayload->u8NumberOfClustersToExempt,      E_ZCL_UINT16,           psPayload->pu16ClusterId                }

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_REQUIRE_APS_ACKS_ON_UNICAST,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Remove APS ACKs on Unicasts Requirement Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_REMOVE_APS_ACKS_ON_UNICAST,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSACKEnablementQueryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an APS ACK Enablement Query Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommanddDebugReportQueryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Debug Report Query Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_DebugReportQueryRequestPayload                   *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommanddDebugReportQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryRequestPayload                   *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,                                      E_ZCL_UINT8,            &psPayload->u8DebugReportID },
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));

}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandUseTrustCenterForClusterServerRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Use Trust Center For OTA Server Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_UseTCForClusterServerPayload                     *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC teZCL_Status eCLD_WWAHCommandUseTrustCenterForClusterServerRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_UseTCForClusterServerPayload                     *psPayload)
{
    tsZCL_TxPayloadItem psPayloadDefinition[] = {
            {   1,                                      E_ZCL_UINT8,            &psPayload->u8NumberOfClusters  },
            {   psPayload->u8NumberOfClusters,          E_ZCL_UINT16,           psPayload->pu16ClusterId        }
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_USE_TRUST_CENTER_FOR_CLUSTER_SERVER,
                                  pu8TransactionSequenceNumber,
                                  psPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(psPayloadDefinition)/sizeof(tsZCL_TxPayloadItem));
}
/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSurveyBeaconsSend
 **
 ** DESCRIPTION:
 ** Builds and sends Survey Beacons Send
 **
 ** PARAMETERS:                                                 Name                           Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SurveyBeaconsPayload                             *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsPayload                             *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
         {  1,                              E_ZCL_BOOL,            &psPayload->bStandardBeacons     },
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_SURVEY_BEACONS,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
    **
    ** NAME:       eCLD_WWAHCommandDisableOTADowngradesRequestSend
    **
    ** DESCRIPTION:
    ** Builds and sends a Disable OTA Downgrades Request command.
    **
    ** PARAMETERS:                                              Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
    ** RETURN:
    ** teZCL_Status
    **
    **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableOTADowngradesRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_OTA_DOWNGRADES,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}
/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable MGMT Leave Without Rejoin Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISBALE_MGMT_LEAVE_WITHOUT_REJOIN,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable Touchlink Interpan message Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_TOUCHLINK_INTERPAN_MESSAGE,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableWWAHParentClassificationRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enable WWAH Parent Classification Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHParentClassificationRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_WWAH_PARENT_CLASSIFICATION,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableParentClassificationRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable Parent Classification Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableParentClassificationRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_WWAH_PARENT_CLASSIFICATION,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}
/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableWWAHParentClassificationRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enable WWAH Parent Classification Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableTCSecurityonNwkKeyRotationRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableBadParentRecoveryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Enable Bad Parent Recovery Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableBadParentRecoveryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_WWAH_BAD_PARENT_RECOVERY,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableWWAHBadParentRecoveryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable WWAH Bad Parent Recovery Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHBadParentRecoveryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_WWAH_BAD_PARENT_RECOVERY,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableConfigurationModeRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Enable Configuration Mode Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 **********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableConfigurationModeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_ENABLE_CONFIGURATION_MODE,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableConfigurationModeRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Disable Configuration Mode Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableConfigurationModeRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_DISABLE_CONFIGURATION_MODE,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Trust Center For cluster Server Query Request command.
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  FALSE,
                                  E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  0);
}


 /* Definitions of WWAH Client Cluster Response Receive Command */

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseReceive
 **
 ** DESCRIPTION:
 ** handles rx of APS Link Key Authorization Query command Response
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               pZPSevent                       Event Message
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload      *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload      *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {   1,                  &u16ActualQuantity,                 E_ZCL_UINT16,    &psPayload->u16ClusterID                   },
            {   1,                  &u16ActualQuantity,                 E_ZCL_BOOL,      &psPayload->bAPSLinkKeyAuthorizationStatus }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandPoweringOnOffNotificationReceive
 **
 ** DESCRIPTION:
 ** handles Powering On/Off Notfication
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               pZPSevent                       Event Message
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandPoweringOnOffNotificationReceive(
        ZPS_tsAfEvent                                           *pZPSevent,
        uint8                                                   *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PoweringOnOffNotificationPayload             *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {   1,                                  &u16ActualQuantity,                 E_ZCL_ENUM8,     &psPayload->ePowNotiReason         },
            {   1,                                  &u16ActualQuantity,                 E_ZCL_UINT16,    &psPayload->u16ManuID              },
            {   1,                                  &u16ActualQuantity,                 E_ZCL_UINT8,     &psPayload->u8ManuPowNotfReasonLen },
            {   CLD_WWAH_MAX_ZB_CMD_PAYLOAD_LENGTH, &u16ActualQuantity,                 E_ZCL_UINT8,     psPayload->pu8ManuPowNotfReason    }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandShortAddressChangeReceive
 **
 ** DESCRIPTION:
 ** handles rx of Short Address Change command Response
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               pZPSevent                       Event Message
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_ShortAddressChangePayload                *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandShortAddressChangeReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_ShortAddressChangePayload                        *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {   1,                  &u16ActualQuantity,                 E_ZCL_IEEE_ADDR,    &psPayload->u64DeviceEUI64  },
            {   1,                  &u16ActualQuantity,                 E_ZCL_UINT16,       &psPayload->u16DeviceShort  }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSACKEnablementQueryResponseReceive
 **
 ** DESCRIPTION:
 **  handles rx of APS ACK Enablement Query command Response
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               pZPSevent                       Event Message
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {   1,                                  &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8NumberOfClustersToExempt      },
            {   CLD_WWAH_NO_OF_CLUSTERS,            &u16ActualQuantity,         E_ZCL_UINT16,           psPayload->pu16ClusterId                    },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandPowerDescriptorChangeReceive
 **
 ** DESCRIPTION:
 **  handles rx of Power Descriptor Change command Response
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               pZPSevent                       Event Message
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_PowerDescriptorChangePayload                     *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandPowerDescriptorChangeReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PowerDescriptorChangePayload                     *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {   1,                  &u16ActualQuantity,                 E_ZCL_BMAP16,    &psPayload->b16NodeDescriptor        },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);
}

/**************************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDebugReportQueryResponseReceive
 **
 ** DESCRIPTION:
 ** handles rx of Debug Report Query command Response
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               pZPSevent                       Event Message
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_DebugReportQueryResponsePayload                  *psPayload                      payload
 ** RETURN:
 ** teZCL_Status
 **
 *****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDebugReportQueryResponseReceive(
        ZPS_tsAfEvent                                           *pZPSevent,
        uint8                                                   *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryResponsePayload              *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {   1,                                      &u16ActualQuantity,                 E_ZCL_UINT8,     &psPayload->u8DebugReportID        },
            {   CLD_WWAH_REPORT_DATA_LENGTH,            &u16ActualQuantity,                 E_ZCL_UINT8,     psPayload->pu8ReportData           }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
}

/**************************************************************************************************************************
**
** NAME:       eCLD_WWAHNewDebugReportNotificationReceive
**
** DESCRIPTION:
** Builds and sends New Debug Report Notification Response Receive command
**
** PARAMETERS:                                                  Name                            Usage
** ZPS_tsAfEvent                                                pZPSevent                       Event Message
** uint8                                                        *pu8TransactionSequenceNumber   Sequence number Pointer
** tsCLD_WWAHNewDebugReportNotificationPayload                  *psPayload                      payload
** RETURN:
** teZCL_Status
**
*****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHNewDebugReportNotificationReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAHNewDebugReportNotificationPayload                 *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {   1,                  &u16ActualQuantity,                 E_ZCL_UINT8,    &psPayload->u8DebugReportID     },
        {   1,                  &u16ActualQuantity,                 E_ZCL_UINT32,   &psPayload->u32SizeOfReport     }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                 pu8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                 E_ZCL_ACCEPT_EXACT);
}

/**************************************************************************************************************************
**
** NAME:       eCLD_WWAHCommandTrustCenterForClusterQueryResponseReceive
**
** DESCRIPTION:
** Builds and sends Trust Center for Cluster Server Query Response Receive command
**
** PARAMETERS:                                                  Name                            Usage
** ZPS_tsAfEvent                                                pZPSevent                       Event Message
** uint8                                                        *pu8TransactionSequenceNumber   Sequence number Pointer
** tsCLD_WWAH_TCForClusterServerQueryResponsePayload            *psPayload                      payload
** RETURN:
** teZCL_Status
**
*****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterQueryResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_TCForClusterServerQueryResponsePayload           *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {   1,                          &u16ActualQuantity,                 E_ZCL_UINT8,    &psPayload->u8NumberOfClusters  },
        {   CLD_WWAH_NO_OF_CLUSTERS,    &u16ActualQuantity,                 E_ZCL_UINT16,   psPayload->pu16ClusterId        }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                 pu8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                 E_ZCL_ACCEPT_LESS);
}
            
/**************************************************************************************************************************
**
** NAME:       eCLD_WWAHCommandSurveyBeaconsResponseReceive
**
** DESCRIPTION:
** Tx the Survey Beacons Response Command
**
** PARAMETERS:                                                  Name                            Usage
** ZPS_tsAfEvent                                                pZPSevent                       Event Message
** uint8                                                        *pu8TransactionSequenceNumber   Sequence number Pointer
** tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal              *psPayload                      payload
** RETURN:
** teZCL_Status
**
*****************************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsResponseReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal             *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {   1,                                          &u16ActualQuantity,                 E_ZCL_UINT8,    &psPayload->u8NumberOfBeacons   },
        {   (CLD_WWAH_MAX_SURVEY_BEACONS * 4),          &u16ActualQuantity,                 E_ZCL_UINT8,    psPayload->pu8BeaconSurvey     },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                 pu8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                 E_ZCL_ACCEPT_LESS);
}
#endif /* WWAH_CLIENT */
