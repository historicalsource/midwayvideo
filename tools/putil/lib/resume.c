/*
 * RESUME.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"


int psyq_resume(void)
{
	__dpmi_regs				r;
	unsigned int			val;

	// Set up the registers for the interrupt service call
	r.h.ah = NOP_COMMAND;
	r.h.al = 0x80;

	// Issue the interrupt
	__dpmi_int(PHOENIX_INT, &r);

	// Error ?
	if(r.x.flags & 1)
	{
		// YES
		return(0);
	}

	// return OK
	return(val);
}


