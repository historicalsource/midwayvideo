/*
 *		$Archive: /video/tools/pack/util.c $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:11p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_util_c = "$Workfile: util.c $" " $Revision: 1 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <ctype.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "util.h"
#include "as.h"

/*
 *		DEFINES
 */

#define STR_QUEUE_SIZE			20
#define STR_QUEUE_ELEMENT_SIZE	256

/*
 *		GLOBAL FUNCTIONS
 */

void *util_xmalloc(lsize_t size)
{
	void *tmp;
	
	tmp = malloc(size);
	if (tmp == NULL)
		as_abort("util_xmalloc:could not allocate %lu bytes", size);
	return tmp;
}  /* util_xmalloc */

void *util_xcalloc(lsize_t size)
{
	void *tmp;
	
	tmp = calloc(size, 1);
	if (tmp == NULL)
		as_abort("util_xcalloc:could not allocate %lu bytes", size);
	return tmp;
}  /* util_xcalloc */

void util_bit_set(uchar *base, int bit)
{
	base[bit / ENV_CHAR_BIT] |= 1 << (bit % ENV_CHAR_BIT);
}  /* util_bit_set */

void util_bit_clear(uchar *base, int bit)
{
	base[bit / ENV_CHAR_BIT] &= ~(1 << (bit % ENV_CHAR_BIT));
}  /* util_bit_clear */

bool util_bit_test(uchar *base, int bit)
{
	int bit_loc;
	
	bit_loc = bit / ENV_CHAR_BIT;
	return base[bit_loc] == 0 ? FALSE : !!(base[bit_loc] & (1 << (bit % ENV_CHAR_BIT)));
}  /* util_bit_test */

char *util_next_string(void)
{
	static char str_queue[STR_QUEUE_SIZE][STR_QUEUE_ELEMENT_SIZE];
	static int str_queue_index = 0;
	
	if (str_queue_index == STR_QUEUE_SIZE)
		str_queue_index = 0;
	return str_queue[str_queue_index++];
}  /* util_next_string */

ushort util_swap_short(ushort s)
{
	uchar t;
	
	t = ((uchar *)&s)[0];
	((uchar *)&s)[0] = ((uchar *)&s)[1];
	((uchar *)&s)[1] = t;
	return s;
}  /* util_swap_short */

void util_swap_short_ptr(ushort *s)
{
	uchar t;
	
	t = ((uchar *)s)[0];
	((uchar *)s)[0] = ((uchar *)s)[1];
	((uchar *)s)[1] = t;
}  /* util_swap_short_ptr */

ulong util_swap_long(ulong l)
{
	uchar t;
	
	t = ((uchar *)&l)[0];
	((uchar *)&l)[0] = ((uchar *)&l)[3];
	((uchar *)&l)[3] = t;
	t = ((uchar *)&l)[1];
	((uchar *)&l)[1] = ((uchar *)&l)[2];
	((uchar *)&l)[2] = t;
	return l;
}  /* util_swap_long */

void util_swap_long_ptr(ulong *l)
{
	uchar t;
	
	t = ((uchar *)l)[0];
	((uchar *)l)[0] = ((uchar *)l)[3];
	((uchar *)l)[3] = t;
	t = ((uchar *)l)[1];
	((uchar *)l)[1] = ((uchar *)l)[2];
	((uchar *)l)[2] = t;
}  /* util_swap_long_ptr */

void util_bit_copy(void *s, long src_bit_offset, void *d, long dst_bit_left, lsize_t bit_len)
{
	static uchar byte_mask0[ENV_CHAR_BIT] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
	static uchar byte_mask1[ENV_CHAR_BIT] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
	uchar *src, *dst;
	long dst_bit_right;
	int shift, shift2;
	uchar mask0, mask1;
	
	src = (uchar *)s;
	dst = (uchar *)d;
	dst_bit_right = dst_bit_left + bit_len;
	shift = (dst_bit_left % ENV_CHAR_BIT) - (src_bit_offset % ENV_CHAR_BIT);
	mask0 = byte_mask0[dst_bit_right % ENV_CHAR_BIT];
	mask1 = byte_mask1[dst_bit_left % ENV_CHAR_BIT];
	src_bit_offset /= ENV_CHAR_BIT;
	dst_bit_left /= ENV_CHAR_BIT;
	dst_bit_right /= ENV_CHAR_BIT;
	src += src_bit_offset;
	dst += dst_bit_left;
	if (shift == 0) {
		if (dst_bit_left == dst_bit_right)
			mask0 &= mask1;
		else {
			*dst = (*dst & ~mask1) | (*src++ & mask1);
			dst++;
			while (++dst_bit_left < dst_bit_right)
				*dst++ = *src++;
		}
		*dst = (*dst & ~mask0) | (*src & mask0);
	} else if (shift > 0) {
		if (dst_bit_left == dst_bit_right) {
			mask0 &= mask1;
			*dst = (*dst & ~mask0) | ((*src >> shift) & mask0);
		} else {
			shift2 = ENV_CHAR_BIT - shift;
			*dst = (*dst & ~mask1) | ((*src >> shift) & mask1);
			dst++;
			while (++dst_bit_left < dst_bit_right) {
				*dst++ = (*src << shift2) | (src[1] >> shift);
				src++;
			}
			*dst = (*dst & ~mask0) | (((*src << shift2) | (src[1] >> shift)) & mask0);
		}
	} else /* shift < 0 ) */{
		shift = -shift;
		shift2 = ENV_CHAR_BIT - shift;
		if (dst_bit_left == dst_bit_right) {
			mask0 &= mask1;
			*dst = (*dst & ~mask0) | (((*src << shift) | (src[1] >> shift2)) & mask0);
		} else {
			*dst = (*dst & ~mask1) | (((*src << shift) | (src[1] >> shift2)) & mask1);
			dst++;
			src++;
			while (++dst_bit_left < dst_bit_right) {
				*dst++ = (*src << shift) | (src[1] >> shift2);
				src++;
			}
			*dst = (*dst & ~mask0) | (((*src << shift) | (src[1] >> shift2)) & mask0);
		}
	}
}  /* util_bit_copy */

ulong util_hashjpw(char *str, bool case_sensitive)
{
	ulong hash_value;
	ulong top4;
	
	for (hash_value = 0; *str != '\0'; str++) {
		hash_value = (hash_value << 4) | (case_sensitive ? *str : toupper(*str));
		top4 = hash_value & 0xf0000000;
		if (top4 != 0) {
			hash_value ^= top4 >> 24;
			hash_value ^= top4;
		}
	}
	return hash_value;
}  /* util_hashjpw */

bool util_isbdigit(int ch)
{
	return ch == '0' || ch == '1';
}  /* util_isbdigit */

bool util_isodigit(int ch)
{
	return ch >= '0' && ch <= '7';
}  /* util_isodigit */

int util_map_hex_char(int hex_char)
{
	ASSERT(isxdigit(hex_char));
	return hex_char <= '9' ? hex_char - '0' : tolower(hex_char) - 'a' + 0x0A;
}  /* util_map_hex_char */

bool util_str_similiar(char *a, char *b)
{
	int i, a_len, b_len;
	bool ret;
	
	a_len = strlen(a);
	b_len = strlen(b);
	if (a_len == b_len) {
		ret = TRUE;
		for (i = 0; i < a_len; i++)
			if (toupper(a[i]) != toupper(b[i])) {
				ret = FALSE;
				break;
			}
	} else
		ret = FALSE;
	return ret;
}  /* str_similiar */

char *util_str_upper(char *src)
{
	int i;
	
	for (i = 0; i < strlen(src); i++)
		src[i] = toupper(src[i]);
	return src;
}  /* str_upper */

char *util_str_lower(char *src)
{
	int i;
	
	for (i = 0; i < strlen(src); i++)
		src[i] = tolower(src[i]);
	return src;
}  /* str_lower */

/*
 *		$History: util.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:11p
 * Created in $/video/tools/pack
 * global utils source file
 */
