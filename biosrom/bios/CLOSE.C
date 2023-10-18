//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<io.h>

extern struct dev_sw_tab	*find_driver();
extern struct dev_init_tab	*find_device();
struct iocntb *get_icntb(void);
struct iocntb *xlate_fd(int fd);

extern struct iocntb			icb[];


/*
** close() - system close routine
**
**	entry:
**		pointer to an io control block
**	return:
**		returns a -1 if not currently open
**		or fd not valid - else returns the
**		value returned by the driver close routine
*/
int _close(int fd)
{
	register struct iocntb *io;
	int retval;

	io = xlate_fd(fd);
	if(io == NULL || io->icb_flags == 0) 
	{
		return(-1);
	}
	retval = (*io->icb_dt->d_close)(io);
	io->icb_flags = 0;
	return(retval);
}


