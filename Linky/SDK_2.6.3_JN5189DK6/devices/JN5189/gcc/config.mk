###############################################################################
#
# MODULE:   config.mk
#
# DESCRIPTION: Selects a configuration makefile based on JENNIC_CHIP and
#              JENNIC_CHIP_FAMILY.
# 
###############################################################################
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as K32W061, JN5189]. 
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
# Copyright NXP B.V. 2012, 2019. All rights reserved
#
###############################################################################

#
# Set up defaults for stack configuration (these are the only supported options)
# JENNIC_STACK specifies the full stack:
#   MAC only
# JENNIC_MAC allows selection of the MAC layer:
#   MiniMac
#
# Values are normally specified by the application make file; the defaults here
# are for legacy builds that pre-date the selection process
#
###############################################################################

JENNIC_STACK ?= MAC
JENNIC_MAC   ?= MiniMac

###############################################################################
# Include the chip or chip family definitions.
# Chip takes precendence over chip family
###############################################################################
# Ensure chip and family are set
override JENNIC_CHIP_FAMILY = JN518x
SDK_DEVICE_FAMILY ?= JN5189
SDK_BOARD ?= jn5189dk6

# Define numeric value for chip type
CFLAGS += -DJN518x=5189
CFLAGS += -DJN5189=5189
CFLAGS += -DJENNIC_CHIP_NAME=_JN5189
CFLAGS += -DJENNIC_CHIP_FAMILY_NAME=_JN518x

CFLAGS += -D__JN518X__
CFLAGS += -DCPU_JN518X

# Base dir for selected chip
CHIP_BASE_DIR = $(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/gcc

# include definitions for the Cortex M4 architecture
include $(CHIP_BASE_DIR)/config_cm4.mk

# Chip specific C flags
# Feature set:
#   JENNIC_HW_xx is silicon feature and so fixed
#   JENNIC_SW_xx is build option and can be changed
#
#   JENNIC_HW_BBC_RXINCCA:    BBC RX during CCA capability
#   JENNIC_HW_BBC_DMA:        BBC frame buffer DMA mechanism
#   JENNIC_HW_BBC_ISA:        BBC inline security engine
#   JENNIC_SW_EXTERNAL_FLASH: whether external flash is supported
#   UART_BACKWARDS_COMPATIBLE_API: API behaves like the older versions
#   PDM_DESCRIPTOR_BASED_API: Use standard API interface
#
CFLAGS += -DJENNIC_HW_BBC_RXINCCA=1
CFLAGS += -DJENNIC_HW_BBC_DMA=1
CFLAGS += -DJENNIC_HW_BBC_ISA=0
CFLAGS += -DJENNIC_SW_EXTERNAL_FLASH=0
# for b/l debug build - comment out so boot lib not required
CFLAGS += -DUART_BACKWARDS_COMPATIBLE_API=1
# PDM API Build flag
#CFLAGS += -DPDM_DESCRIPTOR_BASED_API=1
CFLAGS += -DRXPOWERADJUST_SUPPORT

# Linker flags
LDFLAGS += -nostartfiles

# Library search paths
LDFLAGS += -L$(SDK2_BASE_DIR)/middleware/wireless/ieee-802.15.4/lib
LDFLAGS += -L$(SDK2_BASE_DIR)/middleware/wireless/framework/PDM/Library
LDFLAGS += -L$(SDK2_BASE_DIR)/middleware/wireless/framework/PDUM/Library
LDFLAGS += -L$(SDK2_BASE_DIR)/middleware/wireless/framework/PWRM/Library
LDFLAGS += -L$(SDK2_BASE_DIR)/middleware/wireless/framework/XCVR/lib

# Linker script search paths
LDFLAGS += -L$(CHIP_BASE_DIR)

# Include paths
CFLAGS += -I../board
CFLAGS += -I$(SDK2_BASE_DIR)/CMSIS/Include
CFLAGS += -I$(SDK2_BASE_DIR)/middleware/wireless/framework/boards/$(SDK_BOARD)
CFLAGS += -I$(SDK2_BASE_DIR)/boards/$(SDK_BOARD)
CFLAGS += -I$(SDK2_BASE_DIR)/components/serial_manager
CFLAGS += -I$(SDK2_BASE_DIR)/components/uart
CFLAGS += -I$(SDK2_BASE_DIR)/devices
CFLAGS += -I$(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)
CFLAGS += -I$(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/drivers
CFLAGS += -I$(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/utilities/debug_console
CFLAGS += -I$(SDK2_BASE_DIR)/devices/$(SDK_DEVICE_FAMILY)/utilities/str
CFLAGS += -I$(SDK2_BASE_DIR)/middleware/wireless/framework/Common
CFLAGS += -I$(SDK2_BASE_DIR)/middleware/wireless/ieee-802.15.4/Include
CFLAGS += -I$(SDK2_BASE_DIR)/middleware/wireless/ieee-802.15.4/uMac/Include
CFLAGS += -I$(SDK2_BASE_DIR)/middleware/wireless/ieee-802.15.4/mMac/Include
CFLAGS += -I$(SDK2_BASE_DIR)/middleware/wireless/framework/XCVR/DK6

# IPN build variant
ifdef JENNIC_IPN
MAC_IPN := _IPN
else
MAC_IPN :=
endif

# Standard libraries:  MAC, Radio, etc.
LDLIBS += MiniMac$(MAC_IPN)
LDLIBS += Radio

# Export default linker command file
LINKER_FILE ?= AppBuildNone
LINKCMD ?= $(LINKER_FILE).ld

# MAC security enable (Mini MAC with JenNet-IP)
ifeq ($(JENNIC_STACK_SECURITY), MAC)
  LDFLAGS += -Wl,-ueSecurityTxPrepare -Wl,-ueSecurityTxEncrypt -Wl,-ubSecurityRxProcess
endif

###############################################################################
# Define the selected Jennic chip
###############################################################################

CFLAGS += -DJENNIC_CHIP=$(JENNIC_CHIP)
CFLAGS += -DJENNIC_CHIP_$(JENNIC_CHIP)
CFLAGS += -DJENNIC_CHIP_FAMILY=$(JENNIC_CHIP_FAMILY)
CFLAGS += -DJENNIC_CHIP_FAMILY_$(JENNIC_CHIP_FAMILY)

CFLAGS += -DJENNIC_STACK_$(JENNIC_STACK)
CFLAGS += -DJENNIC_MAC_$(JENNIC_MAC)

###############################################################################
# Chip independent compiler options
###############################################################################

CFLAGS += -Wall
CFLAGS += -Wunreachable-code

# Everything needs this
CFLAGS += -DEMBEDDED

###############################################################################
# Compiler Paths
###############################################################################

#TOOL_BASE_DIR ?= $(SDK_BASE_DIR)/Tools

###############################################################################
# Toolchain
###############################################################################

CC	= gcc
AS	= as
LD	= gcc
AR	= ar
NM	= nm
STRIP	= strip
SIZE	= size
OBJCOPY = objcopy
OBJDUMP = objdump
RANLIB	= ranlib

ifdef CROSS_COMPILE
CC:=$(CROSS_COMPILE)-$(CC)
AS:=$(CROSS_COMPILE)-$(AS)
LD:=$(CROSS_COMPILE)-$(LD)
AR:=$(CROSS_COMPILE)-$(AR)
NM:=$(CROSS_COMPILE)-$(NM)
STRIP:=$(CROSS_COMPILE)-$(STRIP)
SIZE:=$(CROSS_COMPILE)-$(SIZE)
OBJCOPY:=$(CROSS_COMPILE)-$(OBJCOPY)
OBJDUMP:=$(CROSS_COMPILE)-$(OBJDUMP)
# Allow tools to be found from path if TOOLCHAIN_PATH is defined as '.'
ifeq ($(TOOLCHAIN_PATH),.)
TOOLCHAIN_CC:=$(CC)
TOOLCHAIN_AS:=$(AS)
TOOLCHAIN_LD:=$(LD)
TOOLCHAIN_AR:=$(AR)
TOOLCHAIN_NM:=$(NM)
TOOLCHAIN_STRIP:=$(STRIP)
TOOLCHAIN_SIZE:=$(SIZE)
TOOLCHAIN_OBJCOPY:=$(OBJCOPY)
TOOLCHAIN_OBJDUMP:=$(OBJDUMP)
else
TOOLCHAIN_CC:=$(TOOLCHAIN_PATH)/$(CC)
TOOLCHAIN_AS:=$(TOOLCHAIN_PATH)/$(AS)
TOOLCHAIN_LD:=$(TOOLCHAIN_PATH)/$(LD)
TOOLCHAIN_AR:=$(TOOLCHAIN_PATH)/$(AR)
TOOLCHAIN_NM:=$(TOOLCHAIN_PATH)/$(NM)
TOOLCHAIN_STRIP:=$(TOOLCHAIN_PATH)/$(STRIP)
TOOLCHAIN_SIZE:=$(TOOLCHAIN_PATH)/$(SIZE)
TOOLCHAIN_OBJCOPY:=$(TOOLCHAIN_PATH)/$(OBJCOPY)
TOOLCHAIN_OBJDUMP:=$(TOOLCHAIN_PATH)/$(OBJDUMP)
endif
endif

###############################################################################
# exports

export JENNIC_CHIP_FAMILY SDK_DEVICE_FAMILY SDK_BOARD JENNIC_CHIP LDLIBS CFLAGS LDFLAGS LINKCMD
