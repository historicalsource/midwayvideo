/****************************************************************************/
/*                                                                          */
/* prochook.c - Source for process loop function hooking.                   */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* Version:     1.00                                                        */
/* Date:        6/28/95                                                     */
/*                                                                          */
/* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* Use, duplication, or disclosure is strictly forbidden unless approved    */
/* in writing by Williams Electronics Games Inc.                            */
/*                                                                          */
/* $Revision: 3 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<stdlib.h>

char	goose_prochook_c_version[] = {"$Revision: 3 $"};

struct __proc_func {
	struct	__proc_func	*__next;
	void		(*__function)(void);
};

static struct __proc_func	*__pre_dma_proc_ptr = 0;
static struct __proc_func	*__pre_proc_ptr = 0;
static struct __proc_func	*__post_proc_ptr = 0;


int pre_dma_proc_func(void (*a)(void))
{
	struct __proc_func	*pp;

	if(!a)
	{
		return(-1);
	}
	pp = (struct __proc_func *)malloc(sizeof(struct __proc_func));
	if(!pp)
	{
		return(-1);
	}
	pp->__next = __pre_dma_proc_ptr;
	pp->__function = a;
	__pre_dma_proc_ptr = pp;
	return(0);
}

int pre_proc_func(void (*a)(void))
{
	struct __proc_func	*pp;

	if(!a)
	{
		return(-1);
	}
	pp = (struct __proc_func *)malloc(sizeof(struct __proc_func));
	if(!pp)
	{
		return(-1);
	}
	pp->__next = __pre_proc_ptr;
	pp->__function = a;
	__pre_proc_ptr = pp;
	return(0);
}

int post_proc_func(void (*a)(void))
{
	struct __proc_func	*pp;

	if(!a)
	{
		return(-1);
	}
	pp = (struct __proc_func *)malloc(sizeof(struct __proc_func));
	if(!pp)
	{
		return(-1);
	}
	pp->__next = __post_proc_ptr;
	pp->__function = a;
	__post_proc_ptr = pp;
	return(0);
}


void do_pre_dma_proc(void)
{
	struct __proc_func	*pp = __pre_dma_proc_ptr;

	while(pp)
	{
		(pp->__function)();
		pp = pp->__next;
	}
}

void do_pre_proc(void)
{
	struct __proc_func	*pp = __pre_proc_ptr;

	while(pp)
	{
		(pp->__function)();
		pp = pp->__next;
	}
}

void do_post_proc(void)
{
	struct __proc_func	*pp = __post_proc_ptr;

	while(pp)
	{
		(pp->__function)();
		pp = pp->__next;
	}
}
