/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* This file contains the OpGroups and OpCodes for the NVM module
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef __NV_FSCI_COMMANDS_H__
#define __NV_FSCI_COMMANDS_H__

/*****************************************************************************
 *****************************************************************************
 * Include
 *****************************************************************************
 *****************************************************************************/

#include "EmbeddedTypes.h"
#include "FsciInterface.h"
#include "NVM_Interface.h"

#if gFsciIncluded_c && gNvStorageIncluded_d

/*****************************************************************************
 *****************************************************************************
 * Public macros
 *****************************************************************************
 *****************************************************************************/

/* Operation Groups */
#define gNV_FsciReqOG_d                 (0xA7)
#define gNV_FsciCnfOG_d                 (0xA8)

/* Operation Codes */
#define mFsciMsgNVSaveReq_c             (0xE4) /* Fsci-NVSave.Request.            */
#define mFsciMsgGetNVDataSetDescReq_c   (0xE5) /* Fsci-NVGetDataSetDesc.Request.  */
#define mFsciMsgGetNVCountersReq_c      (0xE6) /* Fsci-NVGetNvCounters.Request.   */
#define mFsciMsgSetNVMonitoringReq_c    (0xE9) /* Fsci-NVSetMonitoring.Request.   */
#define mFsciMsgNVWriteMonitoring_c     (0xEA) /* Fsci-NVWriteMonitoring.          */
#define mFsciMsgNVPageEraseMonitoring_c (0xEB) /* Fsci-NVPageEraseMonitoring.     */
#define mFsciMsgFormatNvmReq_c          (0xEC) /* Fsci-NVFormatReq.Request.       */
#define mFsciMsgRestoreNvmReq_c         (0xED) /* Fsci-NVRestoreReq.Request.      */
#define mFsciMsgRestoreMonitoring_c     (0xEE) /* Fsci-NVRestoreMonitoring.       */
#define mFsciMsgVirtualPageMonitoring_c (0xEF) /* Fsci-NVVirtualPageMonitoring.   */

#define mGetFsciInterfaceFromNvTableEntryId_d (0)

/*****************************************************************************
 *****************************************************************************
 * Public functions
 *****************************************************************************
 *****************************************************************************/
#if gNvmEnableFSCIRequests_c
  void   NV_FsciMsgHandler               (void*, void*, uint32_t);
  bool_t FSCI_MsgNVSaveReqFunc           (void*, uint32_t);
  bool_t FSCI_MsgGetNVDataSetDescReqFunc (void*, uint32_t);
  bool_t FSCI_MsgGetNVCountersReqFunc    (void*, uint32_t);
  bool_t FSCI_MsgSetNVMonitoring         (void*, uint32_t);
  bool_t FSCI_MsgNVFormatReq             (void*, uint32_t);
  bool_t FSCI_MsgNVRestoreReq            (void*, uint32_t);
#endif

#if gNvmEnableFSCIMonitoring_c
  void FSCI_MsgNVPageEraseMonitoring(uint32_t pageAddress, uint8_t status);
  void FSCI_MsgNVWriteMonitoring(uint16_t nvmTableEntryId, uint16_t elementIndex, uint8_t saveRestoreAll);
  void FSCI_MsgNVRestoreMonitoring(uint16_t nvmTableEntryId, bool_t bStart, uint8_t status);
  void FSCI_MsgNVVirtualPageMonitoring(bool_t bStart, uint8_t status);
#endif

#endif //gFsciIncluded_c
  
#endif /* #ifndef __NV_FSCI_COMMANDS_H__ */