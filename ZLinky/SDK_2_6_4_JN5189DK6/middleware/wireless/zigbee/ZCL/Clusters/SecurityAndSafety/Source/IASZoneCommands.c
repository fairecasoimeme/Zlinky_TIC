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
 * MODULE:             IAS Zone Cluster
 *
 * COMPONENT:          IASZoneCommands.h
 *
 * DESCRIPTION:        Send IAS Zone cluster commands
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

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

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/
#ifdef IASZONE_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneEnrollReqSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASZone_EnrollRequestPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneEnrollReqSend (
                                uint8                             u8SourceEndPointId,
                                uint8                             u8DestinationEndPointId,
                                tsZCL_Address                     *psDestinationAddress,
                                uint8                             *pu8TransactionSequenceNumber,
                                tsCLD_IASZone_EnrollRequestPayload   *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM16,   &psPayload->e16ZoneType},
        {1, E_ZCL_UINT16,  &psPayload->u16ManufacturerCode}
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,
                                  TRUE,
                                  E_CLD_IASZONE_CMD_ZONE_ENROLL_REQUEST,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneStatusChangeNotificationSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASZone_StatusChangeNotificationPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneStatusChangeNotificationSend (
                                uint8                             u8SourceEndPointId,
                                uint8                             u8DestinationEndPointId,
                                tsZCL_Address                     *psDestinationAddress,
                                uint8                             *pu8TransactionSequenceNumber,
                                tsCLD_IASZone_StatusChangeNotificationPayload   *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_BMAP16,   &psPayload->b16ZoneStatus},
        {1, E_ZCL_BMAP8,    &psPayload->b8ExtendedStatus},
        {1, E_ZCL_UINT8,    &psPayload->u8ZoneId},
        {1, E_ZCL_UINT16,   &psPayload->u16Delay}
                                                };

    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,
                                  TRUE,
                                  E_CLD_IASZONE_CMD_ZONE_STATUS_CHANGE_NOTIFICATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneEnrollRespReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveToHueCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneEnrollRespReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_IASZone_EnrollResponsePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->e8EnrollResponseCode},
        {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->u8ZoneID}
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneNormalOperationModeReqReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneNormalOperationModeReqReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber)
{


    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     NULL,
                                     0,
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

#ifdef CLD_IASZONE_CMD_INITIATE_TEST_MODE
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneTestModeReqReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASZone_InitiateTestModeRequestPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneTestModeReqReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_IASZone_InitiateTestModeRequestPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->u8TestModeDuration},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8CurrentZoneSensitivityLevel},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif
#endif

#ifdef IASZONE_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneEnrollRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends a IAS Zone cluster renroll response
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASZone_EnrollResponsePayload *psPayload             Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneEnrollRespSend (
                                uint8                             u8SourceEndPointId,
                                uint8                             u8DestinationEndPointId,
                                tsZCL_Address                     *psDestinationAddress,
                                uint8                             *pu8TransactionSequenceNumber,
                                tsCLD_IASZone_EnrollResponsePayload   *psPayload)

{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->e8EnrollResponseCode},
        {1, E_ZCL_ENUM8,   &psPayload->u8ZoneID}
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,
                                  FALSE,
                                  E_CLD_IASZONE_CMD_ZONE_ENROLL_RESP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

#ifdef CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneNormalOperationModeReqSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneNormalOperationModeReqSend (
                                uint8                             u8SourceEndPointId,
                                uint8                             u8DestinationEndPointId,
                                tsZCL_Address                     *psDestinationAddress,
                                uint8                             *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,
                                  FALSE,
                                  E_CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE,
                                  pu8TransactionSequenceNumber,
                                  NULL,
                                  FALSE,
                                  0,
                                  0);

}
#endif

#ifdef CLD_IASZONE_CMD_INITIATE_TEST_MODE
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneTestModeReqSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASZone_InitiateTestModeRequestPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneTestModeReqSend (
                                uint8                             u8SourceEndPointId,
                                uint8                             u8DestinationEndPointId,
                                tsZCL_Address                     *psDestinationAddress,
                                uint8                             *pu8TransactionSequenceNumber,
                                tsCLD_IASZone_InitiateTestModeRequestPayload    *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->u8TestModeDuration},
        {1, E_ZCL_UINT8,   &psPayload->u8CurrentZoneSensitivityLevel}
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,
                                  FALSE,
                                  E_CLD_IASZONE_CMD_INITIATE_TEST_MODE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneEnrollReqReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASZone_EnrollRequestPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneEnrollReqReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_IASZone_EnrollRequestPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM16,   &psPayload->e16ZoneType},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16ManufacturerCode}
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneStatusChangeNotificationReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveHueCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneStatusChangeNotificationReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_IASZone_StatusChangeNotificationPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_BMAP16,   &psPayload->b16ZoneStatus},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->b8ExtendedStatus},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8ZoneId},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16Delay}
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

#endif



/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
