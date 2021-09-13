/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "temp_sensor_drv.h"
#include "temp_sensor_drv_test.h"

/*Defines*/
#define TEST_NBR 27
#define CAL_CONFIG_NBR 2

/*Types*/
typedef struct
{
    uint32_t adcout_vbat_sum8_vector;
    uint32_t tsens_adcout_T_sum8_vector;
    int32_t ideal_temp_est_mult128;
    uint8_t nb_samples;
    uint8_t calib_param_idx; // id of the calibration configuration to use
} Test_param;

typedef struct
{
    CfgTempSensor_t temp_sensor_cal_config;
    CfgGpAdcTrim_t adc_cal_config;
} Cal_config;

Cal_config cal_info[CAL_CONFIG_NBR] = {
    {{0, 26214, 68, 29288, 68, 29256, 21001, 20968, 41, (int8_t)154, 3354, 3756, 1008, 1011, (int16_t)39453, 1401},
     {646, 489, 0, 0, 0}},
    {{0, 26214, 77, 29600, 77, 29564, 21155, 21127, 41, (int8_t)154, 3328, 3754, 1011, 969, (int16_t)39793, 1520},
     {682, 525, 0, 0, 0}}};

Test_param test_info[TEST_NBR] = {
    {31744, 19400, 5914, 8, 0},  {31744, 19401, 5912, 8, 0},  {31744, 19402, 5911, 8, 0},  {31744, 19410, 5898, 8, 0},
    {31744, 19420, 5882, 8, 0},  {31744, 19430, 5866, 8, 0},  {31744, 19530, 5707, 8, 0},  {31744, 19630, 5548, 8, 0},
    {31744, 19730, 5389, 8, 0},  {31744, 12000, 17459, 8, 0}, {31744, 13000, 15923, 8, 0}, {31744, 14000, 14380, 8, 0},
    {31744, 15000, 12829, 8, 0}, {31744, 16000, 11271, 8, 0}, {31744, 17000, 9704, 8, 0},  {31744, 18000, 8130, 8, 0},
    {31744, 19000, 6549, 8, 0},  {31744, 20000, 4959, 8, 0},  {31744, 21000, 3362, 8, 0},  {31744, 22000, 1758, 8, 0},
    {31744, 23000, 145, 8, 0},   {31744, 24000, -1475, 8, 0}, {31744, 25000, -3103, 8, 0}, {31744, 26000, -4739, 8, 0},
    {31744, 27000, -6382, 8, 0}, {29997, 18427, 7611, 8, 1},  {16337, 18386, 7637, 8, 1}};

void test_temperature_computation()
{
    int32_t temp;
    int32_t delta              = 0;
    int32_t max_delta          = 0;
    int calibration_config_idx = -1;

    PRINTF("TEST id | Est temp x 128 | Ideal temp x 128 | Delta | Est temp celsius | \r\n");
    for (int i = 0; i < TEST_NBR; i++)
    {
        // load new calibration configuration for test if not yet loaded or if different
        if ((calibration_config_idx == -1) || (calibration_config_idx != test_info[i].calib_param_idx))
        {
            calibration_config_idx = test_info[i].calib_param_idx;
            load_adc_and_temp_cal_parameter(&cal_info[calibration_config_idx].temp_sensor_cal_config,
                                            &cal_info[calibration_config_idx].adc_cal_config);
        }
        temp  = temp_computation(test_info[i].adcout_vbat_sum8_vector, test_info[i].tsens_adcout_T_sum8_vector,
                                test_info[i].nb_samples);
        delta = temp - test_info[i].ideal_temp_est_mult128;

        if (test_info[i].ideal_temp_est_mult128 < 0)
        {
            PRINTF("%7d |         -%5d |           -%5d | %5d |             -%3d |\r\n", i, temp,
                   test_info[i].ideal_temp_est_mult128, delta, temp / 128);
        }
        else
        {
            PRINTF("%7d | %14d | %16d | %5d | %16d |\r\n", i, temp, test_info[i].ideal_temp_est_mult128, delta,
                   temp / 128);
        }
        if (delta < 0)
        {
            delta *= -1;
        }
        if (delta > max_delta)
        {
            max_delta = delta;
        }
    }

    PRINTF("Verdict\r\n");
    if (max_delta > 3)
    {
        PRINTF("Test failed\r\n");
    }
    else
    {
        PRINTF("Test pass\r\n");
    }

    while (1)
        ;
}
