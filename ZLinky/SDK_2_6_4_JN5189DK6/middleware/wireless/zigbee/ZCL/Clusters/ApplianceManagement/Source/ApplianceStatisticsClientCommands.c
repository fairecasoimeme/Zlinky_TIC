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
 * COMPONENT:          ApplianceStatisticsClientCommands.c
 *
 * DESCRIPTION:        Appliance Statistics Cluster definition
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "ApplianceStatistics.h"
#include "ApplianceStatistics_internal.h"

#include "dbg.h"


#ifdef DEBUG_CLD_APPLIANCE_STATISTICS
#define TRACE_APPLIANCE_STATISTICS    TRUE
#else
#define TRACE_APPLIANCE_STATISTICS    FALSE
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

#ifdef APPLIANCE_STATISTICS_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_ASCLogNotificationORLogResponseReceive
 **
 ** DESCRIPTION:
 ** Receives a Log notification & Log Response command
 **
 ** PARAMETERS:                                                     Name                               Usage
 ** uint8                                                           u8SourceEndPointId                 Source EP Id
 ** uint8                                                           u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                                   *psDestinationAddress              Destination Address
 ** uint8                                                           *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_ApplianceStatistics_ServerCommandId                       eCommandId                         Command Id 
 ** tsCLD_ASC_LogNotificationORLogResponsePayload                   *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ASCLogNotificationORLogResponseReceive(
                                        ZPS_tsAfEvent                                               *pZPSevent,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_ASC_LogNotificationORLogResponsePayload               *psPayload)
{

    uint16 u16ActualQuantity;
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1,                                                       &u16ActualQuantity,                  E_ZCL_UINT32,                       &(psPayload->utctTime)},
            {1,                                                       &u16ActualQuantity,                  E_ZCL_UINT32,                       &(psPayload->u32LogId)},
            {1,                                                       &u16ActualQuantity,                  E_ZCL_UINT32,                       &(psPayload->u32LogLength)},
            {CLD_APPLIANCE_STATISTICS_ATTR_LOG_MAX_SIZE,              &u16ActualQuantity,                  E_ZCL_UINT8,                        psPayload->pu8LogData},
                                                          };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ASCLogQueueResponseORStatisticsAvailableReceive
 **
 ** DESCRIPTION:
 ** Builds and sends a Log notification & Log Response command
 **
 ** PARAMETERS:                                                     Name                               Usage
 ** uint8                                                           u8SourceEndPointId                 Source EP Id
 ** uint8                                                           u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                                   *psDestinationAddress              Destination Address
 ** uint8                                                           *pu8TransactionSequenceNumber      Sequence number Pointer
 ** tsCLD_ASC_LogQueueResponseORStatisticsAvailablePayload           *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ASCLogQueueResponseORStatisticsAvailableReceive(
                                                        ZPS_tsAfEvent                                             *pZPSevent,
                                                        uint8                                                     *pu8TransactionSequenceNumber,
                                                        tsCLD_ASC_LogQueueResponseORStatisticsAvailablePayload    *psPayload)
{
    uint16 u16ActualQuantity;
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
             {1,                                                     &u16ActualQuantity,             E_ZCL_UINT8,            &(psPayload->u8LogQueueSize)},
             {CLD_APPLIANCE_STATISTICS_ATTR_LOG_QUEUE_MAX_SIZE,      &u16ActualQuantity,             E_ZCL_UINT32,           psPayload->pu32LogId},
                                                          };
    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ASCLogRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Log Request command
 **
 ** PARAMETERS:                                         Name                               Usage
 ** uint8                                               u8SourceEndPointId                 Source EP Id
 ** uint8                                               u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                       *psDestinationAddress              Destination Address
 ** uint8                                               *pu8TransactionSequenceNumber      Sequence number Pointer
 ** tsCLD_ASC_LogRequestPayload                         *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ASCLogRequestSend(
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    tsCLD_ASC_LogRequestPayload                                 *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    {1,                                     E_ZCL_UINT32,                               &psPayload->u32LogId},
                                                };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS,
                                  FALSE,
                                  E_CLD_APPLIANCE_STATISTICS_CMD_LOG_REQUEST,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/****************************************************************************
 **
 ** NAME:       eCLD_ASCLogQueueRequestSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Log Queue Request command
 **
 ** PARAMETERS:                                         Name                               Usage
 ** uint8                                               u8SourceEndPointId                 Source EP Id
 ** uint8                                               u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                       *psDestinationAddress              Destination Address
 ** uint8                                               *pu8TransactionSequenceNumber      Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ASCLogQueueRequestSend(
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber)
{


    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_STATISTICS,
                                  FALSE,
                                  E_CLD_APPLIANCE_STATISTICS_CMD_LOG_QUEUE_REQUEST,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0);
}
#endif
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
