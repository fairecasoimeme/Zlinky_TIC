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
 * MODULE:             Identify Cluster
 *
 * COMPONENT:          IdentifyClientCommands.h
 *
 * DESCRIPTION:        Send an Identify cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Identify.h"
#include "Identify_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"

#ifdef DEBUG_CLD_IDENTIFY
#define TRACE_IDENTIFY    TRUE
#else
#define TRACE_IDENTIFY    FALSE
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
#ifdef IDENTIFY_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyCommandIdentifyRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an identify cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_Identify_IdentifyRequestPayload *psPayload           Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_IdentifyCommandIdentifyRequestSend(
                                        uint8              u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_Identify_IdentifyRequestPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_UINT16,   &psPayload->u16IdentifyTime},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_IDENTIFY,
                                  FALSE,
                                  E_CLD_IDENTIFY_CMD_IDENTIFY,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyCommandIdentifyQueryRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends an identify cluster command
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
PUBLIC  teZCL_Status eCLD_IdentifyCommandIdentifyQueryRequestSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_IDENTIFY,
                                  FALSE,
                                  E_CLD_IDENTIFY_CMD_IDENTIFY_QUERY,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

#ifdef  CLD_IDENTIFY_CMD_TRIGGER_EFFECT
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyCommandIdentifyTriggerEffectSend
 **
 ** DESCRIPTION:
 ** Builds and sends an identify cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint8                      u8effectId
 ** uint8                      u8EffectVarient
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_IdentifyCommandTriggerEffectSend(
                                    uint8           u8SourceEndPointId,
                                    uint8           u8DestinationEndPointId,
                                    tsZCL_Address   *psDestinationAddress,
                                    uint8           *pu8TransactionSequenceNumber,
                                    uint8           u8EffectId,
                                    uint8           u8EffectVarient)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,  E_ZCL_UINT8,   &u8EffectId},
            {1,  E_ZCL_UINT8,   &u8EffectVarient},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_IDENTIFY,
                                  FALSE,
                                  E_CLD_IDENTIFY_CMD_TRIGGER_EFFECT,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyCommandIdentifyQueryResponseReceive
 **
 ** DESCRIPTION:
 ** handles rx of identify query response
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_Identify_IdentifyQueryResponsePayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_IdentifyCommandIdentifyQueryResponseReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_Identify_IdentifyQueryResponsePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16Timeout},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                        pu8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                        E_ZCL_ACCEPT_EXACT);

}

#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
/****************************************************************************
 **
 ** NAME:       eCLD_IdentifyUpdateCommissionStateCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends an identify cluster command
 **
 ** PARAMETERS:                                 Name                           Usage
 ** uint8                                       u8SourceEndPointId              Source EP Id
 ** uint8                                       u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                               *psDestinationAddress           Destination Address
 ** uint8                                       *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_Identify_UpdateCommissionStatePayload *psPayload                      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_IdentifyUpdateCommissionStateCommandSend(
                                        uint8                                       u8SourceEndPointId,
                                        uint8                                       u8DestinationEndPointId,
                                        tsZCL_Address                               *psDestinationAddress,
                                        uint8                                       *pu8TransactionSequenceNumber,
                                        tsCLD_Identify_UpdateCommissionStatePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_ENUM8,                &psPayload->u8Action},
            {1,                         E_ZCL_BMAP8,   &psPayload->u8CommissionStateMask},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_IDENTIFY,
                                  FALSE,
                                  E_CLD_IDENTIFY_CMD_UPDATE_COMMISSION_STATE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}
#endif
#endif /*IDENTIFY_CLIENT*/
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
