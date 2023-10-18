/*
 * GETVER.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"


int get_target_version(int	*major, int *minor)
{
	__dpmi_regs	r;

	// Set up the registers for the interrupt service call
	r.h.ah = GET_VERSION;
	r.h.al = 0;

	// No cntl-c's while doing this
	setcbrk(0);

	// Issue the interrupt
	__dpmi_int(PHOENIX_INT, &r);

	// Cntl-c's OK now
	setcbrk(1);

	// Error ?
	if(r.x.flags & 1)
	{
		// Return error
		return(1);
	}

	// Set the data
	*major = r.h.ah;
	*minor = r.h.al;

	// Return OK
	return(0);
}


