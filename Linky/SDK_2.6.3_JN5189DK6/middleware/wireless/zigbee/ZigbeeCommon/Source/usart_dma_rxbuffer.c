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


/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
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
 * o Neither the name of the copyright holder nor the names of its
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

#include "board.h"
#include "app.h"
#include "fsl_usart.h"
#include "fsl_usart_dma.h"
#include "fsl_dma.h"
#include "usart_dma_rxbuffer.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void USART_DMA_CreateHandle(dma_handle_t *handle, DMA_Type *base, uint32_t channel);

/*******************************************************************************
 * Variables
 ******************************************************************************/
dma_handle_t g_uartRxDmaHandle;
uint8_t g_rxBuffer[DMA_BUFFER_LENGTH] = {0};
tsDmaSwFifo sSwFifo = {0 , 0};
__attribute__((aligned(16))) dma_descriptor_t g_rxdma_desc = {0};

/*******************************************************************************
 * Additional USART DMA Functions
 ******************************************************************************/
void USART_DMA_Init()
{
	dma_transfer_config_t xferConfig;
	usart_config_t config;;
	usart_transfer_t receiveXfer;

   /* Initialize the UART. */
   /*
    * config.baudRate_Bps = 115200U;
    * config.parityMode = kUART_ParityDisabled;
    * config.stopBitCount = kUART_OneStopBit;
    * config.txFifoWatermark = 0;
    * config.rxFifoWatermark = 1;
    * config.enableTx = false;
    * config.enableRx = false;
    */
    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = UART_BAUD_RATE;
    config.enableTx = true;
    config.enableRx = true;

    USART_Init(UART, &config, UART_CLK_FREQ);

    /* Configure DMA. */
    DMA_Init(DMA0);
    DMA_EnableChannel(DMA0, USART_RX_DMA_CHANNEL);
    USART_DMA_CreateHandle(&g_uartRxDmaHandle, DMA0, USART_RX_DMA_CHANNEL);

	/* Enable DMA request from rxFIFO */
	USART_EnableRxDMA(UART, true);

	/* Prepare transfer. */
    receiveXfer.data = g_rxBuffer;
    receiveXfer.dataSize = sizeof(g_rxBuffer);
	DMA_PrepareTransfer(&xferConfig, (void *)&UART->FIFORD,receiveXfer.data, sizeof(uint8_t), receiveXfer.dataSize, kDMA_PeripheralToMemory, &g_rxdma_desc);

	/* Create the Descriptor */
    DMA_CreateDescriptor(&g_rxdma_desc, &xferConfig.xfercfg, (void *)&UART->FIFORD, receiveXfer.data, &g_rxdma_desc);

	/* Submit transfer. */
	DMA_SubmitTransfer(&g_uartRxDmaHandle, &xferConfig);
	DMA_StartTransfer(&g_uartRxDmaHandle);
}


void USART_DMA_Flush()
{
	// Move the read pointer to be in line with the write pointer.
	sSwFifo.u16Read = DMA_BUFFER_LENGTH - DMA_GetRemainingBytes(DMA0, USART_RX_DMA_CHANNEL);
}

uint16_t USART_DMA_GetCount(void)
{
	uint32_t localCount;
	sSwFifo.u16Write = DMA_BUFFER_LENGTH - DMA_GetRemainingBytes(DMA0, USART_RX_DMA_CHANNEL);

	if(sSwFifo.u16Read > sSwFifo.u16Write ){
		localCount = (sSwFifo.u16Write + DMA_BUFFER_LENGTH - sSwFifo.u16Read);
	}
	else{
		localCount = (sSwFifo.u16Write - sSwFifo.u16Read);
	}
	return localCount;
}

uint16_t USART_DMA_ReadBytes(uint8_t *buffer,uint16_t u16Max)
{

	uint32_t buffercount = USART_DMA_GetCount();

	//PRINTF("DMA COUNT:%d\r\n",buffercount);
	//PRINTF("writeptr:%d readptr:%d \r\n",sSwFifo.u16Write,sSwFifo.u16Read);

	// If you are requesting more than is received just return the received amount
	if (buffercount<u16Max) u16Max=buffercount;

	// Check if there is no data
	if (buffercount==0) return 0;

	if(sSwFifo.u16Read > sSwFifo.u16Write){
		uint32_t maxcount = DMA_BUFFER_LENGTH- sSwFifo.u16Read;

		if( (maxcount ) > u16Max) maxcount = u16Max;
		// Copy the end of the dma buffer
		memcpy(buffer,&g_rxBuffer[sSwFifo.u16Read],maxcount);

		// do we have additional data at the start of the dma buffer?
		if (sSwFifo.u16Write != 0){
			if(u16Max>maxcount) {
				maxcount = u16Max-maxcount;
				if( (maxcount ) > sSwFifo.u16Write)
				{
					maxcount = sSwFifo.u16Write;
				}

				memcpy(&buffer[DMA_BUFFER_LENGTH-sSwFifo.u16Read],&g_rxBuffer[0],maxcount);
			}
		}
	}
	else{
		memcpy(buffer,&g_rxBuffer[sSwFifo.u16Read],u16Max);
	}

	// Update the readptr
	sSwFifo.u16Read += u16Max;
	if(sSwFifo.u16Read >=DMA_BUFFER_LENGTH) sSwFifo.u16Read -= DMA_BUFFER_LENGTH;

	//Return number of bytes read
	return (uint16_t) u16Max;
}


/*******************************************************************************
 * Local Functions
 ******************************************************************************/
void USART_DMA_CreateHandle(dma_handle_t *handle, DMA_Type *base, uint32_t channel)
{
    assert((NULL != handle) && (channel < FSL_FEATURE_DMA_NUMBER_OF_CHANNELS));
    memset(handle, 0, sizeof(*handle));
    handle->base = base;
    handle->channel = channel;
}

/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
