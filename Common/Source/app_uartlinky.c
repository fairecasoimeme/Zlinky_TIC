/*****************************************************************************
 *
 * MODULE:
 *
 * COMPONENT:
 *
 * AUTHOR:
 *
 * DESCRIPTION:
 *
 * $HeadURL: $
 *
 * $Revision: $
 *
 * $LastChangedBy: $
 *
 * $LastChangedDate: $
 *
 * $Id:  $
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
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
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/



#include <jendefs.h>
#include "board.h"
#include "app.h"
#include "fsl_usart.h"
#include "usart_dma_rxbuffer.h"
#include "app_uartlinky.h"
#include "dbg.h"
#include "portmacro.h"

#if (ZIGBEE_USE_FRAMEWORK == 0)
#include <stdarg.h>
#include "stdlib.h"
#include "app_common.h"
#include "ZQueue.h"
#else
#include "app_common.h"
#include "ZQueue.h"

#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/




/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/****************************************************************************
 *
 * NAME: vUART_Init
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PUBLIC void UARTLINKY_vInit(void)
{

    usart_config_t config;

    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = 9600U; //1200U;
    config.enableTx = false;
    config.enableRx = true;
    config.bitCountPerChar = kUSART_7BitsPerChar;
    config.parityMode      = kUSART_ParityEven;
    config.rxWatermark = kUSART_RxFifo8;



    USART_Init(UARTLINKY, &config, CLOCK_GetFreq(kCLOCK_Fro32M));

    /* Enable RX interrupt. */
   /* USART_EnableInterrupts(UARTLINKY, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable );
    EnableIRQ(UARTLINKY_IRQ);*/



}

 PUBLIC void UARTLINKY_vDeInit(void)
 {

	 USART_Deinit(UARTLINKY);
	// USART_DisableInterrupts(UARTLINKY, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);
 }

/****************************************************************************
 *
 * NAME: vUART_SetBuadRate
 *
 * DESCRIPTION:
 *
 * PARAMETERS: Name        RW  Usage
 *
 * RETURNS:
 *
 ****************************************************************************/

PUBLIC void UARTLINKY_vSetBaudRate ( uint32    u32BaudRate )
{

    int result;

    /* setup baudrate */
    result = USART_SetBaudRate(UARTLINKY, u32BaudRate, CLOCK_GetFreq(kCLOCK_Fro32M));
    if (kStatus_Success != result)
    {
        DBG_vPrintf(1,"\r\nFailed to set UART speed ");
    }

}


/****************************************************************************
 *
 * NAME: APP_isrUart
 *
 * DESCRIPTION: Handle interrupts from uart
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/


void APP_isrUartLinky ( void )
{
    if ((kUSART_RxFifoNotEmptyFlag | kUSART_RxError ) & USART_GetStatusFlags(UARTLINKY)) {
            USART_ClearStatusFlags(UARTLINKY, kUSART_RxError);
            uint8 u8Byte = USART_ReadByte(UARTLINKY);
            ZQ_bQueueSend(&APP_msgSerialRx, &u8Byte);
    }
}
PUBLIC bool bSL_ReadMessageStandard(uint16 u16MaxLength, uint8 *command, uint8 *date, uint8 *value,uint8 *error, uint8 *posFinal,uint8 u8Data, bool *lf)
{
	static uint8 u8CRC;
	static uint8 u8TmpCRCHorodate;
	static uint8 u8TmpCRC;
	static uint16 i;
	static uint8 pos;

	switch(u8Data)
	{

		case 0x0A:

			u8TmpCRC=0;
			u8TmpCRCHorodate=0;
			memset(command,0,0);
			memset(date,0,0);
			memset(value,0,0);
			command=0;
			value=0;
			date=0;
			*error=0;
			*posFinal=0;
			pos=0;
			i=0;
			break;
		case 0x09:
			if (*lf)
			{
				pos++;
				*posFinal = pos;
				i=0;
				u8TmpCRCHorodate+=0x09;
				u8TmpCRC=u8TmpCRCHorodate;
			}else{
				*error=1;
				DBG_vPrintf(1, "\r\nError sperator");
				return(FALSE);
			}
			break;
		case 0x0D:
			if (*lf)
			{
				*lf=FALSE;
				if (pos==2)
				{
					//Calcul CRC

					u8TmpCRC=(u8TmpCRC & 0x3F)+0x20;

					if ((memcmp(command,"SMAX",4)!=0) &&
							(memcmp(command,"CCA",3)!=0) &&
							(memcmp(command,"UMOY",4)!=0) &&
							(memcmp(command,"DMP",3)!=0) &&
							(memcmp(command,"FPM",3)!=0)
						)
					{
						if (u8TmpCRC==u8CRC)
						{
							memcpy(value,date,256);
							return(TRUE);
						}else{
							DBG_vPrintf(1, "\r\ncommand : %s Error CRC : tmp=%c - reçu=%c",command, u8TmpCRC,u8CRC);
							*error=1;
							return(FALSE);
						}
					}else{
						*error=1; // Datas miss
						DBG_vPrintf(1, "\r\nDatas miss");
						return(FALSE);
					}

				}else if(pos==3)
				{
					//Calcul CRC
					u8TmpCRCHorodate=(u8TmpCRCHorodate & 0x3F)+0x20;
					if (u8TmpCRCHorodate==u8CRC)
					{
						//Specif for DATE
						if (memcmp(command,"DATE",4)==0)
						{
							memcpy(value,date,256);
						}
						return(TRUE);
					}else{
						*error=1;
						DBG_vPrintf(1, "\r\nHORODATE - command : %s Error CRC : tmp=%c - reçu=%c",command, u8TmpCRCHorodate,u8CRC);
						return(FALSE);

					}
				}else if(pos==0){
					*error=1;
					DBG_vPrintf(1, "\r\nincorrect position : %d",pos);
					return(FALSE);
				}
			}else{
				*error=1;
				DBG_vPrintf(1, "\r\nLF Missing ");
				return(FALSE);
			}
			break;
		case 0x02:
		case 0x03:
		case 0x04:
			break;
		default:
			if (*lf)
			{
				if (pos==0)
				{
					command[i++] = u8Data;
					u8TmpCRCHorodate+=u8Data;
					u8TmpCRC=u8TmpCRCHorodate;
					command[i]='\0';


				}else if (pos==1)
				{
					date[i++] = u8Data;
					u8TmpCRCHorodate+=u8Data;
					u8TmpCRC=u8TmpCRCHorodate;
					date[i]='\0';


				}else if (pos==2)
				{
					value[i++] = u8Data;
					u8TmpCRCHorodate+=u8Data;
					value[i]='\0';
					u8CRC = u8Data;

				}else{
					u8CRC = u8Data;
				}
			}else{
				*error=1;
				DBG_vPrintf(1, "\r\nLF Missing : No valid data ");
				return(FALSE);
			}
			break;
	}
	*error=0;
	return(FALSE);
}

PUBLIC bool bSL_ReadMessageHisto(uint16 u16MaxLength, uint8 *command, uint8 *value,uint8 *error,uint8 u8Data, bool *lf)
{

    static uint8 u8CRC;
    static uint8 u8TmpCRC;
    static uint16 i;
    static uint8 pos;

    switch(u8Data)
    {
    	case 0x0A:
    		u8TmpCRC=0;
    		memset(command,0,0);
    		memset(value,0,0);
    		command=0;
    		value=0;
    		pos=0;
    		i=0;
    		*error=0;
    		break;
    	case 0x0D:
    		if (*lf)
    		{
    			*lf=false;
				if (pos>=2)
				{
					//Calcul CRC
					u8TmpCRC=(u8TmpCRC & 0x3F)+0x20;
					if (u8TmpCRC==u8CRC)
					{
						return(TRUE);
					}else{
						*error=1;
						DBG_vPrintf(1, "\r\nError CRC : tmp=%d - reçu=%d", u8TmpCRC,u8CRC);
						return(FALSE);
					}
				}else if(pos==0){
					*error=1;
					DBG_vPrintf(1, "\r\nEtiquettes / données / séparateur non attendu nb != 2 --> %d", pos);
					return(FALSE);
				}
    		}else{
    			*error=1;
				DBG_vPrintf(1, "\r\nLF Missing ");
				return(FALSE);
    		}
    		break;
    	case 0x20:
    		if (*lf)
    		{
				pos++;
				i=0;
				if (pos<2)
					u8TmpCRC+=0x20;
				else{
					u8CRC = u8Data;
				}

    		}
    		break;
    	case 0x02:
		case 0x03:
		case 0x04:
			break;
    	default:
    		if (*lf)
    		{
    			if (pos==0)
				{
					command[i++] = u8Data;
					u8TmpCRC+=u8Data;
					command[i]='\0';

				}else if (pos==1)
				{
					value[i++] = u8Data;
					u8TmpCRC+=u8Data;
					value[i]='\0';

				}else{
					u8CRC = u8Data;

				}
    		}else{
				*error=1;
				DBG_vPrintf(1, "\r\nLF Missing : No valid data ");
				return(FALSE);
			}
    		break;
    }
    *error = 0;
    return(FALSE);
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
