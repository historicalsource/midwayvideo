//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>

int _open(const char *, int);

unsigned int _creat(const char *filename, unsigned short attr, int *handle)
{
	*handle = _open(filename, O_WRONLY);
	if(*handle >= 0)
	{
		return(0);
	}
	return(-1);
}
