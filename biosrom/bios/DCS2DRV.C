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

#if (PHOENIX_SYS & VEGAS)
extern volatile unsigned long	watchdog_count;
void snd_delay(volatile int delay_time);
#define HTS_DATA_REALTIME  0x8080	 /* realtime bit clear for load while playing */		
#endif


#define	TIMER_START_VAL	0x00ffffff

/* delays and timeouts for values on order of microseconds */
/* this is for delay between word xfers */
#if (!(PHOENIX_SYS & VEGAS))
#define SND_DELAY_MULTIPLIER		500000
#define SND_TIMEOUT_MULTIPLIER	500000
#define SND_TIMEOUT					2 * SND_TIMEOUT_MULTIPLIER
#else
#define SND_TIMEOUT					1
#endif

#define OK 0
#define ERROR 0xEEEE

/* status register bit fields */
#define SND_MONITOR_READY  0x0A      /* ack returned by snd boot rom */
#define SND_OPSYS_READY    0x0C      /* ack returned by snd op sys */
#define STH_DATA_READY     0x0040        /* snd sys has data for us */
#define HTS_DATA_EMPTY     0x0080        /* OK to send data to snd sys */

#define SND_MASK16 0x0000FFFF

#define SND_BYTE0  0x000000FF
#define SND_BYTE1  0x0000FF00
#define SND_BYTE2  0x00FF0000
#define SND_BYTE3  0xFF000000

#define SND_CMD_LOAD_DRAM      0x55D0  /* download to D/RAM immediate */

static int	sound_initialized = 0;

#if (!(PHOENIX_SYS & VEGAS))
static void snd_reset(void);
#else
void snd_reset(void);
#endif

#if (!(PHOENIX_SYS & VEGAS))

static void start_timer3(void)
{
	// Disable the timer
	*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 6);

	// Load the count
	*((volatile int *)(GT_64010_BASE + 0x85c)) = TIMER_START_VAL;

	// Enable the timer
	*((volatile int *)(GT_64010_BASE + 0x864)) |= (1 << 6);
}

static int get_time3(void)
{
	int	time;

	// Read from the timer
	time = *((volatile int *)(GT_64010_BASE + 0x85c));

	// Calculate the ticks
	time = TIMER_START_VAL - time;

	// Convert to nano-seconds
	return(time);
}

static void stop_timer3(void)
{
	*((volatile int *)(GT_64010_BASE + 0x864)) &= ~(3 << 6);
}
#endif

int sound_init(void)
{
#if (!(PHOENIX_SYS & VEGAS))
	// Reset the sound system
	snd_reset();
#endif

	// Set initialized flag
	sound_initialized = 1;

	// Return success
	return(0);
}


#if (!(PHOENIX_SYS & VEGAS))
static void snd_delay(volatile int delay_time)
{
	int	time;

	// Adjust delay time for tiemr	
	delay_time *= SND_DELAY_MULTIPLIER;

	// Fire up a timer
	start_timer3();

	// Wait for it to expire
	do
	{
		time = get_time3();
	}
	while(time < delay_time);

	// Stop the timer
	stop_timer3();
}


static void snd_reset(void)
{
	int	val;

   /* reset bit is bit 0 of I/O ASIC sound control */
   /* 0 = reset, 1 = active, not reset */

	// Assert the reset
	*((volatile int *)IOASIC_SOUND_CONTROL) = 0;

	// Wait a bit
   snd_delay(2);

	// De-assert the reset
	*((volatile int *)IOASIC_SOUND_CONTROL) = 0x1;

	// Wait a bit
   snd_delay(2);

   // bit 15 of I/O ASIC main ctrl enables
   // 'C31 write back mode

	// Get current value of I/O ASIC control reg
	val = *((volatile int *)IOASIC_CONTROL);

	// Turn off bit 15
	val &= ~0x8000;

	// Write it back
	*((volatile int *)IOASIC_CONTROL) = val;
}

static int snd_get_data(unsigned int *data)
{
	int	status;
	int	time;

   // Start a timer to abort in case sound system is dead
	start_timer3();

	// Wait for the data to be available
	status = *((volatile int *)IOASIC_SOUND_STATUS);
	while(!(status & STH_DATA_READY))
	{
		// Grab the time
		time = get_time3();

		// Timeout ?
		if(time > SND_TIMEOUT)
		{
			// YES - Stop the timer
			stop_timer3();

			// Return error
			return(ERROR);
		}

		// Read the status again
		status = *((volatile int *)IOASIC_SOUND_STATUS);
	}

	// Shut down the timer
	stop_timer3();

	// Get the data
	*data = *((volatile int *)IOASIC_SOUND_DATA_IN);
	*data &= 0xffff;

   // In 'C31 mode a read from the sound DSP
   // must be followed by a write.  This clears the data ready bit.
	*((volatile int *)IOASIC_SOUND_DATA_IN) = 0;

	// Return sucess
	return(OK);
}


static int snd_send_data(unsigned int data)
{
	int	status;
	int	time;

	// Start a timer to abort if sound system is hung
	start_timer3();

	// Wait for port to be available
	status = *((volatile int *)IOASIC_SOUND_STATUS);
	while(!(status & HTS_DATA_EMPTY))
	{
		// Get the time
		time = get_time3();

		// Have we timed out ?
		if(time > SND_TIMEOUT)
		{
			// YES - stop the timer
			stop_timer3();

			// Return error
			return(ERROR);
		}

		// Read the status
		status = *((volatile int *)IOASIC_SOUND_STATUS);
	}

	// Stop the timer
	stop_timer3();

	// write the data
	*((volatile int *)IOASIC_SOUND_DATA_OUT) = (data & SND_MASK16);

	// Return sucess
	return(OK);
}


int snd_send_command (unsigned int command)
{
	int	status;
	int	time;

	// wait for the monitor ready signal to show up
	// it should be there if we called this function, but wait
	// for it nonetheless

	// Start a timer in case sound system is hung
	start_timer3();

	// Get the status
	status = *((volatile int *)IOASIC_SOUND_STATUS);

	// Wait for the port to be ready
	while(!(status & STH_DATA_READY))
	{
		// Get the time
		time = get_time3();

		// Have we timed out ?
		if(time > SND_TIMEOUT)
		{
			// Yes - stop the timer
			stop_timer3();

			// Return error
			return(ERROR);
		}

		// Read the status
		status = *((volatile int *)IOASIC_SOUND_STATUS);
	}

	// Stop the timer
	stop_timer3();

	// if the ready bit has gone high, then data is ready
	// make sure it's the 0x0A

	// Get the status
	status = *((volatile int *)IOASIC_SOUND_DATA_IN);

	// Monitor ready ?
	if((status & SND_MASK16) != SND_MONITOR_READY)
	{
		// NOPE - return error
		return(ERROR);
	}

	// dummy write-back
	*((volatile int *)IOASIC_SOUND_DATA_IN) = 0;

	// send the actual command
	*((volatile int *)IOASIC_SOUND_DATA_OUT) = command;

	// Return sucess
	return(OK);
}


int snd_reset_ack(void)
{
	int	ack;

	// Reset the sound system
	snd_reset();

	// Status OK
	if(snd_get_data(&ack) != OK)
	{
		return(ERROR);
	}

	// Check ack code
	if(ack != SND_MONITOR_READY)
	{
		return(ERROR);
	}
	
	// Return sucess
	return(OK);
}



static int snd_download_bank(sound_data_buffer_t *sdb)
{
	unsigned int	buffer[2];
	unsigned int	high_word;
	unsigned int	low_word;
	unsigned int	i;
	unsigned int	j;
	unsigned int	k;
	unsigned int	checksum;
	unsigned int	*bank_data;

	// Get int pointer to bank data
	bank_data = (unsigned int *)sdb->buffer;

	// Send the load DRAM command
	if(snd_send_data(SND_CMD_LOAD_DRAM) != OK)
	{
		// ERROR
		return(ERROR);
	}

	// send the starting load address
	high_word = (sdb->sound_saddr >> 16) & SND_MASK16;
	low_word = sdb->sound_saddr & SND_MASK16;
	if(snd_send_data(high_word) != OK)
	{
		return(ERROR);
	}
	if(snd_send_data(low_word) != OK)
	{
		return(ERROR);
	}

	// send the ending load address
	high_word = (sdb->sound_eaddr >> 16) & SND_MASK16;
	low_word = sdb->sound_eaddr & SND_MASK16;
	if(snd_send_data(high_word) != OK)
   {
		return(ERROR);
	}
	if(snd_send_data(low_word) != OK)
	{
		return(ERROR);
	}

	// now send the data
	k = 0;
	i = 0;

	// checksum is a straight 16-bit sum
	checksum = 0;

	// Generate first 2 words to send
	buffer[0] = ((bank_data[i] & SND_BYTE3) >> 24) | ((bank_data[i] & SND_BYTE2) >> 8);
	buffer[1] = ((bank_data[i+1] & SND_BYTE0) << 8) | ((bank_data[i+1] & SND_BYTE1) >> 8);

	// Increment source buffer pointer
	i++;

	// Send all of the 16 bit words
	for(j = 0; j < sdb->size; j++)
	{
		// Send a word
		if(snd_send_data(buffer[k]) != OK)
		{
			return(ERROR);
		}

		// Accumlate the checksum
		checksum += buffer[k];

		// Next 16 bit word
		k++;

		// Have we sent 2 words ?
		if(k == 2)
		{
			// YES - reset word counter
			k = 0;

			// Generate 2 more words
			buffer[0] = ((bank_data[i] & SND_BYTE3) >> 24) | ((bank_data[i] & SND_BYTE2) >> 8);
			buffer[1] = ((bank_data[i+1] & SND_BYTE0) << 8) | ((bank_data[i+1] & SND_BYTE1) >> 8);

			// Increment source buffer pointer
			i++;
		}
	}

	// Get the checksum calculated by the sound subsystem
	snd_get_data(&sdb->checksum);

	// Checksums match ?
	if((checksum & SND_MASK16) != sdb->checksum)
	{
		// NOPE - error
		return(ERROR);      
	}

	// Return success
   return(OK);
}
#else
//**************************************************************************/
//                                                                         */
// FUNCTION: snd_reset()                                                   */
//                                                                         */
//**************************************************************************/
void snd_reset(void)
{
	// Clear bit 0
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~0x1;

	// Short delay
	snd_delay(2);

	// Set bit 0
	*((volatile short *)IOASIC_SOUND_CONTROL) |= 0x1;

	// Short delay
	snd_delay(2);

	// Clear bit 15
	*((volatile short *)IOASIC_CONTROL) &= ~0x8000;

	// Turn off I/O ASIC LED
	*((volatile short *)IOASIC_CONTROL) |= 0x4000;
}


//**************************************************************************/
//                                                                         */
// FUNCTION: snd_reset_ack()                                               */
//                                                                         */
//**************************************************************************/
int snd_reset_ack (void)
{
	int status;                // whether or not timed out */
	unsigned int snd_ack;      // ack returned by sound boot ROM */

	// Reset sound system
	snd_reset();

	// look for the ack
	status = snd_get_data(&snd_ack);

	// Status OK ?
	if(status != OK)
	{
		// NOPE - Return ERROR
		return ERROR;
	}

	// ACK code correct ?
	if(snd_ack != SND_MONITOR_READY)
	{
		// NOPE - Return ERROR
		return ERROR;
	}

	// Return OK
	return OK;
}


//**************************************************************************/
//                                                                         */
// FUNCTION: snd_get_data()                                                */
//                                                                         */
//**************************************************************************/
int snd_get_data(unsigned int *data)
{
	unsigned int	start_time = watchdog_count;
	unsigned int	time;

	while(!(*((volatile short *)IOASIC_SOUND_STATUS) & STH_DATA_READY))
	{
		time = watchdog_count - start_time;
		if(time > SND_TIMEOUT)
		{
			return ERROR;
		}
	}

	// read the data
	*data = (int)*((volatile unsigned short *)IOASIC_SOUND_DATA_IN);

	// In 'C31 mode a read from the sound DSP
	// must be followed by a write.
	// This clears the data ready bit.
	*((volatile short *)IOASIC_SOUND_DATA_IN) = 0;

	return OK;
}



//**************************************************************************/
//                                                                         */
// FUNCTION: snd_get_debug()                                               */
//                                                                         */
//**************************************************************************/
int snd_get_debug (unsigned int *data)
{
	return(snd_get_data(data));
}


//**************************************************************************/
//                                                                         */
// FUNCTION: snd_send_data()                                               */
//                                                                         */
//**************************************************************************/
int snd_send_data (unsigned int data)
{
	unsigned int	start_time = watchdog_count;
	unsigned int   time;

	while(!(*((volatile short *)IOASIC_SOUND_STATUS) & HTS_DATA_EMPTY))
	{
		time = watchdog_count - start_time;
		if(time > SND_TIMEOUT)
		{
			return ERROR;
		}
	}

	// write the data
	*((volatile short *)IOASIC_SOUND_DATA_OUT) = (short)data;

	return OK;
}



//**************************************************************************/
//                                                                         */
// FUNCTION: snd_send_data_realtime()                                      */
//                                                                         */
//**************************************************************************/
int snd_send_data_realtime (unsigned int data)
{
	unsigned int	start_time = watchdog_count;
	unsigned int   time;

	while((*((volatile short *)IOASIC_SOUND_STATUS) & HTS_DATA_REALTIME) != HTS_DATA_REALTIME)
	{
		time = watchdog_count - start_time;
		if(time > SND_TIMEOUT)
		{
			return ERROR;
		}
	}

	// write the data */
	*((volatile short *)IOASIC_SOUND_DATA_OUT) = (short)data;

	return OK;
}



//**************************************************************************/
//                                                                         */
// FUNCTION: snd_dump_port()                                               */
//                                                                         */
//**************************************************************************/
void snd_dump_port (void)
{
	while(1)
	{
		printf("snd_dump_port(): data:%04X, status:%04X\n",
			*((volatile short *)IOASIC_SOUND_DATA_OUT),
			*((volatile short *)IOASIC_SOUND_STATUS));
	}
}



//**************************************************************************/
//                                                                         */
// FUNCTION: snd_send_command()                                            */
//                                                                         */
//**************************************************************************/
int snd_send_command(unsigned int command)
{
	unsigned int   start_time = watchdog_count;
	unsigned int   time;

	while(!(*((volatile short *)IOASIC_SOUND_STATUS) & STH_DATA_READY))
	{
		time = watchdog_count - start_time;
		if(time > SND_TIMEOUT)
		{
			return ERROR;
		}
	}

	if(*((volatile short *)IOASIC_SOUND_DATA_IN) != SND_MONITOR_READY)
	{
		return ERROR;
	}

	// dummy write-back
	*((volatile short *)IOASIC_SOUND_DATA_IN) = 0;

	// send the actual command
	*((volatile short *)IOASIC_SOUND_DATA_OUT) = command;

	return OK;
}


//**************************************************************************/
//                                                                         */
// FUNCTION: snd_delay()                                                   */
//                                                                         */
//**************************************************************************/
void snd_delay(int delay_time)
{
	unsigned long long	dtime = (unsigned long long)delay_time * 1000000L;
	unsigned long long   start_time = get_timer_val();

	while((get_timer_val() - start_time) < dtime) ;
}



//**************************************************************************/
//                                                                         */
// FUNCTION: snd_clear_latch()                                             */
//                                                                         */
//**************************************************************************/
void snd_clear_latch(void)
{
	(void)*((volatile short *)IOASIC_SOUND_DATA_IN);
	(void)*((volatile short *)IOASIC_SOUND_DATA_IN);
	(void)*((volatile short *)IOASIC_SOUND_DATA_IN);
	(void)*((volatile short *)IOASIC_SOUND_DATA_IN);
	(void)*((volatile short *)IOASIC_SOUND_DATA_IN);

	// In 'C31 mode a read from the sound DSP */
	// must be followed by a write. */           
	// This clears the data ready bit. */                        
	*((volatile short *)IOASIC_SOUND_DATA_IN) = 0;
}

short get_snd_status(void)
{
	return(*((volatile unsigned short *)IOASIC_SOUND_STATUS));
}

short get_snd_data(void)
{
	return(*((volatile unsigned short *)IOASIC_SOUND_DATA_IN));
}

void set_snd_data(short val)
{
	*((volatile short *)IOASIC_SOUND_DATA_IN) = 0;
}
#endif


int sound_ioctl(register struct iocntb *io, int cmd, int arg)
{
	sound_data_buffer_t	*sdb;

	// Has the sound system been initialized ?
	if(!sound_initialized)
	{
		// NOPE - return error
		return(-1);
	}

#if (!(PHOENIX_SYS & VEGAS))
	switch(cmd)
	{
		case FIOCSNDRESET:
		{
			snd_reset();
			break;
		}
		case FIOCSNDRESETACK:
		{
			if(snd_reset_ack())
			{
				return(-1);
			}
			break;
		}
		case FIOCSNDGETDATA:
		{
			if(snd_get_data((unsigned int *)arg))
			{
				return(-1);
			}
			break;
		}
		case FIOCSNDSENDDATA:
		{
			if(snd_send_data(arg))
			{
				return(-1);
			}
			break;
		}
		case FIOCSNDSENDCMD:
		{
			if(snd_send_command(arg))
			{
				return(-1);
			}
			break;
		}
		case FIOCSNDSENDPMDATA:
		{
			sdb = (sound_data_buffer_t *)arg;
			break;
		}
		case FIOCSNDSENDDRAMDATA:
		{
			sdb = (sound_data_buffer_t *)arg;
			if(snd_download_bank(sdb))
			{
				return(-1);
			}
			break;
		}
		case FIOCSNDDELAY:
		{
			snd_delay(arg);
			break;
		}
		default:
		{
			return(-1);
		}
	}
#endif
	// return sucess
	return(0);
}
