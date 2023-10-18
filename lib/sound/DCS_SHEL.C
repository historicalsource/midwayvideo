#include <goose/sound.h>
#include <goose/process.h>
#include <goose/sound.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include <ioctl.h>  // used for kbhit()

#define MAX_COMMAND_SIZE  80
#define CHAR_LINEFEED  0x10
#define CHAR_CARRIAGE  0xA
#define END_CHAR  0x7F
#define ERROR 0
#define OK 1

char small_cmd[] = "?,.+-/hHlLrRqQtTvV\n";

//int stream_detect_fifos(void);
int get_command(char *command_word);
int kbhit(void);
int dcs_shell_print_help(void);
extern int dcs_shell(void);

int current_scall=0;
int current_volume = 140;

int dcs_shell(void)
/************************************************
** simple program to exploit the sound API
************************************************/
{
   char command_string[MAX_COMMAND_SIZE];
   dcs_shell_print_help();
   snd_master_volume(current_volume);
   fprintf(stderr, "master volume set to %i\n", current_volume);

   while (0 == 0) {

		fprintf(stderr, "dcs_shell>  ");
      get_command(command_string);

      switch (command_string[0]) {
         case 'q':
         case 'Q':
            printf("dcs_shell():bye\n");
            return(0);
         case 'h':
         case 'H':
         case '?':
            dcs_shell_print_help();
            break;
         case '\n':
         case '/':
            snd_scall_quick(current_scall);
            fprintf(stderr, "\b%i\n", current_scall);
            break;
         case ',':
            if (current_scall > 0) current_scall--;
            snd_scall_quick(current_scall);
            fprintf(stderr, "\b%i\n", current_scall);
            break;
         case '.':
            snd_scall_quick(++current_scall);
            fprintf(stderr, "\b%i\n", current_scall);
            break;
         case 'r':
         case 'R':
            //snd_load_dcs2();
            break;
         case '0': case '1': case '2': case '3': case '4': case '5':
         case '6': case '7': case '8': case '9':
            sscanf(command_string, "%i", &current_scall);
            if (current_scall == 0) {
            	snd_stop_all();
			} else {
            	snd_scall_quick(current_scall);
            }
            fprintf(stderr,"\n");
            break;
         case 'a':
         case 'A':
            fprintf(stderr,"\nLoading '%s'.BNK...\n", &command_string[2]);
            if (snd_bank_load(&command_string[2]) == ERROR)
               printf("dcs_shell(): ERROR attempting to load '%s.BNK'\n",
                      &command_string[2]);
            break;
         case 'd':
         case 'D':
            fprintf(stderr,"\nDeleting '%s'.BNK...\n", &command_string[2]);
            if (snd_bank_delete(&command_string[2]) != OK)
               printf("dcs_shell(): ERROR attempting to delete '%s.BNK'\n",
                      &command_string[2]);
            break;
         case 'l':
         case 'L': 
            snd_bank_showlist(); 
            break;            
#ifdef SHELL_STREAMING  // not implemented this time around!...yet
         case 's':
         case 'S':
            fprintf(stderr,"\nStreaming '%s.SND'...\n", &command_string[2]);
            strcat(&command_string[2],".SND");
		      stream_init(&command_string[2]);
		      sleep(3);
		      snd_stream_volume(205);
		      stream_start();
            break;
         case 't':
         case 'T':
            if (stream_detect_fifos() == 0)
               fprintf(stderr,"\bFIFO'S NOT PRESENT\n");
            else
               fprintf(stderr,"\bFIFO'S ARE PRESENT\n");
//            snd_load_dcs2();
            break;
#endif
         case 'v':
         case 'V':
            fprintf(stderr,"\bsound opsys version %x\n",
                     snd_get_opsys_ver());
            break;
         case '-':
            if (current_volume > 0) current_volume--;
            snd_master_volume(current_volume);
            fprintf(stderr, "\bmaster volume:%i\n", current_volume);
            break;
         case '+':
            if (current_volume <255) current_volume++;
            snd_master_volume(current_volume);
            fprintf(stderr, "\bmaster volume:%i\n", current_volume);
            break;
         default:
            fprintf(stderr,"  invalid command: '%s'\n",command_string);            
      } // switch


   } // while
} //dcs_shell()

int dcs_shell_print_help(void)
{
   printf("\r\n"
   " ----------------------------------------------------------------------\r\n"
   " -- DCS SHELL\r\n"
   " -- Andrew Eloff, Audio Development Engineer, Midway Games, Inc.\r\n"
   " ----------------------------------------------------------------------\r\n"
   " -- TYPE:        TO:\r\n"
   " -- q            exit\r\n"
   " -- h            print this help message\r\n"
   " -- , (.)        decrement (increment) and send next scall\r\n"
   " -- /            repeat last scall\r\n"
   " -- <scall#>     set current scall to <scall#> and send\r\n"
   " -- r            reset DCS2-HD\r\n"
   " -- a <bankfile> load <bankfile> to DCS\r\n"
   " -- d <bankfile> delete <bankfile> from DCS\r\n"
   " -- l            list all banks loaded\r\n"
   " -- s <sndfile>  stream <sndfile> through FIFO's\r\n"
   " -- v            return operating system version\r\n"
   " -- - (+)        lower (raise) master volume\r\n"
   " -----------------------------------------------------------------------\r\n");
   return(0);
} //dcs_shell_print_help()

int get_command(char *command_word )
/************************************************
** suck down a command from the host - note 
** support for "quick" (1-char) commands
************************************************/
{
   int iIndex;
   char c;

   /* first, look at the first character.  If it's a special character */
   /* then exit, otherwise, read in the full line until <CR> */
   while (!kbhit()) {
         fprintf(stderr," \b");
         sleep(1);
   }
   c = getchar();
	fprintf(stderr, "%c", c);
   command_word[0]=c;
   for (iIndex = 0; small_cmd[iIndex] != '\0' ; iIndex ++) {
      if (c == small_cmd[iIndex]) {
         command_word[1] = '\0';
         return(0);
      } // if
   } // for
 
   /* it's not a one-worder, so read in the rest of the chars until a */
   /* <CR> */
   iIndex = 1;
   while (0 == 0) {
      while (!kbhit()) {
         fprintf(stderr," \b");
         sleep(1);
      }
      c = getchar();

      if (c == '\b') 
         if (iIndex>=1) iIndex--;

      if ((c == CHAR_LINEFEED) || (c == CHAR_CARRIAGE)) {
         command_word[iIndex] = '\0';
         return(0);

      } // if
      command_word[iIndex++] = c;
      if (iIndex > MAX_COMMAND_SIZE) {
         command_word[iIndex-1] = '\0';
         return(0);
      }
	   fprintf(stderr, "%c", c);
   }   
} // get_command()   

int kbhit(void)
/************************************************
** Similar in every way to the PC function ...
************************************************/
{
   int iResponse;
   _ioctl (0, TIOCKEYHIT, (int)&iResponse);
   if (iResponse == 0) 
      return(0);
   else
      return(1);
} //kbhit()

