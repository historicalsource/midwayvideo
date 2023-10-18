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

#ifndef __TGA_H__
#define __TGA_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_tga_h = "$Workfile: $ $Revision: $";
#endif

/*
 *		SYSTEM INCLUDES
 */

/*
 *		USER INCLUDES
 */

#ifndef __SYSTEM_H__
#include "system.h"
#endif

/*
 *		DEFINES
 */

/*
 *		TYPEDEFS
 */

typedef struct {
	uchar ident_len;
	uchar color_map_type;
	uchar image_type;
	ushort color_map_origin;
	ushort color_map_len;
	uchar color_map_depth;
	ushort x_offset;
	ushort y_offset;
	ushort width;
	ushort height;
	uchar pixel_depth;
	uchar image_descriptor;
} tga_header;

typedef union {
	ulong lpixel;
	struct {
		uchar a;
		uchar r;
		uchar g;
		uchar b;
	} spixel;
} tga_pixel;

/*
 *		GLOBAL PROTOTYPES
 */

void tga_read(char *tga_fname, tga_header *header, tga_pixel **image_data);
void tga_write(char *tga_fname, int width, int height, bool output_alpha, tga_pixel *image_data);
void tga_free(tga_pixel *image_data);

/*
 *		GLOBAL VARIABLES
 */

#endif
