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
_zigbee_hdr     = __base_Flash320 + 0x150; /* Fixed offset as expected by JET tool */

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
    .headerA : ALIGN(4)
    {
        _flash_start = ABSOLUTE(.);
        _flash_beg = ABSOLUTE(.);
       
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        __VECTOR_TABLE = .;
        __Vectors = .;
        KEEP(*(.isr_vector))
        /* Global Section Table: now in 2 parts to allow ZigBee headers to 
           stay in place. Part 2 in is headerB */
        . = ALIGN(4) ; 
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(0);
        LONG(0);
        LONG(0);
        __data_discard_section_table = .;
        LONG(LOADADDR(.data_discard));
        LONG(    ADDR(.data_discard));
        LONG(_edata_discard - _data_discard);
        __data_section_table_end = .;
        /* End of Global Section Table part 1 */

        FILL(0xff)
        . = ALIGN (0x10);
    } >Flash320
    
    .ro_nonce _zigbee_hdr : ALIGN(0x10)
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
    
    .headerB : ALIGN(4)
    {
        FILL(0xff)
        /* Global Section Table part 2 */
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(0);
        LONG(0);
        __bss_discard_section_table = .;
        LONG(    ADDR(.bss_discard));
        LONG(_ebss_discard - _bss_discard);
        __bss_section_table_end = .;
        /* End of Global Section Table part 2 */
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
    
    /* RAM0 SECTION */
    /* RAM0 contents are specified out of order so that wildcards do not catch
       specific input sections before their target region is defined. The 
       placing of the RAM0 sections in memory and their characteristics are:
       
         Section                  | Initialised   | Preserved in 
                                  | on cold start | RAMON sleep
         -------------------------+---------------+--------------
         .uninit_RESERVED         |      N        |       Y (should be 0)
         .noinit                  |      N        |       Y (should be 0)
         .interrupts_ram          |      N        |       N
         .data_discard            |   Partial     |       N
         .bss_discard             |   Partial     |       N
         .bss_MAC                 |      N        |       N
         .boot_resume_stack       |      N        |       N
         <spare space>            |      N        |       N
         <CPU stack>              |      N        |       Y
         .heap                    |      N        |       Y
         .data/.data_cpu_stack    |      Y        |       Y
         .bss/.bss_cpu_stack      |      Y        |       Y
         <32 bytes for boot code> |      Y        |       Y
                                  
        This file is designed so that only variables defined to be discarded
        are placed into the discard sections; all other variables are to be
        retained after the CPU stack. bss_MAC is not preserved.
     */
         
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
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
        
    /* RAM0 after here is not saved through sleep */

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

    /* Data discard section */
    .data_discard : ALIGN(4)
    {
       FILL(0xff)
       _data_discard = . ;
       *(.data.discard.stack)
       *(.data.discard.app)
       Mini*.o(.data*)
       MMAC*.o(.data*) 
       zps_*.o(.data*)
       *zps_gen.o(.data*)
       ZQueue*(.data*)
       ZTimer*(.data*)
       *pdum_*(.data*)
       *ZCLobj_*(.data*)
       *lib_a-mallocr.o(.data*)
       *lib_a-impure.o(.data*)
       *lib_a-locale.o(.data*)
       *radio.o(.data*)
       . = ALIGN(4) ;
       _edata_discard = . ;
       *(.data.discard_noinit.stack)
       *(.data.discard_noinit.app)
       . = ALIGN(4) ;
       _edata_discard_noinit = . ;
    } > RAM0 AT>Flash320

    /* This filler goes between the .data_discard and .data sections, to avoid
       gaps in the binary which cause the debug program operation to break */
    .fillerB :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(16);
    } > Flash320

    /* BSS discard section */
    .bss_discard : ALIGN(4)
    {
        _bss_discard = .;
        *(.bss.discard.stack)
        *(.bss.discard.app)
        Mini*.o(.bss*)
        Mini*.o(COMMON)
        MMAC*.o(.bss*)
        MMAC*.o(COMMON)
        zps_*.o(.bss*) 
        zps_*.o(COMMON) 
        *zps_gen.o(.bss*) 
        *zps_gen.o(COMMON) 
        ZQueue*(.bss*)
        ZQueue*(COMMON)
        ZTimer*(.bss*)
        ZTimer*(COMMON)
        *pdum_*(.bss*)
        *pdum_*(COMMON)
        *ZCLobj_*(.bss*)
        *ZCLobj_*(COMMON)
        *psector_api.o(.bss*)
        *SecLib.o(.bss*)
        *TimersManager.o(.bss*)
        *lib_a-mallocr.o(.bss*)
        *lib_a-lock.o(COMMON)
        *GPIO_Adapter.o(.bss*)
        *radio.o(.bss*)
        *radio.o(COMMON)
        *MemManager.o(.bss*)
        . = ALIGN(4) ;
        _ebss_discard = .;
        *(.bss.discard_noinit.stack)
        *(.bss.discard_noinit.app)
        . = ALIGN(4) ;
        _ebss_discard_noinit = .;
    } > RAM0

    /* BSS section for MAC buffers */
    .bss_MAC (NOLOAD) : ALIGN(4)
    {
       /* MAC buffer section: must be within 128kB block. __mac_buffer_base is
          defined further down to be on 128kB alignment */
        __mac_buffer_start = .;
       *(.mac_buffer)

        . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
        __mac_buffer_end = .;
    } > RAM0

    __VECTOR_RAM = __VECTOR_RAM__;
    __RAM_VECTOR_TABLE_SIZE_BYTES = DEFINED(__ram_vector_table__) ? (__interrupts_ram_end__ - __interrupts_ram_start__) : 0x0;
	
    /* stack for rom boot during warm resume */
    .boot_resume_stack (NOLOAD): ALIGN(4)
    {
        _boot_resume_stack = .;
        *(.boot_resume_stack*)
        . += BOOT_RESUME_STACK_SIZE;
        . = ALIGN(4) ;
        _end_boot_resume_stack = .;
    } > RAM0
    
    /* HEAP */
    .heap _vStackTop (NOLOAD) : ALIGN(4)
    {
        _heap = .;
        . += HEAP_SIZE;
        . = ALIGN(4) ;
        _end_heap = .;
    } > RAM0

    /* Main DATA section (RAM0) above stack top */
    .data : ALIGN(16)
    {
       FILL(0xff)
       _data = . ;
       *(.data.cpu_stack)
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(16) ;
       _edata = . ;
       
    } > RAM0 AT>Flash320

    /* MAIN BSS SECTION above CPU stack section */
    .bss _edata (NOLOAD) : ALIGN(16)
    {
        _bss = .;
        *(.bss.cpu_stack)
        *(.bss*)
        *(COMMON)
        *(g_u32NwkFrameCounter)
        . = ALIGN(16) ;
        _ebss = .;

        PROVIDE(end = .);
    } > RAM0


    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : _heap);
    PROVIDE(_pvHeapLimit = _pvHeapStart + (HEAP_SIZE));
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : ((__top_RAM0 - SIZEOF(.data) - SIZEOF(.bss) - SIZEOF(.heap) - 32) & 0xfffffff8));
    PROVIDE(__mac_buffer_base = (__mac_buffer_start & 0xfffe0000));

    __StackLimit = _vStackTop - STACK_SIZE;
    ASSERT(__StackLimit >= _end_boot_resume_stack, "Linker script: Possible stack corruption with data/bss/boot_stack")
    ASSERT(_ebss <= (__top_RAM0 - 32), "Linker script: Possible mis-alignment of retained data sections")
}
