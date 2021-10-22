/*
** ###################################################################
**     Processors:          K32W041-061
**
**     Compiler:            GNU C Compiler
**     Reference manual:    K32W0x1 Series Reference Manual
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright  2019 NXP
**     All rights reserved.
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2018-2019 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/

INCLUDE app_memory_split.ld


/******************* Map of K32W061 FLASH ***********************************


             0x000a_0000    m_flash_end
    - - - +---------------+ - - - - - - - -
          |   _ _ _ _ _   |
          | Flash config  |
   8.5k   |   - - - - -   |
          |               |
          |  RESERVED     |  0x9de00
    - - - +---------------+ - - - - - - - -
          |               | NV_STORAGE_START_ADDRESS
   31.5k  |               |
          |   NVM_region  |
          |               |
          |               | NV_STORAGE_END_ADDRESS
    - - - +---------------+ - - - - - - - -
          |               | m_blob4_end
          |    blob 4     |
          |               | m_blob4_start
    - - - +---------------+ - - - - - - - -
          |               | m_blob3_end
          |    blob 3     |
          |               | m_blob3_start
    - - - +---------------+ - - - - - - - -
          |               | m_blob2_end
          |    blob 2     |
          |               | m_blob2_start
    - - - +---------------+ - - - - - - - -
          |               | m_blob1_end
          |    blob 1     |
          |               | m_blob1_start
    - - - +---------------+ - - - - - - - -
          |               | m_sota_end
          |  SOTA update  |
          |               | m_sota_start
    - - - +---------------+ - - - - - - - -
          |               | m_SSBL_end
          |               |
   8k     |      SSBL     |
          |               |
          |               | m_SSBL_start = m_flash_start
    - - - +---------------+ - - - - - - - -
             0x0000_0000

 *****************************************************************************/

/******************* Map of K32W061 RAM for SOTA ***********************************
Only for RAM0

    - - - +---------------+ - - - - - - - -
          |               | m_blob4_RAM0_end
          |               |
          |    blob 4     |
          |               |
          |               | m_blob4_RAM0_start
    - - - +---------------+ - - - - - - - -
          |               | m_blob3_RAM0_end
          |               |
          |    blob 3     |
          |               |
          |               | m_blob3_RAM0_start
    - - - +---------------+ - - - - - - - -
          |               | m_blob2_RAM0_end
          |               |
          |    blob 2     |
          |               |
          |               | m_blob2_RAM0_start
    - - - +---------------+ - - - - - - - -
          |               | m_blob1_RAM0_end
          |               |
          |    blob 1     |
          |               |
          |               | m_blob1_RAM0_start
    - - - +---------------+ - - - - - - - -
             m_RAM0_start


 *****************************************************************************/

/* Entry Point */
ENTRY(ResetISR)

/*** flash memory characteristics ***/
m_flash_start   = 0x00000000;
m_flash_end     = 0x0009FFFF;
m_flash_size    = 0x000A0000;
m_sector_size   = 512;

/* RAM characteristics */
m_RAM0_start   = 0x4000400;
m_RAM0_size    = 0x15c00; /* 87K bytes (alias RAM) */
m_RAM1_start   = 0x4020000;
m_RAM1_size    = 0x10000; /* 64K */

/****************************************************
 *******         User Defines                 *******
 ****************************************************/
UseNVMLink              = DEFINED(gUseNVMLink_d)            ? gUseNVMLink_d             : 0;
NVMSectorCountLink      = DEFINED(gNVMSectorCountLink_d)    ? gNVMSectorCountLink_d     : ( DEFINED(gUseNVMLink_d) ? gUseNVMLink_d*63 : 0 );
ram_vector_table        = DEFINED(__ram_vector_table__)     ? __ram_vector_table__      : 0;
UseInternalStorageLink  = DEFINED(gUseInternalStorageLink_d)? gUseInternalStorageLink_d : 0;

m_SSBL_size             = 0x2000;
m_blob1_size            = DEFINED(m_blob1_size)             ? m_blob1_size              : 0x7D000; /* 512000 bytes */
m_blob2_size            = DEFINED(m_blob2_size)             ? m_blob2_size              : 0x0;
m_blob3_size            = DEFINED(m_blob3_size)             ? m_blob3_size              : 0x0;
m_blob4_size            = DEFINED(m_blob4_size)             ? m_blob4_size              : 0x0;

m_blob1_RAM0_size       = DEFINED(m_blob1_RAM0_size)        ? m_blob1_RAM0_size         : m_RAM0_size;
m_blob2_RAM0_size       = DEFINED(m_blob2_RAM0_size)        ? m_blob2_RAM0_size         : 0x0;
m_blob3_RAM0_size       = DEFINED(m_blob3_RAM0_size)        ? m_blob3_RAM0_size         : 0x0;
m_blob4_RAM0_size       = DEFINED(m_blob4_RAM0_size)        ? m_blob4_RAM0_size         : 0x0;

HEAP_SIZE               = DEFINED(__heap_size__)            ? __heap_size__             : 0;
STACK_SIZE              = DEFINED(__stack_size__)           ? __stack_size__            : (0x1000-32);
vector_table_size       = 0x120;

/*
 * stack size for the boot rom during warm boot and application
 * 256 is sufficient (pwrm_test) but keep it large to 1024
 */
BOOT_RESUME_STACK_SIZE = 1024;


/****************************************************
 ******          Flash regions sizes           ******
 ****************************************************/
NV_STORAGE_SIZE         = NVMSectorCountLink * m_sector_size;
NV_STORAGE_MAX_SECTORS  = NVMSectorCountLink;
NV_STORAGE_SECTOR_SIZE = m_sector_size;


/****************************************************
 ******        Flash Regions, low to high      ******
 ****************************************************/
m_ssbl_position0        = 0;
m_blob_position1        = 1;
m_blob_position2        = 2;
m_blob_position3        = 3;
m_blob_position4        = 4;

m_SSBL_start              = m_flash_start; /* + m_bootloader_size , can be zero */
m_SSBL_end                = m_SSBL_start + m_SSBL_size - 1;

m_sota_size                = MAX(MAX(MAX(m_blob1_size, m_blob2_size), m_blob3_size), m_blob4_size);

m_blob1_RAM0_start         = m_RAM0_start;
m_blob1_RAM1_start         = m_RAM1_start;
m_blob1_RAM1_size          = m_RAM1_size;

m_blob2_RAM0_start         = m_blob1_RAM0_start + m_blob1_RAM0_size;
m_blob2_RAM1_start         = m_RAM1_start;
m_blob2_RAM1_size          = m_RAM1_size - m_blob1_RAM1_size;

m_blob3_RAM0_start         = m_blob2_RAM0_start + m_blob2_RAM0_size;
m_blob3_RAM1_start         = m_RAM1_start;
m_blob3_RAM1_size          = m_RAM1_size - m_blob1_RAM1_size;

m_blob4_RAM0_start         = m_blob3_RAM0_start + m_blob3_RAM0_size;
m_blob4_RAM1_start         = m_RAM1_start;
m_blob4_RAM1_size          = m_RAM1_size - m_blob1_RAM1_size;

m_sota_start              = m_SSBL_end + 1;
m_sota_end                = m_sota_start + m_sota_size - 1;

m_blob1_start             = m_sota_end + 1;
m_blob1_end               = m_blob1_start + m_blob1_size - 1;

m_blob2_start             = m_blob1_end + 1;
m_blob2_end               = m_blob2_start + m_blob2_size - 1;

m_blob3_start             = m_blob2_end + 1;
m_blob3_end               = m_blob3_start + m_blob3_size - 1;

m_blob4_start             = m_blob3_end + 1;
m_blob4_end               = m_blob4_start + m_blob4_size - 1;

/* APP id of SSBL is 0x0 */
m_app_start               = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? m_SSBL_start : (__blob_position__==m_blob_position1) ? m_blob1_start : (__blob_position__==m_blob_position2) ? m_blob2_start : (__blob_position__==m_blob_position3) ? m_blob3_start : (__blob_position__==m_blob_position4) ? m_blob4_start : -1 : -2;
m_app_end                 = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? m_SSBL_end : (__blob_position__ ==m_blob_position1) ? m_blob1_end : (__blob_position__ ==m_blob_position2) ? m_blob2_end : (__blob_position__ ==m_blob_position3) ? m_blob3_end : (__blob_position__ ==m_blob_position4) ? m_blob4_end : -1 : -2;

m_app_size   = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? m_SSBL_size : (__blob_position__==m_blob_position1) ? m_blob1_size : (__blob_position__==m_blob_position2) ? m_blob2_size : (__blob_position__==m_blob_position3) ? m_blob3_size : (__blob_position__==m_blob_position4) ? m_blob4_size : 0 : 0;

ASSERT(!(m_app_start==-1), "__blob_position__ is unknown ")
ASSERT(!(m_app_start==-2), "__blob_position__ is undefined has to be defined for SOTA it should be the blob ID")

m_interrupts_start        = m_app_start;
m_interrupts_end          = m_interrupts_start + vector_table_size - 1;

m_blob_ram0_start         = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? -3 : (__blob_position__==m_blob_position1) ? m_blob1_RAM0_start : (__blob_position__==m_blob_position2) ? m_blob2_RAM0_start : (__blob_position__==m_blob_position3) ? m_blob3_RAM0_start : (__blob_position__==m_blob_position4) ? m_blob4_RAM0_start : -1 : -2;
m_blob_ram0_size          = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? -3 : (__blob_position__==m_blob_position1) ? m_blob1_RAM0_size : (__blob_position__==m_blob_position2) ? m_blob2_RAM0_size : (__blob_position__==m_blob_position3) ? m_blob3_RAM0_size : (__blob_position__==m_blob_position4) ? m_blob4_RAM0_size : -1 : -2;

m_blob_ram1_start         = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? -3 : (__blob_position__==m_blob_position1) ? m_blob1_RAM1_start : (__blob_position__==m_blob_position2) ? m_blob2_RAM1_start : (__blob_position__==m_blob_position3) ? m_blob3_RAM1_start : (__blob_position__==m_blob_position4) ? m_blob4_RAM1_start : -1 : -2;
m_blob_ram1_size          = DEFINED(__blob_position__) ? (__blob_position__==0x0) ? -3 : (__blob_position__==m_blob_position1) ? m_blob1_RAM1_size : (__blob_position__==m_blob_position2) ? m_blob2_RAM1_size : (__blob_position__==m_blob_position3) ? m_blob3_RAM1_size : (__blob_position__==m_blob_position4) ? m_blob4_RAM1_size : -1 : -2;

ASSERT(!(m_blob_ram0_start==-1), "__blob_position__ is unknown")
ASSERT(!(m_blob_ram0_start==-2), "__blob_position__ is undefined")
ASSERT(!(m_blob_ram0_start==-3), "SSBL cannot be built with this linker file (RAM section not defined for SSBL)")

/****************************************************
 *****        Flash Regions, high to low       ******
 ****************************************************/
NV_STORAGE_START_ADDRESS        = m_flash_size - 17 * m_sector_size - 1;
NV_STORAGE_END_ADDRESS    = NV_STORAGE_START_ADDRESS - NV_STORAGE_SIZE + 1;

/* For SOTA the internal storage section is the OTA blob section */
INT_STORAGE_START         = m_sota_end;
INT_STORAGE_END           = m_sota_start;
INT_STORAGE_SIZE          = INT_STORAGE_START - INT_STORAGE_END;

/* Other Defines */
INT_STORAGE_SECTOR_SIZE   = m_sector_size;
M_VECTOR_RAM_SIZE         = DEFINED(__ram_vector_table__) ? vector_table_size : 0x0;

/* Check that all blobs fit in the flash */
ASSERT(((m_sota_size + m_blob1_size + m_blob2_size + m_blob3_size + m_blob4_size) <= (NV_STORAGE_END_ADDRESS - m_SSBL_end)), "Error all blobs + ota blob section cannot be stored in the flash")

/* Check the ram sum */
ASSERT(((m_blob1_RAM0_size + m_blob2_RAM0_size + m_blob3_RAM0_size + m_blob4_RAM0_size + STACK_SIZE) <= m_RAM0_size), "Error all blobs RAM usage cannot be stored in RAM0")

/* Specify the memory areas */
MEMORY
{
  /* Define each memory region */
    IntFlash        (RX) : ORIGIN = m_app_start,     LENGTH = m_app_size

    RAM0 (rwx)    : ORIGIN = m_blob_ram0_start, LENGTH = m_blob_ram0_size
    RAM1 (rwx)    : ORIGIN = m_blob_ram1_start, LENGTH = m_blob_ram1_size
}

  /* Define a symbol for the top of each memory region */
  __base_Flash640 = 0x00000000  ; /* Flash640 */
  __base_Flash = 0x0 ; /* Flash */
  __top_Flash640 = 0x0 + 0xa0000 ; /* 640K bytes */
  __top_Flash = 0x0 + 0xa0000 ; /* 640K bytes */
  __base_RAM0 = 0x4000400  ; /* RAM0 */
  __base_RAM = 0x4000400 ; /* RAM */
  __top_RAM0 = 0x4000400 + 0x15c00 ; /* 87K bytes */
  __top_RAM = 0x4000400 + 0x15c00 ; /* 87K bytes */
  __base_RAM1 = 0x4020000  ; /* RAM1 */
  __base_RAM2 = 0x4020000 ; /* RAM2 */
  __top_RAM1 = 0x4020000 + 0x10000 ; /* 64K bytes */
  __top_RAM2 = 0x4020000 + 0x10000 ; /* 64K bytes */
_zigbee_hdr     = m_app_start + 0x150; /* Fixed offset as expected by JET tool */

SECTIONS
{
    /* MAIN TEXT SECTION */
    .m_interrupts : ALIGN(4)
    {
        _flash_start = ABSOLUTE(.);
        _flash_beg = ABSOLUTE(.);

        FILL(0xff)
        __vectors_start__ = m_app_start ;
        __VECTOR_TABLE = .;
        KEEP(*(.isr_vector))
        FILL(0xff)
        . = ALIGN (0x10);
    } > IntFlash

    /* MAIN TEXT SECTION */
    .data_bss : ALIGN(10)
    {
        /* Global Section Table: now in 2 parts to allow ZigBee headers to 
           stay in place. Part 2 in is headerB */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        __data_discard_section_table = .;
        LONG(LOADADDR(.data_discard));
        LONG(    ADDR(.data_discard));
        LONG(_edata_discard - _data_discard);
        __data_section_table_end = .;
        /* End of Global Section Table part 1 */

        FILL(0xff)
        . = ALIGN (0x10);
    } > IntFlash

    .ro_nonce _zigbee_hdr : ALIGN(0x10)
    {
        _FlsNonceStart = ABSOLUTE(.);
        *(.ro_nonce) /* nonce value is 16 bytes.*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > IntFlash

    .ro_ota_header : ALIGN(0x10)
    {
        _enc_start = ABSOLUTE(.);
        _enc_offset = (_enc_start & 0x0000000F);
        _FlsOtaHeader = ABSOLUTE(.);
        *(.ro_ota_header) /* Ota Header 69 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > IntFlash

    .ro_se_lnkKey (ALIGN((. - _enc_offset), 16) + _enc_offset):
    {
        _FlsLinkKey = ABSOLUTE(.);
        *(.ro_se_lnkKey)  /* Link Key 16 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > IntFlash
    
    .headerB : ALIGN(4)
    {
        FILL(0xff)
        /* Global Section Table part 2 */
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        __bss_discard_section_table = .;
        LONG(    ADDR(.bss_discard));
        LONG(_ebss_discard - _bss_discard);
        __bss_section_table_end = .;
        /* End of Global Section Table part 2 */
    } > IntFlash

    .filler :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(0x40);
    } > IntFlash

    .text : ALIGN(0x40)
    {
        FILL(0xff)

       *(.after_vectors*)
       *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > IntFlash

    .NVM_TABLE :
    {
      . = ALIGN(4);
      PROVIDE(__start_NVM_TABLE = .);
      KEEP(*(.NVM_TABLE));
      PROVIDE(__stop_NVM_TABLE = .);
      . = ALIGN(4);
    } > IntFlash

    .VERSION_TAGS :
    {
        . = ALIGN(4);
        PROVIDE(__start_VERSION_TAGS = .);
        KEEP(*(.VERSION_TAGS));
        PROVIDE(__stop_VERSION_TAGS = .);
        . = ALIGN(4);
    } > IntFlash

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
       FILL(0xff)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > IntFlash
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
       FILL(0xff)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > IntFlash
    __exidx_end = .;

    _etext = .;

    /* RAM1/RAM2 (different names for same thing) SECTION */
    /* RAM1 contents are specified before RAM0 as they have specific input
       sections and we do not want the RAM0 wildcards to catch them */
    /* DATA section for RAM1 */
    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$RAM1)
        *(.data.$RAM2*)
        *(.data.$RAM1*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
     } > RAM1 AT>IntFlash

    .interrupts_ram :
    {
        . = ALIGN(4);
        __VECTOR_RAM__ = .;
        __interrupts_ram_start__ = .;   /* Create a global symbol at data start */
        *(.m_interrupts_ram)            /* This is a user defined section */
        . += M_VECTOR_RAM_SIZE;
        . = ALIGN(4);
        __interrupts_ram_end__ = .;     /* Define a global symbol at data end */
    } > RAM0

    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RAM0

    /* Data discard section */
    .data_discard _end_fw_retention : ALIGN(4)
    {
       FILL(0xff)
       _data_discard = . ;
       *(.data.discard.stack)
       *(.data.discard.app)
       . = ALIGN(4) ;
       _edata_discard = . ;
       *(.data.discard_noinit.stack)
       *(.data.discard_noinit.app)
       . = ALIGN(4) ;
       _edata_discard_noinit = . ;
    } > RAM0 AT>IntFlash

    /* This filler goes between the .data_discard and .data sections, to avoid
       gaps in the binary which cause the debug program operation to break */
    .fillerB :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(16);
    } > IntFlash

    /* BSS discard section */
    .bss_discard _edata_discard_noinit : ALIGN(4)
    {
        _bss_discard = .;
        *(.bss.discard.stack)
        *(.bss.discard.app)
        . = ALIGN(4) ;
        _ebss_discard = .;
        *(.bss.discard_noinit.stack)
        *(.bss.discard_noinit.app)
        . = ALIGN(4) ;
        _ebss_discard_noinit = .;
    } > RAM0

    /* Main DATA section (RAM0) */
    .data _end_uninit_RESERVED : ALIGN(4)
    {
       FILL(0xff)
       *(.ll_exchange_mem)   /* TO fix : create a specific section for *(.ll_exchange_mem) before the . data section */
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > RAM0 AT>IntFlash

    __VECTOR_RAM = __VECTOR_RAM__;
    __RAM_VECTOR_TABLE_SIZE_BYTES = DEFINED(__ram_vector_table__) ? (__interrupts_ram_end__ - __interrupts_ram_start__) : 0x0;

    /* BSS section for RAM1 */
    .bss_RAM2 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       . = ALIGN(4);
       *(.bss_RAM2)
       *(.bss.$RAM2*)
       *(.bss.$RAM1*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
    } > RAM1

    /* MAIN BSS SECTION */
    .bss (NOLOAD) : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        *(g_u32NwkFrameCounter)
        . = ALIGN(4) ;
        _ebss = .;

        PROVIDE(end = .);
    } > RAM0

    /* BSS section for MAC buffers */
    .bss_MAC (NOLOAD) : ALIGN(4)
    {
       /* MAC buffer section: must be within 128kB block. __mac_buffer_base is
          defined further down to be on 128kB alignment */
        __mac_buffer_start = .;
       *(.mac_buffer)

        . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
    } > RAM0

    /* HEAP */
    .heap (NOLOAD): ALIGN(4)
    {
        _heap = .;
        . += HEAP_SIZE;
        . = ALIGN(4) ;
        _end_heap = .;
    } > RAM0

    /* NOINIT section for RAM1 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM2*)
       *(.noinit.$RAM1*)
       . = ALIGN(4) ;
    } > RAM1

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*)
        . = ALIGN(4) ;
        _end_noinit = .;
    } > RAM0

    /* end of firmware RAM to be retained in power down mode */
    _end_fw_retention = .;

      /* stack for rom boot during warm resume */
    .boot_resume_stack _ebss_discard_noinit (NOLOAD): ALIGN(4)
    {
        _boot_resume_stack = .;
        *(.boot_resume_stack*)
        . += BOOT_RESUME_STACK_SIZE;
        . = ALIGN(4) ;
        _end_boot_resume_stack = .;
    } > RAM0

    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : _heap);
    PROVIDE(_pvHeapLimit = _pvHeapStart + (HEAP_SIZE));
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RAM0 - 32);
    PROVIDE(__mac_buffer_base = (__mac_buffer_start & 0xfffe0000));

    __StackLimit = _vStackTop - STACK_SIZE;
    ASSERT(__StackLimit >= _end_boot_resume_stack, "Possible stack corruption with data/bss/boot_stack")
}

GROUP(libcr_nohost.a libcr_c.a libcr_eabihelpers.a)
