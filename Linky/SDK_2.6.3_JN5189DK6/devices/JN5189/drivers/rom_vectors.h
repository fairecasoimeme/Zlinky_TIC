/****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
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
 * Copyright NXP B.V. 2016. All rights reserved
 ****************************************************************************/

#ifndef ROM_VECTORS_H_
#define ROM_VECTORS_H_

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "rom_common.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/* Expose ROM vectors so they can be re-used by application */
ROM_API WEAK void NMI_Handler(void);
ROM_API WEAK void HardFault_Handler(void);
ROM_API WEAK void MemManage_Handler(void);
ROM_API WEAK void BusFault_Handler(void);
ROM_API WEAK void UsageFault_Handler(void);
ROM_API WEAK void SVC_Handler(void);
ROM_API WEAK void DebugMon_Handler(void);
ROM_API WEAK void PendSV_Handler(void);
ROM_API WEAK void SysTick_Handler(void);
ROM_API WEAK void IntDefaultHandler(void);

ROM_API WEAK void System_IRQHandler(void);
ROM_API WEAK void DMA_IRQHandler(void);
ROM_API WEAK void GINT_IRQHandler(void);
ROM_API WEAK void IRBlaster_IRQHandler(void);
ROM_API WEAK void PINT0_IRQHandler(void);
ROM_API WEAK void PINT1_IRQHandler(void);
ROM_API WEAK void PINT2_IRQHandler(void);
ROM_API WEAK void PINT3_IRQHandler(void);
ROM_API WEAK void SPIFI_IRQHandler(void);
ROM_API WEAK void Timer0_IRQHandler(void);
ROM_API WEAK void Timer1_IRQHandler(void);
ROM_API WEAK void USART0_IRQHandler(void);
ROM_API WEAK void USART1_IRQHandler(void);
ROM_API WEAK void I2C0_IRQHandler(void);
ROM_API WEAK void I2C1_IRQHandler(void);
ROM_API WEAK void SPI0_IRQHandler(void);
ROM_API WEAK void SPI1_IRQHandler(void);
ROM_API WEAK void PWM0_IRQHandler(void);
ROM_API WEAK void PWM1_IRQHandler(void);
ROM_API WEAK void PWM2_IRQHandler(void);
ROM_API WEAK void PWM3_IRQHandler(void);
ROM_API WEAK void PWM4_IRQHandler(void);
ROM_API WEAK void PWM5_IRQHandler(void);
ROM_API WEAK void PWM6_IRQHandler(void);
ROM_API WEAK void PWM7_IRQHandler(void);
ROM_API WEAK void PWM8_IRQHandler(void);
ROM_API WEAK void PWM9_IRQHandler(void);
ROM_API WEAK void PWM10_IRQHandler(void);
ROM_API WEAK void I2C2_IRQHandler(void);
ROM_API WEAK void RTC_IRQHandler(void);
ROM_API WEAK void NFCTag_IRQHandler(void);
ROM_API WEAK void MAILBOX_IRQHandler(void);
ROM_API WEAK void ADC_SEQA_IRQHandler(void);
ROM_API WEAK void ADC_SEQB_IRQHandler(void);
ROM_API WEAK void ADC_THCMP_OVR_IRQHandler(void);
ROM_API WEAK void DMIC_IRQHandler(void);
ROM_API WEAK void HWVAD_IRQHandler(void);
ROM_API WEAK void BLE_DP_IRQHandler(void);
ROM_API WEAK void BLE_DP0_IRQHandler(void);
ROM_API WEAK void BLE_DP1_IRQHandler(void);
ROM_API WEAK void BLE_DP2_IRQHandler(void);
ROM_API WEAK void BLE_LL_ALL_IRQHandler(void);
ROM_API WEAK void ZIGBEE_MAC_IRQHandler(void);
ROM_API WEAK void ZIGBEE_MODEM_IRQHandler(void);
ROM_API WEAK void RFP_TMU_IRQHandler(void);
ROM_API WEAK void RFP_AGC_IRQHandler(void);
ROM_API WEAK void ISO7816_IRQHandler(void);
ROM_API WEAK void ANA_COMP_IRQHandler(void);
ROM_API WEAK void WAKE_UP_TIMER0_IRQHandler(void);
ROM_API WEAK void WAKE_UP_TIMER1_IRQHandler(void);
ROM_API WEAK void PVT_AMBER0_IRQHandler(void);
ROM_API WEAK void PVT_RED0_IRQHandler(void);
ROM_API WEAK void PVT_AMBER1_IRQHandler(void);
ROM_API WEAK void PVT_RED1_IRQHandler(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif /* ROM_VECTORS_H_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
