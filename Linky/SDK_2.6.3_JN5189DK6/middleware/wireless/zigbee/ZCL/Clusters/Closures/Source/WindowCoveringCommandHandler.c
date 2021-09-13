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
 * MODULE:             Window Covering Cluster
 *
 * COMPONENT:          Clusters
 *
 * DESCRIPTION:        Message event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "WindowCovering.h"
#include "WindowCovering_internal.h"

#ifdef CLD_SCENES
#include "Scenes.h"
#include "Scenes_internal.h"
#endif

#include "zcl_options.h"

#include "dbg.h"
#ifdef DEBUG_CLD_WINDOW_COVERING
#define TRACE_WINDOW_COVERING TRUE
#else
#define TRACE_WINDOW_COVERING FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_WINDOW_COVERING) && !defined(WINDOW_COVERING_SERVER) && !defined(WINDOW_COVERING_CLIENT)
#error You Must Have either WINDOW_COVERING_SERVER and/or WINDOW_COVERING_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

#ifdef WINDOW_COVERING_SERVER
PRIVATE teZCL_Status eCLD_WindowCoveringHandleUpOpenCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);

PRIVATE teZCL_Status eCLD_WindowCoveringHandleDownCloseCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);

PRIVATE teZCL_Status eCLD_WindowCoveringHandleStopCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);

#ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
PRIVATE teZCL_Status eCLD_WindowCoveringHandleGoToLiftValueCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif

#ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
PRIVATE teZCL_Status eCLD_WindowCoveringHandleGoToLiftPercentageCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
PRIVATE teZCL_Status eCLD_WindowCoveringHandleGoToTiltValueCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
PRIVATE teZCL_Status eCLD_WindowCoveringHandleGoToTiltPercentageCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif
#endif /* WINDOW_COVERING_SERVER */

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
 ** NAME:       eCLD_WindowCoveringCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Message Cluster custom commands
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

PUBLIC  teZCL_Status eCLD_WindowCoveringCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    // We don't handle messages as a client, so exit
    if(psClusterInstance->bIsServer == FALSE)
    {
        return(E_ZCL_FAIL);
    }

#ifdef WINDOW_COVERING_SERVER
{
    tsZCL_CallBackEvent                 sWindowCoveringCustomCallBackEvent;
    tsCLD_WindowCoveringCallBackMessage sWindowCoveringCallBackMessage;
    bool_t bCallback = FALSE;
    tsZCL_HeaderParams sZCL_HeaderParams;
    DBG_vPrintf(TRACE_WINDOW_COVERING, "\r\nWC: ");

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // SERVER
    switch(sZCL_HeaderParams.u8CommandIdentifier)
    {

    case(E_CLD_WC_CMD_UP_OPEN):
        eCLD_WindowCoveringHandleUpOpenCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        /* Use basic callback */
        bCallback = TRUE;
        break;

    case(E_CLD_WC_CMD_DOWN_CLOSE):
        eCLD_WindowCoveringHandleDownCloseCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        /* Use basic callback */
        bCallback = TRUE;
        break;

    case(E_CLD_WC_CMD_STOP):
        eCLD_WindowCoveringHandleStopCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        /* Use basic callback */
        bCallback = TRUE;
        break;

    #ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
    case E_CLD_WC_CMD_GO_TO_LIFT_VALUE:
        eCLD_WindowCoveringHandleGoToLiftValueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;
    #endif

    #ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
    case E_CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE:
        eCLD_WindowCoveringHandleGoToLiftPercentageCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;
    #endif

    #ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
    case E_CLD_WC_CMD_GO_TO_TILT_VALUE:
        eCLD_WindowCoveringHandleGoToTiltValueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;
    #endif

    #ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
    case E_CLD_WC_CMD_GO_TO_TILT_PERCENTAGE:
        eCLD_WindowCoveringHandleGoToTiltPercentageCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;
    #endif
    default:
        // unlock
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
        break;
    }

    /* Send basic callback ? */
    if (bCallback)
    {
        /* Generate a custom command event */
        eZCL_SetCustomCallBackEvent(&sWindowCoveringCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
        sWindowCoveringCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
        sWindowCoveringCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
        sWindowCoveringCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&sWindowCoveringCallBackMessage;
        sWindowCoveringCustomCallBackEvent.psClusterInstance = psClusterInstance;

        /* Fill in message */
        sWindowCoveringCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

        // call callback
        psEndPointDefinition->pCallBackFunctions(&sWindowCoveringCustomCallBackEvent);
    }

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    vCLD_ScenesUpdateSceneValid(psEndPointDefinition);
#endif

    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}
#endif //WINDOW_COVERING_SERVER

    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

#ifdef WINDOW_COVERING_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleUpOpenCommand
 **
 ** DESCRIPTION:
 ** Handles an On command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleUpOpenCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "UpOpen: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandReceive(pZPSevent,
                                    &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleDownCloseCommand
 **
 ** DESCRIPTION:
 ** Handles an Off command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleDownCloseCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "DownClose: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandReceive(pZPSevent,
                                    &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleStopCommand
 **
 ** DESCRIPTION:
 ** Handles an Off command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleStopCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "Stop: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandReceive(pZPSevent,
                                    &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    return eStatus;
}

#ifdef CLD_WC_CMD_GO_TO_LIFT_VALUE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleGoToLiftValueCommand
 **
 ** DESCRIPTION:
 ** Handles an Off with effect command, See ZCL Specification 3.8.2.3.4.3
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleGoToLiftValueCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WindowCovering_GoToLiftValuePayload sPayload;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_WindowCoveringCallBackMessage sCallBackMessage;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "GoToLiftValue: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandGoToLiftValueReceive(pZPSevent,
                                &u8TransactionSequenceNumber,
                                &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /* Generate a callback to let the app know that we need to do a go to lift value */
    sCallBackMessage.u8CommandId                     = u8CommandIdentifier;
    sCallBackMessage.uMessage.psGoToLiftValuePayload = &sPayload;

    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = pZPSevent;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_CUSTOM;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = CLUSTER_ID_WINDOW_COVERING;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&sCallBackMessage;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    return eStatus;
}
#endif

#ifdef CLD_WC_CMD_GO_TO_LIFT_PERCENTAGE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleGoToLiftPercentageCommand
 **
 ** DESCRIPTION:
 ** Handles an Off with effect command, See ZCL Specification 3.8.2.3.4.3
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleGoToLiftPercentageCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WindowCovering_GoToLiftPercentagePayload sPayload;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_WindowCoveringCallBackMessage sCallBackMessage;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "GoToLiftPercentage: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandGoToLiftPercentageReceive(pZPSevent,
                                &u8TransactionSequenceNumber,
                                &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /* Generate a callback to let the app know that we need to do a go to lift value */
    sCallBackMessage.u8CommandId                     = u8CommandIdentifier;
    sCallBackMessage.uMessage.psGoToLiftPercentagePayload = &sPayload;

    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = pZPSevent;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_CUSTOM;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = CLUSTER_ID_WINDOW_COVERING;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&sCallBackMessage;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    return eStatus;
}
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_VALUE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleGoToTiltValueCommand
 **
 ** DESCRIPTION:
 ** Handles an Off with effect command, See ZCL Specification 3.8.2.3.4.3
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleGoToTiltValueCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WindowCovering_GoToTiltValuePayload sPayload;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_WindowCoveringCallBackMessage sCallBackMessage;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "GoToTiltValue: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandGoToTiltValueReceive(pZPSevent,
                                &u8TransactionSequenceNumber,
                                &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /* Generate a callback to let the app know that we need to do a go to lift value */
    sCallBackMessage.u8CommandId                     = u8CommandIdentifier;
    sCallBackMessage.uMessage.psGoToTiltValuePayload = &sPayload;

    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = pZPSevent;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_CUSTOM;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = CLUSTER_ID_WINDOW_COVERING;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&sCallBackMessage;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    return eStatus;
}
#endif

#ifdef CLD_WC_CMD_GO_TO_TILT_PERCENTAGE
/****************************************************************************
 **
 ** NAME:       eCLD_WindowCoveringHandleGoToTiltPercentageCommand
 **
 ** DESCRIPTION:
 ** Handles an Off with effect command, See ZCL Specification 3.8.2.3.4.3
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_WindowCoveringHandleGoToTiltPercentageCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_WindowCovering_GoToTiltPercentagePayload sPayload;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_WindowCoveringCallBackMessage sCallBackMessage;

    DBG_vPrintf(TRACE_WINDOW_COVERING, "GoToTiltPercentage: ");

    /* Receive the command */
    eStatus = eCLD_WindowCoveringCommandGoToTiltPercentageReceive(pZPSevent,
                                &u8TransactionSequenceNumber,
                                &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_WINDOW_COVERING, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /* Generate a callback to let the app know that we need to do a go to lift value */
    sCallBackMessage.u8CommandId                     = u8CommandIdentifier;
    sCallBackMessage.uMessage.psGoToTiltPercentagePayload = &sPayload;

    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = pZPSevent;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_CUSTOM;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = CLUSTER_ID_WINDOW_COVERING;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&sCallBackMessage;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    return eStatus;
}
#endif
#endif /* WINDOW_COVERING_SERVER */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
