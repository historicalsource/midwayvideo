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

#define	BLOCK_SIZE	8192

char		file_name[256];
char		command_buffer[256];

void main(int argc, char *argv[])
{
	FILE				*fp;
	unsigned char	*buffer;
	unsigned char	*buffer1;
	int				i;
	struct stat		f_stat;
	int				start_addr = 0x80100000;
	int				block_num = 1;
	int				amount2trans;
	int				pass;
	long				size;
	int				error_count = 0;
	long				total_blocks;
	int				led_data = 7;
	int				block_size = BLOCK_SIZE;
	int				disk = 1;
	int				disk_blocks;
	int				block_offset = 0;

	// Check for proper number of arguments
	if(argc < 2)
	{
		printf("\nUSAGE: pdtest [-d] [-bblock_size] <file>\n");
		exit(1);
	}

	// Get the args
	while(--argc)
	{
		switch(argv[argc][0])
		{
			case '-':
			case '/':
			{
				switch(argv[argc][1])
				{
					case 'b':
					case 'B':
					{
						block_size = atoi(&argv[argc][2]);
						if(!block_size)
						{
							block_size = BLOCK_SIZE;
						}
						break;
					}
					case 'd':
					case 'D':
					{
						disk = 1;
						break;
					}
					default:
					{
						fprintf(stderr, "Unrecognized command line argument: %c\r\n", argv[argc][1]);
						exit(1);
					}
				}
				break;
			}
			default:
			{
				strcpy(file_name, argv[argc]);
				break;
			}
		}
	}

	// If in disk test mode, don't allow block sizes > 4k
	if(disk && block_size > 4096)
	{
		block_size = 4096;
	}

	// Check to make sure the PSYQ driver is installed
	if(!check_driver())
	{
		exit(1);
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

	// Reset the target
	psyq_reset();

	// Allocate memory for the buffer
	if((buffer = (unsigned char *)malloc(block_size)) == (unsigned char *)0)
	{
		printf("Can not allocate memory for buffer\n");
		fclose(fp);
		exit(1);
	}

	// Allocate memory for the readback buffer
	if((buffer1 = (unsigned char *)malloc(block_size)) == (unsigned char *)0)
	{
		free(buffer);
		printf("Can not allocate memory for buffer1\n");
		fclose(fp);
		exit(1);
	}

	// Wait for target to finish reset
	usleep(500000);

	// Print some info
	printf("\n\nPDTEST - Host to target data communications data integrity test\n");
	printf("Running test\n");
	printf("Block size: %d (bytes)\n", block_size);
	printf("Press <CNTL-C> to exit\n");

	// Set starting block number
	block_num = 1;

	// Save file size
	size = f_stat.st_size;

	// Set pass counter
	pass = 1;

	// Calculate total number of blocks to transfer
	total_blocks = f_stat.st_size / block_size;
	if(f_stat.st_size % block_size)
	{
		total_blocks++;
	}

	// Infinit loop to run test
	do
	{
		// Set total size to transfer
		f_stat.st_size = size;

		// Close the file
		fclose(fp);

		// Show pass number
		fprintf(stderr, "Pass: %d\n", pass);

		// Open the file to be loaded
		if((fp = fopen(file_name, "rb")) == (FILE *)0)
		{
			printf("Can not open file %s\n", file_name);
			exit(1);
		}

		// If disk test print prompt
		if(disk)
		{
			fprintf(stderr, "Write/Read Pass\r\n");
		}

		// Do the whole file
		while(f_stat.st_size > 0)
		{
			// Show current block number
			fprintf(stderr, "Block Number: %07d of %07ld\r", block_num, total_blocks);

			// Figure out how much to transfer this time
			amount2trans = f_stat.st_size;
			if(amount2trans > block_size)
			{
				amount2trans = block_size;
			}

			// Calculate number of disk blocks
			disk_blocks = amount2trans / 512;
			if(amount2trans % 512)
			{
				disk_blocks++;
			}

			// Read a block of data from the file into the write buffer
			if(fread(buffer, sizeof(char), amount2trans, fp) != amount2trans)
			{
				printf("Could not read all of the data from the file\n");
				free(buffer);
				free(buffer1);
				fclose(fp);
				exit(1);
			}
	
			// Toggle the targets led
			led_data ^= 1;
			psyq_mem_write(0xb7900000, led_data);

			if(!disk)
			{
				// Send the block of data to the target
				if(psyq_blk_write(start_addr, buffer, amount2trans) == 1)
				{
					printf("Could not send data to target\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}
			}
			else
			{
				// Force a seek back to original block
				if(read_sectors(0, buffer1, random()%1500000, 1) != 1)
				{
					printf("Could not seek target disk\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}

				// Send the block of data to the target
				if(write_sectors(0, buffer, 2000000 + block_offset, disk_blocks) != disk_blocks)
				{
					printf("Could not send data to target disk\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}
			}
	
			// Toggle the targets led
			led_data ^= 2;
			psyq_mem_write(0xb7900000, led_data);

			if(!disk)
			{
				// Read the data back from the target into the read buffer
				if(psyq_blk_read(start_addr, buffer1, amount2trans) == 1)
				{
					printf("Could not read data from target\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}
			}
			else
			{
				// Force a seek back to original block
				if(read_sectors(0, buffer1, random()%1500000, 1) != 1)
				{
					printf("Could not seek target disk\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}

				// Read back the block of data from the target disk
				if(read_sectors(0, buffer1, 2000000 + block_offset, disk_blocks) != disk_blocks)
				{
					printf("Could not read data from target disk\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}
			}

			// Increment the block offset
			block_offset += disk_blocks;
	
			// Toggle the targets led
			led_data ^= 4;
			psyq_mem_write(0xb7900000, led_data);

			// Check the read buffer against the write buffer
			if(memcmp(buffer, buffer1, amount2trans))
			{
				// Show error message if NOT the same
				printf("\nData error block: %d  - byte: %d - offset: %d\n", block_num, i, (block_num-1)*block_size + i);

				// Increment the pass error counter
				++error_count;
			}

			// Increment current block number	
			block_num++;

			// Decrement size left to transfer by current transfer size
			f_stat.st_size -= amount2trans;
		}

		// If disk test do a read pass too
		if(disk)
		{
			// Print a prompt
			fprintf(stderr, "\r\nRead pass\r\n");

			// Reset the current block counter
			block_num = 1;

			// Reset the block offset
			block_offset = 0;

			// Set total size to transfer
			f_stat.st_size = size;

			// Close the file
			fclose(fp);

			// Open the file to be loaded
			if((fp = fopen(file_name, "rb")) == (FILE *)0)
			{
				printf("Can not open file %s\n", file_name);
				exit(1);
			}

			// Do the whole file
			while(f_stat.st_size > 0)
			{
				// Show current block number
				fprintf(stderr, "Block Number: %07d of %07ld\r", block_num, total_blocks);

				// Figure out how much to transfer this time
				amount2trans = f_stat.st_size;
				if(amount2trans > block_size)
				{
					amount2trans = block_size;
				}
	
				// Calculate number of disk blocks
				disk_blocks = amount2trans / 512;
				if(amount2trans % 512)
				{
					disk_blocks++;
				}

				// Read a block of data from the file into the write buffer
				if(fread(buffer, sizeof(char), amount2trans, fp) != amount2trans)
				{
					printf("Could not read all of the data from the file\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}
	
				// Toggle the targets led
				led_data ^= 2;
				psyq_mem_write(0xb7900000, led_data);

				// Force a seek back to original block
				if(read_sectors(0, buffer1, random()%1500000, 1) != 1)
				{
					printf("Could not seek target disk\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}

				// Read back the block of data from the target disk
				if(read_sectors(0, buffer1, 2000000 + block_offset, disk_blocks) != disk_blocks)
				{
					printf("Could not read data from target disk\n");
					free(buffer);
					free(buffer1);
					fclose(fp);
					exit(1);
				}

				// Increment the block offset
				block_offset += disk_blocks;
	
				// Toggle the targets led
				led_data ^= 4;
				psyq_mem_write(0xb7900000, led_data);

				// Check the read buffer against the write buffer
				if(memcmp(buffer, buffer1, amount2trans))
				{
					// Show error message if NOT the same
					printf("\nData error block: %d  - byte: %d - offset: %d\n", block_num, i, (block_num-1)*block_size + i);

					// Increment the pass error counter
					++error_count;
				}

				// Increment current block number	
				block_num++;

				// Decrement size left to transfer by current transfer size
				f_stat.st_size -= amount2trans;
			}
		}
	
		// Show status of this pass
		fprintf(stderr, "\r\nTest Done - ");
		if(error_count)
		{
			fprintf(stderr, "%d", error_count);
		}
		else
		{
			fprintf(stderr, "No");
		}
		fprintf(stderr, " passes with errors\r\n\r\n");

		// Increment the pass counter
		pass++;

		// Reset the current block counter
		block_num = 1;

		// Reset the block offset
		block_offset = 0;

	} while(1);

	// Free the allocated buffers
	free(buffer);
	free(buffer1);

	// exit with ok status
	exit(0);
}

