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


/****************************************************************************
 *
 * MODULE:             Scenes Cluster
 *
 * DESCRIPTION:
 * Message event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zps_apl_af.h"
#include "zps_nwk_pub.h"

#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"

#include "Scenes.h"
#include "Scenes_internal.h"

#include "OnOff_internal.h"

#ifdef CLD_LEVEL_CONTROL
#include "LevelControl.h"
#endif

#ifdef CLD_COLOUR_CONTROL
#include "ColourControl.h"
#endif

#include "zcl_options.h"



#include "dbg.h"

#ifdef DEBUG_CLD_SCENES
#define TRACE_SCENES    TRUE
#else
#define TRACE_SCENES    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_SCENES) && !defined(SCENES_SERVER) && !defined(SCENES_CLIENT)
#error You Must Have either SCENES_SERVER and/or SCENES_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum pack
{
    PARSE_CLUSTER_ID,
    PARSER_SKIP,
    PARSER_COPY_TO_DESTINATION
} teZCL_ParseState;
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef SCENES_SERVER
PRIVATE teZCL_Status eCLD_ScenesHandleAddSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);
                            
PRIVATE teZCL_Status eCLD_ScenesHandleViewSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ScenesHandleRemoveSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ScenesHandleRemoveAllScenesRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ScenesHandleStoreSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ScenesHandleRecallSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ScenesHandleGetSceneMembershipRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
PRIVATE teZCL_Status eCLD_ScenesHandleEnhancedAddSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);
#endif

#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
PRIVATE teZCL_Status eCLD_ScenesHandleEnhancedViewSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);
#endif

#ifdef CLD_SCENES_CMD_COPY_SCENE
PRIVATE teZCL_Status eCLD_ScenesHandleCopySceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon);
#endif
#endif

#ifdef SCENES_CLIENT
PRIVATE teZCL_Status eCLD_ScenesHandleAddSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_ScenesHandleViewSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_ScenesHandleRemoveSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_ScenesHandleRemoveAllScenesResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_ScenesHandleStoreSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_ScenesHandleGetSceneMembershipResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
PRIVATE teZCL_Status eCLD_ScenesHandleEnhancedAddSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);
#endif

#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
PRIVATE teZCL_Status eCLD_ScenesHandleEnhancedViewSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);
#endif

#ifdef CLD_SCENES_CMD_COPY_SCENE
PRIVATE teZCL_Status eCLD_ScenesHandleCopySceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition);
#endif
#endif

#ifdef SCENES_SERVER 
PRIVATE teZCL_Status eZCL_CustomCommandReceiveScenes(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsZCL_RxPayloadItem         *psPayloadDefinition,
                    uint16                       u16InitialOffsetinPayload);

#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
PRIVATE teZCL_Status eCLD_ScenesSetLastConfiguredBy(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif
#if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER)
PRIVATE void vCLD_ScenesConfigureColourControlExtension(
                            tsCLD_ScenesExtensionField *psExtensionField);
#endif                          
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
 ** NAME:       eCLD_ScenesCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Message Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_ScenesCustomDataStructure *psCommon;

    tsZCL_Address sZCL_Address;
#if defined ( SCENES_SERVER)
    teZCL_Status eStatus = E_ZCL_SUCCESS;
#endif
#if defined( SCENES_SERVER) || defined (SCENES_CLIENT )
     uint16 u16ActualQuantity;
    /* The u16ActualQuantity and asPayloadDefinition can be declared local to each command handler.
     * This is declared as common to all handlers with default values to save memory*/
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
                {1,                                         &u16ActualQuantity,                     E_ZCL_ENUM8,    0},
                {1,                                         &u16ActualQuantity,                     E_ZCL_UINT16,   0},
                {1,                                         &u16ActualQuantity,                     E_ZCL_UINT8,    0},
                {1,                                         &u16ActualQuantity,                     E_ZCL_UINT16,   0},
                {1,                                         &u16ActualQuantity,                     E_ZCL_CSTRING,  0},
                {CLD_SCENES_MAX_SCENE_STORAGE_BYTES,        &u16ActualQuantity,                     E_ZCL_UINT8,    0},
    };
#endif
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Store return address details */
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psCommon->sReceiveEventAddress);

        // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer)
    {

#ifdef SCENES_SERVER


        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_SCENES_CMD_ADD):
            {
                eCLD_ScenesHandleAddSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance,&asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
            }

            case (E_CLD_SCENES_CMD_VIEW):
            {
                eCLD_ScenesHandleViewSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance,&asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
            }

            case (E_CLD_SCENES_CMD_REMOVE):
            {
                eCLD_ScenesHandleRemoveSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
            }

            case (E_CLD_SCENES_CMD_REMOVE_ALL):
            {
                eCLD_ScenesHandleRemoveAllScenesRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
            }

            case (E_CLD_SCENES_CMD_STORE):
            {
                eCLD_ScenesHandleStoreSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
            }

            case (E_CLD_SCENES_CMD_RECALL):
            {
                eStatus = eCLD_ScenesHandleRecallSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                /* E_ZCL_FAIL means malformed default response already sent */
                if((eStatus !=  E_ZCL_FAIL) && \
                    (((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse)) || \
                     (eStatus != E_ZCL_SUCCESS)))
                {
                    DBG_vPrintf(TRACE_SCENES, "End scenes Handler %02x\n", eStatus);
                    eZCL_SendDefaultResponse(pZPSevent, eStatus);
                }
                break;
            }

            case (E_CLD_SCENES_CMD_GET_SCENE_MEMBERSHIP):
            {
                eCLD_ScenesHandleGetSceneMembershipRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
            }

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
            case E_CLD_SCENES_CMD_ENHANCED_ADD_SCENE:
                eCLD_ScenesHandleEnhancedAddSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
#endif

#ifdef  CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
            case E_CLD_SCENES_CMD_ENHANCED_VIEW_SCENE:
                eCLD_ScenesHandleEnhancedViewSceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[1],&sZCL_Address,psCommon);
                break;
#endif

#ifdef  CLD_SCENES_CMD_COPY_SCENE
            case E_CLD_SCENES_CMD_COPY_SCENE:
                eCLD_ScenesHandleCopySceneRequest(pZPSevent, psEndPointDefinition, psClusterInstance,&asPayloadDefinition[0],&sZCL_Address,psCommon);
                break;
#endif

            default:
            {
                // unlock
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif
    }
    else
    {
#ifdef SCENES_CLIENT
        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_SCENES_CMD_ADD):
            {
                eCLD_ScenesHandleAddSceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
            }

            case (E_CLD_SCENES_CMD_VIEW):
            {
                eCLD_ScenesHandleViewSceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
            }

            case (E_CLD_SCENES_CMD_REMOVE):
            {
                eCLD_ScenesHandleRemoveSceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
            }

            case (E_CLD_SCENES_CMD_REMOVE_ALL):
            {
                eCLD_ScenesHandleRemoveAllScenesResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
            }

            case (E_CLD_SCENES_CMD_STORE):
            {
                eCLD_ScenesHandleStoreSceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
            }

            case (E_CLD_SCENES_CMD_GET_SCENE_MEMBERSHIP):
            {
                eCLD_ScenesHandleGetSceneMembershipResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
            }

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
            case E_CLD_SCENES_CMD_ENHANCED_ADD_SCENE:
                eCLD_ScenesHandleEnhancedAddSceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
#endif

#ifdef  CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
            case E_CLD_SCENES_CMD_ENHANCED_VIEW_SCENE:
                eCLD_ScenesHandleEnhancedViewSceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
#endif

#ifdef  CLD_SCENES_CMD_COPY_SCENE
            case E_CLD_SCENES_CMD_COPY_SCENE:
                eCLD_ScenesHandleCopySceneResponse(pZPSevent, psEndPointDefinition, psClusterInstance, &asPayloadDefinition[0]);
                break;
#endif

            default:
            {
                // unlock
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif
    }



    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}


/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef SCENES_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleAddSceneRequest
 **
 ** DESCRIPTION:
 ** Handles add scene request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleAddSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition, 
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesAddSceneRequestPayload sPayload;
    tsCLD_ScenesAddSceneResponsePayload sResponse;
    uint8 au8SceneName[CLD_SCENES_MAX_SCENE_NAME_LENGTH];
    uint8 au8ExtensionData[CLD_SCENES_MAX_SCENE_STORAGE_BYTES];
    uint8   u8TransactionSequenceNumber;

    DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandAddSceneRequestReceive: ");

    /* Point to storage for received string */
    sPayload.sSceneName.pu8Data = au8SceneName;
    sPayload.sSceneName.u8Length = 0;
    sPayload.sSceneName.u8MaxLength = sizeof(au8SceneName);

    /* Point to storage for scene extension data */
    sPayload.sExtensionField.pu8Data = au8ExtensionData;
    sPayload.sExtensionField.u16Length = 0;
    sPayload.sExtensionField.u16MaxLength = sizeof(au8ExtensionData);

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination = &sPayload.u8SceneId;
    pasPayloadDefinition[2].pvDestination=  &sPayload.u16TransitionTime;
    pasPayloadDefinition[3].pvDestination = &sPayload.sSceneName;
    pasPayloadDefinition[4].pvDestination = sPayload.sExtensionField.pu8Data;

    pasPayloadDefinition[4].u16MaxQuantity = sPayload.sExtensionField.u16MaxLength;
    pasPayloadDefinition[4].pu16ActualQuantity = &sPayload.sExtensionField.u16Length;


    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                     &u8TransactionSequenceNumber,
                                     pasPayloadDefinition,
                                     5,
                                     E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);
    if (eStatus == E_ZCL_SUCCESS)
    {

        tsZCL_RxPayloadItem sPayloadDefinitionPayLoad;
        sPayloadDefinitionPayLoad.u16MaxQuantity = sPayload.sExtensionField.u16MaxLength;
        sPayloadDefinitionPayLoad.eType = E_ZCL_UINT8;
        sPayloadDefinitionPayLoad.pu16ActualQuantity=&(sPayload.sExtensionField.u16Length);
        memset(sPayload.sExtensionField.pu8Data,0,sPayload.sExtensionField.u16MaxLength);
        sPayloadDefinitionPayLoad.pvDestination = sPayload.sExtensionField.pu8Data;
        eZCL_CustomCommandReceiveScenes(pZPSevent,
                                     psEndPointDefinition,
                                     &u8TransactionSequenceNumber,
                                     &sPayloadDefinitionPayLoad,
                                     /* GroupId (2) + Scene Id (1) + TransitionTime(2)*/
                                     5);
    }

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "Receive Error: %d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_SCENES, "NameLen=%d ExtLen=%d", sPayload.sSceneName.u8Length, sPayload.sExtensionField.u16Length);

    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER)
        vCLD_ScenesConfigureColourControlExtension(&sPayload.sExtensionField);
    #endif

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psAddSceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);


    /* Try and add the scene to the table */
    eStatus = eCLD_ScenesAddScene(psEndPointDefinition,
                                  psClusterInstance,
                                  &sPayload,
                                  FALSE);

    DBG_vPrintf(TRACE_SCENES, "AddScene: %d", eStatus);

    /* Construct response */
    switch(eStatus)
    {

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_ZCL_ERR_INSUFFICIENT_SPACE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INSUFFICIENT_SPACE;
        break;

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;

        /* Update attributes */
        eCLD_ScenesCountScenes(psEndPointDefinition, psClusterInstance, &((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount);
#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
        eCLD_ScenesSetLastConfiguredBy(pZPSevent, psEndPointDefinition, psClusterInstance);
        vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_LAST_CONFIGURED_BY);
#endif
        break;

    default:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_FAILURE;
        break;

    }

    vSaveScenesNVM();

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneId  = sPayload.u8SceneId;

    eStatus = eCLD_ScenesCommandAddSceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     psZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);

    DBG_vPrintf(TRACE_SCENES, " Send: %d", eStatus);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleViewSceneRequest
 **
 ** DESCRIPTION:
 ** Handles view scene request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleViewSceneRequest(
                            ZPS_tsAfEvent                   *pZPSevent,
                            tsZCL_EndPointDefinition        *psEndPointDefinition,
                            tsZCL_ClusterInstance           *psClusterInstance,
                            tsZCL_RxPayloadItem             *pasPayloadDefinition,
                            tsZCL_Address                   *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesViewSceneRequestPayload sPayload;
    tsCLD_ScenesViewSceneResponsePayload sResponse;
    uint8 au8NoSceneName[1] = {0};
    uint8 au8NoExtensionData[1] = {0};
    tsCLD_ScenesTableEntry *psTableEntry;
    uint8   u8TransactionSequenceNumber;
    /* Initialise variable length fields */
    sResponse.sSceneName.pu8Data = au8NoSceneName;
    sResponse.sSceneName.u8Length = 0;
    sResponse.sExtensionField.pu8Data = au8NoExtensionData;
    sResponse.sExtensionField.u16Length = 0;

    DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleViewSceneRequest ");

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =  &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination =  &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        2,
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psViewSceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Try and find requested scene */
    eStatus = eCLD_ScenesFindScene(psEndPointDefinition,
                                   psClusterInstance,
                                   sPayload.u16GroupId,
                                   sPayload.u8SceneId,
                                   &psTableEntry);

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    /* Construct response */
    switch(eStatus)
    {

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;
        sResponse.sSceneName.pu8Data = psTableEntry->au8SceneName;
        sResponse.sSceneName.u8Length = strlen((char*)psTableEntry->au8SceneName);
        sResponse.sSceneName.u8MaxLength = sizeof(psTableEntry->au8SceneName);
        sResponse.u16TransitionTime = psTableEntry->u16TransitionTime;
        sResponse.sExtensionField.pu8Data = psTableEntry->au8SceneData;
        sResponse.sExtensionField.u16Length = psTableEntry->u16SceneDataLength;
        sResponse.sExtensionField.u16MaxLength = sizeof(psTableEntry->au8SceneData);

        DBG_vPrintf(TRACE_SCENES, "NameLen=%d ExtLen=%d", sResponse.sSceneName.u8Length, sResponse.sExtensionField.u16Length);
        break;

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_ZCL_ERR_SCENE_NOT_FOUND:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_NOT_FOUND;
        break;

    default:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_FAILURE;
        break;

    }
    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneId  = sPayload.u8SceneId;

    eStatus = eCLD_ScenesCommandViewSceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     psZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);

    return eStatus;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleRemoveSceneRequest
 **
 ** DESCRIPTION:
 ** Handles remove scene request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleRemoveSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesRemoveSceneRequestPayload sPayload;
    tsCLD_ScenesRemoveSceneResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                         &u8TransactionSequenceNumber,
                                         pasPayloadDefinition,
                                         2,
                                         E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandRemoveSceneRequestReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRemoveSceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Try and remove the scene from the table */
    eStatus = eCLD_ScenesRemoveScene(psEndPointDefinition,
                                     psClusterInstance,
                                     &sPayload);

    /* Construct response */
    switch(eStatus)
    {

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;

        /* Update attributes */
        eCLD_ScenesCountScenes(psEndPointDefinition, psClusterInstance, &((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount);
#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
        eCLD_ScenesSetLastConfiguredBy(pZPSevent, psEndPointDefinition, psClusterInstance);
        vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_LAST_CONFIGURED_BY);
#endif
        break;

    case E_ZCL_FAIL:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_NOT_FOUND;
        break;

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    default:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_FAILURE;
        break;

    }

    vSaveScenesNVM();

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneId = sPayload.u8SceneId;

    eStatus = eCLD_ScenesCommandRemoveSceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                        pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                        psZCL_Address,
                                                        &u8TransactionSequenceNumber,
                                                        &sResponse);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleRemoveAllScenesRequest
 **
 ** DESCRIPTION:
 ** Handles remove all scenes request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleRemoveAllScenesRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesRemoveAllScenesRequestPayload sPayload;
    tsCLD_ScenesRemoveAllScenesResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.u16GroupId;
    
    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        1,
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandRemoveAllScenesRequestReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRemoveAllScenesRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Delete all scene entries for this group */
    eStatus = eCLD_ScenesRemoveAllScenes(psEndPointDefinition->u8EndPointNumber,
                                         sPayload.u16GroupId,
                                         (uint64)0xffffffffffffffffLL);

    /* Construct response */
    switch(eStatus)
    {

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;
        /* Update attributes */
        // ToDo:JV: Removeed below line as u8SceneCount is decremented inside eCLD_ScenesRemoveAllScenes
        //((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount = 0; 
#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
        eCLD_ScenesSetLastConfiguredBy(pZPSevent, psEndPointDefinition, psClusterInstance);
        vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_LAST_CONFIGURED_BY);
#endif
        break;

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    default:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_FAILURE;
        break;

    }

    vSaveScenesNVM();

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    sResponse.u16GroupId = sPayload.u16GroupId;

    eStatus = eCLD_ScenesCommandRemoveAllScenesResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                            pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                            psZCL_Address,
                                                            &u8TransactionSequenceNumber,
                                                            &sResponse);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleStoreSceneRequest
 **
 ** DESCRIPTION:
 ** Handles store scene request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleStoreSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesStoreSceneRequestPayload sPayload;
    tsCLD_ScenesStoreSceneResponsePayload sResponse;
    tsCLD_ScenesTableEntry *psTableEntry;
    uint8   u8TransactionSequenceNumber;
    tsZCL_CallBackEvent sZCL_CallBackEvent;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        2,
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);
                                        
                                        

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandStoreSceneRequestReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStoreSceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Scene not valid */
    ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->bSceneValid = FALSE;

    /* Try and get a table entry for the scene */
    eStatus = eCLD_ScenesStoreScene(psEndPointDefinition,
                                    psClusterInstance,
                                    sPayload.u16GroupId,
                                    sPayload.u8SceneId,
                                    &psTableEntry);


    switch(eStatus)
    {

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_ZCL_ERR_INSUFFICIENT_SPACE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INSUFFICIENT_SPACE;
        break;

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;

        /* Save all clusters on this end point */
        eStatus = eCLD_ScenesSaveClusters(psEndPointDefinition,
                                          sizeof(psTableEntry->au8SceneData),
                                          &psTableEntry->u16SceneDataLength,
                                          psTableEntry->au8SceneData,
                                          FALSE);
        if(eStatus != E_ZCL_SUCCESS)
        {
            DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesSaveClusters returned %d", eStatus);
            sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INSUFFICIENT_SPACE;
        }

#ifdef DEBUG_CLD_SCENES
        {
            int n;

            for(n = 0; n < psTableEntry->u16SceneDataLength; n++)
            {
                DBG_vPrintf(TRACE_SCENES, " %02x", psTableEntry->au8SceneData[n]);
            }
        }
#endif

        /* Update attributes */
        eCLD_ScenesCountScenes(psEndPointDefinition, psClusterInstance, &((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount);

        ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->bSceneValid      = TRUE;
        ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u16CurrentGroup  = sPayload.u16GroupId;
        ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8CurrentScene   = sPayload.u8SceneId;

#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
        eCLD_ScenesSetLastConfiguredBy(pZPSevent, psEndPointDefinition, psClusterInstance);
#endif


        /* Inform the application that the cluster has just been updated */
        sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
        sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
        sZCL_CallBackEvent.pZPSevent = NULL;
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_UPDATE;

        // inform user
        psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);



        break;

    default:
        break;

    }

    vSaveScenesNVM();

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    /* Construct response */
    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneId  = sPayload.u8SceneId;

    eStatus = eCLD_ScenesCommandStoreSceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                       pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                       psZCL_Address,
                                                       &u8TransactionSequenceNumber,
                                                       &sResponse);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleRecallSceneRequest
 **
 ** DESCRIPTION:
 ** Handles recall scene request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleRecallSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesRecallSceneRequestPayload sPayload;
    tsCLD_ScenesTableEntry *psTableEntry;
    tsCLD_ScenesTableEntry *psOldActiveScene;


    /* Assuming that transition field is not present or not defined */
    sPayload.u16TransitionTime = 0xFFFF;
    
    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u8SceneId;
    pasPayloadDefinition[2].pvDestination =   &sPayload.u16TransitionTime;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        3,
                                        E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);


    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandRecallSceneRequestReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRecallSceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);


    /* See if the scene exists */
    eStatus = eCLD_ScenesFindScene(psEndPointDefinition,
                                   psClusterInstance,
                                   sPayload.u16GroupId,
                                   sPayload.u8SceneId,
                                   &psTableEntry);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesFindScene returned %d", eStatus);
        if (eStatus == E_ZCL_ERR_INVALID_VALUE)
        {
            eStatus = E_ZCL_CMDS_INVALID_FIELD;
        }
        else if (eStatus == E_ZCL_ERR_SCENE_NOT_FOUND)
        {
            eStatus = E_ZCL_CMDS_NOT_FOUND;
        }
        return eStatus;
    }

#if (defined CLD_COLOUR_CONTROL && defined CLD_COLOURCONTROL_ATTR_COLOUR_MODE)
    /* Finding the Extension Field Set for Color Control Cluster and 
       updating u8ColourMode based on Extension Field Set */
    int i = 0;
    uint16 u16CurrentX = 0 , u16CurrentY = 0, u16ColourTemp = 0;
    while(i < psTableEntry->u16SceneDataLength)
    {
        if(((psTableEntry->au8SceneData[i+1] << 8) | psTableEntry->au8SceneData[i]) == LIGHTING_CLUSTER_ID_COLOUR_CONTROL)
        {
            u16CurrentX = ((uint16)(psTableEntry->au8SceneData[i+4] << 8) | psTableEntry->au8SceneData[i+3]);
            u16CurrentY = ((uint16)(psTableEntry->au8SceneData[i+6] << 8) | psTableEntry->au8SceneData[i+5]);
            u16ColourTemp = ((uint16)(psTableEntry->au8SceneData[i+15] << 8) | psTableEntry->au8SceneData[i+14]);
            /* When CurrentX and CurrentY fields have non Zero value, setting ColorMode to CurrentXY
               else if Current X OR current Y fields have non zero and Colour temperature fields have non 0xFFFF value setting ColourMode to ColourTemp
               else setting ColorMode to HSV */
            if(u16CurrentX > 0x00 && u16CurrentY > 0x00)
            {
                eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                                     E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y);
            }
            else if((psTableEntry->au8SceneData[i+2] >= 0x0D) &&
                    (u16ColourTemp != 0xFFFF))
            {
                eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                     E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE);
            }                    
            else
            {
                #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
                    eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                                             E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
                #else
                    eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);
                #endif
            }
            /* Coming out of the while loop */
            i = psTableEntry->u16SceneDataLength;
        }
        else
        {
            /* Jumping to the next Extension Field Set : i + 2 Bytes for the ClusterId field + 1 Byte for length field + Length of the Extension Field */
            i = i + 2 + 1 + psTableEntry->au8SceneData[i+2];
        }
    }
#endif //CLD_COLOUR_CONTROL && CLD_COLOURCONTROL_ATTR_COLOUR_MODE

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

    /* Before making new scene active, stopping the level control cluster's ongoing transitions */
    #ifdef LEVEL_CONTROL_SERVER
        eCLD_LevelControlStopTransition(psEndPointDefinition->u8EndPointNumber);
    #endif

    /* Make new scene active */
    /* Use the transition time from payload if defined else use scene table value */
    if(sPayload.u16TransitionTime == 0xFFFF)
    {
        psTableEntry->u32TransitionTimer = psTableEntry->u16TransitionTime * 10;
    }
    else
    {
        psTableEntry->u32TransitionTimer = sPayload.u16TransitionTime;
    }
    #ifdef CLD_SCENES_TABLE_SUPPORT_TRANSITION_TIME_IN_MS
        psTableEntry->u32TransitionTimer += psTableEntry->u8TransitionTime100ms;
    #endif

    psTableEntry->bActive = TRUE;
    psTableEntry->bInTransit = TRUE;

    /* Update attributes */
    ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->bSceneValid = TRUE;
    vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_SCENE_VALID);

    ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u16CurrentGroup = sPayload.u16GroupId;
    vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_CURRENT_GROUP);

    ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8CurrentScene = sPayload.u8SceneId;
    vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_CURRENT_SCENE);

#ifdef DEBUG_CLD_SCENES
    {
        int x;
        DBG_vPrintf(TRACE_SCENES, "\nActivating scene Group%04x Scene%02x Time%d Name=", psTableEntry->u16GroupId, psTableEntry->u8SceneId, psTableEntry->u16TransitionTime);
        for(x = 0; x < psTableEntry->u8SceneNameLength; x++)
        {
            DBG_vPrintf(TRACE_SCENES, "%c", psTableEntry->au8SceneName[x]);
        }

        for(x = 0; x < psTableEntry->u16SceneDataLength; x++)
        {
            DBG_vPrintf(TRACE_SCENES, " %02x", psTableEntry->au8SceneData[x]);
        }

    }
#endif

#if ((defined ONOFF_SERVER) && (defined CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL))
    /* Set Global Scene Control attribute to true */
    eCLD_OnOffSetGlobalSceneControl(psEndPointDefinition->u8EndPointNumber, TRUE);
#endif    

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleGetSceneMembershipRequest
 **
 ** DESCRIPTION:
 ** Handles get scene membership request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleGetSceneMembershipRequest(
                            ZPS_tsAfEvent                   *pZPSevent,
                            tsZCL_EndPointDefinition        *psEndPointDefinition,
                            tsZCL_ClusterInstance           *psClusterInstance,
                            tsZCL_RxPayloadItem             *pasPayloadDefinition,
                            tsZCL_Address                   *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{

    teZCL_Status eStatus;
    tsCLD_ScenesGetSceneMembershipRequestPayload sPayload;
    tsCLD_ScenesGetSceneMembershipResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    uint8 u8NumberOfScenes=0;
    uint8 au8ScenesList[CLD_SCENES_MAX_NUMBER_OF_SCENES];

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.u16GroupId;

    eStatus =eZCL_CustomCommandReceive(pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        1,
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandGetSceneMembershipRequestReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetSceneMembershipRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    eStatus = eCLD_ScenesListScenes(psEndPointDefinition,
                                    psClusterInstance,
                                    sPayload.u16GroupId,
                                    &u8NumberOfScenes,
                                    au8ScenesList);

    switch(eStatus)
    {

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;
        break;

    default:
        break;

    }

    /* Check if need to respond back ?
     * Special Case : Send response for group cast request if scene entry found for the group.
     */
    if(  !bZCL_SendCommandResponse( pZPSevent) && ( u8NumberOfScenes == 0 )  )
    {
        /* group cast or broadcast without any scene, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    /* Construct response */
    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneCount = u8NumberOfScenes;
    sResponse.pu8SceneList = au8ScenesList;

    eStatus = eCLD_ScenesCountScenes(psEndPointDefinition,
                                     psClusterInstance,
                                     &u8NumberOfScenes);

    sResponse.u8Capacity = CLD_SCENES_MAX_NUMBER_OF_SCENES - u8NumberOfScenes;

    eStatus = eCLD_ScenesCommandGetSceneMembershipResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                               pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                               psZCL_Address,
                                                               &u8TransactionSequenceNumber,
                                                               &sResponse);

    return eStatus;
}

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleEnhancedAddSceneRequest
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleEnhancedAddSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address               *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{
    teZCL_Status eStatus;
    tsCLD_ScenesEnhancedAddSceneRequestPayload sPayload;
    tsCLD_ScenesEnhancedAddSceneResponsePayload sResponse;
    uint8 au8SceneName[CLD_SCENES_MAX_SCENE_NAME_LENGTH];
    uint8 au8ExtensionData[CLD_SCENES_MAX_SCENE_STORAGE_BYTES];
    uint8   u8TransactionSequenceNumber;

    DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleEnhancedAddSceneRequest: ");

    /* Point to storage for received string */
    sPayload.sSceneName.pu8Data = au8SceneName;
    sPayload.sSceneName.u8Length = 0;
    sPayload.sSceneName.u8MaxLength = sizeof(au8SceneName);

    /* Point to storage for scene extension data */
    sPayload.sExtensionField.pu8Data = au8ExtensionData;
    sPayload.sExtensionField.u16Length = 0;
    sPayload.sExtensionField.u16MaxLength = sizeof(au8ExtensionData);

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination = &sPayload.u8SceneId;
    pasPayloadDefinition[2].pvDestination = &sPayload.u16TransitionTime100ms;
    pasPayloadDefinition[3].pvDestination = &sPayload.sSceneName;
    pasPayloadDefinition[4].pvDestination = sPayload.sExtensionField.pu8Data;

    pasPayloadDefinition[4].u16MaxQuantity = sPayload.sExtensionField.u16MaxLength;
    pasPayloadDefinition[4].pu16ActualQuantity = &sPayload.sExtensionField.u16Length;

    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                       &u8TransactionSequenceNumber,
                                       pasPayloadDefinition,
                                       5,
                                       E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);
    if (eStatus == E_ZCL_SUCCESS)
    {
        tsZCL_RxPayloadItem sPayloadDefinitionPayLoad;
        sPayloadDefinitionPayLoad.u16MaxQuantity = sPayload.sExtensionField.u16MaxLength;
        sPayloadDefinitionPayLoad.eType = E_ZCL_UINT8;
        sPayloadDefinitionPayLoad.pu16ActualQuantity=&(sPayload.sExtensionField.u16Length);
        memset(sPayload.sExtensionField.pu8Data,0,sPayload.sExtensionField.u16MaxLength);
        sPayloadDefinitionPayLoad.pvDestination = sPayload.sExtensionField.pu8Data;
        eZCL_CustomCommandReceiveScenes(pZPSevent,
                                        psEndPointDefinition,
                                        &u8TransactionSequenceNumber,
                                        &sPayloadDefinitionPayLoad,
                                        /* GroupId (2) + Scene Id (1) + TransitionTime(2) */
                                        5);
    }
    else
    {
        DBG_vPrintf(TRACE_SCENES, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedAddSceneRequestPayload = &sPayload;

    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER)
        vCLD_ScenesConfigureColourControlExtension(&sPayload.sExtensionField);
    #endif
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    DBG_vPrintf(TRACE_SCENES, "SNL%d SEL%d", sPayload.sSceneName.u8Length, sPayload.sExtensionField.u16Length);

    /* Try and add the scene to the table */
    eStatus = eCLD_ScenesAddScene(psEndPointDefinition,
                                  psClusterInstance,
                                  (tsCLD_ScenesAddSceneRequestPayload*)&sPayload,
                                  TRUE);

    /* Construct response */
    switch(eStatus)
    {

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_ZCL_ERR_INSUFFICIENT_SPACE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INSUFFICIENT_SPACE;
        break;

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;

        /* Update attributes */
        eCLD_ScenesCountScenes(psEndPointDefinition, psClusterInstance, &((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u8SceneCount);
#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
        eCLD_ScenesSetLastConfiguredBy(pZPSevent, psEndPointDefinition, psClusterInstance);
        vCLD_ScenesGenerateIndividualAttributeCallback(psEndPointDefinition, psClusterInstance, E_CLD_SCENES_ATTR_ID_LAST_CONFIGURED_BY);
#endif
        break;

    default:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_FAILURE;
        break;

    }

    vSaveScenesNVM();

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneId  = sPayload.u8SceneId;

    eStatus = eCLD_ScenesCommandEnhancedAddSceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     psZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);

    return eStatus;
}
#endif

#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleEnhancedViewSceneRequest
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleEnhancedViewSceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address               *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{
    teZCL_Status eStatus;
    tsCLD_ScenesEnhancedViewSceneRequestPayload sPayload;
    tsCLD_ScenesEnhancedViewSceneResponsePayload sResponse;
    uint8 au8NoSceneName[1] = {0};
    uint8 au8NoExtensionData[1] = {0};
    tsCLD_ScenesTableEntry *psTableEntry;
    uint8   u8TransactionSequenceNumber;

    /* Initialise variable length fields */
    sResponse.sSceneName.pu8Data = au8NoSceneName;
    sResponse.sSceneName.u8Length = 0;
    sResponse.sExtensionField.pu8Data = au8NoExtensionData;
    sResponse.sExtensionField.u16Length = 0;

    DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleEnhancedViewSceneRequest: ");

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination = &sPayload.u8SceneId;

    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        2,
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedViewSceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Try and find requested scene */
    eStatus = eCLD_ScenesFindScene(psEndPointDefinition,
                                   psClusterInstance,
                                   sPayload.u16GroupId,
                                   sPayload.u8SceneId,
                                   &psTableEntry);

    DBG_vPrintf(TRACE_SCENES, "Find: %d", eStatus);

    /* Construct response */
    switch(eStatus)
    {

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_SUCCESS;

        sResponse.sSceneName.pu8Data = psTableEntry->au8SceneName;
        sResponse.sSceneName.u8Length = strlen((char*)psTableEntry->au8SceneName);
        sResponse.sSceneName.u8MaxLength = sizeof(psTableEntry->au8SceneName);

        #ifdef CLD_SCENES_TABLE_SUPPORT_TRANSITION_TIME_IN_MS
            sResponse.u16TransitionTime = psTableEntry->u16TransitionTime * 10 + psTableEntry->u8TransitionTime100ms;
        #endif
        sResponse.sExtensionField.pu8Data = psTableEntry->au8SceneData;
        sResponse.sExtensionField.u16Length = psTableEntry->u16SceneDataLength;
        sResponse.sExtensionField.u16MaxLength = sizeof(psTableEntry->au8SceneData);

        DBG_vPrintf(TRACE_SCENES, "SNL%d SEL%d", sResponse.sSceneName.u8Length, sResponse.sExtensionField.u16Length);

        break;

    case E_ZCL_ERR_INVALID_VALUE:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_ZCL_ERR_SCENE_NOT_FOUND:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_NOT_FOUND;
        break;

    default:
        sResponse.eStatus = E_CLD_SCENES_CMD_STATUS_FAILURE;
        break;

    }
    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.u8SceneId  = sPayload.u8SceneId;

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    eStatus = eCLD_ScenesCommandEnhancedViewSceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     psZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);

    DBG_vPrintf(TRACE_SCENES, "Send: %d", eStatus);

    return eStatus;

}
#endif

#ifdef CLD_SCENES_CMD_COPY_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleCopySceneRequest
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleCopySceneRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition,
                            tsZCL_Address               *psZCL_Address,
                            tsCLD_ScenesCustomDataStructure *psCommon)
{
    uint8 u8Status;
    tsCLD_ScenesCopySceneRequestPayload sPayload;
    tsCLD_ScenesCopySceneResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;

    DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleCopySceneRequest: ");

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u8Mode;
    pasPayloadDefinition[1].pvDestination = &sPayload.u16FromGroupId;
    pasPayloadDefinition[2].pvDestination = &sPayload.u8FromSceneId;
    pasPayloadDefinition[3].pvDestination = &sPayload.u16ToGroupId;
    pasPayloadDefinition[4].pvDestination = &sPayload.u8ToSceneId;
    pasPayloadDefinition[4].eType =E_ZCL_UINT8;

    u8Status = eZCL_CustomCommandReceive(pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        5,
                                        E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);


    if(u8Status != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "Error: %d", u8Status);
        return u8Status;
    }

    DBG_vPrintf(TRACE_SCENES, "Mode%d Group%d:%d Scene%d:%d", sPayload.u8Mode,
                                                              sPayload.u16FromGroupId, sPayload.u16ToGroupId,
                                                              sPayload.u8FromSceneId, sPayload.u8ToSceneId);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psCopySceneRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Copy all scene's or just one ? */
    if(sPayload.u8Mode & CLD_SCENES_COPY_ALL_SCENES)
    {

        /* Copy all scene's from one group to another */
        u8Status = eCLD_ScenesCopyAllScenes(psEndPointDefinition,
                                           psClusterInstance,
                                           sPayload.u16FromGroupId,
                                           sPayload.u16ToGroupId);

    }
    else
    {

        /* Copy a single scene */
        u8Status = eCLD_ScenesCopyScene(psEndPointDefinition,
                                       psClusterInstance,
                                       sPayload.u16FromGroupId,
                                       sPayload.u8FromSceneId,
                                       sPayload.u16ToGroupId,
                                       sPayload.u8ToSceneId);

    }

    vSaveScenesNVM();

    /* If message was addressed to multiple nodes, exit, no response */
    if((pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_BOUND)  ||
       (pZPSevent->uEvent.sApsDataIndEvent.u8DstAddrMode == ZPS_E_ADDR_MODE_GROUP))
    {
        return E_ZCL_SUCCESS;
    }

    switch(u8Status)
    {

    case E_ZCL_SUCCESS:
        sResponse.u8Status = E_CLD_SCENES_CMD_STATUS_SUCCESS;
        break;

    case E_CLD_SCENES_NO_RESOURCES:
        sResponse.u8Status = E_CLD_SCENES_CMD_STATUS_INSUFFICIENT_SPACE;
        break;

    case E_CLD_SCENES_NOT_FOUND:
        sResponse.u8Status = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    case E_CLD_GROUPS_NOT_FOUND:
        sResponse.u8Status = E_CLD_SCENES_CMD_STATUS_INVALID_FIELD;
        break;

    default:
        break;

    }

    /* Check if need to respond back ?*/
    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    sResponse.u16FromGroupId = sPayload.u16FromGroupId;
    sResponse.u8FromSceneId = sPayload.u8FromSceneId;

    u8Status = eCLD_ScenesCommandCopySceneResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     psZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);


    return u8Status;

}
#endif
#endif /* SCENES_SERVER*/

#ifdef SCENES_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleAddSceneResponse
 **
 ** DESCRIPTION:
 ** Handles add scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleAddSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_ScenesAddSceneResponsePayload sPayload;
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[2].pvDestination =   &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        3,
                                        E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandAddSceneResponseReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psAddSceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleViewSceneResponse
 **
 ** DESCRIPTION:
 ** Handles view scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleViewSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_ScenesViewSceneResponsePayload sPayload;
    uint8 au8SceneName[CLD_SCENES_MAX_SCENE_NAME_LENGTH];
    uint8 au8ExtensionData[CLD_SCENES_MAX_SCENE_STORAGE_BYTES];
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Initialise variable length fields */
    sPayload.sSceneName.pu8Data = au8SceneName;
    sPayload.sSceneName.u8MaxLength = sizeof(au8SceneName);
    sPayload.sExtensionField.pu8Data = au8ExtensionData;
    sPayload.sExtensionField.u16Length = 0;
    sPayload.sExtensionField.u16MaxLength = sizeof(au8ExtensionData);

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[2].pvDestination =   &sPayload.u8SceneId;
    pasPayloadDefinition[3].pvDestination =   &sPayload.u16TransitionTime;
    pasPayloadDefinition[4].pvDestination =   &sPayload.sSceneName;
    pasPayloadDefinition[5].pvDestination =   sPayload.sExtensionField.pu8Data;

    pasPayloadDefinition[5].u16MaxQuantity =   CLD_SCENES_MAX_SCENE_STORAGE_BYTES;
    pasPayloadDefinition[5].pu16ActualQuantity =  &sPayload.sExtensionField.u16Length;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        6,
                                        E_ZCL_ACCEPT_LESS);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleViewSceneResponse Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psViewSceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleRemoveSceneResponse
 **
 ** DESCRIPTION:
 ** Handles remove scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleRemoveSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_ScenesRemoveSceneResponsePayload sPayload;
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[2].pvDestination =   &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        3,
                                        E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandRemoveSceneResponseReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRemoveSceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleRemoveAllScenesResponse
 **
 ** DESCRIPTION:
 ** Handles remove all scenes response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleRemoveAllScenesResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_ScenesRemoveAllScenesResponsePayload sPayload;
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u16GroupId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                         &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                         pasPayloadDefinition,
                                         2,
                                         E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandRemoveAllScenesResponseReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRemoveAllScenesResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleStoreSceneResponse
 **
 ** DESCRIPTION:
 ** Handles store scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleStoreSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_ScenesStoreSceneResponsePayload sPayload;
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination =   &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination =   &sPayload.u16GroupId;
    pasPayloadDefinition[2].pvDestination =   &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                         &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                         pasPayloadDefinition,
                                         3,
                                         E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandStoreSceneResponseReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStoreSceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleGetSceneMembershipResponse
 **
 ** DESCRIPTION:
 ** Handles Get Scene Membership response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleGetSceneMembershipResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_ScenesGetSceneMembershipResponsePayload sPayload;
    uint8 au8ScenesList[CLD_SCENES_MAX_NUMBER_OF_SCENES];
    tsCLD_ScenesCustomDataStructure *psCommon;

    sPayload.pu8SceneList = au8ScenesList;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination = &sPayload.u8Capacity;
    pasPayloadDefinition[2].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[3].pvDestination = &sPayload.u8SceneCount;
    pasPayloadDefinition[4].pvDestination = sPayload.pu8SceneList;

    pasPayloadDefinition[1].eType =E_ZCL_UINT8;
    pasPayloadDefinition[2].eType =E_ZCL_UINT16;
    pasPayloadDefinition[3].eType =E_ZCL_UINT8;
    pasPayloadDefinition[4].eType =E_ZCL_UINT8;

    pasPayloadDefinition[4].u16MaxQuantity =CLD_SCENES_MAX_NUMBER_OF_SCENES;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        5,
                                        E_ZCL_ACCEPT_LESS);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandGetSceneMembershipResponseReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetSceneMembershipResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

#ifdef  CLD_SCENES_CMD_ENHANCED_ADD_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleEnhancedAddSceneResponse
 **
 ** DESCRIPTION:
 ** Handles Enhanced Add Scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleEnhancedAddSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{
    teZCL_Status eStatus;
    tsCLD_ScenesEnhancedAddSceneResponsePayload sPayload;
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[2].pvDestination = &sPayload.u8SceneId;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        3,
                                        E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesCommandAddSceneResponseReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedAddSceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}
#endif

#ifdef CLD_SCENES_CMD_ENHANCED_VIEW_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleEnhancedViewSceneResponse
 **
 ** DESCRIPTION:
 ** Handles Enhanced View Scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleEnhancedViewSceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{
    teZCL_Status eStatus;
    tsCLD_ScenesEnhancedViewSceneResponsePayload sPayload;
    uint8 au8SceneName[CLD_SCENES_MAX_SCENE_NAME_LENGTH];
    uint8 au8ExtensionData[CLD_SCENES_MAX_SCENE_STORAGE_BYTES];
    //uint8   u8TransactionSequenceNumber;
    tsCLD_ScenesCustomDataStructure *psCommon;

    /* Initialise variable length fields */
    sPayload.sSceneName.pu8Data = au8SceneName;
    sPayload.sSceneName.u8MaxLength = sizeof(au8SceneName);
    sPayload.sExtensionField.pu8Data = au8ExtensionData;
    sPayload.sExtensionField.u16Length = 0;
    sPayload.sExtensionField.u16MaxLength = sizeof(au8ExtensionData);

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.eStatus;
    pasPayloadDefinition[1].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[2].pvDestination = &sPayload.u8SceneId;
    pasPayloadDefinition[3].pvDestination = &sPayload.u16TransitionTime;
    pasPayloadDefinition[4].pvDestination = &sPayload.sSceneName;
    pasPayloadDefinition[5].pvDestination = sPayload.sExtensionField.pu8Data;

    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                         &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                         pasPayloadDefinition,
                                         6,
                                         E_ZCL_ACCEPT_LESS);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleViewSceneResponse Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedViewSceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}
#endif

#ifdef CLD_SCENES_CMD_COPY_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesHandleCopySceneResponse
 **
 ** DESCRIPTION:
 ** Handles Copy Scene response
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesHandleCopySceneResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem         *pasPayloadDefinition)
{
    teZCL_Status eStatus;
    tsCLD_ScenesCopySceneResponsePayload sPayload;
    //uint8   u8TransactionSequenceNumber;
    tsCLD_ScenesCustomDataStructure *psCommon;


    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ScenesCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u8Status;
    pasPayloadDefinition[1].pvDestination = &sPayload.u16FromGroupId;
    pasPayloadDefinition[2].pvDestination = &sPayload.u8FromSceneId;


    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                                        &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
                                        pasPayloadDefinition,
                                        3,
                                        E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_SCENES, "\neCLD_ScenesHandleViewSceneResponse Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psCopySceneResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}
#endif
#endif // SCENES_CLIENT

#ifdef SCENES_SERVER
/****************************************************************************
 **
 ** NAME:       eZCL_CustomCommandReceiveScenes
 **
 ** DESCRIPTION:
 ** handles rx of a custom command for scenes cluster for variable payload in add scenes
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint8                    *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsZCL_RxPayloadItem      *psPayloadDefinition           Payload definition
 ** uint16                   u16InitialOffsetinPayload      Inialial offset
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE teZCL_Status eZCL_CustomCommandReceiveScenes(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsZCL_RxPayloadItem         *psPayloadDefinition,
                    uint16                       u16InitialOffsetinPayload)
{

    tsZCL_ClusterInstance *psZCL_ClusterInstance;
    uint16 u16ClusterIden;
    uint8 u8Len;

    int x;
    uint16 u16Offset;
    uint16 u16HeaderSize;
    uint16 u16PayloadSize;
    teZCL_ParseState eParseState;

    tsZCL_HeaderParams sZCL_HeaderParams;

    // parameter checks
    // input parameter checks
    if(
    #ifdef STRICT_PARAM_CHECK
           (pZPSevent == NULL)                                ||
           (psEndPointDefinition == NULL)                     ||
    #endif
       (psPayloadDefinition == NULL)                      ||
       (psPayloadDefinition->pu16ActualQuantity == NULL)   ||
       (psPayloadDefinition->pvDestination == NULL) )
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }


    // parse command header
    u16HeaderSize =  u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // get payload size
    u16PayloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);

    /* get offset just before the scene name length */
    u16Offset = u16HeaderSize + u16InitialOffsetinPayload;
    
    /* Add the offset of 1 byte for string name length */
    u16Offset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16Offset, E_ZCL_UINT8, &u8Len);
    
    /* Set offset to start of extension field set by adding the offset of scene name*/
    u16Offset += u8Len;

    u8Len = 0;
    
    eParseState=PARSE_CLUSTER_ID;
    /* For each item in the list */
    for(x = 0; (x < psPayloadDefinition->u16MaxQuantity) && (u16Offset < u16PayloadSize); )
    {
        switch(eParseState)
        {
            case PARSE_CLUSTER_ID :
                /*Read ClusterID*/
                u16Offset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16Offset, E_ZCL_UINT16, &u16ClusterIden);
                u16Offset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16Offset, E_ZCL_UINT8, &u8Len);
                /*is the length all right*/
                if((u16Offset+u8Len) > (u16PayloadSize) )
                {
                    return (E_ZCL_ERR_PARAMETER_NULL);
                }
                /*Serch for the cluster */
                /*if not found go to skip , else go to copy*/
                if(eZCL_SearchForClusterEntry(psEndPointDefinition->u8EndPointNumber, u16ClusterIden, TRUE, &psZCL_ClusterInstance) != E_ZCL_SUCCESS)
                {
                    eParseState=PARSER_SKIP;
                }
                else
                {
                    u16Offset -=3;
                    u8Len += 3;
                    eParseState=PARSER_COPY_TO_DESTINATION;
                }
                break;

            case PARSER_COPY_TO_DESTINATION :
                if(u8Len)
                {
                    u8Len--;
                    u16Offset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16Offset, E_ZCL_UINT8, ((uint8 *)((uint8 *)(psPayloadDefinition->pvDestination) + x++)));
                }
                else
                {
                    eParseState=PARSE_CLUSTER_ID;
                }
                break;
            case PARSER_SKIP :
                u16Offset = u16Offset+u8Len;
                eParseState=PARSE_CLUSTER_ID;
                break;
            default:
                break;
        }
        *psPayloadDefinition->pu16ActualQuantity = x;
    }
    return(E_ZCL_SUCCESS);
}

#ifdef CLD_SCENES_ATTR_LAST_CONFIGURED_BY
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesSetLastConfiguredBy
 **
 ** DESCRIPTION:
 ** Saves the Mac address of the sender of the last message in the last
 ** configured by attribute
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesSetLastConfiguredBy(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance)
{

    uint64 u64MacAddress;
    #ifdef STRICT_PARAM_CHECK
        if((pZPSevent == NULL) || (psEndPointDefinition == NULL) || (psClusterInstance == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    if(pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode == E_ZCL_AM_IEEE)
    {
        ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u64LastConfiguredBy = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr;
    }
    else
    {
        u64MacAddress = ZPS_u64NwkNibFindExtAddr(ZPS_pvNwkGetHandle(), pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
        if(u64MacAddress == ZPS_NWK_NULL_EXT_ADDR)
        {
            u64MacAddress = (uint64)0xffffffffffffffffLL;
        }
        ((tsCLD_Scenes*)psClusterInstance->pvEndPointSharedStructPtr)->u64LastConfiguredBy = u64MacAddress;
    }

    return E_ZCL_SUCCESS;

}
#endif

#if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER)
/****************************************************************************
 **
 ** NAME:       vCLD_ScenesConfigureColourControlExtension
 **
 ** DESCRIPTION:
 ** Finding the Extension Field Set for Colour Control Cluster and
 ** Updating irrelevant fields to zero
 **
 ** PARAMETERS:                     Name                      Usage
 ** tsCLD_ScenesExtensionField     *psExtensionField          Pointer to Extension field
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE void vCLD_ScenesConfigureColourControlExtension(
                            tsCLD_ScenesExtensionField *psExtensionField)
{
    int i = 0,j = 0;
    uint16 u16CurrentX = 0, u16CurrentY = 0;
    while(i < psExtensionField->u16Length)
    {
        if(((psExtensionField->pu8Data[i+1] << 8) | psExtensionField->pu8Data[i]) == LIGHTING_CLUSTER_ID_COLOUR_CONTROL)
        {
            /* CCB#1683
               On receipt of the enhanced add scene command of the scenes cluster,
               if the CurrentX and CurrentY parameters are set to zero, the scene
               shall be composed of only the EnhancedCurrentHue, CurrentSaturation,
               ColorLoopActive, ColorLoopDirection and ColorLoopTime parameters.
               If the CurrentX and CurrentY parameters are not set to zero, the 
               scene shall be composed of only the CurrentX and CurrentY parameters.
               All other scene table entries shall be set to zero.*/
               /* ZLO specification    
               if CurrentX==0 OR CurrentY ==0, and ColorTemperatureMired (if present in the extension field) <> 0xFFFF, 
               the ColorTemperature takes precedence over the Enhanced Hue and Saturation attributes. */

            /* If at least CurrentX and CurrentY are present in the extension */
            if(psExtensionField->pu8Data[i+2] > 0x04)
            {
                u16CurrentX = ((uint16)(psExtensionField->pu8Data[i+4] << 8) | psExtensionField->pu8Data[i+3]);
                u16CurrentY = ((uint16)(psExtensionField->pu8Data[i+6] << 8) | psExtensionField->pu8Data[i+5]);
                
                /* Starts from an index after the CurrentX and CurrentY */
                j = 5;
                while(j <= psExtensionField->pu8Data[i+2])
                {
                    /*If CurrentX <> 0 and CurrentY <> 0 THEN  XY Mode ELSE Hue\Sat mode */
                    if((u16CurrentX > 0) && (u16CurrentY > 0))
                    {
                        /* As XY mode make other fields to 0 */
                        if(j <= 0x0B)
                        {
                            psExtensionField->pu8Data[i+j+2] = 0;
                        }
                        else if(j <= 0x0D)
                        {
                            psExtensionField->pu8Data[i+j+2] = 0xFF;
                        }
                    }
                    /*if CurrentX==0 OR CurrentY ==0, and ColorTemperatureMired (if present in the extension field) <> 0xFFFF */ 
                    else if((u16CurrentX > 0) || (u16CurrentY > 0))
                    {
                        /* As colour Temp mode set XY and other fields to zero*/
                        if(j <= 0x0B)
                        {
                            psExtensionField->pu8Data[i+j+2] = 0;
                        }
                        if((j == 0x0D) &&
                          ((psExtensionField->pu8Data[i+14] != 0xFF) ||
                           (psExtensionField->pu8Data[i+15] != 0xFF)))
                            {
                                psExtensionField->pu8Data[i+3] = 0;
                                psExtensionField->pu8Data[i+4] = 0;
                                psExtensionField->pu8Data[i+5] = 0;
                                psExtensionField->pu8Data[i+6] = 0;
                            }
                    }    
                   else
                   {
                       /* As HSV mode set ColorTempMired to invalid */
                       if((j > 0x0B) && (j <= 0x0D))
                        {
                            psExtensionField->pu8Data[i+j+2] = 0xFF;
                        }
                   }
                   j++;
                }
            }  
            /* Coming out of the while loop */
            i = psExtensionField->u16Length;
        }
        else
        {
            /* Jumping to the next Extension Field Set : i + 2 Bytes for the ClusterId field + 1 Byte for length field + Length of the Extension Field */
            i = i + 2 + 1 + psExtensionField->pu8Data[i+2];
        }
    }
}
#endif //CLD_COLOUR_CONTROL
#endif /* SCENES_SERVER*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

