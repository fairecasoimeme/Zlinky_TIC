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
 * COMPONENT:          PriceCustomCommandHandler.c
 *
 * DESCRIPTION:        Event handler for price cluster custom commands
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Price.h"
#include "Price_internal.h"

#include "zcl_options.h"



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_PRICE) && !defined(PRICE_SERVER) && !defined(PRICE_CLIENT)
#error You Must Have either PRICE_SERVER and/or PRICE_CLIENT defined in zcl_options.h
#endif

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
 ** NAME:       eSE_PriceCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Price Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent              *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eSE_PriceCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // store the incoming event details which can be used to send events back to user
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &((tsSE_PriceCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sPrice_Common.sReceiveEventAddress);
    switch(psClusterInstance->bIsServer)
    {
#ifdef PRICE_SERVER
        case(TRUE):
        {
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {
                case(E_SE_GET_CURRENT_PRICE):
                {
                    vSE_HandleGetCurrentPrice(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                case(E_SE_GET_SCHEDULED_PRICES):
                {
                    vSE_HandleGetSheduledPrices(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                case (E_SE_PRICE_ACK):
                {
                    vSE_HandlePriceAck(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }

#ifdef BLOCK_CHARGING
                case (E_SE_GET_BLOCK_PERIOD):
                {
                    vSE_HandleGetBlockPeriods(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
#endif /* BLOCK_CHARGING */

#ifdef PRICE_CONVERSION_FACTOR
                case(E_SE_GET_CONVERSION_FACTOR):
                {
                    vSE_HandleGetConversionFactor(pZPSevent,psEndPointDefinition, psClusterInstance);
                    break;
                }
#endif /*PRICE_CONVERSION_FACTOR */

#ifdef PRICE_CALORIFIC_VALUE
                case(E_SE_GET_CALORIFIC_VALUE):
                {
                    vSE_HandleGetCalorificValue(pZPSevent,psEndPointDefinition, psClusterInstance);
                    break;
                }
#endif /* PRICE_CALORIFIC_VALUE */

                default:
                {
                    return(E_ZCL_FAIL);
                    break;
                }
            }
            break;
        }
#endif

#ifdef PRICE_CLIENT
        case(FALSE):
        {
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {
                case(E_SE_PUBLISH_PRICE):
                {
                    vSE_HandlePublishPrice(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }

#ifdef BLOCK_CHARGING
                case(E_SE_PUBLISH_BLOCK_PERIOD):
                {
                    vSE_HandlePublishBlockPeriod(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
#endif /* BLOCK_CHARGING */

#ifdef PRICE_CALORIFIC_VALUE
                case (E_SE_PUBLISH_CALORIFIC_VALUE):
                {
                    vSE_HandlePublishCalorificValue(pZPSevent,psEndPointDefinition,psClusterInstance);
                    break;
                }
#endif

#ifdef PRICE_CONVERSION_FACTOR
                case (E_SE_PUBLISH_CONVERSION_FACTOR):
                {
                    vSE_HandlePublishConversionFactor(pZPSevent,psEndPointDefinition,psClusterInstance);
                    break;
                }
#endif
                default:
                {
                    return(E_ZCL_FAIL);
                    break;
                }
            }
            break;
        }
#endif

       // ifdef failsafe
        default:
        {
            return(E_ZCL_FAIL);
            break;
        }
    }


#if defined(PRICE_SERVER) || defined(PRICE_CLIENT)
    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);
#endif

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

