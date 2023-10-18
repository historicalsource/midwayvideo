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

#ifndef __UTIL_H__
#define __UTIL_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_util_h = "$Workfile: $ $Revision: $";
#endif

/*
 *		USER INCLUDES
 */

#ifndef __SYSTEM_H__
#include "system.h"
#endif

/*
 *		GLOBAL PROTOTYPES
 */

void *util_xmalloc(lsize_t size);
void *util_xcalloc(lsize_t size);
void util_bit_set(uchar *mask, int bit);
void util_bit_clear(uchar *mask, int bit);
bool util_bit_test(uchar *mask, int bit);
void util_error(char *format, ...);

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */

#endif
