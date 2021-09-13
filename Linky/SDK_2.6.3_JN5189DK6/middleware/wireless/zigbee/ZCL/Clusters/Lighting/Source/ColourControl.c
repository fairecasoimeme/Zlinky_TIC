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
 * MODULE:             Colour Control Cluster
 *
 * COMPONENT:          ColourControl.c
 *
 * DESCRIPTION:        Colour Control cluster definition
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
#include "ColourControl.h"
#include "ColourControl_internal.h"
#include "dbg.h"
#ifdef SCENES_SERVER
#include "Scenes.h"
#endif

#ifdef DEBUG_CLD_COLOUR_CONTROL
#define TRACE_COLOUR_CONTROL    TRUE
#else
#define TRACE_COLOUR_CONTROL    FALSE
#endif

#ifdef DEBUG_CLD_COLOUR_CONTROL_UPDATES
#define TRACE_COLOUR_CONTROL_UPDATES    TRUE
#else
#define TRACE_COLOUR_CONTROL_UPDATES    FALSE
#endif

#ifdef CLD_COLOUR_CONTROL

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_COLOUR_CONTROL) && !defined(COLOUR_CONTROL_SERVER) && !defined(COLOUR_CONTROL_CLIENT)
#error You Must Have either COLOUR_CONTROL_SERVER and/or COLOUR_CONTROL_CLIENT defined in zcl_options.h
#endif

/* 3 way MAX and MIN functions used in colour space conversion routines */
#define MIN3(X,Y,Z)  ((Y) <= (Z) ? \
                     ((X) <= (Y) ? (X) : (Y)) \
                     : \
                     ((X) <= (Z) ? (X) : (Z)))

#define MAX3(X,Y,Z)  ((Y) >= (Z) ? \
                     ((X) >= (Y) ? (X) : (Y)) \
                     : \
                     ((X) >= (Z) ? (X) : (Z)))

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/*

Hue                 = CurrentHue * 360 / 254            (degrees)
Saturation          = CurrentSaturation / 254
x                   = CurrentX / 65536
y                   = CurrentY / 65536
Colour Temperature  = ColourTemperature / 1,000,000

*/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef COLOUR_CONTROL_SERVER
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToHueTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveHueTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleStepHueTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleStepSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToHueAndSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);
#endif

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToColourTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveColourTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE teZCL_Status eCLD_ColourControlHandleStepColourTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToColourTemperatureTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);
#endif
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED) //#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
PRIVATE  teZCL_Status eCLD_ColourControlHandleColourLoopTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);
#endif         

PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveColourTemperatureTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE  teZCL_Status eCLD_ColourControlHandleStepColourTemperatureTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE  teZCL_Status eCLD_ColourControlHandleSceneTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon);

PRIVATE  teZCL_Status eCLD_ColourControlSceneEventHandler(
        teZCL_SceneEvent                        eEvent,
        tsZCL_EndPointDefinition                *psEndPointDefinition,
        tsZCL_ClusterInstance                   *psClusterInstance);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_ColourControlClusterCommandDefinitions[] = {

    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
        {E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE,                      E_ZCL_CF_RX},
        {E_CLD_COLOURCONTROL_CMD_MOVE_HUE,                         E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_STEP_HUE,                         E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_MOVE_TO_SATURATION,               E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_MOVE_SATURATION,                  E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_STEP_SATURATION,                  E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE_AND_SATURATION,       E_ZCL_CF_RX},/* Optional */
    #endif
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)
        {E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR,                   E_ZCL_CF_RX},/* Mandatory */
        {E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR,                      E_ZCL_CF_RX},/* Mandatory */
        {E_CLD_COLOURCONTROL_CMD_STEP_COLOUR,                      E_ZCL_CF_RX},/* Mandatory */
    #endif
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
        {E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE,                E_ZCL_CF_RX}, 
        {E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_HUE,                   E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_ENHANCED_STEP_HUE,                   E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE_AND_SATURATION, E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_COLOUR_LOOP_SET,                     E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_STOP_MOVE_STEP,                      E_ZCL_CF_RX},/* Optional */
    #endif
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        {E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR_TEMPERATURE,          E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR_TEMPERATURE,             E_ZCL_CF_RX},/* Optional */
        {E_CLD_COLOURCONTROL_CMD_STEP_COLOUR_TEMPERATURE,             E_ZCL_CF_RX}/* Optional */
    #endif
    };
#endif

const tsZCL_AttributeDefinition asCLD_ColourControlClusterAttributeDefinitions[] = {
#ifdef COLOUR_CONTROL_SERVER
        /* Colour information attribute set attribute ID (5.2.2.2.1) */
    #ifdef CLD_COLOURCONTROL_ATTR_CURRENT_HUE
        {E_CLD_COLOURCONTROL_ATTR_CURRENT_HUE,              (E_ZCL_AF_RD|E_ZCL_AF_RP),              E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8CurrentHue), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION
        {E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION,      (E_ZCL_AF_RD|E_ZCL_AF_RP|E_ZCL_AF_SE),   E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8CurrentSaturation), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_REMAINING_TIME
        {E_CLD_COLOURCONTROL_ATTR_REMAINING_TIME,           E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16RemainingTime), 0},
    #endif

        {E_CLD_COLOURCONTROL_ATTR_CURRENT_X,                (E_ZCL_AF_RD|E_ZCL_AF_SE|E_ZCL_AF_RP),  E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16CurrentX), 0},
        {E_CLD_COLOURCONTROL_ATTR_CURRENT_Y,                (E_ZCL_AF_RD|E_ZCL_AF_SE|E_ZCL_AF_RP),  E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16CurrentY), 0},

    #ifdef CLD_COLOURCONTROL_ATTR_DRIFT_COMPENSATION
        {E_CLD_COLOURCONTROL_ATTR_DRIFT_COMPENSATION,       E_ZCL_AF_RD,                            E_ZCL_ENUM8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8DriftCompensation), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COMPENSATION_TEXT
        {E_CLD_COLOURCONTROL_ATTR_COMPENSATION_TEXT,        E_ZCL_AF_RD,                            E_ZCL_CSTRING,  (uint32)(&((tsCLD_ColourControl*)(0))->sCompensationText), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED,(E_ZCL_AF_RD|E_ZCL_AF_RP|E_ZCL_AF_SE),   E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourTemperatureMired), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_MODE,             E_ZCL_AF_RD,                             E_ZCL_ENUM8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8ColourMode), 0},
    #endif

        {E_CLD_COLOURCONTROL_ATTR_OPTIONS,                (E_ZCL_AF_RD|E_ZCL_AF_WR),                E_ZCL_BMAP8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Options),0},         /* Mandatory */
        
        /* Defined Primaries Information attribute attribute ID's set (5.2.2.2.2) */
    #ifdef CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES
        {E_CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8NumberOfPrimaries), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_1_X
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_1_X,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary1X), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_1_Y
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_1_Y,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary1Y), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_1_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_1_INTENSITY,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Primary1Intensity), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_2_X
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_2_X,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary2X), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_2_Y
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_2_Y,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary2Y), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_2_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_2_INTENSITY,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Primary2Intensity), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_3_X
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_3_X,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary3X), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_3_Y
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_3_Y,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary3Y), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_3_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_3_INTENSITY,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Primary3Intensity), 0},
    #endif

        /* Additional Defined Primaries Information attribute attribute ID's set (5.2.2.2.3) */
    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_4_X
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_4_X,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary4X), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_4_Y
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_4_Y,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary4Y), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_4_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_4_INTENSITY,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Primary4Intensity), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_5_X
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_5_X,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary5X), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_5_Y
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_5_Y,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary5Y), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_5_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_5_INTENSITY,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Primary5Intensity), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_6_X
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_6_X,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary6X), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_6_Y
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_6_Y,              E_ZCL_AF_RD,                            E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16Primary6Y), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_6_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_PRIMARY_6_INTENSITY,      E_ZCL_AF_RD,                            E_ZCL_UINT8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8Primary6Intensity), 0},
    #endif

        /* Defined Colour Points Settings attribute ID's set (5.2.2.2.4) */
    #ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_X
        {E_CLD_COLOURCONTROL_ATTR_WHITE_POINT_X,            (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16WhitePointX), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y
        {E_CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y,            (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16WhitePointY), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourPointRX), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourPointRY), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_INTENSITY, (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT8,   (uint32)(&((tsCLD_ColourControl*)(0))->u8ColourPointRIntensity), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourPointGX), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourPointGY), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_INTENSITY, (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT8,   (uint32)(&((tsCLD_ColourControl*)(0))->u8ColourPointGIntensity), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourPointBX), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y,         (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourPointBY), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_INTENSITY
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_INTENSITY, (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT8,   (uint32)(&((tsCLD_ColourControl*)(0))->u8ColourPointBIntensity), 0},
    #endif

    /* Colour information attribute set attribute ID's (5.2.2.2.1) */
    #ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE
        {E_CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE,     (E_ZCL_AF_RD|E_ZCL_AF_SE),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16EnhancedCurrentHue), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
        {E_CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE,     (E_ZCL_AF_RD),                          E_ZCL_ENUM8,   (uint32)(&((tsCLD_ColourControl*)(0))->u8EnhancedColourMode), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE,       (E_ZCL_AF_RD|E_ZCL_AF_SE),              E_ZCL_UINT8,   (uint32)(&((tsCLD_ColourControl*)(0))->u8ColourLoopActive), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION,    (E_ZCL_AF_RD|E_ZCL_AF_SE),              E_ZCL_UINT8,   (uint32)(&((tsCLD_ColourControl*)(0))->u8ColourLoopDirection), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME,         (E_ZCL_AF_RD|E_ZCL_AF_SE),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourLoopTime), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE,   (E_ZCL_AF_RD),                          E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourLoopStartEnhancedHue), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_STORED_ENHANCED_HUE
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_STORED_ENHANCED_HUE,  (E_ZCL_AF_RD),                          E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourLoopStoredEnhancedHue), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_CAPABILITIES
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_CAPABILITIES,              (E_ZCL_AF_RD),                          E_ZCL_BMAP16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourCapabilities), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN, (E_ZCL_AF_RD),                          E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourTemperatureMiredPhyMin), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX
        {E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX, (E_ZCL_AF_RD),                          E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ColourTemperatureMiredPhyMax), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED
        {E_CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED, (E_ZCL_AF_RD),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16CoupleColourTempToLevelMinMired), 0},
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED
        {E_CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED, (E_ZCL_AF_RD|E_ZCL_AF_WR),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16StartupColourTemperatureMired), 0},
    #endif

#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_BMAP32,   (uint32)(&((tsCLD_ColourControl*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_UINT16,   (uint32)(&((tsCLD_ColourControl*)(0))->u16ClusterRevision), 0},   /* Mandatory  */
    
    #if (defined COLOUR_CONTROL_SERVER) && (defined CLD_COLOURCONTROL_ATTR_ATTRIBUTE_REPORTING_STATUS)
        {E_CLD_GLOBAL_ATTR_ID_ATTRIBUTE_REPORTING_STATUS,           (E_ZCL_AF_RD|E_ZCL_AF_GA),              E_ZCL_ENUM8,    (uint32)(&((tsCLD_ColourControl*)(0))->u8AttributeReportingStatus), 0},  /* Optional */
    #endif        
};

#ifdef COLOUR_CONTROL_SERVER
    /* List of attributes in the scene extension table */
    tsZCL_SceneExtensionTable sCLD_ColourControlSceneExtensionTable =
    {
            eCLD_ColourControlSceneEventHandler,
            8,
           {
            {E_CLD_COLOURCONTROL_ATTR_CURRENT_X,                E_ZCL_UINT16},
            {E_CLD_COLOURCONTROL_ATTR_CURRENT_Y,                E_ZCL_UINT16},
            {E_CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE,     E_ZCL_UINT16},
            {E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION,       E_ZCL_UINT8 },
            {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE,       E_ZCL_UINT8 },
            {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION,    E_ZCL_UINT8 },
            {E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME,         E_ZCL_UINT16},
            {E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED, E_ZCL_UINT16},
            }
    };

    tsZCL_ClusterDefinition sCLD_ColourControl = {
            LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_ColourControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_ColourControlClusterAttributeDefinitions,
            &sCLD_ColourControlSceneExtensionTable
            #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
                ,
                (sizeof(asCLD_ColourControlClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
                (tsZCL_CommandDefinition*)asCLD_ColourControlClusterCommandDefinitions 
            #endif        
    };

#else
    tsZCL_ClusterDefinition sCLD_ColourControl = {
            LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
            FALSE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_ColourControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_ColourControlClusterAttributeDefinitions,
            NULL
            #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
                ,
                (sizeof(asCLD_ColourControlClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
                (tsZCL_CommandDefinition*)asCLD_ColourControlClusterCommandDefinitions 
            #endif
    };

#endif
uint8 au8ColourControlAttributeControlBits[(sizeof(asCLD_ColourControlClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCreateColourControl
 **
 ** DESCRIPTION:
 ** Creates a Colour Control cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCreateColourControl(
        tsZCL_ClusterInstance                   *psClusterInstance,
        bool_t                                  bIsServer,
        tsZCL_ClusterDefinition                 *psClusterDefinition,
        void                                    *pvEndPointSharedStructPtr,
        uint8                                   *pu8AttributeControlBits,
        tsCLD_ColourControlCustomDataStructure  *psCustomDataStructure)
{

    tsCLD_ColourControl *psSharedStructPtr = (tsCLD_ColourControl*)pvEndPointSharedStructPtr;
    
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
                                   (tfpZCL_ZCLCustomcallCallBackFunction)eCLD_ColourControlCommandHandler);     

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
    
    /* Last received command (for update function) */
    psCustomDataStructure->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    /* Set master colour mode value. Shared struct value is just a copy of this */
    psCustomDataStructure->eColourMode = E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y;

    /* Set attribute default values */
        if(psSharedStructPtr != NULL)
        {
        #ifdef COLOUR_CONTROL_SERVER    
            psSharedStructPtr->u16CurrentX = 0x616b;
            psSharedStructPtr->u16CurrentY = 0x607d;

        // set default state
        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
            #ifdef CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED
                if((psSharedStructPtr->u16StartupColourTemperatureMired >= 0) && (psSharedStructPtr->u16StartupColourTemperatureMired <= 0xFFEF))
                {
                    psSharedStructPtr->u16ColourTemperatureMired = psSharedStructPtr->u16StartupColourTemperatureMired;
                }
            #else
                psSharedStructPtr->u16ColourTemperatureMired = 0x00fa;
            #endif
            
            #if (defined CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MIN)
                if(psSharedStructPtr->u16ColourTemperatureMired < CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MIN)
                {
                    psSharedStructPtr->u16ColourTemperatureMired = CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MIN;
                }
            #endif
            #if (defined CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX)
                if(psSharedStructPtr->u16ColourTemperatureMired > CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX)
                {
                    psSharedStructPtr->u16ColourTemperatureMired = CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX;
                }
            #endif
        #endif
        
        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
            psSharedStructPtr->u8ColourMode = E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y;
        #endif

            /* Fill primaries attributes with correct values */
        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X
            psSharedStructPtr->u16ColourPointRX = (uint16)(CLD_COLOURCONTROL_RED_X * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y
            psSharedStructPtr->u16ColourPointRY = (uint16)(CLD_COLOURCONTROL_RED_Y * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X
            psSharedStructPtr->u16ColourPointGX = (uint16)(CLD_COLOURCONTROL_GREEN_X * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y
            psSharedStructPtr->u16ColourPointGY = (uint16)(CLD_COLOURCONTROL_GREEN_Y * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X
            psSharedStructPtr->u16ColourPointBX = (uint16)(CLD_COLOURCONTROL_BLUE_X * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y
            psSharedStructPtr->u16ColourPointBY = (uint16)(CLD_COLOURCONTROL_BLUE_Y * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_X
            psSharedStructPtr->u16WhitePointX = (uint16)(CLD_COLOURCONTROL_WHITE_X * 65535.0);
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y
            psSharedStructPtr->u16WhitePointY = (uint16)(CLD_COLOURCONTROL_WHITE_Y * 65535.0);
        #endif
        #ifdef CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES
            /* Primaries attributes initialisation */
            psSharedStructPtr->u8NumberOfPrimaries = CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES;
        #endif
        #if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 1)
            psSharedStructPtr->u16Primary1X         = (uint16)(CLD_COLOURCONTROL_PRIMARY_1_X * 65535.0);
            psSharedStructPtr->u16Primary1Y         = (uint16)(CLD_COLOURCONTROL_PRIMARY_1_Y * 65535.0);
            psSharedStructPtr->u8Primary1Intensity  = CLD_COLOURCONTROL_PRIMARY_1_INTENSITY;
        #endif

        #if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 2)
            psSharedStructPtr->u16Primary2X         = (uint16)(CLD_COLOURCONTROL_PRIMARY_2_X * 65535.0);
            psSharedStructPtr->u16Primary2Y         = (uint16)(CLD_COLOURCONTROL_PRIMARY_2_Y * 65535.0);
            psSharedStructPtr->u8Primary2Intensity  = CLD_COLOURCONTROL_PRIMARY_2_INTENSITY;
        #endif

        #if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 3)
            psSharedStructPtr->u16Primary3X         = (uint16)(CLD_COLOURCONTROL_PRIMARY_3_X * 65535.0);
            psSharedStructPtr->u16Primary3Y         = (uint16)(CLD_COLOURCONTROL_PRIMARY_3_Y * 65535.0);
            psSharedStructPtr->u8Primary3Intensity  = CLD_COLOURCONTROL_PRIMARY_3_INTENSITY;
        #endif

        #if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 4)
            psSharedStructPtr->u16Primary4X         = (uint16)(CLD_COLOURCONTROL_PRIMARY_4_X * 65535.0);
            psSharedStructPtr->u16Primary4Y         = (uint16)(CLD_COLOURCONTROL_PRIMARY_4_Y * 65535.0);
            psSharedStructPtr->u8Primary4Intensity  = CLD_COLOURCONTROL_PRIMARY_4_INTENSITY;
        #endif

        #if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 5)
            psSharedStructPtr->u16Primary5X         = (uint16)(CLD_COLOURCONTROL_PRIMARY_5_X * 65535.0);
            psSharedStructPtr->u16Primary5Y         = (uint16)(CLD_COLOURCONTROL_PRIMARY_5_Y * 65535.0);
            psSharedStructPtr->u8Primary5Intensity  = CLD_COLOURCONTROL_PRIMARY_5_INTENSITY;
        #endif

        #if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 6)
            psSharedStructPtr->u16Primary6X         = (uint16)(CLD_COLOURCONTROL_PRIMARY_6_X * 65535.0);
            psSharedStructPtr->u16Primary6Y         = (uint16)(CLD_COLOURCONTROL_PRIMARY_6_Y * 65535.0);
            psSharedStructPtr->u8Primary6Intensity  = CLD_COLOURCONTROL_PRIMARY_6_INTENSITY;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COMPENSATION_TEXT
            psSharedStructPtr->sCompensationText.u8MaxLength = CLD_COLOURCONTROL_COMPENSATION_TEXT_MAX_STRING_LENGTH;
            psSharedStructPtr->sCompensationText.pu8Data = psSharedStructPtr->au8CompensationText;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
            psSharedStructPtr->u8EnhancedColourMode = E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME
            psSharedStructPtr->u16ColourLoopTime = 0x0019;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE
            psSharedStructPtr->u16ColourLoopStartEnhancedHue = 0x2300;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_CAPABILITIES
            psSharedStructPtr->u16ColourCapabilities = CLD_COLOURCONTROL_COLOUR_CAPABILITIES;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN
            psSharedStructPtr->u16ColourTemperatureMiredPhyMin = CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MIN;
        #endif

        #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX
            psSharedStructPtr->u16ColourTemperatureMiredPhyMax = CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX;
        #endif
       
        #ifdef CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED
            psSharedStructPtr->u16CoupleColourTempToLevelMinMired = CLD_COLOURCONTROL_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED;
        #endif
        
        #endif
            psSharedStructPtr->u32FeatureMap = CLD_COLOURCONTROL_FEATURE_MAP;

            psSharedStructPtr->u16ClusterRevision = CLD_COLOURCONTROL_CLUSTER_REVISION;
        }
    
#ifdef COLOUR_CONTROL_SERVER      
    /* Generate XYZ<->RGB conversion matrices */
    eCLD_ColourControlCalculateConversionMatrices(psCustomDataStructure,
                                                  CLD_COLOURCONTROL_RED_X,
                                                  CLD_COLOURCONTROL_RED_Y,
                                                  CLD_COLOURCONTROL_GREEN_X,
                                                  CLD_COLOURCONTROL_GREEN_Y,
                                                  CLD_COLOURCONTROL_BLUE_X,
                                                  CLD_COLOURCONTROL_BLUE_Y,
                                                  CLD_COLOURCONTROL_WHITE_X,
                                                  CLD_COLOURCONTROL_WHITE_Y);
#endif

    /* As this cluster has reportable attributes enable default reporting */
    vZCL_SetDefaultReporting(psClusterInstance);
    
    return E_ZCL_SUCCESS;
}

#ifdef COLOUR_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlSetPrimaries
 **
 ** DESCRIPTION:
 ** Creates the conversion matrices for converting between RGB and XYZ
 ** colour spaces for the given primaries and white point.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          End point ID
 ** float                       fRedX                       X coordinate of Red primary
 ** float                       fRedY                       Y coordinate of Red primary
 ** float                       fGreenX                     X coordinate of Green primary
 ** float                       fGreenY                     Y coordinate of Green primary
 ** float                       fBlueX                      X coordinate of Blue primary
 ** float                       fGreenY                     Y coordinate of Blue primary
 ** float                       fWhiteX                     X coordinate of white point
 ** float                       fWhiteY                     Y coordinate of white point
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlSetPrimaries(
        uint8                                   u8SourceEndPointId,
        float                                   fRedX,
        float                                   fRedY,
        float                                   fGreenX,
        float                                   fGreenY,
        float                                   fBlueX,
        float                                   fBlueY,
        float                                   fWhiteX,
        float                                   fWhiteY)
{

    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    teZCL_Status eStatus;


    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
#if ( defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X || defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y || defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X || \
      defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y || defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X || defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X || \
      defined CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y || defined CLD_COLOURCONTROL_ATTR_WHITE_POINT_X || defined CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y)
    tsCLD_ColourControl *psSharedStructPtr;
    psSharedStructPtr = (tsCLD_ColourControl*)psClusterInstance;
#endif
    /* Fill primaries attributes with correct values */
#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X
    psSharedStructPtr->u16ColourPointRX = (uint16)(fRedX * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y
    psSharedStructPtr->u16ColourPointRY = (uint16)(fRedY * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X
    psSharedStructPtr->u16ColourPointGX = (uint16)(fGreenX * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y
    psSharedStructPtr->u16ColourPointGY = (uint16)(fGreenY * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X
    psSharedStructPtr->u16ColourPointBX = (uint16)(fBlueX * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y
    psSharedStructPtr->u16ColourPointBY = (uint16)(fBlueY * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_X
    psSharedStructPtr->u16WhitePointX = (uint16)(fWhiteX * 65535.0);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y
    psSharedStructPtr->u16WhitePointY = (uint16)(fWhiteY * 65535.0);
#endif


    /* Generate XYZ<->RGB conversion matrices */
    return eCLD_ColourControlCalculateConversionMatrices(psCommon,
                                                         fRedX,
                                                         fRedY,
                                                         fGreenX,
                                                         fGreenY,
                                                         fBlueX,
                                                         fBlueY,
                                                         fWhiteX,
                                                         fWhiteY);

}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlUpdate
 **
 ** DESCRIPTION:
 ** Updates the the state of a colour control cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlUpdate(uint8 u8SourceEndPointId)
{

    teZCL_Status eStatus;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ColourControl *psSharedStruct;
    uint8 u8TempY=0;
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
        uint8 u8TempVal;
    #endif
    u8TempY = u8TempY; //To keep the compiler happy!

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    /* get EP mutex */
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif



    /* If we're in a transition, save previous values in case there is a conversion error later */
    if(psCommon->sTransition.eCommand != E_CLD_COLOURCONTROL_CMD_NONE)
    {
        memcpy(&psCommon->sTransition.sPrevious, &psCommon->sTransition.sCurrent, sizeof(tsCLD_ColourControl_Attributes));
    }


    switch(psCommon->sTransition.eCommand)
    {

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
    case E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE:
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE:
#endif
        eCLD_ColourControlHandleMoveToHueTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_MOVE_HUE:
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_HUE:
#endif
        eCLD_ColourControlHandleMoveHueTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_STEP_HUE:
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_CMD_ENHANCED_STEP_HUE:
#endif
        eCLD_ColourControlHandleStepHueTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_MOVE_TO_SATURATION:
        eCLD_ColourControlHandleMoveToSaturationTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_MOVE_SATURATION:
        eCLD_ColourControlHandleMoveSaturationTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_STEP_SATURATION:
        eCLD_ColourControlHandleStepSaturationTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE_AND_SATURATION:
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE_AND_SATURATION:
#endif
        eCLD_ColourControlHandleMoveToHueAndSaturationTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;
#endif

    case E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR:
        eCLD_ColourControlHandleMoveToColourTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR:
        eCLD_ColourControlHandleMoveColourTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_STEP_COLOUR:
        eCLD_ColourControlHandleStepColourTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
    case E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR_TEMPERATURE:
        eCLD_ColourControlHandleMoveToColourTemperatureTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    case E_CLD_COLOURCONTROL_CMD_COLOUR_LOOP_SET:
        eCLD_ColourControlHandleColourLoopTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;
#endif

    case E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR_TEMPERATURE:
        eCLD_ColourControlHandleMoveColourTemperatureTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_STEP_COLOUR_TEMPERATURE:
        eCLD_ColourControlHandleStepColourTemperatureTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    case E_CLD_COLOURCONTROL_CMD_SCENE_TRANSITION:
        eCLD_ColourControlHandleSceneTransition(psEndPointDefinition, psClusterInstance, psCommon);
        break;

    default:
        /* release EP mutex */
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return (E_ZCL_SUCCESS);

    }


    /* Update colour attributes not directly affected by the current transition */
    DBG_vPrintf(TRACE_COLOUR_CONTROL, " Mode %d", psCommon->eColourMode);

    switch(psCommon->eColourMode)
    {

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION:
#endif //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION:

        /* First we need to convert HSV into xyY which we can then use to get any other modes */
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
        /* Try and convert from HSV to xyY. If a conversion error occurs, revert to previous values */
        eStatus = eCLD_ColourControl_HSV2xyY(u8SourceEndPointId,
                                             (uint16)(psCommon->sTransition.sCurrent.iHue / 100),
                                             (uint8)(psCommon->sTransition.sCurrent.iSaturation / 100),
                                             255,
                                             &psSharedStruct->u16CurrentX,
                                             &psSharedStruct->u16CurrentY,
                                             &u8TempY);
        /* If a conversion error occurred, restore the previous values */
        if(eStatus == E_ZCL_ERR_PARAMETER_RANGE)
        {
            DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nConversion error!");
            memcpy(&psCommon->sTransition.sCurrent, &psCommon->sTransition.sPrevious, sizeof(tsCLD_ColourControl_Attributes));
        }

        /* Get master 16 bit value for Hue */
        psCommon->u16CurrentHue = (uint16)(psCommon->sTransition.sCurrent.iHue / 100);

        /* Get 8 bit current hue from 16 bit but limit so it doesn't read 0xff as that is an invalid value */
        psSharedStruct->u8CurrentHue = (uint8)(psCommon->u16CurrentHue >> 8);
        if(psSharedStruct->u8CurrentHue == 0xff)
        {
            psSharedStruct->u8CurrentHue = CLD_COLOURCONTROL_HUE_MAX_VALUE;
        }

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
        /* Get 16 bit enhanced current hue */
        psSharedStruct->u16EnhancedCurrentHue = psCommon->u16CurrentHue;
#endif //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE

        /* Get saturation */
        psSharedStruct->u8CurrentSaturation = (uint8)(psCommon->sTransition.sCurrent.iSaturation / 100);
        /*Get 8 bit current Sturation from 16 bit but limit so it doesn't read 0xff as that is an invalid value */
        if(psSharedStruct->u8CurrentSaturation == 0xff)
        {
            psSharedStruct->u8CurrentSaturation = CLD_COLOURCONTROL_SATURATION_MAX_VALUE;
        }
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        /* Convert from xyY to CCT */
        vCLD_ColourControl_xyY2CCT(psSharedStruct->u16CurrentX,
                                   psSharedStruct->u16CurrentY,
                                   255,
                                   &psSharedStruct->u16ColourTemperatureMired);
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)

        break;

    case E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y:
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)  
        /* Try and convert from xyY to HSV. If a conversion error occurs, revert to previous values */
        eStatus = eCLD_ColourControl_xyY2HSV(psCommon,
                                            (uint16)(psCommon->sTransition.sCurrent.iX / 100),
                                            (uint16)(psCommon->sTransition.sCurrent.iY / 100),
                                            255,
                                            &psCommon->u16CurrentHue,
                                            &psSharedStruct->u8CurrentSaturation,
                                            &u8TempVal);
        /* If a conversion error occurred, restore the previous values */
        if(eStatus == E_ZCL_ERR_PARAMETER_RANGE)
        {
            DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nConversion error!");
            memcpy(&psCommon->sTransition.sCurrent, &psCommon->sTransition.sPrevious, sizeof(tsCLD_ColourControl_Attributes));
        }
#endif 
        /* Update X & Y attributes with values that should be valid for the primaries used */
        psSharedStruct->u16CurrentX = (uint16)(psCommon->sTransition.sCurrent.iX / 100);
        psSharedStruct->u16CurrentY = (uint16)(psCommon->sTransition.sCurrent.iY / 100);

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
        /* Get 8 bit current hue from 16 bit but limit so it doesn't read 0xff as that is an invalid value */
        psSharedStruct->u8CurrentHue = (uint8)(psCommon->u16CurrentHue >> 8);
        if(psSharedStruct->u8CurrentHue == 0xff)
        {
            psSharedStruct->u8CurrentHue = CLD_COLOURCONTROL_HUE_MAX_VALUE;
        }
        if(psSharedStruct->u8CurrentSaturation == 0xff)
        {
            psSharedStruct->u8CurrentSaturation = CLD_COLOURCONTROL_SATURATION_MAX_VALUE;
        }

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
        /* Get 16 bit enhanced current hue */
        psSharedStruct->u16EnhancedCurrentHue = psCommon->u16CurrentHue;
#endif //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE

#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        /* Convert from xyY to CCT */
        vCLD_ColourControl_xyY2CCT(psSharedStruct->u16CurrentX,
                                   psSharedStruct->u16CurrentY,
                                   255,
                                   &psSharedStruct->u16ColourTemperatureMired);

        if(psSharedStruct->u16ColourTemperatureMired == 0xffff)
        {
            psSharedStruct->u16ColourTemperatureMired = CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX;
        }
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)

        break;

    case E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE:
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        /* Get CCT */
        psSharedStruct->u16ColourTemperatureMired = (uint16)(psCommon->sTransition.sCurrent.iCCT / 100);

        /* Convert CCT to xyY */
        vCLD_ColourControl_CCT2xyY(psSharedStruct->u16ColourTemperatureMired,
                                   &psSharedStruct->u16CurrentX,
                                   &psSharedStruct->u16CurrentY,
                                   &u8TempY);
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
        /* Convert CCT to xyY - if ColorTemperature is supported then avoid 
           because the conversion happened already in above */
        #if !(CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        vCLD_ColourControl_CCT2xyY((uint16)(psCommon->sTransition.sCurrent.iCCT / 100),
                                   &psSharedStruct->u16CurrentX,
                                   &psSharedStruct->u16CurrentY,
                                   &u8TempY);
        #endif

        /* Try and convert from xyY to HSV. If a conversion error occurs, just abort */
        eStatus = eCLD_ColourControl_xyY2HSV(psCommon,
                                            psSharedStruct->u16CurrentX,
                                            psSharedStruct->u16CurrentY,
                                            255,
                                            &psCommon->u16CurrentHue,
                                            &psSharedStruct->u8CurrentSaturation,
                                            &u8TempVal);
        /* If a conversion error occurred, restore the previous values */
        if(eStatus == E_ZCL_SUCCESS)
        {

            /* Get 8 bit current hue from 16 bit but limit so it doesn't read 0xff as that is an invalid value */
            psSharedStruct->u8CurrentHue = (uint8)(psCommon->u16CurrentHue >> 8);
            if(psSharedStruct->u8CurrentHue == 0xff)
            {
                psSharedStruct->u8CurrentHue = CLD_COLOURCONTROL_HUE_MAX_VALUE;
            }
            if(psSharedStruct->u8CurrentSaturation == 0xff)
            {
                 psSharedStruct->u8CurrentSaturation = CLD_COLOURCONTROL_SATURATION_MAX_VALUE;
            }

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
            /* Get 16 bit enhanced current hue */
            psSharedStruct->u16EnhancedCurrentHue = psCommon->u16CurrentHue;
#endif //CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE

        }

#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)

        break;

    }

#ifdef CLD_COLOURCONTROL_ATTR_REMAINING_TIME
    psSharedStruct->u16RemainingTime = psCommon->sTransition.u16Time;
#endif //CLD_COLOURCONTROL_ATTR_REMAINING_TIME


    /* Generate a callback to let the user know that an update event occurred */
    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = NULL;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    vCLD_ScenesUpdateSceneValid( psEndPointDefinition);
#endif

    /* release EP mutex */
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlStopTransition
 **
 ** DESCRIPTION:
 ** Stops any ongoing transitions of a colour control cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlStopTransition(uint8 u8SourceEndPointId)
{

    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;


    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }


#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE
    tsCLD_ColourControl *psSharedStruct;
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;
    if(psSharedStruct->u8ColourLoopActive)
    {
        return eStatus;
    }
#endif
    if(E_CLD_COLOURCONTROL_CMD_NONE != psCommon->sTransition.eCommand)
    {
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }
    return eStatus;
}


#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToHueTransition
 **
 ** DESCRIPTION:
 ** Handles a Move To Hue transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToHueTransition(
        tsZCL_EndPointDefinition               *psEndPointDefinition,
        tsZCL_ClusterInstance                  *psClusterInstance,
        tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveToHue: Curr=%d Step=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.sStep.iHue);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iHue = psCommon->sTransition.sTarget.iHue;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iHue += psCommon->sTransition.sStep.iHue;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.u16Time);


    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveHueTransition
 **
 ** DESCRIPTION:
 ** Handles a Move Hue transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveHueTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveHue: Curr=%d Rate=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.sStep.iHue);

    /* Add step to current values */
    psCommon->sTransition.sCurrent.iHue += psCommon->sTransition.sStep.iHue;

    /* Set upper limit */
    if(psCommon->sTransition.sCurrent.iHue > (CLD_COLOURCONTROL_HUE_MAX_VALUE << 8) * 100)
    {
        psCommon->sTransition.sCurrent.iHue -= (CLD_COLOURCONTROL_HUE_MAX_VALUE << 8) * 100;
    }

    /* Set lower limit */
    if(psCommon->sTransition.sCurrent.iHue < 0)
    {
        psCommon->sTransition.sCurrent.iHue += (CLD_COLOURCONTROL_HUE_MAX_VALUE << 8) * 100;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepHueTransition
 **
 ** DESCRIPTION:
 ** Handles a Step Hue transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepHueTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nStepHue: Curr=%d Step=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.sStep.iHue);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iHue = psCommon->sTransition.sTarget.iHue;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iHue += psCommon->sTransition.sStep.iHue;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);


}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToSaturationTransition
 **
 ** DESCRIPTION:
 ** Handles a Move To Saturation transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveToSaturation: CurrS=%d StepS=%d",
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iSaturation = psCommon->sTransition.sTarget.iSaturation;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iSaturation += psCommon->sTransition.sStep.iSaturation;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveSaturationTransition
 **
 ** DESCRIPTION:
 ** Handles a Move Saturation transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveSaturation: Curr=%d Rate=%d",
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation);

    /* Add step to current values */
    psCommon->sTransition.sCurrent.iSaturation += psCommon->sTransition.sStep.iSaturation;

    /* Set upper limit */
    if(psCommon->sTransition.sCurrent.iSaturation > CLD_COLOURCONTROL_SATURATION_MAX_VALUE * 100)
    {
        psCommon->sTransition.sCurrent.iSaturation = CLD_COLOURCONTROL_SATURATION_MAX_VALUE * 100;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }

    /* Set lower limit */
    if(psCommon->sTransition.sCurrent.iSaturation < 0)
    {
        psCommon->sTransition.sCurrent.iSaturation = 0;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepSaturationTransition
 **
 ** DESCRIPTION:
 ** Handles a Step Saturation transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nStepSaturation: Curr=%d Step=%d",
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iSaturation = psCommon->sTransition.sTarget.iSaturation;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iSaturation += psCommon->sTransition.sStep.iSaturation;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToHueAndSaturationTransition
 **
 ** DESCRIPTION:
 ** Handles a Move To Hue & Saturation transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToHueAndSaturationTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveToHueAndSaturation: CurrH=%d CurrS=%d StepH=%d StepS=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.sStep.iSaturation);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iHue = psCommon->sTransition.sTarget.iHue;
        psCommon->sTransition.sCurrent.iSaturation = psCommon->sTransition.sTarget.iSaturation;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iHue += psCommon->sTransition.sStep.iHue;
        psCommon->sTransition.sCurrent.iSaturation += psCommon->sTransition.sStep.iSaturation;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " NowH=%d NowS=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.sCurrent.iSaturation,
                                      psCommon->sTransition.u16Time);


    return(E_ZCL_SUCCESS);

}
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToColourTransition
 **
 ** DESCRIPTION:
 ** Handles a Move To Colour transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToColourTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveToColour: CurrX=%d CurrY=%d StepX=%d StepY=%d",
                                      psCommon->sTransition.sCurrent.iX,
                                      psCommon->sTransition.sCurrent.iY,
                                      psCommon->sTransition.sStep.iX,
                                      psCommon->sTransition.sStep.iY);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iX = psCommon->sTransition.sTarget.iX;
        psCommon->sTransition.sCurrent.iY = psCommon->sTransition.sTarget.iY;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iX += psCommon->sTransition.sStep.iX;
        psCommon->sTransition.sCurrent.iY += psCommon->sTransition.sStep.iY;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " NowX=%d NowY=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iX,
                                      psCommon->sTransition.sCurrent.iY,
                                      psCommon->sTransition.u16Time);


    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveColourTransition
 **
 ** DESCRIPTION:
 ** Handles a Move Colour transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveColourTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveColour: CurrX=%d CurrY=%d RateX=%d RateY=%d",
                                      psCommon->sTransition.sCurrent.iX,
                                      psCommon->sTransition.sCurrent.iY,
                                      psCommon->sTransition.sStep.iX,
                                      psCommon->sTransition.sStep.iY);

    if((psCommon->sTransition.sStep.iX == 0) && (psCommon->sTransition.sStep.iY == 0))
    {
        /* End transition */
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }
    else
    {
        /* Add step to current values */
        psCommon->sTransition.sCurrent.iX += psCommon->sTransition.sStep.iX;
        psCommon->sTransition.sCurrent.iY += psCommon->sTransition.sStep.iY;

        /* Set upper limit */
        if(psCommon->sTransition.sCurrent.iX > CLD_COLOURCONTROL_X_MAX_VALUE * 100)
        {
            psCommon->sTransition.sCurrent.iX = CLD_COLOURCONTROL_X_MAX_VALUE * 100;
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        }
        if(psCommon->sTransition.sCurrent.iY > CLD_COLOURCONTROL_Y_MAX_VALUE * 100)
        {
            psCommon->sTransition.sCurrent.iY = CLD_COLOURCONTROL_Y_MAX_VALUE * 100;
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        }

        /* Set at lower limit */
        if(psCommon->sTransition.sCurrent.iX < 0)
        {
            psCommon->sTransition.sCurrent.iX = 0;
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        }
        if(psCommon->sTransition.sCurrent.iY < 0)
        {
            psCommon->sTransition.sCurrent.iY = 0;
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        }
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " NowX=%d NowY=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iX,
                                      psCommon->sTransition.sCurrent.iY,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepColourTransition
 **
 ** DESCRIPTION:
 ** Handles a Step Colour transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepColourTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nStepColour: CurrX=%d CurrY=%d StepX=%d StepY=%d",
                                      psCommon->sTransition.sCurrent.iX,
                                      psCommon->sTransition.sCurrent.iY,
                                      psCommon->sTransition.sStep.iX,
                                      psCommon->sTransition.sStep.iY);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iX = psCommon->sTransition.sTarget.iX;
        psCommon->sTransition.sCurrent.iY = psCommon->sTransition.sTarget.iY;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iX += psCommon->sTransition.sStep.iX;
        psCommon->sTransition.sCurrent.iY += psCommon->sTransition.sStep.iY;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " NowX=%d NowY=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iX,
                                      psCommon->sTransition.sCurrent.iY,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToColourTemperatureTransition
 **
 ** DESCRIPTION:
 ** Handles a Move To Colour Temperature transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToColourTemperatureTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    tsCLD_ColourControl *psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveToColourTemperature: Curr=%d Step=%d",
                                      psSharedStruct->u16ColourTemperatureMired,
                                      psCommon->sTransition.sStep.iCCT);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iCCT = psCommon->sTransition.sTarget.iCCT;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iCCT += psCommon->sTransition.sStep.iCCT;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psSharedStruct->u16ColourTemperatureMired,
                                      psCommon->sTransition.u16Time);


    return(E_ZCL_SUCCESS);

}
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED) 
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleColourLoopTransition
 **
 ** DESCRIPTION:
 ** Handles a Move Hue transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleColourLoopTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nColourLoop: CurrHue=%d Step=%d",
                                      psCommon->sTransition.sCurrent.iHue,
                                      psCommon->sTransition.sStep.iHue);

    tsCLD_ColourControl *psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    if(psSharedStruct->u8ColourLoopActive == 0x00)
    {

        /* Restore original start hue */
        psCommon->sTransition.sCurrent.iHue = psSharedStruct->u16ColourLoopStoredEnhancedHue * 100;

        /* Stop transition */
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        return E_ZCL_SUCCESS;
    }

    /* Add step to current values */
    psCommon->sTransition.sCurrent.iHue += psCommon->sTransition.sStep.iHue;

    /* Set upper limit */
    if(psCommon->sTransition.sCurrent.iHue > (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100)
    {
        psCommon->sTransition.sCurrent.iHue -= (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
    }

    /* Set lower limit */
    if(psCommon->sTransition.sCurrent.iHue < 0)
    {
        psCommon->sTransition.sCurrent.iHue += (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " NowHue=%d", psCommon->sTransition.sCurrent.iHue);

    return(E_ZCL_SUCCESS);

}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveColourTemperatureTransition
 **
 ** DESCRIPTION:
 ** Handles a Move Colour Temperature transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveColourTemperatureTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nMoveColourTemperature: Curr=%d Rate=%d",
                                      psCommon->sTransition.sCurrent.iCCT,
                                      psCommon->sTransition.sStep.iCCT);

    /* Add step to current values */
    psCommon->sTransition.sCurrent.iCCT += psCommon->sTransition.sStep.iCCT;

    /* If its a move UP, Set upper limit */
    if((psCommon->sTransition.sStep.iCCT > 0) && (psCommon->sTransition.sCurrent.iCCT > psCommon->sTransition.sTarget.iCCT))
    {
        psCommon->sTransition.sCurrent.iCCT = psCommon->sTransition.sTarget.iCCT;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }


    /* If its a move DOWN, Set lower limit */
    if((psCommon->sTransition.sStep.iCCT < 0) && (psCommon->sTransition.sCurrent.iCCT < psCommon->sTransition.sTarget.iCCT))
    {
        psCommon->sTransition.sCurrent.iCCT = psCommon->sTransition.sTarget.iCCT;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iCCT,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepColourTemperatureTransition
 **
 ** DESCRIPTION:
 ** Handles a Step Colour Temperature transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepColourTemperatureTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, "\nStepColourTemperature: Curr=%d Step=%d",
                                      psCommon->sTransition.sCurrent.iCCT,
                                      psCommon->sTransition.sStep.iCCT);

    if(psCommon->sTransition.u16Time == 0)
    {
        /* End transition */
        psCommon->sTransition.sCurrent.iCCT = psCommon->sTransition.sTarget.iCCT;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    }
    else
    {
        /* Decrement transition timer */
        psCommon->sTransition.u16Time--;

        /* Add step to current values */
        psCommon->sTransition.sCurrent.iCCT += psCommon->sTransition.sStep.iCCT;

        /* If its a step UP, Set upper limit */
        if((psCommon->sTransition.sStep.iCCT > 0) && (psCommon->sTransition.sCurrent.iCCT > psCommon->sTransition.sTarget.iCCT))
        {
            psCommon->sTransition.sCurrent.iCCT = psCommon->sTransition.sTarget.iCCT;
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
    }


        /* If its a step DOWN, Set lower limit */
        if((psCommon->sTransition.sStep.iCCT < 0) && (psCommon->sTransition.sCurrent.iCCT < psCommon->sTransition.sTarget.iCCT))
        {
            psCommon->sTransition.sCurrent.iCCT = psCommon->sTransition.sTarget.iCCT;
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        }

    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL_UPDATES, " Now=%d Time=%d",
                                      psCommon->sTransition.sCurrent.iCCT,
                                      psCommon->sTransition.u16Time);

    return(E_ZCL_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleSceneTransition
 **
 ** DESCRIPTION:
 ** Handles a scene transition
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleSceneTransition(
         tsZCL_EndPointDefinition               *psEndPointDefinition,
         tsZCL_ClusterInstance                  *psClusterInstance,
         tsCLD_ColourControlCustomDataStructure *psCommon)
{

    memcpy(&psCommon->sTransition.sCurrent, &psCommon->sTransition.sTarget, sizeof(tsCLD_ColourControl_Attributes));

    psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE
        /* Get pointer to color control cluster shared struct */
        tsCLD_ColourControl *psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;
        if(psSharedStruct->u8ColourLoopActive == 0x01)
        {
            psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_COLOUR_LOOP_SET;

            /* If direction is decrement, make the step size negative */
            if(psSharedStruct->u8ColourLoopDirection == E_CLD_COLOURCONTROL_COLOURLOOP_DIRECTION_INCREMENT)
            {
                /* Calculate step size */
                if(psSharedStruct->u16ColourLoopTime > 0)
                {
                    psCommon->sTransition.sStep.iHue = (65535 * 100) / ((int)psSharedStruct->u16ColourLoopTime * 10);
                }
                else
                {
                    psCommon->sTransition.sStep.iHue = (65535 * 100);
                }
            }
            else
            {
                /* Calculate step size */
                if(psSharedStruct->u16ColourLoopTime > 0)
                {
                    psCommon->sTransition.sStep.iHue = ((-65535 * 100) / ((int)psSharedStruct->u16ColourLoopTime * 10));
                }
                else
                {
                    psCommon->sTransition.sStep.iHue = (-65535 * 100);
                }
            }
        }
    #endif

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlSceneEventHandler
 **
 ** DESCRIPTION:
 ** Handles events generated by a scenes cluster (if present)
 **
 ** PARAMETERS:                 Name                        Usage
 **
 ** tsZCL_EndPointDefinition *psEndPointDefinition          End Point Definition
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlSceneEventHandler(
                        teZCL_SceneEvent            eEvent,
                        tsZCL_EndPointDefinition   *psEndPointDefinition,
                        tsZCL_ClusterInstance      *psClusterInstance)
{

    /* Get pointer to custom data structure */
    tsCLD_ColourControlCustomDataStructure *psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get pointer to level control cluster shared struct */
    tsCLD_ColourControl *psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;


    switch(eEvent)
    {

    case E_ZCL_SCENE_EVENT_SAVE:
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nCC: Scene Event: Save");
        break;

    case E_ZCL_SCENE_EVENT_RECALL:
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nCC: Scene Event: Recall");

        psCommon->sTransition.sTarget.iX            = (int)psSharedStruct->u16CurrentX * 100;
        psCommon->sTransition.sTarget.iY            = (int)psSharedStruct->u16CurrentY * 100;
#ifdef  CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
        psCommon->sTransition.sTarget.iCCT          = (int)psSharedStruct->u16ColourTemperatureMired * 100;
#endif        
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE      
        psCommon->sTransition.sTarget.iHue          = (int)psSharedStruct->u16EnhancedCurrentHue * 100;
#else
        #ifdef CLD_COLOURCONTROL_ATTR_CURRENT_HUE
        psCommon->sTransition.sTarget.iHue          = (int)((int)psSharedStruct->u8CurrentHue << 8) * 100;  
        #endif
#endif      
#ifdef CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION
        psCommon->sTransition.sTarget.iSaturation   = (int)psSharedStruct->u8CurrentSaturation * 100;
#endif      
        psCommon->sTransition.eCommand              = E_CLD_COLOURCONTROL_CMD_SCENE_TRANSITION;

        eCLD_ColourControlUpdate(psEndPointDefinition->u8EndPointNumber);
        break;

    }

    return E_ZCL_SUCCESS;
}
#endif

#endif          // ifdef CLD_COLOUR_CONTROL

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

