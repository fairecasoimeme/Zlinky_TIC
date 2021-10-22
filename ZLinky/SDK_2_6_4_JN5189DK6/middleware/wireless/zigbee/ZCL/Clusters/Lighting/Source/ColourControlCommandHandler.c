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
 * MODULE:             Colour Control Cluster
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
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"

#include "ColourControl.h"
#include "ColourControl_internal.h"

#ifdef CLD_ONOFF
#include "OnOff_internal.h"
#endif

#include "zcl_options.h"



#include "dbg.h"

#ifdef DEBUG_CLD_COLOUR_CONTROL
#define TRACE_COLOUR_CONTROL   TRUE
#else
#define TRACE_COLOUR_CONTROL   FALSE
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
#ifdef COLOUR_CONTROL_SERVER
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleStepHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleStepSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToHueAndSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToColourCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleMoveColourCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);

PRIVATE teZCL_Status eCLD_ColourControlHandleStepColourCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleEnhancedMoveToHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
PRIVATE teZCL_Status eCLD_ColourControlHandleEnhancedMoveHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
PRIVATE teZCL_Status eCLD_ColourControlHandleEnhancedStepHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
PRIVATE teZCL_Status eCLD_ColourControlHandleEnhancedMoveToHueAndSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleColourLoopSetCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif

#if ((CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)   ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)      ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)               ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED))
PRIVATE teZCL_Status eCLD_ColourControlHandleStopMoveStepCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
PRIVATE teZCL_Status eCLD_ColourControlHandleMoveToColourTemperatureCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
PRIVATE teZCL_Status eCLD_ColourControlHandleMoveColourTemperatureCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
PRIVATE teZCL_Status eCLD_ColourControlHandleStepColourTemperatureCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId);
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
#if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
PRIVATE bool_t bCLD_ColourControlStopExecutionOfCommand(
        uint8                   u8EndPointId,
        uint8                   u8OptionsMask,
        uint8                   u8OptionsOverride,
        tsCLD_ColourControl     *psSharedStruct);
#endif        
#endif /* COLOUR_CONTROL_SERVER */

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
 ** NAME:       eCLD_ColourControlCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Cluster custom commands
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
PUBLIC  teZCL_Status eCLD_ColourControlCommandHandler(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance)
{

#ifdef COLOUR_CONTROL_SERVER

    teZCL_Status eStatus = E_ZCL_FAIL;
    tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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

        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
        case E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE:
            eStatus = eCLD_ColourControlHandleMoveToHueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_MOVE_HUE:
            eStatus = eCLD_ColourControlHandleMoveHueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_STEP_HUE:
            eStatus = eCLD_ColourControlHandleStepHueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_MOVE_TO_SATURATION:
            eStatus = eCLD_ColourControlHandleMoveToSaturationCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_MOVE_SATURATION:
            eStatus = eCLD_ColourControlHandleMoveSaturationCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_STEP_SATURATION:
            eStatus = eCLD_ColourControlHandleStepSaturationCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_MOVE_TO_HUE_AND_SATURATION:
            eStatus = eCLD_ColourControlHandleMoveToHueAndSaturationCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#endif
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)
        case E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR:
            eStatus = eCLD_ColourControlHandleMoveToColourCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR:
            eStatus = eCLD_ColourControlHandleMoveColourCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case E_CLD_COLOURCONTROL_CMD_STEP_COLOUR:
            eStatus = eCLD_ColourControlHandleStepColourCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
        case E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE:
            eStatus = eCLD_ColourControlHandleEnhancedMoveToHueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
        case E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_HUE:
            eStatus = eCLD_ColourControlHandleEnhancedMoveHueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
        case E_CLD_COLOURCONTROL_CMD_ENHANCED_STEP_HUE:
            eStatus = eCLD_ColourControlHandleEnhancedStepHueCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
        case E_CLD_COLOURCONTROL_CMD_ENHANCED_MOVE_TO_HUE_AND_SATURATION:
            eStatus = eCLD_ColourControlHandleEnhancedMoveToHueAndSaturationCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED)
        case E_CLD_COLOURCONTROL_CMD_COLOUR_LOOP_SET:
            eStatus = eCLD_ColourControlHandleColourLoopSetCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#endif

#if ((CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)   ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)      ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)               ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED))
        case E_CLD_COLOURCONTROL_CMD_STOP_MOVE_STEP:
            eStatus = eCLD_ColourControlHandleStopMoveStepCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
        case E_CLD_COLOURCONTROL_CMD_MOVE_TO_COLOUR_TEMPERATURE:
            eStatus = eCLD_ColourControlHandleMoveToColourTemperatureCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
        case E_CLD_COLOURCONTROL_CMD_MOVE_COLOUR_TEMPERATURE:
            eStatus = eCLD_ColourControlHandleMoveColourTemperatureCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
        case E_CLD_COLOURCONTROL_CMD_STEP_COLOUR_TEMPERATURE:
            eStatus = eCLD_ColourControlHandleStepColourTemperatureCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)

        default:
            /* Unsupported command */
            // unlock
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            /*As the command handler for this command is available but it does NOT handle the commands hence return */
            return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
            break;

        }

    }
    
    /* Added the check to make sure the default reponse with status success is centralized */
    if((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse))
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


#endif

    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef COLOUR_CONTROL_SERVER
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToHueCommand
 **
 ** DESCRIPTION:
 ** Handles move to hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    int iDifference;
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveToHueCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToHueCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveToHueCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }
    
    /*Invalid Hue field, reject the command  and send a default response of invalid field*/    
    if(sPayload.u8Hue > CLD_COLOURCONTROL_HUE_MAX_VALUE )
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* 6.8.1.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToHueCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
        /*Call back user before returning with INVALID default response incase user 
     * wants to do some processing on those values
     */    
    switch(sPayload.eDirection)
    {
        case E_CLD_COLOURCONTROL_DIRECTION_SHORTEST_DISTANCE:
        case E_CLD_COLOURCONTROL_DIRECTION_LONGEST_DISTANCE:
        case E_CLD_COLOURCONTROL_DIRECTION_UP:
        case E_CLD_COLOURCONTROL_DIRECTION_DOWN:
        /* Valid mode so continue */
        break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;                
    }

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif

    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iHue = (int)((uint16)sPayload.u8Hue << 8) * 100;
    psCommon->sTransition.sTarget.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    /* Calculate difference */
    iDifference = psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue;

    /* Calculate step size */
    switch(sPayload.eDirection)
    {

    case E_CLD_COLOURCONTROL_DIRECTION_SHORTEST_DISTANCE:
        if(psCommon->sTransition.sTarget.iHue > psCommon->sTransition.sStart.iHue)
        {
            iDifference = psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue;
            if(iDifference > ((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8)/2 * 100))
            {
                iDifference = (( (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100) - iDifference) * -1;
        }
        }
        else
        {
            iDifference = psCommon->sTransition.sStart.iHue - psCommon->sTransition.sTarget.iHue;
            if(iDifference > ((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8)/2 * 100))
            {
                iDifference = (((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100) - iDifference);
            }
            else
            {
                iDifference *= -1;
            }
        }
        break;

    case E_CLD_COLOURCONTROL_DIRECTION_LONGEST_DISTANCE:
        if(psCommon->sTransition.sTarget.iHue > psCommon->sTransition.sStart.iHue)
        {
            iDifference = psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue;
            if(iDifference < ((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8)/2 * 100))
            {
                iDifference = (((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100) - iDifference) * -1;
        }
        }
        else
        {
            iDifference = psCommon->sTransition.sStart.iHue - psCommon->sTransition.sTarget.iHue;
            if(iDifference < ((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8)/2 * 100))
            {
                iDifference = (((CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100) - iDifference);
            }
            else
            {
                iDifference *= -1;
            }
        }
        break;

    case E_CLD_COLOURCONTROL_DIRECTION_UP:
        if(iDifference < 0)
        {
            iDifference += (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
        }
        break;

    case E_CLD_COLOURCONTROL_DIRECTION_DOWN:
        if(iDifference > 0)
        {
            iDifference -= (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
        }
        break;

    }

    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iHue = iDifference / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iHue = iDifference;
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToHue: Hue=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveHueCommand
 **
 ** DESCRIPTION:
 ** Handles move hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveHueCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\neMoveHueCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveHueCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, " Mode=%d Rate=%d", sPayload.eMode, sPayload.u8Rate);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveHueCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    /*Call back user before returning with INVALID default response incase user 
     * wants to do some processing on those values
     */
     
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    switch(sPayload.eMode)
    {
        case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        /* Valid mode so continue */
        break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;                
    }

    /* If rate is 0 and mode isn't "stop", return an invalid field default response and don't process message any further */
    if((sPayload.u8Rate == 0) && (sPayload.eMode != E_CLD_COLOURCONTROL_MOVE_MODE_STOP))
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
        return E_ZCL_ERR_INVALID_VALUE;
    }

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = 0;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

    psCommon->sTransition.eCommand = u8CommandId;


    /* Which direction do we move in and by how much */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Stop!");
        psCommon->sTransition.sStep.iHue = 0;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        psCommon->sTransition.sStep.iHue = (int)((uint16)sPayload.u8Rate << 8) * 10;
        psCommon->sTransition.sTarget.iHue = (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        psCommon->sTransition.sStep.iHue = (int)((uint16)sPayload.u8Rate << 8) * -10;
        psCommon->sTransition.sTarget.iHue = 0;
        break;

    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveHue: Hue=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepHueCommand
 **
 ** DESCRIPTION:
 ** Handles step hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_StepHueCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepHueCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandStepHueCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStepHueCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    /*Call back user before returning with INVALID default response incase user 
     * wants to do some processing on those values
     */
     
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    switch(sPayload.eMode)
    {
        case E_CLD_COLOURCONTROL_STEP_MODE_UP:
        case E_CLD_COLOURCONTROL_STEP_MODE_DOWN:
        /* Valid mode so continue */
        break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;                
    }

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u8TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

    /* Set target values and scale */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_STEP_MODE_UP:
        psCommon->sTransition.sTarget.iHue = psCommon->sTransition.sStart.iHue + ((int)((uint16)sPayload.u8StepSize << 8) * 100);
        if(psCommon->sTransition.u16Time > 0)
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue) / psCommon->sTransition.u16Time;
        }
        else
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue);
        }
        if(psCommon->sTransition.sTarget.iHue > (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100)
        {
            psCommon->sTransition.sTarget.iHue -= (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
        }
        break;

    case E_CLD_COLOURCONTROL_STEP_MODE_DOWN:
        psCommon->sTransition.sTarget.iHue = psCommon->sTransition.sStart.iHue - ((int)((uint16)sPayload.u8StepSize << 8) * 100);
        if(psCommon->sTransition.u16Time > 0)
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue) / psCommon->sTransition.u16Time;
        }
        else
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue);
        }
        if(psCommon->sTransition.sTarget.iHue < 0)
        {
            psCommon->sTransition.sTarget.iHue += (CLD_COLOURCONTROL_HUE_MAX_VALUE<<8) * 100;
        }
        break;

    }

    psCommon->sTransition.eCommand = u8CommandId;


    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepHue: S=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToSaturationCommand
 **
 ** DESCRIPTION:
 ** Handles move to Saturation commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveToSaturationCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToSaturationCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveToSaturationCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToSaturationCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* Handle the error conditions - 
     * 1. for the Saturation value beyond 0xFE send a default response whether or not asked
     * 2. If the value HW can't handle send if the disable difault response with invalid field if enabled.
     */   
    if(sPayload.u8Saturation > CLD_COLOURCONTROL_SATURATION_MAX_VALUE )
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iSaturation = (int)sPayload.u8Saturation * 100;

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation);
    }
    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToSaturation: Sat=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iSaturation,
                                      psCommon->sTransition.sTarget.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveSaturationCommand
 **
 ** DESCRIPTION:
 ** Handles move Saturation commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveSaturationCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveSaturationCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveSaturationCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveSaturationCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* If rate is 0, return an invalid field default response and don't process message any further */
    if((sPayload.u8Rate == 0) && (sPayload.eMode != E_CLD_COLOURCONTROL_MOVE_MODE_STOP))
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
        return E_ZCL_ERR_INVALID_VALUE;
    }

    /* Check validity of the modes after call back to allow user to take a call if it needs handling.*/
    switch(sPayload.eMode)
    { 
        case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
            break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;
            
    }

    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);        
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = 0;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    psCommon->sTransition.eCommand = u8CommandId;

    /* Which direction do we move in and by how much */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        psCommon->sTransition.sStep.iSaturation = 0;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        psCommon->sTransition.sStep.iSaturation = (int)sPayload.u8Rate * 10;
        psCommon->sTransition.sTarget.iSaturation = CLD_COLOURCONTROL_SATURATION_MAX_VALUE * 100;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        psCommon->sTransition.sStep.iSaturation = (int)sPayload.u8Rate * -10;
        psCommon->sTransition.sTarget.iSaturation = 0;
        break;

    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveSaturation: Sat=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iSaturation,
                                      psCommon->sTransition.sTarget.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepSaturationCommand
 **
 ** DESCRIPTION:
 ** Handles step saturation commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_StepSaturationCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepSaturationCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandStepSaturationCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStepSaturationCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* Check validity of the modes after call back to allow user to take a call if it needs handling.*/
    switch(sPayload.eMode)
    { 
        case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
            break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;
            
    }    
    
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);

    #endif
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u8TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    /* Set target values and scale */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_STEP_MODE_UP:
        psCommon->sTransition.sTarget.iSaturation = psCommon->sTransition.sStart.iSaturation + ((int)sPayload.u8StepSize * 100);
        if(psCommon->sTransition.sTarget.iSaturation > CLD_COLOURCONTROL_SATURATION_MAX_VALUE * 100)
        {
            psCommon->sTransition.sTarget.iSaturation = CLD_COLOURCONTROL_SATURATION_MAX_VALUE * 100;
        }
        break;

    case E_CLD_COLOURCONTROL_STEP_MODE_DOWN:
        psCommon->sTransition.sTarget.iSaturation = psCommon->sTransition.sStart.iSaturation - ((int)sPayload.u8StepSize * 100);
        if(psCommon->sTransition.sTarget.iSaturation < 0)
        {
            psCommon->sTransition.sTarget.iSaturation = 0;
        }
        break;

    }

    /* Calculate step size */
    if(psCommon->sTransition.u16Time)
    {
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepSaturation: S=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iSaturation,
                                      psCommon->sTransition.sTarget.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToHueAndSaturationCommand
 **
 ** DESCRIPTION:
 ** Handles move to hue and saturation commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToHueAndSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveToHueAndSaturationCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToHueAndSaturationCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveToHueAndSaturationCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToHueAndSaturationCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /*Error Checking for Hue and sat values - should be within range before taking any action.
     * This check is after the callback to allow the user to take a call 
     */
    if( (sPayload.u8Saturation > CLD_COLOURCONTROL_SATURATION_MAX_VALUE ) ||
        (sPayload.u8Hue > CLD_COLOURCONTROL_HUE_MAX_VALUE ) )
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }    
    
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;
    psCommon->sTransition.sStart.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iHue = (int)((uint16)sPayload.u8Hue << 8) * 100;
    psCommon->sTransition.sTarget.iSaturation = (int)sPayload.u8Saturation * 100;

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue) / psCommon->sTransition.u16Time;
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue);
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToHueAndSaturation: Hue=%i,%i,%i Sat=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.sStart.iSaturation,
                                      psCommon->sTransition.sTarget.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToColourCommand
 **
 ** DESCRIPTION:
 ** Handles move to Colour commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToColourCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveToColourCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToColourCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveToColourCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToColourCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* Check coordinates for validity */
    if( sPayload.u16ColourX > CLD_COLOURCONTROL_XY_MAX_VALUE )
    {
        sPayload.u16ColourX = CLD_COLOURCONTROL_XY_MAX_VALUE;
    }
    if( sPayload.u16ColourY > CLD_COLOURCONTROL_XY_MAX_VALUE )
    {
        sPayload.u16ColourY = CLD_COLOURCONTROL_XY_MAX_VALUE;
    }

    uint16 u16NewX, u16NewY;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Current XY %04x %04x, Target -> %04x %04x\n",
                    psSharedStruct->u16CurrentX,
                    psSharedStruct->u16CurrentY,
                    sPayload.u16ColourX,
                    sPayload.u16ColourY);

    if ( FALSE == CS_bTransitionIsValid(psSharedStruct->u16CurrentX,  psSharedStruct->u16CurrentY,
                                        sPayload.u16ColourX,          sPayload.u16ColourY,
                                        &u16NewX,                     &u16NewY))
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "Clip XY %04x %04x\n",
                u16NewX,
                u16NewY);
        sPayload.u16ColourX = u16NewX;
        sPayload.u16ColourY = u16NewY;
    }
    
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iX = (int)psSharedStruct->u16CurrentX * 100;
    psCommon->sTransition.sCurrent.iY = (int)psSharedStruct->u16CurrentY * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iX = psCommon->sTransition.sCurrent.iX;
    psCommon->sTransition.sStart.iY = psCommon->sTransition.sCurrent.iY;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iX = (int)sPayload.u16ColourX * 100;
    psCommon->sTransition.sTarget.iY = (int)sPayload.u16ColourY * 100;

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iX = (psCommon->sTransition.sTarget.iX - psCommon->sTransition.sStart.iX) / psCommon->sTransition.u16Time;
        psCommon->sTransition.sStep.iY = (psCommon->sTransition.sTarget.iY - psCommon->sTransition.sStart.iY) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iX = (psCommon->sTransition.sTarget.iX - psCommon->sTransition.sStart.iX);
        psCommon->sTransition.sStep.iY = (psCommon->sTransition.sTarget.iY - psCommon->sTransition.sStart.iY);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToColour: X=%i,%i,%i Y=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iX,
                                      psCommon->sTransition.sTarget.iX,
                                      psCommon->sTransition.sStep.iX,
                                      psCommon->sTransition.sStart.iY,
                                      psCommon->sTransition.sTarget.iY,
                                      psCommon->sTransition.sStep.iY,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveColourCommand
 **
 ** DESCRIPTION:
 ** Handles move Colour commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveColourCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveColourCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveColourCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveColourCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveColourCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = 0;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iX = (int)psSharedStruct->u16CurrentX * 100;
    psCommon->sTransition.sCurrent.iY = (int)psSharedStruct->u16CurrentY * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iX = psCommon->sTransition.sCurrent.iX;
    psCommon->sTransition.sStart.iY = psCommon->sTransition.sCurrent.iY;

    /* Set step size */
    psCommon->sTransition.sStep.iX = (int)sPayload.i16RateX * 10;
    psCommon->sTransition.sStep.iY = (int)sPayload.i16RateY * 10;

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveColour: X=%i,%i,%i Y=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iX,
                                      psCommon->sTransition.sTarget.iX,
                                      psCommon->sTransition.sStep.iX,
                                      psCommon->sTransition.sStart.iY,
                                      psCommon->sTransition.sTarget.iY,
                                      psCommon->sTransition.sStep.iY,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepColourCommand
 **
 ** DESCRIPTION:
 ** Handles step Colour commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepColourCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_StepColourCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepColourCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandStepColourCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStepColourCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y);
    #endif
    

    int iTargetX, iTargetY;
    uint16 u16NewX, u16NewY;

    iTargetX = psSharedStruct->u16CurrentX + sPayload.i16StepX;
    iTargetY = psSharedStruct->u16CurrentY + sPayload.i16StepY;

    /* Check coordinates for validity */
    if( iTargetX > CLD_COLOURCONTROL_XY_MAX_VALUE )
    {
        iTargetX = CLD_COLOURCONTROL_XY_MAX_VALUE;
    }
    if( iTargetY > CLD_COLOURCONTROL_XY_MAX_VALUE )
    {
        iTargetY = CLD_COLOURCONTROL_XY_MAX_VALUE;
    }
    /* and negative */
    if( iTargetX < 0 )
    {
        iTargetX = 0;
    }
    if( iTargetY < 0 )
    {
        iTargetY = 0;
    }


    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Current XY %04x %04x, Target -> %04x %04x\n",
                    psSharedStruct->u16CurrentX,
                    psSharedStruct->u16CurrentY,
                    (uint16)iTargetX,
                    (uint16)iTargetY);

    if ( FALSE == CS_bTransitionIsValid( psSharedStruct->u16CurrentX,  psSharedStruct->u16CurrentY,
                                         (uint16)iTargetX,          (uint16)iTargetY,
                                         &u16NewX,                     &u16NewY))
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "Clip XY %04x %04x\n",
                u16NewX,
                u16NewY);
        iTargetX = u16NewX;
        iTargetY = u16NewY;
    }

    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iX = (int)psSharedStruct->u16CurrentX * 100;
    psCommon->sTransition.sCurrent.iY = (int)psSharedStruct->u16CurrentY * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iX = psCommon->sTransition.sCurrent.iX;
    psCommon->sTransition.sStart.iY = psCommon->sTransition.sCurrent.iY;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iX = (iTargetX * 100);
    psCommon->sTransition.sTarget.iY = (iTargetY * 100);

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iX = (psCommon->sTransition.sTarget.iX - psCommon->sTransition.sStart.iX) / psCommon->sTransition.u16Time;
        psCommon->sTransition.sStep.iY = (psCommon->sTransition.sTarget.iY - psCommon->sTransition.sStart.iY) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iX = (psCommon->sTransition.sTarget.iX - psCommon->sTransition.sStart.iX);
        psCommon->sTransition.sStep.iY = (psCommon->sTransition.sTarget.iY - psCommon->sTransition.sStart.iY);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepColour: X=%i,%i,%i Y=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iX,
                                      psCommon->sTransition.sTarget.iX,
                                      psCommon->sTransition.sStep.iX,
                                      psCommon->sTransition.sStart.iY,
                                      psCommon->sTransition.sTarget.iY,
                                      psCommon->sTransition.sStep.iY,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}
#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)


#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleEnhancedMoveToHueCommand
 **
 ** DESCRIPTION:
 ** Handles enhanced move to hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleEnhancedMoveToHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    int iDifference;
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_EnhancedMoveToHueCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedMoveToHueCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandEnhancedMoveToHueCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedMoveToHueCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* For Invalid Direction field, reject the command and send a default response 
     * ZCL - 5.2.2.3.2.3 Direction Field 
     */
    if(sPayload.eDirection > 0x03)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }
    
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes- The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iHue = (int)sPayload.u16EnhancedHue * 100;
    psCommon->sTransition.sTarget.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    /* Calculate difference */
    iDifference = psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue;


    DBG_vPrintf(TRACE_COLOUR_CONTROL, " Hue=%d Diff=%d Target=%d", psCommon->u16CurrentHue, iDifference, sPayload.u16EnhancedHue);

    /* Calculate step size */
    switch(sPayload.eDirection)
    {

    case E_CLD_COLOURCONTROL_DIRECTION_SHORTEST_DISTANCE:
        if(psCommon->sTransition.sTarget.iHue > psCommon->sTransition.sStart.iHue)
        {
            iDifference = psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue;
            if(iDifference > (32767 * 100))
            {
                iDifference = ((65535 * 100) - iDifference) * -1;
            }
        }
        else
        {
            iDifference = psCommon->sTransition.sStart.iHue - psCommon->sTransition.sTarget.iHue;
            if(iDifference > (32767 * 100))
            {
                iDifference = ((65535 * 100) - iDifference);
            }
            else
            {
                iDifference *= -1;
            }
        }
        break;

    case E_CLD_COLOURCONTROL_DIRECTION_LONGEST_DISTANCE:
        if(psCommon->sTransition.sTarget.iHue > psCommon->sTransition.sStart.iHue)
        {
            iDifference = psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue;
            if(iDifference < (32767 * 100))
            {
                iDifference = ((65535 * 100) - iDifference) * -1;
            }
        }
        else
        {
            iDifference = psCommon->sTransition.sStart.iHue - psCommon->sTransition.sTarget.iHue;
            if(iDifference < (32767 * 100))
            {
                iDifference = ((65535 * 100) - iDifference);
            }
            else
            {
                iDifference *= -1;
            }
        }
        break;

    case E_CLD_COLOURCONTROL_DIRECTION_UP:
        if(iDifference < 0)
        {
            iDifference += 65535 * 100;
        }
        break;

    case E_CLD_COLOURCONTROL_DIRECTION_DOWN:
        if(iDifference > 0)
        {
            iDifference -= 65535 * 100;
        }
        break;

    }

    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iHue = iDifference / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iHue = iDifference;
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL,"\nEnhancedMoveToHue: Hue=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleEnhancedMoveHueCommand
 **
 ** DESCRIPTION:
 ** Handles enhanced move hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleEnhancedMoveHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_EnhancedMoveHueCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedMoveHueCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandEnhancedMoveHueCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, " Mode=%d Rate=%d", sPayload.eMode, sPayload.u16Rate);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedMoveHueCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* Check validity of the modes after call back to allow user to take a call if it needs handling.*/
    switch(sPayload.eMode)
    {
        case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        /* Valid mode so continue */
        break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;                
    }

    /* If rate is 0 and mode isn't "stop", return an invalid field default response and don't process message any further */
    if((sPayload.u16Rate == 0) && (sPayload.eMode != E_CLD_COLOURCONTROL_MOVE_MODE_STOP))
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
    }

    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = 0;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

    psCommon->sTransition.eCommand = u8CommandId;


    /* Which direction do we move in and by how much */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Stop!");
        psCommon->sTransition.sStep.iHue = 0;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        psCommon->sTransition.sStep.iHue = (int)sPayload.u16Rate * 10;
        psCommon->sTransition.sTarget.iHue = 0xffff * 100;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        psCommon->sTransition.sStep.iHue = (int)sPayload.u16Rate * -10;
        psCommon->sTransition.sTarget.iHue = 0;
        break;

    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedMoveHue: Hue=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.u16Time);

    return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleEnhancedStepHueCommand
 **
 ** DESCRIPTION:
 ** Handles enhanced step hue commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleEnhancedStepHueCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_EnhancedStepHueCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedStepHueCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandEnhancedStepHueCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedStepHueCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* For Reserved Step Mode field, reject the command and send a default response 
     * ZCL - 5.2.2.3.4.2 Step Mode Field 
     */
    if((sPayload.eMode != E_CLD_COLOURCONTROL_STEP_MODE_UP) &&
       (sPayload.eMode != E_CLD_COLOURCONTROL_STEP_MODE_DOWN))
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* 6.8.1.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

    /* Set target values and scale */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_STEP_MODE_UP:
        psCommon->sTransition.sTarget.iHue = psCommon->sTransition.sStart.iHue + ((int)sPayload.u16StepSize * 100);
        if(psCommon->sTransition.u16Time > 0)
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue) / psCommon->sTransition.u16Time;
        }
        else
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue);
        }
        if(psCommon->sTransition.sTarget.iHue > 65535 * 100)
        {
            psCommon->sTransition.sTarget.iHue -= 65535 * 100;
        }
        break;

    case E_CLD_COLOURCONTROL_STEP_MODE_DOWN:
        psCommon->sTransition.sTarget.iHue = psCommon->sTransition.sStart.iHue - ((int)sPayload.u16StepSize * 100);
        if(psCommon->sTransition.u16Time > 0)
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue) / psCommon->sTransition.u16Time;
        }
        else
        {
            psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue);
        }
        if(psCommon->sTransition.sTarget.iHue < 0)
        {
            psCommon->sTransition.sTarget.iHue += 65535 * 100;
        }
        break;

    }

    psCommon->sTransition.eCommand = u8CommandId;


    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedStepHue: S=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleEnhancedMoveToHueAndSaturationCommand
 **
 ** DESCRIPTION:
 ** Handles enhanced move to hue and saturation commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleEnhancedMoveToHueAndSaturationCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_EnhancedMoveToHueAndSaturationCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedMoveToHueAndSaturationCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandEnhancedMoveToHueAndSaturationCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psEnhancedMoveToHueAndSaturationCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* For Invalid Saturation field, reject the command and send a default response 
     * ZCL - Range of CurrentSaturation - 0x00-0xFE 
     */
    if(sPayload.u8Saturation > 0xFE)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* 6.8.1.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
    psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;
    psCommon->sTransition.sStart.iSaturation = psCommon->sTransition.sCurrent.iSaturation;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iHue = (int)sPayload.u16EnhancedHue * 100;
    psCommon->sTransition.sTarget.iSaturation = (int)sPayload.u8Saturation * 100;

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue) / psCommon->sTransition.u16Time;
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iHue = (psCommon->sTransition.sTarget.iHue - psCommon->sTransition.sStart.iHue);
        psCommon->sTransition.sStep.iSaturation = (psCommon->sTransition.sTarget.iSaturation - psCommon->sTransition.sStart.iSaturation);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nEnhancedMoveToHueAndSaturation: Hue=%i,%i,%i Sat=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iHue,
                                      psCommon->sTransition.sTarget.iHue,
                                      psCommon->sTransition.sStep.iHue,
                                      psCommon->sTransition.sStart.iSaturation,
                                      psCommon->sTransition.sTarget.iSaturation,
                                      psCommon->sTransition.sStep.iSaturation,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleColourLoopSetCommand
 **
 ** DESCRIPTION:
 ** Handles colour loop set commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleColourLoopSetCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_ColourLoopSetCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nColourLoopSetCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandColourLoopSetCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psColourLoopSetCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* For Invalid Action field, reject the command and send a default response 
     * ZLL - 6.8.1.3.6.2 Action Field 
     */
    if(sPayload.eAction > 0x02)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }

    /* For Invalid Direction field, reject the command and send a default response 
     * ZCL - 5.2.2.3.18.3 Direction Field.
     */
    if(sPayload.eDirection > 0x01)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    #endif
    
    /* Handle depending on bits set in update flags field */
    if(sPayload.u8UpdateFlags & E_CLD_COLOURCONTROL_FLAGS_UPDATE_DIRECTION)
    {
        psSharedStruct->u8ColourLoopDirection = sPayload.eDirection;
    }

    if(sPayload.u8UpdateFlags & E_CLD_COLOURCONTROL_FLAGS_UPDATE_TIME)
    {
        psSharedStruct->u16ColourLoopTime = sPayload.u16Time;
    }

    if(sPayload.u8UpdateFlags & E_CLD_COLOURCONTROL_FLAGS_UPDATE_START_HUE)
    {
        psSharedStruct->u16ColourLoopStartEnhancedHue = sPayload.u16StartHue;
    }

    if(sPayload.u8UpdateFlags & E_CLD_COLOURCONTROL_FLAGS_UPDATE_ACTION)
    {
        /* Update transition */
        psCommon->sTransition.eCommand = u8CommandId;
        
        switch(sPayload.eAction)
        {

        case E_CLD_COLOURCONTROL_COLOURLOOP_ACTION_DEACTIVATE:
            /* 
             * If the value of the action field is set to 0x00 and the color loop is active, i.e. the ColorLoopActive
             * attribute is set to 0x01, the device shall de-active the color loop, set the ColorLoopActive attribute to
             * 0x00 and set the EnhancedCurrentHue attribute to the value of the ColorLoopStoredEnhancedHue
             * attribute. If the value of the action field is set to 0x00 and the color loop is inactive, i.e. the
             * ColorLoopActive attribute is set to 0x00, the device shall ignore the action update component of the command.
             */
            if(psSharedStruct->u8ColourLoopActive == 0x01)
            {
                psSharedStruct->u8ColourLoopActive = 0;
                psCommon->u16CurrentHue = psSharedStruct->u16ColourLoopStoredEnhancedHue;
            }
            else
            {
                psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
            }
            break;

        case E_CLD_COLOURCONTROL_COLOURLOOP_ACTION_ACTIVATE_FROM_START:
            /* If the value of the action field is set to 0x01, the device shall set the
             * ColorLoopStoredEnhancedHue attribute to the value of the EnhancedCurrentHue
             * attribute, set the ColorLoopActive attribute to 0x01 and activate the colour loop,
             * starting from the value of the ColorLoopStartEnhancedHue attribute.
             */
            psSharedStruct->u16ColourLoopStoredEnhancedHue = psCommon->u16CurrentHue;
            psSharedStruct->u8ColourLoopActive = 0x01;
            psCommon->u16CurrentHue = psSharedStruct->u16ColourLoopStartEnhancedHue;

            /* Set transition time */
            psCommon->sTransition.u16Time = 0;

            /* Get current values and scale to prevent rounding errors in transition */
            psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
            psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

            /* Save start values */
            psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;

            break;

        case E_CLD_COLOURCONTROL_COLOURLOOP_ACTION_ACTIVATE_FROM_CURRENT:
            /* If the value of the action field is set to 0x02, the device shall set the
             * ColorLoopStoredEnhancedHue attribute to the value of the EnhancedCurrentHue attribute,
             * set the ColorLoopActive attribute to 0x01 and activate the colour loop, starting from
             * the value of the EnhancedCurrentHue attribute.
             */
            psSharedStruct->u16ColourLoopStoredEnhancedHue = psCommon->u16CurrentHue;
            psSharedStruct->u8ColourLoopActive = 0x01;

            /* Set transition time */
            psCommon->sTransition.u16Time = 0;

            /* Get current values and scale to prevent rounding errors in transition */
            psCommon->sTransition.sCurrent.iHue = (int)psCommon->u16CurrentHue * 100;
            psCommon->sTransition.sCurrent.iSaturation = (int)psSharedStruct->u8CurrentSaturation * 100;

            /* Save start values */
            psCommon->sTransition.sStart.iHue = psCommon->sTransition.sCurrent.iHue;
            break;

        }
    }

    /* If the colour loop is active */
    if(psSharedStruct->u8ColourLoopActive == 0x01)
    {

        /* If direction is decrement, make the step size negative */
        if(psSharedStruct->u8ColourLoopDirection == E_CLD_COLOURCONTROL_COLOURLOOP_DIRECTION_INCREMENT)
        {
            /* Calculate step size */
            if(psSharedStruct->u16ColourLoopTime > 0)
            {
                psCommon->sTransition.sStep.iHue = (65535 * 100) / ((int)psSharedStruct->u16ColourLoopTime * 10);
            }
            else
            {
                psCommon->sTransition.sStep.iHue = (65535 * 100);
            }
        }
        else
        {
            /* Calculate step size */
            if(psSharedStruct->u16ColourLoopTime > 0)
            {
                psCommon->sTransition.sStep.iHue = ((-65535 * 100) / ((int)psSharedStruct->u16ColourLoopTime * 10));
            }
            else
            {
                psCommon->sTransition.sStep.iHue = (-65535 * 100);
            }
        }

    }


    return eStatus;

}
#endif

#if ((CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)   ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)      ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)               ||\
     (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED))
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStopMoveStepCommand
 **
 ** DESCRIPTION:
 ** Handles step move stop commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStopMoveStepCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_StopMoveStepCommandPayload sPayload = {0};
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED) || ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        tsCLD_ColourControl *psSharedStruct;
        /* Get a pointer to the shared structure */
        psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;
    #endif

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;



    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStopMoveStepCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandStopMoveStepCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStopMoveStepCommandPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_LOOP_SUPPORTED)
    /* 6.8.1.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
    #endif
    //#endif

    psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;

    return eStatus;

}
#endif

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveToColourTemperatureCommand
 **
 ** DESCRIPTION:
 ** Handles move to Colour Temperature commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveToColourTemperatureCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveToColourTemperatureCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToColourTemperatureCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveToColourTemperatureCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* If target is outside limits posed by the hardware, return error */
    #if ((defined CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN) && (defined CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX))
        /* ZLLv1.0+Errata - Section 6.8.1.3.1 (CB#2011 ZLL specific behaviour)
           For the move to color temperature command, if the target color specified is not achievable by the
            hardware then the color temperature shall be clipped at the physical minimum or maximum achievable,
            depending on the color temperature transition, when the device reaches that color temperature (which
            may be before the requested transition time), and a ZCL default response command shall be generated,
            where not disabled, with a status code equal to SUCCESS */
        if(sPayload.u16ColourTemperatureMired < psSharedStruct->u16ColourTemperatureMiredPhyMin) 
        {
            sPayload.u16ColourTemperatureMired = psSharedStruct->u16ColourTemperatureMiredPhyMin;
        }
        if(sPayload.u16ColourTemperatureMired > psSharedStruct->u16ColourTemperatureMiredPhyMax)
        {
            sPayload.u16ColourTemperatureMired = psSharedStruct->u16ColourTemperatureMiredPhyMax;
        }
    #endif
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToColourTemperatureCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif   

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* ZCL - 5.2.2.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
#endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iCCT = (int)psSharedStruct->u16ColourTemperatureMired * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iCCT = psCommon->sTransition.sCurrent.iCCT;

    /* Set target values and scale */
    psCommon->sTransition.sTarget.iCCT = (int)sPayload.u16ColourTemperatureMired * 100;

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iCCT = (psCommon->sTransition.sTarget.iCCT - psCommon->sTransition.sStart.iCCT) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iCCT = (psCommon->sTransition.sTarget.iCCT - psCommon->sTransition.sStart.iCCT);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveToColourTemperature: CCT=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iCCT,
                                      psCommon->sTransition.sTarget.iCCT,
                                      psCommon->sTransition.sStep.iCCT,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleMoveColourTemperatureCommand
 **
 ** DESCRIPTION:
 ** Handles move colour temperature commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleMoveColourTemperatureCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_MoveColourTemperatureCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveColourTemperatureCommandReceive");

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandMoveColourTemperatureCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveColourTemperatureCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* If rate is 0, return an invalid field default response and don't process message any further */
    if((sPayload.u16Rate == 0) && (sPayload.eMode != E_CLD_COLOURCONTROL_MOVE_MODE_STOP))
    {
        return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
    }

    /* For Invalid Move Mode field, reject the command and send a default response 
     * ZCL - 5.2.2.3.20.1 Move mode Field 
     */
    switch(sPayload.eMode)
    {
        case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        /* Valid mode so continue */
        break;
        default:
            /*reserved mode return without any action , send a default response back as its a reserved field  */
            DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
            eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
            return E_ZCL_ERR_INVALID_VALUE;
        break;                
    }
    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* 6.8.1.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = 0;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iCCT = (int)psSharedStruct->u16ColourTemperatureMired * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iCCT = psCommon->sTransition.sCurrent.iCCT;

    psCommon->sTransition.eCommand = u8CommandId;

    /* Which direction do we move in and by how much */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_MOVE_MODE_STOP:
        psCommon->sTransition.sStep.iCCT = 0;
        psCommon->sTransition.eCommand = E_CLD_COLOURCONTROL_CMD_NONE;
        break;

    case E_CLD_COLOURCONTROL_MOVE_MODE_UP:
        psCommon->sTransition.sStep.iCCT = (int)sPayload.u16Rate * 10;

#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX
        if(sPayload.u16ColourTemperatureMiredMax == 0)
        {
            psCommon->sTransition.sTarget.iCCT = (int)psSharedStruct->u16ColourTemperatureMiredPhyMax * 100;
        }
        else
        {
            psCommon->sTransition.sTarget.iCCT = (int)MIN(psSharedStruct->u16ColourTemperatureMiredPhyMax, sPayload.u16ColourTemperatureMiredMax) * 100;
        }
        break;
#endif
    case E_CLD_COLOURCONTROL_MOVE_MODE_DOWN:
        psCommon->sTransition.sStep.iCCT = (int)sPayload.u16Rate * -10;
#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN
        if(sPayload.u16ColourTemperatureMiredMin == 0)
        {
            psCommon->sTransition.sTarget.iCCT = (int)psSharedStruct->u16ColourTemperatureMiredPhyMin * 100;
        }
        else
        {
            psCommon->sTransition.sTarget.iCCT = (int)MAX(psSharedStruct->u16ColourTemperatureMiredPhyMin, sPayload.u16ColourTemperatureMiredMin) * 100;
        }
        break;
#endif

    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nMoveColourTemperature: CCT=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iCCT,
                                      psCommon->sTransition.sTarget.iCCT,
                                      psCommon->sTransition.sStep.iCCT,
                                      psCommon->sTransition.u16Time);

    return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleStepColourTemperatureCommand
 **
 ** DESCRIPTION:
 ** Handles step colour temperature commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandId               Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControlHandleStepColourTemperatureCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint8                        u8CommandId)
{
    teZCL_Status eStatus;
    uint16 u16Limit;
    uint8 u8TransactionSequenceNumber;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsCLD_ColourControl_StepColourTemperatureCommandPayload sPayload = {0};
    tsCLD_ColourControl *psSharedStruct;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ColourControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get a pointer to the shared structure */
    psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    /* Receive the command */
    eStatus = eCLD_ColourControlCommandStepColourTemperatureCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStep colTemperatureCommandReceive Error: %d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStepColourTemperatureCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        if(bCLD_ColourControlStopExecutionOfCommand(
                psEndPointDefinition->u8EndPointNumber,
                sPayload.u8OptionsMask,
                sPayload.u8OptionsOverride,
                psSharedStruct))
        {
            return E_ZCL_SUCCESS;
        }
    #endif
    
    /* For Reserved Step Mode field, reject the command and send a default response 
     * ZCL - 5.2.2.3.4.2 Step Mode Field 
     */
    if((sPayload.eMode != E_CLD_COLOURCONTROL_STEP_MODE_UP) &&
       (sPayload.eMode != E_CLD_COLOURCONTROL_STEP_MODE_DOWN))
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
    }

    #if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_ENHANCE_HUE_SUPPORTED)
    /* 6.8.1.3.1 Generic usage notes - The colour loop shall not be stopped on
     * receipt of another command such as the enhanced move to hue command.
     */
    if(psSharedStruct->u8ColourLoopActive != 0)
    {
        return E_ZCL_SUCCESS;
    }
    #endif

    #ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
        /* Handle colour mode change */
        eCLD_ColourControlHandleColourModeChange(psEndPointDefinition->u8EndPointNumber,
                                             E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE);
    #endif
    
    /* Set transition time */
    psCommon->sTransition.u16Time = sPayload.u16TransitionTime;

    /* Get current values and scale to prevent rounding errors in transition */
    psCommon->sTransition.sCurrent.iCCT = (int)psSharedStruct->u16ColourTemperatureMired * 100;

    /* Save start values */
    psCommon->sTransition.sStart.iCCT = psCommon->sTransition.sCurrent.iCCT;

    /* Set target values and scale */
    switch(sPayload.eMode)
    {

    case E_CLD_COLOURCONTROL_STEP_MODE_UP:
        psCommon->sTransition.sTarget.iCCT = psCommon->sTransition.sStart.iCCT + ((int)sPayload.u16StepSize * 100);
#ifdef  CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MAX
        if(sPayload.u16ColourTemperatureMiredMax == 0)
        {
            u16Limit = psSharedStruct->u16ColourTemperatureMiredPhyMax;
        }
        else
        {
            u16Limit = MIN(psSharedStruct->u16ColourTemperatureMiredPhyMax, sPayload.u16ColourTemperatureMiredMax);
        }
#endif
        if(psCommon->sTransition.sTarget.iCCT > (int)u16Limit * 100)
        {
            psCommon->sTransition.sTarget.iCCT = (int)u16Limit * 100;
        }
        break;

    case E_CLD_COLOURCONTROL_STEP_MODE_DOWN:
        psCommon->sTransition.sTarget.iCCT = psCommon->sTransition.sStart.iCCT - ((int)sPayload.u16StepSize * 100);
#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED_PHY_MIN
        if(sPayload.u16ColourTemperatureMiredMin == 0)
        {
            u16Limit = psSharedStruct->u16ColourTemperatureMiredPhyMin;
        }
        else
        {
            u16Limit = MAX(psSharedStruct->u16ColourTemperatureMiredPhyMin, sPayload.u16ColourTemperatureMiredMin);
        }
#endif
        if(psCommon->sTransition.sTarget.iCCT < (int)u16Limit * 100)
        {
            psCommon->sTransition.sTarget.iCCT = (int)u16Limit * 100;
        }
        break;

    }

    /* Calculate step size */
    if(psCommon->sTransition.u16Time > 0)
    {
        psCommon->sTransition.sStep.iCCT = (psCommon->sTransition.sTarget.iCCT - psCommon->sTransition.sStart.iCCT) / psCommon->sTransition.u16Time;
    }
    else
    {
        psCommon->sTransition.sStep.iCCT = (psCommon->sTransition.sTarget.iCCT - psCommon->sTransition.sStart.iCCT);
    }

    psCommon->sTransition.eCommand = u8CommandId;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nStepSaturation: S=%i,%i,%i Time=%d",
                                      psCommon->sTransition.sStart.iCCT,
                                      psCommon->sTransition.sTarget.iCCT,
                                      psCommon->sTransition.sStep.iCCT,
                                      psCommon->sTransition.u16Time);

    return E_ZCL_SUCCESS;
}

#endif //#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_COLOUR_TEMPERATURE_SUPPORTED)


#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_MODE
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlHandleColourModeChange
 **
 ** DESCRIPTION:
 ** Handles a change from one colour mode to another
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition       End point definition
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster instance
 ** tsCLD_ColourControlCustomDataStructure psCommon         Common data structure
 ** teCLD_ColourControl_ColourMode eMode                    New colour mode
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlHandleColourModeChange(
        uint8                                   u8SourceEndPointId,
        teCLD_ColourControl_ColourMode          eMode)
{

    teZCL_Status eStatus = E_ZCL_SUCCESS;
#if ((CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED) || defined(CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED))
    uint8 u8Temp;
#endif

    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ColourControlCustomDataStructure *psCommon;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nColorControl: No cluster");
        return eStatus;
    }

    tsCLD_ColourControl *psSharedStruct = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;

    /* If mode has not changed, exit */
    if(psCommon->eColourMode == eMode)
    {
        return(E_ZCL_SUCCESS);
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "\nModeChange %d to %d", psCommon->eColourMode, eMode);

    /* See which colour mode we are in now and convert to xyY */
    switch(psCommon->eColourMode)
    {

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION:
#endif
    case E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION:
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)  
        /* Convert HSV to xyY */
        eStatus = eCLD_ColourControl_HSV2xyY(u8SourceEndPointId,
                                   psCommon->u16CurrentHue,
                                   psSharedStruct->u8CurrentSaturation,
                                   255,
                                   &psSharedStruct->u16CurrentX,
                                   &psSharedStruct->u16CurrentY,
                                   &u8Temp);
        
        /* If an error occurred in any of the colour conversion functions, exit */
        if(eStatus != E_ZCL_SUCCESS)
        {
            return eStatus;
        }
#endif                                 
        break;

    case E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y:
        break;

    case E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE:
#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
        /* Convert CCT to xyY */
        vCLD_ColourControl_CCT2xyY(psSharedStruct->u16ColourTemperatureMired,
                                   &psSharedStruct->u16CurrentX,
                                   &psSharedStruct->u16CurrentY,
                                   &u8Temp);
#endif
        break;

    }
    
    /* Which colour mode are we supposed to go to */
    switch(eMode)
    {

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    case E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION:
#endif
    case E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION:
#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)      
        /* Convert to HSV */
        eStatus = eCLD_ColourControl_xyY2HSV(psCommon,
                                   psSharedStruct->u16CurrentX,
                                   psSharedStruct->u16CurrentY,
                                   255,
                                   &psCommon->u16CurrentHue,
                                   &psSharedStruct->u8CurrentSaturation,
                                   &u8Temp);
#endif                                 
        break;

    case E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y:
        break;

    case E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE:
        /* Convert to CCT */
#ifdef CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED
        vCLD_ColourControl_xyY2CCT(psSharedStruct->u16CurrentX,
                                   psSharedStruct->u16CurrentY,
                                   255,
                                   &psSharedStruct->u16ColourTemperatureMired);
#endif
        break;

    }

    psCommon->eColourMode = eMode;

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_HUE_SATURATION_SUPPORTED)
    psSharedStruct->u8CurrentHue        = (uint8)(psCommon->u16CurrentHue >> 8);
#endif

#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE
    psSharedStruct->u16EnhancedCurrentHue = psCommon->u16CurrentHue;
#endif

    /* Update colour mode attribute(s) */
#ifdef CLD_COLOURCONTROL_ATTR_ENHANCED_COLOUR_MODE
    if(psCommon->eColourMode == E_CLD_COLOURCONTROL_COLOURMODE_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION)
    {
        psSharedStruct->u8ColourMode = E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION;
    }
    else
    {
    psSharedStruct->u8ColourMode = eMode;
    }
    psSharedStruct->u8EnhancedColourMode = eMode;
#else
    psSharedStruct->u8ColourMode = eMode;
#endif

    return eStatus;

}
#endif

#if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
PRIVATE bool_t bCLD_ColourControlStopExecutionOfCommand(
        uint8                   u8EndPointId,
        uint8                   u8OptionsMask,
        uint8                   u8OptionsOverride,
        tsCLD_ColourControl     *psSharedStruct)
{
    bool_t  bOnOff;

    if(eCLD_OnOffGetState(u8EndPointId, &bOnOff) == E_ZCL_SUCCESS)
    {
        if((bOnOff == FALSE) &&
          (((u8OptionsMask & COLOURCONTROL_EXECUTE_IF_OFF_BIT) && ((u8OptionsOverride & COLOURCONTROL_EXECUTE_IF_OFF_BIT)== 0)) ||
          (((u8OptionsMask & COLOURCONTROL_EXECUTE_IF_OFF_BIT) == 0) && ((psSharedStruct->u8Options & COLOURCONTROL_EXECUTE_IF_OFF_BIT) == 0)))
          )
        {
            return TRUE;
        }
    }
    return FALSE;
}
#endif


#endif /* COLOUR_CONTROL_SERVER*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
