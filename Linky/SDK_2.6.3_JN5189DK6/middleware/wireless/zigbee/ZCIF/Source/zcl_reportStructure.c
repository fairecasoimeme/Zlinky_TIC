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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          Report Configuration
 *
 * DESCRIPTION:       Reportable Attribute deviation structure access functions
 *
 ****************************************************************************/
 
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_common.h"
#include "zcl_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCL_WriteToAttributeReportableStructure
 **
 ** DESCRIPTION:
 ** write data into user union
 **
 ** PARAMETERS:                 Name                        Usage
 ** void                       *ppvReportableChange         change value  
 ** teZCL_ZCLAttributeType      eAttributeDataType          attribute type
 ** tuZCL_AttributeReportable  *puAttributeReportableChange change union
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_WriteToAttributeReportableStructure(
                void                       *pvReportableChange,
                teZCL_ZCLAttributeType      eAttributeDataType,
                tuZCL_AttributeReportable  *puAttributeReportable)
{

    if((pvReportableChange==NULL) || (puAttributeReportable==NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    switch(eAttributeDataType)
    {

    /* Unsigned Integer */
        case(E_ZCL_UINT8):
        {
             puAttributeReportable->zuint8ReportableChange = *(zuint8 *)pvReportableChange;
             break;
        }
        case(E_ZCL_UINT16):
        {
             puAttributeReportable->zuint16ReportableChange = *(zuint16 *)pvReportableChange;
             break;
        }
        case(E_ZCL_UINT24):
        {
            puAttributeReportable->zuint24ReportableChange = *(zuint24 *)pvReportableChange;
            break;
        }
        case(E_ZCL_UINT32):
        {
            puAttributeReportable->zuint32ReportableChange = *(zuint32 *)pvReportableChange;
            break;
        }
        case(E_ZCL_UINT40):
        {
            puAttributeReportable->zuint40ReportableChange = *(zuint40 *)pvReportableChange;
            break;
        }
        case(E_ZCL_UINT48):
        {
            puAttributeReportable->zuint48ReportableChange = *(zuint48 *)pvReportableChange;
            break;
        }
        case(E_ZCL_UINT56):
        {
            puAttributeReportable->zuint56ReportableChange = *(zuint56 *)pvReportableChange;
            break;
        }
        case(E_ZCL_UINT64):
        {
            puAttributeReportable->zuint64ReportableChange = *(zuint64 *)pvReportableChange;
            break;
        }

    /* Signed Integer */
        case(E_ZCL_INT8):
        {
            puAttributeReportable->zint8ReportableChange = *(zint8 *)pvReportableChange;
            break;
        }
        
        case(E_ZCL_INT16):
        {   
            puAttributeReportable->zint16ReportableChange = *(zint16 *)pvReportableChange;
            break;
        }
        case(E_ZCL_INT24):
        {
            puAttributeReportable->zint24ReportableChange = *(zint24 *)pvReportableChange;
            break;
        }
        case(E_ZCL_INT32):
        {
            puAttributeReportable->zint32ReportableChange = *(zint32 *)pvReportableChange;
            break;
        }
        case(E_ZCL_INT40):
        {
            puAttributeReportable->zint40ReportableChange = *(zint40 *)pvReportableChange;
            break;
        }
        case(E_ZCL_INT48):
        {
            puAttributeReportable->zint48ReportableChange = *(zint48 *)pvReportableChange;
            break;
        }
        case(E_ZCL_INT56):
        {
            puAttributeReportable->zint56ReportableChange = *(zint56 *)pvReportableChange;
            break;
        }
        case(E_ZCL_INT64):
        {
            puAttributeReportable->zint64ReportableChange = *(zint64 *)pvReportableChange;
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
            puAttributeReportable->zsemiReportableChange = *(zsemi *)pvReportableChange;
            break;
        }
        case(E_ZCL_FLOAT_SINGLE):
        {
            puAttributeReportable->zsingleReportableChange = *(zsingle *)pvReportableChange;
            break;
        }
        case(E_ZCL_FLOAT_DOUBLE):
        {
            puAttributeReportable->zdoubleReportableChange = *(zdouble *)pvReportableChange;
            break;
        }
#endif
    /* Time */
        case(E_ZCL_TOD):
        {
            puAttributeReportable->ztimeReportableChange = *(ztime *)pvReportableChange;
            break;
        }
        case(E_ZCL_DATE):
        {
            puAttributeReportable->zdateReportableChange = *(zdate *)pvReportableChange;
            break;
        }
        case(E_ZCL_UTCT):
        {
            puAttributeReportable->zutctimeReportableChange = *(zutctime *)pvReportableChange;
            break;
        }

        default:
        {
            return(E_ZCL_FAIL);
        }
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_GetAttributeReportableEntry
 **
 ** DESCRIPTION:
 ** 
 **
 ** PARAMETERS:                 Name                        Usage
 ** void                      **ppvReportableChange         change value  
 ** teZCL_ZCLAttributeType      eAttributeDataType          attribute type
 ** tuZCL_AttributeReportable  *puAttributeReportableChange change union
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_GetAttributeReportableEntry(
                    void                      **ppvReportableChange, 
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    tuZCL_AttributeReportable  *puAttributeReportableChange)
{
    if((ppvReportableChange==NULL) || (puAttributeReportableChange==NULL))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    switch(eAttributeDataType)
    {

    /* Unsigned Integer */
        case(E_ZCL_UINT8):
        {
            
             *(zuint8 **)ppvReportableChange = &puAttributeReportableChange->zuint8ReportableChange;
             break;
        }
        case(E_ZCL_UINT16):
        {
             *(zuint16 **)ppvReportableChange = &puAttributeReportableChange->zuint16ReportableChange;
             break;
        }
        case(E_ZCL_UINT24):
        {
            *(zuint24 **)ppvReportableChange = &puAttributeReportableChange->zuint24ReportableChange;
            break;
        }
        case(E_ZCL_UINT32):
        {
            *(zuint32 **)ppvReportableChange = &puAttributeReportableChange->zuint32ReportableChange;
            break;
        }
        case(E_ZCL_UINT40):
        {
            *(zuint40 **)ppvReportableChange = &puAttributeReportableChange->zuint40ReportableChange;
            break;
        }
        case(E_ZCL_UINT48):
        {
            *(zuint48 **)ppvReportableChange = &puAttributeReportableChange->zuint48ReportableChange;
            break;
        }
        case(E_ZCL_UINT56):
        {
            *(zuint56 **)ppvReportableChange = &puAttributeReportableChange->zuint56ReportableChange;
            break;
        }
        case(E_ZCL_UINT64):
        {
            *(zuint64 **)ppvReportableChange = &puAttributeReportableChange->zuint64ReportableChange;
            break;
        }

    /* Signed Integer */
        case(E_ZCL_INT8):
        {
            *(zint8 **)ppvReportableChange = &puAttributeReportableChange->zint8ReportableChange;
            break;
        }
        
        case(E_ZCL_INT16):
        {   
            *(zint16 **)ppvReportableChange = &puAttributeReportableChange->zint16ReportableChange;
            break;
        }
        case(E_ZCL_INT24):
        {
            *(zint24 **)ppvReportableChange = &puAttributeReportableChange->zint24ReportableChange;
            break;
        }
        case(E_ZCL_INT32):
        {
            *(zint32 **)ppvReportableChange = &puAttributeReportableChange->zint32ReportableChange;
            break;
        }
        case(E_ZCL_INT40):
        {
            *(zint40 **)ppvReportableChange = &puAttributeReportableChange->zint40ReportableChange;
            break;
        }
        case(E_ZCL_INT48):
        {
            *(zint48 **)ppvReportableChange = &puAttributeReportableChange->zint48ReportableChange;
            break;
        }
        case(E_ZCL_INT56):
        {
            *(zint56 **)ppvReportableChange = &puAttributeReportableChange->zint56ReportableChange;
            break;
        }
        case(E_ZCL_INT64):
        {
            *(zint64 **)ppvReportableChange = &puAttributeReportableChange->zint64ReportableChange;
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
            *(zsemi **)ppvReportableChange = &puAttributeReportableChange->zsemiReportableChange;
            break;
        }
        case(E_ZCL_FLOAT_SINGLE):
        {
            *(zsingle **)ppvReportableChange = &puAttributeReportableChange->zsingleReportableChange;
            break;
        }
        case(E_ZCL_FLOAT_DOUBLE):
        {
            *(zdouble **)ppvReportableChange = &puAttributeReportableChange->zdoubleReportableChange;
            break;
        }
#endif

    /* Time */
        case(E_ZCL_TOD):
        {
            *(ztime **)ppvReportableChange = &puAttributeReportableChange->ztimeReportableChange;
            break;
        }
        case(E_ZCL_DATE):
        {
            *(zdate **)ppvReportableChange = &puAttributeReportableChange->zdateReportableChange;
            break;
        }
        case(E_ZCL_UTCT):
        {
            *(zutctime **)ppvReportableChange = &puAttributeReportableChange->zutctimeReportableChange;
            break;
        }

        default:
        {
            return(E_ZCL_FAIL);
        }
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_DoesAttributeHaveReportableChange
 **
 ** DESCRIPTION:
 ** 
 **
 ** PARAMETERS:             Name                           Usagee
 ** teZCL_ZCLAttributeType  eAttributeDataType
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_DoesAttributeHaveReportableChange(
                teZCL_ZCLAttributeType      eAttributeDataType)
{
  
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
    /* Signed Integer */
        case(E_ZCL_INT8):
        case(E_ZCL_INT16):
        case(E_ZCL_INT24):
        case(E_ZCL_INT32):
        case(E_ZCL_INT40):
        case(E_ZCL_INT48):
        case(E_ZCL_INT56):
        case(E_ZCL_INT64):
    /* Floating Point */
#ifdef ZCL_ENABLE_FLOAT
          // Floating point has been partially implemented in reporting but disabled with #ifdef ZCL_ENABLE_FLOAT
            // If floats are to be fully implemented, they must be definable at application compile
            // time by moving all the dependent functions to zcl_library_options.c.  Otherwise, the float
            // library will be pulled in by code that doesn't need it, adding approx 7k to the build.

        case(E_ZCL_FLOAT_SEMI):
        case(E_ZCL_FLOAT_SINGLE):
        case(E_ZCL_FLOAT_DOUBLE):
#endif
        /* Time */
        case(E_ZCL_TOD):
        case(E_ZCL_DATE):
        case(E_ZCL_UTCT):
        {
            break;
        }

        default:
        {
            return(E_ZCL_FAIL);
        }
    }

        return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
