OUTPUT_FORMAT("coff-shl")
OUTPUT_ARCH(sh)
MEMORY
{
  ram : o = 0x8C010000, l = 16M
}
SECTIONS
{
  .text :
  {
    *(.text)
    *(.rodata)
    *(.strings)
     _etext = . ; 
  }  > ram
  .tors :
  {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    ___dtors_end = . ;
  } > ram
  .data :
  {
    *(.data)
     _edata = . ; 
  }  > ram
  .bss :
  {
     _bss_start = . ; 
    *(.bss)
    *(COMMON)
     _end = . ;  
  }  > ram
  .stack   :
  {
     _stack = . ; 
    *(.stack)
  }  > ram
  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }
  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}
