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
 * MODULE:             Colour Control Cluster
 *
 * COMPONENT:          ColourControlClientCommands.h
 *
 * DESCRIPTION:        Send and receive Colour Control cluster commands
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "ColourControl.h"
#include "ColourControl_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"

#ifdef DEBUG_CLD_COLOUR_CONTROL
#define TRACE_COLOUR_CONTROL   TRUE
#else
#define TRACE_COLOUR_CONTROL   FALSE
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
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToHueCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveToHueCommandPayload *psPayload     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToHueCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToHueCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->u8Hue},
        {1, E_ZCL_ENUM8,   &psPayload->eDirection},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToHueCommandReceive
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
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToHueCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToHueCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Hue},
        {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eDirection},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveHueCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveHueCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveHueCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveHueCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, E_ZCL_UINT8,   &psPayload->u8Rate},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_HUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveHueCommandReceive
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
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveHueCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveHueCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eMode},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Rate},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepHueCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_StepHueCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepHueCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepHueCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, E_ZCL_UINT8,   &psPayload->u8StepSize},
        {1, E_ZCL_UINT8,   &psPayload->u8TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_STEP_HUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepHueCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_StepHueCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepHueCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepHueCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eMode},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8StepSize},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8TransitionTime},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToSaturationCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveToSaturationCommandPayload *psPayload     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToSaturationCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToSaturationCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->u8Saturation},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_TO_SATURATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToSaturationCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveToSaturationCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToSaturationCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToSaturationCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Saturation},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveSaturationCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveSaturationCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveSaturationCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveSaturationCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, E_ZCL_UINT8,   &psPayload->u8Rate},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_SATURATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveSaturationCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveSaturationCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveSaturationCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveSaturationCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eMode},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Rate},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepSaturationCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_StepSaturationCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepSaturationCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepSaturationCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, E_ZCL_UINT8,   &psPayload->u8StepSize},
        {1, E_ZCL_UINT8,   &psPayload->u8TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_STEP_SATURATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepSaturationCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_StepSaturationCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepSaturationCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepSaturationCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eMode},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8StepSize},
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8TransitionTime},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToHueAndSaturationCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload *psPayload     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToHueAndSaturationCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->u8Hue},
        {1, E_ZCL_UINT8,   &psPayload->u8Saturation},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE_AND_SATURATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToHueAndSaturationCommandReceive
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
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToHueAndSaturationCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Hue},
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Saturation},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToColourCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveToColourCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToColourCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToColourCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT16,  &psPayload->u16ColourX},
        {1, E_ZCL_UINT16,  &psPayload->u16ColourY},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToColourCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveToColourCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToColourCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToColourCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourX},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourY},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveColourCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveColourCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveColourCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveColourCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_INT16,   &psPayload->i16RateX},
        {1, E_ZCL_INT16,   &psPayload->i16RateY},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveColourCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveColourCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveColourCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveColourCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_INT16,   &psPayload->i16RateX},
            {1, &u16ActualQuantity, E_ZCL_INT16,   &psPayload->i16RateY},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepColourCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_StepColourCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepColourCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepColourCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_INT16,   &psPayload->i16StepX},
        {1, E_ZCL_INT16,   &psPayload->i16StepY},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_STEP_COLOUR,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepColourCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_StepColourCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepColourCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepColourCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_INT16,   &psPayload->i16StepX},
            {1, &u16ActualQuantity, E_ZCL_INT16,   &psPayload->i16StepY},
            {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToColourTemperatureCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToColourTemperatureCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMired},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR_TEMPERATURE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveToColourTemperatureCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveToColourTemperatureCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMired},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedMoveToHueCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedMoveToHueCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedMoveToHueCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedMoveToHueCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT16,   &psPayload->u16EnhancedHue},
        {1, E_ZCL_UINT8,   &psPayload->eDirection},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedMoveToHueCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedMoveToHueCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedMoveToHueCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedMoveToHueCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16EnhancedHue},
        {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eDirection},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedMoveHueCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedMoveHueCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedMoveHueCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedMoveHueCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->eMode},
        {1, E_ZCL_UINT16,  &psPayload->u16Rate},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                           };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_HUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedMoveHueCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedMoveHueCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedMoveHueCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedMoveHueCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->eMode},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16Rate},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedStepHueCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedStepHueCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedStepHueCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedStepHueCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->eMode},
        {1, E_ZCL_UINT16,   &psPayload->u16StepSize},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_ENHANCED_STEP_HUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedStepHueCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedStepHueCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedStepHueCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedStepHueCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->eMode},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16StepSize},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedMoveToHueAndSaturationCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedMoveToHueAndSaturationCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT16,   &psPayload->u16EnhancedHue},
        {1, E_ZCL_UINT8,   &psPayload->u8Saturation},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE_AND_SATURATION,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandEnhancedMoveToHueAndSaturationCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandEnhancedMoveToHueAndSaturationCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16EnhancedHue},
        {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8Saturation},
        {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16TransitionTime},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandColourLoopSetCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_ColourLoopSetCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandColourLoopSetCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_ColourLoopSetCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_UINT8,   &psPayload->u8UpdateFlags},
        {1, E_ZCL_UINT8,   &psPayload->eAction},
        {1, E_ZCL_UINT8,   &psPayload->eDirection},
        {1, E_ZCL_UINT16,   &psPayload->u16Time},
        {1, E_ZCL_UINT16,  &psPayload->u16StartHue},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_COLOUR_LOOP_SET,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandColourLoopSetCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_ColourLoopSetCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandColourLoopSetCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_ColourLoopSetCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &psPayload->u8UpdateFlags},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &psPayload->eAction},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &psPayload->eDirection},
        {1, &u16ActualQuantity, E_ZCL_UINT16, &psPayload->u16Time},
        {1, &u16ActualQuantity, E_ZCL_UINT16, &psPayload->u16StartHue},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,  &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,  &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStopMoveStepCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_StopMoveStepCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStopMoveStepCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StopMoveStepCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         
                                                };
                                                
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_STOP_MOVE_STEP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

#if ((CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)   ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)      ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)               ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED))
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStopMoveStepCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_ColourLoopSetCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStopMoveStepCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StopMoveStepCommandPayload *psPayload)
{
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveColourTemperatureCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_MoveColourTemperatureCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveColourTemperatureCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveColourTemperatureCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, E_ZCL_UINT16,  &psPayload->u16Rate},
        {1, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMin},
        {1, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMax},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},  

                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR_TEMPERATURE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}




/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepColourTemperatureCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Colour Control custom command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ColourControl_StepColourTemperatureCommandPayload *psPayload       Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepColourTemperatureCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_StepColourTemperatureCommandPayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
        {1, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, E_ZCL_UINT16,  &psPayload->u16StepSize},
        {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
        {1, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMin},
        {1, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMax},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},         

                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                                  FALSE,
                                  E_CLD_COLOURCONTROL_CMD_STEP_COLOUR_TEMPERATURE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandStepColourTemperatureCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandStepColourTemperatureCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StepColourTemperatureCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eMode},
            {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16StepSize},
            {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
            {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMin},
            {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMax},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCommandMoveColourTemperatureCommandReceive
 **
 ** DESCRIPTION:
 ** handles receive of a colour control custom command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload  Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCommandMoveColourTemperatureCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_MoveColourTemperatureCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_ENUM8,   &psPayload->eMode},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16Rate},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMin},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16ColourTemperatureMiredMax},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
        {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},        
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
