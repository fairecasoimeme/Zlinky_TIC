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
 * COMPONENT:          ACEServerCommands.c
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

#ifdef IASACE_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_ArmRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends an arm response cluster command
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
PUBLIC teZCL_Status eCLD_IASACE_ArmRespSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_ArmRespPayload                 *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_ENUM8,   &psPayload->eArmNotification},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_ARM_RESP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneIDMapRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Zone ID Map Response cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_GetZoneIDMapRespPayload            *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneIDMapRespSend (
                    uint8                                           u8SourceEndPointId,
                    uint8                                           u8DestinationEndPointId,
                    tsZCL_Address                                   *psDestinationAddress,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneIDMapRespPayload            *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES,                    E_ZCL_BMAP16,              psPayload->au16ZoneIDMap},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_GET_ZONE_ID_MAP_RESP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneInfoRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Zone Information response cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_GetZoneInfoRespPayload             *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneInfoRespSend (
                    uint8                                           u8SourceEndPointId,
                    uint8                                           u8DestinationEndPointId,
                    tsZCL_Address                                   *psDestinationAddress,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneInfoRespPayload             *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT8,            &psPayload->u8ZoneID},
            {1,                         E_ZCL_UINT16,           &psPayload->u16ZoneType},
            {1,                         E_ZCL_IEEE_ADDR,        &psPayload->u64IeeeAddress},
            {1,                         E_ZCL_CSTRING,          &psPayload->sZoneLabel}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_GET_ZONE_INFO_RESP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_ZoneStatusChangedSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Zone Status Changed cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_ZoneStatusChangedPayload           *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_ZoneStatusChangedSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_ZoneStatusChangedPayload       *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT8,            &psPayload->u8ZoneID},
            {1,                         E_ZCL_ENUM16,           &psPayload->eZoneStatus},
            {1,                         E_ZCL_ENUM8,            &psPayload->eAudibleNotification},
            {1,                         E_ZCL_CSTRING,          &psPayload->sZoneLabel}
                                              };
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_ZONE_STATUS_CHANGED,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Panel Status Changed Or Get Panel Status Response cluster command
 **
 ** PARAMETERS:                                                 Name                           Usage
 ** uint8                                                       u8SourceEndPointId             Source EP Id
 ** uint8                                                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                               *psDestinationAddress           Destination Address
 ** uint8                                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** teCLD_IASACE_ServerCmdId                                     eCommandId                     Get Panel Status Response / Panel Status Changed
 ** tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload  *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespSend (
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                teCLD_IASACE_ServerCmdId                                    eCommandId,
                tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload  *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_ENUM8,            &psPayload->ePanelStatus},
            {1,                         E_ZCL_UINT8,            &psPayload->u8SecondsRemaining},
            {1,                         E_ZCL_ENUM8,            &psPayload->eAudibleNotification},
            {1,                         E_ZCL_ENUM8,            &psPayload->eAlarmStatus}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  eCommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_SetBypassedZoneListSend
 **
 ** DESCRIPTION:
 ** Builds and sends a set bypassed zone list cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_SetBypassedZoneListPayload         *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_SetBypassedZoneListSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_SetBypassedZoneListPayload     *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT8,    &psPayload->u8NumofZones},
            {psPayload->u8NumofZones,   E_ZCL_UINT8,    psPayload->pu8ZoneID}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_SET_BYPASSED_ZONE_LIST,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_BypassRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends a bypass response cluster command
 **
 ** PARAMETERS:                                     Name                           Usage
 ** uint8                                           u8SourceEndPointId             Source EP Id
 ** uint8                                           u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                                   *psDestinationAddress           Destination Address
 ** uint8                                           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_IASACE_BypassRespPayload                  *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_BypassRespSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_BypassRespPayload              *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT8,    &psPayload->u8NumofZones},
            {psPayload->u8NumofZones,   E_ZCL_UINT8,    psPayload->pu8BypassResult}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_BYPASS_RESP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneStatusRespSend
 **
 ** DESCRIPTION:
 ** Builds and sends a get zone status response  cluster command
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
PUBLIC teZCL_Status eCLD_IASACE_GetZoneStatusRespSend (
                uint8                                       u8SourceEndPointId,
                uint8                                       u8DestinationEndPointId,
                tsZCL_Address                               *psDestinationAddress,
                uint8                                       *pu8TransactionSequenceNumber,
                tsCLD_IASACE_GetZoneStatusRespPayload       *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_BOOL,     &psPayload->bZoneStatusComplete},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8NumofZones},
            {(psPayload->u8NumofZones*3),           E_ZCL_UINT8,    psPayload->pu8ZoneStatus}
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SECURITY_AND_SAFETY_CLUSTER_ID_IASACE,
                                  TRUE,
                                  E_CLD_IASACE_CMD_GET_ZONE_STATUS_RESP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEArmReceive
 **
 ** DESCRIPTION:
 ** handles rx of arm command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_ArmPayload                          *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACEArmReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,                    
                    tsCLD_IASACE_ArmPayload                         *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_ENUM8,            &psPayload->eArmMode},
            {1,                             &u16ActualQuantity,             E_ZCL_CSTRING,          &psPayload->sArmDisarmCode},
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8ZoneID},
                                                };
  
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_ACCEPT_LESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_BypassReceive
 **
 ** DESCRIPTION:
 ** handles rx of bypass command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_BypassPayload                      *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_BypassReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,                    
                    tsCLD_IASACE_BypassPayload                      *psPayload)
{
    uint8 u8Count;
    uint16 u16ActualQuantity,u16inputOffset;
    tsZCL_HeaderParams sZCL_HeaderParams;
    teZCL_Status eStatus;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8NumOfZones},
            {psPayload->u8NumOfZones,       &u16ActualQuantity,             E_ZCL_UINT8,            psPayload->pu8ZoneID},
            {1,                             &u16ActualQuantity,             E_ZCL_CSTRING,          &psPayload->sArmDisarmCode},
                                                };
            
    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_LESS);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "\nErr:eCLD_IASACE_BypassReceive:%d", eStatus);
        return eStatus;
    }else{                           
        u16ActualQuantity = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
        u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);
        
        u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8,  &psPayload->u8NumOfZones);
        
        for(u8Count = 0;u8Count<psPayload->u8NumOfZones;u8Count++)
        {
            u16inputOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8,  &psPayload->pu8ZoneID[u8Count]);
        }
                
        u16inputOffset += u16ZCL_APduInstanceReadStringNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_CSTRING, &psPayload->sArmDisarmCode);
       
        if((u16ActualQuantity - u16inputOffset) != 0){
            eStatus =  E_ZCL_FAIL;
        }        
    }              
    return eStatus;

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_EmergencyReceive
 **
 ** DESCRIPTION:
 ** handles rx of emergency command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_EmergencyReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        0,
                                        0,
                                        E_ZCL_ACCEPT_EXACT);

}


/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_FireReceive
 **
 ** DESCRIPTION:
 ** handles rx of fire command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_FireReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        0,
                                        0,
                                        E_ZCL_ACCEPT_EXACT);

}


/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_PanicReceive
 **
 ** DESCRIPTION:
 ** handles rx of Panic command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_PanicReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        0,
                                        0,
                                        E_ZCL_ACCEPT_EXACT);

}


/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneIDMapReceive
 **
 ** DESCRIPTION:
 ** handles rx of get zone ID Map command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneIDMapReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        0,
                                        0,
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneInfoReceive
 **
 ** DESCRIPTION:
 ** handles rx of arm response
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                           *psClusterInstance             Cluster structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_IASACE_GetZoneInfoPayload                 *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneInfoReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneInfoPayload                 *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8ZoneID},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetPanelStatusReceive
 **
 ** DESCRIPTION:
 ** handles rx of get panel status command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetPanelStatusReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        0,
                                        0,
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetBypassedZoneListReceive
 **
 ** DESCRIPTION:
 ** handles rx of get bypassed zone list command
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** uint8                                           *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetBypassedZoneListReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{
    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        0,
                                        0,
                                        E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACE_GetZoneStatusReceive
 **
 ** DESCRIPTION:
 ** handles rx of arm response
 **
 ** PARAMETERS:                                     Name                          Usage
 ** ZPS_tsAfEvent                                   *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                        *psEndPointDefinition          EP structure
 ** tsCLD_IASACE_GetZoneStatusPayload               *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASACE_GetZoneStatusReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_IASACE_GetZoneStatusPayload               *psPayload)
{   
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8StartingZoneID},
            {1,                             &u16ActualQuantity,             E_ZCL_UINT8,            &psPayload->u8MaxNumOfZoneID},
            {1,                             &u16ActualQuantity,             E_ZCL_BOOL,             &psPayload->bZoneStatusMaskFlag},
            {1,                             &u16ActualQuantity,             E_ZCL_BMAP16,           &psPayload->u16ZoneStatusMask}
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT);

}

#endif /* IASACE_SERVER */

#endif /* CLD_IASACE */
