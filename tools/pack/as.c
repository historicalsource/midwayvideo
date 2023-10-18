/*
 *		$Archive: /video/tools/pack/as.c $
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
char *ss_as_c = "$Workfile: as.c $ $Revision: 1 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "as.h"

/*
 *		DEFINES
 */

#define ASSERT_LOG				"assert.log"
#define MESSAGE_BUFFER_SIZE		1024

/*
 *		GLOBAL FUNCTIONS
 */

void as_assert(int expr, char *format, ...)
{
	if (!expr) {
		char assert_buffer[MESSAGE_BUFFER_SIZE];
		va_list arg_list;
		
		va_start(arg_list, format);
		vsprintf(assert_buffer, format, arg_list);
		va_end(arg_list);
		as_output(assert_buffer);
	}
}  /* as_assert */

void as_abort(char *format, ...)
{
	char abort_buffer[MESSAGE_BUFFER_SIZE];
	va_list arg_list;
	
	va_start(arg_list, format);
	vsprintf(abort_buffer, format, arg_list);
	va_end(arg_list);
	as_output(abort_buffer);
}  /* as_abort */

void as_output(char *message)
{
	FILE *log_file;
	
	fputs(message, stderr);
	fputc('\n', stderr);
	fflush(stderr);
	log_file = fopen(ASSERT_LOG, "w");
	if (log_file != NULL) {
		fputs(message, log_file);
		fputc('\n', log_file);
		fclose(log_file);
	}
	if (as_really_abort)
		abort();
}  /* as_output */

/*
 *		$History: as.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:08p
 * Created in $/video/tools/pack
 * assert library source file
 */
