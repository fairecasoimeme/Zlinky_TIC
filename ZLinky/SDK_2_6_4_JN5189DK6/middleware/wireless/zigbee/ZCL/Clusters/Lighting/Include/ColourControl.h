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
 * COMPONENT:          ColourControl.h
 *
 * DESCRIPTION:        Header for Colour Control Cluster
 *
 *****************************************************************************/

#ifndef COLOUR_CONTROL_H
#define COLOUR_CONTROL_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define LIGHTING_CLUSTER_ID_COLOUR_CONTROL          0x0300

#define CLD_COLOURCONTROL_COMPENSATION_TEXT_MAX_STRING_LENGTH   255

/****************************************************************************/
/*             Colour Control Cluster - Optional Attributes                */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the simple metering cluster.                               */
/****************************************************************************/

/* Colour information attribute set attribute ID's (5.2.2.2.1) */

/* Enable the current hue attribute */
//#define CLD_COLOURCONTROL_ATTR_CURRENT_HUE

/* Enable the current saturation attribute */
//#define CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION

/* Enable the remaining time attribute */
//#define CLD_COLOURCONTROL_ATTR_REMAINING_TIME

/* Enable the drift compensation attribute */
//#define CLD_COLOURCONTROL_ATTR_DRIFT_COMPENSATION

/* Enable the compensation text attribute */
//#define CLD_COLOURCONTROL_ATTR_COMPENSATION_TEXT

/* Enable the colour temperature attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED

/* Enable the colour mode attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_MODE

/* Defined Primaries Information attribute attribute ID's set (5.2.2.2.2) */

/* Set to the number of primaries to enable primary x and y attributes */
//#define CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES  6

/* Enable the primary 1 intensity attribute */
//#define CLD_COLOURCONTROL_ATTR_PRIMARY_1_INTENSITY

/* Enable the primary 2 intensity attribute */
//#define CLD_COLOURCONTROL_ATTR_PRIMARY_2_INTENSITY

/* Enable the primary 3 intensity attribute */
//#define CLD_COLOURCONTROL_ATTR_PRIMARY_3_INTENSITY

/* Additional Defined Primaries Information attribute attribute ID's set (5.2.2.2.3) */

/* Enable the primary 4 intensity attribute */
//#define CLD_COLOURCONTROL_ATTR_PRIMARY_4_INTENSITY

/* Enable the primary 5 intensity attribute */
//#define CLD_COLOURCONTROL_ATTR_PRIMARY_5_INTENSITY

/* Enable the primary 6 intensity attribute */
//#define CLD_COLOURCONTROL_ATTR_PRIMARY_6_INTENSITY

/* Defined Colour Points Settings attribute ID's set (5.2.2.2.4) */

/* Enable the white point x attribute */
//#define CLD_COLOURCONTROL_ATTR_WHITE_POINT_X

/* Enable the white point y attribute */
//#define CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y

/* Enable the colour points attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_POINTS

/* Enable the enhanced current hue attribute */
//#define CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE

/* Enable the enhanced colour mode attribute */
//#define CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE

/* Enable the colour loop active attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE

/* Enable the colour loop direction attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION

/* Enable the colour loop time attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME

/* Enable the colour loop start enhanced hue attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE

/* Enable the colour loop stored enhanced hue attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_STORED_ENHANCED_HUE

/* Enable the colour capabilities attribute */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_CAPABILITIES

/* Enable the colour temperature physical minimum  */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN

/* Enable the colour temperature physical maximum  */
//#define CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX

/* Enable the optional Attribute  Couple Colour Temperature to Level*/
//#define CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED

/* Enable the optional Attribute Startup Colour Temperature */
//#define CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED

/* Enable the optional Attribute Reporting Status attribute */
//#define CLD_COLOURCONTROL_ATTR_ATTRIBUTE_REPORTING_STATUS

/* Bit definitions for capabilities attribute */
#define COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED         (1 << 0)
#define COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED            (1 << 1)
#define COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED            (1 << 2)
#define COLOUR_CAPABILITY_XY_SUPPORTED                     (1 << 3)
#define COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED     (1 << 4)


/* Define primaries x and y coordinates */
#ifndef CLD_COLOURCONTROL_RED_X
#define CLD_COLOURCONTROL_RED_X     (0.73467)
#endif

#ifndef CLD_COLOURCONTROL_RED_Y
#define CLD_COLOURCONTROL_RED_Y     (0.26533)
#endif

#ifndef CLD_COLOURCONTROL_GREEN_X
#define CLD_COLOURCONTROL_GREEN_X   (0.27376)
#endif

#ifndef CLD_COLOURCONTROL_GREEN_Y
#define CLD_COLOURCONTROL_GREEN_Y   (0.71741)
#endif

#ifndef CLD_COLOURCONTROL_BLUE_X
#define CLD_COLOURCONTROL_BLUE_X    (0.16658)
#endif

#ifndef CLD_COLOURCONTROL_BLUE_Y
#define CLD_COLOURCONTROL_BLUE_Y    (0.00886)
#endif

/* Define white point x and y coordinates */
#ifndef CLD_COLOURCONTROL_WHITE_X
#define CLD_COLOURCONTROL_WHITE_X   (1.0 / 3.0)
#endif

#ifndef CLD_COLOURCONTROL_WHITE_Y
#define CLD_COLOURCONTROL_WHITE_Y   (1.0 / 3.0)
#endif


/* Define min & max colour temperature */
#ifndef CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MIN
#define CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MIN    0x0000
#endif

#ifndef CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX
#define CLD_COLOURCONTROL_COLOUR_TEMPERATURE_PHY_MAX    0xfeff
#endif

/* Note that since this attribute is stored as a micro reciprocal degree (mired) value (i.e. color temperature in kelvins = 1,000,000 / CoupleColorTempToLevelMinMireds),
 the CoupleColorTempToLevelMinMireds attribute corresponds to an upper bound on the value of the color temperature in kelvins  supported by the device.
*/
#ifndef CLD_COLOURCONTROL_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED
#define CLD_COLOURCONTROL_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED      (0x0000)  
#endif

#ifndef CLD_COLOURCONTROL_HUE_MAX_VALUE
#define CLD_COLOURCONTROL_HUE_MAX_VALUE 0xFE
#endif

#ifndef CLD_COLOURCONTROL_SATURATION_MAX_VALUE
#define CLD_COLOURCONTROL_SATURATION_MAX_VALUE 0xFE
#endif

#ifndef CLD_COLOURCONTROL_XY_MAX_VALUE
        #define CLD_COLOURCONTROL_XY_MAX_VALUE 0xFEFF
#endif

#ifndef CLD_COLOURCONTROL_X_MAX_VALUE
#define CLD_COLOURCONTROL_X_MAX_VALUE 0xFEFF
#endif

#ifndef CLD_COLOURCONTROL_Y_MAX_VALUE
#define CLD_COLOURCONTROL_Y_MAX_VALUE 0xFEFF
#endif

/* Optional attribute dependencies - Related attributes based on ColorCapabilities */
/* When ColourServer is present, CLD_COLOURCONTROL_COLOUR_CAPABILITIES is must */
#if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER) && !(defined CLD_COLOURCONTROL_COLOUR_CAPABILITIES)
    #error You Must define CLD_COLOURCONTROL_COLOUR_CAPABILITIES in zcl_options.h with appropriate capabilitiy bit fields. Refer ZCLv6.
#endif
/* When only ColourClient is present, CLD_COLOURCONTROL_COLOUR_CAPABILITIES is set to 0x00000 to avoid creation of related attributes */
#if (defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_CLIENT) && !(defined COLOUR_CONTROL_SERVER)
    #ifndef CLD_COLOURCONTROL_COLOUR_CAPABILITIES
        #define CLD_COLOURCONTROL_COLOUR_CAPABILITIES (0x0000)
    #endif
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
    #define CLD_COLOURCONTROL_ATTR_CURRENT_HUE
    #define CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION
    #undef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
    #define CLD_COLOURCONTROL_ATTR_COLOUR_MODE
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    #define CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED)
    #define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE
    #define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION
    #define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME
    #define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE
    #define CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_STORED_ENHANCED_HUE
    #undef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE
    #define CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE
    #undef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    #define CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
    #define CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
    #define CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX
    #define CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN
    #define CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED
    #define CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED
    #undef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
    #define CLD_COLOURCONTROL_ATTR_COLOUR_MODE
#endif

#if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 1)
#define CLD_COLOURCONTROL_ATTR_PRIMARY_1_X
#define CLD_COLOURCONTROL_ATTR_PRIMARY_1_Y
#define CLD_COLOURCONTROL_ATTR_PRIMARY_1_INTENSITY
#ifndef CLD_COLOURCONTROL_PRIMARY_1_X
#define CLD_COLOURCONTROL_PRIMARY_1_X           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_1_Y
#define CLD_COLOURCONTROL_PRIMARY_1_Y           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_1_INTENSITY
#define CLD_COLOURCONTROL_PRIMARY_1_INTENSITY   (0xff)
#endif
#endif

#if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 2)
#define CLD_COLOURCONTROL_ATTR_PRIMARY_2_X
#define CLD_COLOURCONTROL_ATTR_PRIMARY_2_Y
#define CLD_COLOURCONTROL_ATTR_PRIMARY_2_INTENSITY
#ifndef CLD_COLOURCONTROL_PRIMARY_2_X
#define CLD_COLOURCONTROL_PRIMARY_2_X           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_2_Y
#define CLD_COLOURCONTROL_PRIMARY_2_Y           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_2_INTENSITY
#define CLD_COLOURCONTROL_PRIMARY_2_INTENSITY   (0xff)
#endif
#endif

#if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 3)
#define CLD_COLOURCONTROL_ATTR_PRIMARY_3_X
#define CLD_COLOURCONTROL_ATTR_PRIMARY_3_Y
#define CLD_COLOURCONTROL_ATTR_PRIMARY_3_INTENSITY
#ifndef CLD_COLOURCONTROL_PRIMARY_3_X
#define CLD_COLOURCONTROL_PRIMARY_3_X           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_3_Y
#define CLD_COLOURCONTROL_PRIMARY_3_Y           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_3_INTENSITY
#define CLD_COLOURCONTROL_PRIMARY_3_INTENSITY   (0xff)
#endif
#endif

#if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 4)
#define CLD_COLOURCONTROL_ATTR_PRIMARY_4_X
#define CLD_COLOURCONTROL_ATTR_PRIMARY_4_Y
#define CLD_COLOURCONTROL_ATTR_PRIMARY_4_INTENSITY
#ifndef CLD_COLOURCONTROL_PRIMARY_4_X
#define CLD_COLOURCONTROL_PRIMARY_4_X           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_4_Y
#define CLD_COLOURCONTROL_PRIMARY_4_Y           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_4_INTENSITY
#define CLD_COLOURCONTROL_PRIMARY_4_INTENSITY   (0xff)
#endif
#endif

#if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 5)
#define CLD_COLOURCONTROL_ATTR_PRIMARY_5_X
#define CLD_COLOURCONTROL_ATTR_PRIMARY_5_Y
#define CLD_COLOURCONTROL_ATTR_PRIMARY_5_INTENSITY
#ifndef CLD_COLOURCONTROL_PRIMARY_5_X
#define CLD_COLOURCONTROL_PRIMARY_5_X           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_5_Y
#define CLD_COLOURCONTROL_PRIMARY_5_Y           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_5_INTENSITY
#define CLD_COLOURCONTROL_PRIMARY_5_INTENSITY   (0xff)
#endif
#endif

#if (defined CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES) && (CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES >= 6)
#define CLD_COLOURCONTROL_ATTR_PRIMARY_6_X
#define CLD_COLOURCONTROL_ATTR_PRIMARY_6_Y
#define CLD_COLOURCONTROL_ATTR_PRIMARY_6_INTENSITY
#ifndef CLD_COLOURCONTROL_PRIMARY_6_X
#define CLD_COLOURCONTROL_PRIMARY_6_X           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_6_Y
#define CLD_COLOURCONTROL_PRIMARY_6_Y           (0)
#endif
#ifndef CLD_COLOURCONTROL_PRIMARY_6_INTENSITY
#define CLD_COLOURCONTROL_PRIMARY_6_INTENSITY   (0xff)
#endif
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINTS
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_INTENSITY
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_INTENSITY
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y
#define CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_INTENSITY
#endif

#ifndef CLD_COLOURCONTROL_CLUSTER_REVISION
    #define CLD_COLOURCONTROL_CLUSTER_REVISION        3
#endif 

#ifndef CLD_COLOURCONTROL_FEATURE_MAP
    #define CLD_COLOURCONTROL_FEATURE_MAP             0
#endif 

#define COLOURCONTROL_EXECUTE_IF_OFF_BIT             (1<<0)
/* End of Optional attribute dependencies */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Attribute ID's */
typedef enum 
{
    /* Colour information attribute set attribute ID's (5.2.2.2.1) */
    E_CLD_COLOURCONTROL_ATTR_CURRENT_HUE                = 0x0000,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION,                    /* Optional */
    E_CLD_COLOURCONTROL_ATTR_REMAINING_TIME,                        /* Optional */
    E_CLD_COLOURCONTROL_ATTR_CURRENT_X,                             /* Mandatory */
    E_CLD_COLOURCONTROL_ATTR_CURRENT_Y,                             /* Mandatory */
    E_CLD_COLOURCONTROL_ATTR_DRIFT_COMPENSATION,                    /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COMPENSATION_TEXT,                     /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED,              /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_MODE,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_OPTIONS                    = 0x000F,   /* Mandatory */
    /* Defined Primaries Information attribute attribute ID's set (5.2.2.2.2) */
    E_CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES        = 0x0010,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_1_X,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_1_Y,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_1_INTENSITY,                   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_2_X                = 0x0015,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_2_Y,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_2_INTENSITY,                   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_3_X                = 0x0019,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_3_Y,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_3_INTENSITY,                   /* Optional */

    /* Additional Defined Primaries Information attribute attribute ID's set (5.2.2.2.3) */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_4_X                = 0x0020,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_4_Y,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_4_INTENSITY,                   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_5_X                = 0x0024,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_5_Y,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_5_INTENSITY,                   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_6_X                = 0x0028,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_6_Y,                           /* Optional */
    E_CLD_COLOURCONTROL_ATTR_PRIMARY_6_INTENSITY,                   /* Optional */

    /* Defined Colour Points Settings attribute ID's set (5.2.2.2.4) */
    E_CLD_COLOURCONTROL_ATTR_WHITE_POINT_X              = 0x0030,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y,                         /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X,                      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y,                      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_INTENSITY,              /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X           = 0x0036,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y,                      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_INTENSITY,              /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X           = 0x003a,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y,                      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_INTENSITY,              /* Optional */
    
    /* Colour information attribute set attribute ID's (5.2.2.2.1) */
    E_CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE       = 0x4000,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE,                  /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE,                    /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION,                 /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME,                      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE,        /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_STORED_ENHANCED_HUE,       /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_CAPABILITIES        = 0x400a,   /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN,      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX,      /* Optional */
    E_CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED, /* Optional */
    E_CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED = 0x4010,      /* Optional */
} teCLD_ColourControl_ClusterID;


/* Drift compensation types (5.2.2.2.1.6) */
typedef enum
{
    E_CLD_COLOURCONTROL_DRIFT_COMPENSATION_NONE         = 0x00,
    E_CLD_COLOURCONTROL_DRIFT_COMPENSATION_OTHER_UNKNOWN,
    E_CLD_COLOURCONTROL_DRIFT_COMPENSATION_TEMPERATURE_MONITORING,
    E_CLD_COLOURCONTROL_DRIFT_COMPENSATION_OPTICAL_LUMINANCE_MONITORING_AND_FEEDBACK,
    E_CLD_COLOURCONTROL_DRIFT_COMPENSATION_OPTICAL_COLOUR_MONITORING_AND_FEEDBACK
} teCLD_ColourControl_DriftCompensation;


/* Colour Mode attribute values (5.2.2.2.1.9) */
typedef enum
{
    E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION   = 0x00,
    E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y,
    E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE,
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION =0x03
#endif
} teCLD_ColourControl_ColourMode;


/* Command codes */
typedef enum 
{
    E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE            = 0x00,  /* Optional */
    E_CLD_COLOURCONTROL_CMD_MOVE_HUE,                       /* Optional */
    E_CLD_COLOURCONTROL_CMD_STEP_HUE,                       /* Optional */
    E_CLD_COLOURCONTROL_CMD_MOVE_TO_SATURATION,             /* Optional */
    E_CLD_COLOURCONTROL_CMD_MOVE_SATURATION,                /* Optional */
    E_CLD_COLOURCONTROL_CMD_STEP_SATURATION,                /* Optional */
    E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE_AND_SATURATION,     /* Optional */
    E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR,                 /* Mandatory */
    E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR,                    /* Mandatory */
    E_CLD_COLOURCONTROL_CMD_STEP_COLOUR,                    /* Mandatory */
    E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR_TEMPERATURE,     /* Optional */
    E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE = 0x40,            /* Optional */
    E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_HUE,                      /* Optional */
    E_CLD_COLOURCONTROL_CMD_ENHANCED_STEP_HUE,                      /* Optional */
    E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE_AND_SATURATION,    /* Optional */
    E_CLD_COLOURCONTROL_CMD_COLOUR_LOOP_SET,                        /* Optional */
    E_CLD_COLOURCONTROL_CMD_STOP_MOVE_STEP = 0x47,                  /* Optional */

    E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR_TEMPERATURE = 0x4b,         /* Optional */
    E_CLD_COLOURCONTROL_CMD_STEP_COLOUR_TEMPERATURE,                /* Optional */

    E_CLD_COLOURCONTROL_CMD_SCENE_TRANSITION        = 0xfe, /* Scene transition */
    E_CLD_COLOURCONTROL_CMD_NONE                    = 0xff  /* End marker */
} teCLD_ColourControl_Command;


/* Directions */
typedef enum 
{
    E_CLD_COLOURCONTROL_DIRECTION_SHORTEST_DISTANCE = 0x00,
    E_CLD_COLOURCONTROL_DIRECTION_LONGEST_DISTANCE,
    E_CLD_COLOURCONTROL_DIRECTION_UP,
    E_CLD_COLOURCONTROL_DIRECTION_DOWN,
} teCLD_ColourControl_Direction;


/* Move Modes */
typedef enum 
{
    E_CLD_COLOURCONTROL_MOVE_MODE_STOP  = 0x00,
    E_CLD_COLOURCONTROL_MOVE_MODE_UP,
    E_CLD_COLOURCONTROL_MOVE_MODE_DOWN  = 0x03,
} teCLD_ColourControl_MoveMode;


/* Step Modes */
typedef enum 
{
    E_CLD_COLOURCONTROL_STEP_MODE_UP    = 0x01,
    E_CLD_COLOURCONTROL_STEP_MODE_DOWN  = 0x03,
} teCLD_ColourControl_StepMode;

/* Bit definitions of the update flags field of the colour loop set command */
#define E_CLD_COLOURCONTROL_FLAGS_UPDATE_ACTION         (1 << 0)
#define E_CLD_COLOURCONTROL_FLAGS_UPDATE_DIRECTION      (1 << 1)
#define E_CLD_COLOURCONTROL_FLAGS_UPDATE_TIME           (1 << 2)
#define E_CLD_COLOURCONTROL_FLAGS_UPDATE_START_HUE      (1 << 3)


/* Colour loop actions */
typedef enum 
{
    E_CLD_COLOURCONTROL_COLOURLOOP_ACTION_DEACTIVATE            = 0x00,
    E_CLD_COLOURCONTROL_COLOURLOOP_ACTION_ACTIVATE_FROM_START   = 0x01,
    E_CLD_COLOURCONTROL_COLOURLOOP_ACTION_ACTIVATE_FROM_CURRENT = 0x02
} teCLD_ColourControl_LoopAction;


/* Colour loop directions */
typedef enum 
{
    E_CLD_COLOURCONTROL_COLOURLOOP_DIRECTION_DECREMENT          = 0x00,
    E_CLD_COLOURCONTROL_COLOURLOOP_DIRECTION_INCREMENT          = 0x01,
} teCLD_ColourControl_LoopDirection;


/* Colour Control Cluster */
typedef struct
{
#ifdef COLOUR_CONTROL_SERVER
    /* Colour information attribute set attribute ID's (5.2.2.2.1) */
#ifdef CLD_COLOURCONTROL_ATTR_CURRENT_HUE
    zuint8                  u8CurrentHue;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION
    zuint8                  u8CurrentSaturation;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_REMAINING_TIME
    zuint16                 u16RemainingTime;
#endif

    zuint16                 u16CurrentX;

    zuint16                 u16CurrentY;

#ifdef CLD_COLOURCONTROL_ATTR_DRIFT_COMPENSATION
    zenum8                  u8DriftCompensation;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COMPENSATION_TEXT
    tsZCL_CharacterString   sCompensationText;
    uint8                   au8CompensationText[CLD_COLOURCONTROL_COMPENSATION_TEXT_MAX_STRING_LENGTH];
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
    zuint16                 u16ColourTemperatureMired;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
    zenum8                  u8ColourMode;
#endif

    zbmap8                  u8Options;

    /* Defined Primaries Information attribute attribute ID's set (5.2.2.2.2) */
#ifdef CLD_COLOURCONTROL_ATTR_NUMBER_OF_PRIMARIES
    zuint8                  u8NumberOfPrimaries;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_1_X
    zuint16                 u16Primary1X;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_1_Y
    zuint16                 u16Primary1Y;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_1_INTENSITY
    zuint8                  u8Primary1Intensity;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_2_X
    zuint16                 u16Primary2X;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_2_Y
    zuint16                 u16Primary2Y;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_2_INTENSITY
    zuint8                  u8Primary2Intensity;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_3_X
    zuint16                 u16Primary3X;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_3_Y
    zuint16                 u16Primary3Y;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_3_INTENSITY
    zuint8                  u8Primary3Intensity;
#endif

    /* Additional Defined Primaries Information attribute attribute ID's set (5.2.2.2.3) */
#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_4_X
    zuint16                 u16Primary4X;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_4_Y
    zuint16                 u16Primary4Y;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_4_INTENSITY
    zuint8                  u8Primary4Intensity;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_5_X
    zuint16                 u16Primary5X;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_5_Y
    zuint16                 u16Primary5Y;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_5_INTENSITY
    zuint8                  u8Primary5Intensity;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_6_X
    zuint16                 u16Primary6X;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_6_Y
    zuint16                 u16Primary6Y;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_PRIMARY_6_INTENSITY
    zuint8                  u8Primary6Intensity;
#endif

    /* Defined Colour Points Settings attribute ID's set (5.2.2.2.4) */
#ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_X
    zuint16                 u16WhitePointX;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_WHITE_POINT_Y
    zuint16                 u16WhitePointY;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_X
    zuint16                 u16ColourPointRX;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_Y
    zuint16                 u16ColourPointRY;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_R_INTENSITY
    zuint8                  u8ColourPointRIntensity;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_X
    zuint16                 u16ColourPointGX;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_Y
    zuint16                 u16ColourPointGY;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_G_INTENSITY
    zuint8                  u8ColourPointGIntensity;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_X
    zuint16                 u16ColourPointBX;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_Y
    zuint16                 u16ColourPointBY;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_POINT_B_INTENSITY
    zuint8                  u8ColourPointBIntensity;
#endif

/* Colour information attribute set attribute ID's (5.2.2.2.1) */
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE
    zuint16                 u16EnhancedCurrentHue;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    zenum8                 u8EnhancedColourMode;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE
    zuint8                 u8ColourLoopActive;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION
    zuint8                 u8ColourLoopDirection;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME
    zuint16                 u16ColourLoopTime;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_START_ENHANCED_HUE
    zuint16                 u16ColourLoopStartEnhancedHue;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_STORED_ENHANCED_HUE
    zuint16                 u16ColourLoopStoredEnhancedHue;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_CAPABILITIES
    zuint16                 u16ColourCapabilities;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN
    zuint16                 u16ColourTemperatureMiredPhyMin;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX
    zuint16                 u16ColourTemperatureMiredPhyMax;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_COUPLE_COLOUR_TEMPERATURE_TO_LEVEL_MIN_MIRED
    zuint16                 u16CoupleColourTempToLevelMinMired;
#endif

#ifdef CLD_COLOURCONTROL_ATTR_STARTUP_COLOUR_TEMPERATURE_MIRED
    zuint16                 u16StartupColourTemperatureMired;
#endif
    
#ifdef CLD_COLOURCONTROL_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                  u8AttributeReportingStatus;
#endif

#endif
    zbmap32                 u32FeatureMap;
    
    zuint16                 u16ClusterRevision;

} tsCLD_ColourControl;

/* Move to hue command payload */
typedef struct
{
    uint8                           u8Hue;
    teCLD_ColourControl_Direction   eDirection;
    uint16                          u16TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;    
} tsCLD_ColourControl_MoveToHueCommandPayload;


/* Move hue command payload */
typedef struct
{
    teCLD_ColourControl_MoveMode    eMode;
    uint8                           u8Rate;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;    
} tsCLD_ColourControl_MoveHueCommandPayload;


/* Step hue command payload */
typedef struct
{
    teCLD_ColourControl_StepMode    eMode;
    uint8                           u8StepSize;
    uint8                           u8TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_StepHueCommandPayload;


/* Move to saturation command payload */
typedef struct
{
    uint8                           u8Saturation;
    uint16                          u16TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_MoveToSaturationCommandPayload;


/* Move saturation command payload */
typedef struct
{
    teCLD_ColourControl_MoveMode    eMode;
    uint8                           u8Rate;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_MoveSaturationCommandPayload;


/* Step saturation command payload */
typedef struct
{
    teCLD_ColourControl_StepMode    eMode;
    uint8                           u8StepSize;
    uint8                           u8TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_StepSaturationCommandPayload;


/* Move to hue and saturation command payload */
typedef struct
{
    uint8                           u8Hue;
    uint8                           u8Saturation;
    uint16                          u16TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload;


/* Move to colour command payload */
typedef struct
{
    uint16                          u16ColourX;
    uint16                          u16ColourY;
    uint16                          u16TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_MoveToColourCommandPayload;


/* Move colour command payload */
typedef struct
{
    int16                           i16RateX;
    int16                           i16RateY;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_MoveColourCommandPayload;


/* Step colour command payload */
typedef struct
{
    int16                           i16StepX;
    int16                           i16StepY;
    uint16                          u16TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_StepColourCommandPayload;


/* Move to colour temperature command payload */
typedef struct
{
    uint16                          u16ColourTemperatureMired;
    uint16                          u16TransitionTime;
    zbmap8                          u8OptionsMask;
    zbmap8                          u8OptionsOverride;     
} tsCLD_ColourControl_MoveToColourTemperatureCommandPayload;

/* Enhanced Move To Hue command payload */
typedef struct
{
    uint16                              u16EnhancedHue;
    teCLD_ColourControl_Direction       eDirection;
    uint16                              u16TransitionTime;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;     
} tsCLD_ColourControl_EnhancedMoveToHueCommandPayload;

/* Enhanced Move Hue command payload */
typedef struct
{
    teCLD_ColourControl_MoveMode        eMode;
    uint16                              u16Rate;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;     
} tsCLD_ColourControl_EnhancedMoveHueCommandPayload;

/* Enhanced Step Hue command payload */
typedef struct
{
    teCLD_ColourControl_StepMode        eMode;
    uint16                              u16StepSize;
    uint16                              u16TransitionTime;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;     
} tsCLD_ColourControl_EnhancedStepHueCommandPayload;

/* Enhanced Move to Hue and saturation command payload */
typedef struct
{
    uint16                              u16EnhancedHue;
    uint8                               u8Saturation;
    uint16                              u16TransitionTime;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;    
} tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload;

/* Enhanced Colour Loop Set command payload */
typedef struct
{
    uint8                               u8UpdateFlags;
    teCLD_ColourControl_LoopAction      eAction;
    teCLD_ColourControl_LoopDirection   eDirection;
    uint16                              u16Time;
    uint16                              u16StartHue;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;    
} tsCLD_ColourControl_ColourLoopSetCommandPayload;

/* Stop Move Step command payload - Has no payload */

/* Move colour temperature command payload */
typedef struct
{
    teCLD_ColourControl_MoveMode        eMode;
    uint16                              u16Rate;
    uint16                              u16ColourTemperatureMiredMin;
    uint16                              u16ColourTemperatureMiredMax;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;    
} tsCLD_ColourControl_MoveColourTemperatureCommandPayload;

/* Step colour temperature command payload */
typedef struct
{
    teCLD_ColourControl_StepMode        eMode;
    uint16                              u16StepSize;
    uint16                              u16TransitionTime;
    uint16                              u16ColourTemperatureMiredMin;
    uint16                              u16ColourTemperatureMiredMax;
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;    
} tsCLD_ColourControl_StepColourTemperatureCommandPayload;

/* Stop Move Step command payload */
typedef struct
{
    zbmap8                              u8OptionsMask;
    zbmap8                              u8OptionsOverride;    
} tsCLD_ColourControl_StopMoveStepCommandPayload;

/* Definition of Call back Event Structure */
typedef struct
{
    uint8                                                           u8CommandId;
    union
    {
        tsCLD_ColourControl_MoveToHueCommandPayload                 *psMoveToHueCommandPayload;
        tsCLD_ColourControl_MoveHueCommandPayload                   *psMoveHueCommandPayload;
        tsCLD_ColourControl_StepHueCommandPayload                   *psStepHueCommandPayload;
        tsCLD_ColourControl_MoveToSaturationCommandPayload          *psMoveToSaturationCommandPayload;
        tsCLD_ColourControl_MoveSaturationCommandPayload            *psMoveSaturationCommandPayload;
        tsCLD_ColourControl_StepSaturationCommandPayload            *psStepSaturationCommandPayload;
        tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload    *psMoveToHueAndSaturationCommandPayload;
        tsCLD_ColourControl_MoveToColourCommandPayload              *psMoveToColourCommandPayload;
        tsCLD_ColourControl_MoveColourCommandPayload                *psMoveColourCommandPayload;
        tsCLD_ColourControl_StepColourCommandPayload                *psStepColourCommandPayload;
        tsCLD_ColourControl_MoveToColourTemperatureCommandPayload   *psMoveToColourTemperatureCommandPayload;

        tsCLD_ColourControl_EnhancedMoveToHueCommandPayload                 *psEnhancedMoveToHueCommandPayload;
        tsCLD_ColourControl_EnhancedMoveHueCommandPayload                   *psEnhancedMoveHueCommandPayload;
        tsCLD_ColourControl_EnhancedStepHueCommandPayload                   *psEnhancedStepHueCommandPayload;
        tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload    *psEnhancedMoveToHueAndSaturationCommandPayload;
        tsCLD_ColourControl_ColourLoopSetCommandPayload                     *psColourLoopSetCommandPayload;
        tsCLD_ColourControl_MoveColourTemperatureCommandPayload             *psMoveColourTemperatureCommandPayload;
        tsCLD_ColourControl_StepColourTemperatureCommandPayload             *psStepColourTemperatureCommandPayload;
        tsCLD_ColourControl_StopMoveStepCommandPayload                      *psStopMoveStepCommandPayload;
    } uMessage;
} tsCLD_ColourControlCallBackMessage;


typedef struct
{
    int                                                             iHue;
    int                                                             iSaturation;
    int                                                             iX;
    int                                                             iY;
    int                                                             iCCT;
} tsCLD_ColourControl_Attributes;


/* Data related to transitions */
typedef struct
{
    teCLD_ColourControl_Command                                     eCommand;
    uint16                                                          u16Time;
    tsCLD_ColourControl_Attributes                                  sStart;
    tsCLD_ColourControl_Attributes                                  sCurrent;
    tsCLD_ColourControl_Attributes                                  sTarget;
    tsCLD_ColourControl_Attributes                                  sStep;
    tsCLD_ColourControl_Attributes                                  sPrevious;
} tsCLD_ColourControl_Transition;


/* Custom data structure */
typedef struct
{
    teCLD_ColourControl_ColourMode                                  eColourMode;
    uint16                                                          u16CurrentHue;
    tsCLD_ColourControl_Transition                                  sTransition;

    /* Matrices for XYZ <> RGB conversions */
    float                                                           afXYZ2RGB[3][3];
    float                                                           afRGB2XYZ[3][3];

    tsZCL_ReceiveEventAddress                                         sReceiveEventAddress;
    tsZCL_CallBackEvent                                             sCustomCallBackEvent;
    tsCLD_ColourControlCallBackMessage                              sCallBackMessage;
} tsCLD_ColourControlCustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ColourControlCreateColourControl(
        tsZCL_ClusterInstance       *psClusterInstance,
        bool_t                      bIsServer,
        tsZCL_ClusterDefinition     *psClusterDefinition,
        void                        *pvEndPointSharedStructPtr,
        uint8       *pu8AttributeControlBits,
        tsCLD_ColourControlCustomDataStructure  *psCustomDataStructure);

        
#ifdef COLOUR_CONTROL_CLIENT 
PUBLIC teZCL_Status eCLD_ColourControlCommandMoveToHueCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveToHueCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveHueCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveHueCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandStepHueCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_StepHueCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveToSaturationCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveToSaturationCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveSaturationCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveSaturationCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandStepSaturationCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_StepSaturationCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveToHueAndSaturationCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveToColourCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveToColourCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveColourCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveColourCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandStepColourCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_StepColourCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandEnhancedMoveToHueCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_EnhancedMoveToHueCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandEnhancedMoveHueCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_EnhancedMoveHueCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandEnhancedStepHueCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_EnhancedStepHueCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandEnhancedMoveToHueAndSaturationCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandColourLoopSetCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_ColourLoopSetCommandPayload *psPayload);

PUBLIC  teZCL_Status eCLD_ColourControlCommandStopMoveStepCommandSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ColourControl_StopMoveStepCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ColourControlCommandMoveToColourTemperatureCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveToColourTemperatureCommandPayload *psPayload);
        
PUBLIC teZCL_Status eCLD_ColourControlCommandMoveColourTemperatureCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_MoveColourTemperatureCommandPayload *psPayload);
        
PUBLIC teZCL_Status eCLD_ColourControlCommandStepColourTemperatureCommandSend(
        uint8                       u8SourceEndPointId,
        uint8                       u8DestinationEndPointId,
        tsZCL_Address               *psDestinationAddress,
        uint8                       *pu8TransactionSequenceNumber,
        tsCLD_ColourControl_StepColourTemperatureCommandPayload *psPayload);

#endif

#ifdef COLOUR_CONTROL_SERVER
PUBLIC teZCL_Status eCLD_ColourControl_GetRGB(
        uint8                       u8SourceEndPointId,
        uint8                       *pu8Red,
        uint8                       *pu8Green,
        uint8                       *pu8Blue);

PUBLIC teZCL_Status eCLD_ColourControl_HSV2xyY(
        uint8                       u8SourceEndPointId,
        uint16                      u16Hue,
        uint8                       u8Saturation,
        uint8                       u8Value,
        uint16                      *pu16x,
        uint16                      *pu16y,
        uint8                       *pu8Y);

PUBLIC void vCLD_ColourControl_CCT2xyY(
        uint16                      u16ColourTemperatureMired,
        uint16                      *pu16x,
        uint16                      *pu16y,
        uint8                       *pu8Y);

PUBLIC teZCL_Status eCLD_ColourControlUpdate(
        uint8                       u8SourceEndPointId);
        
PUBLIC teZCL_Status eCLD_ColourControlStopTransition(
        uint8                       u8SourceEndPointId);

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
PUBLIC teZCL_Status eCLD_ColourControlHandleColourModeChange(
        uint8                           u8SourceEndPointId,
        teCLD_ColourControl_ColourMode  eMode);
#endif
#endif        
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_ColourControl;
extern const tsZCL_AttributeDefinition asCLD_ColourControlClusterAttributeDefinitions[];
extern uint8 au8ColourControlAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* COLOUR_CONTROL_H */
