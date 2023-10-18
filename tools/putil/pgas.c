#define VERSION "PGAS version 1.0"

/***************************************************************************/
/*                                                                         */
/* PGAS.C                                                                  */
/*                                                                         */
/* 13 March 97 MVB                                                         */
/*                                                                         */
/* Psy-Q/MIPS version of GASPEDAL. Uses Mike Lynch Psy-Q code. Compiled    */
/* with GNU C compiler.                                                    */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* GASPEDAL.C                                                              */
/*                                                                         */
/* Simple utility to test / debug the DCS2 sample playback engine sounds   */
/* and sound call protocol. Reads key hits from PC keyboard to adjust the  */
/* pitch, volume and pan parameters and sends out the corresponding hex    */
/* sound call string to the V+ / DCS2 sound board.                         */
/*                                                                         */
/* Cabbaged together from QCOMM code; see QCOMM source code for more info  */
/* and better general examples of what's going on.                         */
/*                                                                         */
/* See the text file GASPEDAL.DOC for more info on engine parameters.      */
/*                                                                         */
/* (c) 1996 Midway Manufacturing Company                                   */
/*                                                                         */
/* Compiled using Microsoft C version 6.00 A                               */
/*                                                                         */
/* Compiled using Watcom 9.01D                                             */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Version 1.0 - 4 Oct 96 mb                                               */
/*                                                                         */
/* Version 1.1 - 13 Feb 97 mb                                              */
/*                                                                         */
/***************************************************************************/

/***** CONSTANTS AND DEFINES ***********************************************/

#define  YES       1
#define  NO        0
#define  NONE	    0
#define  TRUE      1
#define  FALSE     0
#define  ON        1
#define  OFF       0

/* reserve sound calls pulled from QCOMM stuff */

#define  MASTER 0x55AA        /* reserved sound call for master vol  */
#define  STARTUP_VOLUME 0x67  /* initial vol, also default in snd code */
#define  MAXIMUM_VOLUME 255   /* maximum possible track and master volume */

/* sound system mapping for DCS2 thru I/O ASIC on MIPS */
#define SOUND_DATA_WRITE 0xB5000048 
#define SOUND_DATA_READ 0xB5000058
#define SOUND_STATUS 0xB5000050
#define SOUND_CLEAR_INTERRUPT 0xB5000058
#define CLEAR_READ_INTERRUPT 0x0000 /* for TI 'C31 mode */

#define HOST_TO_SOUND 0x0080
#define SOUND_TO_HOST 0x0040

#define BYTE_MASK 0x000000FF
#define WORD_MASK 0x0000FFFF

/* addrs for talking to V+ via the PC SPIEBUS card */

#define PRIMEBUS     0x0808064
#define DSPRUN	      0x8000    /* bit 15: 0 = hold and 1 = run */
#define DAENZ        0x2000    /* bit 13: 1 = disable bus... ALWAYS ON */

#define HSTDAT_IO		0x0000
#define HSTCTL_IO		0x1000
#define HSTADRL_IO	0x2000
#define HSTADRH_IO	0x3000
#define HODD			0x4000    /* bit 14: 0 = low word and 1 = high word */

#define  DELAY 100L    /* delay between bytes sent */ 

#define CMD_PLAYER 0x55E0
#define CMD_DRONE1 0x55E3
#define CMD_DRONE2 0x55E4

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

/***** FUNCTION PROTOTYPES *************************************************/

void send_sound_call (unsigned int);                  /* sends 16-bit sound call */
void dynamic_master_volume (unsigned int, unsigned int);

void send_engine_parms (unsigned int select, int engine_state, int engine_pitch, 
                        int engine_volume, int engine_pan);

/***** GLOBAL VARIABLES ****************************************************/

int debug;                    /* debug mode */

/***** START of MAIN () ****************************************************/

void main (int argc, char **argv)

{ /* START of main () */

/***** LOCAL VARIABLES *****************************************************/

int i;      /* temp or counter */

int usage_error;     /* bad cmd line or -? */

int done;                  /* user wants out */
int stop_sound;            /* whether or not to kill sounds */
char key_in;               /* single keystroke typed by user */
int update;                /* update parameter display */

int master_volume;	      /* 8-bit, 0=off, 255=max */
int engine_volume;         /* 8-bit, 0=off, 255=max */
int engine_pitch;          /* 8-bit, 0=slowest, 255=fastest */
int engine_pan;            /* 8-bit, 0=left, 127=center, 255=right */
int engine_state;          /* on or off */
int engine_select;         /* 0x55E0, 0x55E3 or 0x55E4 snd call cmd */

char *s_str;
char player_str[] = "PLAYER";
char drone1_str[] = "DRONE1";
char drone2_str[] = "DRONE2";

typedef struct state
   {
   int cmd;       /* cmd to send */
   int state;     /* on or off */
   int volume;    /* 0 - 255 */
   int pitch;     /* 0 - 255 */
   int pan;       /* 0 - 255 */
   char *n_str;   /* name */
   char *s_str;   /* on or off */
   } state_t;

state_t player;   /* player - two voices, three samples */
state_t drone1;   /* drone 1 - single voice */
state_t drone2;   /* drone 2 - single voice */
state_t *snd;     /* current active structure */

char on_str[] = " ON";
char off_str[] = "OFF";
char *e_str;
 
/***** PARSE COMMAND LINE **************************************************/

 usage_error = FALSE;

 master_volume = STARTUP_VOLUME;
 engine_volume = STARTUP_VOLUME; 
 stop_sound = TRUE;

 if (argc > 3)
    usage_error = TRUE;

 if ((argv [1][0] == '?') || (argv [1][1] == '?'))
    usage_error = TRUE;

 i = 1;

 while (i < argc)
       {
	    if (argv[i][0] == '-')
	       {
	       switch (argv [i][1])
		           {
                 case 'd': /* debug mode */
                 case 'D': /* debug mode */
	              debug = TRUE;
                 break;     

                 case 'm': /* set master volume */
                 case 'M': 
                 sscanf (&argv[i][2], "%d", &master_volume);
                 if (master_volume > MAXIMUM_VOLUME)
                    master_volume = MAXIMUM_VOLUME;
                 if (master_volume < 0)
                    master_volume = 0;
                 break;

                 case 'v': /* set engine volume */
                 case 'V': 
                 sscanf (&argv[i][2], "%d", &engine_volume);
                 if (engine_volume > MAXIMUM_VOLUME)
                    engine_volume = MAXIMUM_VOLUME;
                 if (engine_volume < 0)
                    engine_volume = 0;
                 break;

                 case '?': /* show usage info */
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
    fprintf (stderr, "Simple utility to adjust DCS2 engine parameters.\n");
    fprintf (stderr, "\n");	
    fprintf (stderr, "usage: pgas <-vNNN> <-mNNN>\n");	
    fprintf (stderr, "\n");	
    fprintf (stderr, "-vNNN sets engine volume to NNN (0 to 255)\n");	
    fprintf (stderr, "-mNNN sets master volume to NNN (0 to 255)\n");	
    fprintf (stderr, "\n");	
    fprintf (stderr, "p selects player sound\n");	
    fprintf (stderr, "1 selects drone1 sound\n");	
    fprintf (stderr, "2 selects drone2 sound\n");	
    fprintf (stderr, "+/- controls master volume\n");	
    fprintf (stderr, "cursor up/down controls current volume\n");	
    fprintf (stderr, "cursor left/right controls current pitch\n");	
    fprintf (stderr, "l and r control current pan\n");	
    fprintf (stderr, "e toggles current sound on/off\n");	
    fprintf (stderr, "q or Q quits without turning off sounds\n");	
    fprintf (stderr, "escape quits and turns off all sounds\n");	
    fprintf (stderr, "\n");	
    fprintf (stderr, "See text file PGAS.DOC for more details.\n");
    fprintf (stderr, "\n");
    exit (0);
    }	

   /* all OK, show banner */

   printf ("\n");
   printf ("%s ", VERSION);  
   printf ("(compiled "__DATE__" "__TIME__")\n");
   printf ("\n");


/***** initialize player, drone1 and drone2 structures *********************/

   /* start out with player sound on */
   snd = &player;
   snd->cmd = CMD_PLAYER;
   snd->state = ON;     
   snd->volume = engine_volume;    
   snd->pitch = 127;     
   snd->pan = 127;       
   snd->n_str = player_str;   
   snd->s_str = on_str;   

   /* start out with drone1 sound off */
   snd = &drone1;
   snd->cmd = CMD_DRONE1;
   snd->state = OFF;     
   snd->volume = engine_volume;    
   snd->pitch = 127;     
   snd->pan = 127;       
   snd->n_str = drone1_str;   
   snd->s_str = off_str;   

   /* start out with drone2 sound off */
   snd = &drone2;
   snd->cmd = CMD_DRONE2;
   snd->state = OFF;     
   snd->volume = engine_volume;    
   snd->pitch = 127;     
   snd->pan = 127;       
   snd->n_str = drone2_str;   
   snd->s_str = off_str;   

   /* show init values */
   snd = &player;
   s_str = snd->n_str;
   e_str = snd->s_str;
   engine_pitch = snd->pitch;
   engine_pan = snd->pan;
   engine_volume = snd->volume;
   engine_state = snd->state;
   engine_select = snd->cmd;

/***** print initial settings **********************************************/

   /* there are two prints here to get fucked up Win 95 scrolling */
   /* to work right */

   printf (" %6s %3s pitch: %03d (%02X)  pan: %03d (%02X)  vol: %03d (%02X)  mvol: %03d (%02X)\r",
            s_str, e_str, engine_pitch, engine_pitch, engine_pan, engine_pan,
            engine_volume, engine_volume, master_volume, master_volume);

   printf (" %6s %3s pitch: %03d (%02X)  pan: %03d (%02X)  vol: %03d (%02X)  mvol: %03d (%02X)\r",
            s_str, e_str, engine_pitch, engine_pitch, engine_pan, engine_pan,
            engine_volume, engine_volume, master_volume, master_volume);

   fflush (stdout);

   dynamic_master_volume (MASTER, master_volume);

   send_engine_parms (engine_select, engine_state, engine_pitch, engine_volume,  engine_pan);


/***** Main loop ***********************************************************/

   done = FALSE;

   while (!done)
      {

      update = FALSE;

	   if (kbhit())
         {
	      key_in = (char) getch ();
         }
	   else
	      key_in = 0;

      switch (key_in)
         {

		   case 72: /* cursor key up, increase engine volume */
			          snd->volume++;
			          if (snd->volume > 255)
				           snd->volume = 255;
                   update = TRUE;
                   break;

		   case 80: /* cursor key down, decrease engine volume */
			          snd->volume--;
			          if (snd->volume < 0)
				           snd->volume = 0;
                   update = TRUE;
                   break;

		   case 75: /* cursor key left, decrease engine pitch */
			          snd->pitch--;
			          if (snd->pitch < 0)
				           snd->pitch = 0;
                   update = TRUE;
                   break;

		   case 77: /* cursor key right, increase engine pitch */
			          snd->pitch++;
			          if (snd->pitch > 255)
				           snd->pitch = 255;
                   update = TRUE;
                   break;

		   case 'p': /* select player */
                   snd = &player;
                   update = TRUE;
                   break;

		   case '1': /* select drone1 */
                   snd = &drone1;
                   update = TRUE;
                   break;

		   case '2': /* select drone2 */
                   snd = &drone2;
                   update = TRUE;
                   break;


		   case 'l': /* pan to the left */
			          snd->pan--;
			          if (snd->pan < 0)
				           snd->pan = 0;
                   update = TRUE;
                   break;

		   case 'e': /* toggle engine sound on and off */
                   if (snd->state == OFF)
                     {
                     snd->state = ON;
                     snd->s_str = on_str;
                     }
                   else
                   if (snd->state == ON)
                     {
                     snd->state = OFF;
                     snd->s_str = off_str;
                     }
                   update = TRUE;
                   break;

		   case 'r': /* pan to the right */
			          snd->pan++;
			          if (snd->pan > 255)
				           snd->pan = 255;
                   update = TRUE;
                   break;

		   case '+': /* increment master volume */
			          master_volume ++;
			          if (master_volume > 255)
				           master_volume = 255;
                   update = TRUE;
                   dynamic_master_volume (MASTER, master_volume);
                   break;

		   case '-': /* decrement master volume */
			          master_volume --;
			          if (master_volume < 0)
				           master_volume = 0;
                   update = TRUE;
                   dynamic_master_volume (MASTER, master_volume);
                   break;

         case 27: /* escape = quit */
		            done = TRUE;
                  stop_sound = TRUE;
			         break;

		   case 'q': /* quit */
			         done = TRUE;
                  stop_sound = FALSE;
			         break;

		   case 'Q': /* quit */
			         done = TRUE;
                  stop_sound = FALSE;
			         break;

         default: /* */
                  break;

         } /* switch */

     
      /* update sound call and display if something changed */

      if ((update) && (!done))
         {

         /* pull the parameters from the active structure */
         s_str = snd->n_str;
         e_str = snd->s_str;
         engine_pitch = snd->pitch;
         engine_pan = snd->pan;
         engine_volume = snd->volume;
         engine_state = snd->state;
         engine_select = snd->cmd;

         /* there are two prints here to get fucked up Win 95 scrolling */
         /* to work right */

         printf (" %6s %3s pitch: %03d (%02X)  pan: %03d (%02X)  vol: %03d (%02X)  mvol: %03d (%02X)\r",
                 s_str, e_str, engine_pitch, engine_pitch, engine_pan, engine_pan,
                 engine_volume, engine_volume, master_volume, master_volume);

         printf (" %6s %3s pitch: %03d (%02X)  pan: %03d (%02X)  vol: %03d (%02X)  mvol: %03d (%02X)\r",
                 s_str, e_str, engine_pitch, engine_pitch, engine_pan, engine_pan,
                 engine_volume, engine_volume, master_volume, master_volume);

         fflush (stdout);

         send_engine_parms (engine_select, engine_state, engine_pitch, engine_volume, engine_pan);

         } /* if update */

      } /* while !done */

   /* turn the thing off at exit */

   if (stop_sound)
      {
      engine_state = OFF;

      engine_select = CMD_PLAYER;
      send_engine_parms (engine_select, engine_state, engine_pitch, engine_volume, engine_pan);

      engine_select = CMD_DRONE1;
      send_engine_parms (engine_select, engine_state, engine_pitch, engine_volume, engine_pan);

      engine_select = CMD_DRONE2;
      send_engine_parms (engine_select, engine_state, engine_pitch, engine_volume, engine_pan);
      }

   printf ("\n\n");

   fflush (stdout);

} /* main () */

/***** END OF main () ******************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: send_engine_parms()                                           */
/*                                                                         */
/* Sends the engine parameters to the V+ sound system in the correct order */
/* and protocol. Change/modify this function to if you want to expand this */
/* to work with the original Cruis'n type engine system.                   */
/*                                                                         */
/***************************************************************************/

void send_engine_parms (unsigned int select, int engine_state, int engine_pitch, 
                        int engine_volume, int engine_pan)

{ /* send_engine_parms() */

unsigned int temp;
int temp_vol;

   if (engine_state == OFF)
      temp_vol = 0;
   else
      temp_vol = engine_volume;

   /* send over the engine parameter protocol */

   /* main engine signifier */
   send_sound_call (select);

   temp = ((engine_pitch & 0xFF) << 8) | (temp_vol & 0xFF);
   send_sound_call (temp);

   temp = engine_pan & 0xFF;
   send_sound_call (temp);

} /* send_engine_parms() */

/***** END of send_engine_parms() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: send_sound_call ()                                            */
/*                                                                         */
/* Sends a single 16-bit word out to the V+ via the SPIEBUS card.          */
/*                                                                         */
/***************************************************************************/

void send_sound_call (sound_call)

unsigned int sound_call;

{ /* send_sound_call() */

unsigned char low_byte;
unsigned char high_byte;

static long delay;

/***************************************************************************/

   /* this here for back-to-back calls to this function */
   /* need delay in between */

   for (delay=0L; delay <= DELAY; delay++);

   low_byte = (unsigned char) (sound_call & 0x00FF);
   high_byte = (unsigned char)	((sound_call >> 8) & 0x00FF);

   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));

   if (debug)
	   printf ("sending 0x%04X\n", (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));


} /* send_sound_call() */

/***** END of send_sound_call() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: dynamic_master_volume (command, volume)                       */
/*                                                                         */
/* Sends master volume, which is a special case in dynamic track mode.     */
/*                                                                         */
/***************************************************************************/

void dynamic_master_volume (command, volume)

unsigned int command;
unsigned int volume;

{ /* dynamic_master_volume () */

unsigned int temp1;
unsigned int temp2;
unsigned int temp3;

static long delay;

/***************************************************************************/

   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)((command) & WORD_MASK));

	if (debug)
	   printf ("sending 0x%04X\n", (unsigned int)((command) & WORD_MASK));

   for (delay=0L; delay <= DELAY; delay++);

	temp1 = ~volume & 0x00FF;
   temp2 = volume << 8;
   temp3 = temp1 | temp2;

   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)((temp3) & WORD_MASK));

	if (debug)
	   printf ("sending 0x%04X\n", (unsigned int)((temp3) & WORD_MASK));

} /* dynamic_master_volume () */

/***** END of dynamic_master_volume() **************************************/

/***** END of file pgas.c **************************************************/



