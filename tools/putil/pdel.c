#include	<crt0.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<dir.h>
#include	<sys\stat.h>

#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	<pd.h>

static char	drive[MAXDRIVE];
static char	dir[MAXDIR];
static char	name[MAXFILE];
static char	ext[MAXEXT];
static char	*path_name;
static int	file_count;
static int	dir_count;
static int	total_size;
static int	dir_size;
static int	target_drive = 0;

// This function prevents crt0 from expanding wildcards
char **__crt0_glob_function(char *_arg)
{
	return((char **)0);
}


void main(int argc, char *argv[])
{
	int			got_fname = 0;
	struct		stat	s;
	int			is_dir = 0;
	int			i = argc;
	int			src_is_dos = 1;
	char			path_buf[128];
	struct stat	f_stat;
	char			*tmp;

	// Scan arguments for file names
	while(--argc)
	{
		// Set path name
		path_name = argv[argc];

		// Is this targeted at the phoenix system
		if(strstr(argv[argc], "PHX:") || strstr(argv[argc], "phx:"))
		{
			// Go to the end of the string
			while(*path_name)
			{
				++path_name;
			}

			// Back up to the last colon or backslash whichever comes first
			while(*path_name != ':' && *path_name != '\\')
			{
				--path_name;
			}

			// Go past the colon or backslash
			path_name++;

			// Is there a file name there ?
			if(!(*path_name))
			{
				// Nope - make it wild
				path_name = "*.*";
			}

			// Set the system to the target
			_pd_setsystem(FROM_PHOENIX);

			// Get file names and info
			do_dir();

			// Set the target drive flag
			target_drive = 1;

			// Done - go show files
			break;
		}
		else
		{
			// Get the stats for the filename
			stat(path_name, &f_stat);

			// Check to see if it is a directory
			if(S_ISDIR(f_stat.st_mode))
			{
				// YES - Copy it
				strcpy(path_buf, path_name);

				// Go to the end of the string
				tmp = path_buf;
				while(*tmp)
				{
					++tmp;
				}
				--tmp;
				if(!(*tmp == '\\'))
				{
					strcat(path_buf, "\\");
				}

				// Make it wild
				strcat(path_buf, "*.*");

				// Set pointer to new path
				path_name = path_buf;
			}

			// Set the system to DOS
			_pd_setsystem(FROM_DOS);

			// Get file names and info
			do_dir();
		}
	}

	// All done
	exit(0);
}
