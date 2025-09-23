###############################################################################
#
# MODULE:   Makefile
#
# DESCRIPTION: Makefile for the Router
# 
############################################################################
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169
# JN5179, JN5189].
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each copy or partial copy of the
# software.
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
# Copyright NXP B.V. 2017-2019. All rights reserved
#
############################################################################

# Application target name
TARGET = ZLinky

###############################################################################
#User definable make parameters that may be overwritten from the command line

ZBPRO_DEVICE_TYPE       = ZCR
#ZBPRO_DEVICE_TYPE       = ZED
PDM_BUILD_TYPE         ?= _EEPROM
BAUD                   ?= 115200
DEBUG                  ?= NONE
HEAP_SIZE              ?= 0xFA0
GP_SUPPORT ?= 1
GP_DEVICE ?= PROXY_BASIC

APP_CLUSTERS_SMART_ENERGY_SRC ?=1
APP_CLUSTERS_MEASUREMENT_AND_SENSING_SRC ?=1

# Set specific chip	(choose one)
#JENNIC_CHIP           ?= K32W061
#JENNIC_CHIP           ?= K32W041A
#JENNIC_CHIP           ?= K32W041AM
#JENNIC_CHIP           ?= K32W061
#JENNIC_CHIP           ?= K32W041
JENNIC_CHIP           ?= JN5189
#JENNIC_CHIP           ?= JN5188

# Legacy chip family (always JN518x)
JENNIC_CHIP_FAMILY     ?= JN518x

# K32WMCM SDK settings                            
ifeq ($(findstring K32W041A,$(JENNIC_CHIP)),K32W041A)
#Prevent NTAG related functions
override APP_NCI_ICODE = 0
override APP_NCI_OTA = 0
override APP_NTAG_OTA = 0
override APP_NTAG_NWK = 0
override APP_NTAG_ICODE = 0
#Enable MCM +15dbm Tx power in application notes
CFLAGS += -DK32WMCM_APP_BUILD
# Chip/Device                                     
ifeq ($(JENNIC_CHIP),K32W041A)            
SDK_DEVICE_FAMILY      ?= K32W041A
CFLAGS += -DCPU_K32W041AK
else                                      
SDK_DEVICE_FAMILY      ?= K32W041AM
CFLAGS += -DCPU_K32W041AMK
endif                                     
# Board                                           
ifeq ($(JENNIC_CHIP),K32W041A)            
SDK_BOARD      ?= k32w041adk6
else                                      
SDK_BOARD      ?= k32w041amdk6
endif                                     
# SDK folder                                      
ifeq ($(JENNIC_CHIP),K32W041A)            
SDK_FOLDER      ?= K32W041ADK6
else                                      
SDK_FOLDER      ?= K32W041AMDK6
endif                                     
# OTA Chip string                                 
OTA_CHIP_STRING         = K32WMCM
endif                                             

# K32W0xx SDK settings
ifeq ($(findstring K32W061,$(JENNIC_CHIP)),K32W061)
# Chip/Device
SDK_DEVICE_FAMILY      ?= K32W061
# Board
SDK_BOARD              ?= k32w061dk6
# SDK folder
SDK_FOLDER             ?= K32W061DK6
# OTA Chip string
OTA_CHIP_STRING         = $(JENNIC_CHIP)
endif

# JN518x SDK settings
ifeq ($(findstring JN518,$(JENNIC_CHIP)),JN518)
# Chip/Device
SDK_DEVICE_FAMILY      ?= JN5189
# Board
SDK_BOARD              ?= jn5189dk6
# SDK folder
SDK_FOLDER             ?= SDK_2_6_4_JN5189DK6
# OTA Chip string
OTA_CHIP_STRING         = $(JENNIC_CHIP)-
endif

ENDIAN                 = LITTLE_ENDIAN_PROCESSOR
DISABLE_LTO            = 1

OTA ?= 0 
OTA_ENCRYPTED ?= 0
LIMITED ?= 0
VERSION = 17
###############################################################################
# Network settings

# Optional network.mk for channel
#sinclude ../../../network.mk

# Channel (0 for default channels)
SINGLE_CHANNEL ?= 0
CFLAGS         += -DSINGLE_CHANNEL=$(SINGLE_CHANNEL)

###############################################################################
# Default DK4 development kit target hardware

DR ?= OM15081

ifeq ($(DR),OM15081)
OTA_HW = OM15081
endif

MANUFACTURER_CODE = 0x1037
ifeq ($(OTA_ENCRYPTED),0)
	ifeq ($(LIMITED),0)
	    OTA_DEVICE_ID = 0x0001
	    OTA_STRING = $(OTA_HW)-RTR-$(OTA_CHIP_STRING)0000000000000
    else
	    OTA_DEVICE_ID = 0x0002
	    OTA_STRING = $(OTA_HW)-RTR-LIMITED-$(OTA_CHIP_STRING)00000
    endif
    
else
    OTA_DEVICE_ID = 0x8001
    OTA_STRING = $(OTA_HW)-RTR-$(OTA_CHIP_STRING)--ENCRYPTED00
endif

ifeq ($(OTA),1)
    CFLAGS  += -DBUILD_OTA
    CFLAGS += -DCLD_OTA_MANF_ID_VALUE=$(MANUFACTURER_CODE)
    APP_CLUSTERS_OTA_SRC ?= 1
    OTA_ENCRYPTED ?= 0
    ifeq ($(OTA_ENCRYPTED),1)
        CFLAGS  += -DOTA_ENCRYPTED
        ENCRYPTION_KEY = 1234567890ABCDEFA1B2C3D4E5F6F1B4
        CFLAGS += -DCLD_OTA_KEY=\"$(ENCRYPTION_KEY)\"
    endif
    OTA_INTERNAL_STORAGE ?= 1
    ifeq ($(OTA_INTERNAL_STORAGE),1)
        CFLAGS  += -DOTA_INTERNAL_STORAGE
        ifeq ($(OTA_ENCRYPTED),1)
             CFLAGS += -DINTERNAL_ENCRYPTED
        endif
    endif
endif

CFLAGS  += -DBUTTON_MAP_$(DR)

#Always tell any actual drivers they're running under an RTOS in this usage
CFLAGS  += -DRTOS

# Tell the code what target hardware is  
CFLAGS  += -D$(DR)

#NFC FD test
#CFLAGS  += -DAPP_NFC_FD_TEST

# NFC NTAG ICODE (new) functionality
APP_NTAG_NWK ?= 0
APP_NTAG_OTA ?= 0
ifeq ($(APP_NTAG_NWK), 1)
CFLAGS  += -DAPP_NTAG_NWK
ifeq ($(APP_NTAG_OTA), 1)
CFLAGS  += -DAPP_NTAG_OTA
endif
endif

ifeq ($(OTA),1)
    BUILD_OTA_FILES = 1
else
    ifeq ($(APP_NTAG_OTA), 1)
        BUILD_OTA_FILES = 1
    else
        BUILD_OTA_FILES = 0
    endif
endif

RF_COMMANDS ?= 0
ifeq ($(RF_COMMANDS), 1)
CFLAGS += -DRF_COMMANDS
CFLAGS += -DMAC_FILTER_INCOMING_DATA
endif

# 32k clock configuration (1 = FRO, 0 = XTAL)
CLK_32K = 1
ifeq ($(CLK_32K), 1)
CFLAGS += -DgClkUseFro32K=1
CFLAGS += -DgPWR_UseAlgoTimeBaseDriftCompensate=1
# 32k FRO automatic calibration (0 = disable, 1 = enable)
CFLAGS += -DgClkRecalFro32K=0
else
CFLAGS += -DgClkUseFro32K=0
endif

# Use LowPower APIs (PWRM is used if not defined)
CFLAGS += -DAPP_LOW_POWER_API

###############################################################################
# Select the network stack (e.g. MAC, ZBPro, ZCL)

###############################################################################
# ZBPro Stack specific options
ZBPRO_DEVICE_TYPE ?= ZCR
#ZBPRO_DEVICE_TYPE ?= ZED
#PDM_BUILD_TYPE ?= _EXTERNAL_FLASH
PDM_BUILD_TYPE ?=_EEPROM

# BDB features Enable as required
#BDB_SUPPORT_TOUCHLINK_INITIATOR ?= 1
#BDB_SUPPORT_TOUCHLINK_TARGET ?= 1
BDB_SUPPORT_NWK_STEERING ?= 1
BDB_SUPPORT_NWK_FORMATION ?= 1
#BDB_SUPPORT_FIND_AND_BIND_INITIATOR ?= 1
BDB_SUPPORT_FIND_AND_BIND_TARGET ?= 1
ifeq ($(APP_NTAG_NWK), 1)
BDB_SUPPORT_OOBC ?= 1
endif

# Framework features 
FRAMEWORK_SWITCH = 1

###############################################################################
# commands options
# enable optional commands as required per applicaion.
#
#NETWORK_RECOVERY       ?=	0
STACK_MEASURE          ?= 0
#
###############################################################################
# Debug options - define DEBUG to enable
# DEBUG ?=HW
#
# Define which UART to use for HW debug
#DEBUG_PORT ?= UART1
#  DEBUG_MODE  ?=  SWD    # For ARM core

###############################################################################
# Define TRACE to use with DBG module
TRACE ?=1
ifeq ($(TRACE), 1)
CFLAGS += -DDBG_ENABLE
CFLAGS += -DPRINTF_ADVANCED_ENABLE

# Enable any debug output here:
#CFLAGS += -DDEBUG_APP
#CFLAGS += -DDEBUG_APP_BDB
#CFLAGS += -DDEBUG_APP_EVENT
#CFLAGS += -DDEBUG_BDB
#CFLAGS += -DNXP_HARDWARE
#CFLAGS += -DDEBUG_ZCL
#CFLAGS +=  -DDEBUG_EXCEPTIONS
#CFLAGS += -DDEBUG_CLD_IDENTIFY 
#CFLAGS += -DDEBUG_CLD_ONOFF
#CFLAGS += -DDEBUG_CLD_GROUPS
CFLAGS += -DDEBUG_APP_OTA
#CFLAGS  += -DSTRICT_PARAM_CHECK
CFLAGS += -DDEBUG_APP_BUTTON
#CFLAGS += -DDEBUG_APP_NTAG
#CFLAGS += -DDEBUG_APP_GP
#CFLAGS += -DTRACE_GP_DEBUG
#CFLAGS += -DDEBUG_RF_COMMANDS
CFLAGS += -DDEBUG_LINKY
CFLAGS += -DDEBUG_REPORT
endif

ifeq ($(LIMITED), 1)
	CFLAGS += -DLIMITED
endif
CFLAGS += -D$(TARGET)

ICODES ?= 0
ifeq ($(ICODES), 1)
CFLAGS  += -DBDB_JOIN_USES_INSTALL_CODE_KEY=1
endif

###############################################################################
# Path definitions

# Use if application directory contains multiple targets

# Toolchain paths
TOOLCHAIN_DIR_PATH  = C:/NXP/MCUXpressoIDE_11.2.0_4120/ide/tools
TOOLCHAIN_PATH      = $(TOOLCHAIN_DIR_PATH)/bin

# SDK paths
SDK2_BASE_DIR       = $(APP_BASE)/$(TARGET)/$(SDK_FOLDER)

# Application paths
APP_BASE            =  ../../..
APP_BLD_DIR         = $(APP_BASE)/$(TARGET)/Build/jn518x_mcux
APP_SRC_DIR         = $(APP_BASE)/$(TARGET)/Source
APP_COMMON_SRC_DIR  = $(APP_BASE)/Common/Source
BOARD_DIR           = $(APP_BASE)/Common/Source/board
AWK_SRC             = $(APP_BASE)/Common/Awk

# Tool paths
AWK_EXE             = $(APP_BASE)/../../gawk-3.1.6-1-bin/bin/gawk.exe

###############################################################################
# Application Source files

# Note: Path to source file is found using vpath below, so only .c filename is required
APPSRC = app_main.c
APPSRC += port_JN518x.c
APPSRC += pdum_gen.c
APPSRC += zps_gen.c
APPSRC += app_start.c
APPSRC += app_router_node.c
APPSRC += app_zcl_task.c
APPSRC += app_buttons.c
APPSRC += app_leds.c
APPSRC += fsl_debug_console.c
APPSRC += fsl_gpio.c
APPSRC += hardware_init.c
APPSRC += pin_mux.c
APPSRC += fsl_pwm.c
APPSRC += fsl_gint.c
APPSRC += pdum_apdu.S
APPSRC += app_power_on_counter.c
APPSRC += temp_sensor_drv.c
APPSRC += voltage_drv.c
APPSRC += fsl_adc.c
APPSRC += app_reporting.c
APPSRC += board_utility.c
APPSRC += app_blink_led.c
APPSRC += app_uartlinky.c
APPSRC += App_Linky.c
APPSRC += LixeeCluster.c
APPSRC += TuyaSpecific.c
APPSRC += TuyaSpecificCommandHandler.c
APPSRC += meter_identification.c



ifeq ($(APP_NTAG_NWK),1)
APPSRC += app_ntag.c
APPSRC += fsl_i2c.c
APPSRC += fsl_ntag.c
endif

ifeq ($(OTA),1)
APPSRC += app_ota_client.c
endif

ifeq ($(GP_SUPPORT), 1)
APPSRC += app_green_power.c
endif

ifeq ($(JENNIC_CHIP),K32W041AM)
APPSRC += fsl_spifi.c
APPSRC += Eeprom_MX25R8035F_User.c
endif

ifeq ($(LIMITED),0)
	APP_ZPSCFG = app.zpscfg
else
	APP_ZPSCFG = app_limited.zpscfg	
endif

OPTIONAL_STACK_FEATURES = $(shell $(ZPSCONFIG) -n $(TARGET) -f $(APP_COMMON_SRC_DIR)/$(APP_ZPSCFG) -y )

ifeq ($(GP_SUPPORT), 1)
ifeq ($(GP_DEVICE), PROXY_BASIC)
CFLAGS  +=-DGP_PROXY_BASIC_DEVICE
endif
CFLAGS  += -DGP_CERITIFICATION_TEST
CFLAGS  += -DCLD_GREENPOWER
APP_CLUSTERS_GREENPOWER_SRC ?=1
endif 

###############################################################################
# Configure for the selected chip or chip family
ifeq ($(FRAMEWORK_SWITCH), 1)
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee/BuildConfig/ZCL/Build/config_ZCL.mk
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee/BDB/Build/config.mk
else
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee3.0/BuildConfig/ZCL/Build/config_ZCL.mk
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee3.0/BDB/Build/config.mk
endif

# compatibility for SDK 2.6.8 and newer verions
ZPSCONFIG           = $(TOOL_BASE_DIR)/ZPSConfig/bin/ZPSConfig.exe
PDUMCONFIG          = $(TOOL_BASE_DIR)/PDUMConfig/bin/PDUMConfig.exe

###############################################################################
# Standard Application header search paths

INCFLAGS += -I$(APP_SRC_DIR)
INCFLAGS += -I$(APP_COMMON_SRC_DIR)
INCFLAGS += -I$(BOARD_DIR)


###############################################################################
# Application libraries
# Specify additional Component libraries

#APPLIBS += 

###############################################################################
# NTAG ICODE library
ifeq ($(APP_NTAG_NWK),1)
# NTAG library folder
#APP_NTAG_LIB_DIR   = $(SDK2_BASE_DIR)/middleware/wireless/framework/NFC
APP_NTAG_LIB_DIR  = $(APP_BASE)/NFC
# NTAG library
LDLIBS           += NTAG_$(JENNIC_CHIP_FAMILY)
# NTAG library path
#LDFLAGS          += -L$(APP_NTAG_LIB_DIR)/JN518x/Library
LDFLAGS          += -L$(APP_NTAG_LIB_DIR)/Library
# NTAG include path
INCFLAGS         += -I$(APP_NTAG_LIB_DIR)/Include
endif

###############################################################################
# Construct strings for output file names

# Construct binary features list 
ifeq ($(APP_NTAG_NWK),1)
TARGET_FEATURES:=$(TARGET_FEATURES)_NtagNwk
ifeq ($(APP_NTAG_OTA),1)
TARGET_FEATURES:=$(TARGET_FEATURES)_NtagOta
endif
endif
ifeq ($(GP_SUPPORT), 1)
ifeq ($(GP_DEVICE), PROXY_BASIC)
TARGET_FEATURES:=$(TARGET_FEATURES)_GpProxy
endif
endif
ifeq ($(OTA),1)
TARGET_FEATURES:=$(TARGET_FEATURES)_Ota
ifeq ($(OTA_ENCRYPTED),1)
TARGET_FEATURES:=$(TARGET_FEATURES)Enc
endif
endif

# Construct binary hardware list
TARGET_HARDWARE:=_$(DR)
ifneq ($(SINGLE_CHANNEL), 0)
TARGET_HARDWARE:=$(TARGET_HARDWARE)_CH$(SINGLE_CHANNEL)
endif

ifeq ($(LIMITED),0)
	TARGET_FULL=$(TARGET)$(TARGET_FEATURES)
else
	TARGET_FULL=$(TARGET)$(TARGET_FEATURES)_LIMITED
endif

#TARGET_FULL=$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)
ifeq ($(DUAL_BOOT),1)
TARGET_FULL:=Ssbl_$(TARGET_FULL)
endif

$(info Creating folders...)
APP_OUT_DIR = $(APP_BASE)/Binaries/$(TARGET_FULL)
APP_OBJ_DIR = $(APP_BASE)/Binaries/$(TARGET_FULL)/obj
$(info $(shell mkdir -p $(APP_OUT_DIR)))
$(info $(shell mkdir -p $(APP_OBJ_DIR)))


###############################################################################

# You should not need to edit below this line

###############################################################################


###############################################################################

TEMP = $(APPSRC:.c=.o)
APPOBJS_TMP = $(TEMP:.S=.o)
APPOBJS := $(addprefix $(APP_OBJ_DIR)/,$(APPOBJS_TMP))

###############################################################################
# Application dynamic dependencies

APPDEPS_TMP = $(APPOBJS_TMP:.o=.d)
APPDEPS := $(addprefix $(APP_OBJ_DIR)/,$(APPDEPS_TMP))

###############################################################################
# Linker

# Add application libraries before chip specific libraries to linker so
# symbols are resolved correctly (i.e. ordering is significant for GCC)

LDLIBS := $(APPLIBS) $(LDLIBS)

IMAGE_TOOL = py -3 $(SDK2_BASE_DIR)/tools/imagetool/dk6_image_tool.py
ifeq ($(DUAL_BOOT),1)
    CFLAGS += -DDUAL_BOOT
    LDFLAGS += -Wl,--defsym=BUILD_ZIGBEE_STACK=1
    IMAGE_TOOL_CONFIGURATION = -v=1 -i=1 -s 323584 -t 16384
else
    ifeq ($(SELOTA),APP0)
        IMAGE_TOOL_CONFIGURATION = -s 163840
    else
        IMAGE_TOOL_CONFIGURATION = -s 294912
    endif
endif

###############################################################################
# Dependency rules

.PHONY: all clean
# Path to directories containing application source 
vpath % $(APP_SRC_DIR):$(APP_COMMON_SRC_DIR):$(ZCL_SRC_DIRS):$(ZCL_SRC):$(BDB_SRC_DIR):$(BOARD_DIR):$(ZIGBEE_BASE_SRC):$(NTAG_SRC)

ifeq ($(BUILD_OTA_FILES),1)
    all: $(APP_OUT_DIR)/$(TARGET_FULL)_V3.ota
else
    all: $(APP_OUT_DIR)/$(TARGET_FULL).axf
endif

$(APP_SRC_DIR)/pdum_gen.c $(APP_SRC_DIR)/pdum_gen.h: $(APP_COMMON_SRC_DIR)/$(APP_ZPSCFG) $(PDUMCONFIG)
	$(info Configuring the PDUM ...)
	$(PDUMCONFIG) -z $(TARGET) -e $(ENDIAN) -f $< -o $(APP_SRC_DIR)
	@echo

$(APP_SRC_DIR)/zps_gen.c $(APP_SRC_DIR)/zps_gen.h: $(APP_COMMON_SRC_DIR)/$(APP_ZPSCFG) $(ZPSCONFIG)
	$(info Configuring the Zigbee Protocol Stack ...)
	$(ZPSCONFIG) -n $(TARGET) -t $(JENNIC_CHIP) -l $(ZPS_NWK_LIB) -a $(ZPS_APL_LIB) -c $(TOOLCHAIN_DIR_PATH) -e $(ENDIAN) -f $< -o $(APP_SRC_DIR)

$(APP_OBJ_DIR)/app_main.o: app_main.c 
	$(info Compiling $< into $@...)
	$(TOOLCHAIN_PATH)/$(CC) -c -o $@ $(CFLAGS) $(INCFLAGS) $< -MD -MF $(APP_OBJ_DIR)/app_main.d -MP
	@echo Source Files > $(APP_OUT_DIR)/$(TARGET_FULL).log 
	@echo $@ $< >> $(APP_OUT_DIR)/$(TARGET_FULL).log 
	@echo
	
$(APP_OBJ_DIR)/%.o: %.c 
	$(info Compiling $< into $@...)
	$(TOOLCHAIN_PATH)/$(CC) -c -o $@ $(CFLAGS) $(INCFLAGS) $< -MD -MF $(APP_OBJ_DIR)/$*.d -MP
	@echo $@ $< >> $(APP_OUT_DIR)/$(TARGET_FULL).log 
	@echo

$(APP_OBJ_DIR)/%.o: %.S
	$(info Assembling $< into $@...)
	$(TOOLCHAIN_PATH)/$(CC) -c -o $@ $(CFLAGS) $(INCFLAGS) $< -MD -MF $(APP_OBJ_DIR)/$*.d -MP
	@echo $@ $< >> $(APP_OUT_DIR)/$(TARGET_FULL).log 
	@echo

$(APP_OUT_DIR)/$(TARGET_FULL).axf: $(APPOBJS) $(addsuffix.a,$(addprefix $(COMPONENTS_BASE_DIR)/Library/lib,$(APPLDLIBS))) 
	$(info Linking $@ ...)
	$(TOOLCHAIN_PATH)/$(CC)  -Wl,--gc-sections $(LDFLAGS) -T "AppBuildNone_$(JENNIC_CHIP).ld" -o $@ -Wl,--start-group $(APPOBJS) $(addprefix -l,$(LDLIBS)) -Wl,--end-group -Wl,-Map,$(APP_OUT_DIR)/$(TARGET_FULL).map 
	$(info Analysing output ...)
	$(TOOLCHAIN_PATH)/$(SIZE) $@  
	date +%c >> $(APP_BASE)/Doc/size.txt
	$(TOOLCHAIN_PATH)/$(SIZE) $@ >> $(APP_BASE)/Doc/size.txt
	-$(AWK_EXE) -f $(AWK_SRC)/JN51xx_MAP.awk $(APP_OUT_DIR)/$(TARGET_FULL).log $(APP_OUT_DIR)/$(TARGET_FULL).map > $(APP_OBJ_DIR)/temp.txt
	$(info Generating binary ...)
	$(IMAGE_TOOL) $(IMAGE_TOOL_CONFIGURATION) $(APP_OUT_DIR)/$(TARGET_FULL).axf 
	rm -f $(APP_BLD_DIR)/data.bin
	$(TOOLCHAIN_PATH)/$(OBJCOPY) -v -O binary $(APP_OUT_DIR)/$(TARGET_FULL).axf $(APP_OUT_DIR)/$(TARGET_FULL).bin
	$(TOOLCHAIN_PATH)/$(OBJDUMP) -d $(APP_OUT_DIR)/$(TARGET_FULL).axf > $(APP_OUT_DIR)/$(TARGET_FULL).dis

ifeq ($(BUILD_OTA_FILES),1)	
$(info Generating OTA binary ...)
ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
     JET_VERSION=4
else ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
     JET_VERSION=5
else ifeq ($(JENNIC_CHIP_FAMILY), JN518x)
     JET_VERSION=6
endif

$(APP_OUT_DIR)/$(TARGET_FULL)_V3.ota: $(APP_OUT_DIR)/$(TARGET_FULL).axf

# Unencrypted OTA?
ifeq ($(OTA_ENCRYPTED),0)

	# JET add serialisation data for unencrypted devices
	$(JET_BASE)/JET.exe -m combine -f $(APP_OUT_DIR)/$(TARGET_FULL).bin -x configOTA_Cer_Keys_HA.txt -g 1 -v $(JET_VERSION) -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) -k 0xffffffffffffffffffffffffffffffff --sector_size=512 
	# Create an Unencrypted Bootable Client with Version 1
	$(JET_BASE)/JET.exe -m otamerge --embed_hdr -c outputffffffffffffffff.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -v $(JET_VERSION) -n 1 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512

	# Modify Embedded Header to reflect version 2 
	$(JET_BASE)/JET.exe -m otamerge --embed_hdr -c $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V2.bin -v $(JET_VERSION) -n $(VERSION) -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512
	# Wrap the Image with OTA header with version 2
	$(JET_BASE)/JET.exe -m otamerge --ota -c $(APP_OUT_DIR)/$(TARGET_FULL)_V2.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V$(VERSION).ota -p 1 -v $(JET_VERSION) -n $(VERSION) -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512

	# Modify Embedded Header to reflect version 3 
	#$(JET_BASE)/JET.exe -m otamerge --embed_hdr -c $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V3.bin -v $(JET_VERSION) -n 3 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512
	# Wrap the Image with OTA header with version 3
	#$(JET_BASE)/JET.exe -m otamerge --ota -c $(APP_OUT_DIR)/$(TARGET_FULL)_V3.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V3.ota -p 1 -v $(JET_VERSION) -n 3 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512

	# Remove intermediate files
	rm $(APP_OUT_DIR)/$(TARGET_FULL).bin
	rm outputffffffffffffffff.bin
	rm $(APP_OUT_DIR)/$(TARGET_FULL)_V2.bin
	rm $(APP_OUT_DIR)/$(TARGET_FULL)_V5.bin

# Encrypted OTA?
else

	# JET add serialisation data for encrypted devices
	$(JET_BASE)/JET.exe -m combine -f $(APP_OUT_DIR)/$(TARGET_FULL).bin -x configOTA_Cer_Keys_HA.txt -g 1 -v $(JET_VERSION) -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) -k 0xffffffffffffffffffffffffffffffff -i 0x00000010111213141516171800000000 --sector_size=512 
	# Create an Unencrypted Bootable Client with Version 1
	$(JET_BASE)/JET.exe -m otamerge --embed_hdr -c outputffffffffffffffff.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -v $(JET_VERSION) -n 1 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512

	# Modify Embedded Header to reflect version 2 
	$(JET_BASE)/JET.exe -m otamerge --embed_hdr -c $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V2.bin -v $(JET_VERSION) -n 2 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512
	# Now Encrypt the above Version 2  
	$(JET_BASE)/JET.exe -m bin -c $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -f $(APP_OUT_DIR)/$(TARGET_FULL)_V2.bin -e $(APP_OUT_DIR)/$(TARGET_FULL)_V2_ENC.bin -v $(JET_VERSION) -k $(ENCRYPTION_KEY) -i 0x00000010111213141516171800000000 -j $(OTA_STRING) --sector_size=512
	# Wrap the Image with OTA header with version 2
	$(JET_BASE)/JET.exe -m otamerge --ota -c $(APP_OUT_DIR)/$(TARGET_FULL)_V2_ENC.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V2.ota -p 1 -v $(JET_VERSION) -n 2 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512

	# Modify Embedded Header to reflect version 3 
	$(JET_BASE)/JET.exe -m otamerge --embed_hdr -c $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V3.bin -v $(JET_VERSION) -n 3 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512
	# Now Encrypt the above Version 3
	$(JET_BASE)/JET.exe -m bin -c $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin -f $(APP_OUT_DIR)/$(TARGET_FULL)_V3.bin -e $(APP_OUT_DIR)/$(TARGET_FULL)_V3_ENC.bin -v $(JET_VERSION) -k $(ENCRYPTION_KEY) -i 0x00000010111213141516171800000000 -j $(OTA_STRING) --sector_size=512
	# Wrap the Image with OTA header with version 3
	$(JET_BASE)/JET.exe -m otamerge --ota -c $(APP_OUT_DIR)/$(TARGET_FULL)_V3_ENC.bin -o $(APP_OUT_DIR)/$(TARGET_FULL)_V3.ota -p 1 -v $(JET_VERSION) -n 3 -u $(MANUFACTURER_CODE) -t $(OTA_DEVICE_ID) -j $(OTA_STRING) --sector_size=512

	# Remove intermediate files
	rm $(APP_OUT_DIR)/$(TARGET_FULL).bin
	rm outputffffffffffffffff.bin
	rm $(APP_OUT_DIR)/$(TARGET_FULL)_V2.bin
	rm $(APP_OUT_DIR)/$(TARGET_FULL)_V2_ENC.bin
	rm $(APP_OUT_DIR)/$(TARGET_FULL)_V3.bin
	rm $(APP_OUT_DIR)/$(TARGET_FULL)_V3_ENC.bin

endif

endif

###############################################################################

clean:
	rm -f $(APPOBJS)
	rm -f $(APPDEPS)
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).bin
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).axf
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).map
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).dis
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).html
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).log
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL).txt
ifeq ($(OTA),1)
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL)_V1.bin
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL)_V2.ota
	rm -f $(APP_OUT_DIR)/$(TARGET_FULL)_V4.ota
endif
	rm -f $(APP_SRC_DIR)/pdum_gen.* $(APP_SRC_DIR)/zps_gen*.* $(APP_SRC_DIR)/pdum_apdu.S

###############################################################################
