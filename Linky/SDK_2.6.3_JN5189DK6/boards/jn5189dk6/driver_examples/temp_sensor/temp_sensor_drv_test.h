/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TEMP_SENSOR_TEST_H
#define TEMP_SENSOR_TEST_H
#include "temp_sensor_drv.h"

/*Prototypes*/
void load_adc_and_temp_cal_parameter(CfgTempSensor_t *temp_cal, CfgGpAdcTrim_t *adc_cal);
int32_t temp_computation(uint16_t adcout_vbat_lsb_sum8, uint16_t tsens_adcout_T_sum8, uint8_t nb_samples);
void test_temperature_computation(void);
#endif //#ifndef TEMP_SENSOR_TEST_H
