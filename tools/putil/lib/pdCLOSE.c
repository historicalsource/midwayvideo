/*
 * PDCLOSE.C.
 */
#include <libc/dosio.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>

extern int	sys;

unsigned int _pd_close(int handle)
{
	__dpmi_regs r;

	r.h.ah = 0x3E;
	r.x.bx = (handle & ~32);
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno  = __doserr_to_errno(r.x.ax);
		return(r.x.ax);
	}
	return(0);
}
