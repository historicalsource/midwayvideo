/*
 * PDCREATN.C.
 */
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

extern int	sys;

unsigned int _pd_creatnew(const char *filename, unsigned int attr, int *handle)
{
	__dpmi_regs r;
	char			*tmp;

	tmp = get_fname_base(filename);
	_put_path(tmp);
	r.h.ah = 0x5B;
	r.x.cx = attr & 0xFFFF;
	r.x.dx = __tb & 15;
	r.x.ds = __tb / 16;
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno  = __doserr_to_errno(r.x.ax);
		*handle = (unsigned)-1;
		return(r.x.ax);
	}
	if(sys == PHOENIX_INT)
	{
		r.x.ax |= 32;
	}
	*handle = r.x.ax;
	return(0);
}
