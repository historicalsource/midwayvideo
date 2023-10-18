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
** write -- system write rourine
**
**	entry:
**		pointer to an io control block
**		pointer to the users buffer
**		count of bytes to transfer
**	return:
**		integer returned by driver routine
*/
int _write(int fd, char *buf, int cnt)
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
		retval =(*io->icb_dt->d_strategy)(io, WRITE);
	}
	else
	{
		retval = (*io->icb_dt->d_write)(io,buf,cnt);
	}
	return(retval);
}



