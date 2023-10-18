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

#ifndef __BAGGER_H__
#define __BAGGER_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_bagger_h = "$Workfile: $ $Revision: $";
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

/* find the minimum number of elements whose sum is as close as possible to the goal value */
/* return -1 for no possible solution */
/* return 0 for a nonperfect solution, the sum is not the goal */
/* return +1 for a perfect solution, the sum is the goal */
int bagger(int goal, int num_value, int *value, set *result);

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */

#endif
