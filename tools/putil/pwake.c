#define VERSION "PWAKE version 1.0"

/***************************************************************************/
/*                                                                         */
/* PWAKE.C                                                                 */
/*                                                                         */
/* Simple program to unlock the I/O ASIC on the Phoenix target.            */
/*                                                                         */
/* The comm path is:                                                       */
/*                                                                         */
/* PC -> Psy-Q TSR -> PC SCSI card -> Phoenix SCSI card -> Psy-Q monitor   */
/*                                                                         */
/* The PC sends commands to the Psy-Q monitor which runs on the target.    */
/*                                                                         */
/* Note! This must be a 16-bit application. The Psy-Q TSR is a 16-bit TSR. */
/* This code is compiled with Microsoft C version 6.0A (ancient, but it's  */
/* as 16-bit as it gets). Also, the Psy-Q SCSI card is not really a true   */
/* SCSI card, so you can't use a normal ASPI type SCSI driver. You have    */
/* to use the Psy-Q TSR.                                                   */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

/***** DEFINES *************************************************************/

#define FALSE 0
#define TRUE 1

#define ERROR 1
#define OK 0

#if ! defined(__GNUC__)
#define PSYQ_COMM_INT 0x7E    /* int used to talk to the Psy-Q TSR */
#endif

#define IOASIC_RESET   0xB5800000
#define IOASIC_SW_DIP  0xB5000000
#define IOASIC_SW_P12  0xB5000010
#define IOASIC_SW_P34  0xB5000018
#define IOASIC_CONTROL 0xB5000078
#define IOASIC_STATUS  0xB5000070
#define IOASIC_HTS_CTRL 0xB5000040

/***** INCLUDES ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <conio.h>
#include <dos.h>		
#include <process.h>           
#include <signal.h>             
#include <string.h>
#include <bios.h>
#if defined(__GNUC__)
#include	<unistd.h>
//#include	"pd.h"
#include "/video/tools/putil/lib/pd.h"
#endif

/***** FUNCTION PROTOTYPES *************************************************/

#if defined(__GNUC__)
#define	psyq_check_driver	check_driver
#define	psyq_get_id			get_target_id
#define	psyq_get_info		get_target_info
#define	psyq_get_version	get_target_version
#else
int psyq_check_driver (void);
int psyq_get_id (char *processor, char *platform);
int psyq_get_info (int *address);
int psyq_get_version (int *major, int *minor);
int psyq_mem_read (unsigned long address, unsigned long *data);
int psyq_mem_write (unsigned long address, unsigned long data);
#endif
int psyq_startup (void);

void waste_time (int how_long);

int unlock_io_asic (void);
int wait_ioasic (void);
void reset_sound_system (void);


/***** GLOBAL VARIABLES ****************************************************/

int debug;           /* show debugging info */
int usage_error;     /* cmd line problems */
int quiet_mode;      /* minimal output mode */

/***************************************************************************/

void main (int argc, char **argv)

{ /* START of main () */

int i;

/***** PARSE COMMAND LINE **************************************************/

 debug = FALSE;
 usage_error = FALSE;
 quiet_mode = FALSE;  

 if ((argv [1][0] == '?') || (argv [1][1] == '?'))
    usage_error = TRUE;

 i = 1;

 while (i < argc)
       {
	    if (argv[i][0] == '-')
	       {
	       switch (argv [i][1])
		           {
                 case 'd': /* debug mode - show info */
                 case 'D': /* debug mode - show info */
	              debug = TRUE;
                 break;     

                 case 'q': /* quiet mode - no output */
                 case 'Q': /* quiet mode - no output */
	              quiet_mode = TRUE;
                 break;     

		           default:
                 fprintf (stderr,"\nUnknown option %s... check usage.\n", argv[i]);
                 usage_error = TRUE;
			        break;

		           } /* switch */
	       } /* if argv */
	    i++;
       } /* while i */

 if (usage_error)
    {
    fprintf (stderr, "\n");
    fprintf (stderr, "%s ", VERSION);  
    fprintf (stderr, "(compiled "__DATE__" "__TIME__")\n");
    fprintf (stderr, "\n");	
    fprintf (stderr, "usage: pwake\n");
    fprintf (stderr, "\n");	
    fprintf (stderr, "Unlocks Phoenix I/O ASIC and resets DCS2 sound system.");
    fprintf (stderr, "\n");
    fprintf (stderr, "Psy-Q TSR must be loaded.\n");
    fprintf (stderr, "\n");
#if ! defined(__GNUC__)
    fcloseall ();	
#endif
    exit (0);
    }	

   /* all OK, show banner */

   printf ("\n");
   printf ("%s ", VERSION);  
   printf ("(compiled "__DATE__" "__TIME__")\n");

/***************************************************************************/

   if (psyq_startup() != OK)
      {
      exit(1);
      }

   if (unlock_io_asic() == OK)
      {
      printf ("I/O ASIC unlocked OK\n");
      }
   else
      {
      printf ("ERROR unlocking I/O ASIC\n");
      exit(1);
      }


   reset_sound_system();

   printf ("sound system reset OK\n");


/***** cleanup and exit ****************************************************/

   printf ("\n");
   exit (0); 

} /* END of main () */

/***** END of MAIN () ******************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: reset_sound_system()                                          */
/*                                                                         */
/* Toggles the sound system reset bit.                                     */
/*                                                                         */
/***************************************************************************/

void reset_sound_system (void)

{ /* reset_sound_system() */

   if (debug)
	   printf ("resetting sound board\n");

   psyq_mem_write (IOASIC_HTS_CTRL, 0x0000); /* sound board reset */

   psyq_mem_write (IOASIC_HTS_CTRL, 0x0001); /* sound board release */


} /* reset_sound_system() */

/***** END of reset_sound_system() *****************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: unlock_io_asic()                                              */
/*                                                                         */
/* Unlocks the I/O ASIC.                                                   */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int unlock_io_asic (void)

{ /* unlock_io_asic() */

unsigned long pdata;

int i;

/***************************************************************************/

   /* reset the I/O ASIC to put it in a known state */
   if (debug)
      printf ("reset I/O ASIC\n");
   psyq_mem_write (IOASIC_RESET, 0);
   psyq_mem_write (IOASIC_RESET, 1);

   /* send "host arm" sequence to I/O ASIC */
   /* this is a six word "unlock" sequence */
   if (debug)
	   printf ("sending host arm sequence\n");

   wait_ioasic();
   psyq_mem_write (IOASIC_SW_P34, 0x002B); 
   wait_ioasic();
   psyq_mem_write (IOASIC_SW_P34, 0x0093); 
   wait_ioasic();
   psyq_mem_write (IOASIC_SW_P34, 0x00A7);
   wait_ioasic();
   psyq_mem_write (IOASIC_SW_P34, 0x004E);
   wait_ioasic();
   psyq_mem_write (IOASIC_SW_P34, 0x0001);
   wait_ioasic();
	psyq_mem_write (IOASIC_SW_P34, 0x001E);


	/* now send an eight word "seed" sequence */
   /* this builds the "CLL host variable" */
   if (debug)
	   printf ("sending seed data\n");
   for (i=0; i < 8; i++)
      {
      wait_ioasic();
	   psyq_mem_write (IOASIC_SW_P12, 0x1234);
      }

   if (debug)
	   printf ("sending expected result\n");
   wait_ioasic();
	psyq_mem_write (IOASIC_SW_P12, 0x0042);
   wait_ioasic();
	psyq_mem_write (IOASIC_SW_P12, 0x0065);
   wait_ioasic();
	psyq_mem_write (IOASIC_SW_P12, 0x00E3);

   if (debug)
 	   printf ("sending I/O ASIC configuration\n");
   for (i=0; i < 4; i++)
      {
      wait_ioasic();
	   psyq_mem_write (IOASIC_SW_P12, 0x0000);
      }

   if (debug)
 	   printf ("sending unlock request\n");
   wait_ioasic();
	psyq_mem_write (IOASIC_SW_DIP, 0x0054);
   wait_ioasic();
   psyq_mem_write (IOASIC_SW_DIP, 0x0029);
   wait_ioasic();
	psyq_mem_write (IOASIC_SW_DIP, 0x00E2);


	/* when main ctrl goes to zero it is unlocked */

   if (debug)
      printf ("waiting for main ctrl reg to clear\n");

	do {
		psyq_mem_read (IOASIC_CONTROL, &pdata);
		} while ((pdata & 0x00FF) != 0x0000); 

   if (debug)
      printf ("I/O ASIC unlocked\n");

   /* page 8 of I/O ASIC functional spec */
	/* 'C31 write-back mode on - bit 15 */
   /* LED off - bit 14 high turns it off */
	/* all other don't care or zero - disables all interrupts */

   if (debug)
      printf ("setting main control register\n");
   /* turn off LED */
   /* set 'C31 write-back mode */
	psyq_mem_write (IOASIC_CONTROL, 0xC000);

   return OK;

} /* unlock_io_asic() */

/***** END of unlock_io_asic() *********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: wait_ioasic()                                                 */
/*                                                                         */
/* Waits for the I/O ASIC to toggle the bit saying it is ready for the     */
/* next data word.                                                         */
/*                                                                         */
/***************************************************************************/

int wait_ioasic (void)

{ /* wait_ioasic() */

unsigned long pdata;

	do {psyq_mem_read (IOASIC_SW_DIP, &pdata);} while (pdata & 0x8000);

   return OK;

} /* wait_ioasic() */

/***** END of wait_ioasic() ************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_startup()                                                */
/*                                                                         */
/* Simple wrapper for Psy-Q startup functions.                             */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_startup (void)

{ /* psyq_startup() */

char processor [13];    /* target processor type */
char platform [13];     /* target platform description */
int card_address;       /* PC Psy-Q card address */
int major_version;      /* Psy-Q target monitor version */
int minor_version;

/***************************************************************************/

#if ! defined(__GNUC__)
   if (psyq_check_driver() != OK)
#else
   if (psyq_check_driver() == 0)
#endif
      {
      if (!quiet_mode)
         printf ("ERROR: psyq_check_driver() failed.\n\n");
      return ERROR;
      }

   if (psyq_get_info (&card_address) != OK)
      {
      if (!quiet_mode)
         printf ("ERROR: psyq_get_info() could not get card_address.\n\n");
      return ERROR;
      }
   if (!quiet_mode)
      printf ("Psy-Q SCSI found at: 0x%X\n", card_address);


   if (psyq_get_version (&major_version, &minor_version) != OK)
      {
      if (!quiet_mode)
         printf ("ERROR: psyq_get_version() could not get BIOS version.\n\n");
      return ERROR;
      }
   if (!quiet_mode)
      printf ("Psy-Q BIOS version:  %d.%d\n", major_version, minor_version);


   if (psyq_get_id (processor, platform) != OK)
      {
      if (!quiet_mode)
         printf ("ERROR: psyq_get_id() could not get processor and platform type.\n\n");
      return ERROR;
      }
   if (!quiet_mode)
      {
      printf ("target processor:    %s\n", processor);
      printf ("target platform:     %s\n", platform);
      }

   return OK;

} /* psyq_startup() */

/***** END of psyq_startup() *********************************************/
#if ! defined(__GNUC__)
/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_mem_write ()                                             */
/*                                                                         */
/* Writes a 32-bit value to the 32-bit target address given.               */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_mem_write (unsigned long address, unsigned long data)

{ /* psyq_mem_write() */

unsigned long far *dptr;

union REGS out_regs;
union REGS in_regs;
struct SREGS out_sregs;

/***************************************************************************/

   out_regs.h.ah = 0x1A;   /* cmd number */
   out_regs.h.al = 0x00;   /* unit number */

   out_regs.x.cx = 4;      /* send 4 bytes */

   /* dx = upper 16-bits of 32-bit addr */
   out_regs.x.dx = (unsigned int)(address >> 16) & 0xFFFF;  

   /* bx = lower 16-bits of 32-bit addr */
   out_regs.x.bx = (unsigned int)(address & 0xFFFF);          

// ***************************************************************************
// BUG
// dptr is not set here
// This can NOT work
//
   *dptr = data;
   out_sregs.es = FP_SEG (dptr);      /* ptr to data segment */
   out_regs.x.si = FP_OFF (dptr);     /* ptr to data offset */
//
// Should be
// out_sregs.es = FP_SEG(&data);
// out_regs.x.si = FP_OFF(&data);
// END BUG
// ***************************************************************************

   int86x (PSYQ_COMM_INT, &out_regs, &in_regs, &out_sregs);

   if (in_regs.x.cflag)
      {
      printf ("psyq_mem_write(): ERROR writing data (error code %04X).\n",
              in_regs.x.ax);
      return ERROR;
      }

   return OK;


} /* psyq_mem_write() */

/***** END of psyq_mem_write() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_mem_read ()                                              */
/*                                                                         */
/* Reads a 32-bit value from the 32-bit target address given.              */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_mem_read (unsigned long address, unsigned long *data)

{ /* psyq_mem_read() */

unsigned long temp;      /* holds 32-bit value coming back */
unsigned long far *dptr; /* data pointer */

union REGS out_regs;
union REGS in_regs;
struct SREGS out_sregs;

/***************************************************************************/


   out_regs.h.ah = 0x19;   /* cmd number */
   out_regs.h.al = 0x00;   /* unit number */

   out_regs.x.cx = 4;      /* return 4 bytes */

   /* dx = upper 16-bits of 32-bit addr */
   out_regs.x.dx = (unsigned int)(address >> 16) & 0xFFFF;  

   /* bx = lower 16-bits of 32-bit addr */
   out_regs.x.bx = (unsigned int)(address & 0xFFFF);          

   /* this must be cast to a far* to work */
   dptr = (unsigned long far *) &temp;

   out_sregs.es = FP_SEG (dptr);                /* ptr to data segment */
   out_regs.x.si = FP_OFF (dptr);               /* ptr to data offset */

   int86x (PSYQ_COMM_INT, &out_regs, &in_regs, &out_sregs);

   if (in_regs.x.cflag)
      {
      printf ("psyq_mem_read(): ERROR reading data (error code %04X).\n",
              in_regs.x.ax);
      return ERROR;
      }

   *data = temp;

   return OK;


} /* psyq_mem_read() */

/***** END of psyq_mem_read() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_get_id()                                                 */
/*                                                                         */
/* Requests the target ID. Returns a 24-byte string, consisting of a       */
/* 12-byte processor type string and a 12-byte platform type.              */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_get_id (char *processor, char *platform)

{ /* psyq_get_id() */

static char id_str [25];      /* string returned by target */

char far *id_ptr;

union REGS out_regs;
union REGS in_regs;
struct SREGS out_sregs;

int i;
 
/***************************************************************************/

   out_regs.h.ah = 0x11;      /* cmd number */
   out_regs.h.al = 0x00;      /* unit number */

   id_ptr = (char far *) id_str;

   out_sregs.es = FP_SEG (id_ptr);
   out_regs.x.si = FP_OFF (id_ptr);

   int86x (PSYQ_COMM_INT, &out_regs, &in_regs, &out_sregs);

   if (in_regs.x.cflag)
      {
      printf ("psyq_get_id(): ERROR requesting ID (error code %04X).\n",
              in_regs.x.ax);
      return ERROR;
      }

   /* extract strings and null terminate */
   /* the string returned is not null terminated or delineated */
   id_str [24] = 0;
   for (i=0; i < 12; i++)
      {
      processor[i] = id_str[i];
      platform[i] = id_str[i+12];
      }
   processor [11] = 0;
   platform [11] = 0;

   return OK;

} /* psyq_get_id() */

/***** END of psyq_get_id() ************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_get_info()                                               */
/*                                                                         */
/* Requests the SCSI card info from the target.                            */
/*                                                                         */
/* Currently just gets the SCSI card address. Not exactly sure what mode   */
/* we are running the driver in, but it does not use an IRQ or DMA.        */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_get_info (int *address)

{ /* psyq_get_info() */

union REGS out_regs;
union REGS in_regs;
struct SREGS out_sregs;

/***************************************************************************/

   out_regs.h.ah = 0x06;      /* cmd number */
   out_regs.h.al = 0x00;      /* unit number */

   int86x (PSYQ_COMM_INT, &out_regs, &in_regs, &out_sregs);

   if (in_regs.x.cflag)
      {
      printf ("psyq_get_info(): ERROR requesting card info (error code %04X).\n",
              in_regs.x.ax);
      return ERROR;
      }

   *address = in_regs.x.dx;

   return OK;

} /* psyq_get_info() */

/***** END of psyq_get_info() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_get_version()                                            */
/*                                                                         */
/* Requests the versio of the psyq TSR. The version is AA.BB where AA is   */
/* the "major" version returned in ah and BB is the "minor" version        */
/* returned in al.                                                         */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_get_version (int *major, int *minor)

{ /* psyq_get_version() */

union REGS out_regs;
union REGS in_regs;
struct SREGS out_sregs;

/***************************************************************************/

   out_regs.h.ah = 0x0D;      /* cmd number */
   out_regs.h.al = 0x00;      /* unit number */

   int86x (PSYQ_COMM_INT, &out_regs, &in_regs, &out_sregs);

   if (in_regs.x.cflag)
      {
      printf ("psyq_get_version(): ERROR requesting version (error code %04X).\n",
              in_regs.x.ax);
      return ERROR;
      }

   *major = out_regs.h.ah;
   *minor = out_regs.h.al;

   return OK;

} /* psyq_get_version() */

/***** END of psyq_get_version() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: psyq_check_driver()                                           */
/*                                                                         */
/* Checks if the PSYBIOS TSR has been installed.                           */
/*                                                                         */
/* As of 13 Jan 97, this TSR is called TBIOS2.COM.                         */
/*                                                                         */
/* Trying to access the TSR if it is not installed can hang your PC, so    */
/* this function exits the program if the TSR is not found.                */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int psyq_check_driver (void)

{ /* psyq_check_driver() */

union REGS out_regs;
union REGS in_regs;
struct SREGS out_sregs;

/***************************************************************************/

   out_regs.h.ah = 0x35;
   out_regs.h.al = PSYQ_COMM_INT;
   in_regs.x.bx = 0;
   out_sregs.es = 0;   

   int86x (0x21, &out_regs, &in_regs, &out_sregs);

   if (!(in_regs.x.bx | out_sregs.es))
      {
      printf ("\n");
      printf ("ERROR Psy-Q driver not found!\n");
      printf ("\n");
      fcloseall();
      exit(1);
      }

   return OK;

} /* psyq_check_driver() */

/***** END of psyq_check_driver() *****************************************/
#endif
/***************************************************************************/
/*                                                                         */
/* FUNCTION: waste_time ()                                                 */
/*                                                                         */
/* Loop to waste time for delays. Each unit of delay time is equal to one  */
/* bios tick (55 ms between bios ticks gives roughly 18 ticks per second). */
/*                                                                         */
/***************************************************************************/

void waste_time (int how_long)

{ /* waste_time () */
#if ! defined(__GNUC__)
long entry_time;
long current_time;

_bios_timeofday (_TIME_GETCLOCK, &entry_time);

do {
   _bios_timeofday (_TIME_GETCLOCK, &current_time);
   }
   while (abs((int)(current_time - entry_time)) <= how_long);
#else
	usleep (55000 * how_long);
#endif

} /* waste_time () */

/***** END of waste_time() *************************************************/

/***** END of file PWAKE.C ************************************************/


