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
 * MODULE:             On/Off Cluster
 *
 * COMPONENT:          OnOffCommands.h
 *
 * DESCRIPTION:        Send an on/off cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>


#include "zcl.h"
#include "zcl_customcommand.h"

#include "OnOff.h"
#include "OnOff_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"
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
#ifdef ONOFF_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends an on/off cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** teCLD_OnOff_Command         eCommand                       Message type
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_OnOffCommandSend(
                                        uint8              u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        teCLD_OnOff_Command   eCommand)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_ONOFF,
                                  FALSE,
                                  (uint8)eCommand,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0
                                 );

}

#ifdef  CLD_ONOFF_CMD_OFF_WITH_EFFECT
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffCommandOffWithEffectSend
 **
 ** DESCRIPTION:
 ** Builds and sends an off with effect On/Off cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_OnOff_OffEffectRequestPayload *psPayload             Message payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_OnOffCommandOffWithEffectSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_OnOff_OffWithEffectRequestPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
                {1,      E_ZCL_UINT8,   &psPayload->u8EffectId},
                {1,      E_ZCL_UINT8,   &psPayload->u8EffectVariant},
                                                 };

        return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      GENERAL_CLUSTER_ID_ONOFF,
                                      FALSE,
                                      E_CLD_ONOFF_CMD_OFF_EFFECT,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );
}
#endif

#ifdef CLD_ONOFF_CMD_ON_WITH_TIMED_OFF
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffCommandOnWithTimedOffSend
 **
 ** DESCRIPTION:
 ** Builds and sends an on with timed off - On/Off cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_OnOff_OnWithTimedOffRequestPayload *psPayload        Message payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_OnOffCommandOnWithTimedOffSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_OnOff_OnWithTimedOffRequestPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
                {1,       E_ZCL_UINT8,  &psPayload->u8OnOff},
                {1,       E_ZCL_UINT16, &psPayload->u16OnTime},
                {1,       E_ZCL_UINT16, &psPayload->u16OffTime},
                                                  };

        return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      GENERAL_CLUSTER_ID_ONOFF,
                                      FALSE,
                                      E_CLD_ONOFF_CMD_ON_TIMED_OFF,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );
}
#endif
#endif /* ONOFF_CLIENT */

#ifdef ONOFF_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffCommandReceive
 **
 ** DESCRIPTION:
 ** handles rx of an On/Off command
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
PUBLIC  teZCL_Status eCLD_OnOffCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT);

}

#ifdef  CLD_ONOFF_CMD_OFF_WITH_EFFECT
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffCommandOffWithEffectReceive
 **
 ** DESCRIPTION:
 ** handles rx of off with effect commands
 **
 ** PARAMETERS:              Name                           Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_OnOff_OffWithEffectRequestPayload *psPayload      Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_OnOffCommandOffWithEffectReceive(
                ZPS_tsAfEvent               *pZPSevent,
                uint8                       *pu8TransactionSequenceNumber,
                tsCLD_OnOff_OffWithEffectRequestPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8EffectId},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8EffectVariant},
                                                 };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
#endif

#ifdef CLD_ONOFF_CMD_ON_WITH_TIMED_OFF
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffCommandOnWithTimedOffReceive
 **
 ** DESCRIPTION:
 ** handles rx of on with timed off commands
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_OnOff_OnWithTimedOffRequestPayload *psPayload     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_OnOffCommandOnWithTimedOffReceive(
                ZPS_tsAfEvent               *pZPSevent,
                uint8                       *pu8TransactionSequenceNumber,
                tsCLD_OnOff_OnWithTimedOffRequestPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8OnOff},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16OnTime},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16OffTime},
                                                 };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
#endif
#endif /* ONOFF_SERVER */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
