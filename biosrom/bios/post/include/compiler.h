#ifndef _COMPILER_H_
#define _COMPILER_H_

/*
����������������������������������������������������������������������������ͻ
�                                                                            �
� File:    COMPILER.H                                                        �
� Author:  Jack Miller                                                       �
� Created: 29-May-1996                                                       �
�                                                                            �
����������������������������������������������������������������������������͹
�                                                                            �
�   'C' type definitions for the Phoenix system.                             �
�                                                                            �
����������������������������������������������������������������������������͹
� HISTORY:                                                                   �
�                                                                            �
�  25Jan97 JVM  Modified typedefs to be generic (mx - means Midway)          �
�  29May96 JVM  Created.                                                     �
�                                                                            �
����������������������������������������������������������������������������ͼ
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
