//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#include	<filesys.h>
#include	<io.h>
#include	<ioctl.h>

static int	file_initialized = 0;
struct iocntb *xlate_fd(int);

extern int	ide_drive_status;

int fileinit(void)
{
	if(!ide_drive_status)
	{
		return(0);
	}
	if(FSInit())
	{
		return(0);
	}
	file_initialized = 1;
	return(0);
}

int fileopen(register struct iocntb *io)
{
	if(!file_initialized)
	{
		return(-1);
	}

	// Read/Write
	if((io->icb_flags & (F_READ|F_WRITE)) == (F_READ|F_WRITE))
	{
		return(FSCreate(io->fh, io->name, 0, 0));
	}

	// Write only
	else if((io->icb_flags & (F_READ|F_WRITE)) == F_WRITE)
	{
		// Does the file exist ?
		if(!FSOpen(io->fh, io->name))
		{
			// YES
			FSClose(io->fh);

			// Delete it
			FSDelete(io->name);
		}

		// Create the file
		return(FSCreate(io->fh, io->name, 0, 0));
	}

	// Read only
	return(FSOpen(io->fh, io->name));
}

int fileclose(struct iocntb *io)
{
	return(FSClose(io->fh));
}

int fileread(register struct iocntb *io, char *buf, int count)
{
	int	amount_read;

	if(count & 3)
	{
		return(0);
	}
	amount_read = FSRead(io->fh, (unsigned long *)buf, count >> 2);
	if(amount_read >= 0)
	{
		return(amount_read);
	}
	return(ERR_FILE_EOF);
}

//void reinitialize_disk_cache(void);

int filewrite(register struct iocntb *io, char *buf, int count)
{
//reinitialize_disk_cache();
	if(count & 3)
	{
		return(0);
	}
	if(FSWrite(io->fh, (unsigned long *)buf, count>>2))
	{
		return(-1);
	}
	return(count);
}

int fileioctl(register struct iocntb *io, int cmd, int arg)
{
	switch(cmd)
	{
		case FIOCSETDATE:		// Set file time/date stamp (_dos_setftime)
		{
			break;
		}
		case FIOCGETDATE:		// Get file time/date stamp (_dos_getftime)
		{
			break;
		}
		case FIOCSETATTR:		// Set file attribute (_dos_setfileattr)
		{
			break;
		}
		case FIOCGETATTR:		// Get file attribute (_dos_getfileattr)
		{
			break;
		}
		case FIOCFLUSH:		// Flush the file buffers (_dos_commit)
		{
			return(FSFlush());
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}

int _rename(char *from, char *to)
{
	return(FSRename(from, to));
}

int _lseek(int fd, int offset, int whence)
{
	unsigned long	pos = 0;
	struct iocntb	*io;

	if(whence < 0 || whence > 2)
	{
		return(0);
	}
	io = xlate_fd(fd);
	if(!io)
	{
		return(0);
	}
	switch(whence)
	{
		case 0:			// SEEK_SET
		{
			FSSeek(io->fh, offset>>2, &pos);
			break;
		}
		case 1:			// SEEK_CUR
		{
			FSSeek(io->fh, -1, &pos);
			pos += (offset>>2);
			FSSeek(io->fh, pos, &pos);
			break;
		}
		case 2:			// SEEK_END
		{
			FSSize(io->fh, &pos);
			FSSeek(io->fh, pos, &pos);
			break;
		}
	}
	pos <<= 2;
	return((int)pos);
}
