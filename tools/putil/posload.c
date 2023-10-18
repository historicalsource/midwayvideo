#define VERSION "POSLOAD Version 1.0"

/***************************************************************************/
/*                                                                         */
/* POSLOAD.C                                                               */
/*                                                                         */
/* Psy-Q/MIPS version of code to download sound system operating system    */
/* code to DCS2.                                                           */
/*                                                                         */
/* Converted from Watcom to GNU C, and converted from V+/SPIEBUS routines  */
/* to MIPS/Psy-Q routines written by Mike Lynch.                           */
/*                                                                         */
/* 12 March 97 MVB                                                         */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* OSLOAD.C                                                                */
/*                                                                         */
/* Loader to get binary files from PC -> SPIEBUS -> DSP -> PRAM and DRAM.  */
/* This for loading the sound DSP operating system in the DSP internal     */
/* and external RAM. The fast program and data memory available to the     */
/* 2115 DSP on the V+ is:                                                  */
/*                                                                         */
/* internal on-chip 16-bit data memory, 512 words                          */
/* internal on-chip 24-bit program memory, 1024 words                      */
/* external S/RAM 24-bit program memory, 14k words                         */
/* external S/RAM 16-bit data memory bank zero, 8k words                   */
/* external S/RAM 16-bit data memory bank one, 8k words                    */
/*                                                                         */
/* This program talks to code that autoboots out of a 32kx8 EPROM.         */
/* Note that the D/RAM is NOT active at this point and cannot be loaded.   */
/*                                                                         */
/* Use VWAKE.EXE to set up the SPIEBUS, unlock I/O ASIC, etc.              */
/*                                                                         */
/* SPIEBUS routines based on code from Joe Linhoff.                        */
/* Watcom C 9.01D - WCC386P /4r /7 /od /w3 - DOS/4GW protected mode        */
/* (c) 1995 Williams Electronics Games, Inc. - 22 Feb 95 mb                */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* !!! Assumes V-Plus and assumes I/O mapping of SPIEBUS !!!               */
/* Requires that the environment variable VADR be set. This is the I/O map */
/* address for SPIEBUS control, e.g. SET VADR = 0x2A0.                     */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* A 0x002A sent to the DSP is the "run" command. This tells the monitor   */
/* code to jump to the start of the "real" code that has been loaded.      */
/* You must reset the sound DSP to get back into the monitor code.         */
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

#define CLEAR_READ_INTERRUPT 0x0000 /* for TI 'C31 mode */

#define HSTDAT_IO		0x0000
#define HSTCTL_IO		0x1000
#define HSTADRL_IO	0x2000
#define HSTADRH_IO	0x3000
#define HODD			0x4000    /* bit 14: 0 = low word and 1 = high word */

#define BUFFER_SIZE 4 /* used by fread() */

#define PROGRAM_MEMORY 0 /* arbitrary sound DSP memory types - see above */
#define DATA_MEMORY_0 1 
#define DATA_MEMORY_1 2

#define FALSE 0
#define TRUE 1

/* sound system mapping for DCS2 thru I/O ASIC on MIPS */
#define SOUND_DATA_WRITE 0xB5000048 
#define SOUND_DATA_READ 0xB5000058
#define SOUND_STATUS 0xB5000050

#define SOUND_CLEAR_INTERRUPT 0xB5000058

#define HOST_TO_SOUND 0x0080
#define SOUND_TO_HOST 0x0040

#define BYTE_MASK 0x000000FF
#define WORD_MASK 0x0000FFFF

/***** GLOBAL VARIABLES ****************************************************/

unsigned short vunit_io_port;  /* V+ I/O map port defined by SET VADR */
unsigned short buffer [BUFFER_SIZE];

/***** LOCAL VARIABLES *****************************************************/

void main (argc, argv)

int argc;
char *argv [];

{ /* main () */

int usage_error;

char file_name [128];
char path [128];
FILE *file_handle;

int explicit_name; // user typed in entire path
int cwd_name; // user typed file name only

unsigned int file_size;
unsigned int num_read;
unsigned short checksum;

unsigned int start_address; /* start addr to load to target DSP memory */
unsigned int end_address;	/* end addr on target */
unsigned int address_range; /* number of words */
int memory_type; /* int or ext, data or program */

char temp_string [128];
register int i;	

int delay; /* these used for delay between data sent in main loop */
int delay_time;
int j;
int k;
int m;

unsigned long temp1;
unsigned long temp2;

unsigned short word_count;
unsigned short words_sent;

unsigned char low_byte; /* for 16-bit stuff */
unsigned char high_byte;
unsigned short word;

unsigned char usb; /* these used to send a 24-bit as two 16-bit words */
unsigned char msb;
unsigned char lsb;
unsigned char dontcare;

clock_t mark_time; /* for measuring execution time */
clock_t start_time;
clock_t end_time;
clock_t total_time;
float run_time;

/***** START OF MAIN *******************************************************/

   printf ("%s ", VERSION);  
   printf ("(compiled "__DATE__" "__TIME__")\n");

/***** check the command line for right args and special run command *******/

 usage_error = FALSE;

 /* special case exception for "run" command */
 if ((argc == 2) && (!strcmp (strupr(argv[1]), "RUN")))
	 {
    mark_time = clock() + (2 * CLOCKS_PER_SEC);
    psyq_mem_read (SOUND_STATUS, &temp1);
	 /* wait for data to show up in port */
    while (!(temp1 & SOUND_TO_HOST))
          {
          psyq_mem_read (SOUND_STATUS, &temp1);
          if (clock() > mark_time)
             {
             fprintf (stderr, "\n");
             fprintf (stderr, "ERROR: Timeout waiting for 0x000A from DSP.\n\n");
             exit (1);
             }
          }

	 /* the monitor code will send back a 0x000A whenever it is idle */
    /* and ready for data or ready to run the code */
    psyq_mem_read (SOUND_DATA_READ, &temp2);
    psyq_mem_write (SOUND_DATA_READ, CLEAR_READ_INTERRUPT);
    temp2 = temp2 & 0xFFFF;
    if (temp2 != 0x000A)
	    {
       fprintf (stderr, "\n");
       fprintf (stderr, "ERROR: Expected 0x000A ack from DSP but got 0x%04X.\n\n", (unsigned short) temp2);
       exit (1);
	    }

    /* send the "run code" command */
    psyq_mem_write (SOUND_DATA_WRITE, 0x002A);
    mark_time = clock() + (2 * CLOCKS_PER_SEC);
    psyq_mem_read (SOUND_STATUS, &temp1);
    while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get run command.\n\n");
          exit (1);
          }
       }

    /* Wait for the 0x000C to show up saying */
    /* that the real O/S is running. Before, this was */
    /* done using the waitfor.exe utility. */

    mark_time = clock() + (2 * CLOCKS_PER_SEC);
    psyq_mem_read (SOUND_STATUS, &temp1);
	 /* wait for data to show up in port */
    while (!(temp1 & SOUND_TO_HOST))
          {
          psyq_mem_read (SOUND_STATUS, &temp1);
          if (clock() > mark_time)
             {
             fprintf (stderr, "\n");
             fprintf (stderr, "ERROR: Timeout waiting for 0x000C from DSP.\n\n");
             exit (1);
             }
          }

    /* make sure we got a 0x000C */
    psyq_mem_read (SOUND_DATA_READ, &temp2);
    psyq_mem_write (SOUND_DATA_READ, CLEAR_READ_INTERRUPT);
    temp2 = temp2 & 0xFFFF;
    if (temp2 != 0x000C)
	    {
       fprintf (stderr, "\n");
       fprintf (stderr, "ERROR: Expected 0x000C ack from DSP but got 0x%04X.\n\n", (unsigned short) temp2);
       exit (1);
	    }

	 printf ("DCS2 operating system running OK.\n\n");
    exit (0);

	 }	/* if run command */

/***** IF LOADING AND NOT "RUNNING" ****************************************/

 /* there are five command line args: */
 /* 1.filename, 2.start addr, 3.end addr, 4.memory type, 5.delay value */
 if (argc != 6)
    {
    fprintf (stderr,"\nMissing or too many command line arguments... check usage.\n");
    usage_error = TRUE;
    } 

 if ((argv [1][0] == '?') || (argv [1][1] == '?'))
    {
    usage_error = TRUE; /* display usage mesg if '?' given on cmd line */
    }

if (usage_error)
   {
   fprintf (stderr, "\n");
   fprintf (stderr, " usage: posload filename start_addr end_addr mem_type delay_time\n");
   fprintf (stderr, "\n");
   fprintf (stderr, " Downloads sound operating system code to DCS2 on Psy-Q/MIPS target.\n");
   fprintf (stderr, "\n");
   fprintf (stderr, " filename is binary op sys file to download\n");
   fprintf (stderr, " start_addr is 16-bit starting address in hex, e.g. 0x0400\n");
   fprintf (stderr, " end_addr is 16-bit ending address in hex, e.g. 0x1A00\n");
   fprintf (stderr, " mem_type is 0, 1 or 2:\n");
   fprintf (stderr, "  0 - internal or external program memory\n");
   fprintf (stderr, "  1 - internal data memory and external data memory bank zero\n");
   fprintf (stderr, "  2 - external data memory bank one\n");
   fprintf (stderr, " delay_time is positive integer wait value; zero turns off\n");
   fprintf (stderr, "\n");
   exit (0);
   }

/***** get the filename, check the path, open the file *********************/

 // Checking file name and directory:
 // If there are : or / or \ in the filename, then it's explicit so just
 // open it as specified.
 // Otherwise append the current working dir and try that.


 cwd_name = TRUE;
 explicit_name = FALSE;

 strcpy (file_name, argv [1]);
 strupr (file_name); 

 for (i=0; i < strlen (file_name); i++)
	  {
	  if ((file_name[i] == ':') || (file_name[i] == '/') || (file_name[i] == '\\'))
		  {
		  explicit_name = TRUE;
		  cwd_name = FALSE;
		  }
	  }

 if (cwd_name)
	 {
    getcwd (path, 127);
    strcat (path, "\\"); /* add the single backslash at end of path */
    strcat (path, file_name); /* add the file name */
    strupr (path); 
	 }

 if (explicit_name)
    strcpy (path, file_name);


 if ((file_handle = fopen (path, "rb")) == NULL)
    {
    fprintf (stderr, "ERROR: Cannot open %s.\n\n", path);
    exit (1);
    }

 rewind (file_handle);
 fseek (file_handle, 0, SEEK_END);
 file_size = ftell (file_handle);
 rewind (file_handle);

 printf ("file: %s\n", path);

/***** get the target start and end addresses ******************************/

sscanf (argv[2], "%X", &start_address);
sscanf (argv[3], "%X", &end_address);

if (end_address < start_address)
	{
   fprintf (stderr, "ERROR: End address is less than start address.\n\n");
   exit (1);
	}

if (end_address <= 0)
	{
   fprintf (stderr, "ERROR: End address is less or equal to zero.\n\n");
   exit (1);
	}

if (start_address == end_address)
	{
   fprintf (stderr, "ERROR: Start and end addresses are equal.\n\n");
   exit (1);
	}

address_range = end_address - start_address + 1;

/***** get the target memory type from the command line ********************/

sscanf (argv[4], "%d", &memory_type);

if ((memory_type < 0) || (memory_type > 2))
	{
   fprintf (stderr, "ERROR: Invalid memory type %d.\n\n", memory_type);
   exit (1);
	}

if (memory_type == PROGRAM_MEMORY)
	sprintf (temp_string, "%s", "on-chip or ext 24-bit PM");
if (memory_type == DATA_MEMORY_0)
	sprintf (temp_string, "%s", "on-chip 16-bit DM and ext 16-bit DM bank 0");
if (memory_type == DATA_MEMORY_1)
	sprintf (temp_string, "%s", "ext DM 16-bit bank 1");

/***** get the delay time from the command line ****************************/

delay = TRUE;

sscanf (argv[5], "%d", &delay_time);

if (delay_time < 0) 
	{
   fprintf (stderr, "ERROR: delay time must be greater than zero.\n");
   exit (1);
	}

if (delay_time == 0)
   delay = FALSE;

/***************************************************************************/

/* PM has 24-bit words */
if (memory_type == PROGRAM_MEMORY)
	{
	word_count = (file_size / 3);
	}

/* DM has 16-bit words */
if ((memory_type == DATA_MEMORY_0) || (memory_type == DATA_MEMORY_1))
	{
	word_count = (file_size / 2);
	}

/***************************************************************************/

if (delay)
   printf ("delay value: %d\n", delay_time);
printf ("memory type: %s\n", temp_string);
printf ("starting address: $%04X\n", start_address);
printf ("ending address: $%04X\n", end_address);
printf ("address range: $%04X words\n", address_range);
printf ("file size bytes: $%04X bytes\n", file_size);


if (memory_type == PROGRAM_MEMORY)
   printf ("file size 24-bit words: $%04X words\n", word_count);

if ((memory_type == DATA_MEMORY_0) || (memory_type == DATA_MEMORY_1))
   printf ("file size 16-bit words: $%04X words\n", word_count);


if ((unsigned int) address_range != word_count)
	{
   fprintf (stderr, "WARNING: Word count in file does not equal address range.\n");
	}

/***** SEND THE START, END AND TYPE ****************************************/

 start_time = clock();
 words_sent = 0;
 checksum = 0;

// The basic flow is:
// 1. DSP sends host a 0x000A as a signal "I am ready"
// 2. Host sends DSP a 0x001A as a command "Here comes the shit"
// 3. Host sends 16-bit start address                 
// 4. Host sends 16-bit end address
// 5. Host sends 16-bit memory type
// 6. Host sends all data
// 7. Host waits for 16-bit checksum back
// 8. If all has been loaded, host sends DSP a 0x002A as a command 
//    to start executing the code that has been loaded

 /* wait for the sound DSP to wake up and registers to settle */
 /* read the data port 0x990006 until we see a 0x000A */
 /* bit 6 of SOUND_STATUS is whether DSP has data ready for host */
 /* two sec time out */

 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & SOUND_TO_HOST))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for initial ack from DSP.\n\n");
          exit (1);
          }
       }

 psyq_mem_read (SOUND_DATA_READ, &temp2);
 /* after we read a value from the sound DSP, we have to manually */
 /* clear the "data ready" bit */
 psyq_mem_write (SOUND_DATA_READ, CLEAR_READ_INTERRUPT);
 temp2 = temp2 & 0xFFFF;

 if (temp2 != 0x000A)
	 {
    fprintf (stderr, "\n");
    fprintf (stderr, "ERROR: Expected 0x000A ack from DSP but got 0x%04X.\n\n", (unsigned short) temp2);
    exit (1);
	 }

 /* send the start download command */
 psyq_mem_write (SOUND_DATA_WRITE, 0x001A);
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get load command.\n\n");
          exit (1);
          }
       }

 /* send the starting address */
 psyq_mem_write (SOUND_DATA_WRITE, start_address);
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get start address.\n\n");
          exit (1);
          }
       }

 /* send the end address */
 psyq_mem_write (SOUND_DATA_WRITE, end_address);
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get end address.\n\n");
          exit (1);
          }
       }

 /* send the memory type */
 psyq_mem_write (SOUND_DATA_WRITE, memory_type);
 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);
 while (!(temp1 & HOST_TO_SOUND))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for DSP to get memory type.\n\n");
          exit (1);
          }
       }

/***** READ DATA FROM FILE AND DOWNLOAD ************************************/

 // there are two paths here... one for 16-bit target memory and 
 // one for 24-bit 

 for (i=0; i < address_range; i++)
     {

	  /* On faster PC's we have unexplained problems with the PC */
     /* getting ahead of the DSP. A delay value of 2 seems to make it */
     /* work OK on JFL's Pentium 90... more investigation later */

	  if (delay)
        for (j=0; j < delay_time; j ++)
            for (k=0; k < 10; k++)
                m = k + 1;

     /* wait for the port to clear up */
     psyq_mem_read (SOUND_STATUS, &temp1);
     while (!(temp1 & HOST_TO_SOUND))
           psyq_mem_read (SOUND_STATUS, &temp1);

	  if (memory_type == PROGRAM_MEMORY) /* 24-bit */
	     {
	     /* we need a 24-bit word from the file */
		  /* read it as 16 followed by 8 */
        num_read = fread ((unsigned short*) buffer, sizeof (unsigned short), 1, file_handle);

        if (ferror(file_handle))
           {
           fprintf (stderr, "\n");
           fprintf (stderr, "ERROR reading file %s.\n\n", path);
           exit (1);
           }

        msb = buffer [0] & BYTE_MASK;
        usb = (buffer [0] >> 8) & BYTE_MASK;
        word = (msb << 8) | usb;
	     checksum = (unsigned short) (word + checksum); 
        psyq_mem_write (SOUND_DATA_WRITE, (unsigned int) (word & WORD_MASK));


		  /* In real life you probably don't need to check that every word */
        /* made it over, but during debug if the DSP code hangs, then this */
		  /* code would hang without a timeout and then Ed has to reboot */
		  /* his PC every time */
        mark_time = clock() + (2 * CLOCKS_PER_SEC);
        psyq_mem_read (SOUND_STATUS, &temp1);
        while (!(temp1 & HOST_TO_SOUND))
              {
              psyq_mem_read (SOUND_STATUS, &temp1);
              if (clock() > mark_time)
                 {
                 fprintf (stderr, "\n");
                 fprintf (stderr, "ERROR1: DSP not responding during data xfer.\n\n");
                 exit (1);
                 }
              }

	     /* now we need an 8-bit byte from the file */
        num_read = fread ((unsigned char*) buffer, sizeof (unsigned char), 1, file_handle);

        if (ferror(file_handle))
           {
           fprintf (stderr, "\n");
           fprintf (stderr, "ERROR reading file %s.\n\n", path);
           exit (1);
           }

        lsb = buffer [0] & BYTE_MASK;
	     dontcare = 0xFF;
        word = (dontcare << 8) | lsb;
	     checksum = (unsigned short) (word + checksum); 
        psyq_mem_write (SOUND_DATA_WRITE, (unsigned int) (word & WORD_MASK));
        mark_time = clock() + (2 * CLOCKS_PER_SEC);
        psyq_mem_read (SOUND_STATUS, &temp1);
        while (!(temp1 & HOST_TO_SOUND))
              {
              psyq_mem_read (SOUND_STATUS, &temp1);
              if (clock() > mark_time)
                 {
                 fprintf (stderr, "\n");
                 fprintf (stderr, "ERROR2: DSP not responding during data xfer.\n\n");
                 exit (1);
                 }
              }

        words_sent ++;
	     } /* if mem = 24-bit */

	  if ((memory_type == DATA_MEMORY_0) || (memory_type == DATA_MEMORY_1))
	     {
	     /* read a 16-bit word from the file */
        num_read = fread ((unsigned short*) buffer, sizeof (unsigned short), 1, file_handle);

        if (ferror(file_handle))
           {
           fprintf (stderr, "\n");
           fprintf (stderr, "ERROR reading file %s.\n\n", path);
           exit (1);
           }

        /* swap the bytes */
        low_byte = buffer [0] & BYTE_MASK;
        high_byte = (buffer [0] >> 8) & BYTE_MASK;
        word = (low_byte << 8) | high_byte;

        /* checksum is 16-bit unsigned, just add em' up */
	     checksum = (unsigned short) (word + checksum);

        /* write the data */
        psyq_mem_write (SOUND_DATA_WRITE, (unsigned int) (word & WORD_MASK));
        mark_time = clock() + (2 * CLOCKS_PER_SEC);
        psyq_mem_read (SOUND_STATUS, &temp1);

        while (!(temp1 & HOST_TO_SOUND))
              {
              psyq_mem_read (SOUND_STATUS, &temp1);
              if (clock() > mark_time)
                 {
                 fprintf (stderr, "\n");
                 fprintf (stderr, "ERROR3: DSP not responding during data xfer.\n\n");
                 exit (1);
                 }
              }
        words_sent ++;
	     } /* if mem = 16-bit */

     } /* for i */

/***** get the checksum from the DSP ***************************************/

 mark_time = clock() + (2 * CLOCKS_PER_SEC);
 psyq_mem_read (SOUND_STATUS, &temp1);

 while (!(temp1 & SOUND_TO_HOST))
       {
       psyq_mem_read (SOUND_STATUS, &temp1);
       if (clock() > mark_time)
          {
          fprintf (stderr, "\n");
          fprintf (stderr, "ERROR: Timeout waiting for checksum from DSP.\n\n");
          exit (1);
          }
       }

 psyq_mem_read (SOUND_DATA_READ, &temp2);
 psyq_mem_write (SOUND_DATA_READ, CLEAR_READ_INTERRUPT);

 temp2 = temp2 & 0xFFFF;

 if (temp2 != checksum)
	 {
    fprintf (stderr, "\n");
    fprintf (stderr, "ERROR: Bad checksum from DSP. ");
    fprintf (stderr, "Expected %04X but got %04X.\n\n", 
             (unsigned int) checksum, (unsigned int) temp2);
	 }

/***************************************************************************/

 printf ("words sent: $%04X words\n", words_sent);
 printf ("16-bit checksum: $%04X\n", checksum);

 end_time = clock();
 total_time = end_time - start_time;
 run_time = (float) ((total_time * 1.0) / (CLOCKS_PER_SEC * 1.0));

 printf ("load time: %05.2f seconds\n", run_time);

} /* main() */

/***** END of MAIN *********************************************************/

/***** END of POSLOAD.C ****************************************************/


