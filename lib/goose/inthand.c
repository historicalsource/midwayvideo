/****************************************************************************/
/*                                                                          */
/* verthand.c - Interrupt handler                                           */
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
/* $Revision: 3 $                                                          */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<goose/switch.h>

char	goose_inthand_c_version[] = {"$Revision: 3 $"};

// External functions referenced here
void galileo_handler(void);

// Cause bits for hardware generated interrupts
#define	VERTICAL_RETRACE_INT	0x800
#define	GALILEO_INT				0x400

// Number of ticks per second
extern int	tsec;

// Flag to tell processor dispatcher that a vertical retrace interrupt was
// received
volatile int	got_interrupt = 0;

// Running tick counter
int	tick_counter = 0;

// Seconds the program has be running since last reset
int	uptime = 0;


//
// Handler used to deal with hardware generated interrupts
//
//int interrupt_handler(int cause, int rsave[])
int interrupt_handler(void)
{
//	if(cause & VERTICAL_RETRACE_INT)
//	{
		// Tell process dispatcher we got the int
		got_interrupt++;

		// Increment tick counter
		tick_counter++;

		// Update the uptime counter every 1 second
		if(!(tick_counter % tsec))
		{
			uptime++;
		}

		// Scan the switches, buttons, etc.
		scan_switches();
//	}

	// Otherwise it's an interrupt I don't know what to with so the BIOS
	// probably deals with it.
	// Return and tell BIOS to do it's thing
	return(1);
}
