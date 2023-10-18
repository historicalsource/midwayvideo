//
// _exit.c - Code to handle exit calls from application
//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<ioctl.h>

void _exit(int code)
{
	printf("EXIT Called\n");
	disable_interrupts();
	while(1) ;
}
