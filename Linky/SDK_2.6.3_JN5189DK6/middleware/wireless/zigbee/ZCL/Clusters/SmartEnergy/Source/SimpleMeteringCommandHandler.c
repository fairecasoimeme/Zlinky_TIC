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
 * MODULE:             Simple Metering Cluster
 *
 * COMPONENT:          SimpleMetering.c
 *
 * DESCRIPTION:        Simple Metering Cluster Definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "SimpleMetering.h"

#include "SimpleMetering_internal.h"


#ifdef CLD_SIMPLE_METERING
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

#ifdef CLD_SM_SUPPORT_GET_PROFILE

#ifdef SM_SERVER

PRIVATE teZCL_Status eSMReceivedGetProfile(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eSM_ServerSendGetProfileResponse(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsZCL_TxPayloadItem *asPayloadDefinition,
                    uint8 u8NumberOfItems);

PRIVATE teZCL_Status eSM_ServerGetProfileResponseCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    teSM_IntervalChannel sSMIntervalChannel,
                    uint32 u32EndTime,
                    uint8 u8NumberOfPeriods,
                    uint8 u8ProfileIntervalPeriod);

#endif /*SM_SERVER*/

#ifdef SM_CLIENT
PRIVATE teZCL_Status eSMReceivedGetProfileResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);
#endif /*SM_CLIENT*/

#endif /*CLD_SM_SUPPORT_GET_PROFILE*/



#ifdef SM_SERVER
PRIVATE teZCL_Status eSMReceivedRequestMirrorResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE teZCL_Status eSMReceivedMirrorRemoved(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#endif /*SM_SERVER*/
#ifdef CLD_SM_SUPPORT_MIRROR
#ifdef SM_CLIENT

PRIVATE teZCL_Status eSMReceivedRequestMirror(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                        u8TransactionSequenceNumber);

PRIVATE teZCL_Status eSMReceivedRemoveMirror(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                        u8TransactionSequenceNumber);

PRIVATE bool_t bSM_IsMirroredEndPoint(uint8 u8EndPoint);

#endif /*SM_CLIENT*/

#endif /*CLD_SM_SUPPORT_MIRROR*/

#ifdef CLD_SM_SUPPORT_FAST_POLL_MODE

#ifdef SM_CLIENT
PRIVATE teZCL_Status eSMReceivedFastPollModeResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#endif /*SM_CLIENT*/

#ifdef SM_SERVER

PRIVATE teZCL_Status eSMReceivedFastPollMode(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PRIVATE    teZCL_Status eSM_ServerFastPollResonseCommand(
                    uint8                                 u8SourceEndpoint,
                    uint8                                 u8DestinationEndpoint,
                    tsZCL_Address                         *psDestinationAddress,
                    tsSM_RequestFastPollResponseCommand *psFastPollResponseCommand);

PRIVATE void vSM_TimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent);

PRIVATE  teZCL_Status eSM_TimeUpdate(uint8 u8SourceEndPointId, bool_t bIsServer, uint32 u32UTCtime);

PRIVATE void vSM_UpdateFastPollModeStatus(bool_t bStatus);

PRIVATE  bool_t bSM_GetFastPollModeStatus(void);

PRIVATE  void vSM_CheckFastPollEndTime(tsZCL_EndPointDefinition *psEndPointDefinition,
                                                        tsZCL_ClusterInstance       *psClusterInstance);

#endif /*SM_SERVER*/


#endif  /*CLD_SM_SUPPORT_FAST_POLL_MODE*/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

#ifdef CLD_SM_SUPPORT_FAST_POLL_MODE

#ifdef SM_SERVER
PRIVATE tsSE_FastPollModeStatus    sSE_FastPollModeStatus;
#endif /*SM_SERVER*/

#endif  /*CLD_SM_SUPPORT_FAST_POLL_MODE*/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eSimpleMeteringCommandHandler
 **
 ** DESCRIPTION: Command Handler for simple metering command
 **
 ** PARAMETERS:
  ***pZPSevent                  : ZPS Event
 ***psEndPointDefinition     : End point definition details.
 ***psClusterInstance         : Simple Metering Cluster Instance
 ***
 ** RETURN:
 **teZCL_Status
 **
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSimpleMeteringCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
	tsZCL_HeaderParams sZCL_HeaderParams;
    teZCL_Status eStatus;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr)->sReceiveEventAddress);

#ifdef CLD_SM_SUPPORT_MIRROR
#ifdef SM_CLIENT
    //Check for the attribute associated with the Identified cluster, for Mirrored attribute
    if (bSM_IsMirroredEndPoint(psEndPointDefinition->u8EndPointNumber))
    {
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {
            case E_CLD_SM_REMOVE_MIRROR:
                    eStatus = eSMReceivedRemoveMirror(pZPSevent, psEndPointDefinition, psClusterInstance, u8TransactionSequenceNumber);
                break;

            default:
                eStatus = E_ZCL_FAIL;
                break;
        }
    }
    else
#endif /*SM_CLIENT*/
#endif /*CLD_SM_SUPPORT_MIRROR*/
    {
        //Main End point
        switch (psClusterInstance->bIsServer)
        {
#ifdef SM_SERVER
            //server
            case(TRUE):
            {
                switch(sZCL_HeaderParams.u8CommandIdentifier)
                {
#ifdef CLD_SM_SUPPORT_GET_PROFILE
                    case E_CLD_SM_GET_PROFILE:
                        {
                            eStatus = eSMReceivedGetProfile(pZPSevent, psEndPointDefinition, psClusterInstance);
                        }
                        break;
#endif

#ifdef CLD_SM_SUPPORT_FAST_POLL_MODE
                    case E_CLD_SM_REQUEST_FAST_POLL_MODE:
                        eStatus = eSMReceivedFastPollMode(pZPSevent, psEndPointDefinition, psClusterInstance);
                        break;
#endif

//#ifdef CLD_SM_SUPPORT_MIRROR
                    case E_CLD_SM_REQUEST_MIRROR_RESPONSE:
                        {
                            eStatus = eSMReceivedRequestMirrorResponse(pZPSevent, psEndPointDefinition, psClusterInstance);
                        }
                        break;

                    case E_CLD_SM_MIRROR_REMOVED:
                        eStatus = eSMReceivedMirrorRemoved(pZPSevent, psEndPointDefinition, psClusterInstance);
                        break;
//#endif

                    default:
                        eStatus = eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_UNSUP_CLUSTER_COMMAND);
                        break;
                }
            }
            break;
#endif /*SM_SERVER*/

#ifdef SM_CLIENT
            //CLIENT
            case(FALSE):
            {
                switch(sZCL_HeaderParams.u8CommandIdentifier)
                {
#ifdef CLD_SM_SUPPORT_GET_PROFILE
                    case E_CLD_SM_GET_PROFILE_RESPONSE:
                        eStatus = eSMReceivedGetProfileResponse(pZPSevent, psEndPointDefinition, psClusterInstance);
                    break;
#endif

#ifdef CLD_SM_SUPPORT_FAST_POLL_MODE
                    case E_CLD_SM_REQUEST_FAST_POLL_MODE_RESPONSE:
                        eStatus = eSMReceivedFastPollModeResponse(pZPSevent, psEndPointDefinition, psClusterInstance);
                        break;
#endif

#ifdef CLD_SM_SUPPORT_MIRROR
                    case E_CLD_SM_REQUEST_MIRROR:
                        eStatus = eSMReceivedRequestMirror(pZPSevent, psEndPointDefinition, psClusterInstance, u8TransactionSequenceNumber);
                        break;
#endif

                    default:
                        eStatus = eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_UNSUP_CLUSTER_COMMAND);
                        break;
                }
            }
            break;
#endif /*SM_CLIENT*/

            default:
                eStatus = E_ZCL_FAIL;
                break;
        }
    }

    return eStatus;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#ifdef CLD_SM_SUPPORT_GET_PROFILE

#ifdef SM_SERVER

/****************************************************************************
 **
 ** NAME:       eSM_ServerSendGetProfileResponse
 **
 ** DESCRIPTION:
 **             Sends Get profile response based on the received
 **             request from Client end
 **
 ** PARAMETERS:
 **
 ** u8SourceEndpoint        : Source End Point
 ** u8DestinationEndpoint,    : Destination End point
 ** *psDestinationAddress,    : Client address, Profile requester
 ** *asPayloadDefinition,    : Payload that needs to be sent
 ** u8NumberOfItems            : number of Items within the Payload
 **
 ** RETURN:
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eSM_ServerSendGetProfileResponse(
                                    uint8 u8SourceEndpoint,
                                    uint8 u8DestinationEndpoint,
                                    tsZCL_Address *psDestinationAddress,
                                    tsZCL_TxPayloadItem *asPayloadDefinition,
                                    uint8 u8NumberOfItems)
{
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;
    teZCL_Status eZCL_Status = E_ZCL_FAIL;
    uint8 u8SequenceNumber ;

    if((eZCL_Status =
        eZCL_FindCluster(SE_CLUSTER_ID_SIMPLE_METERING,
                        u8SourceEndpoint,
                        TRUE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        (void *)&psCustomData))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer == FALSE)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            psEndPointDefinition->bDisableDefaultResponse = TRUE;
            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                 u8DestinationEndpoint,
                                                 psDestinationAddress,
                                                 SE_CLUSTER_ID_SIMPLE_METERING,
                                                 TRUE,
                                                 E_CLD_SM_GET_PROFILE_RESPONSE,
                                                 &u8SequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 u8NumberOfItems);
        }
    }
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eSM_ServerGetProfileResponseCommand
 **
 ** DESCRIPTION: Get profile response is sent from server end.
 **                The required time is mapped with the circular buffer and then
 **                required Consumption (Received / Delivered) is sent
 ** PARAMETERS:
 **  u8SourceEndpoint : Source End Point
 **  u8DestinationEndpoint : Destination End Point
 **  *psDestinationAddress : Destination Address
 **  sSMIntervalChannel : Required Interval channel consumption (Received/Delivered)
 **  u32EndTime : Received End time request
 **  u8NumberOfPeriods : Required number of Intervals
 **  u8ProfileIntervalPeriod : Profile Interval Period at the server end.
 **
 ** RETURN:
 **teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ServerGetProfileResponseCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    teSM_IntervalChannel sSMIntervalChannel,
                    uint32 u32EndTime,
                    uint8 u8NumberOfPeriods,
                    uint8 u8ProfileIntervalPeriod)
{
    teZCL_Status eZCL_Status;
    tsSM_GetProfileResponseCommand sGetProfileResponseCommand;

    bool_t bFoundEndtime = FALSE;
    uint8 u8LoopCntr;
    uint8 u8PreviousIndex = u8GetProfileCntr;
    zuint24    u24TempValues[CLD_SM_GETPROFILE_MAX_NO_INTERVALS];

    if (u8GetProfileCntr == 0)
    {
        u8PreviousIndex = CLD_SM_GETPROFILE_MAX_NO_INTERVALS - 1;
    }
    else
    {
        u8PreviousIndex = u8GetProfileCntr - 1;
    }

    if((sSMIntervalChannel != E_CLD_SM_CONSUMPTION_RECEIVED) && (sSMIntervalChannel != E_CLD_SM_CONSUMPTION_DELIVERED))
    {
        sGetProfileResponseCommand.u32Endtime = u32EndTime;
        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_UNDEFINED_INTERVAL_CHANNEL_REQUESTED;
        sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = 0;
        sGetProfileResponseCommand.pau24Intervals = NULL;
    }
    else if(!(b8SupportedChannels & (1 << sSMIntervalChannel))) /* check we are supporting requested channel in the request */
    {
        sGetProfileResponseCommand.u32Endtime = u32EndTime;
        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_INTERVAL_CHANNEL_NOT_SUPPORTED;
        sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = 0;
        sGetProfileResponseCommand.pau24Intervals = NULL;
    }
    else if (u32EndTime == 0)
    {
        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_SUCCESS;
        /*sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = 1;*/

        /*switch(sSMIntervalChannel)
        {
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
        case E_CLD_SM_CONSUMPTION_DELIVERED:
            sGetProfileResponseCommand.pau24Intervals = &sSEGetProfile[u8PreviousIndex].u24ConsumptionDelivered;
            break;
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
        case E_CLD_SM_CONSUMPTION_RECEIVED:
            sGetProfileResponseCommand.pau24Intervals = &sSEGetProfile[u8PreviousIndex].u24ConsumptionReceived;
            break;
#endif
        default:
            break;
        }*/

        /*sGetProfileResponseCommand.u32Endtime = sSEGetProfile[u8PreviousIndex].u32UtcTime;*/

        bFoundEndtime = TRUE;
    }
    else if (u32EndTime == 0xFFFFFFFF)
    {
        sGetProfileResponseCommand.u32Endtime = u32EndTime;
        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_INVALID_END_TIME;
        sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = 0;
        sGetProfileResponseCommand.pau24Intervals = NULL;
    }
    else if  (sSEGetProfile[u8PreviousIndex].u32UtcTime < u32EndTime)
    {
        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_SUCCESS;
        bFoundEndtime = TRUE;
    }
    else
    {
        for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_GETPROFILE_MAX_NO_INTERVALS; u8LoopCntr++)
        {
            if (u32EndTime < sSEGetProfile[u8PreviousIndex].u32UtcTime)
            {
                if (u8PreviousIndex == 0)
                {
                    u8PreviousIndex = CLD_SM_GETPROFILE_MAX_NO_INTERVALS - 1;
                }
                else
                {
                    u8PreviousIndex--;
                }
            }
            else
            {
                bFoundEndtime = TRUE;
                break;
            }
        }

        if (!bFoundEndtime)
        {
            sGetProfileResponseCommand.u32Endtime = u32EndTime;
            //No intervals available for the requested time
            sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_NO_INTERVALS_AVAILABLE;
            sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = 0;
            sGetProfileResponseCommand.pau24Intervals = NULL;
        }
    }
    if (bFoundEndtime)
    {
        sGetProfileResponseCommand.u32Endtime = sSEGetProfile[u8PreviousIndex].u32UtcTime;
        for(u8LoopCntr=0; u8LoopCntr < CLD_SM_GETPROFILE_MAX_NO_INTERVALS ;u8LoopCntr++)
        {
            switch(sSMIntervalChannel)
            {
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
            case E_CLD_SM_CONSUMPTION_DELIVERED:
                u24TempValues[u8LoopCntr] = sSEGetProfile[u8PreviousIndex].u24ConsumptionDelivered;
                break;
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
            case E_CLD_SM_CONSUMPTION_RECEIVED:
                u24TempValues[u8LoopCntr] = sSEGetProfile[u8PreviousIndex].u24ConsumptionReceived;
                break;
#endif
            default:
                break;
            }

            if ((u8LoopCntr+1) == u8NumberOfPeriods)
            {
                break;
            }

            else
            {
                if (u8PreviousIndex == 0)
                {
                    u8PreviousIndex = CLD_SM_GETPROFILE_MAX_NO_INTERVALS - 1;
                }
                else
                {
                    u8PreviousIndex--;
                }
            }
            if ( (sSEGetProfile[u8PreviousIndex].u32UtcTime > u32EndTime) && (u32EndTime) )
            {
                break;
            }
        }

        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_SUCCESS;

        /* If end time is zero and available periods less than requested periods we are returning all available periods */
        if(u32EndTime == 0 && CLD_SM_GETPROFILE_MAX_NO_INTERVALS <= u8NumberOfPeriods)
        {
            sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = CLD_SM_GETPROFILE_MAX_NO_INTERVALS;
        }
        else
        {
            sGetProfileResponseCommand.u8NumberOfPeriodsDelivered = u8LoopCntr + 1;
        }
        sGetProfileResponseCommand.pau24Intervals = u24TempValues;
    }

    if ((sGetProfileResponseCommand.u8NumberOfPeriodsDelivered)
            && (sGetProfileResponseCommand.u8NumberOfPeriodsDelivered < u8NumberOfPeriods))
    {
        //More periods are requested then can be returned
        sGetProfileResponseCommand.eStatus = E_CLD_SM_GP_MORE_REQUESTED;
    }

    //TODO: Check for "Interval Channel not supported" is remaining

    sGetProfileResponseCommand.u8ProfileIntervalPeriod = u8ProfileIntervalPeriod;

    if (sGetProfileResponseCommand.u8NumberOfPeriodsDelivered)
    {
        tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT32,   (void*)&sGetProfileResponseCommand.u32Endtime},
                                                      {1, E_ZCL_UINT8,   (void*)&sGetProfileResponseCommand.eStatus},
                                                      {1, E_ZCL_UINT8,   (void*)&sGetProfileResponseCommand.u8ProfileIntervalPeriod},
                                                      {1, E_ZCL_UINT8,   (void*)&sGetProfileResponseCommand.u8NumberOfPeriodsDelivered},
                                                      {sGetProfileResponseCommand.u8NumberOfPeriodsDelivered, E_ZCL_UINT24,   (void*)sGetProfileResponseCommand.pau24Intervals}
                                                    };

        eZCL_Status = eSM_ServerSendGetProfileResponse(u8SourceEndpoint, u8DestinationEndpoint, psDestinationAddress, asPayloadDefinition, (sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)));
    }
    else
    {
        tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT32,   (void*)&sGetProfileResponseCommand.u32Endtime},
                                                      {1, E_ZCL_UINT8,   (void*)&sGetProfileResponseCommand.eStatus},
                                                      {1, E_ZCL_UINT8,   (void*)&sGetProfileResponseCommand.u8ProfileIntervalPeriod},
                                                      {1, E_ZCL_UINT8,   (void*)&sGetProfileResponseCommand.u8NumberOfPeriodsDelivered}
                                                    };

        eZCL_Status = eSM_ServerSendGetProfileResponse(u8SourceEndpoint, u8DestinationEndpoint, psDestinationAddress, asPayloadDefinition, (sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)));
    }

    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eSMReceivedGetProfile
 **
 ** DESCRIPTION: Simple Metering Server receives Get profile command
 **
 ** PARAMETERS:
 ***pZPSevent                  : ZPS Event
 ***psEndPointDefinition     : End point definition details.
 ***psClusterInstance         : Simple Metering Cluster Instance
 ***
 ** RETURN:
 **teZCL_Status
 **
 **
 ****************************************************************************/
PRIVATE teZCL_Status eSMReceivedGetProfile(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    tsZCL_Address    sZclAdd;
    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    teZCL_Status eStatus = E_ZCL_SUCCESS;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.eIntervalChannel},
        {1, &u16ActualQuantity, E_ZCL_UINT32,  &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.u32EndTime},
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.u8NumberOfPeriods}
    };

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                      &u8TransactionSequenceNumber,
                      asPayloadDefinition,
                      sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                      E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_SERVER_RECEIVED_COMMAND;
    if(E_ZCL_SUCCESS == eStatus)
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_GET_PROFILE;
    }
    else
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_SERVER_ERROR;
    }

    pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.u8SourceEndPoint = pSM_Common->sReceiveEventAddress.u8SrcEndpoint;
    pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.sSourceAddress.eAddressMode = pSM_Common->sReceiveEventAddress.u8SrcAddrMode;

    if (pSM_Common->sReceiveEventAddress.u8SrcAddrMode == E_ZCL_AM_SHORT)
    {
        pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.sSourceAddress.uAddress.u16DestinationAddress = pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
    }
    else
    {
        pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.sSourceAddress.uAddress.u64DestinationAddress = pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
    }

    pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.u8DestinationEndPoint = pSM_Common->sReceiveEventAddress.u8DstEndpoint;

    eZCL_SetCustomCallBackEvent(&pSM_Common->sSMCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, pSM_Common->sReceiveEventAddress.u8DstEndpoint);

    psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

    if(E_ZCL_SUCCESS == eStatus)
    {

        sZclAdd.eAddressMode = pSM_Common->sReceiveEventAddress.u8SrcAddrMode;

        if (sZclAdd.eAddressMode == E_ZCL_AM_SHORT)
        {
            sZclAdd.uAddress.u16DestinationAddress = pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
        }
        else if (sZclAdd.eAddressMode == E_ZCL_AM_IEEE)
        {
            sZclAdd.uAddress.u64DestinationAddress = (uint64)pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
        }

        #ifndef COOPERATIVE
            eZCL_GetMutex(psEndPointDefinition);
        #endif

        eStatus = eSM_ServerGetProfileResponseCommand(pSM_Common->sReceiveEventAddress.u8DstEndpoint,
                pSM_Common->sReceiveEventAddress.u8SrcEndpoint,
                &sZclAdd,
                pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.eIntervalChannel,
                pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.u32EndTime,
                pSM_Common->sSMCallBackMessage.uMessage.sGetProfileCommand.u8NumberOfPeriods,
                ((tsCLD_SimpleMetering *)psClusterInstance->pvEndPointSharedStructPtr)->eProfileIntervalPeriod);
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

    }

    return eStatus;
}

#endif /*SM_SERVER*/

#ifdef SM_CLIENT
/****************************************************************************
 **
 ** NAME:       eSMReceivedGetProfileResponse
 **
 ** DESCRIPTION: Simple Metering Client receives Get profile response.
 **
 ** PARAMETERS:
 ***pZPSevent                  : ZPS Event
 ***psEndPointDefinition     : End point definition details.
 ***psClusterInstance         : Simple Metering Cluster Instance
 ***
 ** RETURN:
 **teZCL_Status
 **
 **
 ****************************************************************************/
PRIVATE teZCL_Status eSMReceivedGetProfileResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    teZCL_Status eStatus = E_ZCL_SUCCESS;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT32,  &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.u32Endtime},
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.eStatus},
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.u8ProfileIntervalPeriod},
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.u8NumberOfPeriodsDelivered}
    };

     eStatus = eZCL_CustomCommandReceive(pZPSevent,
                      &u8TransactionSequenceNumber,
                      asPayloadDefinition,
                      sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                      E_ZCL_ACCEPT_EXACT);

     if(pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.u8NumberOfPeriodsDelivered >  0)
     {
         pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.pau24Intervals = (zuint24 *)((uint8 *)(PDUM_pvAPduInstanceGetPayload(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst)) + 10);
     }
     else
     {
         //pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.pau24Intervals = NULL;
         pSM_Common->sSMCallBackMessage.uMessage.sGetProfileResponseCommand.pau24Intervals = NULL;
     }

     pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_CLIENT_RECEIVED_COMMAND;
     pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_GET_PROFILE_RESPONSE;

     eZCL_SetCustomCallBackEvent(&pSM_Common->sSMCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, pSM_Common->sReceiveEventAddress.u8DstEndpoint);

     psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

     return E_ZCL_SUCCESS;
}

#endif /*SM_CLIENT*/

#endif /*CLD_SM_SUPPORT_GET_PROFILE*/


#ifdef CLD_SM_SUPPORT_MIRROR

#ifdef SM_CLIENT
/****************************************************************************
 **
 ** NAME:       eSMReceivedRequestMirror
 **
 ** DESCRIPTION: Simple Metering client receives Request mirror command
 **
 ** PARAMETERS:
 ** *pZPSevent                  : ZPS Event
 ** *psEndPointDefinition     : End point definition details.
 ** *psClusterInstance         : Simple Metering Cluster Instance
 **
 ** RETURN:
 ** teZCL_Status
 **
 **
 ****************************************************************************/
PRIVATE teZCL_Status eSMReceivedRequestMirror(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                        u8TransactionSequenceNumber)
{
    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    uint64 u64RemoteIeeeAddress;
    uint8 u8LoopCntr;
    teZCL_Status eStatus = E_ZCL_FAIL;
    uint16 u16FoundEP = 0x0000;
    bool_t bFound = FALSE;
    tsZCL_Address sSourceAdd;

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_CLIENT_RECEIVED_COMMAND;
    pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_REQUEST_MIRROR;

    eZCL_SetCustomCallBackEvent(&pSM_Common->sSMCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, pSM_Common->sReceiveEventAddress.u8DstEndpoint);

    sSourceAdd.eAddressMode = pSM_Common->sReceiveEventAddress.u8SrcAddrMode;

    if (pSM_Common->sReceiveEventAddress.u8SrcAddrMode == E_ZCL_AM_SHORT)
    {

#ifdef PC_PLATFORM_BUILD
        u64RemoteIeeeAddress = IEEE_SRC_ADD;
#else
        // See if the IEEE address matches with the sender's IEEE address
        u64RemoteIeeeAddress = ZPS_u64NwkNibFindExtAddr((void *)ZPS_pvNwkGetHandle(), pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr);

        /* check address */
        if(!u64RemoteIeeeAddress)
        {
            return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
        }
#endif
        pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr = u64RemoteIeeeAddress;
        sSourceAdd.uAddress.u16DestinationAddress = pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
    }
    else
    {
        u64RemoteIeeeAddress = (uint64)pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
        sSourceAdd.uAddress.u64DestinationAddress = (uint64)pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
    }

    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if (psSE_Mirror[u8LoopCntr].u64SourceAddress == u64RemoteIeeeAddress)
        {
#ifndef PC_PLATFORM_BUILD
            /*Enable the End Point*/
            if (ZPS_eAplAfSetEndpointState(psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber, TRUE) == E_ZCL_SUCCESS)
#endif
            {
                /*Respond Mirror request with the same end point */
                eStatus = eSM_ClientRequestMirrorResponseCommand(pSM_Common->sReceiveEventAddress.u8DstEndpoint,
                                                                pSM_Common->sReceiveEventAddress.u8SrcEndpoint,
                                                                &sSourceAdd,
                                                                psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber);

            }
            bFound = TRUE;
            break;
        }
    }

    /*Allocate End point for mirroring*/
    if (bFound == FALSE)
    {
        eSM_GetFreeMirrorEndPoint(&u16FoundEP);
        if (u16FoundEP != 0xFFFF)
        {
#ifndef PC_PLATFORM_BUILD
            /*Enable if valid end point is found*/
            ZPS_eAplAfSetEndpointState((uint8)u16FoundEP, TRUE);
#endif
        }
    }


    if (bFound == FALSE)
    {
        if (u16FoundEP != 0xFFFF)
        {
            for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
            {
                if (psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber == (uint8)u16FoundEP)
                {
                    psSE_Mirror[u8LoopCntr].u64SourceAddress = (uint64)u64RemoteIeeeAddress;
                    break;
                }
            }
        }
        psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);
        eStatus = eSM_ClientRequestMirrorResponseCommand(pSM_Common->sReceiveEventAddress.u8DstEndpoint,
                                                        pSM_Common->sReceiveEventAddress.u8SrcEndpoint,
                                                        &sSourceAdd,
                                                        u16FoundEP);
    }

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eSMCreateMirror
 **
 ** DESCRIPTION: Creates Mirror from Application upon over power failures
 **
 ** PARAMETERS:
 ** u64RemoteIeeeAddress    Remote Meter Device IEEE Address
 **
 ** RETURN:
 ** teSM_Status
 **
 **
 ****************************************************************************/
PUBLIC teSM_Status eSM_CreateMirror(uint8 u8MirrorEndPoint, uint64 u64RemoteIeeeAddress)
{
    uint8 u8LoopCntr;
    teZCL_Status eStatus = E_ZCL_FAIL;
    bool_t bEnabled = FALSE;

    /* check IEEE address */
    if( (u64RemoteIeeeAddress == 0) || (u64RemoteIeeeAddress == 0xFFFFFFFFFFFFFFFFULL) ||
            (u8MirrorEndPoint >= (psSE_Mirror[0].sEndPoint.u8EndPointNumber+CLD_SM_NUMBER_OF_MIRRORS)))
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    /* Search existing mirror endpoints, if found return success otherwise create mirror */
    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if (psSE_Mirror[u8LoopCntr].u64SourceAddress == u64RemoteIeeeAddress)
        {
#ifndef PC_PLATFORM_BUILD
            /*Enable the End Point*/
            if (ZPS_eAplAfSetEndpointState(u8MirrorEndPoint, TRUE) == E_ZCL_SUCCESS)
#endif
            {
                return E_CLD_SM_STATUS_SUCCESS;
            }
        }
    }

#ifndef PC_PLATFORM_BUILD
    /* Get End Point State */
    eStatus = ZPS_eAplAfGetEndpointState(u8MirrorEndPoint,&bEnabled);
#endif

    /* check status */
    if(eStatus == E_ZCL_SUCCESS && !bEnabled)
    {
#ifndef PC_PLATFORM_BUILD
        ZPS_eAplAfSetEndpointState(u8MirrorEndPoint, TRUE);
#endif
    }

    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if (psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber == u8MirrorEndPoint)
        {
            psSE_Mirror[u8LoopCntr].u64SourceAddress = (uint64)u64RemoteIeeeAddress;
            return E_CLD_SM_STATUS_SUCCESS;
        }
    }

    return E_CLD_SM_STATUS_EP_NOT_AVAILABLE;
}
/****************************************************************************
 **
 ** NAME:       eSMReceivedRemoveMirror
 **
 ** DESCRIPTION: Simple Metering Client receives remove mirror command
 **
 ** PARAMETERS:
 ** *pZPSevent                  : ZPS Event
 ** *psEndPointDefinition     : End point definition details.
 ** *psClusterInstance         : Simple Metering Cluster Instance
 **
 ** RETURN:
 ** teZCL_Status
 **
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eSMReceivedRemoveMirror(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint8                        u8TransactionSequenceNumber)
{
    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    uint64 u64RemoteIeeeAddress;
    uint8 u8LoopCntr;
    teZCL_Status eStatus = E_ZCL_FAIL;
    bool_t bFound = FALSE;
    tsZCL_Address sSourceAdd;

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_CLIENT_RECEIVED_COMMAND;
    pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_REMOVE_MIRROR;

    eZCL_SetCustomCallBackEvent(&pSM_Common->sSMCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, pSM_Common->sReceiveEventAddress.u8DstEndpoint);

    sSourceAdd.eAddressMode = pSM_Common->sReceiveEventAddress.u8SrcAddrMode;

    if (pSM_Common->sReceiveEventAddress.u8SrcAddrMode == E_ZCL_AM_SHORT)
    {

#ifdef PC_PLATFORM_BUILD
        u64RemoteIeeeAddress = IEEE_SRC_ADD;
#else
        // See if the IEEE address matches with the sender's IEEE address
        u64RemoteIeeeAddress = ZPS_u64NwkNibFindExtAddr((void *)ZPS_pvNwkGetHandle(), pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr);

        /* check address */
        if(!u64RemoteIeeeAddress)
        {
            return eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
        }
#endif
        sSourceAdd.uAddress.u16DestinationAddress = pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
    }
    else
    {
        u64RemoteIeeeAddress = (uint64)pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
        sSourceAdd.uAddress.u64DestinationAddress = (uint64)pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
    }

    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if (psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber == pSM_Common->sReceiveEventAddress.u8DstEndpoint)
        {
#ifndef PC_PLATFORM_BUILD
            if (psSE_Mirror[u8LoopCntr].u64SourceAddress == u64RemoteIeeeAddress)
#else
            if (psSE_Mirror[u8LoopCntr].u64SourceAddress == 0)
#endif
            {
                psSE_Mirror[u8LoopCntr].u64SourceAddress = 0;

                psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

                eStatus = eSM_ClientMirrorRemovedCommand(psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber,
                        pSM_Common->sReceiveEventAddress.u8SrcEndpoint, &sSourceAdd);

                if (E_ZCL_SUCCESS == eStatus)
                {
#ifndef PC_PLATFORM_BUILD
                    if (ZPS_eAplAfSetEndpointState(psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber, FALSE) == E_ZCL_SUCCESS)
                    {
                        eStatus = E_ZCL_SUCCESS;
                    }
                    else
                    {
                        eStatus = E_ZCL_FAIL;
                    }
#endif
                }
            }
            else
            {
                //Send Default ZCL response; UN_AUTHORIZED
                eStatus = eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_NOT_AUTHORIZED);
            }
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        eStatus = eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_INVALID_FIELD);
    }

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eSMRemoveMirror
 **
 ** DESCRIPTION: Removing Mirror Endpoint by appliction
 **
 ** PARAMETERS:
 ** u8MirrorEndpoint : Mirror End point number
 ** u64RemoteIeeeAddress : Mirror Device Address
 **
 ** RETURN:
 ** teSM_Status
 **
 ****************************************************************************/
PUBLIC  teSM_Status eSM_RemoveMirror(
                    uint8  u8MirrorEndpoint,
                    uint64 u64RemoteIeeeAddress)
{
    uint8 u8LoopCntr;
    bool_t bFound = FALSE;
    teSM_Status eSM_Status = E_ZCL_SUCCESS;

    /* check IEEE address */
    if(u64RemoteIeeeAddress == 0 || u64RemoteIeeeAddress == 0xFFFFFFFFFFFFFFFFULL)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    /* check to whether mirror end point is created for this remote device or not */
    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if (psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber == u8MirrorEndpoint)
        {
#ifndef PC_PLATFORM_BUILD
            if (psSE_Mirror[u8LoopCntr].u64SourceAddress == u64RemoteIeeeAddress)
#else
            if (psSE_Mirror[u8LoopCntr].u64SourceAddress == 0)
#endif
            {
                psSE_Mirror[u8LoopCntr].u64SourceAddress = 0;

#ifndef PC_PLATFORM_BUILD
                if (ZPS_eAplAfSetEndpointState(psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber, FALSE) == E_ZCL_SUCCESS)
                {
                    eSM_Status = E_ZCL_SUCCESS;
                }
                else
                {
                    eSM_Status = E_ZCL_FAIL;
                }
#endif
                bFound = TRUE;
            }
            else
            {
                return E_ZCL_FAIL;
            }
        }
    }

    if (!bFound)
    {
        return E_CLD_SM_STATUS_MIRROR_EP_NOT_FOUND;
    }

    return eSM_Status;
}

/****************************************************************************
 **
 ** NAME:       bSM_IsMirroredEndPoint
 **
 ** DESCRIPTION: verifies whether the given end point is mirrored one or not?
 **
 ** PARAMETERS:
 ** u8EndPoint : End point number, whose information is required.
 **
 ** RETURN:
 ** bool_t : TRUE/FALSE
 **
 ****************************************************************************/
PRIVATE  bool_t bSM_IsMirroredEndPoint(uint8 u8EndPoint)
{
    uint8 u8LoopCntr;

    if (psSE_Mirror == NULL)
    {
        return FALSE;
    }

    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if(psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber == u8EndPoint)
        {
            return TRUE;
        }
    }
    return FALSE;
}

#endif /*SM_CLIENT*/
#endif
#ifdef SM_SERVER

/****************************************************************************
 **
 ** NAME:       eSMReceivedRequestMirrorResponse
 **
 ** DESCRIPTION: Simple Metering Server received Request Mirror response.
 **
 ** PARAMETERS:
 ** *pZPSevent              : ZPS Event
 ** *psEndPointDefinition     : End point definition details.
 ** *psClusterInstance         : Simple Metering Cluster Instance
 **
 ** RETURN:
 **teZCL_Status
 ****
 ****************************************************************************/
PRIVATE teZCL_Status eSMReceivedRequestMirrorResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;

    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT16,   &pSM_Common->sSMCallBackMessage.uMessage.sRequestMirrorResponseCommand.u16Endpoint}
    };

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                      &u8TransactionSequenceNumber,
                      asPayloadDefinition,
                      sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                      E_ZCL_ACCEPT_EXACT);

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_SERVER_RECEIVED_COMMAND;

    if(E_ZCL_SUCCESS == eStatus)
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_REQUEST_MIRROR_RESPONSE;
    }
    else
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_SERVER_ERROR;
    }

    eZCL_SetCustomCallBackEvent(&pSM_Common->sSMCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, pSM_Common->sReceiveEventAddress.u8DstEndpoint);

    if (pSM_Common->sReceiveEventAddress.u8SrcAddrMode != E_ZCL_AM_IEEE)
    {
#ifdef PC_PLATFORM_BUILD
        pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr = IEEE_SRC_ADD;
#else
        pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr = ZPS_u64NwkNibFindExtAddr((void *)ZPS_pvNwkGetHandle(), pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr);
#endif
    }

    psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eSMReceivedMirrorRemoved
 **
 ** DESCRIPTION: Simple Metering server receives Mirror removed command, this is in
 ** response to the Remove mirror request
 **
 ** PARAMETERS:
 ***pZPSevent                  : ZPS Event
 ***psEndPointDefinition     : End point definition details.
 ***psClusterInstance         : Simple Metering Cluster Instance
 ***
 ** RETURN:
 **teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eSMReceivedMirrorRemoved(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;

    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, (uint16 *)&u16ActualQuantity, E_ZCL_UINT16,   (void *)&pSM_Common->sSMCallBackMessage.uMessage.sMirrorRemovedResponseCommand.u16Endpoint}
        };


    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                      &u8TransactionSequenceNumber,
                      asPayloadDefinition,
                      sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                      E_ZCL_ACCEPT_EXACT);

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_SERVER_RECEIVED_COMMAND;

    if(E_ZCL_SUCCESS == eStatus)
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_MIRROR_REMOVED;
    }
    else
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_SERVER_ERROR;
    }

    eZCL_SetCustomCallBackEvent(&pSM_Common->sSMCustomCallBackEvent, pZPSevent, u8TransactionSequenceNumber, pSM_Common->sReceiveEventAddress.u8DstEndpoint);

    psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

    return eStatus;
}
#endif /*SM_SERVER*/
#ifdef CLD_SM_SUPPORT_MIRROR
/****************************************************************************
 **
 ** NAME:       eSM_IsMirrorSourceAddressValid
 **
 ** DESCRIPTION: Simple Metering server needs to verify the source Address & EP
 **              for Attribute report & also updates the event status
 **
 ** PARAMETERS:
 ** sZCL_ReportAttributeMirror     : Pointer to Report attribute mirror event.
 **
 ** RETURN:
 ** teZCL_ReportAttributeStatus
 **
 ****************************************************************************/
PUBLIC  teZCL_ReportAttributeStatus eSM_IsMirrorSourceAddressValid (tsZCL_ReportAttributeMirror *sZCL_ReportAttributeMirror)
{
    teZCL_ReportAttributeStatus eStatus = E_ZCL_ATTR_REPORT_ERR;
    uint8 u8LoopCntr;

    if (sZCL_ReportAttributeMirror == NULL)
    {
        return eStatus;
    }

    eStatus = E_ZCL_ATTR_REPORT_EP_MISMATCH;

    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if (psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber == sZCL_ReportAttributeMirror->u8DestinationEndPoint)
        {
            if (psSE_Mirror[u8LoopCntr].u64SourceAddress == (uint64)sZCL_ReportAttributeMirror->u64RemoteIeeeAddress)
            {
                eStatus = E_ZCL_ATTR_REPORT_OK;
            }
            else
            {
                eStatus = E_ZCL_ATTR_REPORT_ADDR_MISMATCH;
            }
            break;
        }
    }

    sZCL_ReportAttributeMirror->eStatus = eStatus;

    return eStatus;
}
#endif  /*CLD_SM_SUPPORT_MIRROR*/

#ifdef    CLD_SM_SUPPORT_FAST_POLL_MODE

#ifdef SM_CLIENT
/****************************************************************************
 **
 ** NAME:       eSMReceivedFastPollModeResponse
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:
 ***pZPSevent                  : ZPS Event
 ***psEndPointDefinition     : End point definition details.
 ***psClusterInstance         : Simple Metering Cluster Instance
 **
 ** RETURN:
 **teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSMReceivedFastPollModeResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollResponseCommand.u8AppliedUpdatePeriod},
        {1, &u16ActualQuantity, E_ZCL_UINT32,   &pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollResponseCommand.u32FastPollEndTime}
    };

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                      &u8TransactionSequenceNumber,
                      asPayloadDefinition,
                      sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                      E_ZCL_ACCEPT_EXACT);

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_CLIENT_RECEIVED_COMMAND;
    pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_REQUEST_FAST_POLL_MODE_RESPONSE;

    psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

    return eStatus;
}

#endif /*SM_CLIENT*/

#ifdef SM_SERVER
/****************************************************************************
 **
 ** NAME:       eSM_ServerFastPollResonseCommand
 **
 ** DESCRIPTION:
 **                    Sends Fast polling mode response.
 ** PARAMETERS:
 ** u8SourceEndpoint            : Source End Point
 ** u8DestinationEndpoint,        : Destination End Point
 ** *psDestinationAddress,        : Destination Address who requested Fast poll mode.
 ** *psFastPollResponseCommand    : Fast poll mode response.
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eSM_ServerFastPollResonseCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsSM_RequestFastPollResponseCommand *psFastPollResponseCommand)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;
    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psFastPollResponseCommand->u8AppliedUpdatePeriod},
                                                  {1, E_ZCL_UINT32,   &psFastPollResponseCommand->u32FastPollEndTime}
                                                };
    uint8 u8SequenceNumber ;

    if((eZCL_Status =
        eZCL_FindCluster(SE_CLUSTER_ID_SIMPLE_METERING,
                        u8SourceEndpoint,
                        TRUE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        (void*)&psCustomData))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer == FALSE)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            psEndPointDefinition->bDisableDefaultResponse = TRUE;
            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                 u8DestinationEndpoint,
                                                 psDestinationAddress,
                                                 SE_CLUSTER_ID_SIMPLE_METERING,
                                                 TRUE,
                                                 E_CLD_SM_REQUEST_FAST_POLL_MODE_RESPONSE,
                                                 &u8SequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
        }
    }
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eSMReceivedFastPollMode
 **
 ** DESCRIPTION:
 **             Extract Fast polling mode request
 **
 ** PARAMETERS:
 ***pZPSevent                  : ZPS Event
 ***psEndPointDefinition     : End point definition details.
 ***psClusterInstance         : Simple Metering Cluster Instance
 **
 ** RETURN:
 **teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eSMReceivedFastPollMode(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    uint32 u32TempCalc = 0;
    uint32 u32PrevFastPollEndTime = 0;
    tsZCL_Address    sZclAdd;
    tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);
    tsCLD_SimpleMetering    *psCLD_SM = ((tsCLD_SimpleMetering*)psClusterInstance->pvEndPointSharedStructPtr);
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsSM_RequestFastPollResponseCommand sSM_RequestFastPollResponseCmd;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8FastPollUpdatePeriod},
        {1, &u16ActualQuantity, E_ZCL_UINT8,   &pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8Duration}
    };

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
                      &u8TransactionSequenceNumber,
                      asPayloadDefinition,
                      sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                      E_ZCL_ACCEPT_EXACT | E_ZCL_DISABLE_DEFAULT_RESPONSE);

    pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_SERVER_RECEIVED_COMMAND;

    if(E_ZCL_SUCCESS == eStatus)
    {
        pSM_Common->sSMCallBackMessage.u8CommandId = E_CLD_SM_REQUEST_FAST_POLL_MODE;

        //If it is not short address mode then, only IEEE is used
        if (pSM_Common->sReceiveEventAddress.u8SrcAddrMode == E_ZCL_AM_SHORT)
        {
            sZclAdd.uAddress.u16DestinationAddress = pSM_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
            sZclAdd.eAddressMode = E_ZCL_AM_SHORT;
        }
        else
        {
            sZclAdd.uAddress.u64DestinationAddress = (uint64)pSM_Common->sReceiveEventAddress.uSrcAddress.u64Addr;
            sZclAdd.eAddressMode = E_ZCL_AM_IEEE;
        }

        #ifndef COOPERATIVE
            eZCL_GetMutex(psEndPointDefinition);
        #endif


        if (!(bSM_GetFastPollModeStatus()))
        {
            sSE_FastPollModeStatus.u32MaxFastPollDuration = u32ZCL_GetUTCTime() + (CLD_SM_FAST_POLLING_PERIOD * 60);
        }

        //FPM slot is available
        u32TempCalc = pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8FastPollUpdatePeriod;

        //If Update Period is less , then set to minimum value on the Meter.
        if (u32TempCalc < psCLD_SM->u8FastPollUpdatePeriod)
        {
            u32TempCalc = psCLD_SM->u8FastPollUpdatePeriod;
        }

        pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8AppliedUpdatePeriod = (uint8)u32TempCalc;

        if (pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8Duration > CLD_SM_FAST_POLLING_PERIOD)
        {
            pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8Duration = CLD_SM_FAST_POLLING_PERIOD;
        }

        u32TempCalc = u32ZCL_GetUTCTime() + (pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8Duration * 60);

        if (u32TempCalc <= sSE_FastPollModeStatus.u32MaxFastPollDuration)
        {
            pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u32FastPollEndTime = u32TempCalc;
            if (u32TempCalc > sSE_FastPollModeStatus.u32FastPollEndTime)
            {
                u32PrevFastPollEndTime = sSE_FastPollModeStatus.u32FastPollEndTime;
                sSE_FastPollModeStatus.u32FastPollEndTime = u32TempCalc;
            }
        }
        else
        {
            pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u32FastPollEndTime = sSE_FastPollModeStatus.u32MaxFastPollDuration;
            u32PrevFastPollEndTime = sSE_FastPollModeStatus.u32FastPollEndTime;
            sSE_FastPollModeStatus.u32FastPollEndTime = sSE_FastPollModeStatus.u32MaxFastPollDuration;
        }

        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

    }
    else
    {
        pSM_Common->sSMCallBackMessage.u8CommandId =  E_CLD_SM_SERVER_ERROR;
    }

     pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.bReadyToPoll = FALSE;

    psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);

    if (E_ZCL_SUCCESS == eStatus)
    {
        if (pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.bReadyToPoll)
        {
            sSM_RequestFastPollResponseCmd.u8AppliedUpdatePeriod = pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u8AppliedUpdatePeriod;
            sSM_RequestFastPollResponseCmd.u32FastPollEndTime = pSM_Common->sSMCallBackMessage.uMessage.sRequestFastPollCommand.u32FastPollEndTime;

            #ifndef COOPERATIVE
                eZCL_GetMutex(psEndPointDefinition);
            #endif

            vSM_UpdateFastPollModeStatus(TRUE);
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

        }
        else
        {
            sSM_RequestFastPollResponseCmd.u8AppliedUpdatePeriod = 0;
            sSM_RequestFastPollResponseCmd.u32FastPollEndTime = 0;
            if (u32PrevFastPollEndTime)
            {
                #ifndef COOPERATIVE
                    eZCL_GetMutex(psEndPointDefinition);
                #endif

                sSE_FastPollModeStatus.u32FastPollEndTime = u32PrevFastPollEndTime;
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

            }
        }

        eStatus = eSM_ServerFastPollResonseCommand(pSM_Common->sReceiveEventAddress.u8DstEndpoint,
                                                pSM_Common->sReceiveEventAddress.u8SrcEndpoint,
                                                &sZclAdd,
                                                &sSM_RequestFastPollResponseCmd);
    }

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eSM_RegisterTimeServer
 **
 ** DESCRIPTION:
 ** Registers Simple Metering Time Server with ZCL, required for fast polling
 **
 ** PARAMETERS:                         Name                          Usage
 ** none
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_RegisterTimeServer(void)
{

    static bool_t bTimerRegister=FALSE;
    // add timer click function to ZCL
#ifndef OTA_UNIT_TEST_FRAMEWORK
    if(!bTimerRegister)
#endif
    {
        if(eZCL_TimerRegister(E_ZCL_TIMER_CLICK, 0, vSM_TimerClickCallback)!= E_ZCL_SUCCESS)
            return(E_ZCL_FAIL);

        bTimerRegister = TRUE;
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       vSM_TimerClickCallback
 **
 ** DESCRIPTION:
 ** Timer Call-back registered with the ZCL
 **
 ** PARAMETERS:                 Name               Usage
 ** tsZCL_CallBackEvent        *psCallBackEvent    Timer Server Callback
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/
PRIVATE  void vSM_TimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent)
{
    int i;
    uint8 u8numberOfendpoints;

    u8numberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();

    // find SM clusters on each EP - if any
    for(i=0; i<u8numberOfendpoints; i++)
    {
        // deliver time to any EP-server/client
        eSM_TimeUpdate(u8ZCL_GetEPIdFromIndex(i), TRUE, psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
    }
}

/****************************************************************************
 **
 ** NAME:       eSM_TimeUpdate
 **
 ** DESCRIPTION:
 ** Updates the SM Fast polling period  based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP Id
 ** bool_t                      bIsServer                   Is server
 ** uint32                      u32UTCtime                  Current time
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eSM_TimeUpdate(uint8 u8SourceEndPointId, bool_t bIsServer, uint32 u32UTCtime)
{
    teZCL_Status                eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition     *psEndPointDefinition;
    tsZCL_ClusterInstance         *psClusterInstance;
    tsSM_CustomStruct              *psSM_CustomStruct;

    //Find SMC on the given EP
    eStatus = eSE_FindSimpleMeteringCluster(u8SourceEndPointId, bIsServer, &psEndPointDefinition, &psClusterInstance, &psSM_CustomStruct);

    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    if (bSM_GetFastPollModeStatus())
    {
        vSM_CheckFastPollEndTime(psEndPointDefinition, psClusterInstance);
    }
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       vSM_CheckFastPollEndTime
 **
 ** DESCRIPTION:
 ** Check Fast poll end time if timer has expired then generates User call back
 **
 ** PARAMETERS:                 Name               Usage
 ***psEndPointDefinition,        Pointer to Simple Metering Endpoint definition
 ***psClusterInstance            Pointer to Cluster instance for Simple Metering
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  void vSM_CheckFastPollEndTime(tsZCL_EndPointDefinition *psEndPointDefinition,
                                                        tsZCL_ClusterInstance       *psClusterInstance)
{
    if (u32ZCL_GetUTCTime() >= sSE_FastPollModeStatus.u32FastPollEndTime)
    {
        //Disable Fast Poll mode
        vSM_UpdateFastPollModeStatus(FALSE);

        //Re-Initialise Fast Poll end time to 0
        sSE_FastPollModeStatus.u32FastPollEndTime = 0;

        //Re-Initialise the maximum duration to 0
        sSE_FastPollModeStatus.u32MaxFastPollDuration = 0;

        //Generate a call back to user as fast polling mode is over.
        if ((psEndPointDefinition != NULL) && (psClusterInstance != NULL))
        {
            tsSM_CustomStruct *pSM_Common = ((tsSM_CustomStruct *)psClusterInstance->pvEndPointCustomStructPtr);

            pSM_Common->sSMCallBackMessage.eEventType = E_CLD_SM_FAST_POLLING_TIMER_EXPIRED;
            psEndPointDefinition->pCallBackFunctions(&pSM_Common->sSMCustomCallBackEvent);
        }
    }
}

/****************************************************************************
 **
 ** NAME:       vSM_UpdateFastPollModeStatus
 **
 ** DESCRIPTION:
 ** Enable/Disable Fast poll Mode Status
 **
 ** PARAMETERS:                 Name               Usage
 **    bStatus                    Enable/Disable Fast poll mode.
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  void vSM_UpdateFastPollModeStatus(bool_t bStatus)
{
    sSE_FastPollModeStatus.bFastPollModeEnabled = bStatus;
    return;
}

/****************************************************************************
 **
 ** NAME:       bSM_GetFastPollModeStatus
 **
 ** DESCRIPTION:
 ** Enable/Disable Fast poll Mode Status
 **
 ** PARAMETERS:                 Name               Usage
 **    void
 **
 ** RETURN:
 ** Fast poll mode state
 **
 **
 ****************************************************************************/
PRIVATE  bool_t bSM_GetFastPollModeStatus(void)
{
    return sSE_FastPollModeStatus.bFastPollModeEnabled;
}

#endif /*SM_SERVER*/

#endif /*CLD_SM_SUPPORT_FAST_POLL_MODE*/

#endif     /*CLD_SIMPLE_METERING*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

