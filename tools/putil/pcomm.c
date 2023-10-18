#define VERSION "PCOMM version 1.0"

/***************************************************************************/
/*                                                                         */
/* PCOMM.C                                                                 */
/*                                                                         */
/* Sound call sender utility for MIPS platform. Based on previous 16-bit   */
/* program called QCOMM. Compiled with GNU compiler. Uses Mike Lynch       */
/* Psy-Q library to talk to the target via the Psy-Q debugger card.        */
/*                                                                         */
/* This code is ancient sound dept code and is really messy, and all the   */
/* non DCS2 stuff has been ripped out, which makes the flow a little weird */
/* in places.                                                              */
/*                                                                         */
/* See text file PCOMM.DOC for more info.                                  */
/*                                                                         */
/***************************************************************************/

/***** CONSTANTS AND DEFINES ***********************************************/

 #include "pcomm.h"

/***** INCLUDE FILES *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <conio.h>
#include <dos.h>		
#include <process.h>           
#include <signal.h>             
#include <string.h>
#include <bios.h>

/* GNU C specific */
#include <unistd.h>

/* Mike Lynch Psy-Q functions */
#include "/video/tools/putil/lib/pd.h"

typedef struct io_map_info
{
	int	snd_data_write;
	int	snd_data_read;
	int	snd_status;
	int	snd_clear_int;
} io_map_info_t;

static io_map_info_t	io_map_info[] = {
{(0x9<<2), (0xb<<2), (0xa<<2), (0xb<<2)},
{(0xb<<2), (0x9<<2), (0x8<<2), (0x9<<2)},
{(0x9<<2), (0xb<<2), (0xa<<2), (0xb<<2)},
{(0x8<<2), (0xb<<2), (0xa<<2), (0xb<<2)},
{(0xa<<2), (0xb<<2), (0xc<<2), (0xb<<2)},
{(0x1<<2), (0x3<<2), (0x2<<2), (0x3<<2)},
{(0xd<<2), (0xf<<2), (0xe<<2), (0xf<<2)},
{(0x6<<2), (0x4<<2), (0x5<<2), (0x4<<2)}
};


/***** FUNCTION PROTOTYPES *************************************************/


int getline (char[], int);	/* read DOS cmd line */
void draw_prompt (void);	/* user cmd line prompt */			
void waste_time (int);      /* wait for N bios ticks */

void send_sound_call (unsigned int);      
void dynamic_modify (unsigned int, unsigned int, unsigned int);
void dynamic_master_volume (unsigned int, unsigned int);
void send_dynamic_raw (unsigned int);


/***** GLOBAL VARIABLES ****************************************************/

int dynamic;                 /* new V+ dynamic track mode interface */
int print_mode;

unsigned char running_volume = 255;
unsigned char running_pan = 127;
unsigned char running_priority = 0;

int SOUND_DATA_WRITE;
int SOUND_DATA_READ;
int SOUND_STATUS;
int SOUND_CLEAR_INTERRUPT;

char	processor[128];	// Target platform processor
char	platform[128];		// Target platform name

/***** MAIN () *************************************************************/

void main (argc, argv)

int argc;
char *argv [];

{ /* main () */

/***** VARIABLE DEFINITIONS ************************************************/

int usage_error;           /* user needs a clue... provide it */			 
int mode;                  /* sound calls in decimal or hex */

int signal_count;	       /* send master volume after checksum OK back */
int signal_sum;			   /* only send volume after wake up value back */
char mode_string [30];	   /* used to echo settings to user */
char version_string [30];

char key_in;               /* single keystroke typed by user */
int key_pos;               /* location in string built up from keystrokes */

char key_string [MAX_LEN]; /* string that holds what user has typed */

char dos_string [MAX_LEN]; /* DOS command line string from user */

int dos_len;			   /* len of DOS cmd line typed by user */

int done;	               /* whether or not to quit program */
int valid_character;       /* only allow [0..9] [a..f] and [A..F] */
int return_only;           /* user just hit return... repeat last call */

int master_volume;	       /* 8-bit value [0..255] */

int track_zero_volume;     
int track_one_volume;     
int track_two_volume;     
int track_three_volume;    
int track_four_volume;
int track_five_volume;
int track_six_volume;
int track_seven_volume;

unsigned int one_call;
int one_shot;
int straight;

int track_zero_pan;     /* 8-bit value [0..255] 0=left 255=right */
int track_one_pan;     
int track_two_pan;     
int track_three_pan;    
int track_four_pan;
int track_five_pan;
int track_six_pan;
int track_seven_pan;

int num_calls;	                      /* how many calls typed on one line */
unsigned int sound_call [MAX_CALLS];  /* more than one on a line allowed */
int i;                                /* generic counter */

int temp1;                            /* generic temp variables */
int temp2;
unsigned long temp32;

unsigned int return_value;            	/* 16-bits back from the sound board */
unsigned long info_type;				/* what the 16 bits mean */

int	io_map = 0;
int	is_seattle = 0;

/***** DEFAULT SETTINGS ****************************************************/


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
		is_seattle = 1;
	}
	else
	{
		printf("\n***** UNRECOGNIZED PLATFORM: %s *****\n", platform);
		exit(1);
	}

	printf("\nConnected to %s target system\n", platform);

   strcpy (version_string, VERSION);
   strcpy (mode_string, "decimal");

   usage_error = FALSE;
   one_shot = FALSE;
   print_mode = FALSE;
   mode = DECIMAL; 
   dynamic = TRUE;

 
/***** COMMAND LINE ARGUMENTS **********************************************/

	 if (argc > 5) /* up to three options allowed */
	    {
	    fprintf (stderr,"\n Too many command line arguments... check usage.\n");
	    usage_error = TRUE;
	    } 

	 if ((argv [1][0] == '?') || (argv [1][1] == '?'))
	    {
	    usage_error = TRUE; /* display usage mesg if '?' given on cmd line */
	    }

	 i = 1;

	 while (i < argc)
	       {
		if (argv[i][0] == '-')
		   {
		   switch (argv [i][1])
			  {
	        case 'g': /* one shot sound call for use with Brief */
	             		one_shot = TRUE;
			            if (mode == DECIMAL)
			               sscanf (&argv[i][2], "%d", &one_call);
			            if (mode == HEX)
			               sscanf (&argv[i][2], "%x", &one_call);
			            break;     

			  case 'd': mode = DECIMAL;
				         strcpy (mode_string, "decimal");
				         break;

			  case 'h': mode = HEX;
				         strcpy (mode_string, "hex, no prefix");
				         break;

	        case 'p': print_mode = TRUE;
					 		break;

				case 'i':	io_map = atoi(&argv[i][2]);
								if(io_map < 0 || io_map > 7)
								{
									fprintf(stderr, "io map value is out of range\r\n");
									usage_error = TRUE;
								}
								break;

			  default:
	                  fprintf (stderr,"\n Unknown option %s... check usage.\n", argv[i]);
	                  usage_error = TRUE;
				         break;

			  } /* switch */

		   } /* if argv */

		i++;

	   } /* while i */


	if(is_seattle)
	{
		SOUND_DATA_WRITE = 0xB6000000 + io_map_info[io_map].snd_data_write;
		SOUND_DATA_READ = 0xB6000000 + io_map_info[io_map].snd_data_read;
		SOUND_STATUS = 0xB6000000 + io_map_info[io_map].snd_status;
		SOUND_CLEAR_INTERRUPT = 0xB6000000 + io_map_info[io_map].snd_clear_int;
	}

/***** usage error ***********************************************************/

if (usage_error)
   {
   fprintf (stderr, "\n");
   fprintf (stderr, "%s ", version_string);  
   fprintf (stderr, "(compiled "__DATE__" "__TIME__")\n\n");

   fprintf (stderr, " usage: pcomm <options>\n");
   fprintf (stderr, "\n");
   fprintf (stderr, " See text file pcomm.doc for command line options and controls.\n");  
   fprintf (stderr, "\n");
   exit (0);
   }

/***** deal with silent one-shot sound call send ***************************/

 	if (!one_shot)
    	{
    	system ("cls");
    	printf ("\n");
    	printf ("%s\n", version_string);
    	printf ("(sound calls in %s)\n",mode_string);
    	fflush (stdout);
    	}

   if (one_shot)
      {
      send_sound_call (one_call);
      exit(0);
      }

/***** set track pan, volume, etc to default values ************************/

	 master_volume = STARTUP_VOLUME;

	 track_zero_volume = MAXIMUM_VOLUME;
	 track_one_volume = MAXIMUM_VOLUME;
	 track_two_volume = MAXIMUM_VOLUME;
	 track_three_volume = MAXIMUM_VOLUME;
	 track_four_volume = MAXIMUM_VOLUME;
	 track_five_volume = MAXIMUM_VOLUME;
	 track_six_volume = MAXIMUM_VOLUME;
	 track_seven_volume = MAXIMUM_VOLUME;

	 track_zero_pan = CENTER_PAN;
	 track_one_pan = CENTER_PAN;
	 track_two_pan = CENTER_PAN;
	 track_three_pan = CENTER_PAN;
	 track_four_pan = CENTER_PAN;
	 track_five_pan = CENTER_PAN;
	 track_six_pan = CENTER_PAN;
	 track_seven_pan = CENTER_PAN;

/***** START OF MAIN LOOP **************************************************/

   sound_call [1] = 0;
   done = FALSE;
   key_in = '\0'; 
   num_calls = 1;
   signal_count = 0;
   signal_sum = 0;

while (!done) /* done set by escape key, q or Q */
      {

      key_in = 0; /* start with a null character */
      key_pos = 0;   /* start at the 0th position in the string */

		straight = FALSE; // if TRUE, then no dynamic extras this line

      draw_prompt();

      return_only = FALSE; /* flags if user just hits return */

      /* grab characters and put in string until return or done */

      while ((key_in != 13) && (!done)) /* 13 is carriage return */
	     {

	     /* this is the main loop where we will idle most of the time, */
	     /* so also look for values returned from the sound board...   */
	     /* sound board uses SIGNAL command to send us 8-bit values    */

	     /* if the look_for_signal cmd line option was disabled, then  */
	     /* skip this stuff... speeds up number of quick sound calls   */
	     /* sent in succession */

        /* look for return values from the new V+ scheme */

           psyq_mem_read (SOUND_STATUS, &temp32);

           if (temp32 & SOUND_TO_HOST)
              {
 
				  info_type = temp32;

              psyq_mem_read (SOUND_DATA_READ, &temp32);

              return_value = temp32 & 0xFFFF;

				  /* explicitly clear the latch */
              psyq_mem_write (SOUND_CLEAR_INTERRUPT, CLEAR_READ_INTERRUPT);

				  info_type = info_type & INFO_MASK;


				  switch (info_type)
                     {
							case INFO_SIGNAL:
                     if (return_value == INFO_NULLCALL)
                        {
                        printf ("no sound call there");
                        }
                     else
                        {
                        if (mode == HEX)
                           printf ("V+ playlist signal: %04X (hex)", return_value);
                        if (mode == DECIMAL)
                           printf ("V+ playlist signal: %d (decimal)", return_value);
                        }
                     fflush (stdout);
                     break;


							case INFO_TRACKS:
							if (return_value == 0) // no tracks allocated
							   {
                        printf ("V+ no tracks available, sound call not allocated");
							   }
							else
							   {
							   printf ("tracks used:        ");
                        for (i=0; i < 6; i++)
                            {
								   if ((return_value >> (8 + i)) & 0x0001)
                               printf ("%d ", i);
                        	   else
                               printf ("- ");
							       }
							   printf ("\n");

							   printf ("        tracks interrupted: ");
                        for (i=0; i < 6; i++)
                            {
								   if ((return_value >> i) & 0x0001)
                               printf ("%d ", i);
                        	   else
                               printf ("- ");
							       }
							   } /* else */

                     fflush (stdout);
                     break;

 							case INFO_SYNC:
                     if (mode == HEX)
                        printf ("V+ loopback/sync: %04X (hex)", return_value);
                     if (mode == DECIMAL)
                        printf ("V+ loopback/sync: %d (decimal)", return_value);
                     fflush (stdout);
                     break;
                     
 							case INFO_ERROR:
                     if (mode == HEX)
                        printf ("V+ system error: %04X (hex)", return_value);
                     if (mode == DECIMAL)
                        printf ("V+ system error: %d (decimal)", return_value);
                     fflush (stdout);
                     break;

                     default:
                     if (mode == HEX)
                        printf ("V+ return value: %04X (hex)", return_value);
                     if (mode == DECIMAL)
                        printf ("V+ return value: %d (decimal)", return_value);
                     fflush (stdout);
                     break;

							} /* switch */

              key_in = 0; /* start with a null character */
              key_pos = 0; /* start at the 0th position in the string */

              draw_prompt();
              return_only = FALSE; /* flags if user just hits return */
              } /* if bit set */

	     /* read the PC keyboard for key hits */

	     valid_character = FALSE;

	     if (kbhit())
	        key_in = (char) getch ();
	     else
	        key_in = 0;

/***** deal with discrete key hits that have special functions *************/

	     switch (key_in)
		    {
		    case 0: /* function key or mouse - do nothing */
			    break;

		    case '\b': /* backspace */
			       if (key_pos > 0)
				        {
			           putch ('\b');
			           putch (' ');
			           putch ('\b');
			           key_pos--;
				        }
			       break;

		    case '+': /* increment master volume */
			      if (key_pos == 0)
				      {
			         master_volume ++;
			         if (master_volume > 255)
				         master_volume = 255;
				      if (mode == DECIMAL)
			            printf ("master volume: %d", master_volume);
				      if (mode == HEX)
			            printf ("master volume: %02x (%02x)", master_volume, (~master_volume & 0x00FF));

                  dynamic_master_volume (MASTER, master_volume);

			         key_in = 13; /* pretend it was a carriage return */
				      }
               fflush (stdout);
			      break;

		    case '-': /* decrement master volume */
			      if (key_pos == 0)
				      {
			         master_volume --;
			         if (master_volume < 0)
				         master_volume = 0;
				      if (mode == DECIMAL)
			            printf ("master volume: %d", master_volume);
				      if (mode == HEX)
			            printf ("master volume: %02x (%02x)", master_volume, (~master_volume & 0x00FF));

                  dynamic_master_volume (MASTER, master_volume);

			         key_in = 13; /* pretend it was a carriage return */
				      }
               fflush (stdout);
			      break;

    		 case '.': /* < increment sound call */
			      if (key_pos == 0)
			         {
			         if (sound_call [1] < 0x55AA) /* max normal sound call */
			            {
			             sound_call [1] ++;
		                if (mode == DECIMAL)
	                      printf ("%d", sound_call [1]);
		                if (mode == HEX)
	                      printf ("%x", sound_call [1]);

			             send_sound_call (sound_call [1]);
			             key_in = 13;
			             }
			         }
               fflush (stdout);
			      break;

		    case ',': /* > decrement sound call */
			      if (key_pos == 0)
				 	   {
				      if (sound_call [1] > 0)
				         {
				         sound_call [1] --;
		               if (mode == DECIMAL)
	                      printf ("%d", sound_call [1]);
		               if (mode == HEX)
	                      printf ("%x", sound_call [1]);

				         send_sound_call (sound_call[1]);
				         key_in = 13;
				         }
				      }
               fflush (stdout);
			      break;

    		    case '>': /* increment sound call */
			      if (key_pos == 0)
				     {
				     if ((sound_call [1] + 1) < 0x55AA) /* max normal sound call */
				        {
				        send_sound_call (sound_call [1]);
		                if (mode == DECIMAL)
	                       printf ("%d", sound_call [1]);
		                if (mode == HEX)
	                       printf ("%x", sound_call [1]);

				        sound_call [1] ++;

				        key_in = 13;
				        }
				      }
               fflush (stdout);
			      break;

		    case '<': /* decrement sound call */
			      if (key_pos == 0)
				      {
				      if (sound_call [1] >= 1) 
				         {
				         send_sound_call (sound_call[1]);
		                 if (mode == DECIMAL)
	                        printf ("%d", sound_call [1]);
		                 if (mode == HEX)
	                        printf ("%x", sound_call [1]);
				         sound_call [1] --;
				         key_in = 13;
				         }
				      }
               fflush (stdout);
			      break;

		    case 27: /* escape = quit */
		    case 'Q': /* quit */
		    case 'q': /* quit */
			      done = TRUE;
               fflush (stdout);
			      break;

		    case 'x': // send this line only out straight with no
			 case 'X': // V+ dynamic 3-word extra stuff
			      if (key_pos == 0)
				      {
				      valid_character = TRUE;
                  straight = TRUE;
				      }
               fflush (stdout);
			      break;


		    default: /* letter or number */

			     /* this would be easier to do in Pascal */

			     if ((key_in >= 'a') && (key_in <= 'f')) 
				     valid_character = TRUE;
			     if ((key_in >= 'A') && (key_in <= 'F')) 
				     valid_character = TRUE;
			     if ((key_in == 'v') || (key_in == 'V')) 
				     valid_character = TRUE;
			     if ((key_in == 'r') || (key_in == 'R')) 
				     valid_character = TRUE;
			     if ((key_in == 'm') || (key_in == 'M')) 
				     valid_character = TRUE;
			     if ((key_in == 'p') || (key_in == 'P')) 
				     valid_character = TRUE;
			     if ((key_in == 's') || (key_in == 'S')) 
				     valid_character = TRUE;
			     if ((key_in >= '0') && (key_in <= '9')) 
				     valid_character = TRUE;

			     if ((key_in == 'o') || (key_in == 'O')) 
				     valid_character = TRUE;

			     if (key_in == ' ')
                 valid_character = TRUE;

			     if (key_in == 13)
				     valid_character = TRUE;

			     if ((key_in == 13) && (key_pos == 0))
				     {
				     return_only = TRUE;
				     }

		           break;

		    } /* switch */

/***** keep track of what's been typed and act accordingly *****************/

	     /* build up input string from valid characters */
	     if ((!done) && (valid_character) && (key_in != 13))
		     {
           putch (key_in);
	        key_string [key_pos] = key_in;
	        key_pos ++;
		     } /* if */

	     } /* while key_in not carriage return and not done */

	     key_string [key_pos] = '\0'; /* terminate string */


/***** deal with volume calls typed in as vm NNN, v0 NNN, etc. *************/

	     /* if user typed explicit volume call */

	     if ((key_string [0] == 'v') || (key_string [0] == 'V'))
		     {

           valid_character = FALSE;

		     /* check for track 0 thru 7 */
		     if ((key_string [1] >= '0') && (key_string [1] <= '7'))
		        temp1 = key_string [1] - '0';

		     /* check for master volume */
		     if ((key_string [1] == 'm') || (key_string [1] == 'M'))
		        temp1 = -1;

		     if (mode == DECIMAL)
		        sscanf (&key_string [2], "%d", &temp2);
		     if (mode == HEX)
              sscanf (&key_string [2], "%x", &temp2);

           fflush (stdout);

		     if (temp2 > 255)
              temp2 = 255;
		     if (temp2 < 0)
              temp2 = 0;

		     switch (temp1) /* master or track 0 thru 7 */
			     {

			  case -1:
				  master_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        master volume: %d", master_volume);
				  if (mode == HEX)
			        printf ("\n        master volume: %02x (%02x)", master_volume, (~master_volume & 0x00FF));
              fflush (stdout);
              dynamic_master_volume (MASTER, master_volume);
				  break;

           case 0:
				  track_zero_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track zero volume: %d", track_zero_volume);
				  if (mode == HEX)
			        printf ("\n        track zero volume: %02x (%02x)", track_zero_volume, (~track_zero_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      	     break;

           case 1:
				  track_one_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track one volume: %d", track_one_volume);
				  if (mode == HEX)
			        printf ("\n        track one volume: %02x (%02x)", track_one_volume, (~track_one_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      	  	  break;

           case 2:
				  track_two_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track two volume: %d", track_two_volume);
				  if (mode == HEX)
			        printf ("\n        track two volume: %02x (%02x)", track_two_volume, (~track_two_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      	  	  break;

           case 3:
				  track_three_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track three volume: %d", track_three_volume);
				  if (mode == HEX)
			        printf ("\n        track three volume: %02x (%02x)", track_three_volume, (~track_three_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      	  	  break;

           case 4:
				  track_four_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track four volume: %d", track_four_volume);
				  if (mode == HEX)
			        printf ("\n        track four volume: %02x (%02x)", track_four_volume, (~track_four_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      		  break;

           case 5:
				  track_five_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track five volume: %d", track_five_volume);
				  if (mode == HEX)
			        printf ("\n        track five volume: %02x (%02x)", track_five_volume, (~track_five_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      		  break;

           case 6:
				  track_six_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track six volume: %d", track_six_volume);
				  if (mode == HEX)
			        printf ("\n        track six volume: %02x (%02x)", track_six_volume, (~track_six_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      		  break;

           case 7:
				  track_seven_volume = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track seven volume: %d", track_seven_volume);
				  if (mode == HEX)
			        printf ("\n        track seven volume: %02x (%02x)", track_seven_volume, (~track_seven_volume & 0x00FF));
              dynamic_modify (DYN_VOLUME, temp1, temp2);
              fflush (stdout);
      		  break;


			  default:
            break;

			  } /* switch */

		} /* if 'v' or 'V' for volume */

/***** deal with pan calls typed in as p0 NNN, p1 NNN, etc. ****************/

	     /* if user typed explicit pan call */

	     if ((key_string [0] == 'p') || (key_string [0] == 'P'))
		     {

           valid_character = FALSE;

			  temp1 = -1;

		     /* check for track 0 thru 7 */
		     if ((key_string [1] >= '0') && (key_string [1] <= '7'))
		        temp1 = key_string [1] - '0';

		     if (mode == DECIMAL)
		        sscanf (&key_string [2], "%d", &temp2);
		     if (mode == HEX)
              sscanf (&key_string [2], "%x", &temp2);

		     if (temp2 > 255)
              temp2 = 255;
		     if (temp2 < 0)
              temp2 = 0;

		     switch (temp1) /* track 0 thru 7 */
			       {
            case 0:
				   track_zero_pan = temp2;
				   if (mode == DECIMAL)
			         printf ("\n        track zero pan: %d", track_zero_pan);
				   if (mode == HEX)
			         printf ("\n        track zero pan: %02x (%02x)", track_zero_pan, (~track_zero_pan & 0x00FF));
               dynamic_modify (DYN_PAN, temp1, temp2);
               fflush (stdout);
      			break;

            case 1:
				  track_one_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track one pan: %d", track_one_pan);
				  if (mode == HEX)
			        printf ("\n        track one pan: %02x (%02x)", track_one_pan, (~track_one_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

            case 2:
				  track_two_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track two pan: %d", track_two_pan);
				  if (mode == HEX)
			        printf ("\n        track two pan: %02x (%02x)", track_two_pan, (~track_two_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

           case 3:
				  track_three_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track three pan: %d", track_three_pan);
				  if (mode == HEX)
			        printf ("\n        track three pan: %02x (%02x)", track_three_pan, (~track_three_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

           case 4:
				  track_four_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track four pan: %d", track_four_pan);
				  if (mode == HEX)
			        printf ("\n        track four pan: %02x (%02x)", track_four_pan, (~track_four_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

           case 5:
				  track_five_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track five pan: %d", track_five_pan);
				  if (mode == HEX)
			        printf ("\n        track five pan: %02x (%02x)", track_five_pan, (~track_five_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

           case 6:
				  track_six_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track six pan: %d", track_six_pan);
				  if (mode == HEX)
			        printf ("\n        track six pan: %02x (%02x)", track_six_pan, (~track_six_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

              case 7:
				  track_seven_pan = temp2;
				  if (mode == DECIMAL)
			        printf ("\n        track seven pan: %d", track_seven_pan);
				  if (mode == HEX)
			        printf ("\n        track seven pan: %02x (%02x)", track_seven_pan, (~track_seven_pan & 0x00FF));
              dynamic_modify (DYN_PAN, temp1, temp2);
              fflush (stdout);
      		  break;

			     default:
              break;

			  } /* switch */

		} /* if 'p' or 'P' for pan */


/***** new V+ stop track typed as s0, s1, s2 etc. **************************/

	  /* if user typed explicit stop call */

	     if ((key_string [0] == 's') || (key_string [0] == 'S'))
		     if ((key_string [1] >= '0') && (key_string [1] <= '7'))

		     {

           valid_character = FALSE;

		     /* check for track 0 thru 7 */
		     temp1 = key_string [1] - '0';

		     dynamic_modify (DYN_STOP, temp1, 0);

		     switch (temp1) /* track 0 thru 7 */
			       {
              case 0:
			             printf ("\n        track zero stopped");
      		  break;

              case 1:
			             printf ("\n        track one stopped");
      		  break;

              case 2:
			             printf ("\n        track two stopped");
      		  break;

              case 3:
			             printf ("\n        track three stopped");
      		  break;

              case 4:
			             printf ("\n        track four stopped");
      		  break;

              case 5:
			             printf ("\n        track five stopped");
      		  break;

              case 6:
			             printf ("\n        track six stopped");
      		  break;

              case 7:
			             printf ("\n        track seven stopped");
      		  break;

			     default:
              break;

			  } /* switch */

      fflush (stdout);

		} /* if 's' or 'S' for stop */


/***** new V+ set volume, pan and priority as sv N, sp N, sr N *************/

	     if ((key_string [0] == 's') || (key_string [0] == 'S'))
		     {

			  if ((key_string [1] == 'v') || (key_string [1] == 'V'))
				  {
              valid_character = FALSE;

			     temp1 = -1;

				  if (mode == DECIMAL)
		           sscanf (&key_string [2], "%d", &temp1);
				  if (mode == HEX)
		           sscanf (&key_string [2], "%x", &temp1);

				  running_volume = (unsigned char) (temp1);
              if (running_volume > 255) running_volume = 255;

				  if (mode == DECIMAL)
			        printf ("\n        running track volume: %d", running_volume);
				  if (mode == HEX)
			        printf ("\n        running track volume: 0x%02X", running_volume);

              fflush (stdout);

				  } /* s */

			  if ((key_string [1] == 'p') || (key_string [1] == 'P'))
				  {
              valid_character = FALSE;

			     temp1 = -1;

				  if (mode == DECIMAL)
		           sscanf (&key_string [2], "%d", &temp1);
				  if (mode == HEX)
		           sscanf (&key_string [2], "%x", &temp1);

				  running_pan = (unsigned char) (temp1);

              if (running_pan > 255) running_pan = 255;

				  if (mode == DECIMAL)
			        printf ("\n        running track pan: %d", running_pan);
				  if (mode == HEX)
			        printf ("\n        running track pan: 0x%02X", running_pan);

              fflush (stdout);

				  } /* p */

			  if ((key_string [1] == 'r') || (key_string [1] == 'R'))
				  {
              valid_character = FALSE;

			     temp1 = -1;

				  if (mode == DECIMAL)
		           sscanf (&key_string [2], "%d", &temp1);
				  if (mode == HEX)
		           sscanf (&key_string [2], "%x", &temp1);

				  running_priority = (unsigned char) (temp1);

              if (running_priority > 127) running_priority = 127;

				  if (mode == DECIMAL)
			        printf ("\n        running track priority: %d", running_priority);
				  if (mode == HEX)
			        printf ("\n        running track priority: 0x%02X", running_priority);

              fflush (stdout);

				  } /* r */

		} /* if set 'running' value */


/***** shell out to DOS ****************************************************/


		  if ((strcmp (&key_string[0], "dos") == 0) ||
				(strcmp (&key_string[0], "DOS") == 0))
			  {
           valid_character = FALSE;
	        printf ("\n        enter DOS command: ");
           fflush (stdout);

			  /* leave room for quotes at start and end */
			  dos_len = getline (dos_string, (MAX_LEN-3));

			  /* lop off the carriage return at the end */
			  dos_string [dos_len-1] = '\0';

			  system (dos_string);
        	  fflush (stdout);

			  } /* if "dos" or "DOS" on command line */


/***** now that we have sound calls, get them and send them ****************/

	     /* it's a normal sound call */
		 
	     if ((mode == DECIMAL) && (!return_only) && (valid_character))
		   {

		   /* read in up to 8 calls from the line */
		   /* num_calls tells us how many were actually there */

			if (straight)
				{
		      num_calls = sscanf (key_string + 1, "%u %u %u %u %u %u %u %u",
                     &sound_call[1], &sound_call[2], &sound_call[3], &sound_call[4],
                     &sound_call[5], &sound_call[6], &sound_call[7], &sound_call[8]);

		      for (i=1; i <= num_calls; i++)
		      send_dynamic_raw (sound_call[i]);
				}
			else
				{
		      num_calls = sscanf (key_string, "%u %u %u %u %u %u %u %u",
                        &sound_call[1], &sound_call[2], &sound_call[3], &sound_call[4],
                        &sound_call[5], &sound_call[6], &sound_call[7], &sound_call[8]);

		      for (i=1; i <= num_calls; i++)
		      send_sound_call (sound_call[i]);
				}


		   /* check if the user did a discrete change to either the */
		   /* master volume or the track volume */
		   for (i=1; i <= num_calls; i++)
		       {
		       if (sound_call [i] == 0x55aa)
                         master_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55ab)
                         track_zero_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55ac)
                         track_one_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55ad)
                         track_two_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55ae)
                         track_three_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55af)
                         track_four_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55b0)
                         track_five_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55b1)
                         track_six_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55b2)
                         track_seven_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       } /* for i */
		   } /* if mode is decimal */

	     else
	        if ((mode == HEX) && (!return_only) && (valid_character))
		   		{

				if (straight)
					{
		      	num_calls = sscanf (key_string + 1, "%x %x %x %x %x %x %x %x",
                     &sound_call[1], &sound_call[2], &sound_call[3], &sound_call[4],
                     &sound_call[5], &sound_call[6], &sound_call[7], &sound_call[8]);

		      	for (i=1; i <= num_calls; i++)
		         	  send_dynamic_raw (sound_call[i]);
					}
				else
					{
		      	num_calls = sscanf (key_string, "%x %x %x %x %x %x %x %x",
                     &sound_call[1], &sound_call[2], &sound_call[3], &sound_call[4],
                     &sound_call[5], &sound_call[6], &sound_call[7], &sound_call[8]);

		         for (i=1; i <= num_calls; i++)
		         	  send_sound_call (sound_call[i]);

				} /* if */


		   /* check if the user did a discrete change to either the */
		   /* master volume or the track volume */
		   for (i=1; i <= num_calls; i++)
		       {
		       if (sound_call [i] == 0x55aa)
                         master_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55ab)
                         track_zero_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55ac)
                         track_one_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55ad)
                         track_two_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55ae)
                         track_three_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55af)
                         track_four_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55b0)
                         track_five_volume = (sound_call [i+1] >> 8) & 0x00FF ;
		       if (sound_call [i] == 0x55b1)
                         track_six_volume = (sound_call [i+1] >> 8) & 0x00FF;
		       if (sound_call [i] == 0x55b2)
                         track_seven_volume = (sound_call [i+1] >> 8) & 0x00FF;

		       } /* for i */

		   } /* if mode is hex */


/***** hitting return only repeats the last call sent, except for volumes ****/

        /* repeat last call (last on line if more than one) */
	     if (return_only)
           {

		     if (mode == DECIMAL)
	           printf ("%d", sound_call [num_calls]);
		      if (mode == HEX)
	            printf ("%x", sound_call [num_calls]);
            fflush (stdout);

		      send_sound_call (sound_call [num_calls]);

		      } /* if return only */

      } /* while not done */

/****************************************************************************/

   printf ("\n\n");
   fflush (stdout);
   exit (0);

} /* main () */

/***** END OF main () *******************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: getline()                                                     */
/*                                                                         */
/* Reads characters from console until carriage return or end of max line  */
/* length.                                                                 */
/*                                                                         */
/* Returns number of characters read.                                      */
/*                                                                         */
/***************************************************************************/

int getline (string, max_length)

char string [];
int max_length;

{ /* getline () */

int c;
int i;

/***************************************************************************/

   i = 0;

   while ((--max_length > 0) && ((c=getchar()) != EOF) && (c != '\n'))
		   string [i++] = (char) c;

   if (c == '\n')
      string [i++] = (char) c;

   string [i] = '\0';

   return (i);

} /* getline () */

/***** END of getline() ****************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: waste_time ()                                                 */
/*                                                                         */
/* Loop to waste time for delays. Each unit of delay time is equal to one  */
/* bios tick (55 ms between bios ticks gives roughly 18 ticks per second). */
/*                                                                         */
/***************************************************************************/

void waste_time (how_long)

int how_long;

{ /* waste_time () */

long entry_time;
long current_time;

_bios_timeofday (_TIME_GETCLOCK, &entry_time);

do {
   _bios_timeofday (_TIME_GETCLOCK, &current_time);
   }
   while (abs((int)(current_time - entry_time)) <= how_long);

} /* waste_time () */

/***** END of waste_time() *************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: send_sound_call ()                                            */
/*                                                                         */
/* Break the 16-bit call into two 8-bit bytes, and send out parallel port  */
/* with strobe.                                                            */
/*                                                                         */
/***************************************************************************/

void send_sound_call (sound_call)

unsigned int sound_call;

{ /* send_sound_call() */

unsigned char low_byte;
unsigned char high_byte;

static int delay;

/***************************************************************************/

   /* this here for back-to-back calls to this function */
   /* need delay in between */
   for (delay=0L; delay <= DELAY; delay++);

   low_byte = (unsigned char) (sound_call & 0x00FF);
   high_byte = (unsigned char)	((sound_call >> 8) & 0x00FF);
	/* first word is the 16-bit sound call number */
   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));
	if (print_mode)
      {
	   printf ("\n        sending 0x%04X\n", (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));
      fflush (stdout);
      }


	/* second word is the 8-bit volume + 8-bit pan */
	/* set vol to max and pan to middle */
   high_byte = running_volume;
   low_byte = running_pan;
   if (sound_call == 0)
      {
      high_byte = 0;
      low_byte = 0;
      }
   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));
	if (print_mode)
      {
	   printf ("        sending 0x%04X\n", (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));
      fflush (stdout);
      }


	/* third word is the priority... default set it to highest priority */
	// as of 8 Aug 95 priority is now lowest 7 bits
	low_byte = running_priority & 0x7F;
   high_byte = 0;
   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));
	if (print_mode)
      {
	   printf ("        sending 0x%04X", (unsigned int)(((high_byte << 8) | (low_byte)) & WORD_MASK));
      fflush (stdout);
      }


} /* send_sound_call() */

/***** END of send_sound_call() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: dynamic_modify (command, track, value)                        */
/*                                                                         */
/* Sends "post-modifier" sound calls to V+ using new interface scheme.     */
/*                                                                         */
/* Commands are one of:                                                    */
/*                                                                         */
/*    host track volume                                                    */
/*                                                                         */
/*    host track pan                                                       */
/*                                                                         */
/*    stop a track                                                         */
/*                                                                         */
/***************************************************************************/

void dynamic_modify (command, track, value)

unsigned int command;
unsigned int track;
unsigned int value;

{ /* dynamic_modify () */

unsigned char track_mask;
unsigned char track_value;
unsigned int temp1;

static int delay;

/***************************************************************************/

	/* track tells which track 0..7 the command applies to */
   /* set the appropriate bit 0..7 to show the track number */

	track_mask = (0x01 << track) & 0x00FF;
   track_value = (unsigned char) (value & 0x00FF);
	temp1 = (track_mask << 8) | track_value;
   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)((command) & WORD_MASK));
	if (print_mode)
      {
	   printf ("\n        sending 0x%04X\n", (unsigned int)((command) & WORD_MASK));
      fflush (stdout);
      }

   for (delay=0L; delay <= DELAY; delay++);

   /* send the pan, volume or stop command */
   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)((temp1) & WORD_MASK));
	if (print_mode)
      {
	   printf ("        sending 0x%04X", (unsigned int)((temp1) & WORD_MASK));
      fflush (stdout);
      }

} /* dynamic_modify () */

/***** END of dynamic_modify() *********************************************/

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

static int delay;

/***************************************************************************/

   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)((command) & WORD_MASK));

	if (print_mode)
      {
	   printf ("\n        sending 0x%04X\n", (unsigned int)((command) & WORD_MASK));
      fflush (stdout);
      }

   for (delay=0L; delay <= DELAY; delay++);

	temp1 = ~volume & 0x00FF;
   temp2 = volume << 8;
   temp3 = temp1 | temp2;

   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)((temp3) & WORD_MASK));

	if (print_mode)
      {
	   printf ("        sending 0x%04X", (unsigned int)((temp3) & WORD_MASK));
      fflush (stdout);
      }

} /* dynamic_master_volume () */

/***** END of dynamic_master_volume() **************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: send_dynamic_raw (sound_call)                                 */
/*                                                                         */
/* Sends out a single word in dynamic mode with no extras. Mostly for      */
/* debugging.                                                              */
/*                                                                         */
/***************************************************************************/

void send_dynamic_raw (sound_call)

unsigned int sound_call;

{ /* send_dynamic_raw () */

   psyq_mem_write (SOUND_DATA_WRITE, (unsigned int)(sound_call & WORD_MASK));

	if (print_mode)
      {
	   printf ("\n        sending 0x%04X", (unsigned int)(sound_call & WORD_MASK));
      fflush (stdout);
      }

} /* send_dynamic_raw () */

/***** END of send_dynamic_raw() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: draw_prompt()                                                 */
/*                                                                         */
/* Function to draw the command line prompt when compiling with GNU C,     */
/* to deal with UNIX style stdout.                                         */
/*                                                                         */
/***************************************************************************/

void draw_prompt (void)

{ /* draw_prompt() */

   printf ("\n pcomm> ");
   fflush (stdout);

} /* draw_prompt() */

/***** END of draw_prompt() ************************************************/

/***** END of file pcomm.c *************************************************/


