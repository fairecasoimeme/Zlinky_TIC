/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_spifi_dma.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CLOCK_ABSTRACTION
/* Abstract attaching the clock */
#define EXAMPLE_SPIFI_CLK_SRC (kMAIN_CLK_to_SPIFI)
#define EXAMPLE_SPIFI_ATTACH_MAIN_CLK (CLOCK_AttachClk(EXAMPLE_SPIFI_CLK_SRC))
/* Abstract getting the clock */
#define EXAMPLE_SPIFI_CLK (kCLOCK_Spifi)
#define EXAMPLE_SPIFI_CLOCK_FREQ (CLOCK_GetFreq(EXAMPLE_SPIFI_CLK))

#define EXAMPLE_DMA (DMA0)
#define EXAMPLE_SPIFI_CHANNEL (12)

#define EXAMPLE_SPIFI SPIFI
#define PAGE_SIZE (256)
#define SECTOR_SIZE (4096)
#define EXAMPLE_SPI_BAUDRATE (16000000L)


/*! SR_WIP - Write in progress bit. Volatile bit.
 *  0: not in write operation. 1: write operation.
 */
#define SR_WIP(x) (((uint32_t)(((uint32_t)(x)) << SR_WIP_SHIFT)) & SR_WIP_MASK)
#define SR_WIP_MASK (0x1U)
#define SR_WIP_SHIFT (0U)

/*! SR_WEL - Write enable latch. Volatile bit.
 *  0: not write enable. 1: write enable.
 */
#define SR_WEL(x) (((uint32_t)(((uint32_t)(x)) << SR_WEL_SHIFT)) & SR_WEL_MASK)
#define SR_WEL_MASK (0x2U)
#define SR_WEL_SHIFT (1U)

/*! SR_QE - Quad enable. Non-volatile bit.
 *  0: not Quad enable. 1: Quad enable.
 */
#define SR_QE(x) (((uint32_t)(((uint32_t)(x)) << SR_QE_SHIFT)) & SR_QE_MASK)
#define SR_QE_MASK (0x40U)
#define SR_QE_SHIFT (6U)

typedef enum _command_t
{
    RDID,  /* Write Status Register */
    RDSR,  /* Read Status Register */
    RDCR,  /* Read Configuration Register */
    WREN,  /* Write Enable */
    WRDI,  /* Write Disable */
    WRSR,  /* Write Status Register */
    PP4,   /* Quad Page Program */
    QPP,   /* Quad Input Page Program */
    QREAD, /* 1I-4O read */
    SE,    /* Sector Erase */
    READ,  /* Read */
    DREAD, /* 1I-2O read */
    READ2, /* 2I-2O read */
    PP,    /* Page Program */
    READ4, /* 4I-4O read */
    BE64K, /* Block Erase */
    BE32K, /* Block Erase */
    CE,    /* Chip Erase */
    DP,    /* Deep Power Down */
    RSTEN, /* Reset Enable*/
    RST,   /* Reset*/
    MAX_CMD
} command_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static dma_handle_t s_DmaHandle;
static spifi_dma_handle_t handle;
static volatile bool finished = false;
/*******************************************************************************
 * Variables
 ******************************************************************************/
SDK_ALIGN(uint8_t g_buffer[PAGE_SIZE], 4) = {0};

spifi_command_t command[] = {
    [RDID] = {4, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x9F},
    [RDSR] = {1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
    [RDCR] = {4, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x15},
    [WREN] = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
    [WRDI] = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x04},
    [WRSR] = {3, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x01},
    [PP4]  = {PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes,
             0x38},
    [QPP]  = {PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x32},
    [QREAD] = {PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x6B},
    [SE]    = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
    [READ] = {PAGE_SIZE, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x03},
    [DREAD] = {PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x3B},
    [READ2] = {PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes,
               0xBB},
    [PP] = {PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x02},
    [READ4] = {PAGE_SIZE, false, kSPIFI_DataInput, 3, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes,
               0xEB},
    [BE64K] = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0xD8},
    [BE32K] = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x52},
    [CE]    = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x60},
    [DP]    = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0xB9},
    [RSTEN] = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x66},
    [RST]   = {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x99},
};

/*******************************************************************************
 * Code
 ******************************************************************************/
void callback(SPIFI_Type *base, spifi_dma_handle_t *handle, status_t status, void *userData)
{
    finished = true;
}

uint32_t get_status_register(void)
{
    uint32_t val = 0;

    SPIFI_SetCommand(EXAMPLE_SPIFI, &command[RDSR]);
    while ((EXAMPLE_SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U)
    {
    }

    val = SPIFI_ReadPartialWord(SPIFI, command[RDSR].dataLen);

    return val;
}

void check_if_finish()
{
    uint32_t val = 0;
    /* Polling the WIP bit to make sure SPIFI flash is not in write operation */
    do
    {
        val = get_status_register();
    } while (val & SR_WIP_MASK);
}

void enable_quad_mode()
{
    uint32_t val = 0;

    /* Send the WREN command and polling the WEL bit */
    do
    {
        SPIFI_SetCommand(EXAMPLE_SPIFI, &command[WREN]);

        val = get_status_register();
    } while (!(val & SR_WEL_MASK));

    /* Set write register command */
    SPIFI_SetCommand(EXAMPLE_SPIFI, &command[WRSR]);

    SPIFI_WritePartialWord(EXAMPLE_SPIFI, SR_QE(1U), command[WRSR].dataLen);

    /* Polling the WIP bit to make sure SPIFI flash is not in write operation */
    do
    {
        val = get_status_register();
    } while (val & SR_WIP_MASK);

    /* Check the QE bit of the status register to make sure quad mode is enabled */
    do
    {
        val = get_status_register();
    } while (!(val & SR_QE_MASK));
}

int main(void)
{
    spifi_config_t config = {0};
    spifi_transfer_t xfer = {0};
    uint32_t i = 0, page = 0, err = 0;
    uint8_t *val = (uint8_t *)FSL_FEATURE_SPIFI_START_ADDR;

    /* Init the boards */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    uint32_t sourceClockFreq;

    /* Set SPIFI clock source */
    CLOCK_AttachClk(EXAMPLE_SPIFI_CLK_SRC);
    sourceClockFreq = CLOCK_GetSpifiClkFreq();
    /* Set the clock divider */
    uint32_t divisor;
    /* Do not set null divisor value */
    divisor = sourceClockFreq / EXAMPLE_SPI_BAUDRATE;
    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, divisor ? divisor : 1, false);
    PRINTF("SPIFI flash dma example started \r\n");

    DMA_Init(EXAMPLE_DMA);

    DMA_EnableChannel(EXAMPLE_DMA, EXAMPLE_SPIFI_CHANNEL);
    DMA_CreateHandle(&s_DmaHandle, EXAMPLE_DMA, EXAMPLE_SPIFI_CHANNEL);

    /* Initialize SPIFI */
    SPIFI_GetDefaultConfig(&config);
    SPIFI_Init(EXAMPLE_SPIFI, &config);
    /* Reset the SPIFI to switch to command mode */
    SPIFI_ResetCommand(EXAMPLE_SPIFI);
    SPIFI_TransferRxCreateHandleDMA(EXAMPLE_SPIFI, &handle, callback, NULL, &s_DmaHandle);

    /* Reset Device*/
    SPIFI_SetCommand(EXAMPLE_SPIFI, &command[RSTEN]);
    SPIFI_SetCommand(EXAMPLE_SPIFI, &command[RST]);
    check_if_finish();

    /* Enable Quad mode */
    enable_quad_mode();

    /* Setup memory command */
    SPIFI_SetMemoryCommand(EXAMPLE_SPIFI, &command[QREAD]);

    /* Set the buffer */
    for (i = 0; i < PAGE_SIZE; i++)
    {
        g_buffer[i] = i;
    }

    /* Set address */
    SPIFI_SetCommandAddress(EXAMPLE_SPIFI, FSL_FEATURE_SPIFI_START_ADDR);
    /* Erase sector */
    SPIFI_SetCommand(EXAMPLE_SPIFI, &command[SE]);

    /* Check if finished */
    check_if_finish();

    /* Program page */
    while (page < (SECTOR_SIZE / PAGE_SIZE))
    {
        SPIFI_SetCommand(EXAMPLE_SPIFI, &command[WREN]);
        SPIFI_SetCommandAddress(EXAMPLE_SPIFI, FSL_FEATURE_SPIFI_START_ADDR + page * PAGE_SIZE);
        SPIFI_SetCommand(EXAMPLE_SPIFI, &command[PP]);
        xfer.data     = g_buffer;
        xfer.dataSize = PAGE_SIZE;
        SPIFI_TransferSendDMA(EXAMPLE_SPIFI, &handle, &xfer);
        while (!finished)
        {
        }
        finished = false;
        page++;
        check_if_finish();
    }

    /* Reset to memory command mode */
    SPIFI_ResetCommand(EXAMPLE_SPIFI);

    SPIFI_SetMemoryCommand(EXAMPLE_SPIFI, &command[READ]);

    for (i = 0; i < SECTOR_SIZE; i++)
    {
        val = (uint8_t *)(FSL_FEATURE_SPIFI_START_ADDR + i);
        if (*val != g_buffer[i % PAGE_SIZE])
        {
            PRINTF("Data error in address 0x%x, the value in memory is 0x%x\r\n", i, *val);
            err++;
        }
    }

    if (err == 0)
    {
        PRINTF("All data written is correct!\r\n");
    }

    PRINTF("SPIFI DMA example Finished!\r\n");
    while (1)
    {
    }
}
