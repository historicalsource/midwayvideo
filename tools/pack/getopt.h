/*
 *		$Archive: /video/tools/pack/getopt.h $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:09p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __GETOPT_H__
#define __GETOPT_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_getopt_h = "$Workfile: getopt.h $ $Revision: 1 $";
#endif

/*
 *		USER INCLUDES
 */

#ifndef __SYSTEM_H__
#include "system.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *		GLOBAL PROTOTYPES
 */

int getopt(int argc, char *argv[], char *opt_str);

/*
 *		GLOBAL VARIABLES
 */

__EXTERN__ char *getopt_arg __INIT__(NULL);
__EXTERN__ int getopt_ind __INIT__(0);

#ifdef __cplusplus
}
#endif

/*
 *		$History: getopt.h $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:09p
 * Created in $/video/tools/pack
 * a simple getopt library header
 */

#endif
