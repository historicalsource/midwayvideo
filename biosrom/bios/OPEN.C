//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 3 $
//
// $Author: Mlynch $
//
#include	<io.h>

extern struct dev_sw_tab	*find_driver();
extern struct dev_init_tab	*find_device();
extern int						open_fh[];
struct iocntb *get_icntb(void);

extern struct iocntb			icb[];

/*
** open(device,flags) - open a device per the mode specified in flags
**		open flags supported are:
**			O_RDONLY - open for reading only
**			O_WRONLY - open for reading or writing
**			O_RDWR   - open for both reading and writing
**
*/
extern int	lval;

int _open(char *device, int flags)
{
	int errflg;
	struct iocntb *io;
	struct dev_sw_tab *dp;
	struct dev_init_tab *di;

	io = get_icntb();
	if(io == NULL)
	{
		return(-1);
	}
	if((di = find_device(device)) == NULL)
	{
		// Find the file driver device
		if((di = find_device("file:")) == NULL)
		{
			return(-1);
		}
		io->name = device;
	}
	if((dp = find_driver(di->dev_drv_name)) == NULL)
	{
		return(-1);
	}
	switch(flags & 0xf)
	{
		case O_RDONLY:
		{
			io->icb_flags = F_READ;
			break;
		}
		case O_WRONLY:
		{
			io->icb_flags = F_WRITE;
			break;
		}
		case O_RDWR:
		{
			io->icb_flags = F_READ | F_WRITE;
			break;
		}
	}
	io->icb_dt = dp;
	io->icb_di = di;
	errflg = (*dp->d_open)(io);
	if(errflg)
	{
		io->icb_flags = 0;	/* free up this icb */
		return(-1);
	}

	open_fh[io - icb] = (io - icb);
	
	return(io - icb);
}


