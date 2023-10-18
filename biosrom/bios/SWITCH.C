//
// switch.c - switch driver for VEGAS
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#ifndef TEST
#include	<system.h>
#include	<ioctl.h>

#ifndef NULL
#define	NULL	0
#endif

//
// NOTE about the on_trans_accum field below.  This field is only reset in
// the initialization and get_inputs functions.  This field is used to count
// the number of times the switch either transitioned to an ON state or, if
// the auto repeat feature is enabled, to count the number of virtual ON
// state transitions that have occurred since the last time get_inputs was
// called by the application.
//
typedef struct switch_info
{
	unsigned short	*addr;					// Address of port for this switch
	unsigned int	bit_mask; 				// Bit mask for this switch
	unsigned int	on_debounce;			// On debounce time
	unsigned int	off_debounce;			// Off debounce time
	unsigned int	on_count;				// On time count
	unsigned int	off_count;				// Off time count
	unsigned int	state;					// Current state
	unsigned int	on_accum_count;		// Time on
	unsigned int	off_accum_count;		// Time off
	unsigned int	on_trans_accum;		// On transition accumulations
	unsigned int	repeat_delay;			// Auto repeat delay
	unsigned int	repeat_time;			// Inter-repeat time
} switch_info_t;

//
// Definitions of some defaults (ms)
//
#define	DEFAULT_ON_DEBOUNCE			4		// On debounce time (ms)
#define	DEFAULT_OFF_DEBOUNCE			32		// Off debounce time (ms)
#define	DEFAULT_REPEAT_DELAY			0		// Auto repeat delay time (ms)
#define	DEFAULT_REPEAT_TIME			500	// Auto repeat time (ms)

//
// Definitions of switch states
//
#define	SWITCH_STATE_ON		1
#define	SWITCH_STATE_OFF		0

typedef struct inputs
{
	switch_info_t		switches[MAX_SWITCHES];
	unsigned int		a2d[MAX_A2D_CHANNELS];
} inputs_t;

static inputs_t	inputs;
static int			switches_initialized = 0;

void a2d_intr(void);

//
// Resets all of the switch mappings to their defaults
//
void set_default_switch_map(void)
{
	register	int				i;
	register switch_info_t	*sw;

	sw = inputs.switches;
	for(i = 0; i < MAX_SWITCHES; i++)
	{
		if(i < SWID_P3_UP)
		{
			sw->addr = (unsigned short *)IOASIC_PLAYER_12;
		}
		else if(i < SWID_LEFT_COIN)
		{
			sw->addr = (unsigned short *)IOASIC_PLAYER_34;
		}
		else if(i < SWID_DIP0_0)
		{
			sw->addr = (unsigned short *)IOASIC_COIN_INPUT;
		}
		else
		{
			sw->addr = (unsigned short *)IOASIC_DIP_SWITCHES;
		}
		sw->bit_mask = 1 << (i & 0xf);
	}
}


//
// Function used to allow the user to remap any particular input to a
// particular bit position in the data presented by get_inputs.
//
void set_switch_map(int from, int to)
{
	register switch_info_t	*sw;

	if(from < 0 || from >= MAX_SWITCHES)
	{
		return;
	}
	if(to < 0 || to >= MAX_SWITCHES)
	{
		return;
	}
	sw = &inputs.switches[to];
	sw->bit_mask = 1 << (from & 0xf);
	if(from < SWID_P3_UP)
	{
		sw->addr = (unsigned short *)IOASIC_PLAYER_12;
	}
	else if(from < SWID_LEFT_COIN)
	{
		sw->addr = (unsigned short *)IOASIC_PLAYER_34;
	}
	else if(from < SWID_DIP0_0)
	{
		sw->addr = (unsigned short *)IOASIC_COIN_INPUT;
	}
	else
	{
		sw->addr = (unsigned short *)IOASIC_DIP_SWITCHES;
	}
}

//
// Initialization function called at system init time
//
static void init_inputs(void)
{
	register	int				i;
	register switch_info_t	*sw;

	sw = inputs.switches;
	for(i = 0; i < MAX_SWITCHES; i++)
	{
		sw->on_debounce = DEFAULT_ON_DEBOUNCE;
		sw->off_debounce = DEFAULT_OFF_DEBOUNCE;
		sw->on_count = 0;
		sw->off_count = 0;
		sw->state = SWITCH_STATE_OFF;
		sw->on_accum_count = 0;
		sw->off_accum_count = 0;
		if(i < SWID_P3_UP)
		{
			sw->addr = (unsigned short *)IOASIC_PLAYER_12;
		}
		else if(i < SWID_LEFT_COIN)
		{
			sw->addr = (unsigned short *)IOASIC_PLAYER_34;
		}
		else if(i < SWID_DIP0_0)
		{
			sw->addr = (unsigned short *)IOASIC_COIN_INPUT;
		}
		else
		{
			sw->addr = (unsigned short *)IOASIC_DIP_SWITCHES;
		}
		sw->bit_mask = 1 << (i & 0xf);
		sw->on_trans_accum = 0;
		sw->repeat_delay = DEFAULT_REPEAT_DELAY;
		sw->repeat_time = DEFAULT_REPEAT_TIME;
		sw++;
	}
	switches_initialized = 1;
}


static int	a2d_chan_num = -1;

//
// Input scanning function called once every milli-second by the milli-second
// timer interrupt handler.
//
void scan_inputs(void)
{
	register int							i;
	register switch_info_t				*sw = inputs.switches;
	register volatile unsigned short	*addr = NULL;
	register unsigned short				val;
	static int								scan_count = 0;


	//
	// Have the switches been initialized ?
	//
	if(!switches_initialized)
	{
		//
		// NOPE - Initialize them
		//
		init_inputs();
	}

	//
	// Loop through and read all inputs
	//
	for(i = 0; i < MAX_SWITCHES; i++)
	{
		//
		// Is the address for this switch the same as for the last ?
		//
		if(addr != sw->addr)
		{
			//
			// Nope - set the address
			//
			addr = sw->addr;

			//
			// Read the data
			//
			val = ~(*addr);
		}

		//
		// Is the switch on ?
		//
		if(val & sw->bit_mask)
		{
			//
			// YES - Increment this switches ON count
			//
			sw->on_count++;

			//
			// Is the on count for this switch equal to it on debounce time ?
			//
			if(sw->on_count == sw->on_debounce)
			{
				//
				// YES - Reset the accumulated on time for this switch
				//
				sw->on_accum_count = 0;

				//
				// Set this switch state to ON
				//
				sw->state = SWITCH_STATE_ON;

				//
				// Increment this switches ON transistion count
				//
				sw->on_trans_accum++;
			}

			//
			// Is this switches state ON ?
			//
			else if(sw->state == SWITCH_STATE_ON)
			{
				//
				// YES - Increment this switches accumulated on time
				//
				sw->on_accum_count++;

				//
				// Has the switch been ON for greater than or equal amount of
				// time for the auto-repeat delay ?
				//
				if(sw->on_accum_count >= sw->repeat_delay && sw->repeat_delay != 0)
				{
					//
					// Has the switch been ON for exactly the auto-repeat delay
					// time ?
					//
					if(sw->on_accum_count == sw->repeat_delay)
					{
						//
						// YES - Increment the ON transistion accumulations
						//
						sw->on_trans_accum++;
					}

					//
					// Has the switch been on for a multiple of the auto-repeat
					// time above and beyond the auto-repeat delay time ?
					//
					else if(!((sw->on_accum_count - sw->repeat_delay) % sw->repeat_time))
					{
						//
						// YES - Increment the On transition accumulations
						//
						sw->on_trans_accum++;
					}
				}
			}

			//
			// Reset this switches off time count
			//
			sw->off_count = 0;
			sw->off_accum_count = 0;
		}

		//
		// Switch is OFF
		//
		else
		{
			//
			// Increment this switches OFF time
			//
			sw->off_count++;

			//
			// Does this switches OFF count equal its OFF debounce time ?
			//
			if(sw->off_count == sw->off_debounce)
			{
				//
				// YES - Reset this switches accumulated OFF time
				//
				sw->off_accum_count = 0;

				//
				// Set the switch state to OFF
				//
				sw->state = SWITCH_STATE_OFF;
			}

			//
			// Switch state OFF ?
			//
			else if(sw->state == SWITCH_STATE_OFF)
			{
				//
				// YES - Increment this switches accumulated OFF time
				//
				sw->off_accum_count++;
			}

			//
			// Reset this switches on time count
			//
			sw->on_count = 0;
			sw->on_accum_count = 0;
		}
		sw++;
	}

	//
	// We start A2D conversions every 8ms
	//
	if(!(scan_count & 7) && a2d_chan_num < 0)
	{
		a2d_intr();
	}
	scan_count++;
}


//
// Analog to digital convertor interrupt handler.  Called at system
// initialization time to start conversions and then subsequently called by
// receipt of the interrupt from the analog to digital convertor device.
//
void a2d_intr(void)
{
	//
	// Is channel number >= 0 ?
	//
	if(a2d_chan_num >= 0)
	{
		//
		// YES - get data for the channel
		//
		inputs.a2d[a2d_chan_num] = *((volatile char *)A2D_ADDR);

		//
		// 8 bit device - shift data up 2 bits
		//
		inputs.a2d[a2d_chan_num] <<= 2;
	}

	//
	// Enable the A2D interrupt
	//
	else
	{
		*((volatile char *)INT_ENBL_REG_ADDR) |= (1 << (SIO_A2D_HANDLER_NUM - SIO_WDOG_TIMER_HANDLER_NUM));
	}

	//
	// Increment channel number
	//
	a2d_chan_num++;

	//
	// Last channel ?
	//
	if(a2d_chan_num == MAX_A2D_CHANNELS)
	{
		//
		// YES - Don't do anymore conversions until told to
		//
		a2d_chan_num = -1;

		//
		// Done
		//
		return;
	}

	//
	// Start new conversion for that channel
	//
	*((volatile char *)A2D_ADDR) = a2d_chan_num;
}


//
// Function called by the application to retrieve the input data.
//
void get_inputs(idata_t *idat)
{
	register unsigned short	*od = &idat->p12;
	register switch_info_t	*sw = inputs.switches;
	register int				i;

	//
	// Fill in all of the on transition accumulators and reset the equivelents
	// in the inputs structure and set all of the bits in the idata structure.
	//
	for(i = 0; i < MAX_SWITCHES; i++)
	{
		//
		// Is the switch state ON ?
		//
		if(sw->state == SWITCH_STATE_ON)
		{
			//
			// YES - Set the cooresponding bit
			//
			*od |= (1 << (i & 0xf));
		}

		//
		// Switch state off
		//
		else
		{
			//
			// Reset the cooresponding bit
			//
			*od &= ~(1 << (i & 0xf));
		}

		//
		// Time to increment to next 16 bits ?
		//
		if(!((i + 1) & 0xf))
		{
			//
			// YES - invert the data (for compatability
			//
			*od ^= 0xffff;

			//
			// Increment pointer
			//
			od++;
		}

		//
		// Set ON transition count
		//
		idat->on_transitions[i] = sw->on_trans_accum;

		//
		// Reset ON transition count
		//
		sw->on_trans_accum = 0;

		//
		// Next switch
		//
		sw++;
	}

	//
	// Copy data for analog to digital convertor inputs
	//
	for(i = 0; i < MAX_A2D_CHANNELS; i++)
	{
		idat->a2d[i] = inputs.a2d[i];
	}
}
#endif
