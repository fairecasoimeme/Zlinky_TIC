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
 * MODULE:             IASACE Cluster
 *
 * COMPONENT:          ACECommandHandler.c
 *
 * DESCRIPTION:        IASACE cluster definition
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
#include "IASACE.h"
#include "IASACE_internal.h"

#include "dbg.h"

#ifdef DEBUG_CLD_IASACE
#define TRACE_IASACE    TRUE
#else
#define TRACE_IASACE    FALSE
#endif



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_IASACE) && !defined(IASACE_SERVER) && !defined(IASACE_CLIENT)
#error You Must Have either IASACE_SERVER and/or IASACE_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_IASACE
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef IASACE_SERVER
PRIVATE void vCLD_IASACEUpdate(
                            uint8                       u8Sourceu8EndPoint,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_IASACEHandleArm(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_IASACEHandleBypass(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_IASACEHandleEmergency(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_IASACEHandleFire(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);   
PRIVATE teZCL_Status eCLD_IASACEHandlePanic(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);  
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneIDMap(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);  
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneInfo(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
PRIVATE teZCL_Status eCLD_IASACEHandleGetPanelStatus(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);   
PRIVATE teZCL_Status eCLD_IASACEHandleGetBypassedZoneList(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);   
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneStatus (
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                               
#endif
#ifdef IASACE_CLIENT
PRIVATE teZCL_Status eCLD_IASACEHandleArmResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneIDMapResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);  
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneInfoResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_IASACEHandleZoneStatusChanged (
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                             
 PRIVATE teZCL_Status eCLD_IASACEHandlePanelStatusChangedOrGetPanelStatusResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);   
PRIVATE teZCL_Status eCLD_IASACEHandleSetBypassedZoneList(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);   
PRIVATE teZCL_Status eCLD_IASACEHandleBypassResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneStatusResp (
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
 ** NAME:       eCLD_IASACECommandHandler
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
PUBLIC  teZCL_Status eCLD_IASACECommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_IASACECustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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
#ifdef IASACE_SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
                {

                    case E_CLD_IASACE_CMD_ARM:
                        eCLD_IASACEHandleArm(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;

                    case E_CLD_IASACE_CMD_BYPASS:
                        eCLD_IASACEHandleBypass(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                    case E_CLD_IASACE_CMD_EMERGENCY:
                        eCLD_IASACEHandleEmergency(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                     case E_CLD_IASACE_CMD_FIRE:
                        eCLD_IASACEHandleFire(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;

                    case E_CLD_IASACE_CMD_PANIC:
                        eCLD_IASACEHandlePanic(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                    case E_CLD_IASACE_CMD_GET_ZONE_ID_MAP:
                        eCLD_IASACEHandleGetZoneIDMap(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                    case E_CLD_IASACE_CMD_GET_ZONE_INFO:
                        eCLD_IASACEHandleGetZoneInfo(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;

                    case E_CLD_IASACE_CMD_GET_PANEL_STATUS:
                        eCLD_IASACEHandleGetPanelStatus(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                    case E_CLD_IASACE_CMD_GET_BYPASSED_ZONE_LIST:
                        eCLD_IASACEHandleGetBypassedZoneList(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;                    
                    
                    case E_CLD_IASACE_CMD_GET_ZONE_STATUS:
                        eCLD_IASACEHandleGetZoneStatus(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;  
                    
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
#ifdef IASACE_CLIENT
    else
    {
        switch(sZCL_HeaderParams.u8CommandIdentifier)
                {

                    case E_CLD_IASACE_CMD_ARM_RESP:
                        eCLD_IASACEHandleArmResp(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;

                    case E_CLD_IASACE_CMD_GET_ZONE_ID_MAP_RESP:
                        eCLD_IASACEHandleGetZoneIDMapResp(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                    case E_CLD_IASACE_CMD_GET_ZONE_INFO_RESP:
                        eCLD_IASACEHandleGetZoneInfoResp(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                     case E_CLD_IASACE_CMD_ZONE_STATUS_CHANGED:
                        eCLD_IASACEHandleZoneStatusChanged(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;

                    case E_CLD_IASACE_CMD_PANEL_STATUS_CHANGED:
                    case E_CLD_IASACE_CMD_GET_PANEL_STATUS_RESP:
                        eCLD_IASACEHandlePanelStatusChangedOrGetPanelStatusResp(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
                    
                    case E_CLD_IASACE_CMD_SET_BYPASSED_ZONE_LIST:
                        eCLD_IASACEHandleSetBypassedZoneList(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
    
                    case E_CLD_IASACE_CMD_BYPASS_RESP:
                        eCLD_IASACEHandleBypassResp(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;                    
                    
                    case E_CLD_IASACE_CMD_GET_ZONE_STATUS_RESP:
                        eCLD_IASACEHandleGetZoneStatusResp(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;  
                    
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
#ifdef IASACE_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleArm
 **
 ** DESCRIPTION:
 ** Handles Arm command
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
PRIVATE teZCL_Status eCLD_IASACEHandleArm(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_IASACE_ArmPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_ArmRespPayload sRespPayload = {0};
    uint8 u8NumOfEnrolledZones = CLD_IASACE_ZONE_TABLE_SIZE,u8ZoneCount = 0;
    uint8 au8EnrolledZoneIDList[CLD_IASACE_ZONE_TABLE_SIZE];
    uint8 au8ArmDisarmCode[CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE] = {0};
    tsZCL_Address sZCL_Address;
    tsCLD_IASACE_ZoneTable *psZoneTable;

    sPayload.sArmDisarmCode.u8MaxLength = CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE;
    sPayload.sArmDisarmCode.pu8Data = au8ArmDisarmCode;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACEArmReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACEArmReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psArmPayload = &sPayload;
    
   
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(eCLD_IASACEGetZoneTableEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u8ZoneID,&psZoneTable) != E_ZCL_CMDS_SUCCESS)
    {
        sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_NOT_READY_TO_ARM;
    }
    else if((psCommon->asCLD_IASACE_ZoneParameter[sPayload.u8ZoneID].sArmDisarmCode.u8Length != 0 ) &&
        (((psCommon->asCLD_IASACE_ZoneParameter[sPayload.u8ZoneID].sArmDisarmCode.u8Length != sPayload.sArmDisarmCode.u8Length)) ||
        (memcmp(psCommon->asCLD_IASACE_ZoneParameter[sPayload.u8ZoneID].sArmDisarmCode.pu8Data,sPayload.sArmDisarmCode.pu8Data,psCommon->asCLD_IASACE_ZoneParameter[sPayload.u8ZoneID].sArmDisarmCode.u8Length))))
    {
        sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_INVALID_ARM_DISARM_CODE;
    }
    else{
        if(eCLD_IASACEGetEnrolledZones(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,au8EnrolledZoneIDList,&u8NumOfEnrolledZones) == E_ZCL_CMDS_SUCCESS)
        {
            switch(sPayload.eArmMode)
            {
                case E_CLD_IASACE_ARM_MODE_DISARM:
                {                
                    sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_ALREADY_DISARMED;
                    for(u8ZoneCount = 0;u8ZoneCount<u8NumOfEnrolledZones;u8ZoneCount++)
                    {
                        if(psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].u8ZoneStatusFlag & CLD_IASACE_ZONE_STATUS_FLAG_ARM)
                        {
                            sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_ALL_ZONES_DISARMED;
                        }
                        /* Remove zones from bypassed & armed list */
                        psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].u8ZoneStatusFlag &= 0xFC;
                    }
                    
                break;
                }
                case E_CLD_IASACE_ARM_MODE_ARM_DAY_HOME_ZONES_ONLY:
                case E_CLD_IASACE_ARM_MODE_ARM_NIGHT_SLEEP_ZONES_ONLY:
                case E_CLD_IASACE_ARM_MODE_ARM_ALL_ZONES:
                {                
                    if(psCommon->sCLD_IASACE_PanelParameter.ePanelStatus != E_CLD_IASACE_PANEL_STATUS_PANEL_NOT_READY_TO_ARM)
                    {
                        uint8   u8Flag = 0xFF;
                        if(sPayload.eArmMode == E_CLD_IASACE_ARM_MODE_ARM_DAY_HOME_ZONES_ONLY){
                            u8Flag = CLD_IASACE_ZONE_CONFIG_FLAG_DAY_HOME;
                            sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_ONLY_DAY_HOME_ZONES_ARMED;
                        }
                        else if(sPayload.eArmMode == E_CLD_IASACE_ARM_MODE_ARM_NIGHT_SLEEP_ZONES_ONLY){ 
                            u8Flag = CLD_IASACE_ZONE_CONFIG_FLAG_NIGHT_SLEEP;
                            sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_ONLY_NIGHT_SLEEP_ZONES_ARMED;
                        }else{
                            sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_ALL_ZONES_ARMED;
                        }
                            
                        for(u8ZoneCount = 0;u8ZoneCount<u8NumOfEnrolledZones;u8ZoneCount++)
                        {
                            if(!(psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].u8ZoneStatusFlag & CLD_IASACE_ZONE_STATUS_FLAG_BYPASS) &&
                                    (psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].u8ZoneConfigFlag & u8Flag ||
                                    (u8Flag == 0xFF)))
                            {
                                psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].u8ZoneStatusFlag |= CLD_IASACE_ZONE_STATUS_FLAG_ARM;
                            }
                        }
                    }else
                    {
                        sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_NOT_READY_TO_ARM;
                    }
                break;
                }
                default:
                    sRespPayload.eArmNotification = E_CLD_IASACE_ARM_NOTIF_NOT_READY_TO_ARM;
                break;
            }
        }
    }
    
     /* Gives callback to application */
    vCLD_IASACEUpdate(
                        pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                        psEndPointDefinition,
                        psClusterInstance);
                        
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return  eCLD_IASACE_ArmRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     &sRespPayload);
    
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleBypass
 **
 ** DESCRIPTION:
 ** Handles Bypass command
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
PRIVATE teZCL_Status eCLD_IASACEHandleBypass(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_IASACE_BypassPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_BypassRespPayload sRespPayload = {0};
    uint8 u8ZoneCount = 0;
    tsZCL_Address sZCL_Address;
    tsCLD_IASACE_ZoneTable *psZoneTable;
    uint8 au8ArmDisarmCode[CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE] = {0};
    uint8 au8ZoneList[CLD_IASACE_ZONE_TABLE_SIZE] = {0};
    uint8 au8ZoneBypassResult[CLD_IASACE_ZONE_TABLE_SIZE] = {0};

    sPayload.sArmDisarmCode.u8MaxLength = CLD_IASACE_MAX_LENGTH_ARM_DISARM_CODE;
    sPayload.sArmDisarmCode.pu8Data = &au8ArmDisarmCode[0];
    
    sPayload.pu8ZoneID = &au8ZoneList[0];
    sRespPayload.pu8BypassResult = au8ZoneBypassResult;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_BypassReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_BypassReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
   
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psBypassPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    for(u8ZoneCount=0;u8ZoneCount<sPayload.u8NumOfZones;u8ZoneCount++)
    {
        if(sPayload.pu8ZoneID[u8ZoneCount] >= CLD_IASACE_ZONE_TABLE_SIZE)
        {
            sRespPayload.pu8BypassResult[u8ZoneCount] = E_CLD_IASACE_BYPASS_RESULT_INVALID_ZONE_ID;
        }else if(eCLD_IASACEGetZoneTableEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.pu8ZoneID[u8ZoneCount],&psZoneTable) != E_ZCL_CMDS_SUCCESS){
            sRespPayload.pu8BypassResult[u8ZoneCount] = E_CLD_IASACE_BYPASS_RESULT_UNKNOWN_ZONE_ID;
        }else if(psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].u8ZoneConfigFlag & CLD_IASACE_ZONE_CONFIG_FLAG_NOT_BYPASSED){
            sRespPayload.pu8BypassResult[u8ZoneCount] = E_CLD_IASACE_BYPASS_RESULT_ZONE_NOT_BYPASSED;
        }else if(!(psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].u8ZoneConfigFlag & CLD_IASACE_ZONE_CONFIG_FLAG_BYPASS)){
            sRespPayload.pu8BypassResult[u8ZoneCount] = E_CLD_IASACE_BYPASS_RESULT_NOT_ALLOWED;
        }else if((psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].sArmDisarmCode.u8Length != 0 ) &&
                    ((psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].sArmDisarmCode.u8Length != sPayload.sArmDisarmCode.u8Length) ||
                        (memcmp(psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].sArmDisarmCode.pu8Data,sPayload.sArmDisarmCode.pu8Data,psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].sArmDisarmCode.u8Length))))
        {
            sRespPayload.pu8BypassResult[u8ZoneCount] = E_CLD_IASACE_BYPASS_RESULT_INVALID_ARM_DISARM_CODE;
        }else{
            /* Removing it from Armed Zones and adding to bypassed list */
            psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].u8ZoneStatusFlag &= 0xFD;
            psCommon->asCLD_IASACE_ZoneParameter[sPayload.pu8ZoneID[u8ZoneCount]].u8ZoneStatusFlag |= CLD_IASACE_ZONE_STATUS_FLAG_BYPASS; 
            sRespPayload.pu8BypassResult[u8ZoneCount] = E_CLD_IASACE_BYPASS_RESULT_ZONE_BYPASSED;
        }
    }
    
    sRespPayload.u8NumofZones = sPayload.u8NumOfZones;
    
    /* Gives callback to application */
    vCLD_IASACEUpdate(
                        pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                        psEndPointDefinition,
                        psClusterInstance);
                
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return   eCLD_IASACE_BypassRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     &sRespPayload);
    
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleEmergency
 **
 ** DESCRIPTION:
 ** Handles Emergency command
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
PRIVATE teZCL_Status eCLD_IASACEHandleEmergency(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_EmergencyReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_EmergencyReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return  E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleFire
 **
 ** DESCRIPTION:
 ** Handles Fire command
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
PRIVATE teZCL_Status eCLD_IASACEHandleFire(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_FireReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_FireReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return  E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandlePanic
 **
 ** DESCRIPTION:
 ** Handles Panic command
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
PRIVATE teZCL_Status eCLD_IASACEHandlePanic(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_PanicReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_PanicReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return  E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetZoneIDMap
 **
 ** DESCRIPTION:
 ** Handles get zone ID Map command
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneIDMap(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_GetZoneIDMapRespPayload sRespPayload;
    tsZCL_Address sZCL_Address;
    uint8 u8NumOfEnrolledZones = CLD_IASACE_ZONE_TABLE_SIZE,u8ZoneCount = 0;
    uint8 au8EnrolledZoneIDList[CLD_IASACE_ZONE_TABLE_SIZE];

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_GetZoneIDMapReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetZoneIDMapReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    memset(sRespPayload.au16ZoneIDMap,0,CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES*2);
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    if(eCLD_IASACEGetEnrolledZones(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,au8EnrolledZoneIDList,&u8NumOfEnrolledZones) == E_ZCL_CMDS_SUCCESS)
    {
        for(u8ZoneCount = 0;u8ZoneCount<u8NumOfEnrolledZones; u8ZoneCount++)
        {
            sRespPayload.au16ZoneIDMap[(au8EnrolledZoneIDList[u8ZoneCount]/8)] |= (1 << (au8EnrolledZoneIDList[u8ZoneCount]%8));
        }
    }

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return   eCLD_IASACE_GetZoneIDMapRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     &sRespPayload);
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetZoneInfo
 **
 ** DESCRIPTION:
 ** Handles get zone information command
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneInfo(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_GetZoneInfoPayload sPayload = {0};
    tsCLD_IASACE_GetZoneInfoRespPayload sRespPayload = {0};
    tsZCL_Address sZCL_Address;
    tsCLD_IASACE_ZoneTable *psZoneTable;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_GetZoneInfoReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetZoneInfoReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetZoneInfoPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    sRespPayload.u8ZoneID = sPayload.u8ZoneID;
    
    if(eCLD_IASACEGetZoneTableEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u8ZoneID,&psZoneTable) == E_ZCL_CMDS_SUCCESS)
    {
        sRespPayload.u16ZoneType = psZoneTable->u16ZoneType;
        sRespPayload.u64IeeeAddress = psZoneTable->u64IeeeAddress;
        sRespPayload.sZoneLabel = psCommon->asCLD_IASACE_ZoneParameter[sPayload.u8ZoneID].sZoneLabel;
    }else{
        sRespPayload.u16ZoneType = 0xFFFF;
        sRespPayload.u64IeeeAddress = 0xffffffffffffffffULL;
        sRespPayload.sZoneLabel.u8Length = 0;
    }

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return   eCLD_IASACE_GetZoneInfoRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     &sRespPayload);
}
/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetPanelStatus
 **
 ** DESCRIPTION:
 ** Handles get panel status command
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetPanelStatus(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload sRespPayload = {0};
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_GetPanelStatusReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetPanelStatusReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    memcpy(&sRespPayload,(tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload *)&psCommon->sCLD_IASACE_PanelParameter,sizeof(tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload));
    
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return   eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     E_CLD_IASACE_CMD_GET_PANEL_STATUS_RESP,
                                     &sRespPayload);
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetBypassedZoneList
 **
 ** DESCRIPTION:
 ** Handles get bypassed zone list command
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetBypassedZoneList(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_SetBypassedZoneListPayload sRespPayload = {0};
    tsZCL_Address sZCL_Address;
    uint8 u8NumOfEnrolledZones = CLD_IASACE_ZONE_TABLE_SIZE,u8ZoneCount = 0;
    uint8 au8EnrolledZoneIDList[CLD_IASACE_ZONE_TABLE_SIZE],au8BypassList[CLD_IASACE_ZONE_TABLE_SIZE];

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_GetBypassedZoneListReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetBypassedZoneListReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    sRespPayload.u8NumofZones = 0;
    sRespPayload.pu8ZoneID = au8BypassList;
   
    if(eCLD_IASACEGetEnrolledZones(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,au8EnrolledZoneIDList,&u8NumOfEnrolledZones) == E_ZCL_CMDS_SUCCESS)
    {
        for(u8ZoneCount = 0; u8ZoneCount <u8NumOfEnrolledZones ;u8ZoneCount++ )
        {
            if(psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].u8ZoneStatusFlag & CLD_IASACE_ZONE_STATUS_FLAG_BYPASS)
            {
                sRespPayload.pu8ZoneID[sRespPayload.u8NumofZones] = au8EnrolledZoneIDList[u8ZoneCount];
                sRespPayload.u8NumofZones++;
            }
        }
        
    }
    
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return   eCLD_IASACE_SetBypassedZoneListSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     &sRespPayload);
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetZoneStatus
 **
 ** DESCRIPTION:
 ** Handles get zone status command
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneStatus(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_GetZoneStatusPayload sPayload = {0};
    tsCLD_IASACE_GetZoneStatusRespPayload sRespPayload = {0};
    tsZCL_Address sZCL_Address;
    uint8 u8NumOfEnrolledZones = CLD_IASACE_ZONE_TABLE_SIZE,u8ZoneCount = 0,u8PayloadIndex = 0;
    uint8 au8EnrolledZoneIDList[CLD_IASACE_ZONE_TABLE_SIZE];
    uint8 au8ZoneStatus[CLD_IASACE_ZONE_TABLE_SIZE*3] = {0};
    
    sRespPayload.pu8ZoneStatus = au8ZoneStatus;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    /* Receive the command */
    eStatus = eCLD_IASACE_GetZoneStatusReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetZoneStatusReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetZoneStatusPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    sRespPayload.u8NumofZones = 0;
    sRespPayload.bZoneStatusComplete = TRUE;
    
    if(eCLD_IASACEGetEnrolledZones(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,au8EnrolledZoneIDList,&u8NumOfEnrolledZones) == E_ZCL_CMDS_SUCCESS)
    {
        for(u8ZoneCount = 0; u8ZoneCount <u8NumOfEnrolledZones ;u8ZoneCount++ )
        {
            if(au8EnrolledZoneIDList[u8ZoneCount] >= sPayload.u8StartingZoneID)
            {
                if( (sPayload.bZoneStatusMaskFlag == FALSE) ||
                    (sPayload.bZoneStatusMaskFlag && 
                    (psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].eZoneStatus & sPayload.u16ZoneStatusMask)))
                {
                    sRespPayload.pu8ZoneStatus[u8PayloadIndex++] = au8EnrolledZoneIDList[u8ZoneCount];
                    sRespPayload.pu8ZoneStatus[u8PayloadIndex++] = (uint8)(psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].eZoneStatus);
                    sRespPayload.pu8ZoneStatus[u8PayloadIndex++] = (uint8)(psCommon->asCLD_IASACE_ZoneParameter[au8EnrolledZoneIDList[u8ZoneCount]].eZoneStatus >> 8);
                    sRespPayload.u8NumofZones++;
                }
            }
            if(sRespPayload.u8NumofZones > sPayload.u8MaxNumOfZoneID)
            {
                sRespPayload.u8NumofZones = sPayload.u8MaxNumOfZoneID;
                sRespPayload.bZoneStatusComplete = FALSE;
                break;
            }
        
        }
        
    }

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    return   eCLD_IASACE_GetZoneStatusRespSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &u8TransactionSequenceNumber,
                                     &sRespPayload);
}

/****************************************************************************
 **
 ** NAME:       vCLD_IASACEUpdate
 **
 ** DESCRIPTION:
 ** Provides callback to application with ACE cluster update so that user can refresh its data & save PDM  
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
PRIVATE void vCLD_IASACEUpdate(
                            uint8                       u8Sourceu8EndPoint,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    
    /* Generate a callback to let the user know that an update event occurred */
    sZCL_CallBackEvent.u8EndPoint           = u8Sourceu8EndPoint;
    sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
    sZCL_CallBackEvent.pZPSevent            = NULL;
    sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;
    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

}
#endif /*#ifdef IASACE_SERVER */

#ifdef IASACE_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleArmResp
 **
 ** DESCRIPTION:
 ** Handles Arm response
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
PRIVATE teZCL_Status eCLD_IASACEHandleArmResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_ArmRespPayload sPayload = {0};

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_ArmRespReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_ArmRespReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psArmRespPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
         
    return  E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetZoneIDMapResp
 **
 ** DESCRIPTION:
 ** Handles get zone ID Map response
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneIDMapResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_GetZoneIDMapRespPayload sPayload;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    memset(sPayload.au16ZoneIDMap,0,CLD_IASACE_MAX_BYTES_FOR_NUM_OF_ZONES*2);
    
    /* Receive the command */
    eStatus = eCLD_IASACE_GetZoneIDMapRespReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetZoneIDMapRespReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetZoneIDMapRespPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return   E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetZoneInfoResp
 **
 ** DESCRIPTION:
 ** Handles get zone information response
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneInfoResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_GetZoneInfoRespPayload sPayload = {0};
    uint8 au8ZoneLabel[CLD_IASACE_MAX_LENGTH_ZONE_LABEL];

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    sPayload.sZoneLabel.u8MaxLength = CLD_IASACE_MAX_LENGTH_ZONE_LABEL;
    sPayload.sZoneLabel.pu8Data = au8ZoneLabel;
    
    /* Receive the command */
    eStatus = eCLD_IASACE_GetZoneInfoRespReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetZoneInfoRespReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetZoneInfoRespPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return   E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleZoneStatusChanged
 **
 ** DESCRIPTION:
 ** Handles zone status changed command
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
PRIVATE teZCL_Status eCLD_IASACEHandleZoneStatusChanged(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_ZoneStatusChangedPayload sPayload = {0};
    uint8 au8ZoneLabel[CLD_IASACE_MAX_LENGTH_ZONE_LABEL];

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;
    
    sPayload.sZoneLabel.u8MaxLength = CLD_IASACE_MAX_LENGTH_ZONE_LABEL;
    sPayload.sZoneLabel.pu8Data = au8ZoneLabel;

    /* Receive the command */
    eStatus = eCLD_IASACE_ZoneStatusChangedReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_ZoneStatusChangedReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psZoneStatusChangedPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return   E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandlPanelStatusChangedOrGetPanelStatusResp
 **
 ** DESCRIPTION:
 ** Handles panel status changed command or get panel status response
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
PRIVATE teZCL_Status eCLD_IASACEHandlePanelStatusChangedOrGetPanelStatusResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespPayload sPayload = {0};

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_PanelStatusChangedOrGetPanelStatusRespReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psPanelStatusChangedOrGetPanelStatusRespPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return   E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleSetBypassedZoneList
 **
 ** DESCRIPTION:
 ** Handles set bypassed zone list command
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
PRIVATE teZCL_Status eCLD_IASACEHandleSetBypassedZoneList(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_SetBypassedZoneListPayload sPayload = {0};
    uint8 au8BypassList[CLD_IASACE_ZONE_TABLE_SIZE];
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    sPayload.pu8ZoneID = au8BypassList;
    
    /* Receive the command */
    eStatus = eCLD_IASACE_SetBypassedZoneListReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_SetBypassedZoneListReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psSetBypassedZoneListPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
 
    return   E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleBypassResp
 **
 ** DESCRIPTION:
 ** Handles Bypass response
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
PRIVATE teZCL_Status eCLD_IASACEHandleBypassResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_BypassRespPayload sPayload = {0};
    uint8 au8ZoneBypassResult[CLD_IASACE_ZONE_TABLE_SIZE] = {0};

    sPayload.pu8BypassResult = au8ZoneBypassResult;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_BypassRespReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_BypassRespReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psBypassRespPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return   E_ZCL_SUCCESS;
    
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASACEHandleGetZoneStatusResp
 **
 ** DESCRIPTION:
 ** Handles get zone status response
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
PRIVATE teZCL_Status eCLD_IASACEHandleGetZoneStatusResp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber;
    tsCLD_IASACECustomDataStructure *psCommon;
    tsCLD_IASACE_GetZoneStatusRespPayload sPayload = {0};
    uint8 au8ZoneStatus[CLD_IASACE_ZONE_TABLE_SIZE*3] = {0};
    
    sPayload.pu8ZoneStatus = au8ZoneStatus;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_IASACECustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_IASACE_GetZoneStatusRespReceive(
                                        pZPSevent,
                                        &u8TransactionSequenceNumber,
                                        &sPayload);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_IASACE, "eCLD_IASACE_GetZoneStatusRespReceive Error: %d\r\n", eStatus);
        return eStatus;
    }
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetZoneStatusRespPayload = &sPayload;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return   E_ZCL_SUCCESS;
}

#endif /* IASACE_CLIENT */

#endif /*#ifdef  CLD_IASACE */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

