/*
 * PDEXTERR.C.
 */
#include <dos.h>
#include <dpmi.h>

int _pdexterr(struct _DOSERROR *p_error)
{
	__dpmi_regs r;

	r.h.ah = 0x59;
	r.x.bx = 0;
	__dpmi_int(0x21, &r);
	p_error->exterror = (int)r.x.ax;
	p_error->class    = r.h.bh;
	p_error->action   = r.h.bl;
	p_error->locus    = r.h.ch;
	return((int)r.x.ax);
}
