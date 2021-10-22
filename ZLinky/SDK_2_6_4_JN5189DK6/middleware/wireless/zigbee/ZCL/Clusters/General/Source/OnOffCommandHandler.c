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
 * MODULE:             On/Off Cluster
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

#include "OnOff.h"
#include "OnOff_internal.h"

#ifdef CLD_LEVEL_CONTROL
#include "LevelControl.h"
#include "LevelControl_internal.h"
#endif

#ifdef CLD_SCENES
#include "Scenes.h"
#include "Scenes_internal.h"
#endif

#include "zcl_options.h"


#include "dbg.h"
#ifdef DEBUG_CLD_ONOFF
#define TRACE_ONOFF TRUE
#else
#define TRACE_ONOFF FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_ONOFF) && !defined(ONOFF_SERVER) && !defined(ONOFF_CLIENT)
#error You Must Have either ONOFF_SERVER and/or ONOFF_CLIENT defined zcl_options.h
#endif

#if defined(ONOFF_SERVER) && (defined(CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL) || defined(CLD_ONOFF_CMD_ON_WITH_RECALL_GLOBAL_SCENE)) && !defined(SCENES_SERVER)
#error For Global Scene you Must Have SCENES_SERVER defined in zcl_options.h
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

#ifdef ONOFF_SERVER
PRIVATE teZCL_Status eCLD_OnOffHandleOnCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);

PRIVATE teZCL_Status eCLD_OnOffHandleOffCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);

PRIVATE teZCL_Status eCLD_OnOffHandleToggleCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
                    
#ifdef CLD_ONOFF_CMD_OFF_WITH_EFFECT
PRIVATE teZCL_Status eCLD_OnOffHandleOffWithEffectCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif                    

#ifdef CLD_ONOFF_CMD_ON_WITH_RECALL_GLOBAL_SCENE
PRIVATE teZCL_Status eCLD_OnOffHandleOnWithRecallGlobalSceneCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif

#ifdef CLD_ONOFF_CMD_ON_WITH_TIMED_OFF
PRIVATE teZCL_Status eCLD_OnOffHandleOnWithTimedOffCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier);
#endif
#endif /* ONOFF_SERVER */
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
 ** NAME:       eCLD_OnOffCommandHandler
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

PUBLIC  teZCL_Status eCLD_OnOffCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    DBG_vPrintf(TRACE_ONOFF, "\nONOFF: ");

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // We don't handle messages as a client, so exit
    if(psClusterInstance->bIsServer == FALSE)
    {
        return(E_ZCL_FAIL);
    }

#ifdef ONOFF_SERVER
{
    tsZCL_CallBackEvent sOnOffCustomCallBackEvent;
    tsCLD_OnOffCallBackMessage sOnOffCallBackMessage;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // SERVER
    switch(sZCL_HeaderParams.u8CommandIdentifier)
    {

    case(E_CLD_ONOFF_CMD_ON):
        eCLD_OnOffHandleOnCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;

    case(E_CLD_ONOFF_CMD_OFF):
        eCLD_OnOffHandleOffCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;

    case(E_CLD_ONOFF_CMD_TOGGLE):
        eCLD_OnOffHandleToggleCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;

    #ifdef  CLD_ONOFF_CMD_OFF_WITH_EFFECT
    case E_CLD_ONOFF_CMD_OFF_EFFECT:
        eCLD_OnOffHandleOffWithEffectCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return(E_ZCL_SUCCESS);
        break;
    #endif
    
    #ifdef CLD_ONOFF_CMD_ON_WITH_RECALL_GLOBAL_SCENE
    case E_CLD_ONOFF_CMD_ON_RECALL_GLOBAL_SCENE:
        eCLD_OnOffHandleOnWithRecallGlobalSceneCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
        break;
    #endif
    
    #ifdef CLD_ONOFF_CMD_ON_WITH_TIMED_OFF
    case E_CLD_ONOFF_CMD_ON_TIMED_OFF:
        eCLD_OnOffHandleOnWithTimedOffCommand(pZPSevent, psEndPointDefinition, psClusterInstance, sZCL_HeaderParams.u8CommandIdentifier);
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


    /* Generate a custom command event */
    eZCL_SetCustomCallBackEvent(&sOnOffCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);
    sOnOffCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    sOnOffCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
    sOnOffCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&sOnOffCallBackMessage;
    sOnOffCustomCallBackEvent.psClusterInstance = psClusterInstance;

    /* Fill in message */
    sOnOffCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

    // call callback
    psEndPointDefinition->pCallBackFunctions(&sOnOffCustomCallBackEvent);

    /* Generate a cluster update event */
    sOnOffCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&sOnOffCustomCallBackEvent);

#if (defined CLD_SCENES) && (defined SCENES_SERVER)
    vCLD_ScenesUpdateSceneValid( psEndPointDefinition);
#endif

    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

}
#endif //ONOFF_SERVER

    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
#ifdef ONOFF_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffHandleOnCommand
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
PRIVATE  teZCL_Status eCLD_OnOffHandleOnCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_OnOff *psSharedStruct = (tsCLD_OnOff *)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_ONOFF, "On: ");

    /* Receive the command */
    eStatus = eCLD_OnOffCommandReceive(pZPSevent,
                                    &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ONOFF, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /*
     * 6.6.1.4.2 On command extensions
     * On receipt of the on command, if the value of the OnTime attribute is equal to 0x0000, the device shall
     * set the OffWaitTime attribute to 0x0000.
     */
#if (defined CLD_ONOFF_ATTR_ON_TIME) && (defined CLD_ONOFF_ATTR_OFF_WAIT_TIME)
    if(psSharedStruct->u16OnTime == 0x0000)
    {
        psSharedStruct->u16OffWaitTime = 0;
    }
#endif

#ifdef CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL
    /* See ZCL specification 3.8.2.2.2 */
    psSharedStruct->bGlobalSceneControl = 0x01;
#endif

    /* If OnOff is already on, exit */
    if(psSharedStruct->bOnOff == 0x01)
    {
        return eStatus;
    }

#if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_SERVER)
    if(eCLD_LevelControlClusterIsPresent(psEndPointDefinition->u8EndPointNumber) == E_ZCL_SUCCESS)
    {
        /* If not already on, set it on */
        if((bool_t)psSharedStruct->bOnOff != TRUE)
        {
            DBG_vPrintf(TRACE_ONOFF, "LC Set to 1");
            eCLD_LevelControlSetOnOffState(psEndPointDefinition->u8EndPointNumber,
                                           TRUE,
                                           CLD_ONOFF_OFF_WITH_EFFECT_NONE);
        }
    }
    else
    {
        psSharedStruct->bOnOff = 0x01;
    }
#else
    psSharedStruct->bOnOff = 0x01;
#endif

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_OnOffHandleOffCommand
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

PRIVATE  teZCL_Status eCLD_OnOffHandleOffCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_OnOff *psSharedStruct = (tsCLD_OnOff *)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_ONOFF, "Off: ");

    /* Receive the command */
    eStatus = eCLD_OnOffCommandReceive(pZPSevent,
                                &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ONOFF, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /* See ZCL Specification 3.8.2.3.1.1 */
#if (defined CLD_ONOFF_ATTR_ON_TIME)
    psSharedStruct->u16OnTime = 0x0000;
#endif

    /* If already off, exit */
    if(psSharedStruct->bOnOff == 0)
    {
        return eStatus;
    }

#if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_SERVER)
    if(eCLD_LevelControlClusterIsPresent(psEndPointDefinition->u8EndPointNumber) == E_ZCL_SUCCESS)
    {
        /* If not already off, set it off */
        if((bool_t)psSharedStruct->bOnOff != FALSE)
        {
            DBG_vPrintf(TRACE_ONOFF, "LC Set to 0");
            eCLD_LevelControlSetOnOffState(psEndPointDefinition->u8EndPointNumber,
                                           FALSE,
                                           CLD_ONOFF_OFF_WITH_EFFECT_NONE);
        }
    }
    else
    {
        psSharedStruct->bOnOff = 0x00;
    }
#else
    psSharedStruct->bOnOff = 0x00;
#endif

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eCLD_OnOffHandleToggleCommand
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
PRIVATE  teZCL_Status eCLD_OnOffHandleToggleCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_OnOff *psSharedStruct = (tsCLD_OnOff *)psClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_ONOFF, "Toggle: ");

    /* Receive the command */
    eStatus = eCLD_OnOffCommandReceive(pZPSevent,
                                &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ONOFF, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }


    /*
     * 6.6.1.4.3 Toggle command extensions
     * On receipt of the toggle command, if the value of the OnOff attribute is equal to 0x00 and if the value
     * of the OnTime attribute is equal to 0x0000, the device shall set the OffWaitTime attribute to 0x0000. If
     * the value of the OnOff attribute is equal to 0x01, the OnTime attribute shall be set to 0x0000.
     */
#if (defined CLD_ONOFF_ATTR_ON_TIME) && (defined CLD_ONOFF_ATTR_OFF_WAIT_TIME)
    if(psSharedStruct->bOnOff == 0)
    {
        if(psSharedStruct->u16OnTime == 0)
        {
            psSharedStruct->u16OffWaitTime = 0;
        }
    }
    else
    {
        psSharedStruct->u16OnTime = 0;
    }
#endif


#if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_SERVER)
    if(eCLD_LevelControlClusterIsPresent(psEndPointDefinition->u8EndPointNumber) == E_ZCL_SUCCESS)
    {
        /* Toggle On/Off */
        DBG_vPrintf(TRACE_ONOFF, "LC Set to %d", psSharedStruct->bOnOff ^ 0x01);
        eCLD_LevelControlSetOnOffState(psEndPointDefinition->u8EndPointNumber,
                                       psSharedStruct->bOnOff ^ 0x01,
                                       CLD_ONOFF_OFF_WITH_EFFECT_NONE);
    }
    else
    {
        psSharedStruct->bOnOff ^= 0x01;
    }
#else
    psSharedStruct->bOnOff ^= 0x01;
#endif

#ifdef CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL
    if(psSharedStruct->bOnOff == TRUE)
    {
        psSharedStruct->bGlobalSceneControl = TRUE;
    }
#endif
    return eStatus;
}

#ifdef  CLD_ONOFF_CMD_OFF_WITH_EFFECT
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffHandleOffWithEffectCommand
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

PRIVATE  teZCL_Status eCLD_OnOffHandleOffWithEffectCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_OnOff *psSharedStruct = (tsCLD_OnOff *)psClusterInstance->pvEndPointSharedStructPtr;
    tsCLD_OnOff_OffWithEffectRequestPayload sPayload;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_OnOffCallBackMessage sCallBackMessage;

    DBG_vPrintf(TRACE_ONOFF, "Off With Effect: ");

    /* Receive the command */
    eStatus = eCLD_OnOffCommandOffWithEffectReceive(pZPSevent,
                                &u8TransactionSequenceNumber,
                                &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ONOFF, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }


    /*
     * 6.6.1.4.4.3 Effect on receipt
     * On receipt of the off with effect command and if the GlobalSceneControl attribute is equal to TRUE,
     * the application on the associated endpoint shall store its settings in its global scene then set the
     * GlobalSceneControl attribute to FALSE. The application shall then enter its “off” state, update the OnOff
     * attribute accordingly, and set the OnTime attribute to 0x0000.
     * In all other cases, the application on the associated endpoint shall enter its “off” state, update the OnOff
     * attribute accordingly, and set the OnTime attribute to 0x0000.
     */
#ifdef CLD_ONOFF_ATTR_GLOBAL_SCENE_CONTROL
    if(psSharedStruct->bGlobalSceneControl != 0)
    {
        /* Store the global scene */
        #ifdef SCENES_SERVER
            eCLD_ScenesStore(psEndPointDefinition->u8EndPointNumber, 0, 0);
        #endif
        psSharedStruct->bGlobalSceneControl = 0;
    }
#endif

#if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_SERVER)
    if(eCLD_LevelControlClusterIsPresent(psEndPointDefinition->u8EndPointNumber) == E_ZCL_SUCCESS)
    {
        /* If not already off, set it off */
        if((bool_t)psSharedStruct->bOnOff != FALSE)
        {
            DBG_vPrintf(TRACE_ONOFF, "LC Set to 0");
            switch(((sPayload.u8EffectId) << 8) | (sPayload.u8EffectVariant))
            {
                case CLD_ONOFF_OFF_WITH_EFFECT_FADE_OFF:
                case CLD_ONOFF_OFF_WITH_EFFECT_NO_FADE:
                case CLD_ONOFF_OFF_WITH_EFFECT_DIM_DOWN_FADE_OFF:
                case CLD_ONOFF_OFF_WITH_EFFECT_DIM_UP_FADE_OFF:
                    eCLD_LevelControlSetOnOffState(psEndPointDefinition->u8EndPointNumber,
                                                   FALSE,
                                                   (teCLD_OnOff_OffWithEffect)(((sPayload.u8EffectId) << 8) | (sPayload.u8EffectVariant)));
                    break;
                default:
                    /* For reserved u8EffectId and u8EffectVariant send DefaultResponse with Invalid Value. */
                    eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
                    /* Doing u8EffectId=0|u8EffectVariant=0 (CLD_ONOFF_OFF_WITH_EFFECT_FADE_OFF) */
                    eCLD_LevelControlSetOnOffState(psEndPointDefinition->u8EndPointNumber,
                                                   FALSE,
                                                   CLD_ONOFF_OFF_WITH_EFFECT_FADE_OFF);
                    break;
            }
            
        }
    }
    else
    {
        psSharedStruct->bOnOff = 0x00;
    }
#else
    psSharedStruct->bOnOff = 0x00;
#endif

#ifdef CLD_ONOFF_ATTR_ON_TIME
    /* OnTime = 0 */
    psSharedStruct->u16OnTime = 0;
#endif

    /* Generate a callback to let the app know that we need to do an off effect */
    sCallBackMessage.u8CommandId                            = u8CommandIdentifier;
    sCallBackMessage.uMessage.psOffWithEffectRequestPayload = &sPayload;

    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = pZPSevent;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_CUSTOM;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_ONOFF;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&sCallBackMessage;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    return eStatus;
}
#endif

#ifdef CLD_ONOFF_CMD_ON_WITH_RECALL_GLOBAL_SCENE
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffHandleOnWithRecallGlobalSceneCommand
 **
 ** DESCRIPTION:
 ** Handles an on with global scene recall command
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
PRIVATE  teZCL_Status eCLD_OnOffHandleOnWithRecallGlobalSceneCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;

    tsCLD_OnOff *psSharedStruct = (tsCLD_OnOff *)psClusterInstance->pvEndPointSharedStructPtr;
    DBG_vPrintf(TRACE_ONOFF, "On With Recall Global Scene: ");

    /* Receive the command */
    eStatus = eCLD_OnOffCommandReceive(pZPSevent,
                                &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ONOFF, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    /* 3.8.2.3.5.1 ZCL spec
     * On receipt of the on with recall global scene command, if the 
     * GlobalSceneControl attribute is equal to FALSE, the application on the 
     * associated endpoint shall recall its global scene, entering the
     * appropriate state and updating the OnOff attribute accordingly. It shall
     * then set the GlobalSceneControl attribute to TRUE. If the value of the 
     * OnTime attribute is equal to 0x0000, the device shall then set the 
     * OffWaitTime attribute to 0x0000. In all other cases, the application on 
     * the associated endpoint shall discard the command.
     */
    if(psSharedStruct->bGlobalSceneControl == FALSE)
    {
        /* Before recalling global scene, stopping the level control cluster's ongoing transitions */
        #ifdef LEVEL_CONTROL_SERVER
            eCLD_LevelControlStopTransition(psEndPointDefinition->u8EndPointNumber);
        #endif
        
        /* Recall the global scene */
        #ifdef SCENES_SERVER
            eCLD_ScenesRecall(psEndPointDefinition->u8EndPointNumber, 0, 0);
        #endif
        
        /* See ZCL Specification, 3.8.2.2.2 */
        psSharedStruct->bGlobalSceneControl = 1;

        if(psSharedStruct->u16OnTime == 0)
        {
            psSharedStruct->u16OffWaitTime = 0;
        }
    }
    return eStatus;
}
#endif

#ifdef CLD_ONOFF_CMD_ON_WITH_TIMED_OFF
/****************************************************************************
 **
 ** NAME:       eCLD_OnOffHandleOnWithTimedOffCommand
 **
 ** DESCRIPTION:
 ** Handles an On with timed off command
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
PRIVATE  teZCL_Status eCLD_OnOffHandleOnWithTimedOffCommand(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                       u8CommandIdentifier)
{

    teZCL_Status eStatus;
    uint8 u8TransactionSequenceNumber;
    tsCLD_OnOff *psSharedStruct = (tsCLD_OnOff *)psClusterInstance->pvEndPointSharedStructPtr;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsCLD_OnOffCallBackMessage sCallBackMessage;
    tsCLD_OnOff_OnWithTimedOffRequestPayload sPayload;

    DBG_vPrintf(TRACE_ONOFF, "On With Timed Off: ");

    /* Receive the command */
    eStatus = eCLD_OnOffCommandOnWithTimedOffReceive(pZPSevent,
                                &u8TransactionSequenceNumber,
                                &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_ONOFF, "Error: %d", eStatus);
        return(E_ZCL_FAIL);
    }

    DBG_vPrintf(TRACE_ONOFF, "OnOff=%d OnTime=%d OffTime=%d", sPayload.u8OnOff, sPayload.u16OnTime, sPayload.u16OffTime);
    
    /* Valid range of OnTime and OffWaitTime fields is 0x0000-0xFFFE*/
    if((sPayload.u16OffTime > 0xFFFE) ||
       (sPayload.u16OnTime  > 0xFFFE))
    {
        DBG_vPrintf(TRACE_ONOFF, " Error!");
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_CMDS_INVALID_VALUE;
    }
    
    /*
     * On receipt of this command, if the accept only when on sub-field of the on/off control field is set to
     * 1 and the value of the OnOff attribute is equal to 0x00 (off), the command shall be discarded.
     */
    if(((sPayload.u8OnOff & CLD_ONOFF_OWTO_BIT_ACCEPT_ONLY_WHEN_ON) != 0) && (psSharedStruct->bOnOff == 0))
    {
        return E_ZCL_SUCCESS;
    }

    /*
     * If the value of the OffWaitTime attribute is greater than zero and the value of the OnOff attribute is
     * equal to 0x00, then the device shall set the OffWaitTime attribute to the minimum of the OffWaitTime
     * attribute and the value specified in the off wait time field.
     *
     * In all other cases, the device shall set the OnTime attribute to the maximum of the OnTime attribute and
     * the value specified in the on time field, set the OffWaitTime attribute to the value specified in the off
     * wait time field and set the OnOff attribute to 0x01 (on).
     */
    if((psSharedStruct->u16OffWaitTime > 0) && (psSharedStruct->bOnOff == 0))
    {
        psSharedStruct->u16OffWaitTime = MIN(psSharedStruct->u16OffWaitTime, sPayload.u16OffTime);
    }
    else
    {
        psSharedStruct->u16OnTime = MAX(psSharedStruct->u16OnTime, sPayload.u16OnTime);
        psSharedStruct->u16OffWaitTime = sPayload.u16OffTime;

        /* If already on, exit */
        if(psSharedStruct->bOnOff == 0x01)
        {
            return eStatus;
        }

#if (defined CLD_LEVEL_CONTROL) && (defined LEVEL_CONTROL_SERVER)
        if(eCLD_LevelControlClusterIsPresent(psEndPointDefinition->u8EndPointNumber) == E_ZCL_SUCCESS)
        {
            /* Turn On*/
            eCLD_LevelControlSetOnOffState(psEndPointDefinition->u8EndPointNumber,
                                           TRUE,
                                           CLD_ONOFF_OFF_WITH_EFFECT_NONE);
        }
        else
        {
            psSharedStruct->bOnOff = 0x01;
        }
#else
        psSharedStruct->bOnOff = 0x01;
#endif
    }
    
    /* Generate a callback to let the app know that we need to do an on with timed off */
    sCallBackMessage.u8CommandId                             = u8CommandIdentifier;
    sCallBackMessage.uMessage.psOnWithTimedOffRequestPayload = &sPayload;

    sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = pZPSevent;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_CUSTOM;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = GENERAL_CLUSTER_ID_ONOFF;
    sZCL_CallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&sCallBackMessage;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

    return eStatus;
}
#endif
#endif /* ONOFF_SERVER */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
