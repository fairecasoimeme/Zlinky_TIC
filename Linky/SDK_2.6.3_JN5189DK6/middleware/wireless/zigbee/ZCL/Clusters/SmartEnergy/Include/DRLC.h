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
 * MODULE:              Zigbee Demand Response And Load Control Cluster
 *
 * COMPONENT:          Smart Energy Clusters
 *
 * DESCRIPTION: The API for the Jennic Zigbee Demand Response And Load Control Cluster
 *
 *****************************************************************************/


#ifndef  DRLC_H_INCLUDED
#define  DRLC_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "dlist.h"
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef SE_CLUSTER_ID_DEMAND_RESPONSE_AND_LOAD_CONTROL
#define SE_CLUSTER_ID_DEMAND_RESPONSE_AND_LOAD_CONTROL           0x0701
#endif

#define SE_DRLC_ISSUER_EVENT_ID                                 (0x0000)

#ifndef SE_DRLC_NUMBER_OF_CLIENT_LOAD_CONTROL_ENTRIES
#define SE_DRLC_NUMBER_OF_CLIENT_LOAD_CONTROL_ENTRIES           (3)
#endif

#ifndef SE_DRLC_NUMBER_OF_SERVER_LOAD_CONTROL_ENTRIES
#define SE_DRLC_NUMBER_OF_SERVER_LOAD_CONTROL_ENTRIES           (3)
#endif

#define SE_DRLC_LOAD_CONTROL_EVENT                              (0x00)
#define SE_DRLC_LOAD_CONTROL_EVENT_CANCEL                       (0x01)
#define SE_DRLC_LOAD_CONTROL_EVENT_CANCEL_ALL                   (0x02)

#define SE_DRLC_REPORT_EVENT_STATUS                             (0x00)
#define SE_DRLC_GET_SCHEDULED_EVENTS                            (0x01)

#define SE_DRLC_CONTROL_RANDOMISATION_START_TIME_MASK           (0x01)
#define SE_DRLC_CONTROL_RANDOMISATION_STOP_TIME_MASK            (0x02)

#define SE_DRLC_NO_SIGNATURE                                    (0x00)
#define SE_DRLC_SIGNATURE_TYPE_ECDSA                            (0x01)

#define E_SE_DRLC_MAX_EVENT_DURATION                            (1440) // Duration max is one day
#define CLD_DRLC_NUMBER_OF_MANDATORY_ATTRIBUTE                  (4)  //u8UtilityEnrolmentGroup, u8StartRandomizeMinutes, u8StopRandomizeMinutes,u16DeviceClassValue

#define CLD_DRLC_NUMBER_OF_OPTIONAL_ATTRIBUTE                   (0)

#define CLD_DRLC_MAX_NUMBER_OF_ATTRIBUTE                         \
    ((CLD_DRLC_NUMBER_OF_OPTIONAL_ATTRIBUTE)                +    \
    (CLD_DRLC_NUMBER_OF_MANDATORY_ATTRIBUTE))


/*    DRLC enums */
typedef enum 
{
    E_CLD_DRLC_UTILITY_ENROLMENT_GROUP = 0x0000,
    E_CLD_DRLC_START_RANDOMIZATION_MINUTES,
    E_CLD_DRLC_DURATION_RANDOMIZATION_MINUTES,
    E_CLD_DRLC_DEVICE_CLASS_VALUE
} teCLD_SM_DRLCAttributeID;

// Enum made  for SE 1.7
typedef enum 
{
    E_SE_DRLC_HVAC_COMPRESSOR_OR_FURNACE_BIT = 0x00,
    E_SE_DRLC_STRIP_BASEBOARD_HEATERS_BIT,
    E_SE_DRLC_WATER_HEATER_BIT,
    E_SE_DRLC_POOL_PUMP_SPA_JACUZZI_BIT,
    E_SE_DRLC_SMART_APPLIANCES_BIT,
    E_SE_DRLC_IRRIGATION_PUMP_BIT,
    E_SE_DRLC_MANAGED_COMMERCIAL_AND_INDUSTRIAL_LOADS_BIT,
    E_SE_DRLC_SIMPLE_MISC_LOADS_BIT,
    E_SE_DRLC_EXTERIOR_LIGHTING_BIT,
    E_SE_DRLC_INTERIOR_LIGHTING_BIT,
    E_SE_DRLC_ELECTRIC_VEHICLE_BIT,
    E_SE_DRLC_GENERATION_SYSTEMS_BIT,
    E_SE_DRLC_DEVICE_CLASS_FIRST_RESERVED_BIT
} teSE_DRLCDeviceClassFieldBitmap;

#define E_SE_DRLC_HVAC_COMPRESSOR_OR_FURNACE_MASK                       (1 << E_SE_DRLC_HVAC_COMPRESSOR_OR_FURNACE_BIT)
#define E_SE_DRLC_HVAC_COMPRESSOR_OR_FURNACE_MASK                       (1 << E_SE_DRLC_HVAC_COMPRESSOR_OR_FURNACE_BIT)
#define E_SE_DRLC_STRIP_BASEBOARD_HEATERS_MASK                          (1 << E_SE_DRLC_STRIP_BASEBOARD_HEATERS_BIT)
#define E_SE_DRLC_WATER_HEATER_MASK                                     (1 << E_SE_DRLC_WATER_HEATER_BIT)
#define E_SE_DRLC_POOL_PUMP_SPA_JACUZZI_MASK                            (1 << E_SE_DRLC_POOL_PUMP_SPA_JACUZZI_BIT)
#define E_SE_DRLC_SMART_APPLIANCES_MASK                                 (1 << E_SE_DRLC_SMART_APPLIANCES_BIT)
#define E_SE_DRLC_IRRIGATION_PUMP_MASK                                  (1 << E_SE_DRLC_IRRIGATION_PUMP_BIT)
#define E_SE_DRLC_MANAGED_COMMERCIAL_AND_INDUSTRIAL_LOADS_MASK          (1 << E_SE_DRLC_MANAGED_COMMERCIAL_AND_INDUSTRIAL_LOADS_BIT)
#define E_SE_DRLC_SIMPLE_MISC_LOADS_MASK                                (1 << E_SE_DRLC_SIMPLE_MISC_LOADS_BIT)
#define E_SE_DRLC_EXTERIOR_LIGHTING_MASK                                (1 << E_SE_DRLC_EXTERIOR_LIGHTING_BIT)
#define E_SE_DRLC_INTERIOR_LIGHTING_MASK                                (1 << E_SE_DRLC_INTERIOR_LIGHTING_BIT)
#define E_SE_DRLC_ELECTRIC_VEHICLE_MASK                                 (1 << E_SE_DRLC_ELECTRIC_VEHICLE_BIT)
#define E_SE_DRLC_GENERATION_SYSTEMS_MASK                               (1 << E_SE_DRLC_GENERATION_SYSTEMS_BIT)
#define E_SE_DRLC_DEVICE_CLASS_RESERVED_BITS_MASK                       (0xffff << E_SE_DRLC_DEVICE_CLASS_FIRST_RESERVED_BIT)

// Enum made pack for SE 1.7
typedef enum 
{
    E_SE_DRLC_RESERVED_0_CRITICALITY = 0x00,
    E_SE_DRLC_GREEN_CRITICALITY,
    E_SE_DRLC_VOLUNTARY_1_CRITICALITY,
    E_SE_DRLC_VOLUNTARY_2_CRITICALITY,
    E_SE_DRLC_VOLUNTARY_3_CRITICALITY,
    E_SE_DRLC_VOLUNTARY_4_CRITICALITY,
    E_SE_DRLC_VOLUNTARY_5_CRITICALITY,
    E_SE_DRLC_EMERGENCY_CRITICALITY,
    E_SE_DRLC_PLANNED_OUTAGE_CRITICALITY,
    E_SE_DRLC_SERVICE_DISCONNECT_CRITICALITY,
    E_SE_DRLC_UTILITY_DEFINED_1_CRITICALITY,
    E_SE_DRLC_UTILITY_DEFINED_2_CRITICALITY,
    E_SE_DRLC_UTILITY_DEFINED_3_CRITICALITY,
    E_SE_DRLC_UTILITY_DEFINED_4_CRITICALITY,
    E_SE_DRLC_UTILITY_DEFINED_5_CRITICALITY,
    E_SE_DRLC_UTILITY_DEFINED_6_CRITICALITY,
    E_SE_DRLC_FIRST_RESERVED_CRITICALITY

} teSE_DRLCCriticalityLevels;

typedef enum 
{
    E_SE_DRLC_DUPLICATE_EXISTS  = 0x80,
    E_SE_DRLC_EVENT_LATE,
    E_SE_DRLC_EVENT_NOT_YET_ACTIVE,
    E_SE_DRLC_EVENT_OLD,
    E_SE_DRLC_NOT_FOUND,
    E_SE_DRLC_EVENT_NOT_FOUND,
    E_SE_DRLC_EVENT_IGNORED,
    E_SE_DRLC_CANCEL_DEFERRED,
    E_SE_DRLC_BAD_DEVICE_CLASS,
    E_SE_DRLC_BAD_CRITICALITY_LEVEL,
    E_SE_DRLC_DURATION_TOO_LONG,
    E_SE_DRLC_ENUM_END
} teSE_DRLCStatus;

typedef enum 
{
    E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_RECEIVED =0x01,
    E_SE_DRLC_EVENT_STARTED,
    E_SE_DRLC_EVENT_COMPLETED,
    E_SE_DRLC_USER_CHOSEN_OPT_OUT,
    E_SE_DRLC_USER_CHOSEN_OPT_IN,
    E_SE_DRLC_EVENT_HAS_BEEN_CANCELLED,
    E_SE_DRLC_EVENT_HAS_BEEN_SUPERSEDED,
    E_SE_DRLC_EVENT_PARTIALLY_COMPLETED_WITH_USER_OPT_OUT,
    E_SE_DRLC_EVENT_PARTIALLY_COMPLETED_WITH_USER_OPT_IN,
    E_SE_DRLC_EVENT_COMPLETED_NO_USER_PARTICIPATION,
    E_SE_DRLC_REJECTED_INVALID_CANCEL_COMMAND_DEFAULT =0xF8,
    E_SE_DRLC_REJECTED_INVALID_CANCEL_COMMAND_INVALID_EFFECTIVE_TIME,
    E_SE_DRLC_REJECTED_EVENT_RECEIVED_AFTER_IT_HAD_EXPIRED =0xFB,
    E_SE_DRLC_REJECTED_INVALID_CANCEL_COMMAND_UNDEFINED_EVENT=0xFD,
    E_SE_DRLC_LOAD_CONTROL_EVENT_COMMAND_REJECTED,
    E_SE_DRLC_EVENT_STATUS_ENUM_END
} teSE_DRLCEventStatus;

typedef enum 
{
    E_SE_DRLC_EVENT_API =0x00,
    E_SE_DRLC_EVENT_COMMAND,
    E_SE_DRLC_EVENT_ACTIVE,
    E_SE_DRLC_EVENT_EXPIRED,
    E_SE_DRLC_EVENT_CANCELLED,
    E_SE_DRLC_EVENT_ENUM_END,

} teSE_DRLCCallBackEventType;

typedef enum 
{
    E_SE_DRLC_EVENT_LIST_SCHEDULED =0x00,
    E_SE_DRLC_EVENT_LIST_ACTIVE,
    E_SE_DRLC_EVENT_LIST_CANCELLED,
    E_SE_DRLC_EVENT_LIST_DEALLOCATED,
    E_SE_DRLC_EVENT_LIST_NONE

} teSE_DRLCEventList;

typedef enum 
{
    E_SE_DRLC_CANCEL_CONTROL_IMMEDIATE =0x00,
    E_SE_DRLC_CANCEL_CONTROL_USE_RANDOMISATION =0x10
} teSE_DRLCCancelControl;

typedef enum 
{
    E_SE_DRLC_EVENT_USER_OPT_IN =0x00,
    E_SE_DRLC_EVENT_USER_OPT_OUT
} teSE_DRLCUserEventOption;

typedef enum 
{
    E_SE_DRLC_CRITICALITY_LEVEL_APPLIED =0x00,
    E_SE_DRLC_COOLING_TEMPERATURE_SET_POINT_APPLIED,
    E_SE_DRLC_HEATING_TEMPERATURE_SET_POINT_APPLIED,
    E_SE_DRLC_AVERAGE_LOAD_ADJUSTMENT_PERCENTAGE_APPLIED,
    E_SE_DRLC_DUTY_CYCLE_APPLIED,
    E_SE_DRLC_USER_EVENT_ENUM_END,
} teSE_DRLCUserEventSet;

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

// Structure Reordered for SE 1.7
typedef struct {
    uint8               u8UtilityEnrolmentGroup;
    uint8               u8CriticalityLevel;
    uint8               u8CoolingTemperatureOffset;
    uint8               u8HeatingTemperatureOffset;
    uint8               u8AverageLoadAdjustmentSetPoint;
    uint8               u8DutyCycle;
    uint8               u8EventControl;
    uint16              u16DeviceClass;
    uint16              u16DurationInMinutes;
    uint16              u16CoolingTemperatureSetPoint;
    uint16              u16HeatingTemperatureSetPoint;
    uint32              u32IssuerId;
    uint32              u32StartTime;

} tsSE_DRLCLoadControlEvent;

typedef struct {
    teSE_DRLCCancelControl u8CancelControl;
} tsSE_DRLCCancelLoadControlAllEvent;

typedef struct {
    uint32                  u32IssuerId;
    uint16                  u16DeviceClass;
    uint8                   u8UtilityEnrolmentGroup;
    teSE_DRLCCancelControl  eCancelControl;
    uint32                  u32effectiveTime;
} tsSE_DRLCCancelLoadControlEvent;

typedef struct {
    uint8               au8data[E_ZCL_SIGNATURE_SIZE];
} tsSE_DRLCOctets;

// Structure Reordered for SE 1.7
typedef struct {
    uint8               u8EventStatus;
    uint8               u8AverageLoadAdjustmentPercentageApplied;
    uint8               u8DutyCycleApplied;
    uint8               u8EventControl;
    uint8               u8SignatureType;
    uint8               u8CriticalityLevelApplied;
    bool_t              bSignatureVerified;
    uint16              u16CoolingTemperatureSetPointApplied;
    uint16              u16HeatingTemperatureSetPointApplied;
    uint32              u32IssuerId;
    uint32              u32EventStatusTime;
    tsSE_DRLCOctets     sSignature;
} tsSE_DRLCReportEvent;

typedef struct {
    DNODE                       dllrlcNode;
    uint8                       u8DestinationEndPointId;
    tsZCL_Address               sDestinationAddress;
    teSE_DRLCUserEventOption    eEventOption;
    uint8                       u8EventOptionChangeCount;
    uint8                       u8StartRandomizationMinutes;
    uint8                       u8DurationRandomizationMinutes;
    bool_t                      bDeferredCancel;
    uint32                      u32EffectiveCancelTime;
    tsSE_DRLCReportEvent        sReportEvent;
    teSE_DRLCCancelControl      eCancelControl;
    tsSE_DRLCLoadControlEvent   sLoadControlEvent;
} tsSE_DRLCLoadControlEventRecord;

typedef struct {
    uint32              u32StartTime;
    uint8               u8numberOfEvents;
} tsSE_DRLCGetScheduledEvents;

// Definition of DRLC Callback Event Structure
// Reordered for SE 1.7
typedef struct
{
    teSE_DRLCCallBackEventType              eEventType;
    uint8                                   u8CommandId;
    teSE_DRLCStatus                         eDRLCStatus;
    uint32                                  u32CurrentTime;
    union {
        tsSE_DRLCLoadControlEvent           sLoadControlEvent;
        tsSE_DRLCCancelLoadControlEvent     sCancelLoadControlEvent;
        tsSE_DRLCCancelLoadControlAllEvent  sCancelLoadControlAllEvent;
        tsSE_DRLCReportEvent                sReportEvent;
        tsSE_DRLCGetScheduledEvents         sGetScheduledEvents;
    } uMessage;

} tsSE_DRLCCallBackMessage;

typedef struct
{
    tsZCL_ReceiveEventAddress  sReceiveEventAddress;
    tsZCL_CallBackEvent        sDRLCCustomCallBackEvent;
    tsSE_DRLCCallBackMessage   sDRLCCallBackMessage;
} tsDRLC_Common;

typedef struct {
    tsDRLC_Common               sDRLC_Common;
    DLIST                       lLoadControlEventActiveList;
    DLIST                       lLoadControlEventScheduledList;
    DLIST                       lLoadControlEventCancelledList;
    DLIST                       lLoadControlEventDeAllocList;
    uint8                       u8NumberOfRecordEntries;
    tsSE_DRLCLoadControlEventRecord  *psLoadControlEventRecord;
} tsSE_DRLCCustomDataStructure;

/* DRLC Cluster */
typedef struct
{
    uint8               u8UtilityEnrolmentGroup;
    uint8               u8StartRandomizationMinutes;
    uint8               u8DurationRandomizationMinutes;
    uint16              u16DeviceClassValue;
} tsCLD_DRLC;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teSE_DRLCStatus eSE_DRLCRegisterTimeServer(void);

PUBLIC teZCL_Status eSE_DRLCCreate(
                    bool_t                          bIsServer,
                    uint8                           u8NumberOfRecordEntries,
                    uint8                           *pu8AttributeControlBits,
                    tsZCL_ClusterInstance           *psClusterInstance,
                    tsZCL_ClusterDefinition         *psClusterDefinition,
                    tsSE_DRLCCustomDataStructure    *psCustomDataStructure,
                    tsSE_DRLCLoadControlEventRecord *psDRLCLoadControlEventRecord,
                    void                            *pvEndPointSharedStructPtr
                    );

PUBLIC teSE_DRLCStatus eSE_DRLCGetLoadControlEvent(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8tableIndex,
                    teSE_DRLCEventList          eEventList,
                    tsSE_DRLCLoadControlEvent   **ppsLoadControlEvent);

PUBLIC teSE_DRLCStatus eSE_DRLCFindLoadControlEventRecord(
                    uint8                           u8SourceEndPointId,
                    uint32                          u32IssuerId,
                    bool_t                          bIsServer,
                    tsSE_DRLCLoadControlEventRecord **ppsLoadControlEventRecord,
                    teSE_DRLCEventList              *peEventList);

PUBLIC teSE_DRLCStatus eSE_DRLCAddLoadControlEvent(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    tsSE_DRLCLoadControlEvent   *psLoadControlEvent,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelLoadControlEvent(
                    uint8                           u8SourceEndPointId,
                    uint8                           u8DestinationEndPointId,
                    tsZCL_Address                   *psDestinationAddress,
                    tsSE_DRLCCancelLoadControlEvent *psCancelLoadControlEvent,
                    uint8                           *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelAllLoadControlEvents(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    teSE_DRLCCancelControl      eCancelEventControl,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCGetScheduledEventsSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    tsSE_DRLCGetScheduledEvents *psGetScheduledEvents,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCSetEventUserOption(
                    uint32                      u32IssuerId,
                    uint8                       u8SourceEndPointId,
                    teSE_DRLCUserEventOption    eEventOption);

PUBLIC teSE_DRLCStatus eSE_DRLCSetEventUserData(
                    uint32                      u32IssuerId,
                    uint8                       u8EndPointId,
                    teSE_DRLCUserEventSet       eUserEventSetID,
                    uint16                      u16EventData);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelAllLoadControlEventsSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address               *psDestinationAddress,
                    teSE_DRLCCancelControl      eSE_CancelLCEventsControl,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teSE_DRLCStatus eSE_DRLCCancelLoadControlEventSend(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    tsZCL_Address              *psDestinationAddress,
                    tsSE_DRLCCancelLoadControlEvent *psCancelLoadControlEvent,
                    uint8                      *pu8TransactionSequenceNumber);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

extern const tsZCL_AttributeDefinition asCLD_DRLCClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_DRLC;
extern uint8 au8DRLCClusterAttributeControlBits[];

#if defined __cplusplus
}
#endif

#endif  /* DRLC_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
