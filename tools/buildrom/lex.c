/*
 *		$Archive: /video/tools/buildrom/lex.c $
 *		$Revision: 2 $
 *		$Date: 10/06/97 5:30p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

/*
 *		SYSTEM INCLUDES
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "lex.h"
#include "parse.h"
#include "buildrom.h"
#include "upio.h"
#include "update.h"

/*
 *		TYPEDEFS
 */

typedef struct {
	char* name;
	int symbol;
} reserved_ident;

/*
 *		GLOBAL VARIABLES
 */

int line_number;

/*
 *		STATIC PROTOTYPES
 */

static int get_next_char(void);
static int get_num(void);
static int process_ident(void);
static char *process_string(void);
static int str_upper_eq(char *a, char *b);

/*
 *		STATIC VARIABLES
 */

static int next_char;
static reserved_ident ident[] = {{"four_mb", T_FOUR_MB},
								 {"eight_mb", T_EIGHT_MB},
								 {"old_revision_directory", T_OLD_REVISION_DIRECTORY},
								 {"new_revision_directory", T_NEW_REVISION_DIRECTORY},
								 {"eprom_size", T_EPROM_SIZE},
								 {"revision_name", T_REVISION_NAME},
								 {"old_revision_level", T_OLD_REVISION_LEVEL},
								 {"new_revision_level", T_NEW_REVISION_LEVEL},
								 {"game_id", T_GAME_ID},
								 {"new_file", T_NEW_FILE},
								 {"delete_file", T_DELETE_FILE},
								 {"rename_file", T_RENAME_FILE},
								 {"exec_file", T_EXEC_FILE},
								 {"patch_file", T_PATCH_FILE},
								 {"copy_file", T_COPY_FILE},
								 {"end_script", T_END_SCRIPT}};

/*
 *		GLOBAL FUNCTIONS
 */

void init_lex(void)
{
	next_char = ' ';
	line_number = 1;
}  /* init_lex */

int yylex(void)
{
	for (;;) {
		while (isspace(next_char))
			next_char = get_next_char();
		
		switch (next_char) {
		case '#':			/* comment to end of line */
			while (next_char != '\n' && next_char != EOF)
				next_char = get_next_char();
			if (next_char == EOF)
				return T_EOF;
			break;
		case ';':
			next_char = get_next_char();
			return ';';
			break;
		case ',':
			next_char = get_next_char();
			return ',';
			break;
		case '\"':
			yylval.str = process_string();
			return T_STR;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			yylval.ic = get_num();
			return T_IC;
			break;
		case '_':
		case 'a': case 'A':
		case 'b': case 'B':
		case 'c': case 'C':
		case 'd': case 'D':
		case 'e': case 'E':
		case 'f': case 'F':
		case 'g': case 'G':
		case 'h': case 'H':
		case 'i': case 'I':
		case 'j': case 'J':
		case 'k': case 'K':
		case 'l': case 'L':
		case 'm': case 'M':
		case 'n': case 'N':
		case 'o': case 'O':
		case 'p': case 'P':
		case 'q': case 'Q':
		case 'r': case 'R':
		case 's': case 'S':
		case 't': case 'T':
		case 'u': case 'U':
		case 'v': case 'V':
		case 'w': case 'W':
		case 'x': case 'X':
		case 'y': case 'Y':
		case 'z': case 'Z':
			return process_ident();
			break;
		case EOF :
			return T_EOF;
			break;
		default :
			fprintf(stderr, "stray character `%c\' (0x%hX)\n", next_char, next_char);
			next_char = get_next_char();
			break;
		}
	}
}  /* yylex */

/*
 *		STATIC FUNCTIONS
 */

static int get_next_char(void)
{
	int err;
	uchar ch;
	
	err = file_read(in_file, &ch, sizeof(uchar));
	if (err == UPDATE_FILE_READ_EOF_ERROR)
		return EOF;
	if (ch == '\n')
		line_number++;
	return ch;
}  /* get_next_char */

static int get_num(void)
{
	int number;
	
	number = 0;
	while (isdigit(next_char)) {
		number = number * 10 + (next_char - '0');
		next_char = get_next_char();
	}
	return number;
}  /* get_num */

static int process_ident(void)
{
	char buffer[256];
	int i;
	
	i = 0;
	while ((isalnum(next_char) || next_char == '_') && next_char != EOF) {
		if (i < 255)
			buffer[i++] = next_char;
		next_char = get_next_char();
	}
	buffer[i] = '\0';
	
	for (i = 0; i < sizeof(ident) / sizeof(ident[0]); i++)
		if (str_upper_eq(buffer, ident[i].name)) {
			return ident[i].symbol;
		}
	return T_UNKNOWN_IDENT;
}  /* process_ident */

static char *process_string(void)
{
	char *str;
	int index;

	str = malloc(256 * sizeof(char));
	if (str != NULL) {
		index = 0;
		next_char = get_next_char();			/* move past the opening " */
		while (next_char != '\"' && next_char != EOF) {
			str[index++] = next_char;
			next_char = get_next_char();
		}
		if (next_char == EOF)
			fprintf(stderr, "Unexpected end of file encountered within a string");
		else
			next_char = get_next_char();		/* move past the closing " */
		str[index] = '\0';
	}
	return str;
}  /* process_string */

static int str_upper_eq(char *a, char *b)
{
	int i, a_len, b_len, ret;
	
	a_len = strlen(a);
	b_len = strlen(b);
	if (a_len == b_len) {
		ret = 1;
		for (i = 0; i < a_len; i++)
			if (toupper(a[i]) != toupper(b[i])) {
				ret = 0;
				break;
			}
	} else
		ret = 0;
	return ret;
}  /* str_upper_eq */

/*
 *		$History: lex.c $
 * 
 * *****************  Version 2  *****************
 * User: Markg        Date: 10/06/97   Time: 5:30p
 * Updated in $/video/tools/buildrom
 * added copy_file command
 * 
 * *****************  Version 1  *****************
 * User: Mlynch       Date: 9/30/97    Time: 10:50a
 * Created in $/video/tools/buildrom
 * C sources for the buildrom tool
 */
