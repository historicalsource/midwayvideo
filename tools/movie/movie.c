#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dos.h>
#include	<time.h>
#include <direct.h>
#include <unistd.h>
#include <errno.h>
#include	"/video/tools/putil/lib/pd.h"

#define	true 1;
#define	false	0;
#define	frame_without_buffer 194*512;	//194 blocks of 512 bytes (192 blocks of pixel data, 2 block of pallete)
#define	frame_with_buffer		386*512;	//386 blocks of 512 bytes (192 blocks of pixel data, 2 block of pallete, 192 blocks of z buffer data)

enum
	{
	set_z, 
	set_size,
	set_partition,
	set_blockoffset,
	set_write2dest,
	set_nooutfile,
	set_help
	};

typedef struct
	{
	char	token_char[5];
	int	token_num;
	}token_data;

token_data cmdtokens [8]=
	{
	{"-z", set_z},
	{"-s", set_size},
	{"-p", set_partition},
	{"-b", set_blockoffset},
	{"-w", set_write2dest},
	{"-n", set_nooutfile},
	{"-h", set_help},
	{"", -1}
	};

clock_t
	sumtime,										// Ha!
	starttime,
	endtime;

typedef struct
	{
	int	space_free;
	int	space_required;
	int	total_zblock_count;
	int	total_pixel_block_count;
	int	total_file_block_count;
	}space_data;

struct file_data 
	{
   char  file_input;
	};

typedef struct 
	{
	long unsigned	total_frame_count;
	long unsigned	partition;
	long unsigned	block_offset;
	long unsigned	zbuf;
	long unsigned	size;
	}data_out;

	char	*equate_ram;
	char	*equate_ram_start;
	char	*end_of_equate_ram_file;
	char	*zequate_ram;
	char	*zequate_ram_start;
	char	*zend_of_equate_ram_file;
	void	*x3df_ram;
	void	*pal_ram;
	int	z_buffer = 0;
	long	partition = 2;
	int	num_of_colours;
	int	size =0;
	int	pixel_block_size;
	int	zbuff_block_size;
	int	width;
	int	height;
	long	block_offset = 0;
	int	write2dest = 0;
	int	no_out_file = 0;
	char	*temp_pixel_space;
	char	*temp_zbuf_space;
	data_out	count_data;


/* find how long this file we opened is... */
long int filesize( FILE *fp )
  	{
    long int save_pos, size_of_file;

    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
  	}

/* Read in this file char */
size_t read_data( FILE *fp, struct file_data *p )
  	{
    return( fread( p, sizeof(char), 1, fp ) );
  	}

//write out frame count details
void write_out_count(FILE *fw, long* write_space, char* file_string)
	{
	char	filename[100];
	sprintf (filename, "%s.cnt",file_string); 
	if (( fwrite( write_space, sizeof(count_data), 1, fw)) == NULL)
		{
		// we couldn't write this data, alert the user
			printf ("Problem writing count data to file %s\n",filename);
		exit(0);
		}
	}

// organise the pixel data into temp memory
void organise_pixels(void)
	{
	char *temp_addr;
	void	*a3df_ram;
	int	i;

	if	(size == 0)
		temp_pixel_space = malloc((384*256) + (512)); 
	else
		temp_pixel_space = malloc((512*384) + (512)); 

	temp_addr = temp_pixel_space; 

	// do palette
	memcpy(temp_addr, pal_ram, (256*2));
	temp_addr += (256*2);

	a3df_ram = x3df_ram;

	if (size == 0)
		{
		// normal size, do left 256x256, then right 128x256
		for (i=0;i<256 ;i++)
			{
			memcpy(temp_addr, a3df_ram, 256);
			temp_addr += 256;
			a3df_ram =a3df_ram + 384; 
			}
		a3df_ram = x3df_ram+ 256;
		for (i=0;i<256 ;i++)
			{
			memcpy(temp_addr, a3df_ram, 128);
			temp_addr += 128;
			a3df_ram =a3df_ram + 384; 
			}
		}
	else
		{
		// big size, do top left 256x256, then top right 256x256, then bot left 256x128 and then bot right 256x128 
		for (i=0;i<256 ;i++)
			{
			memcpy(temp_addr, a3df_ram, 256);
			temp_addr += 256;
			a3df_ram =a3df_ram + 512; 
			}
		a3df_ram = x3df_ram+ 256;
		for (i=0;i<256 ;i++)
			{
			memcpy(temp_addr, a3df_ram, 256);
			temp_addr += 256;
			a3df_ram =a3df_ram + 512; 
			}
		a3df_ram = x3df_ram+ 512*256;
		for (i=0;i<128 ;i++)
			{
			memcpy(temp_addr, a3df_ram, 256);
			temp_addr += 256;
			a3df_ram =a3df_ram + 512; 
			}
		a3df_ram = x3df_ram+ 512*256 + 256;
		for (i=0;i<128 ;i++)
			{
			memcpy(temp_addr, a3df_ram, 256);
			temp_addr += 256;
			a3df_ram =a3df_ram + 512; 
			}
		}
	}

//write out organised pixel/pallette data to the output file
void write_pblocks_out_to_targ(void)
	{
	int	i;
	int	size_to_write;
	char	*ram_start;

	if (size == 0)
		size_to_write = 193;
	else
		size_to_write = 385;

	ram_start = temp_pixel_space;

	for (i=0;i<size_to_write ;i++ )
		{
		write_sectors(partition ,ram_start, block_offset++, 1);
		ram_start += 512;
		}
	}

// write out organised pixel/pallette details to the output file
void write_out_details(FILE *fw, char* file_string)
	{
	char	filename[100];
	long	temp_size;

	sprintf (filename, "%s.mov",file_string); 

	if (size == 0)
		temp_size = ((256*2) + (384*256));
	else
		temp_size = ((256*2) + (512*384));

	if (( fwrite(temp_pixel_space ,temp_size , 1, fw)) == NULL)
		{
		// we couldn't write this data, alert the user
		printf ("Problem writing palette data to file %s\n",filename);
		exit(0);
		}
	}

// convert the TGA file we input into a P8 type format texus style data structure.
void texus_convert (char* equate_ram_start,int frame_number, char *file_string)
	{
	int 	i;
	short	*pal_put;
	char	*pal_get;
	short	temp_col_val;
	short	temp_var;
	short	*temp_var_ptr;
	char	*temp_var_2;
	char	num_of_color_bits;
	char	filename[100];
	int	bytes_of_color;

	sprintf (filename, "%s%.4d.tga",file_string, frame_number); 

	temp_var_ptr = (short*)(equate_ram_start +5);
	num_of_colours = *temp_var_ptr;

	temp_var_2 = (char*)(equate_ram_start +7);
	num_of_color_bits = *temp_var_2;

	temp_var_ptr = (short *)(equate_ram_start + 12);
	temp_var = *temp_var_ptr;
	if (temp_var != width)
		{
		printf("\n\nERROR !!!!");
		printf("\nFile %s has x value of %d\n",filename,temp_var);
		exit(0);
		}

	temp_var_ptr = (short *)(equate_ram_start + 14);
	temp_var = *temp_var_ptr;
	if (temp_var != height)
		{
		printf("\n\nERROR !!!!");
		printf("\nFile %s has y value of %d\n",filename,temp_var);
		exit(0);
		}

	pal_ram = malloc(256 *2);
	pal_put = pal_ram;
	pal_get = equate_ram_start + 18;
	if (num_of_color_bits == 24)
		{
		bytes_of_color = 3;
		for (i=0;i<num_of_colours ;i++ )
			{
			temp_col_val = ((*pal_get &0xf8) >> 3) &0x1f;
			pal_get++;
			temp_col_val |= ((*pal_get << 2 ) &0x3e0);
			pal_get++;
			temp_col_val |= ((*pal_get << 7 ) &0x7c00);
			pal_get++;
			temp_col_val |= 0x8000;
			*pal_put = temp_col_val;
			pal_put++;
			}
		}
	else
	if (num_of_color_bits == 16)
		{
		bytes_of_color = 2;
		for (i=0;i<num_of_colours ;i++ )
			{
			temp_col_val = (*pal_get &0x1f);
			temp_col_val |= ((*pal_get >> 1) &0x60);
			pal_get++;
			temp_col_val |= ((*pal_get << 7 ) &0x7f80);
			pal_get++;
			temp_col_val |= 0x8000;
			*pal_put = temp_col_val;
			pal_put++;
			}
		}
	else
	if (num_of_color_bits == 15)
		{
		bytes_of_color = 2;
		for (i=0;i<num_of_colours ;i++ )
			{
			temp_col_val = (*pal_get &0xff);
			pal_get++;
			temp_col_val |= ((*pal_get << 8 ) &0xff00);
			pal_get++;
			temp_col_val |= 0x8000;
			*pal_put = temp_col_val;
			pal_put++;
			}
		}


	x3df_ram = equate_ram_start + 18 + (num_of_colours * bytes_of_color);
	}

// read in the a particular 3df file into memory
void read_in_TGA_frame(int frame_number, char* file_string, char* instring)
	{

	char	in_wave_inc_file[100];
	char	filename[100];
   struct file_data std;
	
	FILE	*fr;

	sprintf (filename, "%s%.4d.tga",file_string, frame_number); 
	sprintf (in_wave_inc_file,"%s\\%s",instring,filename);
	//	read in the wave.inc file
  	if  ((fr = fopen(in_wave_inc_file , "rb" )) != NULL) 
		{
		// allocate memory for the ram array that the file input goes into
		equate_ram = (char *)malloc( filesize( fr) );

		// preserve the ram array start position for later use
		equate_ram_start = (char *) equate_ram;

		// if the file we opened is okay...
	   if( fr != NULL ) 
			{
			// read in the data into the ram array 
	     	while( read_data( fr, &std ) != 0 ) 
				{
				*equate_ram++ = std.file_input;
	        	}
			}
		else
			{
			// file didn't open okay...
			printf ("\nERROR ! Can't read file %s.\n",filename);
			exit(0);
	     	}

		// make sure theres a string terminator at the end of this file.
		*equate_ram++ = '\0';

		// preserve the end of ram array position
		end_of_equate_ram_file = equate_ram;

		// reset us back to start of array
		equate_ram = equate_ram_start;
		}
	 else
	 	{
	 	//couldn't open that output file
	 	printf ("\nERROR !!!!! on open file %s for read\n",filename);
		exit(0);
	 	}

	fclose( fr );
	}

void organise_zbuffer(void)
	{
	char  *temp_addr;
	char	*zbuf_ram;
	int	i;

	if	(size == 0)
		temp_zbuf_space = malloc(384*256); 
	else
		temp_zbuf_space = malloc(512*384); 

	temp_addr = temp_zbuf_space;

	zbuf_ram = zequate_ram;
	if (size ==0)
		{
		// normal size, do left 256x128, then right 128x128
		for (i=0;i<128 ;i++ )
			{
			memcpy(temp_addr,zbuf_ram,512);
			temp_addr +=512;
			zbuf_ram += (((256*2) + (128*2)) * 2);	// every other line
			}

		zbuf_ram = zequate_ram + (256*2);
		for (i=0;i<128 ;i++ )
			{
			memcpy(temp_addr,zbuf_ram,256);
			temp_addr +=256;
			zbuf_ram += (((256*2) + (128*2)) * 2);	// every other line
			}
		}
	else
		{
		// big size, do top left 256x128, then top right 256x128, then bot left 256x64 and then bot right 256x64  
		for (i=0;i<128 ;i++ )
			{
			memcpy(temp_addr,zbuf_ram,512);
			temp_addr +=512;
			zbuf_ram += (((256*2) + (256*2)) * 2);	// every other line
			}
		zbuf_ram = zequate_ram + (256*2);
		for (i=0;i<128 ;i++ )
			{
			memcpy(temp_addr,zbuf_ram,512);
			temp_addr +=512;
			zbuf_ram += (((256*2) + (256*2)) * 2);	// every other line
			}
		zbuf_ram = zequate_ram + ((512*2)*256);
		for (i=0;i<64 ;i++ )
			{
			memcpy(temp_addr,zbuf_ram,512);
			temp_addr +=512;
			zbuf_ram += (((256*2) + (256*2)) * 2);	// every other line
			}
		zbuf_ram = zequate_ram + ((512*2)*256) +(256*2);
		for (i=0;i<64 ;i++ )
			{
			memcpy(temp_addr,zbuf_ram,512);
			temp_addr +=512;
			zbuf_ram += (((256*2) + (256*2)) * 2);	// every other line
			}
		}
	}

//	write out organised z buffer data directly to the target system
void write_zblocks_out_to_targ(void)
	{
	int	i;
	int	size_to_write;
	char	*ram_start;

	if (size == 0)
		size_to_write = 192;
	else
		size_to_write = 384;

	ram_start = temp_zbuf_space; 

	for (i=0;i<size_to_write ;i++ )
		{
		write_sectors(partition ,ram_start, block_offset++, 1);
		ram_start += 512;
		}

	}

// write out organised z buffer data to the output movie file
void write_out_zbuf(FILE *fw, char* file_string)
	{
	char	filename[100];
	long	temp_size;

	sprintf (filename, "%s.mov",file_string); 

	if (size == 0)
		temp_size = 384*256;
	else
		temp_size = 512*384;

	if (( fwrite( temp_zbuf_space, temp_size, 1, fw)) == NULL)
		{
		// we couldn't write this data, alert the user
		printf ("Problem writing zbuf data to file %s\n",filename);
		exit(0);
		}
	}

// read in the a particular zbuf file into memory
void read_in_Zbuf_frame(int frame_number, char* file_string, char* instring)
	{

	char	in_wave_inc_file[100];
	char	filename[100];
   struct file_data std;
	
	FILE	*fz;

	sprintf (filename, "%s%.4d.zb",file_string, frame_number); 
	sprintf (in_wave_inc_file,"%s\\%s",instring,filename);
	//	read in the wave.inc file
  	if  ((fz = fopen(in_wave_inc_file , "rb" )) != NULL) 
		{
		// allocate memory for the ram array that the file input goes into
		zequate_ram_start = (char *)malloc( filesize( fz) );

		// preserve the ram array start position for later use
		zequate_ram = zequate_ram_start;

		// if the file we opened is okay...
	   if( fz != NULL ) 
			{
			// read in the data into the ram array 
	     	while( read_data( fz, &std ) != 0 ) 
				{
				*zequate_ram++ = std.file_input;
	        	}
			}
		else
			{
			// file didn't open okay...
			printf ("\nERROR ! Can't read file %s.\n",filename);
			exit(0);
	     	}

		// preserve the end of ram array position
		zend_of_equate_ram_file = zequate_ram;

		// reset us back to start of array
		zequate_ram = zequate_ram_start + 64;
		}
	 else
	 	{
	 	//couldn't open that output file
	 	printf ("\nERROR !!!!! on open file %s for read\n",filename);
		exit(0);
	 	}

	fclose( fz );
	}

// display the execution time of this program
void display_execution_time( void)
	{
	//
	//	Display execution time.
	//
//	Lah!!
#define	SECONDS_PER_MINUTE		60
#define	MINUTE_PER_HOUR			60

	char	generalstring2[100];
	char	generalstring3[100];

	endtime = clock();
	sumtime = (endtime-starttime);
	sumtime = sumtime / CLOCKS_PER_SEC;
	starttime = sumtime / SECONDS_PER_MINUTE;
	sumtime = sumtime - ( starttime * SECONDS_PER_MINUTE );

	if( starttime )
		{
		sprintf(generalstring2, "%d", starttime);
		}
	else
		{
		sprintf(generalstring2, "0");
		}

	if( sumtime )
		{
		sprintf(generalstring3, " %d", sumtime);
		}
	else
		{
		sprintf(generalstring3, "0");
		}

	printf("\nExecution time is %s mins, %s secs.\n", generalstring2, generalstring3);
	}


//work out how much space we will require to store this file in its entirety
space_data space_necessary(space_data space_vals, int start_offset, char* file_string, char* envstring, int number_of_frames)
	{
	space_vals.total_zblock_count = 0;
	if (z_buffer)
		{
		space_vals.total_zblock_count = zbuff_block_size * number_of_frames;
		}
	space_vals.total_pixel_block_count = pixel_block_size * number_of_frames;
	space_vals.total_file_block_count = space_vals.total_pixel_block_count + space_vals.total_zblock_count;
	space_vals.space_required = space_vals.total_file_block_count * 512;
	return(space_vals);
	}

// put up usage message
void do_usage(void)
	{
 	printf ("\nMOVIE requires three parameters.\nE.g. MOVIE [num of frames] [filename] [options]\n"
			  "Where	No of Frames is the number of frames in the movie\n"
			  "Where	Filename is the file name of the first frame of the TGA movie\n"
			  "(missing the .TGA extension, in 4 char, 4 numbers format)\n\n"
			  "Where Options are :\n"
			  "-z : Z buffer is to be embedded in output file\n"
			  "     Z-Buffer format is Light Wave zbuffer format, filename same as tga file\n"
			  "     name, with .zb extension.\n"
			  "     Default - no Z buffer\n"
			  "-p : Partition selection. Followed by a numeric to determine which\n"
			  "     partition the movie will ultimately end up on\n"
			  "     Default - Partition 2\n"
			  "-s : Size selection. Followed by a numeric to determine which size to use.\n"
			  "     0 = 384x256, 1=512x384\n"
			  "     Default - 384 x 256 by 256 color mapped TGA images\n"
			  "-n : Do not write out Output file to PC hard disk.\n"
			  "     Default YES.\n"
			  "-b : Set block offset on dest disk. Followed by numeric that specifies offset\n"
			  "     from start of partition for begining of Movie data file.\n"
			  "     Default 0\n"
			  "-w : Write to Dest disk. If this option is specified, the movie file\n"
			  "     will get written directly to the target pheonix system hard drive\n"
			  "     using the Partition and Block offset settings.\n"
			  "-h : Show this help screen\n"
			  "\nExample : Movie 200 blah0001 -z -s 1 -p 3 -b 1000 -w<cr>\n"
			  "will condense a 200 frame 512x384 movie, starting with frame blah0001.tga,\n"
			  "include a Z-buffer for each frame, and will physically write it to the target\n"
			  "hard drive starting it at block 1000 of partition 3, as well as creating a\n"
			  "copy on the host hard drive.\n"
			  "\nMOVIE output is placed in the MOVIEOUT environment path variable.\n"
			  "MOVIE count files are placed in the MOVIECNT environment path variable.\n"
			  "Incoming TGA are found using the MOVIEIN environment path variable\n"
			  "\nNOTE ! This program is only designed to work on 384x256 or 512x384 256 color\nmapped TGA type images.\n"
			  );
	exit (0);
	}

// do a comparison of each command against possible commands
int check_against_commands(char* command_string)
	{
	int i=0;

	while (cmdtokens[i].token_num != -1)
		{
		if (strcmp(command_string,cmdtokens[i].token_char) == 0)
			{
			return(cmdtokens[i].token_num);
			}
		i++;
		}

	return(-1);
	}

// parse the commands
void do_commands(int argc, char **argv)
	{
	int	token;
	int	i;

	for (i=3;i<argc ;i++ )
		{
		token = check_against_commands(argv[i]);
		switch(token)
			{

			case set_z:
				z_buffer = true;
				break;

			case set_size:
				if (i+1 >= argc)
					{
					printf ("\n!!!! Error !!!! Invalid Option !!!\nMissing size data for -s option.\n");
					exit (0);
					}
				size = atoi(argv[i+1]);
				i++;
				break;

			case set_partition:
				if (i+1 >= argc)
					{
					printf ("\n!!!! Error !!!! Invalid Option !!!\nMissing partition data for -p option.\n");
					exit (0);
					}
				partition = atoi(argv[i+1]);
				i++;
				break;

			case set_nooutfile:
				no_out_file = 1;
				break;

			case set_write2dest:
				write2dest = 1;
				break;

			case set_blockoffset:
				if (i+1 >= argc)
					{
					printf ("\n!!!! Error !!!! Invalid Option !!!\nMissing block start offset for -b option.\n");
					exit (0);
					}
				block_offset = atoi(argv[i+1]);
				i++;
				break;

			case set_help:
				do_usage();
				break;

			default:
				printf ("\n!!!! Error !!!! Invalid Option !!! '%s'\n",argv[i]);
				do_usage();
				break;
			}
		}
	}

// set up the frame sizes
void set_sizes(void)
	{
	if (size ==0)
		{
		pixel_block_size = 193;
		zbuff_block_size = 192;
		width = 384;
		height = 256;
		}
	else
		{
		pixel_block_size = 385;
		zbuff_block_size = 384;
		width = 512;
		height = 384;
		}
	}


// okay, lets do the program //
void main( int argc, char **argv )
	{

   FILE  *fw;
	FILE	*fc;

	int	number_of_frames;
	int	i;
	int	start_offset;
	int	result;
	int	temp_time;
	float	flo_space_required;
	char	file_string[100];
	char	out_movie_file[100];
	char	out_movie_file2[100];
	char	out_movie_file3[100];
	char	out_movie_file4[100];
	char	out_movie_filename[100];
	char	out_movie_filename2[100];
	char	out_movie_filename3[100];
	char	out_movie_filename4[100];
	char	temp_string[20];
//	struct diskfree_t disk_data;
	struct	dfree	disk_data;
   char*	envstring;
   char*	instring;
	char*	cntstring;
	space_data	space_vals;

	starttime = clock();
	printf ("\nMovie TGA/ZB to flat file Conversion Tool v1.050897\n(c) Copyright Midway Games Inc 1997\n");

	// check all the input arguments
	if ( argc < 3 )
			{
			do_usage();
			}

	number_of_frames = atoi(argv[1]);

	do_commands(argc, argv);

	set_sizes();

	// work out the first 4 chars of the input file name
	strncpy(file_string,argv[2],4);
	file_string[4] = 0;

	// work out the last 4 chars of the input name, for starting offset of input file
	strncpy(temp_string,argv[2],8);
	temp_string[0] = temp_string[4];
	temp_string[1] = temp_string[5];
	temp_string[2] = temp_string[6];
	temp_string[3] = temp_string[7];
	temp_string[4] = 0;
	start_offset = atoi(temp_string);

	// grab environment string CARNEVIL, this is the path to put the MOV file in
	envstring = getenv("MOVIEOUT"); 
	cntstring = getenv("MOVIECNT");
	instring = getenv("MOVIEIN");
	sprintf(out_movie_file, "%s\\%s.tmp", envstring,file_string);
	sprintf(out_movie_file2, "%s\\%s.mov", envstring,file_string);
	sprintf(out_movie_file3, "%s\\%s.cmp", cntstring,file_string);
	sprintf(out_movie_file4, "%s\\%s.cnt", cntstring,file_string);

	//figure out how much space is necessary
	space_vals = space_necessary(space_vals,start_offset,file_string,envstring,number_of_frames);
	flo_space_required = (float) space_vals.space_required/1024000.0;

	// print up concatentation details
	if (no_out_file == 1)
		printf("No output file on PC disk.\n");
	else
		printf("movie name %s\nFile size = %.2f Meg\n",out_movie_file2,flo_space_required);
	printf("Set up for Partition %d, at offset %d on the dest disk\n",(int)partition,(int)block_offset);
	printf("Frame size %d x %d\n",width, height);
	temp_time = number_of_frames/(30*60);
	printf("Movie is %d Mins, and %.2f secs long\n",temp_time,(((float)(number_of_frames-(temp_time*(30*60))))/30.0));

	if (z_buffer == 1)
		printf("Z-buffer for each frame active.\n");
	if (write2dest == 1)
		{
		// Check to make sure TBIOS is installed
		if(!check_driver())
			{
			printf("\n***** DRIVER ERROR *****\n");
			exit(1);
			}
		printf("Writing directly to Target system.\n");
		}
	
	if (no_out_file != 1)
		{
	   /* get information about drive 3 (the C drive) */
//	   if( _dos_getdiskfree( 3, &disk_data ) == 0 ) 
		getdfree(0, &disk_data);
			{
			space_vals.space_free = disk_data.df_avail * (disk_data.df_sclus * disk_data.df_bsec);
			if (space_vals.space_free < space_vals.space_required)
				{
				printf ("Space required %d, Space_availalbe %d\n",	space_vals.space_required, space_vals.space_free);
				printf ("ERROR ! Not enough disk space available.\n\n");
			  	exit (0);
				}
			}
#if 0
	  	else
			{
			printf( "Invalid drive specified\n" );
		  	exit (0);
			}
#endif
		}

	// open the movie count file
  	if  ((fc = fopen( out_movie_file3, "wb" )) == NULL) 
		{
 		//couldn't open that output file
 		printf ("ERROR !!!!! on open count file for write\n");
		exit(0);
		}

	// strip out the stuff we want from the TGA files and throw it in a temp file

	if (no_out_file == 0)
		{
		// open the pixel output file
	  	if  ((fw = fopen( out_movie_file, "wb" )) == NULL) 
			{
	 		//couldn't open that output file
	 		printf ("ERROR !!!!! on open pixel file for write\n");
			exit(0);
			}
		}

	printf("\n");

	// set up the movie description file in the filesystem
	count_data.total_frame_count = number_of_frames;
	count_data.partition = partition;
	count_data.zbuf = z_buffer;
	count_data.size = size;
	count_data.block_offset = block_offset;

	// write the number of frames, the partition this file goes to, whether it has a zbuffer or not,
	// the resolution of the movie, and the block offset from the start of the partition
	write_out_count(fc, (long *)&count_data, file_string);

	// read in each frame and process
	for (i=0;i<number_of_frames;i++ )
		{
		printf("\rProcessing Frame %d of %d.",i+1,number_of_frames);
		fflush(stdout);		//make sure we display the . as we go along

		//read in the TGA frame
		read_in_TGA_frame(i+start_offset,file_string,instring);

		//convert the colour pallete from 24 bit to 16 bit RGB565
		texus_convert(equate_ram_start,i+start_offset,file_string);

		//organise the pixel data into write out format
		organise_pixels();

		// write this out to open pixel file
		if (no_out_file == 0)
			write_out_details(fw,file_string);

		// write this out to target system if need be
		if (write2dest == 1)
			write_pblocks_out_to_targ();

		//free up the memory allocated to the tga input file so it can be used again
		free(equate_ram_start);		
		free(pal_ram);
		free(temp_pixel_space);

		if (z_buffer)			//if we wanted a z buffer in this output stream, deal with it
			{
			// read in zbuffer data
			read_in_Zbuf_frame(i+start_offset,file_string,instring);

			//organise the zbuffer data into data we can shove out
			organise_zbuffer();

			// write this out to open pixel/Zbuffer file if need be
			if (no_out_file == 0)
				write_out_zbuf(fw, file_string);
			// write this out to target system if need be
			if (write2dest == 1)
				write_zblocks_out_to_targ();

			//free up the memory allocated to the zbuffer input file so it can be used again
			free(zequate_ram_start);
			free(temp_zbuf_space);
			}

		}

	if (no_out_file == 0)
		fclose(fw);
	fclose(fc);

	// print up details on output files
	printf("\n\nNumber of pixel blocks per frame %d\n",pixel_block_size);
	printf("Total of pixel blocks for file %d\n",space_vals.total_pixel_block_count);
	if (z_buffer)
		{
		printf("\nNumber of Zbuffer blocks per frame %d\n",zbuff_block_size);
		printf("Total of Zbuffer blocks for file %d\n",space_vals.total_zblock_count);
		}
	printf("\nTotal block count for file %d\n",space_vals.total_file_block_count);

	//remove and rename temp files
	sprintf(out_movie_filename, "%s\\%s.tmp", envstring,file_string);
	sprintf(out_movie_filename2, "%s\\%s.mov", envstring,file_string);

	sprintf(out_movie_filename3, "%s\\%s.cmp", cntstring,file_string);
	sprintf(out_movie_filename4, "%s\\%s.cnt", cntstring,file_string);

	if (no_out_file == 0)
		{
		result = remove(out_movie_filename2);
		result = rename(out_movie_filename,out_movie_filename2);
		}

	result = remove(out_movie_filename4);
	result = rename(out_movie_filename3,out_movie_filename4);

	display_execution_time();
	printf("Concatenation complete.\n\n");
   }








 
