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
 * MODULE:             Power Profile Cluster
 *
 * COMPONENT:          PowerProfile.h
 *
 * DESCRIPTION:        Header for PowerProfile Cluster
 *
 *****************************************************************************/

#ifndef POWERPROFILE_H
#define POWERPROFILE_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_PP                                           0x001a

/****************************************************************************/
/*             PP Control Cluster - Optional Attributes             */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes to the Power Profile cluster.                                 */
/****************************************************************************/
/* Optional Global attribute reporting status */
//#ifdef CLD_PP_ATTR_ATTRIBUTE_REPORTING_STATUS
/****************************************************************************/
/*             PP Control Cluster - Optional Commands             */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* commands to the Power Profile cluster.                                 */
/****************************************************************************/

/* Get power profile price*/
//#define CLD_PP_CMD_GET_POWER_PROFILE_PRICE

/* Get overall schedule price Command */
//#define CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE

/*  Get Power Profile Price Extended Command */
//#define CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#define CLD_PP_NUM_OF_POWER_PROFILES        1

#ifndef CLD_PP_NUM_OF_ENERGY_PHASES
    #define CLD_PP_NUM_OF_ENERGY_PHASES     3
#endif

#ifndef CLD_PP_CLUSTER_REVISION
    #define CLD_PP_CLUSTER_REVISION                     1
#endif   

#ifndef CLD_PP_FEATURE_MAP
    #define CLD_PP_FEATURE_MAP                          0
#endif   



/* Bitmap for Schedule Mode attribute */
#define CLD_PP_ATTR_SCHEDULE_MODE_CHEAPEST          (1>>0)
#define CLD_PP_ATTR_SCHEDULE_MODE_GREENEST          (0>>0)
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/* Power Profile Server Received Command */
typedef enum 
{
 E_CLD_PP_CMD_POWER_PROFILE_REQ        = 0x00,                                    /* Mandatory */
 E_CLD_PP_CMD_POWER_PROFILE_STATE_REQ,                                            /* Mandatory */
 E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_RSP,                                        /* Mandatory */
 E_CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE_RSP,                                 /* Mandatory */
 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_NOTIFICATION,                                /* Mandatory */
 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_RSP,                                         /* Mandatory */
 E_CLD_PP_CMD_POWER_PROFILE_SCHEDULE_CONSTRAINTS_REQ,                          /* Mandatory */
 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_REQ,                                   /* Mandatory */
 E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED_RSP                               /* Mandatory */
 } teCLD_PP_ServerReceivedCommandID;

 /* Power Profile Server Generated Command */
 typedef enum 
{
 E_CLD_PP_CMD_POWER_PROFILE_NOTIFICATION       = 0x00,                              /* Mandatory */
 E_CLD_PP_CMD_POWER_PROFILE_RSP,                                                  /* Mandatory */
 E_CLD_PP_CMD_POWER_PROFILE_STATE_RSP,                                              /* Mandatory */
 E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE,                                              /* Optional */
 E_CLD_PP_CMD_POWER_PROFILE_STATE_NOTIFICATION,                                     /* Mandatory */
 E_CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE,                                           /* Optional */
 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_REQ,                                           /* Mandatory */
 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_RSP,                                     /* Mandatory */
 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_NOTIFICATION,                            /* Mandatory */
 E_CLD_PP_CMD_GET_POWER_PROFILE_SCHEDULE_CONSTRAINTS_NOTIFICATION,                  /* Mandatory */
 E_CLD_PP_CMD_GET_POWER_PROFILE_SCHEDULE_CONSTRAINTS_RSP,                           /* Mandatory */
 E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED                                      /* Optional */
 } teCLD_PP_ServerGeneratedCommandID;



typedef enum 
{
    /* Power Profile attribute set attribute ID's (A1) */
    E_CLD_PP_ATTR_ID_TOTAL_PROFILE_NUM         = 0x0000,        /*0. Mandatory */
    E_CLD_PP_ATTR_ID_MULTIPLE_SCHEDULING,                       /* 1.Mandatory */
    E_CLD_PP_ATTR_ID_ENERGY_FORMATTING,                         /* 2.Mandatory */
    E_CLD_PP_ATTR_ID_ENERGY_REMOTE,                             /* 3. Mandatory */
    E_CLD_PP_ATTR_ID_SCHEDULE_MODE,                             /* 4. Mandatory */
} teCLD_PP_Cluster_AttrID;

/* Power profile State */
typedef enum 
{
    E_CLD_PP_STATE_IDLE                             = 0x00,
    E_CLD_PP_STATE_PROGRAMMED,
    E_CLD_PP_STATE_RUNNING                          = 0x03,
    E_CLD_PP_STATE_PAUSED,
    E_CLD_PP_STATE_WAITING_TO_START,
    E_CLD_PP_STATE_WAITING_PAUSED,
    E_CLD_PP_STATE_ENDED
} teCLD_PP_PowerProfileState;


/* Power Profile Cluster */
typedef struct
{
#ifdef PP_SERVER
    zuint8              u8TotalProfileNum;                     
    zbool               bMultipleScheduling;                     
    zbmap8              u8EnergyFormatting;     
    zbool               bEnergyRemote;                                      
    zbmap8              u8ScheduleMode;
#ifdef CLD_PP_ATTR_ATTRIBUTE_REPORTING_STATUS
    zuint8              u8AttributeReportingStatus;
#endif    
#endif
    zbmap32             u32FeatureMap;      
    zuint16             u16ClusterRevision;    
} tsCLD_PP; 
 
/* Power Profile Request Payload */
typedef struct
{
    zuint8               u8PowerProfileId;
}tsCLD_PP_PowerProfileReqPayload;


/* get Power Profile Price Response Payload */
typedef struct
{
     zuint8               u8PowerProfileId;
     zuint16              u16Currency;
     zuint32              u32Price;
     zuint8               u8PriceTrailingDigits;
}tsCLD_PP_GetPowerProfilePriceRspPayload;

/* Get Overall Schedule Price response Payload */
typedef struct
{
     zuint16            u16Currency;
     zuint32            u32Price;
     zuint8             u8PriceTrailingDigits;
}tsCLD_PP_GetOverallSchedulePriceRspPayload;

/* Energy Phases Schedule Response/Notification */
typedef struct
{
     zuint8               u8EnergyPhaseId;
     zuint16              u16ScheduleTime;
}tsCLD_PP_EnergyPhaseDelay;

typedef struct
{
     zuint8                         u8PowerProfileId;
     zuint8                         u8NumOfScheduledPhases;
     tsCLD_PP_EnergyPhaseDelay      *psEnergyPhaseDelay;
}tsCLD_PP_EnergyPhasesSchedulePayload;

/* Power Profile Response/Notification Payload */
typedef struct
{
     zuint8                 u8EnergyPhaseId;
     zuint8                 u8MacroPhaseId;
     zuint16                u16ExpectedDuration;
     zuint16                u16PeakPower;
     zuint16                u16Energy;
     zuint16                u16MaxActivationDelay;  
}tsCLD_PP_EnergyPhaseInfo;

typedef struct
{
     zuint8                         u8TotalProfileNum;
     zuint8                         u8PowerProfileId;
     zuint8                         u8NumOfTransferredPhases;
     tsCLD_PP_EnergyPhaseInfo       *psEnergyPhaseInfo;
}tsCLD_PP_PowerProfilePayload;

/* Power Profile State Response/Notification Payload */

typedef struct
{
     zuint8               u8PowerProfileId;
     zuint8               u8EnergyPhaseId;
     zbool                bPowerProfileRemoteControl;
     zenum8               u8PowerProfileState;
} tsCLD_PP_PowerProfileRecord;

typedef struct
{
     zuint8                         u8PowerProfileCount;
     tsCLD_PP_PowerProfileRecord    *psPowerProfileRecord;
}tsCLD_PP_PowerProfileStatePayload;

/* Energy Power Profile Schedule Constraints Response/Notification */

typedef struct
{
    zuint8                      u8PowerProfileId;
    zuint16                     u16StartAfter;
    zuint16                     u16StopBefore;
}tsCLD_PP_PowerProfileScheduleConstraintsPayload;

/* Get Power profile Price Extended */
typedef struct
{
     zbmap8            u8Options;
     zuint8            u8PowerProfileId;
     zuint16           u16PowerProfileStartTime;
}tsCLD_PP_GetPowerProfilePriceExtendedPayload;

/* Power Profile Entry */
typedef struct
{
    zuint8                       u8PowerProfileId;
    zuint8                       u8NumOfTransferredEnergyPhases;
    zuint8                       u8NumOfScheduledEnergyPhases;
    zuint8                       u8ActiveEnergyPhaseId;
    tsCLD_PP_EnergyPhaseDelay    asEnergyPhaseDelay[CLD_PP_NUM_OF_ENERGY_PHASES];
    tsCLD_PP_EnergyPhaseInfo     asEnergyPhaseInfo[CLD_PP_NUM_OF_ENERGY_PHASES];
    zbool                        bPowerProfileRemoteControl;
    zenum8                       u8PowerProfileState;
    zuint16                      u16StartAfter;
    zuint16                      u16StopBefore;
} tsCLD_PPEntry;

/* Definition of Power Profile Callback Event Structure */
typedef struct
{
    uint8                           u8CommandId;
#ifdef PP_CLIENT
    bool_t                            bIsInfoAvailable;
#endif    
    union
    {
        tsCLD_PP_PowerProfileReqPayload                         *psPowerProfileReqPayload;
        tsCLD_PP_GetPowerProfilePriceExtendedPayload            *psGetPowerProfilePriceExtendedPayload;
    } uReqMessage;
    union
    {
        tsCLD_PP_GetPowerProfilePriceRspPayload                 *psGetPowerProfilePriceRspPayload;
        tsCLD_PP_GetOverallSchedulePriceRspPayload              *psGetOverallSchedulePriceRspPayload;
        tsCLD_PP_EnergyPhasesSchedulePayload                    *psEnergyPhasesSchedulePayload;
        tsCLD_PP_PowerProfileScheduleConstraintsPayload         *psPowerProfileScheduleConstraintsPayload;
        tsCLD_PP_PowerProfilePayload                            *psPowerProfilePayload;
        tsCLD_PP_PowerProfileStatePayload                       *psPowerProfileStatePayload;
    }uRespMessage;
} tsCLD_PPCallBackMessage;


/* Custom data structure */
typedef struct
{
#if (defined CLD_PP) && (defined PP_SERVER) 
	bool_t                                    bOverrideRunning;
    uint8                                   u8ActSchPhaseIndex;
    tsCLD_PPEntry                           asPowerProfileEntry[CLD_PP_NUM_OF_POWER_PROFILES];
#endif
    tsZCL_ReceiveEventAddress               sReceiveEventAddress;
    tsZCL_CallBackEvent                     sCustomCallBackEvent;
    tsCLD_PPCallBackMessage                 sCallBackMessage;
} tsCLD_PPCustomDataStructure;


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_PPCreatePowerProfile(
                tsZCL_ClusterInstance                  *psClusterInstance,
                bool_t                                 bIsServer,
                tsZCL_ClusterDefinition                *psClusterDefinition,
                void                                   *pvEndPointSharedStructPtr,
                uint8                                  *pu8AttributeControlBits,
                tsCLD_PPCustomDataStructure            *psCustomDataStructure);

PUBLIC teZCL_Status eCLD_PPEnergyPhasesScheduleSend (
                    uint8                                               u8SourceEndPointId,
                    uint8                                               u8DestinationEndPointId,
                    tsZCL_Address                                       *psDestinationAddress,
                    uint8                                               *pu8TransactionSequenceNumber,
                    teCLD_PP_ServerReceivedCommandID                    eCommandId,
                    bool_t                                                bServer,
                    tsCLD_PP_EnergyPhasesSchedulePayload                *psPayload);
                    
PUBLIC teZCL_Status eCLD_PPPowerProfileRequestSend (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                                        bool_t                                                        bServer,
                                        tsCLD_PP_PowerProfileReqPayload                         *psPayload);    
                    
#ifdef PP_CLIENT   
PUBLIC teZCL_Status eCLD_PPPowerProfileStateReqSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber);     
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileReqSend (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileReqPayload                         *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileReqSend(                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileReqPayload                 *psPayload)
{
    return eCLD_PPPowerProfileRequestSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                         E_CLD_PP_CMD_POWER_PROFILE_REQ,
                                                                  FALSE,
                                                              psPayload);
}
                    
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileScheduleConstraintsReqSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileReqPayload                         *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileScheduleConstraintsReqSend (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileReqPayload                 *psPayload)
{
    return eCLD_PPPowerProfileRequestSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                    E_CLD_PP_CMD_POWER_PROFILE_SCHEDULE_CONSTRAINTS_REQ,
                                                                  FALSE,
                                                              psPayload);
}
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleStateReqSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileReqPayload                         *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleStateReqSend (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileReqPayload                     *psPayload)
{
    return eCLD_PPPowerProfileRequestSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                          E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_REQ,
                                                                  FALSE,
                                                              psPayload);
}           
                    
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleNotificationSend   (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_EnergyPhasesSchedulePayload                        *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleNotificationSend  (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_EnergyPhasesSchedulePayload                *psPayload)
{
    return eCLD_PPEnergyPhasesScheduleSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                       E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_NOTIFICATION,
                                                                  FALSE,
                                                              psPayload);
}                   
#endif

#ifdef PP_SERVER 

PUBLIC teZCL_Status eCLD_PPSchedule(void);

PUBLIC teZCL_CommandStatus eCLD_PPSetPowerProfileState(
                uint8                           u8SourceEndPointId,
                uint8                           u8PowerProfileId,
                teCLD_PP_PowerProfileState      ePowerProfileState);
    
PUBLIC teZCL_Status eCLD_PPAddPowerProfileEntry(
                uint8                           u8SourceEndPointId,
                tsCLD_PPEntry                   *psPowerProfileEntry);

PUBLIC teZCL_Status eCLD_PPRemovePowerProfileEntry(
                uint8                           u8SourceEndPointId,
                uint8                           u8PowerProfileId);
PUBLIC teZCL_Status eCLD_PPGetPowerProfileEntry(
                uint8                           u8SourceEndPointId,
                uint8                           u8PowerProfileId,
                tsCLD_PPEntry                   **psPowerProfileEntry);                 
PUBLIC teZCL_Status eCLD_PPPowerProfileSend(
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                    tsCLD_PP_PowerProfilePayload                                *psPayload);
PUBLIC teZCL_Status eCLD_PPPowerProfileStateSend(
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                    tsCLD_PP_PowerProfileStatePayload                           *psPayload);
PUBLIC teZCL_Status eCLD_PPPowerProfileScheduleConstraintsSend (
                    uint8                                                       u8SourceEndPointId,
                    uint8                                                       u8DestinationEndPointId,
                    tsZCL_Address                                               *psDestinationAddress,
                    uint8                                                       *pu8TransactionSequenceNumber,
                    teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                    tsCLD_PP_PowerProfileScheduleConstraintsPayload             *psPayload);                    
#ifdef CLD_PP_CMD_GET_POWER_PROFILE_PRICE                                       
PUBLIC INLINE teZCL_Status eCLD_PPGetPowerProfilePriceSend   (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileReqPayload                         *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPGetPowerProfilePriceSend  (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileReqPayload                 *psPayload)
{
    return eCLD_PPPowerProfileRequestSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                    E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE,
                                                                    TRUE,
                                                              psPayload);
}   
#endif
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleReqSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileReqPayload                         *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleReqSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileReqPayload                 *psPayload)
{
    return eCLD_PPPowerProfileRequestSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_REQ,
                                                                   TRUE,
                                                              psPayload);
}
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileNotificationSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfilePayload                                *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileNotificationSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfilePayload                        *psPayload)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eCLD_PPPowerProfileSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                                E_CLD_PP_CMD_POWER_PROFILE_NOTIFICATION,
                                                              psPayload);
}
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileStateNotificationSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileStatePayload                               *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileStateNotificationSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileStatePayload                   *psPayload)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eCLD_PPPowerProfileStateSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                          E_CLD_PP_CMD_POWER_PROFILE_STATE_NOTIFICATION,
                                                              psPayload);
}
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleStateNotificationSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_EnergyPhasesSchedulePayload                        *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPEnergyPhasesScheduleStateNotificationSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_EnergyPhasesSchedulePayload                *psPayload)
{
    return eCLD_PPEnergyPhasesScheduleSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
                 E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_STATE_NOTIFICATION,
                                                                   TRUE,
                                                              psPayload);
}
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileScheduleConstraintsNotificationSend    (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_PowerProfileScheduleConstraintsPayload             *psPayload) ALWAYS_INLINE;          
PUBLIC INLINE teZCL_Status eCLD_PPPowerProfileScheduleConstraintsNotificationSend   (                
                uint8                                               u8SourceEndPointId,
                uint8                                               u8DestinationEndPointId,
                tsZCL_Address                                       *psDestinationAddress,
                uint8                                               *pu8TransactionSequenceNumber,
                tsCLD_PP_PowerProfileScheduleConstraintsPayload     *psPayload)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eCLD_PPPowerProfileScheduleConstraintsSend(
                                                     u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                   psDestinationAddress,
                                           pu8TransactionSequenceNumber,
        E_CLD_PP_CMD_GET_POWER_PROFILE_SCHEDULE_CONSTRAINTS_NOTIFICATION,
                                                              psPayload);
}
#ifdef CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED
PUBLIC teZCL_Status eCLD_PPGetPowerProfilePriceExtendedSend  (
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber,
                tsCLD_PP_GetPowerProfilePriceExtendedPayload                *psPayload);
#endif
#ifdef CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE
PUBLIC teZCL_Status eCLD_PPGetOverallSchedulePriceSend  (
                uint8                                                       u8SourceEndPointId,
                uint8                                                       u8DestinationEndPointId,
                tsZCL_Address                                               *psDestinationAddress,
                uint8                                                       *pu8TransactionSequenceNumber);
#endif              
#endif
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_PPClusterAttrDefs[];
extern tsZCL_ClusterDefinition sCLD_PP;
extern uint8 au8PPAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* POWERPROFILE_H */
