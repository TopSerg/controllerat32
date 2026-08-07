/*
*****************************************************************************
**
**  File        : AT32F403AxG_FLASH.ld
**
**  Abstract    : Linker script for AT32F403AxG Device with
**                1000KByte FLASH, 96KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Artery Tek AT32
**
**  Environment : Arm gcc toolchain
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20018000;    /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
BASE_SIZE = 1024;
BOOT_SIZE = BASE_SIZE *16;
APP_SIZE = BASE_SIZE * 128;
DEV_SETTINGS_SIZE = BASE_SIZE * 16;
TABLES_SIZE = BASE_SIZE * 16;
CRC_VALUE_SIZE = 16;
FIRMWARE_INFO_SIZE = 0x80;

KEY_ADDR = 0x08000000 + BOOT_SIZE + APP_SIZE + TABLES_SIZE - CRC_VALUE_SIZE;

MEMORY
{
BOOTLOADER (rx)         : ORIGIN = 0x08000000 , LENGTH = BOOT_SIZE

FIRMWARE (rx)      		: ORIGIN = 0x08000000 + BOOT_SIZE, LENGTH = APP_SIZE - CRC_VALUE_SIZE
FIRMWARE_CRC (rx)       : ORIGIN = 0x08000000 + BOOT_SIZE + APP_SIZE  - CRC_VALUE_SIZE, LENGTH = CRC_VALUE_SIZE

TABLES (rx)             : ORIGIN = 0x08000000 + BOOT_SIZE + APP_SIZE, LENGTH = TABLES_SIZE - CRC_VALUE_SIZE
TABLES_CRC (rx)         : ORIGIN = 0x08000000 + BOOT_SIZE + APP_SIZE + TABLES_SIZE - CRC_VALUE_SIZE, LENGTH = CRC_VALUE_SIZE

FIRMWARE_INFO (rx)      : ORIGIN = 0x08000000 + BOOT_SIZE + APP_SIZE + TABLES_SIZE, LENGTH = FIRMWARE_INFO_SIZE

DEV_SETTINGS (rx)		: ORIGIN = 0x08000000 + BOOT_SIZE + APP_SIZE + TABLES_SIZE + FIRMWARE_INFO_SIZE, LENGTH = DEV_SETTINGS_SIZE - CRC_VALUE_SIZE
DEV_SETTINGS_CRC (rx)	: ORIGIN = 0x08000000 + BOOT_SIZE + APP_SIZE + TABLES_SIZE + DEV_SETTINGS_SIZE + FIRMWARE_INFO_SIZE - CRC_VALUE_SIZE, LENGTH = CRC_VALUE_SIZE

BOOT_SHARED_RAM(xrw)    : ORIGIN = 0x20000000, LENGTH = 1K
RAM (xrw)       		: ORIGIN = 0x20000000+1k, LENGTH = 95K

}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FIRMWARE

  /*Make defination for BootLoader data section in RAM*/
  .bootLoaderSection (NOLOAD):
  {

      KEEP(*(.blRAMSection)) /* keep my variable even if not referenced */

  } > BOOT_SHARED_RAM 

  .firmware_crc :
  {
    . = ALIGN(4);
    KEEP(*(.firmware_crc));
    . = ALIGN(4);
  } > FIRMWARE_CRC

  .tables_crc :
  {
    . = ALIGN(4);
    KEEP(*(.tables_crc));
    . = ALIGN(4);
  } > TABLES_CRC

    .dev_settings_crc :
  {
    . = ALIGN(4);
    KEEP(*(.dev_settings_crc));
    . = ALIGN(4);
  } > DEV_SETTINGS_CRC

  .generatedCode : 
  { 
    . = ALIGN(4);
    *indTable.o (.rodata .rodata*) 
    . = ALIGN(4); 
  }  > TABLES /* new section for my special code files */

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FIRMWARE

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FIRMWARE

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FIRMWARE
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FIRMWARE

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FIRMWARE

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FIRMWARE

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FIRMWARE

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    . = ALIGN(4);
    *(.code_in_ram)
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FIRMWARE

  .guideKey  (KEY_ADDR) (NOLOAD):
  {
  	_guideKeyStart = .; 
  	KEEP(*(.guideKeySect)) /* keep my variable even if not referenced */
  	_guideKeyStart = .;
  } >FIRMWARE 

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM
 
	 .deviceSettings (NOLOAD): 
	 {
	 	start_deviceSettings = .;
	 	KEEP (*(.deviceSettings))
	 	*(.deviceSettings)
	 	*(.deviceSettings*)
	 	end_deviceSettings = .;
	 } >DEV_SETTINGS
 
  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}