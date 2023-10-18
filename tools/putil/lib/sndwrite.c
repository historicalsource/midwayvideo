/*
 * SNDWRITE.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

static gen_scsi_cmd_t	c;
static char					command_buffer[16];


int psyq_snd_write(char *buffer, int length)
{
	__dpmi_regs		r;
	unsigned int	data_segment;
	unsigned int	data_selector;
	unsigned int	cmd_segment;
	unsigned int	cmd_selector;
	unsigned int	cbuf_segment;
	unsigned int	cbuf_selector;
	int				amount_2_write;
	int				error = 0;

	// Allocates 8k buffer for the data to be written
	if((data_segment = __dpmi_allocate_dos_memory((8192 + 15) >> 4, &data_selector)) == -1)
	{
		return(1);
	}

	// Allocate a buffer from DOS for the command structure
	if((cmd_segment = __dpmi_allocate_dos_memory((sizeof(gen_scsi_cmd_t) + 15) >> 4, &cmd_selector)) == -1)
	{
		__dpmi_free_dos_memory(data_selector);
		return(0);
	}

	// Allocate a buffer from DOS for the command information buffer
	if((cbuf_segment = __dpmi_allocate_dos_memory((16 + 15) >> 4, &cbuf_selector)) == -1)
	{
		__dpmi_free_dos_memory(data_selector);
		__dpmi_free_dos_memory(cmd_selector);
		return(0);
	}

	while(length)
	{
		amount_2_write = length;
		if(amount_2_write > 8192)
		{
			amount_2_write = 8192;
		}

		// Copy the data to the buffer
		movedata(_my_ds(), (unsigned int)buffer, data_selector, 0, amount_2_write);

		// Set up the command
		c.cmdlen = 6;
		c.cmd_segment = cbuf_segment;
		c.cmd_offset = 0;
		c.data_out_len = amount_2_write;
		c.data_out_segment = data_segment;
		c.data_out_offset = 0;
		c.data_in_len = 0;
		c.data_in_segment = 0;
		c.data_in_offset = 0;

		// Set up the command informaton	
		command_buffer[0] = SOUND_WRITE;
		command_buffer[1] = 0;

			// Copy the command
		movedata(_my_ds(), (unsigned int)&c, cmd_selector, 0, sizeof(gen_scsi_cmd_t));

		// Copy the command information
		movedata(_my_ds(), (unsigned int)command_buffer, cbuf_selector, 0, 16);

		// Set up the registers for the interrupt service call
		r.h.ah = GENERAL_SCSI_CMD;
		r.h.al = PHOENIX_UNIT_NUMBER;
		r.x.es = cmd_segment;
		r.x.si = 0;

		// No cntl-c's while doing this
		setcbrk(0);

		// Issue the interrupt
		__dpmi_int(PHOENIX_INT, &r);

		// Cntl-c's OK now
		setcbrk(1);

		// Error ?
		if(r.x.flags & 1)
		{
			// YES
			error = 1;
			break;
		}

		// Decrement amount length
		length -= amount_2_write;

		// Increment buffer pointer
		buffer += amount_2_write;
	}

	// Free allocated DOS buffers
	__dpmi_free_dos_memory(data_selector);
	__dpmi_free_dos_memory(cmd_selector);
	__dpmi_free_dos_memory(cbuf_selector);

	// return OK
	return(error);
}


