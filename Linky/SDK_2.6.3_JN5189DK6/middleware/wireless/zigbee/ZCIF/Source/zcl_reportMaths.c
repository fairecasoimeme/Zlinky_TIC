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
 * MODULE:             Zigbee Demand Response And Load Control Cluster
 *
 * DESCRIPTION:        DRLC table manager
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include <stdlib.h>

#include "zcl_common.h"
#include "dlist.h"

#include "zcl.h"
#include "zcl_customcommand.h"

#include "zcl_internal.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void vZCL_AlignNumericAttributeValueTo64(
                    void                       *pvAttributeValueToStore,
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    uint64                     *pu64AttributeStorage);

PRIVATE teZCL_Status eZCL_CalculateReportableAttributeChange(
                    void                     *pvAttributeValue,
                    tsZCL_ReportRecord       *psAttributeReportingRecord);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCL_IndicateReportableChange
 **
 ** DESCRIPTION:
 ** Detect and maintain attribute changes
 **
 ** PARAMETERS:             Name                        Usage
 ** tsZCL_ReportRecord     *psAttributeReportingRecord
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCL_IndicateReportableChange(
                     tsZCL_ReportRecord        *psAttributeReportingRecord)
{
    teZCL_ZCLAttributeType     eAttributeDataType;
    uint8 u8typeSize;
    void *pvAttributeValue;

    if(psAttributeReportingRecord==NULL)
    {
        return(E_ZCL_FAIL);
    }

    eAttributeDataType = psAttributeReportingRecord->sAttributeReportingConfigurationRecord.eAttributeDataType;

    // check to see if reportable change is enabled
    if(psAttributeReportingRecord->sAttributeReportingConfigurationRecord.u16MinimumReportingInterval == REPORTING_MINIMUM_NOT_SET )
    {
        return(E_ZCL_FAIL);
    }

    // value - structure base address in cluster struct, offset in attribute structure
    pvAttributeValue = pvZCL_GetAttributePointer(psAttributeReportingRecord->psAttributeDefinition,
                                                 psAttributeReportingRecord->psClusterInstance,
                                                 psAttributeReportingRecord->sAttributeReportingConfigurationRecord.u16AttributeEnum);
    // deal with difference based reportable attributes
    if(eZCL_DoesAttributeHaveReportableChange(eAttributeDataType) == E_ZCL_SUCCESS)
    {
        return eZCL_CalculateReportableAttributeChange(pvAttributeValue, psAttributeReportingRecord);
    }

    switch(eAttributeDataType)
    {
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            if(eZCL_CheckForStringAttributeChange(pvAttributeValue, eAttributeDataType, psAttributeReportingRecord) == E_ZCL_SUCCESS)
            {
                return(E_ZCL_SUCCESS);
            }
            // Return fail if there is no change
            break;
        }

        // any other reportable structures are simply compared
        default:
        {
            uint8 *pu8ReportRecordStorage = (uint8 *)&psAttributeReportingRecord->uAttributeStorage.u64AttributeValue;
            // get type size
            eZCL_GetAttributeTypeSize(psAttributeReportingRecord->sAttributeReportingConfigurationRecord.eAttributeDataType, &u8typeSize);

            // Non difference based reportables are not 64 bit aligned but stored from start of record up to type size
            // This works better on '48 big endian platform.
            if(memcmp((uint8 *)pvAttributeValue, pu8ReportRecordStorage, u8typeSize)!= 0)
            {
                // If change write to start of previous storage - don't try to 64 bit align as get endian problems on '48.
                return(E_ZCL_SUCCESS);
            }
            // Return fail if there is no change
            break;
        }

    }

    // Return fail if there is no change
    return(E_ZCL_FAIL);

}

/****************************************************************************
 **
 ** NAME:       vZCL_StoreAttributeValue
 **
 ** DESCRIPTION:
 ** Stores Attribute value in report record
 **
 ** PARAMETERS:             Name                        Usage
 ** tsZCL_ClusterInstance        *psClusterInstance,
 ** tsZCL_AttributeDefinition    *psAttributeDefinition
 ** tsZCL_ReportRecord     *psAttributeReportingRecord
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCL_StoreChangeAttributeValue(
                     tsZCL_ClusterInstance        *psClusterInstance,
                     tsZCL_AttributeDefinition    *psAttributeDefinition,
                     tsZCL_ReportRecord           *psAttributeReportingRecord)
{
    teZCL_ZCLAttributeType     eAttributeDataType;
    uint64 u64AttributeScratch;
    int64 i64AttributeScratch;
    void  *pvAttributeValueToStore = pvZCL_GetAttributePointer(psAttributeDefinition,
                                                               psClusterInstance,
                                                               psAttributeReportingRecord->sAttributeReportingConfigurationRecord.u16AttributeEnum);

    eAttributeDataType = psAttributeReportingRecord->sAttributeReportingConfigurationRecord.eAttributeDataType;

    if(eZCL_DoesAttributeHaveReportableChange(eAttributeDataType) == E_ZCL_SUCCESS)
    {
        // Reportable values - need to take sign extension into consideration
        switch(eAttributeDataType)
        {
        case(E_ZCL_INT8):
        case(E_ZCL_INT16):
        case(E_ZCL_INT24):
        case(E_ZCL_INT32):
        case(E_ZCL_INT40):
        case(E_ZCL_INT48):
        case(E_ZCL_INT56):
        case(E_ZCL_INT64):
            {
                // Signed - this will extend the sign to 64 bits
                vZCL_AlignNumericAttributeValueTo64(pvAttributeValueToStore, eAttributeDataType, (uint64 *)&i64AttributeScratch);
                psAttributeReportingRecord->uAttributeStorage.i64AttributeValue = i64AttributeScratch;
                break;
            }
#ifdef ZCL_ENABLE_FLOAT
            // Floating point has been partially implemented in reporting but disabled with #ifdef ZCL_ENABLE_FLOAT
            // Since ZCL is now built from source, it should be possible to define this in the app's zcl_options.h

        case(E_ZCL_FLOAT_SEMI):
            {
                psAttributeReportingRecord->uAttributeStorage.zsemiValue = *(zsemi *)pvAttributeValueToStore;
                break;
            }
        case(E_ZCL_FLOAT_SINGLE):
            {
                psAttributeReportingRecord->uAttributeStorage.zsingleValue = *(zsingle *)pvAttributeValueToStore;
                break;
            }
        case(E_ZCL_FLOAT_DOUBLE):
            {
                psAttributeReportingRecord->uAttributeStorage.zdoubleValue = *(zdouble *)pvAttributeValueToStore;
                break;
            }
#endif
        default:
            {
                // Unsigned
                vZCL_AlignNumericAttributeValueTo64(pvAttributeValueToStore, eAttributeDataType, &u64AttributeScratch);
                psAttributeReportingRecord->uAttributeStorage.u64AttributeValue = u64AttributeScratch;
                break;
            }
        }
    }
    else
    {
        // Non reportable change types
        switch(eAttributeDataType)
        {
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
            {
                // If strings are to be used then storage will need allocating and assigning to
                // psAttributeReportingRecord->uAttributeStorage.sOctetString.pu8Data
                // I don't think reporting differences on strings has been tested.
                // So the record will initialise to a maxLength of zero and a NULL pointer.
                // It may then report no change and store zero bytes of string change info unless the string length changes.
                eZCL_CopyStringAttributeToPreviousValue( pvAttributeValueToStore, eAttributeDataType, psAttributeReportingRecord);
                break;
            }
            // any other reportable structures are simply compared
        default:
            {
                uint8 *pu8ReportRecordStorage = (uint8 *)&psAttributeReportingRecord->uAttributeStorage.u64AttributeValue;
                uint8 u8typeSize;
                // get type size
                eZCL_GetAttributeTypeSize(psAttributeReportingRecord->sAttributeReportingConfigurationRecord.eAttributeDataType, &u8typeSize);

                // If change write to start of previous storage - don't try to 64 bit align as get endian problems on '48.
                memcpy(pu8ReportRecordStorage, pvAttributeValueToStore, u8typeSize);
                break;
            }
        }
    }
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       vZCL_CalculateReportableAttributeChangeAndStore
 **
 ** DESCRIPTION:
 ** calculate attribute change
 **
 ** PARAMETERS:             Name              Usage
 ** void                   *pvAttributeValue
 ** tsZCL_ReportRecord     *psAttributeReportingRecord
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCL_CalculateReportableAttributeChange(
                    void                     *pvAttributeValue,
                    tsZCL_ReportRecord       *psAttributeReportingRecord)
{

    zuint64 u64AttributeScratch;
    zuint64 u64AttributeDifference;
    zuint64 u64AttributeReportableDifference;

    zint64 i64AttributeScratch;
    zint64 i64AttributeDifference;
    zint64 i64AttributeReportableDifference;

#ifdef ZCL_ENABLE_FLOAT
          // Floating point has been partially implemented in reporting but disabled with #ifdef ZCL_ENABLE_FLOAT
            // If floats are to be fully implemented, they must be definable at application compile
            // time by moving all the dependent functions to zcl_library_options.c.  Otherwise, the float
            // library will be pulled in by code that doesn't need it, adding approx 7k to the build.
    zsemi zsemiAttributeDifference;
    zsingle zsingleAttributeDifference;
    zdouble zdoubleAttributeDifference;
#endif
    void *pvReportableChange;
    // comparison variable

    teZCL_ZCLAttributeType eAttributeDataType;

    teZCL_Status eStatus = E_ZCL_FAIL;

    eAttributeDataType = psAttributeReportingRecord->sAttributeReportingConfigurationRecord.eAttributeDataType;

    // get reportable difference pointer reference
    eZCL_GetAttributeReportableEntry(&pvReportableChange, eAttributeDataType, &psAttributeReportingRecord->sAttributeReportingConfigurationRecord.uAttributeReportableChange);

    switch(eAttributeDataType)
    {

    /* Unsigned Integer */
        case(E_ZCL_UINT8):
        case(E_ZCL_UINT16):
        case(E_ZCL_UINT24):
        case(E_ZCL_UINT32):
        case(E_ZCL_UINT40):
        case(E_ZCL_UINT48):
        case(E_ZCL_UINT56):
        case(E_ZCL_UINT64):
        case(E_ZCL_TOD):
        case(E_ZCL_DATE):
        case(E_ZCL_UTCT):
        {
            // sort incoming attribute alignment
            vZCL_AlignNumericAttributeValueTo64(pvAttributeValue, eAttributeDataType, &u64AttributeScratch);
            // check reportable difference
            vZCL_AlignNumericAttributeValueTo64(pvReportableChange, eAttributeDataType, &u64AttributeReportableDifference);
            // subtract - always get positive result
            u64AttributeDifference = u64AttributeScratch - psAttributeReportingRecord->uAttributeStorage.u64AttributeValue;
            if(psAttributeReportingRecord->uAttributeStorage.u64AttributeValue > u64AttributeScratch)
            {
                u64AttributeDifference = psAttributeReportingRecord->uAttributeStorage.u64AttributeValue - u64AttributeScratch;
            }
            if(u64AttributeDifference >= u64AttributeReportableDifference)
            {
               eStatus = E_ZCL_SUCCESS;
            }
            break;
        }

    /* Signed Integer */
        case(E_ZCL_INT8):
        case(E_ZCL_INT16):
        case(E_ZCL_INT24):
        case(E_ZCL_INT32):
        case(E_ZCL_INT40):
        case(E_ZCL_INT48):
        case(E_ZCL_INT56):
        case(E_ZCL_INT64):
        {
            // sort incoming attribute alignment
            vZCL_AlignNumericAttributeValueTo64(pvAttributeValue, eAttributeDataType, (uint64 *)&i64AttributeScratch);
            // check reportable difference
            vZCL_AlignNumericAttributeValueTo64(pvReportableChange, eAttributeDataType, (uint64 *)&i64AttributeReportableDifference);
            // subtract - always get positive result
            i64AttributeDifference = i64AttributeScratch - psAttributeReportingRecord->uAttributeStorage.i64AttributeValue;
            if(psAttributeReportingRecord->uAttributeStorage.i64AttributeValue > i64AttributeScratch)
            {
                i64AttributeDifference = psAttributeReportingRecord->uAttributeStorage.i64AttributeValue - i64AttributeScratch;
            }
            // check reportable difference
            if(i64AttributeDifference > i64AttributeReportableDifference)
            {
               eStatus = E_ZCL_SUCCESS;
            }
            break;
        }

        /* Floating Point */
#ifdef ZCL_ENABLE_FLOAT
          // Floating point has been partially implemented in reporting but disabled with #ifdef ZCL_ENABLE_FLOAT
            // If floats are to be fully implemented, they must be definable at application compile
            // time by moving all the dependent functions to zcl_library_options.c.  Otherwise, the float
            // library will be pulled in by code that doesn't need it, adding approx 7k to the build.

        case(E_ZCL_FLOAT_SEMI):
        {
            // check reportable difference
            zsemiAttributeDifference = *(zsemi *)pvAttributeValue - psAttributeReportingRecord->uAttributeStorage.zsemiValue;
            if(*(zsemi *)pvAttributeValue < psAttributeReportingRecord->uAttributeStorage.zsemiValue)
            {
                zsemiAttributeDifference = psAttributeReportingRecord->uAttributeStorage.zsemiValue - *(zsemi *)pvAttributeValue;
            }

            if(zsemiAttributeDifference > *(zsemi *)pvReportableChange)
            {
               eStatus = E_ZCL_SUCCESS;
            }
            // store incoming value
            psAttributeReportingRecord->uAttributeStorage.zsemiValue = *(zsemi *)pvAttributeValue;
            break;
        }

        case(E_ZCL_FLOAT_SINGLE):
        {
            zsingleAttributeDifference = *(zsingle *)pvAttributeValue - psAttributeReportingRecord->uAttributeStorage.zsingleValue;
            if(*(zsingle *)pvAttributeValue < psAttributeReportingRecord->uAttributeStorage.zsingleValue)
            {
                zsingleAttributeDifference = psAttributeReportingRecord->uAttributeStorage.zsingleValue - *(zsingle *)pvAttributeValue;
            }
            if(zsingleAttributeDifference > *(zsingle *)pvReportableChange)
            {
               eStatus = E_ZCL_SUCCESS;
            }
            break;
        }
        case(E_ZCL_FLOAT_DOUBLE):
        {
            zdoubleAttributeDifference = *(zdouble *)pvAttributeValue - psAttributeReportingRecord->uAttributeStorage.zdoubleValue;
            if(*(zdouble *)pvAttributeValue < psAttributeReportingRecord->uAttributeStorage.zdoubleValue)
            {
                zdoubleAttributeDifference = psAttributeReportingRecord->uAttributeStorage.zdoubleValue - *(zdouble *)pvAttributeValue;
            }
            if(zdoubleAttributeDifference > *(zdouble *)pvReportableChange)
            {
               eStatus = E_ZCL_SUCCESS;
            }
            break;
        }
#endif
        default:
        {
            return(E_ZCL_FAIL);
        }

      }

      return(eStatus);

}

/****************************************************************************
 **
 ** NAME:       vZCL_AlignNumericAttributeValueTo64
 **
 ** DESCRIPTION:
 ** data alignment
 **
 ** PARAMETERS:             Name              Usage
 ** void                    *pvAttributeValueToStore
 ** uint64                  *pu64AttributeStorage
 ** teZCL_ZCLAttributeType  eAttributeDataType
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE  void vZCL_AlignNumericAttributeValueTo64(
                    void                       *pvAttributeValueToStore,
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    uint64                     *pu64AttributeStorage)
{

    switch(eAttributeDataType)
    {
        /* 8 bit integer value */
        case(E_ZCL_GINT8):
        case(E_ZCL_UINT8):
        case(E_ZCL_INT8):
        case(E_ZCL_ENUM8):
        case(E_ZCL_BMAP8):
        case(E_ZCL_BOOL):
        {
            uint8 u8Val = *(uint8 *)pvAttributeValueToStore;
            *pu64AttributeStorage = u8Val;
            // account for signed-ness
            if(eAttributeDataType == E_ZCL_INT8)
            {
                // sign extend if top bit set
                if(u8Val &0x80)
                {
                    *pu64AttributeStorage |= ((uint64)(0xffffffffffffffLL) << 8);
                }
            }
            break;
        }

        /* 16 bit integer value */
        case(E_ZCL_GINT16):
        case(E_ZCL_UINT16):
        case(E_ZCL_ENUM16):
        case(E_ZCL_INT16):
        case(E_ZCL_CLUSTER_ID):
        case(E_ZCL_ATTRIBUTE_ID):
        case(E_ZCL_BMAP16):
        {
            uint16 u16Val = *(uint16 *)pvAttributeValueToStore;
            *pu64AttributeStorage = u16Val;
            // account for signed-ness
            if(eAttributeDataType == E_ZCL_INT16)
            {
                // sign extend if top bit set
                if(*pu64AttributeStorage & 0x8000)
                {
                    *pu64AttributeStorage |= ((uint64)(0xffffffffffffLL) << 16);
                }
            }
            break;
        }
        /* 32 bit integer value */
        case(E_ZCL_UINT32):
        case(E_ZCL_INT32):
        case(E_ZCL_GINT32):
        case(E_ZCL_BMAP32):
        case(E_ZCL_UTCT):
        case(E_ZCL_TOD):
        case(E_ZCL_DATE):

        /* 24-bit stored as 32 bit integer value */
        case(E_ZCL_GINT24):
        case(E_ZCL_UINT24):
        case(E_ZCL_INT24):
        case(E_ZCL_BMAP24):
        {
            uint32 u32Val = *(uint32 *)pvAttributeValueToStore;
            *pu64AttributeStorage = u32Val;

            // account for signed-ness
            if(eAttributeDataType == E_ZCL_INT32)
            {
                // sign extend if top bit set
                if(*pu64AttributeStorage &0x80000000)
                {
                    *pu64AttributeStorage |= ((uint64)(0xffffffff) << 32);
                }
            }

            break;
        }
        /* 64 bit integer value */
        case(E_ZCL_GINT64):
        case(E_ZCL_UINT64):
        case(E_ZCL_INT64):
        case(E_ZCL_BMAP64):
        /* 40-bit stored as 64 bit integer value */
        case(E_ZCL_GINT40):
        case(E_ZCL_UINT40):
        case(E_ZCL_INT40):
        case(E_ZCL_BMAP40):
        /* 48-bit stored as 64 bit integer value */
        case(E_ZCL_GINT48):
        case(E_ZCL_UINT48):
        case(E_ZCL_INT48):
        case(E_ZCL_BMAP48):
        /* 56-bit stored as 64 bit integer value */
        case(E_ZCL_GINT56):
        case(E_ZCL_UINT56):
        case(E_ZCL_INT56):
        case(E_ZCL_BMAP56):
        {
            *pu64AttributeStorage = *(uint64 *)pvAttributeValueToStore;
            break;
        }

        // unrecognised
        default:
        {
            break;
        }
    }
}

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

