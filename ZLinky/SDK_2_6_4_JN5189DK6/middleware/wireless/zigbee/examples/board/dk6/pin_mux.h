/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/
#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

#include "board.h"
#include "fsl_common.h"

#define IOCON_QSPI_MODE_FUNC        (7U)
#define IOCON_PIO_DIGITAL_EN 0x80u    /*!<@brief Enables digital function */
#define IOCON_PIO_INV_DI 0x00u        /*!<@brief Input function is not inverted */
#define IOCON_PIO_INPFILT_OFF 0x0100u /*!<@brief Input filter disabled */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SSEL_DI 0x00u       /*!<@brief SSEL is disabled */


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/
       /*!
        * @brief configure all pins for this demo/example
        *
        */
void BOARD_InitPins(void);
void BOARD_SetPinsForPowerMode(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _PIN_MUX_H_  */
