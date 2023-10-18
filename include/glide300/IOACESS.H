//***************************************************************
// IoAccess.h    Include file for WinRT Memory map operations.
//       (c)1994 BlueWater Systems
//
//      These macros are required for accessing mapped memory 
//      directly from Win32 apps. Memory addresses are obtained
//      using WinRTMapMemory().  The platform specific define
//      must be set from the compiler: _X86_ for Intel,
//      _ALPHA_ for Alpha AXP, _MIPS_ for Mips, _PPC_ for PowerPC. 
//      Each platform requires a platform specific WinRT driver.
//
//      30Jul94 PDL  Initial code
//      30Mar95 PDL  Add PowerPC
//***************************************************************

#ifndef _IOACCESS_
#define _IOACCESS_



#if defined (_X86_)

//
// Intel specific Memory access macros
//

#define READ_REGISTER_UCHAR(Register) (*(PUCHAR)(Register))

#define READ_REGISTER_USHORT(Register) (*(PUSHORT)(Register))

#define READ_REGISTER_ULONG(Register) (*(PULONG)(Register))

#define WRITE_REGISTER_UCHAR(Register, Value) (*(volatile UCHAR *)(Register) = (Value))

#define WRITE_REGISTER_USHORT(Register, Value) (*(volatile USHORT *)(Register) = (Value))

#define WRITE_REGISTER_ULONG(Register, Value) (*(volatile ULONG *)(Register) = (Value))

#endif        // defined(_X86_)

#if defined (_ALPHA_)

//
// Alpha AXP specific Memory access macros
//

#if defined(_MSC_VER)

#define MEMORY_BARRIER()  __MB()

#else

#if defined(ACCASM) && !defined(__CPLUSPLUS)

long asm(const char *,...);
#pragma intrinsic(asm)

#define ACC_ASM_BUG 1

#define MEMORY_BARRIER()        asm("mb")

#endif  // ACCASM
#endif  // _MSC_VER
        // Alpha QVA definitions
#define QVA_ENABLE         0xa0000000
#define QVA_ADDRESS_SPACE  0x18000000
#define QVA_SELECTORS      0xb8000000  

#define JENSEN_QVA       0xb0000000

#define EISA_BIT_SHIFT   0x07      

#define EISA_BYTE_LEN    0x00      
#define EISA_WORD_LEN    0x20      
#define EISA_LONG_LEN    0x60      

#define OTHER_QVA        0xa0000000

#define IO_BIT_SHIFT     0x05

#define IO_BYTE_LEN  0x00
#define IO_WORD_LEN  0x08
#define IO_LONG_LEN  0x18

#define READ_PORT_UCHAR READ_REGISTER_UCHAR

#define READ_PORT_USHORT READ_REGISTER_USHORT

#define READ_PORT_ULONG READ_REGISTER_ULONG

#define WRITE_PORT_UCHAR(Port,Value)\
    MEMORY_BARRIER();\
    WRITE_REGISTER_UCHAR ((Port), (Value));\
    MEMORY_BARRIER();
     

#define QUICK_WRITE_PORT_UCHAR(Port,Value)\
    WRITE_REGISTER_UCHAR ((Port), (Value))

#define WRITE_PORT_USHORT(Port,Value)\
    MEMORY_BARRIER();\
    WRITE_REGISTER_USHORT ((Port), (Value));\
    MEMORY_BARRIER();

#define QUICK_WRITE_PORT_USHORT(Port,Value)\
    WRITE_REGISTER_USHORT ((Port), (Value))

#define WRITE_PORT_ULONG(Port,Value)\
    MEMORY_BARRIER();\
    WRITE_REGISTER_ULONG ((Port), (Value));\
    MEMORY_BARRIER();
    

#define QUICK_WRITE_PORT_ULONG(Port,Value)\
    WRITE_REGISTER_ULONG ((Port), (Value))

#define READ_REGISTER_UCHAR(Register)\
   ( \
    ( ((ULONG) (Register) & QVA_SELECTORS) == OTHER_QVA )  ?\
    ((UCHAR)((*(volatile ULONG *)((((ULONG)(Register)\
         ) << IO_BIT_SHIFT)   | IO_BYTE_LEN  )) >> (((ULONG)(Register) & 3) * 8)))\
   : \
    ((UCHAR)((*(volatile ULONG *)((((ULONG)(Register)\
         ) << EISA_BIT_SHIFT) | EISA_BYTE_LEN)) >> (((ULONG)(Register) & 3) * 8)))\
   )

#define READ_REGISTER_USHORT(Register)\
   ( \
    ( ((ULONG) (Register) & QVA_SELECTORS) == OTHER_QVA )  ?\
    ((USHORT)((*(volatile ULONG *)((((ULONG)(Register)\
         ) << IO_BIT_SHIFT)   | IO_WORD_LEN  )) >> (((ULONG)(Register) & 3) * 8)))\
   : \
    ((USHORT)((*(volatile ULONG *)((((ULONG)(Register)\
         ) << EISA_BIT_SHIFT) | EISA_WORD_LEN)) >> (((ULONG)(Register) & 3) * 8)))\
   )

#define READ_REGISTER_ULONG(Register)\
    ( \
     ( ((ULONG) (Register) & QVA_SELECTORS) == OTHER_QVA )  ?\
     (*(volatile ULONG *)((((ULONG)(Register) ) << IO_BIT_SHIFT) | IO_LONG_LEN))\
    : \
     (*(volatile ULONG *)((((ULONG)(Register) ) << EISA_BIT_SHIFT) | EISA_LONG_LEN))\
    )

#define WRITE_REGISTER_UCHAR(Register,Value)\
  if ( (((ULONG) (Register)) & QVA_SELECTORS) == OTHER_QVA)\
    (*(volatile ULONG *)((((ULONG)(Register) ) << IO_BIT_SHIFT) | IO_BYTE_LEN) =\
        (ULONG)((ULONG)(Value) << (((ULONG)(Register) & 3) * 8)));\
  else \
    (*(volatile ULONG *)((((ULONG)(Register) ) << EISA_BIT_SHIFT) | EISA_BYTE_LEN) =\
        (ULONG)((ULONG)(Value) << (((ULONG)(Register) & 3) * 8)));


#define WRITE_REGISTER_USHORT(Register,Value)\
 if ( (((ULONG) (Register)) & QVA_SELECTORS) == OTHER_QVA)\
    (*(volatile ULONG *)((((ULONG)(Register) ) << IO_BIT_SHIFT) | IO_WORD_LEN) =\
        (ULONG)((ULONG)(Value) << (((ULONG)(Register) & 3) * 8)));\
 else \
    (*(volatile ULONG *)((((ULONG)(Register) ) << EISA_BIT_SHIFT) | EISA_WORD_LEN) =\
        (ULONG)((ULONG)(Value) << (((ULONG)(Register) & 3) * 8))); 


#define WRITE_REGISTER_ULONG(Register,Value)\
 if ( (((ULONG) (Register)) & QVA_SELECTORS) == OTHER_QVA)\
    (*(volatile ULONG *)((((ULONG)(Register) ) << IO_BIT_SHIFT) | IO_LONG_LEN) =\
        (ULONG)(Value)) ; \
 else \
    (*(volatile ULONG *)((((ULONG)(Register) ) << EISA_BIT_SHIFT) | EISA_LONG_LEN) =\
        (ULONG)(Value)); 



#endif // defined(_ALPHA_)

#if defined(_MIPS_)

//
// MIPS specific Memory access macros
//

#define MEMORY_BARRIER()


#define READ_PORT_UCHAR(A) READ_REGISTER_UCHAR((PUCHAR)(A))

#define READ_PORT_USHORT(A) READ_REGISTER_USHORT((PUSHORT)(A))

#define READ_PORT_ULONG(A) READ_REGISTER_ULONG((PULONG)(A))

#define WRITE_PORT_UCHAR(A,V) WRITE_REGISTER_UCHAR((PUCHAR)(A),(UCHAR)(V))

#define WRITE_PORT_USHORT(A,V) WRITE_REGISTER_USHORT((PUSHORT)(A),(USHORT)(V))

#define WRITE_PORT_ULONG(A,V) WRITE_REGISTER_ULONG((PULONG)(A),(ULONG)(V))


#define READ_REGISTER_UCHAR(x) \
    (*(volatile UCHAR * const)(x))

#define READ_REGISTER_USHORT(x) \
    (*(volatile USHORT * const)(x))

#define READ_REGISTER_ULONG(x) \
    (*(volatile ULONG * const)(x))

#define WRITE_REGISTER_UCHAR(x, y) \
    (*(volatile UCHAR * const)(x) = (y))

#define WRITE_REGISTER_USHORT(x, y) \
    (*(volatile USHORT * const)(x) = (y))

#define WRITE_REGISTER_ULONG(x, y) \
    (*(volatile ULONG * const)(x) = (y))

#endif // defined(_MIPS_)

#if defined(_PPC_)

#define READ_REGISTER_UCHAR(x) \
    (*(volatile UCHAR * const)(x))

#define READ_REGISTER_USHORT(x) \
    (*(volatile USHORT * const)(x))

#define READ_REGISTER_ULONG(x) \
    (*(volatile ULONG * const)(x))


#define WRITE_REGISTER_UCHAR(x, y) \
    (*(volatile UCHAR * const)(x) = (y))

#define WRITE_REGISTER_USHORT(x, y) \
    (*(volatile USHORT * const)(x) = (y))

#define WRITE_REGISTER_ULONG(x, y) \
    (*(volatile ULONG * const)(x) = (y))

#endif // defined(_PPC_)


#endif // _IOACCESS_
