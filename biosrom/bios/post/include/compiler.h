#ifndef _COMPILER_H_
#define _COMPILER_H_

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    COMPILER.H                                                        บ
บ Author:  Jack Miller                                                       บ
บ Created: 29-May-1996                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   'C' type definitions for the Phoenix system.                             บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  25Jan97 JVM  Modified typedefs to be generic (mx - means Midway)          บ
บ  29May96 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/

#if defined(__WATCOMC__) || defined(_MSC_VER) || defined(CLANGUAGE)
typedef          unsigned char  mxU8;
typedef          signed   char  mxS8;
typedef          unsigned short mxU16;
typedef          signed   short mxS16;
typedef          unsigned long  mxU32;
typedef          signed   long  mxS32;
#if !defined(__WATCOMC__) && !defined(_MSC_VER)
typedef          unsigned long long     mxU64;
typedef          signed   long long     mxS64;
#endif

typedef volatile unsigned char  mxVU8;
typedef volatile signed   char  mxVS8;
typedef volatile unsigned short mxVU16;
typedef volatile signed   short mxVS16;
typedef volatile unsigned long  mxVU32;
typedef volatile signed   long  mxVS32;
#if !defined(__WATCOMC__) && !defined(_MSC_VER)
typedef volatile unsigned long long     mxVU64;
typedef volatile signed   long long     mxVS64;
#endif

typedef int                     mxBool;
typedef float                   mxFloat;
typedef double                  mxDouble;
#endif

#define CHAR    unsigned char
#define UCHAR   unsigned char
#define UBYTE	unsigned char
#define BYTE	char
#define SHORT   unsigned short
#define USHORT  unsigned short
#define UWORD   unsigned short
#define WORD    short
#define ULONG   unsigned long
#define LONG    long
#define DWORD   unsigned long
#define BOOL    unsigned int

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#endif
