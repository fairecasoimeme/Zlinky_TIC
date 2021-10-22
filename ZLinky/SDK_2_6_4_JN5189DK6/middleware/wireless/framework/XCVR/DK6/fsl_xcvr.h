/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FSL_XCVR_H_
/* clang-format off */
#define _FSL_XCVR_H_
/* clang-format on */

#include "EmbeddedTypes.h"

/*!
 * @addtogroup xcvr
 * @{
 */

/*! @file*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief  Error codes for the XCVR driver. */
typedef enum _xcvrStatus
{
    gXcvrSuccess_c = 0,
    gXcvrInvalidParameters_c,
    gXcvrUnsupportedOperation_c,
    gXcvrTrimFailure_c
} xcvrStatus_t;


/*! @brief  Radio operating mode setting types. */
typedef enum _radio_mode
{
    BLE_MODE = 0,
    ZIGBEE_MODE = 1,
    ANT_MODE = 2,

    /* BT=0.5, h=** */
    GFSK_BT_0p5_h_0p5  = 3, /* < BT=0.5, h=0.5 [BLE at 1MBPS data rate; CS4 at 250KBPS data rate] */
    GFSK_BT_0p5_h_0p32 = 4, /* < BT=0.5, h=0.32*/
    GFSK_BT_0p5_h_0p7  = 5, /* < BT=0.5, h=0.7 [ CS1 at 500KBPS data rate] */
    GFSK_BT_0p5_h_1p0  = 6, /* < BT=0.5, h=1.0 [ CS4 at 250KBPS data rate] */

    /* BT=** h=0.5 */
    GFSK_BT_0p3_h_0p5 = 7, /* < BT=0.3, h=0.5 [ CS2 at 1MBPS data rate] */
    GFSK_BT_0p7_h_0p5 = 8, /* < BT=0.7, h=0.5 */

    MSK = 9,
    NUM_RADIO_MODES = 10,
} radio_mode_t;


/*! @brief  Data rate selections. */
typedef enum _data_rate
{
    DR_1MBPS = 0, /* Must match bit assignment in BITRATE field */
    DR_500KBPS = 1, /* Must match bit assignment in BITRATE field */
    DR_250KBPS = 2, /* Must match bit assignment in BITRATE field */
    DR_2MBPS = 3, /* Must match bit assignment in BITRATE field */
    DR_UNASSIGNED = 4, /* Must match bit assignment in BITRATE field */
} data_rate_t;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name XCVR functional Operation
 * @{
 */

/*!
 * @brief  Initializes an XCVR instance.
 *
 * This function initializes the XCVR module according to the radio_mode and data_rate settings.
 * This the only function call required to start up the XCVR in most situations.
 *
 * @param radio_mode The radio mode for which the XCVR should be configured.
 * @param data_rate The data rate for which the XCVR should be configured.
 * @note This function encompasses the ::XCVR_RadioBleConfigure() and XCVR_RadioZigbeeConfigure functions.
 */
xcvrStatus_t XCVR_Init(radio_mode_t radio_mode, data_rate_t data_rate);

/*!
 * @brief  Reinitializes an XCVR instance on wake up after a Power Down
 *
 * This function initializes the XCVR module according to the radio_mode and data_rate settings.
 * This is very similar to XCVR_Init but to be called on wake up from Power Down.
 *
 * @note: XCVR_Init must have been called beforehand. current_radio_mode and current_data_rate
 * are taken for reconfiguration
 */
xcvrStatus_t XCVR_WakeUpInit(void);


/*!
 * @brief  Configure XCVR radio banks for 1Mbps and possibly 2Mbps rate. It switched to BLE mode if coming from ZB.
 *
 * This function initializes the XCVR BLE module data_rate settings. This the only function call required to
 * set up the radio banks to get ready for 1Mbps and 2Mbps operation.
 * @param data_rate The data rate for which the XCVR should be configured. this does not set the actual rate that will be used,
 * but sets the radio banks. The selection is performed dynamically per connection.
 */
void XCVR_RadioBleConfigure(data_rate_t data_rate);

/*!
 * @brief  Configure XCVR radio banks for ZB regular operation. It switches to ZB if coming from BLE.
 *
 * This function initializes the XCVR ZB regular mode.
 */
void XCVR_RadioZigbeeConfigure(void);

/*!
 * @brief  Deinitialize XCVR radio.
 *
 * Useful to return control of MODEM clock to the PMC for instance before Powering Down (until next wake-up)
 *
 */
void XCVR_Deinit(void);

/*!
 * @brief  Update Temperature in XCVR radio.
 *
 * Update Temperature in XCVR for calibration purpose
 *
 */
void XCVR_TemperatureUpdate(int32_t temperature);

/*!
 * @brief  Recalibrate XCVR radio.
 *
 * Call for XCVR radio to recalibrate
 *
 */
void XCVR_Recalibrate(void);

 /*!
  * @brief  Get Next Radio calibration duration.
  *
  * Call for XCVR radio to provide next calibration duration in microseconds if any
  *
  */
uint32_t XCVR_GetRecalDuration(void);

 /*!
   * @brief  Check and abort Radio lockup.
   *
   * Call to check and unlock XCVR radio if locked up
   *
   */
void XCVR_LockupCheckAndAbortRadio(void);

/*!
   * @brief  Enable Radio Fast TX
   *
   * Call to enable XCVR fast TX
   *
   */
void XCVR_EnableBLEFastTX(bool_t keepPll);

/*!
   * @brief  Disable Radio Fast TX
   *
   * Call to disable XCVR fast TX
   *
   */
void XCVR_DisableBLEFastTX(void);

/*!
   * @brief  Read RSSI
   *
   * Call to read RSSI from RFP
   *
   */
int16_t XCVR_ReadRSSI(data_rate_t rate);

/*!
   * @brief  Set LL Exchange Memory Address
   *
   * Call to set LL Exchange memory address in DP_TOP register
   *
   */
void XCVR_SetBLEdpTopEmAddr(uint32_t em_addr);

/*!
   * @brief  Set Tx Power value
   *
   * Call to set Tx Power value in dBm : takes place on radio re-initialization
   *
   */
int8_t XCVR_SetTxPwr(int8_t tx_pwr_dbm);

/*!
   * @brief  Set Tx Power operation range
   *
   *
   * Call to set Tx Power operation range : high Tx power range allows operation between [-30dBm .. +15dBm]
   * whereas low Tx power range in in the [-30dBm..+10dBm] range,
   * Hight Tx power is only possible on chips having this capability, other remain in low range.
   *
   */
bool_t XCVR_SetTxOperationRange(bool_t high);

#if defined (gMWS_UseCoexistence_d) && (gMWS_UseCoexistence_d)
/*!
   * @brief  Register callback to be notified of next RF activity
   *
   * Call to be notified when next RF Activity started to drive rfStatusPrio coexistence signal
   *
   */
void XCVR_RegisterRfActivityCallback(void *pf_callback);
#endif

/* @} */



#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_XCVR_H_ */

