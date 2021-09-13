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
 * MODULE:             Groups Cluster
 *
 * COMPONENT:          Groups.c
 *
 * DESCRIPTION:        Groups cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "dlist.h"
#include "OnOff.h"
#include "zcl_options.h"
#include "string.h"
#include "Groups_internal.h"


#ifdef CLD_GROUPS

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
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_GroupsClusterCommandDefinitions[] = {
        {E_CLD_GROUPS_CMD_ADD_GROUP,                E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_GROUPS_CMD_VIEW_GROUP,               E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_GROUPS_CMD_GET_GROUP_MEMBERSHIP,     E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_GROUPS_CMD_REMOVE_GROUP,             E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_GROUPS_CMD_REMOVE_ALL_GROUPS,        E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_GROUPS_CMD_ADD_GROUP_IF_IDENTIFYING, E_ZCL_CF_RX}     /* Mandatory */

    };
#endif
const tsZCL_AttributeDefinition asCLD_GroupsClusterAttributeDefinitions[] = {
#ifdef GROUPS_SERVER
    {E_CLD_GROUPS_ATTR_ID_NAME_SUPPORT, 		E_ZCL_AF_RD,                E_ZCL_BMAP8,    (uint32)(&((tsCLD_Groups*)(0))->u8NameSupport),0},     /* Mandatory */
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,              (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_BMAP32,   (uint32)(&((tsCLD_Groups*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
	{E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,  	(E_ZCL_AF_RD|E_ZCL_AF_GA),	E_ZCL_UINT16,   (uint32)(&((tsCLD_Groups*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};

tsZCL_ClusterDefinition sCLD_Groups = {
        GENERAL_CLUSTER_ID_GROUPS,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_GroupsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_GroupsClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_GroupsClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_GroupsClusterCommandDefinitions         
        #endif        
};
uint8 au8GroupsAttributeControlBits[(sizeof(asCLD_GroupsClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:       eCLD_GroupsCreateGroups
 *
 * DESCRIPTION:
 * Creates an on/off cluster
 *
 * PARAMETERS:  Name                        Usage
 *              psClusterInstance           Cluster structure
 *              bIsServer                   Server Client Flag
 *              psClusterDefinition         Pointer to Cluster Definition
 *              pvEndPointSharedStructPtr   Pointer to EndPoint Shared Structure
 *              pu8AttributeControlBits     Pointer to Attribute Control Bits
 *              psCustomDataStructure       Pointer to Custom Data Structure
 * RETURN:
 * teZCL_Status
 *
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_GroupsCreateGroups(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsCLD_GroupsCustomDataStructure    *psCustomDataStructure,
                tsZCL_EndPointDefinition           *psEndPointDefinition)
{

    #if (defined CLD_GROUPS) && (defined GROUPS_SERVER)
    tsCLD_GroupTableEntry *psTableEntry;
    uint8 u8ByteOffset, u8BitOffset;
    uint32 n;
    #endif
    
    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) || (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    // cluster data
    vZCL_InitializeClusterInstance(
               psClusterInstance, 
               bIsServer,
               psClusterDefinition,
               pvEndPointSharedStructPtr,
               pu8AttributeControlBits,
               psCustomDataStructure,
               eCLD_GroupsCommandHandler);
                   
    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance;                    

    psCustomDataStructure->bIdentifying = FALSE;

    /* initialise lists */
    vDLISTinitialise(&((tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr)->lGroupsAllocList);
    vDLISTinitialise(&((tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr)->lGroupsDeAllocList);
    #ifdef GROUPS_SERVER
	    ZPS_tsAplAib *psAib;
        for(n=0; n < CLD_GROUPS_MAX_NUMBER_OF_GROUPS; n++)
        {
            /* add all header slots to the to free list */
            vDLISTaddToTail(&psCustomDataStructure->lGroupsDeAllocList, (DNODE *)&psCustomDataStructure->asGroupTableEntry[n]);
        }
        
        /* Get AIB find out group table capacity */
        psAib = ZPS_psAplAibGetAib();

        if(psAib->psAplApsmeGroupTable->u32SizeOfGroupTable < CLD_GROUPS_MAX_NUMBER_OF_GROUPS)
        {
            return E_CLD_GROUPS_TABLE_SIZE_MISMATCH;
        }

        /* Check psAplApsmeGroupTable and update lGroupsDeAllocList and lGroupsAllocList */
        /* Calculate endpoint bit position in group table array */
        u8ByteOffset = (psEndPointDefinition->u8EndPointNumber - 1) / 8;
        u8BitOffset  = (psEndPointDefinition->u8EndPointNumber - 1) % 8;

        for(n = 0; n < psAib->psAplApsmeGroupTable->u32SizeOfGroupTable; n++)
        {
            if((psAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[n].au8Endpoint[u8ByteOffset] & (1 << u8BitOffset)) != 0)
            {
                /* Get a free table entry */
                psTableEntry = (tsCLD_GroupTableEntry*)psDLISTgetHead(&psCustomDataStructure->lGroupsDeAllocList);
                if(psTableEntry == NULL)
                {
                    return E_ZCL_ERR_INSUFFICIENT_SPACE;
                }
                /* Remove from list of free table entries */
                psDLISTremove(&psCustomDataStructure->lGroupsDeAllocList, (DNODE*)psTableEntry);
                /* Add to allocated list */
                vDLISTaddToTail(&psCustomDataStructure->lGroupsAllocList, (DNODE*)psTableEntry);

                /* Fill in table entry */
                psTableEntry->u16GroupId = psAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[n].u16Groupid;
                memset(psTableEntry->au8GroupName, 0, sizeof(psTableEntry->au8GroupName));
            }
        }
    #endif    
        /* Initialise attributes */
        if(pvEndPointSharedStructPtr != NULL)
        {
            #ifdef GROUPS_SERVER    
                ((tsCLD_Groups*)pvEndPointSharedStructPtr)->u8NameSupport = CLD_GROUPS_NAME_SUPPORT;
            #endif
            ((tsCLD_Groups*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_GROUPS_FEATURE_MAP ;
            ((tsCLD_Groups*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_GROUPS_CLUSTER_REVISION ;
        }


    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_GroupsSetIdentifying
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Endpoint id
 ** bool_t                      bIsIdentifying              Identify state
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_GroupsSetIdentifying(uint8 u8SourceEndPointId,
                                                              bool_t bIsIdentifying)
{
    uint8 u8Status;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Find pointers to cluster */
    u8Status = eZCL_FindCluster(GENERAL_CLUSTER_ID_GROUPS, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(u8Status != E_ZCL_SUCCESS)
    {
        return u8Status;
    }

    psCommon->bIdentifying = bIsIdentifying;

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsAdd
 **
 ** DESCRIPTION:
 ** Adds a group
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                       u8SourceEndPointId      Endpoint id
 ** uint16                      u16GroupId              Group Id
 ** uint8 *                     pu8GroupName            Pointer to group name
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_GroupsAdd(uint8 u8SourceEndPointId,
                                                   uint16 u16GroupId,
                                                   uint8 *pu8GroupName)
{
    uint8 u8Status;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_GroupsCustomDataStructure *psCommon;
    tsCLD_Groups_AddGroupRequestPayload sPayload;

    /* Find pointers to cluster */
    u8Status = eZCL_FindCluster(GENERAL_CLUSTER_ID_GROUPS, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(u8Status != E_ZCL_SUCCESS)
    {
        return u8Status;
    }

    sPayload.u16GroupId = u16GroupId;
    sPayload.sGroupName.pu8Data = pu8GroupName;
    sPayload.sGroupName.u8Length = strlen((char*)pu8GroupName);
    sPayload.sGroupName.u8MaxLength = sPayload.sGroupName.u8Length;

    return eCLD_GroupsAddGroup(psEndPointDefinition, psClusterInstance, &sPayload);

}

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsCheckGroupExists
 **
 ** DESCRIPTION:
 ** Search a group in Group Table
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** uint16                      u16GroupId
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_GroupsCheckGroupExists(
                                                tsZCL_EndPointDefinition    *psEndPointDefinition,
                                                uint16                      u16GroupId)
{

    ZPS_tsAplAib *psAib;
    uint8 u8ByteOffset;
    uint8 u8BitOffset;
    uint32 n;
    
    /* Parameter check */
    #ifdef STRICT_PARAM_CHECK
        if(psEndPointDefinition == NULL)
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Calculate endpoint bit position in group table array */
    u8ByteOffset = (psEndPointDefinition->u8EndPointNumber - 1) / 8;
    u8BitOffset  = (psEndPointDefinition->u8EndPointNumber - 1) % 8;

    /* Get AIB and search group table for a matching group entry */
    psAib = ZPS_psAplAibGetAib();

    for(n = 0; n < psAib->psAplApsmeGroupTable->u32SizeOfGroupTable; n++)
    {
        if((psAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[n].u16Groupid == u16GroupId) &&
           ((psAib->psAplApsmeGroupTable->psAplApsmeGroupTableId[n].au8Endpoint[u8ByteOffset] & (1 << u8BitOffset)) != 0)
          )
        {
            return E_ZCL_SUCCESS;
        }
    }
    return E_ZCL_ERR_INVALID_VALUE;
}
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

