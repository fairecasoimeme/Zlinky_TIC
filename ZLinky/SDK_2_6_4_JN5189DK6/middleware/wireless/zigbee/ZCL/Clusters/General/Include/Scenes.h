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
 * COMPONENT:          Scenes.h
 *
 * DESCRIPTION:        Header for Scenes Cluster
 *
 *****************************************************************************/

#ifndef SCENES_H
#define SCENES_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_SCENES                 0x0005

/****************************************************************************/
/*             Scenes Cluster - Optional Attributes and configuration       */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the scenes cluster.                                        */
/****************************************************************************/

/* Enable the optional Last Configured By attribute */
//#define   CLD_SCENES_ATTR_LAST_CONFIGURED_BY


/* 3.7.2.3.1 */
/* Configure the maximum length of the Scene Name storage */
#ifndef CLD_SCENES_MAX_SCENE_NAME_LENGTH
#define CLD_SCENES_MAX_SCENE_NAME_LENGTH            (16)
#endif

/* 3.7.2.3.2 */
/* Configure the maximum number of scenes */
#ifndef CLD_SCENES_MAX_NUMBER_OF_SCENES
#define CLD_SCENES_MAX_NUMBER_OF_SCENES             (16)
#endif

/* Configure the maximum number of bytes available for scene storage */
#ifndef CLD_SCENES_MAX_SCENE_STORAGE_BYTES
#define CLD_SCENES_MAX_SCENE_STORAGE_BYTES          (24)
#endif

/* Name Support */
#ifndef CLD_SCENES_DISABLE_NAME_SUPPORT
#define CLD_SCENES_NAME_SUPPORT                     (1 << 7)
#else
#define CLD_SCENES_NAME_SUPPORT                     (0)
#endif

#ifndef CLD_SCENES_CLUSTER_REVISION
    #define CLD_SCENES_CLUSTER_REVISION              3
#endif 

#ifndef CLD_SCENES_FEATURE_MAP
    #define CLD_SCENES_FEATURE_MAP                   0
#endif 

/* Bit definitions in Copy Scene Mode field */
#define CLD_SCENES_COPY_ALL_SCENES                  (1 << 0)

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    extern PUBLIC void vSaveScenesNVM(void);
#endif    
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Return values - Extensions to teZCL_Status */
typedef enum 
{
    E_CLD_SCENES_BAD_ADDRESS_MODE    = 0x80,
    E_CLD_SCENES_EXTENSION_TOO_BIG,
    E_CLD_SCENES_DUPLICATE_EXISTS,
    E_CLD_SCENES_NOT_FOUND,
    E_CLD_SCENES_NO_RESOURCES,
    E_CLD_GROUPS_NOT_FOUND
} teCLD_ScenesStatus;


/* Command codes */
typedef enum 
{
    E_CLD_SCENES_CMD_ADD                 = 0x00,            /* Mandatory */
    E_CLD_SCENES_CMD_VIEW,                                  /* Mandatory */
    E_CLD_SCENES_CMD_REMOVE,                                /* Mandatory */
    E_CLD_SCENES_CMD_REMOVE_ALL,                            /* Mandatory */
    E_CLD_SCENES_CMD_STORE,                                 /* Mandatory */
    E_CLD_SCENES_CMD_RECALL,                                /* Mandatory */
    E_CLD_SCENES_CMD_GET_SCENE_MEMBERSHIP,                  /* Mandatory */
    E_CLD_SCENES_CMD_ENHANCED_ADD_SCENE = 0x40,             /* Optional  */
    E_CLD_SCENES_CMD_ENHANCED_VIEW_SCENE,                   /* Optional  */
    E_CLD_SCENES_CMD_COPY_SCENE,                            /* Optional  */
} teCLD_Scenes_Command;


/* Attribute ID's */
typedef enum 
{
    /* Scenes attribute set attribute ID's (3.7.2.2) */
    E_CLD_SCENES_ATTR_ID_SCENE_COUNT            = 0x0000,   /* Mandatory */
    E_CLD_SCENES_ATTR_ID_CURRENT_SCENE,                     /* Mandatory */
    E_CLD_SCENES_ATTR_ID_CURRENT_GROUP,                     /* Mandatory */
    E_CLD_SCENES_ATTR_ID_SCENE_VALID,                       /* Mandatory */
    E_CLD_SCENES_ATTR_ID_NAME_SUPPORT,                      /* Mandatory */
    E_CLD_SCENES_ATTR_ID_LAST_CONFIGURED_BY,                /* Optional  */
} teCLD_Scenes_ClusterID;


/* Command status values */
typedef enum 
{
    E_CLD_SCENES_CMD_STATUS_SUCCESS             = 0x00,
    E_CLD_SCENES_CMD_STATUS_FAILURE,
    E_CLD_SCENES_CMD_STATUS_INVALID_FIELD       = 0x85,
    E_CLD_SCENES_CMD_STATUS_INSUFFICIENT_SPACE  = 0x89,
    E_CLD_SCENES_CMD_STATUS_DUPLICATE_EXISTS,
    E_CLD_SCENES_CMD_STATUS_NOT_FOUND
} teCLD_Scenes_ResponseStatus;


/* Scenes Cluster */
typedef struct
{
#ifdef SCENES_SERVER    
    zuint8                  u8SceneCount;                   /* Mandatory */
    zuint8                  u8CurrentScene;                 /* Mandatory */
    zuint16                 u16CurrentGroup;                /* Mandatory */
    zbool                   bSceneValid;                    /* Mandatory */
    zuint8                  u8NameSupport;                  /* Mandatory */

#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
    zieeeaddress            u64LastConfiguredBy;            /* Optional  */
#endif
#endif
    zbmap32                     u32FeatureMap;

    zuint16                 u16ClusterRevision;
} tsCLD_Scenes;

/* Extension field storage */
typedef struct
{
    uint16                      u16Length;
    uint16                      u16MaxLength;
    uint8                       *pu8Data;
} tsCLD_ScenesExtensionField;


/* Add scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
    uint16                      u16TransitionTime;
    tsZCL_CharacterString       sSceneName;
    tsCLD_ScenesExtensionField  sExtensionField;
} tsCLD_ScenesAddSceneRequestPayload;


/* Add scene response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesAddSceneResponsePayload;


/* View scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesViewSceneRequestPayload;


/* View scene response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
    uint16                      u16TransitionTime;
    tsZCL_CharacterString       sSceneName;
    tsCLD_ScenesExtensionField  sExtensionField;
} tsCLD_ScenesViewSceneResponsePayload;


/* Remove scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesRemoveSceneRequestPayload;


/* Remove scene response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesRemoveSceneResponsePayload;


/* Remove all scenes request command - payload */
typedef struct
{
    uint16                      u16GroupId;
} tsCLD_ScenesRemoveAllScenesRequestPayload;


/* Remove all scenes response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
} tsCLD_ScenesRemoveAllScenesResponsePayload;


/* Store scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesStoreSceneRequestPayload;


/* Store scene response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesStoreSceneResponsePayload;


/* Recall scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
    uint16                      u16TransitionTime;
} tsCLD_ScenesRecallSceneRequestPayload;


/* Get scene membership request command - payload */
typedef struct
{
    uint16                      u16GroupId;
} tsCLD_ScenesGetSceneMembershipRequestPayload;


/* Get scene membership response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint8                       u8Capacity;
    uint16                      u16GroupId;
    uint8                       u8SceneCount;
    uint8                       *pu8SceneList;
} tsCLD_ScenesGetSceneMembershipResponsePayload;

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
/* Add scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
    uint16                      u16TransitionTime100ms;
    tsZCL_CharacterString       sSceneName;
    tsCLD_ScenesExtensionField  sExtensionField;
} tsCLD_ScenesEnhancedAddSceneRequestPayload;

/* Add scene response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesEnhancedAddSceneResponsePayload;
#endif

#ifdef  CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
/* View scene request command - payload */
typedef struct
{
    uint16                      u16GroupId;
    uint8                       u8SceneId;
} tsCLD_ScenesEnhancedViewSceneRequestPayload;

/* View scene response command - payload */
typedef struct
{
    zenum8                      eStatus;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
    uint16                      u16TransitionTime;
    tsZCL_CharacterString       sSceneName;
    tsCLD_ScenesExtensionField  sExtensionField;
} tsCLD_ScenesEnhancedViewSceneResponsePayload;
#endif

#ifdef  CLD_SCENES_CMD_COPY_SCENE
/* Copy scene request command - payload */
typedef struct
{
    uint8       u8Mode;
    uint16      u16FromGroupId;
    uint8       u8FromSceneId;
    uint16      u16ToGroupId;
    uint8       u8ToSceneId;
} tsCLD_ScenesCopySceneRequestPayload;

typedef struct
{
    uint8       u8Status;
    uint16      u16FromGroupId;
    uint8       u8FromSceneId;
} tsCLD_ScenesCopySceneResponsePayload;
#endif

/* Scene table entry */
typedef struct
{
    DNODE                       dllScenesNode;
    bool_t                        bActive;
    bool_t                        bInTransit;
    uint16                      u16GroupId;
    uint8                       u8SceneId;
    uint16                      u16TransitionTime;
    uint32                      u32TransitionTimer;
    uint8                       u8SceneNameLength;
    uint8                       au8SceneName[CLD_SCENES_MAX_SCENE_NAME_LENGTH + 1];
    uint16                      u16SceneDataLength;
    uint8                       au8SceneData[CLD_SCENES_MAX_SCENE_STORAGE_BYTES];
    #ifdef  CLD_SCENES_TABLE_SUPPORT_TRANSITION_TIME_IN_MS
        uint8                       u8TransitionTime100ms;
    #endif
} tsCLD_ScenesTableEntry;


/* Definition of Call back Event Structure */
typedef struct
{
    uint8                                               u8CommandId;
    union
    {
        tsCLD_ScenesAddSceneRequestPayload              *psAddSceneRequestPayload;
        tsCLD_ScenesAddSceneResponsePayload             *psAddSceneResponsePayload;

        tsCLD_ScenesViewSceneRequestPayload             *psViewSceneRequestPayload;
        tsCLD_ScenesViewSceneResponsePayload            *psViewSceneResponsePayload;

        tsCLD_ScenesRemoveSceneRequestPayload           *psRemoveSceneRequestPayload;
        tsCLD_ScenesRemoveSceneResponsePayload          *psRemoveSceneResponsePayload;

        tsCLD_ScenesRemoveAllScenesRequestPayload       *psRemoveAllScenesRequestPayload;
        tsCLD_ScenesRemoveAllScenesResponsePayload      *psRemoveAllScenesResponsePayload;

        tsCLD_ScenesStoreSceneRequestPayload            *psStoreSceneRequestPayload;
        tsCLD_ScenesStoreSceneResponsePayload           *psStoreSceneResponsePayload;

        tsCLD_ScenesRecallSceneRequestPayload           *psRecallSceneRequestPayload;

        tsCLD_ScenesGetSceneMembershipRequestPayload    *psGetSceneMembershipRequestPayload;
        tsCLD_ScenesGetSceneMembershipResponsePayload   *psGetSceneMembershipResponsePayload;

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
        tsCLD_ScenesEnhancedAddSceneRequestPayload      *psEnhancedAddSceneRequestPayload;
        tsCLD_ScenesEnhancedAddSceneResponsePayload     *psEnhancedAddSceneResponsePayload;
#endif
#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
        tsCLD_ScenesEnhancedViewSceneRequestPayload     *psEnhancedViewSceneRequestPayload;
        tsCLD_ScenesEnhancedViewSceneResponsePayload    *psEnhancedViewSceneResponsePayload;
#endif
#ifdef CLD_SCENES_CMD_COPY_SCENE        
        tsCLD_ScenesCopySceneRequestPayload             *psCopySceneRequestPayload;
        tsCLD_ScenesCopySceneResponsePayload            *psCopySceneResponsePayload;
#endif
    } uMessage;
} tsCLD_ScenesCallBackMessage;


/* Custom data structure */
typedef struct
{
    DLIST   lScenesAllocList;
    DLIST   lScenesDeAllocList;

    tsZCL_ReceiveEventAddress       sReceiveEventAddress;
    tsZCL_CallBackEvent             sCustomCallBackEvent;
    tsCLD_ScenesCallBackMessage     sCallBackMessage;
#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    tsCLD_ScenesTableEntry          asScenesTableEntry[CLD_SCENES_MAX_NUMBER_OF_SCENES];
#endif
} tsCLD_ScenesCustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ScenesCreateScenes(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsCLD_ScenesCustomDataStructure    *psCustomDataStructure,
                tsZCL_EndPointDefinition           *psEndPointDefinition);

#ifdef SCENES_SERVER 
PUBLIC  void vCLD_ScenesUpdateSceneValid(
                            tsZCL_EndPointDefinition    *psEndPointDefinition);
PUBLIC teZCL_Status eCLD_ScenesUpdate(
                uint8           u8SourceEndPointId);

PUBLIC teZCL_Status eCLD_ScenesAdd(
                uint8                       u8SourceEndPointId,
                uint16                      u16GroupId,
                uint8                       u8SceneId);

PUBLIC teZCL_Status eCLD_ScenesStore(
                uint8                       u8SourceEndPointId,
                uint16                      u16GroupId,
                uint8                       u8SceneId);

PUBLIC teZCL_Status eCLD_ScenesRecall(
                uint8                       u8SourceEndPointId,
                uint16                      u16GroupId,
                uint8                       u8SceneId);
#endif
#ifdef SCENES_CLIENT
PUBLIC teZCL_Status eCLD_ScenesCommandAddSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesAddSceneRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandViewSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesViewSceneRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandRemoveSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesRemoveSceneRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandRemoveAllScenesRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesRemoveAllScenesRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandStoreSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesStoreSceneRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandRecallSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesRecallSceneRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesCommandGetSceneMembershipRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesGetSceneMembershipRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_ScenesRemoveAllScenes(
                    uint8                        u8SourceEndPointId,
                    uint16                       u16GroupId,
                    uint64                       u64LastConfiguredBy);


#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
PUBLIC teZCL_Status eCLD_ScenesCommandEnhancedAddSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesEnhancedAddSceneRequestPayload *psPayload);
#endif
#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
PUBLIC teZCL_Status eCLD_ScenesCommandEnhancedViewSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesEnhancedViewSceneRequestPayload *psPayload);
#endif
#ifdef CLD_SCENES_CMD_COPY_SCENE
PUBLIC teZCL_Status eCLD_ScenesCommandCopySceneSceneRequestSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
                tsCLD_ScenesCopySceneRequestPayload *psPayload);
#endif
#endif
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_Scenes;
extern tsZCL_AttributeDefinition asCLD_ScenesClusterAttributeDefinitions[];
extern uint8 au8ScenesAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* SCENES_H */
