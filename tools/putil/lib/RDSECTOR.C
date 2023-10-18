/*
 * RDSECTORS.C
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

int read_sectors(int partition, const void *buffer, unsigned int lstart, unsigned int num)
{
	__dpmi_regs				r;
	unsigned int			dos_segment;
	unsigned int			dos_selector;
	unsigned int			cmd_segment;
	unsigned int			cmd_selector;
	unsigned int			cbuf_segment;
	unsigned int			cbuf_selector;
	const unsigned char	*p_buffer;
	int						num_2_read;
	int						num_read;

	// Allocates 8k buffer for data
	if((dos_segment = __dpmi_allocate_dos_memory((8192 + 15) >> 4, &dos_selector)) == -1)
	{
		return(0);
	}

	// Allocate a buffer from DOS for the command structure
	if((cmd_segment = __dpmi_allocate_dos_memory((sizeof(gen_scsi_cmd_t) + 15) >> 4, &cmd_selector)) == -1)
	{
		__dpmi_free_dos_memory(dos_selector);
		return(0);
	}

	// Allocate a buffer from DOS for the command information buffer
	if((cbuf_segment = __dpmi_allocate_dos_memory((16 + 15) >> 4, &cbuf_selector)) == -1)
	{
		__dpmi_free_dos_memory(dos_selector);
		__dpmi_free_dos_memory(cmd_selector);
		return(0);
	}

	// Set up a pointer to the users buffer
	p_buffer = (const unsigned char *)buffer;

	// Set count of number read
	num_read = 0;

	// Loop to read number of sectors requested
	while(num)
	{
		// Figure out how many sectors to read for this pass
		num_2_read = num;

		// More than size of buffer ?
		if(num_2_read > 16)
		{
			// YES - limit to buffer size
			num_2_read = 16;
		}

		// Set up the command
		c.cmdlen = 10;
		c.cmd_segment = cbuf_segment;
		c.cmd_offset = 0;
		c.data_out_len = 0;
		c.data_out_segment = 0;
		c.data_out_offset = 0;
		c.data_in_len = num_2_read * 512;
		c.data_in_segment = dos_segment;
		c.data_in_offset = 0;
	
		// Set up the command information
		command_buffer[0] = READ_SECTORS;
		command_buffer[1] = 0;
		command_buffer[2] = partition;
		command_buffer[3] = 0;
		command_buffer[4] = ((lstart >> 0) & 0xff);
		command_buffer[5] = ((lstart >> 8) & 0xff);
		command_buffer[6] = ((lstart >> 16) & 0xff);
		command_buffer[7] = ((lstart >> 24) & 0xff);
		command_buffer[8] = num_2_read;
		command_buffer[9] = 0;

		// Copy the command
		movedata(_my_ds(), (unsigned int)&c, cmd_selector, 0, sizeof(gen_scsi_cmd_t));

		// Copy the command information
		movedata(_my_ds(), (unsigned int)command_buffer, cbuf_selector, 0, 16);

		// Set up the registers for the interrupt service call
		r.h.ah = GENERAL_SCSI_CMD;
		r.h.al = PHOENIX_UNIT_NUMBER;
		r.x.es = cmd_segment;
		r.x.si = 0;

		// Issue the interrupt
		__dpmi_int(PHOENIX_INT, &r);

		// Error ?
		if(r.x.flags & 1)
		{
			// YES - Free up resources and return
			__dpmi_free_dos_memory(dos_selector);
			__dpmi_free_dos_memory(cmd_selector);
			__dpmi_free_dos_memory(cbuf_selector);

			// Return how many where read successfully
			return(num_read);
		}

		// Copy the data from DOS to the user buffer
		movedata(dos_selector, 0, _my_ds(), (unsigned int)p_buffer, num_2_read * 512);

		// Decrement count of number to read
		num -= num_2_read;

		// Increment user buffer pointer
		p_buffer += (num_2_read * 512);

		// Increment count of number read
		num_read += num_2_read;
	}

	// Free allocated DOS buffers
	__dpmi_free_dos_memory(dos_selector);
	__dpmi_free_dos_memory(cmd_selector);
	__dpmi_free_dos_memory(cbuf_selector);

	// Return count of number of sectors read
	return(num_read);
}


