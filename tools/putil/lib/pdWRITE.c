/*
 * PDWITE.C.
 */
#include <libc/stubs.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

extern int	sys;

unsigned int _pd_write(int handle, const void *buffer, unsigned int count, unsigned int *result)
{
	__dpmi_regs r;
	unsigned int dos_segment, dos_selector, dos_buffer_size, write_size;
	const unsigned char *p_buffer;

	/* Allocates ~64K or less transfer buffer from DOS */
	dos_buffer_size = ( count < 0xFFE0 ) ? count : 0xFFE0;
	if((dos_segment=__dpmi_allocate_dos_memory((dos_buffer_size + 15) >> 4, &dos_selector)) == -1 )
	{
		errno = ENOMEM;
		return(8);
	}

	/* Reads blocks from user buffer and writes to file. */
	p_buffer = (const unsigned char *)buffer;
	*result  = 0;
	while(count)
	{
		write_size = ( count < dos_buffer_size ) ? count : dos_buffer_size;
		movedata(_my_ds(), (unsigned int)p_buffer, dos_selector, 0, write_size);
		r.h.ah = 0x40;
		r.x.bx = (handle & ~32);
		r.x.cx = write_size;
		r.x.ds = dos_segment;
		r.x.dx = 0;
		__dpmi_int(sys, &r);
		if(r.x.flags & 1)
		{
			__dpmi_free_dos_memory(dos_selector);
			errno = __doserr_to_errno(r.x.ax);
			return(r.x.ax);
		}
		count    -= write_size;
		p_buffer += write_size;
		*result  += r.x.ax;
	}

	/* Frees allocated DOS transfer buffer. */
	__dpmi_free_dos_memory(dos_selector);
	return(0);
}
