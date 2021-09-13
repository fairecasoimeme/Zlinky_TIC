/*****************************************************************************
 *
 * MODULE:          JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:       TSL2550.c
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
#include "TSL2550.h"
#include "SMBus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_TSL2550
    #define TRACE_TSL2550   TRUE
#else
    #define TRACE_TSL2550   FALSE
#endif

#define TSL2550_ADDRESS             0x39

#define TSL2550_POWER_DOWN          0x00
#define TSL2550_POWER_UP            0x03
#define TSL2550_MODE_STANDARD       0x18
#define TSL2550_MODE_EXTENDED       0x1d
#define TSL2550_READ_ADC_0          0x43
#define TSL2550_READ_ADC_1          0x83

#define TSL2550_TIMEOUT             300000

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
/****************************************************************************
 *
 * NAME: bTSL2550_Init
 *
 * DESCRIPTION:
 * Initialises the TSL2550 ambient light sensor IC
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bTSL2550_Init(void)
{

    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Initialising I2C bus\n");

    /* run bus at 100KHz */
    vSMBusInit();

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Init: Power Up\n");
    bOk &= bSMBusWrite(TSL2550_ADDRESS, TSL2550_POWER_UP, 0, NULL);

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Init: Extended mode\n");
    bOk &= bSMBusWrite(TSL2550_ADDRESS, TSL2550_MODE_EXTENDED, 0, NULL);

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Init: Done\n");

    return(bOk);
}


/****************************************************************************
 *
 * NAME: bTSL2550_PowerDown
 *
 * DESCRIPTION:
 * Places the TSL2550 ambient light sensor IC in power down mode
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bTSL2550_PowerDown(void)
{

    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Power down\n");
    bOk &= bSMBusWrite(TSL2550_ADDRESS, TSL2550_POWER_DOWN, 0, NULL);

    return(bOk);
}


/****************************************************************************
 *
 * NAME: bTSL2550_StartRead
 *
 * DESCRIPTION:
 * Starts the TSL2550 ambient light sensor IC sampling. There are two
 * channels, one for the full light range and one for infra-red only.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  u8Channel       R   channel to sample
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bTSL2550_StartRead(uint8 u8Channel)
{

    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Read:\n");
    if(u8Channel == TSL2550_CHANNEL_0){

        DBG_vPrintf(TRACE_TSL2550, "TSL2550: Read ADC 0\n");

        bOk &= bSMBusWrite(TSL2550_ADDRESS, TSL2550_READ_ADC_0, 0, NULL);

    } else {

        DBG_vPrintf(TRACE_TSL2550, "TSL2550: Read ADC 1\n");

        bOk &= bSMBusWrite(TSL2550_ADDRESS, TSL2550_READ_ADC_1, 0, NULL);

    }

    return(bOk);

}


/****************************************************************************
 *
 * NAME: u16TSL2550_ReadResult
 *
 * DESCRIPTION:
 * Reads a light level from the sensor and converts it to a reasonably
 * linear value.
 *
 * RETURNS:
 * Value in range 0-4015, 0 is darkest
 *
 ****************************************************************************/
PUBLIC uint16 u16TSL2550_ReadResult(void)
{
    uint16 u16Result = 99;

    static const uint16 au16ChordValue[8] = {0, 16, 49, 115, 247, 511, 1039, 2095};

    uint8 u8ChordIndex;
    uint16 u16ChordValue;
    uint16 u16StepValue;
    uint16 u16StepNumber;
    uint8 u8Result = 0;
    int iTimeout = 0;

    DBG_vPrintf(TRACE_TSL2550, "TSL2550: Read result\n");

    do{
        bSMBusSequentialRead(TSL2550_ADDRESS, 1, &u8Result);
        iTimeout++;
    }while (((u8Result & 128) == 0) && (iTimeout < TSL2550_TIMEOUT));

    if (iTimeout == TSL2550_TIMEOUT){
        return(65534);
    }

    /* Work out value, using formula in TSL2550 data sheet */
    u8ChordIndex = (u8Result & 0x70) >> 4;
    u16ChordValue = au16ChordValue[u8ChordIndex];
    u16StepValue = 1 << u8ChordIndex;
    u16StepNumber = u8Result & 0x0f;
    u16Result = u16ChordValue + u16StepValue * u16StepNumber;

    return(u16Result);

}

/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
