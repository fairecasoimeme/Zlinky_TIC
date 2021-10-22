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
 * MODULE:             Appliance Identification Cluster
 *
 * COMPONENT:          ApplianceIdentification.h
 *
 * DESCRIPTION:        Header for Appliance Identification Cluster
 *
 *****************************************************************************/

#ifndef APPLIANCE_IDENTIFICATION_H
#define APPLIANCE_IDENTIFICATION_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/*             Appliance Identification Cluster - Optional Attributes                          */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Appliance Identification cluster.                                         */
/****************************************************************************/

/* Enable the optional Company name attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_COMPANY_NAME

/* Enable the optional Company ID attribute*/
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_COMPANY_ID

/* Enable the optional brand name attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_BRAND_NAME

/* Enable the optional brand ID attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_BRAND_ID

/* Enable the optional Model attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_MODEL

/* Enable the optional Part Number attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_PART_NUMBER

/* Enable the optional Product Revision attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_PRODUCT_REVISION

/* Enable the optional Software Revision attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_SOFTWARE_REVISION

/* Enable the optional Product Type Name attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_PRODUCT_TYPE_NAME

/* Enable the optional Product Type ID attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_PRODUCT_TYPE_ID

/* Enable the optional Disable CECED Specification Version attribute */
//#define   CLD_APPLIANCE_IDENTIFICATION_ATTR_CECED_SPEC_VERSION
/* End of optional attributes */

/* Cluster ID's */
#define APPLIANCE_MANAGEMENT_CLUSTER_ID_APPLIANCE_IDENTIFICATION                      0x0b00

/* Bitmap of Attribute Basic Identification */
#ifndef CLD_APPLIANCE_IDENTIFICATION_COMPANY_ID
#define CLD_APPLIANCE_IDENTIFICATION_COMPANY_ID                (0)
#endif
#ifndef CLD_APPLIANCE_IDENTIFICATION_BRAND_ID
#define CLD_APPLIANCE_IDENTIFICATION_BRAND_ID                  (0)
#endif
#ifndef CLD_APPLIANCE_IDENTIFICATION_PRODUCT_TYPE_ID
#define CLD_APPLIANCE_IDENTIFICATION_PRODUCT_TYPE_ID           (0)
#endif
#ifndef CLD_APPLIANCE_IDENTIFICATION_SPEC_VERSION
#define CLD_APPLIANCE_IDENTIFICATION_SPEC_VERSION              ((uint64)0)
#endif

#ifndef CLD_APPLIANCE_IDENTIFICATION_ATTR_BASIC_IDENTIFICATION
    #define CLD_APPLIANCE_IDENTIFICATION_ATTR_BASIC_IDENTIFICATION          (CLD_APPLIANCE_IDENTIFICATION_COMPANY_ID | \
                                                                            (CLD_APPLIANCE_IDENTIFICATION_BRAND_ID << 15) |  \
                                                                            (CLD_APPLIANCE_IDENTIFICATION_PRODUCT_TYPE_ID << 31) |  \
                                                                            ((CLD_APPLIANCE_IDENTIFICATION_SPEC_VERSION & 0x7)<< 47))
#endif

#ifndef CLD_APPLIANCE_IDENTIFICATION_CLUSTER_REVISION
    #define CLD_APPLIANCE_IDENTIFICATION_CLUSTER_REVISION                     1
#endif

#ifndef CLD_APPLIANCE_IDENTIFICATION_FEATURE_MAP
    #define CLD_APPLIANCE_IDENTIFICATION_FEATURE_MAP                          0
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Basic Appliance Identification attribute set attribute ID's (9.8.2.2) */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_BASIC_IDENTIFICATION        = 0x0000, /* Mandatory */
    /* Extended Appliance Identification attribute set attribute ID's (9.8.2.4)*/
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_COMPANY_NAME                = 0x0010, /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_COMPANY_ID,                           /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_BRAND_NAME,                           /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_BRAND_ID,                             /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_MODEL,                                /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_PART_NUMBER,                          /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_PRODUCT_REVISION,                     /* Optional */                      
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_SOFTWARE_REVISION,                    /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_PRODUCT_TYPE_NAME,                    /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_PRODUCT_TYPE_ID,                      /* Optional */
    E_CLD_APPLIANCE_IDENTIFICATION_ATTR_ID_CECED_SPEC_VERSION,                   /* Optional */
} teCLD_ApplianceIdentification_Cluster_AttrID;


/* Product Type ID */
typedef enum 
{
    E_CLD_AI_PT_ID_WHITE_GOODS                                             = 0x0000,
    E_CLD_AI_PT_ID_DISHWASHER                                              = 0x5601,
    E_CLD_AI_PT_ID_TUMBLE_DRYER,
    E_CLD_AI_PT_ID_WASHER_DRYER,
    E_CLD_AI_PT_ID_WASHING_MACHINE,
    E_CLD_AI_PT_ID_HOBS                                                    = 0x5E03,
    E_CLD_AI_PT_ID_INDUCTION_HOBS                                          = 0x5E09,
    E_CLD_AI_PT_ID_OVEN                                                    = 0x5E01,
    E_CLD_AI_PT_ID_ELECTRICAL_OVEN                                         = 0x5E06,
    E_CLD_AI_PT_ID_REFRIGERATOR_FREEZER                                    = 0x6601
} teCLD_ApplianceIdentification_ProductTypeId;



/* Appliance Identification Cluster */
typedef struct
{
#ifdef APPLIANCE_IDENTIFICATION_SERVER    
    zbmap56                      u64BasicIdentification;

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_COMPANY_NAME
    tsZCL_CharacterString       sCompanyName;
    uint8                       au8CompanyName[16];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_COMPANY_ID
    zuint16                      u16CompanyId;
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_BRAND_NAME
    tsZCL_CharacterString       sBrandName;
    uint8                       au8BrandName[16];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_BRAND_ID
    zuint16                      u16BrandId;
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_MODEL
    tsZCL_OctetString            sModel;
    uint8                       au8Model[16];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_PART_NUMBER
    tsZCL_OctetString            sPartNumber;
    uint8                       au8PartNumber[16];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_PRODUCT_REVISION
    tsZCL_OctetString            sProductRevision;
    uint8                       au8ProductRevision[6];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_SOFTWARE_REVISION
    tsZCL_OctetString            sSoftwareRevision;
    uint8                       au8SoftwareRevision[6];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_PRODUCT_TYPE_NAME
    tsZCL_OctetString            sProductTypeName;
    uint8                       au8ProductTypeName[2];
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_PRODUCT_TYPE_ID
    zuint16                      u16ProductTypeId;
#endif

#ifdef CLD_APPLIANCE_IDENTIFICATION_ATTR_CECED_SPEC_VERSION
    zuint8                       u8CECEDSpecificationVersion;
#endif
#endif
    zbmap32                      u32FeatureMap;

    zuint16                      u16ClusterRevision;

} tsCLD_ApplianceIdentification;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_ApplianceIdentificationCreateApplianceIdentification(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_ApplianceIdentification;
extern const tsZCL_AttributeDefinition asCLD_ApplianceIdentificationClusterAttributeDefinitions[];
extern uint8 au8ApplianceIdentificationAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* APPLIANCE_IDENTIFICATION_H */
