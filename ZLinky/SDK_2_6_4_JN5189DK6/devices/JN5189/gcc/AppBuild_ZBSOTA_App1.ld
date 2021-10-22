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
GROUP (
  libcr_c.a
  libcr_eabihelpers.a
)

RAM0_TOP_ADDR = (0x04016000 - 32);
RAM0_FOR_APP1 = 0x800; /* 2K bytes */
RAM0_DISCARD_FOR_APP1 = 0x1000; /* 4K bytes */ 
RAM0_BASE_ADDR = (RAM0_TOP_ADDR - RAM0_FOR_APP1);

MEMORY
{
  /* Define each memory region */
  Flash640 (rx) : ORIGIN = 0x72000, LENGTH = 0x22000 /* 136K bytes (alias Flash) */  
  RAM0_DISCARD (rwx) : ORIGIN = 0x04000400, LENGTH = RAM0_DISCARD_FOR_APP1
  RAM0 (rwx) : ORIGIN = RAM0_BASE_ADDR, LENGTH = RAM0_FOR_APP1
}

/* Define a symbol for the top of each memory region */
__base_Flash640 = 0x72000  ; /* Flash640 */  
__base_Flash = 0x72000 ; /* Flash */  
__top_Flash640 = 0x72000 + 0x22000 ; /* 162K bytes */  
__top_Flash = 0x72000 + 0x22000 ; /* 136K bytes */  
__base_RAM0 = RAM0_BASE_ADDR  ; /* RAM0 */  
__base_RAM = RAM0_BASE_ADDR ; /* RAM */  
__top_RAM0 = RAM0_TOP_ADDR ; /* 5K bytes */  
__top_RAM = RAM0_TOP_ADDR ; /* 5K bytes */  
_zigbee_hdr = __base_Flash640 + 0x150; /* Fixed offset as expected by JET tool */
__mac_buffer_base = 0x04000000;

ENTRY(ResetISR)

SECTIONS
{
    /* MAIN TEXT SECTION */
    .header : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_discard_section_table = .;
        LONG(LOADADDR(.data_discard));
        LONG(    ADDR(.data_discard));
        LONG(_edata_discard - _data_discard);
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_discard_section_table = .;
        LONG(    ADDR(.bss_discard));
        LONG(_ebss_discard - _bss_discard);
        __bss_section_table_end = .;
        /* End of Global Section Table */
        FILL(0xff)
        . = ALIGN (0x10);
    } >Flash640
	
    .ro_nonce _zigbee_hdr : ALIGN(0x10)
    {
        _FlsNonceStart = ABSOLUTE(.);
        KEEP(*(.ro_nonce)) /* nonce value is 16 bytes.*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash640
    
    .ro_ota_header  : ALIGN(0x10)
    {
        _enc_start = ABSOLUTE(.);
        _enc_offset = (_enc_start & 0x0000000F);
        _FlsOtaHeader = ABSOLUTE(.);
        KEEP(*(.ro_ota_header)) /* Ota Header 69 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash640
        
    .ro_se_lnkKey (ALIGN((. - _enc_offset), 16) + _enc_offset):
    {
        _FlsLinkKey = ABSOLUTE(.);
        KEEP(*(.ro_se_lnkKey))  /* Link Key 16 bytes*/
        FILL(0xff)
        . = ALIGN (0x10);
    } > Flash640

    .filler :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(0x40);
    } > Flash640
    
    .text : ALIGN(4)
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
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > Flash640
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > Flash640
    __exidx_end = .;

    _etext = .;
        
    /* RAM0 after here is not saved through sleep */

    /* Data discard section */
    .data_discard : ALIGN(4)
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
    } > RAM0_DISCARD AT>Flash640

    /* This filler goes between the .data_discard and .data sections, to avoid
       gaps in the binary which cause the debug program operation to break */
    .fillerB :
    {
        BYTE(0xff)
        FILL(0xff);
        . = ALIGN(16);
    } > Flash640

    /* BSS discard section */
    .bss_discard : ALIGN(4)
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
    } > RAM0_DISCARD

    /* MAIN DATA SECTION: saved through sleep */
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
    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
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

    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}