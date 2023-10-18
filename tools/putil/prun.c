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

#define	EPC_OFFSET	(34*8)

char		file_name[256];
char		command_buffer[256];

void main(int argc, char *argv[])
{
	int	regs_address;
	FILE	*fp;
	char	*buffer;
	struct stat	f_stat;
	int	start_addr = 0x800c0000;
	int	is_exe = 0;
	int	size;

	// Check for proper number of arguments
	if(argc < 2)
	{
		printf("\nUSAGE: prun <file> [start_addr]\n");
		exit(1);
	}

	// Get the args
	while(--argc)
	{
		switch(argv[argc][0])
		{
			case '0':
			{
				sscanf(argv[argc], "%x", &start_addr);
				break;
			}
			default:
			{
				strcpy(file_name, argv[argc]);
				break;
			}
		}
	}

	// Check to make sure the PSYQ driver is installed
	if(!check_driver())
	{
		exit(1);
	}

	// Get the address of the registers
	regs_address = psyq_get_regs_addr();

	// Convert to lower case
	strlwr(file_name);

	// Check for a .exe extension
	if(!strstr(file_name, ".exe"))
	{
		// Check for a .bin extension
		if(!strstr(file_name, ".bin"))
		{
			// Check for a .cpe extension
			if(strstr(file_name, ".cpe"))
			{
				// Check to see if the file exists
				if(access(file_name, F_OK))
				{
					printf("File Not Found: %s\n", file_name);
					exit(1);
				}
	
				// Reset the target
				psyq_reset();
	
				// Wait for it to finish
	//			usleep(3000000);
	
				// Create the run command
				sprintf(command_buffer, "run %s", file_name);
	
				// Run the command
				system(command_buffer);

				// Done
				exit(0);
			}

			// Append a .bin extension to the file name
			strcat(file_name, ".bin");
		}
	}
	else
	{
		is_exe = 1;
	}

	// Check to see if the file exists
	if(access(file_name, F_OK))
	{
		printf("File Not Found: %s\n", file_name);
		exit(1);
	}

	// Figure out how big the file is
	if(stat(file_name, &f_stat))
	{
		printf("Can not get stats for file %s\n", file_name);
		exit(1);
	}

	// Open the file to be loaded
	if((fp = fopen(file_name, "rb")) == (FILE *)0)
	{
		printf("Can not open file %s\n", file_name);
		exit(1);
	}

	// Set the size to transfer
	size = f_stat.st_size;
	if(is_exe)
	{
		// Read the execution address
		fread(&start_addr, sizeof(int), 1, fp);

		// Subtract 4 from the file size
		size -= 4;
	}

	// Reset the target
	psyq_reset();

	// Allocate memory for the buffer
	if((buffer = (char *)malloc(size)) == (char *)0)
	{
		printf("Can not allocate memory for buffer\n");
		fclose(fp);
		exit(1);
	}

	// Read the file into the buffer
	if(fread(buffer, sizeof(char), size, fp) != size)
	{
		printf("Could not read all of the data from the file\n");
		fclose(fp);
		exit(1);
	}

	// Close the file
	fclose(fp);

	// Show the name of the file being downloaded
	printf("Downloading: %s to address: 0x%8.8X\n", file_name, start_addr);

	// Wait a bit for the target reset to complete
	usleep(500000);

	// Get the address of the registers
	regs_address = psyq_get_regs_addr();

	// Send the file to the target
	if(psyq_blk_write(start_addr, buffer, size) == 1)
	{
		printf("Could not send file to target\n");
		free(buffer);
		exit(1);
	}

	// Free the allocated buffer
	free(buffer);

	// Set the target's EPC register
	psyq_mem_write(regs_address + EPC_OFFSET, start_addr);

	// Print another message
	printf("Starting Target\n");

	// Let the target run
	psyq_enable();

	// exit with ok status
	exit(0);
}

