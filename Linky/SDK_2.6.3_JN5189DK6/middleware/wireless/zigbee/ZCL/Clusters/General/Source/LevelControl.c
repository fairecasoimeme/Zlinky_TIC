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
 * MODULE:             Level Control Cluster
 *
 * COMPONENT:          LevelControl.c
 *
 * DESCRIPTION:        Level Control cluster definition
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
#include "zcl_options.h"
#include "string.h"
#include "LevelControl.h"
#include "LevelControl_internal.h"


#ifdef CLD_ONOFF
#include "OnOff.h"
#include "OnOff_internal.h"
#endif

#ifdef CLD_COLOUR_CONTROL
#include "ColourControl.h"
#endif

#ifdef CLD_SCENES
#include "Scenes.h"
#endif

#include "dbg.h"

#ifdef DEBUG_CLD_LEVEL_CONTROL
#define TRACE_LEVEL_CONTROL    TRUE
#else
#define TRACE_LEVEL_CONTROL    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_LEVEL_CONTROL) && !defined(LEVEL_CONTROL_SERVER) && !defined(LEVEL_CONTROL_CLIENT)
#error You Must Have either LEVEL_CONTROL_SERVER and/or LEVEL_CONTROL_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef LEVEL_CONTROL_SERVER
PRIVATE teZCL_Status eCLD_LevelControlSceneEventHandler(
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
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_LevelControlClusterCommandDefinitions[] = {
        {E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL,                E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_LEVELCONTROL_CMD_MOVE,                         E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_LEVELCONTROL_CMD_STEP,                         E_ZCL_CF_RX},     /* Mandatory */    
        {E_CLD_LEVELCONTROL_CMD_STOP,                         E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF,    E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF,             E_ZCL_CF_RX},     /* Mandatory */  
        {E_CLD_LEVELCONTROL_CMD_STEP_WITH_ON_OFF,             E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_LEVELCONTROL_CMD_STOP_WITH_ON_OFF,             E_ZCL_CF_RX},
        #ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
        {E_CLD_LEVELCONTROL_CMD_MOVE_TO_CLOSEST_FREQ,         E_ZCL_CF_RX}
        #endif
    };
#endif

#ifdef LEVEL_CONTROL_SERVER
    const tsZCL_AttributeDefinition asCLD_LevelControlClusterAttributeDefinitions[] = {
        {E_CLD_LEVELCONTROL_ATTR_ID_CURRENT_LEVEL,                  (E_ZCL_AF_RD|E_ZCL_AF_SE|E_ZCL_AF_RP),  E_ZCL_UINT8,   (uint32)(&((tsCLD_LevelControl*)(0))->u8CurrentLevel),0},    /* Mandatory */

    #ifdef CLD_LEVELCONTROL_ATTR_REMAINING_TIME
        {E_CLD_LEVELCONTROL_ATTR_ID_REMAINING_TIME,                 E_ZCL_AF_RD,                            E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16RemainingTime),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_MIN_LEVEL
        {E_CLD_LEVELCONTROL_ATTR_ID_MIN_LEVEL,                      E_ZCL_AF_RD,                            E_ZCL_UINT8,  (uint32)(&((tsCLD_LevelControl*)(0))->u8MinLevel),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_MAX_LEVEL
        {E_CLD_LEVELCONTROL_ATTR_ID_MAX_LEVEL,                      E_ZCL_AF_RD,                            E_ZCL_UINT8,  (uint32)(&((tsCLD_LevelControl*)(0))->u8MaxLevel),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
        {E_CLD_LEVELCONTROL_ATTR_ID_CURRENT_FREQUENCY,             (E_ZCL_AF_RD|E_ZCL_AF_SE|E_ZCL_AF_RP),   E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16CurrentFrequency),0},

        {E_CLD_LEVELCONTROL_ATTR_ID_MIN_FREQUENCY,                 E_ZCL_AF_RD,                            E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16MinFrequency),0},

        {E_CLD_LEVELCONTROL_ATTR_ID_MAX_FREQUENCY,                 E_ZCL_AF_RD,                            E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16MaxFrequency),0},
    #endif
        
        {E_CLD_LEVELCONTROL_ATTR_ID_OPTIONS,                        (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_BMAP8,   (uint32)(&((tsCLD_LevelControl*)(0))->u8Options),0},         /* Mandatory */
        
    #ifdef CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME
        {E_CLD_LEVELCONTROL_ATTR_ID_ON_OFF_TRANSITION_TIME,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16OnOffTransitionTime),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_ON_LEVEL
        {E_CLD_LEVELCONTROL_ATTR_ID_ON_LEVEL,                       (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT8,   (uint32)(&((tsCLD_LevelControl*)(0))->u8OnLevel),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_ON_TRANSITION_TIME
        {E_CLD_LEVELCONTROL_ATTR_ID_ON_TRANSITION_TIME,             (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16OnTransitionTime),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_OFF_TRANSITION_TIME
        {E_CLD_LEVELCONTROL_ATTR_ID_OFF_TRANSITION_TIME,            (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,  (uint32)(&((tsCLD_LevelControl*)(0))->u16OffTransitionTime),0},
    #endif

    #ifdef CLD_LEVELCONTROL_ATTR_DEFAULT_MOVE_RATE
        {E_CLD_LEVELCONTROL_ATTR_ID_DEFAULT_MOVE_RATE,              (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_UINT8,  (uint32)(&((tsCLD_LevelControl*)(0))->u8DefaultMoveRate),0},
    #endif
    
    #ifdef CLD_LEVELCONTROL_ATTR_STARTUP_CURRENT_LEVEL
        {E_CLD_LEVELCONTROL_ATTR_ID_STARTUP_CURRENT_LEVEL,          (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT8,   (uint32)(&((tsCLD_LevelControl*)(0))->u8StartUpCurrentLevel),0},    /* Optional */
    #endif

    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                              (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_BMAP32,   (uint32)(&((tsCLD_LevelControl*)(0))->u32FeatureMap),0},   /* Mandatory  */        
        
    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                         (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_UINT16,   (uint32)(&((tsCLD_LevelControl*)(0))->u16ClusterRevision),0},   /* Mandatory  */
            
    #ifdef CLD_LEVELCONTROL_ATTR_ATTRIBUTE_REPORTING_STATUS
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,           (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_ENUM8,    (uint32)(&((tsCLD_LevelControl*)(0))->u8AttributeReportingStatus),0},
    #endif
    };
    
    /* List of attributes in the scene extension table */
    tsZCL_SceneExtensionTable sCLD_LevelControlSceneExtensionTable = {
            eCLD_LevelControlSceneEventHandler,
            1,
			{ {E_CLD_LEVELCONTROL_ATTR_ID_CURRENT_LEVEL, E_ZCL_UINT8} }
    };

    tsZCL_ClusterDefinition sCLD_LevelControl = {
            GENERAL_CLUSTER_ID_LEVEL_CONTROL,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_LevelControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_LevelControlClusterAttributeDefinitions,
            &sCLD_LevelControlSceneExtensionTable
            #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
                ,
                (sizeof(asCLD_LevelControlClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
                (tsZCL_CommandDefinition*)asCLD_LevelControlClusterCommandDefinitions         
            #endif        
    };
    
    uint8 au8LevelControlAttributeControlBits[(sizeof(asCLD_LevelControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];    
#endif

#ifdef LEVEL_CONTROL_CLIENT
    const tsZCL_AttributeDefinition asCLD_LevelControlClientClusterAttributeDefinitions[] = {
      {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_BMAP32,   (uint32)(&((tsCLD_LevelControlClient*)(0))->u32FeatureMap),0},   /* Mandatory  */
      {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),                E_ZCL_UINT16,   (uint32)(&((tsCLD_LevelControlClient*)(0))->u16ClusterRevision),0},   /* Mandatory  */
    };

    tsZCL_ClusterDefinition sCLD_LevelControlClient = {
            GENERAL_CLUSTER_ID_LEVEL_CONTROL,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_LevelControlClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_LevelControlClientClusterAttributeDefinitions,
            NULL
            #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
                ,
                (sizeof(asCLD_LevelControlClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
                (tsZCL_CommandDefinition*)asCLD_LevelControlClusterCommandDefinitions         
            #endif        
    };
    uint8 au8LevelControlClientAttributeControlBits[(sizeof(asCLD_LevelControlClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))]; 
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlCreateLevelControl
 **
 ** DESCRIPTION:
 ** Creates a Level Control cluster
 **
 ** PARAMETERS:                 Name                                Usage
 ** tsZCL_ClusterInstance      *psClusterInstance                   Cluster structure
 ** bool_t                      bIsServer                           Server Client flag
 ** tsZCL_ClusterDefinition    *psClusterDefinition                 Cluster Definition
 ** void                       *pvEndPointSharedStructPtr           EndPoint Shared Structure
 ** uint8                      *pu8AttributeControlBits             Attribute Control Bits
 ** tsCLD_LevelControlCustomDataStructure *psCustomDataStructure    Custom Data Structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlCreateLevelControl(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,
                tsCLD_LevelControlCustomDataStructure   *psCustomDataStructure)
{

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
           eCLD_LevelControlCommandHandler);    

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
    /* Level control transition state */
    psCustomDataStructure->sTransition.eTransition      = E_CLD_LEVELCONTROL_CMD_NONE;
    psCustomDataStructure->sTransition.eMode            = E_CLD_LEVELCONTROL_MOVE_MODE_UP;
    psCustomDataStructure->sTransition.iCurrentLevel    = CLD_LEVELCONTROL_MIN_LEVEL * 100;
    psCustomDataStructure->sTransition.iTargetLevel     = CLD_LEVELCONTROL_MIN_LEVEL * 100;
    psCustomDataStructure->sTransition.iPreviousLevel   = CLD_LEVELCONTROL_MIN_LEVEL * 100;

    psCustomDataStructure->sTransition.iStepSize        = 0;
    psCustomDataStructure->sTransition.u32Time          = 0;

    /* Set attribute default values */
    if(pvEndPointSharedStructPtr != NULL)
    {

        if(bIsServer)
        {
        #ifdef LEVEL_CONTROL_SERVER
            // set default state
            #ifdef CLD_LEVELCONTROL_ATTR_STARTUP_CURRENT_LEVEL
                if((((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8StartUpCurrentLevel > 0) && (((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8StartUpCurrentLevel < 0xFF))
                {
                    ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8CurrentLevel = ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8StartUpCurrentLevel;
                }
            #else
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8CurrentLevel = CLD_LEVELCONTROL_MIN_LEVEL;
            #endif

                if(((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8CurrentLevel < CLD_LEVELCONTROL_MIN_LEVEL)
                {
                    ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8CurrentLevel = CLD_LEVELCONTROL_MIN_LEVEL;
                }
                if(((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8CurrentLevel > CLD_LEVELCONTROL_MAX_LEVEL)
                {
                    ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8CurrentLevel = CLD_LEVELCONTROL_MAX_LEVEL;
                }
            
            #ifdef CLD_LEVELCONTROL_ATTR_REMAINING_TIME
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u16RemainingTime = 0;
            #endif

            #ifdef CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u16OnOffTransitionTime = CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME;
            #endif

            #ifdef CLD_LEVELCONTROL_ATTR_ON_LEVEL
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8OnLevel = CLD_LEVELCONTROL_ATTR_ON_LEVEL;
            #endif
            
            ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_LEVELCONTROL_CLUSTER_REVISION;
            
            ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap = 0x00000000; 

            #ifdef CLD_ONOFF
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap = ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap | LEVELCONTROL_OO_BIT;
            #endif            
            #if (defined CLD_LEVELCONTROL_ATTR_REMAINING_TIME) || (defined CLD_LEVELCONTROL_ATTR_STARTUP_CURRENT_LEVEL)
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap = ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap | LEVELCONTROL_LT_BIT;
            #endif
            
            #ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u16MinFrequency = CLD_LEVELCONTROL_MIN_FREQ;
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u16MaxFrequency = CLD_LEVELCONTROL_MAX_FREQ;
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u16CurrentFrequency = CLD_LEVELCONTROL_MIN_FREQ;
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap = ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u32FeatureMap | LEVELCONTROL_FQ_BIT;
            #endif
            
            #ifdef CLD_LEVELCONTROL_ATTR_MIN_LEVEL
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8MinLevel = CLD_LEVELCONTROL_MIN_LEVEL;
            #endif
            #ifdef CLD_LEVELCONTROL_ATTR_MAX_LEVEL
                ((tsCLD_LevelControl*)pvEndPointSharedStructPtr)->u8MaxLevel = CLD_LEVELCONTROL_MAX_LEVEL;
            #endif
        #endif    
        }
        else
        {
        #ifdef LEVEL_CONTROL_CLIENT
            ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_LEVELCONTROL_CLUSTER_REVISION;
            ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap = 0x00000000; 

            #ifdef CLD_ONOFF
                ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap = ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap | LEVELCONTROL_OO_BIT;
            #endif 
            
            #if (defined CLD_LEVELCONTROL_ATTR_REMAINING_TIME) || (defined CLD_LEVELCONTROL_ATTR_STARTUP_CURRENT_LEVEL)
                ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap = ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap | LEVELCONTROL_LT_BIT;
            #endif
            #ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
                ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap = ((tsCLD_LevelControlClient*)pvEndPointSharedStructPtr)->u32FeatureMap | LEVELCONTROL_FQ_BIT;
            #endif
        #endif
        }

    }
    
    #ifdef LEVEL_CONTROL_SERVER
        if(bIsServer)
        {    
            /* As this cluster has reportable attributes enable default reporting */
            vZCL_SetDefaultReporting(psClusterInstance);
        }
    #endif
    
    return E_ZCL_SUCCESS;
}

#ifdef LEVEL_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlUpdate
 **
 ** DESCRIPTION:
 ** Updates the the state of a level control cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlUpdate(uint8 u8SourceEndPointId)
{

	bool_t bGenerateEvent = FALSE;
    teZCL_Status eStatus;
    tsCLD_LevelControlCustomDataStructure *psCommon;

    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_LevelControl *psSharedStruct;
    
    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER) && (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        tsCLD_ColourControlCustomDataStructure *psColourCommon;
        tsZCL_ClusterInstance *psColourClusterInstance;
        tsCLD_ColourControl *psColourSharedStruct;
        uint16 u16MinMireds,u16MaxMireds;
        float fMiredsRange,fLevelRange,fLevel;  
    #endif
    bool_t bOnOff = FALSE;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_LEVEL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    /* Get pointer to level control cluster shared struct */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;

    /* Make sure target tracks any changes to current level made by the app */
    if((psCommon->sTransition.iCurrentLevel / 100) != psSharedStruct->u8CurrentLevel)
    {
        psCommon->sTransition.iCurrentLevel = psSharedStruct->u8CurrentLevel * 100;
    }


#if (defined CLD_ONOFF && defined ONOFF_SERVER)
    /* Get pointer to shared struct in On/Off cluster */
    if(eCLD_OnOffGetState(u8SourceEndPointId, &bOnOff) != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Failed to get ON/OFF");
        return eStatus;
    }
#endif


    /* get EP mutex */
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif



    switch(psCommon->sTransition.eTransition)
    {

    case E_CLD_LEVELCONTROL_TRANSITION_MOVE_TO_LEVEL:
    case E_CLD_LEVELCONTROL_TRANSITION_STEP:
        bGenerateEvent = TRUE;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Updating EP:%d Target=%i Cur=%i Step=%i Time=%i OnOff=%i", u8SourceEndPointId, psCommon->sTransition.iTargetLevel, psCommon->sTransition.iCurrentLevel, psCommon->sTransition.iStepSize, psCommon->sTransition.u32Time, bOnOff);

        if(psCommon->sTransition.u32Time > 0)
        {
            psCommon->sTransition.iCurrentLevel += psCommon->sTransition.iStepSize;

            /* Ensure it stays within limits */
            psCommon->sTransition.iCurrentLevel = CLAMP(psCommon->sTransition.iCurrentLevel, CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
            psCommon->sTransition.u32Time--;
        }
        else
        {
            /* Ensure final value stays within limits */
            psCommon->sTransition.iCurrentLevel = CLAMP(psCommon->sTransition.iTargetLevel, CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
            psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_CMD_NONE;
        }
        break;

    case E_CLD_LEVELCONTROL_TRANSITION_MOVE:
        bGenerateEvent = TRUE;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Updating EP:%d Target=%i Cur=%i Step=%i Time=%i OnOff=%i", u8SourceEndPointId, psCommon->sTransition.iTargetLevel, psCommon->sTransition.iCurrentLevel, psCommon->sTransition.iStepSize, psCommon->sTransition.u32Time, bOnOff);

        switch(psCommon->sTransition.eMode)
        {

        case E_CLD_LEVELCONTROL_MOVE_MODE_UP:
            psCommon->sTransition.iCurrentLevel = MIN(CLD_LEVELCONTROL_MAX_LEVEL * 100, psCommon->sTransition.iCurrentLevel + psCommon->sTransition.iStepSize);
            if(psCommon->sTransition.iCurrentLevel == (CLD_LEVELCONTROL_MAX_LEVEL * 100))
            {
                psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_CMD_NONE;
            }
            break;

        case E_CLD_LEVELCONTROL_MOVE_MODE_DOWN:
            psCommon->sTransition.iCurrentLevel = MAX(CLD_LEVELCONTROL_MIN_LEVEL * 100, psCommon->sTransition.iCurrentLevel + psCommon->sTransition.iStepSize);
            if(psCommon->sTransition.iCurrentLevel == (CLD_LEVELCONTROL_MIN_LEVEL * 100))
            {
                psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_CMD_NONE;
            }
            break;

        }
        break;

    case E_CLD_LEVELCONTROL_TRANSITION_ON:
        bGenerateEvent = TRUE;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Updating EP:%d Target=%i Cur=%i Step=%i Time=%i OnOff=%i", u8SourceEndPointId, psCommon->sTransition.iTargetLevel, psCommon->sTransition.iCurrentLevel, psCommon->sTransition.iStepSize, psCommon->sTransition.u32Time, bOnOff);
        /* If not already on, turn on */
        if(bOnOff == FALSE)
        {
            #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
                eCLD_OnOffSetState(u8SourceEndPointId, TRUE);
            #endif
        }

        if(psCommon->sTransition.u32Time > 0)
        {
            psCommon->sTransition.iCurrentLevel += psCommon->sTransition.iStepSize;
            psCommon->sTransition.iCurrentLevel = MIN(CLD_LEVELCONTROL_MAX_LEVEL * 100, psCommon->sTransition.iCurrentLevel);

            psCommon->sTransition.u32Time--;
            }
        else
        {
            /* Ensure final value stays within limits */
            psCommon->sTransition.iCurrentLevel = CLAMP(psCommon->sTransition.iTargetLevel, CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
            psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_CMD_NONE;
        }
        break;

    case E_CLD_LEVELCONTROL_TRANSITION_OFF:
        bGenerateEvent = TRUE;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Updating EP:%d Target=%i Cur=%i Step=%i Time=%i OnOff=%i", u8SourceEndPointId, psCommon->sTransition.iTargetLevel, psCommon->sTransition.iCurrentLevel, psCommon->sTransition.iStepSize, psCommon->sTransition.u32Time, bOnOff);

        if(psCommon->sTransition.u32Time > 0)
            {
            psCommon->sTransition.iCurrentLevel += psCommon->sTransition.iStepSize;
            psCommon->sTransition.iCurrentLevel = MAX(CLD_LEVELCONTROL_MIN_LEVEL * 100, psCommon->sTransition.iCurrentLevel);
            //ToDo:RemoveThisLine:- psCommon->sTransition.iPreviousLevel = psCommon->sTransition.iCurrentLevel;
            psCommon->sTransition.u32Time--;
            }
        else
        {
            #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
                eCLD_OnOffSetState(u8SourceEndPointId, FALSE);
            #endif
            
            #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER)
                eCLD_ColourControlStopTransition(u8SourceEndPointId);
            #endif
            
            /* Ensure final value stays within limits */
            psCommon->sTransition.iCurrentLevel = CLAMP(psCommon->sTransition.iPreviousLevel, CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
            psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_CMD_NONE;
        }
        break;

    case E_CLD_LEVELCONTROL_TRANSITION_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF:
        bGenerateEvent = TRUE;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Updating EP:%d Target=%i Cur=%i Step=%i Time=%i OnOff=%i", u8SourceEndPointId, psCommon->sTransition.iTargetLevel, psCommon->sTransition.iCurrentLevel, psCommon->sTransition.iStepSize, psCommon->sTransition.u32Time, bOnOff);

        if(psCommon->sTransition.u32Time > 0)
            {
            psCommon->sTransition.iCurrentLevel += psCommon->sTransition.iStepSize;
            psCommon->sTransition.iCurrentLevel = MAX(CLD_LEVELCONTROL_MIN_LEVEL * 100, psCommon->sTransition.iCurrentLevel);
            psCommon->sTransition.u32Time--;
            }
        else
        {
            psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_TRANSITION_OFF;
            psCommon->sTransition.u32Time = 40;  // Instead of 12 seconds (120)
            psCommon->sTransition.iStepSize = (psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
        }
        break;

    case E_CLD_LEVELCONTROL_TRANSITION_OFF_WITH_EFFECT_DIM_UP_FADE_OFF:
        bGenerateEvent = TRUE;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Updating EP:%d Target=%i Cur=%i Step=%i Time=%i OnOff=%i", u8SourceEndPointId, psCommon->sTransition.iTargetLevel, psCommon->sTransition.iCurrentLevel, psCommon->sTransition.iStepSize, psCommon->sTransition.u32Time, bOnOff);

        if(psCommon->sTransition.u32Time > 0)
            {
            psCommon->sTransition.iCurrentLevel += psCommon->sTransition.iStepSize;
            psCommon->sTransition.iCurrentLevel = MAX(CLD_LEVELCONTROL_MIN_LEVEL * 100, psCommon->sTransition.iCurrentLevel);
            psCommon->sTransition.u32Time--;
            }
        else
        {
            psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_TRANSITION_OFF;
            psCommon->sTransition.u32Time = 10;
            psCommon->sTransition.iStepSize = (psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
        }
        break;
        
    default:
        break;

        }


    if(psCommon->sTransition.bWithOnOff == TRUE)
    {

        if(psCommon->sTransition.iCurrentLevel > (CLD_LEVELCONTROL_MIN_LEVEL * 100))
        {
            if(bOnOff == FALSE)
            {
                #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
                    eCLD_OnOffSetState(u8SourceEndPointId, TRUE);
                #endif
    }
        }
        else
        {
            if(bOnOff == TRUE)
            {
                #if (defined CLD_ONOFF) && (defined ONOFF_SERVER)
                    eCLD_OnOffSetState(u8SourceEndPointId, FALSE);
                #endif
            }
        }

    }


    /* Update attributes */
#ifdef CLD_LEVELCONTROL_ATTR_REMAINING_TIME
    psSharedStruct->u16RemainingTime = psCommon->sTransition.u32Time;
#endif

#ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
    if(psCommon->sTransition.u16TargetFrequency < CLD_LEVELCONTROL_MIN_FREQ)
      psCommon->sTransition.u16TargetFrequency = CLD_LEVELCONTROL_MIN_FREQ;
    if (psCommon->sTransition.u16TargetFrequency > CLD_LEVELCONTROL_MAX_FREQ)
      psCommon->sTransition.u16TargetFrequency = CLD_LEVELCONTROL_MAX_FREQ;
    psSharedStruct->u16CurrentFrequency = psCommon->sTransition.u16TargetFrequency;
#endif

    psSharedStruct->u8CurrentLevel = psCommon->sTransition.iCurrentLevel / 100;

    #if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER) && (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        /* Find pointers to cluster */
        if(bGenerateEvent)
        {
            eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psColourClusterInstance, (void*)&psColourCommon);
            if(eStatus == E_ZCL_SUCCESS)
            {
                psColourSharedStruct = (tsCLD_ColourControl*)psColourClusterInstance->pvEndPointSharedStructPtr;
                DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Mod=%d Level=%d MoredTolevl=%d Options %d", psColourCommon->eColourMode,psSharedStruct->u8CurrentLevel, psColourSharedStruct->u16CoupleColourTempToLevelMinMired,psSharedStruct->u8Options);
                if((psColourCommon->eColourMode == E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE) && 
                    (psSharedStruct->u8Options & LEVELCONTROL_COUPLE_COLOUR_TEMP_TO_LEVEL_BIT))
                {
                    /*The CoupleColorTempToLevelMinMireds attribute specifies a lower bound on the value of the ColorTemperatureMireds 
                    attribute for the purposes of coupling the ColorTemperatureMireds attribute to the CurrentLevel attribute when 
                    the CoupleColorTempToLevel bit of the Options attribute of the  Level Control cluster is equal to 1. */
                    u16MinMireds = MAX(psColourSharedStruct->u16ColourTemperatureMiredPhyMin, psColourSharedStruct->u16CoupleColourTempToLevelMinMired);

                    /* Just take the max value (lowest colour temperature and should correspond to the minimum value of CurrentLevel) */
                    u16MaxMireds = psColourSharedStruct->u16ColourTemperatureMiredPhyMax;

                    /* Calculate the Mired range */
                    fMiredsRange = (float)u16MaxMireds - (float)u16MinMireds;

                    /* Calculate the Level range */
                    fLevelRange = (float)CLD_LEVELCONTROL_MAX_LEVEL - (float)CLD_LEVELCONTROL_MIN_LEVEL;
                    
                    /* Work out the level as a value between 0 & 1 */
                    fLevel = (((float)psSharedStruct->u8CurrentLevel - (float)CLD_LEVELCONTROL_MIN_LEVEL)/ fLevelRange);
                    
                    /* Remember, when CurrentLevel is at its minimum, the Mireds value needs to be at its maximum */
                    psColourSharedStruct->u16ColourTemperatureMired = (uint16)((float)u16MaxMireds - (fMiredsRange * fLevel));

                    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: u16ColourTemperatureMired=%d ",psColourSharedStruct->u16ColourTemperatureMired);
                }
            }
        }        
    #endif
    
    if(bGenerateEvent == TRUE)
    {

        /* Set up a callback to let the user know that an update event occurred */
        sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
        sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
        sZCL_CallBackEvent.pZPSevent            = NULL;
        sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;

    /* Call callback to let user know an update occurred */
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
#if (defined CLD_SCENES) && (defined SCENES_SERVER)
        vCLD_ScenesUpdateSceneValid( psEndPointDefinition);
#endif
    }

    /* release EP mutex */
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlSetOnOffState
 **
 ** DESCRIPTION:
 ** Can be used by other clusters to set the state of the level control cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** bool_t                        bOn                         On or Off
 ** teCLD_OnOff_OffWithEffect   eCLD_OnOff_OffWithEffect    OffWithEffect based on EffectId and EffectVariant
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_LevelControlSetOnOffState(
                                                uint8 u8SourceEndPointId,
                                                bool_t bOn,
                                                teCLD_OnOff_OffWithEffect eCLD_OnOff_OffWithEffect)
{

    teZCL_Status eStatus;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_LevelControl *psSharedStruct;
    tsCLD_LevelControlCustomDataStructure *psCommon;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_LEVEL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: No cluster");
        return eStatus;
    }

    /* Get pointer to level control cluster shared struct */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* Save current level */
    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: SetOnOff=%i Saving curr level (%d)", bOn, psCommon->sTransition.iCurrentLevel);

    if(bOn == TRUE)
    {
        psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_TRANSITION_ON;

#ifdef CLD_LEVELCONTROL_ATTR_ON_LEVEL
        /* If OnLevel is a valid value, use it as the target value, else use previous level */
        if(psSharedStruct->u8OnLevel != 0xff)
        {
            psCommon->sTransition.iTargetLevel = (int)psSharedStruct->u8OnLevel * 100;
        }
        else
#endif
        {
            /* Set target level to previous level */
            psCommon->sTransition.iTargetLevel = psCommon->sTransition.iCurrentLevel;
        }
        psCommon->sTransition.iCurrentLevel = CLD_LEVELCONTROL_MIN_LEVEL * 100;
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " ON");

/* Set the Transition Time */       
#ifdef CLD_LEVELCONTROL_ATTR_ON_TRANSITION_TIME
        if (psSharedStruct->u16OnTransitionTime == 0xFFFF)
        {
        #ifdef CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME
            psCommon->sTransition.u32Time = psSharedStruct->u16OnOffTransitionTime;
        #else
            psCommon->sTransition.u32Time = 0;
        #endif
        }
        else
        {
            psCommon->sTransition.u32Time = psSharedStruct->u16OnTransitionTime;
        }
#elif (defined CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME)
        psCommon->sTransition.u32Time = psSharedStruct->u16OnOffTransitionTime;
#else
        psCommon->sTransition.u32Time = 0;
#endif
    }
    else
    {
        /* If an Off transition is already ongoing; avoid interruption */
        if((psCommon->sTransition.eTransition == E_CLD_LEVELCONTROL_TRANSITION_OFF) ||
           (psCommon->sTransition.eTransition == E_CLD_LEVELCONTROL_TRANSITION_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF) ||
           (psCommon->sTransition.eTransition == E_CLD_LEVELCONTROL_TRANSITION_OFF_WITH_EFFECT_DIM_UP_FADE_OFF))
        {
            DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlSetOnOffState: avoiding interruption");
            // release EP
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            return 0;
        }
        /* Setting eTransition based on eCLD_OnOff_OffWithEffect */
        switch(eCLD_OnOff_OffWithEffect)
        {
            case CLD_ONOFF_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF:
                psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_TRANSITION_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF;
                break;
            case CLD_ONOFF_OFF_WITH_EFFECT_DIM_UP_FADE_OFF:
                psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_TRANSITION_OFF_WITH_EFFECT_DIM_UP_FADE_OFF;
                break;
            default:
                psCommon->sTransition.eTransition = E_CLD_LEVELCONTROL_TRANSITION_OFF;
                break;
        }
    
#ifdef CLD_LEVELCONTROL_ATTR_ON_LEVEL
        /* If OnLevel is valid, use it as the previous level */
        if(psSharedStruct->u8OnLevel != 0xff)
        {
            psCommon->sTransition.iPreviousLevel = (int)psSharedStruct->u8OnLevel * 100;
        }
        else
#endif
        {
            /* Save level */
            psCommon->sTransition.iPreviousLevel = psCommon->sTransition.iCurrentLevel;
        }
        psCommon->sTransition.iTargetLevel = CLD_LEVELCONTROL_MIN_LEVEL * 100;

        /* Set the Transition Time */       
#ifdef CLD_LEVELCONTROL_ATTR_OFF_TRANSITION_TIME
        if (psSharedStruct->u16OffTransitionTime == 0xFFFF)
        {
        #ifdef CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME
            psCommon->sTransition.u32Time = psSharedStruct->u16OnOffTransitionTime;
        #else
            psCommon->sTransition.u32Time = 0;
        #endif
        }else{
            psCommon->sTransition.u32Time = psSharedStruct->u16OffTransitionTime;
        }
#elif (defined CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME)
        psCommon->sTransition.u32Time = psSharedStruct->u16OnOffTransitionTime;
#else
        psCommon->sTransition.u32Time = 0;
#endif
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " OFF");

    }

    /* Set transition time; By passing OnOffTransitionTime for OffWithEffect command */
    switch(eCLD_OnOff_OffWithEffect)
    {
        case CLD_ONOFF_OFF_WITH_EFFECT_FADE_OFF:
            /* Fade to off in 0.8 seconds (6.6.1.4.4.2 ) */
            psCommon->sTransition.u32Time = 8;
            break;
        case CLD_ONOFF_OFF_WITH_EFFECT_NO_FADE:
            /* No fade (6.6.1.4.4.2) */
            psCommon->sTransition.u32Time = 0;
            break;
        case CLD_ONOFF_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF:
            /* 50% dim down in 0.8 seconds then fade to off in 12 seconds (6.6.1.4.4.2) */
            /* Since 12 seconds seems too long for the effect; keeping 4 seconds */
            psCommon->sTransition.u32Time = 8;
            break;
        case CLD_ONOFF_OFF_WITH_EFFECT_DIM_UP_FADE_OFF:
            /* 20% dim up in 0.5s then fade to off in 1 second (6.6.1.4.4.2) */
            psCommon->sTransition.u32Time = 5;
            break;
        default:
            break;
    }

    /* Calculate step size */
    if(psCommon->sTransition.u32Time > 0)
    {
        switch(eCLD_OnOff_OffWithEffect)
        {
            case CLD_ONOFF_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF:
                psCommon->sTransition.iStepSize = (((psCommon->sTransition.iCurrentLevel - psCommon->sTransition.iTargetLevel)/2) - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
                break;
            case CLD_ONOFF_OFF_WITH_EFFECT_DIM_UP_FADE_OFF:
                psCommon->sTransition.iStepSize = ((psCommon->sTransition.iCurrentLevel + (((CLD_LEVELCONTROL_MAX_LEVEL * 100) - psCommon->sTransition.iCurrentLevel)/5)) - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
                break;
            default:
                psCommon->sTransition.iStepSize = (psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
                break;
        }
 
    }
    else
    {
        psCommon->sTransition.iStepSize = psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel;
    }


    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Prev:%d Curr:%d Target:%d Time%d Step:%i",
            psCommon->sTransition.iPreviousLevel,
            psCommon->sTransition.iCurrentLevel,
            psCommon->sTransition.iTargetLevel,
            psCommon->sTransition.u32Time,
            psCommon->sTransition.iStepSize);

    /* Update current level */
    psSharedStruct->u8CurrentLevel = psCommon->sTransition.iCurrentLevel / 100;

    /* Clear "With On/Off flag */
    psCommon->sTransition.bWithOnOff = FALSE;

    /* Set up a callback to let the user know that an update event occurred */
    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = NULL;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    /* Do an update quickly if time = 0 */
    if(psCommon->sTransition.u32Time == 0)
    {
        eCLD_LevelControlUpdate(u8SourceEndPointId);
    }

    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlSetLevel
 **
 ** DESCRIPTION:
 ** Can be used by other clusters or the local application to change the level
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** uint8                       u8Level                     Level
 ** uint16                      u16TransitionTime           Transition time
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_LevelControlSetLevel(
        uint8                   u8SourceEndPointId,
        uint8                   u8Level,
        uint16                  u16TransitionTime)
{

    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_LevelControlCustomDataStructure *psCommon;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_LEVEL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: No cluster");
        return eStatus;
    }

     // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Set Level=%i Time=%d", u8Level, u16TransitionTime);

    /* Fill in transition details */
    psCommon->sTransition.eTransition   = E_CLD_LEVELCONTROL_TRANSITION_MOVE_TO_LEVEL;
    psCommon->sTransition.iTargetLevel =  CLAMP((int)(u8Level * 100), CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
    if(u16TransitionTime > 0)
        psCommon->sTransition.iStepSize     = (psCommon->sTransition.iCurrentLevel - psCommon->sTransition.iTargetLevel) / (int)u16TransitionTime;
    else
        psCommon->sTransition.iStepSize     = (psCommon->sTransition.iCurrentLevel - psCommon->sTransition.iTargetLevel);
    
    psCommon->sTransition.u32Time       = u16TransitionTime;

    DBG_vPrintf(TRACE_LEVEL_CONTROL, " Level=%d Time=%d Step*100=%d", u8Level, u16TransitionTime, psCommon->sTransition.iStepSize);


    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlStopTransition
 **
 ** DESCRIPTION:
 ** Can be used by other clusters or the local application to stop the ongoing transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlStopTransition(
        uint8                   u8SourceEndPointId)
{

    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_LevelControlCustomDataStructure *psCommon;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_LEVEL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: No cluster");
        return eStatus;
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    if(psCommon->sTransition.u32Time)
    {
        psCommon->sTransition.u32Time = 0x0000;
    }

    /* Doing a quick update */
    eCLD_LevelControlUpdate(u8SourceEndPointId);
        
    // release EP
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlGetLevel
 **
 ** DESCRIPTION:
 ** Can be used by other clusters or the local application to get the level
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** uint8*                      pu8Level                    Level
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_LevelControlGetLevel(
        uint8                   u8SourceEndPointId,
        uint8                   *pu8Level)
{

    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_LevelControl *psSharedStruct;
    tsCLD_LevelControlCustomDataStructure *psCustomDataStructure;

    if(pu8Level == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_LEVEL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Failed to find cluster");
        return eStatus;
    }

    /* Get pointer to level control cluster shared struct */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;

    *pu8Level = psSharedStruct->u8CurrentLevel;

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlClusterIsPresent
 **
 ** DESCRIPTION:
 ** Can be used by other clusters or the local application to check if
 ** a level control cluster exists on the specified endpoint
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_LevelControlClusterIsPresent(
        uint8                   u8SourceEndPointId)
{

    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_LevelControlCustomDataStructure *psCustomDataStructure;

    /* Find pointers to cluster */
    return eZCL_FindCluster(GENERAL_CLUSTER_ID_LEVEL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);

}
#ifdef LEVEL_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlSceneEventHandler
 **
 ** DESCRIPTION:
 ** Handles events generated by a scenes cluster (if present)
 **
 ** PARAMETERS:               Name                        Usage
 ** teZCL_SceneEvent          eEvent                      Scene Event
 ** tsZCL_EndPointDefinition *psEndPointDefinition        End Point Definition
 ** tsZCL_ClusterInstance    *psClusterInstance           Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_LevelControlSceneEventHandler(
                        teZCL_SceneEvent            eEvent,
                        tsZCL_EndPointDefinition   *psEndPointDefinition,
                        tsZCL_ClusterInstance      *psClusterInstance)
{

    tsZCL_CallBackEvent sZCL_CallBackEvent;

    /* Get pointer to custom data structure */
    tsCLD_LevelControlCustomDataStructure *psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get pointer to level control cluster shared struct */
    tsCLD_LevelControl *psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;


    switch(eEvent)
    {

    case E_ZCL_SCENE_EVENT_SAVE:
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Scene Event: Save");
        break;

    case E_ZCL_SCENE_EVENT_RECALL:
        DBG_vPrintf(TRACE_LEVEL_CONTROL, "\nLC: Scene Event: Recall");
        psCommon->sTransition.bWithOnOff = FALSE;
        psCommon->sTransition.eMode = E_CLD_LEVELCONTROL_TRANSITION_NONE;
        psCommon->sTransition.iCurrentLevel = psSharedStruct->u8CurrentLevel * 100;
        psCommon->sTransition.iTargetLevel = psCommon->sTransition.iCurrentLevel;
#ifdef CLD_LEVELCONTROL_ATTR_REMAINING_TIME
        psCommon->sTransition.u32Time = psSharedStruct->u16RemainingTime;
#else
        psCommon->sTransition.u32Time = 0;
#endif


        /* Inform the application that the cluster has just been updated */
        sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
        sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
        sZCL_CallBackEvent.pZPSevent = NULL;
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_UPDATE;

        // call user
        psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

        break;

    }

    return E_ZCL_SUCCESS;
}
#endif
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

