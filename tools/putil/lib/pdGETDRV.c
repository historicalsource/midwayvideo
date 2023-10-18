/*
 * PDGETDRV.C.
 */
#include <dos.h>
#include <dpmi.h>
#include	"pd.h"

extern int	sys;

void _pd_getdrive(unsigned int *p_drive)
{
	__dpmi_regs r;

	if(sys == PHOENIX_INT)
	{
		*p_drive = 0x100;
		return;
	}
	r.h.ah = 0x19;
	__dpmi_int(0x21, &r);
	*p_drive = (unsigned int)r.h.al + 1;
}
