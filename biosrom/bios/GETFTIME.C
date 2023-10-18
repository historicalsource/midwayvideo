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

unsigned int _getftime(int handle, unsigned int *p_date, unsigned int *p_time)
{
	register struct iocntb	*io;
	unsigned short				date;
	unsigned short				time;

	io = xlate_fd(handle);
	if(io == NULL || io->icb_flags == 0)
	{
		return(1);
	}
	if(!FSGetDateTime(io->fh, &date, &time))
	{
		*p_date = (int)date;
		*p_time = (int)time;
		return(0);
	}
	return(1);
}
