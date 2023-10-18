/*
 * GETPTBL.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	<stdio.h>
#include	"pd.h"

static gen_scsi_cmd_t	c;
static char					command_buffer[16];

int getptbl(partition_table_t *ptable)
{
	__dpmi_regs		r;
	unsigned int	ptbl_segment;
	unsigned int	ptbl_selector;
	unsigned int	cmd_segment;
	unsigned int	cmd_selector;
	unsigned int	cbuf_segment;
	unsigned int	cbuf_selector;

	// Allocate a buffer from DOS for the partition table
	if((ptbl_segment = __dpmi_allocate_dos_memory((512 + 15) >> 4, &ptbl_selector)) == -1)
	{
		return(0);
	}

	// Allocate a buffer from DOS for the command structure
	if((cmd_segment = __dpmi_allocate_dos_memory((sizeof(gen_scsi_cmd_t) + 15) >> 4, &cmd_selector)) == -1)
	{
		__dpmi_free_dos_memory(ptbl_selector);
		return(0);
	}

	// Allocate a buffer from DOS for the command information buffer
	if((cbuf_segment = __dpmi_allocate_dos_memory((16 + 15) >> 4, &cbuf_selector)) == -1)
	{
		__dpmi_free_dos_memory(ptbl_selector);
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
	c.data_in_len = 512;
	c.data_in_segment = ptbl_segment;
	c.data_in_offset = 0;

	// Set up the command informaton	
	command_buffer[0] = GET_PARTITION_TABLE;
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

	// Cntl-c's Ok now
	setcbrk(1);

	// Error ?
	if(!(r.x.flags & 1))
	{
		// NO - Move the data to the users buffer
		movedata(ptbl_selector, 0, _my_ds(), (unsigned int)ptable, 512);
	}

	// Free the memory
	__dpmi_free_dos_memory(ptbl_selector);
	__dpmi_free_dos_memory(cmd_selector);
	__dpmi_free_dos_memory(cbuf_selector);

	// Return success
	return(1);
}
