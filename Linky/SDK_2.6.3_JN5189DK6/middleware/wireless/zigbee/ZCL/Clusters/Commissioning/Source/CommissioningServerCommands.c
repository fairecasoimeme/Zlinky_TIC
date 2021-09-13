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
 * MODULE:             Commissioning Cluster
 *
 * COMPONENT:          CommissioningServerCommands.h
 *
 * DESCRIPTION:        Send an Commissioning cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Commissioning.h"
#include "Commissioning_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "dbg.h"

#ifdef DEBUG_CLD_COMMISSIONING
#define TRACE_COMMS    TRUE
#else
#define TRACE_COMMS    FALSE
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
#ifdef COMMISSIONING_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_CommissioningCommandResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a commissioning cluster response command
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                               u8SourceEndPointId             Source EP Id
 ** uint8                               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address                       *psDestinationAddress           Destination Address
 ** uint8                               *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_Commissioning_ResponsePayload *psPayload                      Payload
 ** teCLD_Commissioning_Command          eCLD_Commissioning_Command     CommandID
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_CommissioningCommandResponseSend(
                                        uint8                               u8SourceEndPointId,
                                        uint8                               u8DestinationEndPointId,
                                        tsZCL_Address                       *psDestinationAddress,
                                        uint8                               *pu8TransactionSequenceNumber,
                                        tsCLD_Commissioning_ResponsePayload *psPayload,
                                        teCLD_Commissioning_Command          eCLD_Commissioning_Command)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                         E_ZCL_ENUM8,   &psPayload->u8Status},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  COMMISSIONING_CLUSTER_ID_COMMISSIONING,
                                  TRUE,
                                  eCLD_Commissioning_Command,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eCLD_CommissioningCommandRestartDeviceReceive
 **
 ** DESCRIPTION:
 ** handles rx of Restart Device command
 **
 ** PARAMETERS:                                 Name                          Usage
 ** ZPS_tsAfEvent                               *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition                    *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                       *psClusterInstance             Cluster structure
 ** uint8                                       *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_Commissioning_RestartDevicePayload    *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_CommissioningCommandRestartDeviceReceive(
                    ZPS_tsAfEvent                            *pZPSevent,
                    uint8                                    *pu8TransactionSequenceNumber,
                    tsCLD_Commissioning_RestartDevicePayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8Options},
    {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Delay},
    {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Jitter},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME: eCLD_CommissioningCommandModifyStartupParamsReceive
 **
 ** DESCRIPTION:
 ** handles rx of Save, restore and Reset Startup Parameters command
 **
 ** PARAMETERS:                                 Name                          Usage
 ** ZPS_tsAfEvent                               *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                    *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                       *psClusterInstance             Cluster structure
 ** uint8                                       *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_Commissioning_ModifyStartupParametersPayload    *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_CommissioningCommandModifyStartupParamsReceive(
                    ZPS_tsAfEvent                                       *pZPSevent,
                    uint8                                               *pu8TransactionSequenceNumber,
                    tsCLD_Commissioning_ModifyStartupParametersPayload  *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psPayload->u8Options},
    {1, &u16ActualQuantity, E_ZCL_UINT8,   &psPayload->u8Index},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif /* COMMISSIONING_SERVER */

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
