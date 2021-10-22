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


/*
* Copyright 2016-2017 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef TIME_H
#define TIME_H

/*!
\file       Time.h
\brief      Header for Time Cluster
*/
/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/     
#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*             Time Cluster - Optional Attributes                           */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the time cluster.                                          */
/****************************************************************************/

/* Enable the optional Time Zone attribute */
//#define CLD_TIME_ATTR_TIME_ZONE

/* Enable the optional DST Start attribute */
//#define CLD_TIME_ATTR_DST_START

/* Enable the optional DST End attribute */
//#define CLD_TIME_ATTR_DST_END

/* Enable the optional DST Shift attribute */
//#define CLD_TIME_ATTR_DST_SHIFT

/* Enable the optional Standard Time attribute */
//#define CLD_TIME_ATTR_STANDARD_TIME

/* Enable the optional Local Time attribute */
//#define CLD_TIME_ATTR_LOCAL_TIME

/* End of optional attributes */


/* Cluster ID's */
#define GENERAL_CLUSTER_ID_TIME                            0x000a


/* Time Status  - bit definitions and bit masks */
#define CLD_TM_TIME_STATUS_MASTER_BIT                 0
#define CLD_TM_TIME_STATUS_SYNCHRONIZED_BIT           1
#define CLD_TM_TIME_STATUS_MASTER_ZONE_DST_BIT        2
#define CLD_TM_TIME_STATUS_SUPERSEDING_BIT            3

#define CLD_TM_TIME_STATUS_MASTER_MASK                (1 << CLD_TM_TIME_STATUS_MASTER_BIT)
#define CLD_TM_TIME_STATUS_SYNCHRONIZED_MASK          (1 << CLD_TM_TIME_STATUS_SYNCHRONIZED_BIT)
#define CLD_TM_TIME_STATUS_MASTER_ZONE_DST_MASK       (1 << CLD_TM_TIME_STATUS_MASTER_ZONE_DST_BIT)
#define CLD_TM_TIME_STATUS_SUPERSEDING_MASK           (1 << CLD_TM_TIME_STATUS_SUPERSEDING_BIT)


#define CLD_TIME_NUMBER_OF_MANDATORY_ATTRIBUTE           2  //utctTime, u8TimeStatus

#ifdef CLD_TIME_ATTR_TIME_ZONE
    #define ATTR_TIME_ZONE                               1
#else
    #define ATTR_TIME_ZONE                               0
#endif

#ifdef CLD_TIME_ATTR_DST_START
    #define ATTR_DST_START                               1
#else
    #define ATTR_DST_START                               0
#endif

#ifdef CLD_TIME_ATTR_DST_END
    #define ATTR_DST_END                                 1
#else
    #define ATTR_DST_END                                 0
#endif

#ifdef CLD_TIME_ATTR_DST_SHIFT
    #define ATTR_DST_SHIFT                               1
#else
    #define ATTR_DST_SHIFT                               0
#endif
#ifdef CLD_TIME_ATTR_STANDARD_TIME
    #define ATTR_STANDARD_TIME                           1
#else
    #define ATTR_STANDARD_TIME                           0
#endif
#ifdef CLD_TIME_ATTR_LOCAL_TIME
    #define ATTR_LOCAL_TIME                              1
#else
    #define ATTR_LOCAL_TIME                              0
#endif
#ifdef CLD_TIME_ATTR_LAST_SET_TIME
    #define ATTR_LAST_SET_TIME                           1
#else
    #define ATTR_LAST_SET_TIME                           0
#endif
#ifdef CLD_TIME_ATTR_VALID_UNTIL_TIME
    #define ATTR_VALID_UNTIL_TIME                        1
#else
    #define ATTR_VALID_UNTIL_TIME                        0
#endif

#ifndef CLD_TIME_CLUSTER_REVISION
    #define CLD_TIME_CLUSTER_REVISION                     1
#endif    

#ifndef CLD_TIME_FEATURE_MAP
    #define CLD_TIME_FEATURE_MAP                          0
#endif 

#define CLD_TIME_NUMBER_OF_OPTIONAL_ATTRIBUTE  \
    ATTR_TIME_ZONE           +                 \
    ATTR_DST_START           +                 \
    ATTR_DST_END             +                 \
    ATTR_DST_SHIFT           +                 \
    ATTR_STANDARD_TIME       +                 \
    ATTR_LOCAL_TIME          +                 \
    ATTR_LAST_SET_TIME       +                 \
    ATTR_VALID_UNTIL_TIME

#define CLD_TIME_MAX_NUMBER_OF_ATTRIBUTE       \
    CLD_TIME_NUMBER_OF_OPTIONAL_ATTRIBUTE +    \
    CLD_TIME_NUMBER_OF_MANDATORY_ATTRIBUTE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Time attribute set attribute ID's (3.12.2.2) */
    E_CLD_TIME_ATTR_ID_TIME                = 0x0000,  /* Mandatory */
    E_CLD_TIME_ATTR_ID_TIME_STATUS,                   /* Mandatory */
    E_CLD_TIME_ATTR_ID_TIME_ZONE,
    E_CLD_TIME_ATTR_ID_DST_START,
    E_CLD_TIME_ATTR_ID_DST_END,
    E_CLD_TIME_ATTR_ID_DST_SHIFT,
    E_CLD_TIME_ATTR_ID_STANDARD_TIME,
    E_CLD_TIME_ATTR_ID_LOCAL_TIME,
    E_CLD_TIME_ATTR_ID_LAST_SET_TIME,
    E_CLD_TIME_ATTR_ID_VALID_UNTIL_TIME,
} teCLD_TM_AttributeID;

/* Time Cluster */
typedef struct
{
#ifdef TIME_SERVER    
    zutctime                utctTime;

    zbmap8                  u8TimeStatus;

#ifdef CLD_TIME_ATTR_TIME_ZONE
    zint32                 i32TimeZone;
#endif

#ifdef CLD_TIME_ATTR_DST_START
    zuint32                u32DstStart;
#endif

#ifdef CLD_TIME_ATTR_DST_END
    zuint32                u32DstEnd;
#endif

#ifdef CLD_TIME_ATTR_DST_SHIFT
    zint32                 i32DstShift;
#endif

#ifdef CLD_TIME_ATTR_STANDARD_TIME
    zuint32                u32StandardTime;
#endif

#ifdef CLD_TIME_ATTR_LOCAL_TIME
    zuint32                u32LocalTime;
#endif

#ifdef CLD_TIME_ATTR_LAST_SET_TIME
    zutctime                u32LastSetTime;
#endif

#ifdef CLD_TIME_ATTR_VALID_UNTIL_TIME
    zutctime                u32ValidUntilTime;
#endif
#endif
    zbmap32                 u32FeatureMap;

    zuint16                 u16ClusterRevision;
} tsCLD_Time;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_TimeCreateTime(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern tsZCL_ClusterDefinition sCLD_Time;
extern const tsZCL_AttributeDefinition asCLD_TimeClusterAttributeDefinitions[];
extern uint8 au8TimeClusterAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* TIME_H */
