//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

typedef struct coin_counter_control
{
	int	count;
	int	ticks;
	int	running;
} coin_counter_control_t;

static coin_counter_control_t	coin_counter_control[4];

static int	coin_drv_initialized = 0;
static int	coin_drv_open = 0;

int coininit(void)
{
	int	i;

	//
	// Initialize coin counter control structures
	//
	for(i = 0; i < sizeof(coin_counter_control)/sizeof(coin_counter_control_t); i++)
	{
		coin_counter_control[i].count = 0;
		coin_counter_control[i].ticks = 0;
		coin_counter_control[i].running = 0;
	}

	//
	// Turn off all coin counter drivers
	//
	*((volatile short *)IOASIC_COIN_METERS) = 0x10;
	*((volatile short *)IOASIC_COIN_METERS) = 0;

	//
	// Mark driver initialized
	//
	coin_drv_initialized = 1;
}


int coinopen(register struct iocntb *io)
{
	//
	// Reset initialized flag
	//
	coin_drv_initialized = 0;

	//
	// Initialize counters
	//
	coininit();

	//
	// Mark driver open
	//
	coin_drv_open = 1;

	//
	// Return OK
	//
	return(0);
}

int coinclose(register struct iocntb *io)
{
	//
	// Mark driver closed
	//
	coin_drv_open = 0;

	//
	// Return OK
	//
	return(0);
}

int coinioctl(struct iocntb *io, int cmd, int arg)
{
	//
	// Is driver NOT initialized or open ?
	//
	if(!coin_drv_initialized || !coin_drv_open)
	{
		//
		// Return fail
		//
		return(-1);
	}

	//
	// Do the requested command
	//
	switch(cmd)
	{
		case CIOCCLEARCOUNTS:
		{
			coin_drv_initialized = 0;
			coininit();
			break;
		}
		case CIOCADDCOUNT0:
		{
			coin_counter_control[0].count += arg;
			break;
		}
		case CIOCADDCOUNT1:
		{
			coin_counter_control[1].count += arg;
			break;
		}
		case CIOCADDCOUNT2:
		{
			coin_counter_control[2].count += arg;
			break;
		}
		case CIOCADDCOUNT3:
		{
			coin_counter_control[3].count += arg;
			break;
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}


void coinintr(void)
{
	int	i;
	short	val;

	//
	// Has the driver been initialized ?
	//
	if(!coin_drv_initialized)
	{
		//
		// NOPE - do nothing
		//
		return;
	}

	//
	// Loop through all of the coin counters
	//
	for(i = 0; i < sizeof(coin_counter_control)/sizeof(coin_counter_control_t); i++)
	{
		//
		// Is this counter supposed to be counting ?
		//
		if(coin_counter_control[i].count)
		{
			//
			// YES - Is it already in a count sequence ?
			//
			if(!coin_counter_control[i].running)
			{
				//
				// NOPE - Set its sequence tick count
				//
				coin_counter_control[i].ticks = 2;

				//
				// Flag it as running
				//
				coin_counter_control[i].running = 1;
			}

			//
			// Has the tick count for this counter reached 0 ?
			//
			if(!coin_counter_control[i].ticks)
			{
				//
				// YES - Set counter state to idle
				//
				coin_counter_control[i].running = 0;

				//
				// Decrement count
				//
				coin_counter_control[i].count--;
			}
			
			//
			// Is this counters tick value at initial value ?
			//
			if(coin_counter_control[i].ticks == 2)
			{
				//
				// YES - Turn coin counter on
				//
				val = *((volatile short *)IOASIC_COIN_METERS);
				val |= (1<<i);
				val |= 0x10;
				*((volatile short *)IOASIC_COIN_METERS) = val;
				val &= ~0x10;
				*((volatile short *)IOASIC_COIN_METERS) = val;
			}

			//
			// Is this counter tick value 1/2 of initial value ?
			//
			else if(coin_counter_control[i].ticks == 1)
			{
				//
				// YES - Turn coin counter off
				//
				val = *((volatile short *)IOASIC_COIN_METERS);
				val &= ~((1<<i) & 0xf);
				val |= 0x10;
				*((volatile short *)IOASIC_COIN_METERS) = val;
				val &= ~0x10;
				*((volatile short *)IOASIC_COIN_METERS) = val;
			}

			//
			// Decrement this counters tick count
			//
			coin_counter_control[i].ticks--;
		}
	}
}
