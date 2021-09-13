###############################################################################
# Copyright 2021 NXP
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
###############################################################################

USE_FREERTOS ?= 0

ifeq ($(USE_FREERTOS), 1)
FREERTOS_INC         = $(SDK2_BASE_DIR)/rtos/amazon-freertos/lib/include
FREERTOS_SRC         = $(SDK2_BASE_DIR)/rtos/amazon-freertos/lib/FreeRTOS
FREERTOS_MM_SRC      = $(SDK2_BASE_DIR)/rtos/amazon-freertos/lib/FreeRTOS/portable/MemMang
FREERTOS_PORT_SRC    = $(SDK2_BASE_DIR)/rtos/amazon-freertos/lib/FreeRTOS/portable/GCC/ARM_CM3
endif

ifeq ($(USE_FREERTOS), 1)
    APPSRC += event_groups.c
    APPSRC += list.c
    APPSRC += queue.c
    APPSRC += stream_buffer.c
    APPSRC += tasks.c
    APPSRC += timers.c
    APPSRC += heap_4.c
    APPSRC += port.c
    APPSRC += fsl_os_abstraction_free_rtos.c
else
    APPSRC += fsl_os_abstraction_bm.c
endif

ifeq ($(USE_FREERTOS), 1)
INCFLAGS += -I$(FREERTOS_INC)
INCFLAGS += -I$(FREERTOS_INC)/private
INCFLAGS += -I$(FREERTOS_PORT_SRC)
endif

ifeq ($(USE_FREERTOS), 1)
CFLAGS  += -DFSL_RTOS_FREE_RTOS
endif

ifneq ($(USE_FREERTOS), 1)
ifneq ($(SELOVERIDE),1)
CFLAGS  += -DosNumberOfSemaphores=0
CFLAGS  += -DosNumberOfMutexes=0
CFLAGS  += -DosNumberOfMessageQs=0
CFLAGS  += -DosNumberOfMessages=0
CFLAGS  += -DosNumberOfEvents=0
CFLAGS  += -DgMainThreadStackSize_c=0
CFLAGS  += -DgMainThreadPriority_c=0
CFLAGS  += -DgTaskMultipleInstancesManagement_c=0
CFLAGS  += -DFSL_OSA_BM_TIMER_CONFIG=FSL_OSA_BM_TIMER_SYSTICK
endif
endif

ifeq ($(USE_FREERTOS), 1)
OSA_BASE_SRC += $(FREERTOS_SRC):$(FREERTOS_MM_SRC):$(FREERTOS_PORT_SRC)
endif
