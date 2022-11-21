/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VOLTAGE_DRV_H
#define VOLTAGE_DRV_H

/*Types*/
/*typedef struct
{
    uint16_t voltage;
} CfgVoltageSensor_t;*/


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

uint16_t Get_BattVolt(void);

#endif /*#ifndef VOLTAGE_DRV_H*/
