/*
 * RESET.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"


int psyq_reset(void)
{
	__dpmi_regs				r;

	// Set up the registers for the interrupt service call
	r.h.ah = RESET_TARGET;
	r.h.al = 0;

	// Issue the interrupt
	__dpmi_int(PHOENIX_INT, &r);

	// Error ?
	if(r.x.flags & 1)
	{
		// YES
		return(1);
	}

	// return OK
	return(0);
}


