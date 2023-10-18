//
// fpgaload.c - Source for downloading program to SIO FPGA
//
// functions to load an image from the Altera Max+II software into the
// FPGA	on the Vegas SIO-1 H/W
//
// Written By:	Andrew Dyer <adyer@mcs.com>
//              Midway Games, Inc.
// Date:	Fri Apr  4 14:22:42 CST 1997
//
// Copyright (c) 1997, 1998  by Midway Games Inc.
// All Rights Reserved
//
// Use, duplication, or disclosure is strictly forbidden unless
// approved in writing by Midway Games Inc.
//
//
// $Revision: 3 $
//
#include	<system.h>
#include <stdio.h>

// NOTE: The following constants are somewhat arbitrary
// these may of may not correspond with reality

// number of times to retry an individual byte program cycle
#define FPGA_RDY_RETRY 50

// number of times to retry the final 'all ok' status check
#define FPGA_CONF_DONE_TIMEOUT 0x10

// this array holds the data to be programmed into the device
// use the C preprocessor to stick the comma delineated
// version of the fpga data into this array

const unsigned char fpgadata[] = {
  #include "vsio_v30.ttf"
};

// functions needed for this to work
void clear_count(void);
int gcount(void);

__asm__("
	.globl	clear_count
	.set		noreorder
clear_count:
	mtc0	$0,$9
	nop
	nop
	jr	$31
	nop
	.set		reorder

	.globl	gcount
	.set		noreorder
gcount:
	mfc0	$2,$9
	nop
	nop
	jr	$31
	nop
	.set		reorder
");

void delay_us(int usec)
{
	clear_count();
	usec *= 1000;
	usec /= NANOS_PER_TICK;
	while(gcount() < usec) ;
}

//
// fpgaload() - returns zero on success
//              returns >= one on failure
//
int fpgaload(void)
{
	unsigned int	i;
	unsigned int	j;
	unsigned int	bit;
	unsigned char	status;
	unsigned long	dcs2;
	unsigned long	old_dcs2;
	unsigned long	old_lcst2;

	// make sure I/O board gets reset
	*((volatile unsigned char *)VEGAS_RESET_REG) &= ~VEGAS_RESET_LOCALBUS;
	delay_us(10);
	*((volatile unsigned char *)VEGAS_RESET_REG) |= VEGAS_RESET_LOCALBUS;

	// set up dcs2 for 32-bits and correct timing
	// 50ns setup to rising edge of write, 0ns hold
	// 50ns write pulse width

	dcs2 = *((volatile unsigned long *)N4_DCS2_LO);

	old_dcs2 = dcs2;

	dcs2 = (dcs2 & ~N4_PDAR_WIDTH_MASK) | N4_PDAR_32BIT;
	*((volatile unsigned long *)N4_DCS2_LO) = dcs2;

	old_lcst2 = *((volatile unsigned long *)N4_LCST2);

	*((volatile unsigned long *)N4_LCST2) = (N4_LCL_CSON|\
		(1 << N4_LCL_CONSET_SHIFT)	|\
		(2 << N4_LCL_CONWID_SHIFT)	|\
		(2 << N4_LCL_SUBSCWID_SHIFT)	|\
		(1 << N4_LCL_CSOFF_SHIFT)		|\
		(1 << N4_LCL_COFHOLD_SHIFT)	|\
		(1 << N4_LCL_BUSIDLE_SHIFT)	|\
		(0 << N4_LCL_CONOFF_SHIFT)	|\
		(N4_LCL_CSPOL_NEG)		|\
		(N4_LCL_CONPOL_NEG));

	// start the re-configuration by yanking nCONFIG pin (aka LOC_SPARE0 )
	// low, hold it for 3 usec (Tcfg)
	*((volatile unsigned char *)VEGAS_SIOPLD_CONFIG) &= ~SIOPLD_CONFIGN;
	delay_us(3);

	// check nSTATUS (aka LOC_SPARE1) for low (Tcf2st0)
	status = *((volatile unsigned char *)VEGAS_SIOPLD_STATUS);
	if (status & SIOPLD_STATUS)
	{
		// status should be low here
		// print an error message
		printf("fpgaload() - SIO PLD nSTATUS not low after nCONFIG pulsed\n");
		*((volatile unsigned long *)N4_DCS2_LO) = old_dcs2;
		*((volatile unsigned long *)N4_LCST2) = old_lcst2;
		return(1);
	}

	// raise nCONFIG
	*((volatile unsigned char *)VEGAS_SIOPLD_CONFIG) |= SIOPLD_CONFIGN;

	// wait 5 usec (Tcfst1) to start to twiddle
	delay_us(5);

	// check nSTATUS (aka LOC_SPARE1) for high
	status = *((volatile unsigned char *)VEGAS_SIOPLD_STATUS);
	if(!(status & SIOPLD_STATUS))
	{
		// status should be high here
		// print an error message
		printf("fpgaload() - SIO PLD nSTATUS not high 5us after nCONFIG pulsed\n");
		*((volatile unsigned long *)N4_DCS2_LO) = old_dcs2;
		*((volatile unsigned long *)N4_LCST2) = old_lcst2;
		return(2);
	}

	// wait 2 usec (Tst2ws) to start writing
	delay_us(2);

	// loop through config data
	for(i = 0; i < sizeof(fpgadata); i++) 
	{
		// loop through byte lsb to msb
		for(j = 0; j < 8; j++)
		{
			// check nSTATUS (aka LOC_SPARE1) for low
			status = *((volatile unsigned char *)VEGAS_SIOPLD_STATUS);
			if(!(status & SIOPLD_STATUS))
			{
				// status should be high here
				// print an error message
				printf("fpgaload() - SIO PLD nSTATUS low during programming byte=%d bit=%d\n", i, j);
				*((volatile unsigned long *)N4_DCS2_LO) = old_dcs2;
				*((volatile unsigned long *)N4_LCST2) = old_lcst2;
				return(3);
			}

			// get current bit
			// put the bit onto the correct output line (D16)
			// write the bit to fpga
			// delay 1 usec (Tws2b + Tbusy + Trdy2ws)
			bit = (fpgadata[i] >> j) & 0x01;
			bit = bit << 16;
			*((volatile unsigned long *)VEGAS_DCS2_BASE) = bit;
			delay_us(1);
		}
	}

	// done sending program data

	for(i = 0; i <= FPGA_CONF_DONE_TIMEOUT; i++) 
	{
		// wait a usec
		delay_us(1);

		// check CONFIG_DONE (aka LOC_SPARE2) for high
		status = *((volatile unsigned char *)VEGAS_SIOPLD_STATUS);
		if(status & SIOPLD_CFGDONE)
		{
			// all done
			*((volatile unsigned long *)N4_DCS2_LO) = old_dcs2;
			*((volatile unsigned long *)N4_LCST2) = old_lcst2;
			return(0);
		}
	}

	printf("fpgaload() - fpgaload timed out waiting for CFG_DONE\n");
	*((volatile unsigned long *)N4_DCS2_LO) = old_dcs2;
	*((volatile unsigned long *)N4_LCST2) = old_lcst2;
	return(4);
}
