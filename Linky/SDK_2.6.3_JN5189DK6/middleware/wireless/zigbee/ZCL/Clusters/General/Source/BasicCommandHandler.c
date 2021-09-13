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
 * MODULE:             Basic Cluster
 *
 * DESCRIPTION:
 * Message event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "Basic.h"
#include "Basic_internal.h"

#include "zcl_options.h"

#include "dbg.h"

#ifdef DEBUG_CLD_BASIC
#define TRACE_BASIC    TRUE
#else
#define TRACE_BASIC    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef BASIC_SERVER
#ifdef CLD_BAS_CMD_RESET_TO_FACTORY_DEFAULTS
PRIVATE teZCL_Status eCLD_BasicHandleResetToFactoryDefaultsCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif
#endif

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
 ** NAME:       eCLD_BasicCommandHandler
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

PUBLIC teZCL_Status eCLD_BasicCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    teZCL_Status eReturnStatus = E_ZCL_SUCCESS;
#ifdef BASIC_SERVER

	tsZCL_HeaderParams sZCL_HeaderParams;
    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // We don't handle messages as a client, so exit
    if(psClusterInstance->bIsServer == FALSE)
    {
        return(E_ZCL_FAIL);
    }

    /* 
     * The command is successfully qualified for all the errors , now take action 
     */
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // SERVER
    switch(sZCL_HeaderParams.u8CommandIdentifier)
    {
        #ifdef CLD_BAS_CMD_RESET_TO_FACTORY_DEFAULTS
            case(E_CLD_BASIC_CMD_RESET_TO_FACTORY_DEFAULTS):
            {
                eCLD_BasicHandleResetToFactoryDefaultsCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                eReturnStatus = E_ZCL_SUCCESS;
                break;
            }
        #endif
            default:
            {
                eReturnStatus = E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR;
                break;
            }
    }

    // unlock and return
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


#endif

    // delete the i/p buffer on return
    return(eReturnStatus);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef BASIC_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_BasicHandleResetToFactoryDefaultsCommand
 **
 ** DESCRIPTION:
 ** Handles Reset To Factory Defaults Command
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
#ifdef CLD_BAS_CMD_RESET_TO_FACTORY_DEFAULTS
PRIVATE teZCL_Status eCLD_BasicHandleResetToFactoryDefaultsCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;

    tsZCL_CallBackEvent sBasicCustomCallBackEvent;
    tsCLD_BasicCallBackMessage sBasicCallBackMessage;

    /* Receive the command */
    eStatus = eCLD_BasicCommandResetToFactoryDefaultsReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&sBasicCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    sBasicCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    sBasicCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
    sBasicCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&sBasicCallBackMessage;
    sBasicCustomCallBackEvent.psClusterInstance = psClusterInstance;

    /* Fill in message */
    sBasicCallBackMessage.u8CommandId = E_CLD_BASIC_CMD_RESET_TO_FACTORY_DEFAULTS;

    // call callback
    psEndPointDefinition->pCallBackFunctions(&sBasicCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}
#endif
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
