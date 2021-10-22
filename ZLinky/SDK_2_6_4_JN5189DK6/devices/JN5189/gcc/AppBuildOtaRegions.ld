/*****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5189].
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
 * Copyright NXP B.V. 2019. All rights reserved
 *
 ***************************************************************************/

/* Memory region definitions for various memory maps. Modify as required */
FLASH_SIZE_SSBL         = 0x04000;
FLASH_SIZE_ZIGBEE_STACK = 0x4f000;
FLASH_SIZE_BLE_STACK    = 0x40000;

/* Inferred information */
FLASH_BASE_SSBL         = FLASH_BASE_DEFAULT;
FLASH_BASE_ZIGBEE_STACK = FLASH_BASE_SSBL + FLASH_SIZE_SSBL;
FLASH_BASE_BLE_STACK    = FLASH_BASE_ZIGBEE_STACK + FLASH_SIZE_ZIGBEE_STACK;

/* Selection based on build parameters */
MEM_FLASH_BASE = DEFINED(MEM_FLASH_BASE)     ? MEM_FLASH_BASE :
                 DEFINED(BUILD_SSBL)         ? FLASH_BASE_SSBL : 
                 DEFINED(BUILD_ZIGBEE_STACK) ? FLASH_BASE_ZIGBEE_STACK : 
                 DEFINED(BUILD_BLE_STACK)    ? FLASH_BASE_BLE_STACK : 
                 FLASH_BASE_DEFAULT;

MEM_FLASH_SIZE = DEFINED(MEM_FLASH_SIZE)     ? MEM_FLASH_SIZE :
                 DEFINED(BUILD_SSBL)         ? FLASH_SIZE_SSBL : 
                 DEFINED(BUILD_ZIGBEE_STACK) ? FLASH_SIZE_ZIGBEE_STACK : 
                 DEFINED(BUILD_BLE_STACK)    ? FLASH_SIZE_BLE_STACK : 
                 FLASH_SIZE_DEFAULT;

MEM_RAM0_BASE  = DEFINED(MEM_RAM0_BASE)  ? MEM_RAM0_BASE  : RAM0_BASE_DEFAULT;
MEM_RAM0_SIZE  = DEFINED(MEM_RAM0_SIZE)  ? MEM_RAM0_SIZE  : RAM0_SIZE_DEFAULT;
