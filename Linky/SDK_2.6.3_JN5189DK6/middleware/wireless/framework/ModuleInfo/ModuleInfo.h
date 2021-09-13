/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* This file contains the structure used by every module present to identify itself
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _MODULE_INFO_H_
#define _MODULE_INFO_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#if defined(__GNUC__)

  extern uint32_t __start_VERSION_TAGS[];
  extern uint32_t __stop_VERSION_TAGS[];

  #define gVERSION_TAGS_startAddr_d ((moduleInfo_t*)__start_VERSION_TAGS)
  #define gVERSION_TAGS_endAddr_d   ((moduleInfo_t*)__stop_VERSION_TAGS)

#elif (defined(__IAR_SYSTEMS_ICC__))
  #define gVERSION_TAGS_startAddr_d ((moduleInfo_t*)__section_begin(".VERSION_TAGS"))
  #define gVERSION_TAGS_endAddr_d   ((moduleInfo_t*)__section_end(".VERSION_TAGS"))
#elif (defined(__CC_ARM))

  extern uint32_t Image$$VERSION_TAGS$$Base[];
  extern uint32_t Image$$VERSION_TAGS$$Limit[];
	
	#define gVERSION_TAGS_startAddr_d ((moduleInfo_t*)Image$$VERSION_TAGS$$Base)
  #define gVERSION_TAGS_endAddr_d   ((moduleInfo_t*)Image$$VERSION_TAGS$$Limit)
	
#else
  #define gVERSION_TAGS_startAddr_d ((moduleInfo_t*)0)
  #define gVERSION_TAGS_endAddr_d   ((moduleInfo_t*)0)
  #warning Module informaion will not be stored!
#endif

#define gVERSION_TAGS_entries_d  ( ((uint32_t)gVERSION_TAGS_endAddr_d - \
                                    (uint32_t)gVERSION_TAGS_startAddr_d)/ \
                                    sizeof(moduleInfo_t) )

#define gVERSION_TAGS_entrySizeNoPadding_d (sizeof(moduleInfo_t) -  \
                                            GetSizeOfMember(moduleInfo_t, padding))

/* Concatenate macros */
#define QU(x) #x
#define QUH(x) QU(x)

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/* VERSION_TAGS is the section where module information will be stored */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma section=".VERSION_TAGS"
#elif defined(__CC_ARM)
#pragma arm section rodata = "VERSION_TAGS"
#endif

typedef const char * ro_string_t;

typedef PACKED_STRUCT moduleInfo_tag {
  ro_string_t* const moduleString;
  uint8_t  moduleId;
  uint8_t  versionNumber[3];
  uint16_t buildNumber;
  uint8_t  padding[2];
} moduleInfo_t;

/* MODULE INFO (VERSION_TAGS) REGISTER MACRO, DO NOT EDIT or REMOVE */
#define SET_MODULE_NAME(name) name##_##VERSION
#if defined(__IAR_SYSTEMS_ICC__)
#define RegisterModuleInfo(moduleName, moduleNameString, moduleId, versionNoMajor, versionNoMinor, versionNoPatch, buildNo) \
    _Pragma("location=\".VERSION_TAGS\"") __root \
    const moduleInfo_t \
    SET_MODULE_NAME(moduleName) \
      = { (ro_string_t*)&moduleNameString, moduleId, {versionNoMajor, versionNoMinor, versionNoPatch}, buildNo }
#elif defined(__CC_ARM)
#define RegisterModuleInfo(moduleName, moduleNameString, moduleId, versionNoMajor, versionNoMinor, versionNoPatch, buildNo) \
    const moduleInfo_t \
    SET_MODULE_NAME(moduleName) \
      = { (ro_string_t*)&moduleNameString, moduleId, {versionNoMajor, versionNoMinor, versionNoPatch}, buildNo }
#elif defined(__GNUC__)
#define RegisterModuleInfo(moduleName, moduleNameString, moduleId, versionNoMajor, versionNoMinor, versionNoPatch, buildNo) \
    const moduleInfo_t \
    SET_MODULE_NAME(moduleName) \
    __attribute__((section (".VERSION_TAGS"), used)) \
    = { (ro_string_t*)&moduleNameString, moduleId, {versionNoMajor, versionNoMinor, versionNoPatch}, buildNo }
#else
#define RegisterModuleInfo(moduleName, moduleNameString, moduleId, versionNoMajor, versionNoMinor, versionNoPatch, buildNo)
    const moduleInfo_t \
    SET_MODULE_NAME(moduleName) \
    = { (ro_string_t*)&moduleNameString, moduleId, {versionNoMajor, versionNoMinor, versionNoPatch}, buildNo } \
   #warning Unknown/undefined toolchain!
#endif

#endif /* _MODULE_INFO_H_ */
