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
 * MODULE:             Level Control Cluster
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

#include "LevelControl.h"
#include "LevelControl_internal.h"
#include "OnOff_internal.h"

#include "zcl_options.h"

#include "dbg.h"


#ifdef DEBUG_CLD_LEVEL_CONTROL
#define TRACE_LEVEL_CONTROL    TRUE
#else
#define TRACE_LEVEL_CONTROL    FALSE
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

#ifdef LEVEL_CONTROL_SERVER
PRIVATE teZCL_Status eCLD_LevelControlHandleMoveToLevelCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier);
#endif

#ifdef LEVEL_CONTROL_SERVER
PRIVATE teZCL_Status eCLD_LevelControlHandleMoveCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier);
#endif

#ifdef LEVEL_CONTROL_SERVER
PRIVATE teZCL_Status eCLD_LevelControlHandleStepCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier);
#endif

#ifdef LEVEL_CONTROL_SERVER
PRIVATE teZCL_Status eCLD_LevelControlHandleStopCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier);
#endif



#ifdef LEVEL_CONTROL_SERVER
#ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
PRIVATE teZCL_Status eCLD_LevelControlHandleMoveToClosestFreqCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier);
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
 ** NAME:       eCLD_LevelControlCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Level Control Cluster custom commands
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
PUBLIC  teZCL_Status eCLD_LevelControlCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

#ifdef LEVEL_CONTROL_SERVER
    teZCL_Status eStatus = E_ZCL_FAIL;
    tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_LevelControlCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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

        case (E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL):
        case (E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF):
            eStatus = eCLD_LevelControlHandleMoveToLevelCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case (E_CLD_LEVELCONTROL_CMD_MOVE):
        case (E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF):
            eStatus = eCLD_LevelControlHandleMoveCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case (E_CLD_LEVELCONTROL_CMD_STEP):
        case (E_CLD_LEVELCONTROL_CMD_STEP_WITH_ON_OFF):
            eStatus = eCLD_LevelControlHandleStepCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;

        case (E_CLD_LEVELCONTROL_CMD_STOP):
        case (E_CLD_LEVELCONTROL_CMD_STOP_WITH_ON_OFF):
            eStatus = eCLD_LevelControlHandleStopCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
            break;
#ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
        case (E_CLD_LEVELCONTROL_CMD_MOVE_TO_CLOSEST_FREQ):
          eStatus = eCLD_LevelControlHandleMoveToClosestFreqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
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

    }

#if 0
    /* If immediate update required, do it now rather than wait for next ZCL timer tick */
    if(psCommon->u16RemainingTime == 0)
    {
        eCLD_LevelControlUpdate(psEndPointDefinition->u8EndPointNumber);
    }
#endif

    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    /* Added the check to make sure the default reponse with status success is centralized */
    if((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse))
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
#endif
    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef LEVEL_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlHandleMoveToLevelCommand
 **
 ** DESCRIPTION:
 ** Handles move to level commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                    u8CommandIdentifier        Command Identifier
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_LevelControlHandleMoveToLevelCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    tsCLD_LevelControl_MoveToLevelCommandPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_LevelControlCustomDataStructure *psCommon;
    tsCLD_LevelControl *psSharedStruct;
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        bool_t bOnOff;
    #endif

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get pointer to shared data structure */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlCommandMoveToLevelCmdReceive:");

    /* Receive the command */
    eStatus = eCLD_LevelControlCommandMoveToLevelCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " Err:%d", eStatus);
        return eStatus;
    }

    /* 3.10.2.2  Generic usage notes
     * If a move to level, move, step or stop command is received while the
     * device is in its off state, i.e. the OnOff attribute of the on/off
     * cluster is equal to 0x00, the command shall be ignored.
     */
#if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL)
    {
        /* Get pointer to shared struct in On/Off cluster */
        if(eCLD_OnOffGetState(psEndPointDefinition->u8EndPointNumber, &bOnOff) != E_ZCL_SUCCESS)
        {
            DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlHandleMoveToLevelCommand: Failed to get ON/OFF");
            return E_ZCL_FAIL;
        }
        
        /*The Options attribute is meant to be changed only during commissioning. 
        The Options attribute is a bitmap that determines the default behavior of some cluster commands. 
        Each command that is de-pendent on the Options attribute SHALL first construct a temporary Options bitmap that is in effect during the command processing.
        The temporary Options bitmap is has the same format and meaning as the Options attribute, but includes any bits that may be overridden by command fields.
        0	ExecuteIfOff	0 – Do not execute command if OnOff is 0x00 (FALSE)
                            1 – Execute command if OnOff is 0x00 (FALSE)
        */
        if((((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) && ((sPayload.u8OptionsOverride & LEVELCONTROL_EXECUTE_IF_OFF_BIT)== 0)) ||
           (((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0) && ((psSharedStruct->u8Options & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0))) &&
            (bOnOff == FALSE)
          )
        {
            return E_ZCL_SUCCESS;
        }
    }
#endif

    /* Check transition time */
    if(sPayload.u16TransitionTime == 0xffff)
    {
#ifdef CLD_LEVELCONTROL_ATTR_ON_OFF_TRANSITION_TIME
        sPayload.u16TransitionTime = psSharedStruct->u16OnOffTransitionTime;
#else
        sPayload.u16TransitionTime = 0;
#endif
    }


    /* Fill in transition details */
    psCommon->sTransition.eTransition   = E_CLD_LEVELCONTROL_TRANSITION_MOVE_TO_LEVEL;
    psCommon->sTransition.iTargetLevel  = CLAMP((int)sPayload.u8Level * 100, CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
    psCommon->sTransition.u32Time       = sPayload.u16TransitionTime;

    /* Calculate step size */
    if(psCommon->sTransition.u32Time > 0)
    {
        psCommon->sTransition.iStepSize = (psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
    }
    else
    {
        psCommon->sTransition.iStepSize = psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel;
    }

    /* Is command "with On/Off" ? */
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_MOVE_TO_LEVEL_WITH_ON_OFF)
    {
        psCommon->sTransition.bWithOnOff = TRUE;

        #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER) && (defined CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL))
        /* Set Global Scene Control attribute to true */
        eCLD_OnOffSetGlobalSceneControl(psEndPointDefinition->u8EndPointNumber, TRUE);
        #endif
    }
    else
    {
        psCommon->sTransition.bWithOnOff = FALSE;
    }

    DBG_vPrintf(TRACE_LEVEL_CONTROL, " Level=%i Time=%d Step=%i WithOnOff=%d", psCommon->sTransition.iTargetLevel, sPayload.u16TransitionTime, psCommon->sTransition.iStepSize, psCommon->sTransition.bWithOnOff);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToLevelCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* If transition time is zero, transition immediately, don't wait for update tick */
    if(psCommon->sTransition.u32Time == 0)
    {
        eCLD_LevelControlUpdate(psEndPointDefinition->u8EndPointNumber);
    }

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlHandleMoveCommand
 **
 ** DESCRIPTION:
 ** Handles move commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_LevelControlHandleMoveCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    tsCLD_LevelControl_MoveCommandPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_LevelControlCustomDataStructure *psCommon;
    tsCLD_LevelControl *psSharedStruct;
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        bool_t bOnOff;
    #endif

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get pointer to shared data structure */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlCommandMoveCommandReceive:");

    /* Receive the command */
    eStatus = eCLD_LevelControlCommandMoveCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " Err:%d", eStatus);
        return eStatus;
    }

    /* 3.10.2.2  Generic usage notes
     * If a move to level, move, step or stop command is received while the
     * device is in its off state, i.e. the OnOff attribute of the on/off
     * cluster is equal to 0x00, the command shall be ignored.
     */
#if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_MOVE)
    {
        /* Get pointer to shared struct in On/Off cluster */
        if(eCLD_OnOffGetState(psEndPointDefinition->u8EndPointNumber, &bOnOff) != E_ZCL_SUCCESS)
        {
            DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlHandleMoveToLevelCommand: Failed to get ON/OFF");
            return E_ZCL_FAIL;
        }
        /*The Options attribute is meant to be changed only during commissioning. 
        The Options attribute is a bitmap that determines the default behavior of some cluster commands. 
        Each command that is de-pendent on the Options attribute SHALL first construct a temporary Options bitmap that is in effect during the command processing.
        The temporary Options bitmap is has the same format and meaning as the Options attribute, but includes any bits that may be overridden by command fields.
        0	ExecuteIfOff	0 – Do not execute command if OnOff is 0x00 (FALSE)
                            1 – Execute command if OnOff is 0x00 (FALSE)
        */
        if((((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) && ((sPayload.u8OptionsOverride & LEVELCONTROL_EXECUTE_IF_OFF_BIT)== 0)) ||
           (((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0) && ((psSharedStruct->u8Options & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0))) &&
            (bOnOff == FALSE)
          )
        {
            return E_ZCL_SUCCESS;
        }
    }
#endif

    /* Fill in transition details */
    psCommon->sTransition.eTransition   = E_CLD_LEVELCONTROL_TRANSITION_MOVE;
    psCommon->sTransition.eMode         = sPayload.u8MoveMode;
    psCommon->sTransition.u32Time       = 0;

#ifdef CLD_LEVELCONTROL_ATTR_DEFAULT_MOVE_RATE
    if(sPayload.u8Rate == 0xFF)
    {
        sPayload.u8Rate = psSharedStruct->u8DefaultMoveRate;
    }
#endif
    switch(sPayload.u8MoveMode)
    {

    case E_CLD_LEVELCONTROL_MOVE_MODE_UP:
        psCommon->sTransition.iTargetLevel  = CLD_LEVELCONTROL_MAX_LEVEL * 100;
        psCommon->sTransition.iStepSize     = (int)sPayload.u8Rate * 10;
        break;

    case E_CLD_LEVELCONTROL_MOVE_MODE_DOWN:
        psCommon->sTransition.iTargetLevel  = CLD_LEVELCONTROL_MIN_LEVEL * 100;
        psCommon->sTransition.iStepSize     = (int)sPayload.u8Rate * -10;
        break;

    default:
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
        break;

    }

    /* Is command "with On/Off" ? */
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_MOVE_WITH_ON_OFF)
    {
        psCommon->sTransition.bWithOnOff = TRUE;
        #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER) && (defined CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL))
        /* Set Global Scene Control attribute to true */
        eCLD_OnOffSetGlobalSceneControl(psEndPointDefinition->u8EndPointNumber, TRUE);
        #endif
    }
    else
    {
        psCommon->sTransition.bWithOnOff = FALSE;
    }

    DBG_vPrintf(TRACE_LEVEL_CONTROL, " Mode=%d Rate=%d Curr=%d WithOnOff=%d", sPayload.u8MoveMode, sPayload.u8Rate, psSharedStruct->u8CurrentLevel, psCommon->sTransition.bWithOnOff);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlHandleStepCommand
 **
 ** DESCRIPTION:
 ** Handles step commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_LevelControlHandleStepCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    tsCLD_LevelControl_StepCommandPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_LevelControlCustomDataStructure *psCommon;
    tsCLD_LevelControl *psSharedStruct;
    
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        bool_t bOnOff;
    #endif

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get pointer to shared data structure */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;
    
    DBG_vPrintf(TRACE_LEVEL_CONTROL, "eCLD_LevelControlCommandStepCommandReceive:");

    /* Receive the command */
    eStatus = eCLD_LevelControlCommandStepCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " Err:%d", eStatus);
        return eStatus;
    }

    /* 3.10.2.2  Generic usage notes
     * If a move to level, move, step or stop command is received while the
     * device is in its off state, i.e. the OnOff attribute of the on/off
     * cluster is equal to 0x00, the command shall be ignored.
     */
#if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_STEP)
    {
        /* Get pointer to shared struct in On/Off cluster */
        if(eCLD_OnOffGetState(psEndPointDefinition->u8EndPointNumber, &bOnOff) != E_ZCL_SUCCESS)
        {
            DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlHandleMoveToLevelCommand: Failed to get ON/OFF");
            return E_ZCL_FAIL;
        }
        /*The Options attribute is meant to be changed only during commissioning. 
        The Options attribute is a bitmap that determines the default behavior of some cluster commands. 
        Each command that is de-pendent on the Options attribute SHALL first construct a temporary Options bitmap that is in effect during the command processing.
        The temporary Options bitmap is has the same format and meaning as the Options attribute, but includes any bits that may be overridden by command fields.
        0	ExecuteIfOff	0 – Do not execute command if OnOff is 0x00 (FALSE)
                            1 – Execute command if OnOff is 0x00 (FALSE)
        */
        if((((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) && ((sPayload.u8OptionsOverride & LEVELCONTROL_EXECUTE_IF_OFF_BIT)== 0)) ||
           (((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0) && ((psSharedStruct->u8Options & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0))) &&
            (bOnOff == FALSE)
          )
        {
            return E_ZCL_SUCCESS;
        }
    }
#endif

    /* If transition time is 0xffff, transition immediately */
    if(sPayload.u16TransitionTime == 0xffff)
    {
        psCommon->sTransition.u32Time = 0;
    }
    /* Add transition time to remaining time (limit to 0xffff) */
    else
    {
        psCommon->sTransition.u32Time = MIN(psCommon->sTransition.u32Time + sPayload.u16TransitionTime, 0xffff);
    }

    /* Fill in transition details */
    psCommon->sTransition.eTransition   = E_CLD_LEVELCONTROL_TRANSITION_STEP;
    psCommon->sTransition.eMode         = sPayload.u8StepMode;

    switch(sPayload.u8StepMode)
    {

    case E_CLD_LEVELCONTROL_MOVE_MODE_UP:
        psCommon->sTransition.iTargetLevel = CLAMP(psCommon->sTransition.iTargetLevel + (sPayload.u8StepSize * 100), CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
        break;

    case E_CLD_LEVELCONTROL_MOVE_MODE_DOWN:
        psCommon->sTransition.iTargetLevel = CLAMP(psCommon->sTransition.iTargetLevel - (sPayload.u8StepSize * 100), CLD_LEVELCONTROL_MIN_LEVEL * 100, CLD_LEVELCONTROL_MAX_LEVEL * 100);
        break;

    default:
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_ERR_INVALID_VALUE;
        break;

    }

    /* Is command "with On/Off" ? */
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_STEP_WITH_ON_OFF)
    {
        psCommon->sTransition.bWithOnOff = TRUE;
    }
    else
    {
        psCommon->sTransition.bWithOnOff = FALSE;
    }

    DBG_vPrintf(TRACE_LEVEL_CONTROL, " Mode=%d Size=%d Time=%d WithOnOff=%d", sPayload.u8StepMode, sPayload.u8StepSize, sPayload.u16TransitionTime, psCommon->sTransition.bWithOnOff);

    /* Calculate step size */
    if(psCommon->sTransition.u32Time > 0)
    {
        psCommon->sTransition.iStepSize = (psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel) / (int)psCommon->sTransition.u32Time;
    }
    else
    {
        psCommon->sTransition.iStepSize = psCommon->sTransition.iTargetLevel - psCommon->sTransition.iCurrentLevel;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStepCommandPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlHandleStopCommand
 **
 ** DESCRIPTION:
 ** Handles stop commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_LevelControlHandleStopCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_LevelControlCustomDataStructure *psCommon;
    tsCLD_LevelControl *psSharedStruct;
    tsCLD_LevelControl_StopCommandPayload sPayload = {0};
    #if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
        bool_t bOnOff;
    #endif

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Get pointer to shared data structure */
    psSharedStruct = (tsCLD_LevelControl*)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlCommandStopCommandReceive:");

    /* Receive the command */
    eStatus = eCLD_LevelControlCommandStopCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " Err:%d", eStatus);
        return eStatus;
    }

#if ((defined CLD_ONOFF) && (defined ONOFF_SERVER))
    if(u8CommandIdentifier == E_CLD_LEVELCONTROL_CMD_STOP)
    {
        /* Get pointer to shared struct in On/Off cluster */
        if(eCLD_OnOffGetState(psEndPointDefinition->u8EndPointNumber, &bOnOff) != E_ZCL_SUCCESS)
        {
            DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlHandleMoveToLevelCommand: Failed to get ON/OFF");
            return E_ZCL_FAIL;
        }
        /*The Options attribute is meant to be changed only during commissioning. 
        The Options attribute is a bitmap that determines the default behavior of some cluster commands. 
        Each command that is de-pendent on the Options attribute SHALL first construct a temporary Options bitmap that is in effect during the command processing.
        The temporary Options bitmap is has the same format and meaning as the Options attribute, but includes any bits that may be overridden by command fields.
        0	ExecuteIfOff	0 – Do not execute command if OnOff is 0x00 (FALSE)
                            1 – Execute command if OnOff is 0x00 (FALSE)
        */
        if((((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) && ((sPayload.u8OptionsOverride & LEVELCONTROL_EXECUTE_IF_OFF_BIT)== 0)) ||
           (((sPayload.u8OptionsMask & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0) && ((psSharedStruct->u8Options & LEVELCONTROL_EXECUTE_IF_OFF_BIT) == 0))) &&
            (bOnOff == FALSE)
          )
        {
            return E_ZCL_SUCCESS;
        }
    }
#endif

    /* Stop any transition */
    psCommon->sTransition.eTransition   = E_CLD_LEVELCONTROL_TRANSITION_NONE;
    psCommon->sTransition.iCurrentLevel = ((int)psSharedStruct->u8CurrentLevel * 100);
    psCommon->sTransition.iTargetLevel  = psCommon->sTransition.iCurrentLevel;
    psCommon->sTransition.u32Time       = 0;
    psCommon->sTransition.bWithOnOff    = FALSE;

#ifdef CLD_LEVELCONTROL_ATTR_REMAINING_TIME
    psSharedStruct->u16RemainingTime = psCommon->sTransition.u32Time;
#endif

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psStopCommandPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


#ifdef CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY
/****************************************************************************
 **
 ** NAME:       eCLD_LevelControlHandleMoveToClosestFreqCommand
 **
 ** DESCRIPTION:
 ** Handles Move to Closest Frequency command
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint8                     u8CommandIdentifier       Command Identifier
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_LevelControlHandleMoveToClosestFreqCommand(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_LevelControlCustomDataStructure *psCommon;
    tsCLD_LevelControl_MoveToClosestFreqCommandPayload sPayload = {0};

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_LevelControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    DBG_vPrintf(TRACE_LEVEL_CONTROL, "\neCLD_LevelControlCommandMoveToClosestFreqCommandReceive:");

    /* Receive the command */
    eStatus = eCLD_LevelControlCommandMoveToClosestFreqCommandReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_LEVEL_CONTROL, " Err:%d", eStatus);
        return eStatus;
    }

    /* Fill in transition details */
    if(sPayload.u16Frequency > CLD_LEVELCONTROL_MAX_FREQ)
    {
      sPayload.u16Frequency = CLD_LEVELCONTROL_MAX_FREQ;
    }
    
    if(sPayload.u16Frequency < CLD_LEVELCONTROL_MIN_FREQ)
    {
      sPayload.u16Frequency = CLD_LEVELCONTROL_MIN_FREQ;
    }
    
    psCommon->sTransition.u16TargetFrequency    = sPayload.u16Frequency;
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psMoveToClosestFreqCommandPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    eCLD_LevelControlUpdate(psEndPointDefinition->u8EndPointNumber);

    return E_ZCL_SUCCESS;
}
#endif /* CLD_LEVELCONTROL_ATTR_CURRENT_FREQUENCY*/
#endif /* LEVEL_CONTROL_SERVER */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
