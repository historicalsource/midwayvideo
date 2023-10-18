/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <dir.h>
#include <ctype.h>
#include <string.h>
#include	<dos.h>
#include	"pd.h"

static char *max_ptr(char *p1, char *p2)
{
	if(p1 > p2)
	{
		return(p1);
	}
	return(p2);
}

int _fnsplit(const char *path, char *drive, char *dir, char *name, char *ext)
{
	int				flags = 0;
	int				len;
	const char		*pp;
	const char		*pe;
	char				*tmp;
	unsigned int	attrib;

	if(drive)
	{
		*drive = '\0';
	}
	if(dir)
	{
		*dir = '\0';
	}
	if(name)
	{
		*name = '\0';
	}
	if(ext)
	{
		*ext = '\0';
	}

	pp = path;

	// First check to see if phoenix system
	if((strstr(pp, "phx:") == pp) || (strstr(pp, "PHX:") == pp))
	{
		flags |= DRIVE;
		if(drive)
		{
			tmp = drive;
			while(*pp != ':')
			{
				*tmp++ = *pp++;
			}
			*tmp++ = *pp++;
			*tmp = 0;
		}
	}

	// Otherwise check for DOS drive
	else if((isalpha(*pp) || strchr("[\\]^_`", *pp)) && (pp[1] == ':'))
	{
		flags |= DRIVE;
		if (drive)
		{
			strncpy(drive, pp, 2);
			drive[2] = '\0';
		}
		pp += 2;
	}

	// Check to see if path ends with a backslash
	tmp = strrchr(path, '\\');
	if(tmp && *(tmp+1) == 0)
	{
		flags |= DIRECTORY;
		strcpy(dir, pp);
		flags |= FILENAME;
		strcpy(name, "*");
		flags |= EXTENSION;
		strcpy(ext, ".*");
		flags |= WILDCARDS;
		return(flags);
	}

	// Check to see if path is a directory
	if(!_dos_getfileattr(path, &attrib))
	{
		if((attrib & _A_SUBDIR))
		{
			flags |= DIRECTORY;
			strcpy(dir, pp);
			strcat(dir, "\\");
			flags |= FILENAME;
			strcpy(name, "*");
			flags |= EXTENSION;
			strcpy(ext, ".*");
			flags |= WILDCARDS;
			return(flags);
		}
	}

	// Get the directory
	pe = max_ptr(strrchr(pp, '\\'), strrchr(pp, '/'));
	if(pe)
	{ 
		flags |= DIRECTORY;
		pe++;
		len = pe - pp;
		if (dir)
		{
			strncpy(dir, pp, len);
			dir[len] = '\0';
		}
		pp = pe;
	}
	else
	{
		pe = pp;
	}

	// Special case: "c:/path/." or "c:/path/.."
	// These mean FILENAME, not EXTENSION.
	while(*pp == '.')
	{
		++pp;
	}

	if(pp > pe)
	{
		flags |= FILENAME;
		if(name)
		{
			len = pp - pe;
			strncpy(name, pe, len);
			name[len] = '\0';
		}
	}

	pe = strrchr(pp, '.');
	if(pe)
	{
		flags |= EXTENSION;
		if(ext) 
		{
      strcpy(ext, pe);
		}
	}
	else 
	{
		pe = strchr(pp, '\0');
	}

	if(pp != pe)
	{
		flags |= FILENAME;
		len = pe - pp;
		if(name)
		{
			strncpy(name, pp, len);
			name[len] = '\0';
		}
	}

	if(strcspn(path, "*?[") < strlen(path))
	{
		flags |= WILDCARDS;
	}
	return(flags);
}
