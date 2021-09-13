/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2019
 * Generated linker script file for JN5189
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on 05-Jan-2017 14:07:01
 */

INCLUDE JN5189_rom.ld

/*
 * stack size for the boot rom during warm boot and application
 * 256 is sufficient (pwrm_test) but keep it large to 1024
 */
BOOT_RESUME_STACK_SIZE = 1024;

/* Set Stack size to 4K minus 32Bytes reserved for ROM code at end of BANK7 so
   the stack fits in BANK7. In practice the active stack size at the time of
   going to sleep is more important than the total available stack size */
STACK_SIZE             = (4096 - 32);

FLASH_BASE_DEFAULT = 0x0000000;
FLASH_SIZE_DEFAULT = 0x0050000;
RAM0_BASE_DEFAULT  = 0x4000400;
RAM0_SIZE_DEFAULT  = 0x0015c00;

INCLUDE AppBuildOtaRegions.ld

MEMORY
{
  /* Define each memory region. RAM0 definition leaves the first 1kB for the
     boot code */
  Flash320 (rx) : ORIGIN = MEM_FLASH_BASE, LENGTH = MEM_FLASH_SIZE /* 320K bytes (alias Flash) */  
  RAM0 (rwx)    : ORIGIN = MEM_RAM0_BASE,  LENGTH = MEM_RAM0_SIZE  /* 87K bytes (alias RAM) */  
}

/* Define a symbol for the top of each memory region */
__base_Flash320 = MEM_FLASH_BASE; /* Flash320 */  
__base_Flash    = MEM_FLASH_BASE; /* Flash */  
__top_Flash320  = MEM_FLASH_BASE + MEM_FLASH_SIZE; /* 320K bytes */  
__top_Flash     = MEM_FLASH_BASE + MEM_FLASH_SIZE; /* 320K bytes */  
__base_RAM0     = MEM_RAM0_BASE; /* RAM0 */  
__base_RAM      = MEM_RAM0_BASE; /* RAM */  
__top_RAM0      = MEM_RAM0_BASE + MEM_RAM0_SIZE; /* 87K bytes */  
__top_RAM       = MEM_RAM0_BASE + MEM_RAM0_SIZE; /* 87K bytes */  

HEAP_SIZE = DEFINED(HEAP_SIZE) ? HEAP_SIZE : 0x2F4;

/*** flash memory characteristics definitions required for OTA ***/
m_flash_start   = 0x00000000;
m_flash_end     = 0x0009FFFF;
m_flash_size    = 0x000A0000;
m_sector_size   = 512;
m_fsl_prodInfo_size = m_sector_size;

m_fsl_prodInfo_end                             = m_flash_size - 17 * m_sector_size - 1;
m_fsl_prodInfo_start                           = m_fsl_prodInfo_end - m_fsl_prodInfo_size + 1;
NV_STORAGE_SIZE                                = 63 * m_sector_size;
NV_STORAGE_START_ADDRESS                       = m_fsl_prodInfo_start - 1;
NV_STORAGE_END_ADDRESS                         = NV_STORAGE_START_ADDRESS - NV_STORAGE_SIZE + 1;
INT_STORAGE_START                              = NV_STORAGE_END_ADDRESS - 1;
INT_STORAGE_END                                = 0x48000;
INT_STORAGE_SIZE                               = INT_STORAGE_START - INT_STORAGE_END;
FREESCALE_PROD_DATA_BASE_ADDR                  = m_fsl_prodInfo_start;
INT_STORAGE_SECTOR_SIZE                        = m_sector_size;
M_VECTOR_RAM_SIZE         = DEFINED(__ram_vector_table__) ? vector_table_size : 0x0;
ENTRY(ResetISR)

SECTIONS
{
    /* MAIN TEXT SECTION */
    .header : ALIGN(4)
    {
        _flash_start = ABSOLUTE(.);
        _flash_beg = ABSOLUTE(.);
       
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        __VECTOR_TABLE = .;
        __Vectors = .;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(0);
        LONG(0);
        LONG(0);
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(0);
        LONG(0);
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        FILL(0xff)
        . = ALIGN (0x10);
    } >Flash320
    
    .ro_nonce : ALIGN(0x10)
    {
        _FlsNonceStart = ABSOLUTE(.);
        *(.ro_nonce) /* nonce value is 16 bytes.*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320
    
    .ro_ota_header : ALIGN(0x10)
    {
        _enc_start = ABSOLUTE(.);
        _enc_offset = (_enc_start & 0x0000000F);
        _FlsOtaHeader = ABSOLUTE(.);
        *(.ro_ota_header) /* Ota Header 69 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320
        
    .ro_se_lnkKey (ALIGN((. - _enc_offset), 16) + _enc_offset):
    {
        _FlsLinkKey = ABSOLUTE(.);
        *(.ro_se_lnkKey)  /* Link Key 16 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash320
    
    .filler :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(0x40);
    } > Flash320

    .text : ALIGN(0x40)
    {
        FILL(0xff)
        
       *(.after_vectors*)        
       *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > Flash320
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
       FILL(0xff)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > Flash320
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
       FILL(0xff)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > Flash320
    __exidx_end = .;

    _etext = .;
    
    .interrupts_ram : ALIGN(0x200)
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

    /* Main DATA section (RAM0) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > RAM0 AT>Flash320

    __VECTOR_RAM = __VECTOR_RAM__;
    __RAM_VECTOR_TABLE_SIZE_BYTES = DEFINED(__ram_vector_table__) ? (__interrupts_ram_end__ - __interrupts_ram_start__) : 0x0;

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

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
        . = ALIGN(4) ;
        _end_noinit = .;
        _end_fw_retention = .;
    } > RAM0
	
    /* stack for rom boot during warm resume */
    .boot_resume_stack (NOLOAD): ALIGN(4)
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