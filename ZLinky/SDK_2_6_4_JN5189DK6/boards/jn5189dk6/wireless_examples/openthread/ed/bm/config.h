/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _THREAD_END_DEVICE_CONFIG_H_
#define _THREAD_END_DEVICE_CONFIG_H_

/*!=================================================================================================
\file       config.h
\brief      This is the header file for the configuration of the Thread end device demo
            application.
==================================================================================================*/

/*==================================================================================================
CONFIG APPLICATION
==================================================================================================*/

#define APP_NAME_c                      "End Device Application Demo"

#define gLEDSupported_d                 1
#define OM15082

#define OT_USE_CLI                      1
#define OT_USE_SPINEL                   0
#define OT_ED_CONFIG                    1

/* Enable OTA client */
#define gEnableOTAClient_d              0

/* Defines the number of available keys for the keyboard module */
#if !(defined OM15082)
#define gKBD_KeysCount_c                2
#else
#define gKBD_KeysCount_c                3
#endif

/* Specifies the number of physical LEDs on the target board */
#define gLEDsOnTargetBoardCnt_c         2

#if (defined OM15082)
#define gLED_InvertedMode_d             1
#endif

#if gEnableOTAClient_d

/* Image type defines for OTA: 0x0000 - REED, 0x0001 - ED, 0x0002 - LPED
   Use value of gOtaCurrentImageTypeNo_c in Image Type text box in Test Tool's OTAP Thread.
   Define values must be kept in sync, for example on ED:
   #define gOtaCurrentImageType_c    0x01, 0x00
   #define gOtaCurrentImageTypeNo_c  0x0001
*/
#define gOtaCurrentImageType_c                  0x01, 0x00
#define gOtaCurrentImageTypeNo_c                0x0001

/* Current file version defines
   Value of gOtaCurrentFileVersionNo_c must be used in Test Tool's OTAP Thread */
#define gOtaCurrentFileVersion_c                0x05, 0x40, 0x03, 0x40
#define gOtaCurrentFileVersionNo_c              0x40034005

/* Specifies whether to use external EEPROM instead of internal for OTA */
#define gOTA_externalFlash_d            1
/* Specifies the type of EEPROM available on the target board */
#define gEepromType_d                   gEepromDevice_MX25R8035F_c
#define gEepromPostedOperations_d       1
#define gOtaEepromPostedOperations_d    1
#define gFlashEraseDuringWrite          0

#if (gFlashEraseDuringWrite!=0)
#define gFlashBlockBitmap_d
#endif

#define gOtaVerifyWrite_d               1 /* set to 2 if you wish to compare  the Ciphered - Written - Read - Deciphered buffers */
#define gBootData_None_c                1
#endif /* gEnableOTAClient_d */

/* Enable Latency measurement application */
#define gEnableLatencyMeasure_d         0

/* Enable uart app console */
#define gUartAppConsole_d               1
#define APP_SERIAL_INTERFACE_TYPE       gSerialMgrUsart_c

#define gUartDebugConsole_d             0
/************************************************************************************
*
*       CONFIG STACK
*
************************************************************************************/
/* Default channel */
#define OT_CHANNEL                                    11

/*!Setting 0 to the define makes the OpenThread stack use all channel ids from 1 to 26.
   Setting 0x07FFF800 means all 2.4GHz 16 channels are used (from channel id 11 to 26).
   This channel mask is used during network creation to select the best channel or during network discovery
   to find the available networks on that channels.
   To set the scan mask to a single channel, set the mask to 0x00000001 shifted with the channel ID
   e.g.: to set mask for channel 25, set OT_SCANCHANNEL_MASK to: (0x00000001 << 25) */
#ifndef OT_SCANCHANNEL_MASK
    #define OT_SCANCHANNEL_MASK                        (1 << OT_CHANNEL)
#endif

/*! The PAN identifier.
   If this value is 0xFFFF a random PAN ID will be generated on network creation */
#ifndef OT_PAN_ID
    #define OT_PAN_ID                                  THR_ALL_FFs16
#endif

/*! The device is out-of-band commissioned (the node is pre-configured). This means that the device
 * has all network parameters to directly attach to that network (e.g. master key, PSKc, mesh-local ULA,
 * extended PAN ID, Network name) */
#ifndef OT_DEV_IS_OUT_OF_BAND_CONFIGURED
    #define OT_DEV_IS_OUT_OF_BAND_CONFIGURED           FALSE
#endif

/*! Node Addresses - if TRUE, unique device ID is USED, otherwise will have OT_EXT_ADDR */
#ifndef OT_UNIQUE_EUI64_ADDR_ENABLED
    #define OT_UNIQUE_EUI64_ADDR_ENABLED               TRUE
#endif

/*! Default MAC extended address (IEEE address, long address or 64-bit MAC address) */
#ifndef OT_EUI64_ADDR
    #define OT_EUI64_ADDR                             {0x14,0x6E,0x0A,0x00,0x00,0x00,0x00,0x01}
#endif

/*! Default master key set if the current device creates a new network
    !!!WARNING!!! USE PRESET VALUE FOR TESTING ONLY!!!
    For Production Devices set to unique application define key at runtime */
#ifndef OT_MASTER_KEY
    #define OT_MASTER_KEY                             {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, \
                                                       0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}
#endif

/*! The default Device Passphrase (PSKd) used in commissioning procedure. The minimum length is 6
    and the maximum length is 32. It has to be encoded in base32-thread format:
    (0-9,A-Y excluding I,O,Q, and Z)
    !!!WARNING!!! USE PRESET VALUE FOR TESTING ONLY!!!
    For production devices, the application MUST set a UNIQUE PSKd comming for production data */
#ifndef OT_PSK_D
    #define OT_PSK_D                                  {"THREAD"}
#endif

/*! Rx on when Idle */
#define RX_ON_IDLE                                     TRUE

/*! Secure Data Requests */
#define SECURE_DATA_REQUESTS                           TRUE

/*! Device Type (FED - TRUE / MED - FALSE) */
#define DEVICE_TYPE                                    FALSE

/*! Network data (Full Network Data - TRUE / Stable Network Data - FALSE) */
#define NETWORK_DATA                                   TRUE


/************************************************************************************
*
*       CONFIG FRAMEWORK
*
************************************************************************************/
/* Set this define TRUE if the PIT frequency is an integer number of MHZ */
#define gTMR_PIT_FreqMultipleOfMHZ_d    0

/* Enable/Disable Periodical Interrupt Timer */
#define gTMR_PIT_Timestamp_Enabled_d    0

/* Enables / Disables the precision timers platform component */
#define gTimestamp_Enabled_d            1

/* Use Ctimer instance 1, instance 0 is used by Open Thread stack */
#define gStackTimerInstance_c           1

/* Use Toolchain Memory functions in MemManager */
#define gUseToolchainMemFunc_d          1

#define gSerialMgrUseCustomInterface_c  0
#define gSerialMgrUseUart_c             1

/* PDM is required by Open Thread stack*/
#define gUsePdm_d                       1

/*! The APP memory pool ID */
#define AppPoolId_d 0

/*! Application pools configuration */
#ifndef AppPoolsDetails_c
    #define AppPoolsDetails_c\
              _block_size_  16      _number_of_blocks_  10  _pool_id_(AppPoolId_d)  _eol_  \
              _block_size_  68      _number_of_blocks_  8   _pool_id_(AppPoolId_d)  _eol_  \
              _block_size_  160     _number_of_blocks_  4   _pool_id_(AppPoolId_d)  _eol_  \
              _block_size_  260     _number_of_blocks_  2   _pool_id_(AppPoolId_d)  _eol_  \
              _block_size_  800     _number_of_blocks_  2   _pool_id_(AppPoolId_d)  _eol_
#endif

/*! *********************************************************************************
 *     Auxilary PoolID Configuration
 ********************************************************************************** */
#if gAppUseNvm_d
  #define gNvmMemPoolId_c 1
  #if gUsePdm_d
    #define gPdmMemPoolId_c 2
    #if gOtaEepromPostedOperations_d
      #define gOtaMemPoolId_c 3
    #endif
  #else
    #if gOtaEepromPostedOperations_d
      #define gOtaMemPoolId_c 2
    #endif
  #endif
#else
  #if gUsePdm_d
    #define gPdmMemPoolId_c 1
    #if gOtaEepromPostedOperations_d
      #define gOtaMemPoolId_c 2
    #endif
  #else
    #if gOtaEepromPostedOperations_d
      #define gOtaMemPoolId_c 1
    #endif
  #endif
#endif

#if gUsePdm_d
   #define gPdmNbSegments         63 /* number of sectors contained in PDM storage */
   #define PdmInternalPoolsDetails_c \
        _block_size_ 512                  _number_of_blocks_  2 _pool_id_(gPdmMemPoolId_c) _eol_ \
        _block_size_ (gPdmNbSegments*12)  _number_of_blocks_  1 _pool_id_(gPdmMemPoolId_c) _eol_
#endif

#if gOtaEepromPostedOperations_d
#if gOTA_externalFlash_d
    #define gUsePasswordCiphering_d 0
    #define gExternalFlashIsCiphered_d 0
    #define PROGRAM_PAGE_SZ (256)
    #define  OTA_NB_PENDING_TRANSACTIONS (4)
#endif

    #define OTA_TRANSACTION_BUFFER_SZ (PROGRAM_PAGE_SZ + 12)

   /* Defines NVM pools by block size and number of blocks. Must be aligned to 4 bytes.*/
    #define OtaPoolDetails_c \
         _block_size_    OTA_TRANSACTION_BUFFER_SZ    _number_of_blocks_    OTA_NB_PENDING_TRANSACTIONS    _pool_id_(gOtaMemPoolId_c)    _eol_
#endif /* gOtaEepromPostedOperations_d */

/*! *********************************************************************************
 *     Memory Pools Configuration
 ********************************************************************************** */
/* Defines pools by block size and number of blocks. Must be aligned to 4 bytes.
 * DO NOT MODIFY THIS DIRECTLY. CONFIGURE AppPoolsDetails_c
 * If gMaxBondedDevices_c increases, adjust NvmPoolsDetails_c
*/
#if gAppUseNvm_d
  #if gOtaEepromPostedOperations_d
    #define PoolsDetails_c \
         AppPoolsDetails_c \
         NvmPoolsDetails_c \
         PdmInternalPoolsDetails_c \
         OtaPoolDetails_c
  #else
    #define PoolsDetails_c \
         AppPoolsDetails_c \
         NvmPoolsDetails_c \
         PdmInternalPoolsDetails_c
  #endif
#elif gUsePdm_d /* Open Thread uses PDM but no NVM over PDM */
  #if gOtaEepromPostedOperations_d
    #define PoolsDetails_c \
         AppPoolsDetails_c \
         PdmInternalPoolsDetails_c \
         OtaPoolDetails_c
  #else
    #define PoolsDetails_c \
         AppPoolsDetails_c \
         PdmInternalPoolsDetails_c
  #endif
#else /* gUsePdm_d not defined */
  #if gOtaEepromPostedOperations_d
  #define PoolsDetails_c     \
         AppPoolsDetails_c \
         OtaPoolDetails_c
  #else
  #define PoolsDetails_c     \
         AppPoolsDetails_c
  #endif
#endif

/************************************************************************************
*
*       CONFIG Mac/Phy
*
************************************************************************************/

/************************************************************************************
*
*      COMMON MODULES CONFIGURATIONS FILES
*
************************************************************************************/


/************************************************************************************
*
*      CHECKS
*
************************************************************************************/


#endif /* _THREAD_END_DEVICE_CONFIG_H_ */
