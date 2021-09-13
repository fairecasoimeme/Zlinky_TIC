/****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016. All rights reserved
 ****************************************************************************/
#include "jendefs.h"
#include "string.h"
#include "Debug.h"
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
#include "AppHardwareApi.h"
#else
#if (defined __USE_LPCOPEN)
#include "jn518x.h"
#include "chip.h"
#else
#include "fsl_debug_console.h"
#endif
#endif

#ifdef DEBUG_ENABLE

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)

 #ifdef DEBUG_FIFO
  #define MAX_BUFFER 1500
 #else
  #define MAX_BUFFER 16
 #endif

 #if defined JENNIC_CHIP_FAMILY_JN514x
  #error Requires UART DMA
 #endif

 uint8 au8RxBuffer[16];
 uint8 au8TxBuffer[MAX_BUFFER];
#else
 static int iUartGetChar(void);

 #define LPC_USART LPC_USART0
#endif


#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
PUBLIC void vDebugFifoFlush(void)
{
}
#endif

void vDebugInit(void)
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    (void)bAHI_UartEnable(DEBUG_UART_NUMBER,
                          au8TxBuffer, MAX_BUFFER,
                          au8RxBuffer, 16);

    vAHI_UartSetBaudDivisor(DEBUG_UART_NUMBER, ((2000000 / 115200) + 1) / 2);
#else
#if (defined __USE_LPCOPEN)
	/* UART0 on GPIO 8 & 9 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, IOCON_FUNC2 | IOCON_DIGITAL_EN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, IOCON_FUNC2 | IOCON_DIGITAL_EN);

	/* Setup UART */
	Chip_UART_Init(LPC_USART);
	Chip_UART_ConfigData(LPC_USART, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);

	Chip_UART_SetBaud(LPC_USART, 115200);

	Chip_UART_Enable(LPC_USART);
	Chip_UART_TXEnable(LPC_USART);
#else
    /* Do nothing: assume board initialisation has been done */
#endif
#endif
}

/****************************************************************************
 *
 * NAME: vDebug
 *
 * DESCRIPTION:
 * Sends a string to UART0 using the peripheral API.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  pcMessage       R   Null-terminated message to send
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vDebug(char *pcMessage)
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    while (*pcMessage)
    {
 #ifndef DEBUG_FIFO
        while ((u8AHI_UartReadLineStatus(DEBUG_UART_NUMBER) & 0x60) != 0x60);
 #endif
        vAHI_UartWriteData(DEBUG_UART_NUMBER, *pcMessage);
        pcMessage++;
    }
#else
 #if (defined __USE_LPCOPEN)
  #ifdef DEBUG_FIFO
	Chip_UART_Send(LPC_USART, pcMessage, strlen(pcMessage));
  #else
    /* Blocking version */
    Chip_UART_SendBlocking(LPC_USART, pcMessage, strlen(pcMessage));
  #endif
 #else
    PRINTF(pcMessage);
 #endif
#endif

}

PUBLIC void vDebugVal(char *pcMessage, uint32 u32Value)
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    char c;

    while (*pcMessage)
    {
        c = *pcMessage;
        if (c == '%')
        {
            pcMessage++;
            vDebugHex(u32Value, *pcMessage - '0');
        }
        else
        {
#ifndef DEBUG_FIFO
            while ((u8AHI_UartReadLineStatus(DEBUG_UART_NUMBER) & 0x60) != 0x60);
#endif
            vAHI_UartWriteData(DEBUG_UART_NUMBER, c);
        }
        pcMessage++;
    }
#else
    char acMessage[200];
    char *pcPartMessage = acMessage;
    char *pcCurrent = acMessage;

    strcpy(acMessage, pcMessage);

    while (*pcCurrent)
    {
        if (*pcCurrent == '%')
        {
        	/* Found value print request, so terminate first part of string and then output it */
        	*pcCurrent = '\0';
        	vDebug(pcPartMessage);
        	/* Output value */
        	pcCurrent++;
            vDebugHex(u32Value, *pcCurrent - '0');
            /* Move part message pointer to just after value print request */
            pcPartMessage = pcCurrent + 1;
        }
        pcCurrent++;
    }

    /* Print remaining part of string */
    vDebug(pcPartMessage);
#endif
}

/****************************************************************************
 *
 * NAME: vDebugHex
 *
 * DESCRIPTION:
 * Creates a string containing the hex representation of a number then sends
 * it to the UART. String always has leading 0s.
 *
 * PARAMETERS:      Name        RW  Usage
 *                  u32Data     R   Number to convert
 *                  iSize       R   Number of digits to display
 *
 ****************************************************************************/
PUBLIC void vDebugHex(uint32 u32Data, int iSize)
{
    char acValue[9];
    char *pcString = acValue;
    uint8 u8Nybble;
    int i;

    for (i = (iSize << 2) - 4; i >= 0; i -= 4)
    {
        u8Nybble = (uint8)((u32Data >> i) & 0x0f);
        u8Nybble += 0x30;
        if (u8Nybble > 0x39)
            u8Nybble += 7;

        *pcString = u8Nybble;
        pcString++;
    }

    *pcString = 0;
    vDebug(acValue);
}

/****************************************************************************
 *
 * NAME: vDebugNonInt
 *
 * DESCRIPTION:
 * Blocking Print Routine.
 *
 * Sits in an endless loop afterwards.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  pcErrorMessage  R   Message to display
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vDebugNonInt(char *pcMessage)
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    while (*pcMessage)
    {
        while ((u8AHI_UartReadLineStatus(DEBUG_UART_NUMBER) & 0x60) != 0x60);
        vAHI_UartWriteData(DEBUG_UART_NUMBER, *pcMessage);
        pcMessage++;
    }
#else
 #if (defined __USE_LPCOPEN)
	Chip_UART_SendBlocking(LPC_USART, pcMessage, strlen(pcMessage));
 #else
    PRINTF(pcMessage);
 #endif
#endif
}

PUBLIC void vDebugValNonInt(char *pcMessage, uint32 u32Value)
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    char c;

    while (*pcMessage)
    {
        c = *pcMessage;
        if (c == '%')
        {
            pcMessage++;
            vDebugHexNonInt(u32Value, *pcMessage - '0');
        }
        else
        {
            while ((u8AHI_UartReadLineStatus(DEBUG_UART_NUMBER) & 0x60) != 0x60);
            vAHI_UartWriteData(DEBUG_UART_NUMBER, c);
        }
        pcMessage++;
    }
#else
    char acMessage[200];
    char *pcPartMessage = acMessage;
    char *pcCurrent = acMessage;

    strcpy(acMessage, pcMessage);

    while (*pcCurrent)
    {
        if (*pcCurrent == '%')
        {
        	/* Found value print request, so terminate first part of string and then output it */
        	*pcCurrent = '\0';
        	vDebugNonInt(pcPartMessage);
        	/* Output value */
        	pcCurrent++;
            vDebugHexNonInt(u32Value, *pcCurrent - '0');
            /* Move part message pointer to just after value print request */
            pcPartMessage = pcCurrent + 1;
        }
        pcCurrent++;
    }

    /* Print remaining part of string */
    vDebugNonInt(pcPartMessage);
#endif
}

/****************************************************************************
 *
 * NAME: vDebugHexNonInt
 *
 * DESCRIPTION:
 * Blocking Version Creates a string containing the hex representation of a
 * number then sends it to the UART. String always has leading 0s.
 *
 * PARAMETERS:      Name        RW  Usage
 *                  u32Data     R   Number to convert
 *                  iSize       R   Number of digits to display
 *
 ****************************************************************************/
PUBLIC void vDebugHexNonInt(uint32 u32Data, int iSize)
{
    char acValue[9];
    char *pcString = acValue;
    uint8 u8Nybble;
    int i;

    for (i = (iSize << 2) - 4; i >= 0; i -= 4)
    {
        u8Nybble = (uint8)((u32Data >> i) & 0x0f);
        u8Nybble += 0x30;
        if (u8Nybble > 0x39)
            u8Nybble += 7;

        *pcString = u8Nybble;
        pcString++;
    }

    *pcString = 0;
    vDebugNonInt(acValue);
}

PUBLIC void vDisplayHex(uint32 u32Data, int iSize)
{
    vDebugHex(u32Data, iSize);
}

/****************************************************************************
 *
 * NAME: vDisplayError
 *
 * DESCRIPTION:
 * Used to display fatal errors, by sending them to UART0 if this approach
 * has been enabled at compile time.
 *
 * Sits in an endless loop afterwards.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  pcErrorMessage  R   Message to display
 *                  u32Data         R   Data to display
 *
 * RETURNS:
 * void, never returns
 *
 ****************************************************************************/
PUBLIC void vDebugFatal(char *pcErrorMessage, uint32 u32Data)
{
    vDebug(pcErrorMessage);
    vDebugHex(u32Data, 8);

    /* Fatal error has occurred, so loop indefinitely */
    while (1);
}

PUBLIC void vUtils_Debug(char *pcMessage)
{
    vDebug(pcMessage);
}

PUBLIC void vUtils_DisplayMsg(char *pcMessage, uint32 u32Data)
{
    vDebug(pcMessage);
    vDebugHex(u32Data, 8);
    vDebug(" ");
}

PUBLIC void vUtils_DisplayHex(uint32 u32Data, int iSize)
{
    vDebugHex(u32Data, iSize);
}

PUBLIC void vUtils_Init(void)
{
}

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
#else
void vDebug_WaitForkey(void)
{
	int i = 0;
	while (iUartGetChar() == -1)
	{
		i++;
		if (i > 1000000)
		{
			i = 0;
			vDebug("\r\nWaiting for keypress...\r\n");
		}
	}

}

int iUartGetChar(void)
{
 #if (defined __USE_LPCOPEN)
	uint8_t u8Data;

	/* If a char is available, read & return it */
	if(Chip_UART_Read(LPC_USART, &u8Data, 1))
    {
        return u8Data;
    }
 #else
    int iData;
    
    iData = DbgConsole_Getchar();
    if (iData >= 0)
    {
        return iData;
    }
 #endif

    /* Otherwise, return -1 */
    return -1;
}

#endif
#endif
