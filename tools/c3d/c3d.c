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
char *ss_c3d_c = "$Workfile: $ $Revision: $";
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
	SDL_PATH,
	LWO_PATH,
	UV_PATH,
	INC_PATH,
	SDL_CVT,
	SDL_PARAM,
	SDL,
	LWO_CVT,
	LWO_PARAM,
	LWO,
	PACK_WIDTH,
	PACK_HEIGHT,
	PACK,
	FILENAME,
	BASE_FILENAME,
	BASE_FILENAME_H,
	HDLESS_BASE_FILENAME
};

typedef struct {
	const char *token_str;
	const int token_id;
} token;

typedef struct tga_element_tag {
	struct tga_element_tag *file_next;
	struct tga_element_tag *global_next;
	char tga_fname[FILENAME_MAX];
	tga_header header;
	tga_pixel *image;
	int texture_index;
	int new_left;
	int new_top;
} tga_element;

typedef struct inc_element_tag {
	struct inc_element_tag *next;
	char inc_fname[FILENAME_MAX];
	bool normalized_st;
	int tga_count;
	tga_element *tga_ref;
} inc_element;

/*
 *		STATIC PROTOTYPES
 */

static void usage(char *prog_name);
static void init_globals(void);
static void process_l3d_file(FILE *fp);
static int get_cmd(char *buffer, int len);
static bool get_param(char *buffer, int len, char *param);
static int get_param_cmd(char *buffer, int len);
static bool read_line(char *buffer, size_t buffer_size, FILE *fp, int *len, bool honor_comments);
static void write_line(char *buffer, FILE *fp);
static void filter_st(char *str);
static bool str_similiar(const char *a, const char *b);
static char *add_file_ext(char *file_name, const char *file_ext);
static void check_path(char *path);
static char *cvt_filename(char *fname, int cmd);
static void base_fname(char *fname);
static int get_param_count(char *buffer);
static void get_param_num(char *buffer, int param_num, char *param);
static void cache_inc(char *new_inc, bool normalized);
static int scan_inc_for_textures(char *new_inc, tga_element **tga_ref);
static void cache_tga(tga_element *file_head);
static tga_pixel *pack_textures(void);
static void back_patch_inc(inc_element *inc, char *new_texture_name);

/*
 *		STATIC VARIABLES
 */

static const token l3d_cmds[] = {
	{"TGA_PATH>", TGA_PATH},			/* path to find .TGA texture map files */
	{"SDL_PATH>", SDL_PATH},			/* path to find .SDL model files */
	{"LWO_PATH>", LWO_PATH},			/* path to find .LWO model files */
	{"UV_PATH>", UV_PATH},				/* path to find .UV texel map files */
	{"INC_PATH>", INC_PATH},			/* path to find processed model headers */
	{"SDL_CVT>", SDL_CVT},				/* program to convert .SDL models */
	{"SDL_PARAM>", SDL_PARAM},			/* params to pass to .SDL converter */
	{"SDL>", SDL},						/* process a .SDL model file */
	{"LWO_CVT>", LWO_CVT},				/* program to convert .LWO models */
	{"LWO_PARAM>", LWO_PARAM},			/* params to pass to .LWO converter */
	{"LWO>", LWO},						/* process a .LWO model file */
	{"PACK_WIDTH>", PACK_WIDTH},		/* page width to pack in */
	{"PACK_HEIGHT>", PACK_HEIGHT},		/* page height to pack in */
	{"PACK>", PACK}						/* pack all of the processed model texture maps */
};

static const token param_cmds[] = {
	{"$FILENAME", FILENAME},			/* the file name */
	{"$BASE_FILENAME", BASE_FILENAME},	/* strip the extension from the file name */
	{"$BASE_FILENAME_H", BASE_FILENAME_H},	/* strip the extension from the file name and append .h */
	{"$HDLESS_BASE_FILENAME", HDLESS_BASE_FILENAME}	/* strip the HD_ and extension from the file name */
};

static char tga_path[BUFFER_SIZE + 1];
static char sdl_path[BUFFER_SIZE + 1];
static char lwo_path[BUFFER_SIZE + 1];
static char uv_path[BUFFER_SIZE + 1];
static char inc_path[BUFFER_SIZE + 1];
static char sdl_cvt[BUFFER_SIZE + 1];
static char sdl_param[BUFFER_SIZE + 1];
static char lwo_cvt[BUFFER_SIZE + 1];
static char lwo_param[BUFFER_SIZE + 1];
static int pack_width;
static int pack_height;
static tga_element *tga_cache;
static inc_element *inc_cache;
static bool sdl_normalized;
static bool lwo_normalized;
static bool silent_operation;

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	char fname[FILENAME_MAX];
	FILE *fp;
	
	silent_operation = FALSE;
	if ((argc < 2) || (argc > 3))
		usage(argv[0]);
	if (argc == 3) {
		if (strcmp(argv[1], "-s") == 0) {
			silent_operation = TRUE;
			strncpy(fname, argv[2], FILENAME_MAX);
		} else
			usage(argv[0]);
	} else {
		strncpy(fname, argv[1], FILENAME_MAX);
	}
	add_file_ext(fname, ".l3d");
	fp = fopen(fname, "r");
	if (fp == NULL)
		util_error("%s could not open \"%s\"\n", argv[0], argv[1]);
	else {
		init_globals();
		process_l3d_file(fp);
		fclose(fp);
	}
	return EXIT_SUCCESS;
}	/* main */

/*
 *		STATIC FUNCTIONS
 */

static void usage(char *prog_name)
{
	util_error("usage: %s [-s] xxx.l3d\n", prog_name);
}	/* usage */

static void init_globals(void)
{
	strcpy(tga_path, "\\video\\nba\\img3d\\");
	strcpy(sdl_path, "\\video\\nba\\models\\");
	strcpy(lwo_path, "\\video\\nba\\models\\");
	strcpy(uv_path, "\\video\\nba\\models\\");
	strcpy(inc_path, "\\video\\nba\\models\\");
	
	strcpy(sdl_cvt, "cvtmod.exe");
	strcpy(sdl_param, "-n $BASE_FILENAME < $FILENAME > $BASE_FILENAME_H");
	
	strcpy(lwo_cvt, "clw.exe");
	strcpy(lwo_param, "$BASE_FILENAME -n $HDLESS_BASE_FILENAME > $BASE_FILENAME_H");
	
	pack_width = 256;
	pack_height = 256;
	
	tga_cache = NULL;
	inc_cache = NULL;
	
	sdl_normalized = TRUE;
	lwo_normalized = TRUE;
}	/* init_globals */

static void process_l3d_file(FILE *fp)
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
			case SDL_PATH:
				/* SDL_PATH> \video\nba\models\ */
				if (get_param(buffer, len, param)) {
					strcpy(sdl_path, param);
					check_path(sdl_path);
				} else
					util_error("no path specified for SDL_PATH>, \"%s\"\n", buffer);
				break;

			case LWO_PATH:
				/* LWO_PATH> \video\nba\models\ */
				if (get_param(buffer, len, param)) {
					strcpy(lwo_path, param);
					check_path(lwo_path);
				} else
					util_error("no path specified for LWO_PATH>, \"%s\"\n", buffer);
				break;
			case UV_PATH:
				/* UV_PATH> \video\nba\models\ */
				if (get_param(buffer, len, param)) {
					strcpy(uv_path, param);
					check_path(uv_path);
				} else
					util_error("no path specified for UV_PATH>, \"%s\"\n", buffer);
				break;
			case INC_PATH:
				/* INC_PATH> \video\nba\models\ */
				if (get_param(buffer, len, param)) {
					strcpy(inc_path, param);
					check_path(inc_path);
				} else
					util_error("no path specified for INC_PATH>, \"%s\"\n", buffer);
				break;
			case SDL_CVT:
				/* SDL_CVT> cvtmod.exe */
				if (get_param(buffer, len, param))
					strcpy(sdl_cvt, param);
				else
					util_error("no name specified for SDL_CVT>, \"%s\"\n", buffer);
				break;
			case SDL_PARAM: {
				char opt[BUFFER_SIZE + 1];
				int cnt, cmd, i;
				bool uses_fname, uses_base_h;
				
				/* SDL_PARAM> -n $BASE_FILENAME < $FILENAME > $BASE_FILENAME_H */
				if (get_param(buffer, len, param)) {
					strcpy(sdl_param, param);
					cnt = get_param_count(sdl_param);
					uses_fname = FALSE;
					uses_base_h = FALSE;
					for (i = 0; i < cnt; i++) {
						get_param_num(sdl_param, i, opt);
						cmd = get_param_cmd(opt, strlen(opt));
						switch (cmd) {
						case FILENAME:
							uses_fname = TRUE;
							break;
						case BASE_FILENAME_H:
							uses_base_h = TRUE;
							break;
						case BASE_FILENAME:
						case HDLESS_BASE_FILENAME:
							break;
						default:
							/* check for denormalized S and T */
							if ((opt[0] == '-') && (opt[1] == 'm'))
								sdl_normalized = FALSE;
							else if (opt[0] == '$')
								util_error("unknown param command, \"%s\"\n", opt);
							break;
						}
					}
					if (!uses_fname)
						util_error("SDL_PARAM> must reference $FILENAME, \"%s\"\n", buffer);
					if (!uses_base_h)
						util_error("SDL_PARAM> must reference $BASE_FILENAME_H, \"%s\"\n", buffer);
				} else
					util_error("no string specified for SDL_PARAM>, \"%s\"\n", buffer);
				break;
			}
			case SDL: {
				/* SDL> xxx.sdl */
				char exec_cmd[BUFFER_SIZE + 1], opt[BUFFER_SIZE + 1], new_inc[BUFFER_SIZE + 1];
				int cnt, cmd, i;
				
				if (get_param(buffer, len, param)) {
					strcpy(exec_cmd, sdl_cvt);
					cnt = get_param_count(sdl_param);
					for (i = 0; i < cnt; i++) {
						strcat(exec_cmd, " ");
						get_param_num(sdl_param, i, opt);
						cmd = get_param_cmd(opt, strlen(opt));
						switch (cmd) {
						case FILENAME:
							strcat(exec_cmd, sdl_path);
							strcat(exec_cmd, cvt_filename(param, cmd));
							break;
						case BASE_FILENAME:
						case HDLESS_BASE_FILENAME:
							strcat(exec_cmd, cvt_filename(param, cmd));
							break;
						case BASE_FILENAME_H:
							strcat(exec_cmd, inc_path);
							strcpy(new_inc, cvt_filename(param, cmd));
							strcat(exec_cmd, new_inc);
							break;
						default:
							strcat(exec_cmd, opt);
							break;
						}
					}
					if (system(exec_cmd) != 0)
						util_error("error execing:%s\n", exec_cmd);
					cache_inc(new_inc, sdl_normalized);
				} else
					util_error("no .SDL file specified for SDL>, \"%s\"\n", buffer);
				break;
			}
			/* scan the produced .h for the .TGA list */
			/* build a list of all .TGAs to be packed, be sure it is not already in the list */
			/* build a list of .hs referencing the .TGAs */
			case LWO_CVT:
				/* LWO_CVT> clw.exe */
				if (get_param(buffer, len, param))
					strcpy(lwo_cvt, param);
				else
					util_error("no name specified for LWO_CVT>, \"%s\"\n", buffer);
				break;
			case LWO_PARAM: {
				char opt[BUFFER_SIZE + 1];
				int cnt, cmd, i;
				bool uses_base_h;
				
				/* LWO_PARAM> $BASE_FILENAME -n $HDLESS_BASE_FILENAME > $BASE_FILENAME_H */
				if (get_param(buffer, len, param)) {
					strcpy(lwo_param, param);
					cnt = get_param_count(lwo_param);
					uses_base_h = FALSE;
					for (i = 0; i < cnt; i++) {
						get_param_num(lwo_param, i, opt);
						cmd = get_param_cmd(opt, strlen(opt));
						switch (cmd) {
						case FILENAME:
						case BASE_FILENAME:
						case HDLESS_BASE_FILENAME:
							break;
						case BASE_FILENAME_H:
							uses_base_h = TRUE;
							break;
						default:
							/* check for denormalized S and T */
							if ((opt[0] == '-') && (opt[1] == 'm'))
								lwo_normalized = FALSE;
							else if (opt[0] == '$')
								util_error("unknown param command, \"%s\"\n", opt);
							break;
						}
					}
					if (!uses_base_h)
						util_error("LWO_PARAM> must reference $BASE_FILENAME_H, \"%s\"\n", buffer);
				} else
					util_error("no string specified for LWO_PARAM>, \"%s\"\n", buffer);
				break;
			}
			case LWO: {
				/* LWO> xxx.lwo */
				char exec_cmd[BUFFER_SIZE + 1], opt[BUFFER_SIZE + 1], new_inc[BUFFER_SIZE + 1];
				int cnt, cmd, i;
				
				if (get_param(buffer, len, param)) {
					strcpy(exec_cmd, lwo_cvt);
					cnt = get_param_count(lwo_param);
					for (i = 0; i < cnt; i++) {
						strcat(exec_cmd, " ");
						get_param_num(lwo_param, i, opt);
						cmd = get_param_cmd(opt, strlen(opt));
						switch (cmd) {
						case FILENAME:
							strcat(exec_cmd, lwo_path);
							strcat(exec_cmd, cvt_filename(param, cmd));
							break;
						case BASE_FILENAME:
						case HDLESS_BASE_FILENAME:
							strcat(exec_cmd, cvt_filename(param, cmd));
							break;
						case BASE_FILENAME_H:
							strcat(exec_cmd, inc_path);
							strcpy(new_inc, cvt_filename(param, cmd));
							strcat(exec_cmd, new_inc);
							break;
						default:
							strcat(exec_cmd, opt);
							break;
						}
					}
					if (system(exec_cmd) != 0)
						util_error("error execing:%s\n", exec_cmd);
					cache_inc(new_inc, lwo_normalized);
				} else
					util_error("no .LWO file specified for LWO>, \"%s\"\n", buffer);
				break;
			}
			/* scan the produced .h for the .TGA list */
			/* build a list of all .TGAs to be packed, be sure it is not already in the list */
			/* build a list of .hs referencing the .TGAs */
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
			case PACK: {
				/* PACK> zzz.tga */
				tga_pixel *image_data;
				inc_element *inc_el;
				tga_element *tga_el;
				int mn, mx;
				
				if (get_param(buffer, len, param)) {
					add_file_ext(param, ".tga");
					mn = MIN(pack_width, pack_height);
					mx = MAX(pack_width, pack_height);
					if ((mn != mx) && ((mn * 2) != mx) && ((mn * 4) != mx) && ((mn * 8) != mx))
						util_error("pack width %d and pack height %d must be 1:1, 1:2|2:1, 1:4|4:1, or 1:8|8:1\n", pack_width, pack_height);
					image_data = pack_textures();
					tga_write(param, pack_width, pack_height, FALSE, image_data);
					
					if (!silent_operation)
						printf("Texture maps packed to \"%s\"\n", param);
					
					free(image_data);
					
					/* backpatch the S and T in the .h for all of the files */
					for (inc_el = inc_cache; inc_el != NULL; inc_el = inc_el->next) 
						back_patch_inc(inc_el, param);
					
					/* clear the inc and tga caches, release any storage used */
					for (inc_el = inc_cache; inc_el != NULL;) {
						inc_element *t;
						
						t = inc_el->next;
						free(inc_el);
						inc_el = t;
					}
					inc_cache = NULL;
					for (tga_el = tga_cache; tga_el != NULL;) {
						tga_element *t;
						
						t = tga_el->global_next;
						free(tga_el->image);
						free(tga_el);
						tga_el = t;
					}
					tga_cache = NULL;
				} else
					util_error("no output file name for PACK>, \"%s\"\n", buffer);
				break;
			}			
			case UNKNOWN_COMMAND:
			default:
				util_error("unknown command, \"%s\"\n", buffer);
				break;
			}
		}
	}
}	/* process_l3d_file */

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

static int get_param_cmd(char *buffer, int len)
{
	char token[BUFFER_SIZE + 1];
	int i;
	
	strcpy(token, buffer);

	for (i = 0; i < len; i++)
		if (isspace(token[i])) {
			token[i] = '\0';
			break;
		}
	for (i = 0; i < ARRAY_SIZE(param_cmds); i++) {
		if (str_similiar(param_cmds[i].token_str, token))
			return param_cmds[i].token_id;
	}
	return 	UNKNOWN_COMMAND;
}	/* get_param_cmd */

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

static char *cvt_filename(char *fname, int cmd)
{
	static char cvt_fname[FILENAME_MAX];
	
	switch (cmd) {
	case FILENAME:
		strcpy(cvt_fname, fname);
		break;
	case BASE_FILENAME:
		strcpy(cvt_fname, fname);
		base_fname(cvt_fname);
		break;
	case BASE_FILENAME_H:
		strcpy(cvt_fname, fname);
		base_fname(cvt_fname);
		strcat(cvt_fname, ".h");
		break;
	case HDLESS_BASE_FILENAME:
		if ((fname[0] == 'h' || fname[0] == 'H') &&
			(fname[1] == 'd' || fname[1] == 'D') &&
			fname[2] == '_') {
			fname += 3;
		}
		strcpy(cvt_fname, fname);
		base_fname(cvt_fname);
		break;
	default:
		util_error("unknown cvt_filename command\n");
		break;
	}
	return cvt_fname;
}	/* cvt_filename */

static void base_fname(char *fname)
{
	int i;
	
	for (i = 0; i < strlen(fname); i++)
		if (fname[i] == '.') {
			fname[i] = '\0';
			break;
		}
}	/* base_fname */

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

static void cache_inc(char *new_inc, bool normalized)
{
	inc_element *elem;
	
	/* error if the .h is already in the cache */
	for (elem = inc_cache; elem != NULL; elem = elem->next) {
		if (str_similiar(new_inc, elem->inc_fname))
			util_error("the same model file \"%s\" was processed multiple times per packing\n", new_inc);
	}
	
	/* add the .h to the cache list */
	elem = util_xmalloc(sizeof(inc_element));
	elem->next = inc_cache;
	strcpy(elem->inc_fname, new_inc);
	elem->normalized_st = normalized;
	/* read the header file, scanning for the "char *" to build the list of .TGAs */
	elem->tga_count = scan_inc_for_textures(new_inc, &elem->tga_ref);
	inc_cache = elem;
	
	/* add the cached TGAs to the global list */
	cache_tga(elem->tga_ref);
}	/* cache_inc */

/* example texture map list from a model file
 *	char *jumbo_textures[] =
 *	{
 *		"omnibotm.wms",
 *		"omnitron.wms",
 *		NULL
 *	};
 */
static int scan_inc_for_textures(char *new_inc, tga_element **tga_ref)
{
	char fname[BUFFER_SIZE + 1];
	char buffer[BUFFER_SIZE + 1];
	tga_element *file_head, *file_elem;
	char *str;
	FILE *fp;
	bool at_eof, found;
	int len, i, j, count;
	
	/* build the absolute path to the include file */
	strcpy(fname, inc_path);
	strcat(fname, new_inc);

	count = 0;
	file_head = NULL;
	fp = fopen(fname, "r");
	if (fp == NULL)
		util_error("could not open the model header file \"%s\"\n", fname);
	else {
		at_eof = FALSE;
		found = FALSE;
		while (!at_eof) {
			at_eof = read_line(buffer, sizeof(buffer), fp, &len, FALSE);
			if (len > 0) {
				/* look for the start of the texture map table */
				if (strncmp(buffer, "char *", 6) == 0) {
					while (!at_eof) {
						found = TRUE;
						at_eof = read_line(buffer, sizeof(buffer), fp, &len, FALSE);
						
						/* read line until the NULL sentiel */
						str = strstr(buffer, "NULL");
						if (str != NULL)
							break;
						
						/* all lines with double quote in them are texture map names */
						str = strstr(buffer, "\"");
						if (str != NULL) {
							/* add the texture file name to the linked list */
							file_elem = util_xmalloc(sizeof(tga_element));
							file_elem->file_next = file_head;
							file_elem->global_next = NULL;
							file_elem->image = NULL;
							file_elem->texture_index = count;
							file_elem->new_left = -1;
							file_elem->new_top = -1;
							
							/* find the index of the character just past the opening quote */
							i = (str - buffer) + 1;
							/* copy the .wms texture map name */
							for (j = 0; (i < len) && (buffer[i] != '\"'); i++, j++) 
								file_elem->tga_fname[j] = buffer[i];
							file_elem->tga_fname[j] = '\0';
							/* replace the .wms extension with a .tga */
							add_file_ext(file_elem->tga_fname, ".tga");
							file_head = file_elem;
							count++;
						}
					}
					break;
				}
			}
		}
		if (!found)
			util_error("Could not find the texture map table in the model header file \"%s\"\n", fname);
		fclose(fp);
	}
	*tga_ref = file_head;
	return count;
}	/* scan_inc_for_textures */

static void cache_tga(tga_element *file_head)
{
	char tga_fname[FILENAME_MAX];
	tga_element *elem;
	
	for (elem = file_head; elem != NULL; elem = elem->file_next) {
		elem->global_next = tga_cache;
		tga_cache = elem;
		/* append the tga_path and open the TGA */
		strcpy(tga_fname, tga_path);
		strcat(tga_fname, elem->tga_fname);
		
		/* read in the tga */
		if (!silent_operation)
			printf("Loading texture map \"%s\"\n", tga_fname);
		
		tga_read(tga_fname, &elem->header, &elem->image);
	}
}	/* cache_tga */

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
	for (elem = tga_cache; elem != NULL; elem = elem->global_next)
		texture_count++;
	block_list = util_xmalloc(texture_count * sizeof(block_t));
	i = 0;
	for (elem = tga_cache; elem != NULL; elem = elem->global_next) {
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
static void back_patch_inc(inc_element *inc, char *new_texture_name)
{
	char old_fname[FILENAME_MAX];
	char new_fname[FILENAME_MAX];
	char buffer[BUFFER_SIZE + 1];
	char *str;
	FILE *old_fp, *new_fp;
	bool at_eof, texture_replaced;
	int tga_count, len, i, current_tga;
	int cnt, v0, v1, v2, st0, st1, st2, t;
	int *st_tga, max_st;
	int st_index;
	tga_element **st_delta, *tga_ref;
	float f_st0, f_st1;
	
	if (!silent_operation)
		printf("Backpatching model header file \"%s\"\n", inc->inc_fname);
	
	texture_replaced = FALSE;
	max_st = -1;
	st_tga = util_xmalloc(4096 * sizeof(int));
	for (i = 0; i < 4096; i++)
		st_tga[i] = -1;
	/* build the st_delta array */
	tga_count = 0;
	for (tga_ref = inc->tga_ref; tga_ref != NULL; tga_ref = tga_ref->file_next)
		tga_count++;
	st_delta = util_xmalloc(tga_count * sizeof(tga_element *));
	for (i = 0; i < tga_count; i++)
		st_delta[i] = NULL;
	for (tga_ref = inc->tga_ref; tga_ref != NULL; tga_ref = tga_ref->file_next) {
		if ((tga_ref->texture_index < 0) || (tga_ref->texture_index >= tga_count))
			util_error("back_patch_inc:texture index out of range\n");
		st_delta[tga_ref->texture_index] = tga_ref;
	}
	for (i = 0; i < tga_count; i++)
		if (st_delta[i] == NULL)
			util_error("back_patch_inc:texture index not filled\n");
	
	/* build the absolute path to the existing model include file */
	strcpy(old_fname, inc_path);
	strcat(old_fname, inc->inc_fname);
	old_fp = fopen(old_fname, "r");
	if (old_fp == NULL)
		util_error("could not reopen the model header file \"%s\"\n", old_fname);
	
	/* build the absolute path to the new model include file */
	strcpy(new_fname, old_fname);
	add_file_ext(new_fname, ".$h$");
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
						if (cnt != 7)
							util_error("error sscanfing TRIs from the model header file\n");
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
						cnt = sscanf(str, "{%f,%f}", &f_st0, &f_st1);
						if (cnt != 2)
							util_error("error sscanfing STs from the model header file\n");
						
						if (st_index > max_st)
							util_error("encountered a st_index > max_st\n");
						if (st_tga[st_index] > tga_count)
							util_error("encountered an index > tga_count\n");
						
						if (inc->normalized_st) {
							/* convert from 0.0-1.0 to pixel space add offset and convert back */
							/* what about values > 1.0 */
							/* S = X / texture_block_width */
							/* so X = S * texture_block_width */
							/* X' = X + texture_block_left_offset */
							/* S' = X' / new_page_width */
							/* so S' = ((S * texture_block_width) + texture_block_left_offset) / new_page_width */
							/* and T' = ((T * texture_block_height) + texture_block_top_offset) / new_page_height */
							f_st0 = ((f_st0 * st_delta[st_tga[st_index]]->header.width) + st_delta[st_tga[st_index]]->new_left) / pack_width;
							f_st1 = ((f_st1 * st_delta[st_tga[st_index]]->header.height) + st_delta[st_tga[st_index]]->new_top) / pack_height;
						} else {
							/* S' = S + texture_block_left_offset */
							/* T' = T + texture_block_top_offset */
							f_st0 += st_delta[st_tga[st_index]]->new_left;
							f_st1 += st_delta[st_tga[st_index]]->new_top;
						}
						fprintf(new_fp, "\t{%ff,%ff}%s\n", f_st0, f_st1, strstr(buffer, "},") != NULL ? "," : "");
						st_index++;
					} else {
						write_line(buffer, new_fp);						/* echo the line to the new file */
					}
				}
			} if (strncmp(buffer, "char *", 6) == 0) {
				/* rewrite the texture map name */
				write_line(buffer, new_fp);						/* echo the line to the new file */
				
				while (!at_eof) {
					at_eof = read_line(buffer, sizeof(buffer), old_fp, &len, FALSE);
					
					str = strstr(buffer, "\"");
					if (str != NULL) {
						if (!texture_replaced) {
							fprintf(new_fp, "\t\"%s\",\n", new_texture_name);
							texture_replaced = TRUE;
						}
					} else {
						write_line(buffer, new_fp);						/* echo the line to the new file */
						/* read line until the NULL sentiel */
						str = strstr(buffer, "NULL");
						if (str != NULL)
							break;
					}
				}
			} else {
				write_line(buffer, new_fp);
			}
		} else
			write_line(buffer, new_fp);
	}
	fclose(old_fp);
	fclose(new_fp);
	remove(old_fname);
	rename(new_fname, old_fname);
	free(st_tga);
	free(st_delta);
}	/* back_patch_inc */

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */
