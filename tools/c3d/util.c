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

#ifdef INCLUDE_SSID
char *ss_util_c = "$Workfile: $ $Revision: $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "util.h"

/*
 *		GLOBAL FUNCTIONS
 */

void *util_xmalloc(lsize_t size)
{
	void *tmp;
	
	tmp = malloc(size);
	if (tmp == NULL)
		util_error("util_xmalloc:could not allocate %lu bytes", size);
	return tmp;
}  /* util_xmalloc */

void *util_xcalloc(lsize_t size)
{
	void *tmp;
	
	tmp = calloc(size, 1);
	if (tmp == NULL)
		util_error("util_xcalloc:could not allocate %lu bytes", size);
	return tmp;
}  /* util_xcalloc */

void util_bit_set(uchar *base, int bit)
{
	base[bit / CHAR_BIT] |= 1 << (bit % CHAR_BIT);
}  /* util_bit_set */

void util_bit_clear(uchar *base, int bit)
{
	base[bit / CHAR_BIT] &= ~(1 << (bit % CHAR_BIT));
}  /* util_bit_clear */

bool util_bit_test(uchar *base, int bit)
{
	int bit_loc;
	
	bit_loc = bit / CHAR_BIT;
	return base[bit_loc] == 0 ? FALSE : !!(base[bit_loc] & (1 << (bit % CHAR_BIT)));
}  /* util_bit_test */

void util_error(char *format, ...)
{
	va_list arg_list;
	
	va_start(arg_list, format);
	vfprintf(stderr, format, arg_list);
	va_end(arg_list);
	exit(EXIT_FAILURE);
}	/* error */

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */
