/*
 *		$Archive: /video/tools/pack/system.h $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:10p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_system_h = "$Workfile: system.h $" " $Revision: 1 $";
#endif

/*
 *		USER INCLUDES
 */

#ifndef COMPILING_GENENV
#if defined(__DOS__)
#include "dosenv.h"
#elif defined(__PHX__)
#include "phxenv.h"
#else
#error execution platform not defined, use -D__DOS__ or -D__PHX__
#endif
#endif

/*
 *		DEFINES
 */

/* Limits for the long long, 64 bit data type. */
#if defined(__GNUC__)						/* GNU C */
#define LLONG_MIN		(-9223372036854775807LL - 1LL)
#define LLONG_MAX		9223372036854775807LL
#define ULLONG_MAX		18446744073709551615ULL
#elif defined(_MSC_VER)						/* Microsoft Visual C++ */
#define LLONG_MIN		(-9223372036854775807i64 - 1)
#define LLONG_MAX		9223372036854775807i64
#define ULLONG_MAX		18446744073709551615ui64
#else										/* no support for 64 bit, defaults to a long */
#define LLONG_MIN		(-2147483647L - 1L)
#define LLONG_MAX		2147483647L
#define ULLONG_MAX		4294967295UL
#endif

/* Define NULL if it does not already exist.*/
#ifndef NULL
#define NULL			((void *)0)
#endif

/* Define symbols for the boolean constants, override previous delcarations. */
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
#define FALSE			((bool)0)
#define TRUE			((bool)1)

/* Indirect the preprocessor symbol to a string. */
#define TO_STRING(a)	TO_VALUE(a)

/* Stringify the paramter. */
#define TO_VALUE(a)		#a

/* Convert a variable to a specified type. */
#define MAKE_TYPE(var, type)	(*((type *) &(var)))

/* Calculate the number of entries in a staticly declared array. */
#define ARRAY_SIZE(a)		(sizeof(a) / sizeof(a[0]))

/* Used to eliminate unused parameter or variable warnings. */
#define UNUSED(a)		((void)a)

/* Generate a value for bit, 0 to 31 */
#define BIT(a)			(1UL << (a))

/*
 *		UNSAFE MACROS
 *		Parameters are evaluated more than once, beware of side affects.
 */

/* Return the square of the parameter. */
#define SQR(a)			((a) * (a))

/* Return the absolute value of the parameter. */
#define ABS(a)			((a) < 0 ? -(a) : (a))

/* Return -1 if parameter is less than zero. */
/* Return  0 if parameter is zero. */
/* Return +1 if parameter is greater than zero. */
#define SIGN0(a)		((a) == 0 ? 0 : SIGN(a))

/* Return the minimum of two values. */
#define MIN(a, b)		((a) < (b) ? (a) : (b))

/* Return the maximum of two value. */
#define MAX(a, b)		((a) > (b) ? (a) : (b))

/* If a is not a multiple of n round a up to the next multiple of n. */
#define ROUND(a, n)		(((a) / (n) + !!((a) % (n))) * (n))

/* If a is not a multiple of n round a up to the next multiple of n. */
/* Only valid when n is a power of two. */
#define ROUND2(a, n)	(((a) + ((n) - 1)) & ~((n) - 1))

/*
 *		SAFE MACROS
 *		Parameters are evaluated exactly once.
 */

/* Return wether the parameter odd or not. */
#define ODD(a)			((bool)((a) & 1))

/* Return -1 if parameter is less than zero. */
/* Return +1 otherwise. */
#define SIGN(a)			((a) < 0 ? -1 : +1)

/* Set bit number bit in long a, bits are numbered 0 to 31. */
#define BIT_SET(a, bit)		((a) |= BIT(bit))

/* Clear bit number bit in long a, bits are numbered 0 to 31. */
#define BIT_CLEAR(a, bit)	((a) &= ~(BIT(bit)))

/* Test bit number bit in long a, bits are numbered 0 to 31. */
#define BIT_TEST(a, bit)	(((a) & (BIT(bit))) != 0)

/* Return the low order word from a long. */
#define LOWORD(l)		((short)(l))

/* Return the high order word from a long. */
#define HIWORD(l)		((short)((ulong)(l) >> 16))

/* Return the low order byte from a short. */
#define LOBYTE(w)		((char)(w))

/* Return the high order byte from a short. */
#define HIBYTE(w)		((char)((ushort)(w) >> 8))

/*
 *		Make it easy to declare and initialize globals variables.
 *
 *		example usage:__EXTERN__ int foo __INIT__(1);
 *
 *		this becomes:
 *			extern int foo;		For declaring access to the global variable.
 *								(done many times)
 *		or:
 *			int foo = 1;		For defining storage for the global variable.
 *								(done exactly once)
 *
 *		then in one file, say globals.c, have:
 *		#define DECLARE_GLOBALS
 *		#include "system.h"
 *		#include "abc.h"
 *			.
 *			.					include all program header files
 *			.
 *		#include "xyz.h"
 */
#ifdef DECLARE_GLOBALS						/* For allocating global storage. */
#define __EXTERN__			/* empty */
#define __INIT__(value)		= value
#else										/* For declaring a global reference. */
#define __EXTERN__			extern
#define __INIT__(value)		/* empty */
#endif

/*
 *		TYPEDEFS
 */

typedef int error_t;						/* an error value */
typedef unsigned long int lsize_t;			/* size_t that is guaranteed to be a long */
typedef char *str;							/* reference to a C library string */
typedef unsigned char *pstr;				/* reference to a Pascal string */

typedef int bool;							/* a boolean result */
typedef unsigned char uchar;				/* an explicitly unsigned character */
typedef signed char schar;					/* an explicitly signed character */
typedef unsigned short int ushort;			/* an unsigned short int */
typedef unsigned int uint;					/* an unsigned int */
typedef unsigned long int ulong;			/* an unsigned long int */
#if defined(__GNUC__)						/* GNU C */
typedef signed long long int llong;			/* a int that is at least as big as a long */
typedef unsigned long long int ullong;		/* an unsigned int that is at least as big as a ulong */
#elif defined(_MSC_VER)						/* Microsoft Visual C++ */
typedef signed __int64 llong;				/* a int that is at least as big as a long */
typedef unsigned __int64 ullong;			/* an unsigned int that is at least as big as a ulong */
#else										/* no support for 64 bit, defaults to a long */
typedef signed long int llong;
typedef unsigned long int ullong;
#endif

/*
 *		$History: system.h $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:10p
 * Created in $/video/tools/pack
 * global typedef and define file
 */

#endif
