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

#ifndef _debugexceptionhandlers_JN51xx_h_
#define _debugexceptionhandlers_JN51xx_h_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
// these are place holders
#define WRITE_REG32_PLACEHOLDER(A, B) *(volatile uint32_t *)(A) = (B)
#define READ_REG32_PLACEHOLDER(A)     *(volatile uint32_t *)(A)

#define u32REG_Read(addr)               (READ_REG32_PLACEHOLDER(addr))
#define u32REG_ReadNC(addr)             (READ_REG32_PLACEHOLDER(addr))

#define vREG_Write(addr, u32Data)       (WRITE_REG32_PLACEHOLDER(addr, u32Data))
#define vREG_WriteNC(addr, u32Data)     (WRITE_REG32_PLACEHOLDER(addr, u32Data))

/****************************************************************************/
/**** 		TYPE DEFINITIONS 											 ****/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void vDebugExceptionHandlersInitialise(void);
void vDebugExceptionConvertImpreciseToPreciseError(void);
void vPrintNVICconfiguration(void);

void HardFault_Handler( void )__attribute__((naked));
void BusFault_Handler( void )__attribute__((naked));
void UsageFault_Handler( void )__attribute__((naked));
void MemManage_Handler(void)__attribute__((naked));

/****************************************************************************/
/***        Callback Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Helper Functions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#ifdef __cplusplus
};
#endif

#endif /* _debugexceptionhandlers_JN51xx_h_ */
