/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_usart.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include "fsl_clock.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_DEBUG_CONSOLE_USART 1U
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M
#define DEMO_USART_CLK_FREQ CLOCK_GetFreq(DEMO_USART_CLK_SRC)
#define DELAY_TIME 100000U
#define TRANSFER_SIZE     256U    /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 115200U /*! Transfer baudrate - 115200 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* USART user callback */
void USART_UserCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t transferRxData[TRANSFER_SIZE] = {0U};
uint8_t transferTxData[TRANSFER_SIZE] = {0U};
usart_handle_t g_usartHandle;
volatile bool isTransferCompleted = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */
status_t DEMO_InitDebugConsole(void)
{
    status_t result;

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

    result =
        DbgConsole_Init(DEMO_DEBUG_CONSOLE_USART, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);

#ifndef RTL_SIMU_ON_ES2
    CLOCK_uDelay(500);
#endif

    return result;
}

/* USART user callback */
void USART_UserCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_USART_TxIdle == status)
    {
        isTransferCompleted = true;
    }
}
/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t i = 0U, errCount = 0U;
    status_t status = 0;
    usart_config_t config;
    usart_transfer_t sendXfer;

    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART0 and USART1 */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART0 and USART1 */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    DEMO_InitDebugConsole();
    BOARD_InitPins();
    PRINTF("This is USART hardware flow control example on one board.\r\n");
    PRINTF("This example will send data to itself and will use hardware flow control to avoid the overflow.\r\n");
    PRINTF("Please make sure you make the correct line connection. Basically, the connection is: \r\n");
    PRINTF("      USART_TX    --     USART_RX    \r\n");
    PRINTF("      USART_RTS   --     USART_CTS   \r\n");

    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUSART_ParityDisabled;
     * config.stopBitCount = kUSART_OneStopBit;
     * config.txWatermark = kUSART_TxFifo0;
     * config.rxWatermark = kUSART_RxFifo1;
     * config.enableTx = false;
     * config.enableRx = false;
     * config.enableHardwareFlowControl = false;
     */
    USART_GetDefaultConfig(&config);
    config.baudRate_Bps              = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx                  = true;
    config.enableRx                  = true;
    config.enableHardwareFlowControl = true;

    USART_Init(DEMO_USART, &config, DEMO_USART_CLK_FREQ);
    USART_TransferCreateHandle(DEMO_USART, &g_usartHandle, USART_UserCallback, NULL);

    /* Set up the transfer data */
    for (i = 0U; i < TRANSFER_SIZE; i++)
    {
        transferTxData[i] = i % 256U;
        transferRxData[i] = 0U;
    }

    sendXfer.data     = (uint8_t *)transferTxData;
    sendXfer.dataSize = TRANSFER_SIZE;
    USART_TransferSendNonBlocking(DEMO_USART, &g_usartHandle, &sendXfer);

    /* Delay for some time to let the RTS pin dessart. */
    for (i = 0U; i < DELAY_TIME; i++)
    {
        __NOP();
    }

    status = USART_ReadBlocking(DEMO_USART, transferRxData, TRANSFER_SIZE);
    if (kStatus_Success != status)
    {
        PRINTF(" Error occurred when USART receiving data.\r\n");
    }
    /* Wait for the transmit complete. */
    while (!isTransferCompleted)
    {
    }

    for (i = 0U; i < TRANSFER_SIZE; i++)
    {
        if (transferTxData[i] != transferRxData[i])
        {
            errCount++;
        }
    }
    if (errCount)
    {
        PRINTF("Data not matched! Transfer error.\r\n");
    }
    else
    {
        PRINTF("Data matched! Transfer successfully.\r\n");
    }

    /* Deinit the USART. */
    USART_Deinit(DEMO_USART);

    while (1)
    {
    }
}
