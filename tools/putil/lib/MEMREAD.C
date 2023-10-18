/*
 * MEMREAD.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

int psyq_mem_read(unsigned long address, unsigned long *data)
{
	__dpmi_regs				r;
	unsigned int			data_segment;
	unsigned int			data_selector;

	// Allocates 16 byte buffer for the data to be read
	if((data_segment = __dpmi_allocate_dos_memory((4 + 15) >> 4, &data_selector)) == -1)
	{
		return(0);
	}

	// Set up for memory read
	r.h.ah = READ_MEMORY;
	r.h.al = 0;
	r.x.cx = sizeof(unsigned long);
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

	// Errors ?
	if(r.x.flags & 1)
	{
		__dpmi_free_dos_memory(data_selector);
		return(1);
	}

	// Copy the data to the users pointer
	movedata(data_selector, 0, _my_ds(), (unsigned int)data, sizeof(unsigned long));

	// Free the selector
	__dpmi_free_dos_memory(data_selector);

	// Return OK
	return(0);
}



