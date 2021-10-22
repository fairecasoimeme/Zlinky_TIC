###############################################################################
#
# MODULE:   Config_ZCL.mk
#
# DESCRIPTION:unified Profile make requirements
# 
###############################################################################
#
 ############################################################################
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as  JN5168, JN5164,
# JN5161, JN5148, JN5142, JN5139]. 
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
# Copyright NXP B.V. 2016. All rights reserved
#
###############################################################################

###############################################################################
RAMOPT ?= 0
OTA    ?= 0
# Get ZigBee stack makefile requirements
ifneq ($(JENNIC_CHIP_FAMILY),JN518x)
include $(STACK_BASE_DIR)/ZBPro/Build/config_ZBPro.mk
else
ifeq ($(FRAMEWORK_SWITCH),0)
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee3.0/BuildConfig/ZBPro/Build/config_ZBPro.mk
else
include $(SDK2_BASE_DIR)/middleware/wireless/zigbee/BuildConfig/ZBPro/Build/config_ZBPro.mk
endif
endif
###############################################################################
ZCIF_BASE        = $(COMPONENTS_BASE_DIR)/ZCIF
ZCIF_SRC         = $(ZCIF_BASE)/Source
ZCL_BASE         = $(COMPONENTS_BASE_DIR)/ZCL
CLUSTERS_BASE    = $(ZCL_BASE)/Clusters
DEVICES_BASE     = $(ZCL_BASE)/Devices

#Always include General for basic, so no compile time option
CLUSTERS_GENERAL_SRC                    = $(CLUSTERS_BASE)/General/Source

ifeq ($(APP_CLUSTERS_MEASUREMENT_AND_SENSING_SRC), 1)
CLUSTERS_MEASUREMENT_AND_SENSING_SRC    = $(CLUSTERS_BASE)/MeasurementAndSensing/Source
endif

ifeq ($(APP_CLUSTER_LIGHTING_SRC), 1)
CLUSTER_LIGHTING_SRC                    = $(CLUSTERS_BASE)/Lighting/Source
DEVICES_ZLO_SRC                         = $(DEVICES_BASE)/ZLO/Source
endif

ifeq ($(APP_CLUSTERS_HVAC_SRC), 1)
CLUSTERS_HVAC_SRC                       = $(CLUSTERS_BASE)/HVAC/Source
DEVICES_HVAC_SRC                        = $(DEVICES_BASE)/ZHA/HVAC/Source
endif

ifeq ($(APP_CLUSTERS_CLOSURES_SRC), 1)
CLUSTERS_CLOSURES_SRC                   = $(CLUSTERS_BASE)/Closures/Source
DEVICES_CLOSURES_SRC                    = $(DEVICES_BASE)/ZHA/Closures/Source
endif

ifeq ($(APP_CLUSTERS_SECURITY_AND_SAFETY_SRC), 1)
CLUSTERS_SECURITY_AND_SAFETY_SRC        = $(CLUSTERS_BASE)/SecurityAndSafety/Source
DEVICES_SECURITY_AND_SAFETY_SRC         = $(DEVICES_BASE)/ZHA/SecurityAndSafety/Source
endif

ifeq ($(APP_CLUSTERS_SMART_ENERGY_SRC), 1)
CLUSTERS_SMART_ENERGY_SRC               = $(CLUSTERS_BASE)/SmartEnergy/Source
DEVICES_SMART_ENERGY_SRC                = $(DEVICES_BASE)/ZHA/SmartEnergy/Source
endif

ifeq ($(APP_CLUSTERS_OTA_SRC), 1)
CLUSTERS_OTA_SRC                        = $(CLUSTERS_BASE)/OTA/Source
endif

ifeq ($(APP_CLUSTER_COMMISSIONING_SRC), 1)
CLUSTERS_COMMISSIOING_SRC               = $(CLUSTERS_BASE)/Commissioning/Source
endif

ifeq ($(APP_CLUSTERS_APPLIANCE_MANAGEMENT_SRC), 1)
CLUSTERS_APPLIANCE_MANAGEMENT_SRC       = $(CLUSTERS_BASE)/ApplianceManagement/Source 
DEVICES_APPLIANCE_MANAGEMENT_SRC        = $(DEVICES_BASE)/ZHA/ApplianceManagement/Source  
endif

ifeq ($(APP_CLUSTERS_GREENPOWER_SRC), 1)
CLUSTERS_GREENPOWER_SRC                 = $(CLUSTERS_BASE)/GreenPower/Source
DEVICES_GP_SRC                          = $(DEVICES_BASE)/ZGP/Source
endif

DEVICES_GENERIC_SRC                     = $(DEVICES_BASE)/ZHA/Generic/Source

CLUSTERS_SRC_DIRS                       = $(CLUSTERS_GENERAL_SRC):$(CLUSTERS_MEASUREMENT_AND_SENSING_SRC):$(CLUSTER_LIGHTING_SRC):$(CLUSTERS_HVAC_SRC):$(CLUSTERS_CLOSURES_SRC):$(CLUSTERS_SECURITY_AND_SAFETY_SRC):$(CLUSTERS_SMART_ENERGY_SRC):$(CLUSTERS_OTA_SRC):$(CLUSTERS_COMMISSIOING_SRC):$(CLUSTERS_APPLIANCE_MANAGEMENT_SRC):$(CLUSTERS_GREENPOWER_SRC)
DEVICES_SRC_DIRS                        = $(DEVICES_GENERIC_SRC):$(DEVICES_ZLO_SRC):$(DEVICES_HVAC_SRC):$(DEVICES_CLOSURES_SRC):$(DEVICES_SECURITY_AND_SAFETY_SRC):$(DEVICES_SMART_ENERGY_SRC):$(DEVICES_APPLIANCE_MANAGEMENT_SRC):$(DEVICES_GP_SRC)
ZCL_SRC_DIRS                            = $(ZCIF_SRC):$(CLUSTERS_SRC_DIRS):$(DEVICES_SRC_DIRS)
###############################################################################


ifeq ($(GP_SUPPORT), 1)
GPSRC:=$(shell cd $(CLUSTERS_GREENPOWER_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(GPSRC)
ZCLAPPSRC += gp.c
else
APPSRC += $(GPSRC)
APPSRC += gp.c
endif 
endif


# Clusters
# All General cluster files
GENSRCS:=$(shell cd $(CLUSTERS_GENERAL_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(GENSRCS)
else
APPSRC += $(GENSRCS)
endif

# All Generic devices
GENERIC_SRCS:=$(shell cd $(DEVICES_GENERIC_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(GENERIC_SRCS)
else
APPSRC += $(GENERIC_SRCS)
endif

ifeq ($(APP_CLUSTERS_MEASUREMENT_AND_SENSING_SRC), 1)
# All measurement and sensing cluster files
MSSRCS:=$(shell cd $(CLUSTERS_MEASUREMENT_AND_SENSING_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(MSSRCS)
else
APPSRC += $(MSSRCS)
endif
endif

ifeq ($(APP_CLUSTER_LIGHTING_SRC), 1)
# All Lighting cluster files
LSRCS:=$(shell cd $(CLUSTER_LIGHTING_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(LSRCS)
else
APPSRC += $(LSRCS)
endif
ZLO_SRCS:=$(shell cd $(DEVICES_ZLO_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(ZLO_SRCS)
else
APPSRC += $(ZLO_SRCS)
endif
endif

ifeq ($(APP_CLUSTERS_HVAC_SRC), 1)
# All HVAC cluster files
HVACSRCS:=$(shell cd $(CLUSTERS_HVAC_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(HVACSRCS)
else
APPSRC += $(HVACSRCS)
endif

# All HVAC devices
HVAC_HOME_SRC :=$(shell cd $(DEVICES_HVAC_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(HVAC_HOME_SRC)
else
APPSRC += $(HVAC_HOME_SRC)
endif
endif

ifeq ($(APP_CLUSTERS_CLOSURES_SRC), 1)
# All Closures cluster files
CLSSRCS:=$(shell cd $(CLUSTERS_CLOSURES_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(CLSSRCS)
else
APPSRC += $(CLSSRCS)
endif

# All CLosure devices
CLS_HOME_SRC :=$(shell cd $(DEVICES_CLOSURES_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(CLS_HOME_SRC)
else
APPSRC += $(CLS_HOME_SRC)
endif

endif


ifeq ($(APP_CLUSTERS_SECURITY_AND_SAFETY_SRC), 1)
# All SecurityAndSafety cluster files
IASSRCS:=$(shell cd $(CLUSTERS_SECURITY_AND_SAFETY_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(IASSRCS)
else
APPSRC += $(IASSRCS)
endif

SECURITY_AND_SAFETY_HOME_SRC :=$(shell cd $(DEVICES_SECURITY_AND_SAFETY_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(SECURITY_AND_SAFETY_HOME_SRC)
else
APPSRC += $(SECURITY_AND_SAFETY_HOME_SRC)
endif
endif

ifeq ($(APP_CLUSTERS_SMART_ENERGY_SRC), 1)
# Selected Smart Energy cluster files
SESRCS:=$(shell cd $(CLUSTERS_SMART_ENERGY_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(SESRCS)
else
APPSRC += $(SESRCS)
endif

SMART_ENERGY_HOME_SRC :=$(shell cd $(DEVICES_SMART_ENERGY_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(SMART_ENERGY_HOME_SRC)
else
APPSRC += $(SMART_ENERGY_HOME_SRC)
endif
endif

# OTA files 
ifeq ($(APP_CLUSTERS_OTA_SRC), 1)
OTASRCS:= $(shell cd $(CLUSTERS_OTA_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(OTASRCS)
else
APPSRC += $(OTASRCS)
endif
endif

ifeq ($(APP_CLUSTER_COMMISSIONING_SRC), 1)
# All Commissioning cluster files
COMSRCS:=$(shell cd $(CLUSTERS_COMMISSIOING_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(COMSRCS)
else
APPSRC += $(COMSRCS)
endif
endif

ifeq ($(APP_CLUSTERS_APPLIANCE_MANAGEMENT_SRC), 1)
# All ApplianceManagement cluster files
AMSRCS:=$(shell cd $(CLUSTERS_APPLIANCE_MANAGEMENT_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(AMSRCS)
else
APPSRC += $(AMSRCS)
endif
# All ApplianceManagement devices
APPLIANCE_MANAGEMENT_SRC :=$(shell cd $(DEVICES_APPLIANCE_MANAGEMENT_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(APPLIANCE_MANAGEMENT_SRC)
else
APPSRC += $(APPLIANCE_MANAGEMENT_SRC)
endif
endif


# ZCL now built from source
ZCLSRCS:= $(shell cd $(ZCIF_SRC); ls *.c)
ifeq ($(RAMOPT), 1)
ZCLAPPSRC += $(ZCLSRCS)
else
APPSRC += $(ZCLSRCS)
endif


###############################################################################

# ZCIF Includes
INCFLAGS += -I$(ZCIF_SRC)
INCFLAGS += -I$(ZCIF_BASE)/Include
# Cluster Includes
INCFLAGS += -I$(CLUSTERS_BASE)/General/Include
INCFLAGS += -I$(CLUSTERS_BASE)/General/Source
INCFLAGS += -I$(CLUSTERS_BASE)/MeasurementAndSensing/Include
INCFLAGS += -I$(CLUSTERS_BASE)/Lighting/Include
INCFLAGS += -I$(CLUSTERS_BASE)/HVAC/Include
INCFLAGS += -I$(CLUSTERS_BASE)/Closures/Include
INCFLAGS += -I$(CLUSTERS_BASE)/SecurityAndSafety/Include
INCFLAGS += -I$(CLUSTERS_BASE)/SmartEnergy/Include
INCFLAGS += -I$(CLUSTERS_BASE)/OTA/Include
INCFLAGS += -I$(CLUSTERS_BASE)/Commissioning/Include
INCFLAGS += -I$(CLUSTERS_BASE)/ApplianceManagement/Include
INCFLAGS += -I$(CLUSTERS_BASE)/GreenPower/Include

# Device Includes
INCFLAGS += -I$(DEVICES_BASE)/ZHA/Generic/Include

ifeq ($(APP_CLUSTER_LIGHTING_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZLO/Include
endif

ifeq ($(APP_CLUSTERS_HVAC_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZHA/HVAC/Include
endif

ifeq ($(APP_CLUSTERS_CLOSURES_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZHA/Closures/Include
endif

ifeq ($(APP_CLUSTERS_SECURITY_AND_SAFETY_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZHA/SecurityAndSafety/Include
endif

ifeq ($(APP_CLUSTERS_SMART_ENERGY_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZHA/SmartEnergy/Include
endif

ifeq ($(APP_CLUSTERS_APPLIANCE_MANAGEMENT_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZHA/ApplianceManagement/Include
endif

ifeq ($(APP_CLUSTERS_GREENPOWER_SRC), 1)
INCFLAGS += -I$(DEVICES_BASE)/ZGP/Include
endif

###############################################################################

CFLAGS += -DZPS_APL_OPT_SINGLE_INSTANCE
CFLAGS += -DOTA_NO_CERTIFICATE
###############################################################################

CFLAGS += -DPLME_SAP
LDFLAGS += -L$(STACK_BASE_DIR)/ZBPro/Build

###############################################################################



