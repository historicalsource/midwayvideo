#include	<crt0.h>
#include	<stdlib.h>
#include <ctype.h>
#include	<string.h>
#include	<stdio.h>
#include	<dir.h>
#include	<sys\stat.h>

#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include	<fcntl.h>
#include <dos.h>
#include	<pd.h>

// Buffer used for copying files
static char					file_buffer[16384+8192];
static char					vfile_buffer[16384+8192];

// File information for source file
static struct _find_t	finfo;

// File information for destination file (if it exists)
static struct _find_t	finfo2;

// Buffer used to hold the destination path
//static char					dst_path[128];

// Buffer used to hold the source path
//static char					src_path[128];

// Buffer used to hold the full pathname to the destination file
static char					dst_file_name[256];

// Buffer used to hold the full pathname to the source file
static char					src_file_name[256];

// Buffers used to hold source and destination drive, path, file, and ext
static char					dst_drive[81];
static char					dst_path[81];
static char					dst_file[81];
static char					dst_ext[81];
static char					src_drive[81];
static char					src_path[81];
static char					src_file[81];
static char					src_ext[81];
static int					src_d = 0;
static int					dst_d = 0;
static char					d_drive[81];
static char					d_path[81];
static int					cur_drive;
static int					verify = 0;		// Verify file flag
static int					prepend_crc = 0;	// Prepend CRC flag
static int					dst_is_target = 0;

unsigned long staged_crc(char *file);

// This function prevents crt0 from expanding wildcards
char **__crt0_glob_function(char *_arg)
{
	return((char **)0);
}

static int do_verify(char *src, char *dst)
{
	int			src_fd;
	int			dst_fd;
	long			amount_2_read;
	unsigned		amount_read;
	unsigned		amount_written;
	int			pdrv;
	int			i;

	_pd_setdrive(src_d, &pdrv);
		
	// Open the source file
	if(_pd_open(src, _O_RDONLY|_O_BINARY, &src_fd))
	{
		printf("Can not open src file: %s\n", src);
		return(0);
	}
	
	_pd_setdrive(dst_d, &pdrv);
		
	// Open the destination file
	if(_pd_open(dst, _O_RDONLY|_O_BINARY, &dst_fd))
	{
		printf("Can not open dst file: %s\n", dst);
		_pd_setdrive(src_d, &pdrv);
		_pd_close(src_fd);
		return(0);
	}

	if(dst_is_target && prepend_crc)
	{
		_pd_read(dst_fd, &i, sizeof(i), &amount_read);
	}

	// Copy the data from the src file to the destination file
	while(finfo.size)
	{
		// Figure out how much to transfer
		amount_2_read = finfo.size;
		if(amount_2_read > sizeof(file_buffer))
		{
			amount_2_read = sizeof(file_buffer);
		}

		// Set to the source drive
		_pd_setdrive(src_d, &pdrv);

		// Read from the source file
		if(_pd_read(src_fd, file_buffer, (unsigned)amount_2_read, &amount_read))
		{
			printf("Read error on source file\n");
			_pd_close(src_fd);
			_pd_setdrive(dst_d, &pdrv);
			_pd_close(dst_fd);
			return(0);
		}
	
		// Check to see if we got what we expected
		if(amount_read != (unsigned)amount_2_read)
		{
			printf("Short read on file: %s\n", src);
			_pd_close(src_fd);
			_pd_setdrive(dst_d, &pdrv);
			_pd_close(dst_fd);
			return(0);
		}

		// Set to the destination drive
		_pd_setdrive(dst_d, &pdrv);

		// Read from the destination file
		if(_pd_read(dst_fd, vfile_buffer, (unsigned)amount_2_read, &amount_written))
		{
			printf("Read error on destination file\n");
			_pd_close(dst_fd);
			_pd_setdrive(src_d, &pdrv);
			_pd_close(src_fd);
			// NOTE - Should delete the destination file here
			return(0);
		}
	
		// Check to make sure we read all of it
		if(amount_written != (unsigned)amount_2_read)
		{
			printf("Short read on file: %s - %d\n", dst, amount_written);
			_pd_close(dst_fd);
			_pd_setdrive(src_d, &pdrv);
			_pd_close(src_fd);
			// NOTE - Should delete the destination file here
			return(0);
		}

		// Check the data
		for(i = 0; i < amount_2_read; i++)
		{
			if(vfile_buffer[i] != file_buffer[i])
			{
				printf("Data error detected\n");
				_pd_close(dst_fd);
				_pd_setdrive(src_d, &pdrv);
				_pd_close(src_fd);
				return(0);
			}
		}

		/* Decrement the amount left to verify */
		finfo.size -= amount_2_read;
	}

	// Set to the source drive
	_pd_setdrive(src_d, &pdrv);
	
	// Close the source file
	_pd_close(src_fd);

	// Set to the destination drive
	_pd_setdrive(dst_d, &pdrv);

	// Close the destination file
	_pd_close(dst_fd);

	return(1);		
}

static unsigned long	crc_val;

static int do_file_copy(char *src, char *dst)
{
	int			src_fd;
	int			dst_fd;
	long			amount_2_read;
	unsigned		amount_read;
	unsigned		amount_written;
	unsigned		date;
	unsigned		time;
	int			pdrv;
	int			size;
	int			retry = 4;
	int			tsize;

do_retry:

	if(dst_is_target && prepend_crc)
	{
		crc_val = staged_crc(src);
	}

	_pd_setdrive(src_d, &pdrv);
		
	// Open the source file
	if(_pd_open(src, _O_RDONLY|_O_BINARY, &src_fd))
	{
		printf("Can not open src file: %s\n", src);
		return(0);
	}
	
	// Get the source file timestamp
	if(_pd_getftime(src_fd, &date, &time))
	{
		printf("Could not get date and time for source file\n");
		return(0);
	}
	// Swap bytes of low words if source is target system
	if (src_d == 0x100)
	{
		date = ((date >> 8) & 0xff) | ((date << 8) & 0xff00);
		time = ((time >> 8) & 0xff) | ((time << 8) & 0xff00);
	}
	
	_pd_setdrive(dst_d, &pdrv);
		
	// Create the destination file
	if(_pd_creat(dst, _A_NORMAL, &dst_fd))
	{
		printf("Can not open dst file: %s\n", dst);
		_pd_setdrive(src_d, &pdrv);
		_pd_close(src_fd);
		return(0);
	}

	// Save size for possible verify stage
	size = finfo.size;

	if(dst_is_target && prepend_crc)
	{
		// Set to the destination drive
		_pd_setdrive(dst_d, &pdrv);

		// Write the data to the destination file
		if(_pd_write(dst_fd, &crc_val, sizeof(crc_val), &amount_written))
		{
			printf("Could not write CRC\n");
			_pd_close(dst_fd);
			_pd_setdrive(src_d, &pdrv);
			_pd_close(src_fd);

			// NOTE - Should delete the destination file here
			return(0);
		}
	}

	tsize = finfo.size;

	// Copy the data from the src file to the destination file
	while(tsize > 0)
	{
		// Figure out how much to transfer
		amount_2_read = tsize;
		if(amount_2_read > sizeof(file_buffer))
		{
			amount_2_read = sizeof(file_buffer);
		}

		// Set to the source drive
		_pd_setdrive(src_d, &pdrv);

		// Read from the source file
		if(_pd_read(src_fd, file_buffer, (unsigned)amount_2_read, &amount_read))
		{
			printf("Read error on source file\n");
			_pd_close(src_fd);
			_pd_setdrive(dst_d, &pdrv);
			_pd_close(dst_fd);
			return(0);
		}
	
		// Check to see if we got what we expected
		if(amount_read != (unsigned)amount_2_read)
		{
			printf("Short read on file: %s\n", src);
			_pd_close(src_fd);
			_pd_setdrive(dst_d, &pdrv);
			_pd_close(dst_fd);
			return(0);
		}

		// Set to the destination drive
		_pd_setdrive(dst_d, &pdrv);

		if((amount_2_read & 3) && dst_is_target && prepend_crc)
		{
			file_buffer[amount_2_read+0] = 0;
			file_buffer[amount_2_read+1] = 0;
			file_buffer[amount_2_read+2] = 0;
			file_buffer[amount_2_read+3] = 0;
			amount_2_read += (4 - (amount_2_read & 3));
		}

		// Write the data to the destination file
		if(_pd_write(dst_fd, file_buffer, (unsigned)amount_2_read, &amount_written))
		{
			printf("Write error on destincation file\n");
			_pd_close(dst_fd);
			_pd_setdrive(src_d, &pdrv);
			_pd_close(src_fd);
			// NOTE - Should delete the destination file here
			return(0);
		}
	
		// Check to make sure we wrote all of it
		if(amount_written != (unsigned)amount_2_read)
		{
			printf("Short write on file: %s - %d\n", dst, amount_written);
			_pd_close(dst_fd);
			_pd_setdrive(src_d, &pdrv);
			_pd_close(src_fd);
			// NOTE - Should delete the destination file here
			return(0);
		}
	
		/* Decrement the amount left to copy */
		tsize -= amount_2_read;
	}

	// Set to the source drive
	_pd_setdrive(src_d, &pdrv);
	
	// Close the source file
	_pd_close(src_fd);

	// Set to the destination drive
	_pd_setdrive(dst_d, &pdrv);

	// Set the destination file timestamp
	if(_pd_setftime(dst_fd, date, time))
	{
		_pd_close(dst_fd);
		return(0);
	}
	_pd_close(dst_fd);

//	if(verify)
//	{
//		printf("Verifying\n");
//		// Reset size for verify
//		finfo.size = size;
//
//		// Verify and return
//		return(do_verify(src, dst));
//	}

return(1);

	if(retry)
	{
		finfo.size = size;

		if(do_verify(src, dst))
		{
			return(1);
		}

		printf("Verification failure - retrying: %d\n", 5 - retry);
		retry--;
		goto do_retry;
	}

	return(0);
}

void main(int argc, char *argv[])
{
	int	num_copied = 0;	 	// Actual number copied
	int	num_attempted = 0; 	// Number we attempted to copy/update
	int	update_only = 0;	 	// Flag used to tell to update only
	int	do_copy = 1;		 	// Flag used to tell to do the copy
	char	*dst_f = (char *)0;	// Pointer to the destination file name
	char	*dst_e = (char *)0;
	int	checked = 0;		 	// Count of the number of files check for update
	int	updated = 0;			// Count of the number of files updated
	int	new_file = 0;			// Count of the number of new files during update
	int	i;							// General iterator
	int	done;						// Variable used to determine when done
	int	is_new;					// Flag used to print status messages
	int	status;
	int	pdrv;

	// Check to make sure TBIOS is installed
	if(!check_driver())
	{
		printf("\n***** DRIVER ERROR *****\n");
		exit(1);
	}

	// Check to make sure there at least 3 arguments
	if(argc < 3)
	{
		printf("\nUSAGE: pcopy [/u] src dst\n");
		exit(1);
	}

	// Get the drive we are on
	_pd_getdrive(&cur_drive);

	// Walk the argument list looking for options
	i = argc;
	while(--i)
	{
		// Option introducer
		if(argv[i][0] == '/')
		{
			// Update option
			if(argv[i][1] == 'u' || argv[i][1] == 'U')
			{
				update_only = 1;
			}
			else if(argv[i][1] == 'v' || argv[i][1] == 'V')
			{
				verify = 1;
			}
			else if(argv[i][1] == 'c' || argv[i][1] == 'C')
			{
				prepend_crc = 1;
			}
			else
			{
				printf("\nUnrecognized command line option: %s\n", argv[i]);
				exit(1);
			}
		}
	}

	// Walk the argument list looking for the destination
	while(--argc)
	{
		// The last file on the argument list is the destination
		if(argv[argc][0] != '/')
		{
			// Split the destination file spec
			status = _fnsplit(argv[argc], dst_drive, dst_path, dst_file, dst_ext);

			// Check for filename
			if(!(status & FILENAME))
			{
				// NO filename specified - use source filename
				dst_f = src_file;
			}

			// Is filename wild ?
			else if(!strcmp(dst_file, "*"))
			{
				// Yes - Use source file name
				dst_f = src_file;
			}

			// Filename specified
			else
			{
				// Use the specified name
				dst_f = dst_file;
			}

			// Check for wildcard ext
			if(!(status & EXTENSION))
			{
				// No extension specified - use source extension
				dst_e = src_ext;
			}

			// Is extension wild ?
			else if(!strcmp(dst_ext, ".*"))
			{
				// Yes - use source extension
				dst_e = src_ext;
			}

			// Extension specified
			else
			{
				// Use the specified extension
				dst_e = dst_ext;
			}

			// Done looking for destination
			break;
		}
	}

	// Walk the argument list looking for files to copy/update
	while(--argc)
	{
		// Is this a command line option ?
		if(argv[argc][0] != '/')
		{
			// NOPE - Reset is new file
			is_new = 0;

			// Get the drive and path for the source
			_fnsplit(argv[argc], src_drive, src_path, NULL, NULL);

			// set the drive for the source
			if(strstr(argv[argc], "phx:") || strstr(argv[argc], "PHX:"))
			{
				src_d = 0x100;
			}
			else if(strchr(argv[argc], ':'))
			{
				src_d = toupper(src_file_name[0]);
				src_d -= 0x40;
			}

			// Set to the source drive
			_pd_setdrive(src_d, &pdrv);

			// Get the source file info
			done = _pd_findfirst(argv[argc], _A_NORMAL, &finfo);

			if(!done)
			{
				// Get the drive and path for the source
				_fnsplit(argv[argc], src_drive, src_path, NULL, NULL);
			}

			// Loop to copy/update files
			while(!done)
			{
				// Split the filespec into name and extension
				_fnsplit(finfo.name, d_drive, d_path, src_file, src_ext);

				// Generate a full path name to the destination file
				sprintf(dst_file_name, "%s%s%s%s", dst_drive, dst_path, dst_f, dst_e);

				// Generate a full path name to the source file
				sprintf(src_file_name, "%s%s%s%s", src_drive, src_path, src_file, src_ext);

				// set the drive for the source
				if(strstr(src_file_name, "phx:") || strstr(src_file_name, "PHX:"))
				{
					src_d = 0x100;
				}
				else if(strchr(src_file_name, ':'))
				{
					src_d = toupper(src_file_name[0]);
					src_d -= 0x40;
				}

				// set the drive for the destination
				if(strstr(dst_file_name, "phx:") || strstr(dst_file_name, "PHX:"))
				{
					dst_d = 0x100;
					dst_is_target = 1;
				}
				else if(strchr(dst_file_name, ':'))
				{
					dst_d = toupper(dst_file_name[0]);
					dst_d -= 0x40;
				}

				// Updating files only
				if(update_only)
				{
					// Increment count of files checked
					++checked;

					// Set to destination drive
					_pd_setdrive(dst_d, &pdrv);

					printf("Checking: %-40s\r", dst_file_name);
					fflush(stdout);

					// Does the source file exist on the destination machine ?
					if(_pd_findfirst(dst_file_name, _A_NORMAL, &finfo2))
					{
						// NOPE - Then copy it and increment the new file count
						do_copy = 1;
						new_file++;
						is_new = 1;
					}

					// If file exists on destination machine is source newer ?				
					else if(finfo.wr_time == finfo2.wr_time && finfo.wr_date == finfo2.wr_date)
					{
						// NOPE - Don't bother
						do_copy = 0;
					}

					// File does exist on destination but is not same as source
					else
					{
						// Copy the file and increment updated count
						do_copy = 1;
						updated++;
					}
				}
	
				// Not in update mode
				else
				{
					// Just do the copy
					do_copy = 1;
				}

				// This file need copying ?
				if(do_copy)
				{
					// YES - increment number attempted count
					++num_attempted;

					// Print a message about what is going on
					if(update_only && !is_new)
					{
						printf("Updating %-40s ", dst_file_name);
					}
					else
					{
						printf("Copying %s to %s ", src_file_name, dst_file_name);
					}

					// Copy the source file from the source machine to the destination
					// file on the destination machine
					if(do_file_copy(src_file_name, dst_file_name))
					{
						// If copied ok increment the successful copy count
						++num_copied;

						if(dst_is_target && prepend_crc)
						{
							printf("CRC: 0x%08.8lX\n", crc_val);
						}
						else
						{
							printf("\n");
						}
					}
					else if(update_only && !is_new)
					{
						_pd_setdrive(dst_d, &pdrv);
						pdremove(dst_file_name);
						printf("***** UPDATE FAILURE *****\n");
					}
					else
					{
						_pd_setdrive(dst_d, &pdrv);
						pdremove(dst_file_name);
						printf("***** COPY FAILURE *****\n");
					}
				}

				// Set to source drive
				_pd_setdrive(src_d, &pdrv);

				// Get next file name
				done = _pd_findnext(&finfo);
			}
			break;
		}
	}

	// Restore drive
	_pd_setdrive(cur_drive, &pdrv);

	// Print statistical messages about what went on
	if(update_only)
	{
		if(!updated && !new_file)
		{
			printf("\n");
		}
		printf("%d file(s) checked for update/copy\n", checked);
		printf("%d file(s) updated\n", updated);
		printf("%d new file(s) copied\n", new_file);
	}
	printf("%d of %d file(s) OK\n", num_copied, num_attempted);

	// If all did not get copied exit with error
	if(num_copied != num_attempted)
	{
		exit(1);
	}

	// Exit with success
	exit(0);
}


static unsigned long crctab[] = {
	0x0,
	0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6,
	0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
	0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f,
	0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a,
	0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58,
	0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033,
	0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
	0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4,
	0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5,
	0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
	0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c,
	0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1,
	0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b,
	0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698,
	0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
	0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f,
	0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80,
	0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
	0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629,
	0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c,
	0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e,
	0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65,
	0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
	0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2,
	0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74,
	0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
	0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a,
	0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087,
	0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d,
	0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce,
	0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
	0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09,
	0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf,
	0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


//
// crc_stage1() - This function is stage 1 of CRC generation on a buffer.
// After this stage is run on all of the data for a buffer, stage 2 (below)
// MUST be run.
//
unsigned long crc_stage1(unsigned long crc, unsigned char *data, int len)
{
	while(len--)
	{
		crc = crc << 8 ^ crctab[crc >> 24 ^ (*data)];
		data++;
	}
	return(crc);
}

//
// crc_stage2() - This function is the final stage of CRC generation.  After
// running stage 1 (above) on the entire data set, this function MUST be
// run to finish the CRC generation.
//
unsigned long crc_stage2(unsigned long crc, int len)
{
	while(len)
	{
		crc = crc << 8 ^ crctab[crc >> 24 ^ (len & 0xff)];
		len >>= 8;
	}
	return(~crc);
}


unsigned long staged_crc(char *file)
{
	FILE				*fp;
	int				length;
	int				amount;
	unsigned char	*buf;
	struct stat		f_stat;
	unsigned long	crc;
	int				extra = 0;
	int				extra_bytes;

	if(stat(file, &f_stat))
	{
		return(0);
	}

	if(f_stat.st_size & 3)
	{
		extra = 4 - (f_stat.st_size & 3);
	}
	extra_bytes = extra;
	
	if((fp = fopen(file, "rb")) == (FILE *)0)
	{
		return(0);
	}

	if((buf = (unsigned char *)malloc(8192)) == (unsigned char *)0)
	{
		fclose(fp);
		return(0);
	}

	length = f_stat.st_size;
	crc = 0;
	while(length || extra)
	{
		if(length)
		{
			amount = length;
			if(amount > 8192)
			{
				amount = 8192;
			}
			if(fread(buf, sizeof(char), amount, fp) != amount)
			{
				fclose(fp);
				free(buf);
				return(0);
			}
			crc = crc_stage1(crc, buf, amount);
			length -= amount;
		}
		else if(extra)
		{
			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
			buf[3] = 0;
			crc = crc_stage1(crc, buf, extra);
			extra = 0;
		}
	}
	fclose(fp);
	free(buf);

	return(crc_stage2(crc, f_stat.st_size + extra_bytes));
}


	
