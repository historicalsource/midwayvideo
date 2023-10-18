//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#include	<io.h>

#define	START_ADDRESS	0x80100000
#define	END_ADDRESS		0x80800000

int	kbhit(void);

void show_addr(int address)
{
	int	i;
	char	val;

	for(i = 7; i >= 0; i--)
	{
		val = (address >> (i * 4)) & 0xf;
		if(val > 9)
		{
			val += 0x37;
		}
		else
		{
			val += 0x30;
		}
		_write(1, &val, 1);
	}
}

#if defined(TEST) || (PHOENIX_SYS & 2)
static char	mt_prompt[] = {"Testing Memory"};
static char	mt_cr[] = {"\r"};
static char	mt_nl[] = {"\n"};
static char	mt_error[] = {"\r\nMemory ERROR\r\n\r\n"};

//
// memory_test() - This function tests memory by writing an incrementing
// pattern with no zeros to the entire memory region then reads back the
// memory region and checks the integrity of the data.  This is a through
// test that will catch both data and address errors.
//
int memory_test(void)
{
	unsigned int	addr;
	unsigned char	tdata = 1;
	unsigned char	cdata;

	// Show the prompt
	_write(1, mt_prompt, sizeof(mt_prompt) - 1);

	// And a couple of new lines
	_write(1, mt_nl, sizeof(mt_nl) - 1);
	_write(1, mt_cr, sizeof(mt_cr) - 1);

	// Write pass
	for(addr = START_ADDRESS; addr < END_ADDRESS; addr++)
	{
		// Show address every 16k
		if(!(addr & 0x3fff))
		{
			show_addr(addr);
			_write(1, mt_cr, 1);
		}

		// Write the data
		*((unsigned char *)addr) = tdata;

		// Increment the data
		++tdata;

		// If data is 0 (zero) reset it to 1
		if(!tdata)
		{
			tdata = 1;
		}
	}

	// Show Last Address
	show_addr(addr);
	_write(1, mt_cr, 1);

	// Read Pass
	tdata = 1;
	for(addr = START_ADDRESS; addr < END_ADDRESS; addr++)
	{
		// Show address every 16k
		if(!(addr & 0x3fff))
		{
			show_addr(addr);
			_write(1, mt_cr, 1);
		}

		// Read the data
		cdata = *((unsigned char *)addr);

		// Is the data correect ?
		if(cdata != tdata)
		{
			// NOPE - Display the error
			_write(1, mt_error, sizeof(mt_error) - 1);

			// Return Address of error
			return(addr);
		}

		// Increment the test data
		++tdata;

		// If test data is 0 (zero), reset it to 1
		if(!tdata)
		{
			tdata = 1;
		}
	}

	// Show Last Address
	show_addr(addr);

	// A newline
	_write(1, mt_cr, 1);
	_write(1, mt_nl, sizeof(mt_nl) - 1);

	// Return sucess
	return(0);
}
#endif
