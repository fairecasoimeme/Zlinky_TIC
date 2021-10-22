/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "temp_sensor.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_ADC_BASE ADC0
#define DEMO_ADC_TEMPERATURE_SENSOR_CHANNEL 7U
#define DEMO_ADC_SAMPLE_TEST_CHANNEL 0U
#define THREE_HUNDRED_MICROSECONDS 300

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

// uncomment if you want to verify the computation of the temperature against expected values (unit test)
//#define TEST
#ifdef TEST
#include "temp_sensor_drv_test.h"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

static void ADC_ClockPower_Configuration(void)
{
    /* Enable ADC clock */
    CLOCK_EnableClock(kCLOCK_Adc0);

    // CLOCK_EnableClock(kCLOCK_PVT);

    /* Power on the ADC converter. */
    POWER_EnablePD(kPDRUNCFG_PD_LDO_ADC_EN);

    /* Enable the clock. */
    CLOCK_AttachClk(kXTAL32M_to_ADC_CLK);

    /* Enable LDO ADC 1v1 */
    PMC->PDRUNCFG |= PMC_PDRUNCFG_ENA_LDO_ADC_MASK;
}


/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize board hardware. */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    /* Init the boards */
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /* Enable the power and clock for ADC. */
    ADC_ClockPower_Configuration();
    PRINTF("Temperature measurement example.\r\n");

#ifdef TEST
    test_temperature_computation();
#else
    bool status = false;
    // Read temperature and adc calibration parameter once. Store ADC calibration values into ADC register.
    status = load_calibration_param_from_flash(DEMO_ADC_BASE);

    while (1)
    {
        // get the temperature (average on 8 ADC samples)
        int32_t temperature_in_128th_degree = 0;
        status = get_temperature(DEMO_ADC_BASE, DEMO_ADC_TEMPERATURE_SENSOR_CHANNEL, THREE_HUNDRED_MICROSECONDS, 8,
                                 &temperature_in_128th_degree);
        if (status)
        {
            // integer part
            int32_t temp_int_part = temperature_in_128th_degree / 128;
            // decimal part
            int32_t temp_dec_part = (((temperature_in_128th_degree - (temp_int_part * 128))) * 10) / 128;
            PRINTF("\r\n Temperature in Celsius (average on 8 samples) = %d.%d\r\n", temp_int_part, temp_dec_part);
        }
        else
        {
            PRINTF(
                "FAIL: temperature measurement failed due to wrong ADC configuration or missing calibration data!\r\n");
        }
        PRINTF("\r\n");
        /* Get the input from terminal to start another conversion  */
        GETCHAR();
    }
#endif
}
