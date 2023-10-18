/*
 * MEMWRITE.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

static unsigned long		wr_data;

int psyq_mem_write(unsigned long address, unsigned long data)
{
	__dpmi_regs				r;
	unsigned int			data_segment;
	unsigned int			data_selector;

	// Allocates 8k buffer for the data to be written
	if((data_segment = __dpmi_allocate_dos_memory((4 + 15) >> 4, &data_selector)) == -1)
	{
		return(0);
	}

	// Set the data
	wr_data = data;

	// Copy the data to the buffer
	movedata(_my_ds(), (unsigned int)&wr_data, data_selector, 0, 4);

	// Set up the registers for the interrupt service call
	r.h.ah = WRITE_MEMORY;
	r.h.al = 0;
	r.x.cx = sizeof(int);
	r.x.dx = (unsigned short)((address >> 16) & 0xffff);
	r.x.bx = (unsigned short)(address & 0xffff);
	r.x.es = data_segment;
	r.x.si = 0;

	// No cntl-c's while doing this
	setcbrk(0);

	// Issue the interrupt
	__dpmi_int(PHOENIX_INT, &r);

	// Cntl-c's OK now
	setcbrk(1);

	// Free allocated DOS buffers
	__dpmi_free_dos_memory(data_selector);

	// Error ?
	if(r.x.flags & 1)
	{
		// YES
		return(1);
	}

	// return OK
	return(0);
}


