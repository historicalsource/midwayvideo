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
char *ss_bagger_c = "$Workfile: $ $Revision: $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <limits.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "bagger.h"
#include "util.h"

/*
 *		STATIC PROTOTYPES
 */

static int cmp(const void *a, const void *b);
static int last_elem(uchar *mask, int mask_size);
static void init_set(set *s, int mask_size);
static void add_to_set(set *s, int *value, int elem);
static void sub_from_set(set *s, int *value, int elem);

/*
 *		GLOBAL FUNCTIONS
 */

/* find the minimum number of elements whose sum is as close as possible to the goal value */
/* return -1 for no possible solution */
/* return 0 for a nonperfect solution, the sum is not the goal */
/* return +1 for a perfect solution, the sum is the goal */
int bagger(int goal, int num_value, int *value, set *result)
{
	set current;						/* current total */
	set best;							/* best set found so far */
	int *sort;							/* list of values sorted largest to smallest */
	int last_added_elem;				/* index of last element added to set */
	int num_char;						/* number of char that make up a set */
	int i, j;							/* loop indices */
	bool done;							/* exit sentinel */
	
	/* determine the size of the set mask */
	num_char = ROUND2(num_value, CHAR_BIT) / CHAR_BIT;
	
	/* build the result buffer */
	init_set(result, num_char);
	
	/* insure there is data to be considered */
	if (goal <= 0 || num_value <= 0)
		return -1;
	
	/* build the current buffer */
	init_set(&current, num_char);
	
	/* build the best buffer */
	init_set(&best, num_char);
	
	/* build the sorted buffer */
	sort = util_xmalloc(num_value * sizeof(int));
	memcpy(sort, value, num_value * sizeof(int));
	qsort(sort, num_value, sizeof(int), cmp);
	
	/* no elements added yet */
	last_added_elem = -1;
	
	/* init the exit condition */
	done = FALSE;
	
	/* make sure the smallest block will fit */
	if (sort[num_value - 1] > goal)
		done = TRUE;
	
	/* quick check to see if a single value is the goal */
	if (!done) {
		for (i = 0; i < num_value; i++)
			if (sort[i] == goal) {
				add_to_set(&best, sort, i);
				done = TRUE;
				break;
			}
	}
	
	/* quick check to see if all of the values will fit */
	if (!done) {
		for (i = 0, j = 0; i < num_value; i++)
			j += sort[i];
		if (j <= goal) {
			for (i = 0; i < num_value; i++)
				add_to_set(&best, sort, i);
			done = TRUE;
		}
	}
	
	/* quick check to see if all the values are the same */
	/* if so, just grab the first (goal / value) elements */
	if (!done) {
		for (i = 1; i < num_value; i++)
			if (sort[i] != sort[0])
				break;
		if (i == num_value) {
			for (i = 0; i < goal / sort[0]; i++)
				add_to_set(&best, sort, i);
			done = TRUE;
		}
	}
	
	for (; !done; ) {
		/* find the next value that will fit */
		for (i = last_added_elem + 1; i < num_value; i++) {
			if (current.total + sort[i] <= goal) {
				add_to_set(&current, sort, i);
				last_added_elem = i;
				break;
			}
		}
		if (i == num_value) {
			/* can not add any more elements, removed last added element */
			sub_from_set(&current, sort, last_added_elem);
			
			/* choose next smaller value */
			last_added_elem++;
			if (last_added_elem == num_value) {
				/* no more values availiable this pass */
				if (current.num_elem == 0) {
					/* exhausted all choices, best has the nonperfect solution */
					done = TRUE;
				} else {
					/* backtrack to previous level */
					last_added_elem = last_elem(current.mask, num_value);
					sub_from_set(&current, sort, last_added_elem);
					continue;
				}
			} else
				add_to_set(&current, sort, last_added_elem);
		}
		
		/* compare and update the best set */
		if ((current.total > best.total) || ((current.total == best.total) && (current.num_elem < best.num_elem))) {
			best.total = current.total;
			best.num_elem = current.num_elem;
			memcpy(best.mask, current.mask, num_char);
			/* check for a perfect solution */
			if (best.total == goal)
				done = TRUE;
		}
	}
	
	/* translate the best buffer back to sorting the original data used */
	result->total = best.total;
	result->num_elem = best.num_elem;
	for (i = 0; i < num_value; i++)
		if (util_bit_test(best.mask, i)) {
			for (j = 0; j < num_value; j++)
				if (value[j] == sort[i] && !util_bit_test(result->mask, j)) {
					util_bit_set(result->mask, j);
					break;
				}
		}
	
	/* free the temp buffers */
	free(current.mask);
	free(best.mask);
	free(sort);
	
	/* determine the return value */
	return result->total == 0 ? -1 : (result->total == goal ? +1 : 0);
}  /* bagger */

/*
 *		STATIC FUNCTIONS
 */

static int cmp(const void *a, const void *b)
{
	int *aa, *bb;
	
	aa = (int *)a;
	bb = (int *)b;
	return *aa == *bb ? 0 : (*aa < *bb ? +1 : -1);
}  /* cmp */

static int last_elem(uchar *mask, int mask_size)
{
	int i;
	
	for (i = mask_size - 1; i >= 0; i--)
		if (util_bit_test(mask, i))
			return i;
	return -1;
}  /* last_elem */

static void init_set(set *s, int mask_size)
{
	s->total = 0;
	s->num_elem = 0;
	s->mask = util_xcalloc(mask_size);
}  /* init_set */

static void add_to_set(set *s, int *value, int elem)
{
	if (!util_bit_test(s->mask, elem)) {
		s->total += value[elem];
		s->num_elem++;
		util_bit_set(s->mask, elem);
	}
}  /* add_to_set */

static void sub_from_set(set *s, int *value, int elem)
{
	if (util_bit_test(s->mask, elem)) {
		s->total -= value[elem];
		s->num_elem--;
		util_bit_clear(s->mask, elem);
	}
}  /* sub_from_set */

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */
