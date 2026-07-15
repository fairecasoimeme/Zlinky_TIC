/* Host-test stub for the NXP USART/clock API referenced by app_uartlinky.c.
 * Only the symbols the file uses are declared; behaviour is a no-op.
 * NOT used by the firmware build. */
#ifndef HOST_STUB_FSL_USART_H
#define HOST_STUB_FSL_USART_H

#include "jendefs.h"

#define UARTLINKY        ((void *)0)
#define UARTLINKY_IRQ    (0)

/* status / config enums */
enum {
    kUSART_7BitsPerChar = 0,
    kUSART_ParityEven = 0,
    kUSART_RxFifo8 = 0,
    kUSART_RxFifoNotEmptyFlag = 1,
    kUSART_RxError = 2,
    kUSART_RxLevelInterruptEnable = 4,
    kUSART_RxErrorInterruptEnable = 8,
    kStatus_Success = 0,
    kCLOCK_Fro32M = 0,
};

typedef struct {
    uint32 baudRate_Bps;
    int    enableTx;
    int    enableRx;
    int    bitCountPerChar;
    int    parityMode;
    int    rxWatermark;
} usart_config_t;

void   USART_GetDefaultConfig(usart_config_t *config);
void   USART_Init(void *base, const usart_config_t *config, uint32 srcClock_Hz);
void   USART_Deinit(void *base);
int    USART_SetBaudRate(void *base, uint32 baudRate_Bps, uint32 srcClock_Hz);
uint32 USART_GetStatusFlags(void *base);
void   USART_ClearStatusFlags(void *base, uint32 mask);
uint8  USART_ReadByte(void *base);
uint32 CLOCK_GetFreq(int name);

#endif /* HOST_STUB_FSL_USART_H */
