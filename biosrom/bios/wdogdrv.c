//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 3 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

int	reset_status = 0;
int	do_the_dog = 0;

int wdogwrite(register struct iocntb *io, char *buf, int count)
{
#if (!(PHOENIX_SYS & VEGAS))
	*((volatile int *)WATCHDOG_ADDR) = 0;
#else
	*((volatile char *)WATCHDOG_ADDR) = 0;
#endif
	return(count);
}

int wdogioctl(register struct iocntb *io, int cmd, int arg)
{
	switch(cmd)
	{
		case FIOCGETWDOGSTATUS:
		{
			*((int *)arg) = reset_status;
			break;
		}
		case FIOCWALKTHEDOG:
		{
			if(!do_the_dog)
			{
				// Set flag to tell interrupt handler to walk the dog
				do_the_dog = 1;

				// Make sure the Galileo timer 0 interrupt is enabled
#if (!(PHOENIX_SYS & VEGAS))
				*((volatile int *)(GT_64010_BASE + 0xc1c)) &= ~0x100;
#endif

				// Kick the dog to enable it
#if (!(PHOENIX_SYS & VEGAS))
				*((volatile int *)WATCHDOG_ADDR) = 0;

				// Start the watchdog kicker timer
				start_timer_0();
#else
				*((volatile char *)WATCHDOG_ADDR) = 0;
#endif

			}
			break;
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}

void kick_the_dog(void)
{
#if (!(PHOENIX_SYS & VEGAS))
	*((volatile int *)WATCHDOG_ADDR) = 0;
#else
	*((volatile char *)WATCHDOG_ADDR) = 0;
#endif
}
