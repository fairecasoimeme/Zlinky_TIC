/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_UTILITY_H_
#define _BOARD_UTILITY_H_

/* Application should include fsl_clock.h prior to include board_utility.h */

/* Values below are for NDK NX2016SA 32MHz EXS00A-CS11213-6(IEC) */

/* Temperature related to element 0 of CLOCK_ai32MXtalIecLoadFfVsTemp */
#define HW_32M_LOAD_VS_TEMP_MIN (-40)

/* Temperature related to final element of CLOCK_ai32MXtalIecLoadFfVsTemp */
#define HW_32M_LOAD_VS_TEMP_MAX (130)

/* Temperature step between elements of CLOCK_ai32MXtalIecLoadFfVsTemp */
#define HW_32M_LOAD_VS_TEMP_STEP (5)

#define HW_32M_LOAD_VS_TEMP_SIZE ((HW_32M_LOAD_VS_TEMP_MAX - HW_32M_LOAD_VS_TEMP_MIN) / HW_32M_LOAD_VS_TEMP_STEP + 1U)

/* Temperature related to element 0 of CLOCK_ai32kXtalIecLoadFfVsTemp */
#define HW_32k_LOAD_VS_TEMP_MIN (-40)

/* Temperature related to final element of CLOCK_ai32kXtalIecLoadFfVsTemp */
#define HW_32k_LOAD_VS_TEMP_MAX (130)

/* Temperature step between elements of CLOCK_ai32kXtalIecLoadFfVsTemp */
#define HW_32k_LOAD_VS_TEMP_STEP (5)

#define HW_32k_LOAD_VS_TEMP_SIZE ((HW_32k_LOAD_VS_TEMP_MAX - HW_32k_LOAD_VS_TEMP_MIN) / HW_32k_LOAD_VS_TEMP_STEP + 1U)

#define BOARD_MAINCLK_FRO12M            0U
#define BOARD_MAINCLK_XTAL32M           2U
#define BOARD_MAINCLK_FRO32M            3U
#define BOARD_MAINCLK_FRO48M            4U

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
int32_t BOARD_GetTemperature(void);

int BOARD_GetLastKnownTemperature(void);

const ClockCapacitanceCompensation_t *BOARD_GetClock32MCapacitanceCharacteristics(void);

const ClockCapacitanceCompensation_t *BOARD_GetClock32kCapacitanceCharacteristics(void);

void BOARD_tcxo32M_compensation_run(uint32_t temperature_change_threshold, uint32_t us_delay_post_tcxo_adj);

void BOARD_tcxo32k_compensation_run(uint32_t temperature_change_threshold, uint32_t us_delay_post_tcxo_adj);

int32_t Calculate_32MOscCapCompensation(int32_t iTemperature);

int32_t Calculate_32kOscCapCompensation(int32_t iTemperature);

int16_t i16GetAteTemp(void);

const int32_t *CLOCK_GetAi32MXtalIecLoadFfVsTemp(void);

const int32_t *CLOCK_GetAi32kXtalIecLoadFfVsTemp(void);

void BOARD_CpuClockUpdate48Mz(void);
void BOARD_CpuClockUpdate32MhzFro(void);
void BOARD_CpuClockUpdate32MhzXtal(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_UTILITY_H_ */
