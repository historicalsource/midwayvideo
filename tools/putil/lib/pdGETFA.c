/*
 * PDGETFA.C.
 */
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

extern int	sys;

unsigned int _pd_getfileattr(const char *filename, unsigned int *p_attr)
{
	__dpmi_regs r;
	char			*tmp;

	if(filename == 0 || p_attr == 0)
	{
		errno = EINVAL;
		return -1;
	}

	tmp = get_fname_base(filename);
	_put_path(tmp);
	r.x.ax = 0x4300;
	r.x.dx = __tb & 15;
	r.x.ds = __tb / 16;
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno = __doserr_to_errno(r.x.ax);
		return(r.x.ax);
	}
	if(sys == PHOENIX_INT)
	{
		*p_attr = _A_NORMAL;
	}
	else
	{
		*p_attr = r.x.ax;
	}
	return(0);
}
