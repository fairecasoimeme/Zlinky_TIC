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
 * COMPONENT:          PriceCommandPublishCalorificValueReceive.c
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



#ifdef PRICE_CALORIFIC_VALUE

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
 ** NAME:       eSE_PublishBlockPeriodPriceReceive
 **
 ** DESCRIPTION:
 ** Receives the Publish Block Period command from ZCL.
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsSE_PricePublishBlockPeriodCmdPayload *psPublishBlockPeriodCmdPayload      Block Period Table record
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** uint32                   *pu32CurrentTimeInMessage      Timestamp
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teSE_PriceStatus eSE_PublishCalorificValueReceive(
                            ZPS_tsAfEvent              *pZPSevent,
                            tsSE_PricePublishCalorificValueCmdPayload   *psPublishCalorificValueCmdPayload,
                            uint8                      *pu8TransactionSequenceNumber,
                            uint32                     *pu32CurrentTimeInMessage)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
                {1,&u16ActualQuantity, E_ZCL_UINT32,   &psPublishCalorificValueCmdPayload->u32IssuerEventId},
                {1,&u16ActualQuantity, E_ZCL_UINT32,   &psPublishCalorificValueCmdPayload->u32StartTime},
                {1,&u16ActualQuantity, E_ZCL_UINT32,   &psPublishCalorificValueCmdPayload->u32CalorificValue},
                {1,&u16ActualQuantity, E_ZCL_ENUM8,    &psPublishCalorificValueCmdPayload->e8CalorificValueUnit},
                {1,&u16ActualQuantity, E_ZCL_BMAP8,    &psPublishCalorificValueCmdPayload->b8CalorificValueTrailingDigit},
                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);



}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif
