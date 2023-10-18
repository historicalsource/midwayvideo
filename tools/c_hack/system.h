/*
 *		$Archive: $
 *		$Revision: $
 *		$Date: $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_system_h = "$Workfile: $ $Revision: $";
#endif

/*
 *		DEFINES
 */

/* define NULL if it does not already exist */
#ifndef NULL
#define NULL			((void *)0)
#endif

/* define NO_ERR if it does not already exist */
#ifndef NO_ERR
#define NO_ERR			0
#endif

/* define symbols for the boolean constants, override previous delcarations */
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
#define FALSE			((bool)0)
#define TRUE			((bool)1)

/* indirect a preprocessor symbol to a string */
#define TO_STRING(a)	TO_VALUE(a)

/* stringify the paramter */
#define TO_VALUE(a)		#a

/* convert a variable to a specified type */
#define TO_TYPE(var, type)	(*((type *) &(var)))

/* offset of a field in bytes from the beginning of a struct */
#define OFFSETOF(type, field)	((lsize_t) &((type *)NULL)->field)

/* get sizeof a struct field without declaring a variable */
#define SSIZEOF(type, field)	(sizeof(((type *)NULL)->field))

/* calculate the number of entries in a staticly declared array */
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

/* used to eliminate unused parameter or variable warnings */
#define UNUSED(a)		((void)a)

/* generate a value for bit, 0 to 31 */
#define BIT(a)			(1UL << (a))

/*
 *		UNSAFE MACROS
 *		parameters are evaluated more than once, beware of side affects
 */

/* return the square of the parameter */
#define SQR(a)			((a) * (a))

/* return the absolute value of the parameter */
#define ABS(a)			((a) < 0 ? -(a) : (a))

/* return -1 if parameter is less than zero */
/* return  0 if parameter is zero */
/* return +1 if parameter is greater than zero */
#define SIGN0(a)		((a) == 0 ? 0 : SIGN(a))

/* return the minimum of two values */
#define MIN(a, b)		((a) < (b) ? (a) : (b))

/* return the maximum of two value */
#define MAX(a, b)		((a) > (b) ? (a) : (b))

/* if a is not a multiple of n round a up to the next multiple of n */
#define ROUND(a, n)		(((a) / (n) + !!((a) % (n))) * (n))

/* if a is not a multiple of n round a up to the next multiple of n */
/* only valid when n is a power of two */
#define ROUND2(a, n)	(((a) + ((n) - 1)) & ~((n) - 1))

/*
 *		SAFE MACROS
 *		parameters are evaluated exactly once
 */

/* return wether the parameter odd or not */
#define ODD(a)			((bool)((a) & 1))

/* return -1 if parameter is less than zero */
/* return +1 otherwise */
#define SIGN(a)			((a) < 0 ? -1 : +1)

/* set bit number bit in long a, bits are numbered 0 to 31 */
#define BIT_SET(a, bit)		((a) |= BIT(bit))

/* clear bit number bit in long a, bits are numbered 0 to 31 */
#define BIT_CLEAR(a, bit)	((a) &= ~(BIT(bit)))

/* test bit number bit in long a, bits are numbered 0 to 31 */
#define BIT_TEST(a, bit)	(((a) & (BIT(bit))) != 0)

/* set the flag */
#define FLAG_SET(a, f)		((a) |= (f))

/* clear the flag */
#define FLAG_CLEAR(a, f)	((a) &= ~(f))

/* test the flag */
#define FLAG_TEST(a, f)		(((a) & (f)) != 0)

/* return the low order word from a long */
#define LOWORD(l)		((short)(l))

/* return the high order word from a long */
#define HIWORD(l)		((short)((ulong)(l) >> 16))

/* return the low order byte from a short */
#define LOBYTE(w)		((char)(w))

/* return the high order byte from a short */
#define HIBYTE(w)		((char)((ushort)(w) >> 8))

/*
 *		make it easy to declare and initialize globals variables
 *
 *		example usage:__EXTERN__ int foo __INIT__(1);
 *
 *		this becomes:
 *			extern int foo;		for declaring access to the global variable
 *								(done many times)
 *		or:
 *			int foo = 1;		for defining storage for the global variable
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
#ifdef DECLARE_GLOBALS						/* for allocating global storage */
#define __EXTERN__			/* empty */
#define __INIT__(value)		= value
#else										/* for declaring a global reference */
#define __EXTERN__			extern
#define __INIT__(value)		/* empty */
#endif

/*
 *		TYPEDEFS
 */

typedef int error_t;						/* an error value */
typedef unsigned long lsize_t;				/* size_t that is guaranteed to be a long */
typedef int bool;							/* a boolean result */
typedef signed char schar;					/* an explicitly signed character */
typedef unsigned char uchar;				/* an explicitly unsigned character */
typedef unsigned short ushort;				/* an unsigned short int */
typedef unsigned int uint;					/* an unsigned int */
typedef unsigned long ulong;				/* an unsigned long int */
#if defined(__GNUC__)						/* GNU C */
typedef signed long long llong;
typedef unsigned long long ullong;
#elif defined(_MSC_VER)						/* Microsoft Visual C++ */
typedef signed __int64 llong;
typedef unsigned __int64 ullong;
#else										/* no support for 64 bit, defaults to a long */
typedef signed long llong;
typedef unsigned long ullong;
#endif
typedef long double ldouble;				/* a long double precision float */

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */

#endif
