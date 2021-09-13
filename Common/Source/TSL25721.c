/*****************************************************************************
 *
 * MODULE:          Light Sensor Driver
 *
 * COMPONENT:       TSL25721.c
 *
 * DESCRIPTION:     ZLO Demo: Light sensor driver
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
 * Copyright NXP B.V. 2016-2019. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "dbg.h"
#include "TSL25721.h"
#include "SMBus.h"
#include "fsl_debug_console.h"



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_TSL25721
    #define TRACE_TSL25721   TRUE
#else
    #define TRACE_TSL25721   FALSE
#endif

#define GA          1  // Assume no glass attenuation

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
PRIVATE uint16 AGAINx = 1;   // reset value
PRIVATE uint32 u32CPL;
PRIVATE uint32 ATIME_ms;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: bTSL25721_Init
 *
 * DESCRIPTION:
 * Initialises the TSL25721 ambient light sensor IC
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bTSL25721_Init(void)
{
    bool_t bOk = TRUE;
    uint8 u8Data = 0x80;

    DBG_vPrintf(TRACE_TSL25721, "\r\nTSL25721: Initialising I2C bus");

    /* run bus at 100KHz */
    //vSMBusInit();

    DBG_vPrintf(TRACE_TSL25721, "\r\nTSL25721: Init: Power Up");

   // bOk &= bSMBusWrite(TSL25721_ADDRESS, TSL25721_COMMAND, 0, NULL);
   // u8Data = 1;
   // bOk &= bSMBusWrite(TSL25721_ADDRESS, TSL25721_COMMAND | TSL25721_ENABLE, 1, &u8Data);

    //bTSL25721_SetATime(0xC0);  // default to 175ms (gives max count of 65535)

    DBG_vPrintf(TRACE_TSL25721, "\r\nTSL25721: Init: Done");

    return(bOk);
}


/****************************************************************************
 *
 * NAME: bTSL25721_PowerDown
 *
 * DESCRIPTION:
 * Places the TSL25721 ambient light sensor IC in power down mode
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bTSL25721_PowerDown(void)
{

    bool_t bOk = TRUE;
    uint8 u8Data = 0;

    DBG_vPrintf(TRACE_TSL25721, "\r\nTSL25721: Power down");
   // bOk &= bSMBusWrite(TSL25721_ADDRESS, TSL25721_COMMAND | TSL25721_ENABLE,  1, &u8Data);

    return(bOk);

}

PUBLIC bool_t bTSL25721_SetATime(uint8 u8Value)
{
     bool_t bOk = TRUE;
     //bOk &= bSMBusWrite(TSL25721_ADDRESS, TSL25721_COMMAND | TSL25721_ATIME,  1, &u8Value);

     ATIME_ms = (256 - u8Value) * 273;   // 100 times bigger to avoid floating point maths
     u32CPL = ((ATIME_ms * AGAINx) / (GA * 60));

     return(bOk);
}


/****************************************************************************
 *
 * NAME: bTSL25721_StartRead
 *
 * DESCRIPTION:
 * Starts the TSL25721 ambient light sensor IC sampling. There are two
 * channels, one for the full light range and one for infra-red only.
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bTSL25721_StartRead()
{

    bool_t bOk = TRUE;
    uint8 u8Data = 3;   // Set AEN


    DBG_vPrintf(TRACE_TSL25721, "\r\nTSL25721: Read:");

    //bOk &= bSMBusWrite(TSL25721_ADDRESS, TSL25721_COMMAND | TSL25721_ENABLE, 1, &u8Data);

    return(bOk);

}


/****************************************************************************
 *
 * NAME: u16TSL25721_ReadResult
 *
 * DESCRIPTION:
 * Reads a light level from the sensor and converts it to a reasonably
 * linear value.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  u8Channel       R   channel to sample
 *
 * RETURNS:
 * Value in range 0-4015, 0 is darkest
 *
 ****************************************************************************/
PUBLIC uint16 u16TSL25721_ReadResult(void)
{
    uint16 C0DATA = 0;
    uint16 C1DATA = 0;
    uint8 u8Result = 0;

    int iTimeout = 0;

    DBG_vPrintf(TRACE_TSL25721, "\r\nTSL25721: Read result");


    do{
       // bSMBusRandomRead(TSL25721_ADDRESS, TSL25721_COMMAND | TSL25721_STATUS,  1, &u8Result);
        iTimeout++;
    }while (((u8Result & 1) == 0) && (iTimeout < TSL25721_TIMEOUT));

    if (iTimeout == TSL25721_TIMEOUT){
        return(65534);
    }

   // bSMBusRandomRead(TSL25721_ADDRESS, TSL25721_COMMAND | TSL257212_CMD_AUTOINC | TSL25721_READ_ADC_0_LO,  2, (uint8 *) &C0DATA);
    DBG_vPrintf(TRACE_TSL25721,"C0DATA:%4x\r",C0DATA);

   // bSMBusRandomRead(TSL25721_ADDRESS, TSL25721_COMMAND | TSL257212_CMD_AUTOINC | TSL25721_READ_ADC_1_LO,  2, (uint8 *) &C1DATA);
    DBG_vPrintf(TRACE_TSL25721,"C1DATA:%4x\r",C1DATA);


    // Replace code with 32 bits integers to avoid using float library and maths
    //int16 Lux1 = ( C0DATA - (1.87 * C1DATA)) / CPL;
    //int16 Lux2 = ((0.63 * C0DATA) - C1DATA) / CPL;
    //int16 Lux = MAX(Lux1, Lux2);
    //return( Lux);

    int32 i32C0DATA = (int32)C0DATA;
    int32 i32C1DATA = (int32)C1DATA;
    int32 i32Lux1 = ((i32C0DATA*100) - (187 * i32C1DATA)) / u32CPL;
    int32 i32Lux2 = ((63 * i32C0DATA) - (i32C1DATA*100)) / u32CPL;
    int32 i32Lux = MAX(i32Lux1, i32Lux2);
    i32Lux = MAX(i32Lux,0);

   return((uint16) i32Lux);

}

/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
