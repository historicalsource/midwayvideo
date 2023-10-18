//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<filesys.h>

unsigned int _getdiskfree(unsigned int drive, diskfree_t *drivespace)
{
	if(drive != 0 || drive != 3)
	{
		return(1);
	}
	return(FSGetDiskFree(drivespace));
}
	
