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
 * MODULE:             Fan Control Cluster
 *
 * COMPONENT:          FanControl.h
 *
 * DESCRIPTION:        Fan Control Cluster Definition
 *
 ****************************************************************************/

#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*             Fan Control Cluster - Optional Attributes                    */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the time cluster.                                          */
/****************************************************************************/
    /* None at the moment */
/* End of optional attributes */

#ifndef CLD_FAN_CONTROL_CLUSTER_REVISION
#define CLD_FAN_CONTROL_CLUSTER_REVISION                (1)
#endif

/* Cluster ID's */
#define HVAC_CLUSTER_ID_FAN_CONTROL                     (0x0202)

#define CLD_FAN_CONTROL_NUMBER_OF_MANDATORY_ATTRIBUTE   (2)
#define CLD_FAN_CONTROL_NUMBER_OF_OPTIONAL_ATTRIBUTE    (0)

#define CLD_FAN_CONTROL_MAX_NUMBER_OF_ATTRIBUTE       \
    CLD_FAN_CONTROL_NUMBER_OF_OPTIONAL_ATTRIBUTE +    \
    CLD_FAN_CONTROL_NUMBER_OF_MANDATORY_ATTRIBUTE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Fan Control attribute set attribute ID's (6.4.2.2) */
typedef enum 
{
    E_CLD_FAN_CONTROL_ATTR_ID_FAN_MODE              = 0x0000,  /* Mandatory */
    E_CLD_FAN_CONTROL_ATTR_ID_FAN_MODE_SEQUENCE,               /* Mandatory */
} teCLD_FanControl_AttributeID;

/* FanMode Attribute Values (6.4.2.2.1 ) */
typedef enum 
{
    E_CLD_FC_FAN_MODE_OFF       = 0x00,
    E_CLD_FC_FAN_MODE_LOW,      //0x01
    E_CLD_FC_FAN_MODE_MEDIUM,   //0x02
    E_CLD_FC_FAN_MODE_HIGH,     //0x03
    E_CLD_FC_FAN_MODE_ON,       //0x04
    E_CLD_FC_FAN_MODE_AUTO,     //0x05
    E_CLD_FC_FAN_MODE_SMART,    //0x06
} teCLD_FC_FanMode;

/* FanModeSequence Attribute Values (6.4.2.2.2 ) */
typedef enum 
{
    E_CLD_FC_FAN_MODE_SEQUENCE_LOW_MED_HIGH         = 0x00,
    E_CLD_FC_FAN_MODE_SEQUENCE_LOW_HIGH,            //0x01
    E_CLD_FC_FAN_MODE_SEQUENCE_LOW_MED_HIGH_AUTO,   //0x02
    E_CLD_FC_FAN_MODE_SEQUENCE_LOW_HIGH_AUTO,       //0x03
    E_CLD_FC_FAN_MODE_SEQUENCE_ON_AUTO,             //0x04
} teCLD_FC_FanModeSequence;

/* Fan Control Cluster */
typedef struct
{
    #ifdef FAN_CONTROL_SERVER    
        zenum8  e8FanMode;
        zenum8  e8FanModeSequence;
    #endif

    zuint16 u16ClusterRevision;
} tsCLD_FanControl;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_CreateFanControl(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_FanControl;
extern const tsZCL_AttributeDefinition asCLD_FanControlClusterAttributeDefinitions[];
extern uint8 au8FanControlClusterAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* TIME_H */
