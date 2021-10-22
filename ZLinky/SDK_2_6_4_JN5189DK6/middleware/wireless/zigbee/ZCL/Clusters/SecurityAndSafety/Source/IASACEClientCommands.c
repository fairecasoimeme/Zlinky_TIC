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
 * MODULE:             IASACE Cluster
 *
 * COMPONENT:          ACEClientCommands.c
 *
 * DESCRIPTION:        Send/Receive IAS ACE cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "IASACE.h"
#include "IASACE_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"

#ifdef DEBUG_CLD_IASACE
#define TRACE_IASACE    TRUE
#else
#define TRACE_IASACE    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_IASACE
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

#ifdef IASACE_CLIENT

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_ArmSend
 **
 ** DESCRIPTION:
 ** Builds and sends an arm cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_ArmRespPayload                     *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_ArmSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_ArmPayload                     *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_ENUM8,            &psPayload->eArmMode},
            {1,                         E_ZCL_CSTRING,          &psPayload->sArmDisarmCode},
            {1,                         E_ZCL_UINT8,            &psPayload->u8ZoneID}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_ARM,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_BypassSend
 **
 ** DESCRIPTION:
 ** Builds and sends a bypass cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_BypassPayload                       *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_BypassSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_BypassPayload                  *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT8,            &psPayload->u8NumOfZones},
            {psPayload->u8NumOfZones,   E_ZCL_UINT8,            psPayload->pu8ZoneID},
            {1,                         E_ZCL_CSTRING,          &psPayload->sArmDisarmCode}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_BYPASS,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_EmergencySend
 **
 ** DESCRIPTION:
 ** Builds and sends an Emergency cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_EmergencySend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_EMERGENCY,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_FireSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Fire cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_FireSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_FIRE,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_PanicSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Panic cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_PanicSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_PANIC,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneIDMapSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Zone ID Map cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneIDMapSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_GET_ZONE_ID_MAP,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneInfoSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Zone Information cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_GetZoneInfoPayload                 *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneInfoSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_GetZoneInfoPayload             *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT8,            &psPayload->u8ZoneID}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_GET_ZONE_INFO,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetPanelStatusSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Panel Status cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetPanelStatusSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_GET_PANEL_STATUS,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetBypassedZoneListSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Bypassed Zone List cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetBypassedZoneListSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_GET_BYPASSED_ZONE_LIST,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneStatusSend
 **
 ** DESCRIPTION:
 ** Builds and sends a get zone status cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_GetZoneStatusRespPayload           *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneStatusSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_GetZoneStatusPayload           *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                             E_ZCL_UINT8,    &psPayload->u8StartingZoneID},
            {1,                             E_ZCL_UINT8,    &psPayload->u8MaxNumOfZoneID},
            {1,                             E_ZCL_BOOL,     &psPayload->bZoneStatusMaskFlag},
            {1,                             E_ZCL_BMAP16,   &psPayload->u16ZoneStatusMask}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  FALSE,
                                  E_CLD_IASACE_CMD_GET_ZONE_STATUS,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEArmRespReceive
 **
 ** DESCRIPTION:
 ** handles rx of arm response command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_ArmRespPayload                     *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_ArmRespReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_ArmRespPayload                     *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM8,            &psPayload->eArmNotification}
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneIDMapRespReceive
 **
 ** DESCRIPTION:
 ** handles rx of get zone ID Map Resp command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_GetZoneIDMapRespPayload            *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneIDMapRespReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneIDMapRespPayload            *psPayload)
{
    uint16 u16ActualQuantity = CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES,           &u16ActualQuantity,             E_ZCL_BMAP16,            psPayload->au16ZoneIDMap}
                                                };
                                                
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneInfoRespReceive
 **
 ** DESCRIPTION:
 ** handles rx of get zone info response
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_GetZoneInfoRespPayload             *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneInfoRespReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneInfoRespPayload             *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8ZoneID},
            {1,                             &u16ActualQuantity,             E_ZCL_BMAP16,           &psPayload->u16ZoneType},
            {1,                             &u16ActualQuantity,             E_ZCL_IEEE_ADDR,        &psPayload->u64IeeeAddress},
            {1,                             &u16ActualQuantity,             E_ZCL_CSTRING,          &psPayload->sZoneLabel},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_ACCEPT_LESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_ZoneStatusChangedReceive
 **
 ** DESCRIPTION:
 ** handles rx of zone status changed command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_ZoneStatusChangedPayload           *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_ZoneStatusChangedReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_ZoneStatusChangedPayload           *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8ZoneID},
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM16,           &psPayload->eZoneStatus},
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM8,            &psPayload->eAudibleNotification},
            {1,                             &u16ActualQuantity,             E_ZCL_CSTRING,          &psPayload->sZoneLabel},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_ACCEPT_LESS);

}

/****************************************************************************
 **
 ** NAME:       tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload
 **
 ** DESCRIPTION:
 ** handles rx of panel status changed or get panel status response command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_ZoneStatusChangedPayload           *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespReceive (
                    ZPS_tsAfEvent                                                       *pZPSevent,
                    uint8                                                               *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload          *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM8,            &psPayload->ePanelStatus},
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8SecondsRemaining},
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM8,            &psPayload->eAudibleNotification},
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM8,            &psPayload->eAlarmStatus},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_SetBypassedZoneListReceive
 **
 ** DESCRIPTION:
 ** handles rx of Set bypassed zone list command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_SetBypassedZoneListPayload         *psPayload                     Pointer to Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_SetBypassedZoneListReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_SetBypassedZoneListPayload         *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8NumofZones},
            {CLD_IASACE_ZONE_TABLE_SIZE,    &u16ActualQuantity,             E_ZCL_UINT8,            psPayload->pu8ZoneID},
                                                };
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_ACCEPT_LESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_BypassRespReceive
 **
 ** DESCRIPTION:
 ** handles rx of bypass response
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_BypassRespPayload                  *psPayload                     Pointer to Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_BypassRespReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_BypassRespPayload                  *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8NumofZones},
            {CLD_IASACE_ZONE_TABLE_SIZE,    &u16ActualQuantity,             E_ZCL_UINT8,            psPayload->pu8BypassResult},
                                                };
                                                
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_ACCEPT_LESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneStatusRespReceive
 **
 ** DESCRIPTION:
 ** handles rx of get zone status response
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_GetZoneStatusRespPayload           *psPayload                     Pointer to Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneStatusRespReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneStatusRespPayload           *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                                 &u16ActualQuantity,             E_ZCL_BOOL,             &psPayload->bZoneStatusComplete},
            {1,                                 &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8NumofZones},
            {(CLD_IASACE_ZONE_TABLE_SIZE*3),    &u16ActualQuantity,             E_ZCL_UINT8,            psPayload->pu8ZoneStatus},
                                                };
                                                
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_ACCEPT_LESS);

}

#endif /* IASACE_CLIENT */

#endif /* CLD_IASACE */
