/*
 *		$Archive: /video/tools/c_hack/c_hack.c $
 *		$Revision: 7 $
 *		$Date: 7/15/98 4:52p $
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
char *ss_c_hack_c = "$Workfile: c_hack.c $ $Revision: 7 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/*
 *		USER INCLUDES
 */

#include "system.h"
#include "pack.h"
#include "tga.h"
#include "util.h"

/*
 *		DEFINES
 */

#define BUFFER_SIZE		1024

/*
 *		TYPEDEFS
 */

enum {
	UNKNOWN_COMMAND,
	TGA_PATH,
	INC_PATH,
	TGA_COUNT,
	PACK_WIDTH,
	PACK_HEIGHT,
	ST,
	PACK_TEXTURES,
	PACK,
	PATCH_PLYR_H,
	PATCH_JERSEYS_H
};

typedef struct {
	const char *token_str;
	const int token_id;
} token;

typedef struct tga_element_tag {
	struct tga_element_tag *file_next;
	char tga_fname[FILENAME_MAX];
	tga_header header;
	tga_pixel *image;
	int player_flag;
	int texture_index;
	int new_left;
	int new_top;
} tga_element;

typedef struct {
	int player_flag;
	int texture_index;
	int new_left;
	int new_top;
	int orig_width;
	int orig_height;
} tga_info;
/*
 *		STATIC PROTOTYPES
 */

static void usage(char *prog_name);
static void init_globals(void);
static void process_ld_file(FILE *fp);
static int get_cmd(char *buffer, int len);
static bool get_param(char *buffer, int len, char *param);
static bool read_line(char *buffer, size_t buffer_size, FILE *fp, int *len, bool honor_comments);
static void write_line(char *buffer, FILE *fp);
static void filter_st(char *str);
static bool str_similiar(const char *a, const char *b);
static char *add_file_ext(char *file_name, const char *file_ext);
static void check_path(char *path);
static int get_param_count(char *buffer);
static void get_param_num(char *buffer, int param_num, char *param);
static tga_pixel *pack_textures(void);
static void back_patch_inc(char *header_file_name, bool is_plyr_file);
static void find_map_scale(int map_width, int map_height, float *s_scale, float *t_scale);

/*
 *		STATIC VARIABLES
 */

static const token l3d_cmds[] = {
	{"TGA_PATH>", TGA_PATH},			/* path to find .TGA texture map files */
	{"INC_PATH>", INC_PATH},			/* path to find processed model headers */
	{"TGA_COUNT>", TGA_COUNT},			/* number of TGAs to pack */
	{"PACK_WIDTH>", PACK_WIDTH},		/* page width to pack in */
	{"PACK_HEIGHT>", PACK_HEIGHT},		/* page height to pack in */
	{"PACK_TEXTURES>", PACK_TEXTURES},	/* specify the texture input TGAs */
	{"PACK>", PACK},					/* pack all of the texture maps */
	{"PATCH_PLYR_H>", PATCH_PLYR_H},	/* back patch the player header file */
	{"PATCH_JERSEYS_H>", PATCH_JERSEYS_H}/* back patch the jersey header file */
};

static char tga_path[BUFFER_SIZE + 1];
static char inc_path[BUFFER_SIZE + 1];
static int pack_width;
static int pack_height;
static int tga_count;
static tga_element *tga_cache;
static tga_info offset_list[100];
static int offset_list_count;
static bool offset_list_valid;

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	char fname[FILENAME_MAX];
	FILE *fp;
	
	if (argc != 2)
		usage(argv[0]);
	strncpy(fname, argv[1], FILENAME_MAX);
	add_file_ext(fname, ".ld");
	fp = fopen(fname, "r");
	if (fp == NULL)
		util_error("%s could not open \"%s\"\n", argv[0], argv[1]);
	else {
		init_globals();
		process_ld_file(fp);
		fclose(fp);
	}
	return EXIT_SUCCESS;
}	/* main */

/*
 *		STATIC FUNCTIONS
 */

static void usage(char *prog_name)
{
	util_error("usage: %s xxx.ld\n", prog_name);
}	/* usage */

static void init_globals(void)
{
	strcpy(tga_path, "\\video\\nfl\\img3d\\");
	strcpy(inc_path, "\\video\\nfl\\models\\");
	
	pack_width = 128;
	pack_height = 256;
	
	tga_count = 0;
	
	tga_cache = NULL;
	offset_list_count = 0;
	offset_list_valid = FALSE;
}	/* init_globals */

static void process_ld_file(FILE *fp)
{
	bool at_eof;
	int len, token;
	char buffer[BUFFER_SIZE + 1];
	char param[BUFFER_SIZE + 1];
	
	at_eof = FALSE;
	while (!at_eof) {
		at_eof = read_line(buffer, sizeof(buffer), fp, &len, TRUE);
		if (len > 0) {
			token = get_cmd(buffer, len);
			switch (token) {
			case TGA_PATH:
				/* TGA_PATH> \video\nba\img3d\ */
				if (get_param(buffer, len, param)) {
					strcpy(tga_path, param);
					check_path(tga_path);
				} else
					util_error("no path specified for TGA_PATH>, \"%s\"\n", buffer);
				break;
			case INC_PATH:
				/* INC_PATH> \video\nba\models\ */
				if (get_param(buffer, len, param)) {
					strcpy(inc_path, param);
					check_path(inc_path);
				} else
					util_error("no path specified for INC_PATH>, \"%s\"\n", buffer);
				break;
			case TGA_COUNT:
				/* TGA_COUNT> 4 */
				if (get_param(buffer, len, param)) {
					tga_count = atoi(param);
					if (tga_count < 1)
						util_error("invalid tga count, must be greater than zero, \"%s\"\n", buffer);
				} else
					util_error("no value specified for TGA_COUNT>, \"%s\"\n", buffer);
				break;
			case PACK_WIDTH:
				/* PACK_WIDTH> 256 */
				if (get_param(buffer, len, param)) {
					pack_width = atoi(param);
					if ((pack_width != 1) && (pack_width != 2) && (pack_width != 4)
						&& (pack_width != 8) && (pack_width != 16)  && (pack_width != 32)
						&& (pack_width != 64) && (pack_width != 128)  && (pack_width != 256))
						util_error("invalid pack width, must be a power of two, \"%s\"\n", buffer);
				} else
					util_error("no value specified for PACK_WIDTH>, \"%s\"\n", buffer);
				break;
			case PACK_HEIGHT:
				/* PACK_HEIGHT> 256 */
 				if (get_param(buffer, len, param)) {
					pack_height = atoi(param);
					if ((pack_height != 1) && (pack_height != 2) && (pack_height != 4)
						&& (pack_height != 8) && (pack_height != 16)  && (pack_height != 32)
						&& (pack_height != 64) && (pack_height != 128)  && (pack_height != 256))
						util_error("invalid pack height, must be a power of two, \"%s\"\n", buffer);
				} else
					util_error("no value specified for PACK_HEIGHT>, \"%s\"\n", buffer);
				break;
			case PACK_TEXTURES:
				if (tga_count != 0) {
					char tga_name[BUFFER_SIZE + 1];
					char tga_fullname[FILENAME_MAX];
					tga_element *file_elem;
					int i, cnt, list_count;
					
					if (get_param(buffer, len, param)) {
						cnt = get_param_count(param);
						if (cnt != tga_count)
							util_error("PACK_TEXTURES> list size != TGA_COUNT>, \"%s\"\n", buffer);
						
						list_count = 0;
						tga_cache = NULL;
						for (i = 0; i < cnt; i++) {
							get_param_num(param, i, tga_name);
							
							/* add the texture file name to the linked list */
							file_elem = util_xmalloc(sizeof(tga_element));
							file_elem->file_next = tga_cache;
							file_elem->image = NULL;
							file_elem->player_flag = i != 3;
							file_elem->texture_index = i == 3 ? 0 : i + 2;
							file_elem->new_left = -1;
							file_elem->new_top = -1;
							strcpy(file_elem->tga_fname, tga_name);
							
							/* append the tga_path and open the TGA */
							strcpy(tga_fullname, tga_path);
							strcat(tga_fullname, file_elem->tga_fname);
							
							/* read in the tga */
							printf("Loading texture map \"%s\"\n", tga_fullname);
							tga_read(tga_fullname, (i == 2) || (i == 3), &file_elem->header, &file_elem->image);
							
							tga_cache = file_elem;
							list_count++;
						}
					} else
						util_error("TGA_COUNT> must be set before PACK_TEXTURES>, \"%s\"\n", buffer);
				}
				break;
			case PACK: {
				/* PACK> zzz.tga */
				char tga_fullname[FILENAME_MAX];
				tga_pixel *image_data;
				tga_element *tga_el;
				int mn, mx, cnt;
				
				if (get_param(buffer, len, param)) {
					add_file_ext(param, ".tga");
					mn = MIN(pack_width, pack_height);
					mx = MAX(pack_width, pack_height);
					if ((mn != mx) && ((mn * 2) != mx) && ((mn * 4) != mx) && ((mn * 8) != mx))
						util_error("pack width %d and pack height %d must be 1:1, 1:2|2:1, 1:4|4:1, or 1:8|8:1\n", pack_width, pack_height);
					image_data = pack_textures();
					
					if (offset_list_valid) {
						cnt = 0;
						for (tga_el = tga_cache; tga_el != NULL; tga_el = tga_el->file_next) {
							if (offset_list[cnt].player_flag != tga_el->player_flag)
								util_error("packing mismatch1!\n");
							if (offset_list[cnt].texture_index != tga_el->texture_index)
								util_error("packing mismatch2!\n");
							if (offset_list[cnt].new_left != tga_el->new_left)
								util_error("packing mismatch3!\n");
							if (offset_list[cnt].new_top != tga_el->new_top)
								util_error("packing mismatch4!\n");
							if (offset_list[cnt].orig_width != tga_el->header.width)
								util_error("packing mismatch5!\n");
							if (offset_list[cnt].orig_height != tga_el->header.height)
								util_error("packing mismatch6!\n");
							cnt++;
						}
						if (cnt != offset_list_count)
							util_error("packing mismatch7!\n");
					} else {
						offset_list_count = 0;
						for (tga_el = tga_cache; tga_el != NULL; tga_el = tga_el->file_next) {
							offset_list[offset_list_count].player_flag = tga_el->player_flag;
							offset_list[offset_list_count].texture_index = tga_el->texture_index;
							offset_list[offset_list_count].new_left = tga_el->new_left;
							offset_list[offset_list_count].new_top = tga_el->new_top;
							offset_list[offset_list_count].orig_width = tga_el->header.width;
							offset_list[offset_list_count].orig_height = tga_el->header.height;
							offset_list_count++;
						}
						offset_list_valid = TRUE;
					}
					
					/* append the tga_path and open the TGA */
					strcpy(tga_fullname, tga_path);
					strcat(tga_fullname, param);
					tga_write(tga_fullname, pack_width, pack_height, TRUE, image_data);
					printf("Texture maps packed to \"%s\"\n", param);
					free(image_data);
					
					for (tga_el = tga_cache; tga_el != NULL;) {
						tga_element *t;
						
						t = tga_el->file_next;
						free(tga_el->image);
						free(tga_el);
						tga_el = t;
					}
					tga_cache = NULL;
				} else
					util_error("no output file name for PACK>, \"%s\"\n", buffer);
				break;
			}			

			case PATCH_PLYR_H:
			case PATCH_JERSEYS_H:
				if (!offset_list_valid)
					util_error("PACK> must be done before PATCH>\n");
				if (get_param(buffer, len, param))
					back_patch_inc(param, token == PATCH_PLYR_H);
				else
					util_error("no header file name for PATCH_PLYR_H>, \"%s\"\n", buffer);
				break;
			case UNKNOWN_COMMAND:
			default:
				util_error("unknown command, \"%s\"\n", buffer);
				break;
			}
		}
	}
}	/* process_ld_file */

static int get_cmd(char *buffer, int len)
{
	char token[BUFFER_SIZE + 1];
	int i;
	
	strcpy(token, buffer);
	for (i = 0; i < len; i++)
		if (token[i] == '>') {
			token[i + 1] = '\0';
			break;
		}
	for (i = 0; i < ARRAY_SIZE(l3d_cmds); i++) {
		if (str_similiar(l3d_cmds[i].token_str, token))
			return l3d_cmds[i].token_id;
	}
	return 	UNKNOWN_COMMAND;
}	/* get_cmd */

static bool get_param(char *buffer, int len, char *param)
{
	int i;
	
	for (i = 0; i < len; i++)
		if (buffer[i] == '>') {
			i++;
			break;
		}
	while ((i < len) && isspace(buffer[i]))
		   i++;
	if (i < len) {
		strcpy(param, &buffer[i]);
		return TRUE;
	} else
		return FALSE;
}	/* get_param */

static bool read_line(char *buffer, size_t buffer_size, FILE *fp, int *len, bool honor_comments)
{
	bool at_eof;
	int ch, i;
	
	at_eof = FALSE;
	buffer_size--;
	i = 0;
	while (i < buffer_size) {
		ch = fgetc(fp);
		/* check for end of file */
		if (ch == EOF) {
			at_eof = TRUE;
			break;
		}
		/* check for line break */
		if (ch == '\r' || ch == '\n')
			break;
		/* store the character into the buffer */
		buffer[i] = ch;
		i++;
	}

	/* check for a comment line */
	if (honor_comments)
		if ((i > 0) && (buffer[0] == '#'))
			i = 0;
	buffer[i] = '\0';
	*len = i;
	return at_eof;
}	/* read_line */

static void write_line(char *buffer, FILE *fp)
{
	int len;
	
	len = strlen(buffer);
	fwrite(buffer, len, 1, fp);
	fputc('\n', fp);
}	/* write_line */

static void filter_st(char *str)
{
	int i, j;
	
	for (i = 0; i < strlen(str); i++)
		if ((str[i] == 'f') || (str[i] == 'F'))
			for (j = i; j <= strlen(str); j++)
				str[j] = str[j + 1];
}	/* filter_st */

static bool str_similiar(const char *a, const char *b)
{
	while (*a != '\0') {
		if (toupper(*a) != toupper(*b))
			return FALSE;
		a++;
		b++;
	}
	return (*b == '\0');
}	/* str_similiar */

static char *add_file_ext(char *file_name, const char *file_ext)
{
	char *str;
	
	for (str = file_name; (*str != '.') && (*str != '\0'); str++)
		;
	if (*file_ext != '.')
		*str++ = '.';
	while (*file_ext != '\0')
		*str++ = *file_ext++;
	*str = '\0';
	return file_name;
}	/* add_file_ext */

static void check_path(char *path)
{
	struct stat s;
	int i, len;

	len = strlen(path);
	for (i = 0; i < len; i++)
		if (path[i] == '/')
			path[i] = '\\';
	if (path[len - 1] != '\\') {
		path[len] = '\\';
		path[len + 1] = '\0';
	}
	stat(path, &s);
	if (!S_ISDIR(s.st_mode))
		util_error("%s is not a directory or valid path\n", path);
}	/* check_path */

static int get_param_count(char *buffer)
{
	int i, slen, param_count;
	
	param_count = 0;
	slen = strlen(buffer);
	for (i = 0; i < slen;) {
		/* skip spaces */
		while ((i < slen) && isspace(buffer[i]))
			i++;
		
		if (i < slen) {
			param_count++;
			while ((i < slen) && isgraph(buffer[i]))
				i++;
		}
	}
	return param_count;
}	/* get_param_count */

static void get_param_num(char *buffer, int param_num, char *param)
{
	int i, j, slen;
	
	slen = strlen(buffer);
	j = 0;
	for (i = 0; i < param_num; i++) {
		/* skip spaces */
		while ((j < slen) && isspace(buffer[j]))
			j++;
		
		while ((j < slen) && isgraph(buffer[j]))
			j++;
	}
	while ((j < slen) && isspace(buffer[j]))
		j++;
	i = 0;
	while ((j < slen) && isgraph(buffer[j]))
		param[i++] = buffer[j++];
	param[i] = '\0';
}	/* get_param_num */

static tga_pixel *pack_textures(void)
{
	int texture_count, i, j, k, err;
	int piece_width, piece_height;
	pack_params params;
	pack_stats stats;
	tga_element *elem;
	block_t *block_list;
	tga_pixel *image_data, *piece_data, *dst_data;
	
	/* set up the packing parameters */
	params.pack_mode = PAGE_MODE;
	params.block_align = 1;
	params.page_phy_width = pack_width;
	params.page_phy_height = pack_height;
	params.left_inset = 0;
	params.top_inset = 0;
	params.right_inset = 0;
	params.bottom_inset = 0;
	params.debug = FALSE;
	
	/* build the list of blocks to pack */
	texture_count = 0;
	for (elem = tga_cache; elem != NULL; elem = elem->file_next)
		texture_count++;
	block_list = util_xmalloc(texture_count * sizeof(block_t));
	i = 0;
	for (elem = tga_cache; elem != NULL; elem = elem->file_next) {
		block_list[i].block_width = elem->header.width;
		block_list[i].block_height = elem->header.height;
		block_list[i].block_data = elem;
		i++;
	}
	/* pack the blocks into the new texture page */
	err = pack(texture_count, block_list, &params, &stats);

	if (err == PACK_NO_ERR) {
		if (stats.page_count > 1)
			util_error("error, the TGAs packed into %d pages of %d by %d pixels\n", stats.page_count, pack_width, pack_height);
		
		/* transfer the pixels for the individuals TGA into a larger one */
		image_data = util_xmalloc((pack_width * pack_height) * sizeof(tga_pixel));
		for (i = 0; i < pack_width * pack_height; i++) {
			image_data[i].spixel.a = 255;
			image_data[i].spixel.r = 0;
			image_data[i].spixel.g = 0;
			image_data[i].spixel.b = 0;
		}
		
		for (i = 0; i < texture_count; i++) {
			((tga_element *)block_list[i].block_data)->new_left = block_list[i].block_x_offset;
			((tga_element *)block_list[i].block_data)->new_top = block_list[i].block_y_offset;
			piece_width = ((tga_element *)block_list[i].block_data)->header.width;
			piece_height = ((tga_element *)block_list[i].block_data)->header.height;
			piece_data = ((tga_element *)block_list[i].block_data)->image;
			dst_data = image_data + (block_list[i].block_y_offset * pack_width) + block_list[i].block_x_offset;
			
			for (j = 0; j < piece_height; j++) {
				for (k = 0; k < piece_width; k++)
					dst_data[k] = piece_data[k];
				dst_data += pack_width;
				piece_data += piece_width;
			}
		}
		free(block_list);
	} else {
		image_data = NULL;
		switch (err) {
		case PACK_NO_PARAM_BLOCK:
			util_error("No parameter block passed\n");
			break;			
		case PACK_BAD_BLOCK_COUNT:
			util_error("Invalid block count passed\n");
			break;			
		case PACK_NO_BLOCK_LIST:
			util_error("No block list passed\n");
			break;
		case PACK_BAD_PACK_MODE:
			util_error("Bad packing mode\n");
			break;
		case PACK_BAD_BLOCK_ALIGN:
			util_error("Bad block alignment\n");
			break;
		case PACK_BAD_PAGE_PHY_WIDTH:
			util_error("Bad page width\n");
			break;
		case PACK_BAD_PAGE_PHY_HEIGHT:
			util_error("Bad page height\n");
			break;
		case PACK_BAD_LEFT_INSET:
			util_error("Bad left inset\n");
			break;
		case PACK_BAD_TOP_INSET:
			util_error("Bad top inset\n");
			break;
		case PACK_BAD_RIGHT_INSET:
			util_error("Bad right inset\n");
			break;
		case PACK_BAD_BOTTOM_INSET:
			util_error("Bad bottom inset\n");
			break;
		case PACK_BAD_BLOCK_WIDTH:
			util_error("Bad block width\n");
			break;
		case PACK_BAD_BLOCK_HEIGHT:
			util_error("Bad bock height\n");
			break;
		default:
			util_error("Unknown error occurred\n");
			break;
		}
	}
	return image_data;
}	/* pack_textures */

/* need to alter the .h file */
/* all ST need to be adjust, to reflect the movement of the origin of their texture block */
/* the ST in the array can refer to different texture maps */
/* we need to determine which ST is associated with which texture */

/* read and analyze the TRI decl to determine the ST map ownership */
/* read, convert, and rewrite the ST decl */

/* if !normalized_st then just add top left offset */
/* otherwise convert from 0.0-1.0 to pixel space add offset and convert back */
static void back_patch_inc(char *header_file_name, bool is_plyr_file)
{
	char old_fname[FILENAME_MAX];
	char new_fname[FILENAME_MAX];
	char syscmd[FILENAME_MAX + 40];
	char buffer[BUFFER_SIZE + 1];
	char *str;
	FILE *old_fp, *new_fp;
	bool at_eof;
	int len, i, current_tga;
	int cnt, v0, v1, v2, st0, st1, st2, t;
	int *st_tga, max_st;
	int st_index;
	float three_dfx_s, three_dfx_t;
	tga_info *current_tga_info;
	
	printf("Backpatching model header file \"%s\"\n", header_file_name);
	
	st_tga = util_xmalloc(4096 * sizeof(int));
	max_st = -1;
	current_tga = -1;
	for (i = 0; i < 4096; i++)
		st_tga[i] = -1;
	/* build the absolute path to the existing model include file */
	strcpy(old_fname, inc_path);
	strcat(old_fname, header_file_name);
	old_fp = fopen(old_fname, "r");
	if (old_fp == NULL)
		util_error("could not reopen the model header file \"%s\"\n", old_fname);
	
	/* build the absolute path to the new model include file */
	strcpy(new_fname, old_fname);
	add_file_ext(new_fname, ".XhX");
	new_fp = fopen(new_fname, "w");
	if (new_fp == NULL)
		util_error("could not create the new model header file \"%s\"\n", new_fname);
	
	at_eof = FALSE;
	while (!at_eof) {
		at_eof = read_line(buffer, sizeof(buffer), old_fp, &len, FALSE);
		if (len > 0) {
			/* parse the line, it is in the form of: */
			/*	{V0,V1,V2, ST0,ST1,ST2, T}, */
			/* if T >= 0 then T/4 is the index of the texture for the ST until next positive T */
			/* if T < = then */
			/*		-4 ST0, ST1, ST2 are valid */
			/*		-8 ST0, ST1 are valid */
			/*		-12 ST0 is valid */
			/*		-20 end of triangle data structure */
			
			/* TRI hand_r_tris[] = */
			/* { */
			/* \t{0,3,6, 0,1,2, 8}, */
			/* \t{132,21,18, 56,57,58, -4}, */
			/* \t{129,135,48, 36,48,255, -8}, */
			/* \t{129,15,0, 34,255,255, -12}, */
			/* \t{135,51,48, 255,255,255, -16}, */
			/* \t{0, 0, 0, 0, 0, 0, -20} */
			/* }; */
			if (strncmp(buffer, "TRI ", 4) == 0) {
				max_st = -1;
				current_tga = -1;
				for (i = 0; i < 4096; i++)
					st_tga[i] = -1;
				write_line(buffer, new_fp);						/* echo the line to the new file */
				while (!at_eof) {
					at_eof = read_line(buffer, sizeof(buffer), old_fp, &len, FALSE);
					write_line(buffer, new_fp);					/* echo the line to the new file */
					
					str = strstr(buffer, "}");
					if (str != NULL) {
						str = strstr(buffer, "{");
						cnt = sscanf(str, "{%d,%d,%d,%d,%d,%d,%d}", &v0, &v1, &v2, &st0, &st1, &st2, &t);
						if (cnt != 7) {
							printf("line = %s\n", buffer);
							util_error("error sscanfing TRIs from the model header file\n");
						}
						if (t < 0) {
							/* check for the end of structure */
							if (t == -20)
								break;
							if (current_tga == -1)
								util_error("using current_tga before it is set!\n");
							if (t == -4) {
								st_tga[st0] = current_tga;
								if (st0 > max_st)
									max_st = st0;
								st_tga[st1] = current_tga;
								if (st1 > max_st)
									max_st = st1;
								st_tga[st2] = current_tga;
								if (st2 > max_st)
									max_st = st2;
							} else if (t == -8) {
								st_tga[st0] = current_tga;
								if (st0 > max_st)
									max_st = st0;
								st_tga[st1] = current_tga;
								if (st1 > max_st)
									max_st = st1;
							} else if (t == -12) {
								st_tga[st0] = current_tga;
								if (st0 > max_st)
									max_st = st0;
							} else if (t == -16) {
							} else {
								util_error("invalid T valid in model header file\n");
							}
						} else {
							current_tga = t / 4;
							st_tga[st0] = current_tga;
							if (st0 > max_st)
								max_st = st0;
							st_tga[st1] = current_tga;
							if (st1 > max_st)
								max_st = st1;
							st_tga[st2] = current_tga;
							if (st2 > max_st)
								max_st = st2;
						}
					}
				}
				if (is_plyr_file)
					for (i = 0; i <= max_st; i++)
						if (st_tga[i] == -1)
							util_error("tga_texture not found for st_tga[%d]\n", i);
			} else if (strncmp(buffer, "ST ", 3) == 0) {
				/* ST hand_r_st[] = */
				/* { */
				/* \t{51.754753f,13.252090f}, */
				/* \t{59.383297f,8.278015f} */
				/* }; */
				write_line(buffer, new_fp);						/* echo the line to the new file */
				st_index = 0;
				
				while (!at_eof) {
					at_eof = read_line(buffer, sizeof(buffer), old_fp, &len, FALSE);
					str = strstr(buffer, "};");
					if (str != NULL) {
						write_line(buffer, new_fp);
						break;
					}
					str = strstr(buffer, "}");
					if (str != NULL) {
						str = strstr(buffer, "{");
						filter_st(str);
						cnt = sscanf(str, "{%f,%f}", &three_dfx_s, &three_dfx_t);
						if (cnt != 2)
							util_error("error sscanfing STs from the model header file\n");
						
						if (st_index > max_st)
							util_error("encountered a st_index > max_st\n");

						if (is_plyr_file) {
							if (st_tga[st_index] > 4)
								util_error("encountered an index > tga_count\n");
						} else {
							if (st_tga[st_index] > 1)
								util_error("encountered an index > tga_count\n");
						}
						
						current_tga_info = NULL;
						for (i = 0; i < offset_list_count; i++) {
							if (is_plyr_file) {
								if (offset_list[i].player_flag && (st_tga[st_index] == offset_list[i].texture_index)) {
									current_tga_info = &offset_list[i];
									break;
								}
							} else {
								if (!offset_list[i].player_flag) {
									current_tga_info = &offset_list[i];
									break;
								}
							}
						}

						if (current_tga_info != NULL) {
							float map_s, map_t;
							float map_x, map_y;
							float s_scale, t_scale;
							
							if (is_plyr_file) {
								/* find the proper scale for the map */
								find_map_scale(current_tga_info->orig_width, current_tga_info->orig_height, &s_scale, &t_scale);
								/* convert them to the map s and t values */
								map_s = three_dfx_s / s_scale;
								map_t = three_dfx_t / t_scale;
								
								/* convert to pixel coords */
								map_x = map_s * current_tga_info->orig_width;
								map_y = map_t * current_tga_info->orig_height;
								
								/* offset the pixel coords into the packed map */
								map_x += current_tga_info->new_left;
								map_y += current_tga_info->new_top;
								
								/* convert to map s and values */
								map_s = map_x / pack_width;
								map_t = map_y / pack_height;
								
								/* find the new scale for the map */
								find_map_scale(pack_width, pack_height, &s_scale, &t_scale);
								
								/* convert to 3Dfx s and t values */
								three_dfx_s = map_s * s_scale;
								three_dfx_t = map_t * t_scale;
							} else {
								/* convert to pixel coords */
								map_x = three_dfx_s * current_tga_info->orig_width;
								map_y = three_dfx_t * current_tga_info->orig_height;
								
								/* offset the pixel coords into the packed map */
								map_x += current_tga_info->new_left;
								map_y += current_tga_info->new_top;
								
								/* convert to map s and values */
								three_dfx_s = map_x / pack_width;
								three_dfx_t = map_y / pack_height;
							}
						}
						fprintf(new_fp, "\t{%ff,%ff}%s\n", three_dfx_s, three_dfx_t, strstr(buffer, "},") != NULL ? "," : "");
						st_index++;
					} else
						write_line(buffer, new_fp);						/* echo the line to the new file */
				}
			} else {
				write_line(buffer, new_fp);
			}
		} else
			write_line(buffer, new_fp);
	}
	fclose(old_fp);
	fclose(new_fp);
	
	strcpy(syscmd, "attrib -r ");
	strcat(syscmd, old_fname);
	system(syscmd);
	
	remove(old_fname);
	rename(new_fname, old_fname);
	
//	strcpy(syscmd, "attrib +r ");
//	strcat(syscmd, old_fname);
//	system(syscmd);
	
	free(st_tga);
}	/* back_patch_inc */

static void find_map_scale(int map_width, int map_height, float *s_scale, float *t_scale)
{
	if ((map_width / map_height) == 1) {
		*s_scale = 256.0F;
		*t_scale = 256.0F;
	} else if ((map_width / map_height) == 2) {
		*s_scale = 256.0F;
		*t_scale = 128.0F;
	} else if ((map_height / map_width) == 2) {
		*s_scale = 128.0F;
		*t_scale = 256.0F;
	} else if ((map_width / map_height) == 4) {
		*s_scale = 256.0F;
		*t_scale = 64.0F;
	} else if ((map_height / map_width) == 4) {
		*s_scale = 64.0F;
		*t_scale = 256.0F;
	} else if ((map_width / map_height) == 8) {
		*s_scale = 256.0F;
		*t_scale = 32.0F;
	} else if ((map_height / map_width) == 8) {
		*s_scale = 32.0F;
		*t_scale = 256.0F;
	} else
		util_error("find_map_scale: map_width = %d, map_height = %d\n", map_width, map_height);
}	/* find_map_scale */

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */
