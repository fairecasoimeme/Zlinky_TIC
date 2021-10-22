/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_rng.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_PINT_PIN_INT0_SRC kINPUTMUX_GpioPort0Pin4ToPintsel
#define DEMO_PINT_PIN_INT1_SRC kINPUTMUX_GpioPort0Pin5ToPintsel

#define MAX_NUM_BYTES (16)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void delay(uint32_t d);


/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint8_t rand_data_buf[MAX_NUM_BYTES];

/*******************************************************************************
 * Code
 ******************************************************************************/

void delay(uint32_t d)
{
    // 800000 gives about 200ms
    volatile uint32_t i = 0;
    for (i = 0; i < d; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

void rng_updateonce_example(void)
{
    trng_config_t user_config;
    uint32_t rand_data;

    TRNG_GetDefaultConfig(&user_config);

    /* Change to update once mode */
    user_config.mode = trng_UpdateOnce;
    TRNG_Init(RNG, &user_config);

    /* Read 32bit data */
    TRNG_GetRandomData(RNG, &rand_data, sizeof(uint32_t));

    PRINTF("Update once example: 0x%x \r\n", rand_data);
}

void rng_freerunning_example(void)
{
    trng_config_t user_config;
    uint32_t rand_data;
    uint32_t i;
    uint32_t j;

    TRNG_GetDefaultConfig(&user_config);

    /* Initialise RNG */
    TRNG_Init(RNG, &user_config);

    /* Read various data sizes */
    for (i = 1; i <= MAX_NUM_BYTES; i++)
    {
        PRINTF("No. of bytes Read: 0x%x\r\n", i);

        /* Reset buffer to known value */
        for (j = 0; j < i; j++)
        {
            rand_data_buf[j] = 0;
        }

        /* Read random data */
        TRNG_GetRandomData(RNG, &rand_data_buf[0], (i * sizeof(uint8_t)));

        /* Print random data */
        for (j = 0; j < i; j++)
        {
            PRINTF("Byte: 0x%x Random data: 0x%x \r\n", j, rand_data_buf[j]);
        }
    }

    /* Read random data in a loop */
    while (1)
    {
        TRNG_GetRandomData(RNG, &rand_data, sizeof(uint32_t));

        PRINTF("Free running example: 0x%x\r\n", rand_data);
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init the boards */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* Init the boards */
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    delay(10000);

    PRINTF("RNG Examples \n\r");

    /* Update once example */
    rng_updateonce_example();

    /* FreeRunning example */
    rng_freerunning_example();

    /* Should never arrive here */
    return 1;
}
