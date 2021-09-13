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


/****************************************************************************
 *
 * MODULE:             Zigbee Demand Response And Load Control Cluster
 *
 * DESCRIPTION:
 * eSE_DRLCGetScheduledEventsSend Send and Receive functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "DRLC.h"
#include "DRLC_internal.h"
#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"


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
 ** NAME:       eSE_DRLCGetScheduledEventsSend
 **
 ** DESCRIPTION:
 ** Sends Load Control Event
 **
 ** PARAMETERS:                     Name                            Usage
 ** uint8                           u8SourceEndPointId              Source EP Id
 ** uint8                           u8DestinationEndPointId         Destination EP Id
 ** tsZCL_Address                   *psDestinationAddress           Destination Address
 ** tsSE_DRLCGetScheduledEvents     *psGetScheduledEvents           scheduled events data
 ** uint8                           *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCGetScheduledEventsSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    tsSE_DRLCGetScheduledEvents *psGetScheduledEvents,
                    uint8                       *pu8TransactionSequenceNumber)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    {1, E_ZCL_UINT32,   &psGetScheduledEvents->u32StartTime},
    {1, E_ZCL_UINT8,    &psGetScheduledEvents->u8numberOfEvents},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              SE_CLUSTER_ID_DEMAND_RESPONSE_AND_LOAD_CONTROL,
                              FALSE,
                              SE_DRLC_GET_SCHEDULED_EVENTS,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
}

/****************************************************************************
 **
 ** NAME:       eSE_GetScheduledEventsReceive
 **
 ** DESCRIPTION:
 ** Gets the Get Scheduled Event command field values
 **
 ** PARAMETERS:                 Name                            Usage
 ** ZPS_tsAfEvent               *pZPSevent                      Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition           EP structure
 ** tsSE_DRLCGetScheduledEvents *psGetScheduledEvents           scheduled events details
 ** uint8                       *pu8TransactionSequenceNumber   Sequence number Pointer
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCGetScheduledEventsReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsSE_DRLCGetScheduledEvents *psGetScheduledEvents,
                    uint8                       *pu8TransactionSequenceNumber)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT32,   &psGetScheduledEvents->u32StartTime},
        {1, &u16ActualQuantity, E_ZCL_UINT8,    &psGetScheduledEvents->u8numberOfEvents},
                                                };

    if(eZCL_CustomCommandDRLCReceive(pZPSevent,
                                 psEndPointDefinition,
                                 pu8TransactionSequenceNumber,
                                 asPayloadDefinition,
                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                 E_ZCL_DISABLE_DEFAULT_RESPONSE) != E_ZCL_SUCCESS)
    {
        return E_ZCL_FAIL;
    }

    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
