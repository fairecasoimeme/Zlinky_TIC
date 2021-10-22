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
 * MODULE:             Diagnostics Cluster
 *
 * COMPONENT:          Diagnostics.h
 *
 * DESCRIPTION:        Diagnostics Cluster
 *
 *****************************************************************************/

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/* DeviceTemperatureConfiguration Cluster - Optional Attributes             */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the cluster.                                               */
/****************************************************************************/

/* Hardware Information attribute set attribute ID's (ZCL r6 3.15.2.2.1)  */
//#define CLD_DIAGNOSTICS_ATTR_ID_NUMBER_OF_RESETS
//#define CLD_DIAGNOSTICS_ATTR_ID_PERSISTENT_MEMORY_WRITES
/* Stack/Network Information attribute set attribute ID's (ZCL Spec r6 3.15.2.2.2) */
//#define CLD_DIAGNOSTICS_ATTR_ID_MAC_RX_BCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_BCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_MAC_RX_UCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST_RETRY
//#define CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST_FAIL
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_RX_BCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_TX_BCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_RX_UCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_SUCCESS
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_RETRY
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_FAIL
//#define CLD_DIAGNOSTICS_ATTR_ID_ROUTE_DISC_INITIATED
//#define CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_ADDED
//#define CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_REMOVED
//#define CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_STALE
//#define CLD_DIAGNOSTICS_ATTR_ID_JOIN_INDICATION
//#define CLD_DIAGNOSTICS_ATTR_ID_CHILD_MOVED
//#define CLD_DIAGNOSTICS_ATTR_ID_NWK_FC_FAILURE
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_FC_FAILURE
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_UNAUTHORIZED_KEY
//#define CLD_DIAGNOSTICS_ATTR_ID_NWK_DECRYPT_FAILURE
//#define CLD_DIAGNOSTICS_ATTR_ID_APS_DECRYPT_FAILURE
//#define CLD_DIAGNOSTICS_ATTR_ID_PACKET_BUFFER_ALLOCATE_FAILURE
//#define CLD_DIAGNOSTICS_ATTR_ID_RELAYED_UCAST
//#define CLD_DIAGNOSTICS_ATTR_ID_PHY_TO_MAC_QUEUE_LIMIT_REACHED
//#define CLD_DIAGNOSTICS_ATTR_ID_PACKET_VALIDATE_DROP_COUNT

//#define CLD_DIAGNOSTICS_ATTR_ID_AVERAGE_MAC_RETRY_PER_APS_MESSAGE_SENT
//#define CLD_DIAGNOSTICS_ATTR_ID_LAST_MESSAGE_LQI
//#define CLD_DIAGNOSTICS_ATTR_ID_LAST_MESSAGE_RSSI
/* End of optional attributes */

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_DIAGNOSTICS     0x0B05

#ifndef CLD_DIAGNOSTICS_CLUSTER_REVISION
    #define CLD_DIAGNOSTICS_CLUSTER_REVISION                         3
#endif 

#ifndef CLD_DIAGNOSTICS_FEATURE_MAP
    #define CLD_DIAGNOSTICS_FEATURE_MAP                              0
#endif 

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum 
{
    /* Hardware Information attribute set attribute ID's (ZCL Spec r6 3.15.2.2.1) */
    E_CLD_DIAGNOSTICS_ATTR_ID_NUMBER_OF_RESETS = 0x0000,
    E_CLD_DIAGNOSTICS_ATTR_ID_PERSISTENT_MEMORY_WRITES,

    /* Stack/Network Information attribute set attribute ID's (ZCL Spec r6 3.15.2.2.2) */
    E_CLD_DIAGNOSTICS_ATTR_ID_MAC_RX_BCAST = 0x0100,
    E_CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_BCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_MAC_RX_UCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST_RETRY,
    E_CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST_FAIL,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_RX_BCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_TX_BCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_RX_UCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_SUCCESS,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_RETRY,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_FAIL,
    E_CLD_DIAGNOSTICS_ATTR_ID_ROUTE_DISC_INITIATED,
    E_CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_ADDED,
    E_CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_REMOVED,
    E_CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_STALE,
    E_CLD_DIAGNOSTICS_ATTR_ID_JOIN_INDICATION,
    E_CLD_DIAGNOSTICS_ATTR_ID_CHILD_MOVED,
    E_CLD_DIAGNOSTICS_ATTR_ID_NWK_FC_FAILURE,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_FC_FAILURE,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_UNAUTHORIZED_KEY,
    E_CLD_DIAGNOSTICS_ATTR_ID_NWK_DECRYPT_FAILURE,
    E_CLD_DIAGNOSTICS_ATTR_ID_APS_DECRYPT_FAILURE,
    E_CLD_DIAGNOSTICS_ATTR_ID_PACKET_BUFFER_ALLOCATE_FAILURE,
    E_CLD_DIAGNOSTICS_ATTR_ID_RELAYED_UCAST,
    E_CLD_DIAGNOSTICS_ATTR_ID_PHY_TO_MAC_QUEUE_LIMIT_REACHED,
    E_CLD_DIAGNOSTICS_ATTR_ID_PACKET_VALIDATE_DROP_COUNT,
    E_CLD_DIAGNOSTICS_ATTR_ID_AVERAGE_MAC_RETRY_PER_APS_MESSAGE_SENT,
    E_CLD_DIAGNOSTICS_ATTR_ID_LAST_MESSAGE_LQI,
    E_CLD_DIAGNOSTICS_ATTR_ID_LAST_MESSAGE_RSSI,
} teCLD_Diagnostics_AttributeId;

/* Diagnostics Cluster */
typedef struct
{

#ifdef DIAGNOSTICS_SERVER
    /* Hardware Information attribute set attribute ID's (ZCL Spec r6 3.15.2.2.1) */
    #ifdef CLD_DIAGNOSTICS_ATTR_ID_NUMBER_OF_RESETS
        uint16    u16NumberOfResets;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_PERSISTENT_MEMORY_WRITES
        uint16 u16PersistentMemoryWrites;
    #endif

    /* Stack/Network Information attribute set attribute ID's (ZCL Spec r6 3.15.2.2.2) */
    #ifdef CLD_DIAGNOSTICS_ATTR_ID_MAC_RX_BCAST
        uint32 u32MacRxBcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_BCAST
        uint32 u32MacTxBcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_MAC_RX_UCAST
        uint32 u32MacRxUcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST
        uint32 u32MacTxUcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST_RETRY
        uint16 u16MacTxUcastRetry;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_MAC_TX_UCAST_FAIL
        uint16 u16MacTxUcastFail;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_RX_BCAST
        uint16 u16ApsRxBcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_TX_BCAST
        uint16 u16ApsTxBcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_RX_UCAST
        uint16 u16ApsRxUcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_SUCCESS
        uint16 u16ApsTxUcastSuccess;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_RETRY
        uint16 u16ApsTxUcastRetry;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_TX_UCAST_FAIL
        uint16 u16ApsTxUcastFail;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_ROUTE_DISC_INITIATED
        uint16 u16RouteDiscInitiated;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_ADDED
        uint16 u16NeighborAdded;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_REMOVED
        uint16 u16NeighborRemoved;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_NEIGHBOR_STALE
        uint16 u16NeighborStale;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_JOIN_INDICATION
        uint16 u16JoinIndication;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_CHILD_MOVED
        uint16 u16ChildMoved;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_NWK_FC_FAILURE
        uint16 u16NWKFCFailure;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_FC_FAILURE
        uint16 u16APSFCFailure;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_UNAUTHORIZED_KEY
        uint16 u16APSUnauthorizedKey;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_NWK_DECRYPT_FAILURE
        uint16 u16NWKDecryptFailure;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_APS_DECRYPT_FAILURE
        uint16 u16APSDecryptFailure;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_PACKET_BUFFER_ALLOCATE_FAILURE
        uint16 u16PacketBufferAllocateFailure;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_RELAYED_UCAST
        uint16 u16RelayedUcast;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_PHY_TO_MAC_QUEUE_LIMIT_REACHED
        uint16 u16PhyToMACQueueLimitReached;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_PACKET_VALIDATE_DROP_COUNT
        uint16 u16PacketValidateDropCount;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_AVERAGE_MAC_RETRY_PER_APS_MESSAGE_SENT
        uint16 u16AverageMACRetryPerAPSMessageSent;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_LAST_MESSAGE_LQI
        uint8 u8LastMessageLQI;
    #endif

    #ifdef CLD_DIAGNOSTICS_ATTR_ID_LAST_MESSAGE_RSSI
        int8 i8LastMessageRSSI;
    #endif
#endif
    zbmap32 u32FeatureMap;
    
    zuint16 u16ClusterRevision;
} tsCLD_Diagnostics;

typedef struct
{
    uint8                   u8Dummy;
} tsCLD_DiagnosticsCustomDataStructure;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_DiagnosticsCreateDiagnostics(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

#ifdef DIAGNOSTICS_SERVER                
    PUBLIC teZCL_Status eCLD_DiagnosticsUpdate(uint8 u8SourceEndPointId);
#endif
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/


extern tsZCL_ClusterDefinition sCLD_Diagnostics;
extern const tsZCL_AttributeDefinition asCLD_DiagnosticsClusterAttributeDefinitions[];
extern uint8 au8DiagnosticsClusterAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* DEVICE_TEMPERATURE_CONFIGURATION_H */
