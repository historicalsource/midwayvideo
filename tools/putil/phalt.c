#include	<crt0.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<dir.h>
#include	<ctype.h>
#include	<unistd.h>
#include	<sys\stat.h>
#include	<fcntl.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	<pd.h>

void main(int argc, char *argv[])
{
	// Check to make sure the PSYQ driver is installed
	if(!check_driver())
	{
		exit(1);
	}

	psyq_halt();
}

