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
 * MODULE:             Window Covering Cluster
 *
 * COMPONENT:          cluster_window_covering.c
 *
 * DESCRIPTION:        Window Covering cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"
#include "WindowCovering.h"
#include "string.h"
#include "WindowCovering_internal.h"

#include "dbg.h"
#ifdef DEBUG_CLD_WINDOW_COVERING
#define TRACE_WINDOW_COVERING TRUE
#else
#define TRACE_WINDOW_COVERING FALSE
#endif

#ifdef CLD_SCENES
#include "Scenes.h"
#endif

#ifdef CLD_WINDOW_COVERING
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef WINDOW_COVERING_SERVER
PRIVATE teZCL_Status eCLD_WindowCoveringSceneEventHandler(
                        teZCL_SceneEvent            eEvent,
                        tsZCL_EndPointDefinition   *psEndPointDefinition,
                        tsZCL_ClusterInstance      *psClusterInstance);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef WINDOW_COVERING_SERVER

#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_WindowCoveringClusterCommandDefinitions[] = {
         {E_CLD_WC_CMD_UP_OPEN,               E_ZCL_CF_RX}    /* Mandatory */
        ,{E_CLD_WC_CMD_DOWN_CLOSE,            E_ZCL_CF_RX}    /* Mandatory */
        ,{E_CLD_WC_CMD_STOP,                  E_ZCL_CF_RX}    /* Mandatory */
#ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
        ,{E_CLD_WC_CMD_GO_TO_LIFT_VALUE,      E_ZCL_CF_RX}    /* Optional */
#endif 
#ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
        ,{E_CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE, E_ZCL_CF_RX}    /* Optional */
#endif
#ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
        ,{E_CLD_WC_CMD_GO_TO_TILT_VALUE,      E_ZCL_CF_RX}    /* Optional */
#endif
#ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
        ,{E_CLD_WC_CMD_GO_TO_TILT_PERCENTAGE, E_ZCL_CF_RX}    /* Optional */
#endif
    };
#endif

const tsZCL_AttributeDefinition asCLD_WindowCoveringClusterAttributeDefinitions[] = {
    {E_CLD_WC_ATTR_ID_WINDOW_COVERING_TYPE,       (E_ZCL_AF_RD), E_ZCL_ENUM8,  (uint32)(&((tsCLD_WindowCovering*)(0))->eWindowCoveringType),        0},
#ifdef CLD_WC_ATTR_PHYSICAL_CLOSED_LIMIT_LIFT
    {E_CLD_WC_ATTR_ID_PHYSICAL_CLOSED_LIMIT_LIFT, (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16PhysicalClosedLimitLift), 0},
#endif
#ifdef CLD_WC_ATTR_PHYSICAL_CLOSED_LIMIT_TILT
    {E_CLD_WC_ATTR_ID_PHYSICAL_CLOSED_LIMIT_TILT, (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16PhysicalClosedLimitTilt), 0},
#endif
#ifdef CLD_WC_ATTR_CURRENT_POSITION_LIFT
    {E_CLD_WC_ATTR_ID_CURRENT_POSITION_LIFT,      (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16CurrentPositionLift),     0},
#endif
#ifdef CLD_WC_ATTR_CURRENT_POSITION_TILT
    {E_CLD_WC_ATTR_ID_CURRENT_POSITION_TILT,      (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16CurrentPositionTilt),     0},
#endif
#ifdef CLD_WC_ATTR_NUMBER_OF_ACTUATIONS_LIFT
    {E_CLD_WC_ATTR_ID_NUMBER_OF_ACTUATIONS_LIFT,  (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16NumberOfActuationsLift),  0},
#endif
#ifdef CLD_WC_ATTR_NUMBER_OF_ACTUATIONS_TILT
    {E_CLD_WC_ATTR_ID_NUMBER_OF_ACTUATIONS_TILT,  (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16NumberOfActuationsTilt),  0},
#endif
    {E_CLD_WC_ATTR_ID_CONFIG_STATUS,              (E_ZCL_AF_RD), E_ZCL_BMAP8,  (uint32)(&((tsCLD_WindowCovering*)(0))->u8ConfigStatus),             0},
#ifdef CLD_WC_ATTR_CURRENT_POSITION_LIFT_PERCENTAGE
    {E_CLD_WC_ATTR_ID_CURRENT_POSITION_LIFT_PERCENTAGE, (E_ZCL_AF_RD|E_ZCL_AF_RP|E_ZCL_AF_SE), E_ZCL_UINT8, (uint32)(&((tsCLD_WindowCovering*)(0))->u8CurrentPositionLiftPercentage), 0},
#endif
#ifdef CLD_WC_ATTR_CURRENT_POSITION_TILT_PERCENTAGE
    {E_CLD_WC_ATTR_ID_CURRENT_POSITION_TILT_PERCENTAGE, (E_ZCL_AF_RD|E_ZCL_AF_RP|E_ZCL_AF_SE), E_ZCL_UINT8, (uint32)(&((tsCLD_WindowCovering*)(0))->u8CurrentPositionTiltPercentage), 0},
#endif
#ifdef CLD_WC_ATTR_INSTALLED_OPEN_LIMIT_LIFT
    {E_CLD_WC_ATTR_ID_INSTALLED_OPEN_LIMIT_LIFT,   (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16InstalledOpenLimitLift),    0},
#endif
#ifdef CLD_WC_ATTR_INSTALLED_CLOSED_LIMIT_LIFT
    {E_CLD_WC_ATTR_ID_INSTALLED_CLOSED_LIMIT_LIFT, (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16InstalledClosedLimitLift),  0},
#endif
#ifdef CLD_WC_ATTR_INSTALLED_OPEN_LIMIT_TILT
    {E_CLD_WC_ATTR_ID_INSTALLED_OPEN_LIMIT_TILT,   (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16InstalledOpenLimitTilt),    0},
#endif
#ifdef CLD_WC_ATTR_INSTALLED_CLOSED_LIMIT_TILT
    {E_CLD_WC_ATTR_ID_INSTALLED_CLOSED_LIMIT_TILT, (E_ZCL_AF_RD), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16InstalledClosedLimitTilt),  0},
#endif
#ifdef CLD_WC_ATTR_VELOCITY_LIFT
    {E_CLD_WC_ATTR_ID_VELOCITY_LIFT,               (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16VelocityLift),          0},
#endif
#ifdef CLD_WC_ATTR_ACCELERATION_TIME_LIFT
    {E_CLD_WC_ATTR_ID_ACCELERATION_TIME_LIFT,      (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16AccelerationTimeLift),  0},
#endif
#ifdef CLD_WC_ATTR_DECELERATION_TIME_LIFT
    {E_CLD_WC_ATTR_ID_DECELERATION_TIME_LIFT,      (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCovering*)(0))->u16DecelerationTimeLift),  0},
#endif
    {E_CLD_WC_ATTR_ID_MODE,                        (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_BMAP8,  (uint32)(&((tsCLD_WindowCovering*)(0))->u8Mode),                   0},
#ifdef CLD_WC_ATTR_INTERMEDIATE_SETPOINTS_LIFT
    {E_CLD_WC_ATTR_ID_INTERMEDIATE_SETPOINTS_LIFT, (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_OSTRING, (uint32)(&((tsCLD_WindowCovering*)(0))->sIntermediateSetpointsLift), 0},
#endif
#ifdef CLD_WC_ATTR_INTERMEDIATE_SETPOINTS_TILT
    {E_CLD_WC_ATTR_ID_INTERMEDIATE_SETPOINTS_TILT, (E_ZCL_AF_RD|E_ZCL_AF_WR), E_ZCL_OSTRING, (uint32)(&((tsCLD_WindowCovering*)(0))->sIntermediateSetpointsTilt), 0},
#endif
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,             (E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_BMAP32,  (uint32)(&((tsCLD_WindowCovering*)(0))->u32FeatureMap),         0},
    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,        (E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_UINT16,  (uint32)(&((tsCLD_WindowCovering*)(0))->u16ClusterRevision),         0}
};

/* List of attributes in the scene extension table */
tsZCL_SceneExtensionTable sCLD_WindowCoveringSceneExtensionTable =
{
    eCLD_WindowCoveringSceneEventHandler,
    2,
    {E_CLD_WC_ATTR_ID_CURRENT_POSITION_LIFT_PERCENTAGE,
     E_CLD_WC_ATTR_ID_CURRENT_POSITION_TILT_PERCENTAGE}
};

tsZCL_ClusterDefinition sCLD_WindowCovering = {
    CLUSTER_ID_WINDOW_COVERING,
    FALSE,
    E_ZCL_SECURITY_NETWORK,
    (sizeof(asCLD_WindowCoveringClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
    (tsZCL_AttributeDefinition*)asCLD_WindowCoveringClusterAttributeDefinitions,
    &sCLD_WindowCoveringSceneExtensionTable
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    ,
    (sizeof(asCLD_WindowCoveringClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
    (tsZCL_CommandDefinition*)asCLD_WindowCoveringClusterCommandDefinitions
#endif
};

uint8 au8WindowCoveringAttributeControlBits[(sizeof(asCLD_WindowCoveringClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

#endif /* WINDOW_COVERING_SERVER */

#ifdef WINDOW_COVERING_CLIENT

const tsZCL_AttributeDefinition asCLD_WindowCoveringClientClusterAttributeDefinitions[] = {
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP, (E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_BMAP32, (uint32)(&((tsCLD_WindowCoveringClient*)(0))->u32FeatureMap), 0}
    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION, (E_ZCL_AF_RD|E_ZCL_AF_GA), E_ZCL_UINT16, (uint32)(&((tsCLD_WindowCoveringClient*)(0))->u16ClusterRevision), 0}
};

tsZCL_ClusterDefinition sCLD_WindowCoveringClient = {
        CLUSTER_ID_WINDOW_COVERING,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_WindowCoveringClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_WindowCoveringClientClusterAttributeDefinitions,
        NULL
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
        ,
        0,
        NULL
#endif
};

uint8 au8WindowCoveringClientAttributeControlBits[(sizeof(asCLD_WindowCoveringClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

#endif /* WINDOW_COVERING_CLIENT */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringCreateWindowCovering
 **
 ** DESCRIPTION:
 ** Creates an on/off cluster
 **
 ** PARAMETERS:                             Name                       Usage
 ** tsZCL_ClusterInstance                   *psClusterInstance         Cluster structure
 ** bool_t                                   bIsServer                 Server Client flag
 ** tsZCL_ClusterDefinition                 *psClusterDefinition       Cluster Definition
 ** void                                    *pvEndPointSharedStructPtr Endpoint shared structure
 ** uint8                                   *pu8AttributeControlBits   Attribute control bits
 ** tsCLD_WindowCoveringCustomDataStructure *psCustomDataStructure     Custom data Structure
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_WindowCoveringCreateWindowCovering(
    tsZCL_ClusterInstance                   *psClusterInstance,
    bool_t                                   bIsServer,
    tsZCL_ClusterDefinition                 *psClusterDefinition,
    void                                    *pvEndPointSharedStructPtr,
    uint8                                   *pu8AttributeControlBits,
    tsCLD_WindowCoveringCustomDataStructure *psCustomDataStructure)
{

    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if(psClusterInstance==NULL)
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
           NULL,
           eCLD_WindowCoveringCommandHandler);

    if(psClusterInstance->pvEndPointSharedStructPtr != NULL)
    {
        if(bIsServer)
        {
        #ifdef WINDOW_COVERING_SERVER
            // set default state
            ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u8ConfigStatus = 0x3;
            #ifdef CLD_WC_ATTR_INSTALLED_CLOSED_LIMIT_LIFT
                ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u16InstalledClosedLimitLift = 0xFFFF;
            #endif
            #ifdef CLD_WC_ATTR_INSTALLED_CLOSED_LIMIT_TILT
                ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u16InstalledClosedLimitTilt = 0xFFFF;
            #endif
            ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u8Mode = 0x4;
            ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ClusterRevision = CLD_WC_CLUSTER_REVISION;
            ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u32FeatureMap = CLD_WC_FEATURE_MAP;
        #endif
        }
        else
        {
        #ifdef WINDOW_COVERING_CLIENT
            // set default state
            ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ClusterRevision = CLD_WC_CLUSTER_REVISION;
            ((tsCLD_WindowCovering *)(psClusterInstance->pvEndPointSharedStructPtr))->u32FeatureMap = CLD_WC_FEATURE_MAP;
        #endif
        }
    }

    #ifdef WINDOW_COVERING_SERVER
        if(bIsServer)
        {
            /* As this cluster has reportable attributes enable default reporting */
            vZCL_SetDefaultReporting(psClusterInstance);
        }
    #endif

    return E_ZCL_SUCCESS;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
#ifdef WINDOW_COVERING_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringSceneEventHandler
 **
 ** DESCRIPTION:
 ** Handles events generated by a scenes cluster (if present)
 **
 ** PARAMETERS:                 Name                        Usage
 ** teZCL_SceneEvent            eEvent                      Scene Event
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End Point Definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringSceneEventHandler(
    teZCL_SceneEvent            eEvent,
    tsZCL_EndPointDefinition   *psEndPointDefinition,
    tsZCL_ClusterInstance      *psClusterInstance)
{

    tsZCL_CallBackEvent sZCL_CallBackEvent;

    switch(eEvent)
    {

    case E_ZCL_SCENE_EVENT_SAVE:
        DBG_vPrintf(TRACE_WINDOW_COVERING, "\r\nWC: Scene Event: Save");
        break;

    case E_ZCL_SCENE_EVENT_RECALL:
        DBG_vPrintf(TRACE_WINDOW_COVERING, "\r\nWC: Scene Event: Recall");

        /* Inform the application that the cluster has just been updated */
        sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
        sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
        sZCL_CallBackEvent.pZPSevent = NULL;
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_UPDATE;

        // call user
        psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
        vCLD_ScenesUpdateSceneValid(psEndPointDefinition);
#endif
        break;
    }

    return E_ZCL_SUCCESS;
}
#endif /* WINDOW_COVERING_SERVER */
#endif /* CLD_WINDOW_COVERING */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

