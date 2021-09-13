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
 * MODULE:             Poll Control Cluster
 *
 * COMPONENT:          PollControlServerCommands.c
 *
 * DESCRIPTION:        Poll Control Cluster definition
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "PollControl.h"
#include "PollControl_internal.h"

#include "dbg.h"


#ifdef DEBUG_CLD_POLL_CONTROL
#define TRACE_POLL_CONTROL    TRUE
#else
#define TRACE_POLL_CONTROL    FALSE
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

#ifdef POLL_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlCheckinSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Checkin Command
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlCheckinSend(
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber)
{

    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_POLL_CONTROL,
                                  TRUE,
                                  E_CLD_POLL_CONTROL_CMD_CHECKIN,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);

}

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlCheckinResponseReceive
 **
 ** DESCRIPTION:
 ** handles rx of Check in Response
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PollControl_CheckinResponsePayload         *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlCheckinResponseReceive(
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PollControl_CheckinResponsePayload        *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_BOOL,                 &psPayload->bStartFastPolling},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,               &psPayload->u16FastPollTimeout},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlFastPollStopReceive
 **
 ** DESCRIPTION:
 ** handles rx of Fast Poll Stop
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlFastPollStopReceive(
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

#ifdef CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlSetLongPollIntervalReceive
 **
 ** DESCRIPTION:
 ** handles rx of Set Long Poll Interval
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PollControl_SetLongPollIntervalPayload     *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlSetLongPollIntervalReceive(
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PollControl_SetLongPollIntervalPayload    *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT32,               &psPayload->u32NewLongPollInterval},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif

#ifdef CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlSetShortPollIntervalReceive
 **
 ** DESCRIPTION:
 ** handles rx of Set Long Poll Interval
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PollControl_SetShortPollIntervalPayload     *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlSetShortPollIntervalReceive(
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PollControl_SetShortPollIntervalPayload   *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,               &psPayload->u16NewShortPollInterval},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

}
#endif

#endif
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
