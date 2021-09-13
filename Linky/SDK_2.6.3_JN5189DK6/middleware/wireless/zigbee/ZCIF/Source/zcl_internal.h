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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl_internal.h
 *
 * DESCRIPTION:       Definitions used to build the ZCL library
 *
 ****************************************************************************/

#ifndef  ZCL_INTERNAL_H_INCLUDED
#define  ZCL_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "dlist.h"

#include "zcl.h"
#include "zcl_common.h"
#include "zcl_heap.h"
#include "zcl_customcommand.h"

#ifdef UART_DEBUG
extern PUBLIC void vDebug(char *pcMessage);
extern PUBLIC void vDebugHex(uint32 u32Data, int iSize);
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

// 0x0000 – 0x7fff Standard ZigBee application profile.
#define MAX_STD_PROFILE_ID              (0x7FFF)
// 0x8000 – 0xbfff Reserved.
// 0xc000 – 0xffff Manufacturer Specific application profile.
#define MIN_MAN_PROFILE_ID              (0xC000)

// 0x0000 – 0xbfff Standard ZigBee device description.
#define MAX_STD_DEVICE_ID               (0xBFFF)
// 0xc000 – 0xffff Reserved.

// 0x0000 – 0x7fff Standard ZigBee cluster.
#define MAX_STD_CLUSTER_ID              (0x7FFF)
// 0x8000 – 0xfbff Reserved.
// 0xfc00 – 0xffff Manufacturer specific cluster within a standard ZigBee profile.
#define MIN_MAN_CLUSTER_ID              (0xFC00)

#define MAX_COMMAND_ID                  (0x10)

#define MAX_ENDPOINT_ID                 (240)

#ifdef UART_DEBUG
#define vZCL_HeapAlloc(A, B, C, D, E) \
    vDebugHex(u32HeapStart, 8); \
    vDebug(":"); \
    A = (B *)pvZCL_HeapAlloc((void *)(A), C, D); \
    vDebug(E); \
    vDebug(":Req:"); \
    vDebugHex(C, 4); \
    vDebug(":"); \
    vDebugHex(u32HeapStart, 8); \
    vDebug(" ")
#else
#define vZCL_HeapAlloc(A, B, C, D, E) A = (B *)pvZCL_HeapAlloc((void *)(A), C, D)
#endif

#define SE_CLUSTER_ID_KEY_ESTABLISHMENT                    (0x0800)
#define SE_CLUSTER_ID_TUNNELING                            (0x0704)

#if PC_PLATFORM_BUILD
    /*To fix Long address for Unit testing*/
    #define IEEE_SRC_ADD    0x1234567812345678LL
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef struct
{
    uint8    u8Length;
    uint8    *pu8Data;
} tsZCL_String;

typedef struct
{
    uint16    u16Length;
    uint8    *pu8Data;
} tsZCL_LString;
typedef union
  {
    uint8                       u8Data;
    uint16                      u16Data;
    uint32                      u32Data;
    uint64                      u64Data;
    float                       flData;
    double                      dbData;
    tsZCL_String                sStrData;
    tsZCL_Array                 saData;
    tsZCL_Structure             sData;
    tsZCL_Key                   sKeyData;
    tsZCL_LString               sLString;
  }uAttribData;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


PUBLIC teZCL_Status eZCL_CheckUserStringStructureRead(
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct);

PUBLIC teZCL_Status eZCL_GetStringLengthFieldSize(
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    uint8                      *pu8typeSize);

PUBLIC uint16 u16ZCL_GetStringStructureSize(
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct);

PUBLIC bool_t bZCL_CheckStringFitsInStructure(
                    uint8                       u8EndPointId,
                    uint16                      u16AttributeId,
                    bool_t                      bIsManufacturerSpecific,
                    bool_t                      bIsClientAttribute,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    uint16                      u16offset,
                    PDUM_thAPduInstance         hAPduInst);

PUBLIC void vZCL_GetInternalMutex(void);

PUBLIC void vZCL_ReleaseInternalMutex(void);

PUBLIC void *pvZCL_HeapAlloc(
                    void                       *pvPointer,
                    uint32                      u32BytesNeeded,
                    bool_t                      bClear);

PUBLIC teZCL_Status eZCL_CreateTimer(
                    uint8                       u8NumberOfTimers);

PUBLIC void vZCL_TimerSchedulerUpdate(tsZCL_CallBackEvent *psZCLcallBackEvent);

PUBLIC void vReportTimerClickCallback(
                    tsZCL_CallBackEvent        *psCallBackEvent);
PUBLIC uint8 u8ZCL_GetAttributeAllignToFourBytesBoundary(
                    uint8                   u8TypeSize);                    
PUBLIC teZCL_Status eZCL_GetAttributeTypeSize(
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    uint8                      *pu8typeSize) __attribute__((noinline));

PUBLIC teZCL_Status eZCL_GetNumberOfFreeReportRecordEntries(
                    uint8                      *pu8NumberOfEntries);

PUBLIC teZCL_Status eZCLAddReport(
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    tsZCL_AttributeDefinition  *psAttributeDefinition,
                    tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord);

PUBLIC teZCL_Status eZCLFindReportEntryByAttributeIdAndDirection(
                     uint8                      u8EndPoint,
                     uint16                     u16ClusterID,
                     uint8                      u8DirectionIsReceived,
                     uint16                     u16AttributeEnum,
                     tsZCL_AttributeReportingConfigurationRecord    **ppsAttributeReportingRecord);

PUBLIC teZCL_Status eZCL_CalculateSizeOfReportRecord(
                     tsZCL_AttributeReportingConfigurationRecord *psAttributeReportingConfigurationRecord,
                     uint8                                       *pu8SizeOfReportRecord);

PUBLIC teZCL_Status eZCL_DoesAttributeHaveReportableChange(
                     teZCL_ZCLAttributeType      eAttributeDataType);

PUBLIC teZCL_Status eZCL_GetAttributeReportableEntry(
                     void                      **ppvReportableChange,
                     teZCL_ZCLAttributeType      eAttributeDataType,
                     tuZCL_AttributeReportable  *puAttributeReportableChange);


PUBLIC void vZCL_HandleEntireProfileCommand(uint8                       u8CommandIdentifier,
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psZCL_EndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

PUBLIC teZCL_Status eZCL_CreateOptionalManagers(
                    uint8                        u8NumberOfReports,
                    uint16                       u16SystemMinimumReportingInterval,
                    uint16                       u16SystemMaximumReportingInterval);

PUBLIC bool_t bZCL_CheckManufacturerSpecificAttributeFlagMatch(
                    tsZCL_AttributeDefinition  *psAttributeDefinition,
                    bool_t                      bManufacturerSpecificAttributeFlag);
                    
PUBLIC bool_t bZCL_CheckManufacturerSpecificCommandFlagMatch(
                    tsZCL_CommandDefinition  *psCommandDefinition,
                    bool_t                    bManufacturerSpecificCommandFlag);                    

PUBLIC void *pvZCL_GetAttributePointer(tsZCL_AttributeDefinition *psAttributeDefinition,
                                       tsZCL_ClusterInstance     *psClusterInstance,
                                       uint16                     u16AttributeId);
PUBLIC bool_t bZCL_CheckAttributeDirectionFlagMatch(
                    tsZCL_AttributeDefinition  *psAttributeDefinition,
                    bool_t                      bIsServer);                    

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif
                    
#endif  /* ZCL_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
