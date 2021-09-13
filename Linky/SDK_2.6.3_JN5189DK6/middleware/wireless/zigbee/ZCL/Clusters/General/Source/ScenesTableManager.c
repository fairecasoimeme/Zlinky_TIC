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
 * MODULE:             Scenes Cluster
 *
 * COMPONENT:          ScenesTableManager.c
 *
 * DESCRIPTION:        Scene management functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Scenes.h"
#include "Scenes_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "string.h"
#include "Groups.h"


#include "dbg.h"

#ifdef DEBUG_CLD_SCENES
#define TRACE_SCENES    TRUE
#else
#define TRACE_SCENES    FALSE
#endif

#ifdef SCENES_SERVER
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define CLD_SCENES_SEARCH_GROUP_ID      (1 << 0)
#define CLD_SCENES_SEARCH_SCENE_ID      (1 << 1)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef struct
{
    uint8   u8SearchOptions;
    uint16  u16GroupId;
    uint8   u8SceneId;
} tsCLD_ScenesMatch;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE bool_t bCLD_ScenesSearchForScene(void *pvSearchParam, void *psNodeUnderTest);


#ifdef  CLD_SCENES_CMD_COPY_SCENE
PRIVATE teZCL_Status eCLD_ScenesCopySceneTableEntry(
                            tsCLD_ScenesTableEntry      *psTo,
                            tsCLD_ScenesTableEntry      *psFrom,
                            uint16                      u16ToGroupId,
                            uint8                       u8ToSceneId);
#endif
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
 ** NAME:       eCLD_ScenesAddScene
 **
 ** DESCRIPTION:
 ** Adds a scene to the table
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** tsCLD_ScenesAddSceneRequestPayload *psPayload
 ** bool_t                      bEnhanced
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesAddScene(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsCLD_ScenesAddSceneRequestPayload *psPayload,
                            bool_t                      bEnhanced)
{

    tsCLD_ScenesTableEntry *psTableEntry;
    tsCLD_ScenesCustomDataStructure *psCommon;
    tsCLD_ScenesMatch sSearchParameter;
    teZCL_Status eStatus;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition == NULL)           ||
           (psClusterInstance == NULL)              ||
           (psPayload == NULL)                      ||
           (psPayload->sSceneName.pu8Data == NULL)  ||
           (psPayload->sExtensionField.pu8Data == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Check that the group exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, psPayload->u16GroupId);

    /* Group 0 really exist, its a special case for the global scene */
    if((eStatus != E_ZCL_SUCCESS) && (psPayload->u16GroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return eStatus;
    }

    /* Search list for any existing entry to avoid duplication */
    sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID|CLD_SCENES_SEARCH_SCENE_ID);
    sSearchParameter.u16GroupId = psPayload->u16GroupId;
    sSearchParameter.u8SceneId  = psPayload->u8SceneId;
    psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);

    /* If no matching entry is found that we can update, try and get a free table entry */
    if(psTableEntry == NULL)
    {
        DBG_vPrintf(TRACE_SCENES, "\nNo Existing scene entry in table");

        /* Get a free table entry */
        psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTgetHead(&psCommon->lScenesDeAllocList);
        if(psTableEntry == NULL)
        {
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif


            DBG_vPrintf(TRACE_SCENES, "\nError: Insufficient space");

            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }

        /* Remove from list of free table entries */
        psDLISTremove(&psCommon->lScenesDeAllocList, (DNODE*)psTableEntry);

        /* Add to allocated list */
        vDLISTaddToTail(&psCommon->lScenesAllocList, (DNODE*)psTableEntry);

    }

    /* If scene name length is invalid (0xff), set length to 0 */
    if(psPayload->sSceneName.u8Length == 0xff)
    {
        psPayload->sSceneName.u8Length = 0;
    }

    /* Fill in table entry */
    psTableEntry->bActive               = FALSE;
    psTableEntry->u16GroupId            = psPayload->u16GroupId;
    psTableEntry->u8SceneId             = psPayload->u8SceneId;

    if(bEnhanced == FALSE)
    {
        psTableEntry->u16TransitionTime     = psPayload->u16TransitionTime;

    #ifdef CLD_SCENES_TABLE_SUPPORT_TRANSITION_TIME_IN_MS
        psTableEntry->u8TransitionTime100ms = 0;
    #endif

    }
    else
    {
        psTableEntry->u16TransitionTime     = psPayload->u16TransitionTime / 10;

    #ifdef CLD_SCENES_TABLE_SUPPORT_TRANSITION_TIME_IN_MS
        psTableEntry->u8TransitionTime100ms = psPayload->u16TransitionTime % 10;
    #endif

    }

    psTableEntry->u32TransitionTimer    = 0;
    psTableEntry->u8SceneNameLength     = psPayload->sSceneName.u8Length;

    memset(psTableEntry->au8SceneName, 0, sizeof(psTableEntry->au8SceneName));
    DBG_vPrintf(TRACE_SCENES, "\nScene name length is %d bytes", psPayload->sSceneName.u8Length);
    strncpy((char*)psTableEntry->au8SceneName, (char*)psPayload->sSceneName.pu8Data, psPayload->sSceneName.u8Length);
    
    memset(psTableEntry->au8SceneData, 0, sizeof(psTableEntry->au8SceneData));
    DBG_vPrintf(TRACE_SCENES, "\nCopying %d bytes of scene extension data into table", psPayload->sExtensionField.u16Length);

    psTableEntry->u16SceneDataLength = psPayload->sExtensionField.u16Length;
    memcpy(&psTableEntry->au8SceneData, psPayload->sExtensionField.pu8Data, psPayload->sExtensionField.u16Length);


    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesStoreScene
 **
 ** DESCRIPTION:
 ** Used when storing a scene. Returns a pointer to a table entry for the scene
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** uint16                      u16GroupId
 ** uint8                       u8SceneId
 ** tsCLD_ScenesTableEntry      *psStoreTableEntry
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesStoreScene(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16GroupId,
                            uint8                       u8SceneId,
                            tsCLD_ScenesTableEntry      **ppsTableEntry)
{

    tsCLD_ScenesTableEntry *psTableEntry;
    tsCLD_ScenesTableEntry *psOldActiveScene;
    tsCLD_ScenesCustomDataStructure *psCommon;
    tsCLD_ScenesMatch sSearchParameter;
    teZCL_Status eStatus;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition == NULL)           ||
           (psClusterInstance == NULL)              ||
           (ppsTableEntry == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Check that the group exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16GroupId);

    /* Group 0 really exist, its a special case for the global scene */
    if((eStatus != E_ZCL_SUCCESS) && (u16GroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return eStatus;
    }


    /* Search list for any existing entry to avoid duplication */
    sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID|CLD_SCENES_SEARCH_SCENE_ID);
    sSearchParameter.u16GroupId = u16GroupId;
    sSearchParameter.u8SceneId  = u8SceneId;
    psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);

    /* If no matching entry is found that we can update, try and get a free table entry */
    if(psTableEntry == NULL)
    {
        DBG_vPrintf(TRACE_SCENES, "\nNo Existing scene entry in table");

        /* Get a free table entry */
        psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTgetHead(&psCommon->lScenesDeAllocList);
        if(psTableEntry == NULL)
        {
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif


            DBG_vPrintf(TRACE_SCENES, "\nError: Insufficient space");

            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }

        /* Remove from list of free table entries */
        psDLISTremove(&psCommon->lScenesDeAllocList, (DNODE*)psTableEntry);

        /* Add to allocated list */
        vDLISTaddToTail(&psCommon->lScenesAllocList, (DNODE*)psTableEntry);

        /* Fill in table entry */
        psTableEntry->bActive               = FALSE;
        psTableEntry->u16GroupId            = u16GroupId;
        psTableEntry->u8SceneId             = u8SceneId;
        psTableEntry->u16SceneDataLength    = 0;
        psTableEntry->u8SceneNameLength     = 0;
        psTableEntry->u16TransitionTime     = 0;
        memset(psTableEntry->au8SceneName, 0, sizeof(psTableEntry->au8SceneName));
        memset(psTableEntry->au8SceneData, 0, sizeof(psTableEntry->au8SceneData));

    }

    /* See if there are any active scenes */
    eStatus = eCLD_ScenesFindActiveScene(psEndPointDefinition,
                                         psClusterInstance,
                                         &psOldActiveScene);
    /* If an active scene was found, make it inactive and set transition timer to 0 */
    if(eStatus == E_ZCL_SUCCESS)
    {
        psOldActiveScene->bActive = FALSE;
        psOldActiveScene->u32TransitionTimer = 0;
    }
    /* mark as active */
    psTableEntry->bActive = TRUE;

    /* Return a pointer to the table entry */
    *ppsTableEntry = psTableEntry;

    /* Update the u8SceneCount */
    eCLD_ScenesCountScenes(psEndPointDefinition, psClusterInstance, &((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       PUBLIC eCLD_ScenesFindScene
 **
 ** DESCRIPTION:
 ** Searches both tables for a matching scene
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** uint16                      u16GroupId                     group to find
 ** uint8                       u8SceneId                       scene to find
 ** tsCLD_ScenesTableEntry       **ppsTableEntry            Returned pointer
 **                                                             to matching group
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesFindScene(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16GroupId,
                            uint8                       u8SceneId,
                            tsCLD_ScenesTableEntry      **ppsTableEntry)
{

    tsCLD_ScenesTableEntry *psTableEntry;
    tsCLD_ScenesCustomDataStructure *psCommon;
    tsCLD_ScenesMatch sSearchParameter;
    teZCL_Status eStatus;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition==NULL) || (psClusterInstance==NULL) || (ppsTableEntry==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Check that the group exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16GroupId);

    /* Group 0 really exist, its a special case for the global scene */
    if((eStatus != E_ZCL_SUCCESS) && (u16GroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return eStatus;
    }

    eStatus = E_ZCL_ERR_SCENE_NOT_FOUND;

    /* Search table for a matching entry */
    sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID|CLD_SCENES_SEARCH_SCENE_ID);
    sSearchParameter.u16GroupId = u16GroupId;
    sSearchParameter.u8SceneId  = u8SceneId;
    psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);
    if(psTableEntry != NULL)
    {
        DBG_vPrintf(TRACE_SCENES, "\nFound matching scene entry in table");
        *ppsTableEntry = psTableEntry;
        eStatus = E_ZCL_SUCCESS;
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}


/****************************************************************************
 **
 ** NAME:       PUBLIC eCLD_ScenesFindActiveScene
 **
 ** DESCRIPTION:
 ** Searches both tables for a matching scene
 **
 ** PARAMETERS:                 Name                   Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** tsCLD_ScenesTableEntry      **ppsTableEntry        Returned pointer to matching group
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesFindActiveScene(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsCLD_ScenesTableEntry      **ppsTableEntry)
{
    tsCLD_ScenesTableEntry *psTableEntry;
    tsCLD_ScenesCustomDataStructure *psCommon;
    teZCL_Status eStatus = E_ZCL_FAIL;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition==NULL) || (psClusterInstance==NULL) || (ppsTableEntry==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Search table for an active entry */
    psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTgetHead(&psCommon->lScenesAllocList);
    while(psTableEntry != NULL)
    {
        if(psTableEntry->bActive == TRUE)
        {
            eStatus = E_ZCL_SUCCESS;
            break;
        }
        psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTgetNext((DNODE*)psTableEntry);
    }

    *ppsTableEntry = psTableEntry;

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}


/****************************************************************************
 **
 ** NAME:       PUBLIC eCLD_ScenesListScenes
 **
 ** DESCRIPTION:
 ** Generates a list of scenes
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** uint16                      u16GroupId
 ** uint8                       *pu8NumberOfScenes
 ** uint8                       *pau8ListOfScenes
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesListScenes(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16GroupId,
                            uint8                       *pu8NumberOfScenes,
                            uint8                       *pau8ListOfScenes)
{
    int n;
    tsCLD_ScenesCustomDataStructure *psCommon;
    tsCLD_ScenesTableEntry *psTableEntry;
    teZCL_Status eStatus;

    /* Parameter check */
#ifdef STRICT_PARAM_CHECK
    if((psEndPointDefinition == NULL) || (pau8ListOfScenes == NULL) || (pu8NumberOfScenes == NULL))
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }
#endif
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Check that the group exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16GroupId);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    *pu8NumberOfScenes = 0;

    psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTgetHead(&psCommon->lScenesAllocList);

    for(n = 0; n < iDLISTnumberOfNodes(&psCommon->lScenesAllocList); n++)
    {

        if((psTableEntry != NULL) && (psTableEntry->u16GroupId == u16GroupId))
        {
            pau8ListOfScenes[(*pu8NumberOfScenes)++] = psTableEntry->u8SceneId;
        }

        psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTgetNext((DNODE*)psTableEntry);
    }

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       PUBLIC eCLD_ScenesCountScenes
 **
 ** DESCRIPTION:
 ** Returns the number of scenes
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** uint8                       *pu8NumberOfScenes
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCountScenes(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       *pu8NumberOfScenes)
{

    tsCLD_ScenesCustomDataStructure *psCommon;
    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition == NULL)   ||
           (psClusterInstance == NULL)      ||
           (pu8NumberOfScenes == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    *pu8NumberOfScenes = (uint8)iDLISTnumberOfNodes(&psCommon->lScenesAllocList);

    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesRemoveScene
 **
 ** DESCRIPTION:
 ** Removes a scene from the table
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** tsCLD_ScenesRemoveSceneRequestPayload *psPayload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesRemoveScene(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsCLD_ScenesRemoveSceneRequestPayload *psPayload)
{

    tsCLD_ScenesTableEntry *psTableEntry;
    tsCLD_ScenesCustomDataStructure *psCommon;
    tsCLD_ScenesMatch sSearchParameter;
    teZCL_Status eStatus;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition == NULL)           ||
           (psClusterInstance == NULL)              ||
           (psPayload == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Check that the group exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, psPayload->u16GroupId);

    /* Group 0 really exist, its a special case for the Global scene */
    if((eStatus != E_ZCL_SUCCESS) && (psPayload->u16GroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return eStatus;
    }

    /* Search list for any matching entry */
    sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID|CLD_SCENES_SEARCH_SCENE_ID);
    sSearchParameter.u16GroupId = psPayload->u16GroupId;
    sSearchParameter.u8SceneId  = psPayload->u8SceneId;
    psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);

    /* If no matching entry is found exit */
    if(psTableEntry == NULL)
    {
        DBG_vPrintf(TRACE_SCENES, "\nNo Existing scene entry in table");

        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif


        return E_ZCL_FAIL;

    }


    /* Remove from list of allocated table entries */
    psDLISTremove(&psCommon->lScenesAllocList, (DNODE*)psTableEntry);

    /* Add to free allocated list */
    vDLISTaddToTail(&psCommon->lScenesDeAllocList, (DNODE*)psTableEntry);

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesRemoveAllScenes
 **
 ** DESCRIPTION:
 ** Removes all scenes from the table
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId,
 ** uint16                      u16GroupId
 ** uint64                      u64LastConfiguredBy
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesRemoveAllScenes(
                            uint8                        u8SourceEndPointId,
                            uint16                       u16GroupId,
                            uint64                       u64LastConfiguredBy)
{
    int n;
    tsCLD_ScenesTableEntry *psTableEntry;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsCLD_ScenesCustomDataStructure *psCommon;
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsCLD_ScenesMatch sSearchParameter;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_SCENES, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    DBG_vPrintf(TRACE_SCENES, "\nRemoving all scenes from group %04x", u16GroupId);

    /* Check that the group exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16GroupId);

    /* Group 0 doesn't really exist, its a special case for global scene */
    if((eStatus != E_ZCL_SUCCESS) && (u16GroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return eStatus;
    }

    /* Move all allocated table entries to the unallocated list */
    for(n = 0; n < CLD_SCENES_MAX_NUMBER_OF_SCENES; n++)
    {
        /* Get an entry from the allocated list, exit if there are none */
        sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID);
        sSearchParameter.u16GroupId = u16GroupId;
        psTableEntry = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);

        if(psTableEntry == NULL)
        {
            break;
        }

        /* If group id matches, delete the entry and reduce u8SceneCount */
        if(psTableEntry->u16GroupId == u16GroupId)
        {
            /* Remove from list of allocated table entries */
            psDLISTremove(&psCommon->lScenesAllocList, (DNODE*)psTableEntry);

            /* Add to deallocated list */
            vDLISTaddToTail(&psCommon->lScenesDeAllocList, (DNODE*)psTableEntry);

            /* Reduce u8SceneCount */
            (((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount)--;
            /* Set LastConfiguredBy attribute */
#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
            ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u64LastConfiguredBy = u64LastConfiguredBy;
#endif
        }
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}

#ifdef  CLD_SCENES_CMD_COPY_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCopyScene
 **
 ** DESCRIPTION:
 ** Copy one or many scenes
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** uint16                      u16FromGroupId
 ** uint16                      u16FromSceneId
 ** uint16                      u16ToGroupId
 ** uint8                       u8ToSceneId
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCopyScene(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16FromGroupId,
                            uint8                       u8FromSceneId,
                            uint16                      u16ToGroupId,
                            uint8                       u8ToSceneId)
{
    teZCL_Status eStatus;
    tsCLD_ScenesTableEntry *psTo;
    tsCLD_ScenesTableEntry *psFrom;

    tsCLD_ScenesMatch sSearchParameter;

    tsCLD_ScenesCustomDataStructure *psCommon;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition==NULL) || (psClusterInstance==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    DBG_vPrintf(TRACE_SCENES, "\nCopying scene %d from group %04x to Scene%d Group%d", u8FromSceneId, u16FromGroupId, u8ToSceneId, u16ToGroupId);

    /* Check that the u16ToGroupId exists in the group table */
   eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16ToGroupId);
    /* Group 0 doesn't really exist, its a special case for the Global Scene */
    if((eStatus != E_ZCL_SUCCESS) && (u16ToGroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_CLD_GROUPS_NOT_FOUND;
    }

    /* Search table for a matching "From" entry */
    sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID|CLD_SCENES_SEARCH_SCENE_ID);
    sSearchParameter.u16GroupId = u16FromGroupId;
    sSearchParameter.u8SceneId  = u8FromSceneId;
    psFrom = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);
    if(psFrom == NULL)
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_CLD_SCENES_NOT_FOUND;
    }

    DBG_vPrintf(TRACE_SCENES, "\nFound matching scene entry in table");


    /* Search table for a matching "To" entry */
    sSearchParameter.u16GroupId = u16ToGroupId;
    sSearchParameter.u8SceneId  = u8ToSceneId;
    psTo = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);
    if (psTo == NULL)
    {
        psTo = (tsCLD_ScenesTableEntry*)psDLISTremoveFromHead(&psCommon->lScenesDeAllocList);
        if(psTo == NULL)
        {
            DBG_vPrintf(TRACE_SCENES, "\nNo free table entries");
            #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

            return E_CLD_SCENES_NO_RESOURCES;
        }
        else
        {
            /* Add to allocated list */
            vDLISTaddToTail(&psCommon->lScenesAllocList, (DNODE*)psTo);
        }
    }
    else
    {
        DBG_vPrintf(TRACE_SCENES, "\nTo already exists, overwriting it");
    }

    /* Copy scene table entry */
    eCLD_ScenesCopySceneTableEntry(psTo, psFrom, u16ToGroupId, u8ToSceneId);

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCopyAllScenes
 **
 ** DESCRIPTION:
 ** Copy one or many scenes
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** tsZCL_ClusterInstance       *psClusterInstance
 ** uint16                      u16FromGroupId
 ** uint16                      u16ToGroupId
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCopyAllScenes(
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16FromGroupId,
                            uint16                      u16ToGroupId)
{
    teZCL_Status eStatus;
    tsCLD_ScenesTableEntry *psTo;
    tsCLD_ScenesTableEntry *psFrom;

    tsCLD_ScenesMatch sSearchParameter;

    tsCLD_ScenesCustomDataStructure *psCommon;

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psEndPointDefinition==NULL) || (psClusterInstance==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Check that the u16FromGroupId exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16FromGroupId);
    /* Group 0 doesn't really exist, its a special case for the Global Scene */
    if((eStatus != E_ZCL_SUCCESS) && (u16FromGroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_CLD_GROUPS_NOT_FOUND;
    }
    
    /* Check that the u16ToGroupId exists in the group table */
    eStatus = eCLD_GroupsCheckGroupExists(psEndPointDefinition, u16ToGroupId);
    /* Group 0 doesn't really exist, its a special case for the Global Scene */
    if((eStatus != E_ZCL_SUCCESS) && (u16ToGroupId != 0))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_CLD_GROUPS_NOT_FOUND;
    }

    DBG_vPrintf(TRACE_SCENES, "\nCopying all scenes from group %04x to Group%d", u16FromGroupId, u16ToGroupId);

    /* Search table for a matching "From" entry */
    sSearchParameter.u8SearchOptions = CLD_SCENES_SEARCH_GROUP_ID;
    sSearchParameter.u16GroupId = u16FromGroupId;
    psFrom = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);

    while(psFrom != NULL)
    {

        DBG_vPrintf(TRACE_SCENES, "\nFound matching scene entry in table (G:%04x:S%02x)", u16FromGroupId, psFrom->u8SceneId);

        /* Search table for a matching "To" entry */
        sSearchParameter.u8SearchOptions = (CLD_SCENES_SEARCH_GROUP_ID|CLD_SCENES_SEARCH_SCENE_ID);
        sSearchParameter.u16GroupId = u16ToGroupId;
        sSearchParameter.u8SceneId = psFrom->u8SceneId;

        psTo = (tsCLD_ScenesTableEntry*)psDLISTsearchFromHead(&psCommon->lScenesAllocList, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);
        if (psTo == NULL)
        {
            psTo = (tsCLD_ScenesTableEntry*)psDLISTremoveFromHead(&psCommon->lScenesDeAllocList);
            if(psTo == NULL)
            {
                DBG_vPrintf(TRACE_SCENES, "\nNo free table entries");
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return E_CLD_SCENES_NO_RESOURCES;
            }
            else
            {
                /* Add to allocated list */
                vDLISTaddToTail(&psCommon->lScenesAllocList, (DNODE*)psTo);
            }
        }
        else
        {
            DBG_vPrintf(TRACE_SCENES, "\nTo already exists, overwriting it");
        }

        /* Copy scene table entry */
        eCLD_ScenesCopySceneTableEntry(psTo, psFrom, u16ToGroupId, psFrom->u8SceneId);

        /* Search table for next matching "From" entry */
        psFrom = (tsCLD_ScenesTableEntry*)psDLISTgetNext((DNODE*)psFrom);
        sSearchParameter.u8SearchOptions = CLD_SCENES_SEARCH_GROUP_ID;
        sSearchParameter.u16GroupId = u16FromGroupId;
        psFrom = (tsCLD_ScenesTableEntry*)psDLISTsearchForward((DNODE*)psFrom, bCLD_ScenesSearchForScene, (void*)&sSearchParameter);

    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}

#endif

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       bCLD_ScenesSearchForScene
 **
 ** DESCRIPTION:
 ** Searches for a scene entry
 **
 ** PARAMETERS:             Name                           Usage
 ** void                    *pvSearchParam
 ** DNODE                   *psNodeUnderTest
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  bool_t bCLD_ScenesSearchForScene(void *pvSearchParam, void *psNodeUnderTest)
{

    tsCLD_ScenesMatch *psSearchParameter = (tsCLD_ScenesMatch*)pvSearchParam;
    tsCLD_ScenesTableEntry *psSearchEntry = (tsCLD_ScenesTableEntry*)psNodeUnderTest;

    DBG_vPrintf(TRACE_SCENES, "\nSearch: G%04x S%02x:G%04x S%02x", psSearchParameter->u16GroupId,
                                                                     psSearchParameter->u8SceneId,
                                                                     psSearchEntry->u16GroupId,
                                                                     psSearchEntry->u8SceneId);

    if((psSearchParameter->u8SearchOptions & CLD_SCENES_SEARCH_GROUP_ID) && (psSearchParameter->u16GroupId != psSearchEntry->u16GroupId))
    {
        return FALSE;
    }

    if((psSearchParameter->u8SearchOptions & CLD_SCENES_SEARCH_SCENE_ID) && (psSearchParameter->u8SceneId != psSearchEntry->u8SceneId))
    {
    return FALSE;
    }

    return TRUE;

}


#ifdef  CLD_SCENES_CMD_COPY_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCopySceneTableEntry
 **
 ** DESCRIPTION:
 ** Copy a scene table entry
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsCLD_ScenesTableEntry      *psTo
 ** tsCLD_ScenesTableEntry      *psFrom
 ** uint16                      u16ToGroupId
 ** uint8                       u8ToSceneId
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesCopySceneTableEntry(
                            tsCLD_ScenesTableEntry      *psTo,
                            tsCLD_ScenesTableEntry      *psFrom,
                            uint16                      u16ToGroupId,
                            uint8                       u8ToSceneId)
{

    /* Do parameter check */
    if(psTo == NULL || psFrom == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* Copy scene table entry */
    psTo->bActive               = FALSE;
    psTo->bInTransit            = FALSE;
    psTo->u16GroupId            = u16ToGroupId;
    psTo->u8SceneId             = u8ToSceneId;
    psTo->u16TransitionTime     = psFrom->u16TransitionTime;
    psTo->u32TransitionTimer    = 0;
    psTo->u8SceneNameLength     = psFrom->u8SceneNameLength;
    memcpy(psTo->au8SceneName, psFrom->au8SceneName, CLD_SCENES_MAX_SCENE_NAME_LENGTH + 1);
    psTo->u16SceneDataLength    = psFrom->u16SceneDataLength;
    memcpy(psTo->au8SceneData, psFrom->au8SceneData, CLD_SCENES_MAX_SCENE_STORAGE_BYTES);
    #ifdef CLD_SCENES_TABLE_SUPPORT_TRANSITION_TIME_IN_MS
        psTo->u8TransitionTime100ms = psFrom->u8TransitionTime100ms;
    #endif
    return E_ZCL_SUCCESS;

}
#endif

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
