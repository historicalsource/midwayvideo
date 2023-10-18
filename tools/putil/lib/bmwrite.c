/*
 * BMWRITE.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

int psyq_blk_write(unsigned long address, char *buffer, int length)
{
	__dpmi_regs				r;
	unsigned int			data_segment;
	unsigned int			data_selector;
	int						amount_2_write;
	int						error = 0;

	// Allocates 8k buffer for the data to be written
	if((data_segment = __dpmi_allocate_dos_memory((8192 + 15) >> 4, &data_selector)) == -1)
	{
		return(1);
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

		// Set up the registers for the interrupt service call
		r.h.ah = WRITE_MEMORY;
		r.h.al = 0;
		r.x.cx = amount_2_write;
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

		// Error ?
		if(r.x.flags & 1)
		{
			// YES
			error = 1;
			break;
		}

		// Decrement amount length
		length -= amount_2_write;

		// Increment address
		address += amount_2_write;

		// Increment buffer pointer
		buffer += amount_2_write;
	}

	// Free allocated DOS buffers
	__dpmi_free_dos_memory(data_selector);

	// return OK
	return(error);
}


