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
 * MODULE:             Appliance Statistics Cluster
 *
 * COMPONENT:          ApplianceControlClientCommands.c
 *
 * DESCRIPTION:        Appliance Control Cluster definition
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "ApplianceControl.h"
#include "ApplianceControl_internal.h"

#include "dbg.h"


#ifdef DEBUG_CLD_APPLIANCE_CONTROL
#define TRACE_APPLIANCE_CONTROL    TRUE
#else
#define TRACE_APPLIANCE_CONTROL    FALSE
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

#ifdef APPLIANCE_CONTROL_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_ACExecutionOfCommandSend
 **
 ** DESCRIPTION:
 ** Sends an Execution of command
 **
 ** PARAMETERS:                                     Name                               Usage
 ** uint8                                           8SourceEndPointId                 Source EP Id
 ** uint8                                           u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                      *psDestinationAddress              Destination Address
 ** uint8                                              pu8TransactionSequenceNumber      Sequence number Pointer
 ** tsCLD_AC_ExecutionOfCommandPayload                 *psPayload                        Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ACExecutionOfCommandSend(
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    tsCLD_AC_ExecutionOfCommandPayload                          *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    {1,                                     E_ZCL_UINT8,                               &psPayload->eExecutionCommandId},
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_CONTROL,
                                  FALSE,
                                  E_CLD_APPLIANCE_CONTROL_CMD_EXECUTION_OF_COMMAND,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/****************************************************************************
 **
 ** NAME:       eCLD_ACSignalStateSend
 **
 ** DESCRIPTION:
 ** Sends a Signal State Command
 **
 ** PARAMETERS:                     Name                               Usage
 ** uint8                           u8SourceEndPointId                 Source EP Id
 ** uint8                           u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                   *psDestinationAddress              Destination Address
 ** uint8                           *pu8TransactionSequenceNumber      Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ACSignalStateSend(
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_CONTROL,
                                  FALSE,
                                  E_CLD_APPLIANCE_CONTROL_CMD_SIGNAL_STATE,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);
}

/****************************************************************************
 **
 ** NAME:       eCLD_ACSignalStateResponseORSignalStateNotificationReceive
 **
 ** DESCRIPTION:
 ** Builds and sends a Log notification & Log Response command
 **
 ** PARAMETERS:                                                     Name                               Usage
 ** uint8                                                           u8SourceEndPointId                 Source EP Id
 ** uint8                                                           u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                                   *psDestinationAddress              Destination Address
 ** uint8                                                           *pu8TransactionSequenceNumber      Sequence number Pointer
 **    teCLD_ApplianceControl_ServerCommandId                       eCommandId                        Command Id 
 ** tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload    *psPayload                        Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ACSignalStateResponseORSignalStateNotificationReceive(
                                                    ZPS_tsAfEvent                                                       *pZPSevent,
                                                    uint8                                                               *pu8TransactionSequenceNumber,
                                                    tsCLD_AC_SignalStateResponseORSignalStateNotificationPayload        *psPayload)
{
    uint16 u16ActualQuantity;
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
             {1,                                &u16ActualQuantity,         E_ZCL_UINT8,            &(psPayload->eApplianceStatus)},
             {1,                                &u16ActualQuantity,         E_ZCL_UINT8,            &(psPayload->u8RemoteEnableFlagAndDeviceStatusTwo)},
             {1,                                &u16ActualQuantity,         E_ZCL_UINT24,           &(psPayload->u24ApplianceStatusTwo)},
                                                          };
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);

}

#endif
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
