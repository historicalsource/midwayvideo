/*
 * WRSECTORS.C
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

int write_sectors(int partition, const void *buffer, unsigned int lstart, unsigned int num)
{
	__dpmi_regs				r;
	unsigned int			dos_segment;
	unsigned int			dos_selector;
	unsigned int			cmd_segment;
	unsigned int			cmd_selector;
	unsigned int			cbuf_segment;
	unsigned int			cbuf_selector;
	const unsigned char	*p_buffer;
	int						num_2_write;
	int						num_written;

	// Allocates 8k buffer for the data to be written
	if((dos_segment = __dpmi_allocate_dos_memory((16384 + 15) >> 4, &dos_selector)) == -1)
	{
		return(0);
	}

	// Allocate a buffer from DOS for the command structure
	if((cmd_segment = __dpmi_allocate_dos_memory((sizeof(gen_scsi_cmd_t) + 15) >> 4, &cmd_selector)) == -1)
	{
		__dpmi_free_dos_memory(dos_selector);
		return(0);
	}

	// Allocate a buffer from DOS for the command buffer
	if((cbuf_segment = __dpmi_allocate_dos_memory((16 + 15) >> 4, &cbuf_selector)) == -1)
	{
		__dpmi_free_dos_memory(dos_selector);
		__dpmi_free_dos_memory(cmd_selector);
		return(0);
	}

	// Set a pointer to the users data
	p_buffer = (const unsigned char *)buffer;

	// Set count of number of sectors written
	num_written = 0;

	// Loop to write the number of sectors requested
	while(num)
	{
		// Figure out how many sectors to write for this pass
		num_2_write = num;

		// More than buffer size ?
		if(num_2_write > 32)
		{
			// Limit to buffer size
			num_2_write = 32;
		}

		// Set up the command
		c.cmdlen = 10;
		c.cmd_segment = cbuf_segment;
		c.cmd_offset = 0;
		c.data_out_len = num_2_write * 512;
		c.data_out_segment = dos_segment;
		c.data_out_offset = 0;
		c.data_in_len = 0;
		c.data_in_segment = 0;
		c.data_in_offset = 0;
	
		// Set up the command information
		command_buffer[0] = WRITE_SECTORS;
		command_buffer[1] = 0;
		command_buffer[2] = partition;
		command_buffer[3] = 0;
		command_buffer[4] = ((lstart >> 0) & 0xff);
		command_buffer[5] = ((lstart >> 8) & 0xff);
		command_buffer[6] = ((lstart >> 16) & 0xff);
		command_buffer[7] = ((lstart >> 24) & 0xff);
		command_buffer[8] = num_2_write;
		command_buffer[9] = 0;

		// Copy the data to the buffer
		movedata(_my_ds(), (unsigned int)p_buffer, dos_selector, 0, num_2_write * 512);

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
			return(num_written);
		}

		// Increment count of num written
		num_written += num_2_write;

		// Decrement count of num left to write
		num -= num_2_write;

		// Increment buffer pointer
		p_buffer += (num_2_write * 512);

		// Increment the sector
		lstart += num_2_write;
	}

	// Free allocated DOS buffers
	__dpmi_free_dos_memory(dos_selector);
	__dpmi_free_dos_memory(cmd_selector);
	__dpmi_free_dos_memory(cbuf_selector);
	return(num_written);
}

