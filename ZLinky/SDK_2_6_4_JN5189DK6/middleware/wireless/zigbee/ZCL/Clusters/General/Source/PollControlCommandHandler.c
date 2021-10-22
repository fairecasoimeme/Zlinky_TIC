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
 * MODULE:             Poll Control Cluster
 *
 * COMPONENT:          PollControlCommandHandler.c
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

#include "PollControl.h"
#include "PollControl_internal.h"

#include "zcl_options.h"

#include "dbg.h"



#ifdef DEBUG_CLD_POLL_CONTROL
#define TRACE_POLL_CONTROL    TRUE
#else
#define TRACE_POLL_CONTROL    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_POLL_CONTROL) && !defined(POLL_CONTROL_SERVER) && !defined(POLL_CONTROL_CLIENT)
#error You Must Have either POLL_CONTROL_SERVER and/or POLL_CONTROL_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef POLL_CONTROL_CLIENT
PRIVATE teZCL_Status eCLD_PollControlHandleCheckin(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif

#ifdef POLL_CONTROL_SERVER
PRIVATE teZCL_Status eCLD_PollControlHandleCheckinResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eCLD_PollControlHandleFastPollStop(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);

#ifdef CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL                          
PRIVATE teZCL_Status eCLD_PollControlHandleSetLongPollInterval(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif                          
                            
#ifdef CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL                         
PRIVATE teZCL_Status eCLD_PollControlHandleSetShortPollInterval(
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
 ** NAME:       eCLD_PollControlCommandHandler
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
PUBLIC  teZCL_Status eCLD_PollControlCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;

    #ifdef POLL_CONTROL_SERVER
        teZCL_Status eStatus = E_ZCL_FAIL;
    #endif

    tsCLD_PollControlCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PollControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

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

#ifdef POLL_CONTROL_SERVER

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == TRUE)
    {
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_POLL_CONTROL_CMD_CHECKIN):
            {
                eStatus = eCLD_PollControlHandleCheckinResponse(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_POLL_CONTROL_CMD_FAST_POLL_STOP):
            {
                eStatus = eCLD_PollControlHandleFastPollStop(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

#ifdef CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL          
            case (E_CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL):
            {
                eStatus = eCLD_PollControlHandleSetLongPollInterval(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
#endif

#ifdef CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL         
            case (E_CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL):
            {
                eStatus = eCLD_PollControlHandleSetShortPollInterval(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }
#endif
            
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

#ifdef POLL_CONTROL_CLIENT
    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == FALSE)
    {

        if (sZCL_HeaderParams.u8CommandIdentifier == E_CLD_POLL_CONTROL_CMD_CHECKIN){
            eCLD_PollControlHandleCheckin(pZPSevent, psEndPointDefinition, psClusterInstance);
        }else{
            // unlock
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

            return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
        }
    }
#endif

    /* Added the check to make sure the default response with status success is centralized */
#ifdef POLL_CONTROL_SERVER
    if((eStatus == E_ZCL_SUCCESS) && !(sZCL_HeaderParams.bDisableDefaultResponse))
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SUCCESS);
#endif
        
    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

#ifdef POLL_CONTROL_SERVER
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlHandleCheckinResponse
 **
 ** DESCRIPTION:
 ** Handles Check in Response
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

PRIVATE  teZCL_Status eCLD_PollControlHandleCheckinResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{
    teZCL_Status eStatus;
    tsCLD_PollControl_CheckinResponsePayload sPayload;
    uint8   u8TransactionSequenceNumber,u8NumOfClients;
    tsCLD_PollControlCustomDataStructure *psCommon;
    bool_t bEntryFound;
    
    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PollControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PollControlCheckinResponseReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_POLL_CONTROL, "\nErr:eCLD_PollControlCheckinResponseReceive:%d", eStatus);
        return eStatus;
    }
    
    if(psCommon->sControlParameters.u8CurrentClient > CLD_POLL_CONTROL_NUMBER_OF_MULTIPLE_CLIENTS)
    {
        return E_ZCL_FAIL;
    }
    
    psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u8EndPointId = pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint;
    psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u16Address = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
    
    u8NumOfClients =  u8CLD_PollControlFindCheckinEntry(&(psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient]),&bEntryFound);
    if(FALSE == bEntryFound )
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_ACTION_DENIED);
        return E_ZCL_CMDS_ACTION_DENIED;    
    }
    else if( FALSE == psCommon->sControlParameters.bFastPollMode)
    {
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_TIMEOUT);
        return E_ZCL_CMDS_TIMEOUT;  
    }
    else
    {
        if(sPayload.bStartFastPolling)
        {
#ifdef CLD_POLL_CONTROL_ATTR_FAST_POLL_TIMEOUT_MAX
            if( (sPayload.u16FastPollTimeout > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16FastPollTimeoutMax))
            {
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
                return E_ZCL_ERR_INVALID_VALUE;
            }   
#endif  
            if(!(sPayload.u16FastPollTimeout))
            {
                psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u16FastPollTimeout = ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16FastPollTimeout;
            }
            else
            {
                psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u16FastPollTimeout = sPayload.u16FastPollTimeout;
            }
            if((psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u16FastPollTimeout > psCommon->sControlParameters.u16CurrentFastPollTimeout) ||
               (psCommon->sControlParameters.u8CurrentClient == 0))
            {
                psCommon->sControlParameters.u16FastPollTimeoutMax = psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u16FastPollTimeout;
                psCommon->sControlParameters.u16CurrentFastPollTimeout = psCommon->sControlParameters.asCheckinEntry[psCommon->sControlParameters.u8CurrentClient].u16FastPollTimeout;
                psCommon->sControlParameters.bFastPollMode = TRUE;
                psCommon->sControlParameters.u32PollTickUpdate = 0;
            } 
        }
        else
        {
            if(u8NumOfClients == 1)
            {
                psCommon->sControlParameters.u16CurrentFastPollTimeout = 0;
            }                   
        }
        psCommon->sControlParameters.u8CurrentClient++;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psCheckinResponsePayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlHandleFastPollStop
 **
 ** DESCRIPTION:
 ** Handles Fast Poll Stop
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

PRIVATE  teZCL_Status eCLD_PollControlHandleFastPollStop(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    uint8   u8TransactionSequenceNumber,u8NumOfClients,j,i;
    tsCLD_PollControlCustomDataStructure *psCommon;
    bool_t    bEntryFound,bStopPoll = FALSE;
    tsCLD_PollControlCheckinEntry sCheckinEntry;
    uint16 u16FastPollDifference;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PollControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PollControlFastPollStopReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_POLL_CONTROL, "\nErr:eCLD_PollControlFastPollStopReceive:%d", eStatus);
        return eStatus;
    }

    sCheckinEntry.u8EndPointId = pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint;
    sCheckinEntry.u16Address = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
    
    u8NumOfClients =  u8CLD_PollControlFindCheckinEntry(&sCheckinEntry,&bEntryFound);
    
    u16FastPollDifference = psCommon->sControlParameters.u16FastPollTimeoutMax - psCommon->sControlParameters.u16CurrentFastPollTimeout;

    if(bEntryFound && psCommon->sControlParameters.bFastPollMode)
    {
        bStopPoll = FALSE;
        if(u8NumOfClients > 1)
        {
            for(j = 0 ; j <u8NumOfClients ; j++)
            {
                if((sCheckinEntry.u8EndPointId == psCommon->sControlParameters.asCheckinEntry[j].u8EndPointId) &&
                (sCheckinEntry.u16Address == psCommon->sControlParameters.asCheckinEntry[j].u16Address))
                {
                    psCommon->sControlParameters.asCheckinEntry[j].bFastPollStop = TRUE;
                    if(psCommon->sControlParameters.asCheckinEntry[j].u16FastPollTimeout == psCommon->sControlParameters.u16FastPollTimeoutMax)
                    {
                        for(i = 0 ; i <u8NumOfClients ; i++)
                        {
                            if(i != j)
                            {
                                if(u16FastPollDifference >= psCommon->sControlParameters.asCheckinEntry[i].u16FastPollTimeout){
                                    bStopPoll = TRUE;
                                }else{
                                    bStopPoll = FALSE;
                                    break;
                                }
                            }
                        }
                        if(bStopPoll)
                        {
                            psCommon->sControlParameters.u16CurrentFastPollTimeout = 0;
                        }
                    }
                }
            }
            for(j = 0 ; j < u8NumOfClients ; j++)
            {
                if(psCommon->sControlParameters.asCheckinEntry[j].bFastPollStop){
                    if(j == (u8NumOfClients - 1))
                        bStopPoll = TRUE;
                }else{
                    bStopPoll = FALSE;
                    break;
                }
            } 
            if(bStopPoll)
            {
                psCommon->sControlParameters.u16CurrentFastPollTimeout = 0;
            }else{
                /* unable to stop fast polling due to the fact that there is another bound client which has requested that polling continue*/
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_ACTION_DENIED);
                return E_ZCL_CMDS_ACTION_DENIED; 
            }
        }else{
            psCommon->sControlParameters.u16CurrentFastPollTimeout = 0;
        }
    }else
    {
        /*No Paired Client in the entry & Not in fast Poll Mode*/
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_ACTION_DENIED);
        return E_ZCL_CMDS_ACTION_DENIED;      
    }

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
   
    return E_ZCL_SUCCESS;
}

#ifdef CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlHandleSetLongPollInterval
 **
 ** DESCRIPTION:
 ** Handles set long poll interval
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

PRIVATE  teZCL_Status eCLD_PollControlHandleSetLongPollInterval(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PollControl_SetLongPollIntervalPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_PollControlCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PollControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PollControlSetLongPollIntervalReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_POLL_CONTROL, "\nErr:eCLD_PollControlSetLongPollIntervalReceive:%d", eStatus);
        return eStatus;
    }

    
    if( (sPayload.u32NewLongPollInterval > CLD_POLL_CONTROL_LONG_POLL_INTERVAL_MAX) || 
        (sPayload.u32NewLongPollInterval > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinInterval) ||
            (sPayload.u32NewLongPollInterval < ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ShortPollInterval)
#ifdef CLD_POLL_CONTROL_ATTR_LONG_POLL_INTERVAL_MIN
             || ((sPayload.u32NewLongPollInterval < ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollIntervalMin) ) 
#else
             || (sPayload.u32NewLongPollInterval < CLD_POLL_CONTROL_LONG_POLL_INTERVAL_MIN) 
#endif
            ){
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
                return E_ZCL_ERR_INVALID_VALUE;
    }
    
    ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval = sPayload.u32NewLongPollInterval;
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psSetLongPollIntervalPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}

#endif

#ifdef CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlHandleSetShortPollInterval
 **
 ** DESCRIPTION:
 ** Handles set Short poll interval
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

PRIVATE  teZCL_Status eCLD_PollControlHandleSetShortPollInterval(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PollControl_SetShortPollIntervalPayload sPayload;
    uint8   u8TransactionSequenceNumber;
    tsCLD_PollControlCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PollControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PollControlSetShortPollIntervalReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber,
                                            &sPayload);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_POLL_CONTROL, "\nErr:eCLD_PollControlSetLongPollIntervalReceive:%d", eStatus);
        return eStatus;
    }

    
    if((sPayload.u16NewShortPollInterval == 0) || 
        (sPayload.u16NewShortPollInterval > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinInterval) ||
            (sPayload.u16NewShortPollInterval > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval)
            #if CLD_POLL_CONTROL_SHORT_POLL_INTERVAL_MAX != 0xFFFF
                ||(sPayload.u16NewShortPollInterval > (uint32)CLD_POLL_CONTROL_SHORT_POLL_INTERVAL_MAX) 
            #endif
            || (sPayload.u16NewShortPollInterval < CLD_POLL_CONTROL_SHORT_POLL_INTERVAL_MIN)
    ){
                eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_VALUE);
                return E_ZCL_ERR_INVALID_VALUE;
    }
    
    ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ShortPollInterval = sPayload.u16NewShortPollInterval;
    
    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psSetShortPollIntervalPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    return E_ZCL_SUCCESS;
}

#endif

#endif

#ifdef POLL_CONTROL_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlHandleCheckin
 **
 ** DESCRIPTION:
 ** Handles Check in message
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
PRIVATE  teZCL_Status eCLD_PollControlHandleCheckin(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    tsCLD_PollControl_CheckinResponsePayload sResponse = {0};
    uint8   u8TransactionSequenceNumber;
    tsCLD_PollControlCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_PollControlCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eCLD_PollControlCheckinReceive(
                                            pZPSevent,
                                            &u8TransactionSequenceNumber);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_POLL_CONTROL, "\nErr:eCLD_PollControlCheckinReceive:%d", eStatus);
        return eStatus;
    }

    psCommon->sCallBackMessage.uMessage.psCheckinResponsePayload = &sResponse;
    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    
    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_PollControlCheckinResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                  pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                  &sZCL_Address,
                                                  &u8TransactionSequenceNumber,
                                                  &sResponse);
    
    return eStatus;
}

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
