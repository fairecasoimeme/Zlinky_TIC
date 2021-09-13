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
 * MODULE:             Thermostat Cluster
 *
 * COMPONENT:          ThermostatCommandHandler.c
 *
 * DESCRIPTION:        Thermostat cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "Thermostat.h"
#include "Thermostat_internal.h"

#include "dbg.h"

#ifdef DEBUG_CLD_THERMOSTAT
#define TRACE_THERMOSTAT    TRUE
#else
#define TRACE_THERMOSTAT    FALSE
#endif



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_THERMOSTAT) && !defined(THERMOSTAT_SERVER) && !defined(THERMOSTAT_CLIENT)
#error You Must Have either THERMOSTAT_SERVER and/or THERMOSTAT_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_THERMOSTAT
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef THERMOSTAT_SERVER
PRIVATE teZCL_Status eCLD_ThermostatHandleSetpointRaiseOrLower(
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

/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatCommandHandler
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
PUBLIC  teZCL_Status eCLD_ThermostatCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_ThermostatCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ThermostatCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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
#ifdef THERMOSTAT_SERVER
       
            if(sZCL_HeaderParams.u8CommandIdentifier == E_CLD_THERMOSTAT_CMD_SETPOINT_RAISE_LOWER){
                eCLD_ThermostatHandleSetpointRaiseOrLower(pZPSevent, psEndPointDefinition, psClusterInstance);
            }else{
                // unlock
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
            }
#endif
    }
#ifdef THERMOSTAT_CLIENT
    else
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
    }
#endif


    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
#ifdef THERMOSTAT_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatHandleSetpointRaiseOrLower
 **
 ** DESCRIPTION:
 ** Handles Thermostat Setpoint raise or lower command
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
PRIVATE  teZCL_Status eCLD_ThermostatHandleSetpointRaiseOrLower(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_Thermostat_SetpointRaiseOrLowerPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_ThermostatCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ThermostatCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_ThermostatCommandSetpointRaiseOrLowerReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_THERMOSTAT, "eCLD_ThermostatCommandSetpointRaiseOrLowerReceive Error: %d\r\n", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psSetpointRaiseOrLowerPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    /* Update attribute */
    if((sPayload.eMode == E_CLD_THERMOSTAT_SRLM_HEAT) || (sPayload.eMode == E_CLD_THERMOSTAT_SRLM_BOTH))
        ((tsCLD_Thermostat*)psClusterInstance->pvEndPointSharedStructPtr)->i16OccupiedHeatingSetpoint += (int16)sPayload.i8Amount;
    if((sPayload.eMode == E_CLD_THERMOSTAT_SRLM_COOL) || (sPayload.eMode == E_CLD_THERMOSTAT_SRLM_BOTH))
        ((tsCLD_Thermostat*)psClusterInstance->pvEndPointSharedStructPtr)->i16OccupiedCoolingSetpoint += (int16)sPayload.i8Amount;

    return E_ZCL_SUCCESS;
}

#endif /*#ifdef THERMOSTAT_SERVER */
#endif /*#ifdef  CLD_THERMOSTAT */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

