/*
 * qspi_flash.c
 *
 *  Created on: Dec 20, 2018
 *      Author: nxa27412
 */


/*
 * Copyright 2018-2019 NXP
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

#include "fsl_device_registers.h"
#include "fsl_spifi.h"
#include "fsl_iocon.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SPIFI_DMA_CHANNEL    12
#define MHz                  (1000000UL)

#define SPI_BAUDRATE         (32*MHz)
// #define SPI_BAUDRATE         (16*MHz)

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define QSPIFLASH_DEV_IS_MX25R8035F
//#define QSPIFLASH_DEV_IS_S25FL116


#if defined (QSPIFLASH_DEV_IS_MX25R8035F)

/* Macronix MX25R8035F QSPI Flash Part */
#define MX25R8035F_DEVICE_ID         0x1428C2             /* 0xC2 is Manufacturer Id for Macronix, 0x28 is the type, 0x14 is the density  */
#define XT25F08B_DEVICE_ID           0x14400B

#define MX25R8035F_PAGE_SIZE        256                  /* 256B sectors */
#define MX25R8035F_SECTOR_SIZE      4096                 /* 4KB sectors */
#define MX25R8035F_BLOCK_SIZE       QSPIFLASH_SECTOR_SIZE * 16     /* 64KB blocks */
#define MX25R8035F_CHIP_SIZE        QSPIFLASHF_SECTOR_SIZE * 2000   /* 8MB capacity */

#define QSPIFLASH_PAGE_SIZE        MX25R8035F_PAGE_SIZE
#define QSPIFLASH_SECTOR_SIZE      MX25R8035F_SECTOR_SIZE
#define QSPIFLASH_BLOCK_SIZE       MX25R8035F_BLOCK_SIZE
#define QSPIFLASH_CHIP_SIZE        MX25R8035F_CHIP_SIZE

#define MX25_SR_WIP_POS 0     /* Write In Progress */
#define MX25_SR_WEL_POS 1     /* Write Enable Latch */
#define MX25_SR_BP_POS 2      /* Level of Protected block  */
#define MX25_SR_BP_WIDTH 4
#define MX25_SR_BP_MASK       (((1<<MX25_SR_BP_WIDTH)-1) << MX25_SR_BP_POS)
#define MX25_SR_QE_POS 6      /* Non Volatile  */


#define MX25_CR1_TB_POS 3     /* Top-Bottom selected */
#define MX25_CR1_DC_POS 6     /* Dummy Cycle */
#define MX25_CR2_LH_POS 1      /* LowPower / HighPerformance  */

#define MX25R8035_CFG_STATUS_QUAD_MODE   BIT(MX25_SR_QE_POS)
#define MX25R8035_CFG_REG1_DUMMY_CYCLE   (BIT(MX25_CR1_DC_POS) << 8)
#define MX25R8035_CFG_REG2_HI_PERF_MODE  (BIT(MX25_CR2_LH_POS) << 16)

#elif defined (QSPIFLASH_DEV_IS_S25FL116)
/* Spansion S25FL116 QSPI Flash Part */
#define S25FL116_DEVICE_ID        0x154001             /* 0x01 is Manufacturer Id for Spansion, 0x40 is the type, 0x15 is the density  */
#define S25FL116_PAGE_SIZE        256                  /* 256B sectors */
#define S25FL116_SECTOR_SIZE      4096                 /* 4KB sectors */
#define S25FL116_BLOCK_SIZE       S25FL116_SECTOR_SIZE * 16     /* 64KB blocks */
#define S25FL116_CHIP_SIZE        S25FL116_SECTOR_SIZE * 2000   /* 8MB capacity */

#define QSPIFLASH_PAGE_SIZE        S25FL116_PAGE_SIZE
#define QSPIFLASH_SECTOR_SIZE      S25FL116_SECTOR_SIZE
#define QSPIFLASH_BLOCK_SIZE       S25FL116_BLOCK_SIZE
#define QSPIFLASH_CHIP_SIZE        S25FL116_CHIP_SIZE

/* Spansion SF25FL Status Registers */
#define S25FL116_SR1_BUSY_POS       0    /* Busy */
#define S25FL116_SR1_WEL_POS        1    /* Write Enable Latch */
#define S25FL116_SR1_BP_POS         2    /* Level of Protected block  */
#define S25FL116_SR1_BP_WIDTH       3    /* Write Enable Latch */
#define S25FL116_SR1_BP_MASK        (((1<<S25FL116_SR1_BP_WIDTH)-1) << S25FL116_SR1_BP_POS)
#define S25FL116_SR1_TB_POS         5    /* Top-Bottom Protect  */
#define S25FL116_SR1_SEC_POS        6    /* Sector-Block Protect */
#define S25FL116_SR1_SRP0_POS       7    /* Sector-Block Protect */

#define S25FL116_SR2_SRP1_POS       0    /* Busy */
#define S25FL116_SR2_QE_POS         1    /* Write Enable Latch */
#define S25FL116_SR2_LB_POS         2    /* Level of Protected block  */
#define S25FL116_SR2_LB_WIDTH       4    /* Write Enable Latch */
#define S25FL116_SR2_LB_MASK        (((1<<S25FL116_SR2_LB_WIDTH)-1) << S25FL116_SR2_LB_POS)
#define S25FL116_SR2_CMP_POS        6    /* Complement Protect */
#define S25FL116_SR2_SUS_POS        7    /* Suspend Status */

#define S25FL116_SR3_LC_POS         0    /* Latency Control */
#define S25FL116_SR3_LC_WIDTH       4
#define S25FL116_SR3_LC_MASK        (((1<<S25FL116_SR3_LC_WIDTH)-1) << S25FL116_SR3_LC_POS)
#define S25FL116_SR3_WRAP_EN_POS    4    /* Burst Wrap enable */
#define S25FL116_SR3_WRAP_LEN_POS   5    /* Burst Wrap length  */
#define S25FL116_SR3_WRAP_LEN_WIDTH 2
#define S25FL116_SR3_WRAP_LEN_MASK  (((1<<S25FL116_SR3_WRAP_LEN_WIDTH)-1) << S25FL116_SR3_WRAP_LEN_POS)

/* Spansion SF25FL Status Registers */
#define S25FL116_CR1_TB_POS         3     /* Top-Bottom selected */
#define S25FL116_CR1_DC_POS         6     /* Dummy Cycle */
#define S25FL116_CR2_LH_POS         1      /* LowPower / HighPerformance  */
#else
#endif
#define QSPI_CS_PIN 16
#define QSPI_WP_PIN 20

/*******************************************************************************
 * Types
 ******************************************************************************/
typedef enum _command_t
{
      RDID,      /* Write Status Register */
      RDSR,      /* Read Status Register */

//    RDCR,      /* Read Configuration Register */
      WREN,      /* Write Enable */
//    WRDI,      /* Write Disable */
      WRSR,      /* Write Status Register */
//    PP4,       /* Quad Page Program */
//    QPP,       /* Quad Input Page Program */
      QREAD,     /* 1I-4O read */
      SE,        /* Sector Erase */
//    READ,      /* Read */
      DREAD,     /* 1I-2O read */
//    READ2,     /* 2I-2O read */
//    PP,        /* Page Program */
//    READ4,     /* 4I-4O read */
      BE64K,        /* Block Erase */
      BE32K,        /* Block Erase */
      CE,        /* Chip Erase */
      DP,        /* Deep Power Down */
      MAX_CMD
} command_t;


typedef struct QSpiFlashConfig_tag {
    uint32_t device_id;
    uint16_t page_size;
    uint16_t sector_size;
    uint32_t block_size;
    uint32_t capacity;
} QSpiFlashConfig_t;


/*******************************************************************************
 * Data
 ******************************************************************************/
#if 0

const QSpiFlashConfig_t Macronix_MX25R8035F_device_def = {
        .device_id   = MX25R8035F_DEVICE_ID,
        .page_size   = MX25R8035F_PAGE_SIZE,
        .sector_size = MX25R8035F_SECTOR_SIZE,
        .block_size  = MX25R8035F_BLOCK_SIZE,
        .capacity    = MX25R8035F_CHIP_SIZE
};
const QSpiFlashConfig_t Spansion_S25FL116_device_def = {
        .device_id   = S25FL116_DEVICE_ID,
        .page_size   =  S25FL116_PAGE_SIZE,
        .sector_size = S25FL116_SECTOR_SIZE,
        .block_size  = S25FL116_BLOCK_SIZE,
        .capacity    = S25FL116_CHIP_SIZE
};
const QSpiFlashConfig_t * const supported_flash_devices[] = {
        &Macronix_MX25R8035F_device_def,
#if 0
        &Spansion_S25FL116_device_def
#endif
};
#endif



spifi_config_t config = {
        .timeout = 0xFFFFU,
        .csHighTime = 0xFU,
        .disablePrefetch = false,
        .disableCachePrefech = false,
        .isFeedbackClock = true,
        .spiMode = kSPIFI_SPISckLow,
        .isReadFullClockCycle = false,
        .dualMode = kSPIFI_QuadMode
};


static spifi_command_t command[MAX_CMD] =
{
    [RDID]  = {4                  , false, kSPIFI_DataInput,  0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,   0x9F},
    [RDSR]  = {1                  , false, kSPIFI_DataInput,  0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,   0x05},
//  [RDCR]  = {4                  , false, kSPIFI_DataInput,  0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,   0x15},
    [WREN]  = {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,   0x06},
//  [WRDI]  = {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,   0x04},
    [WRSR]  = {3                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,   0x01},
//  [PP4]   = {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x38},
//  [QPP]   = {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandDataQuad,     kSPIFI_CommandOpcodeAddrThreeBytes, 0x32},
    [QREAD] = {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataInput,  1, kSPIFI_CommandDataQuad,     kSPIFI_CommandOpcodeAddrThreeBytes, 0x6B},
    [SE]    = {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
//  [READ]  = {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataInput,  0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeAddrThreeBytes, 0x03},
    [DREAD] =  {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataInput,  1, kSPIFI_CommandDataQuad,     kSPIFI_CommandOpcodeAddrThreeBytes, 0x3B},
//  [READ2] =  {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataInput,  1, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0xBB},
//  [PP]    =  {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeAddrThreeBytes, 0x02},
//  [READ4] =  {QSPIFLASH_PAGE_SIZE, false, kSPIFI_DataInput,  3, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0xEB},
    [BE64K] =  {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeAddrThreeBytes, 0xD8},
    [BE32K] =  {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeAddrThreeBytes, 0x52},
    [CE]    =  {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,           0x60},
    [DP]    =  {0                  , false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly,           0xB9},
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void qspi_wait_for_completion(void);

static uint32_t SPIFI_readCommand (command_t cmd);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/

static void ConfigureSpiFi(void)
{
#define IOCON_QSPI_MODE_FUNC    (7U)

    /* Set up SPIFI: it comprises 6 pins : PIO[16:21]
     * PIO[16] : CS
     * PIO[18] : CLK
     * PIO[19] : MOSI
     * PIO[21] : MISO
     * */
    for (int pin = 16; pin <= 21; pin++)
    {
        IOCON_PinMuxSet(IOCON, 0, pin,
                        IOCON_PIO_FUNC(IOCON_QSPI_MODE_FUNC) | IOCON_PIO_MODE(2)
                      | IOCON_PIO_DIGIMODE(1) | IOCON_PIO_FILTEROFF(0)
                      | IOCON_PIO_SLEW0(1) | IOCON_PIO_SLEW1(1));
    }
}




int SPIFI_Flash_Init(void)
{
    int status = -1;

    spifi_config_t config = {0};
#define SPIFI_CLK  kMAIN_CLK_to_SPIFI
//#define SPIFI_CLK kFRO48M_to_SPIFI

    do {
        // Init SPIFI clk
#ifdef NO_OPT_SZ
        RESET_SetPeripheralReset(kSPIFI_RST_SHIFT_RSTn);
        CLOCK_AttachClk(SPIFI_CLK);
        uint32_t divisor = CLOCK_GetSpifiClkFreq() / SPI_BAUDRATE;
        CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, divisor ? divisor : 1, false);
        CLOCK_EnableClock(kCLOCK_Spifi);
        /* Set the clock divider */
        SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRLSET0_SPIFI_CLK_SET_MASK;
        ConfigureSpiFi();
        RESET_ClearPeripheralReset(kSPIFI_RST_SHIFT_RSTn);
#else
        SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL0_SPIFI_RST_SHIFT; /* Hold SPIFI under reset */
        SYSCON->SPIFICLKDIV |= SYSCON_SPIFICLKDIV_HALT_MASK;
        SYSCON->SPIFICLKSEL = kCLOCK_SpifiMainClk;  /* Main CLK to SPIFI CLK : supposed to be 32MHz */

        ConfigureSpiFi();
        /* Set the clock divider */
        uint32_t divisor = 32 / SPI_BAUDRATE;
        // CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, divisor ? divisor : 1, false);
        SYSCON->SPIFICLKDIV &= ~SYSCON_SPIFICLKDIV_DIV_MASK;
        if ((divisor-1) > 0)
        {
            SYSCON->SPIFICLKDIV |= SYSCON_SPIFICLKDIV_DIV(divisor-1);
        }
        SYSCON->SPIFICLKDIV &= ~SYSCON_SPIFICLKDIV_HALT_MASK;
        SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRLSET0_SPIFI_CLK_SET_MASK;
        SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL0_SPIFI_RST_SHIFT; /* Release SPIFI reset */
#endif
        /* Initialize SPIFI */
        SPIFI_GetDefaultConfig(&config);
        bool dual_mode = false;
        if (CHIP_USING_SPIFI_DUAL_MODE())
        {
            config.dualMode = kSPIFI_DualMode;
            dual_mode = true;
        }
        SPIFI_Init(SPIFI, &config);

        uint32_t val = SPIFI_readCommand(RDID);
        val &= 0x00FFFFFF;

        /* Write enable */
        SPIFI_SetCommand(SPIFI, &command[WREN]);


        switch (val)
        {
            case MX25R8035F_DEVICE_ID:
            {
                /* Set write register command */
                uint32_t cfg_word = 0x000000; /* 24 bit register */
#ifdef gSpiFiHiPerfMode_d
                cfg_word |= MX25R8035_CFG_REG2_HI_PERF_MODE;
#endif
                if (! dual_mode )
                {
                    /* insert dummy cycles for Quad mode operation */
                    cfg_word |= MX25R8035_CFG_STATUS_QUAD_MODE;
                }
                SPIFI_SetCommand(SPIFI, &command[WRSR]);
                SPIFI_WritePartialWord(SPIFI, cfg_word, 3);
                status = 0;
            }
            break;

            case XT25F08B_DEVICE_ID:
                command[WRSR].dataLen=2;
                SPIFI_SetCommand(SPIFI, &command[WRSR]);
                SPIFI_WriteDataHalfword(SPIFI, 0x0200);
                status = 0;
                break;
            default:
                break;
        }

        qspi_wait_for_completion();

        // Set address
        uint32_t addr = 0;
        SPIFI_SetCommandAddress(SPIFI, FSL_FEATURE_SPIFI_START_ADDR + addr);
        /* Setup memory command */

        command_t read_op =  dual_mode ? DREAD : QREAD;

        SPIFI_SetMemoryCommand(SPIFI, &command[read_op]);

    } while (0);
    return status;
}



void SPIFI_Flash_Deinit(void)
{
    SPIFI_SetCommand(SPIFI, &command[DP]);

    SPIFI_Deinit(SPIFI);
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRLSET0_SPIFI_CLK_SET_MASK;
#ifdef NO_OPT_SZ
    CLOCK_DisableClock(kCLOCK_Spifi);
#endif
}


static void qspi_wait_for_completion(void)
{
    uint32_t val = 0;

    /* Check WIP bit */
    do
    {
        SPIFI_SetCommand(SPIFI, &command[RDSR]);
        {
            while ((SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U);
        }
        val = SPIFI_ReadPartialWord(SPIFI, command[RDSR].dataLen);
    } while (val & 0x1); /* BIT(0) is BUSY flag */
}


uint32_t SPIFI_readCommand (command_t cmd)
{
    /* Reset the SPIFI to switch to command mode */
    SPIFI_ResetCommand(SPIFI);
    /* Setup command */
    SPIFI_SetCommand(SPIFI, &command[cmd]);
    while ((SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U);

    // Return data
    return(SPIFI_ReadData(SPIFI));
}

void SPIFI_ChipErase(void)
{
    /* Reset the SPIFI to switch to command mode */
    SPIFI_ResetCommand(SPIFI);

    qspi_wait_for_completion();

    SPIFI_SetCommand(SPIFI, &command[WREN]);

    /* Chip Erase command */
    SPIFI_SetCommand(SPIFI, &command[CE]);
    while ((SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U);

}
/*******************************************************************/
