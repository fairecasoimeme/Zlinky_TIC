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
 * MODULE:             Alarms Cluster
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
//#include "zps_apl_aib.h"
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"

#include "Alarms.h"
#include "Alarms_internal.h"

#include "zcl_options.h"

#include "dbg.h"

#ifdef DEBUG_CLD_ALARMS
#define TRACE_ALARMS    TRUE
#else
#define TRACE_ALARMS    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_ALARMS) && !defined(ALARMS_SERVER) && !defined(ALARMS_CLIENT)
#error You Must Have either ALARMS_SERVER and/or ALARMS_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef ALARMS_SERVER
PRIVATE teZCL_Status eCLD_AlarmsHandleResetAlarmCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eCLD_AlarmsHandleResetAllAlarmsCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eCLD_AlarmsHandleGetAlarmCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eCLD_AlarmsHandleResetAlarmLogCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif

#ifdef ALARMS_CLIENT
PRIVATE teZCL_Status eCLD_AlarmsHandleAlarmCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eCLD_AlarmsHandleGetAlarmResponseCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
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
 ** NAME:       eCLD_AlarmsCommandHandler
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
PUBLIC teZCL_Status eCLD_AlarmsCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_AlarmsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Store return address details */
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psCommon->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer)
    {

#ifdef ALARMS_SERVER
        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_ALARMS_CMD_RESET_ALARM):
            {
                eCLD_AlarmsHandleResetAlarmCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_ALARMS_CMD_RESET_ALL_ALARMS):
            {
                eCLD_AlarmsHandleResetAllAlarmsCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_ALARMS_CMD_GET_ALARM):
            {
                eCLD_AlarmsHandleGetAlarmCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_ALARMS_CMD_RESET_ALARM_LOG):
            {
                eCLD_AlarmsHandleResetAlarmLogCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            default:
            {
                // unlock
                #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif
    }
    else
    {
#ifdef ALARMS_CLIENT
        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_ALARMS_CMD_ALARM):
            {
                eCLD_AlarmsHandleAlarmCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_ALARMS_CMD_GET_ALARM_RESPONSE):
            {
                eCLD_AlarmsHandleGetAlarmResponseCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            default:
            {
                // unlock
                #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif
    }


    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef ALARMS_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsHandleResetAlarmCommand
 **
 ** DESCRIPTION:
 ** Handles reset alarm commands
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
PRIVATE teZCL_Status eCLD_AlarmsHandleResetAlarmCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_AlarmsResetAlarmCommandPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_AlarmsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_AlarmsCommandResetAlarmCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandResetAlarmCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }


    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psResetAlarmCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsHandleResetAllAlarmsCommand
 **
 ** DESCRIPTION:
 ** Handles reset all alarms commands
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
PRIVATE teZCL_Status eCLD_AlarmsHandleResetAllAlarmsCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_AlarmsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_AlarmsCommandResetAllAlarmsCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandResetAllAlarmsCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }


    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsHandleGetAlarmCommand
 **
 ** DESCRIPTION:
 ** Handles get alarm commands
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
PRIVATE teZCL_Status eCLD_AlarmsHandleGetAlarmCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_AlarmsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    tsCLD_AlarmsGetAlarmResponsePayload sResponse;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_AlarmsCommandGetAlarmCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandGetAlarmCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Try and get a log entry */
    eStatus = eCLD_AlarmsGetAlarmFromLog(psEndPointDefinition,
                                         psClusterInstance,
                                         &sResponse.u8AlarmCode,
                                         &sResponse.u16ClusterId,
                                         &sResponse.u32TimeStamp);
    switch(eStatus)
    {
    case E_ZCL_SUCCESS:
        sResponse.u8Status = E_ZCL_CMDS_SUCCESS;
        break;

    case E_ZCL_FAIL:
        sResponse.u8Status = E_ZCL_CMDS_NOT_FOUND;
        break;

    default:
        return eStatus;
        break;

    }

#ifdef CLD_ALARMS_ATTR_ALARM_COUNT
    /* Update alarm count attribute */
    if(((tsCLD_Alarms*)psClusterInstance->pvEndPointSharedStructPtr)->u16AlarmCount > 0)
    {
        ((tsCLD_Alarms*)psClusterInstance->pvEndPointSharedStructPtr)->u16AlarmCount--;
    }
#endif

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    /* Send response */
    eStatus = eCLD_AlarmsCommandGetAlarmResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsHandleResetAlarmLogCommand
 **
 ** DESCRIPTION:
 ** Handles reset alarm log commands
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
PRIVATE teZCL_Status eCLD_AlarmsHandleResetAlarmLogCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_AlarmsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_AlarmsCommandResetAlarmLogCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandResetAlarmLogCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Reset the alarm log */
    eStatus = eCLD_AlarmsResetAlarmLog(psEndPointDefinition, psClusterInstance);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsResetAlarmToLog Error: %d\r\n", eStatus);
        return eStatus;
    }

    return eStatus;
}

#endif

#ifdef ALARMS_CLIENT

/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsHandleAlarmCommand
 **
 ** DESCRIPTION:
 ** Handles alarm commands
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
PRIVATE teZCL_Status eCLD_AlarmsHandleAlarmCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_AlarmsAlarmCommandPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_AlarmsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_AlarmsCommandAlarmCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandAlarmCommandReceive Error: %d\r\n", eStatus);
        return eStatus;
    }


    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psAlarmCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_AlarmsHandleGetAlarmResponseCommand
 **
 ** DESCRIPTION:
 ** Handles get alarm response commands
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
PRIVATE teZCL_Status eCLD_AlarmsHandleGetAlarmResponseCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_AlarmsGetAlarmResponsePayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_AlarmsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_AlarmsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_AlarmsCommandGetAlarmResponseReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ALARMS, "eCLD_AlarmsCommandGetAlarmResponseReceive Error: %d\r\n", eStatus);
        return eStatus;
    }


    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetAlarmResponse = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return eStatus;
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
