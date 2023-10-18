//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>

int rom_diag_exec(unsigned int entry_address, int *arg_ptr);

int rdiag_init(void)
{
	int	args[4] = {0, 0, 0, 0};

#if (PHOENIX_SYS & SEATTLE)
	args[3] = *((volatile int *)0xbfd00008);
	if(args[3] != 0x47414944)		// "DIAG"
	{
		return(1);
	}

	// Map and enable the vertical retrace interrupt regardless of whether
	// or not the scsi card initializes properly
	*((volatile int *)ICPLD_INT_MAP_REG) |= (2<<14);
	*((volatile int *)ICPLD_INT_ENBL_REG) |= (1<<7);
#endif

	// Run the ROM based diagnostics
	rom_diag_exec(0x80100000, args);

	// Return OK
	return(1);
}
