//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<filesys.h>
#define	__FFIRST_C__
#include	"find.h"

/* Find first/next stuff */

char *strchr(char *, int);

static void strcpy(char *s1, char *s2)
{
	while(*s2)
	{
		*s1++ = *s2++;
	}
	*s1 = 0;
}

static int strlen(char *s)
{
	int	count = 0;

	while(*s)
	{
		++count;
		++s;
	}
	return(count);
}

static void generate_pattern(char *org, struct find_t *ft)
{
	int	i;
	int	ii;
	char	*tmp;
	char	tfname[16];

	ft->mask = 0;
	tmp = org;
	i = 0;
	strcpy(tfname, org);
	tmp = strchr(tfname, '.');
	if(tmp)
	{
		*tmp = 0;
	}

	/* No "*" in base file name */
	if(!strchr(tfname, '*'))
	{
		i = 0;
		while(tfname[i])
		{
			ft->pattern[i] = tfname[i];
			if(tfname[i] != '?')
			{
				ft->mask |= (1 << i);
			}
			i++;
		}

		/* Pad the name out with spaces */
		while(i < 8)
		{
			ft->pattern[i] = ' ';
			ft->mask |= (1 << i);
			i++;
		}
	}

	/* There is a "*" in the base name */
	else
	{
		ii = 0;
		for(i = 0; i < 8; i++)
		{
			if(tfname[ii] != '?' && tfname[ii] != '*')
			{
				ft->pattern[i] = tfname[ii];
				ft->mask |= (1<<i);
			}
			else if(tfname[ii] == '*')
			{
				i += (8 - strlen(tfname));
			}
			ii++;
		}
	}
					
	strcpy(tfname, org);
	tmp = strchr(tfname, '.');

	/* We only care about the extension if it is specfied */
	if(tmp)
	{
		ft->pattern[8] = '.';
		ft->mask |= (1 << 8);

		/* Step past the extension . (dot) */
		++tmp;

		/* No "*" in extension */
		if(!strchr(tmp, '*'))
		{
			i = 9;
			while(tmp[i-9])
			{
				ft->pattern[i] = tmp[i-9];
				if(tmp[i-9] != '?')
				{
					ft->mask |= (1 << i);
				}
				i++;
			}

			/* Pad the extension out with spaces */
			while(i < 12)
			{
				ft->pattern[i] = ' ';
				ft->mask |= (1 << i);
				i++;
			}
		}
	
		/* There is a "*" in extension */
		else
		{
			ii = 0;
			for(i = 9; i < 12; i++)
			{
				if(tmp[ii] != '?' && tmp[ii] != '*')
				{
					ft->pattern[i] = tmp[ii];
					ft->mask |= (1<<i);
				}
				else if(tmp[ii] == '*')
				{
					i += (3 - strlen(tmp));
				}
				ii++;
			}
		}
	}

	/* Convert it to upper case */
	for(i = 0; i < 12; i++)
	{
		if(ft->pattern[i] >= 'a' && ft->pattern[i] <= 'z')
		{
			ft->pattern[i] &= ~0x20;
		}
	}
}

int _findfirst(char *name, int attrib, struct find_t *ft)
{
	// Generate the filename pattern matching stuff
	generate_pattern(name, ft);

	// Begin file search at beginning of disk
	FSDirFirst(&fsd);

	// Get the first file name
	return(_findnext(ft));
}
