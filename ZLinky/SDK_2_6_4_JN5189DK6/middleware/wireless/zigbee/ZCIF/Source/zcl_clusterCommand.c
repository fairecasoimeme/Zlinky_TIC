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
 * COMPONENT:          zcl_attribute.c
 *
 * DESCRIPTION:       Attribute read and write functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"
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
 ** NAME:       bZCL_CheckManufacturerSpecificCommandFlagMatch
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                    Usage
 ** tsZCL_CommandDefinition  *psCommandDefinition   Attribute data type struct
 ** bool_t                      bManufacturerSpecificAttributeFlag
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC bool_t bZCL_CheckManufacturerSpecificCommandFlagMatch(
                    tsZCL_CommandDefinition  *psCommandDefinition,
                    bool_t                    bManufacturerSpecificCommandFlag)
{

    bool_t bLocalManufacturerSpecificCommandFlag = FALSE;

    if(psCommandDefinition==NULL)
    {
        return(FALSE);
    }
    // get attribute flag and set bool flag
    if(psCommandDefinition->u8CommandFlags & E_ZCL_CF_MS)
    {
        bLocalManufacturerSpecificCommandFlag = TRUE;
    }
    // compare
    if(bLocalManufacturerSpecificCommandFlag == bManufacturerSpecificCommandFlag)
    {
        return(TRUE);
    }
    return(FALSE);
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/



