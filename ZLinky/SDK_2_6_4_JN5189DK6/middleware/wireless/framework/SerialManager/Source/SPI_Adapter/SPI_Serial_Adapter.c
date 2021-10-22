/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "SPI_Serial_Adapter.h"
#include "pin_mux.h"
#include "Panic.h"
#include "fsl_common.h"
#include "board.h"
#include "fsl_os_abstraction.h"

/* TODO modify to take into account if multiple SPIs are present */
#if FSL_FEATURE_SOC_LPSPI_COUNT
#include "fsl_lpspi.h"
#elif FSL_FEATURE_SOC_DSPI_COUNT
#include "fsl_dspi.h"
#else
#include "fsl_spi.h"
#endif


/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#ifndef gXcvrSpiInstance_c
#define gXcvrSpiInstance_c (0xFF)
#endif

#if FSL_FEATURE_SOC_LPSPI_COUNT
    #define SPI_INSTANCE_COUNT FSL_FEATURE_SOC_LPSPI_COUNT
#elif FSL_FEATURE_SOC_DSPI_COUNT
    #define SPI_INSTANCE_COUNT FSL_FEATURE_SOC_DSPI_COUNT
#else 
    #define SPI_INSTANCE_COUNT FSL_FEATURE_SOC_SPI_COUNT
#endif

#if defined (CPU_QN908X)
#define FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT    FSL_FEATURE_SOC_SPI_COUNT   /* To move in *_features.h */
#endif

#if defined FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT
#define gSpi_UseFifo_c (1)
#endif


#ifndef gSpi_UseFifo_c
    #if (defined(FSL_FEATURE_SPI_HAS_FIFO) && FSL_FEATURE_SPI_HAS_FIFO) || \
        (defined(FSL_FEATURE_SOC_DSPI_COUNT) && FSL_FEATURE_SOC_DSPI_COUNT)
        #define gSpi_UseFifo_c              (1)
    #else
        #define gSpi_UseFifo_c              (0)
    #endif
#endif

#if FSL_FEATURE_SOC_LPSPI_COUNT
#define SPI_Type LPSPI_Type
#define SPI_BASE_PTRS LPSPI_BASE_PTRS
#define SPI_IRQS LPSPI_IRQS
#endif


/*! *********************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
********************************************************************************** */


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
static void SPIx_ISR(void);
static bool_t Spi_IsMaster(SPI_Type* baseAddr);
static void Spi_SetIntState(SPI_Type* baseAddr, bool_t state);
static uint32_t Spi_ReadData(SPI_Type* baseAddr);
static void Spi_WriteData(SPI_Type* baseAddr, spiState_t* pState, uint32_t data);
static void Spi_FillTxFifoSlave(SPI_Type* baseAddr, spiState_t *pState);
static bool_t Spi_SlaveTransmitCompleted(SPI_Type* baseAddr);

#ifdef FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT
static void SPI_EnableFIFO(SPI_Type *base, bool enable);
static inline void SPI_ClearInterrupt(SPI_Type *base, uint32_t interrupts);
#endif

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
/*! @brief Pointer to runtime state structure.*/
spiState_t * gpaSpiState[SPI_INSTANCE_COUNT];
/*! @brief Table of base pointers for SPI instances. */
SPI_Type * const mSpiBase[SPI_INSTANCE_COUNT] = SPI_BASE_PTRS;
/*! @brief Table to save SPI IRQ enum numbers defined in CMSIS files. */
const IRQn_Type mSpiIrqId[] = SPI_IRQS;

static uint8_t mSPI_dummyData = gSpi_DummyChar_d;

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief   This function will initialize the SPI module
*
* \param[in] instance     The SPI module number
* \param[in] pSpiState    Pointer to a location where to store the SPI state
* \param[in] pfCallback   Pointer to a Callback function
* \param[in] cbParam      Parameter to be used in Callback function
*
* \return error code
*
********************************************************************************** */
spiStatus_t Spi_Init(uint32_t instance, spiState_t* pSpiState, pfSPIx_TRxCB_t pfCallback, void* cbParam)
{
    spiStatus_t status = spiSuccess;
    IRQn_Type spiIrq = mSpiIrqId[instance];

    if( (NULL == pSpiState) || (instance == gXcvrSpiInstance_c) || (instance >= SPI_INSTANCE_COUNT))
    {
        panic(0,(uint32_t)Spi_Init,0,0);
        status = spiInvalidParameter;
    }
    else
    {
        /* set SPI Pin Mux */    
        BOARD_InitSPI();
        /* Store Callback function and parameter */
        gpaSpiState[instance] = pSpiState;
        pSpiState->cb = pfCallback;
        pSpiState->callbackParam = cbParam;
        pSpiState->pRxData = NULL;
        pSpiState->pTxData = NULL;
        pSpiState->trxByteCount = 0;
        pSpiState->txByteCount  = 0;
        /* Overwrite old ISR */
        OSA_InstallIntHandler(spiIrq, SPIx_ISR);
        /* set interrupt priority */
        NVIC_SetPriority(spiIrq, gSpi_IsrPrio_c >> (8 - __NVIC_PRIO_BITS));
        NVIC_ClearPendingIRQ(spiIrq);
        NVIC_EnableIRQ(spiIrq);
    }

    return status;
}


/*! *********************************************************************************
* \brief   This function will configure the SPI module
*
* \param[in] instance     The SPI module number
* \param[in] pConfig      Pointer to the configuration structure
*
* \return error code
*
********************************************************************************** */
spiStatus_t Spi_Configure(uint32_t instance, spiBusConfig_t* pConfig)
{
    bool_t intState;
    spiStatus_t status = spiSuccess;
    SPI_Type *baseAddr = mSpiBase[instance];
    spiState_t* pState = gpaSpiState[instance];
    
    if( (NULL == pConfig) || (instance >= SPI_INSTANCE_COUNT) )
    {
        status = spiInvalidParameter;
    }
    else
    {
        /* Enable SPI IRQ is configured as Slave OR if Rx byte signaling is enabled*/
        intState = (!pConfig->master) || (pState->signalRxByte);

#if FSL_FEATURE_SOC_LPSPI_COUNT
        if( pConfig->master )
        {
            lpspi_master_config_t config;
            
            LPSPI_MasterGetDefaultConfig(&config);
            config.baudRate = pConfig->bitsPerSec;
            config.betweenTransferDelayInNanoSec = 1000;
            config.lastSckToPcsDelayInNanoSec = 0;
            config.pcsToSckDelayInNanoSec = 0;
            /* SPI Clock Polarity */    
            if( !pConfig->clkActiveHigh )
            {
                config.cpol = kLPSPI_ClockPolarityActiveLow;
            }
            /* SPI Clock Phase */
            if( !pConfig->clkPhaseFirstEdge )
            {
                config.cpha = kLPSPI_ClockPhaseSecondEdge;
            }
            /* SPI shift direction */
            if( !pConfig->MsbFirst )
            {
                config.direction = kLPSPI_LsbFirst;
            }
            LPSPI_MasterInit(baseAddr, &config, BOARD_GetSpiClock(instance));
        }
        else
        {
            lpspi_slave_config_t config;
            
            LPSPI_SlaveGetDefaultConfig(&config);
            /* SPI Clock Polarity */    
            if( !pConfig->clkActiveHigh )
            {
                config.cpol = kLPSPI_ClockPolarityActiveLow;
            }
            /* SPI Clock Phase */
            if( !pConfig->clkPhaseFirstEdge )
            {
                config.cpha = kLPSPI_ClockPhaseSecondEdge;
            }
            LPSPI_SlaveInit(baseAddr, &config);
            LPSPI_WriteData(baseAddr, gSpi_DummyChar_d);
        }
#elif FSL_FEATURE_SOC_DSPI_COUNT
        /* Apply DSPI configuration */
        if( pConfig->master )
        {
            dspi_master_config_t config;
            dspi_command_data_config_t cmdConfig;
            
            DSPI_MasterGetDefaultConfig(&config);
            config.ctarConfig.baudRate = pConfig->bitsPerSec;
            config.ctarConfig.betweenTransferDelayInNanoSec = 1000;
            config.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
            config.ctarConfig.pcsToSckDelayInNanoSec = 0;
            /* SPI Clock Polarity */    
            if( !pConfig->clkActiveHigh )
            {
                config.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
            }
            /* SPI Clock Phase */
            if( !pConfig->clkPhaseFirstEdge )
            {
                config.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
            }
            /* SPI shift direction */
            if( !pConfig->MsbFirst )
            {
                config.ctarConfig.direction = kDSPI_LsbFirst;
            }
            DSPI_MasterInit(baseAddr, &config, BOARD_GetSpiClock(instance));
            
            DSPI_GetDefaultDataCommandConfig(&cmdConfig);
            pState->dspiCmd = DSPI_MasterGetFormattedCommand(&cmdConfig);
        }
        else
        {
            dspi_slave_config_t config;
            
            DSPI_SlaveGetDefaultConfig(&config);
            /* SPI Clock Polarity */    
            if( !pConfig->clkActiveHigh )
            {
                config.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
            }
            /* SPI Clock Phase */
            if( !pConfig->clkPhaseFirstEdge )
            {
                config.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
            }
            DSPI_SlaveInit(baseAddr, &config);
            DSPI_SlaveWriteData(baseAddr, gSpi_DummyChar_d);
        }
        
#if gSpi_UseFifo_c
        /* Enable DSPI FIFO */
        DSPI_SetFifoEnable(baseAddr, TRUE, TRUE);
        DSPI_FlushFifo(baseAddr, TRUE, TRUE);
#endif
        
#else /* FSL_FEATURE_SOC_DSPI_COUNT */
        
        /* Apply SPI configuration */
        if( pConfig->master )
        {
            spi_master_config_t config;
            
            SPI_MasterGetDefaultConfig(&config);
            /* SPI Clock Polarity */
            if( !pConfig->clkActiveHigh )
            {
                config.polarity = kSPI_ClockPolarityActiveLow;
            }
            /* SPI Clock Phase */
            if( !pConfig->clkPhaseFirstEdge )
            {
                config.phase = kSPI_ClockPhaseSecondEdge;
            }
            /* SPI shift direction */
            if( !pConfig->MsbFirst )
            {
                config.direction = kSPI_LsbFirst;
            }
            config.baudRate_Bps = pConfig->bitsPerSec;
            SPI_MasterInit(baseAddr, &config, BOARD_GetSpiClock(instance));
        }
        else
        {
            spi_slave_config_t config;
            
            SPI_SlaveGetDefaultConfig(&config);
            /* SPI Clock Polarity */
            if( !pConfig->clkActiveHigh )
            {
                config.polarity = kSPI_ClockPolarityActiveLow;
            }
            /* SPI Clock Phase */
            if( !pConfig->clkPhaseFirstEdge )
            {
                config.phase = kSPI_ClockPhaseSecondEdge;
            }
            /* SPI shift direction */
            if( !pConfig->MsbFirst )
            {
                config.direction = kSPI_LsbFirst;
            }
            SPI_SlaveInit(baseAddr, &config);
        }
#if gSpi_UseFifo_c
        /* Enable SPI FIFO */
        SPI_EnableFIFO(baseAddr, 1);
#endif
#endif /* FSL_FEATURE_SOC_DSPI_COUNT */
        
        Spi_SetIntState(baseAddr, intState);
    }
    return status;
}


/*! *********************************************************************************
* \brief   This function transfer data synchronously over SPI.
*          The Callback function will not run since the transfer ends when this function returns
*
* \param[in] instance     The SPI module number
* \param[in] pTxData      Pointer to the data to be sent over SPI
                          Can be NULL if there is no data to TX (RX only)
* \param[in] pRxData      Pointer to a location where received data will be stored.
                          Can be NULL if no RX data is expected (TX only).
* \param[in] size         Number of bytes o be transfered over the SPI
*
* \return error code
*
********************************************************************************** */
spiStatus_t Spi_SyncTransfer(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    volatile uint32_t data;
    spiStatus_t status = spiSuccess;
    SPI_Type *baseAddr = mSpiBase[instance];
    spiState_t* pState = gpaSpiState[instance];
    bool_t intState = (!Spi_IsMaster(baseAddr)) || (pState->signalRxByte);

    /* Parameter validation */
    if( ((NULL == pTxData) && (NULL == pRxData)) || (instance >= SPI_INSTANCE_COUNT) )
    {
        status = spiInvalidParameter;
    }
    else
    {
        /* Check if SPI is Busy */
        OSA_InterruptDisable();
        if( pState->trxByteCount )
        {
            OSA_InterruptEnable();
            status = spiBusy;
        }
        else
        {
            /* Mark SPI as busy*/
            pState->trxByteCount = size;
            /* Disable SPI interrupts */
            Spi_SetIntState(baseAddr, FALSE);
#if FSL_FEATURE_SOC_DSPI_COUNT
            DSPI_ClearStatusFlags(baseAddr, kDSPI_TxCompleteFlag);
            /*DSPI_FlushFifo(baseAddr, TRUE, TRUE) */
#elif FSL_FEATURE_SOC_LPSPI_COUNT
            LPSPI_ClearStatusFlags(baseAddr, kLPSPI_TransferCompleteFlag);
#endif
            OSA_InterruptEnable();
            
            while( size-- )
            {
                if( pTxData )
                {
                    data = *pTxData;
                    pTxData++;
                }
                else
                {
                    data = gSpi_DummyChar_d;
                }
                
                /* Start SPI transfer */
#if FSL_FEATURE_SOC_LPSPI_COUNT
                Spi_WriteData(baseAddr, pState, data);
                while( !(LPSPI_GetStatusFlags(baseAddr) & kLPSPI_TransferCompleteFlag) ) {}
                LPSPI_ClearStatusFlags(baseAddr, kLPSPI_TransferCompleteFlag);
#elif FSL_FEATURE_SOC_DSPI_COUNT
                
                Spi_WriteData(baseAddr, pState, data);
                while( !(DSPI_GetStatusFlags(baseAddr) & kDSPI_TxCompleteFlag) ) {}
                DSPI_ClearStatusFlags(baseAddr, kDSPI_TxCompleteFlag);
                
#else /* FSL_FEATURE_SOC_DSPI_COUNT */
                       
                while( !(SPI_GetStatusFlags(baseAddr) & kSPI_TxBufferEmptyFlag) ) {}
                Spi_WriteData(baseAddr, pState, data);
                while( !(SPI_GetStatusFlags(baseAddr) & kSPI_TxBufferEmptyFlag) ) {}
#endif /* FSL_FEATURE_SOC_DSPI_COUNT */
                
                data = Spi_ReadData(baseAddr);
                
                if( pRxData )
                {
                    *pRxData = data;
                    pRxData++;
                }
            }
            
            /* Mark SPI as Idle */
            pState->trxByteCount = 0;
            /* Restore SPI IRQ state */
            Spi_SetIntState(baseAddr, intState);
        }
    }

    return status;
}

/*! *********************************************************************************
* \brief   This function transfer data asynchronously over SPI.
*          If a Callback function was registered, it will run at the end of the transfer
*
* \param[in] instance     The SPI module number
* \param[in] pTxData      Pointer to the data to be sent over SPI
                          Can be NULL if there is no data to TX (RX only)
* \param[in] pRxData      Pointer to a location where received data will be stored.
                          Can be NULL if no RX data is expected (TX only).
* \param[in] size         Number of bytes o be transfered over the SPI
*
* \return error code
*
********************************************************************************** */
spiStatus_t Spi_AsyncTransfer(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    spiStatus_t status = spiSuccess;
    SPI_Type *baseAddr = mSpiBase[instance];
    spiState_t* pState = gpaSpiState[instance];
#if !defined(FSL_FEATURE_SOC_LPSPI_COUNT) || (FSL_FEATURE_SOC_LPSPI_COUNT == 0)
    uint8_t temp = Spi_IsMaster(baseAddr);
#endif
    
    /* Parameter validation */
    if( ((NULL == pTxData) && (NULL == pRxData)) || (instance >= SPI_INSTANCE_COUNT) )
    {
        status = spiInvalidParameter;
    }
    else
    {
        /* Check if SPI is Busy */
        OSA_InterruptDisable();
        if( pState->trxByteCount )
        {
            OSA_InterruptEnable();
            status = spiBusy;
        }
        else
        {
            /* Mark SPI as busy*/
            pState->trxByteCount = size;
            OSA_InterruptEnable();
            
            /* Fill transfer parameters */
            pState->pTxData = pTxData;
            pState->pRxData = pRxData;
            pState->txByteCount = size;

#if FSL_FEATURE_SOC_LPSPI_COUNT
            /* Fill the SPI Tx FIFO (if available) */
            LPSPI_FlushFifo(baseAddr, TRUE, TRUE);

            while( pState->txByteCount  && (LPSPI_GetTxFifoCount(baseAddr) < LPSPI_GetTxFifoSize(baseAddr)) )
            {
                OSA_InterruptDisable();
                --pState->txByteCount;
                
                if( pState->pTxData )
                {
                    Spi_WriteData(baseAddr, pState, *pState->pTxData);
                    pState->pTxData++;
                }
                else
                {
                    Spi_WriteData(baseAddr, pState, gSpi_DummyChar_d);
                }
                OSA_InterruptEnable();
            }
#elif FSL_FEATURE_SOC_DSPI_COUNT
            /* Fill the SPI Tx FIFO (if available) */
            DSPI_FlushFifo(baseAddr, TRUE, TRUE);
            DSPI_ClearStatusFlags(baseAddr, kDSPI_TxFifoFillRequestFlag);
            
            while( pState->txByteCount )
            {
                OSA_InterruptDisable();
                if( DSPI_GetStatusFlags(baseAddr) & kDSPI_TxFifoFillRequestFlag )
                {
                    --pState->txByteCount;
                    
                    if( pState->pTxData )
                    {
                        Spi_WriteData(baseAddr, pState, *pState->pTxData);
                        pState->pTxData++;
                    }
                    else
                    {
                        Spi_WriteData(baseAddr, pState, gSpi_DummyChar_d);
                    }
                }
                else
                {
                    temp = 1;
                }
                OSA_InterruptEnable();

                DSPI_ClearStatusFlags(baseAddr, kDSPI_TxFifoFillRequestFlag);

                if( temp )
                {
                    break;
                }
            }
#else /* FSL_FEATURE_SOC_DSPI_COUNT */
            
#if gSpi_UseFifo_c
            while( pState->txByteCount )
            {
#endif
                OSA_InterruptDisable();
                if(
#if gSpi_UseFifo_c
    #if  !defined FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT || (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT== 0)
                   !(SPI_GetStatusFlags(baseAddr) & SPI_S_TXFULLF_MASK)
    #else
                  /* Flexcomm SPI uses positive logic for TX_FIFO_NOT_FULL*/
                   SPI_GetStatusFlags(baseAddr) & SPI_FIFOSTAT_TXNOTFULL_MASK
    #endif
#else
                   SPI_GetStatusFlags(baseAddr) & kSPI_TxBufferEmptyFlag
#endif
                  )
                {
                    uint8_t data;
                    --pState->txByteCount;
                    
                    if( pState->pTxData )
                    {
                        data = *pState->pTxData;
                        ++pState->pTxData;
                    }
                    else
                    {
                        data = gSpi_DummyChar_d;
                    }
                    
                    Spi_WriteData(baseAddr, pState, data);
                }
#if gSpi_UseFifo_c
                else
                {
                    temp = 1;
                }
#else
                temp = temp; /* avoid compiler warnings */
#endif
                OSA_InterruptEnable();
                
#if gSpi_UseFifo_c
                if( temp )
                {
                    break;
                }
            } /* while( pState->txByteCount ) */
#endif /* gSpi_UseFifo_c */

#endif /* FSL_FEATURE_SOC_DSPI_COUNT */
            
            /* Enable SPI IRQ */
            Spi_SetIntState(baseAddr, TRUE);
        }
    }
    return status;
}
/*! *********************************************************************************
* \brief   This function transfer data asynchronously over SPI.
*          If a Callback function was registered, it will run at the end of the transfer
*
* \param[in] instance     The SPI module number
* \param[in] pTxData      Pointer to the data to be sent over SPI
Can be NULL if there is no data to TX (RX only)
* \param[in] pRxData      Pointer to a location where received data will be stored.
Can be NULL if no RX data is expected (TX only).
* \param[in] size         Number of bytes o be transfered over the SPI
*
* \return error code
*
********************************************************************************** */
spiStatus_t Spi_AsyncTransferSlave(uint32_t instance, uint8_t* pTxData, uint8_t* pRxData, uint16_t size)
{
    spiStatus_t status = spiSuccess;
#if FSL_FEATURE_SOC_DSPI_COUNT || FSL_FEATURE_SOC_LPSPI_COUNT
    SPI_Type *baseAddr = mSpiBase[instance];
#endif
    spiState_t* pState = gpaSpiState[instance];
    
    /* Parameter validation */
    if( ((NULL == pTxData) && (NULL == pRxData)) || (instance >= SPI_INSTANCE_COUNT) )
    {
        status = spiInvalidParameter;
    }
    else
    {
        /* Check if SPI is Busy */
        OSA_InterruptDisable();
        if( pState->trxByteCount )
        {
            OSA_InterruptEnable();
            status = spiBusy;
        }
        else
        {
            /* Mark SPI as busy*/
            pState->trxByteCount = size+1;
            pState->pTxData = pTxData;
            pState->pRxData = pRxData;
            pState->txByteCount = size +1;// +1 to accommodate the dummy char at the end of the transfer
#if FSL_FEATURE_SOC_DSPI_COUNT
            DSPI_FlushFifo(baseAddr, TRUE, FALSE);
#elif FSL_FEATURE_SOC_LPSPI_COUNT
            LPSPI_FlushFifo(baseAddr, TRUE, FALSE);
#endif
            OSA_InterruptEnable();
            
            /* Fill transfer parameters */
        }
    }
    return status;
}

/*! *********************************************************************************
* \brief   This function transfers a dummy byte over the SPI instance
*
* \param[in] instance     The SPI module number
* \param[in] pRxData      Pointer to a location where received data will be stored.
                          Can be NULL if no RX data is expected (TX only).
*
* \return error code
*
********************************************************************************** */
spiStatus_t Spi_SendDummyByte(uint32_t instance, uint8_t* pRxData)
{
    
    spiStatus_t status = spiSuccess;
    SPI_Type *baseAddr = mSpiBase[instance];
    spiState_t* pState = gpaSpiState[instance];
    
    OSA_InterruptDisable();
    if( pState->trxByteCount )
    {
        status = spiBusy;
    }
    else
    {
        /* Mark SPI as busy*/
        pState->trxByteCount = 1;
               
        /* Fill transfer parameters */
        pState->pTxData = &mSPI_dummyData;
        pState->pRxData = pRxData;
        pState->txByteCount = 1;
        
        Spi_WriteData(baseAddr, pState, gSpi_DummyChar_d);
    }
     OSA_InterruptEnable();
    return status;
}

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
********************************************************************************* */

/*! *********************************************************************************
* \brief   This is the ISR of the SPI module
*
********************************************************************************** */
static void SPIx_ISR(void)
{
    uint32_t    irq = __get_IPSR() - 16;
    spiState_t *pState;
    SPI_Type   *baseAddr;
    uint32_t    instance;
    uint32_t    data;
    uint32_t    flags;

    /* Get SPI instance */
    for( instance=0; instance<SPI_INSTANCE_COUNT; instance++ )
    {
        if( irq == mSpiIrqId[instance] )
        {
            pState = gpaSpiState[instance];
            baseAddr = mSpiBase[instance];
            
            /* Data was transfered */
            /* Clear status and read received byte */
            while(
#if FSL_FEATURE_SOC_LPSPI_COUNT
                  LPSPI_GetStatusFlags(baseAddr) & kLPSPI_RxDataReadyFlag
#elif FSL_FEATURE_SOC_DSPI_COUNT
                  DSPI_GetStatusFlags(baseAddr) & kDSPI_RxFifoDrainRequestFlag
#else
#if gSpi_UseFifo_c
    #if  !defined FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT || (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT== 0)
                  !(SPI_GetStatusFlags(baseAddr) & SPI_S_RFIFOEF_MASK)
    #else
                  /* Flexcomm uses positive logic for RX_FIFO_NOT_EMPTY*/
                  SPI_GetStatusFlags(baseAddr) & SPI_FIFOSTAT_RXNOTEMPTY_MASK
    #endif
#else
    #ifndef CPU_JN518X
                  SPI_GetStatusFlags(baseAddr) & kSPI_RxBufferFullFlag
    #else
                  SPI_GetStatusFlags(baseAddr) & kSPI_RxFullFlag
    #endif
#endif /* gSpi_UseFifo_c */
#endif
                 )
            {
                flags = 0;
                data  = Spi_ReadData(baseAddr);
#if FSL_FEATURE_SOC_LPSPI_COUNT
                LPSPI_ClearStatusFlags(baseAddr, kLPSPI_RxDataReadyFlag);
                   
#elif FSL_FEATURE_SOC_DSPI_COUNT
                DSPI_ClearStatusFlags(baseAddr, kDSPI_RxFifoDrainRequestFlag);
#else
#if gSpi_UseFifo_c
                SPI_ClearInterrupt(baseAddr, kSPI_RxNearFullClearInterrupt);
#endif
#endif
                
                if( pState->signalRxByte )
                {
                    flags |= gSPI_ByteRxFlag_d;
                }
                
                if( pState->pRxData )
                {
                    *pState->pRxData = data;
                    ++pState->pRxData;
                }
                
                if( pState->trxByteCount )
                {
                    if(Spi_IsMaster(baseAddr))
                    {
                        --pState->trxByteCount;
                    } 
                    else
                    {
                        if(pState->txByteCount)
                        {
                            Spi_FillTxFifoSlave(baseAddr,pState);
                        }
                        else
                        {
                            if(Spi_SlaveTransmitCompleted(baseAddr))
                            {
                                pState->trxByteCount = 0;
                            }
                        }
                    }
                    
                    /* Check if transfer is complete */
                    if( !pState->trxByteCount )
                    {
                        if( Spi_IsMaster(baseAddr) )
                        {
                            /* Disable SPI IRQ if continuous SPI RX is not enabled */
                            Spi_SetIntState(baseAddr, pState->signalRxByte);
                        }
                        
                        if( pState->pTxData )
                        {
                            pState->pTxData = NULL;
                            flags |= gSPI_TxEndFlag_d;
                        }
                        
                        if( pState->pRxData )
                        {
                            pState->pRxData = NULL;
                            flags |= gSPI_RxEndFlag_d;
                        }
                    }
                    /* Get next byte to transmit */
                    else 
                    {
                        if(Spi_IsMaster(baseAddr))
                        {
                            data = gSpi_DummyChar_d;
                            
                            if( pState->txByteCount )
                            {
                                --pState->txByteCount;
                                
                                if( pState->pTxData )
                                {
                                    data = *pState->pTxData;
                                    ++pState->pTxData;
                                }
                                
                                Spi_WriteData(baseAddr, pState, data);
                            }
                            else
                            {
                                Spi_WriteData(baseAddr, pState, data);
                            } 
                        }
                    }
                }
                
                /* Run callback function */
                if( (NULL != pState->cb) && flags )
                {
                    pState->cb(flags, pState);
                }
                
            }/* while(...) */
            break;
        }
    }/* for(...) */
    
}

/*! *********************************************************************************
* \brief   SPI driver helper function used to write data into SPI HW
*
* \param[in] baseAddr  pointer to the SPI registers
* \param[in] pState    pointer to the SPI driver data
* \param[in] data      data to be sent over SPI
*
********************************************************************************** */
static void Spi_WriteData(SPI_Type* baseAddr, spiState_t *pState, uint32_t data)
{
#if FSL_FEATURE_SOC_LPSPI_COUNT
    LPSPI_WriteData(baseAddr, data);
#elif FSL_FEATURE_SOC_DSPI_COUNT
    if( DSPI_IsMaster(baseAddr) )
    {
        ((uint8_t*)&pState->dspiCmd)[0] = (uint8_t)data;
        baseAddr->PUSHR = pState->dspiCmd;
    }
    else
    {
        DSPI_SlaveWriteData(baseAddr, data);
    }
    
#else
    SPI_WriteData(baseAddr, data
#if  (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT)
                  ,0x0U   //extra flags argument needed by QN spi driver
  #endif
                  );
#endif    
}
/*! *********************************************************************************
* \brief   SPI driver helper function used to write data into SPI HW
*
* \param[in] baseAddr  pointer to the SPI registers
* \param[in] pState    pointer to the SPI driver data
* \param[in] data      data to be sent over SPI
*
********************************************************************************** */
static bool_t Spi_SlaveTransmitCompleted(SPI_Type* baseAddr)
{
    bool_t status = FALSE;
#if FSL_FEATURE_SOC_LPSPI_COUNT
    if(LPSPI_GetStatusFlags(baseAddr) & kLPSPI_TxDataRequestFlag)
    {
        status = TRUE;
    }
#elif FSL_FEATURE_SOC_DSPI_COUNT
    //  if(DSPI_GetStatusFlags(baseAddr) & kDSPI_TxFifoUnderflowFlag)
    //  {
    //    status = TRUE;
    //  }
    
    if((DSPI_GetStatusFlags(baseAddr) & SPI_SR_TXCTR_MASK) == 0)
    {
        status = TRUE;
    }    
#else
    if(SPI_GetStatusFlags(baseAddr) & kSPI_TxBufferEmptyFlag )
    {
        status = TRUE;
    }
#endif
    return status;
}
/*! *********************************************************************************
* \brief   
*
* \param[in] baseAddr  pointer to the SPI registers
* \param[in] pState    pointer to the SPI driver data
*
********************************************************************************** */
static void Spi_FillTxFifoSlave(SPI_Type* baseAddr, spiState_t *pState)
{
#if FSL_FEATURE_SOC_LPSPI_COUNT
    while( pState->txByteCount  && (LPSPI_GetTxFifoCount(baseAddr) < LPSPI_GetTxFifoSize(baseAddr)) )
    {
        --pState->txByteCount;
        if( pState->pTxData && pState->txByteCount)
        {
            LPSPI_WriteData(baseAddr, *pState->pTxData);
            pState->pTxData++;
        }
        else
        {
            LPSPI_WriteData(baseAddr, gSpi_DummyChar_d);
            
        }
    }
    
#elif FSL_FEATURE_SOC_DSPI_COUNT
    /* Fill the SPI Tx FIFO (if available) */
    DSPI_ClearStatusFlags(baseAddr, kDSPI_TxFifoFillRequestFlag);
    
    while( pState->txByteCount  && ( DSPI_GetStatusFlags(baseAddr) & kDSPI_TxFifoFillRequestFlag ) )
    {
        --pState->txByteCount;
        if( pState->pTxData && pState->txByteCount)
        {
            DSPI_SlaveWriteData(baseAddr, *pState->pTxData);
            pState->pTxData++;
        }
        else
        {
            DSPI_SlaveWriteData(baseAddr, gSpi_DummyChar_d);
            
        }
        DSPI_ClearStatusFlags(baseAddr, kDSPI_TxFifoFillRequestFlag | kDSPI_TxFifoUnderflowFlag);
    }
#else /* FSL_FEATURE_SOC_DSPI_COUNT */
    
#if gSpi_UseFifo_c
    while( pState->txByteCount )
    {
#endif
        if(
#if gSpi_UseFifo_c
    #if  !defined FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT || (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT== 0)
           !(SPI_GetStatusFlags(baseAddr) & SPI_S_TXFULLF_MASK)
    #else
           /* Flexcomm uses positive logic for TX_FIFO_NOT_FULL*/
           SPI_GetStatusFlags(baseAddr) & SPI_FIFOSTAT_TXNOTFULL_MASK
    #endif 
#else
           SPI_GetStatusFlags(baseAddr) & kSPI_TxBufferEmptyFlag
#endif
          )
        {
            uint8_t data;
            --pState->txByteCount;
            if( pState->pTxData && pState->txByteCount)
            {
                data = *pState->pTxData;
                ++pState->pTxData;
            }
            else
            {
                data = gSpi_DummyChar_d;
            }
            
            Spi_WriteData(baseAddr, pState, data);
        }
#if gSpi_UseFifo_c
        else
        {
            break;
        }
#endif
        
        
#if gSpi_UseFifo_c
    } /* while( pState->txByteCount ) */
#endif /* gSpi_UseFifo_c */
    
#endif /* FSL_FEATURE_SOC_DSPI_COUNT */
    
}
/*! *********************************************************************************
* \brief   SPI driver helper function used to read data drom SPI HW
*
* \param[in] baseAddr  pointer to the SPI registers
*
* \return data from SPI HW.
*
********************************************************************************** */
static uint32_t Spi_ReadData(SPI_Type* baseAddr)
{
#if FSL_FEATURE_SOC_LPSPI_COUNT
    return LPSPI_ReadData(baseAddr);
#elif FSL_FEATURE_SOC_DSPI_COUNT
    return DSPI_ReadData(baseAddr);
#else
    return SPI_ReadData(baseAddr);
#endif    
}

/*! *********************************************************************************
* \brief   SPI driver helper function used to determine the SPI role (Master/Slave)
*
* \param[in] baseAddr  pointer to the SPI registers
*
* \return TRUE is SPI is configured as Master, else FALSE.
*
********************************************************************************** */
static bool_t Spi_IsMaster(SPI_Type* baseAddr)
{
#if FSL_FEATURE_SOC_LPSPI_COUNT
    return LPSPI_IsMaster(baseAddr);
#elif FSL_FEATURE_SOC_DSPI_COUNT
    return DSPI_IsMaster(baseAddr);
#elif defined (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT)
    return ((baseAddr->CFG & SPI_CFG_MASTER_MASK) == SPI_CFG_MASTER_MASK);
#else
    return (baseAddr->C1 & SPI_C1_MSTR_MASK) == SPI_C1_MSTR_MASK;
#endif
}

/*! *********************************************************************************
* \brief   SPI driver helper function used to enable/disable SPI interrupts
*
* \param[in] baseAddr  pointer to the SPI registers
* \param[in] state     the state of the SPI interrupts
*
********************************************************************************** */
static void Spi_SetIntState(SPI_Type* baseAddr, bool_t state)
{
#if FSL_FEATURE_SOC_LPSPI_COUNT
    if(state)
    {
        LPSPI_EnableInterrupts(baseAddr, kLPSPI_RxInterruptEnable);
    }
    else
    {
        LPSPI_DisableInterrupts(baseAddr, kLPSPI_RxInterruptEnable);
    }
#elif FSL_FEATURE_SOC_DSPI_COUNT
    if(state)
    {
        DSPI_EnableInterrupts(baseAddr, kDSPI_RxFifoDrainRequestInterruptEnable);
    }
    else
    {
        DSPI_DisableInterrupts(baseAddr, kDSPI_RxFifoDrainRequestInterruptEnable);
    }
#elif gSpi_UseFifo_c
    #if  (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT)
    if (state)
    {
        SPI_EnableInterrupts(baseAddr, SPI_FIFOSTAT_RXLVL_MASK);
    }
    else
    {
        SPI_DisableInterrupts(baseAddr, SPI_FIFOSTAT_RXLVL_MASK);
    }
    #else
    if (state)
    {
        SPI_EnableInterrupts(baseAddr, kSPI_RxFifoNearFullInterruptEnable);
    }
    else
    {
        SPI_DisableInterrupts(baseAddr, kSPI_RxFifoNearFullInterruptEnable);
    }
    #endif
#else /* gSpi_UseFifo_c */
    if (state)
    {
        SPI_EnableInterrupts(baseAddr, kSPI_RxFullAndModfInterruptEnable);
    }
    else
    {
    	SPI_DisableInterrupts(baseAddr, kSPI_RxFullAndModfInterruptEnable);
    }
    }
#endif
}

#if  (FSL_FEATURE_SOC_FLEXCOMM_SPI_COUNT)
static void SPI_EnableFIFO(SPI_Type *base, bool enable)
{
    /* This is also done by default by SPI_MasterInit() and SPI_SlaveInit().*/
    base->FIFOCFG |= SPI_FIFOCFG_ENABLETX_MASK | SPI_FIFOCFG_ENABLERX_MASK;
}

static inline void SPI_ClearInterrupt(SPI_Type *base, uint32_t interrupts)
{
    /* Only TXERR and RXERR can be cleared. Blissfully ignore any other bit! */
    base->FIFOSTAT |= (interrupts & (SPI_FIFOSTAT_TXERR_MASK | SPI_FIFOSTAT_RXERR_MASK));
}
#endif
