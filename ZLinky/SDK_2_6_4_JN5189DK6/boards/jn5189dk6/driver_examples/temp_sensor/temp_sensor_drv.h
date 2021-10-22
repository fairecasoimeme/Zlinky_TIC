/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TEMP_SENSOR_DRV_H
#define TEMP_SENSOR_DRV_H

/*Types*/
typedef struct
{
    int16_t ref0v0;
    int16_t ref0v8;
    int16_t adcout_ref0v0_vbat3v3_sum8;
    int16_t adcout_ref0v8_vbat3v3_sum8;
    int16_t adcout_ref0v0_vbat1v8_sum8;
    int16_t adcout_ref0v8_vbat1v8_sum8;
    int16_t adcout_Tref_vbat3v3_sum8;
    int16_t adcout_Tref_vbat1v8_sum8;
    int8_t nlfit_Toffset;
    int8_t nlfit_gain;
    int16_t Tref;
    uint16_t vbatsum_offset;
    int16_t vbatsum_gain;
    int16_t adcfs_vbat3v3;
    int16_t slopefitinv_gain;
    int16_t slopefitinv_vbatgain;
} CfgTempSensor_t;

typedef struct
{
    uint16_t offset_cal;
    uint16_t gain_cal;
    uint16_t unity_cal;
    uint16_t unity_vout_val;
    uint16_t unity_gain_input_val;

} CfgGpAdcTrim_t;

/*Defines*/
#define ADC_GPADC_CTRL0_GPADC_TSAMP_Pos 9 /*!< ADC GPADC_CTRL0: GPADC_TSAMP Position */
#define ADC_GPADC_CTRL0_GPADC_TSAMP_Msk \
    (0x1fUL << ADC_GPADC_CTRL0_GPADC_TSAMP_Pos)                       /*!< ADC GPADC_CTRL0: GPADC_TSAMP Mask     */
#define ADC_GPADC_CTRL0_TEST_Pos 14                                   /*!< ADC GPADC_CTRL0: TEST Position        */
#define ADC_GPADC_CTRL0_TEST_Msk (0x03UL << ADC_GPADC_CTRL0_TEST_Pos) /*!< ADC GPADC_CTRL0: TEST Mask            */

#define ADC_GPADC_CTRL1_OFFSET_CAL_Pos 0 /*!< ADC GPADC_CTRL1: OFFSET_CAL Position  */
#define ADC_GPADC_CTRL1_OFFSET_CAL_Msk \
    (0x000003ffUL << ADC_GPADC_CTRL1_OFFSET_CAL_Pos) /*!< ADC GPADC_CTRL1: OFFSET_CAL Mask      */
#define ADC_GPADC_CTRL1_GAIN_CAL_Pos 10              /*!< ADC GPADC_CTRL1: GAIN_CAL Position    */
#define ADC_GPADC_CTRL1_GAIN_CAL_Msk \
    (0x000003ffUL << ADC_GPADC_CTRL1_GAIN_CAL_Pos) /*!< ADC GPADC_CTRL1: GAIN_CAL Mask        */

#define FLASH_CFG_GPADC_TRIM_VALID_Pos 0
#define FLASH_CFG_GPADC_TRIM_VALID_Width 1
#define FLASH_CFG_GPADC_OFFSETCAL_Pos 1
#define FLASH_CFG_GPADC_OFFSETCAL_Width 10
#define FLASH_CFG_GPADC_GAINCAL_Pos 11
#define FLASH_CFG_GPADC_GAINCAL_Width 10
#define FLASH_CFG_GPADC_UNITY_OFFSET_CAL_Pos 21
#define FLASH_CFG_GPADC_UNITY_OFFSET_CAL_Width 10

#define FLASH_CFG_GPADC_UNITY_OFFSET_VOUT_VAL_Pos 0
#define FLASH_CFG_GPADC_UNITY_OFFSET_VOUT_VAL_Width 12
#define FLASH_CFG_GPADC_UNITY_GAIN_INPUT_VAL_Pos 12
#define FLASH_CFG_GPADC_UNITY_GAIN_INPUT_VAL_Width 12

#define FLASH_CFG_TEMPSENSOR_TRIM_VALID_Pos 0
#define FLASH_CFG_TEMPSENSOR_TRIM_VALID_Width 1

#define FLASH_CFG_TEMP_SENSOR_REF0V0_Pos 0
#define FLASH_CFG_TEMP_SENSOR_REF0V0_Width 16
#define FLASH_CFG_TEMP_SENSOR_REF0V8_Pos 16
#define FLASH_CFG_TEMP_SENSOR_REF0V8_Width 16

#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V0_VBAT3V3_SUM8_Pos 0
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V0_VBAT3V3_SUM8_Width 16
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V8_VBAT3V3_SUM8_Pos 16
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V8_VBAT3V3_SUM8_Width 16

#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V0_VBAT1V8_SUM8_Pos 0
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V0_VBAT1V8_SUM8_Width 16
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V8_VBAT1V8_SUM8_Pos 16
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V8_VBAT1V8_SUM8_Width 16

#define FLASH_CFG_TEMP_SENSOR_ADCOUT_TREF_VBAT3V3_SUM8_Pos 0
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_TREF_VBAT3V3_SUM8_Width 16
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_TREF_VBAT1V8_SUM8_Pos 16
#define FLASH_CFG_TEMP_SENSOR_ADCOUT_TREF_VBAT1V8_SUM8_Width 16

#define FLASH_CFG_TEMP_SENSOR_NLFIT_TOFFSET_Pos 0
#define FLASH_CFG_TEMP_SENSOR_NLFIT_TOFFSET_Width 8
#define FLASH_CFG_TEMP_SENSOR_NLFIT_GAIN_Pos 8
#define FLASH_CFG_TEMP_SENSOR_NLFIT_GAIN_Width 8
#define FLASH_CFG_TEMP_SENSOR_NLFIT_TREF_Pos 16
#define FLASH_CFG_TEMP_SENSOR_NLFIT_TREF_Width 16

#define FLASH_CFG_TEMP_SENSOR_VBATSUM_OFFSET_Pos 0
#define FLASH_CFG_TEMP_SENSOR_VBATSUM_OFFSET_Width 12
#define FLASH_CFG_TEMP_SENSOR_VBATSUM_GAIN_Pos 12
#define FLASH_CFG_TEMP_SENSOR_VBATSUM_GAIN_Width 10
#define FLASH_CFG_TEMP_SENSOR_ADCFS_ERR_Pos 22
#define FLASH_CFG_TEMP_SENSOR_ADCFS_ERR_Width 10

#define FLASH_CFG_TEMP_SENSOR_SLOPEFITINV_GAIN_Pos 0
#define FLASH_CFG_TEMP_SENSOR_SLOPEFITINV_GAIN_Width 16
#define FLASH_CFG_TEMP_SENSOR_SLOPEFITINV_VBATGAIN_Pos 16
#define FLASH_CFG_TEMP_SENSOR_SLOPEFITINV_VBATGAIN_Width 16

#define FIELD_VAL(wrd, _FLD) (((wrd) >> (_FLD##_Pos)) & ((1 << (_FLD##_Width)) - 1))

#define CONFIG_PAGE_BASE_ADDRESS 0x9FC00
#define ADC_TRIM_PARAM_OFFSET (0x80)
#define TEMP_SENSOR_TRIM_VALID_OFFSET (0x8C)
#define TEMP_SENSOR_TRIM_PARAM_OFFSET (0xA0)

#define VBAT_CHANNEL 6

#define FRO12M_FREQ 12000000UL
#define FRO32M_FREQ 32000000UL
#define XTAL32M_FREQ 32000000UL

/*Prototypes*/
bool load_calibration_param_from_flash(ADC_Type *base);
bool get_temperature(ADC_Type *base, uint8_t channel, uint32_t delay_value, uint8_t nb_samples, int32_t *temperature);

#endif /*#ifndef TEMP_SENSOR_DRV_H*/
