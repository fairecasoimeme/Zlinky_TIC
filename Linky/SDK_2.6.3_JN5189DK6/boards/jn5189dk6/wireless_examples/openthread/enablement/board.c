/*
* Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_debug_console.h"
#include "fsl_clock.h"
#include "board.h"
#include "pin_mux.h"


/*******************************************************************************
 * Code
 ******************************************************************************/
#if (defined TCXO_32M_MODE_EN) && (TCXO_32M_MODE_EN != 0)
/* Table of load capacitance versus temperature for 32MHz crystal. Values below
   are for NDK NX2016SA 32MHz EXS00A-CS11213-6(IEC). Values are for temperatures
   from -40 to +130 in steps of 5 */
int32_t CLOCK_ai32MXtalIecLoadPfVsTemp_x1000[HW_32M_LOAD_VS_TEMP_SIZE] =
    {  960,  1097,  1194,  1246,  1253,  1216,  1137,  1023, /* -40, -35, ... -5 */
       879,   710,   523,   325,   122,   -81,  -277,  -464, /* 0, 5, ... 35 */
      -637,  -794,  -933, -1052, -1150, -1227, -1283, -1317, /* 40, 45, ... 75 */
     -1328, -1315, -1274, -1202, -1090,  -930,  -709,  -409, /* 80, 85, ... 115 */
        -9,   518,  1205};                                   /* 120, 125, 130 */
#endif

#if (defined TCXO_32k_MODE_EN) && (TCXO_32k_MODE_EN != 0)
/* Table of load capacitance versus temperature for 32kHz crystal. Values are
   for temperatures from -20 to +100 in steps of 20. *Note* values below are
   just for example */
int32_t CLOCK_ai32kXtalIecLoadPfVsTemp_x1000[HW_32k_LOAD_VS_TEMP_SIZE] =
    {  960,  /* -20 */
      1097,  /*   0 */
      1194,  /*  20 */
      1246,  /*  40 */
      1253,  /*  60 */
      1216,  /*  80 */
      1137}; /* 100 */
#endif

/*******************************************************************************
* Local Prototypes
******************************************************************************/

/*****************************************************************************
* Local functions
****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

uint32_t BOARD_GetUsartClock(int8_t instance)
{
    (void)instance;
    uint32_t ret = CLOCK_GetFreq(kCLOCK_Fro32M);
    return ret;
}

void BOARD_Init_UART_Pins(uint32_t instance)
{
   switch (instance)
    {
        case 0:
            BOARD_InitUART0Pins();
            break;

        case 1:
            BOARD_InitUART1Pins();
            break;

        default:
            break;
    }
}


