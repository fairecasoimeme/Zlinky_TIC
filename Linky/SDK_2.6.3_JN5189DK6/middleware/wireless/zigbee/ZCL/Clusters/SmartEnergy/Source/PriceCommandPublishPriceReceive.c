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
 * COMPONENT:          PriceCommandPublishPriceReceive.h
 *
 * DESCRIPTION:        Receive a publish price command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Price.h"

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
 ** NAME:       eSE_PublicPriceReceive
 **
 ** DESCRIPTION:
 ** checks validity of a user EP structure
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent              *pZPSevent                     Zigbee stack event structure
 ** tsSE_PricePublishPriceCmdPayload *psPublishPriceCmdPayload      Price Table record
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PublishPriceReceive(
                            ZPS_tsAfEvent                *pZPSevent,
                            tsSE_PricePublishPriceCmdPayload   *psPublishPriceCmdPayload,
                            uint8                      *pu8TransactionSequenceNumber,
                            uint32                     *pu32CurrentTimeInMessage)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psPublishPriceCmdPayload->u32ProviderId},
            {1, &u16ActualQuantity, E_ZCL_OSTRING,  &psPublishPriceCmdPayload->sRateLabel},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psPublishPriceCmdPayload->u32IssuerEventId},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   pu32CurrentTimeInMessage},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8UnitOfMeasure},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPublishPriceCmdPayload->u16Currency},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8PriceTrailingDigitAndPriceTier},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8NumberOfPriceTiersAndRegisterTiers},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psPublishPriceCmdPayload->u32StartTime},
            {1, &u16ActualQuantity, E_ZCL_UINT16,   &psPublishPriceCmdPayload->u16DurationInMinutes},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psPublishPriceCmdPayload->u32Price},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8PriceRatio},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psPublishPriceCmdPayload->u32GenerationPrice},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8GenerationPriceRatio},
            {1, &u16ActualQuantity, E_ZCL_UINT32,   &psPublishPriceCmdPayload->u32AlternateCostDelivered},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8AlternateCostUnit},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8AlternateCostTrailingDigit},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8NumberOfBlockThresholds},
            {1, &u16ActualQuantity, E_ZCL_UINT8,    &psPublishPriceCmdPayload->u8PriceControl},
    };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS  | E_ZCL_DISABLE_DEFAULT_RESPONSE);

}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
