/*
 * PDGETFTM.C.
 */
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

extern int	sys;

unsigned int _pd_getftime(int handle, unsigned int *p_date, unsigned int *p_time)
{
	__dpmi_regs r;

	if(p_date == 0 || p_time == 0)
	{
		errno = EINVAL;
		return -1;
	}

	r.x.ax = 0x5700;
	r.x.bx = (handle & ~32);
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno = EBADF;
		return r.x.ax;
	}
	*p_time = r.x.cx;
	*p_date = r.x.dx;
	return(0);
}
