#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<dir.h>
#include	<string.h>
#include	<sys/stat.h>
#include	"zlib.h"

static char	drive[256];
static char	dir[256];
static char	name[256];
static char	ext[256];
static char	out_fname[256];

void main(int argc, char *argv[])
{
	FILE	*fp;
	char	*in_buffer;
	char	*out_buffer;
	struct stat	f_stat;
	int	status;
	long	out_buf_size;
	float	compression;
	unsigned long	total_in_size = 0;
	unsigned long	total_out_size = 0;
	int				total_files = 0;
	float	lowest_ratio = 10000.0f;
	float	highest_ratio = 0.0f;
	int				largest_file = 0;
	int				smallest_file = 0x7fffffff;
	int				average_size;

	if(argc < 2)
	{
		fprintf(stderr, "USAGE: tcomp <files...>\n");
		exit(1);
	}

	printf("Compressing %d files\n", argc - 1);

	while(--argc)
	{
		if(stat(argv[argc], &f_stat))
		{
			fprintf(stderr, "Can not open file: %s\n", argv[argc]);
			exit(1);
		}

		if((in_buffer = (char *)malloc(f_stat.st_size)) == (char *)0)
		{
			fprintf(stderr, "Can not allocate memory for input file\n");
			exit(1);
		}

		total_in_size += f_stat.st_size;

		out_buf_size = f_stat.st_size + (int)((float)f_stat.st_size * 0.01f);
		out_buf_size += 8;

		if((out_buffer = (char *)malloc(out_buf_size)) == (char *)0)
		{
			free(in_buffer);
			fprintf(stderr, "Can not allocate memory for output buffer\n");
			exit(1);
		}

		if((fp = fopen(argv[argc], "rb")) == (FILE *)0)
		{
			free(in_buffer);
			free(out_buffer);
			fprintf(stderr, "Can not open file: %s\n", argv[argc]);
			exit(1);
		}

		if(fread(in_buffer, sizeof(char), f_stat.st_size, fp) != f_stat.st_size)
		{
			free(in_buffer);
			free(out_buffer);
			fclose(fp);
			fprintf(stderr, "Can not read file: %s\n", argv[argc]);
			exit(1);
		}

		fclose(fp);

		status = compress(out_buffer, &out_buf_size, in_buffer, f_stat.st_size);

		if(status != Z_OK)
		{
			free(in_buffer);
			free(out_buffer);
			fprintf(stderr, "Could not compress file: %s\n", argv[argc]);
			exit(1);
		}

		free(in_buffer);

		fnsplit(argv[argc], drive, dir, name, ext);

		status = strlen(ext);
		ext[status - 1] = 'z';

		sprintf(out_fname, "%s%s", name, ext);
	
		if((fp = fopen(out_fname, "wb")) == (FILE *)0)
		{
			free(out_buffer);
			fprintf(stderr, "Can not open file: %s\n", out_fname);
			exit(1);
		}

		if(fwrite(&out_buf_size, sizeof(int), 1, fp) != 1)
		{
			free(out_buffer);
			fclose(fp);
			unlink(out_fname);
			fprintf(stderr, "Could not write size to file: %s\n", out_fname);
			exit(0);
		}

		if(fwrite(out_buffer, sizeof(char), out_buf_size, fp) != out_buf_size)
		{
			free(out_buffer);
			fclose(fp);
			unlink(out_fname);
			fprintf(stderr, "Could not write file: %s\n", out_fname);
			exit(0);
		}

		total_out_size += (sizeof(int) + out_buf_size);

		if((sizeof(int) + out_buf_size) < smallest_file)
		{
			smallest_file = sizeof(int) + out_buf_size;
		}
		if((sizeof(int) + out_buf_size) > largest_file)
		{
			largest_file = sizeof(int) + out_buf_size;
		}


		free(out_buffer);

		compression = (float)f_stat.st_size / (float)out_buf_size;

		if(compression < lowest_ratio)
		{
			lowest_ratio = compression;
		}
		if(compression > highest_ratio)
		{
			highest_ratio = compression;
		}

		strupr(argv[argc]);

		printf("File %-12.12s compressed %6.2f:1\n", argv[argc], compression);

		fclose(fp);

		total_files++;
	}

	compression = (float)total_in_size / (float)total_out_size;

	average_size = total_out_size / total_files;

	printf("\n\n%d Files compressed with average compression ratio of %6.2f:1\n", total_files, compression);
	printf("Highest compression ratio: %7.2f\n", highest_ratio);
	printf("Lowest compression ratio:  %7.2f\n", lowest_ratio);
	printf("Largest file produced:     %7d (bytes)\n", largest_file);
	printf("Smallest file produced:    %7d (bytes)\n", smallest_file);
	printf("Average file size:         %7d (bytes)\n", average_size);
}

	
