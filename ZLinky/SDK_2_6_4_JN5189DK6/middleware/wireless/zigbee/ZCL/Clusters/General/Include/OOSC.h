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
 * MODULE:             On/Off Switch Configuration Cluster
 *
 * COMPONENT:          OOSC.h
 *
 * DESCRIPTION:        Header for On/Off Switch Configuration Cluster
 *
 *****************************************************************************/

#ifndef OOSC_H
#define OOSC_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_ONOFF_SWITCH_CONFIGURATION   0x0007

#ifndef CLD_OOSC_CLUSTER_REVISION
    #define CLD_OOSC_CLUSTER_REVISION                         1
#endif 

#ifndef CLD_OOSC_FEATURE_MAP
    #define CLD_OOSC_FEATURE_MAP                              0
#endif 
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* On/Off switch configuration attribute ID's (3.9.2.2) */
typedef enum 
{
    E_CLD_OOSC_ATTR_ID_SWITCH_TYPE          = 0x0000,   /* Mandatory */
    E_CLD_OOSC_ATTR_ID_SWITCH_ACTIONS       = 0x0010,   /* Mandatory */
} teCLD_OOSC_ClusterID;


/* On/Off switch types */
typedef enum 
{
    E_CLD_OOSC_TYPE_TOGGLE,
    E_CLD_OOSC_TYPE_MOMENTARY,
    E_CLD_OOSC_TYPE_MULTI_FUNCTION
} teCLD_OOSC_SwitchType;


/* On/Off switch actions */
typedef enum 
{
    E_CLD_OOSC_ACTION_S2ON_S1OFF,
    E_CLD_OOSC_ACTION_S2OFF_S1ON,
    E_CLD_OOSC_ACTION_TOGGLE
} teCLD_OOSC_SwitchAction;


/* On/Off Switch Configuration Cluster */
typedef struct
{
#ifdef OOSC_SERVER    
    zenum8                  eSwitchType;                /* Mandatory */
    zenum8                  eSwitchActions;             /* Mandatory */
#endif    
    zbmap32                 u32FeatureMap;
    
    zuint16                 u16ClusterRevision;
} tsCLD_OOSC;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_OOSCCreateOnOffSwitchConfig(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_OOSC;
extern uint8 au8OOSCAttributeControlBits[];
extern const tsZCL_AttributeDefinition asCLD_OOSCClusterAttributeDefinitions[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* OOSC_H */
