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
 * MODULE:             Binary Input Basic Cluster
 *
 * COMPONENT:          BinaryInputBasic.h
 *
 * DESCRIPTION:        Header for Binary Input Basic Cluster
 *
 *****************************************************************************/

#ifndef BINARY_INPUT_BASIC_H
#define BINARY_INPUT_BASIC_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*             Binary Input Basic Cluster - Optional Attributes                          */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the binary input basic cluster.                                         */
/****************************************************************************/

/* Enable the optional Active Text attribute */
//#define   CLD_BINARY_INPUT_BASIC_ATTR_ACTIVE_TEXT

/* Enable the optional Description attribute*/
//#define   CLD_BINARY_INPUT_BASIC_ATTR_DESCRIPTION

/* Enable the optional Inactive text attribute */
//#define   CLD_BINARY_INPUT_BASIC_ATTR_INACTIVE_TEXT

/* Enable the optional Polarity attribute */
//#define   CLD_BINARY_INPUT_BASIC_ATTR_POLARITY

/* Enable the optional Reliability attribute */
//#define   CLD_BINARY_INPUT_BASIC_ATTR_RELIABILITY

/* Enable the optional Application type attribute */
//#define   CLD_BINARY_INPUT_BASIC_ATTR_APPLICATION_TYPE

/* Enable the optional Attribute Reporting Status attribute */
//#define   CLD_BINARY_INPUT_BASIC_ATTR_ATTRIBUTE_REPORTING_STATUS

/* End of optional attributes */


/* Cluster ID's */
#define GENERAL_CLUSTER_ID_BINARY_INPUT_BASIC                            0x000F


/* 3.14.10.12 & 3.14.10.13 If either the ActiveText attribute or the InactiveText attribute are present, then both of them shall be
Present */
#if((defined CLD_BINARY_INPUT_BASIC_ATTR_ACTIVE_TEXT) || (defined CLD_BINARY_INPUT_BASIC_ATTR_INACTIVE_TEXT))
    #ifndef CLD_BINARY_INPUT_BASIC_ATTR_ACTIVE_TEXT
        #define CLD_BINARY_INPUT_BASIC_ATTR_ACTIVE_TEXT
    #endif
    #ifndef     CLD_BINARY_INPUT_BASIC_ATTR_INACTIVE_TEXT
        #define CLD_BINARY_INPUT_BASIC_ATTR_INACTIVE_TEXT
    #endif
#endif

#ifndef CLD_BINARY_INPUT_BASIC_CLUSTER_REVISION
    #define CLD_BINARY_INPUT_BASIC_CLUSTER_REVISION                         1
#endif 

#ifndef CLD_BINARY_INPUT_BASIC_FEATURE_MAP
    #define CLD_BINARY_INPUT_BASIC_FEATURE_MAP                              0
#endif 

/* Status Flags - bit definitions and bit masks */
#define BINARY_INPUT_BASIC_STATUS_FLAGS_IN_ALARM                              0
#define BINARY_INPUT_BASIC_STATUS_FLAGS_FAULT                                 1
#define BINARY_INPUT_BASIC_STATUS_FLAGS_OVERRIDDEN                            2
#define BINARY_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE                        3

#define BINARY_INPUT_BASIC_STATUS_FLAGS_IN_ALARM_MASK                          (1 << BINARY_INPUT_BASIC_STATUS_FLAGS_IN_ALARM)
#define BINARY_INPUT_BASIC_STATUS_FLAGS_FAULT_MASK                             (1 << BINARY_INPUT_BASIC_STATUS_FLAGS_FAULT)
#define BINARY_INPUT_BASIC_STATUS_FLAGS_OVERRIDDEN_MASK                        (1 << BINARY_INPUT_BASIC_STATUS_FLAGS_OVERRIDDEN)
#define BINARY_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE_MASK                    (1 << BINARY_INPUT_BASIC_STATUS_FLAGS_OUT_OF_SERVICE)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Command codes */


typedef enum 
{
    /* Binary Input (Basic) Device Information attribute set attribute ID's (3.2.2.2.1) */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_ACTIVE_TEXT                    =                0x0004, /* Optional */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_DESCRIPTION                    =                0x001C, /*Optional */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_INACTIVE_TEXT                  =                0x002E, /*Optional */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_OUT_OF_SERVICE                 =                0x0051, /*Mandatory */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_POLARITY                       =                0x0054, /*Optional */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_PRESENT_VALUE                  =                0x0055, /* Mandatory */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_RELIABILITY                    =                0x0067, /*Optional */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_STATUS_FLAGS                   =                0x006F, /* Mandatory */
    E_CLD_BINARY_INPUT_BASIC_ATTR_ID_APPLICATION_TYPE               =                0x0100, /*Optional */
} teCLD_BinaryInputBasicCluster_AttrID;



/* Polarity attribute */
typedef enum 
{ 
    E_CLD_BINARY_INPUT_BASIC_POLARITY_NORMAL                       =                0x0000,
    E_CLD_BINARY_INPUT_BASIC_POLARITY_REVERSE
}teCLD_BinaryInputBasic_Polarity;


/* Reliability attribute */
typedef enum 
{ 
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_NO_FAULT_DETECTED         =                0x0000,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_NO_SENSOR,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_OVER_RANGE,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_UNDER_RANGE,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_OPEN_LOOP,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_SHORTED_LOOP,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_NO_OUTPUT,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_UNRELIABLE_OTHER,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_PROCESS_ERROR,
                E_CLD_BINARY_INPUT_BASIC_RELIABILITY_CONFIGURATION_ERROR       =                0x000A 
 }teCLD_BinaryInputBasic_Reliability;


/* Binary Input Basic Cluster */
#ifdef BINARY_INPUT_BASIC_SERVER
typedef struct
{
#ifdef CLD_BINARY_INPUT_BASIC_ATTR_ACTIVE_TEXT
    tsZCL_CharacterString       sActiveText;
    uint8                       au8ActiveText[16];
#endif

#ifdef CLD_BINARY_INPUT_BASIC_ATTR_DESCRIPTION                
    tsZCL_CharacterString       sDescription;
    uint8                       au8Description[16];
#endif

#ifdef CLD_BINARY_INPUT_BASIC_ATTR_INACTIVE_TEXT
    tsZCL_CharacterString       sInactiveText;
    uint8                       au8InactiveText[16];
#endif

    zbool                       bOutOfService;

#ifdef CLD_BINARY_INPUT_BASIC_ATTR_POLARITY
    zenum8                      u8Polarity;
#endif

    zbool                       bPresentValue;

#ifdef CLD_BINARY_INPUT_BASIC_ATTR_RELIABILITY
    zenum8                      u8Reliability;
#endif

    zbmap8                      u8StatusFlags;

#ifdef CLD_BINARY_INPUT_BASIC_ATTR_APPLICATION_TYPE
    zuint32                     u32ApplicationType;
#endif
    
#ifdef CLD_BINARY_INPUT_BASIC_ATTR_ATTRIBUTE_REPORTING_STATUS
    zenum8                      u8AttributeReportingStatus;
#endif

    zbmap32                     u32FeatureMap;

    zuint16                     u16ClusterRevision;

} tsCLD_BinaryInputBasic;
#endif

#ifdef BINARY_INPUT_BASIC_CLIENT
typedef struct
{
    zbmap32                     u32FeatureMap;
  
    zuint16                     u16ClusterRevision;
} tsCLD_BinaryInputBasicClient;
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_BinaryInputBasicCreateBinaryInputBasic(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);
                            
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
#ifdef BINARY_INPUT_BASIC_SERVER
    extern tsZCL_ClusterDefinition sCLD_BinaryInputBasic;
    extern tsZCL_AttributeDefinition asCLD_BinaryInputBasicClusterAttributeDefinitions[];
    extern uint8 au8BinaryInputBasicAttributeControlBits[];
#endif
#ifdef BINARY_INPUT_BASIC_CLIENT
    extern tsZCL_ClusterDefinition sCLD_BinaryInputBasicClient;
    extern tsZCL_AttributeDefinition asCLD_BinaryInputBasicClientClusterAttributeDefinitions[];
    extern uint8 au8BinaryInputBasicClientAttributeControlBits[];
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* BASIC_H */
