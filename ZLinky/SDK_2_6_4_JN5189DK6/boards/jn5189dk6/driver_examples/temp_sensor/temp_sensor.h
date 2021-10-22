/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

/*Prototypes*/
bool load_calibration_param_from_flash(ADC_Type *base);
bool get_temperature(ADC_Type *base, uint8_t channel, uint32_t delay_value, uint8_t nb_samples, int32_t *temperature);

#endif /*#ifndef TEMP_SENSOR_DRV_H*/
