/*
 * PDSETDRV.C.
 */

#include <dos.h>
#include <dpmi.h>
#include	"pd.h"

extern int	sys;

void _pd_setdrive(unsigned int drive, unsigned int *p_drives)
{
	__dpmi_regs r;

	if(drive & 0x100)
	{
		sys = PHOENIX_INT;
		*p_drives = 1;
		return;
	}
	else
	{
		sys = DOS_INT;
	}
	r.h.ah = 0x0E;
	r.h.dl = drive - 1;
	__dpmi_int(0x21, &r);
	*p_drives = (unsigned int)r.h.al;
}
