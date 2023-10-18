/*
 *		$Archive: /video/tools/c_hack/tga.c $
 *		$Revision: 2 $
 *		$Date: 7/15/98 11:08a $
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
char *ss_tga_c = "$Workfile: tga.c $ $Revision: 2 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "tga.h"
#include "util.h"

/*
 *		STATIC PROTOTYPES
 */

static void read_header(FILE *fp, tga_header *header);
static void write_header(FILE *fp, tga_header *header);
#if 0
static void print_header(tga_header *header);
#endif
static void read_type1(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data);
static void read_type2(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data);
static void write_type2(FILE *fp, tga_header *header, tga_pixel *image_data);
static void read_type9(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data);
static void read_type10(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data);
static void read_color_map(FILE *fp, tga_header *header, tga_pixel **color_map);
static uchar scale_color(uchar color);
static void flip_image(tga_header *header, tga_pixel **image_data);
static uchar get_char(FILE *fp);
static ushort get_short(FILE *fp);
static ulong get_long(FILE *fp);
static void put_char(uchar data, FILE *fp);
static void put_short(ushort data, FILE *fp);
#if 0
static void put_long(ulong data, FILE *fp);
#endif
static void skip_data(FILE *fp, int count);

/*
 *		GLOBAL FUNCTIONS
 */

void tga_read(char *tga_fname, bool input_alpha, tga_header *header, tga_pixel **image_data)
{
	FILE *fp;
	
	fp = fopen(tga_fname, "rb");
	if (fp == NULL)
		util_error("Targa file \"%s\" not found\n", tga_fname);
	else {
		read_header(fp, header);
		switch (header->image_type) {
		case 0:
			util_error("%s is a type 0 targa file, with no pixel data\n", tga_fname);
			break;
		case 1:
			read_type1(tga_fname, input_alpha, fp,header, image_data);
			break;
		case 2:
			read_type2(tga_fname, input_alpha, fp, header, image_data);
			break;
		case 3:
			util_error("%s is a type 3 black and white targa file, which is not supported\n", tga_fname);
			break;
		case 9:
			read_type9(tga_fname, input_alpha, fp, header, image_data);
			break;
		case 10:
			read_type10(tga_fname, input_alpha, fp, header, image_data);
			break;
		case 11:
			util_error("%s is a type 11 compressed black and white targa file, which is not supported\n", tga_fname);
			break;
		case 32:
		case 33:
			util_error("%s is a type %d new style compressed targa file, which is not supported\n", header->image_type, tga_fname);
			break;
		}
		fclose(fp);
	}
}	/* tga_read */

void tga_write(char *tga_fname, int width, int height, bool output_alpha, tga_pixel *image_data)
{
	FILE *fp;
	tga_header header;
	
	fp = fopen(tga_fname, "wb");
	if (fp == NULL)
		util_error("Targa file \"%s\" could not be created\n", tga_fname);
	else {
		header.ident_len = 0;
		header.color_map_type = 0;
		header.image_type = 2;
		header.color_map_origin = 0;
		header.color_map_len = 0;
		header.color_map_depth = 0;
		header.x_offset = 0;
		header.y_offset = 0;
		header.width = width;
		header.height = height;
		header.pixel_depth = output_alpha ? 32 : 24;
		header.image_descriptor = output_alpha ? 8 : 0;
		write_header(fp, &header);
		flip_image(&header, &image_data);
		write_type2(fp, &header, image_data);
		fclose(fp);
	}
}	/* tga_write */

void tga_free(tga_pixel *image_data)
{
	free(image_data);
}	/* tga_free */

/*
 *		STATIC FUNCTIONS
 */

static void read_header(FILE *fp, tga_header *header)
{
	header->ident_len = get_char(fp);
	header->color_map_type = get_char(fp);
	header->image_type = get_char(fp);
	header->color_map_origin = get_short(fp);
	header->color_map_len = get_short(fp);
	header->color_map_depth = get_char(fp);
	header->x_offset = get_short(fp);
	header->y_offset = get_short(fp);
	header->width = get_short(fp);
	header->height = get_short(fp);
	header->pixel_depth = get_char(fp);
	header->image_descriptor = get_char(fp);
}	/* read_header */

static void write_header(FILE *fp, tga_header *header)
{
	put_char(header->ident_len, fp);
	put_char(header->color_map_type, fp);
	put_char(header->image_type, fp);
	put_short(header->color_map_origin, fp);
	put_short(header->color_map_len, fp);
	put_char(header->color_map_depth, fp);
	put_short(header->x_offset, fp);
	put_short(header->y_offset, fp);
	put_short(header->width, fp);
	put_short(header->height, fp);
	put_char(header->pixel_depth, fp);
	put_char(header->image_descriptor, fp);
}	/* write_header */

#if 0
static void print_header(tga_header *header)
{
	printf("ident_len = %u\n", (uint)header->ident_len);
	printf("color_map_type = %u\n", (uint)header->color_map_type);
	printf("image_type = %u\n", (uint)header->image_type);
	printf("color_map_origin = %u\n", (uint)header->color_map_origin);
	printf("color_map_len = %u\n", (uint)header->color_map_len);
	printf("color_map_depth = %u\n", (uint)header->color_map_depth);
	printf("x_offset = %hd\n", header->x_offset);
	printf("y_offset = %hd\n", header->y_offset);
	printf("width = %hd\n", header->width);
	printf("height = %hd\n", header->height);
	printf("pixel_depth = %u\n", (uint)header->pixel_depth);
	printf("images_descriptor = %x\n", (uint)header->image_descriptor);
}	/* print_header */
#endif

static void read_type1(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data)
{
	tga_pixel *color_map;
	tga_pixel *row;
	long lindex;
	int i, j, index;
	
	skip_data(fp, header->ident_len);
	if (header->color_map_type == 1)
		read_color_map(fp, header, &color_map);
	else
		util_error("Targa file \"%s\" does not contain a color map but is indexed\n", tga_fname);
	*image_data = util_xmalloc((header->width * header->height) * sizeof(tga_pixel));
	row = *image_data;
	for (i = 0; i < header->height; i++) {
		for (j = 0; j < header->width; j++) {
			switch (header->pixel_depth) {
			case 1 ... 8:
				index = get_char(fp);
				row[j] = color_map[index];
				break;
			case 9 ... 16:
				index = get_short(fp);
				row[j] = color_map[index];
				break;
			case 17 ... 24:
				lindex = get_short(fp) << 8;
				lindex += get_char(fp);
				row[j] = color_map[lindex];
				break;
			case 25 ... 32:
				lindex = get_long(fp);
				row[j] = color_map[lindex];
				break;
			default:
				util_error("Targa file \"%s\" has invalid pixel_depth\n", tga_fname);
				break;
			}
			if (!input_alpha)
				row[j].spixel.a = 255;
		}
		row += header->width;
	}
	flip_image(header, image_data);
	free(color_map);
}	/* read_type1 */

static void read_type2(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data)
{
	tga_pixel *color_map;
	tga_pixel *row;
	ushort pix;
	int i, j, index;
	
	skip_data(fp, header->ident_len);
	if (header->color_map_type == 1)
		read_color_map(fp, header, &color_map);
	else {
		color_map = NULL;
		if (header->pixel_depth == 8)
			util_error("Targa file \"%s\" does not contain a color map but is indexed\n", tga_fname);
	}
	
	*image_data = util_xmalloc((header->width * header->height) * sizeof(tga_pixel));
	row = *image_data;
	for (i = 0; i < header->height; i++) {
		for (j = 0; j < header->width; j++) {
			switch (header->pixel_depth) {
			case 8:
				index = get_char(fp);
				row[j] = color_map[index];
				break;
			case 16:
				pix = get_short(fp);
				row[j].spixel.r = scale_color((pix >> 10) & 0x1F);
				row[j].spixel.g = scale_color((pix >> 5) & 0x1F);
				row[j].spixel.b = scale_color(pix & 0x1F);
				row[j].spixel.a = (pix & 0x8000) ? 255 : 0;
				break;
			case 24:
				row[j].spixel.b = get_char(fp);
				row[j].spixel.g = get_char(fp);
				row[j].spixel.r = get_char(fp);
				row[j].spixel.a = 255;
				break;
			case 32:
				row[j].spixel.b = get_char(fp);
				row[j].spixel.g = get_char(fp);
				row[j].spixel.r = get_char(fp);
				row[j].spixel.a = get_char(fp);
				break;
			default:
				util_error("Targa file \"%s\" has invalid pixel_depth\n", tga_fname);
				break;
			}
			if (!input_alpha)
				row[j].spixel.a = 255;
		}
		row += header->width;
	}
	flip_image(header, image_data);
	if (color_map != NULL)
		free(color_map);
}	/* read_type2 */

static void write_type2(FILE *fp, tga_header *header, tga_pixel *image_data)
{
	tga_pixel *row;
	int i, j;
	
	row = image_data;
	for (i = 0; i < header->height; i++) {
		for (j = 0; j < header->width; j++) {
			switch (header->pixel_depth) {
			case 24:
				put_char(row[j].spixel.b, fp);
				put_char(row[j].spixel.g, fp);
				put_char(row[j].spixel.r, fp);
				break;
			case 32:
				put_char(row[j].spixel.b, fp);
				put_char(row[j].spixel.g, fp);
				put_char(row[j].spixel.r, fp);
				put_char(row[j].spixel.a, fp);
				break;
			default:
				util_error("Invalid pixel_depth in write_type2\n");
				break;
			}
		}
		row += header->width;
	}
}	/* write_type2 */

static void read_type9(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data)
{
	tga_pixel *color_map;
	tga_pixel *pix;
	tga_pixel color;
	ulong p, pix_count, lindex;
	uchar rle, count, index;
	
	skip_data(fp, header->ident_len);
	if (header->color_map_type == 1)
		read_color_map(fp, header, &color_map);
	else {
		color_map = NULL;
		if (header->pixel_depth == 8)
			util_error("Targa file \"%s\" does not contain a color map but is indexed\n", tga_fname);
	}
	
	*image_data = util_xmalloc((header->width * header->height) * sizeof(tga_pixel));
	pix_count = header->width * header->height;
	pix = *image_data;
	for (p = 0; p < pix_count;) {
		rle = get_char(fp);
		if ((rle & 0x80) != 0) {
			/* rle packet */
			count = (rle & 0x7F) + 1;
			p += count;
			
			/* get the color to be repeated */
			switch (header->pixel_depth) {
			case 1 ... 8:
				index = get_char(fp);
				color = color_map[index];
				break;
			case 9 ... 16:
				index = get_short(fp);
				color = color_map[index];
				break;
			case 17 ... 24:
				lindex = get_short(fp) << 8;
				lindex += get_char(fp);
				color = color_map[lindex];
				break;
			case 25 ... 32:
				lindex = get_long(fp);
				color = color_map[lindex];
				break;
			default:
				util_error("Targa file \"%s\" has invalid pixel_depth\n", tga_fname);
				break;
			}
			if (!input_alpha)
				color.spixel.a = 255;
			while (count > 0)
				*pix++ = color;
		} else {
			/* raw packet */
			count = rle + 1;			
			p += count;
			
			while (count > 0) {
				switch (header->pixel_depth) {
				case 1 ... 8:
					index = get_char(fp);
					color = color_map[index];
					break;
				case 9 ... 16:
					index = get_short(fp);
					color = color_map[index];
					break;
				case 17 ... 24:
					lindex = get_short(fp) << 8;
					lindex += get_char(fp);
					color = color_map[lindex];
					break;
				case 25 ... 32:
					lindex = get_long(fp);
					color = color_map[lindex];
					break;
				default:
					util_error("Targa file \"%s\" has invalid pixel_depth\n", tga_fname);
					break;
				}
				if (!input_alpha)
					color.spixel.a = 255;
				*pix++ = color;
			}
		}
	}
	flip_image(header, image_data);
	if (color_map != NULL)
		free(color_map);
}	/* read_type9 */

static void read_type10(char *tga_fname, bool input_alpha, FILE *fp, tga_header *header, tga_pixel **image_data)
{
	tga_pixel *color_map;
	tga_pixel *pix;
	tga_pixel color;
	ulong p, pix_count;
	uchar rle, count, index;
	ushort color_16;
	
	skip_data(fp, header->ident_len);
	if (header->color_map_type == 1)
		read_color_map(fp, header, &color_map);
	else {
		color_map = NULL;
		if (header->pixel_depth == 8)
			util_error("Targa file \"%s\" does not contain a color map but is indexed\n", tga_fname);
	}
	
	*image_data = util_xmalloc((header->width * header->height) * sizeof(tga_pixel));
	pix_count = header->width * header->height;
	pix = *image_data;
	for (p = 0; p < pix_count;) {
		rle = get_char(fp);
		if ((rle & 0x80) != 0) {
			/* rle packet */
			count = (rle & 0x7F) + 1;
			p += count;
			
			/* get the color to be repeated */
			switch (header->pixel_depth) {
			case 8:
				index = get_char(fp);
				color = color_map[index];
				break;
			case 16:
				color_16 = get_short(fp);
				color.spixel.r = scale_color((color_16 >> 10) & 0x1F);
				color.spixel.g = scale_color((color_16 >> 5) & 0x1F);
				color.spixel.b = scale_color(color_16 & 0x1F);
				color.spixel.a = (color_16 & 0x8000) ? 255 : 0;
				break;
			case 24:
				color.spixel.b = get_char(fp);
				color.spixel.g = get_char(fp);
				color.spixel.r = get_char(fp);
				color.spixel.a = 255;
				break;
			case 32:
				color.spixel.b = get_char(fp);
				color.spixel.g = get_char(fp);
				color.spixel.r = get_char(fp);
				color.spixel.a = get_char(fp);
				break;
			default:
				util_error("Targa file \"%s\" has invalid pixel_depth\n", tga_fname);
				break;
			}
			
			if (!input_alpha)
				color.spixel.a = 255;
			while (count > 0)
				*pix++ = color;
		} else {
			/* raw packet */
			count = rle + 1;			
			p += count;
			
			while (count > 0) {
				switch (header->pixel_depth) {
				case 8:
					index = get_char(fp);
					color = color_map[index];
					break;
				case 16:
					color_16 = get_short(fp);
					color.spixel.r = scale_color((color_16 >> 10) & 0x1F);
					color.spixel.g = scale_color((color_16 >> 5) & 0x1F);
					color.spixel.b = scale_color(color_16 & 0x1F);
					color.spixel.a = (color_16 & 0x8000) ? 255 : 0;
					break;
				case 24:
					color.spixel.b = get_char(fp);
					color.spixel.g = get_char(fp);
					color.spixel.r = get_char(fp);
					color.spixel.a = 255;
					break;
				case 32:
					color.spixel.b = get_char(fp);
					color.spixel.g = get_char(fp);
					color.spixel.r = get_char(fp);
					color.spixel.a = get_char(fp);
					break;
				default:
					util_error("Targa file \"%s\" has invalid pixel_depth\n", tga_fname);
					break;
				}
				if (!input_alpha)
					color.spixel.a = 255;
				*pix++ = color;
			}
		}
	}
	flip_image(header, image_data);
	if (color_map != NULL)
		free(color_map);
}	/* read_type10 */

static void read_color_map(FILE *fp, tga_header *header, tga_pixel **color_map)
{
	int i;
	
	/* all color are always adjusted to 32 bit */
	*color_map = util_xmalloc(header->color_map_len * sizeof(tga_pixel));
	for (i = 0; i < header->color_map_len; i++) {
		switch (header->color_map_depth) {
		case 15:
		case 16: {
			ushort pix;
			
			pix = get_short(fp);
			(*color_map)[i].spixel.b = scale_color(pix & 0x1F);
			(*color_map)[i].spixel.g = scale_color((pix >> 5) & 0x1F);
			(*color_map)[i].spixel.r = scale_color((pix >> 10) & 0x1F);
			(*color_map)[i].spixel.a = ((header->color_map_depth == 15) || ((pix & 0x8000) != 0)) ? 255 : 0;
			break;
		}
		case 24:
			(*color_map)[i].spixel.b = get_char(fp);
			(*color_map)[i].spixel.g = get_char(fp);
			(*color_map)[i].spixel.r = get_char(fp);
			(*color_map)[i].spixel.a = 255;
			break;
		case 32:
			(*color_map)[i].spixel.b = get_char(fp);
			(*color_map)[i].spixel.g = get_char(fp);
			(*color_map)[i].spixel.r = get_char(fp);
			(*color_map)[i].spixel.a = get_char(fp);
			break;
		}
	}
}	/* read_color_map */

static uchar scale_color(uchar color)
{
	return ((float)color / 31.0F) * 255.0F;
}	/* scale_color */

static void flip_image(tga_header *header, tga_pixel **image_data)
{
	tga_pixel *top, *bottom;
	tga_pixel *trow;
	int count;
	
	trow = util_xmalloc(header->width * sizeof(tga_pixel));
	top = *image_data;
	bottom = *image_data + ((header->height - 1) * header->width);
	for (count = 0; count < header->height / 2; count++) {
		memcpy(trow, top, header->width * sizeof(tga_pixel));
		memcpy(top, bottom, header->width * sizeof(tga_pixel));
		memcpy(bottom, trow, header->width * sizeof(tga_pixel));
		top += header->width;
		bottom -= header->width;
	}
	free(trow);
}	/* flip_image */

static uchar get_char(FILE *fp)
{
	uchar data;
	
	if (fread(&data, sizeof(data), 1, fp) != 1)
		util_error("get_char:error reading from TGA file\n");
	return data;
}	/* get_char */

static ushort get_short(FILE *fp)
{
	ushort data;
	
	if (fread(&data, sizeof(data), 1, fp) != 1)
		util_error("get_short:error reading from TGA file\n");
	return data;
}	/* get_short */

static ulong get_long(FILE *fp)
{
	ulong data;
	
	if (fread(&data, sizeof(data), 1, fp) != 1)
		util_error("get_long:error reading from TGA file\n");
	return data;
}	/* get_long */

static void skip_data(FILE *fp, int count)
{
	uchar dummy;
	
	while (count-- > 0)
		if (fread(&dummy, sizeof(dummy), 1, fp) != 1)
			util_error("skip_data:error reading from TGA file\n");
}	/* skip_data */

static void put_char(uchar data, FILE *fp)
{
	if (fwrite(&data, sizeof(data), 1, fp) != 1)
		util_error("put_char:error writing to TGA file\n");
}	/* put_char */

static void put_short(ushort data, FILE *fp)
{
	if (fwrite(&data, sizeof(data), 1, fp) != 1)
		util_error("put_short:error writing to TGA file\n");
}	/* put_short */

#if 0
static void put_long(ulong data, FILE *fp)
{
	if (fwrite(&data, sizeof(data), 1, fp) != 1)
		util_error("put_long:error writing to TGA file\n");
}	/* put_long */
#endif

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */
