/*
 * PDSETFTM.C.
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

#if (THE_PIECE_O_SHIT_PSYQ_BIOS_IS_BROKEN & 1)
static gen_scsi_cmd_t	c;
static char					command_buffer[16];
#endif

extern int	sys;

unsigned int _pd_setftime(int handle, unsigned int date, unsigned int time)
{
	__dpmi_regs r;

#if (THE_PIECE_O_SHIT_PSYQ_BIOS_IS_BROKEN & 1)
	unsigned int	cmd_segment;
	unsigned int	cmd_selector;
	unsigned int	cbuf_segment;
	unsigned int	cbuf_selector;

	if(sys == PHOENIX_INT)
	{
		// Allocate a buffer from DOS for the command structure
		if((cmd_segment = __dpmi_allocate_dos_memory((sizeof(gen_scsi_cmd_t) + 15) >> 4, &cmd_selector)) == -1)
		{
			return(0);
		}

		// Allocate a buffer from DOS for the command information buffer
		if((cbuf_segment = __dpmi_allocate_dos_memory((16 + 15) >> 4, &cbuf_selector)) == -1)
		{
			__dpmi_free_dos_memory(cmd_selector);
			return(0);
		}

		// Set up the command
		c.cmdlen = 10;
		c.cmd_segment = cbuf_segment;
		c.cmd_offset = 0;
		c.data_out_len = 0;
		c.data_out_segment = 0;
		c.data_out_offset = 0;
		c.data_in_len = 0;
		c.data_in_segment = 0;
		c.data_in_offset = 0;

		// Set up the command informaton	
		command_buffer[0] = SET_FILE_TIMESTAMP;
		command_buffer[1] = 0;
		command_buffer[2] = (handle >> 8);
		command_buffer[3] = handle & 0x1f;
		command_buffer[4] = date >> 8;
		command_buffer[5] = date & 0xff;
		command_buffer[6] = time >> 8;
		command_buffer[7] = time & 0xff;

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
			// Set the error code
			errno = EBADF;

			// Free the memory
			__dpmi_free_dos_memory(cmd_selector);
			__dpmi_free_dos_memory(cbuf_selector);

			// Return fail
			return(r.x.ax);
		}

		// Free the memory
		__dpmi_free_dos_memory(cmd_selector);
		__dpmi_free_dos_memory(cbuf_selector);

		// Return success
		return(0);
	}
#endif
	r.x.ax = 0x5701;
	r.x.bx = (handle & ~32);
	r.x.cx = time;
	r.x.dx = date;
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno = EBADF;
		return(r.x.ax);
	}
	return(0);
}
