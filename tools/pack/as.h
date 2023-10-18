/*
 *		$Archive: /video/tools/pack/as.h $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:08p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

/* This header may be included multiple times with different NDEBUG values. */
#ifdef ASSERT
#undef ASSERT
#endif
#ifdef ASSERTF
#undef ASSERTF
#endif

#ifndef NDEBUG
#define ASSERT(expr)	((expr) ? (void)0 : as_output("assertion failed:`" #expr "\', file \"" __FILE__ "\", line " TO_STRING(__LINE__)))
#define ASSERTF(args)	as_assert args
#else
#define ASSERT(expr)	((void)0)
#define ASSERTF(args)	((void)0)
#endif

#ifndef __AS_H__
#define __AS_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_as_h = "$Workfile: as.h $ $Revision: 1 $";
#endif

/*
 *		USER INCLUDES
 */

#ifndef __SYSTEM_H__
#include "system.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *		GLOBAL PROTOTYPES
 */

void as_assert(int expr, char *format, ...);
void as_abort(char *format, ...);
void as_output(char *message);

/*
 *		GLOBAL VARIABLES
 */

__EXTERN__ bool as_really_abort __INIT__(TRUE);

#ifdef __cplusplus
}
#endif

/*
 *		$History: as.h $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:08p
 * Created in $/video/tools/pack
 * assert library header
 */

#endif
