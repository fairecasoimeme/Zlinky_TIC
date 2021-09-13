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
 * COMPONENT:          WWAHServerCommands.c
 *
 * DESCRIPTION:        Sends a WWAH cluster command
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
#include "stdlib.h"
//#include "app_main.h"

#ifdef DEBUG_CLD_DOOR_LOCK
#define TRACE_DOOR_LOCK    TRUE
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

#ifdef WWAH_SERVER

/* Definitions of WWAH Server Cluster Send Command */

/**********************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends an APS Link Key Authorization Query Response command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload      *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************************************************/
PUBLIC  teZCL_Status  eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryResponsePayload      *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {   1,              E_ZCL_UINT16,      &psPayload->u16ClusterID                     },
            {   1,              E_ZCL_BOOL,        &psPayload->bAPSLinkKeyAuthorizationStatus   },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandPoweringOffNotificationSend
 **
 ** DESCRIPTION:
 ** Builds and sends Powering Off Notification Response Send
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandPoweringOffNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
         {  1,                                  E_ZCL_ENUM8,        &psPayload->ePowNotiReason          },
         {  1,                                  E_ZCL_UINT16,       &psPayload->u16ManuID               },
         {  1,                                  E_ZCL_UINT8,        &psPayload->u8ManuPowNotfReasonLen  },
         {  psPayload->u8ManuPowNotfReasonLen,  E_ZCL_UINT8,        psPayload->pu8ManuPowNotfReason     },
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GENERAL_CLUSTER_ID_WWAH,
                              TRUE,
                              E_CLD_WWAH_CMD_POWERING_OFF_NOTIFICATION,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              TRUE,
                              ZCL_MANUFACTURER_CODE,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandPoweringOnNotificationSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Powering On Notification  Response command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandPoweringOnNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PoweringOnOffNotificationPayload                 *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {   1,                                      E_ZCL_ENUM8,        &psPayload->ePowNotiReason          },
            {   1,                                      E_ZCL_UINT16,       &psPayload->u16ManuID               },
            {   1,                                      E_ZCL_UINT8,        &psPayload->u8ManuPowNotfReasonLen  },
            {   psPayload->u8ManuPowNotfReasonLen,      E_ZCL_UINT8,        psPayload->pu8ManuPowNotfReason     }

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_POWERING_ON_NOTIFICATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandShortAddressChangeSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Short Address Change  command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_ShortAddressChangePayload                *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandShortAddressChangeSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_ShortAddressChangePayload                *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {   1,                                      E_ZCL_IEEE_ADDR,        &psPayload->u64DeviceEUI64  },
            {   1,                                      E_ZCL_UINT16,           &psPayload->u16DeviceShort  },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_SHORT_ADDRESS_CHANGE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSACKEnablementQueryResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends an APS ACK Enablement Query Response command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {   1,                                      E_ZCL_UINT8,            &psPayload->u8NumberOfClustersToExempt  },
            {   psPayload->u8NumberOfClustersToExempt,  E_ZCL_UINT16,           psPayload->pu16ClusterId                }
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_APS_ACK_ENABLEMENT_QUERY_RESPONSE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandPowerDescriptorChangeSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Descriptor Change Response command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_PowerDescriptorChangePayload             *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandPowerDescriptorChangeSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_PowerDescriptorChangePayload                     *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {   1,                                      E_ZCL_BMAP16,        &psPayload->b16NodeDescriptor        },
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_POWER_DESCRIPTOR_CHANGE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDebugReportQueryResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Debug Report Query Response command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_APSACKEnablementQueryResponsePayload             *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDebugReportQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryResponsePayload                  *psPayload)
{
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {   1,                                      E_ZCL_UINT8,        &psPayload->u8DebugReportID  },
            {   CLD_WWAH_REPORT_DATA_LENGTH,            E_ZCL_UINT8,        psPayload->pu8ReportData     },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_DEBUG_REPORT_QUERY_RESPONSE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandNewDebugReportNotificationSend
 **
 ** DESCRIPTION:
 ** Builds and sends New Debug Report Notification Response Send
 **
 ** PARAMETERS:                                                 Name                           Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAHNewDebugReportNotificationPayload                 *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandNewDebugReportNotificationSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAHNewDebugReportNotificationPayload                 *psPayload)
{
    
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
         {  1,              E_ZCL_UINT8,      &psPayload->u8DebugReportID   },
         {  1,              E_ZCL_UINT32,     &psPayload->u32SizeOfReport   },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GENERAL_CLUSTER_ID_WWAH,
                              TRUE,
                              E_CLD_WWAH_CMD_NEW_DEBUG_REPORT_NOTIFICATION,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              TRUE,
                              ZCL_MANUFACTURER_CODE,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandTrustCenterForClusterServerQueryResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends Trust Center For Cluster Server Query Response Send
 **
 ** PARAMETERS:                                                 Name                           Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_TCForClusterServerQueryResponsePayload       *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterServerQueryResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_TCForClusterServerQueryResponsePayload           *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
         {  1,                                          E_ZCL_UINT8,      &psPayload->u8NumberOfClusters    },
         {  psPayload->u8NumberOfClusters,              E_ZCL_UINT16,     psPayload->pu16ClusterId          },

    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/********************************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSurveyBeaconsResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends Survey Beacons Response Send
 **
 ** PARAMETERS:                                                 Name                           Usage
 ** uint8                                                       u8SourceEndPointId              Source EP Id
 ** uint8                                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal             *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ***********************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsResponseSend(
        uint8                                                       u8SourceEndPointId,
        uint8                                                       u8DestinationEndPointId,
        tsZCL_Address                                               *psDestinationAddress,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsResponsePayloadInternal             *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
         {  1,                                          E_ZCL_UINT8,      &psPayload->u8NumberOfBeacons     },
         {  (psPayload->u8NumberOfBeacons * 4),         E_ZCL_UINT8,      psPayload->pu8BeaconSurvey     },
    };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_WWAH,
                                  TRUE,
                                  E_CLD_WWAH_CMD_SURVEY_BEACON_RESPONSE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  TRUE,
                                  ZCL_MANUFACTURER_CODE,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/* Definitions of WWAH Server Cluster Request Receive Command */

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableOrDisableAPSLinkKeyAuthorizationRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of an Enable APS Link Key Authorization request command
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload         *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableOrDisableAPSLinkKeyAuthorizationRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableOrDisableAPSLinkKeyAuthorizationRequestPayload     *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,                              &u16ActualQuantity,         E_ZCL_UINT8,                &psPayload->u8NumberOfClustersToExempt  },
            {   CLD_WWAH_NO_OF_CLUSTERS,        &u16ActualQuantity,         E_ZCL_UINT16,               psPayload->pu16ClusterId                }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of an APS Link Key Authorization Query Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **  tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload      *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSLinkKeyAuthorizationQueryRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_APSLinkKeyAuthorizationQueryRequestPayload       *psPayload)
{
    uint16 u16ActualQuantity;
    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,               &psPayload->u16ClusterID    }
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRequestNewAPSLinkKeyRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Request New APS Link Key Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRequestNewAPSLinkKeyRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of an Enable WWWAH Event Retry Algorithm Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload   *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHAppEventRetryAlgorithmRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHAppEventRetryAlgorithmRequestPayload   *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8FirstBackoffTime              },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8BackoffSequenceCommonRatio    },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT32,           &psPayload->u32MaxBackoffTime               },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8MaxReDeliveryAttempts         },


    };
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Disable WWWAH Event Retry Algorithm Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHAppEventRetryAlgorithmRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRequestTimeRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Request Time Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRequestTimeRequestReceive(
        ZPS_tsAfEvent                                               *pZPSevent,
        uint8                                                       *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/*********************************************************************************************
 **
 ** NAME:       eCLD_WWAHEnableWWAHRejoinAlgorithmRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of an Enable WWAH Rejoin Algorithm Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload          *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 *******************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHRejoinAlgorithmRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnableWWAHRejoinAlgorithmRequestPayload              *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16FastRejoinTimeout            },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16DurationBetweenEachRejoin    },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16FastRejoinFirstBackoff       },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16MaxBackoffTime               },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16MaxBackoffIterations         },

    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Disable WWAH Rejoin Algorithm Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableWWAHRejoinAlgorithmRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Set IAS Zone Enrollment Method Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload         *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSetIASZoneEnrollmentMethodRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetIASZoneEnrollmentMethodRequestPayload             *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_ENUM8,            &psPayload->eEnrollmentMode     },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandClearBindingTableRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a  Clear Binding Table Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandClearBindingTableRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Enable Periodic Router Check-Ins Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload        *psPayload                      Payload
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnablePeriodicRouterCheckInsRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_EnablePeriodicRouterCheckInsRequestPayload           *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16CheckInInterval      },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Disable Periodic Router Check-Ins Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisablePeriodicRouterCheckInsRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Set MAC Poll CCA Wait Time Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload              *psPayload                      payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSetMACPollCCAWaitTimeRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetMACPollCCAWaitTimeRequestPayload                  *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8WaitTime      },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandSetPendingNetworkUpdateRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Set IAS Zone Enrollment Method Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload            *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSetPendingNetworkUpdateRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SetPendingNetworkUpdateRequestPayload                *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8Channel       },
            {   1,      &u16ActualQuantity,         E_ZCL_UINT16,           &psPayload->u16PANID        },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Require APS ACKs on Unicasts Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload            *psPayload                      Payload
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRequireAPSACKsOnUnicastsRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_RequireAPSACKsOnUnicastsRequestPayload               *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,                                  &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8NumberOfClustersToExempt      },
            {   CLD_WWAH_NO_OF_CLUSTERS,            &u16ActualQuantity,         E_ZCL_UINT16,           psPayload->pu16ClusterId                    },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Remove APS ACKs on Unicasts Requirement Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandRemoveAPSACKsOnUnicastsRequirementRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     NULL,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}
/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandAPSACKEnablementQueryRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of APS ACK Enablement Query Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandAPSACKEnablementQueryRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     NULL,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/*******************************************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDebugReportQueryRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of Debug Report Query Request Command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition                                    *psEndPointDefinition           EP structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_DebugReportQueryRequestPayload                   *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDebugReportQueryRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_DebugReportQueryRequestPayload                       *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,      &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8DebugReportID     },
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandUseTrustCenterForClusterServerRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Use Trust Center For OTA Server Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_WWAH_UseTCForClusterServerPayload                     *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandUseTrustCenterForClusterServerRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_UseTCForClusterServerPayload                         *psPayload)
{
    uint16 u16ActualQuantity;
    
    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,                                  &u16ActualQuantity,         E_ZCL_UINT8,            &psPayload->u8NumberOfClusters      },
            {   CLD_WWAH_NO_OF_CLUSTERS,            &u16ActualQuantity,         E_ZCL_UINT16,           psPayload->pu16ClusterId            },
    };    
    
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}
/*******************************************************************************************************
   **
   ** NAME:       eCLD_WWAHCommandSurveyBeaconsReceive
   **
   ** DESCRIPTION:
   ** handles rx of Survey Beacons Receive
   **
   ** PARAMETERS:                                               Name                                Usage
   ** ZPS_tsAfEvent                                             *pZPSevent                          Zigbee stack event structure
   ** uint8                                                     *pu8TransactionSequenceNumber       Sequence number Pointer
   **
   ** RETURN:
   ** teZCL_Status
   **
   ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandSurveyBeaconsReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber,
        tsCLD_WWAH_SurveyBeaconsPayload                                 *psPayload)        
{
    uint16 u16ActualQuantity;
    
    tsZCL_RxPayloadItem psPayloadDefinition[] = {
            {   1,                                  &u16ActualQuantity,         E_ZCL_BOOL,         &psPayload->bStandardBeacons        },
    };    
    
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     psPayloadDefinition,
                                     sizeof(psPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}                                   
/*******************************************************************************************************
   **
   ** NAME:       eCLD_WWAHCommandDisableOTADowngradesRequestReceive
   **
   ** DESCRIPTION:
   ** handles rx of Disable OTA Downgrades Request Receive
   **
   ** PARAMETERS:                                               Name                                Usage
   ** ZPS_tsAfEvent                                             *pZPSevent                          Zigbee stack event structure
   ** uint8                                                     *pu8TransactionSequenceNumber       Sequence number Pointer
   **
   ** RETURN:
   ** teZCL_Status
   **
   ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableOTADowngradesRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
   return eZCL_CustomCommandReceive(pZPSevent,
                                    pu8TransactionSequenceNumber,
                                    NULL,
                                    0,
                                    E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Disable MGMT Leave Without Rejoin Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableMGMTLeaveWithoutRejoinRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
   **
   ** NAME:       eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestReceive
   **
   ** DESCRIPTION:
   ** handles rx of Disable Touchlink Interpan Message Request Receive
   **
   ** PARAMETERS:                                               Name                                Usage
   ** ZPS_tsAfEvent                                             *pZPSevent                          Zigbee stack event structure
   ** uint8                                                     *pu8TransactionSequenceNumber       Sequence number Pointer
   **
   ** RETURN:
   ** teZCL_Status
   **
   ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableTouchlinkInterpanMessageRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
   return eZCL_CustomCommandReceive(pZPSevent,
                                    pu8TransactionSequenceNumber,
                                    NULL,
                                    0,
                                    E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableWWAHParentClassificationRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of an Enable WWAH Parent Classification Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableWWAHParentClassificationRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
   **
   ** NAME:       eCLD_WWAHCommandDisableParentClassificationRequestReceive
   **
   ** DESCRIPTION:
   ** handles rx of Disable Parent Classification Request Receive
   **
   ** PARAMETERS:                                               Name                                Usage
   ** ZPS_tsAfEvent                                             *pZPSevent                          Zigbee stack event structure
   ** uint8                                                     *pu8TransactionSequenceNumber       Sequence number Pointer
   **
   ** RETURN:
   ** teZCL_Status
   **
   ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandDisableParentClassificationRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
   return eZCL_CustomCommandReceive(pZPSevent,
                                    pu8TransactionSequenceNumber,
                                    NULL,
                                    0,
                                    E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandEnableTCSecurityonNwkKeyRotationRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of an Enable WWAH Parent Classification Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableTCSecurityonNwkKeyRotationRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
   **
   ** NAME:       eCLD_WWAHCommandEnableBadParentRecoveryRequestReceive
   **
   ** DESCRIPTION:
   ** handles rx of Enable Bad Parent Recovery Request Receive
   **
   ** PARAMETERS:                                               Name                                Usage
   ** ZPS_tsAfEvent                                             *pZPSevent                          Zigbee stack event structure
   ** uint8                                                     *pu8TransactionSequenceNumber       Sequence number Pointer
   **
   ** RETURN:
   ** teZCL_Status
   **
   ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableBadParentRecoveryRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
   return eZCL_CustomCommandReceive(pZPSevent,
                                    pu8TransactionSequenceNumber,
                                    NULL,
                                    0,
                                    E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHDisableWWAHBadParentRecoveryRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Disable WWAH Bad Parent Recovery Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHDisableWWAHBadParentRecoveryRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/*******************************************************************************************************
   **
   ** NAME:       eCLD_WWAHCommandEnableConfigurationModeRequestReceive
   **
   ** DESCRIPTION:
   ** handles rx of Enable Configuration Mode Request Receive
   **
   ** PARAMETERS:                                               Name                                Usage
   ** ZPS_tsAfEvent                                             *pZPSevent                          Zigbee stack event structure
   ** uint8                                                     *pu8TransactionSequenceNumber       Sequence number Pointer
   **
   ** RETURN:
   ** teZCL_Status
   **
   ************************************************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandEnableConfigurationModeRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
   return eZCL_CustomCommandReceive(pZPSevent,
                                    pu8TransactionSequenceNumber,
                                    NULL,
                                    0,
                                    E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHDisableConfigurationModeRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Disable Configuration Mode Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHDisableConfigurationModeRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestReceive
 **
 ** DESCRIPTION:
 ** handles rx of a Trust Center for Cluster server query Request command
 **
 ** PARAMETERS:                                                 Name                            Usage
 ** ZPS_tsAfEvent                                               *pZPSevent                      Zigbee stack event structure
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WWAHCommandTrustCenterForClusterServerQueryRequestReceive(
        ZPS_tsAfEvent                                                   *pZPSevent,
        uint8                                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}


#endif /* WWAH_SERVER */
