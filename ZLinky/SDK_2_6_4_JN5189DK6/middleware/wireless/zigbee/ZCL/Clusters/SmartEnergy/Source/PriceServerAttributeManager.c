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
 * COMPONENT:          PriceServerAttributeManager.c
 *
 * DESCRIPTION:        Managing server attributes.
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



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

#if defined(BLOCK_CHARGING) && !defined(TOU_CHARGING)
/****************************************************************************
 **
 ** NAME:       eSE_PriceAddNoTierBlockPrices
 **
 ** DESCRIPTION:
 ** Adds a No-Tier block prices to server attributes.  Only valid for a server.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** uint8                       u8NoOfNoTierBlocks            No.of No-Tier block Prices
 ** tsSE_NoTierBlockPrices      *psNoTierBlockPrices        No-tier block price values
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/
PUBLIC  teSE_PriceStatus eSE_PriceAddNoTierBlockPrices(
                            uint8 u8SourceEndPointId,
                            uint8 u8NoOfNoTierBlocks,
                            tsSE_NoTierBlockPrices *psNoTierBlockPrices)
{
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    uint16 u16AttributeId;
    uint8  u8AttrLoopIndex;

    /* error check for parameters */
    if(psNoTierBlockPrices == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if(u8NoOfNoTierBlocks > (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT + 1))
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);

    /* check return status */
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    /* NoTier Block Prices Attribute Id starts from 0x0400 */
    /* Search Attribute Entry */
    u16AttributeId = E_CLD_P_ATTR_NOTIER_BLOCK1_PRICE;
    for(u8AttrLoopIndex = 0; u8AttrLoopIndex < u8NoOfNoTierBlocks; u8AttrLoopIndex++)
    {
        eZCL_SetLocalAttributeValue(u16AttributeId,
                                    FALSE,
                                    FALSE,
                                    psEndPointDefinition,
                                    psClusterInstance,
                                    &psNoTierBlockPrices->au32NoTierBlockPrices[u8AttrLoopIndex]);

        u16AttributeId++;
    }
    return E_ZCL_SUCCESS;
}
#endif /* defined(BLOCK_CHARGING) && !defined(TOU_CHARGING) */


#if defined(BLOCK_CHARGING) && defined(TOU_CHARGING)
/****************************************************************************
 **
 ** NAME:       eSE_PriceAddTierBlockPrices
 **
 ** DESCRIPTION:
 ** Adds a No-Tier block prices to server attributes.  Only valid for a server.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** uint8                         u8NoOfTiers                    No.of Tiers
 ** uint8                         u8NoOfBlocks                No. Of blocks
 ** tsSE_TierBlockPrices         *psTierBlockPrices          Tier-block price values
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/
PUBLIC  teSE_PriceStatus eSE_PriceAddTierBlockPrices(
                            uint8 u8SourceEndPointId, uint8 u8NoOfTiers,
                            uint8 u8NoOfBlocks,
                            tsSE_TierBlockPrices *psTierBlockPrices)
{
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    uint16 u16AttributeId;
    uint8  u8AttrLoopIndex, u8TierLoopIndex, u8Index;

    /* error check for parameters */
    if( (u8NoOfBlocks > (CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT + 1))  ||
        (u8NoOfTiers > CLD_P_ATTR_NUM_OF_TIERS_PRICE) )
    {
        return(E_ZCL_ERR_EP_RANGE);;
    }

    if(psTierBlockPrices == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);

    /* check return status */
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    /* Tier Block Prices Attribute Id starts from 0x0410 */
    /* Search Attribute Entry */
    for(u8TierLoopIndex = 0; u8TierLoopIndex < u8NoOfTiers; u8TierLoopIndex++)
    {
        /* Initialize Attribute ID to start Tier-Block1 Price */
        u16AttributeId = E_CLD_P_ATTR_TIER1_BLOCK1_PRICE +
                            (u8TierLoopIndex * CLD_P_MAX_BLOCKS_PER_TIER);

        u8Index = u8TierLoopIndex * u8NoOfBlocks; /* To access Tier-Price values */
        for(u8AttrLoopIndex = 0; u8AttrLoopIndex < u8NoOfBlocks; u8AttrLoopIndex++)
        {
               eZCL_SetLocalAttributeValue(u16AttributeId,
                                                FALSE,
                                                FALSE,
                                                &u8TransactionSequenceNumber)
                                                psClusterInstance,
                                                &psTierBlockPrices->au32TierBlockPrices[u8Index]);

            u8Index++;            /* Increment Index */
            u16AttributeId++;    /* Increment Attr Id */
        }
    }
    return E_ZCL_SUCCESS;
}
#endif /* defined(BLOCK_CHARGING) && defined(TOU_CHARGING) */

#ifdef BLOCK_CHARGING
/****************************************************************************
 **
 ** NAME:       eSE_PriceAddBlockThresholds
 **
 ** DESCRIPTION:
 ** Adds block thresholds to server attributes.  Only valid for a server.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** tsSE_BlockThresholds        *psBlockThresholds          block threshold values
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/
PUBLIC  teSE_PriceStatus eSE_PriceAddBlockThresholds(
                            uint8 u8SourceEndPointId,
                            tsSE_BlockThresholds *psBlockThresholds)
{
    teSE_PriceStatus eFindPriceClusterReturn;
    tsSE_PriceCustomDataStructure *psPriceCustomDataStructure;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;

    uint16 u16AttributeId;
    uint8  u8AttrLoopIndex;
    uint8  u8NoOfBlockThresholds;

    /* error check for parameters */
    if(psBlockThresholds == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }
    else if(psBlockThresholds->u8NoOfBlockThreshold > CLD_P_ATTR_BLOCK_THRESHOLD_MAX_COUNT)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    // error check via EP number
    eFindPriceClusterReturn = eSE_FindPriceCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psPriceCustomDataStructure);

    /* check return status */
    if(eFindPriceClusterReturn != E_ZCL_SUCCESS)
    {
        return eFindPriceClusterReturn;
    }

    u8NoOfBlockThresholds = psBlockThresholds->u8NoOfBlockThreshold;

    /* Block Thresholds Attribute Id starts from 0x0100 */
    /* Search Attribute Entry */
    u16AttributeId = E_CLD_P_ATTR_BLOCK1_THRESHOLD;
    for(u8AttrLoopIndex = 0; u8AttrLoopIndex < u8NoOfBlockThresholds; u8AttrLoopIndex++)
    {
        eZCL_SetLocalAttributeValue(u16AttributeId,
                                    FALSE,
                                    FALSE,
                                    psEndPointDefinition,
                                    psClusterInstance,
                                    &psBlockThresholds->au48BlockThreholds[u8AttrLoopIndex]);

        u16AttributeId++;
    }

    return E_ZCL_SUCCESS;
}
#endif /* BLOCK_CHARGING */
