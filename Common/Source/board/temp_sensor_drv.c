/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "temp_sensor_drv.h"

// Uncomment the define to get trace output for debugging
//#define DBG_DRIVER

// Comment the define if you want to use specific calibration values or want to debug
#define LOAD_CAL_FROM_FLASH

// Comment the define if you want to use asynchronous mode
//#define USE_SYNCHRONOUS_MODE

#ifdef LOAD_CAL_FROM_FLASH
static CfgTempSensor_t temp_sensor_calibration;
static CfgGpAdcTrim_t adc_calibration;
#else
static CfgTempSensor_t temp_sensor_calibration = {0,  26214, 68,   29288, 68,   29256, 21001, 20968,
                                                  41, 154,   3354, 3756,  1008, 1011,  39453, 1401};
static CfgGpAdcTrim_t adc_calibration          = {646, 489, 0, 0, 0};
#endif

CfgTempSensor_t *cal = &temp_sensor_calibration;
adc_conv_seq_config_t adcConvSeqConfigStruct;
adc_config_t adcConfigStruct;
bool is_valid_calibration_avail = false;

/*Private functions*/
bool flashConfig_ExtractGpAdcTrimValues(CfgGpAdcTrim_t *gp_adc_cfg)
{
    uint32_t *gpadc_cal = (uint32_t *)(CONFIG_PAGE_BASE_ADDRESS + ADC_TRIM_PARAM_OFFSET);

    int valid = 0;

    do
    {
        uint32_t wrd = gpadc_cal[0];

        valid = FIELD_VAL(wrd, FLASH_CFG_GPADC_TRIM_VALID);
        if (!valid)
            break;

        gp_adc_cfg->offset_cal           = FIELD_VAL(wrd, FLASH_CFG_GPADC_OFFSETCAL);
        gp_adc_cfg->gain_cal             = FIELD_VAL(wrd, FLASH_CFG_GPADC_GAINCAL);
        gp_adc_cfg->unity_cal            = FIELD_VAL(wrd, FLASH_CFG_GPADC_UNITY_OFFSET_CAL);
        wrd                              = gpadc_cal[1];
        gp_adc_cfg->unity_vout_val       = FIELD_VAL(wrd, FLASH_CFG_GPADC_UNITY_OFFSET_VOUT_VAL);
        gp_adc_cfg->unity_gain_input_val = FIELD_VAL(wrd, FLASH_CFG_GPADC_UNITY_GAIN_INPUT_VAL);

    } while (0);

    return valid;
}

bool flashConfig_ExtractTempSensorTrimValues(CfgTempSensor_t *temp_sensor_cfg)
{
    uint32_t *tempsensor_trim_valid = (uint32_t *)(CONFIG_PAGE_BASE_ADDRESS + TEMP_SENSOR_TRIM_VALID_OFFSET);
    uint32_t *temp_sensor           = (uint32_t *)(CONFIG_PAGE_BASE_ADDRESS + TEMP_SENSOR_TRIM_PARAM_OFFSET);

    int valid = 0;

    do
    {
        uint32_t wrd = *tempsensor_trim_valid;

        valid = FIELD_VAL(wrd, FLASH_CFG_TEMPSENSOR_TRIM_VALID);
        if (!valid)
            break;

        /* valid bit belongs to another separate flash line or 'word' so need a new safe read */
        wrd                                         = temp_sensor[0];
        temp_sensor_cfg->ref0v0                     = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_REF0V0);
        temp_sensor_cfg->ref0v8                     = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_REF0V8);
        wrd                                         = temp_sensor[1];
        temp_sensor_cfg->adcout_ref0v0_vbat3v3_sum8 = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V0_VBAT3V3_SUM8);
        temp_sensor_cfg->adcout_ref0v8_vbat3v3_sum8 = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V8_VBAT3V3_SUM8);
        wrd                                         = temp_sensor[2];
        temp_sensor_cfg->adcout_ref0v0_vbat1v8_sum8 = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V0_VBAT1V8_SUM8);
        temp_sensor_cfg->adcout_ref0v8_vbat1v8_sum8 = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCOUT_REF0V8_VBAT1V8_SUM8);
        wrd                                         = temp_sensor[3];
        temp_sensor_cfg->adcout_Tref_vbat3v3_sum8   = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCOUT_TREF_VBAT3V3_SUM8);
        temp_sensor_cfg->adcout_Tref_vbat1v8_sum8   = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCOUT_TREF_VBAT1V8_SUM8);
        wrd                                         = temp_sensor[4];
        temp_sensor_cfg->nlfit_Toffset              = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_NLFIT_TOFFSET);
        temp_sensor_cfg->nlfit_gain                 = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_NLFIT_GAIN);
        temp_sensor_cfg->Tref                       = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_NLFIT_TREF);
        wrd                                         = temp_sensor[5];
        temp_sensor_cfg->vbatsum_offset             = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_VBATSUM_OFFSET);
        temp_sensor_cfg->vbatsum_gain               = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_VBATSUM_GAIN);
        temp_sensor_cfg->adcfs_vbat3v3              = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_ADCFS_ERR);
        wrd                                         = temp_sensor[6];
        temp_sensor_cfg->slopefitinv_gain           = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_SLOPEFITINV_GAIN);
        temp_sensor_cfg->slopefitinv_vbatgain       = FIELD_VAL(wrd, FLASH_CFG_TEMP_SENSOR_SLOPEFITINV_VBATGAIN);
    } while (0);

    return valid;
}

void load_adc_and_temp_cal_parameter(CfgTempSensor_t *temp_cal, CfgGpAdcTrim_t *adc_cal)
{
    memcpy(&temp_sensor_calibration, temp_cal, sizeof(CfgTempSensor_t));
    memcpy(&adc_calibration, adc_cal, sizeof(CfgGpAdcTrim_t));

    cal = (CfgTempSensor_t *)&temp_sensor_calibration;
    if (cal->vbatsum_gain & (1 << 9))
    {
        temp_sensor_calibration.vbatsum_gain = (int16_t)(cal->vbatsum_gain | 0xFC00);
    }
    if (cal->adcfs_vbat3v3 & (1 << 9))
    {
        temp_sensor_calibration.adcfs_vbat3v3 = (int16_t)(cal->adcfs_vbat3v3 | 0xFC00);
    }
}

static uint16_t compute_estimated_sum(ADC_Type *base, uint8_t nb_samples, uint8_t channel_id)
{
    uint8_t count          = 0;
    uint16_t estimated_sum = 0;
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

static int32_t compute_nlfit_corr(int32_t input)
{
    int32_t signed_T_linearfit = input >> 7;

    // nlfit_corr_tlinearfit_soft_mult128 =
    // (2**7)*(signed_tsens_nlfit_gain*1.0/(2**19))*((T_linearfit-signed_tsens_nlfit_Toffset)**2)
    int32_t coefnlfit1        = (signed_T_linearfit - cal->nlfit_Toffset) * (signed_T_linearfit - cal->nlfit_Toffset);
    int32_t coefnlfit1_shift6 = coefnlfit1 >> 6;

    return (cal->nlfit_gain * coefnlfit1_shift6) >> 6;
}

/*Public functions*/
int32_t temp_computation(uint16_t adcout_vbat_lsb_sum8, uint16_t tsens_adcout_T_sum8, uint8_t nb_samples)
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

#ifdef DBG_DRIVER
    if (T_linearfit_soft_mult128 < 0)
    {
        PRINTF("T_linearfit_soft_mult128=-%d\r\n", T_linearfit_soft_mult128);
    }
    else
    {
        PRINTF("T_linearfit_soft_mult128=%d\r\n", T_linearfit_soft_mult128);
    }
    if (Test_Vbatsumcorr_soft_mult128 < 0)
    {
        PRINTF("Test_Vbatsumcorr_soft_mult128=-%d\r\n", Test_Vbatsumcorr_soft_mult128);
    }
    else
    {
        PRINTF("Test_Vbatsumcorr_soft_mult128=%d\r\n", Test_Vbatsumcorr_soft_mult128);
    }
    if (nlfit_corr_tlinearfit_soft_mult128 < 0)
    {
        PRINTF("nlfit_corr_tlinearfit_soft_mult128=-%d\r\n", nlfit_corr_tlinearfit_soft_mult128);
    }
    else
    {
        PRINTF("nlfit_corr_tlinearfit_soft_mult128=%d\r\n", nlfit_corr_tlinearfit_soft_mult128);
    }
    if (nlfit_corr_tref_soft_mult128 < 0)
    {
        PRINTF("nlfit_corr_tref_soft_mult128=-%d\r\n", nlfit_corr_tref_soft_mult128);
    }
    else
    {
        PRINTF("nlfit_corr_tref_soft_mult128=%d\r\n", nlfit_corr_tref_soft_mult128);
    }
#endif

    return Test_final_soft_mult128;
}

bool ADC_Configuration(ADC_Type *base, uint32_t delay_value)
{
    int valid = 1;
/* Configure the converter. */
#ifdef USE_SYNCHRONOUS_MODE
    uint32_t freq             = CLOCK_GetFreq(kCLOCK_CoreSysClk);
    adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
    switch (freq)
    {
        case FRO12M_FREQ:
        {
            adcConfigStruct.clockDividerNumber = 2; /* The divider for sync clock is 3. */
            break;
        }
        case FRO32M_FREQ:
        {
            adcConfigStruct.clockDividerNumber = 7; /* The divider for sync clock is 8. */
            break;
        }
        default:
        {
            valid = 0;
            PRINTF("ADC configuration error: unsupported main clk in driver\r\n");
        }
    }
#else
    adcConfigStruct.clockMode   = kADC_ClockAsynchronousMode; /* Using async clock source. */
    adc_clock_src_t adc_clk_src =
        (adc_clock_src_t)((SYSCON->ADCCLKSEL) & SYSCON_ADCCLKSEL_SEL_MASK >> SYSCON_ADCCLKSEL_SEL_SHIFT);
    switch (adc_clk_src)
    {
        case kCLOCK_AdcXtal32M:
        {
            CLOCK_SetClkDiv(kCLOCK_DivAdcClk, 8, true);
            break;
        }
        case kCLOCK_AdcFro12M:
        {
            CLOCK_SetClkDiv(kCLOCK_DivAdcClk, 3, true);
            break;
        }
        case kCLOCK_AdcNoClock:
        default:
        {
            valid = 0;
            PRINTF("ADC configuration error: no clock selected in asynchronous mode\r\n");
            break;
        }
    }
#endif /*#ifdef USE_SYNCHRONOUS_MODE*/

    if (!valid)
    {
        return valid;
    }

    adcConfigStruct.resolution = kADC_Resolution12bit;
#if defined(FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL) && FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL
    adcConfigStruct.enableBypassCalibration = false;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL. */
    adcConfigStruct.sampleTimeNumber = 0U;
    ADC_Init(base, &adcConfigStruct);

    /* Enable conversion in Sequence A. */
    /* Channel to be used is set up at the beginning of each sequence */
    adcConvSeqConfigStruct.channelMask      = 0;
    adcConvSeqConfigStruct.triggerMask      = 0U;
    adcConvSeqConfigStruct.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = false;
    adcConvSeqConfigStruct.interruptMode    = kADC_InterruptForEachSequence;
    ADC_SetConvSeqAConfig(base, &adcConvSeqConfigStruct);
#ifdef CPU_JN518X
    // A problem with the ADC requires a delay after setup, see RFT 1340
    CLOCK_uDelay(delay_value);
#endif
    ADC_EnableConvSeqA(base, true); /* Enable the conversion sequence A. */

    return valid;
}

/* Function shall be called prior reading the temperature.
 * Returns 1 if calibration data is available else returns 0.
 * If there is no calibration data available for ADC or temperature sensor, computation of temperature with
 * get_temperature() is not possible.*/
bool load_calibration_param_from_flash(ADC_Type *base)
{
    bool is_valid;

#ifdef LOAD_CAL_FROM_FLASH
    // Load ADC calibration data from flash
    is_valid = flashConfig_ExtractGpAdcTrimValues(&adc_calibration);
    if (!is_valid)
    {
        PRINTF("Calibration data loading error: no ADC calibration data available!\r\n");
        return is_valid;
    }

    // Load temperature sensor calibration data from flash
    is_valid = flashConfig_ExtractTempSensorTrimValues(&temp_sensor_calibration);
    if (!is_valid)
    {
        PRINTF("Calibration data loading error: no temperature sensor calibration data available!\r\n");
        return is_valid;
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
    reg          = ((adc_calibration.offset_cal << ADC_GPADC_CTRL1_OFFSET_CAL_Pos) & ADC_GPADC_CTRL1_OFFSET_CAL_Msk);
    reg |= ((adc_calibration.gain_cal << ADC_GPADC_CTRL1_GAIN_CAL_Pos) & ADC_GPADC_CTRL1_GAIN_CAL_Msk);

    base->GPADC_CTRL1 = reg;
#else
    is_valid = true;
#endif

    is_valid_calibration_avail = true;

    return is_valid;
}

/*Nb_samples shall be a power of 2. The temperature is reported in 128th degree Celsius.
 * Return value is 1 if temperature is computed else 0 is returned (no valid calibration data).*/
bool get_temperature(ADC_Type *base, uint8_t channel, uint32_t delay_value, uint8_t nb_samples, int32_t *temperature)
{
    bool is_valid = false;
    if (!is_valid_calibration_avail)
    {
        return is_valid;
    }

    is_valid = ADC_Configuration(base, delay_value);
    if (!is_valid)
    {
        return is_valid;
    }

    ADC_EnableConvSeqA(base, false);
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
    return true;
}
