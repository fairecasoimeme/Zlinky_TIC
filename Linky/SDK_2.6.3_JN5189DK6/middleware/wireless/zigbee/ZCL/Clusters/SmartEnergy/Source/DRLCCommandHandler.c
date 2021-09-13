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
 * DESCRIPTION:
 * DRLC event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "DRLC.h"
#include "DRLC_internal.h"
#include "zcl_options.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_DRLC) && !defined(DRLC_SERVER) && !defined(DRLC_CLIENT)
#error You Must Have either DRLC_SERVER and/or DRLC_CLIENT defined zcl_options.h
#endif
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
 ** NAME:       eSE_DRLCCommandHandler
 **
 ** DESCRIPTION:
 ** Handles DRLC Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eSE_DRLCCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsDRLC_Common *psDRLC_Common;

    // initialise pointer
    psDRLC_Common = &((tsSE_DRLCCustomDataStructure *)psClusterInstance->pvEndPointCustomStructPtr)->sDRLC_Common;

    // store the incoming event details which can be used to send events back to the
    // server at anytime
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psDRLC_Common->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
     u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    switch(psClusterInstance->bIsServer)
    {
#ifdef DRLC_SERVER
        // SERVER
        case(TRUE):
        {
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {
                case(SE_DRLC_REPORT_EVENT_STATUS):
                {
                    vSE_DRLCHandleReportEventStatus(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                case(SE_DRLC_GET_SCHEDULED_EVENTS):
                {
                    vSE_DRLCHandleGetScheduledEvents(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                default:
                {
                    return(E_ZCL_FAIL);
                    break;
                }
            }
            break;
        }
#endif

#ifdef DRLC_CLIENT
        case(FALSE):
        {
            // CLIENT
            switch(sZCL_HeaderParams.u8CommandIdentifier)
            {
                case(SE_DRLC_LOAD_CONTROL_EVENT):
                {
                    vSE_DRLCHandleLoadControlEvent(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                case(SE_DRLC_LOAD_CONTROL_EVENT_CANCEL):
                {
                    vSE_DRLCHandleLoadControlEventCancel(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                case(SE_DRLC_LOAD_CONTROL_EVENT_CANCEL_ALL):
                {
                    vSE_DRLCHandleLoadControlEventCancelAll(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                }
                default:
                {
                    return(E_ZCL_FAIL);
                    break;
                }
            }
            break;
        }
#endif
        // ifdef failsafe
        default:
        {
            return(E_ZCL_FAIL);
            break;
        }
    }

#if defined(DRLC_SERVER) || defined(DRLC_CLIENT)
    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);
#endif

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
