/*
 * PDGETTIM.C.
 */
#include <dpmi.h>
#include <dos.h>

void _pd_gettime(struct _dostime_t *time)
{
	__dpmi_regs r;

	r.h.ah = 0x2C;
	__dpmi_int(0x21, &r);
	time->hour    = r.h.ch;
	time->minute  = r.h.cl;
	time->second  = r.h.dh;
	time->hsecond = r.h.dl;
}
