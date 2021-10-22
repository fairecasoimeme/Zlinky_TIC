/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spifi_dma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.spifi_dma"
#endif

/*<! Structure definition for spifi_dma_private_handle_t. The structure is private. */
typedef struct _spifi_dma_private_handle
{
    SPIFI_Type *base;
    spifi_dma_handle_t *handle;
} spifi_dma_private_handle_t;

/* SPIFI DMA transfer handle. */
enum _spifi_dma_tansfer_states
{
    kSPIFI_Idle,   /* TX idle. */
    kSPIFI_BusBusy /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*<! Private handle only used for internally. */
static spifi_dma_private_handle_t s_dmaPrivateHandle[FSL_FEATURE_SOC_SPIFI_COUNT][2];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief SPIFI DMA send finished callback function.
 *
 * This function is called when SPIFI DMA send finished. It disables the SPIFI
 * TX DMA request and sends @ref kStatus_SPIFI_TxIdle to SPIFI callback.
 *
 * @param handle The DMA handle.
 * @param param Callback function parameter.
 */
static void SPIFI_SendDMACallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t intmode);

/*!
 * @brief SPIFI DMA receive finished callback function.
 *
 * This function is called when SPIFI DMA receive finished. It disables the SPIFI
 * RX DMA request and sends @ref kStatus_SPIFI_RxIdle to SPIFI callback.
 *
 * @param handle The DMA handle.
 * @param param Callback function parameter.
 */
static void SPIFI_ReceiveDMACallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t intmode);

/*******************************************************************************
 * Code
 ******************************************************************************/

static void SPIFI_SendDMACallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t intmode)
{
    spifi_dma_private_handle_t *spifiPrivateHandle = (spifi_dma_private_handle_t *)param;

    /* Avoid the warning for unused variables. */
    handle  = handle;
    intmode = intmode;

    if (transferDone)
    {
        SPIFI_TransferAbortSendDMA(spifiPrivateHandle->base, spifiPrivateHandle->handle);

        if (spifiPrivateHandle->handle->callback)
        {
            spifiPrivateHandle->handle->callback(spifiPrivateHandle->base, spifiPrivateHandle->handle,
                                                 kStatus_SPIFI_Idle, spifiPrivateHandle->handle->userData);
        }
    }
}

static void SPIFI_ReceiveDMACallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t intmode)
{
    spifi_dma_private_handle_t *spifiPrivateHandle = (spifi_dma_private_handle_t *)param;

    /* Avoid warning for unused parameters. */
    handle  = handle;
    intmode = intmode;

    if (transferDone)
    {
        /* Disable transfer. */
        SPIFI_TransferAbortReceiveDMA(spifiPrivateHandle->base, spifiPrivateHandle->handle);

        if (spifiPrivateHandle->handle->callback)
        {
            spifiPrivateHandle->handle->callback(spifiPrivateHandle->base, spifiPrivateHandle->handle,
                                                 kStatus_SPIFI_Idle, spifiPrivateHandle->handle->userData);
        }
    }
}

/*!
 * brief Initializes the SPIFI handle for send which is used in transactional functions and set the callback.
 *
 * param base SPIFI peripheral base address
 * param handle Pointer to spifi_dma_handle_t structure
 * param callback SPIFI callback, NULL means no callback.
 * param userData User callback function data.
 * param rxDmaHandle User requested DMA handle for DMA transfer
 */
void SPIFI_TransferTxCreateHandleDMA(SPIFI_Type *base,
                                     spifi_dma_handle_t *handle,
                                     spifi_dma_callback_t callback,
                                     void *userData,
                                     dma_handle_t *dmaHandle)
{
    assert(handle);

    uint32_t instance = SPIFI_GetInstance(base);

    s_dmaPrivateHandle[instance][0].base   = base;
    s_dmaPrivateHandle[instance][0].handle = handle;

    memset(handle, 0, sizeof(*handle));

    handle->state     = kSPIFI_Idle;
    handle->dmaHandle = dmaHandle;

    handle->callback = callback;
    handle->userData = userData;

    /* Configure TX dma callback */
    DMA_SetCallback(handle->dmaHandle, SPIFI_SendDMACallback, &s_dmaPrivateHandle[instance][0]);
}

/*!
 * brief Initializes the SPIFI handle for receive which is used in transactional functions and set the callback.
 *
 * param base SPIFI peripheral base address
 * param handle Pointer to spifi_dma_handle_t structure
 * param callback SPIFI callback, NULL means no callback.
 * param userData User callback function data.
 * param rxDmaHandle User requested DMA handle for DMA transfer
 */
void SPIFI_TransferRxCreateHandleDMA(SPIFI_Type *base,
                                     spifi_dma_handle_t *handle,
                                     spifi_dma_callback_t callback,
                                     void *userData,
                                     dma_handle_t *dmaHandle)
{
    assert(handle);

    uint32_t instance = SPIFI_GetInstance(base);

    s_dmaPrivateHandle[instance][1].base   = base;
    s_dmaPrivateHandle[instance][1].handle = handle;

    memset(handle, 0, sizeof(*handle));

    handle->state     = kSPIFI_Idle;
    handle->dmaHandle = dmaHandle;

    handle->callback = callback;
    handle->userData = userData;

    /* Configure RX dma callback */
    DMA_SetCallback(handle->dmaHandle, SPIFI_ReceiveDMACallback, &s_dmaPrivateHandle[instance][1]);
}

/*!
 * brief Transfers SPIFI data using an DMA non-blocking method.
 *
 * This function writes data to the SPIFI transmit FIFO. This function is non-blocking.
 * param base Pointer to QuadSPI Type.
 * param handle Pointer to spifi_dma_handle_t structure
 * param xfer SPIFI transfer structure.
 */
status_t SPIFI_TransferSendDMA(SPIFI_Type *base, spifi_dma_handle_t *handle, spifi_transfer_t *xfer)
{
    assert(handle && (handle->dmaHandle));

    dma_transfer_config_t xferConfig;
    status_t status;

    /* If previous TX not finished. */
    if (kSPIFI_BusBusy == handle->state)
    {
        status = kStatus_SPIFI_Busy;
    }
    else
    {
        handle->state = kSPIFI_BusBusy;

        /* Prepare transfer. */
        DMA_PrepareTransfer(&xferConfig, xfer->data, (void *)SPIFI_GetDataRegisterAddress(base), sizeof(uint32_t),
                            xfer->dataSize, kDMA_MemoryToPeripheral, NULL);

        /* Submit transfer. */
        DMA_SubmitTransfer(handle->dmaHandle, &xferConfig);
        DMA_StartTransfer(handle->dmaHandle);

        /* Enable SPIFI TX DMA. */
        SPIFI_EnableDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Receives data using an DMA non-blocking method.
 *
 * This function receive data from the SPIFI receive buffer/FIFO. This function is non-blocking.
 * param base Pointer to QuadSPI Type.
 * param handle Pointer to spifi_dma_handle_t structure
 * param xfer SPIFI transfer structure.
 */
status_t SPIFI_TransferReceiveDMA(SPIFI_Type *base, spifi_dma_handle_t *handle, spifi_transfer_t *xfer)
{
    assert(handle && (handle->dmaHandle));

    dma_transfer_config_t xferConfig;
    status_t status;

    /* If previous TX not finished. */
    if (kSPIFI_BusBusy == handle->state)
    {
        status = kStatus_SPIFI_Busy;
    }
    else
    {
        handle->state = kSPIFI_BusBusy;

        /* Prepare transfer. */
        DMA_PrepareTransfer(&xferConfig, (void *)SPIFI_GetDataRegisterAddress(base), xfer->data, sizeof(uint32_t),
                            xfer->dataSize, kDMA_PeripheralToMemory, NULL);

        /* Submit transfer. */
        DMA_SubmitTransfer(handle->dmaHandle, &xferConfig);
        DMA_StartTransfer(handle->dmaHandle);

        /* Enable SPIFI TX DMA. */
        SPIFI_EnableDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the sent data using DMA.
 *
 * This function aborts the sent data using DMA.
 *
 * param base SPIFI peripheral base address.
 * param handle Pointer to spifi_dma_handle_t structure
 */
void SPIFI_TransferAbortSendDMA(SPIFI_Type *base, spifi_dma_handle_t *handle)
{
    assert(handle && (handle->dmaHandle));

    /* Disable SPIFI TX DMA. */
    SPIFI_EnableDMA(base, false);

    /* Stop transfer. */
    DMA_AbortTransfer(handle->dmaHandle);

    handle->state = kSPIFI_Idle;
}

/*!
 * brief Aborts the receive data using DMA.
 *
 * This function abort receive data which using DMA.
 *
 * param base SPIFI peripheral base address.
 * param handle Pointer to spifi_dma_handle_t structure
 */
void SPIFI_TransferAbortReceiveDMA(SPIFI_Type *base, spifi_dma_handle_t *handle)
{
    assert(handle && (handle->dmaHandle));

    /* Disable SPIFI RX DMA. */
    SPIFI_EnableDMA(base, false);

    /* Stop transfer. */
    DMA_AbortTransfer(handle->dmaHandle);

    handle->state = kSPIFI_Idle;
}

/*!
 * brief Gets the transferred counts of send.
 *
 * param base Pointer to QuadSPI Type.
 * param handle Pointer to spifi_dma_handle_t structure.
 * param count Bytes sent.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t SPIFI_TransferGetSendCountDMA(SPIFI_Type *base, spifi_dma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSPIFI_BusBusy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->transferSize - DMA_GetRemainingBytes(handle->dmaHandle->base, handle->dmaHandle->channel);
    }

    return status;
}

/*!
 * brief Gets the status of the receive transfer.
 *
 * param base Pointer to QuadSPI Type.
 * param handle Pointer to spifi_dma_handle_t structure
 * param count Bytes received.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t SPIFI_TransferGetReceiveCountDMA(SPIFI_Type *base, spifi_dma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSPIFI_BusBusy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->transferSize - DMA_GetRemainingBytes(handle->dmaHandle->base, handle->dmaHandle->channel);
    }

    return status;
}
