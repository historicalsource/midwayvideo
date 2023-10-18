/*
 * PDGETDF.C.
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dos.h>
#include <dpmi.h>
#include <errno.h>
#include	<unistd.h>
#include	"pd.h"

#if (THE_PIECE_O_SHIT_PSYQ_BIOS_IS_BROKEN & 1)
static gen_scsi_cmd_t	c;
static char					command_buffer[16];
struct _diskfree_t		df;
#endif

extern int	sys;

unsigned int _pd_getdiskfree(unsigned int drive, struct _diskfree_t *diskspace)
{
	__dpmi_regs r;

#if (THE_PIECE_O_SHIT_PSYQ_BIOS_IS_BROKEN & 1)
	unsigned int	df_segment;
	unsigned int	df_selector;
	unsigned int	cmd_segment;
	unsigned int	cmd_selector;
	unsigned int	cbuf_segment;
	unsigned int	cbuf_selector;

	if(drive >= 0x100)
	{
		// Allocate a buffer from DOS for data to be returned
		if((df_segment = __dpmi_allocate_dos_memory((sizeof(struct _diskfree_t) + 15) >> 4, &df_selector)) == -1)
		{
			return(0);
		}

		// Allocate a buffer from DOS for the command structure
		if((cmd_segment = __dpmi_allocate_dos_memory((sizeof(gen_scsi_cmd_t) + 15) >> 4, &cmd_selector)) == -1)
		{
			__dpmi_free_dos_memory(df_selector);
			return(0);
		}

		// Allocate a buffer from DOS for the command information buffer
		if((cbuf_segment = __dpmi_allocate_dos_memory((16 + 15) >> 4, &cbuf_selector)) == -1)
		{
			__dpmi_free_dos_memory(df_selector);
			__dpmi_free_dos_memory(cmd_selector);
			return(0);
		}

		// Set up the command
		c.cmdlen = 6;
		c.cmd_segment = cbuf_segment;
		c.cmd_offset = 0;
		c.data_out_len = 0;
		c.data_out_segment = 0;
		c.data_out_offset = 0;
		c.data_in_len = 0;
		c.data_in_segment = 0;
		c.data_in_offset = 0;

		// Set up the command informaton	
		command_buffer[0] = GET_DISK_FREE;
		command_buffer[1] = 0;
		command_buffer[2] = (drive & 0xff);

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

#if 0
		// Error ?
		if(r.x.flags & 1)
		{
			// Set the error code
			errno = EINVAL;

			// Free the memory
			__dpmi_free_dos_memory(df_selector);
			__dpmi_free_dos_memory(cmd_selector);
			__dpmi_free_dos_memory(cbuf_selector);

			// Return fail
			return(1);
		}
#endif

		// Set up the command
		c.cmdlen = 10;
		c.cmd_segment = cbuf_segment;
		c.cmd_offset = 0;
		c.data_out_len = 0;
		c.data_out_segment = 0;
		c.data_out_offset = 0;
		c.data_in_len = sizeof(struct _diskfree_t);
		c.data_in_segment = df_segment;
		c.data_in_offset = 0;

		// Set up the command informaton	
		command_buffer[0] = 0xff;
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

		// Issue the interrupt
		__dpmi_int(PHOENIX_INT, &r);

		// Copy the data to the user buffer
		movedata(df_selector, 0, _my_ds(), (unsigned int)diskspace, sizeof(struct _diskfree_t));

		// Free the memory
		__dpmi_free_dos_memory(df_selector);
		__dpmi_free_dos_memory(cmd_selector);
		__dpmi_free_dos_memory(cbuf_selector);

		// Return success
		return(0);
	}
#endif
	r.h.ah = 0x36;
	r.h.dl = (drive & 0xff);
	__dpmi_int(DOS_INT, &r);
	if(r.x.ax == 0xFFFF)
	{
		diskspace->sectors_per_cluster =
		diskspace->avail_clusters      =
		diskspace->bytes_per_sector    =
		diskspace->total_clusters      = 0;
		errno = EINVAL;
		return 1;
	}
	diskspace->sectors_per_cluster = r.x.ax;
	diskspace->avail_clusters      = r.x.bx;
	diskspace->bytes_per_sector    = r.x.cx;
	diskspace->total_clusters      = r.x.dx;
	return(0);
}
