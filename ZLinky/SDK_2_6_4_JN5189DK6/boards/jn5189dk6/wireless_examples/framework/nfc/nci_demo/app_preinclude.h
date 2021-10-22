/*! *********************************************************************************
 * \defgroup app
 * @{
 ********************************************************************************** */
/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _APP_PREINCLUDE_H_
#define _APP_PREINCLUDE_H_

/* Defines the number of available keys for the keyboard module */
#define gKBD_KeysCount_c                0
#define gKeyBoardSupported_d            0

#define gSupportBle                     0
#define gUartAppConsole_d               0

#define DEBUG_SERIAL_INTERFACE_INSTANCE 0
#define APP_SERIAL_INTERFACE_INSTANCE   1

#define PoolsDetails_c \
         _block_size_  8  _number_of_blocks_    20 _pool_id_(0) _eol_  \
         _block_size_  16  _number_of_blocks_    20 _pool_id_(0) _eol_  \
         _block_size_  32  _number_of_blocks_    20 _pool_id_(0) _eol_  

#endif /* _APP_PREINCLUDE_H_ */

/*! *********************************************************************************
 * @}
 ********************************************************************************** */
