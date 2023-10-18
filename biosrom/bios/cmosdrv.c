//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

static unsigned int	*transfer_address = (unsigned int *)CMOS_RAM_ADDR;
static unsigned int	cmos_size = CMOS_SIZE;
static int				cmos_initialized = 0;

#define	CMOS_END_ADDR	((CMOS_RAM_ADDR + (CMOS_SIZE << 2)))

int cmosinit(void)
{
	// Find and set the size of the cmos memory
	if(!cmos_initialized)
	{
		cmos_size = CMOS_SIZE;
		cmos_initialized = 1;
	}
	return(0);
}

int cmosread(register struct iocntb *io, char *buf, int count)
{
	unsigned int	*addr;
	int				save_count;

	if(count > (CMOS_END_ADDR - (int)transfer_address))
	{
		count = (CMOS_END_ADDR - (int)transfer_address) >> 2;
	}
	save_count = count;
	addr = transfer_address;
	while(count--)
	{
		*buf++ = (char)*addr++;
	}
	return(save_count);
}

int cmoswrite(register struct iocntb *io, char *buf, int count)
{
	unsigned int	*addr;
	int				save_count;

	if(count > (CMOS_END_ADDR - (int)transfer_address))
	{
		count = (CMOS_END_ADDR - (int)transfer_address) >> 2;
	}
	save_count = count;
	addr = transfer_address;
	while(count--)
	{
#if (PHOENIX_SYS & SA1)
		*((volatile int *)CMOS_UNLOCK_ADDR) = 0;
#elif (PHOENIX_SYS & SEATTLE)
		*((volatile int *)CMOS_UNLOCK_ADDR) = 1;
#endif
		*addr++ = (int)*buf++;
	}
	return(save_count);
}


int cmosioctl(register struct iocntb *io, int cmd, int arg)
{
	switch(cmd)
	{
		case FIOCSETCMOSADDR:
		{
			if((unsigned int)arg < CMOS_RAM_ADDR || (unsigned int)arg >= CMOS_END_ADDR)
			{
				return(CMOS_INVALID_ADDRESS);
			}
			transfer_address = (unsigned int *)arg;
			break;
		}
		case FIOCGETCMOSADDR:
		{
			*((int *)arg) = CMOS_RAM_ADDR;
			break;
		}
		case FIOCGETCMOSSIZE:
		{
			*((int *)arg) = cmos_size;
			break;
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}

