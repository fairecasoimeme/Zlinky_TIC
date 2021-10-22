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
 * COMPONENT:          zcl_buffer.c
 *
 * DESCRIPTION:        ZCL Buffer Stack Wrapper. 
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_common.h"
#include "zcl_internal.h"
#include "pdum_apl.h"

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
/***        Imported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       hZCL_AllocateAPduInstance
 **
 ** DESCRIPTION:
 ** ZCL wrapper to get a stack buffer
 **
 ** PARAMETERS:                 Name                      Usage
 ** Nothing
 **
 ** RETURN:
 ** PDUM_thAPduInstance
 **
 ****************************************************************************/

PUBLIC PDUM_thAPduInstance hZCL_AllocateAPduInstance(void)
{
    tsZCL_CallBackEvent sZCL_CallBackEvent;

    PDUM_thAPduInstance pdum_tsAPduInstance;
    
    pdum_tsAPduInstance = PDUM_hAPduAllocateAPduInstance(psZCL_Common->hZCL_APdu);

    if(pdum_tsAPduInstance==PDUM_INVALID_HANDLE)
    {
        // fill in non-attribute specific values in callback event structure
        memset(&sZCL_CallBackEvent, 0, sizeof(tsZCL_CallBackEvent));
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_ERROR;
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ZBUFFER_FAIL;
        // general c/b
        psZCL_Common->pfZCLinternalCallBackFunction(&sZCL_CallBackEvent);
    }

    return pdum_tsAPduInstance;
}

/****************************************************************************
 **
 ** NAME:       hZCL_GetBufferPoolHandle
 **
 ** DESCRIPTION:
 ** Returns number of ZCL buffer pool handle
 **
 ** PARAMETERS:               Name                    Usage
 ** none
 **
 ** RETURN:
 ** PDUM_thAPdu
 **
 ****************************************************************************/

PUBLIC PDUM_thAPdu hZCL_GetBufferPoolHandle(void)
{
    PDUM_thAPdu hZCL_APdu;

	#ifndef COOPERATIVE
    // get ZCL mutex
        vZCL_GetInternalMutex();
    #endif

    hZCL_APdu = psZCL_Common->hZCL_APdu;
    #ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
    #endif
    return(hZCL_APdu);

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
