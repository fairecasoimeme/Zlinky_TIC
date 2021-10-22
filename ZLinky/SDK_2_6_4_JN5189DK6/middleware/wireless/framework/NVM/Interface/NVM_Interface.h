/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* Non-volatile storage module interface declarations for the CORTEX-M4 processor
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _NVM_INTERFACE_H
#define _NVM_INTERFACE_H

#include "EmbeddedTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/

/*
 * Name: gNvStorageIncluded_d
 * Description: enable/disable NV storage module
 */
#ifndef gNvStorageIncluded_d
#define gNvStorageIncluded_d            0
#endif

/*
 * Name: gNvDebugEnabled_d
 * Description: enable/disable debug mode for NVM
 */
#ifndef gNvDebugEnabled_d
#define gNvDebugEnabled_d                 0
#endif

/*
 * Name: gNvUseFlexNVM_d
 * Description: Use FlexNVM block as EEPROM backup (if the device supports FLEX-Memory)
 *              If the device has no such a feature, this define will have no effect.
 *              The configuration of the EEPROM and EERAM is done in NV_FlashHAL.h
 * WARNING: When FlexNVM is used, the NVM table must fit within the size of the
 *          FlexRAM R/W window. The calculus is as follow:
 *          Total_size = sum(Element_Size(i) x Elements_Count(i)) +
 *                       (Sizeof_Meta x table entries count) +
 *                       4 extra (guard) bytes.
 *                       
 *                       where:
 *                           i is the index of a particular table entry (range = 0..N)
 *                           N is the total count of valid table entries
 *                           Sizeof_Meta is the size of a meta information tag (4 bytes)
 *          
 *          If Total_size exceeds the FlexRAM size, the initialization function
 *          NvModuleInit(), will return with 'gNVM_NvTableExceedFlexRAMSize_c'
 *          error code and the module initialization is failed.
 *
 */
#ifndef gNvUseFlexNVM_d
#define gNvUseFlexNVM_d               0
#endif

/*
 * Name: gNvFragmentation_Enabled_d
 * Description: enables/disables fragmented saves/restores, i.e. a particular element from a table entry
 *              can be saved / restored.
 */
#ifndef gNvFragmentation_Enabled_d
#define gNvFragmentation_Enabled_d    0
#endif

/*
 * Name: gNvMinimumFreeBytesCountStart_c
 * Description: if the free space at init is smaller than this value, a page copy will be triggered.
 */
#ifndef gNvMinimumFreeBytesCountStart_c
#define gNvMinimumFreeBytesCountStart_c    128
#endif

/*
 * Name: gNvUseExtendedFeatureSet_d
 * Description: enables/disables the extended feature set of the module:
 *              - removes existing NV table entries
 *              - register new NV table entries
 *              - dynamic NV RAM tables
 */
#ifndef gNvUseExtendedFeatureSet_d
#define gNvUseExtendedFeatureSet_d    0
#endif

/*
 * Name: gUnmirroredFeatureSet_d
 * Description: enables/disables the use of unmirrored in ram feature of the module:
 *              - enables the access of a data set directly from NVM
 *                without a copy in RAM
 */
#ifndef gUnmirroredFeatureSet_d
#define gUnmirroredFeatureSet_d    FALSE
#endif

/*
 * Name: gNvTableKeptInRam_d
 * Description: set gNvTableKeptInRam_d to FALSE, if the NVM table is stored in FLASH memory (default)
 *              set gNvTableKeptInRam_d to TRUE, if the NVM table is stored in RAM memory
 */
#ifndef gNvTableKeptInRam_d
#define gNvTableKeptInRam_d    FALSE
#endif

/*
 * Name: gNvTableEntriesCountMax_c
 * Description: the maximum count of table entries that the application is
 *              going to use.
 */
#ifndef gNvTableEntriesCountMax_c
#define gNvTableEntriesCountMax_c       32
#endif

/*
* Name: gNvRecordsCopiedBufferSize_c
* Description: the size of the buffer used by page copy function;
*              it is used to store the indexes of the elements that needs
*              to be merged (defragmentation); the chosen value must be
*              a power of 2
*/
#ifndef gNvRecordsCopiedBufferSize_c
#define gNvRecordsCopiedBufferSize_c    64
#endif

/*
 * Name: gNvCacheBufferSize_c
 * Description: cache buffer size used by internal copy function (no defragmentation);
 *              the chosen value needs to be a multiple of 4
 */
#ifndef gNvCacheBufferSize_c
#define gNvCacheBufferSize_c            64
#endif

/*
 * Name: gNvMinimumTicksBetweenSaves_c
 * Description: Default minimum-timer-ticks-between-dataset-saves, in seconds
 * Notes: See NvSaveOnInterval(). This is used for all data sets.
 */
#ifndef gNvMinimumTicksBetweenSaves_c
#define gNvMinimumTicksBetweenSaves_c   4
#endif

/*
 * Name: gNvCountsBetweenSaves_c
 * Description: Default number-of-calls-to-NvSaveOnCount-between-dataset-saves
 * Notes: See NvSaveOnCount(). This is used for all data sets.
 */
#ifndef gNvCountsBetweenSaves_c
#define gNvCountsBetweenSaves_c         256
#endif


/*
 * Name: gNvInvalidDataEntry_c
 * Description: macro used to mark an invalid entry in NVM table
 */
#ifndef gNvInvalidDataEntry_c
#define gNvInvalidDataEntry_c           0xFFFFU
#endif

/*
 * Name: gNvFormatRetryCount_c
 * Description: retry count of the format operation, when it fails
 */
#ifndef gNvFormatRetryCount_c
#define gNvFormatRetryCount_c           3
#endif

/*
 * Name: gNvPendigSavesQueueSize_c
 * Description: pending saves queue size
 */
#ifndef gNvPendingSavesQueueSize_c
#define gNvPendingSavesQueueSize_c       32
#endif

/*
 * Name: gNvTableMarker_c
 * Description: table marker (ASCII = TB)
 */
#ifndef gNvTableMarker_c
#define gNvTableMarker_c                0x4254U
#endif

/*
 * Name: gNvAppVersion_c
 * Description: application version, used for table upgrade
 */
#ifndef gNvFlashTableVersion_c
#define gNvFlashTableVersion_c                   1
#endif
/*
 * Name: gNvEnableCriticalSection_c
 * Description: This macro is used to enable/disable protection of the critical
 *               sequences on the  operations nvm saves or erases . If the FLASH
 *               region used by the NVM is placed in the same program block as
 *               the ISR's executable code, the interrupts MUST be disabled.
 *               This could disrupt critical sequences.
 */
#ifndef gNvEnableCriticalSection_c
#define gNvEnableCriticalSection_c      (1)
#endif

/* constants for FlexNVM  only */
#ifndef gNvEepromDatasetSizeCode_c
#define gNvEepromDatasetSizeCode_c     0x3u
#endif

#ifndef gNvEepromBackupSizeCode_c
#define gNvEepromBackupSizeCode_c      0x4u
#endif

/* Save on interval w/ or w/o jitter */
#ifndef gNvmUseTimerJitter_d
#define gNvmUseTimerJitter_d        (1)
#endif

/* Debugging */
#ifndef gNvmEnableFSCIRequests_c
#define gNvmEnableFSCIRequests_c        (0)
#endif

#ifndef gNvmEnableFSCIMonitoring_c
#define gNvmEnableFSCIMonitoring_c      (0)
#endif

#ifndef gNvmDefaultFsciInterface_c
#define gNvmDefaultFsciInterface_c      (0)
#endif

#ifndef gNvmMemPoolId_c
#define gNvmMemPoolId_c                 (0)
#endif

/* Define section for keeping NVM table datasets */
#if defined(__GNUC__)
  extern uint32_t __start_NVM_TABLE[];
  extern uint32_t __stop_NVM_TABLE[];
  #define gNVM_TABLE_startAddr_c    ((NVM_DataEntry_t*)__start_NVM_TABLE)
  #define gNVM_TABLE_endAddr_c      ((NVM_DataEntry_t*)__stop_NVM_TABLE)
#elif (defined(__IAR_SYSTEMS_ICC__))
  #define gNVM_TABLE_startAddr_c    ((NVM_DataEntry_t*)__section_begin("NVM_TABLE"))
  #define gNVM_TABLE_endAddr_c      ((NVM_DataEntry_t*)__section_end("NVM_TABLE"))
#elif (defined(__CC_ARM))

  extern uint32_t Image$$ER_NVM_TABLE$$Base[];
  extern uint32_t Image$$ER_NVM_TABLE$$Limit[];

  #define gNVM_TABLE_startAddr_c    ((NVM_DataEntry_t*)Image$$ER_NVM_TABLE$$Base)
  #define gNVM_TABLE_endAddr_c      ((NVM_DataEntry_t*)Image$$ER_NVM_TABLE$$Limit)
	
#else
  #define gNVM_TABLE_startAddr_c    ((NVM_DataEntry_t*)0)
  #define gNVM_TABLE_endAddr_c      ((NVM_DataEntry_t*)0)
  #warning Module information will not be stored!
#endif

#define gNVM_TABLE_entries_c        ((uint16_t)(((uint32_t)gNVM_TABLE_endAddr_c \
                                    - (uint32_t)gNVM_TABLE_startAddr_c) \
                                    / sizeof(NVM_DataEntry_t)))

#if defined(__IAR_SYSTEMS_ICC__)
#pragma section="NVM_TABLE"
#endif

#if gNvTableKeptInRam_d
  #define SET_DATASET_STRUCT_NAME(datasetId) gNvmTableEntry##_##datasetId
  #if defined(__IAR_SYSTEMS_ICC__)
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      _Pragma("location=\"NVM_TABLE\"") __root \
      NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
  #elif defined(__GNUC__)
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      __attribute__((section (".NVM_TABLE"), used)) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
	#elif defined(__CC_ARM)
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      __attribute__((section("NVM_TABLE"))) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
  #else
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
     #warning Unknown/undefined toolchain!
  #endif
#else
  #define SET_DATASET_STRUCT_NAME(datasetId) gNvmTableEntry##_##datasetId
  #if defined(__IAR_SYSTEMS_ICC__)
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      _Pragma("location=\"NVM_TABLE\"") __root \
      const NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
  #elif defined(__CC_ARM)
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      const NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      __attribute__((section("NVM_TABLE"))) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
  #elif defined(__GNUC__)
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      const NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      __attribute__((section (".NVM_TABLE"), used)) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
  #else
  #define NVM_RegisterDataSet(pData, elementsCount, elementSize, dataEntryID, dataEntryType) \
      const NVM_DataEntry_t \
      SET_DATASET_STRUCT_NAME(dataEntryID) \
      = { pData, elementsCount, elementSize, dataEntryID, dataEntryType }
     #warning Unknown/undefined toolchain!
  #endif
#endif

/*****************************************************************************
******************************************************************************
* Public type declarations
******************************************************************************
*****************************************************************************/

/*
 * Name: NvSaveCounter_t
 * Description: save counter data type definition
 */
typedef uint16_t NvSaveCounter_t;

/*
 * Name: NvSaveInterval_t
 * Description: save interval data type definition
 */
typedef uint16_t NvSaveInterval_t;


/*
 * Name: NvTableEntryId_t
 * Description: type definition for the unique ID of each NVM table entry
 */
typedef uint16_t NvTableEntryId_t;


/*
 * Name: NVM_ElementInfo_t
 * Description: element information data type definition
 */
typedef struct NVM_DatasetInfo_tag
{
    bool_t saveNextInterval;
    NvSaveInterval_t ticksToNextSave;
    NvSaveCounter_t countsToNextSave;
#if gUnmirroredFeatureSet_d
    uint16_t elementIndex;
#endif
}NVM_DatasetInfo_t;

/*
 * Name: NVM_DataEntryType_tag
 * Description: enumerated datasets entry types
 */
typedef enum NVM_DataEntryType_tag
{
  gNVM_MirroredInRam_c,
#if gUnmirroredFeatureSet_d
  gNVM_NotMirroredInRam_c,
  gNVM_NotMirroredInRamAutoRestore_c
#endif
}NVM_DataEntryType_t;

/*
 * Name: NVM_DataEntry_t
 * Description: NVM dataset entry info definition
 */
typedef struct NVM_DataEntry_tag
{
    void* pData;
    uint16_t ElementsCount;
    uint16_t ElementSize;
    uint16_t DataEntryID;
    uint16_t DataEntryType;
} NVM_DataEntry_t;

/*
 * Name: NVM_Status_t
 * Description: enumerated data type used by NVM functions as return code
 */
typedef enum NVM_Status_tag
{
    gNVM_OK_c,
    gNVM_Error_c,
    gNVM_InvalidPageID_c,
    gNVM_PageIsNotBlank_c,
    gNVM_SectorEraseFail_c,
    gNVM_NullPointer_c,
    gNVM_PointerOutOfRange_c,
    gNVM_AddressOutOfRange_c,
    gNVM_InvalidSectorsCount_c,
    gNVM_InvalidTableEntry_c,
    gNVM_PageIsEmpty_c,
    gNVM_MetaNotFound_c,
    gNVM_RecordWriteError_c,
    gNVM_MetaInfoWriteError_c,
    gNVM_ModuleNotInitialized_c,
    gNVM_CriticalSectionActive_c,
    gNVM_ModuleAlreadyInitialized_c,
    gNVM_PageCopyPending_c,
    gNVM_RestoreFailure_c,
    gNVM_FormatFailure_c,
    gNVM_RegisterFailure_c,
    gNVM_AlreadyRegistered,
    gNVM_SaveRequestRejected_c,
    gNVM_NvTableExceedFlexRAMSize_c,
    gNVM_NvWrongFlashDataIFRMap_c,
    gNVM_CannotCreateMutex_c,
    gNVM_NoMemory_c,
    gNVM_IsMirroredDataSet_c,
    gNVM_DefragBufferTooSmall_c,
    gNVM_FragmentatedEntry_c,
    gNVM_AlignamentError_c,
    gNVM_InvalidTableEntriesCount_c,
#if gNvUseFlexNVM_d
    gNVM_FlashMemoryIsSecured_c,
    gNVM_FlexNVMPartitioningFail_c
#endif
} NVM_Status_t;

/*
 * Name: NVM_Statistics_t
 * Description: structure used to store pages statistic information
 *              (erase cycle count of each page)
 */
typedef struct NVM_Statistics_tag
{
    uint32_t FirstPageEraseCyclesCount;
    uint32_t SecondPageEraseCyclesCount;
} NVM_Statistics_t;


/*****************************************************************************
******************************************************************************
* Public memory declarations
*****************************************************************************
*****************************************************************************/

#if gNvStorageIncluded_d
/*
 * Name: pNVM_DataTable
 * Description: a pointer to the NV data table stored in RAM
 */
extern NVM_DataEntry_t* const pNVM_DataTable;

#endif /* #if gNvStorageIncluded_d */


/*****************************************************************************
******************************************************************************
* External declarations
*****************************************************************************
*****************************************************************************/

#ifndef gHostApp_d
/*
 * Name: BUtl_SetReceiverOff
 * Description: Turn OFF the 802.15.4 receiver
 * Parameter(s): none
 * Return: -
 */
extern void BUtl_SetReceiverOff(void);

/*
 * Name: BUtl_SetReceiverOn
 * Description: Turn ON the 802.15.4 receiver
 * Parameter(s): none
 * Return: -
 */
extern void BUtl_SetReceiverOn(void);

/*
 * Name: NvOperationStart
 * Description: Turn OFF the 802.15.4 receiver
 */
#define NvOperationStart()  BUtl_SetReceiverOff()
/*
 * Name: NvOperationEnd
 * Description: Turn ON the 802.15.4 receiver
 */
#define NvOperationEnd()  BUtl_SetReceiverOn()
#else
#define NvOperationStart()
#define NvOperationEnd()
#endif

/*****************************************************************************
******************************************************************************
* Public prototypes (Module Interface)
******************************************************************************
*****************************************************************************/

/*****************************************************************************
 * INFORMATION
 *****************************************************************************
 *
 * Data sets are only saved by the idle task. There is no save-immediately
 * capability.
 *
 * There are three save mechanism, each with it's own API function.
 *
 *      1) NvSaveOnIdle(): save the next time the idle task runs. This
 *      will save the data set at least as soon as either of the other
 *      functions, and usually sooner.
 *
 *      2) NvSaveOnInterval(): save no more often than a given time
 *      interval. If it has been at least that long since the last save,
 *      this function will cause a save the next time the idle task runs.
 *
 *      3) NvSaveOnCount(): increment a counter that is associated with the
 *      data set. When that counter equals or exceeds some trigger value,
 *      save the next time the idle task runs.
 *
 * The data set will be saved to NV storage during the next execution of
 * the idle task if:
 *      NvSaveOnIdle() has been called,
 *  OR
 *      NvSaveOnInterval() has been called, and at least the specified
 *      time has past since the last save,
 *  OR
 *      NvSaveOnCount() has been called at least as often as the counter's
 *      trigger value since the last save.
 *
 * All three functions cause NvIsDataSetDirty() to return TRUE. No API is
 * provided to allow the client to know which save function or functions
 * have been called. The data set is either believed to be identical to
 * the saved copy (== not dirty) or not identical (== dirty).
 *
 * The NV code does not require a data set to have a minimum-time-between-
 * saves or a minimum-count-between-saves. A data set may have both.
 *
 * Whenever a data set is saved for any reason:
 *      it's dirty flag is cleared
 *  AND
 *      it's minimum-time-between-saves timer is restarted from 0,
 *  AND
 *      it's minimum-count-between-saves counter is set to 0.
 *****************************************************************************/

/******************************************************************************
 * Name: NvModuleInit
 * Description: Initialise the NV storage module
 * Parameter(s): -
 * Return: gNVM_ModuleAlreadyInitialized_c - if the module is already
 *                                           initialised
 *         gNVM_InvalidSectorsCount_c - if the sector count configured in the
 *                                      project linker file is invalid
 *         gNVM_MetaNotFound_c - if no meta information was found
 *         gNVM_OK_c - module was successfully initialised
 *****************************************************************************/
extern NVM_Status_t NvModuleInit
(
    void
);

/******************************************************************************
 * Name: NvMoveToRam
 * Description: Move from NVM to Ram
 * Parameter(s):  ppData     double pointer to the entity to be moved from flash to RAM
 * Return: gNVM_OK_c - if operation completed successfully
 *         gNVM_NoMemory_c - in case there is not a memory block free
 *         Note: see also return codes of NvGetEntryFromDataPtr() function
 *****************************************************************************/

extern NVM_Status_t NvMoveToRam
(
  void** ppData
);

/******************************************************************************
 * Name: GetFlashTableVersion
 * Description: returns the flash table version
 * Parameter(s): -
 * Return: 0 or flash table version
 *****************************************************************************/
extern uint16_t GetFlashTableVersion
(
    void
);

/******************************************************************************
 * Name: NvErase
 * Description: Erase from NVM an unmirrored dataset
 * Parameter(s):  ppData     double pointer to the entity to be moved from flash to RAM
 * Return: gNVM_OK_c - if operation completed successfully
 *         gNVM_NoMemory_c - in case there is not a memory block free
 *         Note: see also return codes of NvGetEntryFromDataPtr() function
 *****************************************************************************/

extern NVM_Status_t NvErase
(
  void** ppData
);



/******************************************************************************
 * Name: NvSaveOnIdle
 * Description: Save the data pointed by ptrData on the next call to NvIdle()
 * Parameter(s): [IN] ptrData - pointer to data to be saved
 *               [IN] saveRestoreAll - specify if all the elements from the NVM table
 *                              entry shall be saved
 * Return: gNVM_OK_c - if operation completed successfully
 *         gNVM_Error_c - in case of error(s)
 *         Note: see also return codes of NvGetEntryFromDataPtr() function
 ******************************************************************************/
extern NVM_Status_t NvSaveOnIdle
(
    void* ptrData,
    bool_t saveAll
);


/******************************************************************************
 * Name: NvSaveOnInterval
 * Description:  save no more often than a given time interval. If it has
 *               been at least that long since the last save,
 *               this function will cause a save the next time the idle
 *               task runs.
 * Parameters: [IN] ptrData - pointer to data to be saved
 * NOTE: this function saves all the element of the table entry pointed by
 *       ptrData
 * Return: NVM_OK_c - if operation completed successfully
 *         Note: see also return codes of NvGetEntryFromDataPtr() function
 ******************************************************************************/
extern NVM_Status_t NvSaveOnInterval
(
    void* ptrData
);


/******************************************************************************
 * Name: NvSaveOnCount
 * Description: Decrement the counter. Once it reaches 0, the next call to
 *              NvIdle() will save the entire table entry (all elements).
 * Parameters: [IN] ptrData - pointer to data to be saved
 * Return: NVM_OK_c - if operation completed successfully
 *         Note: see also return codes of NvGetEntryFromDataPtr() function
 ******************************************************************************/
extern NVM_Status_t NvSaveOnCount
(
    void* ptrData
);


/******************************************************************************
 * Name: NvSetMinimumTicksBetweenSaves
 * Description: Set the timer used by NvSaveOnInterval(). Takes effect after
 *              the next save.
 * Parameters: [IN] newInterval - new time interval
 * Return: -
 ******************************************************************************/
extern void NvSetMinimumTicksBetweenSaves
(
    NvSaveInterval_t newInterval
);


/******************************************************************************
 * Name: NvSetCountsBetweenSaves
 * Description: Set the counter trigger value used by NvSaveOnCount().
 *              Takes effect after the next save.
 * Parameters: [IN] newCounter - new counter value
 * Return: -
 ******************************************************************************/
extern void NvSetCountsBetweenSaves
(
    NvSaveCounter_t newCounter
);

/******************************************************************************
 * Name: NvTimerTick
 * Description: Called from the idle task to process save-on-interval requests
 * Parameters: [IN] countTick - enable/disable tick count
 * Return: FALSE if the timer tick counters for all data sets have reached
 *         zero. In this case, the timer can be turned off.
 *         TRUE if any of the data sets' timer tick counters have not yet
 *         counted down to zero. In this case, the timer should be active
 ******************************************************************************/
extern bool_t NvTimerTick
(
    bool_t countTick
);


/******************************************************************************
 * Name: NvRestoreDataSet
 * Description: copy the most recent version of the element/table entry pointed
 *              by ptrData from NVM storage system to RAM memory
 * Parameter(s): [IN] ptrData - pointer to data (element) to be restored
 *               [IN] restoreAll - if FALSE restores a single element
 *                               - if TRUE restores an entire table entry
 * Return: status of the restore operation
 *****************************************************************************/
extern NVM_Status_t NvRestoreDataSet
(
    void* ptrData,
    bool_t restoreAll
);


/******************************************************************************
 * Name: NvClearCriticalSection
 * Description: leave critical section
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void NvClearCriticalSection
(
    void
);


/******************************************************************************
 * Name: NvSetCriticalSection
 * Description: enter critical section
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void NvSetCriticalSection
(
    void
);
/******************************************************************************
 * Name: NvIdle
 * Description: Called from the idle task (bare-metal) or NVM_Task (MQX,
 *              FreeRTOS) to process the pending saves, erase or copy
 *              operations.
 * Parameters: -
 * Return: -
 ******************************************************************************/
extern void NvIdle
(
    void
);
/******************************************************************************
 * Name: NvGetNvIdleTaskId
 * Description:
 * Parameters: -
 * Return: returns the Id of the task which hosts NvIdle function
 ******************************************************************************/
void* NvGetNvIdleTaskId
(
void
);

/******************************************************************************
 * Name: NvIsDataSetDirty
 * Description: return TRUE if the element pointed by ptrData is dirty
 * Parameters: [IN] ptrData - pointer to data to be checked
 * Return: TRUE if the element is dirty, FALSE otherwise
 ******************************************************************************/
extern bool_t NvIsDataSetDirty
(
    void* ptrData
);


/******************************************************************************
 * Name: NvGetStatistics
 * Description:
 * Parameter(s): [OUT] ptrStat - pointer to a memory location where the pages
 *                               statistics (erase cycles of each page) will
 *                               be stored
 * Return: -
 *****************************************************************************/
extern void NvGetPagesStatistics
(
    NVM_Statistics_t* ptrStat
);


/******************************************************************************
 * Name: NvFormat
 * Description: Format the NV storage system. The function erases both virtual
 *              pages and then writes the page counter to active page.
 * Parameter(s): -
 * Return: gNVM_OK_c - if the operation completes successfully
 *         gNVM_FormatFailure_c - if the format operation fails
 *         gNVM_ModuleNotInitialized_c - if the NVM  module is not initialised
 *         gNVM_CriticalSectionActive_c - if the system has entered in a
 *                                        critical section
 *****************************************************************************/
extern NVM_Status_t NvFormat
(
    void
);


/******************************************************************************
 * Name: NvRegisterTableEntry
 * Description: The function tries to register a new table entry within an
 *              existing NV table. If the NV table contained an erased (invalid)
 *              entry, the entry will be overwritten with a new one (provided
 *              by the mean of this function arguments)
 * Parameter(s): [IN] ptrData - generic pointer to RAM data to be registered
 *                              within the NV storage system
 *               [IN] uniqueId - an unique ID of the table entry
 *               [IN] elemCount - how many elements the table entry contains
 *               [IN] elemSize - the size of an element
 *               [IN] overwrite - if an existing table entry shall be
 *                                overwritten
 * Return: gNVM_OK_c - if the operation completes successfully
 *         gNVM_ModuleNotInitialized_c - if the NVM  module is not initialised
 *****************************************************************************/
extern NVM_Status_t NvRegisterTableEntry
(
    void* ptrData,
    NvTableEntryId_t uniqueId,
    uint16_t elemCount,
    uint16_t elemSize,
    uint16_t dataEntryType,
    bool_t overwrite
);


/******************************************************************************
 * Name: NvEraseEntryFromStorage
 * Description: The function removes a table entry within the existing NV
 *              table.
 * Parameter(s): [IN] ptrData - a pointer to an existing RAM data that is
 *                              managed by the NV storage system
 * Return: gNVM_OK_c - if the operation completes successfully
 *         gNVM_ModuleNotInitialized_c - if the NVM  module is not initialised
 *         gNVM_NullPointer_c - if a NULL pointer is provided
 *****************************************************************************/
extern NVM_Status_t NvEraseEntryFromStorage
(
    void* ptrData
);

/******************************************************************************
 * Name: NvSyncSave
 * Description: The function saves the pointed element or the entire table
 *              entry to the storage system. The save operation is not
 *              performed on the idle task but within this function call.
 * Parameter(s): [IN] ptrData - a pointer to data to be saved
 *               [IN] saveAll - specifies if the entire table entry shall be
 *                              saved or only the pointed element
 * Return: gNVM_OK_c - if the operation completes successfully
 *         gNVM_ModuleNotInitialized_c - if the NVM  module is not initialized
 *         gNVM_NullPointer_c - if a NULL pointer is provided
 *         gNVM_PointerOutOfRange_c - if the pointer is out of range
 *         gNVM_InvalidTableEntry_c - if the table entry is not valid
 *         gNVM_MetaInfoWriteError_c - meta tag couldn't be written
 *         gNVM_RecordWriteError_c - record couldn't be written
 *         gNVM_CriticalSectionActive_c - the module is in critical section
 *****************************************************************************/
extern NVM_Status_t NvSyncSave
(
    void* ptrData,
    bool_t saveAll
);


/******************************************************************************
 * Name: NvAtomicSave
 * Description: The function performs an atomic save of the entire NV table
 *              to the storage system. The operation is performed
 *              in place (atomic).
 * Parameter(s):  [IN] ignoreCriticalSectionFlag - if set to TRUE, the critical
 *                                                section flag is ignored
 * Return: gNVM_OK_c - if the operation completes successfully
 *         gNVM_ModuleNotInitialized_c - if the NVM  module is not initialized
 *         gNVM_NullPointer_c - if a NULL pointer is provided
 *         gNVM_PointerOutOfRange_c - if the pointer is out of range
 *         gNVM_InvalidTableEntry_c - if the table entry is not valid
 *         gNVM_MetaInfoWriteError_c - meta tag couldn't be written
 *         gNVM_RecordWriteError_c - record couldn't be written
 *         gNVM_CriticalSectionActive_c - the module is in critical section
 *****************************************************************************/
extern NVM_Status_t NvAtomicSave
(
    void
);

/******************************************************************************
 * Name: NvShutdown
 * Description: The function waits for all idle saves to be processed.
 * Parameter(s):  -
 * Return: -
 *****************************************************************************/
extern void NvShutdown
(
    void
);

/******************************************************************************
 * Name: NvCompletePendingOperations
 * Description: The function attemps to complete all the NVM related pending
 *              operations.
 * Parameter(s):  -
 * Return: -
 *****************************************************************************/
extern void NvCompletePendingOperations
(
    void
);

/******************************************************************************
 * Name: RecoverNvEntry
 * Description: Reads a flash entry so that the application can handle dinamic entries.
 * Parameter(s): [IN] index - the ram entry index
 *               [OUT] entry - the flash entry at the specified index
 * Return: gNVM_OK_c - if the operation completes successfully
           gNVM_RestoreFailure_c - if the operation failed
 *****************************************************************************/
extern NVM_Status_t RecoverNvEntry
(
    uint16_t index,
    NVM_DataEntry_t *entry
);

#ifdef __cplusplus
}
#endif

#endif /* _NVM_INTERFACE_H */

