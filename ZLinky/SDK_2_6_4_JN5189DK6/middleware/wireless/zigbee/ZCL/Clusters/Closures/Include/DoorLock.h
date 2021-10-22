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
 * MODULE:             Door Lock Cluster
 *
 * COMPONENT:          DoorLock.h
 *
 * DESCRIPTION:        Header for DoorLock Cluster
 *
 *****************************************************************************/

#ifndef DOORLOCK_H
#define DOORLOCK_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define CLOSURE_CLUSTER_ID_DOOR_LOCK                        0x0101

/****************************************************************************/
/*             Door_Lock Control Cluster - Optional Attributes             */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the door lock cluster.                                 */
/****************************************************************************/

/* Door state attribute */
//#define CLD_DOOR_LOCK_ATTR_DOOR_STATE

/* number of door open evenrs attribute */
//#define CLD_DOOR_LOCK_ATTR_NUMBER_OF_DOOR_OPEN_EVENTS

/* number of door closed events attribute */
//#define CLD_DOOR_LOCK_ATTR_NUMBER_OF_DOOR_CLOSED_EVENTS

/* Number of minutes door has been open since this last time it transitioned from close to open */
//#define CLD_DOOR_LOCK_ATTR_NUMBER_OF_MINUTES_DOOR_OPENED

/* Zigbee Security Level*/
//#define CLD_DOOR_LOCK_ZIGBEE_SECUTRITY_LEVEL

#ifndef CLD_DOOR_LOCK_CLUSTER_REVISION
    #define CLD_DOOR_LOCK_CLUSTER_REVISION                         1
#endif 

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Door Lock Command - Payload */
typedef enum 
{
    E_CLD_DOOR_LOCK_CMD_LOCK                      = 0x00,        /* Mandatory */
    E_CLD_DOOR_LOCK_CMD_UNLOCK,                                  /* Mandatory */
} teCLD_DoorLock_CommandID;


typedef enum 
{
    /* Door Lock attribute set attribute ID's (A1) */
    E_CLD_DOOR_LOCK_ATTR_ID_LOCK_STATE                = 0x0000,             /* 0.Mandatory */
    E_CLD_DOOR_LOCK_ATTR_ID_LOCK_TYPE,                                      /* 1.Mandatory */
    E_CLD_DOOR_LOCK_ATTR_ID_ACTUATOR_ENABLED,                               /* 2.Mandatory */
    E_CLD_DOOR_LOCK_ATTR_ID_DOOR_STATE,                                     /* 3.Optional */
    E_CLD_DOOR_LOCK_ATTR_ID_NUMBER_OF_DOOR_OPEN_EVENTS,                     /* 4.Optional */
    E_CLD_DOOR_LOCK_ATTR_ID_NUMBER_OF_DOOR_CLOSED_EVENTS,                   /* 5.Optional */
    E_CLD_DOOR_LOCK_ATTR_ID_NUMBER_OF_MINUTES_DOOR_OPENED,                  /* 6.Optional */
    E_CLD_DOOR_LOCK_ATTR_ID_ZIGBEE_SECURITY_LEVEL     = 0x0034,
} teCLD_DoorLock_Cluster_AttrID; 

/* Lock State */
typedef enum 
{
    E_CLD_DOORLOCK_LOCK_STATE_NOT_FULLY_LOCKED  = 0x00,
    E_CLD_DOORLOCK_LOCK_STATE_LOCKED,
    E_CLD_DOORLOCK_LOCK_STATE_UNLOCKED,
    E_CLD_DOORLOCK_LOCK_STATE_UNDEFINED         = 0xFF
} teCLD_DoorLock_LockState;

typedef enum 
{
    E_CLD_DOORLOCK_LOCK_TYPE_DEAD_BOLT  = 0x00,
    E_CLD_DOORLOCK_LOCK_TYPE_MAGNETIC,
    E_CLD_DOORLOCK_LOCK_TYPE_OTHER,
    E_CLD_DOORLOCK_LOCK_TYPE_MORTISE,
    E_CLD_DOORLOCK_LOCK_TYPE_RIM,
    E_CLD_DOORLOCK_LOCK_TYPE_LATCH_BOLT,
    E_CLD_DOORLOCK_LOCK_TYPE_CYLINDRICAL_LOCK,
    E_CLD_DOORLOCK_LOCK_TYPE_TUBULAR_LOCK,
    E_CLD_DOORLOCK_LOCK_TYPE_INTERCONNECTED_LOCK,
    E_CLD_DOORLOCK_LOCK_TYPE_DEAD_LATCH,
    E_CLD_DOORLOCK_LOCK_TYPE_DOOR_FURNITURE
} teCLD_DoorLock_LockType;

#ifdef CLD_DOOR_LOCK_ATTR_DOOR_STATE
typedef enum 
{
    E_CLD_DOORLOCK_DOOR_STATE_OPEN  = 0x00,
    E_CLD_DOORLOCK_DOOR_STATE_CLOSED,
    E_CLD_DOORLOCK_DOOR_STATE_ERROR_JAMMED,
    E_CLD_DOORLOCK_DOOR_STATE_ERROR_FORCED_OPEN,
    E_CLD_DOORLOCK_DOOR_STATE_ERROR_UNSPECIFIED,
    E_CLD_DOORLOCK_DOOR_STATE_UNDEFINED = 0xFF
} teCLD_DoorLock_DoorState;
#endif

/* Door Lock Cluster */
typedef struct
{
#ifdef DOOR_LOCK_SERVER
    zenum8                   eLockState;
    zenum8                   eLockType;
    zbool                    bActuatorEnabled;
    
#ifdef CLD_DOOR_LOCK_ATTR_DOOR_STATE
    zenum8                   eDoorState;
#endif

#ifdef CLD_DOOR_LOCK_ATTR_NUMBER_OF_DOOR_OPEN_EVENTS
    zuint32                   u32NumberOfDoorOpenEvent;
#endif

#ifdef CLD_DOOR_LOCK_ATTR_NUMBER_OF_DOOR_CLOSED_EVENTS
    zuint32                   u32NumberOfDoorClosedEvent;
#endif

#ifdef CLD_DOOR_LOCK_ATTR_NUMBER_OF_MINUTES_DOOR_OPENED
    zuint16                   u16NumberOfMinutesDoorOpened;
#endif

#ifdef CLD_DOOR_LOCK_ZIGBEE_SECUTRITY_LEVEL
    zuint8                     u8ZigbeeSecurityLevel;
#endif

#ifdef CLD_DOOR_LOCK_ATTRIBUTE_REPORTING_STATUS
    zuint8                     u8AttributeReportingStatus;
#endif
#endif
    zuint16                    u16ClusterRevision;
} tsCLD_DoorLock;


typedef struct
{
    zenum8                  eStatus;
}tsCLD_DoorLock_LockUnlockResponsePayload;

/* Definition of Door Lock Callback Event Structure */
typedef struct
{
    uint8                           u8CommandId;
        union
    {
        tsCLD_DoorLock_LockUnlockResponsePayload              *psLockUnlockResponsePayload;
    } uMessage;
} tsCLD_DoorLockCallBackMessage;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_DoorLockCreateDoorLock(
                tsZCL_ClusterInstance                  *psClusterInstance,
                bool_t                                 bIsServer,
                tsZCL_ClusterDefinition                *psClusterDefinition,
                void                                   *pvEndPointSharedStructPtr,
                uint8                                  *pu8AttributeControlBits);

#ifdef CLD_DOOR_LOCK_ZIGBEE_SECUTRITY_LEVEL
PUBLIC teZCL_Status eCLD_DoorLockSetSecurityLevel(
                    uint8 u8SourceEndPointId,
					bool_t bServer,
                    uint8 u8SecurityLevel);                
#endif

#ifdef DOOR_LOCK_CLIENT                  
PUBLIC teZCL_Status eCLD_DoorLockCommandLockUnlockRequestSend(
                    uint8                              u8SourceEndPointId,
                    uint8                           u8DestinationEndPointId,
                    tsZCL_Address                   *psDestinationAddress,
                    uint8                           *pu8TransactionSequenceNumber,
                    teCLD_DoorLock_CommandID          eCommand);
#endif

#ifdef DOOR_LOCK_SERVER 
PUBLIC teZCL_Status eCLD_DoorLockSetLockState(
                    uint8                       u8SourceEndPointId,
                    teCLD_DoorLock_LockState    eLock);
PUBLIC teZCL_Status eCLD_DoorLockGetLockState(
                    uint8                       u8SourceEndPointId,
                    teCLD_DoorLock_LockState    *eLock);
#endif
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern tsZCL_ClusterDefinition sCLD_DoorLock;
extern const tsZCL_AttributeDefinition asCLD_DoorLockClusterAttributeDefinitions[];
extern uint8 au8DoorLockAttributeControlBits[];
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* DOORLOCK_H */
