/*
 * PDREMOVE.C
 */
#include <libc/stubs.h>
#include <io.h>
#include <stdio.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include	<dos.h>
#include	"pd.h"
 
extern int	sys;

int pdremove(const char *fn)
{
	__dpmi_regs	r;
	unsigned		attr;
	int			directory_p;
	char			*tmp;
 
	tmp = get_fname_base(fn);
	_put_path(tmp);

	/* Get the file attribute byte.  */
	if(sys != PHOENIX_INT)
	{
		attr = _chmod(fn, 0);
		directory_p = attr & 0x10;
 
		/* Now, make the file writable.  We must reset Vol, Dir, Sys and Hidden bits 
			in addition to the Read-Only bit, or else 214301 will fail.  */
		_chmod(fn, 1, attr & 0xffe0);
	}
	else
	{
		directory_p = 0;
	}

	/* Now delete it.  Note, _chmod leave dir name in tranfer buffer. */
	if(directory_p)
	{
		r.h.ah = 0x3a;		/* DOS Remove Directory function */
	}
	else
	{
		r.h.ah = 0x41;		/* DOS Remove File function */
	}
	r.x.dx = __tb & 15;
	r.x.ds = __tb / 16;
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		/* We failed.  Leave the things as we've found them.  */
		int e = __doserr_to_errno(r.x.ax);
 
		if(sys != PHOENIX_INT)
		{
			_chmod(fn, 1, attr & 0xffe7);
		}
		errno = e;
		return -1;
	}
	return 0;
}
