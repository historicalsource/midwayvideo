//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<filesys.h>
#include	"find.h"

char	*strchr(char *, int);

static int pattern_match(struct find_t *ft)
{
	int	i;
	int	ii;
	char	name_2_check[12];
	char	*tmp;

	/* Clear name to all spaces */
	for(i = 0; i < 12; i++)
	{
		name_2_check[i] = ' ';
	}

	/* Is there an extension in the filename ? */
	if((tmp = strchr(fsd.name, '.')) != (char *)0)
	{
		/* Copy up to the . (dot) */
		for(i = 0; i < 8; i++)
		{
			if(fsd.name[i] == '.')
			{
				break;
			}
			name_2_check[i] = fsd.name[i];
		}

		ii = i + 1;

		/* Pad the basename with spaces if needed */
		while(i < 8)
		{
			name_2_check[i++] = ' ';
		}

		/* Add the . (dot) */
		name_2_check[8] = '.';

		/* Add the extension */
		for(i = 9; i < 12; i++)
		{
			if(!fsd.name[ii])
			{
				break;
			}
			name_2_check[i] = fsd.name[ii++];
		}

		/* Pad the extension with spaces if needed */
		while(i < 12)
		{
			name_2_check[i++] = ' ';
		}
	}

	/* No extension on file name - just copy it */
	else
	{
		for(i = 0; i < 12; i++)
		{
			if(!fsd.name[i])
			{
				break;
			}
			name_2_check[i] = fsd.name[i];
		}

		/* Pad out the rest of the file name if needed */
		while(i < 12)
		{
			if(i == 8)
			{
				name_2_check[i++] = '.';
			}
			else
			{
				name_2_check[i++] = ' ';
			}
		}
	}
		
	for(i = 0; i < 12; i++)
	{
		/* Do we care about this character ? */
		if(ft->mask & (1 << i))
		{
			/* Yes we care - do the characters match ? */
			if(name_2_check[i] != ft->pattern[i])
			{
				/* NOPE */
				break;
			}
		}
	}

	/* Did all of the characters match ? */
	if(i < 12)
	{
		/* NOPE - return no match */
		return(0);
	}

	/* Return match */
	return(1);
}


int _findnext(struct find_t *ft)
{
	int	i;
	struct ffblk	f;

	for(;;)
	{
		if(FSDirNext(&fsd))
		{
			return(-1);
		}

		/* Does the retrieved filename match the pattern ? */
		if (pattern_match(ft))
		{
			/* Copy the filename into the find_t structure */
			for(i = 0; i < 12; i++)
			{
				ft->file_name[i] = fsd.name[i];
				if(!fsd.name[i])
				{
					break;
				}
			}

			/* Make sure it's null terminated */
			ft->file_name[i] = 0;

			/* Set file size */
			FSGetFFblk(ft->file_name, &f);
			ft->size = f.ff_fsize;
			ft->attrib = f.ff_attrib;
			ft->wr_time = f.ff_ftime;
			ft->wr_date = f.ff_fdate;

			/* Return match found */
			return(0);
		}
	}
}
