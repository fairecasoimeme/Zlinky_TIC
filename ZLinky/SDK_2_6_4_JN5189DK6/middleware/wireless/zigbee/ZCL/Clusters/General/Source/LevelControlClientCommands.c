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
 * MODULE:             Level Control Cluster
 *
 * COMPONENT:          LevelControlClientCommands.h
 *
 * DESCRIPTION:        Send and receive Level Control cluster commands
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "LevelControl.h"
#include "LevelControl_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"


#ifdef DEBUG_CLD_LEVEL_CONTROL
#define TRACE_LEVEL_CONTROL    TRUE
#else
#define TRACE_LEVEL_CONTROL    FALSE
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
 ** NAME:       eCLD_LevelControlCommandMoveToLevelCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Level Control cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** bool_t                      bWithOnOff                     With On/Off ?
 ** tsCLD_LevelControl_MoveToLevelCommandPayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandMoveToLevelCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    bool_t                      bWithOnOff,
                    tsCLD_LevelControl_MoveToLevelCommandPayload *psPayload)
{
    uint8 u8CommandId;
    uint8 u8PayloadSize = 0;
    
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1, E_ZCL_UINT8,   &psPayload->u8Level},
            {1, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
            {1, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},
                                              };

    if(bWithOnOff == FALSE)
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL;
        u8PayloadSize = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);
    }
    else
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF;
        /* As only first two parameters needs to be passed */
        u8PayloadSize = 2;
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_LEVEL_CONTROL,
                                  FALSE,
                                  u8CommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8PayloadSize);

}


/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandMoveToLevelCommandReceive
 **
 ** DESCRIPTION:
 ** handles rx a level control command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_LevelControl_MoveToLevelCommandPayload *psPayload Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandMoveToLevelCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_LevelControl_MoveToLevelCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Level},
            {1, &u16ActualQuantity, E_ZCL_UINT16,  &psPayload->u16TransitionTime},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8OptionsOverride},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandMoveCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Level Control cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** bool_t                      bWithOnOff                     With On/Off ?
 ** tsCLD_LevelControl_MoveCommandPayload *psPayload           Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandMoveCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    bool_t                      bWithOnOff,
                    tsCLD_LevelControl_MoveCommandPayload *psPayload)
{
    uint8 u8CommandId;
    uint8 u8PayloadSize = 0;
    
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1, E_ZCL_UINT8,    &psPayload->u8MoveMode},
            {1, E_ZCL_UINT8,    &psPayload->u8Rate},
            {1, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
            {1, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},
                            };

    if(bWithOnOff == FALSE)
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_MOVE;
        u8PayloadSize = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);
    }
    else
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF;
        /* As only first two parameters needs to be passed */
        u8PayloadSize = 2;
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_LEVEL_CONTROL,
                                  FALSE,
                                  u8CommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8PayloadSize);

}


/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandMoveCommandReceive
 **
 ** DESCRIPTION:
 ** handles rx a level control command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_LevelControl_MoveCommandPayload *psPayload        Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandMoveCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_LevelControl_MoveCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8MoveMode},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8Rate},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandStepCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Level Control cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** bool_t                      bWithOnOff                     With On/Off ?
 ** tsCLD_LevelControl_StepCommandPayload *psPayload           Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandStepCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    bool_t                      bWithOnOff,
                    tsCLD_LevelControl_StepCommandPayload *psPayload)
{
    uint8 u8CommandId;
    uint8 u8PayloadSize = 0;
    
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1, E_ZCL_UINT8,    &psPayload->u8StepMode},
            {1, E_ZCL_UINT8,    &psPayload->u8StepSize},
            {1, E_ZCL_UINT16,   &psPayload->u16TransitionTime},
            {1, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
            {1, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},
                                                };

    if(bWithOnOff == FALSE)
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_STEP;
        u8PayloadSize = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);
    }
    else
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_STEP_WITH_ON_OFF;
        /* As only first three parameters needs to be passed */
        u8PayloadSize = 3;
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_LEVEL_CONTROL,
                                  FALSE,
                                  u8CommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8PayloadSize);

}


/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandStepCommandReceive
 **
 ** DESCRIPTION:
 ** handles rx a level control command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_LevelControl_StepCommandPayload *psPayload        Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandStepCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_LevelControl_StepCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8StepMode},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPayload->u8StepSize},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPayload->u16TransitionTime},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandStopCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Level Control cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_LevelControl_StopCommandPayload *psPayload           Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandStopCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    bool_t                      bWithOnOff,
                    tsCLD_LevelControl_StopCommandPayload *psPayload)
{
    uint8 u8CommandId;
    uint8 u8PayloadSize = 0;
    
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
            {1, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},
                                                };

    if(bWithOnOff == FALSE)
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_STOP;
        u8PayloadSize = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);
    }
    else
    {
        u8CommandId = E_CLD_LEVELCONTROL_CMD_STOP_WITH_ON_OFF;
        u8PayloadSize = 0;
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_LEVEL_CONTROL,
                                  FALSE,
                                  u8CommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8PayloadSize);
}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandStopCommandReceive
 **
 ** DESCRIPTION:
 ** handles rx a level control command
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
PUBLIC  teZCL_Status eCLD_LevelControlCommandStopCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_LevelControl_StopCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsMask},
            {1, &u16ActualQuantity, E_ZCL_BMAP8,    &psPayload->u8OptionsOverride},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);
}

#ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandMoveToClosestFreqCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Level Control cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_LevelControl_MoveToClosestFreqCommandPayload *psPayload           Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCommandMoveToClosestFreqCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_LevelControl_MoveToClosestFreqCommandPayload *psPayload)
{
    uint8 u8CommandId;
    uint8 u8PayloadSize = 0;
    
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1, E_ZCL_UINT16,    &psPayload->u16Frequency}
                                                };

    u8CommandId = E_CLD_LEVELCONTROL_CMD_MOVE_TO_CLOSEST_FREQ;
    u8PayloadSize = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_LEVEL_CONTROL,
                                  FALSE,
                                  u8CommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8PayloadSize);
}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCommandMoveToClosestFreqCommandReceive
 **
 ** DESCRIPTION:
 ** handles rx a level control command
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
PUBLIC  teZCL_Status eCLD_LevelControlCommandMoveToClosestFreqCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_LevelControl_MoveToClosestFreqCommandPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity,E_ZCL_UINT16,    &psPayload->u16Frequency}
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
