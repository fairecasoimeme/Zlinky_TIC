/****************************************************************************
 *
 * MODULE:             PCA9634 LED Driver header file
 *
 * COMPONENT:          $RCSfile: $
 *
 * VERSION:            $Name:  $
 *
 * REVISION:           $Revision:  $
 *
 * DATED:              $Date:  $
 *
 * STATUS:             $State: Exp $
 *
 * AUTHOR:             Matt Gillespie
 *
 * DESCRIPTION:
 * A Driver for the NXP PCA9634 LED Driver (header file)
 *
 * CHANGE HISTORY:
 *
 * $Log:  $
 *
 *
 *
 * LAST MODIFIED BY:   $Author: $
 *                     $Modtime: $
 *
 ****************************************************************************
 *
 * This software is owned by Jennic and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on Jennic products. You, and any third parties must reproduce
 * the copyright and warranty notice and any other legend of ownership on
 * each copy or partial copy of the software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". JENNIC MAKES NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * ACCURACY OR LACK OF NEGLIGENCE. JENNIC SHALL NOT, IN ANY CIRCUMSTANCES,
 * BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.
 *
 * Copyright Jennic Ltd 2009. All rights reserved
 *
 ***************************************************************************/

#ifndef  PCA9634_H_INCLUDED
#define  PCA9634_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define PCA9634_ADDRESS			0x15
#define PCA9634_RESET_ADDRESS	0x03

#define PCA9634_MODE1			0x00
#define PCA9634_MODE2			0x01
#define PCA9634_PWM0			0x02
#define PCA9634_PWM1			0x03
#define PCA9634_PWM2			0x04
#define PCA9634_PWM3			0x05
#define PCA9634_PWM4			0x06
#define PCA9634_PWM5			0x07
#define PCA9634_PWM6			0x08
#define PCA9634_PWM7			0x09
#define PCA9634_GRPPWM			0x0A
#define PCA9634_GRPFREQ			0x0B
#define PCA9634_LEDOUT0			0x0C
#define PCA9634_LEDOUT1			0x0D
#define PCA9634_SUBADR1			0x0E
#define PCA9634_SUBADR2			0x0F
#define PCA9634_SUBADR3			0x10
#define PCA9634_ALLCALLADR		0x11

#define PCA9634_TIMEOUT     		300000

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
PUBLIC bool_t bPCA9634_Init(void);
PUBLIC bool_t bPCA9634_SetChannelLevel(uint8 u8Channel, uint8 u8Level);
PUBLIC bool_t bPCA9634_SetGroupLevel(uint8 u8Level);
PUBLIC bool_t bPCA9634_WriteRegister(uint8 u8Command, uint8 u8Data);
PUBLIC uint8 u8PCA9634_ReadRegister(uint8 u8Command);


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* PCA9634_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

