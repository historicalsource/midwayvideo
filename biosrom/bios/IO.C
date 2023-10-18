//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 7 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<filesys.h>
#include	<io.h>
#include	<ioctl.h>

void	init_drivers(void);
int	_close(int);
int	_open(char *, int);
int	_ioctl(int, int, int);


void scandevs(void);
void circ_putc(int c, register struct circ_buf *cb);

extern struct dev_sw_tab	*find_driver();
extern struct dev_init_tab	*find_device();

#if defined(TTY_DRIVER)
#define	CTRL(a)			(a&0x1f)
#endif

struct iocntb			icb[NICB];
static FSFile			fsf[NICB];

//int				open_file_count = 0;
int				open_fh[NICB];

/*
** xlate_fd - translate fd(file describtor) to icb pointer
*/ 
struct iocntb *xlate_fd(int fd)
{
	if(fd < 0 || fd >= NICB)
	{
		return(NULL);
	}
	return(&icb[fd]);
}

/*
** get_icntb() - get a free io control block - returns pointer to block
*/
struct iocntb *get_icntb(void)
{
	struct iocntb *io;

	for(io = icb; io < &icb[NICB]; io++)
	{
		if(io->icb_flags == 0)
		{
			return(io);
		}
	}
	return(NULL);
}

/*
**	Initialize the i/o system - cycles through the devices and 
**	invokes each device initailization function. Also opens
**	up stdin and stdout.
*/
int init_io(void)
{
	int i;

	// Initialize the IO Control blocks
	for(i = 0; i < NICB; i++)
	{
		icb[i].icb_flags = 0;
		icb[i].fh = &fsf[i];
		open_fh[i] = -1;
	}

	// Initialize all of the drivers
	init_drivers();

	// NOTE - The order in which these devices is opened is important
	// If the order changes, or additional devices are opened here closeall()
	// in this file will need to be modified and the std*.c files in the
	// \video\lib\libc\ansi\stdio will need to be modified.  Also sound.c in
	// in \video\lib\sound and cmos.c in \video\lib\goose will need modifying.
	// Best bet is - Don't fuck with this unless you want to change alot of
	// files!!
#if defined(TTY_DRIVER)
	// Open the standard input device
	_open("con:", O_RDONLY);			// stdin device

	// Open the standard output device
	_open("con:", O_WRONLY);			// Stdout device

	// Open the standard error device
	_open("con:", O_WRONLY);			// stderr device
#endif
	// Open the standard cmos device
	i = _open("cmos:", O_RDWR);

	// Open the standard asic device
	i = _open("ioasic:", O_RDWR);

	// Open the standard timer device
	i = _open("timer:", O_RDWR);

#if (PHOENIX_SYS & VEGAS)
	// Open the coin counter device
	i = _open("coin:", O_WRONLY);
#endif

	// Return sucess
	return(1);
}

static void set_fh_slot(int handle)
{
	int	i;

	for(i = 0; i < NICB; i++)
	{
		if(open_fh[i] < 0)
		{
			open_fh[i] = handle;
			return;
		}
	}
}


void closeall(void)
{
	int	i;

	for(i = 0; i < NICB; i++)
	{
#if (!(PHOENIX_SYS & VEGAS))
		if(open_fh[i] > 5)
#else
		if(open_fh[i] > 6)
#endif
		{
			_close(open_fh[i]);
			open_fh[i] = -1;
		}
	}
}

int isatty(int fd)
{
	struct iocntb	*io;
	struct dev_init_tab	*icb_di;

	io = xlate_fd(fd);
	if(io == NULL)
	{
		return(0);
	}
	icb_di = io->icb_di;
	if(icb_di->dev_name[0] == 'c' && icb_di->dev_name[1] == 'o' && \
		icb_di->dev_name[2] == 'n' && icb_di->dev_name[3] == ':' && \
		icb_di->dev_name[4] == 0)
	{
		return(1);
	}
	return(0);
}
		

int kbhit(void)
{
	int	status;

	if(_ioctl(0, TIOCKEYHIT, (int)&status) < 0)
	{
		return(0);
	}
	return(status);
}

/*
** scandevs -- scan all enabled open devices that have the F_SCAN
**		bit set in icb_flags.
*/
void scandevs(void)
{
	register struct iocntb *io;

	for(io = icb; io < &icb[NICB]; io++)
	{
		if(io->icb_flags & F_SCAN)
		{
			_ioctl(io - icb , FIOCSCAN, 0);
		}
	}
}

#if defined(TTY_DRIVER)
/*
** ttyinput -- called by char driver scandev ioctl routines to deal with
** circ_buf and special characters
*/
void ttyinput(register struct circ_buf *cb, register char c)
{
	if(!(cb->cb_flags & CB_RAW))
	{
		if((c & 0x7f) == CTRL('S'))
		{
			cb->cb_flags |= CB_STOPPED;
			return;
		}
		if((c & 0x7f) == CTRL('Q'))
		{
			cb->cb_flags &= ~CB_STOPPED;
			return;
		}
	}
	circ_putc(c, cb);
}
#endif

/*
** circ_getc -- remove character from circular buffer
*/
int circ_getc(register struct circ_buf *cb)
{
	int c;

	if(CIRC_EMPTY(cb))	/* should always check before calling */
	{
	}

	c = *cb->cb_out++ & 0xff;
	if(cb->cb_out >= &cb->cb_buf[sizeof(cb->cb_buf)])
	{
		cb->cb_out = cb->cb_buf;
	}
	return (c);
}

/*
** circ_putc -- insert character in circular buffer
*/
void circ_putc(int c, register struct circ_buf *cb)
{
	char *cp;

	cp = cb->cb_in + 1 >= &cb->cb_buf[sizeof(cb->cb_buf)] ? cb->cb_buf : cb->cb_in + 1;
	if(cp == cb->cb_out)
	{
		return;
	}
	*cb->cb_in = c;
	cb->cb_in = cp;
}

/*
** nodev -- device table stub for routines that should never
** be called
*/
int nodev(struct iocntb *io)
{
	return(-1);
}

/*
** nulldev -- device table routine for routines that are
** nop's
*/
int nulldev(void)
{
	return(0);
}
