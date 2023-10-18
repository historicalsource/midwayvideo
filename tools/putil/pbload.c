#define VERSION "PBLOAD version 1.0"

/***************************************************************************/
/*                                                                         */
/* PBLOAD.C                                                                */
/*                                                                         */
/* 13 March 97                                                             */
/*                                                                         */
/* Psy-Q/MIPS version of BANKLOAD to download sound banks (.BNK files) to  */
/* the DCS2 sound system. Uses Mike Lynch Psy-Q routines. Compiled with    */
/* GNU C compiler.                                                         */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Reads in one or more banks, creates a sound call table, and loads the   */
/* bank data and the sound call table into the V+ sound RAM.               */
/*                                                                         */
/* For more information about the download commands and protocol, see the  */
/* source code for DRAMLOAD.EXE.                                           */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Basic usage:                                                            */
/*                                                                         */
/* BANKLOAD BANK1 BANK2 ... BANKN                                          */
/*                                                                         */
/* Above loads N banks from scratch. This resets/quiets the sound opsys    */
/* and reloads the entire 16 kbyte sound call table. Two files are made:   */
/* BANKLOAD.LST, which is a text file listing of all the sound calls that  */
/* were loaded by the banks specified, and BANKLOAD.DAT, which is a text/  */
/* data file that has a shorthand indication of which banks were loaded    */
/* and how big they were, where they went, etc.                            */
/*                                                                         */
/* BANKLOAD -P BANK1 BANK2 ... BANKM                                       */
/*                                                                         */
/* Above loads M banks while sound plays. This takes a little longer since */
/* the sound DSP is also busy playing sound in addition to handling the    */
/* download. This mode reads in the BANKLOAD.DAT file and adds the         */
/* specified banks to those already loaded. The BANKLOAD.LST file is       */
/* updated to reflect the newly loaded sounds and sound calls.             */
/*                                                                         */
/* BANKLOAD -P can be used repeatedly to load more banks, until the sound  */
/* RAM is filled (2 Mbytes on a production board, 4 Mbytes on development  */
/* boards.                                                                 */
/*                                                                         */
/* For more information see the general usage file BANKLOAD.DOC.           */
/*                                                                         */
/*                                                                         */
/* (c) 1995 Williams Electronics Games, Inc. - 9 June 95 mb                */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Watcom 9.01D, CFLAGS = /4r /7 /oailrtx /w3                              */
/*                                                                         */
/***************************************************************************/

/***** INCLUDE FILES *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <conio.h>
#include <dos.h>		
#include <process.h>           
#include <signal.h>             
#include <string.h>
#include <time.h>
#include <bios.h>
#include <pc.h>

/* GNU C specific */
#include <unistd.h>

/* Mike Lynch Psy-Q functions */
#include "/video/tools/putil/lib/pd.h"

/***** DEFINES *************************************************************/

#define LST_FILE_NAME "BANKLOAD.LST"
#define DAT_FILE_NAME "BANKLOAD.DAT"

#define FALSE 0
#define TRUE 1

// Various V+ and sound DSP control bit masks.

#define CLEAR_READ_INTERRUPT 0x0000

#define HSTDAT_IO		0x0000
#define HSTCTL_IO		0x1000
#define HSTADRL_IO	0x2000
#define HSTADRH_IO	0x3000
#define HODD			0x4000 

/* sound system mapping for DCS2 thru I/O ASIC on MIPS */
int SOUND_DATA_WRITE;
int SOUND_DATA_READ;
int SOUND_STATUS;
int SOUND_CLEAR_INTERRUPT;

#define HOST_TO_SOUND 0x0080
#define SOUND_TO_HOST 0x0040
#define PLAYMODE_MASK 0x8000

// The V+ sound system reserves the first 16 kbytes of RAM for the sound call
// table. There are 4096 possible entries in the table. Each entry is a 32
// bit address; that address is the byte-wise location in RAM of the start
// of the playlist for that sound call.

#define MAX_SOUNDCALLS 4096
#define SOUNDCALL_SIZE 4
#define TABLE_SIZE MAX_SOUNDCALLS * SOUNDCALL_SIZE

// 4 samples, start and end for each, 4 bytes for each = 32 

#define MAX_SAMPLES 4  // number of engine samples used
#define SAMPLE_ENTRY_SIZE 4 // 4 bytes for each address
#define SAMPLE_TABLE_SIZE MAX_SAMPLES * (SAMPLE_ENTRY_SIZE * 2)
#define DCS2_PAGE_SIZE 0x400

// V+ production systems have 2 megabytes of RAM. Development systems have
// 4 megabytes of RAM. 0x100000 = 1,048,576 bytes. 0x200000 = 2,097,152 bytes.

#define RAM_SIZE 0x200000

#define SIG_SIZE 128 // size of header info in a .BNK file

#define MAX_BANKS 16	// arbitrary max per command line

#define MAX_NAME 20	// for DOS file names
#define MAX_LINE 128	// for getline etc.

#define SAMPLE_RATE 15625 // DCS sample rate

#define BYTES_PER_SAMPLE 2 // each sample is 16-bit


/***** TYPE DEFS ***********************************************************/

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;


/***** FUNCTION PROTOTYPES *************************************************/

void setup_addresses (unsigned int, unsigned int);
void verify_checksum (unsigned short);


/***** GLOBAL VARIABLES ****************************************************/

//unsigned short vunit_io_port;  /* V+ I/O map port defined by SET VADR */


/***** dramload variables **************************************************/

uint16 checksum; /* 16-bit unsigned */

uint32 start_address; /* target start in DSP DRAM */
uint32 end_address;   /* target end in DSP RAM */
uint32 address_range; /* range == # words to send */
uint32 sample_start;   /* used for running sample start addr */
uint32 sample_end;
uint32 total_sample_size; /* size of all samples in bytes */

// Following are used to assemble various 16 and 32 bit words
// before sending to V+.

unsigned long temp1;
unsigned long temp2;

uint16 high_word; 
uint16 low_word;  

uint16 data_word; 
uint16 data_msb;  
uint16 data_lsb;  

// Following are V-UNIT address and control constants
// assigned to variables to speed things up.

uint16 vunit_addr_high;
uint16 vunit_addr_low;
uint16 host_high;
uint16 host_low;
uint16 host_ctrl;
uint16 host_data;
uint16 hod;
uint32 mask;
uint16 zero;

int play_mode;
int engine_mode;
int check_mode;
int HTS_AND_PLAYMODE;

clock_t mark_time;   // for timeouts talking to DSP

uint32 scall_table [MAX_SOUNDCALLS]; // size requires global decl

char	processor[128];	// Target platform processor
char	platform[128];		// Target platform name

/***** START OF MAIN *******************************************************/

void main (argc, argv)

int argc;
char *argv [];

{ /* main () */

int i; // temp or counter
int j; // temp or counter
int k; // temp or counter
int p; // temp or counter

//char *env_string;
int usage_error;
int errors;

char path [MAX_LINE]; 	    
char cwd [MAX_LINE];
char line [128];      

char tempstr1 [MAX_NAME];
char tempstr2 [MAX_NAME];

char engine_file [MAX_NAME];

// Following used to read in sound calls and size and figure out the 
// relocation for each bank and each sound call entry.

uint32 current_scall;
uint32 final_scall;
uint32 scall_temp;

uint32 total_scalls;
uint32 total_size;

uint32 skip_size;
uint32 current_ramloc;
uint32 final_ramloc;

uint32 upper_word;
uint32 lower_word;
uint32 addr_page;
uint32 addr_offset;

int num_banks;

// Following used by -p option to keep track of what was previously loaded.
// This comes from the .DAT file.

uint32 loaded_scalls;    // how many sound call entries already loaded
uint32 loaded_ramloc;	 // next available RAM location in * words *
uint32 loaded_numbanks;	 // how many banks were loaded last time

struct bank 
       {
		 FILE *file_handle;
		 char bnk_name [MAX_NAME];
       char lst_name [MAX_NAME];
       uint16 num_playlists;
		 uint16 num_calls;
		 uint32 size;
		 char sig_string [SIG_SIZE];
       } bank [MAX_BANKS];

struct loaded_bank 
       {
		 char bnk_name [MAX_NAME];
       char lst_name [MAX_NAME];
		 uint16 num_calls;
		 uint32 size;
		 char sig_string [SIG_SIZE];
       } loaded_bank [MAX_BANKS];

struct engine_sample
       {
		 FILE *file_handle;
       char sample_name [MAX_NAME];
       uint32 size;
       float length;
       } engine_sample [MAX_SAMPLES];

struct engine_sample *sp; // sample_pointer

struct bank *bp;  // bank pointer

struct loaded_bank *lbp; // loaded bank pointer

clock_t start_time;   // start of program execution in BIOS ticks
clock_t end_time;     // end of program execution in BIOS ticks
clock_t total_time;   // how long it took to run in BIOS ticks
float run_time;       // how long it took to run in seconds
char build_time [16]; // string for report file
char build_date [16]; // string for report file
time_t now;           // time struct used by bios call
struct tm *tp;        // time and date struct

char bank_name [MAX_NAME];           

char list_name [MAX_NAME]; // .LST file is text file with all sound calls
FILE *list_handle;

char dat_name [MAX_NAME];	//	.DAT file is text/data file for -p option
FILE *dat_handle;

FILE *engine_handle;

struct find_t fileinfo;	   

uint32 begin_addr;
uint32 end_addr;
uint32 address_range;

unsigned	short checksum;

FILE *temp_handle;

// Following used by report file generator to keep track of renumbering
// sound calls.
 
int last_call;
int this_call;
int new_call;
int call_delta;

int cur_mark;
int new_mark;

/***** START of main() *****************************************************/
	if(!check_driver())
	{
		printf("\n***** DRIVER ERROR *****\n");
		exit(1);
	}

	get_target_id(processor, platform);

	if(strstr(platform, "PHOENIX"))
	{
		SOUND_DATA_WRITE = 0xB5000048;
		SOUND_DATA_READ = 0xB5000058;
		SOUND_STATUS = 0xB5000050;
		SOUND_CLEAR_INTERRUPT = 0xB5000058;
	}
	else if(strstr(platform, "SEATTLE"))
	{
		SOUND_DATA_WRITE = 0xB6000024;
		SOUND_DATA_READ = 0xB600002c;
		SOUND_STATUS = 0xB6000028;
		SOUND_CLEAR_INTERRUPT = 0xB600002c;
	}
	else
	{
		printf("\n***** UNRECOGNIZED PLATFORM: %s *****\n", platform);
		exit(1);
	}

	printf("\nConnected to %s target system\n", platform);


   start_time = clock();

	printf ("\n");
   printf ("%s ", VERSION);  
   printf ("(compiled "__DATE__" "__TIME__")\n");

/***** SETUP the SPIEBUS and V+ ********************************************/

   #if 0
   /* read the user defined I/O port address */

	if ((env_string = getenv ("VADR")) == NULL)
	   {
      //fcloseall();
      fprintf (stderr, "\n");
		fprintf (stderr, "ERROR: I/O port must be defined with SET VADR\n");
      fprintf (stderr, "\n");
      exit (1);
	   }

   /* 16 tells strtoul() to look for number in hex notation */
	vunit_io_port = strtoul (env_string, NULL, 16);

   /* assign often used addresses and constants to variables */
   /* to improve execution time */

   vunit_addr_high = 0x0099; /* high word of SPIEBUS data port to DSP */
   vunit_addr_low = 0x0009;  /* low word of SPIEBUS data port to DSP */

   host_high = vunit_io_port + HSTADRH_IO; /* SPIEBUS address control reg */
   host_low = vunit_io_port + HSTADRL_IO;  /* SPIEBUS address control reg */

   host_ctrl = vunit_io_port + HSTCTL_IO; /* SPIEBUS read & write control reg */
   host_data = vunit_io_port + HSTDAT_IO; /* SPIEBUS read & write control reg */

   hod = HODD;
   zero = 0x00000000;

   #endif


   mask = 0x000000FF; /* for long and short to byte conversion and back */


/***** CHECK THE COMMAND LINE **********************************************/

 usage_error = FALSE;


 if (argc > 16)
    {
    fprintf (stderr,"\nToo many command line arguments... check usage.\n");
    usage_error = TRUE;
    } 

 if (argc <= 1)
    {
    fprintf (stderr,"\nNot enough command line arguments... check usage.\n");
    usage_error = TRUE;
    } 

 if ((argv [1][0] == '?') || (argv [1][1] == '?'))
    {
    usage_error = TRUE; /* display usage mesg if '?' given on cmd line */
    }

 i=0;

 play_mode = FALSE;
 check_mode = FALSE;
 engine_mode = FALSE;  

 while (i < argc)
       {
	    if (argv[i][0] == '-')
	       {
	       switch (argv [i][1])
		           {

                 case 'p': /* load while playing mode */
                 case 'P': 
					  play_mode = TRUE;
                 break;


                 case 'c': /* check size only, no actual loading */
                 case 'C': 
					  check_mode = TRUE;
                 break;


                 case 'e': /* read engine def file and load samples */
                 case 'E': 
					  engine_mode = TRUE;
                 break;


		           default:
                 fprintf (stderr,"\n Unknown option %s... check usage.\n", argv[i]);
                 usage_error = TRUE;
			        break;

		           } /* switch */
	       } /* if argv */
	    i++;
       } /* while i */

 if (engine_mode && play_mode)
    {
    fprintf (stderr, "\n");
    fprintf (stderr, "-P and -E options are mutually exclusive.\n");
    fprintf (stderr, "Can't download samples while playing.\n");
    fprintf (stderr, "\n");
    exit(0);
    }

 if (usage_error)
    {
    fprintf (stderr, "\n");
    fprintf (stderr, " usage: PBLOAD <-e filename> <-p> <-c> bankname1 bankname2 ... banknameN\n");
    fprintf (stderr, "\n");
    fprintf (stderr, " Psy-Q/MIPS version of BANKLOAD.\n");
    fprintf (stderr, " Downloads bank files to DCS2 sound D/RAM.\n");
    fprintf (stderr, "\n");
    fprintf (stderr, " -p option downloads while playing (slower)...\n");
    fprintf (stderr, " ...must have valid .DAT file from previous load.\n");
    fprintf (stderr, " -e option downloads list of 'engine' samples...\n");
    fprintf (stderr, " ...specified in filename (cannot use with -p option)\n");
    fprintf (stderr, " -c parses data but does not send it to port (debug)\n");
    fprintf (stderr, " Bankname is .BNK file name without extension.\n");
    fprintf (stderr, " Creates BANKLOAD.LST (sound call table for all loaded banks).\n");
    fprintf (stderr, " Creates BANKLOAD.DAT (data needed for -P option).\n");
    fprintf (stderr, "\n");
    fprintf (stderr, " See text file PBLOAD.DOC for more info.\n");
    fprintf (stderr, "\n");
    exit (0);
    }

 if (play_mode)
 	 {
    printf ("DOWNLOAD WHILE PLAYING ENABLED\n");
    printf ("\n");
 	 }
 else
    printf ("\n");

 // Preassemble "busy" bit mask for download while playing.
 HTS_AND_PLAYMODE = PLAYMODE_MASK | HOST_TO_SOUND;


/***** READ THE .DAT FILE, GET INFO ABOUT PREV LOADED BANKS ****************/

   /* get current directory... .BNK, .LST and .DAT must be in CWD */
   getcwd (cwd, 127);
   strcat (cwd, "\\"); 

   strcpy (path, cwd);
   strcpy (dat_name, "BANKLOAD.DAT");
   strcat (path, dat_name);

	loaded_scalls = 0;
   loaded_ramloc = 0;
	loaded_numbanks = 0;

	total_size = 0;

	if (play_mode)
		{
      if ((dat_handle = fopen (path, "rt")) == NULL)
         {
         printf ("ERROR: Cannot read .DAT file %s.\n", path);
	      printf ("\n");
	      //fcloseall ();
	      exit (1);
         }

		// skip the header line
	   if (fgets (line, 127, dat_handle) == NULL)
         {
         printf ("ERROR reading header in %s.\n", dat_name);
	      printf ("\n");
	      //fcloseall ();
	      exit (1);
         }

		// get the next available sound call entry
      if (fscanf (dat_handle, "%x", &loaded_scalls) != 1)
         {
         printf ("ERROR reading sound call address in %s.\n", dat_name);
	      printf ("\n");
	      //fcloseall ();
	      exit (1);
         }

		// get the next available ram location (in words)
      if (fscanf (dat_handle, "%x", &loaded_ramloc) != 1)
         {
         printf ("ERROR reading ram address in %s.\n", dat_name);
	      printf ("\n");
	      //fcloseall ();
	      exit (1);
         }

		// get the number of previously loaded banks
      if (fscanf (dat_handle, "%x", &loaded_numbanks) != 1)
         {
         printf ("ERROR reading number of banks in %s.\n", dat_name);
	      printf ("\n");
	      //fcloseall ();
	      exit (1);
         }

		// For each previously loaded bank... 

		for (i=0; i < loaded_numbanks; i++)
          {

			 lbp = &loaded_bank[i];

			 // clears newline from previous line in file
          fgets (line, 127, dat_handle);

		    // get the signature string for the .BNK file
          if (fgets (line, 127, dat_handle) == NULL)
             {
             printf ("ERROR reading signature in %s.\n", dat_name);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }
			 else
				 {
				 strcpy (lbp->sig_string, line);
				 j = strlen (lbp->sig_string) - 1;
				 lbp->sig_string[j] = '\0';
				 }

		    // get the name of the .BNK file
          if (fscanf (dat_handle, "%s", (char *) &lbp->bnk_name) != 1)
             {
             printf ("ERROR reading .BNK name in %s.\n", dat_name);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }

		    // get the name of the .LST file
          if (fscanf (dat_handle, "%s", (char *)&lbp->lst_name) != 1)
             {
             printf ("ERROR reading .LST name in %s.\n", dat_name);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }

		    // get the number of sound calls in that bank
          if (fscanf (dat_handle, "%x", (int *) &lbp->num_calls) != 1)
             {
             printf ("ERROR reading number of sound calls in %s.\n", dat_name);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }

		    // get the size of the data in the bank
          if (fscanf (dat_handle, "%x", &lbp->size) != 1)
             {
             printf ("ERROR reading size of bank in %s.\n", dat_name);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }

			 total_size += lbp->size;

			 } /* for i */

		fclose (dat_handle);

		// Make sure that the .BNK and .LST file for each previously
      // loaded bank exists.

		for (i=0; i < loaded_numbanks; i++)
          {

			 lbp = &loaded_bank[i];

			 strcpy (path, cwd);
          strcat (path, lbp->bnk_name);

		    if ((temp_handle = fopen (path, "rb")) == NULL)
             {
             printf ("ERROR: Cannot find %s.\n", path);
			    printf ("\n");
			    //fcloseall ();
			    exit (1);
             }

			 fclose (temp_handle);

			 strcpy (path, cwd);
          strcat (path, lbp->lst_name);

		    if ((temp_handle = fopen (path, "rt")) == NULL)
             {
             printf ("ERROR: Cannot find %s.\n", path);
			    printf ("\n");
			    //fcloseall ();
			    exit (1);
             }

			 fclose (temp_handle);

			 } /* for i */

		} /* if play mode */


/***** IF ENGINE MODE ENABLED, READ ENGINE DEF FILE ************************/

   // get the name and size of each sample to download
   // the actual data gets downloaded after the sound call table

   if (engine_mode)
      {

      total_sample_size = 0;

      strcpy (engine_file, argv[2]);
      strupr (engine_file);

      strcpy (path, cwd);
      strcat (path, engine_file);
 
		/* error and stop if engine def file does not exist */

      if (_dos_findfirst (path, _A_NORMAL, &fileinfo) != 0)	
         {
         printf ("ERROR: Engine sample list file %s not found.\n", path);
         printf ("\n");
	      //fcloseall ();
         exit (1);
         }

      printf ("SAMPLE PLAYBACK MODE ENABLED\n");
      printf ("(%d-bit samples @ %d sample rate)\n", 
              BYTES_PER_SAMPLE * 8, SAMPLE_RATE);

      printf ("sample list: %s\n", path);

      if ((engine_handle = fopen (path, "rt")) == NULL)
         {
         printf ("ERROR: Cannot read file %s.\n", path);
	      printf ("\n");
	      //fcloseall ();
	      exit (1);
         }

      /* get the name and size of each sample file */

      for (i=0; i < MAX_SAMPLES; i++)
          {
          sp = &engine_sample[i];

		    // get the name of the .SND file to use as an engine sample
          if (fscanf (engine_handle, "%s", (char *) &sp->sample_name) != 1)
             {
             printf ("ERROR reading sample name in %s.\n", engine_file);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }

          strupr (sp->sample_name);

          strcpy (path, cwd);
          strcat (path, sp->sample_name);
          strupr (path);

          if (_dos_findfirst (path, _A_NORMAL, &fileinfo) != 0)	
             {
	          printf ("ERROR: Sample file %s not found.\n", path);
	          printf ("\n");
	          //fcloseall ();
	          exit (1);
             }

          sp->size = fileinfo.size;

          total_size += sp->size;

          total_sample_size += sp->size;

          sp->length = (float) ((fileinfo.size / BYTES_PER_SAMPLE) / (1.0 * SAMPLE_RATE));

          printf ("sample %d: %s (%d bytes, %4.2f sec)\n", i, 
                  sp->sample_name, (int) sp->size, sp->length);

          } /* for i */

      } /* if engine_mode */


/***** CHECK EACH CURRENT .BNK and .LST FILE TO MAKE SURE ALL IS OK ********/

   errors = 0;
	num_banks = 0;

	if (argc > MAX_BANKS)
		{
      printf ("ERROR: Too many bank files (max of %d).\n", MAX_BANKS);
      printf ("\n");
	   //fcloseall ();
      exit (1);
		}

	j = 0;

   if (engine_mode)
      {
      // argv [1][] = '-e'
      // argv [2][] = engine def file name
      /* first file on cmd line is engine def file */
      k = 3;
      }
   else
      {
      /* first file is a bank file */
      k = 1;
      }

   for (i=k; i < argc; i++)
       {

		 /* first make sure that the .BNK file exists */

       strcpy (bank_name, argv[i]);
       strupr (bank_name);
       strcat (bank_name, ".BNK");

       strcpy (path, cwd);
       strcat (path, bank_name);
 
		 /* error and stop if bank file does not exist */

		 /* if this argv[][] is an option then skip it */

		 if (argv [i][0] != '-')
			 {
          if (_dos_findfirst (path, _A_NORMAL, &fileinfo) != 0)	
             {
	          printf ("ERROR: Bank file %s not found.\n", path);
			    errors++;
             }
		    else
			    {
			    strcpy (bank[j].bnk_name, bank_name);
				 j++;
			    num_banks++;
			    }
			 }
       } /* for i */


	j = 0;

   for (i=k; i < argc; i++)
       {

		 /* second make sure that the .LST file exists */

       strcpy (bank_name, argv[i]);
       strupr (bank_name);
       strcat (bank_name, ".LST");

       strcpy (path, cwd);
       strcat (path, bank_name);
 
		 /* error and stop if list file does not exist */

		 /* if this argv[][] is an option then skip it */

		 if (argv [i][0] != '-')
			 {
          if (_dos_findfirst (path, _A_NORMAL, &fileinfo) != 0)	
             {
	          printf ("ERROR: List file %s not found.\n", path);
			    errors++;
             }
		    else
			    {
			    strcpy (bank[j].lst_name, bank_name);
				 j++;
			    }
			 }
       } /* for i */

   if (errors)
      {
      printf ("\n");
	   //fcloseall ();
      exit (1);
	   }

/***** READ EACH .BNK FILE, CHECK SIZES FOR FIT ****************************/

   total_scalls = 0;

   for (i=0; i < num_banks; i++)
       {

		 bp = &bank[i];

		 if ((bp->file_handle = fopen (bp->bnk_name, "rb")) == NULL)
          {
          printf ("ERROR: Cannot open bank file %s.\n", bp->bnk_name);
			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }

		 rewind (bp->file_handle); 

		 /* read the 128 byte signature string */

		 /* read in the signature string */
		 if (fread (bp->sig_string, 128, 1, bp->file_handle) != 1)
          {
          printf ("ERROR reading signature string in %s.\n", bp->bnk_name);
			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }

		 /* truncate the signature string to 80 chars if need be */
		 /* this is just so it looks OK on a screen and when printed out */

		 for (j=0; j < SIG_SIZE; j++)
			  {
			  if (bp->sig_string[j] == (char) 0xFF)
				  {
				  bp->sig_string[j] = '\0';
				  break;
			     }
			  }

		 /* read the number of playlist entries */
		 /* this includes null entries */
		 if (fread (&bp->num_playlists, 2, 1, bp->file_handle) != 1)
          {
          printf ("ERROR reading playlist count in %s.\n", bp->bnk_name);
			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }

		 /* read the size of the data in bytes */
		 if (fread (&bp->size, 4, 1, bp->file_handle) != 1)
          {
          printf ("ERROR reading data size in %s.\n", bp->bnk_name);
			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }

		 // dram loading works in words... if the data to be sent over 
		 // is an odd number of bytes then even it up

		 if ((bp->size % 2) != 0)
			 {
			 bp->size += 1;
			 exit (0);
			 }

		 total_scalls += bp->num_playlists;
       total_size += bp->size;


       } /* for i */


		 // The V+ sound call table has a max of 4096 entries.

       if ((total_scalls + loaded_scalls) > MAX_SOUNDCALLS)
          {
          printf ("ERROR: Too many total sound calls %d (max %d).\n", 
                  total_scalls, MAX_SOUNDCALLS);

			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }

		 // Total_size here is the size of all the playlist data and compressed
       // sound data for all the banks specified. This has to fit in the
       // 2 or 4 MB, minus the 16k for the sound call table.

       if (total_size > (RAM_SIZE - TABLE_SIZE))
          {
          printf ("ERROR: Banks will not fit into RAM.\n"); 

          printf ("       (%u specified, %u available)\n",
                  total_size, (RAM_SIZE - TABLE_SIZE));

			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }


/***** READ THE PLAYLIST OFFSETS AND BUILD THE SOUND CALL TABLE ************/

	// Initialize the sound call table to all 0xFFFFFFFF...
   // ...this is the flag for null sound call.
	// This isn't absolutely necessary, but as a development tool, it makes
   // sure that the sound call table doesn't have any random data in it, 
   // which could point to weird RAM locations. The only check that the
   // sound opsys has for a valid sound call is that it's not 0xFFFFFF.

	for (i=0; i < MAX_SOUNDCALLS; i++)
		 scall_table [i] = 0xFFFFFFFF;

	// Sound call zero is always defined as 'stop all sound'
   // so start at sound call 1 (in normal mode).

   // In load while play mode, we just use the scall_table as temp
   // storage which we will load up at the right place later.

	if (!play_mode)
	   current_scall = 1;
	else
		current_scall = 0;

	// The offsets we read from the .BNK file are relative to the start
	// of the .BNK file... we need to calculate the absolute address
   // in the sound RAM by keeping track of the absolute start address
   // for each bank and adding that to the relative amount.

	// In normal mode, we start loading data right after the sound call table.
   // In load while play mode, we start loading data right after the last
   // bank that was loaded; we got that address from the .DAT file.

	if (!play_mode)
	   current_ramloc = TABLE_SIZE;
	else
		current_ramloc = loaded_ramloc * 2;

   if (engine_mode)
      {
      current_ramloc += (SAMPLE_TABLE_SIZE + total_sample_size);
      }

   for (i=0; i < num_banks; i++)
       {

		 bp = &bank[i];

		 /* skip the 128 byte signature string */
		 /* also skip the 2 bytes for # of entries */
       /* also skip the 4 bytes for size of the data */

		 skip_size = SIG_SIZE + 2 + 4;

		 rewind (bp->file_handle);
       fseek (bp->file_handle, skip_size, SEEK_SET);


		 // this stores the actual # of valid playable playlists
       // does not incl null entries like num_playlists does

		 bp->num_calls = 0;

 		 // read each offset and store it in the sound call table

		 for (j=0; j < bp->num_playlists; j++)
			  {
		     if (fread (&scall_temp, SOUNDCALL_SIZE, 1, bp->file_handle) != 1)
              {
              printf ("ERROR reading offset table in %s.\n", bp->bnk_name);
			     printf ("\n");
			     //fcloseall ();
			     exit (1);
              }

			  if (scall_temp != 0xFFFFFFFF)
				  {
			     scall_table [current_scall] = scall_temp + current_ramloc;
				  bp->num_calls++;
				  }
			  else
			     scall_table [current_scall] = scall_temp;

			  // Each time we find a valid playlist, bump up the running
			  // sound call number.

			  current_scall++;

			  } /* for j */

		 // add the size of the bank just loaded to the running
       // offset used to calculate the absolute addresses

		 current_ramloc += bp->size;

       } /* for i */

       final_scall = current_scall + loaded_scalls;


/***** DOWNLOAD THE SOUND CALL TABLE ***************************************/

   /* if only checking that data will fit into RAM, */
   /* blow off the actual load */

   if (check_mode)
      {
      goto skip_sound_call_table;
      }

	current_ramloc = 0; // in case we need some other offset later

	// The sound call table occupies the first 16k of sound RAM.
   // For a 16k byte table, we will send 8k words
	// so address range is TABLE_SIZE / 2.
   // Since each entry is 2 words, that is 4k entries.

	if (play_mode)
      {
		// only send over new part of table
	   begin_addr = (loaded_scalls * SOUNDCALL_SIZE) / 2;
		address_range = (TABLE_SIZE - (loaded_scalls * SOUNDCALL_SIZE)) / 2;
      end_addr = begin_addr + address_range - 1;
		}
	else
		{
		// normal mode - send over entire table
	   begin_addr = current_ramloc;
	   address_range = TABLE_SIZE / 2;
      end_addr = begin_addr + address_range - 1;
		}

	setup_addresses (begin_addr, end_addr);

	checksum = 0x0000;


	if (play_mode)
		{
	   printf ("loading partial sound call table (%3.1f kB)...\n",
		  	     (float) (address_range * 2) / 1024);
		}
	else
		{
	   printf ("loading sound call table (%3.1f kB)...\n",
		  	     (float) (address_range * 2) / 1024);
		}

	// Here is where the actual 16 kbytes of actual sound call data gets 
   // sent over. Note that the whole table doesn't have to be sent over...
   // ... only the entries that have actual sound calls are needed... the
   // rest are padded with 0xFFFFFFFF as a 'no sound call here' flag.
   // For development we go ahead and fill up the whole thing each time,
   // and pad with FF's to avoid any random stuff, but for production
   // you really only need to send as many sound calls as have been
   // specified.

	// In normal mode, we send the data fast, with no handshaking.
   // This loop has been tuned to the DSP - if you speed it up more,
   // there might be problems.

   // In load while play mode, we check the "busy" bit before each entry sent.

	// Load while playing is slower (1.5X to 2.0X) than normal mode... that is
   // why both modes are supported.

   j = 0;

	if (!play_mode) // normal mode
		{

      for (i=0; i < address_range / 2; i++)
		    {
 
		    // get the top 16 of the 32-bit word
    	    upper_word = (scall_table[j] >> 16) & 0x0000FFFF;

		    // get the bottom 16 of the 32-bit word
		    lower_word = scall_table[j] & 0x0000FFFF;

		    j++;

		    // send the top 16 
          //outpw (host_low, vunit_addr_low);
          //data_word = upper_word;
          //outpw (host_ctrl, hod);  // here only for delay
          //outpw (host_data, zero); // here only for delay
	       //outpw (host_ctrl, 0);
	       //outpw (host_data, data_word);

          psyq_mem_write (SOUND_DATA_WRITE, upper_word);
	       checksum += upper_word; 

			 // wait until sound DSP is not busy
	       psyq_mem_read (SOUND_STATUS, &temp1);
	       while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			       psyq_mem_read(SOUND_STATUS, &temp1);

		    // send the bottom 16 
          //outpw (host_low, vunit_addr_low);
          //data_word = lower_word;
          //outpw (host_ctrl, hod);  // here only for delay
          //outpw (host_data, zero); // here only for delay
	       //outpw (host_ctrl, 0);
	       //outpw (host_data, data_word);

          psyq_mem_write (SOUND_DATA_WRITE, lower_word);
	       checksum += lower_word; 

	       }
		}
	else // download while playing mode
		{
      for (i=0; i < address_range / 2; i++)
		    {
 
		    // get the top 16 of the 32-bit word
    	    upper_word = (scall_table[j] >> 16) & 0x0000FFFF;

		    // get the bottom 16 of the 32-bit word
		    lower_word = scall_table[j] & 0x0000FFFF;

		    j++;

			 // wait until sound DSP is not busy
	       psyq_mem_read (SOUND_STATUS, &temp1);
	       while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			       psyq_mem_read(SOUND_STATUS, &temp1);

		    // send the top 16 
          //outpw (host_low, vunit_addr_low);
          //data_word = upper_word;
          //outpw (host_ctrl, hod);  // here only for delay
          //outpw (host_data, zero); // here only for delay
	       //outpw (host_ctrl, 0);
	       //outpw (host_data, data_word);

          psyq_mem_write (SOUND_DATA_WRITE, upper_word);
	       checksum += upper_word; 

			 // wait until sound DSP is not busy
	       psyq_mem_read (SOUND_STATUS, &temp1);
	       while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			       psyq_mem_read(SOUND_STATUS, &temp1);

		    // send the bottom 16 
          //outpw (host_low, vunit_addr_low);
          //data_word = lower_word;
          //outpw (host_ctrl, hod);  // here only for delay
          //outpw (host_data, zero); // here only for delay
	       //outpw (host_ctrl, 0);
	       //outpw (host_data, data_word);

          psyq_mem_write (SOUND_DATA_WRITE, lower_word);
	       checksum += lower_word; 

	       } /* for */
		} /* else */


	 verify_checksum (checksum);


skip_sound_call_table:


/***** DOWNLOAD THE ENGINE ADDRESS TABLE ***********************************/

   // when in engine mode, the first 8 locations in sound DRAM
   // are reserved for the start and end addresses for the 4 samples
   // these are *byte* addresses (see docs and comments)

   if (engine_mode)
      {

	   printf ("loading sample address table...\n");

      // reserve room for 4 start/end pairs at top

      current_ramloc = TABLE_SIZE;

      sample_start = TABLE_SIZE + SAMPLE_TABLE_SIZE;

    	begin_addr = (TABLE_SIZE/2);

      for (i=0; i < MAX_SAMPLES; i++)
          {

          if (check_mode)
             printf ("sample: %d\n", i);

          /* set ptr to sample structure */
          sp = &engine_sample [i];

          // a 32-bit byte address needs two 16-bit locations
          // total of 4 16-bit words for start and end
	       address_range = SAMPLE_ENTRY_SIZE;
          end_addr = begin_addr + address_range - 1;

          if (!check_mode)
	          setup_addresses (begin_addr, end_addr);

          if (check_mode)
             printf ("dsp begin addr:%x  dsp end addr:%x\n", begin_addr, end_addr);

          begin_addr = end_addr + 1;
      
	       checksum = 0x0000;

          /***** send the starting address for the sample *****/

          // the ending address needs to be the actual last sample

          sample_end = sample_start + sp->size - 2;

          if (check_mode)
             printf ("sample start addr:%x  sample end addr:%x\n", 
                     (uint32) sample_start, (uint32 ) sample_end);

		    // get the top 16 of the 32-bit word
    	    upper_word = (sample_start >> 16) & 0x0000FFFF;

		    // get the bottom 16 of the 32-bit word
		    lower_word = sample_start & 0x0000FFFF;

          //printf ("upper:%x lower:%x\n", upper_word, lower_word);

          // now using convert to page:offset instead of upper:lower

          addr_page = (sample_start/2) / DCS2_PAGE_SIZE;
          addr_offset = (sample_start/2) % DCS2_PAGE_SIZE;

          if (check_mode)
             printf ("sample start page:%x  sample start offset:%x\n", addr_page, addr_offset);

          if (!check_mode)
             {
		       // send the top 16 
             //outpw (host_low, vunit_addr_low);
             //data_word = addr_offset;
             //outpw (host_ctrl, hod);  // here only for delay
             //outpw (host_data, zero); // here only for delay
	          //outpw (host_ctrl, 0);
	          //outpw (host_data, data_word);
             //printf ("data_word being sent: %04X ", data_word);

             psyq_mem_write (SOUND_DATA_WRITE, addr_offset);
	          checksum += addr_offset; 

			    // wait until sound DSP is not busy
	          psyq_mem_read (SOUND_STATUS, &temp1);
	          while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			          psyq_mem_read(SOUND_STATUS, &temp1);

		       // send the bottom 16 
             //outpw (host_low, vunit_addr_low);
             //data_word = addr_page;
             //outpw (host_ctrl, hod);  // here only for delay
             //outpw (host_data, zero); // here only for delay
	          //outpw (host_ctrl, 0);
	          //outpw (host_data, data_word);
             //printf ("data_word being sent: %04X\n", data_word);

             psyq_mem_write (SOUND_DATA_WRITE, addr_page);
	          checksum += addr_page; 


             /***** send the ending address for the sample *****/

		       // get the top 16 of the 32-bit word
    	       upper_word = (sample_end >> 16) & 0x0000FFFF;

		       // get the bottom 16 of the 32-bit word
		       lower_word = sample_end & 0x0000FFFF;

             //printf ("upper:%x lower:%x\n", upper_word, lower_word);

             addr_page = (sample_end/2) / DCS2_PAGE_SIZE;
             addr_offset = (sample_end/2) % DCS2_PAGE_SIZE;

             //printf ("page:%x offset:%x\n", addr_page, addr_offset);

		       // send the top 16 
             //outpw (host_low, vunit_addr_low);
             //data_word = addr_offset;
             //outpw (host_ctrl, hod);  // here only for delay
             //outpw (host_data, zero); // here only for delay
	          //outpw (host_ctrl, 0);
	          //outpw (host_data, data_word);
             //printf ("data_word being sent: %04X ", data_word);
             
             psyq_mem_write (SOUND_DATA_WRITE, addr_offset);
	          checksum += addr_offset; 

			    // wait until sound DSP is not busy
	          psyq_mem_read (SOUND_STATUS, &temp1);
	          while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			          psyq_mem_read(SOUND_STATUS, &temp1);

		       // send the bottom 16 
             //outpw (host_low, vunit_addr_low);
             //data_word = addr_page;
             //outpw (host_ctrl, hod);  // here only for delay
             //outpw (host_data, zero); // here only for delay
	          //outpw (host_ctrl, 0);
	          //outpw (host_data, data_word);
             //printf ("data_word being sent: %04X\n", data_word);
             
             psyq_mem_write (SOUND_DATA_WRITE, addr_page);
	          checksum += addr_page; 
             
	          verify_checksum (checksum);
             
             // printf ("\n");
             } /* if !check_mode */

          // sample_start and sample_end are _byte_ addrs
          // we are storing page and offset as _16-bit-word_ addrs
          // so we need to add 2-bytes, or one word
          // sample_end points to the last sample in the sound, NOT
          // to the first sample of the next sound

          sample_start = sample_end + 2;

          } /* i */

      } /* if engine_mode */


/***** DOWNLOAD THE ENGINE SAMPLE DATA *************************************/

   if (engine_mode)
      {

      current_ramloc = (TABLE_SIZE + SAMPLE_TABLE_SIZE) / 2;

      for (i=0; i < MAX_SAMPLES; i++)
          {
      
          /* set a pointer to structure */
      
          sp = &engine_sample [i];
      
          /* read in the sample and download it */
      
	   	 if ((sp->file_handle = fopen (sp->sample_name, "rb")) == NULL)
             {
             printf ("ERROR: Cannot open sample file %s.\n", sp->sample_name);
	   		 printf ("\n");
	   		 //fcloseall ();
	   		 exit (1);
             }
      
	   	 rewind (sp->file_handle); 
      
       	 begin_addr = current_ramloc;
	       address_range = sp->size / 2;
          end_addr = begin_addr + address_range - 1;
      
	       printf ("loading %s (%3.1f kB, %4.2f sec)...\n", sp->sample_name,
	   	  	      (float) (address_range * 2) / 1024, sp->length);

          if (check_mode)
             printf ("addr setup: begin_addr:%x end_addr:%x\n", begin_addr, end_addr);

          if (!check_mode)  
	         setup_addresses (begin_addr, end_addr);
      
	       checksum = 0x0000;
      
	       /* this sends over the actual data */
      
          for (j=0; j < address_range; j++)
              {
      
      
	   		  // read each 16-bit word to send as two bytes from file
              // endian of sound file is determined by program that wrote it out
              // Sound Forge writes out LSB then MSB (IBM format option)

              data_lsb = (uint16) (fgetc (sp->file_handle));
              data_msb = (uint16) (fgetc (sp->file_handle));

              data_word = (data_msb << 8) | data_lsb;

              if (!check_mode)
                {       
	             //outpw (host_low, vunit_addr_low);
                //outpw (host_ctrl, hod);  // here only for delay
                //outpw (host_data, zero); // here only for delay
	             //outpw (host_ctrl, 0);
	             //outpw (host_data, data_word);
                psyq_mem_write (SOUND_DATA_WRITE, data_word);
                }

	           checksum += data_word; 

              if ((check_mode) && (j < 10))
                 printf ("data word %02d: %04x\n", j, data_word);
      
              } /* for j */

          if (!check_mode)        
	          verify_checksum (checksum);
          else
             printf ("checksum: %04X\n", checksum);    
      
	   	 current_ramloc += (sp->size / 2);
      
	   	 fclose (sp->file_handle);
      
          } /* for i */

      } /* if engine_mode  */


/***** DOWNLOAD THE DATA FOR EACH BANK *************************************/

   /* !!! current_ramloc now goes from a byte-wise addr !!! */
   /* !!! to a word-wise addr !!! */

	if (!play_mode)
	   current_ramloc = TABLE_SIZE / 2;
	else
		current_ramloc = loaded_ramloc;

   if (engine_mode)
      {
      current_ramloc += ((SAMPLE_TABLE_SIZE + total_sample_size) / 2);
      }


   for (i=0; i < num_banks; i++)
       {

		 bp = &bank[i];

       printf ("loading %s (%d playlists, %3.1f kB)...\n", 
               bp->bnk_name, bp->num_calls, (float) bp->size / 1024);

       // If just checking for sizes and fit, blow off 
       // the actual load

       if (check_mode)
          {
          goto skip_bank_load;
          }

		 // The file pointer for each bank should now be positioned
		 // at the start of the playlist and audio data. 

     	 begin_addr = current_ramloc;
	    address_range = bp->size / 2;
       end_addr = begin_addr + address_range - 1;

	    setup_addresses (begin_addr, end_addr);

	    checksum = 0x0000;

	    /* this sends over the actual data */

		 if (!play_mode) // normal mode
			 {
          for (j=0; j < address_range; j++)
              {

	           //outpw (host_low, vunit_addr_low);

				  // read each 16-bit word to send as two bytes from file
              data_msb = (uint16) (fgetc (bp->file_handle) & 0xFF);
              data_lsb = (uint16) (fgetc (bp->file_handle) & 0xFF);
              data_word = (data_msb << 8) | data_lsb;

              //outpw (host_ctrl, hod);  // here only for delay
              //outpw (host_data, zero); // here only for delay
	           //outpw (host_ctrl, 0);
	           //outpw (host_data, data_word);

              psyq_mem_write (SOUND_DATA_WRITE, data_word);

	           checksum += data_word; 

              } /* for j */
			 }
		 else	// download while playing mode
			 {

          for (j=0; j < address_range; j++)
              {

				  // wait until DSP not busy
	           psyq_mem_read (SOUND_STATUS, &temp1);
	           while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
                    {
 			           psyq_mem_read(SOUND_STATUS, &temp1);
                    }

	           //outpw (host_low, vunit_addr_low);

				  // read 16 bits from file as two 8 bits
              data_msb = (uint16) (fgetc (bp->file_handle) & 0xFF);
              data_lsb = (uint16) (fgetc (bp->file_handle) & 0xFF);
              data_word = (data_msb << 8) | data_lsb;

	           //outpw (host_data, data_word);

              psyq_mem_write (SOUND_DATA_WRITE, data_word);
	           checksum += data_word; 

              } /* for j */

			 } /* else */


	    verify_checksum (checksum);

       skip_bank_load:

		 current_ramloc += bp->size / 2;

		 fclose (bp->file_handle);

       } /* for i */

		 // This is the next available RAM address, in words, which we
       // save in the .DAT file.

		 final_ramloc = current_ramloc;


/***** FOR THE .LST FILE, INSERT PREV LOADED BANKS INTO LIST OF BANKS ******/


	// If we are in load while play mode, then we need to insert the
   // banks that have already been loaded into the list for the report
   // file.

	if (play_mode)
		{

	   // Shift the current banks out by the number previously loaded.

		num_banks += loaded_numbanks;

	   for (i = (num_banks - 1); i > (loaded_numbanks - 1); i--)
 		    {
		    bank [i] = bank [i-loaded_numbanks]; 
		    }

	   // Copy the data from the previously loaded banks into the blank spaces
		// created above.

      for (i=0; i < loaded_numbanks; i++)
		    {

		    bp = &bank[i];
			 lbp = &loaded_bank[i];

			 strcpy (bp->sig_string, lbp->sig_string);
			 strcpy (bp->bnk_name, lbp->bnk_name);
			 strcpy (bp->lst_name, lbp->lst_name);

			 bp->num_calls = lbp->num_calls;
			 bp->size = lbp->size;

		    }

		} /* if play_mode */


/***** CREATE AND WRITE OUT THE .LST REPORT FILE ***************************/


	// get the current time and date for BANKLOAD.LST header
	time (& now);
	tp = localtime (&now);
	sprintf (build_time, "%02d:%02d:%02d", tp->tm_hour, tp->tm_min, tp->tm_sec);
	sprintf (build_date, "%02d/%02d/%02d", tp->tm_mon+1, tp->tm_mday, tp->tm_year);


	// create the list file

   strcpy (path, cwd);
	strcpy (list_name, LST_FILE_NAME);
   strcat (path, list_name);

   if ((list_handle = fopen (path, "wt")) == NULL)
      {
      printf ("ERROR: Cannot create listing file %s.\n", list_name);
	   printf ("\n");
	   //fcloseall ();
	   exit (1);
      }

	// print header information

	fprintf (list_handle, "%s (%s, %s)\n", VERSION, build_date, build_time);
   fprintf (list_handle, "\n");
	fprintf (list_handle, "%-11s   %9s   %9s\n", "Bank Name", "Playlists", "Data Size");
	fprintf (list_handle, "%-11s   %9s   %9s\n", "---------", "---------", "---------");
   fprintf (list_handle, "\n");


	total_scalls = 0;

	// for each bank print how many playlists and size of data

   for (i=0; i < num_banks; i++)
       {

		 bp = &bank[i];
		 fprintf (list_handle, "%-11s   %9d   %6.1f kB\n", bp->bnk_name,
               bp->num_calls, (float) bp->size / 1024);
		 fprintf (list_handle, "\n");

		 total_scalls += bp->num_calls;

       } /* for i */

   fprintf (list_handle, "\n");
   fprintf (list_handle, "Total Sound Calls: %d\n", total_scalls);
   fprintf (list_handle, "\n");
   fprintf (list_handle, "Total Data Size: %3.1f kB\n", (float) total_size / 1024);
   fprintf (list_handle, "\n");

	// In each .LST file, the sound calls will have been renumbered by PLBUILD
   // to take into account null entries. In a bank, the playlists can start
   // at zero. We need to reserve sound call zero, and then renumber all the
   // sound calls we have loaded, taking into account all skips and null entries.

   new_call = 1; // zero is reserved for 'kill all sound'

   for (i=0; i < num_banks; i++)
       {

		 bp = &bank[i];

		 if ((bp->file_handle = fopen (bp->lst_name, "rt")) == NULL)
          {
          printf ("ERROR: Cannot open list file %s.\n", bp->lst_name);
			 printf ("\n");
			 //fcloseall ();
			 exit (1);
          }

		 // header to show start of each bank
		 fprintf (list_handle, "\n");
		 fprintf (list_handle, "%s: %s\n", bp->bnk_name, bp->sig_string);
		 fprintf (list_handle, "\n");

		 // same as info printed out by PLBUILD
       fprintf (list_handle, "  Dec  Hex   # Trks  Video  Pinball  Description\n");
       fprintf (list_handle, "  ---- ----- ------ ------- -------  -----------\n");
		 fprintf (list_handle, "\n");

		 if (i==0)
			 {
		    fprintf (list_handle, "     0 $0000   -          -       -  STOP ALL SOUND\n");
		    fprintf (list_handle, "\n");
			 }

		 // read the .LST file for the bank and pull out any line
       // that has a sound call description on it... will start with a
       // space and have a $ to indicate hex call in the 8th space over

		 errors = FALSE;

		 // sound calls in the banks start at 0, not 1
       // we need to take this into account in
       // successive banks

	    if (i==0)
          last_call = 0;
		 else
          last_call = -1;


       while ((!feof(bp->file_handle)) && (!errors))
	          {
	          if (fgets (line, 127, bp->file_handle) == NULL)
					 {
	             errors = TRUE;
					 }
	          else
	             {

					 // playlist table has space and $ in known locations
					 if ((line[0] == ' ') && (line[7] == '$'))
						 {
						 // get the sound call number stored in the .LST file
						 sscanf (line, "%d", &this_call);
 
						 new_call += (this_call - last_call);
						 call_delta = new_call - this_call;
						 last_call = this_call;

						 // reformat scall number with new running #
						 sprintf (tempstr1, "%4d", new_call);
						 sprintf (tempstr2, "%04X", new_call);
						 // replace existing number with new number
						 for (j=0; j < 4; j++)
							  {
							  line [2+j] = tempstr1[j];
							  line [8+j] = tempstr2[j];
							  }

					    // marked call has "MARKED" at line[37] and %d at line [50]

					    if (line[37] == 'M')
						    {
						    for (p=0; p <= 5; p++)
							     tempstr1 [p] = line [37+p];

						    tempstr1 [6] = '\0';
						    strcpy (tempstr2, "MARKED");

						    if (strcmp (tempstr1, tempstr2) == 0)
							    {
								 // read the marked target playlist #
							    sscanf (line + 50, "%d", &cur_mark);

								 // offset the mark by whatever the call moved 
							    new_mark = cur_mark + call_delta;

							    sprintf (tempstr1, "%-4d", new_mark);

								 // replace the line with the new #
						       for (j=0; j < 4; j++)
							        line [50+j] = tempstr1[j];

							    } /* if tempstr */

						    } /* if line has M */

					    fprintf (list_handle, "%s", line);

					    }	/* if line is sound call */


	             } /* else */

	          if ((errors) && (!feof(bp->file_handle)))
	             {
	             printf ("\n");
                printf ("ERROR: Cannot read %s\n", bp->lst_name);
                //fcloseall ();
                exit (1);
                }

 	          } /* while */


		 fclose (bp->file_handle);

		 fprintf (list_handle, "\n");

       } /* for i banks */


   fclose (list_handle);


/***** CREATE THE .DAT FILE THAT IS USED IN -P MODE ************************/


   strcpy (path, cwd);
	strcpy (dat_name, DAT_FILE_NAME);
   strcat (path, dat_name);

	// Remember that this got closed before.

   if ((dat_handle = fopen (path, "wt")) == NULL)
      {
      printf ("ERROR: Cannot create info file %s.\n", path);
	   printf ("\n");
	   //fcloseall ();
	   exit (1);
      }

	// The BANKLOAD.DAT file is a text file containing information that
   // is used in "download while playing" mode to keep track of what
   // was loaded the last time that BANKLOAD.EXE was run... this is needed
   // so that no previous information is overwritten... this is only read
   // when -p mode is activated.

   // Contents:

	// time and date info (skipped during readback)
   // index of the first free position the sound call table (uint32)
   // address of the first free position in RAM (uint32)
   // # of banks that were loaded (int)
   // name of .BNK file for each bank loaded (string)
   // name of .LST file for each bank loaded (string)
	// number of calls in each bank loaded (uin32)
   // size of actual data in bytes for each bank loaded (uint32)


	fprintf (dat_handle, "%s (%s, %s)\n", VERSION, build_date, build_time);

   fprintf (dat_handle, "%X\n", final_scall);

   fprintf (dat_handle, "%X\n", final_ramloc);

	fprintf (dat_handle, "%d\n", num_banks);

	for (i=0; i < num_banks; i++)
		 {
		 bp = &bank[i];
       fprintf (dat_handle, "%s\n", bp->sig_string);
       fprintf (dat_handle, "%s\n", bp->bnk_name);
       fprintf (dat_handle, "%s\n", bp->lst_name);
       fprintf (dat_handle, "%X\n", bp->num_calls);
       fprintf (dat_handle, "%X\n", bp->size);
		 }

	fclose (dat_handle);


/***** SHOW RUN TIME AND INFO **********************************************/

 end_time = clock();
 total_time = end_time - start_time;
 run_time = (float) ((total_time * 1.0) / (CLOCKS_PER_SEC * 1.0));

 printf ("\n");
 printf ("total sound calls: %d\n", total_scalls);
 printf ("total data size: %3.1f kB\n", (float) total_size / 1024);
 printf ("load time: %4.2f seconds\n", run_time);
 printf ("\n");

} /* main() */

/***** END of MAIN *********************************************************/

#if 0
/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_mem_write()                                                      */
/*                                                                         */
/* Writes a 32-bit unsigned word to the V+.                                */
/*                                                                         */
/***************************************************************************/

void psyq_mem_write (address, data)

/* 32-bit words */
uint32 address;
uint32 data;

{ /* psyq_mem_write() */

/***************************************************************************/

   /* send 32-bit address high word then low word */
	outpw (vunit_io_port + HSTADRH_IO, address >> 16);
	outpw (vunit_io_port + HSTADRL_IO, address);

   /* send the high data word */
	outpw (vunit_io_port + HSTCTL_IO, HODD);
	outpw (vunit_io_port + HSTDAT_IO, data >> 16);

   /* send the low data word */
	outpw (vunit_io_port + HSTCTL_IO, 0);
	outpw (vunit_io_port + HSTDAT_IO, data);

} /* psyq_mem_write() */

/***** END of psyq_mem_write() *****************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_mem_read()                                                       */
/*                                                                         */
/* Reads a 32-bit unsigned word from the V+.                               */
/*                                                                         */
/***************************************************************************/

void psyq_mem_read (address, data)

/* 32-bit words */
uint32 address;
uint32 *data;

{ /* psyq_mem_read() */

/* 16-bit words */
uint16	low_word;
uint16 high_word;

/***************************************************************************/

   /* send 32-bit address high word then low word */
	outpw (vunit_io_port + HSTADRH_IO, address >> 16);
	outpw (vunit_io_port + HSTADRL_IO, address);

   /* read the high 16 bits */
	outpw (vunit_io_port + HSTCTL_IO, HODD);
	high_word = inpw (vunit_io_port + HSTDAT_IO);

   /* read the low 16 bits */
	outpw (vunit_io_port + HSTCTL_IO, 0);
	low_word = inpw (vunit_io_port + HSTDAT_IO);

   /* combine into 32-bits and return */
	*data = (high_word << 16) | low_word;

} /* psyq_mem_read() */

/***** END of psyq_mem_read() ******************************************************/
#endif

/***************************************************************************/
/*                                                                         */
/* FUNCTION: setup_addresses ()                                            */
/*                                                                         */
/* Sends the "start download command", the start addr and the end addr     */
/* to the sound board to start the download sequence.                      */
/*                                                                         */
/* For a better description of what's going on here, see DRAMLOAD.C,       */
/*                                                                         */
/***************************************************************************/

void setup_addresses (start_address, end_address)

uint32 start_address;
uint32 end_address;

{ /* setup_addresses */


 // For an immediate load send 0x55D0 as 'start download' command.
 // For 'download while playing' send 0x55D1.


 if (!play_mode)
	 {
    psyq_mem_write (SOUND_DATA_WRITE, 0x55D0);
    mark_time = clock() + (2 * CLOCKS_PER_SEC);
    //printf ("clock():%lu mark_time:%lu\n", clock(), mark_time);
    psyq_mem_read (SOUND_STATUS, &temp1);
    while (!(temp1 & HOST_TO_SOUND))  
          {
          //printf ("clock():%lu mark_time:%lu\n", clock(), mark_time);
   	    psyq_mem_read (SOUND_STATUS, &temp1);
          if (clock() > mark_time)
             {
             fprintf (stderr, "\n");
             fprintf (stderr, "ERROR: Timeout waiting for DSP to get load command.\n\n");
             //fcloseall ();
             exit (1);
             }
   	    } /* while */
	 } /* if !play_mode */

 if (play_mode)
	 {

	 // wait for realtime to clear up
    psyq_mem_read (SOUND_STATUS, &temp1);
    while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 	       psyq_mem_read(SOUND_STATUS, &temp1);								 

    psyq_mem_write (SOUND_DATA_WRITE, 0x55D1);
    mark_time = clock() + (2 * CLOCKS_PER_SEC);
    psyq_mem_read (SOUND_STATUS, &temp1);
    while (!(temp1 & HOST_TO_SOUND))  
          {
   	    psyq_mem_read (SOUND_STATUS, &temp1);
          if (clock() > mark_time)
             {
             fprintf (stderr, "\n");
             fprintf (stderr, "ERROR: Timeout waiting for DSP to get load command.\n\n");
             //fcloseall ();
             exit (1);
             }
   	    } /* while */
	 } /* if play_mode */


 /***** send the high word of the start address ******/

 high_word = (start_address >> 16) & 0x00FF;
 low_word = (start_address) & 0xFFFF;

 if (play_mode) 
    {
	 psyq_mem_read (SOUND_STATUS, &temp1);
	 while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			 psyq_mem_read(SOUND_STATUS, &temp1);
	 }																		 

 psyq_mem_write (SOUND_DATA_WRITE, (high_word & 0x0000FFFF));
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get high word of start address.\n\n");
          //fcloseall ();
          exit (1);
          }
       }


 /***** send the low word of the start address *****/

 if (play_mode) 
    {
	 psyq_mem_read (SOUND_STATUS, &temp1);
	 while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			 psyq_mem_read(SOUND_STATUS, &temp1);
	 }																		 

 psyq_mem_write (SOUND_DATA_WRITE, low_word);
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get low word of start address.\n\n");
          //fcloseall ();
          exit (1);
          }
       }


 /***** send the high word of the end address *****/

 high_word = (end_address >> 16) & 0x00FF;
 low_word = (end_address) & 0xFFFF;

 if (play_mode) 
    {
	 psyq_mem_read (SOUND_STATUS, &temp1);
	 while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			 psyq_mem_read(SOUND_STATUS, &temp1);
	 }																		 
 
 psyq_mem_write (SOUND_DATA_WRITE, (high_word & 0x0000FFFF));
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get high word of end address.\n\n");
          //fcloseall ();
          exit (1);
          }
       }


 /***** send the low word of the end address *****/

 if (play_mode) 
    {
	 psyq_mem_read (SOUND_STATUS, &temp1);
	 while ((temp1 & HTS_AND_PLAYMODE) != HTS_AND_PLAYMODE)
 			 psyq_mem_read(SOUND_STATUS, &temp1);
	 }																		 

 psyq_mem_write (SOUND_DATA_WRITE, low_word);
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get low word of end address.\n\n");
          //fcloseall ();
          exit (1);
          }
       }


} /* setup_addresses() */

/***************************************************************************/
/*                                                                         */
/* FUNCTION: verify_checksum ()                                            */
/*                                                                         */
/* Reads back and verifies checksum for data just sent over.               */
/*                                                                         */
/***************************************************************************/

void verify_checksum (checksum)

uint16 checksum;

{ /* verify_checksum */

 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & SOUND_TO_HOST))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for checksum from DSP.\n\n");
          //fcloseall ();
          exit (1);
          }
       }

 /* read the checksum value */
 psyq_mem_read (SOUND_DATA_READ, &temp2);
 psyq_mem_write (SOUND_DATA_READ, CLEAR_READ_INTERRUPT);

 temp2 = temp2 & 0xFFFF;

 if (temp2 != checksum)
	 {
    fprintf (stderr, "\n");
    fprintf (stderr, "ERROR: Bad checksum from DSP. ");
    fprintf (stderr, "Expected %04X but got %04X.\n\n", 
             (uint32) checksum, (uint32) temp2);
    //fcloseall ();
	 exit (1);
	 }


} /* verify_checksum() */

/***************************************************************************/

/***** END of BANKLOAD.C ***************************************************/



