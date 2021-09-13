/****************************************************************************
 *
 * MODULE:             SMBus functions
 * DESCRIPTION:
 * A set of functions to communicate with devices on the SMBus
*/
/****************************************************************************
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

* Copyright NXP B.V. 2012-2019. All rights reserved
*
***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "dbg.h"
#include "fsl_i2c.h"
#include "app.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef DEBUG_SMBUS
#define TRACE_SMBUS TRUE
#else
#define TRACE_SMBUS FALSE
#endif

#define I2C_MASTER ((I2C_Type *)I2C_MASTER_BASE)

#define I2C_BAUDRATE 100000U

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

PUBLIC void vSMBusInit(void)
{

    i2c_master_config_t masterConfig;

    /*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kI2C_2PinOpenDrain;
     * masterConfig.baudRate_Bps = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    /* Initialise the I2C master peripheral */
    I2C_MasterInit(I2C_MASTER, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);

}


PUBLIC bool_t bSMBusWrite(uint8 u8Address, uint8 u8Command, uint8 u8Length, uint8* pu8Data)
{

    status_t reVal = kStatus_Fail;

    /* Send master blocking data to slave */
    DBG_vPrintf(TRACE_SMBUS, "SMBUS: Start\n");
    reVal = I2C_MasterStart(I2C_MASTER, u8Address, kI2C_Write);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    DBG_vPrintf(TRACE_SMBUS, "SMBUS: Write command=%d\n", u8Command);
    reVal = I2C_MasterWriteBlocking(I2C_MASTER, &u8Command, 1, kI2C_TransferNoStopFlag);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    if(u8Length > 0)
    {
        DBG_vPrintf(TRACE_SMBUS, "SMBUS: Write data Len=%d\n", u8Length);
        reVal = I2C_MasterWriteBlocking(I2C_MASTER, pu8Data, u8Length, kI2C_TransferNoStopFlag);
        if (reVal != kStatus_Success)
        {
            DBG_vPrintf(TRACE_SMBUS, "SMBUS: Write data failed with return val %d\n", reVal);
            return FALSE;
        }
    }

    DBG_vPrintf(TRACE_SMBUS, "SMBUS: Stop\n");
    reVal = I2C_MasterStop(I2C_MASTER);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    return TRUE;

}


PUBLIC bool_t bSMBusRandomRead(uint8 u8Address, uint8 u8Command, uint8 u8Length, uint8* pu8Data)
{

    status_t reVal = kStatus_Fail;

    /* Send master blocking data to slave */
    reVal = I2C_MasterStart(I2C_MASTER, u8Address, kI2C_Write);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    /* Send the command byte */
    reVal = I2C_MasterWriteBlocking(I2C_MASTER, &u8Command, 1, kI2C_TransferNoStopFlag);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    reVal = I2C_MasterStart(I2C_MASTER, u8Address, kI2C_Read);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    /* Read data */
    reVal = I2C_MasterReadBlocking(I2C_MASTER, pu8Data, u8Length, kI2C_TransferNoStopFlag);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    reVal = I2C_MasterStop(I2C_MASTER);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    return TRUE;

}


PUBLIC bool_t bSMBusSequentialRead(uint8 u8Address, uint8 u8Length, uint8* pu8Data)
{

    status_t reVal = kStatus_Fail;

    /* Send master blocking data to slave */
    reVal = I2C_MasterStart(I2C_MASTER, u8Address, kI2C_Read);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    /* Read data */
    reVal = I2C_MasterReadBlocking(I2C_MASTER, pu8Data, u8Length, kI2C_TransferNoStopFlag);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    reVal = I2C_MasterStop(I2C_MASTER);
    if (reVal != kStatus_Success)
    {
        return FALSE;
    }

    return TRUE;

}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
