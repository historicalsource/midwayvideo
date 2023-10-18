/*
 * PDGETDAT.C.
 */
#include <dpmi.h>
#include <errno.h>
#include <dos.h>

void _pd_getdate(struct _dosdate_t *date)
{
	__dpmi_regs r;

	r.h.ah = 0x2A;
	__dpmi_int(0x21, &r);
	date->year      = (unsigned short)r.x.cx;
	date->month     = r.h.dh;
	date->day       = r.h.dl;
	date->dayofweek = r.h.al;
}
