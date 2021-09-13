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
 * MODULE:             Poll Control Cluster
 *
 * COMPONENT:          PollControl.c
 *
 * DESCRIPTION:        Poll Control cluster definition
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
#include "PollControl.h"
#include "PollControl_internal.h"

#include "zps_apl_aps.h"

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
#error You Must Have either POLL_CONTROL_SERVER and/or POLL_CONTROL_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_POLL_CONTROL
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_PollControlClusterCommandDefinitions[] = {
        {E_CLD_POLL_CONTROL_CMD_CHECKIN,                E_ZCL_CF_RX|E_ZCL_CF_TX},     /* Mandatory */
        {E_CLD_POLL_CONTROL_CMD_FAST_POLL_STOP,         E_ZCL_CF_RX},     /* Mandatory */
        {E_CLD_POLL_CONTROL_CMD_SET_LONG_POLL_INTERVAL, E_ZCL_CF_RX},     /* Mandatory */    
        {E_CLD_POLL_CONTROL_CMD_SET_SHORT_POLL_INTERVAL,E_ZCL_CF_RX}      /* Mandatory */     
    };
#endif

const tsZCL_AttributeDefinition asCLD_PollControlClusterAttrDefs[] = {
#ifdef POLL_CONTROL_SERVER
        {E_CLD_POLL_CONTROL_ATTR_ID_CHECKIN_INTERVAL,                 (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_UINT32,  (uint32)(&((tsCLD_PollControl*)(0))->u32CheckinInterval),0},          /* Mandatory */

        {E_CLD_POLL_CONTROL_ATTR_ID_LONG_POLL_INTERVAL,               (E_ZCL_AF_RD),                           E_ZCL_UINT32,  (uint32)(&((tsCLD_PollControl*)(0))->u32LongPollInterval),0},         /* Mandatory */

        {E_CLD_POLL_CONTROL_ATTR_ID_SHORT_POLL_INTERVAL,              (E_ZCL_AF_RD),                           E_ZCL_UINT16,  (uint32)(&((tsCLD_PollControl*)(0))->u16ShortPollInterval),0},        /* Mandatory */
        
        {E_CLD_POLL_CONTROL_ATTR_ID_FAST_POLL_TIMEOUT,                (E_ZCL_AF_RD|E_ZCL_AF_WR),               E_ZCL_UINT16,  (uint32)(&((tsCLD_PollControl*)(0))->u16FastPollTimeout),0},          /* Mandatory */

    #ifdef CLD_POLL_CONTROL_ATTR_CHECKIN_INTERVAL_MIN
        {E_CLD_POLL_CONTROL_ATTR_ID_CHECKIN_INTERVAL_MIN,              E_ZCL_AF_RD,                            E_ZCL_UINT32,  (uint32)(&((tsCLD_PollControl*)(0))->u32CheckinIntervalMin),0},       /* Optional */
    #endif

    #ifdef CLD_POLL_CONTROL_ATTR_LONG_POLL_INTERVAL_MIN
        {E_CLD_POLL_CONTROL_ATTR_ID_LONG_POLL_INTERVAL_MIN,            E_ZCL_AF_RD,                            E_ZCL_UINT32,  (uint32)(&((tsCLD_PollControl*)(0))->u32LongPollIntervalMin),0},      /* Optional */
    #endif

    #ifdef CLD_POLL_CONTROL_ATTR_FAST_POLL_TIMEOUT_MAX
        {E_CLD_POLL_CONTROL_ATTR_ID_FAST_POLL_TIMEOUT_MAX,             E_ZCL_AF_RD,                            E_ZCL_UINT16,  (uint32)(&((tsCLD_PollControl*)(0))->u16FastPollTimeoutMax),0},       /* Optional */
    #endif
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                             (E_ZCL_AF_RD|E_ZCL_AF_GA),               E_ZCL_BMAP32,  (uint32)(&((tsCLD_PollControl*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                        (E_ZCL_AF_RD|E_ZCL_AF_GA),               E_ZCL_UINT16,  (uint32)(&((tsCLD_PollControl*)(0))->u16ClusterRevision),0},   /* Mandatory  */

    };
tsZCL_ClusterDefinition sCLD_PollControl = {
        GENERAL_CLUSTER_ID_POLL_CONTROL,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_PollControlClusterAttrDefs) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_PollControlClusterAttrDefs,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_PollControlClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_PollControlClusterCommandDefinitions        
        #endif        
};
uint8 au8PollControlAttributeControlBits[(sizeof(asCLD_PollControlClusterAttrDefs) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlCreatePollControl
 **
 ** DESCRIPTION:
 ** Creates a Poll Control cluster
 **
 ** PARAMETERS:                              Name                                Usage
 ** tsZCL_ClusterInstance                   *psClusterInstance                   Cluster structure
 ** bool_t                                   bIsServer                           Server Client flag
 ** tsZCL_ClusterDefinition                 *psClusterDefinition                 Cluster Definition
 ** void                                    *pvEndPointSharedStructPtr           EndPoint Shared Structure
 ** uint8                                   *pu8AttributeControlBits             Attribute Control Bits
 ** tsCLD_PollControlCustomDataStructure    *psCustomDataStructure               Pointer to Custom Data Structure
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlCreatePollControl(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits,
                tsCLD_PollControlCustomDataStructure    *psCustomDataStructure)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
           if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_PollControlCommandHandler);      

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 
  
    /* Set attribute default values */
   
    if(pvEndPointSharedStructPtr != NULL)
    {
        #ifdef POLL_CONTROL_SERVER    
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32CheckinInterval     =   0x3840;
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32LongPollInterval    =   0x14;
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u16ShortPollInterval   =   0x02;
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u16FastPollTimeout     =   0x028;

        #ifdef CLD_POLL_CONTROL_ATTR_LONG_POLL_INTERVAL_MIN
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32LongPollIntervalMin =  CLD_POLL_CONTROL_ATTR_LONG_POLL_INTERVAL_MIN;
        #endif  

        #ifdef CLD_POLL_CONTROL_ATTR_CHECKIN_INTERVAL_MIN
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32CheckinIntervalMin =  CLD_POLL_CONTROL_ATTR_CHECKIN_INTERVAL_MIN;
        #endif

        #ifdef CLD_POLL_CONTROL_ATTR_FAST_POLL_TIMEOUT_MAX
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u16FastPollTimeoutMax =  CLD_POLL_CONTROL_ATTR_FAST_POLL_TIMEOUT_MAX;
        #endif  
        
        psCustomDataStructure->sControlParameters.u32CurrentPollInterval = ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32LongPollInterval;
        psCustomDataStructure->sControlParameters.u32CheckinTickUpdate = ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32CheckinInterval;
        
        #endif
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_POLL_CONTROL_FEATURE_MAP;
            
            ((tsCLD_PollControl*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_POLL_CONTROL_CLUSTER_REVISION;
        }

    return E_ZCL_SUCCESS;

}

#ifdef POLL_CONTROL_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_PollControlUpdate
 **
 ** DESCRIPTION:
 ** Used by cluster to update its attributes
 **
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_PollControlUpdate(void)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_Address sZCL_Address = {0};
    tsCLD_PollControlCustomDataStructure *psCustomDataStructure;
    uint8 u8TransactionSeqNumber = 0;
    uint8 u8NumEndpoints;

    u8NumEndpoints = u8ZCL_GetNumberOfEndpointsRegistered();
    
    int i;        
    
    /* Update clusters on each end point if any */
    for(i = 0; i < u8NumEndpoints; i++)
    {
        eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_POLL_CONTROL, u8ZCL_GetEPIdFromIndex(i), TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
        if(eStatus != E_ZCL_SUCCESS)
        {
            continue;
        }
        
        // get EP mutex
        #ifndef COOPERATIVE
            eZCL_GetMutex(psEndPointDefinition);
        #endif

    
        /* If in fast polling mode, Keep polling at ShortInterval till the FastPollTimeout and 
        as soon as fast poll mode has timed out start polling at Long Poll Interval*/
        if(psCustomDataStructure->sControlParameters.bFastPollMode)
        {
            psCustomDataStructure->sControlParameters.u32CurrentPollInterval = ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ShortPollInterval;
            if(psCustomDataStructure->sControlParameters.u16CurrentFastPollTimeout > 0){
                psCustomDataStructure->sControlParameters.u16CurrentFastPollTimeout--;
            }else{
                psCustomDataStructure->sControlParameters.u32CurrentPollInterval = ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval;
                psCustomDataStructure->sControlParameters.u32PollTickUpdate = 0;
                psCustomDataStructure->sControlParameters.bFastPollMode = FALSE;
                psCustomDataStructure->sControlParameters.u16FastPollTimeoutMax = 0;
                psCustomDataStructure->sControlParameters.u8CurrentClient = 0;
                memset(psCustomDataStructure->sControlParameters.asCheckinEntry,0,sizeof(tsCLD_PollControlCheckinEntry) * CLD_POLL_CONTROL_NUMBER_OF_MULTIPLE_CLIENTS);
            }   
        }else{   /* If not in fast poll mode poll at LongInterval*/
            psCustomDataStructure->sControlParameters.u32CurrentPollInterval = ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval;
        }
        
        /* The polling should happen at  u32PollTickUpdate rate and as soon as poll timer times out send a data request to parent
        PollTicks are updated based on whether server is in fast poll mode or not */
        if(psCustomDataStructure->sControlParameters.u32PollTickUpdate > 0){
            psCustomDataStructure->sControlParameters.u32PollTickUpdate --;
        }else{
            psCustomDataStructure->sControlParameters.u32PollTickUpdate = psCustomDataStructure->sControlParameters.u32CurrentPollInterval;
            ZPS_eAplZdoPoll();
        }
        
        /* The check in message should be sent at rate u32CheckinTickUpdate and as soon as Checkin ticks times out send a check in message
        to bound device. u32CheckinTickUpdate is updated from set attributes */
        if(psCustomDataStructure->sControlParameters.u32CheckinTickUpdate > 0){
            psCustomDataStructure->sControlParameters.u32CheckinTickUpdate --;
        }else{
            psCustomDataStructure->sControlParameters.u32CheckinTickUpdate = ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinInterval;
            #ifdef CLD_POLL_CONTROL_BOUND_TX_WITH_APS_ACK_DISABLED
                sZCL_Address.eAddressMode = E_ZCL_AM_BOUND_NO_ACK;
            #else
                sZCL_Address.eAddressMode = E_ZCL_AM_BOUND;
            #endif  
            eCLD_PollControlCheckinSend(u8ZCL_GetEPIdFromIndex(i),
                                        0,
                                        &sZCL_Address,
                                        &u8TransactionSeqNumber);
            
            psCustomDataStructure->sControlParameters.bFastPollMode = TRUE;
            psCustomDataStructure->sControlParameters.u32PollTickUpdate = 0;
            /* We need to wait for 7.68 seconds to receive all the check in response so its 31 counts if we consider 250 ms as tick*/
            psCustomDataStructure->sControlParameters.u16CurrentFastPollTimeout = 31;
        }
    
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

    }
    return eStatus;

}

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlWriteHandler
 **
 ** DESCRIPTION:
 ** Used by cluster to handle write to its attribute
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_PollControlWriteHandler(
                                          uint8             u8SourceEndPointId,
                                          uint16            u16AttributeId,
                                          uint32            *pu32AttributeData,
                                          uint8             *pu8ErrorCode)
{
    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_PollControlCustomDataStructure *psCustomDataStructure;

    
    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_POLL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    switch(u16AttributeId)
    {
        case E_CLD_POLL_CONTROL_ATTR_ID_CHECKIN_INTERVAL:
        {
            /* Checks if CheckinInterval is in range ; if not return with INVALID value else update the checkin ticks*/
            if( (((*pu32AttributeData) > CLD_POLL_CONTROL_CHECKIN_INTERVAL_MAX ) )  
#ifdef CLD_POLL_CONTROL_ATTR_CHECKIN_INTERVAL_MIN
             || (((*pu32AttributeData) < ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinIntervalMin) ) 
#else
             || (((*pu32AttributeData)) < CLD_POLL_CONTROL_CHECKIN_INTERVAL_MIN )
#endif
             || (((*pu32AttributeData) < ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval))
             ){
                *pu8ErrorCode = E_ZCL_CMDS_INVALID_VALUE;
                return E_ZCL_ERR_INVALID_VALUE;
             }else{
                    psCustomDataStructure->sControlParameters.u32CheckinTickUpdate = *pu32AttributeData;
            }
            break;
        }
        case E_CLD_POLL_CONTROL_ATTR_ID_FAST_POLL_TIMEOUT:
        {
            /* Checks if Fast Poll Timeout is in range ; if not return with INVALID value */
            if(
#ifdef CLD_POLL_CONTROL_ATTR_FAST_POLL_TIMEOUT_MAX
            (((uint16)(*pu32AttributeData) > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16FastPollTimeoutMax) )
#else
            ((uint16)(*pu32AttributeData) > CLD_POLL_CONTROL_FAST_POLL_TIMEOUT_MAX) 
#endif      
            ||((*pu32AttributeData) < CLD_POLL_CONTROL_FAST_POLL_TIMEOUT_MIN))
            {
                *pu8ErrorCode = E_ZCL_CMDS_INVALID_VALUE;
                return E_ZCL_ERR_INVALID_VALUE;
             }
            break;
        }
        default:
            break;
    }

    return E_ZCL_SUCCESS;

}

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlSetAttribute
 **
 ** DESCRIPTION:
 ** Used by cluster to set its attributes
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** uint8                       u8AttributeId               Attribute Id
 ** uint32                      u32AttributeValue           Attribute Value
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_PollControlSetAttribute(
                                            uint8               u8SourceEndPointId,
                                            uint8               u8AttributeId,
                                            uint32              u32AttributeValue)  
{
    teZCL_Status eStatus;
    uint8 u8Status;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_PollControlCustomDataStructure *psCustomDataStructure;

    eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_POLL_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
        
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    switch(u8AttributeId)
    {
    case E_CLD_POLL_CONTROL_ATTR_ID_CHECKIN_INTERVAL:
    {
        eStatus = eCLD_PollControlWriteHandler(u8SourceEndPointId,u8AttributeId,&u32AttributeValue,&u8Status);
        if(eStatus == E_ZCL_SUCCESS)
            ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinInterval = u32AttributeValue;
        break;
    }
    case E_CLD_POLL_CONTROL_ATTR_ID_LONG_POLL_INTERVAL:
    {
        /* Checks if LongPollInterval is in range ; if not return with INVALID value else update the Long Poll Interval 
        and update Poll Tick if not in Fast Poll Mode*/
        if( (u32AttributeValue > CLD_POLL_CONTROL_LONG_POLL_INTERVAL_MAX) || 
        (u32AttributeValue > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinInterval) ||
            (u32AttributeValue < ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ShortPollInterval) 
#ifdef CLD_POLL_CONTROL_ATTR_LONG_POLL_INTERVAL_MIN
             || ((u32AttributeValue < ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollIntervalMin) ) 
#else
             || (u32AttributeValue < CLD_POLL_CONTROL_LONG_POLL_INTERVAL_MIN) 
#endif
        ){
                return E_ZCL_ERR_INVALID_VALUE;
        }
        ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval = u32AttributeValue;
        if(!(psCustomDataStructure->sControlParameters.bFastPollMode))
            psCustomDataStructure->sControlParameters.u32PollTickUpdate = 0;
        break;
    }
    case E_CLD_POLL_CONTROL_ATTR_ID_SHORT_POLL_INTERVAL:
    {
        /* Checks if ShortPollInterval is in range ; if not return with INVALID value else update the Short Poll Interval 
        and update Poll Tick if in Fast Poll Mode*/
        if((u32AttributeValue == 0) || 
        (u32AttributeValue > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32CheckinInterval) ||
            (u32AttributeValue > ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u32LongPollInterval) ||
            ((u32AttributeValue > CLD_POLL_CONTROL_SHORT_POLL_INTERVAL_MAX) || (u32AttributeValue < CLD_POLL_CONTROL_SHORT_POLL_INTERVAL_MIN))    
        ){
                return E_ZCL_ERR_INVALID_VALUE;
        }
        ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ShortPollInterval = (uint16)u32AttributeValue;
        if(psCustomDataStructure->sControlParameters.bFastPollMode)
            psCustomDataStructure->sControlParameters.u32PollTickUpdate = 0;
        break;
    }
    case E_CLD_POLL_CONTROL_ATTR_ID_FAST_POLL_TIMEOUT:
    {
        eStatus = eCLD_PollControlWriteHandler(u8SourceEndPointId,u8AttributeId,&u32AttributeValue,&u8Status);
        if(eStatus == E_ZCL_SUCCESS)
            ((tsCLD_PollControl *)(psClusterInstance->pvEndPointSharedStructPtr))->u16FastPollTimeout = (uint16)u32AttributeValue;
        break;
    }
    default:
        return E_ZCL_DENY_ATTRIBUTE_ACCESS;
        break;
    }
    
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    
    return eStatus;

}

/****************************************************************************
 **
 ** NAME:       u8CLD_PollControlFindCheckinEntry
 **
 ** DESCRIPTION:
 ** Used by cluster to find entry from Binding table
 **
 ** PARAMETERS:                             Name                        Usage
 ** tsCLD_PollControlCheckinEntry           psCheckinEntry              Pointer to Check In entry
 ** bool_t                                    *pbEntryFound               Whether Entry is present or not
 **
 ** RETURN:
 ** uint8 
 ** 
 ****************************************************************************/

PUBLIC  uint8 u8CLD_PollControlFindCheckinEntry(
                                            tsCLD_PollControlCheckinEntry      *psCheckinEntry,
											bool_t                               *pbEntryFound)
{   
    uint8   u8NumofEntries = 0,j = 0;
    ZPS_tsAplAib * tsAplAib;
    uint64 u64MacAddress;
    tsAplAib  = ZPS_psAplAibGetAib();
    
    *pbEntryFound = FALSE;
    
    if((psCheckinEntry == NULL) || (pbEntryFound == NULL))
    {
        return 0;
    }
    
    u64MacAddress = ZPS_u64NwkNibFindExtAddr(ZPS_pvNwkGetHandle(), psCheckinEntry->u16Address);
    DBG_vPrintf(TRACE_POLL_CONTROL, "\r\nBind Size %d",  tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable );

    for( j = 0 ; j < tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable ; j++ )
    {
        if((tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId == GENERAL_CLUSTER_ID_POLL_CONTROL))
        {
            
            if(tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DstAddrMode == ZPS_E_ADDR_MODE_IEEE)
            {
                if((u64MacAddress == ZPS_u64NwkNibGetMappedIeeeAddr (ZPS_pvNwkGetHandle(),tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16AddrOrLkUp)) &&
                   (psCheckinEntry->u8EndPointId == tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DestinationEndPoint))
                {
                    *pbEntryFound = TRUE;
                }
            }
        
            u8NumofEntries++;
        }
        
    }
    return u8NumofEntries;
}

/****************************************************************************
 **
 ** NAME:       eCLD_PollControlUpdateSleepInterval
 **
 ** DESCRIPTION:
 ** Used by cluster to update its check in tick based on sleeping time
 **
 ** PARAMETERS:             Name                        Usage
 ** uint32                  u32QuarterSecondsAsleep     Time in quaterseconds for which device has slept
 **      
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PollControlUpdateSleepInterval(
                                        uint32           u32QuarterSecondsAsleep)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_PollControlCustomDataStructure *psCustomDataStructure;
    uint8 u8NumEndpoints;

    u8NumEndpoints = u8ZCL_GetNumberOfEndpointsRegistered();
    
    int i;        
    
    /* Update clusters on each end point if any */
    for(i = 0; i < u8NumEndpoints; i++)
    {
        eStatus = eZCL_FindCluster(GENERAL_CLUSTER_ID_POLL_CONTROL, u8ZCL_GetEPIdFromIndex(i), TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
        if(eStatus != E_ZCL_SUCCESS)
        {
            continue;
        }
        
        if(u32QuarterSecondsAsleep > psCustomDataStructure->sControlParameters.u32PollTickUpdate)
        {
            psCustomDataStructure->sControlParameters.u32PollTickUpdate = 0;
        }
        else
        {
            psCustomDataStructure->sControlParameters.u32PollTickUpdate -= u32QuarterSecondsAsleep;
        }
        
        if(psCustomDataStructure->sControlParameters.u32CheckinTickUpdate > u32QuarterSecondsAsleep)
        {
            psCustomDataStructure->sControlParameters.u32CheckinTickUpdate -= u32QuarterSecondsAsleep;
        }
        else
        {
            psCustomDataStructure->sControlParameters.u32CheckinTickUpdate = 0;
        }
    }  
    return eStatus;
}

#endif  /*#define POLL_CONTROL_SERVER */
#endif  

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

