/*
 * GETRADDR.C
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"


int psyq_get_regs_addr(void)
{
	__dpmi_regs				r;
	unsigned int			val;

	// Set up the registers for the interrupt service call
	r.h.ah = GET_REGISTERS_ADDR;
	r.h.al = 0;

	// Issue the interrupt
	__dpmi_int(PHOENIX_INT, &r);

	// Error ?
	if(r.x.flags & 1)
	{
		// YES
		return(0);
	}

	val = r.x.dx << 16;
	val |= r.x.ax;

	// return OK
	return(val);
}


