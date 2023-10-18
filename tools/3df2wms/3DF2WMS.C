/* $Revision: 10 $ */

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<dir.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<zlib.h>
#include	<glide/glide.h>

#define	MAX_INPUT_FILES	2500

#undef	printf

typedef struct texture_hdr
{
	int					version;
	float					lod_bias;
	int					filter_type;
	int					trilinear;
	Gu3dfHeader			gu_header;
} texture_hdr_t;


static texture_hdr_t	thdr;
static float			lod_bias = 0.0f;
static int				type = GR_MIPMAP_NEAREST;
static int				trilinear = 0;
static FILE				*log = stderr;
static char				version_str[] = {"$Revision: 10 $"};
static int				wms_version;
static int				verbose = 0;
static int				compress_textures = 0;

static char	*usage_strs[] = {
"\nUSAGE: 3df2wms [-v] [-b val] [-f type] [-l file] [-t] <3df_files...>\n\n",
"-v         Print all sorts of status information\n",
"-b val     Set the LOD Bias to the floating point value specified by val\n",
"           If not specified the default is 0.0.\n",
"-f type    Set the mip map filter mode to the value specified by type\n",
"           Type can be GR_MIPMAP_DISABLE, GR_MIPMAP_NEAREST, or\n",
"           GR_MIPMAP_NEAREST_DITHER.  If not specified the default is\n",
"           GR_MIPMAP_NEAREST\n",
"-t         Texture is to be used for trilinear mip mapping\n",
"-l file    Set the log file to the name specified by file.  If not specified\n",
"           the default is stderr\n",
"-z         Create Compressed .wmz texture\n"
};

static char	*title_strs[] = {
"\n",
"3DF2WMS for DOS - $Revision: 10 $",
"\n",
"Copyright (c) 1997 by Midway Video Inc.",
"\n"
};


static void show_usage(void)
{
	unsigned int	i;

	for(i = 0; i < sizeof(usage_strs)/sizeof(void *); i++)
	{
		printf("%s", usage_strs[i]);
	}
}

static void show_title(void)
{
	unsigned int	i;

	if(!verbose)
	{
		return;
	}
	for(i = 0; i < sizeof(title_strs)/sizeof(void *); i++)
	{
		printf("%s", title_strs[i]);
	}
}


typedef struct
{
	const char			*name;
	GrTextureFormat_t	fmt;
	FxBool				valid;
} CfTableEntry;

static GrAspectRatio_t	wh_aspect_table[] = {
GR_ASPECT_1x1, GR_ASPECT_1x2, GR_ASPECT_1x4, GR_ASPECT_1x8
};

static GrAspectRatio_t	hw_aspect_table[] = {
GR_ASPECT_1x1, GR_ASPECT_2x1, GR_ASPECT_4x1, GR_ASPECT_8x1
};

static int get_info(int aspect_width, int aspect_height,	int lod_min, int lod_max, int format, Gu3dfHeader *Info)
{
	int	index;
	int	i;
	int	ratio_found = FXFALSE;
	int	format_found = FXFALSE;

	if(!lod_min)
	{
		lod_min = 256;
	}
	if(!lod_max)
	{
		lod_max = 256;
	}
	if(!aspect_width)
	{
		aspect_width = 256;
	}
	if(!aspect_height)
	{
		aspect_height = 256;
	}


	/*
	** determine aspect ratio, height, and width
	*/
	i = 0;
	ratio_found = FXFALSE;
	while((i < 4) && (!ratio_found))
	{
		if((aspect_width << i) == aspect_height)
		{
			Info->aspect_ratio = wh_aspect_table[i];
			ratio_found = FXTRUE;
		}
		i++;
	}
	i = 0;
	while((i < 4) && (!ratio_found))
	{
		if((aspect_height << i ) == aspect_width)
		{
			Info->aspect_ratio = hw_aspect_table[i];
			ratio_found = FXTRUE;
		}
		i++;
	}
	if(!ratio_found)
	{
		return(FXFALSE);
	}

	/*
	** determine height and width of the mip map
	*/
	if(aspect_width >= aspect_height)
	{
		Info->width  = lod_max;
		Info->height = lod_max / aspect_width;
	}
	else
	{
		Info->height = lod_max;
		Info->width  = lod_max / aspect_height;
	}


	/*
	** calculate proper LOD values
	*/
	switch(lod_min)
	{
		case 1:
		{
			Info->small_lod = GR_LOD_1;
			break;
		}
		case 2:
		{
			Info->small_lod = GR_LOD_2;
			break;
		}
		case 4:
		{
			Info->small_lod = GR_LOD_4;
			break;
		}
		case 8:
		{
			Info->small_lod = GR_LOD_8;
			break;
		}
		case 16:
		{
			Info->small_lod = GR_LOD_16;
			break;
		}
		case 32:
		{
			Info->small_lod = GR_LOD_32;
			break;
		}
		case 64:
		{
			Info->small_lod = GR_LOD_64;
			break;
		}
		case 128:
		{
			Info->small_lod = GR_LOD_128;
			break;
		}
		case 256:
		{
			Info->small_lod = GR_LOD_256;
			break;
		}
	}

	switch(lod_max)
	{
		case 1:
		{
			Info->large_lod = GR_LOD_1;
			break;
		}
		case 2:
		{
			Info->large_lod = GR_LOD_2;
			break;
		}
		case 4:
		{
			Info->large_lod = GR_LOD_4;
			break;
		}
		case 8:
		{
			Info->large_lod = GR_LOD_8;
			break;
		}
		case 16:
		{
			Info->large_lod = GR_LOD_16;
			break;
		}
		case 32:
		{
			Info->large_lod = GR_LOD_32;
			break;
		}
		case 64:
		{
			Info->large_lod = GR_LOD_64;
			break;
		}
		case 128:
		{
			Info->large_lod = GR_LOD_128;
			break;
		}
		case 256:
		{
			Info->large_lod = GR_LOD_256;
			break;
		}
	}
	Info->format = format;
	return(FXTRUE);
}



typedef struct cf_tab
{
	char	*name;
	int	fmt;
} cf_tab_t;

static cf_tab_t	cftable[] = {			      
{"I8", GR_TEXFMT_INTENSITY_8},					
{"A8", GR_TEXFMT_ALPHA_8},							
{"AI44", GR_TEXFMT_ALPHA_INTENSITY_44},		
{"YIQ", GR_TEXFMT_YIQ_422},						
{"RGB332", GR_TEXFMT_RGB_332},					
{"RGB565", GR_TEXFMT_RGB_565},					
{"ARGB8332", GR_TEXFMT_ARGB_8332},				
{"ARGB1555", GR_TEXFMT_ARGB_1555},				
{"AYIQ8422", GR_TEXFMT_AYIQ_8422},				
{"ARGB4444", GR_TEXFMT_ARGB_4444},				
{"AI88", GR_TEXFMT_ALPHA_INTENSITY_88},		
{"P8", GR_TEXFMT_P_8},  
{"AP88", GR_TEXFMT_AP_88},
{0, 0}			
};

static char	*filter_types[] = {
"GR_MIPMAP_DISABLE",
"GR_MIPMAP_NEAREST",
"GR_MIPMAP_NEAREST_DITHER"
};

static char	buffer[100];
static char	version[5];
static char	color_format[10];
static int	org_size;

static int create_new_header(char *fname)
{
	FILE	*in_fp;
	int	newlines = 0;
	int	j;
	int	lod_min, lod_max, aspect_width, aspect_height;
	int	header_size;
	int	format_found;
	int	format = 0;

	if((in_fp = fopen(fname, "rb")) == (FILE *)0)
	{
		fprintf(log, "\n\nCan not open file %s\r\n", fname);
		return(0);
	}
	j = 0;
	newlines = 0;
	while(newlines < 4)
	{
		buffer[j] = fgetc(in_fp);
		if(buffer[j] == '\n')
		{
			newlines++;
			buffer[j] = ' ';
		}
		j++;
	}
	buffer[j] = 0;
	header_size = strlen(buffer);
	if(sscanf(buffer, "3df v%s %s lod range: %i %i aspect ratio: %i %i\n", version, color_format, &lod_min, &lod_max, &aspect_width, &aspect_height) == 0)
	{
		fprintf(log, "Error decoding header for file: %s\r\n", fname);
		fclose(in_fp);
		return(0);
	}
	j = 0;
	while(color_format[j])
	{
		color_format[j] = toupper(color_format[j]);
		j++;
	}

	j = 0;
	format_found = 0;
	while(cftable[j].name && !format_found)
	{
		if(strcmp(color_format, cftable[j].name) == 0)
		{
			format_found = 1;
			format = cftable[j].fmt;
		}
		j++;
	}
	if(!format_found)
	{
		fprintf(log, "\nFile: %s - Color format not found\r\n", fname);
		fclose(in_fp);
		return(0);
	}
	fclose(in_fp);
	org_size = header_size;
	aspect_width &= 0xff;
	aspect_height &= 0xff;
	lod_min &= 0xff;
	lod_max &= 0xff;
	format &= 0xff;
	if(get_info(aspect_width, aspect_height, lod_min, lod_max, format, &thdr.gu_header) == FXFALSE)
	{
		fprintf(log, "\nFile: %s - Problem generating info header\r\n", fname);
		fclose(in_fp);
		return(0);
	}
	return(1);
}

static void swap_short_bytes(short *val)
{
	short	v1, v2;

	v1 = v2 = *val;
	v1 &= 0xff;
	v1 <<= 8;
	v2 >>= 8;
	v2 &= 0xff;
	v2 |= v1;
	*val = v2;
}

static int write_ncc_table(FILE *in, FILE *out)
{
	int				index;
	unsigned short	yRGB[16];
	unsigned short	iRGB[4][3];
	unsigned short	qRGB[4][3];
	FxU32				packed_data[12];
	FxU32				packedvalue;

	//
	// Read in Y
	//
	for(index = 0; index < 16; index++)
	{
		if(fread(&yRGB[index], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table Y values\r\n");
			return(0);
		}
		yRGB[index] >>= 8;
		yRGB[index] &= 0xff;
	}

	//
	// read in I
	//
	for(index = 0; index < 4; index++)
	{
		if(fread(&iRGB[index][0], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table I values\r\n");
			return(0);
		}
		if(fread(&iRGB[index][1], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table I values\r\n");
			return(0);
		}
		if(fread(&iRGB[index][2], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table I values\r\n");
			return(0);
		}
		swap_short_bytes(&iRGB[index][0]);
		swap_short_bytes(&iRGB[index][1]);
		swap_short_bytes(&iRGB[index][2]);
		iRGB[index][0] &= 0x1ff;
		iRGB[index][1] &= 0x1ff;
		iRGB[index][2] &= 0x1ff;
  	}

	//
	// read in Q
	//
	for(index = 0; index < 4; index++)
	{
		if(fread(&qRGB[index][0], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table Q values\r\n");
			return(0);
		}
		if(fread(&qRGB[index][1], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table Q values\r\n");
			return(0);
		}
		if(fread(&qRGB[index][2], sizeof(short), 1, in) != 1)
		{
			fprintf(log, "Problem reading NCC table Q values\r\n");
			return(0);
		}
		swap_short_bytes(&qRGB[index][0]);
		swap_short_bytes(&qRGB[index][1]);
		swap_short_bytes(&qRGB[index][2]);
		qRGB[index][0] &= 0x1ff;
		qRGB[index][1] &= 0x1ff;
		qRGB[index][2] &= 0x1ff;
  	}

  	//
  	// pack the table Y entries
  	//
  	for(index = 0; index < 4; index++)
  	{

  		packedvalue  = ((FxU32)yRGB[index*4+0]);
  		packedvalue |= ((FxU32)yRGB[index*4+1]) << 8;
  		packedvalue |= ((FxU32)yRGB[index*4+2]) << 16;
  		packedvalue |= ((FxU32)yRGB[index*4+3]) << 24;
                                    
  		packed_data[index] = packedvalue;
  	}

  	//
  	// pack the table I entries
  	//
  	for(index = 0; index < 4; index++)
  	{
  		packedvalue  = ((FxU32)iRGB[index][0]) << 18;
  		packedvalue |= ((FxU32)iRGB[index][1]) << 9;
  		packedvalue |= ((FxU32)iRGB[index][2]) << 0;

  		packed_data[index+4] = packedvalue;
  	}

  	//
  	// pack the table Q entries
  	//
  	for(index = 0; index < 4; index++)
  	{
  		packedvalue  = ((FxU32)qRGB[index][0]) << 18;
  		packedvalue |= ((FxU32)qRGB[index][1]) << 9;;
  		packedvalue |= ((FxU32)qRGB[index][2]) << 0;

  		packed_data[index+8] = packedvalue;
  	}

	//
	// Write the table to the file
	//
	return(fwrite(packed_data, sizeof(packed_data), 1, out));
}

static int write_wms_file(FILE *fp, char *ifname)
{
	int		c;
	int		c1;
	FILE		*in_fp;
	unsigned int	palette[256];

	// Show the file we are processing
	if(verbose)
	{
		printf("File: %-60.60s\r", ifname);
		fflush(stdout);
	}

	// Open it up
	if((in_fp = fopen(ifname, "rb")) == (FILE *)0)
	{
		fprintf(log, "\n\nCan not open file %s\r\n", ifname);
		return(0);
	}

	// Read past the original file header
	for(c = 0; c < org_size; c++)
	{
		fgetc(in_fp);
	}

	// Write the new file header
	if(fwrite(&thdr, sizeof(texture_hdr_t), 1, fp) != 1)
	{
		fprintf(log, "Problem writting header\r\n");
		fclose(in_fp);
		return(0);
	}

	// Is texture 8 bit format ?
	if(thdr.gu_header.format == GR_TEXFMT_INTENSITY_8 || thdr.gu_header.format == GR_TEXFMT_ALPHA_8 || thdr.gu_header.format == GR_TEXFMT_ALPHA_INTENSITY_44 || thdr.gu_header.format == GR_TEXFMT_YIQ_422 || thdr.gu_header.format == GR_TEXFMT_RGB_332 || \
		thdr.gu_header.format == GR_TEXFMT_P_8)
	{
		// YES - Is it palettized ?
		if(thdr.gu_header.format == GR_TEXFMT_P_8)
		{
			// Read the palette data
			if(fread(palette, sizeof(int), sizeof(palette)/sizeof(int), in_fp) != sizeof(palette)/sizeof(int))
			{
				fprintf(log, "Problem reading palette data from: %s\r\n", ifname);
				fclose(in_fp);
				return(0);
			}

			// Write the palette data
			if(fwrite(palette, sizeof(int), sizeof(palette)/sizeof(int), fp) != sizeof(palette)/sizeof(int))
			{
				fprintf(log, "Problem writing palette data\r\n");
				fclose(in_fp);
				return(0);
			}
		}

		// Is the texture and NCC texture ?
		else if(thdr.gu_header.format == GR_TEXFMT_YIQ_422 ||
			thdr.gu_header.format == GR_TEXFMT_AYIQ_8422)
		{
			// Yes - write the ncc table
			if(!write_ncc_table(in_fp, fp))
			{
				fprintf(log, "Problem writting NCC table\r\n");
				fclose(in_fp);
				return(0);
			}
		}

		// Read the pixel data from the source and write to the destination
		while((c = fgetc(in_fp)) != EOF)
		{
			if(fputc(c, fp) == EOF)
			{
				fprintf(log, "Problem writing pixel data\r\n");
				fclose(in_fp);
				return(0);
			}
		}
	}

	// 16 Bit textures
	else
	{
		// Read the pixel data from the source and write it to the destination
		while((c = fgetc(in_fp)) != EOF)
		{
			c1 = fgetc(in_fp);
			if(fputc(c1, fp) == EOF)
			{
				fprintf(log, "Problem writing pixel data\r\n");
				fclose(in_fp);
				return(0);
			}
			if(fputc(c, fp) == EOF)
			{
				fprintf(log, "Problem writing pixel data\r\n");
				fclose(in_fp);
				return(0);
			}
		}
	}

	// Close the input file
	fclose(in_fp);

	// Return OK
	return(1);
}

char	*in_filenames[MAX_INPUT_FILES];
char	out_filename[256];

void main(int argc, char *argv[])
{
	FILE			*out_fp;
	struct stat	stat_buf;
	char			*tmp;
	int			input_files_found = 0;
	int			i;
	int			j;
	int			error;

	// Any arguments ?
	if(argc < 1)
	{
		show_usage();
		exit(1);
	}

	// Scan the command line for arguments
	for(i = 1; i < argc; i++)
	{
		switch(argv[i][0])
		{
			// Option introducer
			case '-':
			case '/':
			{
				switch(toupper(argv[i][1]))
				{
					case 'V':		// Be Verbose
					{
						verbose = 1;
						break;
					}
					case 'B':		// LOD Bias
					{
						i++;
						if(!sscanf(argv[i], "%f", &lod_bias))
						{
							fprintf(stderr, "Bad value for lod bias: %s\r\n", argv[i]);
							if(log != stderr)
							{
								fflush(log);
								fclose(log);
							}
							exit(1);
						}
						if(lod_bias < -7.0f || lod_bias > 7.0f)
						{
							fprintf(stderr, "Lod Bias must be between -7.0 and +7.0\r\n");
							if(log != stderr)
							{
								fflush(log);
								fclose(log);
							}
							exit(1);
						}
						break;
					}
					case 'F':		// Filter type
					{
						i++;
						strupr(argv[i]);
						for(j = 0; j < sizeof(filter_types)/sizeof(void *); j++)
						{
							if(!strcmp(argv[i], filter_types[j]))
							{
								break;
							}
						}
						if(j >= sizeof(filter_types)/sizeof(void *))
						{
							fprintf(stderr, "Unrecognized filter type: %s\r\n", argv[i]);
							if(log != stderr)
							{
								fflush(log);
								fclose(log);
							}
							exit(1);
						}
						type = j;
						break;
					}
					case 'L':		// Log file name
					{
						i++;
						if((log = fopen(argv[i], "wb")) == (FILE *)0)
						{
							fprintf(stderr, "Can not open log file: %s\r\n", argv[i]);
							log = stderr;
						}
						break;
					}
					case 'T':		// Enable trilinear mode
					{
						trilinear = 1;
						break;
					}
					case 'Z':		// Compress textures
					{
						compress_textures = 1;
						break;
					}
					default:			// Unrecognized option
					{
						fprintf(stderr, "ERROR - Unregonized command line option: %s\r\n", argv[i]);
						if(log != stderr)
						{
							fflush(log);
							fclose(log);
						}
						exit(1);
					}
				}
				break;
			}
			// Otherwise must be a file name
			default:
			{
				in_filenames[input_files_found] = argv[i];
				input_files_found++;
				break;
			}
		}
	}

	// Did we find and input file names on the command line ?
	if(!input_files_found)
	{
		// NOPE - Show the dummy how to use the command and exit with error
		show_usage();
		if(log != stderr)
		{
			fflush(log);
			fclose(log);
		}
		exit(1);
	}

	// Show the pretty banner string
	show_title();

	// Start on a new line
	if(verbose || log != stderr)
	{
		fprintf(log, "\r\n");
	}

	// Get the version of 3df2wms being used to generate the wms files
//	sscanf(version_str, "$Revision: 10 $", &wms_version);
	wms_version = 5;

	// Print the lod, format, and tri mode we are using for the conversions
	if(verbose || log != stderr)
	{
		fprintf(log, "Version:            %d\r\n", wms_version);
		fprintf(log, "LOD Bias set to:    %3.1f\r\n", lod_bias);
		fprintf(log, "Filter type set to: %s\r\n", filter_types[type]);
		fprintf(log, "Trilinear mode is:  ");
		if(trilinear)
		{
			fprintf(log, "ENABLED");
		}
		else
		{
			fprintf(log, "DISABLED");
		}
		fprintf(log, "\r\n");
	}

	// Set the mode information for the wms style textures
	// I or in a bit onto the version number so it won't be confused with
	// the first field of the old wms file format.
	thdr.version = (wms_version | 0x8000);
	thdr.lod_bias = lod_bias;
	thdr.filter_type = type;
	thdr.trilinear = trilinear;

	// No errors found yet
	error = 0;

	// Loop and convert all files specified
	for(i = 0; i < input_files_found; i++)
	{
		// Get the stats for an input file
		if((stat(in_filenames[i], &stat_buf)))
		{
			fprintf(log, "Can not get stats for file: %s\r\n", in_filenames[i]);
			error++;
			continue;
		}

		// Is the file size equal to 0
		if(stat_buf.st_size == 0)
		{
			// YES - Then texus failed and we can not convert the file
			fprintf(log, "ERROR - File %s is empty\r\n", in_filenames[i]);
			error++;
			continue;
		}

		// Copy the input filename to the output filename
		strcpy(out_filename, in_filenames[i]);

		// Check to see that the input file name is .3df
		if(!strstr(out_filename, ".3df") && !strstr(out_filename, ".3DF"))
		{
			fprintf(log, "ERROR - File %s is not a 3df file\r\n", in_filenames[i]);
			error++;
			continue;
		}

		// Change the extension on the output filename to .wms
		if((tmp = strstr(out_filename, ".3df")) != (char *)0)
		{
			strcpy(tmp, ".wms");
		}
		else if((tmp = strstr(out_filename, ".3DF")) != (char *)0)
		{
			strcpy(tmp, ".WMS");
		}
		else
		{
			fprintf(log, "ERROR - Could not find extension on file %s\r\n", in_filenames[i]);
			error++;
			continue;
		}

		// Open what will become the output file (WMS)
		if((out_fp = fopen(out_filename, "wb")) == (FILE *)0)
		{
			fprintf(log, "ERROR - Can not open file %s\r\n", out_filename);
			error++;
			continue;
		}

		// Create the header for the output file (WMS)
		if(!create_new_header(in_filenames[i]))
		{
			fprintf(log, "ERROR - Creating new headers\r\n\r\n");
			fclose(out_fp);
			unlink(out_filename);
			error++;
			continue;
		}

		// Write the output file (WMS)
		if(write_wms_file(out_fp, in_filenames[i]))
		{
			fflush(out_fp);
			fclose(out_fp);
			if(compress_textures)
			{
				if(!do_compress_texture(out_filename))
				{
					fprintf(log, "ERROR - %s NOT converted\r\n", in_filenames[i]);
					error++;
				}
				else if(verbose || log != stderr)
				{
					fprintf(log, "%s converted\r\n", in_filenames[i]);
				}
//				unlink(out_filename);
			}
			else if(verbose || log != stderr)
			{
				fprintf(log, "%s converted\r\n", in_filenames[i]);
			}
		}
		else
		{
			fclose(out_fp);
			unlink(out_filename);
			fprintf(log, "ERROR - %s NOT converted\r\n", in_filenames[i]);
			error++;
		}

	}

	// Print summary
	if(verbose || log != stderr)
	{
		fprintf(log, "%d of %d files converted\r\n", input_files_found - error, input_files_found);
	}

	// If log is to file - flush and close it
	if(log != stderr)
	{
		fprintf(stderr, "\r\n%d of %d files converted\r\n", input_files_found - error, input_files_found);
		fflush(log);
		fclose(log);
	}

	// Exit with error code
	exit((error>0?1:0));
}


int do_compress_texture(char *wms_fname)
{
	char				wmz_fname[256];
	struct ffblk	ffblk;
	unsigned long	length;
	FILE				*in_fp;
	FILE				*out_fp;
	char				*in_buffer;
	char				*out_buffer;
	char				*tmp;
	int				status;

	strcpy(wmz_fname, wms_fname);
	tmp = strstr(wmz_fname, ".wms");
	if(!tmp)
	{
		tmp = strstr(wmz_fname, ".WMS");
	}
	if(!tmp)
	{
		return(0);
	}
	*tmp = 0;
	strcat(wmz_fname, ".wmz");

	if(!findfirst(wms_fname, &ffblk, 0))
	{
		if((in_buffer = (char *)malloc(ffblk.ff_fsize)) == NULL)
		{
			return(0);
		}

		if((out_buffer = (char *)malloc(ffblk.ff_fsize * 2)) == NULL)
		{
			free(in_buffer);
			return(0);
		}

		if((in_fp = fopen(wms_fname, "rb")) == (FILE *)0)
		{
			free(in_buffer);
			free(out_buffer);
			return(0);
		}

		if((out_fp = fopen(wmz_fname, "wb")) == (FILE *)0)
		{
			fclose(in_fp);
			free(in_buffer);
			free(out_buffer);
			return(0);
		}

		if(fread(in_buffer, sizeof(char), ffblk.ff_fsize, in_fp) != ffblk.ff_fsize)
		{
			fclose(in_fp);
			fclose(out_fp);
			free(in_buffer);
			free(out_buffer);
			unlink(wmz_fname);
			return(0);
		}

		fclose(in_fp);

		length = ffblk.ff_fsize * 2;
		status = compress(out_buffer, &length, in_buffer, ffblk.ff_fsize);

		free(in_buffer);

		if(status != Z_OK)
		{
			fclose(out_fp);
			free(out_buffer);
			unlink(wmz_fname);
			return(0);
		}

		if(fwrite(&length, sizeof(int), 1, out_fp) != 1)
		{
			fclose(out_fp);
			free(out_buffer);
			unlink(wmz_fname);
			return(0);
		}

		if(fwrite(out_buffer, sizeof(char), length, out_fp) != length)
		{
			fclose(out_fp);
			free(out_buffer);
			unlink(wmz_fname);
			return(0);
		}

		fclose(out_fp);
		free(out_buffer);

		unlink(wms_fname);
		rename(wmz_fname, wms_fname);

		return(1);

	}
	return(0);
}
