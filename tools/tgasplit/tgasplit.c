#include <stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<dos.h>
#include	<dir.h>
#include	<fcntl.h>
#include	<search.h>
#include    <errno.h>

typedef struct tga_header
{
	unsigned char	id_length __attribute__ ((packed));
	unsigned char	color_map_type __attribute__ ((packed));
	unsigned char	image_type __attribute__ ((packed));
	unsigned short	first_entry_index __attribute__ ((packed));
	unsigned short	color_map_length __attribute__ ((packed));
	unsigned char	color_map_entry_size __attribute__ ((packed));
	unsigned short	x_origin __attribute__ ((packed));
	unsigned short	y_origin __attribute__ ((packed));
	unsigned short	width __attribute__ ((packed));
	unsigned short	height __attribute__ ((packed));
	unsigned char	pixel_depth __attribute__ ((packed));
	unsigned char	image_descriptor __attribute__ ((packed));
} tga_header_t;

int tga_split(char *tga_file);

int usage(char *progname)
{
	fprintf(stdout, "Usage: %s [-h] <*.tga>\n", progname);
	fprintf(stdout, "Splits a 384x256 true color tga file in to two tga files.\n");
	fprintf(stdout, "One is 256x256 and the other is 128x256.\n");
	return 1;
}

int main(int argc, char **argv)
{

	int i, errors = 0;
	struct ffblk	ffblk;

	i=1;
	while (i<argc) {
		if (*argv[i] == '/' || *argv[i] == '-') {
			switch(*argv[i]) {
			case 'h':
			default:
				return usage(argv[0]);
			}
		}
		else {
			if (!findfirst(argv[i], &ffblk, 0)) {
				printf("found name: %s\n", ffblk.ff_name); 
				tga_split(ffblk.ff_name); 
				while (!findnext(&ffblk)) {
					tga_split(ffblk.ff_name); 
				}
			}
			else {
				fprintf(stdout, "Couldn't match filespec: %s\n", argv[i]);
				errors++;
			}
		}
		i++;
	}
	return (errors!=0);
}

				

int tga_split(char *tga_file)
{
	FILE				*fp;
	struct ffblk	ffblk;
	char				*tga_buf;
	tga_header_t	*thdr;
	char				*tga_256_buf;
	char				*tga_128_buf;
	int				offset;
	int				*data_ptr;
	int				y;
	char				fname[16];

	// Get the size of the file
	if(findfirst(tga_file, &ffblk, 0))
	{
		fprintf(stderr, "Can not find file: %s\r\n", tga_file);
		return(1);
	}

	// Allocate memory for the file
	if((tga_buf = (char *)malloc(ffblk.ff_fsize)) == (char *)0)
	{
		fprintf(stderr, "Can not allocate memory for tga file: %s\r\n", tga_file);
		return(1);
	}

	// Open the file
	if((fp = fopen(tga_file, "rb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open file: %s\r\n", tga_file);
		return(1);
	}

	// Read the file into the buffer
	if(fread(tga_buf, sizeof(char), ffblk.ff_fsize, fp) != ffblk.ff_fsize)
	{
		fclose(fp);
		fprintf(stderr, "Could not read file: %s\r\n", tga_file);
		return(1);
	}

	// Done with file - close it
	fclose(fp);

	// Set the header pointer
	thdr = (tga_header_t *)tga_buf;

	// Check for proper width and height
	if(thdr->width != 384 || thdr->height != 256)
	{
		fprintf(stderr, "TGA file %s is NOT correct dimension: %d x %d\r\n", tga_file, thdr->width, thdr->height);
		return(1);
	}

	// Check for proper format
	if(thdr->image_type != 2)
	{
		fprintf(stderr, "TGA file %s is NOT true-color\r\n", tga_file);
		return(1);
	}

	// Set the image offset
	offset = sizeof(tga_header_t);

	// Adjust the offset for id field if it exists
	if(thdr->id_length)
	{
		offset += thdr->id_length;
	}

	// Adjust the offset for color map field if it exists
	if(thdr->color_map_type)
	{
		offset += (thdr->color_map_length * (thdr->color_map_entry_size / 8));
	}

	// First TGA is 256x256 true color
	thdr->width = 256;

	// Generate file name for 1st TGA
	fname[0] = tga_file[0];
	fname[1] = tga_file[1];
	fname[2] = tga_file[2];
	fname[3] = tga_file[3];
	fname[4] = 0;
	strcat(fname, "1.tga");

	// Open the file
	if((fp = fopen(fname, "wb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open %s\r\n", fname);
		return(1);
	}

	// Write the header
	if(fwrite(thdr, sizeof(char), sizeof(tga_header_t), fp) != sizeof(tga_header_t))
	{
		fclose(fp);
		unlink(fname);
		fprintf(stderr, "Can not write header to %s\r\n", fname);
		return(1);
	}

	// Set source pixel data pointer
	data_ptr = (int *)(tga_buf + offset);

	// Write 256 lines of 256 pixels to file
	for(y = 0; y < 256; y++)
	{
		if(fwrite(data_ptr, sizeof(char), 256*4, fp) != 256*4)
		{
			fclose(fp);
			unlink(fname);
			fprintf(stderr, "Could not write line %d to %s\r\n", y, fname);
			return(1);
		}

		// Increment data pointer by line pitch (pixels)
		data_ptr += 384;
	}

	// Done with first file - close it
	fclose(fp);

	// Second TGA is 128x256 true color
	thdr->width = 128;

	// Generate file name for 2nd TGA
	fname[0] = tga_file[0];
	fname[1] = tga_file[1];
	fname[2] = tga_file[2];
	fname[3] = tga_file[3];
	fname[4] = 0;
	strcat(fname, "2.tga");

	// Open the file
	if((fp = fopen(fname, "wb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open %s\r\n", fname);
		return(1);
	}

	// Write the header
	if(fwrite(thdr, sizeof(char), sizeof(tga_header_t), fp) != sizeof(tga_header_t))
	{
		fclose(fp);
		unlink(fname);
		fprintf(stderr, "Can not write header to %s\r\n", fname);
		return(1);
	}

	// Set the source data pointer
	data_ptr = (int *)(tga_buf + offset);
	data_ptr += 256;

	// Write 256 lines of 128 pixels to file
	for(y = 0; y < 256; y++)
	{
		if(fwrite(data_ptr, sizeof(char), 128*4, fp) != 128*4)
		{
			fclose(fp);
			unlink(fname);
			fprintf(stderr, "Could not write line %d to %s\r\n", y, fname);
			return(1);
		}

		// Increment data pointer by line pitch (pixels)
		data_ptr += 384;
	}

	// Done with second file - close it
	fclose(fp);

	// Free up the memory
	free(tga_buf);
}

