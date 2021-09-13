/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2017
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

MEMORY
{
  /* Define each memory region. RAM0 definition leaves the first 1kB for the
     boot code */
  Flash640 (rx) : ORIGIN = 0x0,       LENGTH = 0xa0000 /* 640K bytes (alias Flash) */  
  RAM0 (rwx)    : ORIGIN = 0x4001800, LENGTH = 0x14800 /* 82K bytes (alias RAM) */  
  RAM1 (rwx)    : ORIGIN = 0x4020000, LENGTH = 0x10000 /* 64K bytes (alias RAM2) */  
}

/* Define a symbol for the top of each memory region */
__base_Flash640 = 0x0  ; /* Flash640 */  
__base_Flash = 0x0 ; /* Flash */  
__top_Flash640 = 0x0 + 0xa0000 ; /* 640K bytes */  
__top_Flash = 0x0 + 0xa0000 ; /* 640K bytes */  
__base_RAM0 = 0x4001800  ; /* RAM0 */  
__base_RAM = 0x4001800 ; /* RAM */  
__top_RAM0 = 0x4001800 + 0x14800 ; /* 82K bytes */  
__top_RAM = 0x4001800 + 0x14800 ; /* 82K bytes */  
__base_RAM1 = 0x4020000  ; /* RAM1 */  
__base_RAM2 = 0x4020000 ; /* RAM2 */  
__top_RAM1 = 0x4020000 + 0x10000 ; /* 64K bytes */  
__top_RAM2 = 0x4020000 + 0x10000 ; /* 64K bytes */  

HEAP_SIZE  = 0x2F4;
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
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        FILL(0xff)
        . = ALIGN (0x10);
    } >Flash640
    
    .ro_nonce : ALIGN(0x10)
    {
        _FlsNonceStart = ABSOLUTE(.);
        *(.ro_nonce) /* nonce value is 16 bytes.*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash640
    
    .ro_ota_header : ALIGN(0x10)
    {
        _enc_start = ABSOLUTE(.);
        _enc_offset = (_enc_start & 0x0000000F);
        _FlsOtaHeader = ABSOLUTE(.);
        *(.ro_ota_header) /* Ota Header 69 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash640
        
    .ro_se_lnkKey (ALIGN((. - _enc_offset), 16) + _enc_offset):
    {
        _FlsLinkKey = ABSOLUTE(.);
        *(.ro_se_lnkKey)  /* Link Key 16 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash640
    
    .filler :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(0x40);
    } > Flash640

    .text : ALIGN(0x40)
    {
        FILL(0xff)
        
       *(.after_vectors*)        
       *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > Flash640
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
       FILL(0xff)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > Flash640
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
       FILL(0xff)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > Flash640
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
     } > RAM1 AT>Flash640

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
    } > RAM0 AT>Flash640


    /* BSS section for RAM1 */
    .bss_RAM2 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
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