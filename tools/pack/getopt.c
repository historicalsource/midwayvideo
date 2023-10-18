/*
 *		$Archive: /video/tools/pack/getopt.c $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:09p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_getopt_c = "$Workfile: getopt.c $ $Revision: 1 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "getopt.h"

/*
 *		GLOBAL FUNCTIONS
 */

int getopt(int argc, char *argv[], char *opt_str)
{
	char *option;
	char *opt_str_p;
	char ch_opt;
	
	while (++getopt_ind < argc) {
		option = argv[getopt_ind];
		if (strchr("-/", *option++)) {
			if (*option == *(option - 1)) {
				getopt_ind++;
				return -1;
			}
			ch_opt = *option;
			if ((ch_opt != ':') && (opt_str_p = strchr(opt_str, ch_opt))) {
				if ((*(opt_str_p + 1) == ':') && (*(getopt_arg = option + 1) == '\0'))
					getopt_arg = argv[++getopt_ind];
				return ch_opt;
			} else {
				fprintf(stderr, "%s: unknown command line option `%c\'\n", argv[0], ch_opt);
				return '?';
			}
		}
		return -1;
	}
	return -1;
}  /* getopt */

/*
 *		$History: getopt.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:09p
 * Created in $/video/tools/pack
 * a simple getopt library source file
 */
