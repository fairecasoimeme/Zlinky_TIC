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
 * MODULE:             Price Cluster
 *
 * COMPONENT:          PriceCommandGetCalorificValue
 *
 * DESCRIPTION:        Send and Receive functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Price.h"
#include "Price_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"



#ifdef PRICE_CALORIFIC_VALUE

/****************************************************************************
 **
 ** NAME:       eSE_PriceGetCalorificValueSend
 **
 ** DESCRIPTION:
 ** Sends Get Current Price Request
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint8                u8numberofEvents                Number of Events
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PriceGetCalorificValueSend(
                    uint8                      u8SourceEndPointId,
                    uint8                      u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint32                       u32StartTime,
                    uint8                       u8NumberOfEvents)
{

     

    tsZCL_TxPayloadItem asPayloadDefinition[] = {   {1, E_ZCL_UINT32, &u32StartTime},
                                                    {1, E_ZCL_UINT8,  &u8NumberOfEvents}
                                                };


    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  SE_CLUSTER_ID_PRICE,
                                  FALSE,
                                  E_SE_GET_CALORIFIC_VALUE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

}

/****************************************************************************
 **
 ** NAME:       eSE_GetCalorificValueReceive
 **
 ** DESCRIPTION:
 ** Handles Get Current Price Request
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                   Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** teSE_PriceCommandOptions  *pePriceCommandOptions        Response option
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_GetCalorificValueReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint32                        *pu32StartTime,
                    uint8                        *pu8NumberOfEvents)
{
 
    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT32, pu32StartTime},
            {1, &u16ActualQuantity, E_ZCL_UINT8,  pu8NumberOfEvents}
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_DISABLE_DEFAULT_RESPONSE);

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
#endif
