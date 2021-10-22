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


/*###############################################################################
#
# MODULE:      BDB
#
# COMPONENT:   bdb_fb_common.c
#
# DESCRIPTION: BDB Find & Bind Common functionality
#
#
###############################################################################*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <string.h>
#include "bdb_fb_api.h"
#include "bdb_api.h"
#include "zcl.h"
#include "Identify.h"
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
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       BDB_vFbZclHandler
 **
 ** DESCRIPTION:
 ** This function is used to to handle ZCL events
 **
 ** PARAMETERS:                 Name               Usage
 ** tsBDB_ZCLEvent              psCallBackEvent    BDB ZCL call back structure
 **
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void BDB_vFbZclHandler(uint8 u8EventType,tsZCL_CallBackEvent *psCallBackEvent)
{
    if(u8EventType == BDB_E_ZCL_EVENT_IDENTIFY_QUERY)
    {
        #if (defined BDB_SUPPORT_FIND_AND_BIND_INITIATOR)
            BDB_vFbHandleQueryResponse(psCallBackEvent);// identify query response
        #endif
    }
    else if(u8EventType == BDB_E_ZCL_EVENT_IDENTIFY)
    {
        #if (defined BDB_SUPPORT_FIND_AND_BIND_TARGET)
            BDB_vFbHandleStopIdentification(psCallBackEvent);// identify time
        #endif
    }
}
