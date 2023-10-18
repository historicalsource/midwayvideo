OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(start)
STARTUP(obj/crt0.o)
MEMORY
{
  ROM : ORIGIN = 0xBFC00000, LENGTH = 512K
  RAM : ORIGIN = 0x80100000, LENGTH = (8192K-786K-16K)
}
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .text :
  {
    _ftext = . ;
    *(.text)
    CREATE_OBJECT_SYMBOLS
    _etext = .;
  } > RAM
  .init    ALIGN(8)  : { *(.init)    } > RAM = 0
  .fini    ALIGN(8)  : { *(.fini)    } > RAM = 0
  .ctors   ALIGN(8)  : { *(.ctors)   } > RAM
  .dtors   ALIGN(8)  : { *(.dtors)   } > RAM
  .rodata  ALIGN(8)  : { *(.rodata)  } > RAM
  .rodata1 ALIGN(8)  :
  {
    *(.rodata1)
    . = ALIGN(8);
  } > RAM
  .reginfo . : { *(.reginfo) } > RAM
  /*
  ** also: .hash .dynsym .dynstr .plt(if r/o) .rel.got
  ** Read-write section, merged into data segment:
  */
  .data  . :
  {
    _fdata = . ;
    *(.data)
    CONSTRUCTORS
  } > RAM
  .data1 ALIGN(8)  : { *(.data1) } > RAM
  _gp = . + 0x8000;
  .lit8 . : { *(.lit8) } > RAM
  .lit4 . : { *(.lit4) } > RAM
  /*
  ** also (before uninitialized portion): .dynamic .got .plt(if r/w)
  ** (or does .dynamic go into its own segment?)
  ** We want the small data sections together, so single-instruction offsets
  ** can access them all, and initialized data all before uninitialized, so
  ** we can shorten the on-disk segment size.
  */
  .sdata   ALIGN(8)  : { *(.sdata) } > RAM
  _edata  =  .;
  .sbss    ALIGN(8)  : { *(.sbss) *(.scommon) } > RAM
  .bss     ALIGN(8)  :
  {
    _fbss = .;
    *(.bss)
    *(COMMON)
    *(.scommon)
    _end = . ;
  } > RAM
  /*
  ** Debug sections.  These should never be loadable, but they must have
  ** zero addresses for the debuggers to work correctly.
  */
  .line             0 : { *(.line)           }
  .debug            0 : { *(.debug)          }
  .debug_sfnames    0 : { *(.debug_sfnames)  }
  .debug_srcinfo    0 : { *(.debug_srcinfo)  }
  .debug_macinfo    0 : { *(.debug_macinfo)  }
  .debug_pubnames   0 : { *(.debug_pubnames) }
  .debug_aranges    0 : { *(.debug_aranges)  }
}
