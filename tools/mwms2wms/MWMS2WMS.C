#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<glide\glide.h>

#undef	printf

typedef struct wms_header
{
	int			version;
	float			lod_bias;
	int			filter_type;
	int			trilinear;
	Gu3dfHeader	gu_header;
} wms_header_t;

typedef struct wms_node
{
	char 				*file_name;
	wms_header_t	wms_header;
} wms_node_t;

static wms_node_t		wms_node_list[9];
static int				wms_node_count = 0;

static wms_header_t	composite_wms_header;

int node_cmp(const void *a, const void *b)
{
	wms_node_t	*a1;
	wms_node_t	*b1;

	a1 = (wms_node_t *)a;
	b1 = (wms_node_t *)b;

	return(a1->wms_header.gu_header.large_lod - b1->wms_header.gu_header.large_lod);
}
	

void main(int argc, char *argv[])
{
	FILE	*fp;
	FILE	*out_fp;
	int	i;
	char	*out_fname;
	int	amount;
	int	c;

	if(argc < 3)
	{
		fprintf(stderr, "USAGE: mwms2wms <wms_file_list> <out_file>\r\n");
		exit(1);
	}

	--argc;

	// Last name on list is assumed to be output file
	out_fname = argv[argc];

	// Get the file names for all of the input files
	while(--argc)
	{
		if(!strstr(argv[argc], ".WMS") && !strstr(argv[argc], "wms"))
		{
			fprintf(stderr, "File %s does not appear to be a WMS file\r\n", argv[argc]);
			exit(1);
		}
		wms_node_list[wms_node_count++].file_name = argv[argc];
		if(wms_node_count > 9)
		{
			fprintf(stderr, "Too many input files\r\n");
			exit(1);
		}
	}

	// Are there input files ?
	if(wms_node_count < 1)
	{
		// NO - error
		fprintf(stderr, "No WMS input files\r\n");
		exit(1);
	}

	// Read the headers for each of the input files
	for(i = 0; i < wms_node_count; i++)
	{
		// Open the file
		if((fp = fopen(wms_node_list[i].file_name, "rb")) == (FILE *)0)
		{
			// ERROR
			fprintf(stderr, "Can not open file: %s\r\n", wms_node_list[i].file_name);
			exit(1);
		}

		// Read the header
		if((amount = fread(&wms_node_list[i].wms_header, 1, sizeof(wms_header_t), fp)) != sizeof(wms_header_t))
		{
			// ERROR
			fprintf(stderr, "Could not read header for file: %s\r\n", wms_node_list[i].file_name);
			fclose(fp);
			exit(1);
		}

		// Close the file
		fclose(fp);
	}

	// Sort the input file list into ascending lod order
	qsort((void *)wms_node_list, wms_node_count, sizeof(wms_node_t), node_cmp);

	// Walk the list and make sure all input files have only 1 level of detail
	for(i = 0; i < wms_node_count; i++)
	{
		if(wms_node_list[i].wms_header.gu_header.small_lod != wms_node_list[i].wms_header.gu_header.large_lod)
		{
			fprintf(stderr, "File %s has more than 1 LOD in it\r\n", wms_node_list[i].file_name);
			exit(1);
		}
	}

	// Walk the list and make sure there sequential levels of detail
	for(i = 1; i < wms_node_count; i++)
	{
		if(wms_node_list[i-1].wms_header.gu_header.small_lod != 
			(wms_node_list[i].wms_header.gu_header.small_lod - 1))
		{
			fprintf(stderr, "Non-seqential LODs in input files\r\n");
			exit(1);
		}
	}

	// Walk the list and make sure the aspect ratios of all of the files are
	// the same, all of the formats are the same, all of the lod bias values
	// are the same, all of the filter types are the same, all of the
	// trilinear modes are the same, and all of the versions are the same
	for(i = 1; i < wms_node_count; i++)
	{
		if(wms_node_list[0].wms_header.gu_header.aspect_ratio != 
			wms_node_list[i].wms_header.gu_header.aspect_ratio)
		{
			fprintf(stderr, "Aspect ratio mismatch in input files\r\n");
			exit(1);
		}
		if(wms_node_list[0].wms_header.gu_header.format != 
			wms_node_list[i].wms_header.gu_header.format)
		{
			fprintf(stderr, "Texture format mismatch in input files\r\n");
			exit(1);
		}
		if(wms_node_list[0].wms_header.version != 
			wms_node_list[i].wms_header.version)
		{
			fprintf(stderr, "Version mismatch in input files\r\n");
			exit(1);
		}
		if(wms_node_list[0].wms_header.lod_bias != 
			wms_node_list[i].wms_header.lod_bias)
		{
			fprintf(stderr, "LOD Bias mismatch in input files\r\n");
			exit(1);
		}
		if(wms_node_list[0].wms_header.filter_type != 
			wms_node_list[i].wms_header.filter_type)
		{
			fprintf(stderr, "Filter type mismatch in input files\r\n");
			exit(1);
		}
		if(wms_node_list[0].wms_header.trilinear != 
			wms_node_list[i].wms_header.trilinear)
		{
			fprintf(stderr, "Trilinear mode mismatch in input files\r\n");
			exit(1);
		}
	}

	// Fill in the info for the composite wms header to be put on the output
	// file.
	memcpy((void *)&composite_wms_header, (void *)&wms_node_list[0].wms_header, sizeof(wms_header_t));

	// Get the small LOD from the last header
	composite_wms_header.gu_header.small_lod = wms_node_list[wms_node_count-1].wms_header.gu_header.small_lod;

	// Open the output file
	if((out_fp = fopen(out_fname, "wb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open output file: %s\r\n", out_fname);
		exit(1);
	}

	// Write the composite wms header to the output file
	if((amount = fwrite(&composite_wms_header, 1, sizeof(wms_header_t), out_fp)) != sizeof(wms_header_t))
	{
		fclose(out_fp);
		fprintf(stderr, "Could not write header to file: %s\r\n", out_fname);
		unlink(out_fname);
		exit(1);
	}

	// Now we are all set to start reading the pixel data from each of
	// the input wms files and send it to the output wms file
	
	for(i = 0; i < wms_node_count; i++)
	{
		// Open the input file
		if((fp = fopen(wms_node_list[i].file_name, "rb")) == (FILE *)0)
		{
			fprintf(stderr, "Can not open file: %s\r\n", wms_node_list[i].file_name);
			fclose(out_fp);
			unlink(out_fname);
			exit(1);
		}

		// Seek past the WMS file header on the input file
		if(fseek(fp, sizeof(wms_header_t), SEEK_SET))
		{
			fprintf(stderr, "Could not perform file seek on file %s\r\n", wms_node_list[i].file_name);
			fclose(out_fp);
			unlink(out_fname);
			exit(1);
		}

		// Read all of the data from the input file and write it to the
		// output file
		while((c = fgetc(fp)) != EOF)
		{
			fputc(c, out_fp);
		}

		// Close the input file
		fclose(fp);

		// Flush the output file
		fflush(out_fp);
	}

	// Close the output file
	fclose(out_fp);

	// Tell what we did
	printf("Files:\n");
	for(i = 0; i < wms_node_count; i++)
	{
		printf("%s\n", wms_node_list[i].file_name);
	}
	printf("\nCombined into file: %s\n", out_fname);
	// Return success
	exit(0);
}

