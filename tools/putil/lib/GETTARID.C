/*
 * GETTARID.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"


int get_target_id(char *processor, char *platform)
{
	__dpmi_regs				r;
	unsigned int			data_segment;
	unsigned int			data_selector;

	// Allocates 8k buffer for the data to be read back
	if((data_segment = __dpmi_allocate_dos_memory((24 + 15) >> 4, &data_selector)) == -1)
	{
		return(0);
	}

	// Set up the registers for the interrupt service call
	r.h.ah = GET_ID;
	r.h.al = 0;
	r.x.es = data_segment;
	r.x.si = 0;

	// No cntl-c's while doing this
	setcbrk(0);

	// Issue the interrupt
	__dpmi_int(PHOENIX_INT, &r);

	// Cntl-c's Ok now
	setcbrk(1);

	if(r.x.flags & 1)
	{
		// Free allocated DOS buffers
		__dpmi_free_dos_memory(data_selector);

		// Make zero length strings for user
		processor[0] = 0;
		platform[0] = 0;

		// Return error
		return(1);
	}

	// Copy the processor information to the users buffer
	movedata(data_selector, 0, _my_ds(), (unsigned int)processor, 12);

	// Copy the platform information to the users buffer
	movedata(data_selector, 12, _my_ds(), (unsigned int)platform, 12);

	// Make sure the users processor and platform buffers are NULL terminated
	processor[11] = 0;
	platform[11] = 0;

	// Free allocated DOS buffers
	__dpmi_free_dos_memory(data_selector);

	return(0);
}
