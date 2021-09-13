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
 * COMPONENT:          Scenes_internal.h
 *
 * DESCRIPTION:        The internal API for the Scenes Cluster
 *
 *****************************************************************************/

#ifndef  SCENES_INTERNAL_H_INCLUDED
#define  SCENES_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "Scenes.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_ScenesCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#ifdef SCENES_SERVER
PUBLIC void vCLD_ScenesGenerateIndividualAttributeCallback(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16 u16AttributeId);
                    
PUBLIC teZCL_Status eCLD_ScenesCommandAddSceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesAddSceneResponsePayload *psPayload);
                    
PUBLIC teZCL_Status eCLD_ScenesCommandViewSceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesViewSceneResponsePayload *psPayload);
                    
PUBLIC teZCL_Status eCLD_ScenesCommandRemoveSceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesRemoveSceneResponsePayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandRemoveAllScenesResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesRemoveAllScenesResponsePayload *psPayload);


PUBLIC teZCL_Status eCLD_ScenesCommandStoreSceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesStoreSceneResponsePayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandGetSceneMembershipResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesGetSceneMembershipResponsePayload *psPayload);

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE                    
PUBLIC teZCL_Status eCLD_ScenesCommandEnhancedAddSceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesEnhancedAddSceneResponsePayload *psPayload);
#endif

#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
PUBLIC teZCL_Status eCLD_ScenesCommandEnhancedViewSceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesEnhancedViewSceneResponsePayload *psPayload);
#endif

#ifdef CLD_SCENES_CMD_COPY_SCENE
PUBLIC teZCL_Status eCLD_ScenesCommandCopySceneResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_ScenesCopySceneResponsePayload *psPayload);
#endif

PUBLIC teZCL_Status eCLD_ScenesAddScene(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsCLD_ScenesAddSceneRequestPayload *psPayload,
                    bool_t                      bEnhanced);

PUBLIC teZCL_Status eCLD_ScenesStoreScene(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                      u16GroupId,
                    uint8                       u8SceneId,
                    tsCLD_ScenesTableEntry      **ppsTableEntry);

PUBLIC teZCL_Status eCLD_ScenesFindScene(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                      u16GroupId,
                    uint8                       u8SceneId,
                    tsCLD_ScenesTableEntry      **ppsTableEntry);

PUBLIC teZCL_Status eCLD_ScenesFindActiveScene(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsCLD_ScenesTableEntry      **ppsTableEntry);

PUBLIC teZCL_Status eCLD_ScenesListScenes(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                      u16GroupId,
                    uint8                       *pu8NumberOfScenes,
                    uint8                       *pau8ListOfScenes);

PUBLIC teZCL_Status eCLD_ScenesCountScenes(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       *pu8NumberOfScenes);

PUBLIC teZCL_Status eCLD_ScenesRemoveScene(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    tsCLD_ScenesRemoveSceneRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesRemoveAllScenes(
                    uint8                        u8SourceEndPointId,
                    uint16                       u16GroupId,
                    uint64                       u64LastConfiguredBy);

#ifdef CLD_SCENES_CMD_COPY_SCENE
PUBLIC teZCL_Status eCLD_ScenesCopyScene(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                      u16FromGroupId,
                    uint8                       u8FromSceneId,
                    uint16                      u16ToGroupId,
                    uint8                       u8ToSceneId);

PUBLIC teZCL_Status eCLD_ScenesCopyAllScenes(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                      u16FromGroupId,
                    uint16                      u16ToGroupId);
#endif

PUBLIC teZCL_Status eCLD_ScenesSaveClusters(
                    tsZCL_EndPointDefinition *psEndPointDefinition,
                    uint16 u16MaxLength,
                    uint16 *pu16Length,
                    uint8 *pu8Buffer,
                    bool_t bIsTempory );

PUBLIC teZCL_Status eCLD_ScenesRestoreClusters(
                    tsZCL_EndPointDefinition *psEndPointDefinition,
                    uint16 u16Length,
                    uint8 *pu8Buffer,
                    uint32 u32RemainingTime);
#endif /* SCENES_SERVER */

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* SCENES_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
