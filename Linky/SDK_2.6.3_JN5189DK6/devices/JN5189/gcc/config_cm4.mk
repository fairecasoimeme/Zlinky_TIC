##############################################################################
#                                                                            #
# MODULE:   config_cm4.mk                                                    #
#                                                                            #
# DESCRIPTION: Compiler definitions for the Cortex M3 architecture.          #
# NOTE: This file should only contain architecture specific options.         #
#                                                                            #
##############################################################################

##############################################################################
#                                                                            #
# This software is owned by NXP B.V. and/or its supplier and is protected    #
# under applicable copyright laws. All rights are reserved. We grant You,    #
# and any third parties, a license to use this software solely and           #
# exclusively on NXP products [NXP Microcontrollers such as JN5172].         #
# You, and any third parties must reproduce the copyright and warranty       #
# notice and any other legend of ownership on each copy or partial copy of   #
# the software.                                                              #
#                                                                            #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        #
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  #
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A            #
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER  #
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,   #
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        #
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    #
# PROFITS; OR BUSINESS CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING        #
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         #
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               #
#                                                                            #
# Copyright NXP B.V. 2014. All rights reserved                               #
#                                                                            #
##############################################################################

DISABLE_LTO ?= 1

# Set toolchain path to use arm version of compiler
TOOLCHAIN_PATH ?= .

CROSS_COMPILE  ?= arm-none-eabi

# Cortex M4 architecture/LPCXpresso compiler flags
CFLAGS += -std=gnu99
CFLAGS += -DCORE_M4
CFLAGS += -D__CODE_RED
CFLAGS += -fno-common
CFLAGS += -fmessage-length=0
CFLAGS += -fno-builtin
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb

# Cortex M4 architecture/LPCXpresso linker flags
#LDFLAGS += -nostdlib
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mthumb

# omit frame pointer by default
CFLAGS += -fomit-frame-pointer
LDFLAGS += -fomit-frame-pointer

# default to optimise for size
CFLAGS += -Os
LDFLAGS += -Os

# Default to smallest possible enums
CFLAGS += -fshort-enums
LDFLAGS += -fshort-enums

# Turn on all common warnings
CFLAGS += -Wall 

# Turn on some additional useful warnings
CFLAGS += -Wpacked
#CFLAGS += -Wcast-align

# Output each function and static data in their own sections
CFLAGS += -fdata-sections -ffunction-sections

# Debug Support
ifeq ($(DEBUG), HW)
    DEBUG_MODE?=SWD
endif

ifeq ($(DEBUG), HW_SIZEOPT)
    DEBUG_MODE?=SWD
    DEBUG_OPT?=SIZE
endif

ifneq (,$(findstring $(DEBUG_MODE), SWD SWD_TRACE JTAG))
    HARDWARE_DEBUG_ENABLED=1
else
    HARDWARE_DEBUG_ENABLED=0
endif

ifeq ($(HARDWARE_DEBUG_ENABLED), 1)
    DISABLE_LTO ?= 1
    CFLAGS  += -g -DGDB
    LDFLAGS  += -g
    ifeq ($(DEBUG_OPT), SIZE)
    else
        # Optimise for debug instead of size
        CFLAGS  := $(subst -Os,-Og,$(CFLAGS))
        LDFLAGS  := $(subst -Os,-Og,$(LDFLAGS))
    endif
    CFLAGS  += -DHWDEBUG
    BIN_SUFFIX ?= _hwdbg
else
    CFLAGS  += -DWATCHDOG_ALLOWED
endif

# Link Time Optimisation configuration

# Default (unless debugging) is to compile & link using link time optimisation, 
# but allow it to be disabled by setting DISABLE_LTO=1
ifneq ($(DISABLE_LTO), 1)
CFLAGS += -flto -ffat-lto-objects 
LDFLAGS += -flto 
else
CFLAGS += -fno-lto
LDFLAGS += -fno-lto
endif

# Software flag for little endian build option
CFLAGS += -DLITTLE_ENDIAN_PROCESSOR

###############################################################################
