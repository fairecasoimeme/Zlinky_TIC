/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/





#include "app.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef DMA_BUFFER_LENGTH
#define DMA_BUFFER_LENGTH 8
#endif

typedef struct {
  uint16_t u16Read;                    // index of oldest data byte in buffer
  uint16_t u16Write;                     // index of newest data byte in buffer
}tsDmaSwFifo;

/*******************************************************************************
 * Additional USART DMA Functions
 ******************************************************************************/
void USART_DMA_Init();
void USART_DMA_Flush();
uint16_t USART_DMA_GetCount(void);
uint16_t USART_DMA_ReadBytes(uint8_t *buffer,uint16_t u16Max);
