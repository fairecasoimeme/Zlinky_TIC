###############################################################################
#
# MODULE:   Config.mk
#
# DESCRIPTION: ZBPro stack configuration. Defines tool, library and
#              header file details for building an app using the ZBPro stack
#
###############################################################################
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN514x, JN516x, JN517x].
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each  copy or partial copy of the software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright NXP B.V. 2015-2019. All rights reserved
#
###############################################################################

###############################################################################
# Tools

WWAH ?= 0
LEGACY ?= 0
OTA ?= 0
OTA_INTERNAL_STORAGE ?= 0
ifeq ($(OS),Windows_NT)
    PDUMCONFIG = $(TOOL_BASE_DIR)/PDUMConfig/bin/PDUMConfig.exe
    ZPSCONFIG  = $(TOOL_BASE_DIR)/ZPSConfig/bin/ZPSConfig.exe
else
    PDUMCONFIG = $(TOOL_BASE_DIR)/PDUMConfig/linuxbin/PDUMConfig
    ZPSCONFIG  = $(TOOL_BASE_DIR)/ZPSConfig/linuxbin/ZPSConfig
endif

ifneq ($(JENNIC_CHIP_FAMILY),JN518x) # start of legacy
STACK_SIZE ?= 5000
MINIMUM_HEAP_SIZE ?= 2000
###############################################################################
# ROM based software components

INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Mac/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/MicroSpecific/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/MiniMAC/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/MMAC/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/TimerServer/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Random/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZigbeeCommon/Include
ifeq ($(JENNIC_MAC), MAC)
    $(info JENNIC_MAC is MAC )
    APPLIBS +=ZPSMAC
    CFLAGS  += -DREDUCED_ZIGBEE_MAC_BUILD
    REDUCED_MAC_LIB_SUFFIX = ZIGBEE_
else
    $(info JENNIC_MAC is Mini MAC shim )
    JENNIC_MAC = MiniMacShim
    JENNIC_MAC_PLATFORM ?= SOC

###############################################################################
# Determine correct MAC library for platform

    ifeq ($(JENNIC_MAC_PLATFORM),SOC)
        $(info JENNIC_MAC_PLATFORM is SOC)
        APPLIBS +=ZPSMAC_Mini_SOC
    else
        ifeq ($(JENNIC_MAC_PLATFORM),SERIAL)
            $(info JENNIC_MAC_PLATFORM is SERIAL)
            APPLIBS +=ZPSMAC_Mini_SERIAL
            APPLIBS +=SerialMiniMacUpper
        else
            ifeq ($(JENNIC_MAC_PLATFORM),MULTI)
                 $(info JENNIC_MAC_PLATFORM is MULTI)
                 APPLIBS +=ZPSMAC_Mini_MULTI
                 APPLIBS +=SerialMiniMacUpper
            endif
       endif
    endif
endif


ifeq ($(WWAH),1)
    CFLAGS += -DWWAH_SUPPORT
endif

###############################################################################
# RAM based software components


CFLAGS += -DPDM_USER_SUPPLIED_ID
CFLAGS += -DPDM_NO_RTOS
ifeq ($(PDM_BUILD_TYPE),_EEPROM)
    CFLAGS += -DPDM$(PDM_BUILD_TYPE)
else
    ifeq ($(PDM_BUILD_TYPE),_EXTERNAL_FLASH)
        CFLAGS += -DPDM$(PDM_BUILD_TYPE)
    else
        ifeq ($(PDM_BUILD_TYPE),_NONE)
            CFLAGS += -DPDM$(PDM_BUILD_TYPE)
        else
            $(error PDM_BUILD_TYPE must be defined please define PDM_BUILD_TYPE=_EEPROM or PDM_BUILD_TYPE=_EXTERNAL_FLASH)
        endif
    endif
endif


# NB Order is significant for GNU linker

APPLIBS +=PWRM
APPLIBS +=ZPSTSV
APPLIBS +=AES_SW
APPLIBS +=PDUM
ifeq ($(WWAH),0)
    ifeq ($(LEGACY),0)
        APPLIBS +=ZPSAPL
    else
        APPLIBS +=ZPSAPL_LEGACY
        CFLAGS +=  -DLEGACY_SUPPORT
    endif
else
    APPLIBS +=ZPSAPL_WWAH
endif

APPLIBS +=Random


INCFLAGS += $(addsuffix /Include,$(addprefix -I$(COMPONENTS_BASE_DIR)/,$(APPLIBS)))
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/PDM/Include

ifneq ($(PDM_BUILD_TYPE),_NONE)
    APPLIBS +=PDM$(PDM_BUILD_TYPE)_NO_RTOS
endif

ifeq ($(TRACE), 1)
    CFLAGS  += -DDBG_ENABLE
    $(info Building trace version ...)
    APPLIBS +=DBG
else
    INCFLAGS += -I$(COMPONENTS_BASE_DIR)/DBG/Include
endif

ifeq ($(OPTIONAL_STACK_FEATURES),1)
    ifneq ($(ZBPRO_DEVICE_TYPE), ZED)
        APPLIBS += ZPSIPAN
    else
        APPLIBS += ZPSIPAN_ZED
    endif
endif

ifeq ($(OPTIONAL_STACK_FEATURES),2)
    ifneq ($(ZBPRO_DEVICE_TYPE), ZED)
        APPLIBS += ZPSGP
    else
        APPLIBS += ZPSGP_ZED
    endif
endif

ifeq ($(OPTIONAL_STACK_FEATURES),3)
    ifneq ($(ZBPRO_DEVICE_TYPE), ZED)
        APPLIBS += ZPSGP
        APPLIBS += ZPSIPAN
    else
        APPLIBS += ZPSGP_ZED
        APPLIBS += ZPSIPAN_ZED
    endif
endif


###############################################################################
# Paths to components provided as source

APPSRC += ZQueue.c
APPSRC += ZTimer.c
APPSRC += app_zps_link_keys.c
###############################################################################
# Paths to network and application layer libs for stack config tools

INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSMAC/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSNWK/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSAPL/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZigbeeCommon/Include
ifeq ($(ZBPRO_DEVICE_TYPE), ZCR)
    ifeq ($(WWAH),0)
        APPLIBS +=ZPSNWK
    else
        APPLIBS +=ZPSNWK_WWAH
    endif
else
    ifeq ($(ZBPRO_DEVICE_TYPE), ZED)
        ifeq ($(WWAH),0)
            APPLIBS +=ZPSNWK_ZED
        else
            APPLIBS +=ZPSNWK_WWAH_ZED
        endif
    else
        $(error ZBPRO_DEVICE_TYPE must be set to either ZCR or ZED)
    endif
endif

ifeq ($(ZBPRO_DEVICE_TYPE), ZCR)
    ifeq ($(WWAH),0)
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_$(JENNIC_CHIP_FAMILY).a
    else
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_WWAH_$(JENNIC_CHIP_FAMILY).a
    endif
endif
ifeq ($(ZBPRO_DEVICE_TYPE), ZED)
    ifeq ($(WWAH),0)
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_ZED_$(JENNIC_CHIP_FAMILY).a
    else
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_WWAH_ZED_$(JENNIC_CHIP_FAMILY).a
    endif
endif

ifeq ($(WWAH),0)
    ifeq ($(LEGACY),0)
        ZPS_APL_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSAPL_$(JENNIC_CHIP_FAMILY).a
    else
        ZPS_APL_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSAPL_LEGACY_$(JENNIC_CHIP_FAMILY).a
    endif
else
    ZPS_APL_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSAPL_WWAH_$(JENNIC_CHIP_FAMILY).a
endif
LDFLAGS += -Wl,--gc-sections

#############################LEGACY CHIP END########################################

else  #Configurations for JN518x

# For backwards compatibility during transition to new definitions. Once the
# following are defined in devices/<CHIP>/gcc/config.mk, these definitions can
# be removed from here
SDK_DEVICE_FAMILY ?= $(JENNIC_CHIP)
SDK_BOARD ?= jn5189dk6
PDM_NONE            ?= 0
SELOVERIDE          ?= 0
FRAMEWORK_SWITCH    ?= 0
JENNIC_CHIP_FAMILY  ?= JN518x
JENNIC_CHIP         ?= JN5189
SELOTA              ?= NONE
LEGACY              ?= 0
HEAP_SIZE           ?= 2000
SDK2_BASE_DIR        ?=  ../../../../../..
ifeq ($(FRAMEWORK_SWITCH),0)
ZIGBEE_BASE_DIR       = $(SDK2_BASE_DIR)/middleware/wireless/zigbee3.0
else
ZIGBEE_BASE_DIR       = $(SDK2_BASE_DIR)/middleware/wireless/zigbee
endif
COMPONENTS_BASE_DIR   = $(ZIGBEE_BASE_DIR)
FRAMEWORK_BASE_DIR   ?= $(SDK2_BASE_DIR)/middleware/wireless/framework

ifeq ($(FRAMEWORK_SWITCH),0)
BASE_FRAMEWORK_BASE_DIR ?= $(SDK2_BASE_DIR)/middleware/wireless/framework_5.3.2
else
BASE_FRAMEWORK_BASE_DIR   ?= $(SDK2_BASE_DIR)/middleware/wireless/framework
endif

ifeq ($(FRAMEWORK_SWITCH),0)
    SUBWCREV             ?= $(SDK2_BASE_DIR)/tools/zigbee_3.0/TortoiseSVN/bin/SubWCRev.exe
endif

TOOLCHAIN_PATH       ?= $(SDK2_BASE_DIR)/../MCUXpressoIDE_11.2.1_4149/ide/tools/bin
TOOLCHAIN_DIR_PATH   ?= $(SDK2_BASE_DIR)/../MCUXpressoIDE_11.2.1_4149/ide/tools
ifeq ($(FRAMEWORK_SWITCH),0)
TOOL_BASE_DIR        ?= $(SDK2_BASE_DIR)/tools/zigbee_3.0
else
TOOL_BASE_DIR        ?= $(SDK2_BASE_DIR)/middleware/wireless/zigbee/tools
endif
STACK_BASE_DIR       ?= $(ZIGBEE_BASE_DIR)/BuildConfig
ZIGBEE_COMMON_SRC    ?= $(ZIGBEE_BASE_DIR)/ZigbeeCommon/Source
KSDK2_PWRM_COMMON    ?= $(BASE_FRAMEWORK_BASE_DIR)/Common
OS_ABSTRACT_SRC      ?= $(BASE_FRAMEWORK_BASE_DIR)/OSAbstraction/Source
FSL_COMPONENTS       ?= $(SDK2_BASE_DIR)/components
DEVICE_SP_UTILITIES  ?= $(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/utilities
DEVICE_SP_DRIVERS    ?= $(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/drivers
GENERIC_LIST_SRC     ?= $(BASE_FRAMEWORK_BASE_DIR)/Lists
BOARD_LEVEL_SRC      ?= $(SDK2_BASE_DIR)/boards/$(SDK_BOARD)
CHIP_STARTUP_SRC     ?= $(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/gcc
CHIP_SYSTEM_SRC      ?= $(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)
FSL_EXCEPTIONS_SRC   ?= $(FRAMEWORK_BASE_DIR)/DebugExceptionHandlers_jn518x/src
DEBUG_FIFO_SRC       ?= $(FRAMEWORK_BASE_DIR)/DebugFifo/Source
FSL_EEPROM_INT       ?= $(FRAMEWORK_BASE_DIR)/Flash/Internal
FSL_EEPROM_GEN       ?= $(FRAMEWORK_BASE_DIR)/Flash/External/Source
FSL_OTA              ?= $(FRAMEWORK_BASE_DIR)/OtaSupport/Source
FRMWK_RNG_SRC        ?= $(FRAMEWORK_BASE_DIR)/RNG/Source
FRMWK_SECLIB_SRC     ?= $(FRAMEWORK_BASE_DIR)/SecLib
FRMWK_FUNCLIB_SRC    ?= $(FRAMEWORK_BASE_DIR)/FunctionLib
FRMWK_MSG_SRC        ?= $(FRAMEWORK_BASE_DIR)/Messaging/Source
FRMWK_MEMMGR_SRC     ?= $(FRAMEWORK_BASE_DIR)/MemManager/Source
FRMWK_TMRMGR_SRC     ?= $(FRAMEWORK_BASE_DIR)/TimersManager/Source
FRMWK_PANIC_SRC      ?= $(FRAMEWORK_BASE_DIR)/Panic/Source
ifeq ($(FRAMEWORK_SWITCH),0)
FRMWK_HEAP_SRC       ?= $(FRAMEWORK_BASE_DIR)/JennicCommon/Source
else
FRMWK_HEAP_SRC       ?= $(FRAMEWORK_BASE_DIR)/Common
FRMWK_LOWPOWER_SRC   ?= $(FRAMEWORK_BASE_DIR)/LowPower/Source/$(SDK_BOARD)
FRMWK_GPIO_SRC       ?= $(FRAMEWORK_BASE_DIR)/GPIO
endif
ifeq ($(OTA),1)
    ifeq ($(ZBPRO_DEVICE_TYPE), ZED)
        NXP_OTA_APP          ?= $(ZIGBEE_BASE_DIR)/ZigbeeCommon/SelectiveOtaApp1/JN518x_mcux/ReleaseEndDevice
    else
        NXP_OTA_APP          ?= $(ZIGBEE_BASE_DIR)/ZigbeeCommon/SelectiveOtaApp1/JN518x_mcux/Release
    endif
endif
JET_BASE            ?= $(TOOL_BASE_DIR)/JET/win
vector_table_size    ?= 512
__ram_vector_table__ ?= 1
##################################################################################
## Source included by default
ifeq ($(OTA),1)
    ifneq ($(SELOTA),NONE)
        CFLAGS += -D$(SELOTA)
        JN518X_TOOL = jn518x_image_tool.py -s 163840
        NXP_MANUFACTURER_CODE     ?= 0x1037
        ifeq ($(ZBPRO_DEVICE_TYPE), ZED)
            NXP_STRING                ?= ZBSTACK_JN5189_APP1_ENCRYPTEDZED
        else
            NXP_STRING                ?= ZBSTACK_JN5189_APP1_ENCRYPTEDZCR
        endif
        NXP_STACK_OTA_DEVICE_ID   ?= 0x0000
    endif
endif
ifneq ($(SELOTA),APP1)
ifeq ($(FRAMEWORK_SWITCH),1)
    LDLIBS += _crypto_m4
else
    APPLIBS += AES_SW
    APPLIBS += Random
endif
    APPSRC += ZQueue.c
    APPSRC += ZTimer.c
    APPSRC += app_zps_link_keys.c
    APPSRC += appZdpExtraction.c
    APPSRC += appZpsBeaconHandler.c
    APPSRC += appZpsExtendedDebug.c
    APPSRC += board.c
    APPSRC += clock_config.c
    APPSRC += system_$(SDK_DEVICE_FAMILY).c
    APPSRC += startup_$(SDK_DEVICE_FAMILY).c
    APPSRC += fsl_wwdt.c
    APPSRC += fsl_clock.c
    APPSRC += fsl_reset.c
    APPSRC += fsl_rng.c
    APPSRC += fsl_usart.c
    APPSRC += fsl_flexcomm.c
    APPSRC += fsl_flash.c
    APPSRC += fsl_common.c
    APPSRC += fsl_aes.c
    APPSRC += fsl_power.c
    APPSRC += fsl_wtimer.c
    APPSRC += fsl_inputmux.c
    APPSRC += fsl_fmeas.c
    APPSRC += Exceptions_NVIC.c
    APPSRC += MicroExceptions_arm.c
    APPSRC += Debug.c
    ifeq ($(OTA),1)
        APPSRC += OtaSupport.c
        APPSRC += psector_api.c
    endif
    APPSRC += FunctionLib.c
ifeq ($(FRAMEWORK_SWITCH),1)
    APPSRC += SecLib.c
    APPSRC += RNG.c
    APPSRC += GenericList.c
    APPSRC += Messaging.c
    APPSRC += MemManager.c
    APPSRC += TimersManager.c
    APPSRC += TMR_Adapter.c
    APPSRC += fsl_ctimer.c
    APPSRC += fsl_rtc.c
    APPSRC += Panic.c
    APPSRC += fsl_dma.c
    APPSRC += MicroInt_arm_sdk2.c
    APPSRC += PWR.c
    APPSRC += PWR_setjmp.S
    APPSRC += PWRLib.c
    APPSRC += GPIO_Adapter.c
    APPSRC += fsl_str.c
    APPSRC += serial_manager.c
    APPSRC += serial_port_uart.c
    APPSRC += usart_adapter.c
    APPSRC += fsl_assert.c
endif
    APPSRC += Heap.c
    ifeq ($(OTA),1)
        ifeq ($(OTA_INTERNAL_STORAGE),1)
            APPSRC += Eeprom_InternalFlash.c
        else
            APPSRC += Eeprom_MX25R8035F.c
            APPSRC += fsl_spifi.c
        endif
    endif
endif
ifeq ($(SELOTA),APP0)
    APPSRC += app_dependencies.c
    APPSRC += app_jumptable.S
    INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/Selective_OTA/Include
endif



ifeq ($(SELOTA),APP1)
    APPSRC += startup_jn5189_app1.c
endif
##################################################################################
## INCLUDE paths



include $(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/gcc/config.mk
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee/BuildConfig/ZBPro/Build/config_OSA.mk

INCFLAGS += -I$(COMPONENTS_BASE_DIR)/SerialMAC/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSMAC/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSNWK/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSTSV/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZigbeeCommon/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZPSAPL/Include
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/PWRM/Include
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/PDUM/Include
ifeq ($(FRAMEWORK_SWITCH),0)
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/MiniMac/Include
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/MiniMac/mMac/Include
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/MiniMac/uMac/Include
endif
INCFLAGS += -I$(GENERIC_LIST_SRC)
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/PDM/Include
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/DBG/Include

INCFLAGS += -I$(BOARD_LEVEL_SRC)
INCFLAGS += -I$(CHIP_SYSTEM_SRC)
INCFLAGS += -I$(FSL_EXCEPTIONS_SRC)/../inc
INCFLAGS += -I$(SEL_OTA_SRC)/../Include
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/DebugFifo/Include
INCFLAGS += -I$(BASE_FRAMEWORK_BASE_DIR)/OSAbstraction/Interface
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/FunctionLib
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/SecLib
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/RNG/Interface
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/MemManager/Interface
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/Panic/Interface
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/Messaging/Interface
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/OtaSupport/Source/../Interface/
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/TimersManager/Interface/
INCFLAGS += -I$(FSL_EEPROM_INT)
INCFLAGS += -I$(FSL_EEPROM_GEN)/../Interface/
INCFLAGS += -I$(FSL_EEPROM_INT)/../Interface/
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/Panic/Interface/
INCFLAGS += -I$(DEVICE_SP_DRIVERS)
INCFLAGS += -I$(FSL_COMPONENTS)/serial_manager/
INCFLAGS += -I$(FSL_COMPONENTS)/uart
ifeq ($(OTA),1)
    ifeq ($(OTA_INTERNAL_STORAGE),1)
        CFLAGS +=  -DgFlashEraseDuringWrite=0
        CFLAGS +=  -DgInvalidateHeaderLength=336
        CFLAGS += -DgEepromType_d=gEepromDevice_InternalFlash_c
        CFLAGS += -DgOtaVerifyWrite_d=0
        CFLAGS += -DgBootData_None_c=1
        CFLAGS += -DgEePromParams_BufferSize_c=512
        CFLAGS += -DgEepromParams_bufferedWrite_c=1
        CFLAGS += -DgEepromParams_CurrentOffset=0
    else
        CFLAGS += -DgEepromParams_ResetBitmap=1
        CFLAGS += -DgFlashEraseDuringWrite=0
        CFLAGS += -DgOtaVerifyWrite_d=0
        CFLAGS += -DgBootData_None_c=1
        CFLAGS += -DgEepromType_d=gEepromDevice_MX25R8035F_c
    endif
endif

ifeq ($(TRACE), 1)
    CFLAGS  += -DDBG_ENABLE
    CFLAGS  += -DDEBUG_ENABLE
    $(info Building trace version ...)
endif
##################################################################################
## LIBS
CFLAGS  += -DENABLE_RAM_VECTOR_TABLE

CFLAGS  += -DSDK_DEVICE_FAMILY=$(SDK_DEVICE_FAMILY)
ifneq ($(SELOVERIDE),1)
ifeq ($(FRAMEWORK_SWITCH),1)
CFLAGS  += -DgPWRM_Support=1
CFLAGS  += -DgSupportBle=0
CFLAGS  += -DcPWR_FullPowerDownMode=1
INCFLAGS += -I$(FRMWK_LOWPOWER_SRC)/../../Interface/$(SDK_BOARD)
INCFLAGS += -I$(FRAMEWORK_BASE_DIR)/GPIO
endif
endif

ifeq ($(FRAMEWORK_SWITCH),1)
CFLAGS  += -DZIGBEE_USE_FRAMEWORK=1
endif

ifneq ($(SELOTA),APP0)
    APPLIBS +=ZPSTSV
    APPLIBS +=PDUM
ifneq ($(PDM_NONE),1)
    APPLIBS +=PDM
endif
    ifeq ($(WWAH),0)
        ifeq ($(LEGACY),0)
            APPLIBS +=ZPSAPL
        else
            APPLIBS +=ZPSAPL_LEGACY
            CFLAGS +=  -DLEGACY_SUPPORT
        endif
    else
        APPLIBS +=ZPSAPL_WWAH
    endif
ifeq ($(JENNIC_MAC), MAC)
$(info JENNIC_MAC is MAC )
APPLIBS +=ZPSMAC
CFLAGS  += -DREDUCED_ZIGBEE_MAC_BUILD
REDUCED_MAC_LIB_SUFFIX = ZIGBEE_
else
$(info JENNIC_MAC is Mini MAC shim )
JENNIC_MAC = MiniMacShim
#APPLIBS +=ZPSMAC_Mini
JENNIC_MAC_PLATFORM ?= SOC
###############################################################################
# Determine correct MAC library for platform

ifeq ($(JENNIC_MAC_PLATFORM),SOC)
$(info JENNIC_MAC_PLATFORM is SOC)
APPLIBS +=ZPSMAC_Mini_SOC
else
ifeq ($(JENNIC_MAC_PLATFORM),SERIAL)
$(info JENNIC_MAC_PLATFORM is SERIAL)
APPLIBS +=ZPSMAC_Mini_SERIAL
APPLIBS +=SerialMiniMacUpper
else
ifeq ($(JENNIC_MAC_PLATFORM),MULTI)
$(info JENNIC_MAC_PLATFORM is MULTI)
APPLIBS +=ZPSMAC_Mini_MULTI
APPLIBS +=SerialMiniMacUpper
endif
endif
endif
endif
endif


ifneq ($(SELOTA),APP1)
    APPLIBS +=PWRM
endif

ifneq ($(SELOTA),NONE)
    APPLIBS +=Selective_OTA
endif

CFLAGS += -DPDM_USER_SUPPLIED_ID
CFLAGS += -DPDM_NO_RTOS
ifneq ($(SELOTA),APP0)
    ifeq ($(OPTIONAL_STACK_FEATURES),1)
        ifneq ($(ZBPRO_DEVICE_TYPE), ZED)
            APPLIBS += ZPSIPAN
        else
             APPLIBS += ZPSIPAN_ZED
        endif
    endif

    ifeq ($(OPTIONAL_STACK_FEATURES),2)
        ifneq ($(ZBPRO_DEVICE_TYPE), ZED)
            APPLIBS += ZPSGP
        else
            APPLIBS += ZPSGP_ZED
        endif
    endif

    ifeq ($(OPTIONAL_STACK_FEATURES),3)
        ifneq ($(ZBPRO_DEVICE_TYPE), ZED)
            APPLIBS += ZPSGP
            APPLIBS += ZPSIPAN
        else
            APPLIBS += ZPSGP_ZED
           APPLIBS += ZPSIPAN_ZED
        endif
    endif

    ifeq ($(ZBPRO_DEVICE_TYPE), ZCR)
        ifeq ($(WWAH),0)
            APPLIBS +=ZPSNWK
        else
            APPLIBS +=ZPSNWK_WWAH
        endif
    else
        ifeq ($(ZBPRO_DEVICE_TYPE), ZED)
            ifeq ($(WWAH),0)
                APPLIBS +=ZPSNWK_ZED
            else
                APPLIBS +=ZPSNWK_WWAH_ZED
            endif
        else
            $(error ZBPRO_DEVICE_TYPE must be set to either ZCR or ZED)
        endif
    endif
endif

ifeq ($(ZBPRO_DEVICE_TYPE), ZCR)
    ifeq ($(WWAH),0)
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK.a
    else
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_WWAH.a
    endif
endif

ifeq ($(ZBPRO_DEVICE_TYPE), ZED)
    ifeq ($(WWAH),0)
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_ZED.a
    else
        ZPS_NWK_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSNWK_WWAH_ZED.a
    endif
endif

ifeq ($(WWAH),0)
    ifeq ($(LEGACY),0)
        ZPS_APL_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSAPL.a
    else
        ZPS_APL_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSAPL_LEGACY.a
    endif
else
    ZPS_APL_LIB = $(COMPONENTS_BASE_DIR)/Library/libZPSAPL_WWAH.a
endif

ZIGBEE_BASE_SRC = $(OSA_BASE_SRC):$(ZIGBEE_COMMON_SRC):$(OS_ABSTRACT_SRC):$(GENERIC_LIST_SRC):$(BOARD_LEVEL_SRC)\
                  :$(CHIP_STARTUP_SRC):$(CHIP_SYSTEM_SRC):$(FSL_EXCEPTIONS_SRC):$(DEBUG_FIFO_SRC)\
                  :$(FRMWK_RNG_SRC):$(FRMWK_SECLIB_SRC):$(FRMWK_FUNCLIB_SRC):$(FRMWK_MSG_SRC)\
                  :$(ZIGBEE_COMMON_SRC)/../SelectiveOtaApp0/Source:$(FRMWK_MEMMGR_SRC)\
                  :$(FRMWK_TMRMGR_SRC):$(ZIGBEE_COMMON_SRC)/../SelectiveOtaApp1/Source\
                  :$(FSL_EEPROM_INT):$(FSL_OTA):$(FSL_EEPROM_GEN):$(FUNCTIONLIB):$(FRMWK_PANIC_SRC)\
                  :$(FRMWK_HEAP_SRC):$(FRMWK_LOWPOWER_SRC):$(FRMWK_GPIO_SRC):$(DEVICE_SP_DRIVERS)\
                  :$(FSL_COMPONENTS)/serial_manager:$(FSL_COMPONENTS)/uart:$(DEVICE_SP_UTILITIES)\
                  :$(DEVICE_SP_UTILITIES)/str:$(DEVICE_SP_UTILITIES)/debug_console

LDFLAGS += -L  $(COMPONENTS_BASE_DIR)/Library
ifeq ($(FRAMEWORK_SWITCH),1)
LDFLAGS += -Wl,--defsym,vector_table_size=$(vector_table_size)
LDFLAGS += -Wl,--defsym,__ram_vector_table__=$(__ram_vector_table__)
endif
LDFLAGS += -L  $(COMPONENTS_BASE_DIR)/BuildConfig/ZBPro/Build
LDFLAGS += -Wl,--defsym,HEAP_SIZE=$(HEAP_SIZE)
LDFLAGS += --specs=nosys.specs
LDFLAGS += -L  $(FRMWK_SECLIB_SRC)
endif


###############################################################################
