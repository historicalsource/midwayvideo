/****************************************************************************/
/*                                                                          */
/* galhand.c - Interrupt handler for GT64010 generated interrupts           */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* Version:     1.00                                                        */
/* Date:        10/11/95                                                    */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* Use, duplication, or disclosure is strictly forbidden unless approved    */
/* in writing by Williams Electronics Games Inc.                            */
/*                                                                          */
/* $Revision: 5 $                                                          */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<ioctl.h>
#include	<glide/glide.h>

char	goose_galhand_c_version[] = {"$Revision: 5 $"};

#ifndef DEBUG
void start_timeout_timer(void);
void stop_timeout_timer(void);
void galileo_timer3_handler(void);
int get_timeout_time(void);
void do_dma_timeout_reboot(void);
#else
void do_dma_timeout_reboot(void)
{
	fprintf(stderr, "DMA TIMEOUT - IN NON-DEBUG MODE THIS CAUSES A RESET\r\n");
}
#endif

#define	GT_INT_CAUSE_REG	0

extern int	gt_fd;

volatile int	dma_int = 1;

#ifdef PROFILE
extern int	dma_int_time;
extern int	dma_int_exit_time;
#endif

#ifdef PROFILE
int ___get_count(void);
#define	GET_TIME(A)	(A) = (___get_count() * 13)
#endif

void galileo_dma0_handler(void)
{
#ifdef PROFILE
	GET_TIME(dma_int_time);
#endif

#ifndef DEBUG
	// Stop the timeout timer
	stop_timeout_timer();
#endif

	// Swap the draw and display pages
	grBufferSwap(1);

	// Set flag saying dma interrupt was received
	dma_int = 1;

#ifdef PROFILE
	GET_TIME(dma_int_exit_time);
#endif
}

#ifndef DEBUG
static int	timer_int_count = 0;

void start_timeout_timer(void)
{
	timer_int_count = 0;
	_ioctl(5, FIOCSTARTTIMER3, 0);
}

void stop_timeout_timer(void)
{
	timer_int_count = 0;
	_ioctl(5, FIOCSTOPTIMER3, 0);
}

int get_timeout_time(void)
{
	int	val;

	_ioctl(5, FIOCGETTIMER3, (int)&val);
	return(val);
}

void galileo_timer3_handler(void)
{
	if(++timer_int_count > 4)
	{
		do_dma_timeout_reboot();
	}
}
#endif
