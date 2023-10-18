//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 9 $
//
// $Author: Mlynch $
//
#include	<system.h>
#if (SYSTEM_CONTROLLER & GT64010)
#include	<io.h>
#include	<gt64010.h>
#include	<ioctl.h>

#define	TIMER_START_VAL	0x00ffffff

#if defined(SPACE)
#define	TIMER_PERIOD	21
#else
#define	TIMER_PERIOD	20
#endif

#define	WDOG_TIME	(25000000/TIMER_PERIOD)

#define	TIMER_100MS	(100000000/TIMER_PERIOD)

static int	timer_start_val[4];
int			ide_timer_start;

int timerioctl(register struct iocntb *io, int cmd, int arg)
{
	int	time;

	switch(cmd)
	{
		case FIOCSTOPTIMER0:
		{
#if (WDOG_TIMER == TIMER3_WDOG)
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 0);
#endif
			// Done
			break;
		}
		case FIOCSTOPTIMER1:
		{
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 2);

			// Done
			break;
		}
		case FIOCSTOPTIMER2:
		{
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 4);

			// Done
			break;
		}
		case FIOCSTOPTIMER3:
		{
#if (WDOG_TIMER == TIMER0_WDOG)
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 6);
#endif
			// Done
			break;
		}
		case FIOCSTARTTIMER0:
		{
#if (WDOG_TIMER == TIMER0_WDOG)
			if(get_scsi_device_number() < 0)
			{
				*((volatile int *)WATCHDOG_ADDR) = 0;
			}
#endif
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 0);

			// Load the count
			if(!arg)
			{
				*((volatile int *)(GT_64010_BASE + 0x850)) = TIMER_START_VAL;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (1 << 0);

				timer_start_val[0] = TIMER_START_VAL;
			}
			else
			{
#if (WDOG_TIMER == TIMER0_WDOG)
				if(arg > TIMER_100MS)
				{
					arg = TIMER_100MS;
				}
#endif
				*((volatile int *)(GT_64010_BASE + 0x850)) = arg;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 0);

				timer_start_val[0] = arg;
			}

			ide_timer_start = timer_start_val[0];

			// Done
			break;
		}
		case FIOCSTARTTIMER1:
		{
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 2);

			// Load the count
			if(!arg)
			{
				*((volatile int *)(GT_64010_BASE + 0x854)) = TIMER_START_VAL;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (1 << 2);

				timer_start_val[1] = TIMER_START_VAL;
			}
			else
			{
				*((volatile int *)(GT_64010_BASE + 0x854)) = arg;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 2);

				timer_start_val[1] = arg;
			}

			// Done
			break;
		}
		case FIOCSTARTTIMER2:
		{
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 4);

			// Load the count
			if(!arg)
			{
				*((volatile int *)(GT_64010_BASE + 0x858)) = TIMER_START_VAL;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (1 << 4);

				timer_start_val[2] = TIMER_START_VAL;
			}
			else
			{
				*((volatile int *)(GT_64010_BASE + 0x858)) = arg;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 4);

				timer_start_val[2] = arg;
			}

			// Done
			break;
		}
		case FIOCSTARTTIMER3:
		{
#if (WDOG_TIMER == TIMER0_WDOG)
			// Disable the timer
			*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 6);

			// Load the count
			if(!arg)
			{
				*((volatile int *)(GT_64010_BASE + 0x85c)) = TIMER_START_VAL;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (1 << 6);

				timer_start_val[3] = TIMER_START_VAL;
			}
			else
			{
				*((volatile int *)(GT_64010_BASE + 0x85c)) = arg;

				// Enable the timer
				*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 6);

				timer_start_val[3] = arg;
			}
#endif
			ide_timer_start = timer_start_val[3];

			// Done
			break;
		}
		case FIOCGETTIMER0:
		{
#if (WDOG_TIMER == TIMER3_WDOG)
			// Read from the timer
			time = *((volatile int *)(GT_64010_BASE + 0x850));

			// Calculate the ticks
			time = timer_start_val[0] - time;

			// Convert to nano-seconds
			time *= 20;

			// Set the value
			*((int *)arg) = time;
#else
			*((int *)arg) = 0;
#endif
			// Done
			break;
		}
		case FIOCGETTIMER1:
		{
			// Read from the timer
			time = *((volatile int *)(GT_64010_BASE + 0x854));

			// Calculate the ticks
			time = timer_start_val[1] - time;

			// Convert to nano-seconds
			time *= 20;

			// Set the value
			*((int *)arg) = time;

			// Done
			break;
		}
		case FIOCGETTIMER2:
		{
			// Read from the timer
			time = *((volatile int *)(GT_64010_BASE + 0x858));

			// Calculate the ticks
			time = timer_start_val[2] - time;

			// Convert to nano-seconds
			time *= 20;

			// Set the value
			*((int *)arg) = time;

			// Done
			break;
		}
		case FIOCGETTIMER3:
		{
#if (WDOG_TIMER == TIMER0_WDOG)
			// Read from the timer
			time = *((volatile int *)(GT_64010_BASE + 0x85c));

			// Calculate the ticks
			time = timer_start_val[3] - time;

			// Convert to nano-seconds
			time *= 20;

			// Set the value
			*((int *)arg) = time;
#else
			*((int *)arg) = 0;
#endif
			// Done
			break;
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}

void start_timer_0(void)
{
	// Disable the timer
	*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 0);

	// Load the count (1/2 second)
	*((volatile int *)(GT_64010_BASE + 0x850)) = 25000000;

	// Enable the timer (timer mode)
	*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 0);

	// Enable the interrupt for the timer
	*((volatile int *)(GT_64010_BASE + 0xc1c)) &= ~0x100;
}

int	not_debug_mode = 0;

void start_dog_feed(void)
{
	// Is a SCSI card attached ?
	if(get_scsi_device_number() >= 0 || not_debug_mode)
	{
		// YES - Debug mode
		return;
	}

	// Make sure the Gt64010 interrupt is enabled
	enable_ip(GALILEO_INT);

	// Set normal mode
	not_debug_mode = 1;

	// Enable the watchdog
	*((volatile int *)WATCHDOG_ADDR) = 0;

	// Disable the timer
#if (WDOG_TIMER == TIMER3_WDOG)
	*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 6);
#else
	*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 0);
#endif

	// Load the count
#if (WDOG_TIMER == TIMER3_WDOG)
	*((volatile int *)(GT_64010_BASE + 0x85c)) = TIMER_100MS;
#else
	*((volatile int *)(GT_64010_BASE + 0x850)) = TIMER_100MS;
#endif

	// Set the starting time
#if (WDOG_TIMER == TIMER3_WDOG)
	timer_start_val[3] = TIMER_100MS;
#else
	timer_start_val[0] = TIMER_100MS;
#endif
	ide_timer_start = TIMER_100MS;

	// Make sure the any pending interrupt from this is cleared before
	// Enabling the interrupt
#if (WDOG_TIMER == TIMER3_WDOG)
	*((volatile int *)GT_INT_CAUSE) &= ~(1<<11);
#else
	*((volatile int *)GT_INT_CAUSE) &= ~(1<<8);
#endif

	// Enable the timer interrupt
#if (WDOG_TIMER == TIMER3_WDOG)
	*((volatile int *)GT_INT_CPU_MASK) |= (1<<11);
#else
	*((volatile int *)GT_INT_CPU_MASK) |= (1<<8);
#endif

	// Enable the timer
#if (WDOG_TIMER == TIMER3_WDOG)
	*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 6);
#else
	*((volatile int *)(GT_64010_BASE + 0x864)) |= (3 << 0);
#endif

	// Enable the watchdog
	*((volatile int *)WATCHDOG_ADDR) = 0;
}

#if (WDOG_TIMER == TIMER0_WDOG)
static int	timer_ints = 0;
#endif

void feed_the_dog(void)
{
#if (WDOG_TIMER == TIMER0_WDOG)
	int	divisor = 500000000 / (timer_start_val[0] * TIMER_PERIOD);

	// Make sure divisor is good
	if(divisor <= 0)
	{
		divisor = 1;
	}

	// Time to feed the dog ?
	if(!(timer_ints % divisor))
	{
		// YES - Reset interrupt count
		timer_ints = 0;

		// Feed the dog
		*((volatile int *)WATCHDOG_ADDR) = 0;

		// Flash an LED
		*((volatile int *)LED_ADDR) ^= 4;
	}

	// Increment timer interrupt count
	timer_ints++;
#else
	*((volatile int *)WATCHDOG_ADDR) = 0;
	*((volatile int *)LED_ADDR) ^= 4;
#endif
}
#endif
