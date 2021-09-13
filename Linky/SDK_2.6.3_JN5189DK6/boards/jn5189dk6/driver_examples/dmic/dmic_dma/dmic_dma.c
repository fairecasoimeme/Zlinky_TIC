/*
 * Copyright 2016, NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_dmic.h"
#include "fsl_dma.h"
#include "fsl_dmic_dma.h"
#include <stdlib.h>
#include <string.h>

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define APP_DMIC_CHANNEL kDMIC_Channel0
#define APP_DMIC_CHANNEL_ENABLE DMIC_CHANEN_EN_CH0(1)
#define APP_DMAREQ_CHANNEL DMAREQ_DMIC0
#define DMAREQ_DMIC0 15U
#define VOLUME_THRESHOLD 100U /* Higher valuer for higher volume threshold */
#define THRESHOLD_LED_PORT 0
#define THRESHOLD_LED_PIN 0
//#define CLOCK_DIVIDER kDMIC_PdmDiv48
#define CLOCK_DIVIDER kDMIC_PdmDiv16
#define OVER_SAMPLE_RATE 21U

#define FIFO_DEPTH 15U
#define BUFFER_LENGTH 32U
#if defined(FSL_FEATURE_DMIC_CHANNEL_HAS_SIGNEXTEND) && (FSL_FEATURE_DMIC_CHANNEL_HAS_SIGNEXTEND)
#define DEMO_DMIC_DATA_WIDTH uint32_t
#else
#define DEMO_DMIC_DATA_WIDTH uint16_t
#endif
/*******************************************************************************

 * Prototypes
 ******************************************************************************/
/* DMIC user callback */
void DMIC_UserCallback(DMIC_Type *base, dmic_dma_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
DEMO_DMIC_DATA_WIDTH g_rxBuffer[BUFFER_LENGTH] = {0};

dmic_dma_handle_t g_dmicDmaHandle;
dma_handle_t g_dmicRxDmaHandle;
volatile bool g_Transfer_Done = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* DMIC user callback */
void DMIC_UserCallback(DMIC_Type *base, dmic_dma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;
    if (status == kStatus_DMIC_Idle)
    {
        g_Transfer_Done = true;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    dmic_channel_config_t dmic_channel_cfg;

    dmic_transfer_t receiveXfer;

    uint32_t i;
    /* Board pin, clock, debug console init */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    CLOCK_AttachClk(kMAIN_CLK_to_DMI_CLK);
    SYSCON->DMICCLKDIV = 6;

    dmic_channel_cfg.divhfclk            = kDMIC_PdmDiv1;
    dmic_channel_cfg.osr                 = 25U;
    dmic_channel_cfg.gainshft            = 2U;
    dmic_channel_cfg.preac2coef          = kDMIC_CompValueZero;
    dmic_channel_cfg.preac4coef          = kDMIC_CompValueZero;
    dmic_channel_cfg.dc_cut_level        = kDMIC_DcCut155;
    dmic_channel_cfg.post_dc_gain_reduce = 1;
    dmic_channel_cfg.saturate16bit       = 1U;
    dmic_channel_cfg.sample_rate         = kDMIC_PhyFullSpeed;
#if defined(FSL_FEATURE_DMIC_CHANNEL_HAS_SIGNEXTEND) && (FSL_FEATURE_DMIC_CHANNEL_HAS_SIGNEXTEND)
    dmic_channel_cfg.enableSignExtend = true;
#endif

    DMIC_Init(DMIC0);
#if !(defined(FSL_FEATURE_DMIC_HAS_NO_IOCFG) && FSL_FEATURE_DMIC_HAS_NO_IOCFG)
    DMIC_SetIOCFG(DMIC0, kDMIC_PdmDual);
#endif

    DMIC_Use2fs(DMIC0, true);
    DMIC_EnableChannelDma(DMIC0, APP_DMIC_CHANNEL, true);
    DMIC_ConfigChannel(DMIC0, APP_DMIC_CHANNEL, kDMIC_Left, &dmic_channel_cfg);

    DMIC_FifoChannel(DMIC0, APP_DMIC_CHANNEL, FIFO_DEPTH, true, true);

    DMIC_EnableChannnel(DMIC0, APP_DMIC_CHANNEL_ENABLE);
    PRINTF("Configure DMA\r\n");

    DMA_Init(DMA0);

    DMA_EnableChannel(DMA0, APP_DMAREQ_CHANNEL);

    /* Request dma channels from DMA manager. */
    DMA_CreateHandle(&g_dmicRxDmaHandle, DMA0, APP_DMAREQ_CHANNEL);

    /* Create DMIC DMA handle. */
    DMIC_TransferCreateHandleDMA(DMIC0, &g_dmicDmaHandle, DMIC_UserCallback, NULL, &g_dmicRxDmaHandle);
    receiveXfer.dataSize               = 2 * BUFFER_LENGTH;
    receiveXfer.data                   = g_rxBuffer;
    receiveXfer.dataAddrInterleaveSize = kDMA_AddressInterleave1xWidth;
    receiveXfer.dataWidth              = sizeof(DEMO_DMIC_DATA_WIDTH);
    receiveXfer.linkTransfer           = NULL;
    PRINTF("Buffer Data before transfer \r\n");
    for (i = 0; i < BUFFER_LENGTH; i++)
    {
        PRINTF("%d\r\n", g_rxBuffer[i]);
    }
    DMIC_TransferReceiveDMA(DMIC0, &g_dmicDmaHandle, &receiveXfer, APP_DMIC_CHANNEL);

    /* Wait for DMA transfer finish */
    while (g_Transfer_Done == false)
    {
    }

    PRINTF("Transfer completed\r\n");
    PRINTF("Buffer Data after transfer \r\n");
    for (i = 0; i < BUFFER_LENGTH; i++)
    {
        PRINTF("%d\r\n", g_rxBuffer[i]);
    }
    while (1)
    {
    }
}
