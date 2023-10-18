//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<io.h>
#include	<ioctl.h>

extern struct dev_sw_tab	*find_driver();
extern struct dev_init_tab	*find_device();
struct iocntb *get_icntb(void);
struct iocntb *xlate_fd(int fd);

extern struct iocntb			icb[];


/*
** ioctl - invoke a drivers ioctl routine and/or also sets some control
**		flags based on cmd
**
**	entry:
**		pointer to an io control block
**		integer command
**		integer argument
**
**	return:
**		
*/
int _ioctl(int fd, int cmd, int arg)
{
	register struct iocntb *io;
	int retval;

	io = xlate_fd(fd);
	if(io == NULL || io->icb_flags == 0) 
	{
		return(-1);
	}
	switch(cmd)
	{
		case FIOCNBLOCK:
		{
			if(arg)
			{
				io->icb_flags |= F_NBLOCK;
			}
			else
			{
				io->icb_flags &= ~F_NBLOCK;
			}
			return(0);
		}
		case FIOCINTBRK:
		{
			io->icb_flags |= F_REMOTE;
			break;
		}
		case FIOCINTBRKNOT:
		{
			io->icb_flags &= ~F_REMOTE;
			break;
		}
	}
	retval = (*io->icb_dt->d_ioctl)(io, cmd, arg);
	return(retval);
}


