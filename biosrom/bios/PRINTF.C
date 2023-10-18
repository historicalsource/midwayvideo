/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <libc/file.h>

int	doprnt(const char *_fmt, va_list _args);

int printf(const char *fmt, ...)
{
	int len;

	len = doprnt(fmt, (&fmt)+1);
	return(len);
}
