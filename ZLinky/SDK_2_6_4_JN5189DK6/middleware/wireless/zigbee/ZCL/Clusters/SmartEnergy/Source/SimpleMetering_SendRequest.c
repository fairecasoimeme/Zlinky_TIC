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

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

#ifdef CLD_SM_SUPPORT_GET_PROFILE

#ifdef SM_CLIENT
/****************************************************************************
 **
 ** NAME:       eSM_ClientGetProfileCommand
 **
 ** DESCRIPTION:
 **    Sends the Get profile request from client end to the server.
 **
 ** PARAMETERS:
 ** u8SourceEndpoint         : Source End point from where get profile request is sent
 ** u8DestinationEndpoint     : Destination end Point
 ** psDestinationAddress     : Destination Address
 ** u8IntervalChannel         : Interval Channel (1/0) for Consumption Received / Delivered
 ** u32EndTime                : Time for which data is required
 ** u8NumberOfPeriods         : Number of Intervals required
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ClientGetProfileCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    uint8 u8IntervalChannel,
                    uint32 u32EndTime,
                    uint8      u8NumberOfPeriods)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;
    tsSM_GetProfileCommand sGetProfileCommand = {u8IntervalChannel, u32EndTime, u8NumberOfPeriods};

    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &sGetProfileCommand.eIntervalChannel},
                                                  {1, E_ZCL_UINT32,   &sGetProfileCommand.u32EndTime},
                                                  {1, E_ZCL_UINT8,   &sGetProfileCommand.u8NumberOfPeriods}
                                                };
    uint8 u8SequenceNumber ;

    if((eZCL_Status =
        eZCL_FindCluster(SE_CLUSTER_ID_SIMPLE_METERING,
                        u8SourceEndpoint,
                        FALSE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        (void*)&psCustomData))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
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
                                                 FALSE,
                                                 E_CLD_SM_GET_PROFILE,
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
 ** NAME:       u32GetReceivedProfileData
 **
 ** DESCRIPTION:    Returns the received Get profile data
 **
 ** PARAMETERS:
 **
 **     psSMGetProfileResponseCommand : Pointer to the call-back message,
 **                                     having received get profile data
 **
 **
 ** RETURN:
 **     uint32 : returns 0xFFFFFFFF on error
 **
 ****************************************************************************/
PUBLIC  uint32 u32SM_GetReceivedProfileData(tsSM_GetProfileResponseCommand *psSMGetProfileResponseCommand)
{
    uint16 u16Offset = 0;
    uint32 u32RetVal = 0xFFFFFFFF;

    if (psSMGetProfileResponseCommand->pau24Intervals == NULL)
    {
        return u32RetVal;
    }

    u16Offset = u16ZCL_ReadTypeNBO((uint8 *)psSMGetProfileResponseCommand->pau24Intervals, E_ZCL_UINT24, (void*)&u32RetVal);
    if (u16Offset)
    {
        psSMGetProfileResponseCommand->pau24Intervals = (zuint24 *)((uint8*)(psSMGetProfileResponseCommand->pau24Intervals) + u16Offset);
    }
    else
    {
        psSMGetProfileResponseCommand->pau24Intervals = NULL;
        u32RetVal = 0xFFFFFFFF;
    }

    return u32RetVal;
}

#endif  /*SM_CLIENT*/

#ifdef SM_SERVER

/****************************************************************************
 **
 ** NAME:       vSM_GetProfileBufferedData
 **
 ** DESCRIPTION:    Returns Profile buffered data
 **
 ** PARAMETERS:
 **        u8SourceEndPointId : End point where Simple Metering server is registered,
 **     psSEGetProfile : Pointer to an array of Get profile structure
 **
 ** RETURN:
 **        teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status vSM_GetProfileBufferedData(uint8 u8SourceEndPointId,tsSEGetProfile *psSEGetProfile)
{
    teZCL_Status                eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition     *psEndPointDefinition;
    tsZCL_ClusterInstance         *psClusterInstance;
    tsSM_CustomStruct              *psSM_CustomStruct;
    uint8                         u8LoopCntr;

    //Find SMC on the given EP
    eStatus = eSE_FindSimpleMeteringCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psSM_CustomStruct);

    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    else
    {
        //Get Mutex as we are updating the shared GetProfile data.
        #ifndef COOPERATIVE
            eZCL_GetMutex(psEndPointDefinition);
        #endif

        for(u8LoopCntr = 0; u8LoopCntr < CLD_SM_GETPROFILE_MAX_NO_INTERVALS; u8LoopCntr++)
        {
            psSEGetProfile[u8LoopCntr].u32UtcTime = sSEGetProfile[u8LoopCntr].u32UtcTime;
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
            psSEGetProfile[u8LoopCntr].u24ConsumptionReceived = sSEGetProfile[u8LoopCntr].u24ConsumptionReceived;
#endif
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
            psSEGetProfile[u8LoopCntr].u24ConsumptionDelivered = sSEGetProfile[u8LoopCntr].u24ConsumptionDelivered;
#endif
        }
        //Release Mutex, data is updated.
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

    }

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       vSM_SetProfileBufferedData
 **
 ** DESCRIPTION:    Returns Profile buffered data
 **
 ** PARAMETERS:
 **        u8SourceEndPointId : End point where Simple Metering server is registered,
 **     psSEGetProfile : Pointer to an array of Get profile structure
 **
 ** RETURN:
 **        teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status vSM_SetProfileBufferedData(uint8 u8SourceEndPointId,tsSEGetProfile *psSEGetProfile)
{
    teZCL_Status                eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition     *psEndPointDefinition;
    tsZCL_ClusterInstance         *psClusterInstance;
    tsSM_CustomStruct              *psSM_CustomStruct;
    uint8                         u8LoopCntr;

    //Find SMC on the given EP
    eStatus = eSE_FindSimpleMeteringCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psSM_CustomStruct);

    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    else
    {
        //Get Mutex as we are updating the shared GetProfile data.
        #ifndef COOPERATIVE
            eZCL_GetMutex(psEndPointDefinition);
        #endif

        for(u8LoopCntr = 0; u8LoopCntr < CLD_SM_GETPROFILE_MAX_NO_INTERVALS; u8LoopCntr++)
        {
            sSEGetProfile[u8LoopCntr].u32UtcTime = psSEGetProfile[u8LoopCntr].u32UtcTime;
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
            sSEGetProfile[u8LoopCntr].u24ConsumptionReceived = psSEGetProfile[u8LoopCntr].u24ConsumptionReceived;
#endif

#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
            sSEGetProfile[u8LoopCntr].u24ConsumptionDelivered = psSEGetProfile[u8LoopCntr].u24ConsumptionDelivered;
#endif

            if (psSEGetProfile[u8LoopCntr].u32UtcTime)
            {
                u8GetProfileCntr = u8LoopCntr;
            }
        }
        //Release Mutex, data is updated.
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

    }

    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eSM_ServerUpdateConsumption
 **
 ** DESCRIPTION:    Updates consumption received and consumption delivered
 **
 ** PARAMETERS:
 **        u8SourceEndPointId : Source EndPoint Id
 **     u32UtcTime : The values at which the values need to be saved
 **
 **
 ** RETURN:
 **     teZCL_eStatus
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ServerUpdateConsumption(uint8 u8SourceEndPointId, uint32 u32UtcTime)
{
    teZCL_Status                eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition     *psEndPointDefinition;
    tsZCL_ClusterInstance         *psClusterInstance;
    tsSM_CustomStruct              *psSM_CustomStruct;
    tsCLD_SimpleMetering        *psCldSMDev;

    //Find SMC on the given EP
    eStatus = eSE_FindSimpleMeteringCluster(u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, &psSM_CustomStruct);

    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    else
    {
        //Get Mutex as we are updating the shared GetProfile data.
        #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

        psCldSMDev = (tsCLD_SimpleMetering *)psClusterInstance->pvEndPointSharedStructPtr;
        sSEGetProfile[u8GetProfileCntr].u32UtcTime = u32UtcTime;
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED
        sSEGetProfile[u8GetProfileCntr].u24ConsumptionReceived = psCldSMDev->u24CurrentPartialProfileIntervalValueReceived;
#endif
#ifdef CLD_SM_ATTR_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED
        sSEGetProfile[u8GetProfileCntr].u24ConsumptionDelivered = psCldSMDev->u24CurrentPartialProfileIntervalValueDelivered;
#endif

        if (u8GetProfileCntr >= (CLD_SM_GETPROFILE_MAX_NO_INTERVALS -1))
        {
            u8GetProfileCntr = 0;
        }
        else
        {
            u8GetProfileCntr++;
        }
        //Release Mutex, data is updated.
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    }

    return eStatus;
}
#endif /*SM_SERVER*/

#endif   /*CLD_SM_SUPPORT_GET_PROFILE*/


#ifdef CLD_SM_SUPPORT_MIRROR
/****************************************************************************
 **
 ** NAME:       eSM_GetFreeMirrorEndPoint
 **
 ** DESCRIPTION: Finds out the the free end point from the list of available
 **             Mirror end point list.
 **
 ** PARAMETERS:
 ** *pu16FreeEP : Returns the free End Point or 0xFFFF
 **
 ** RETURN:
 **teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_GetFreeMirrorEndPoint(uint16 *pu16FreeEP)
{
    teZCL_Status eZCL_Status = E_ZCL_SUCCESS;

#ifdef PC_PLATFORM_BUILD
    *pu16FreeEP = (uint16)psSE_Mirror[0].sEndPoint.u8EndPointNumber;;
#else
    uint8 u8LoopCntr;
    bool_t bEnabled;

    *pu16FreeEP = 0xFFFF;
    for (u8LoopCntr = 0; u8LoopCntr < CLD_SM_NUMBER_OF_MIRRORS ; u8LoopCntr++)
    {
        if(ZPS_E_SUCCESS != ZPS_eAplAfGetEndpointState(psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber,&bEnabled))
        {
            break;
        }
        else
        {
            if(bEnabled == FALSE)
            {
                *pu16FreeEP = (uint16)psSE_Mirror[u8LoopCntr].sEndPoint.u8EndPointNumber;
                break;
            }
        }
    }
#endif
    return eZCL_Status;
}
#endif
/****************************************************************************
 **
 ** NAME:       eSM_ServerRequestMirrorCommand
 **
 ** DESCRIPTION: Simple Metring Server makes a Mirror request
 **
 ** PARAMETERS:
 ** uint8 u8SourceEndpoint : Source Address
 ** uint8 u8DestinationEndpoint : Destination End Point
 ** tsZCL_Address *psDestinationAddress: Destination Address
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ServerRequestMirrorCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;

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
                                                 E_CLD_SM_REQUEST_MIRROR,
                                                 &u8SequenceNumber,
                                                 NULL,
                                                 FALSE,
                                                 0,
                                                 0);
        }
    }
    return eZCL_Status;
}


/****************************************************************************
 **
 ** NAME:       eSM_ServerRemoveMirrorCommand
 **
 ** DESCRIPTION: Simple Metering Server sends Remove Mirror command
 **
 ** PARAMETERS:
 **  u8SourceEndpoint : Source End Point
 **  u8DestinationEndpoint : Destination End Point
 **  *psDestinationAddress : Pointer to Destination Address
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ServerRemoveMirrorCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;

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
        psEndPointDefinition->bDisableDefaultResponse = TRUE;
        eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                             u8DestinationEndpoint,
                                             psDestinationAddress,
                                             SE_CLUSTER_ID_SIMPLE_METERING,
                                             TRUE,
                                             E_CLD_SM_REMOVE_MIRROR,
                                             &u8SequenceNumber,
                                             NULL,
                                             FALSE,
                                             0,
                                             0);
    }

    return eZCL_Status;
}
#ifdef CLD_SM_SUPPORT_MIRROR
/****************************************************************************
 **
 ** NAME:       eSM_ClientRequestMirrorResponseCommand
 **
 ** DESCRIPTION:
 ** Simple Metering Client returns Mirror request, with valid/Non valid End point
 ** PARAMETERS:
 **u8SourceEndpoint : Source end Point
 **uint8 u8DestinationEndpoint : Destination End Point
 ***psDestinationAddress  : Pointer to Destination Address
 **u16MirroredEP : Mirrored End point or 0xFFFF
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ClientRequestMirrorResponseCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    uint16 u16MirroredEP)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;
    uint8 u8SequenceNumber;
    tsSM_RequestMirrorResponseCommand sRequestMirrorResponseCommand;
    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT16,   &sRequestMirrorResponseCommand.u16Endpoint} };

    sRequestMirrorResponseCommand.u16Endpoint = u16MirroredEP;
    if((eZCL_Status = eZCL_FindCluster(SE_CLUSTER_ID_SIMPLE_METERING,
                        u8SourceEndpoint,
                        FALSE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        (void*)&psCustomData))
                           == E_ZCL_SUCCESS)
    {
        psEndPointDefinition->bDisableDefaultResponse = TRUE;

        eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                             u8DestinationEndpoint,
                                             psDestinationAddress,
                                             SE_CLUSTER_ID_SIMPLE_METERING,
                                             FALSE,
                                             E_CLD_SM_REQUEST_MIRROR_RESPONSE,
                                             &u8SequenceNumber,
                                             asPayloadDefinition,
                                             FALSE,
                                             0,
                                             sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

    }
    return eZCL_Status;
}


/****************************************************************************
 **
 ** NAME:       eSM_ClientMirrorRemovedCommand
 **
 ** DESCRIPTION: Simple Metering client sends mirror removed command
 **
 ** PARAMETERS:
 ** uint8 u8SourceEndpoint, : Source End point
 ** uint8 u8DestinationEndpoint : Destination end point
 ** tsZCL_Address *psDestinationAddress : Pointer to Destination Address
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ClientMirrorRemovedCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;
    uint16 u16PayLoadEndPoint = (uint16)u8SourceEndpoint;

    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT16,  (void*)&u16PayLoadEndPoint}
                                                };
    uint8 u8SequenceNumber ;

    if((eZCL_Status =
        eZCL_FindCluster(SE_CLUSTER_ID_SIMPLE_METERING,
                        u8SourceEndpoint,
                        FALSE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        (void*)&psCustomData))
                           == E_ZCL_SUCCESS)
    {
        psEndPointDefinition->bDisableDefaultResponse = TRUE;
        eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                             u8DestinationEndpoint,
                                             psDestinationAddress,
                                             SE_CLUSTER_ID_SIMPLE_METERING,
                                             FALSE,
                                             E_CLD_SM_MIRROR_REMOVED,
                                             &u8SequenceNumber,
                                             asPayloadDefinition,
                                             FALSE,
                                             0,
                                             sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
        if (eZCL_Status == E_ZCL_SUCCESS)
        {
#ifndef PC_PLATFORM_BUILD
            eZCL_Status = ZPS_eAplAfSetEndpointState(u8SourceEndpoint, FALSE);
#endif
        }
    }
    return eZCL_Status;
}

#endif  /*CLD_SM_SUPPORT_MIRROR*/



#ifdef CLD_SM_SUPPORT_FAST_POLL_MODE

#ifdef SM_CLIENT
/****************************************************************************
 **
 ** NAME:       eSM_ClientRequestFastPollCommand
 **
 ** DESCRIPTION:
 **                Simple Metering Cluster requests for Fast polling mode from Server
 ** PARAMETERS:
 ** u8SourceEndpoint        :
 ** u8DestinationEndpoint    :
 ** *psDestinationAddress   : Address of SM server
 ** u8FastPollUpdatePeriod    : Required Fast Polling update period
 ** u8Duration                : Required duration for Fast polling mode.
 **
 ** RETURN:
 **teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eSM_ClientRequestFastPollCommand(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    uint8 u8FastPollUpdatePeriod,
                    uint8 u8Duration)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsSM_CustomStruct *psCustomData;
    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &u8FastPollUpdatePeriod},
                                                  {1, E_ZCL_UINT8,   &u8Duration}
                                                };
    uint8 u8SequenceNumber ;

    if((eZCL_Status =
        eZCL_FindCluster(SE_CLUSTER_ID_SIMPLE_METERING,
                        u8SourceEndpoint,
                        FALSE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        (void*)&psCustomData))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
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
                                                 FALSE,
                                                 E_CLD_SM_REQUEST_FAST_POLL_MODE,
                                                 &u8SequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
        }
    }
    return eZCL_Status;
}

#endif /*SM_CLIENT*/

#endif /*CLD_SM_SUPPORT_FAST_POLL_MODE*/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif
