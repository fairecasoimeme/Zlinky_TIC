/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_NTAG_H_
#define __FSL_NTAG_H_

#include "fsl_common.h"

/*!
 * @addtogroup jn_ntag
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief ntag operating state */
typedef enum _ntag_state
{
    kNTAG_StateActive,  /*!< NTAG powered on (ready for I2C communication) */
    kNTAG_StateInactive /*!< NTAG powered off or MCU in low power state */
} ntag_state_t;

/*! @brief Field Detect line state */
typedef enum _ntag_FieldDetect
{
    kNTAG_FieldDetectIn, /*!< NTAG is in field */
    kNTAG_FieldDetectOut /*!< NTAG is out of field */
} ntag_field_detect_t;

/*! @brief NTAG Field Detect callback typedef. */
typedef void (*ntag_field_detect_callback_t)(ntag_field_detect_t fd, void *userData);

/*! @brief NTAG user configuration. */
typedef struct _ntag_config
{
    ntag_field_detect_callback_t callback; /*!< A callback function called at the transfer event. */
    void *userData;                        /*!< A callback parameter passed to the callback function. */
} ntag_config_t;

/*! @brief FD polling implementation options. */
#define RFT1503
//#define RFT1750

/*! @brief NTAG FD interrupt line */
#define NTAG_IRQ NFCTag_IRQn

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @brief  Sets the NTAG configuration structure to default values.
 *
 * @param config A pointer to the configuration structure.
 */
void NTAG_GetDefaultConfig(ntag_config_t *config);

/*!
 * @brief Initialize the internal NTAG peripheral
 *
 * @param config A pointer to the NTAG configuration structure
 */
void NTAG_Init(const ntag_config_t *config);

/*!
 * @brief Poll state of Field Detect line
 *
 */
ntag_field_detect_t NTAG_PollFieldDetect(void);

/*!
 * @brief Configure NTAG operating state
 *
 * @param state NTAG operating state
 */
void NTAG_SetState(ntag_state_t state);

/* @} */
#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*@}*/

#endif /* __FSL_NTAG_H_*/
