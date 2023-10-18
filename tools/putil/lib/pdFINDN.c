/*
 * PDFINDN.C.
 */
#include	<stdio.h>
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <errno.h>
#include <dpmi.h>
#include <dos.h>
#include	"pd.h"

char *	strcat(char *_s1, const char *_s2);

static char	buf[128];
extern int	sys;

unsigned int _pd_findnext(struct _find_t *result)
{
	__dpmi_regs r;
	char			*tmp;

	if(sys == PHOENIX_INT)
	{
		sprintf(buf, "%s", result->name);
		tmp = buf;
		while(*tmp)
		{
			++tmp;
		}
		while(*tmp != '\\' && *tmp != ':')
		{
			--tmp;
		}
		++tmp;
		strcpy(result->name, tmp);
		*tmp = 0;
	}
	r.x.dx = __tb & 15;
	r.x.ds = __tb / 16;
	r.h.ah = 0x1A;
	__dpmi_int(0x21, &r);
	dosmemput(result, sizeof(struct _find_t), __tb);
	r.h.ah = 0x4F;
	__dpmi_int(sys, &r);
	if(r.x.flags & 1)
	{
		errno = __doserr_to_errno(r.x.ax);
		return(r.x.ax);
	}
	dosmemget(__tb, sizeof(struct _find_t), result);

	// Was this targeted at the phoenix system
	if(sys == PHOENIX_INT)
	{
		strcat(buf, result->name);

		// Put the result into the name field
		sprintf(result->name, "%s", buf);
	}

	return(0);
}
