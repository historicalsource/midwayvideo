#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<dos.h>
#include	<dir.h>
#include	<fcntl.h>
#include	<search.h>

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

typedef struct flist
{
	struct flist	*next;
	struct flist	*prev;
	char				file_name[32];
	int				num;
} flist_t;


static flist_t	*flist;
static char		command_buffer[128];

static void tga_split(char *tga_file);

void main(int argc, char *argv[])
{
	char				fname[128];
	char				fname2[128];
	char				fbase[8];
	int				offset;
	struct ffblk	ffblk;
	flist_t			*fl;
	flist_t			*fl1;
	int				i;
	int				append;
	int				file_count = 0;
	char				*num_ptr;
	char				num[8];

	if(argc < 4)
	{
		fprintf(stderr, "USAGE: reseq <fname_base> <src_dir> <blk_offset>\r\n");
		exit(1);
	}

	sprintf(fname, "%s*.tga", argv[1]);

	if(strlen(argv[1]) > 4)
	{
//		fprintf(stderr, "File basename is longer than 4 characters\r\n");
//		exit(1);
		fbase[0] = argv[1][0];
		fbase[1] = argv[1][1];
		fbase[2] = argv[1][2];
		fbase[3] = argv[1][3];
		fbase[4] = 0;
	}

	else
	{
		strcpy(fbase, argv[1]);
	}
	while(strlen(fbase) < 4)
	{
		strcat(fbase, "_");
	}

	sscanf(argv[3], "%d", &offset);

	printf("Searching for files and ordering files...");
	fflush(stdout);
	sprintf(fname2, "%s\\%s", argv[2], fname);
	if(!findfirst(fname2, &ffblk, 0))
	{
		file_count++;
		flist = (flist_t *)malloc(sizeof(flist_t));
		if(!flist)
		{
			fprintf(stderr, "Can not allocate memory of file node\r\n");
			exit(1);
		}
		flist->next = (flist_t *)0;
		flist->prev = (flist_t *)0;
		strcpy(flist->file_name, ffblk.ff_name);
		num_ptr = ffblk.ff_name + strlen(argv[1]);
		i = 0;
		while(*num_ptr != '.')
		{
			num[i] = *num_ptr++;
			++i;
		}
		num[i] = 0;
		sscanf(num, "%d", &flist->num);
		while(!findnext(&ffblk))
		{
			file_count++;
			fl = (flist_t *)malloc(sizeof(flist_t));
			fl->next = (flist_t *)0;
			fl->prev = (flist_t *)0;
			if(!fl)
			{
				fprintf(stderr, "Can not allocate memory for file node\r\n");
				exit(1);
			}
			strcpy(fl->file_name, ffblk.ff_name);
			num_ptr = ffblk.ff_name + strlen(argv[1]);
			i = 0;
			while(*num_ptr != '.')
			{
				num[i] = *num_ptr++;
				++i;
			}
			num[i] = 0;
			sscanf(num, "%d", &fl->num);
			fl1 = flist;
			append = 1;
			while(fl1)
			{
				if(fl1->num == fl->num)
				{
					fprintf(stderr, "Multiple files with same sequence number %s %s\r\n", fl1->file_name, fl->file_name);
					exit(1);
				}
				if(fl1->num > fl->num)
				{
					append = 0;
					if(!fl1->prev)
					{
						flist = fl;
						flist->next = fl1;
						fl1->prev = fl;
						break;
					}
					else
					{
						insque((struct qelem *)fl, (struct qelem *)fl1->prev);
						break;
					}
				}
				fl1 = fl1->next;
			}
			if(append)
			{
				fl1 = flist;
				while(fl1->next)
				{
					fl1 = fl1->next;
				}
				fl1->next = fl;
				fl->prev = fl1;
			}
		}

		printf("\n%d Files\n", file_count);
		printf("Copying files for contigous TGB sequence\n");
		fl = flist;
		i = 0;
		while(fl)
		{
			sprintf(command_buffer, "copy %s\\%s %s%04d.tgb", argv[2], fl->file_name, fbase, i);
			printf("%s\\%s -> %s%04d.tgb\n", argv[2], fl->file_name, fbase, i);
			system(command_buffer);
			if(!i)
			{
				printf("Splitting first frame\n");
				sprintf(command_buffer, "%s%04d.tgb", fbase, i);
				tga_split(command_buffer);
				printf("Converting first frame to 3df fromat\n");
				sprintf(command_buffer, "texus -mn -t argb1555 -o %s1.3df %s1.tga", fbase, fbase);
				system(command_buffer);
				sprintf(command_buffer, "texus -mn -t argb1555 -o %s2.3df %s2.tga", fbase, fbase);
				system(command_buffer);
				printf("Converting first frame to wms format\n");
				sprintf(command_buffer, "3df2wms %s1.3df", fbase);
				system(command_buffer);
				sprintf(command_buffer, "3df2wms %s2.3df", fbase);
				system(command_buffer);
				printf("Deleting intermediate TGA and 3DF files\n");
				sprintf(command_buffer, "del %s1.tga", fbase);
				system(command_buffer);
				sprintf(command_buffer, "del %s2.tga", fbase);
				system(command_buffer);
				sprintf(command_buffer, "del %s1.3df", fbase);
				system(command_buffer);
				sprintf(command_buffer, "del %s2.3df", fbase);
				system(command_buffer);
			}
			++i;
			fl = fl->next;
		}

		printf("Converting files to palettized TGAs\n");
		system("alchemy -c 256 -q -a -^ -o *.tgb");

		printf("Deleting TGBs\n");
		system("del *.tgb");

		setenv("MOVIEIN", ".", 1);
		setenv("MOVIEOUT", ".", 1);
		setenv("MOVIECNT", ".", 1);
		printf("Convering palettized TGAs to movie file\n");
		sprintf(command_buffer, "movie %d %s0000 -b %d", file_count, fbase, offset);
		system(command_buffer);

		printf("Delete TGAs\n");
		system("del *.tga");
	}

	else
	{
		fprintf(stderr, "No file with base name %s found\r\n", fname);
		exit(1);
	}
	exit(0);
}


static void tga_split(char *tga_file)
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
		exit(1);
	}

	// Allocate memory for the file
	if((tga_buf = (char *)malloc(ffblk.ff_fsize)) == (char *)0)
	{
		fprintf(stderr, "Can not allocate memory for tga file: %s\r\n", tga_file);
		exit(1);
	}

	// Open the file
	if((fp = fopen(tga_file, "rb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open file: %s\r\n", tga_file);
		exit(1);
	}

	// Read the file into the buffer
	if(fread(tga_buf, sizeof(char), ffblk.ff_fsize, fp) != ffblk.ff_fsize)
	{
		fclose(fp);
		fprintf(stderr, "Could not read file: %s\r\n", tga_file);
		exit(1);
	}

	// Done with file - close it
	fclose(fp);

	// Set the header pointer
	thdr = (tga_header_t *)tga_buf;

	// Check for proper width and height
	if(thdr->width != 384 || thdr->height != 256)
	{
		fprintf(stderr, "TGA file %s is NOT correct dimension: %d x %d\r\n", tga_file, thdr->width, thdr->height);
		exit(1);
	}

	// Check for proper format
	if(thdr->image_type != 2)
	{
		fprintf(stderr, "TGA file %s is NOT true-color\r\n", tga_file);
		exit(1);
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
		exit(1);
	}

	// Write the header
	if(fwrite(thdr, sizeof(char), sizeof(tga_header_t), fp) != sizeof(tga_header_t))
	{
		fclose(fp);
		unlink(fname);
		fprintf(stderr, "Can not write header to %s\r\n", fname);
		exit(1);
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
			exit(1);
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
		exit(1);
	}

	// Write the header
	if(fwrite(thdr, sizeof(char), sizeof(tga_header_t), fp) != sizeof(tga_header_t))
	{
		fclose(fp);
		unlink(fname);
		fprintf(stderr, "Can not write header to %s\r\n", fname);
		exit(1);
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
			exit(1);
		}

		// Increment data pointer by line pitch (pixels)
		data_ptr += 384;
	}

	// Done with second file - close it
	fclose(fp);

	// Free up the memory
	free(tga_buf);
}

