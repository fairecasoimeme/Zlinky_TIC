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
 * MODULE:             Window Covering Cluster
 *
 * COMPONENT:          cluster_window_covering_commands.h
 *
 * DESCRIPTION:        Send a window covering cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "WindowCovering.h"
#include "WindowCovering_internal.h"

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

#ifdef WINDOW_COVERING_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandSend
 **
 ** DESCRIPTION:
 ** Builds and sends an on/off cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** teCLD_WindowCovering_Command         eCommand                       Message type
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_WindowCoveringCommandSend(
    uint8                         u8SourceEndPointId,
    uint8                         u8DestinationEndPointId,
    tsZCL_Address                *psDestinationAddress,
    uint8                        *pu8TransactionSequenceNumber,
    teCLD_WindowCovering_Command  eCommand)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  CLUSTER_ID_WINDOW_COVERING,
                                  FALSE,
                                  (uint8)eCommand,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0
                                 );

}

#ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToLiftValueSend
 **
 ** DESCRIPTION:
 ** Builds and sends an off with effect On/Off cluster command
 **
 ** PARAMETERS:                                Name                         Usage
 ** uint8                                      u8SourceEndPointId           Source EP Id
 ** uint8                                      u8DestinationEndPointId      Destination EP Id
 ** tsZCL_Address                             *psDestinationAddress         Destination Address
 ** uint8                                     *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToLiftValuePayload *psPayload                    Message payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToLiftValueSend(
    uint8                                      u8SourceEndPointId,
    uint8                                      u8DestinationEndPointId,
    tsZCL_Address                             *psDestinationAddress,
    uint8                                     *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftValuePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {{1, E_ZCL_UINT16, &psPayload->u16LiftValue}};

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  CLUSTER_ID_WINDOW_COVERING,
                                  FALSE,
                                  E_CLD_WC_CMD_GO_TO_LIFT_VALUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                  );
}
#endif

#ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToLiftPercentageSend
 **
 ** DESCRIPTION:
 ** Builds and sends an off with effect On/Off cluster command
 **
 ** PARAMETERS:                                     Name                         Usage
 ** uint8                                           u8SourceEndPointId           Source EP Id
 ** uint8                                           u8DestinationEndPointId      Destination EP Id
 ** tsZCL_Address                                  *psDestinationAddress         Destination Address
 ** uint8                                          *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToLiftPercentagePayload *psPayload                    Message payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToLiftPercentageSend(
    uint8                                           u8SourceEndPointId,
    uint8                                           u8DestinationEndPointId,
    tsZCL_Address                                  *psDestinationAddress,
    uint8                                          *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftPercentagePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {{1, E_ZCL_UINT8, &psPayload->u8LiftPercentage}};

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  CLUSTER_ID_WINDOW_COVERING,
                                  FALSE,
                                  E_CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                  );
}
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToTiltValueSend
 **
 ** DESCRIPTION:
 ** Builds and sends an off with effect On/Off cluster command
 **
 ** PARAMETERS:                                Name                         Usage
 ** uint8                                      u8SourceEndPointId           Source EP Id
 ** uint8                                      u8DestinationEndPointId      Destination EP Id
 ** tsZCL_Address                             *psDestinationAddress         Destination Address
 ** uint8                                     *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToTiltValuePayload *psPayload                    Message payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToTiltValueSend(
    uint8                                      u8SourceEndPointId,
    uint8                                      u8DestinationEndPointId,
    tsZCL_Address                             *psDestinationAddress,
    uint8                                     *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftValuePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {{1, E_ZCL_UINT16, &psPayload->u16TiltValue}};

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  CLUSTER_ID_WINDOW_COVERING,
                                  FALSE,
                                  E_CLD_WC_CMD_GO_TO_TILT_VALUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                  );
}
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToTiltPercentageSend
 **
 ** DESCRIPTION:
 ** Builds and sends an off with effect On/Off cluster command
 **
 ** PARAMETERS:                                     Name                         Usage
 ** uint8                                           u8SourceEndPointId           Source EP Id
 ** uint8                                           u8DestinationEndPointId      Destination EP Id
 ** tsZCL_Address                                  *psDestinationAddress         Destination Address
 ** uint8                                          *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToTiltPercentagePayload *psPayload                    Message payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToTiltPercentageSend(
    uint8                                           u8SourceEndPointId,
    uint8                                           u8DestinationEndPointId,
    tsZCL_Address                                  *psDestinationAddress,
    uint8                                          *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToTiltPercentagePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {{1, E_ZCL_UINT8, &psPayload->u8TiltPercentage}};

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  CLUSTER_ID_WINDOW_COVERING,
                                  FALSE,
                                  E_CLD_WC_CMD_GO_TO_TILT_PERCENTAGE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                  );
}
#endif
#endif /* WINDOW_COVERING_CLIENT */

#ifdef WINDOW_COVERING_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandReceive
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
PUBLIC  teZCL_Status eCLD_WindowCoveringCommandReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT);

}

#ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToLiftValueReceive
 **
 ** DESCRIPTION:
 ** handles rx of off with effect commands
 **
 ** PARAMETERS:                               Name                          Usage
 ** ZPS_tsAfEvent                             *pZPSevent                    Zigbee stack event structure
 ** tsZCL_EndPointDefinition                  *psEndPointDefinition         EP structure
 ** tsZCL_ClusterInstance                     *psClusterInstance            Cluster structure
 ** uint8                                     *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToLiftValuePayload *psPayload                    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToLiftValueReceive(
    ZPS_tsAfEvent                             *pZPSevent,
    uint8                                     *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftValuePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {{1, &u16ActualQuantity, E_ZCL_UINT16, &psPayload->u16LiftValue}};

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
#endif

#ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToLiftPercentageReceive
 **
 ** DESCRIPTION:
 ** handles rx of off with effect commands
 **
 ** PARAMETERS:                                    Name                          Usage
 ** ZPS_tsAfEvent                                  *pZPSevent                    Zigbee stack event structure
 ** tsZCL_EndPointDefinition                       *psEndPointDefinition         EP structure
 ** tsZCL_ClusterInstance                          *psClusterInstance            Cluster structure
 ** uint8                                          *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToLiftPercentagePayload *psPayload                    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToLiftPercentageReceive(
    ZPS_tsAfEvent                                  *pZPSevent,
    uint8                                          *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToLiftPercentagePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {{1, &u16ActualQuantity, E_ZCL_UINT8, &psPayload->u8LiftPercentage}};

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToTiltValueReceive
 **
 ** DESCRIPTION:
 ** handles rx of off with effect commands
 **
 ** PARAMETERS:                               Name                          Usage
 ** ZPS_tsAfEvent                             *pZPSevent                    Zigbee stack event structure
 ** tsZCL_EndPointDefinition                  *psEndPointDefinition         EP structure
 ** tsZCL_ClusterInstance                     *psClusterInstance            Cluster structure
 ** uint8                                     *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToTiltValuePayload *psPayload                    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToTiltValueReceive(
    ZPS_tsAfEvent                             *pZPSevent,
    uint8                                     *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToTiltValuePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {{1, &u16ActualQuantity, E_ZCL_UINT16, &psPayload->u16TiltValue}};

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCommandGoToTiltPercentageReceive
 **
 ** DESCRIPTION:
 ** handles rx of off with effect commands
 **
 ** PARAMETERS:                                    Name                          Usage
 ** ZPS_tsAfEvent                                  *pZPSevent                    Zigbee stack event structure
 ** tsZCL_EndPointDefinition                       *psEndPointDefinition         EP structure
 ** tsZCL_ClusterInstance                          *psClusterInstance            Cluster structure
 ** uint8                                          *pu8TransactionSequenceNumber Sequence number Pointer
 ** tsCLD_WindowCovering_GoToTiltPercentagePayload *psPayload                    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCommandGoToTiltPercentageReceive(
    ZPS_tsAfEvent                                  *pZPSevent,
    uint8                                          *pu8TransactionSequenceNumber,
    tsCLD_WindowCovering_GoToTiltPercentagePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {{1, &u16ActualQuantity, E_ZCL_UINT8, &psPayload->u8TiltPercentage}};

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
#endif
#endif /* WINDOW_COVERING_SERVER */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/