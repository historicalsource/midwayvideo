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

static int	paginate = 0;					// pagination defaulted off
static int	wide = 0;						// wide listing defaulted off
static int	recurse = 0;					// recurse subdirectories off
static int	summary = 1;					// summary info on
static int	lower_case = 0;				// lower case file names off
static int	sort = 0;						// sorting off
//static int	compression_ratios = 0;		// compression ratios off
static int	hidden_files = 0;				// hidden files off
static int	system_files = 0;				// system files off
static int	directories = 1;				// directories on
static int	archive_files = 1;			// archive files on
static int	read_only_files = 1;			// read only files on
//static int	name_sort = -1;				// alpha name sort (-1 disable, 0 alpha, 1 ralpha)
//static int	extension_sort = -1;			// extension sort (-1 disable, 0 alpha, 1 ralpha)
//static int	date_time_sort = -1;			// date time sort (-1 disable, 0 earliest, 1 latest)
//static int	size_sort = -1;				// size sort (-1 disable, 0 smallest, 1 largest)
//static int	directories_first = -1;		// directories (-1 disable, 0 dirs first, 1 dirs last)
//static int	compression_sort = -1;		// compression ratios (-1 disable, 0 lowest, highest)
static int	drive_clusters = 0;			// compression use FAT clusters

static char	drive[MAXDRIVE];
static char	dir[MAXDIR];
static char	name[MAXFILE];
static char	ext[MAXEXT];
static char	*path_name;
static int	file_count;
static int	dir_count;
static int	total_size;
static unsigned dir_size;
static char	f_spec[256];
static int	rdrv;
static int	drv;

typedef struct ffnode
{
	struct ffnode	*next;
	struct ffnode	*prev;
	struct _find_t	ffblk;
} ffnode_t;

static ffnode_t	*fflist = (ffnode_t *)0;

static void do_file_sort(void);
static void do_wide_list(void);
static void do_normal_list(void);
static void show_files(void);
static int add_file_node(struct _find_t *ffblk);
static void do_dir(void);
void			clean_up(void);

// This function prevents crt0 from expanding wildcards
char **__crt0_glob_function(char *_arg)
{
	return((char **)0);
}

int parse_argument(char *arg)
{
	char	drive[81];
	char	path[81];
	char	file[81];
	char	ext[81];
	int	status;
	char	*tmp;
	int	attrib;
	int	ldrv;

	// First split the argument into is individual components
	status = _fnsplit(arg, drive, path, file, ext);

	// If we got a drive specifier set the drive
	if(status & DRIVE)
	{
		if(strstr(drive, "phx:") || strstr(drive, "PHX:"))
		{
			drv = 0x100;
		}
		else
		{
			drv = toupper(drive[0]) - 0x40;
		}
		_pd_setdrive(drv, &ldrv);
	}

	// Are there wildcards in the argument
	if(status & WILDCARDS)
	{
		sprintf(f_spec, "%s%s%s%s", drive, path, file, ext);
		return(1);
	}

	// Check to see if no path, file, and ext
	if(!(status & (DIRECTORY|FILENAME|EXTENSION|WILDCARDS)))
	{
		sprintf(f_spec, "%s*.*", drive);
		return(1);
	}

	// Check to see if the arg ends in a backslash
	tmp = strrchr(arg, '\\');
	if(tmp && (strlen(tmp) == 1))
	{
		sprintf(f_spec, "%s%s*.*", drive, path);
		return(1);
	}

	// Check to see if the arg is a directory
	if(_pd_getfileattr(arg, &attrib))
	{
		printf("\nFile Not Found: %s\n", arg);
		return(0);
	}
	else if(attrib & _A_SUBDIR)
	{
		sprintf(f_spec, "%s%s%s\\*.*", drive, path, file);
		return(1);
	}
	sprintf(f_spec, "%s%s%s%s", drive, path, file, ext);
	return(1);
}

void main(int argc, char *argv[])
{
	int			got_fname = 0;
	int			i = argc;

	// Check to make sure the PSYQ driver is installed
	if(!check_driver())
	{
		exit(1);
	}

	// Set the clean up function for when we leave
	atexit(clean_up);

	// Get the current drive so it can be restored when we leave
	_pd_getdrive(&rdrv);

	// Scan arguments for options
	while(--i)
	{
		switch(argv[i][0])
		{
			case '/':			// Option introducer
			{
				switch(toupper(argv[i][1]))
				{
					case 'P':	// Paginated listing
					{
						paginate = 1;
						break;
					}
					case 'W':	// Wide Listing
					{
						wide = 1;
						break;
					}
					case 'S':	// Recurse subdirectories
					{
						recurse = 1;
						break;
					}
					case 'B':	// Filenames only
					{
						summary = 0;
						wide = 0;
						break;
					}
					case 'L':	// Lower case filenames
					{
						lower_case = 1;
						break;
					}
					case 'C':	// File compression ratio
					{
						if(!wide && summary)
						{
							if(toupper(argv[i][2]) == 'H')	// Drive cluster size
							{
								drive_clusters = 1;
							}
						}
						break;
					}
					case 'A':	// Attributes
					{
						break;
					}
					case 'O':	// Sort order
					{
						break;
					}
					default:		// Unrecognized option
					{
						printf("Invalid command line option: %s\n", argv[i]);
						exit(1);
					}
				}
				break;
			}
		}
	}

	// Scan arguments for file names
	while(--argc)
	{
		if(argv[argc][0] != '/')
		{
			// Got a file name
			got_fname = 1;

			// Get a real pathname we can use
			if(!parse_argument(argv[argc]))
			{
				exit(1);
			}
			path_name = f_spec;

			// Done scanning arguments
			break;
		}
	}

	// Did we get a pathname on the command line ?
	if(!got_fname)
	{
		// NOPE - set to wild
		path_name = "*.*";

	}

	// Get filenames and info
	do_dir();

	// Show the files
	show_files();

	// All done
	exit(0);
}

static void do_dir(void)
{
	struct _find_t	ffblk;
	int				attrib = 0;
	int				done;

	// Set up the options
	if(hidden_files)
	{
		attrib |= FA_HIDDEN;
	}
	if(system_files)
	{
		attrib |= FA_SYSTEM;
	}
	if(read_only_files)
	{
		attrib |= FA_RDONLY;
	}
	if(directories)
	{
		attrib |= FA_DIREC;
	}
	if(archive_files)
	{
		attrib |= FA_ARCH;
	}
	done = _pd_findfirst(path_name, attrib, &ffblk);
	while(!done)
	{
		if(add_file_node(&ffblk))
		{
			file_count++;
			done = _pd_findnext(&ffblk);
		}
	}
}

static int add_file_node(struct _find_t *ffblk)
{
	ffnode_t	*fn;

	fn = malloc(sizeof(ffnode_t));
	if(!fn)
	{
		printf("Can not allocate memory for ffnode\n");
		return(0);
	}

	// List exists already ?
	if(fflist)
	{
		// YES - back link it to this node
		fflist->prev = fn;
	}

	// No nodes prior to this on
	fn->prev = (ffnode_t *)0;

	// This nodes next is the fflist
	fn->next = fflist;

	// Copy the info
	memcpy((void *)&fn->ffblk, (void *)ffblk, sizeof(struct _find_t));

	// Set the new list pointer
	fflist = fn;

	// Return success
	return(1);
}


static void show_files(void)
{
	int					drv;
	struct	find_t	f;
	int					fd = -1;
	char					vol_label[256];
	int					r;
	char					*tmp;

	_pd_getdrive(&drv);
	total_size = 0;
	dir_size = 0;
	file_count = 0;
	dir_count = 0;
	printf("\n");

	if(drv < 0x100)
	{
		sprintf(vol_label, "%c:\\*.*", drv | 0x40);
	}
	else
	{
		sprintf(vol_label, "*.*");
	}

	// Get the volume label of the drive if it has one
	if(!_pd_findfirst(vol_label, _A_VOLID, &f))
	{
		// Is the file a volume id file ?
		if(f.attrib & _A_VOLID)
		{
			// YES - open it
			if(!_pd_open(f.name, O_RDONLY, &fd))
			{
				// Read the data
				if(!_pd_read(fd, vol_label, f.size, &r))
				{
					// OK - close it
					_pd_close(fd);
				}
				else
				{
					// NOT OK - Close file and set fd to negative
					_pd_close(fd);
					fd = -1;
				}
			}
			else
			{
				fd = -1;
			}
		}
	}

	// DOS Drives
	if(drv < 0x100)
	{
		drv |= 0x40;
		if(fd < 0)
		{
			printf(" Volume in drive %c has no label\n", drv);
		}
		else
		{
			printf(" Volume in drive %c is %s\n", drv, vol_label);
		}
		printf(" Volume Serial Number is 2F7E-17CD\n");
		_fnsplit(path_name, drive, dir, name, ext);
		printf(" Directory of ");
//		if(strlen(dir) && !strcmp(dir, ".") && !strcmp(dir, ".\\"))
		if(strlen(dir))
		{
			tmp = strrchr(dir, '\\');
			if(tmp && (strlen(tmp) == 1))
			{
				*tmp = 0;
			}
			if(dir[0] != '\\')
			{
				tmp = getcwd(f_spec, sizeof(f_spec));
				*tmp = toupper(*tmp);
				printf("%s/%s", tmp, dir);
			}
			else
			{
				printf("%c:%s", drv, dir);
			}
		}
		else
		{
			tmp = getcwd(dir, sizeof(dir));
			*tmp = toupper(*tmp);
			printf("%s", tmp);
		}
		printf("\n\n");
	}

	// Phoenix drives
	else
	{
		drv -= 0x100;
		if(fd < 0)
		{
			printf(" Volume in drive PHX%d has no label\n", drv);
		}
		else
		{
			printf(" Volume in drive PHX%d is %s\n", drv, vol_label);
		}
		printf(" Volume Serial Number is 2F7E-17CD\n");
		printf(" Directory of PHX%d:\\\n\n", drv);
	}

	// Sort
	if(sort)
	{
		do_file_sort();
	}

	// Listing
	if(wide)
	{
		do_wide_list();
	}
	else
	{
		do_normal_list();
	}
}

static void show_f_and_d(void)
{
	char		billions[8];
	char		millions[8];
	char		thousands[8];
	char		hundreds[8];
	char		size_str[32];
	struct diskfree_t	df;
	int		status;

	billions[0] = 0;
	millions[0] = 0;
	thousands[0] = 0;
	hundreds[0] = 0;
	size_str[0] = 0;
	if(total_size >= 1000000000)
	{
		sprintf(billions, "%d", total_size/1000000000);
		total_size -= ((total_size / 1000000000) * 1000000000);
	}
	if(total_size >= 1000000)
	{
		if(billions[0])
		{
			sprintf(millions, "%03d,", total_size/1000000);
		}
		else
		{
			sprintf(millions, "%d,", total_size/1000000);
		}
		total_size -= ((total_size / 1000000) * 1000000);
	}
	if(total_size >= 1000)
	{
		if(millions[0])
		{
			sprintf(thousands, "%03d,", total_size/1000);
		}
		else
		{
			sprintf(thousands, "%d,", total_size/1000);
		}
		total_size -= ((total_size / 1000) * 1000);
	}
	if(thousands[0])
	{
		sprintf(hundreds, "%03d", total_size);
	}
	else
	{
		sprintf(hundreds, "%d", total_size);
	}
	strcat(size_str, billions);
	strcat(size_str, millions);
	strcat(size_str, thousands);
	strcat(size_str, hundreds);
	printf("%10d file(s)%15s bytes\n", file_count, size_str);

	// Get disk free space info
	status = _pd_getdiskfree(drv, &df);

	// Calculate number of bytes available
	dir_size = (unsigned long)df.avail_clusters *
		(unsigned long)df.bytes_per_sector *
		(unsigned long)df.sectors_per_cluster;

	billions[0] = 0;
	millions[0] = 0;
	thousands[0] = 0;
	hundreds[0] = 0;
	size_str[0] = 0;
	if(dir_size >= 1000000000)
	{
		sprintf(billions, "%d,", dir_size/1000000000);
		dir_size -= ((dir_size / 1000000000) * 1000000000);
	}
	if(dir_size >= 1000000)
	{
		if(billions[0])
		{
			sprintf(millions, "%03d,", dir_size/1000000);
		}
		else
		{
			sprintf(millions, "%d,", dir_size/1000000);
		}
		dir_size -= ((dir_size / 1000000) * 1000000);
	}
	if(dir_size >= 1000)
	{
		if(millions[0])
		{
			sprintf(thousands, "%03d,", dir_size/1000);
		}
		else
		{
			sprintf(thousands, "%d,", dir_size/1000);
		}
		dir_size -= ((dir_size / 1000) * 1000);
	}
	if(thousands[0])
	{
		sprintf(hundreds, "%03d", dir_size);
	}
	else
	{
		sprintf(hundreds, "%d", dir_size);
	}
	strcat(size_str, billions);
	strcat(size_str, millions);
	strcat(size_str, thousands);
	strcat(size_str, hundreds);
	printf("%10d dir(s)%16s bytes free\n", dir_count, size_str);
}

static void do_normal_list(void)
{
	ffnode_t	*f = fflist;
	char		millions[8];
	char		thousands[8];
	char		hundreds[8];
	char		size_str[32];
	int		size;
	int		time;
	int		pm;
	char		*tmp;
	char		buf1[81];

	if(!f)
	{
		return;
	}
	while(f->next)
	{
		f = f->next;
	}
	while(f)
	{
		_fnsplit(f->ffblk.name, drive, dir, name, ext);
		if(lower_case)
		{
			strlwr(name);
			strlwr(ext);
		}
		else
		{
			strupr(name);
			strupr(ext);
		}
		if(strlen(name) > 8)
		{
			name[6] = '~';
			name[7] = '1';
			name[8] = 0;
		}
		if(f->ffblk.attrib & FA_DIREC)
		{
			dir_count++;
			dir_size += f->ffblk.size;
			tmp = strrchr(dir, '\\');
			if(tmp)
			{
				*tmp = 0;
			}
			if(strlen(dir) > 0)
			{
				sprintf(buf1, "%s", dir);
			}
			else
			{
				sprintf(buf1, "%s", name);
			}
			printf("%-12s", buf1);
		}
		else
		{
			printf("%-9s", name);
			if(strlen(ext))
			{
				printf("%-3s", &ext[1]);
			}
			else
			{
				printf("   ");
			}
		}
		if(f->ffblk.attrib & FA_DIREC)
		{
			printf("   <DIR>      ");
		}
		else
		{
			millions[0] = 0;
			thousands[0] = 0;
			hundreds[0] = 0;
			size_str[0] = 0;
			size = f->ffblk.size;
			total_size += size;
			if(size >= 1000000)
			{
				sprintf(millions, "%d,", size/1000000);
				size -= ((size / 1000000) * 1000000);
			}
			if(size >= 1000)
			{
				if(millions[0])
				{
					sprintf(thousands, "%03d,", size/1000);
				}
				else
				{
					sprintf(thousands, "%d,", size/1000);
				}
				size -= ((size / 1000) * 1000);
			}
			if(thousands[0])
			{
				sprintf(hundreds, "%03d", size);
			}
			else
			{
				sprintf(hundreds, "%d", size);
			}
			strcat(size_str, millions);
			strcat(size_str, thousands);
			strcat(size_str, hundreds);
			printf("%14.14s", size_str);
			file_count++;
		}
		printf("  ");
		printf("%02d-", ((f->ffblk.wr_date >> 5) & 0xf));
		printf("%02d-", (f->ffblk.wr_date & 0x1f));
		printf("%02d ", ((f->ffblk.wr_date >> 9) & 0x7f) + 80);
		time = ((f->ffblk.wr_time >> 11) & 0x1f);
		pm = 'a';
		if(time > 12)
		{
			pm = 'p';
			time -= 12;
		}
		printf("%2d:", time);
		printf("%02d", ((f->ffblk.wr_time >> 5) & 0x3f));
		putchar(pm);
		putchar(' ');
		if(f->ffblk.attrib & FA_DIREC)
		{
			tmp = strrchr(dir, '\\');
			if(tmp)
			{
				*tmp = 0;
			}
			if(strlen(dir) > 0)
			{
				sprintf(buf1, "%s", dir);
			}
			else
			{
				sprintf(buf1, "%s", name);
			}
			printf("%s\n", buf1);
		}
		else
		{
			printf("%s%s\n", name, ext);
		}
		f = f->prev;
	}
	show_f_and_d();
}

static void do_wide_list(void)
{
	ffnode_t	*f = fflist;
	int		count = 0;
	char		buf[32];
	char		buf1[32];
	char		*tmp;

	if(f)
	{
		while(f->next)
		{
			f = f->next;
		}
		while(f)
		{
			count++;
			_fnsplit(f->ffblk.name, drive, dir, name, ext);
			if(strlen(name) > 8)
			{
				name[6] = '~';
				name[7] = '1';
				name[8] = 0;
			}
			sprintf(buf, "%s%s", name, ext);
			if(lower_case)
			{
				strlwr(buf);
			}
			else
			{
				strupr(buf);
			}
			if(f->ffblk.attrib & FA_DIREC)
			{
				dir_count++;
				dir_size += f->ffblk.size;
				tmp = strrchr(dir, '\\');
				if(tmp)
				{
					*tmp = 0;
				}
				if(strlen(dir) > 0)
				{
					sprintf(buf1, "[%s]", dir);
				}
				else
				{
					sprintf(buf1, "[%s]", name);
				}
			}
			else
			{
				file_count++;
				total_size += f->ffblk.size;
				sprintf(buf1, "%s", buf);
			}
			printf("%-16s", buf1);
			f = f->prev;
		}
		if(count % 5)
		{
			printf("\n");
		}
	}
	show_f_and_d();
}

static void do_file_sort(void)
{
}


void clean_up(void)
{
	ffnode_t	*f;
	ffnode_t	*fnext;
	int		l;

	f = fflist;
	while(f)
	{
		fnext = f->next;
		free(f);
		f = fnext;
	}
	_pd_setdrive(rdrv, &l);
}
