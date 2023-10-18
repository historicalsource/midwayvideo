/*
 *		$Archive: /video/tools/pack/bagger.h $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:08p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __BAGGER_H__
#define __BAGGER_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_bagger_h = "$Workfile: bagger.h $ $Revision: 1 $";
#endif

/*
 *		USER INCLUDES
 */
#ifndef __SYSTEM_H__
#include "system.h"
#endif

/*
 *		TYPEDEFS
 */

typedef struct {
	int total;						/* the current total of the set elements */
	int num_elem;					/* number of elements in the set */
	uchar *mask;					/* bit mask representing elements in set */
} set;

/*
 *		GLOBAL PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

/* find the minimum number of elements whose sum is as close as possible to the goal value */
/* return -1 for no possible solution */
/* return 0 for a nonperfect solution, the sum is not the goal */
/* return +1 for a perfect solution, the sum is the goal */
int bagger(int goal, int num_value, int *value, set *result);

#ifdef __cplusplus
}
#endif

/*
 *		$History: bagger.h $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:08p
 * Created in $/video/tools/pack
 * bagger header
 */

#endif
