#include	<stdio.h>
#include	<unistd.h>
#include	<sys/stat.h>

static unsigned int calc_checksum(unsigned char *buf, int length)
{
	unsigned int	checksum = 0;

	printf("Calculating checksum on %d bytes\n", length);
	while(length--)
	{
		checksum += (unsigned int)*buf++;
	}
	return(checksum);
}

int main(int argc, char *argv[])
{
	FILE				*fp;
	unsigned char	*buf;
	int				extra_bytes;
	struct stat		f_stat;
	int				i;
	unsigned int	*iptr;
	unsigned int	cksum;
	
	// Check for proper arguments
	if(argc < 1)
	{
		printf("\n\nUSAGE: romsum <file>\n\n");
		exit(1);
	}

	// Get the info about the file
	if(stat(argv[1], &f_stat))
	{
		printf("\n\nCan not get stats for file %s\n", argv[1]);
		exit(1);
	}

	// Is the file any even multiple of 4 bytes long ?
	if(f_stat.st_size < (0x80000 - 4))
	{
		extra_bytes = (0x80000 - 4) - f_stat.st_size;
	}
	else
	{
		extra_bytes = 0;
	}

	// Open the file
	if((fp = fopen(argv[1], "rb")) == (FILE *)0)
	{
		printf("\n\nCan not open file %s\n", argv[1]);
		exit(1);
	}

	// Allocate memory for the file padded to 4 byte multiple and check value
	if((buf = (unsigned char *)malloc(f_stat.st_size + extra_bytes + 4)) == (unsigned char *)0)
	{
		fclose(fp);
		printf("\n\nCan not allocate memory for buffer\n");
		exit(1);
	}

	// Read the entire file
	if(fread(buf, sizeof(char), f_stat.st_size, fp) != f_stat.st_size)
	{
		fclose(fp);
		free(buf);
		printf("\n\nCan not read file %s\n", argv[1]);
		exit(1);
	}

	// Done with file - close it
	fclose(fp);

	// Clear the extra bytes
	for(i = f_stat.st_size; i < (f_stat.st_size + extra_bytes); i++)
	{
		buf[i] = 0xff;
	}

	// Calculate the checksum on file and extra bytes
	cksum = calc_checksum(buf, f_stat.st_size + extra_bytes);

	// Generate a pointer to the check part of buffer
	iptr = (unsigned int *)&buf[f_stat.st_size + extra_bytes];

	// Put the checksum there
	*iptr = cksum;

	// Delete the old file
	unlink(argv[1]);

	// Open the new file
	if((fp = fopen(argv[1], "wb")) == (FILE *)0)
	{
		free(buf);
		printf("\n\nCould not open file %s\n", argv[1]);
		exit(1);
	}

	// Write the new data to the file
	if(fwrite(buf, sizeof(char), f_stat.st_size+extra_bytes+4, fp) != (f_stat.st_size+extra_bytes+4))
	{
		fclose(fp);
		unlink(argv[1]);
		free(buf);
		printf("\nCould not write file %s\n", argv[1]);
		exit(1);
	}

	// Close the file
	fclose(fp);

	// Free the buffer
	free(buf);

	// Print out a message
	printf("Checksum for file %s: 0x%08.8X\n", argv[1], cksum);

	// Exit with good status
	exit(0);

	// Shut up the compiler
	return(0);
}

