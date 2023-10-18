//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 2 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

// Processor speed in MHz
#define	SPEED	((PROCESSOR_CLOCK_SPEED * 2)/1000000)

#define	HW_NANOS_PER_TICK	(1000 / (SPEED / 2))

#define	ONE_SECOND			((SPEED / 2) * 1000000)

#define	MAX_TIMERS			4
#define	TIMER_START_VAL	0x00ffffff

#define	VTIMER_ENABLE		1
#define	VTIMER_MODE_COUNT	2

// Definition of a virtual timer.  These are count down timers.  They function
// in two mode.  Timer counts to its termination value, issues an interrupt
// (if enabled), and stops.  In count mode, the timer counts to its
// termination value, issues and interrupt (if enabled), reloads the counter,
// and continues to count.
typedef struct virtual_timer
{
	int	control;					// Timer control
	int	start_val;				// Start value for the timer
	int	cur_val;					// Current value of the timer
} virtual_timer_t;

static virtual_timer_t	vtimer[MAX_TIMERS];
static int					vtimers_initialized = 0;
int timerinit(void)
{
	int	i;

	if(vtimers_initialized)
	{
		return(1);
	}
	for(i = 0; i < MAX_TIMERS; i++)
	{
		vtimer[i].control = 0;
		vtimer[i].start_val = 0;
		vtimer[i].cur_val = 0;
	}
	vtimers_initialized = 1;
	return(1);
}

static int get_hardware_timer(void)
{
	int	val;

	// Disable the timer
	*((volatile int *)NILE4_GPT_CNTL_HI_ADDR) = 0;

	// Return the counter value
	val = *((volatile int *)NILE4_GPT_COUNT_LO_ADDR);

	// Enable the timer
	*((volatile int *)NILE4_GPT_CNTL_HI_ADDR) = 1;

	return(val);
}

static void start_hw_timer(int val)
{
	// Disable the timer
	*((volatile int *)NILE4_GPT_CNTL_HI_ADDR) = 0;

	// Load the starting count value
	*((volatile int *)NILE4_GPT_CNTL_LO_ADDR) = val;

	// Clear the count register
	*((volatile int *)NILE4_GPT_COUNT_LO_ADDR) = val;

	// Enable the timer
	*((volatile int *)NILE4_GPT_CNTL_HI_ADDR) = 1;
}

static int get_wdog_timer(void)
{
	int	val;

	// Disable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 0;

	// Return the counter value
	val = *((volatile int *)NILE4_WDOG_COUNT_LO_ADDR);

	// Enable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 1;

	return(val);
}

static void stop_wdog_timer(void)
{
	// Disable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 0;
}

void start_wdog_timer(int val)
{
	// Disable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 0;

	// Load the starting count value
	if(!val)
	{
		*((volatile int *)NILE4_WDOG_CNTL_LO_ADDR) = TIMER_START_VAL;
		*((volatile int *)NILE4_WDOG_COUNT_LO_ADDR) = TIMER_START_VAL;
	}
	else
	{
		*((volatile int *)NILE4_WDOG_CNTL_LO_ADDR) = val;
		*((volatile int *)NILE4_WDOG_COUNT_LO_ADDR) = val;
	}

	// Enable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 1;
}

static void update_timers(void)
{
	int	i;
	int	cur_hw;

	cur_hw = get_hardware_timer();
	for(i = 0; i < MAX_TIMERS; i++)
	{
		if(vtimer[i].control & VTIMER_ENABLE)
		{
			vtimer[i].cur_val -= (TIMER_START_VAL - cur_hw);
			if(vtimer[i].control & VTIMER_MODE_COUNT && vtimer[i].cur_val <= 0)
			{
				vtimer[i].cur_val += vtimer[i].start_val;
			}
			else if(vtimer[i].cur_val <= 0)
			{
				vtimer[i].cur_val = 0;
			}
		}
	}
}

static int get_timer(int num)
{
	update_timers();
	return((vtimer[num].start_val - vtimer[num].cur_val) * HW_NANOS_PER_TICK);
}

static void stop_timer(int num)
{
	update_timers();
	vtimer[num].control &= ~VTIMER_ENABLE;
}

static void start_timer(int num, int val)
{
	vtimer[num].control |= VTIMER_ENABLE;
	if(!val)
	{
		vtimer[num].start_val = TIMER_START_VAL;
	}
	else
	{
		vtimer[num].start_val = val;
	}
	vtimer[num].cur_val = vtimer[num].start_val;
	start_hw_timer(vtimer[num].cur_val);
	update_timers();
}


int timerioctl(register struct iocntb *io, int cmd, int arg)
{
	int	time;

	switch(cmd)
	{
		case FIOCSTOPTIMER0:
		{
			stop_timer(0);
			break;
		}
		case FIOCSTOPTIMER1:
		{
			stop_timer(1);
			break;
		}
		case FIOCSTOPTIMER2:
		{
			stop_timer(2);
			break;
		}
		case FIOCSTOPTIMER3:
		{
			stop_timer(3);
			break;
		}
		case FIOCSTARTTIMER0:
		{
			start_timer(0, arg);
			break;
		}
		case FIOCSTARTTIMER1:
		{
			start_timer(1, arg);
			break;
		}
		case FIOCSTARTTIMER2:
		{
			start_timer(2, arg);
			break;
		}
		case FIOCSTARTTIMER3:
		{
			start_timer(3, arg);
			break;
		}
		case FIOCGETTIMER0:
		{
			*((int *)arg) = get_timer(0);
			break;
		}
		case FIOCGETTIMER1:
		{
			*((int *)arg) = get_timer(1);
			break;
		}
		case FIOCGETTIMER2:
		{
			*((int *)arg) = get_timer(2);
			break;
		}
		case FIOCGETTIMER3:
		{
			*((int *)arg) = get_timer(3);
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
	start_timer(0, ONE_SECOND);
}
