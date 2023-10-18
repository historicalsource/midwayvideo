/*
 * PDFINDF.C.
 */
#include	<stdio.h>
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <errno.h>
#include <dpmi.h>
#include <string.h>
#include <dos.h>
#include	"pd.h"

static char	buf[128];
extern int	sys;

unsigned int _pd_findfirst(char *name, unsigned int attr, struct _find_t *result)
{
	__dpmi_regs r;
	char			*tmp;
	char			*t1;
	int			i;

	tmp = get_fname_base(name);
	_put_path(tmp);
	r.x.dx = (__tb & 15) + strlen(tmp) + 1;
	r.x.ds = __tb / 16;
	r.h.ah = 0x1A;
	__dpmi_int(0x21, &r);

	r.h.ah = 0x4E;
	r.x.dx = (__tb & 15);
	r.x.ds = __tb / 16;
	r.x.cx = attr;
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno = __doserr_to_errno(r.x.ax);
		return(r.x.ax);
	}
	dosmemget(__tb + strlen(tmp) + 1, sizeof(struct _find_t), result);

	// Was this targeted at the phoenix system
	if(sys == PHOENIX_INT)
	{
		// YES - Copy the path into a buffer
		t1 = name;
		i = 0;
		while(t1 < tmp)
		{
			buf[i++] = *t1++;
		}

		// Make sure it's null terminated
		buf[i] = 0;

		// Append the name returned
		strcat(buf, result->name);

		// Put the result into the name field
		sprintf(result->name, "%s", buf);
	}
	return(0);
}
