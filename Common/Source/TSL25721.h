/*****************************************************************************
 *
 * MODULE:          Light Sensor Driver
 *
 * COMPONENT:       TSL25721.h
 *
 * DESCRIPTION:     ZLO Demo: Light sensor driver
 *
 *****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2013-2019. All rights reserved
 *
 ****************************************************************************/
#ifndef TSL25721_H_
#define TSL25721_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define TSL25721_CHANNEL_0   0
#define TSL25721_CHANNEL_1   1

#define TSL25721_ADDRESS             0x39

#define TSL25721_COMMAND             0x80

#define TSL25721_CMD_SPL_FN          0x60
#define TSL25721_CMD_REPEAT          0x00
#define TSL257212_CMD_AUTOINC        0x20

#define TSL25721_ENABLE              0x00
#define TSL25721_ATIME               0x01
#define TSL25721_WTIME               0x03
#define TSL25721_AILTL               0x04
#define TSL25721_AILTH               0x05
#define TSL25721_AIHTL               0x06
#define TSL25721_AIHTH               0x07
#define TSL25721_PERS                0x0C
#define TSL25721_CONFIG              0x0D
#define TSL25721_CONTROL             0x0F
#define TSL25721_ID                  0x12
#define TSL25721_STATUS              0x13
#define TSL25721_READ_ADC_0_LO       0x14
#define TSL25721_READ_ADC_0_HI       0x15
#define TSL25721_READ_ADC_1_LO       0x16
#define TSL25721_READ_ADC_1_HI       0x17

#define TSL25721_TIMEOUT             300000

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC bool_t bTSL25721_Init(void);
PUBLIC bool_t bTSL25721_PowerDown(void);
PUBLIC bool_t bTSL25721_StartRead(void);
PUBLIC bool_t bTSL25721_SetATime(uint8 u8Value);
PUBLIC uint16 u16TSL25721_ReadResult(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#endif /* TSL25721_H_ */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
