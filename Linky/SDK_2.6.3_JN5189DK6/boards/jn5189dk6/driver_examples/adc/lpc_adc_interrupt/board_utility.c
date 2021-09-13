/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "board.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "board_utility.h"
#include "fsl_debug_console.h"

/*****************************************************************************
 * temperature sensor driver
 ****************************************************************************/
#if (ADC_TEMP_SENSOR_DRIVER_EN)

// Comment the define if you want to use specific calibration values or want to debug
//#define LOAD_CAL_FROM_FLASH

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
} CfgTempSensor_tt;

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
#define TEMPERATURE_SENSOR_CHANNEL 7U
#define DEMO_ADC_SAMPLE_TEST_CHANNEL 0U

#if gAdcUsed_d
static adc_conv_seq_config_t adcConvSeqConfigStruct;
#endif

/* Need to remember the latest temperature value */
int last_temperature_report_in_128th_of_degree = TEMP_ZERO_K_128th_DEG; /* absolute zero : no measurement yet */

#ifdef LOAD_CAL_FROM_FLASH
static CfgTempSensor_tt temp_sensor_calibration;
static CfgGpAdcTrim_t adc_calibration;
#else
static CfgTempSensor_tt temp_sensor_calibration = {0,  26214,       68,   29288, 68,   29256, 21001,          20968,
                                                   41, (int8_t)154, 3354, 3756,  1008, 1011,  (int16_t)39453, 1401};
#endif

static CfgTempSensor_tt *cal           = &temp_sensor_calibration;
static bool is_valid_calibration_avail = false;

/* Function shall be called prior reading the temperature.
 * Returns 1 if calibration data is available else returns 0.
 * If there is no calibration data available for ADC or temperature sensor, computation of temperature with
 * get_temperature() is not possible.*/
static bool load_calibration_param_from_flash(ADC_Type *base)
{
    bool is_valid = false;
    do
    {
        is_valid = is_valid_calibration_avail;
#ifdef LOAD_CAL_FROM_FLASH
        if (is_valid)
            break; /* was done already */
        // Load ADC calibration data from flash
        is_valid = flashConfig_ExtractGpAdcTrimValues(&adc_calibration);
        if (!is_valid)
        {
            PRINTF("Calibration data loading error: no ADC calibration data available!\r\n");
            break;
        }

        // Load temperature sensor calibration data from flash
        is_valid = flashConfig_ExtractTempSensorTrimValues(&temp_sensor_calibration);
        if (!is_valid)
        {
            PRINTF("Calibration data loading error: no temperature sensor calibration data available!\r\n");
            break;
        }

        if (temp_sensor_calibration.vbatsum_gain & (1 << 9))
        {
            temp_sensor_calibration.vbatsum_gain = (int16_t)(temp_sensor_calibration.vbatsum_gain | 0xFC00);
        }
        if (temp_sensor_calibration.adcfs_vbat3v3 & (1 << 9))
        {
            temp_sensor_calibration.adcfs_vbat3v3 = (int16_t)(temp_sensor_calibration.adcfs_vbat3v3 | 0xFC00);
        }

        // Initialize ADC with calibration data
        uint32_t reg = 0;
        reg = ((adc_calibration.offset_cal << ADC_GPADC_CTRL1_OFFSET_CAL_Pos) & ADC_GPADC_CTRL1_OFFSET_CAL_Msk);
        reg |= ((adc_calibration.gain_cal << ADC_GPADC_CTRL1_GAIN_CAL_Pos) & ADC_GPADC_CTRL1_GAIN_CAL_Msk);

        base->GPADC_CTRL1 = reg;
#else
        is_valid = true;
#endif
    } while (0);
    is_valid_calibration_avail = is_valid;
    return is_valid;
}

static void update_ctrl0_adc_register(ADC_Type *base, uint8_t mode, uint8_t tsamp)
{
    // read the GPADC_CTRL0 register and update settings
    uint32_t read_reg = base->GPADC_CTRL0;

    // clear the "TEST" and "TSAMP" fields
    read_reg &= ~(ADC_GPADC_CTRL0_TEST_Msk | ADC_GPADC_CTRL0_GPADC_TSAMP_Msk);

    read_reg |= ((tsamp << ADC_GPADC_CTRL0_GPADC_TSAMP_Pos) & ADC_GPADC_CTRL0_GPADC_TSAMP_Msk);
    read_reg |= ((mode << ADC_GPADC_CTRL0_TEST_Pos) & ADC_GPADC_CTRL0_TEST_Msk);

    base->GPADC_CTRL0 = read_reg;
}

static uint16_t compute_estimated_sum(ADC_Type *base, uint8_t nb_samples, uint8_t channel_id)
{
    uint8_t count          = 0U;
    uint16_t estimated_sum = 0U;
    adc_result_info_t adcResultInfoStruct;
    while (count < nb_samples)
    {
        /*  trigger the converter by software. */
        ADC_DoSoftwareTriggerConvSeqA(base);
        // At that step in CTRl register, START = 1
        /* Wait for the converter to be done. */
        while (!ADC_GetChannelConversionResult(base, channel_id, &adcResultInfoStruct))
        {
        }
        estimated_sum += adcResultInfoStruct.result;
#ifdef DBG_DRIVER
        PRINTF("adc_result=%d\r\n", adcResultInfoStruct.result);
#endif
        count++;
    }
    return estimated_sum;
}

static int32_t compute_nlfit_corr(int32_t input)
{
    int32_t signed_T_linearfit = input >> 7;

    // nlfit_corr_tlinearfit_soft_mult128 =
    // (2**7)*(signed_tsens_nlfit_gain*1.0/(2**19))*((T_linearfit-signed_tsens_nlfit_Toffset)**2)
    int32_t coefnlfit1        = (signed_T_linearfit - cal->nlfit_Toffset) * (signed_T_linearfit - cal->nlfit_Toffset);
    int32_t coefnlfit1_shift6 = coefnlfit1 >> 6;

    return (cal->nlfit_gain * coefnlfit1_shift6) >> 6;
}

static int32_t temp_computation(uint16_t adcout_vbat_lsb_sum8, uint16_t tsens_adcout_T_sum8, uint8_t nb_samples)
{
    int16_t diff_adcout_vbat_lsb_mult8 = adcout_vbat_lsb_sum8 - (cal->vbatsum_offset * nb_samples);

    /*
    tsens_slopefitinv_vbatgain is 16 bit signed value
    tsens_slopefitinv_gain is 16 bit signed value

    SlopeFitInv_oC_lsb=
    (signed_tsens_slopefitinv_gain*1.0/(2**18))*(1-(signed_tsens_slopefitinv_vbatgain*(adcout_vbat_lsb-tsens_vbatsum_offset))*1.0/(2**31))
    Trunc under 16 bits: 15 bit + sign bit
    */
    int32_t signed_val_coef1_mult8 = (cal->slopefitinv_vbatgain * diff_adcout_vbat_lsb_mult8);

    int32_t signed_val_coef1_shift16 = signed_val_coef1_mult8 >> 16;
    int32_t signed_val_coef2_mult8   = ((1 << 18) - signed_val_coef1_shift16);
    // Divide by 8 now as the result of signed_val_coef3_shift16 below may not fit in a signed int
    int32_t signed_val_coef2 = ((1 << 18) - signed_val_coef1_shift16) / nb_samples;

    // Rounding: if decimal part >= 0.5
    if (signed_val_coef2_mult8 & (1 << 2))
    {
        if (signed_val_coef1_mult8 & (1 << 31))
        {
            signed_val_coef2--;
        }
        else
        {
            signed_val_coef2++;
        }
    }

    // SlopeFitInv_oC_lsb= (signed_tsens_slopefitinv_gain*1.0/(2**18))*signed_val_coef2/2**15
    // SlopeFitInv_oC_lsb= signed_tsens_slopefitinv_gain*signed_val_coef2/(2**15 * 2**18)
    int32_t signed_val_coef3_shift16 = (cal->slopefitinv_gain * signed_val_coef2) >> 16;

    int32_t signed_val_coef4         = (tsens_adcout_T_sum8 - cal->adcout_Tref_vbat3v3_sum8) * signed_val_coef3_shift16;
    int32_t signed_val_coef4_shift13 = signed_val_coef4 >> 13;

    int32_t T_linearfit_soft_mult128 = signed_val_coef4_shift13 + cal->Tref;

    int32_t Test_Vbatsumcorr_soft_mult128 = ((cal->vbatsum_gain * diff_adcout_vbat_lsb_mult8) / nb_samples) >> 9;

    // T_linearfit : No decimal value would be enough
    // nlfit_corr_tlinearfit = (signed_tsens_nlfit_gain*1.0/(2**19))*((T_linearfit-signed_tsens_nlfit_Toffset)**2)
    // nlfit_corr_tref =  (signed_tsens_nlfit_gain*1.0/(2**19))*((signed_tsens_Tref-signed_tsens_nlfit_Toffset)**2)

    int32_t nlfit_corr_tlinearfit_soft_mult128 = compute_nlfit_corr(T_linearfit_soft_mult128);
    int32_t nlfit_corr_tref_soft_mult128       = compute_nlfit_corr(cal->Tref);

    int32_t Test_final_soft_mult128 = T_linearfit_soft_mult128 + nlfit_corr_tlinearfit_soft_mult128 -
                                      nlfit_corr_tref_soft_mult128 - Test_Vbatsumcorr_soft_mult128;

    return Test_final_soft_mult128;
}

/* Nb_samples shall be a power of 2. The temperature is reported in 128th degree Celsius.
 * Return value is 1 if temperature is computed else 0 is returned (no valid calibration data).*/
static void get_temperature(ADC_Type *base, uint8_t channel, uint8_t nb_samples, int32_t *temperature)
{
    /**********************************************/
    /*ADCout from temperature sensor input in div1*/
    /**********************************************/
#ifdef DBG_DRIVER
    PRINTF("ADC output code from current temperature\r\n");
#endif
    // Setup the temperature sensor to on
    ADC_EnableTemperatureSensor(base, true);
    // Add some delay to prevent first ADC acquisition before temperature sensor is ready
    CLOCK_uDelay(100);

    // Set test mode=2 (ADC in unity gain) and TSAMP = 0x1f
    update_ctrl0_adc_register(base, 0x2, 0x1f);

    // run a 8 sample acquisition
    /* Enable conversion in Sequence A. */
    /* Channel to be used is set up at the beginning of each sequence */
    adcConvSeqConfigStruct.channelMask      = (1 << channel);
    adcConvSeqConfigStruct.triggerMask      = 2U;
    adcConvSeqConfigStruct.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = true;
    adcConvSeqConfigStruct.interruptMode    = kADC_InterruptForEachConversion;
    ADC_SetConvSeqAConfig(base, &adcConvSeqConfigStruct);

    // At that step in CTRl register, TRIGGER=2 (PWM9), TRIGPOL=1, SYNCBYPASS=1, MODE=0
    ADC_EnableConvSeqA(base, true);
    // At that step in CTRl register, SEQ_ENA = 1

    uint16_t tsens_adcout_T_sum8 = compute_estimated_sum(base, nb_samples, channel);

#ifdef DBG_DRIVER
    PRINTF("tsens_adcout_T_sum8 = %d\r\n", tsens_adcout_T_sum8);
    PRINTF("tsens_adcout_T = %d\r\n", tsens_adcout_T_sum8 / nb_samples);
    PRINTF("\r\n");
#endif

    ADC_EnableTemperatureSensor(base, false);
    ADC_EnableConvSeqA(base, false);

    /*************************************/
    /*ADCout from from Vbat input in div4*/
    /*************************************/
#ifdef DBG_DRIVER
    PRINTF("ADC output code from current VBAT\r\n");
#endif

    // Set test mode=0 (ADC in normal mode => /4) and TSAMP = 0x14
    update_ctrl0_adc_register(base, 0x0, 0x14);

    /* Channel to be used is set up at the beginning of each sequence */
    adcConvSeqConfigStruct.channelMask      = (1 << VBAT_CHANNEL);
    adcConvSeqConfigStruct.triggerMask      = 2U;
    adcConvSeqConfigStruct.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = true;
    adcConvSeqConfigStruct.interruptMode    = kADC_InterruptForEachConversion;
    ADC_SetConvSeqAConfig(base, &adcConvSeqConfigStruct);

    ADC_EnableConvSeqA(base, true);

    uint16_t adc12b_estimated_sum = compute_estimated_sum(base, nb_samples, VBAT_CHANNEL);

#ifdef DBG_DRIVER
    PRINTF("adcout_vbat_sum = %d\r\n", adc12b_estimated_sum);
    PRINTF("adcout_vbat_lsb = %d\r\n", adc12b_estimated_sum / nb_samples);
#endif

    *temperature = temp_computation(adc12b_estimated_sum, tsens_adcout_T_sum8, nb_samples);
}
#endif

#if gAdcUsed_d
int32_t BOARD_GetTemperature(void)
{
    uint32_t status                     = 0;
    int32_t temperature_in_128th_degree = 0;
    int32_t temp_int_part               = 0;

#if (ADC_TEMP_SENSOR_DRIVER_EN)
    int32_t temp_dec_part = 0;

    status = load_calibration_param_from_flash(ADC0);

    if (status)
    {
        if (!BOARD_IsADCEnabled())
        {
            ADC_DBG_LOG("ADC not enabled");
            BOARD_InitAdc();
            // A problem with the ADC requires a delay after setup, see RFT 1340
            CLOCK_uDelay(ADC_WAIT_TIME_US);
        }

        get_temperature(ADC0, TEMPERATURE_SENSOR_CHANNEL, 8, &temperature_in_128th_degree);

        last_temperature_report_in_128th_of_degree = temperature_in_128th_degree;

        // integer part
        temp_int_part = temperature_in_128th_degree / 128;
        // Update Temperature in XCVR for calibration purpose
        // decimal part
        temp_dec_part = (((temperature_in_128th_degree - (temp_int_part * 128))) * 10) / 128;
        PRINTF("Temperature = %d.%d C\r\n", (short int)temp_int_part, (short int)temp_dec_part);
        ADC_DBG_LOG("Temperature=%d.%d", temp_int_part, temp_dec_part);
    }
    else
    {
        ADC_DBG_LOG("Wrong ADC configuration or missing calibration data");
        PRINTF("FAIL: temperature measurement failed due to wrong ADC configuration or missing calibration data!\r\n");
    }
#endif
    return temp_int_part;
}

int BOARD_GetLastKnownTemperature(void)
{
    return last_temperature_report_in_128th_of_degree;
}
#endif

const ClockCapacitanceCompensation_t BOARD_Clock32MCapacitanceCharacteristics = {
    .clk_XtalIecLoadpF_x100    = CLOCK_32MfXtalIecLoadpF_x100,
    .clk_XtalPPcbParCappF_x100 = CLOCK_32MfXtalPPcbParCappF_x100,
    .clk_XtalNPcbParCappF_x100 = CLOCK_32MfXtalNPcbParCappF_x100,
};

const ClockCapacitanceCompensation_t BOARD_Clock32kCapacitanceCharacteristics = {
    .clk_XtalIecLoadpF_x100    = CLOCK_32kfXtalIecLoadpF_x100,
    .clk_XtalPPcbParCappF_x100 = CLOCK_32kfXtalPPcbParCappF_x100,
    .clk_XtalNPcbParCappF_x100 = CLOCK_32kfXtalNPcbParCappF_x100,
};

/* Get the pointer of the board specific ClockCapacitanceCompensation_t for 32M. */
const ClockCapacitanceCompensation_t *BOARD_GetClock32MCapacitanceCharacteristics(void)
{
    return &BOARD_Clock32MCapacitanceCharacteristics;
}

/* Get the pointer of the board specific ClockCapacitanceCompensation_t for 32K. */
const ClockCapacitanceCompensation_t *BOARD_GetClock32kCapacitanceCharacteristics(void)
{
    return &BOARD_Clock32kCapacitanceCharacteristics;
}

#if gXo32M_Trim_c
void BOARD_tcxo32M_compensation_run(uint32_t temperature_change_threshold, uint32_t us_delay_post_tcxo_adj)
{
    static int32_t prev_compensation_temperature_in_celsius                          = TEMP_ZERO_K;
    const ClockCapacitanceCompensation_t *default_Clock32MCapacitanceCharacteristics = NULL;
    int32_t XO_32M_OSC_CAP_Delta_x1000                                               = 0;
    int32_t latest_temperature_in_celsius;
    int32_t delta = 0;

    latest_temperature_in_celsius = BOARD_GetLastKnownTemperature() / 128;

    if (latest_temperature_in_celsius == TEMP_ZERO_K) /* Temperature never monitored yet */
    {
        latest_temperature_in_celsius = 25;
    }
    else
    {
        delta = latest_temperature_in_celsius - prev_compensation_temperature_in_celsius;
        delta = ABSOLUTE_VALUE(delta);
    }
    if (delta >= temperature_change_threshold)
    {
        prev_compensation_temperature_in_celsius = latest_temperature_in_celsius;
        XO_32M_OSC_CAP_Delta_x1000               = Calculate_32MOscCapCompensation(latest_temperature_in_celsius);

        default_Clock32MCapacitanceCharacteristics = BOARD_GetClock32MCapacitanceCharacteristics();

        assert(NULL != default_Clock32MCapacitanceCharacteristics);

        CLOCK_Xtal32M_Trim(XO_32M_OSC_CAP_Delta_x1000, default_Clock32MCapacitanceCharacteristics);

        CLOCK_uDelay(us_delay_post_tcxo_adj); /* delay to be determined but should be fast */
    }
}
#endif

#if gXo32k_Trim_c
void BOARD_tcxo32k_compensation_run(uint32_t temperature_change_threshold, uint32_t us_delay_post_tcxo_adj)
{
    static int32_t prev_compensation_temperature_in_celsius = TEMP_ZERO_K;
    int32_t OSC_CAP_Delta_x1000                             = 0;
    int32_t latest_temperature_in_celsius;
    int32_t delta = 0;

    latest_temperature_in_celsius = BOARD_GetLastKnownTemerature() / 128;

    if (latest_temperature_in_celsius == TEMP_ZERO_K) /* Temperature never monitored yet */
    {
        latest_temperature_in_celsius = 25;
    }
    else
    {
        delta = latest_temperature_in_celsius - prev_compensation_temperature_in_celsius;
        delta = ABSOLUTE_VALUE(delta);
    }
    if (delta >= temperature_change_threshold)
    {
        prev_compensation_temperature_in_celsius = latest_temperature_in_celsius;
        xo_32k_osc_cap_delta_fF                  = Calculate_32kOscCapCompensation(latest_temperature_in_celsius);

        CLOCK_Xtal32k_Trim(xo_32k_osc_cap_delta_fF, &BOARD_Clock32kCapacitanceCharacteristics);

        CLOCK_uDelay(us_delay_post_tcxo_adj); /* delay to be determined but should be fast */
    }
}
#endif

#if gXo32M_Trim_c
#if ((defined gTcxo32M_ModeEn_c) && (gTcxo32M_ModeEn_c != 0))

static int iCapDeltaCalculation_MHz_x1000(int32_t iTemp)
{
    uint32_t Ind_A_32M;
    uint32_t Ind_B_32M;
    int32_t a_Temp_32M_x1000;
    int32_t b_Temp_32M_x1000;
    const int32_t *CLOCK_ai32MXtalIecLoadFfVsTemp = NULL;

    /* Get index into array for temperature value at or below the ATE
     * temperature, but not above. For case where ATE temperature is at
     * highest index in array, drop back by 1 because we want to get the
     * next index later */
    if (iTemp >= HW_32M_LOAD_VS_TEMP_MAX)
    {
        Ind_A_32M = ((HW_32M_LOAD_VS_TEMP_MAX - HW_32M_LOAD_VS_TEMP_MIN) / HW_32M_LOAD_VS_TEMP_STEP) - 1;
    }
    else if (iTemp < HW_32M_LOAD_VS_TEMP_MIN)
    {
        Ind_A_32M = 0;
    }
    else
    {
        Ind_A_32M = (iTemp - HW_32M_LOAD_VS_TEMP_MIN) / HW_32M_LOAD_VS_TEMP_STEP;
    }

    /* Get index after the selected one */
    Ind_B_32M = Ind_A_32M + 1;

    CLOCK_ai32MXtalIecLoadFfVsTemp = CLOCK_GetAi32MXtalIecLoadFfVsTemp();
    assert(NULL != CLOCK_ai32MXtalIecLoadFfVsTemp);

    /* Linear fit coefficients calculation */
    a_Temp_32M_x1000 = (CLOCK_ai32MXtalIecLoadFfVsTemp[Ind_B_32M] - CLOCK_ai32MXtalIecLoadFfVsTemp[Ind_A_32M]) /
                       HW_32M_LOAD_VS_TEMP_STEP;
    b_Temp_32M_x1000 = CLOCK_ai32MXtalIecLoadFfVsTemp[Ind_A_32M] -
                       a_Temp_32M_x1000 * (HW_32M_LOAD_VS_TEMP_MIN + HW_32M_LOAD_VS_TEMP_STEP * (int32_t)Ind_A_32M);

    return iTemp * a_Temp_32M_x1000 + b_Temp_32M_x1000;
}

int32_t Calculate_32MOscCapCompensation(int32_t iTemperature)
{
    int32_t x0_32M_osc_cap_delta_fF = 0;
#if ((defined gTcxo32M_ModeEn_c) && (gTcxo32M_ModeEn_c != 0))

    int16_t i16AteTemp;

    /* LUT Normalization to ATE temperature */
    i16AteTemp = i16GetAteTemp();

    if (i16AteTemp != 23)
    {
        x0_32M_osc_cap_delta_fF = iCapDeltaCalculation_MHz_x1000(i16AteTemp);
    }
    else
    {
        x0_32M_osc_cap_delta_fF = 0;
    }

    x0_32M_osc_cap_delta_fF = iCapDeltaCalculation_MHz_x1000(iTemperature) - x0_32M_osc_cap_delta_fF;
#endif
    return x0_32M_osc_cap_delta_fF;
}
#endif
#endif

#if defined gXo32k_Trim_c && (gXo32k_Trim_c == 1)
#if (defined gTcxo32k_ModeEn_c) && (gTcxo32k_ModeEn_c != 0)

static int iCapDeltaCalculation_kHz_x1000(int32_t iTemp)
{
    uint32_t Ind_A_32k;
    uint32_t Ind_B_32k;
    int32_t a_Temp_32k_x1000;
    int32_t b_Temp_32k_x1000;
    const int32_t *CLOCK_ai32kXtalIecLoadFfVsTemp = NULL;

    /* Get index into array for temperature value at or below the ATE
     * temperature, but not above. For case where ATE temperature is at
     * highest index in array, drop back by 1 because we want to get the
     * next index later */
    if (iTemp >= HW_32k_LOAD_VS_TEMP_MAX)
    {
        Ind_A_32k = ((HW_32k_LOAD_VS_TEMP_MAX - HW_32k_LOAD_VS_TEMP_MIN) / HW_32k_LOAD_VS_TEMP_STEP) - 1;
    }
    else if (iTemp < HW_32k_LOAD_VS_TEMP_MIN)
    {
        Ind_A_32k = 0;
    }
    else
    {
        Ind_A_32k = (iTemp - HW_32k_LOAD_VS_TEMP_MIN) / HW_32k_LOAD_VS_TEMP_STEP;
    }

    /* Get index after the selected one */
    Ind_B_32k = Ind_A_32k + 1;

    CLOCK_ai32kXtalIecLoadFfVsTemp = CLOCK_GetAi32kXtalIecLoadFfVsTemp();
    assert(NULL != CLOCK_ai32kXtalIecLoadFfVsTemp);

    /* Linear fit coefficients calculation */
    a_Temp_32k_x1000 = (CLOCK_ai32kXtalIecLoadFfVsTemp[Ind_B_32k] - CLOCK_ai32kXtalIecLoadFfVsTemp[Ind_A_32k]) /
                       HW_32k_LOAD_VS_TEMP_STEP;
    b_Temp_32k_x1000 = CLOCK_ai32kXtalIecLoadFfVsTemp[Ind_A_32k] -
                       a_Temp_32k_x1000 * (HW_32k_LOAD_VS_TEMP_MIN + HW_32k_LOAD_VS_TEMP_STEP * Ind_A_32k);

    return iTemp * a_Temp_32k_x1000 + b_Temp_32k_x1000;
}

int32_t Calculate_32kOscCapCompensation(int32_t iTemperature)
{
    int32_t xo_32k_osc_cap_delta_fF = 0;
#if (defined gTcxo32k_ModeEn_c) && (gTcxo32k_ModeEn_c != 0)
    int16_t i16AteTemp;

    /* LUT Normalization to ATE temperature */
    i16AteTemp = i16GetAteTemp();

    if (i16AteTemp != 23)
    {
        xo_32k_osc_cap_delta_fF = iCapDeltaCalculation_kHz_x1000(i16AteTemp);
    }
    else
    {
        xo_32k_osc_cap_delta_fF = 0;
    }

    xo_32k_osc_cap_delta_fF = iCapDeltaCalculation_kHz_x1000(iTemperature) - xo_32k_osc_cap_delta_fF;
#endif
    return xo_32k_osc_cap_delta_fF;
}
#endif
#endif

#if (((defined gTcxo32k_ModeEn_c) && (gTcxo32k_ModeEn_c != 0)) || \
     ((defined gTcxo32M_ModeEn_c) && (gTcxo32M_ModeEn_c != 0)))

#define GET_ATE_TEMP() (*(uint32_t *)0x9FDC8)

#if gXo32M_Trim_c
#if ((defined gTcxo32M_ModeEn_c) && (gTcxo32M_ModeEn_c != 0))
/* Table of load capacitance versus temperature for 32MHz crystal. Values are
   for temperatures from -40 to +130 in steps of 5, expressed in femto Farads */
#if OLD_XTAL32M_CAL
static const int32_t CLOCK_ai32MXtalIecLoadFfVsTemp[HW_32M_LOAD_VS_TEMP_SIZE] = {
    537,   714,   837,   909,   935,  922,  873,  796,   /* -40, -35, ... -5 */
    694,   574,   440,   297,   149,  0,    -147, -290,  /* 0, 5, ... 35 */
    -425,  -551,  -667,  -770,  -860, -936, -998, -1042, /* 40, 45, ... 75 */
    -1069, -1076, -1061, -1019, -947, -837, -682, -472,  /* 80, 85, ... 115 */
    -196,  161,   618                                    /* 120, 125, 130 */
};
#else
static const int32_t CLOCK_ai32MXtalIecLoadFfVsTemp[HW_32M_LOAD_VS_TEMP_SIZE] = {
    506,   687,   818,   900,  938,  933,  892,   819,   /* -40, -35, ... -5 */
    720,   599,   461,   313,  158,  0,    -156,  -307,  /* 0, 5, ... 35 */
    -451,  -584,  -705,  -812, -903, -977, -1032, -1067, /* 40, 45, ... 75 */
    -1079, -1067, -1025, -950, -835, -673, -453,  -163,  /* 80, 85, ... 115 */
    212,   691,   1296                                   /* 120, 125, 130 */
};
#endif

const int32_t *CLOCK_GetAi32MXtalIecLoadFfVsTemp(void)
{
    return CLOCK_ai32MXtalIecLoadFfVsTemp;
}

#endif
#endif

#if defined gXo32k_Trim_c && (gXo32k_Trim_c == 1)
#if (defined gTcxo32k_ModeEn_c) && (gTcxo32k_ModeEn_c != 0)
/* Capacitance variation for 32kHz crystal across temperature
   ----------------------------------------------------------

   gTcxo32k_ModeEn_c should be 1 to indicate that temperature-compensated 32kHz
   XO is supported and required. If so, HW_32k_LOAD_VS_TEMP_MIN,
   _MAX, _STEP must be defined here and CLOCK_ai32kXtalIecLoadFfVsTemp
   must be defined in board.c.

   Values are used as follows:
   CLOCK_ai32kXtalIecLoadFfVsTemp is an array of crystal load capacitance
   values across temp, with each value being at a specific temp. First value is
   for temp given by HW_32k_LOAD_VS_TEMP_MIN, next value is for
   temp given by HW_32k_LOAD_VS_TEMP_MIN + _STEP, next value is
   for temp given by HW_32k_LOAD_VS_TEMP_MIN + _ STEP x 2, etc.
   Final value is for temp given by HW_32k_LOAD_VS_TEMP_MAX. It is
   important for HW_32k_LOAD_VS_TEMP_x defines and the table to be
   matched to one another */

/* Table of load capacitance versus temperature for 32kHz crystal. Values are
   for temperatures from -20 to +100 in steps of 20. *Note* values below are
   just for example */
#ifdef OLD_XTAL32K_CAL
static const int32_t CLOCK_ai32kXtalIecLoadFfVsTemp[HW_32k_LOAD_VS_TEMP_SIZE] = {
    -1843, -1583, -1351, -1138, -939,  -752,  -579,  -422,  /* -40, -35, ... -5 */
    -285,  -170,  -81,   -22,   5,     0,     -38,   -107,  /* 0, 5, ... 35 */
    -205,  -330,  -478,  -645,  -830,  -1031, -1247, -1482, /* 40, 45, ... 75 */
    -1742, -2078, -2409, -2766, -3147, -3554, -3995, -4442, /* 80, 85, ... 115 */
    -4923, -5430, -5961};                                   /* 120, 125, 130 */
#else
static const int32_t CLOCK_ai32kXtalIecLoadFfVsTemp[HW_32k_LOAD_VS_TEMP_SIZE] = {
    -1984, -1728, -1496, -1278, -1070, -870,  -680,  -504,  /* -40, -35, ... -5 */
    -347,  -214,  -109,  -37,   0,     0,     -37,   -111,  /* 0, 5, ... 35 */
    -218,  -356,  -521,  -709,  -917,  -1143, -1388, -1656, /* 40, 45, ... 75 */
    -1957, -2293, -2657, -3048, -3467, -3913, -4386, -4887, /* 80, 85, ... 115 */
    -5416, -5971, -6555};                                   /* 120, 125, 130 */
#endif

const int32_t *CLOCK_GetAi32kXtalIecLoadFfVsTemp(void)
{
    return CLOCK_ai32kXtalIecLoadFfVsTemp;
}
#endif
#endif

int16_t i16GetAteTemp(void)
{
    uint32_t u32AteTempReg;
    int16_t i16AteTempValue;

    /* ATE temperature should be stored in flash in a 32-bit word:
         bit 0     1 if value is valid, 0 if not
         bit 16:1  Value, degrees C x 128 */
    u32AteTempReg = GET_ATE_TEMP();
    if (u32AteTempReg & 1U)
    {
        /* Stored value is valid, so read it, taking care of sign bits */
        i16AteTempValue = (int16_t)(u32AteTempReg >> 1U);
        i16AteTempValue /= 128;
    }
    else
    {
        /* Assume 23 degrees C */
        i16AteTempValue = 23;
    }

    return i16AteTempValue;
}
#endif
