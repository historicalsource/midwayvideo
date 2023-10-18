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
void scandevs(void);

extern struct iocntb			icb[];

/*
** read -- system read routine
**
**	entry:
**		pointer to a io control block
**		pointer to users buffer
**		count of bytes to transfer
**	returns
**		integer returned from driver routine
*/
int _read(int fd, char *buf, int cnt)
{
	register struct iocntb *io;
	int retval;

	io = xlate_fd(fd);
	if(io == NULL || io->icb_flags == 0)
	{
		return(-1);
	}
	scandevs();	/* polls all fds that have flag bit F_SCAN set */
	if(io->icb_flags & F_STRAT)
	{
		io->icb_addr = buf;
		io->icb_count = cnt;
		retval =(*io->icb_dt->d_strategy)(io, READ);
	}
	else
	{
		retval = (*io->icb_dt->d_read)(io,buf,cnt);
	}
	return(retval);
}


