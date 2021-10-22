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
 * MODULE:             Power Profile Cluster
 *
 * COMPONENT:          PowerProfileCommandHandler.c
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

#include "PowerProfile.h"
#include "PowerProfile_internal.h"

#include "zcl_options.h"

#include "dbg.h"



#ifdef DEBUG_CLD_PP
#define TRACE_PP    TRUE
#else
#define TRACE_PP    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_PP) && !defined(PP_SERVER) && !defined(PP_CLIENT)
#error You Must Have either PP_SERVER and/or PP_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef PP_SERVER
PRIVATE teZCL_Status eCLD_PPHandlePowerProfileReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_PPHandlePowerProfileStateReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
PRIVATE teZCL_Status eCLD_PPHandleGetPowerProfilePriceRsp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
PRIVATE teZCL_Status eCLD_PPHandleGetOverallSchedulePriceRsp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
PRIVATE teZCL_Status eCLD_PPHandleEnergyPhasesSchedule(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset);    
PRIVATE teZCL_Status eCLD_PPHandlePowerProfileScheduleConstraintsReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                                
PRIVATE teZCL_Status eCLD_PPHandleEnergyPhasesScheduleStateReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
#endif

#ifdef PP_CLIENT
PRIVATE teZCL_Status eCLD_PPHandlePowerProfile(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset);
PRIVATE teZCL_Status eCLD_PPHandlePowerProfileState(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset);
PRIVATE teZCL_Status eCLD_PPHandleGetPowerProfilePriceExtended(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_PPHandleGetPowerProfilePrice(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);                            
PRIVATE teZCL_Status eCLD_PPHandleGetOverallSchedulePrice(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_PPHandleEnergyPhasesScheduleReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
PRIVATE teZCL_Status eCLD_PPHandleEnergyPhasesScheduleState(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset);
PRIVATE teZCL_Status eCLD_PPHandlePowerProfileScheduleConstraints(
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
#ifdef CLD_PP
/****************************************************************************
 **
 ** NAME:       eCLD_PPCommandHandler
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
PUBLIC  teZCL_Status eCLD_PPCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

    
	tsZCL_HeaderParams sZCL_HeaderParams;
	tsCLD_PPCustomDataStructure *psCommon;
    uint16 u16HeaderOffset = 0;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Store return address details */
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psCommon->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
    u16HeaderOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

#ifdef PP_SERVER

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == TRUE)
    {

        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_PP_CMD_POWER_PROFILE_REQ):
            {
                eCLD_PPHandlePowerProfileReq(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_PP_CMD_POWER_PROFILE_STATE_REQ):
            {
                eCLD_PPHandlePowerProfileStateReq(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_RSP):
            case (E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED_RSP):
            {
                eCLD_PPHandleGetPowerProfilePriceRsp(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE_RSP):
            {
                eCLD_PPHandleGetOverallSchedulePriceRsp(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
            
            case (E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_NOTIFICATION):
            case (E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_RSP):
            {
                eCLD_PPHandleEnergyPhasesSchedule(pZPSevent, psEndPointDefinition, psClusterInstance,u16HeaderOffset);
                break;
            }
            
            case (E_CLD_PP_CMD_POWER_PROFILE_SCHEDULE_CONSTRAINTS_REQ):
            {
                eCLD_PPHandlePowerProfileScheduleConstraintsReq(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
            case (E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_REQ):
            {
                eCLD_PPHandleEnergyPhasesScheduleStateReq(pZPSevent, psEndPointDefinition, psClusterInstance);
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
    }
#endif

#ifdef PP_CLIENT
    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == FALSE)
    {

        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_PP_CMD_POWER_PROFILE_NOTIFICATION):
            case (E_CLD_PP_CMD_POWER_PROFILE_RSP):
            {
                eCLD_PPHandlePowerProfile(pZPSevent, psEndPointDefinition, psClusterInstance,u16HeaderOffset);
                break;
            }

            case (E_CLD_PP_CMD_POWER_PROFILE_STATE_RSP):
            case (E_CLD_PP_CMD_POWER_PROFILE_STATE_NOTIFICATION):
            {
                eCLD_PPHandlePowerProfileState(pZPSevent, psEndPointDefinition, psClusterInstance,u16HeaderOffset);
                break;
            }

            case (E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE):
            {
                eCLD_PPHandleGetPowerProfilePrice(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
            
            case (E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED):
            {
                eCLD_PPHandleGetPowerProfilePriceExtended(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE):
            {
                eCLD_PPHandleGetOverallSchedulePrice(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
            
            case (E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_REQ):
            {
                eCLD_PPHandleEnergyPhasesScheduleReq(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
            
            case (E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_RSP):
            case (E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_NOTIFICATION):
            {
                eCLD_PPHandleEnergyPhasesScheduleState(pZPSevent, psEndPointDefinition, psClusterInstance,u16HeaderOffset);
                break;
            }
            
            case (E_CLD_PP_CMD_GET_POWER_PROFILE_SCHEDULE_CONSTRAINTS_RSP):
            case (E_CLD_PP_CMD_GET_POWER_PROFILE_SCHEDULE_CONSTRAINTS_NOTIFICATION):
            {
                eCLD_PPHandlePowerProfileScheduleConstraints(pZPSevent, psEndPointDefinition, psClusterInstance);
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
    }
#endif

    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

#ifdef PP_SERVER
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandlePowerProfileReq
 **
 ** DESCRIPTION:
 ** Handles Power Profile Request
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

PRIVATE  teZCL_Status eCLD_PPHandlePowerProfileReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileReqPayload sPayload = {0};
    tsCLD_PP_PowerProfilePayload    sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    uint8 i;
    tsZCL_Address sZCL_Address;
    tsCLD_PPEntry *psPPEntry;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileReqReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uReqMessage.psPowerProfileReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(sPayload.u8PowerProfileId == 0 && (((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum != 0))
    {
        for(i= 0; i < CLD_PP_NUM_OF_POWER_PROFILES; i++)
        {
            if(psCommon->asPowerProfileEntry[i].u8PowerProfileId != 0) {
            sResponse.u8PowerProfileId = psCommon->asPowerProfileEntry[i].u8PowerProfileId;
            sResponse.u8NumOfTransferredPhases = psCommon->asPowerProfileEntry[i].u8NumOfTransferredEnergyPhases;
            sResponse.psEnergyPhaseInfo = psCommon->asPowerProfileEntry[i].asEnergyPhaseInfo;
            break;
            }
        }
    }else if(sPayload.u8PowerProfileId > CLD_PP_NUM_OF_POWER_PROFILES)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
        return E_ZCL_FAIL;
    }else if((eCLD_PPGetPowerProfileEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u8PowerProfileId,&psPPEntry) != E_ZCL_SUCCESS)   ||
        (((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum == 0))
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }else{
        sResponse.u8PowerProfileId = psPPEntry->u8PowerProfileId;
        sResponse.u8NumOfTransferredPhases = psPPEntry->u8NumOfTransferredEnergyPhases;
        sResponse.psEnergyPhaseInfo = psPPEntry->asEnergyPhaseInfo;
    }
    
    sResponse.u8TotalProfileNum = ((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum;

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    eStatus = eCLD_PPPowerProfileSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_PP_CMD_POWER_PROFILE_RSP,
                                                     &sResponse);
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandlePowerProfileStateReq
 **
 ** DESCRIPTION:
 ** Handles Power Profile Request
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

PRIVATE  teZCL_Status eCLD_PPHandlePowerProfileStateReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileStatePayload   sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    uint8 i,j = 0;
    tsZCL_Address sZCL_Address;
    tsCLD_PP_PowerProfileRecord asPowerProfileRecord[CLD_PP_NUM_OF_POWER_PROFILES];
    
    sResponse.psPowerProfileRecord = asPowerProfileRecord;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileStateReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileStateReqReceive:%d", eStatus);
        return eStatus;
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum == 0)
    {
        sResponse.u8PowerProfileCount = 0;
    }else{  
    for(i= 0; i < CLD_PP_NUM_OF_POWER_PROFILES; i++)    
    {
        if(psCommon->asPowerProfileEntry[i].u8PowerProfileId != 0)
        {
            sResponse.psPowerProfileRecord[j].u8PowerProfileId = psCommon->asPowerProfileEntry[i].u8PowerProfileId;
            sResponse.psPowerProfileRecord[j].bPowerProfileRemoteControl = psCommon->asPowerProfileEntry[i].bPowerProfileRemoteControl;
            sResponse.psPowerProfileRecord[j].u8PowerProfileState = psCommon->asPowerProfileEntry[i].u8PowerProfileState;
            if(sResponse.psPowerProfileRecord[j].u8PowerProfileState == E_CLD_PP_STATE_IDLE || 
                sResponse.psPowerProfileRecord[j].u8PowerProfileState == E_CLD_PP_STATE_ENDED){
                sResponse.psPowerProfileRecord[j].u8EnergyPhaseId = 0xFF;
            }else{
                sResponse.psPowerProfileRecord[j].u8EnergyPhaseId = psCommon->asPowerProfileEntry[i].u8ActiveEnergyPhaseId;
            }
            j++;
        }
    }
    sResponse.u8PowerProfileCount = ((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum;
    }
    
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    eStatus = eCLD_PPPowerProfileStateSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_PP_CMD_POWER_PROFILE_STATE_RSP,
                                                     &sResponse);
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleGetPowerProfilePriceRsp
 **
 ** DESCRIPTION:
 ** Handles Power Profile Request
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

PRIVATE  teZCL_Status eCLD_PPHandleGetPowerProfilePriceRsp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_GetPowerProfilePriceRspPayload    sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPGetPowerProfilePriceOrExtendedRspReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPGetPowerProfilePriceOrExtendedRspReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psGetPowerProfilePriceRspPayload = &sPayload;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}
/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleGetOverallSchedulePriceRsp
 **
 ** DESCRIPTION:
 ** Handles Power Profile Request
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

PRIVATE  teZCL_Status eCLD_PPHandleGetOverallSchedulePriceRsp(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_GetOverallSchedulePriceRspPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPGetOverallSchedulePriceRspReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPGetOverallSchedulePriceRspReceive:%d", eStatus);
        return eStatus;
    }

    
    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psGetOverallSchedulePriceRspPayload = &sPayload;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleEnergyPhasesSchedule
 **
 ** DESCRIPTION:
 ** Handles Energy Phases Schedule
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

PRIVATE  teZCL_Status eCLD_PPHandleEnergyPhasesSchedule(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset)
{

    teZCL_Status eStatus;
    tsCLD_PP_EnergyPhasesSchedulePayload    sPayload = {0};
    tsCLD_PP_EnergyPhasesSchedulePayload sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    uint8 i,j,u8NumOfScheduledPhases = 0;
    tsZCL_Address sZCL_Address;
    tsCLD_PPEntry *psPPEntry;
    tsCLD_PP_EnergyPhaseDelay     asEnergyPhaseDelay[CLD_PP_NUM_OF_ENERGY_PHASES];
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    sPayload.psEnergyPhaseDelay = asEnergyPhaseDelay;
    /* Receive the command */
    eStatus = eCLD_PPEnergyPhasesScheduleReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload,
                                            u16HeaderOffset);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPEnergyPhasesScheduleReceive:%d", eStatus);
        return eStatus;
    }
    
    if(eCLD_PPGetPowerProfileEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u8PowerProfileId,&psPPEntry) == E_ZCL_SUCCESS)
    {
        if(psPPEntry->bPowerProfileRemoteControl)
        {
            for(i = 0; i <psPPEntry->u8NumOfTransferredEnergyPhases; i++ )
            {
                if((psPPEntry->asEnergyPhaseInfo[i].u16MaxActivationDelay == 0))
                {
                    eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
                    return E_ZCL_FAIL;
                }
            }
            
            for (i = 0 ; i <sPayload.u8NumOfScheduledPhases ; i++)
            {
                for(j = 0; j<psPPEntry->u8NumOfTransferredEnergyPhases; j++)
                {
                    if(psPPEntry->asEnergyPhaseInfo[j].u8EnergyPhaseId == sPayload.psEnergyPhaseDelay[i].u8EnergyPhaseId)
                    {
                        if(u8NumOfScheduledPhases == 0)
                        {
                            memset(psPPEntry->asEnergyPhaseDelay,0, sizeof(tsCLD_PP_EnergyPhaseDelay) * CLD_PP_NUM_OF_ENERGY_PHASES);
                            psPPEntry->u8NumOfScheduledEnergyPhases = 0;
                        }
                        psPPEntry->asEnergyPhaseDelay[u8NumOfScheduledPhases].u8EnergyPhaseId = sPayload.psEnergyPhaseDelay[i].u8EnergyPhaseId;
                        psPPEntry->asEnergyPhaseDelay[u8NumOfScheduledPhases].u16ScheduleTime = sPayload.psEnergyPhaseDelay[i].u16ScheduleTime;
                        psPPEntry->u8NumOfScheduledEnergyPhases++;
                        u8NumOfScheduledPhases++;
                    }
                }
                        if(u8NumOfScheduledPhases == CLD_PP_NUM_OF_ENERGY_PHASES)
                        {
                            break;
                        }
            }
                            
        }
    }
    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psEnergyPhasesSchedulePayload = &sPayload;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(u8NumOfScheduledPhases)
    {
        // build address structure
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
        
        sResponse.u8PowerProfileId = sPayload.u8PowerProfileId;
        sResponse.u8NumOfScheduledPhases = u8NumOfScheduledPhases;
        sResponse.psEnergyPhaseDelay = psPPEntry->asEnergyPhaseDelay;
        
        eStatus = eCLD_PPEnergyPhasesScheduleSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_NOTIFICATION,
                                                     TRUE,
                                                     &sResponse);
        
        if((psPPEntry->u8PowerProfileState != E_CLD_PP_STATE_RUNNING) &&
         (psPPEntry->u8PowerProfileState != E_CLD_PP_STATE_PAUSED)){
            psPPEntry->u8PowerProfileState = E_CLD_PP_STATE_WAITING_TO_START;
        }
        
        psCommon->bOverrideRunning = TRUE;
        psCommon->u8ActSchPhaseIndex = 0;
        return eStatus;
    }
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandlePowerProfileScheduleConstraintsReq
 **
 ** DESCRIPTION:
 ** Handles Power Profile Request
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

PRIVATE  teZCL_Status eCLD_PPHandlePowerProfileScheduleConstraintsReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileReqPayload sPayload = {0};
    tsCLD_PP_PowerProfileScheduleConstraintsPayload sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsCLD_PPEntry *psPPEntry;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileReqReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uReqMessage.psPowerProfileReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if((eCLD_PPGetPowerProfileEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u8PowerProfileId,&psPPEntry) != E_ZCL_SUCCESS)     ||
        (((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum == 0))
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }else{
        sResponse.u8PowerProfileId = psPPEntry->u8PowerProfileId;
        sResponse.u16StartAfter = psPPEntry->u16StartAfter;
        sResponse.u16StopBefore = psPPEntry->u16StopBefore;
    }
    
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    eStatus = eCLD_PPPowerProfileScheduleConstraintsSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_PP_CMD_GET_POWER_PROFILE_SCHEDULE_CONSTRAINTS_RSP,
                                                     &sResponse);
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleEnergyPhasesScheduleStateReq
 **
 ** DESCRIPTION:
 ** Handles Energy Phase Schedule State Request
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

PRIVATE  teZCL_Status eCLD_PPHandleEnergyPhasesScheduleStateReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileReqPayload sPayload = {0};
    tsCLD_PP_EnergyPhasesSchedulePayload    sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsCLD_PPEntry *psPPEntry;
    tsZCL_Address sZCL_Address;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileReqReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uReqMessage.psPowerProfileReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if((eCLD_PPGetPowerProfileEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,sPayload.u8PowerProfileId,&psPPEntry) != E_ZCL_SUCCESS)     ||
        (((tsCLD_PP *)(psClusterInstance->pvEndPointSharedStructPtr))->u8TotalProfileNum == 0))
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }else{
        sResponse.u8PowerProfileId = psPPEntry->u8PowerProfileId;
        sResponse.u8NumOfScheduledPhases = psPPEntry->u8NumOfScheduledEnergyPhases;
        if(psPPEntry->u8NumOfScheduledEnergyPhases)
        {
            sResponse.psEnergyPhaseDelay = psPPEntry->asEnergyPhaseDelay;
        }
        
    }
    
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    eStatus = eCLD_PPEnergyPhasesScheduleSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_RSP,
                                                     TRUE,
                                                     &sResponse);
    return eStatus;
}
#endif

#ifdef PP_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_PPHandlePowerProfile
 **
 ** DESCRIPTION:
 ** Handles Power Profile Response / Notification
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

PRIVATE  teZCL_Status eCLD_PPHandlePowerProfile(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfilePayload   sCallBackPayload = {0};
    tsCLD_PP_EnergyPhaseInfo sEnergyPhaseInfo;
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    uint16 u16ActualQuantity;
    int i = 0;
    
    sCallBackPayload.psEnergyPhaseInfo = &sEnergyPhaseInfo; 


    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,                &sCallBackPayload.u8TotalProfileNum},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,                &sCallBackPayload.u8PowerProfileId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,                &sCallBackPayload.u8NumOfTransferredPhases},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,                &sCallBackPayload.psEnergyPhaseInfo->u8EnergyPhaseId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,                &sCallBackPayload.psEnergyPhaseInfo->u8MacroPhaseId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,               &sCallBackPayload.psEnergyPhaseInfo->u16ExpectedDuration},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,               &sCallBackPayload.psEnergyPhaseInfo->u16PeakPower},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,               &sCallBackPayload.psEnergyPhaseInfo->u16Energy},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,               &sCallBackPayload.psEnergyPhaseInfo->u16MaxActivationDelay},
                                                };
        /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                     &u8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileReceive:%d", eStatus);
        return eStatus;
    }else{
        /* Message data for callback */
        psCommon->sCallBackMessage.uRespMessage.psPowerProfilePayload = &sCallBackPayload;
    
        /* call callback */
        psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
        
        u16ActualQuantity = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
    
        /* sizeof(tsCLD_PP_PowerProfilePayload)  = 13 bytes*/
        u16HeaderOffset += 13;
        
        for( i = 1 ; i < sCallBackPayload.u8NumOfTransferredPhases ; i++ ) 
        {
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT8,  &sCallBackPayload.psEnergyPhaseInfo->u8EnergyPhaseId);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT8,  &sCallBackPayload.psEnergyPhaseInfo->u8MacroPhaseId);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT16, &sCallBackPayload.psEnergyPhaseInfo->u16ExpectedDuration);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT16, &sCallBackPayload.psEnergyPhaseInfo->u16PeakPower);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT16, &sCallBackPayload.psEnergyPhaseInfo->u16Energy);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT16, &sCallBackPayload.psEnergyPhaseInfo->u16MaxActivationDelay);
            
            /* Message data for callback */
            psCommon->sCallBackMessage.uRespMessage.psPowerProfilePayload = &sCallBackPayload;

            /* call callback */
            psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
        }
        
        if((u16ActualQuantity - u16HeaderOffset) != 0){
            eStatus =  E_ZCL_FAIL;
        }
    }
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandlePowerProfileState
 **
 ** DESCRIPTION:
 ** Handles Power Profile State Response / Notification
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

PRIVATE  teZCL_Status eCLD_PPHandlePowerProfileState(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileStatePayload  sCallBackPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    uint16 u16ActualQuantity;
    int i = 0;
    tsCLD_PP_PowerProfileRecord sPowerProfileRecord;
    
    sCallBackPayload.psPowerProfileRecord = &sPowerProfileRecord;


    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,                &sCallBackPayload.u8PowerProfileCount},
                                                };

        /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    eStatus =  eZCL_CustomCommandReceive(pZPSevent,
                                     &u8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
    
    
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileStateReceive:%d", eStatus);
        return eStatus;
    }else{
        if(sCallBackPayload.u8PowerProfileCount != 0)
        {       
            u16ActualQuantity = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
        
            /* 1 byte for the power count */
            u16HeaderOffset += 1; 
            
            for( i = 0 ; i < sCallBackPayload.u8PowerProfileCount ; i++ ) 
            {
                u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT8,  &sCallBackPayload.psPowerProfileRecord->u8PowerProfileId);
                u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT8,  &sCallBackPayload.psPowerProfileRecord->u8EnergyPhaseId);
                u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_BOOL,   &sCallBackPayload.psPowerProfileRecord->bPowerProfileRemoteControl);
                u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_ENUM8,  &sCallBackPayload.psPowerProfileRecord->u8PowerProfileState);
            
                /* Message data for callback */
                psCommon->sCallBackMessage.uRespMessage.psPowerProfileStatePayload = &sCallBackPayload;
        
                /* call callback */
                psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
            }
        
            if((u16ActualQuantity - u16HeaderOffset) != 0){
                eStatus =  E_ZCL_FAIL;
            }
        }else{
                /* Message data for callback */
                psCommon->sCallBackMessage.uRespMessage.psPowerProfileStatePayload = &sCallBackPayload;
                /* call callback */
                psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
            }
    }
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleGetPowerProfilePriceExtended
 **
 ** DESCRIPTION:
 ** Handles Power Profile Price Extended Request 
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

PRIVATE  teZCL_Status eCLD_PPHandleGetPowerProfilePriceExtended(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_GetPowerProfilePriceExtendedPayload sExtendedPayload = {0};
    tsCLD_PP_GetPowerProfilePriceRspPayload    sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPGetPowerProfilePriceExtendedReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sExtendedPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPGetPowerProfilePriceExtendedReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uReqMessage.psGetPowerProfilePriceExtendedPayload = &sExtendedPayload;

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psGetPowerProfilePriceRspPayload = &sResponse;

    psCommon->sCallBackMessage.bIsInfoAvailable = FALSE;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(psCommon->sCallBackMessage.bIsInfoAvailable)
    {
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
        eStatus = eCLD_PPGetPowerProfilePriceOrExtendedRspSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                        pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                        &sZCL_Address,
                                                        &u8TransactionSequenceNumber,
                                                        E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED_RSP,
                                                        &sResponse); 
        return eStatus;
    }else{
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }
    
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleGetPowerProfilePrice
 **
 ** DESCRIPTION:
 ** Handles Power Profile Price 
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

PRIVATE  teZCL_Status eCLD_PPHandleGetPowerProfilePrice(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileReqPayload sPayload = {0};
    tsCLD_PP_GetPowerProfilePriceRspPayload    sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
   
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileReqReceive:%d", eStatus);
        return eStatus;
    }
   
    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psGetPowerProfilePriceRspPayload = &sResponse;
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uReqMessage.psPowerProfileReqPayload = &sPayload;
    
    psCommon->sCallBackMessage.bIsInfoAvailable = FALSE;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    if(psCommon->sCallBackMessage.bIsInfoAvailable)
    {
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
        eStatus = eCLD_PPGetPowerProfilePriceOrExtendedRspSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                        pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                        &sZCL_Address,
                                                        &u8TransactionSequenceNumber,
                                                        E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_RSP,
                                                        &sResponse);
        return eStatus;
    }else{
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }
}
/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleGetOverallSchedulePrice
 **
 ** DESCRIPTION:
 ** Handles Get Overall Schedule Price Request
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

PRIVATE  teZCL_Status eCLD_PPHandleGetOverallSchedulePrice(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PP_GetOverallSchedulePriceRspPayload sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPGetOverallSchedulePriceReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPGetOverallSchedulePriceReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psGetOverallSchedulePriceRspPayload = &sResponse;

    psCommon->sCallBackMessage.bIsInfoAvailable = FALSE;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(psCommon->sCallBackMessage.bIsInfoAvailable)
    {
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
    
        eStatus = eCLD_PPGetOverallSchedulePriceRspSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                        pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                        &sZCL_Address,
                                                        &u8TransactionSequenceNumber,
                                                        &sResponse);
        return eStatus;
    }else{
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleEnergyPhasesScheduleReq
 **
 ** DESCRIPTION:
 ** Handles Energy Phases Schedule Request
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

PRIVATE  teZCL_Status eCLD_PPHandleEnergyPhasesScheduleReq(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{


    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileReqPayload sPayload = {0};
    tsCLD_PP_EnergyPhasesSchedulePayload    sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileReqReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileReqReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uReqMessage.psPowerProfileReqPayload = &sPayload;
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psEnergyPhasesSchedulePayload = &sResponse;

    psCommon->sCallBackMessage.bIsInfoAvailable = FALSE;
    
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    if(psCommon->sCallBackMessage.bIsInfoAvailable)
    {
        eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);
    
        eStatus = eCLD_PPEnergyPhasesScheduleSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_RSP,
                                                     FALSE,
                                                     &sResponse);
        return eStatus;
    }else{
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_FOUND);
        return E_ZCL_FAIL;
    }
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandleEnergyPhasesScheduleState
 **
 ** DESCRIPTION:
 ** Handles Energy Phases Schedule State Response/ Notification
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

PRIVATE  teZCL_Status eCLD_PPHandleEnergyPhasesScheduleState(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            uint16                      u16HeaderOffset)
{


    teZCL_Status eStatus;
    tsCLD_PP_EnergyPhasesSchedulePayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;
    tsCLD_PP_EnergyPhaseDelay  sEnergyPhaseDelay;
    uint16 u16ActualQuantity;
    int i = 0;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    sPayload.psEnergyPhaseDelay = &sEnergyPhaseDelay;
    
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &sPayload.u8PowerProfileId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &sPayload.u8NumOfScheduledPhases},
                                                };

    eStatus  =  eZCL_CustomCommandReceive(pZPSevent,
                                     &u8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
    
    if(eStatus == E_ZCL_SUCCESS)
    {
        u16ActualQuantity = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
        
        /* 1 byte for Profile Id + 1 byte for schedule phases + structure size (sizeof(tsCLD_PP_EnergyPhaseDelay))) */
        u16HeaderOffset += 2; 
        
        for( i = 0 ; i < sPayload.u8NumOfScheduledPhases ; i++ ) 
        {
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT8,  &sPayload.psEnergyPhaseDelay[0].u8EnergyPhaseId);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT16, &sPayload.psEnergyPhaseDelay[0].u16ScheduleTime);
            /* Message data for callback */
            psCommon->sCallBackMessage.uRespMessage.psEnergyPhasesSchedulePayload = &sPayload;
        
                /* call callback */
            psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
        }
        
        if((u16HeaderOffset - u16ActualQuantity ) == 0){
            eStatus =  E_ZCL_FAIL;
        }
    }
    
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPHandlePowerProfileScheduleConstraints
 **
 ** DESCRIPTION:
 ** Handles Power Profile Schedule Constraints Response/ Notification
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

PRIVATE  teZCL_Status eCLD_PPHandlePowerProfileScheduleConstraints(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{


    teZCL_Status eStatus;
    tsCLD_PP_PowerProfileScheduleConstraintsPayload sPayload = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PPCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PPCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PPPowerProfileScheduleConstraintsReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_PP, "\nErr:eCLD_PPPowerProfileScheduleConstraintsReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uRespMessage.psPowerProfileScheduleConstraintsPayload = &sPayload;


    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    

    return E_ZCL_SUCCESS;
}
#endif
#endif /* CLD_PP */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
