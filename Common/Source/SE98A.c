/*****************************************************************************
 *
 * MODULE:          JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:       SE98A.c
 *
 * DESCRIPTION:     ZLO Demo: Temperature sensor driver
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
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
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "dbg.h"
#include "SMBus.h"
#include "SE98A.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_SE98A
    #define TRACE_SE98A TRUE
#else
    #define TRACE_SE98A FALSE
#endif

#define SE98A_ADDRESS                       (0x18)

#define SE98A_CAPABILITY_REG                (0x00)
#define SE98A_CONFIGURATION_REG             (0x01)
#define SE98A_UPPER_BOUNDRY_ALARM_TRIP_REG  (0x02)
#define SE98A_LOWER_BOUNDRY_ALARM_TRIP_REG  (0x03)
#define SE98A_CRITICAL_ALARM_TRIP_REG       (0x04)
#define SE98A_TEMPERATURE_REG               (0x05)
#define SE98A_MANUFACTURER_ID_REG           (0x06)
#define SE98A_DEVICE_ID_REG                 (0x07)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE bool_t bSE98A_Write(uint8 u8PointerRegister, uint8 u8Length, uint8 *pu8Data);
PRIVATE bool_t bSE98A_Read(uint8 u8PointerRegister, uint8 u8Length, uint8 *pu8Data);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: bSE98A_Init
 *
 * DESCRIPTION:
 * Initialises the SE98A sensor IC
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bSE98A_Init(void)
{

    bool_t bOk = TRUE;
    uint8 au8Buffer[2];

    DBG_vPrintf(TRACE_SE98A, "SE98A: Initialising I2C bus\n");

    /* run bus at 100KHz */
    vSMBusInit();

    DBG_vPrintf(TRACE_SE98A, "SE98A: Write config\n");
    au8Buffer[0] = 0;
    au8Buffer[1] = 0;
    bOk &= bSE98A_Write(SE98A_CONFIGURATION_REG, 2, au8Buffer);

    DBG_vPrintf(TRACE_SE98A, "SE98A: Init: Done\n");

    return(bOk);

}


/****************************************************************************
 *
 * NAME: bSE98A_PowerUp
 *
 * DESCRIPTION:
 * Places the SE98A in power up mode
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bSE98A_PowerUp(void)
{

    bool_t bOk = TRUE;
    uint8 au8Buffer[2];

    DBG_vPrintf(TRACE_SE98A, "SE98A: Power up\n");

    au8Buffer[0] = 0x00;
    au8Buffer[1] = 0x00;
    bOk &= bSE98A_Write(SE98A_CONFIGURATION_REG, 2, au8Buffer);

    return(bOk);

}


/****************************************************************************
 *
 * NAME: bSE98A_PowerDown
 *
 * DESCRIPTION:
 * Places the SE98A in power down mode
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bSE98A_PowerDown(void)
{

    bool_t bOk = TRUE;
    uint8 au8Buffer[2];

    DBG_vPrintf(TRACE_SE98A, "SE98A: Power down\n");

    au8Buffer[0] = 0x01;
    au8Buffer[1] = 0x00;
    bOk &= bSE98A_Write(SE98A_CONFIGURATION_REG, 2, au8Buffer);

    return(bOk);

}


/****************************************************************************
 *
 * NAME: bSE98A_ReadResult
 *
 * DESCRIPTION:
 * Reads the temperature.
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bSE98A_ReadResult(int16 *pi16Result)
{

    bool_t bOk = TRUE;
    uint8 au8Buffer[2] = {0};

    DBG_vPrintf(TRACE_SE98A, "SE98A: ReadResult\n");

    bOk &= bSE98A_Read(SE98A_TEMPERATURE_REG, 2, au8Buffer);

    DBG_vPrintf(TRACE_SE98A, "SE98A: Read=%d Got %02x:%02x\n", bOk, au8Buffer[0], au8Buffer[1]);

#if 0
    int16 i16Result;

    /* Calculate result in 0.1C but rounded to nearest 1C !?! */
    i16Result = (int16)(au8Buffer[0] << 8) | (int16)au8Buffer[1];
    i16Result &= ~0xe000;
    i16Result >>= 4;
    if(i16Result & 0x80)
    {
        i16Result |= 0xff00;
    }
    i16Result *= 10;
    *pi16Result = i16Result;

    DBG_vPrintf(TRACE_SE98A, "SE98A: Result=%d\n", i16Result);
#else
    /* Calculate result in 0.01C without rounding */
    *pi16Result = i16SE98A_ResultToHundrethsC(au8Buffer);
    DBG_vPrintf(TRACE_SE98A, "SE98A: Result=%d\n", *pi16Result);
#endif

    return bOk;

}

/****************************************************************************
 *
 * NAME: i16SE98A_ResultToHundrethsC
 *
 * DESCRIPTION:
 * Converts raw result to hundredths C
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC int16 i16SE98A_ResultToHundrethsC(uint8 *pu8Buffer)
{
//    DBG_vPrintf(TRUE, "\r\nSE98A: pu8Buffer (input) = 0x%02x%02x", pu8Buffer[0], pu8Buffer[1]);
    /* Convert result to int32 */
    int32 i32Result = ((int32) (pu8Buffer[0] & 0x1F) << 8) | (int32) pu8Buffer[1];
//    DBG_vPrintf(TRUE, "\r\nSE98A: i32Result (raw) = %d, 0x%04x", i32Result, i32Result);
    /* Correct negative numbers */
    if (pu8Buffer[0] & 0x10) i32Result |= 0xFFFFE000;
//    DBG_vPrintf(TRUE, "\r\nSE98A: i32Result (signed) = %d, 0x%04x", i32Result, i32Result);
    /* Convert result to 0.01C values */
    i32Result *= 800;
//    DBG_vPrintf(TRUE, "\r\nSE98A: i32Result (scaled) = %d, 0x%04x", i32Result, i32Result);
    i32Result >>= 7;
//    DBG_vPrintf(TRUE, "\r\nSE98A: i32Result (shifted) = %d, 0x%04x", i32Result, i32Result);

    return ((int16) i32Result);
}

/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: bSE98A_Write
 *
 * DESCRIPTION:
 * Writes a value to the SE98A
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PRIVATE bool_t bSE98A_Write(uint8 u8PointerRegister, uint8 u8Length, uint8 *pu8Data)
{

    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_SE98A, "SE98A: Write PtrReg=%02x Len=%d\n", u8PointerRegister, u8Length);
    bOk &= bSMBusWrite(SE98A_ADDRESS, u8PointerRegister, u8Length, pu8Data);

    return(bOk);

}


/****************************************************************************
 *
 * NAME: bSE98A_Read
 *
 * DESCRIPTION:
 * Reads a value from the SE98A
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PRIVATE bool_t bSE98A_Read(uint8 u8PointerRegister, uint8 u8Length, uint8 *pu8Data)
{

    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_SE98A, "SE98A: Read PtrReg=%02x Len=%d\n", u8PointerRegister, u8Length);

    bOk &= bSMBusWrite(SE98A_ADDRESS, u8PointerRegister, 0, NULL);

    bOk &= bSMBusSequentialRead(SE98A_ADDRESS, 2, pu8Data);

    return(bOk);

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
