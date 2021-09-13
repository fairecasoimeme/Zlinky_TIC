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
 * COMPONENT:          ScenesServerCommands.c
 *
 * DESCRIPTION:        Send a Scenes cluster command
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Scenes.h"
#include "Scenes_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"



#include "dbg.h"

#ifdef DEBUG_CLD_SCENES
#define TRACE_SCENES    TRUE
#else
#define TRACE_SCENES    FALSE
#endif

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
#ifdef SCENES_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandAddSceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends an add scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesAddSceneResponsePayload *psPayload             Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandAddSceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesAddSceneResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8SceneId},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_ADD,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandViewSceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a view scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesViewSceneResponsePayload *psPayload            Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandViewSceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesViewSceneResponsePayload *psPayload)
{

    uint8 u8ItemsInPayload;

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8SceneId},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16TransitionTime},
            {1,                                     E_ZCL_CSTRING,  &psPayload->sSceneName},
            {psPayload->sExtensionField.u16Length,  E_ZCL_UINT8,    psPayload->sExtensionField.pu8Data},
                                              };

    /* We only respond with all fields if the result was success */
    if(psPayload->eStatus != E_CLD_SCENES_CMD_STATUS_SUCCESS)
    {
        u8ItemsInPayload = 3;
    }
    else
    {
        u8ItemsInPayload = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_VIEW,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8ItemsInPayload
                                 );

}

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandRemoveSceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a remove scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesRemoveSceneResponsePayload *psPayload             Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandRemoveSceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesRemoveSceneResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8SceneId},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_REMOVE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandRemoveAllScenesResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a remove all scenes response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesRemoveAllScenesResponsePayload *psPayload             Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandRemoveAllScenesResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesRemoveAllScenesResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16GroupId},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_REMOVE_ALL,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandStoreSceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Store scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesStoreSceneResponsePayload *psPayload             Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandStoreSceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesStoreSceneResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8SceneId},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_STORE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandGetSceneMembershipResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Scene Membership response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesGetSceneMembershipResponsePayload *psPayload             Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandGetSceneMembershipResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesGetSceneMembershipResponsePayload *psPayload)
{

    uint8 u8ItemsInPayload;

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,                                     E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8Capacity},
            {1,                                     E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,                                     E_ZCL_UINT8,    &psPayload->u8SceneCount},
            {psPayload->u8SceneCount,               E_ZCL_UINT8,    psPayload->pu8SceneList}
                                              };

    /* We only respond with all fields if the result was success */
    if(psPayload->eStatus != E_CLD_SCENES_CMD_STATUS_SUCCESS)
    {
        u8ItemsInPayload = 3;
    }
    else
    {
        u8ItemsInPayload = sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem);
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_GET_SCENE_MEMBERSHIP,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  u8ItemsInPayload
                                 );

}

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandEnhancedAddSceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enhanced Add Scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesEnhancedAddSceneResponsePayload *psPayload     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandEnhancedAddSceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesEnhancedAddSceneResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,      E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,      E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,      E_ZCL_UINT8,    &psPayload->u8SceneId},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_ENHANCED_ADD_SCENE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );
}
#endif

#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandEnhancedViewSceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends an Enhanced View Scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesEnhancedViewSceneResponsePayload *psPayload    Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandEnhancedViewSceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesEnhancedViewSceneResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,      E_ZCL_ENUM8,    &psPayload->eStatus},
            {1,      E_ZCL_UINT16,   &psPayload->u16GroupId},
            {1,      E_ZCL_UINT8,    &psPayload->u8SceneId},
            {1,      E_ZCL_UINT16,   &psPayload->u16TransitionTime},
            {1,      E_ZCL_CSTRING,  &psPayload->sSceneName},
            {psPayload->sExtensionField.u16Length,  E_ZCL_UINT8,    psPayload->sExtensionField.pu8Data},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_ENHANCED_VIEW_SCENE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}
#endif

#ifdef CLD_SCENES_CMD_COPY_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesCommandCopySceneResponseSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Copy Scene response cluster command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                       u8SourceEndPointId             Source EP Id
 ** uint8                       u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address              *psDestinationAddress           Destination Address
 ** uint8                      *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsCLD_ScenesCopySceneResponsePayload *psPayload            Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandCopySceneResponseSend(
                                        uint8           u8SourceEndPointId,
                                        uint8           u8DestinationEndPointId,
                                        tsZCL_Address   *psDestinationAddress,
                                        uint8           *pu8TransactionSequenceNumber,
                                        tsCLD_ScenesCopySceneResponsePayload *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
            {1,      E_ZCL_UINT8,    &psPayload->u8Status},
            {1,      E_ZCL_UINT16,   &psPayload->u16FromGroupId},
            {1,      E_ZCL_UINT8,    &psPayload->u8FromSceneId},
                                              };

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_SCENES,
                                  TRUE,
                                  E_CLD_SCENES_CMD_COPY_SCENE,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );
}
#endif
#endif
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
