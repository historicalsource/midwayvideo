//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<filesys.h>

unsigned int _commit(int handle)
{
	FSFlush();
	return(0);
}
