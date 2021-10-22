/*****************************************************************************
 *
 * MODULE:             ARM CM3 register definitions
 *
 * DESCRIPTION:        Register and bit field definitions used in the JN517x.
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139].
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
 * Copyright NXP B.V. 2014. All rights reserved
 *
 ***************************************************************************/

#ifndef _armcortexm3registers_JN51xx_h_
#define _armcortexm3registers_JN51xx_h_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
//#include <PeripheralRegs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Bitfield definitions */
#ifndef BIT_W_1
#define BIT_W_1                                     0x00000001UL
#define BIT_W_2                                     0x00000003UL
#define BIT_W_3                                     0x00000007UL
#define BIT_W_4                                     0x0000000FUL
#define BIT_W_5                                     0x0000001FUL
#define BIT_W_6                                     0x0000003FUL
#define BIT_W_7                                     0x0000007FUL
#define BIT_W_8                                     0x000000FFUL
#define BIT_W_10                                    0x00000BFFUL
#define BIT_W_12                                    0x00000FFFUL
#define BIT_W_15                                    0x00007FFFUL
#define BIT_W_16                                    0x0000FFFFUL
#define BIT_W_17                                    0x0001FFFFUL
#define BIT_W_18                                    0x0003FFFFUL
#define BIT_W_19                                    0x0007FFFFUL
#define BIT_W_20                                    0x000FFFFFUL
#define BIT_W_21                                    0x001FFFFFUL
#define BIT_W_25                                    0x01FFFFFFUL
#endif

// macro used to program interrupt priorities in the NVIC
#define vREG_Write8(u8Address, u8Data)                          *((volatile uint8 *)(u8Address)) = (u8Data)
#define u8REG_Read(u8Address)                                   *((volatile uint8 *)(u8Address))

// NVIC

#define REG_INT_ICTR                                            0xE000E004

#define REG_AUXILIARY_CONTROL                                   0xE000E008

#define REG_AUXILIARY_CONTROL_DISMCYCINT_BIT                    0
#define REG_AUXILIARY_CONTROL_DISMCYCINT_MASK                   ((uint32_t)(BIT_W_1 << REG_AUXILIARY_CONTROL_DISMCYCINT_BIT))
#define REG_AUXILIARY_CONTROL_DISDEFWBUF_BIT                    1
#define REG_AUXILIARY_CONTROL_DISDEFWBUF_MASK                   ((uint32_t)(BIT_W_1 << REG_AUXILIARY_CONTROL_DISDEFWBUF_BIT))
#define REG_AUXILIARY_CONTROL_DISFOLD_BIT                       2
#define REG_AUXILIARY_CONTROL_DISFOLD_MASK                      ((uint32_t)(BIT_W_1 << REG_AUXILIARY_CONTROL_DISFOLD_BIT))
#define REG_AUXILIARY_CONTROL_DISOOFP_BIT                       8
#define REG_AUXILIARY_CONTROL_DISOOFP_MASK                      ((uint32_t)(BIT_W_1 << REG_AUXILIARY_CONTROL_DISOOFP_BIT))
#define REG_AUXILIARY_CONTROL_DISFPCA_BIT                       9
#define REG_AUXILIARY_CONTROL_DISFPCA_MASK                      ((uint32_t)(BIT_W_1 << REG_AUXILIARY_CONTROL_DISFPCA_BIT))

#define REG_INT_SETENA0                                         0xE000E100
#define REG_INT_CLRENA0                                         0xE000E180

#define REG_INT_SETPEND0                                        0xE000E200
#define REG_INT_CLRPEND0                                        0xE000E280

#define REG_INT_ACTIVE0                                         0xE000E300

#define REG_INT_PRIORITY_BASE                                   0xE000E400

#define REG_INT_CTRL_AND_STATE                                  0xE000ED04

#define REG_INT_CTRL_AND_STATE_NMIPENDSET_BIT                   31
#define REG_INT_CTRL_AND_STATE_NMIPENDSET_MASK                  ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_NMIPENDSET_BIT))
#define REG_INT_CTRL_AND_STATE_PENDSVSET_BIT                    28
#define REG_INT_CTRL_AND_STATE_PENDSVSET_MASK                   ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_PENDSVSET_BIT))
#define REG_INT_CTRL_AND_STATE_PENDSVCLR_BIT                    27
#define REG_INT_CTRL_AND_STATE_PENDSVCLR_MASK                   ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_PENDSVCLR_BIT))
#define REG_INT_CTRL_AND_STATE_PENDSTSET_BIT                    26
#define REG_INT_CTRL_AND_STATE_PENDSTSET_MASK                   ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_NMIPENDSTSET_BIT))
#define REG_INT_CTRL_AND_STATE_PENDSTCLR_BIT                    25
#define REG_INT_CTRL_AND_STATE_PENDSTCLR_MASK                   ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_PENDSTCLR_BIT))
#define REG_INT_CTRL_AND_STATE_ISRPENDING_BIT                   22
#define REG_INT_CTRL_AND_STATE_ISRPENDING_MASK                  ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_ISRPENDING_BIT))
#define REG_INT_CTRL_AND_STATE_VECTPENDING_BIT                  12
#define REG_INT_CTRL_AND_STATE_VECTPENDING_MASK                 ((uint32_t)(BIT_W_10 << REG_INT_CTRL_AND_STATE_VECTPENDING_BIT))
#define REG_INT_CTRL_AND_STATE_RETTOBASE_BIT                    11
#define REG_INT_CTRL_AND_STATE_RETTOBASE_MASK                   ((uint32_t)(BIT_W_1 << REG_INT_CTRL_AND_STATE_RETTOBASE_BIT))
#define REG_INT_CTRL_AND_STATE_VECTACTIVE_BIT                   0
#define REG_INT_CTRL_AND_STATE_VECTACTIVE_MASK                  ((uint32_t)(BIT_W_8 << REG_INT_CTRL_AND_STATE_VECTACTIVE_BIT))

#define REG_APP_INT_RESET_CTRL                                  0xE000ED0C

#define REG_APP_INT_RESET_CTRL_VECTKEY_BIT                      16
#define REG_APP_INT_RESET_CTRL_VECTKEY                          ((uint32_t)(0x05FA << REG_APP_INT_RESET_CTRL_VECTKEY_BIT))
#define REG_APP_INT_RESET_CTRL_PRIGROUP_BIT                     8
#define REG_APP_INT_RESET_CTRL_PRIGROUP_MASK                    ((uint32_t)(BIT_W_3 << REG_APP_INT_RESET_CTRL_PRIGROUP_BIT))

#define REG_INT_VECTOR_TABLE_OFFSET                             0xE000ED08

#define REG_INT_VECTOR_TABLE_OFFSET_TBLBASE_BIT                 29
#define REG_INT_VECTOR_TABLE_OFFSET_TBLBASE_MASK                ((uint32_t)(BIT_W_1 << REG_INT_VECTOR_TABLE_OFFSET_TBLBASE_BIT))
#define REG_INT_VECTOR_TABLE_OFFSET_TBLOFF_BIT                  7
#define REG_INT_VECTOR_TABLE_OFFSET_TBLOFF_MASK                 ((uint32_t)(BIT_W_21 << REG_INT_VECTOR_TABLE_OFFSET_TBLOFF_BIT))

#define REG_SYSTEM_CONTROL                                      0xE000ED10
#define REG_SYSTEM_CONTROL_SLEEPONEXIT_BIT                      1
#define REG_SYSTEM_CONTROL_SLEEPONEXIT_MASK                     ((uint32_t)(BIT_W_1 << REG_SYSTEM_CONTROL_SLEEPONEXIT_BIT))
#define REG_SYSTEM_CONTROL_SLEEPDEEP_BIT                        2
#define REG_SYSTEM_CONTROL_SLEEPDEEP_MASK                       ((uint32_t)(BIT_W_1 << REG_SYSTEM_CONTROL_SLEEPDEEP_BIT))
#define REG_SYSTEM_CONTROL_SEVONPEND_BIT                        4
#define REG_SYSTEM_CONTROL_SEVONPEND_MASK                       ((uint32_t)(BIT_W_1 << REG_SYSTEM_CONTROL_SEVONPEND_BIT))

#define REG_CONFIGURATION_CONTROL                               0xE000ED14
#define REG_CONFIGURATION_CONTROL_UNALIGN_TRP_BIT               3
#define REG_CONFIGURATION_CONTROL_UNALIGN_TRP_MASK              ((uint32_t)(BIT_W_1 << REG_CONFIGURATION_CONTROL_UNALIGN_TRP_BIT))
#define REG_CONFIGURATION_CONTROL_DIV_0_TRP_BIT                 4
#define REG_CONFIGURATION_CONTROL_DIV_0_TRP_MASK                ((uint32_t)(BIT_W_1 << REG_CONFIGURATION_CONTROL_DIV_0_TRP_BIT))

// memory management fault
#define REG_SYSTEM_HANDLER_PRIORITY_4                           0xE000ED18
// bus fault
#define REG_SYSTEM_HANDLER_PRIORITY_5                           0xE000ED19
// usage fault
#define REG_SYSTEM_HANDLER_PRIORITY_6                           0xE000ED1A
// svc
#define REG_SYSTEM_HANDLER_PRIORITY_11                          0xE000ED1F
// debug monitor
#define REG_SYSTEM_HANDLER_PRIORITY_12                          0xE000ED20
// pend SV
#define REG_SYSTEM_HANDLER_PRIORITY_14                          0xE000ED22
// SYSTICK
#define REG_SYSTEM_HANDLER_PRIORITY_15                          0xE000ED23

#define REG_SYSTEM_HANDLER_CNTRL_STATE                          0xE000ED24

#define REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTACT_BIT          0
#define REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTACT_MASK         ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTACT_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTACT_BIT          1
#define REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTACT_MASK         ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTACT_BIT))
// bit 2 reserved
#define REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTACT_BIT          3
#define REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTACT_MASK         ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTACT_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_SVCALLACT_BIT            7
#define REG_SYSTEM_HANDLER_CNTRL_STATE_SVCALLACT_MASK           ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_SVCALLACT_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_MONITORACT_BIT           8
#define REG_SYSTEM_HANDLER_CNTRL_STATE_MONITORACT_MASK          ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_MONITORACT_BIT))
// bit 9 reserved
#define REG_SYSTEM_HANDLER_CNTRL_STATE_PENDSVACT_BIT            10
#define REG_SYSTEM_HANDLER_CNTRL_STATE_PENDSVACT_MASK           ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_PENDSVACT_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_SYSTICKACT_BIT           11
#define REG_SYSTEM_HANDLER_CNTRL_STATE_SYSTICKACT_MASK          ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_SYSTICKACT_BIT))

#define REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTPENDED_BIT       12
#define REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTPENDED_MASK      ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTPENDED_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTPENDED_BIT       13
#define REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTPENDED_MASK      ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTPENDED_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTPENDED_BIT       14
#define REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTPENDED_MASK      ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTPENDED_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_SVCALLPENDED_BIT         15
#define REG_SYSTEM_HANDLER_CNTRL_STATE_SVCALLPENDED_MASK        ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_SVCALLPENDED_BIT))

#define REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTEN_BIT           16
#define REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTEN_MASK          ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_MEMFAULTEN_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTEN_BIT           17
#define REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTEN_MASK          ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_BUSFAULTEN_BIT))
#define REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTEN_BIT           18
#define REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTEN_MASK          ((uint32_t)(BIT_W_1 << REG_SYSTEM_HANDLER_CNTRL_STATE_USGFAULTEN_BIT))

#define REG_CONFIGURABLE_FAULT_STATUS                           0xE000ED28

#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_IACCVIOL_BIT        0
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_IACCVIOL_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_MMFSR_IACCVIOL_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_DACCVIOL_BIT        1
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_DACCVIOL_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_MMFSR_DACCVIOL_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MUNSTKERR_BIT       3
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MUNSTKERR_MASK      ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MUNSTKERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MSTKERR_BIT         4
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MSTKERR_MASK        ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MSTKERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MMARVALID_BIT       7
#define REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MMARVALID_MASK      ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_MMFSR_MMARVALID_BIT))

#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_IBUSERR_BIT          8
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_IBUSERR_MASK         ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_BFSR_IBUSERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_PRECISERR_BIT        9
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_PRECISERR_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_BFSR_PRECISERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_IMPRECISERR_BIT      10
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_IMPRECISERR_MASK    ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_BFSR_IMPRECISERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_UNSTKERR_BIT         11
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_UNSTKERR_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_BFSR_UNSTKERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_STKERR_BIT           12
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_STKERR_MASK          ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_BFSR_STKERR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_BFARVALID_BIT        15
#define REG_CONFIGURABLE_FAULT_STATUS_BFSR_BFARVALID_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_BFSR_BFARVALID_BIT))

#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNDEFINSTR_BIT       16
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNDEFINSTR_MASK      ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNDEFINSTR_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVSTATE_BIT         17
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVSTATE_MASK        ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVSTATE_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVPC_BIT            18
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVPC_MASK           ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_UFSR_INVPC_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_NOCP_BIT             19
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_NOCP_MASK            ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_UFSR_NOCP_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNALIGNED_BIT        24
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNALIGNED_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_UFSR_UNALIGNED_BIT))
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_DIVBYZERO_BIT        25
#define REG_CONFIGURABLE_FAULT_STATUS_UFSR_DIVBYZERO_MASK       ((uint32_t)(BIT_W_1 << REG_CONFIGURABLE_FAULT_STATUS_UFSR_DIVBYZERO_BIT))

#define REG_HARDFAULT_STATUS                                    0xE000ED2C

#define REG_HARDFAULT_STATUS_VECTTBL_BIT                        1
#define REG_HARDFAULT_STATUS_VECTTBL_MASK                       ((uint32_t)(BIT_W_1 << REG_HARDFAULT_STATUS_VECTTBL_BIT))
#define REG_HARDFAULT_STATUS_FORCED_BIT                         30
#define REG_HARDFAULT_STATUS_FORCED_MASK                        ((uint32_t)(BIT_W_1 << REG_HARDFAULT_STATUS_FORCED_BIT))
#define REG_HARDFAULT_STATUS_DEBUGEVT_BIT                       31
#define REG_HARDFAULT_STATUS_DEBUGEVT_MASK                      ((uint32_t)(BIT_W_1 << REG_HARDFAULT_STATUS_DEBUGEVT_BIT))

#define REG_DEBUG_FAULT_STATUS                                  0xE000ED30
#define REG_MEMMANAGE_FAULT_ADDRESS                             0xE000ED34
#define REG_BUSFAULT_ADDRESS                                    0xE000ED38
#define REG_AUXILIARY_FAULT_STATUS                              0xE000ED3C
#define REG_DEBUG_HALTING_STATUS_AND_CONTROL                    0xE000EDF0

// TICK Timer
#define REG_SYSTICK_CALIBRATION                                 0xE000E01C
#define REG_SYSTICK_CURRENT_VALUE                               0xE000E018
#define REG_SYSTICK_RELOAD_VALUE                                0xE000E014
#define REG_SYSTICK_CONTROL_STATUS                              0xE000E010

#define REG_SYSTICK_VALUE_MASK                                  0x00FFFFFF

#define REG_SYSTICK_CONTROL_EN_BIT                              0
#define REG_SYSTICK_CONTROL_EN_MASK                             ((uint32_t)(BIT_W_1 << REG_SYSTICK_CONTROL_EN_BIT))
#define REG_SYSTICK_CONTROL_TICKINT_BIT                         1
#define REG_SYSTICK_CONTROL_TICKINT_MASK                        ((uint32_t)(BIT_W_1 << REG_SYSTICK_CONTROL_TICKINT_BIT))
#define REG_SYSTICK_CONTROL_CLKSOURCE_BIT                       2
#define REG_SYSTICK_CONTROL_CLKSOURCE_MASK                      ((uint32_t)(BIT_W_1 << REG_SYSTICK_CONTROL_CLKSOURCE_BIT))
#define REG_SYSTICK_CONTROL_COUNTFLAG_BIT                       16
#define REG_SYSTICK_CONTROL_COUNTFLAG_MASK                      ((uint32_t)(BIT_W_1 << REG_SYSTICK_CONTROL_COUNTFLAG_BIT))

/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/

#ifdef __cplusplus
};
#endif

#endif /* _armcortexm3registers_JN51xx_h_ */
