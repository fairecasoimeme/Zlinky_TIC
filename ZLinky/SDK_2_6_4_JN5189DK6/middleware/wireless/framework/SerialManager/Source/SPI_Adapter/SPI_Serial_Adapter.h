/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef __SPI_ADAPTER_H__
#define __SPI_ADAPTER_H__

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "fsl_device_registers.h"


/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#ifndef gSpi_IsrPrio_c
#define gSpi_IsrPrio_c              (0x40)
#endif

#ifndef gSpi_DummyChar_d
#define gSpi_DummyChar_d            (0xFF)
#endif

#define gSPI_ByteRxFlag_d           (1 << 0)
#define gSPI_RxEndFlag_d            (1 << 1)
#define gSPI_TxEndFlag_d            (1 << 2)

/* Defines for QN9080 */
#ifndef kSPI_TxBufferEmptyFlag
#define kSPI_TxBufferEmptyFlag kSPI_TxEmptyFlag
#endif

#ifndef kSPI_RxNearFullClearInterrupt
#define kSPI_RxNearFullClearInterrupt SPI_FIFOSTAT_RXFULL_MASK
#endif

#ifndef kSPI_RxFifoNearFullInterruptEnable
#define kSPI_RxFifoNearFullInterruptEnable SPI_FIFOINTENSET_RXLVL_MASK
#endif
/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
typedef enum {
    spiSuccess,
    spiInvalidParameter,
    spiBusy
}spiStatus_t;

typedef struct spiState_tag spiState_t;

typedef void (*pfSPIx_TRxCB_t)(uint32_t flags, spiState_t* pSpiState);


typedef struct spiBusConfig_tag{
    uint32_t bitsPerSec;
    bool_t   master;
    bool_t   clkActiveHigh;
    bool_t   clkPhaseFirstEdge;
    bool_t   MsbFirst;
}spiBusConfig_t;


struct spiState_tag{
#if FSL_FEATURE_SOC_DSPI_COUNT
    uint32_t dspiCmd;             /* Preformatted DSPI command (internal use) */
#endif
    pfSPIx_TRxCB_t cb;            /* Callback function which signals the end of a transfer */
    void*             callbackParam; /* Parameter which can be used in Callback function */
    uint8_t          *pTxData;       /* Pointer to the data to be sent over SPI */
    uint8_t          *pRxData;       /* Pointer to a location where to store received data */
    volatile uint16_t txByteCount;   /* Remaining bytes to tx over SPI */
    volatile uint16_t trxByteCount;  /* Remaining bytes to be transfered over SPI */
    bool_t            signalRxByte;  /* Run Callback function at every rx byte ? */
};


/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */
spiStatus_t Spi_Init      (uint32_t instance, spiState_t* pSpiState, pfSPIx_TRxCB_t pfCallback, void* cbParam);
spiStatus_t Spi_Configure (uint32_t instance, spiBusConfig_t* pConfig);
spiStatus_t Spi_SyncTransfer (uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size);
spiStatus_t Spi_AsyncTransfer(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size);
spiStatus_t Spi_AsyncTransferSlave(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size);
spiStatus_t Spi_SendDummyByte(uint32_t instance, uint8_t* pRxData);
#endif /* __SPI_ADAPTER_H__ */