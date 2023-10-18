/*
** io.h -- Header file for Operating System i/o functions 
*/

#define NICB	16

#ifndef NULL
#define NULL 0
#endif

/*
** device switch table - dispatch table for monitor to invoke
**		io function to drivers - one entry per driver
*/
struct dev_sw_tab
{
	int	(*d_open)();	/* open routine */
	int	(*d_close)();	/* close routine */
	int	(*d_read)();	/* read doutine */
	int	(*d_write)();	/* write routinre */
	int	(*d_init)();	/* initialization routine */
	int	(*d_strategy)();	/* io startegy rouitne */
	int	(*d_ioctl)();		/* io strategy routine */
	char	*d_driver_name;		/* pointer to driver name */
};

/*
** device init table - one for each device in the system (ie.  tty0, tty1)
*/
struct dev_init_tab
{
	char	*dev_name;
	char	*dev_descrip;
	char	*dev_drv_name;
	int	dev_cntl;
	int	dev_unit;
	int	dev_part;
	int	dev_io_addr;
};

/*
** io control block - analogous to a file descriptor - contains
**		the necessary information so when io reguests
**		are made (ie read(iocntb,buf,cnt) ) addresses, flags,
**		pointer to device and driver tables will be known.
*/
struct iocntb
{
	char	*icb_addr;	/* user buffer address */
	int	icb_count;	/* count of char to transfer */
	int	icb_blkno;	/* random access block number */
	int	icb_errno;	/* return error number */
	int	icb_flags;	/* dev. type and status flags */
	struct dev_init_tab *icb_di; /* pointer to device init tab */
	struct dev_sw_tab *icb_dt; /* pointer to driver table */
	void	*fh;						// file handle
	char	*name;
};
		

/*
** circular buffer definition for character devices
** character device drivers should provide one buffer per channel
** and specify that the device should be scanned.
*/
#define	CBUFSIZE	256	

struct circ_buf
{
	int cb_flags;		/* character device flags */
	char *cb_in;		/* pts at next free char */
	char *cb_out;		/* pts at next filled char */
	char cb_buf[CBUFSIZE];	/* circular buffer for input */
};

/*
** circular buffer flag definitions
*/
#define	CB_RAW		0x1	/* don't interpret special chars */
#define	CB_STOPPED	0x2	/* stop output */

/*
** Simple circular buffer functions
*/
#define	CIRC_EMPTY(x)	((x)->cb_in == (x)->cb_out)
#define	CIRC_FLUSH(x)	((x)->cb_in = (x)->cb_out = (x)->cb_buf)
#define	CIRC_STOPPED(x)	((x)->cb_flags & CB_STOPPED)
#define CIRC_CLEAR(x)	((x)->cb_flags = 0)
/*
** io control block flags flags
*/
#define F_READ		0x0001		/* file opened for reading */
#define F_WRITE		0x0002		/* file opened for writing */
#define	F_NBLOCK	0x0004		/* non-blocking io */
#define	F_SCAN		0x0008		/* device should be scanned */
#define F_STRAT		0x0010		/* use strategy routine */
#define F_REMOTE	0x0020		/* set up to gen interrupt */
#define	F_FILE	0x0040		// Is a file

/*
** io access  flags
*/
#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		2
#define	O_APPEND	0010	/* append (writes guaranteed at the end) */
#define	O_CREAT		0400
#define	O_TRUNC		01000
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

/*
** Request codes
*/
#define	READ	1
#define	WRITE	2

