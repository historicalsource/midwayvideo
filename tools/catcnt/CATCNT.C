#include	<stdio.h>
#include	<dir.h>
#include	<stdlib.h>

typedef struct control_file_data
{
	unsigned long	num_frames;
	unsigned long	partition;
	unsigned long	start_block;
	unsigned long	zbuffer;
	unsigned long	size;
} control_file_data_t;

#define	MAX_CFILES	1024

static control_file_data_t	control[MAX_CFILES];
static char						*control_file[MAX_CFILES];

void main(int argc, char *argv[])
{
	int	i;
	int	j;
	int	total_blocks;
	int	partition;
	FILE	*fp;

	i = 0;
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: catcnt <cntfiles>\r\n");
		exit(1);
	}
	printf("Reading Control Files\n");
	while(--argc)
	{
		control_file[i] = argv[argc];

		if((fp = fopen(argv[argc], "rb")) == (FILE *)0)
		{
			fprintf(stderr, "Can not open file: %s\r\n", argv[argc]);
			exit(1);
		}
		if((fread(&control[i], sizeof(control_file_data_t), 1, fp)) != 1)
		{
			fprintf(stderr, "Can not read control file %s\r\n", argv[argc]);
			fclose(fp);
			exit(1);
		}
		fclose(fp);
		++i;
	}
	printf("%d Control Files Read\n", i);

	total_blocks = 0;
	partition = control[0].partition;
	for(j = 0; j < i; j++)
	{
		if(j)
		{
			if(control[j].partition != partition)
			{
				fprintf(stderr, "Control files use different partitions\r\n");
				exit(1);
			}
		}
		total_blocks += (control[j].num_frames * (control[j].size == 0 ? 193 : 385));
	}

	printf("Total number of blocks: %d\n", total_blocks);

	control[0].start_block = 0;
	printf("New start block for %s: %d\n", control_file[0], control[0].start_block);

	for(j = 1; j < i; j++)
	{
		control[j].start_block = control[j-1].start_block + (control[j-1].num_frames * (control[j-1].size == 0 ? 193 : 385));
		printf("New start block for %s: %d\n", control_file[j], control[j].start_block);
	}

	for(j = 0; j < i; j++)
	{
		if((fp = fopen(control_file[j], "wb")) == (FILE *)0)
		{
			fprintf(stderr, "Can not open file %s for update\r\n", control_file[j]);
			exit(1);
		}

		if(fwrite(&control[j], sizeof(control_file_data_t), 1, fp) != 1)
		{
			fclose(fp);
			fprintf(stderr, "Could not write data to control file %s\r\n", control_file[j]);
			exit(1);
		}
		fflush(fp);
		fclose(fp);
	}
	printf("Control Files Updated\n");
	exit(0);
}

