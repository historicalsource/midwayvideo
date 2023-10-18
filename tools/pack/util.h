/*
 *		$Archive: /video/tools/pack/util.h $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:11p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_util_h = "$Workfile: util.h $" " $Revision: 1 $";
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

void *util_xmalloc(lsize_t size);
void *util_xcalloc(lsize_t size);
void util_bit_set(uchar *mask, int bit);
void util_bit_clear(uchar *mask, int bit);
bool util_bit_test(uchar *mask, int bit);
ushort util_swap_short(ushort s);
void util_swap_short_ptr(ushort *s);
ulong util_swap_long(ulong l);
void util_swap_long_ptr(ulong *l);
void util_bit_copy(void *s, long src_bit_offset, void *d, long dst_bit_left, lsize_t bit_len);
ulong util_hashjpw(char *str, bool case_sensitive);
bool util_isbdigit(int ch);
bool util_isodigit(int ch);
int util_map_hex_char(int hex_char);
bool util_str_similiar(char *a, char *b);
char *util_str_upper(char *src);
char *util_str_lower(char *src);

#ifdef __cplusplus
}
#endif

/*
 *		$History: util.h $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:11p
 * Created in $/video/tools/pack
 * global utils header file
 */

#endif
