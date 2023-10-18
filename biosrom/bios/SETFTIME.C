//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<filesys.h>
#include	<io.h>

struct iocntb *xlate_fd(int fd);

unsigned int _setftime(int handle, unsigned int date, unsigned int time)
{
	register struct iocntb	*io;

	io = xlate_fd(handle);
	if(io == NULL || io->icb_flags == 0)
	{
		return(1);
	}
	return(FSSetDateTime(io->fh, (unsigned short)date, (unsigned short)time));
}
