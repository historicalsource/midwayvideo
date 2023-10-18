//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<filesys.h>
#include	<io.h>

int	_open(const char *, int);

unsigned int _getfileattr(const char *filename, unsigned int *p_attr)
{
	int	h;

	h = _open(filename, O_RDONLY);
	if(h >= 0)
	{
		_close(h);
		*p_attr = 0;
		return(0);
	}
	return(1);
}
