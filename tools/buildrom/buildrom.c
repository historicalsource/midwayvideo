/*
 *		$Archive: /video/tools/buildrom/buildrom.c $
 *		$Revision: 1 $
 *		$Date: 9/30/97 10:50a $
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

#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "buildrom.h"
#include "lex.h"
#include "update.h"
#include "upio.h"

/*
 *		GLOBAL VARIABLES
 */

int in_file;
char in_file_name[256];

/*
 *		EXTERNS
 */
int yyparse(void);

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	int result;

	result = 1;
	if (argc != 2) {
		fprintf(stderr, "Usage:%s script_file\n", argv[0]);
	} else {
		int err;
		
		strcpy(in_file_name, argv[1]);
		err = file_open(in_file_name, &in_file);
		if (err == UPDATE_NO_ERROR) {
			init_lex();
			result = yyparse();
			err = file_close(in_file);
		} else {
			fprintf(stderr, "Error opening script file \'%s\'\n", argv[1]);
		}
	}
	return result == 1 ? EXIT_FAILURE : EXIT_SUCCESS;
}  /* main */

/*
 *		$History: buildrom.c $
 * 
 * *****************  Version 1  *****************
 * User: Mlynch       Date: 9/30/97    Time: 10:50a
 * Created in $/video/tools/buildrom
 * C sources for the buildrom tool
 */
