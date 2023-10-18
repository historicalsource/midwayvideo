/*
 *		$Archive: /video/tools/pack/GENENV.c $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:08p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_genenv_c = "$Workfile: GENENV.c $ $Revision: 1 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#define COMPILING_GENENV
#include "system.h"

/*
 *		DEFINES
 */

#define DEFAULT_H_NAME			"env.h"

/*
 *		TYPEDEFS
 */

typedef struct {	char ch;	char test;			} align_char;
typedef struct {	char ch;	short test;			} align_short;
typedef struct {	char ch;	int test;			} align_int;
typedef struct {	char ch;	long test;			} align_long;
typedef struct {	char ch;	llong test;			} align_llong;
typedef struct {	char ch;	float test;			} align_float;
typedef struct {	char ch;	double test;		} align_double;
typedef struct {	char ch;	long double test;	} align_ldouble;
typedef struct {	char ch;	void *test;			} align_ptr;

enum {
	OFFSET_CHAR,
	OFFSET_SHORT,
	OFFSET_INT,
	OFFSET_LONG,
	OFFSET_LLONG,
	OFFSET_FLOAT,
	OFFSET_DOUBLE,
	OFFSET_LDOUBLE,
	OFFSET_PTR,
	OFFSET_COUNT
};

/*
 *		STATIC PROTOTYPES
 */

static int find_bit_count(void);
static char *find_compliment(void);
static char *find_endian(void);
static char *find_stack_direction(void);
static void fname_to_label(char *label, char *fname, bool force_upper);
static void make_env_file(FILE *env_file);
static uint max_list(int count, uint *data);

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	char label[FILENAME_MAX];
	FILE *env_file;
	str out_name;
	int ret;
	
	if (argc < 3) {
		out_name = argc == 2 ? argv[1] : DEFAULT_H_NAME;
		env_file = fopen(out_name, "w");
		if (env_file != NULL) {
			fprintf(env_file, "/*\n");
			fprintf(env_file, " *\t%s\n", out_name);
			fprintf(env_file, " *\tthis file was generated by " __FILE__ " %s\n", " $Revision: 1 $");
			fprintf(env_file, " *\tdo not edit this file directly\n");
			fprintf(env_file, " */\n");
			fprintf(env_file, "\n");

			fname_to_label(label, out_name, TRUE);
			fprintf(env_file, "#ifndef __%s__\n", label);
			fprintf(env_file, "#define __%s__\n", label);
			fprintf(env_file, "\n");
			fname_to_label(label, out_name, FALSE);
			fprintf(env_file, "#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)\n");
			fprintf(env_file, "char *ss_%s = \"%s\"\n", label, "$Workfile: GENENV.c $ $Revision: 1 $");
			fprintf(env_file, "#endif\n");
			fprintf(env_file, "\n");

			make_env_file(env_file);

			fprintf(env_file, "\n");
			fprintf(env_file, "#endif\n");
			fclose(env_file);
			ret = EXIT_SUCCESS;
		} else {
			fprintf(stderr, "%s could not create the file \"%s\"\n", argv[0], out_name);
			ret = EXIT_FAILURE;
		}
	} else {
		fprintf(stderr, "usage:%s [header_file_name]\n", argv[0]);
		ret = EXIT_FAILURE;
	}
	return ret;
}  /* main */

/*
 *		STATIC FUNCTIONS
 */

static int find_bit_count(void)
{
	int cnt;
	uchar ch;
	
	cnt = 0;
	ch = 0x01;
	while (ch != 0) {
		cnt++;
		ch <<= 1;
	}
	return cnt;
}  /* find_bit_count */

static char *find_compliment(void)
{
	return (uint)(~1) == (uint)(-1) ? "ENV_ONES_COMPLIMENT" : "ENV_TWOS_COMPLIMENT";
}  /* find_compliment */

static char *find_endian(void)
{
	ulong test;
	char *str;
	
	test = 0x11223344;
	switch (*(uchar *)&test) {
	case 0x11:					/* stored as 0x11223344 MSB first: 68000, ibm, net */
		str = "ENV_BIG_ENDIAN";
		break;
	case 0x22:					/* stored as 0x22114433 LSB first in word, MSW first in long: PDP 11 series */
		str = "ENV_PDP_ENDIAN";
		break;
	case 0x44:					/* stored as 0x44332211 LSB first: i386, VAX */
		str = "ENV_LITTLE_ENDIAN";
		break;
	default:					/* no known processor */
		str = "ENV_UNKNOWN_ENDIAN";
		break;
	}
	return str;
}  /* find_endian */

static char *find_stack_direction(void)
{
	static int *addr = NULL;
	int temp;
	
	if (addr == NULL) {
		addr = &temp;
		return find_stack_direction();
    } else
		return &temp > addr ? "ENV_STACK_GROWS_UP" : "ENV_STACK_GROWS_DOWN";
}  /* find_stack_direction */

static void fname_to_label(char *label, char *fname, bool force_upper)
{
	int i, j;
	
	for (i = 0, j = strlen(fname); i < j; i++) {
		if (isalnum(fname[i])) {
			if (force_upper && islower(fname[i]))
				label[i] = toupper(fname[i]);
			else
				label[i] = fname[i];
		} else
			label[i] = '_';
	}
	label[i] = '\0';
}  /* fname_to_label */

static void make_env_file(FILE *env_file)
{
	uint offset_data[OFFSET_COUNT], offset_max;
	
	offset_data[OFFSET_CHAR] = offsetof(align_char, test);
	offset_data[OFFSET_SHORT] = offsetof(align_short, test);
	offset_data[OFFSET_INT] = offsetof(align_int, test);
	offset_data[OFFSET_LONG] = offsetof(align_long, test);
	offset_data[OFFSET_LLONG] = offsetof(align_llong, test);
	offset_data[OFFSET_FLOAT] = offsetof(align_float, test);
	offset_data[OFFSET_DOUBLE] = offsetof(align_double, test);
	offset_data[OFFSET_LDOUBLE] = offsetof(align_ldouble, test);
	offset_data[OFFSET_PTR] = offsetof(align_ptr, test);
	offset_max = max_list(OFFSET_COUNT, offset_data);

	fprintf(env_file, "enum {\n");
	fprintf(env_file, "\t/* ENV_ENDIAN */\n");
	fprintf(env_file, "\tENV_LITTLE_ENDIAN,\n");
	fprintf(env_file, "\tENV_BIG_ENDIAN,\n");
	fprintf(env_file, "\tENV_PDP_ENDIAN,\n");
	fprintf(env_file, "\tENV_UNKNOWN_ENDIAN,\n");
	fprintf(env_file, "\t\n");

	fprintf(env_file, "\t/* ENV_COMPLIMENT */\n");
	fprintf(env_file, "\tENV_ONES_COMPLIMENT,\n");
	fprintf(env_file, "\tENV_TWOS_COMPLIMENT,\n");
	fprintf(env_file, "\t\n");

	fprintf(env_file, "\t/* ENV_STACK */\n");
	fprintf(env_file, "\tENV_STACK_GROWS_DOWN,\n");
	fprintf(env_file, "\tENV_STACK_GROWS_UP\n");
	fprintf(env_file, "};\n");
	fprintf(env_file, "\n");

	fprintf(env_file, "/* processor characteristics */\n");
	fprintf(env_file, "#define ENV_ENDIAN\t\t\t%s\n", find_endian());
	fprintf(env_file, "#define ENV_COMPLIMENT\t\t%s\n", find_compliment());
	fprintf(env_file, "#define ENV_STACK\t\t\t%s\n", find_stack_direction());
	fprintf(env_file, "\n");
	
	fprintf(env_file, "/* number of bits in a byte */\n");
	fprintf(env_file, "#define ENV_CHAR_BIT\t\t%d\n", find_bit_count());
	fprintf(env_file, "\n");
	
	fprintf(env_file, "/* size of basic types, in bytes */\n");
	fprintf(env_file, "#define ENV_CHAR_SIZE\t\t%u\n", (uint)sizeof(char));
	fprintf(env_file, "#define ENV_SHORT_SIZE\t\t%u\n", (uint)sizeof(short));
	fprintf(env_file, "#define ENV_INT_SIZE\t\t%u\n", (uint)sizeof(int));
	fprintf(env_file, "#define ENV_LONG_SIZE\t\t%u\n", (uint)sizeof(long));
	fprintf(env_file, "#define ENV_LLONG_SIZE\t\t%u\n", (uint)sizeof(llong));
	fprintf(env_file, "#define ENV_FLOAT_SIZE\t\t%u\n", (uint)sizeof(float));
	fprintf(env_file, "#define ENV_DOUBLE_SIZE\t\t%u\n", (uint)sizeof(double));
	fprintf(env_file, "#define ENV_LDOUBLE_SIZE\t%u\n", (uint)sizeof(long double));
	fprintf(env_file, "#define ENV_PTR_SIZE\t\t%u\n", (uint)sizeof(void *));
	fprintf(env_file, "\n");
	
	fprintf(env_file, "/* required alignment for basic types, in bytes */\n");
	fprintf(env_file, "#define ENV_CHAR_ALIGN\t\t%u\n", offset_data[OFFSET_CHAR]);
	fprintf(env_file, "#define ENV_SHORT_ALIGN\t\t%u\n", offset_data[OFFSET_SHORT]);
	fprintf(env_file, "#define ENV_INT_ALIGN\t\t%u\n", offset_data[OFFSET_INT]);
	fprintf(env_file, "#define ENV_LONG_ALIGN\t\t%u\n", offset_data[OFFSET_LONG]);
	fprintf(env_file, "#define ENV_LLONG_ALIGN\t\t%u\n", offset_data[OFFSET_LLONG]);
	fprintf(env_file, "#define ENV_FLOAT_ALIGN\t\t%u\n", offset_data[OFFSET_FLOAT]);
	fprintf(env_file, "#define ENV_DOUBLE_ALIGN\t%u\n", offset_data[OFFSET_DOUBLE]);
	fprintf(env_file, "#define ENV_LDOUBLE_ALIGN\t%u\n", offset_data[OFFSET_LDOUBLE]);
	fprintf(env_file, "#define ENV_PTR_ALIGN\t\t%u\n", offset_data[OFFSET_PTR]);
	fprintf(env_file, "\n");

	fprintf(env_file, "/* largest alignment required by any type, in bytes */\n");
	fprintf(env_file, "#define ENV_STRICTEST_ALIGN\t%u\n", offset_max);
}  /* make_env_file */

static uint max_list(int count, uint *data)
{
	uint max;
	
	for (max = 0; --count >= 0;)
		if (data[count] > max)
			max = data[count];
	return max;
}  /* max_list */

/*
 *		$History: GENENV.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:08p
 * Created in $/video/tools/pack
 * a tool to generate an env.h file describing the host processor
 */