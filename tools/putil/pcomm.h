/***************************************************************************/
/*                                                                         */
/* FILE: defines.h                                            20 Jan 93 mb */
/*                                                                         */
/* 11 Mar 97 MVB - There are a lot of defines in here that are no longer   */
/* used by PCOMM since all the pinball and old video stuff has been        */
/* stripped out.                                                           */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Contains the defines and equates used by qcomm and qscam.               */
/*                                                                         */
/***************************************************************************/
/*                                                                         */
/* Make sure that QCOMM and QSCAM have the latest version of this since    */
/* there are defines in here for both, but not all are used by both.       */
/*                                                                         */
/***************************************************************************/

#define  YES       1
#define  NO        0
#define  NONE	    0
#define  TRUE      1
#define  FALSE     0

#define SPEED 0x23CB  /* sets the system clock to 7.68ms */

#define  NAME_LENGTH 20		 /* max length of script file name  */
#define  LINE_LENGTH 80		 /* max length of input file line   */        

#define  MAX_LINES 999                  /* max num of lines in a script    */  

#define  STACK_SIZE 200		 /* max number of stack locations   */

/* these parallel port data and status register addresses are for computers */
/* that have the parallel port on the mother board... machines using a      */
/* separate parallel port card or a monitor card will need a new address    */

#define  DATA_REGISTER 0x0378   /* address of parallel port data register */             
#define  STATUS_REGISTER 0x37a  /* parallel port status register */
#define  STROBE_HIGH 0x00       /* this is inverted so high is a zero... */	         
#define  STROBE_LOW 0x01        /* ...and low is a one */            

#define  BASE 0x120           /* base address of plug-in card */
#define  RESET_VECTOR 0x801C  /* BASE + RESET = reset location */	
#define  DATA_VECTOR 0x1C     /* BASE + DATA = 8-bit data location */
#define  STATUS_VECTOR 0x1E   /* BASE + STATUS = flag data is ready to get */
#define  NOP 0x5800           /* ADSP-2105 no op to put in port on boot-up */
#define  BOOT_PAGE 0x5A00     /* tell monitor to boot ROM page 80 */
#define  RESET_HIGH 0x01      /* send this first to 2105 and hold for... */ 
#define  RESET_LOW 0x00       /* about a msec and then send this */

#define  TIME_OUT 500000L   /* board reset time-out value */
#define  BYTE_READY 0x80   /* bit 7 is flag for byte ready from card */
#define  ALIVE 0x1000      /* board sends back 0x1000 to say it's OK */

#define  DELAY 100L    /* delay between bytes sent */ 

#define  EXTERNAL 0    /* stand-alone sound board via PC parallel port */
#define  INTERNAL 1    /* plug-in development card via game port */
#define  T_UNIT 2      /* T-Unit using dedicated sound port */
#define  T_AUX 3       /* T-Unit using aux port */
#define  X_UNIT 4      /* X-Unit to DCS via UART */
#define  WWF_UNIT 5    /* TX Hybrid Mutant for Wrestling Video per Flanders */
#define  V_PLUS 6      /* talk to V+ via SPIEBUS card */

#define SOUND_LATCH  0x1E00000   /* addresses for GSP to sound board comm */
#define TSOUND_LATCH 0x1d01030	/* these taken from Petro's SOUND program */
#define ASOUND_LATCH 0x1d01010

#define WWF_DATA 0x1680000   /* added 1 June 94 */ 
#define WWF_RESET 0x1860010
#define WWF_STATUS 0x1860040

#define WWF_START_VOLUME 50 /* per Skiles... "It's comfortable." */

#define CLEAR_READ_INTERRUPT 0x0000 /* for TI 'C31 mode */

// old V+ addresses
//#define SOUND_DATA_WRITE 0x990009
//#define SOUND_DATA_READ 0x99000B
//#define SOUND_STATUS 0x99000A
//#define SOUND_CLEAR_INTERRUPT 0x99000B

//new MIPS/PCI addresses
//#define SOUND_DATA_WRITE 0xB5000048 
//#define SOUND_DATA_READ 0xB5000058
//#define SOUND_STATUS 0xB5000050
//#define SOUND_CLEAR_INTERRUPT 0xB5000058

#define HOST_TO_SOUND 0x0080
#define SOUND_TO_HOST 0x0040

#define BYTE_MASK 0x000000FF
#define WORD_MASK 0x0000FFFF

/* used in new V+ scheme to decode type of data coming back to host */
/* from sound DSP */

#define INFO_SIGNAL     0x0100
#define INFO_TRACKS     0x0200
#define INFO_CHECKSUM   0x0300
#define INFO_SYNC 	  	0x0400
#define INFO_ERROR		0x0700
#define INFO_MASK       0x0700
#define INFO_NULLCALL   0xEEEE

#define  DECIMAL 0	    /* enter calls in decimal... */
#define  HEX 1		       /* ... or in hex */
#define  MAX_LEN 50	    /* maximum input line length */

#define  MASTER 0x55AA       /* reserved sound call for master vol  */

#define  TRACK_ZERO 0x55AB   /* reserved sound call for track 0 vol */
#define  TRACK_ONE 0x55AC    /* reserved sound call for track 1 vol */
#define  TRACK_TWO 0x55AD    /* reserved sound call for track 2 vol */
#define  TRACK_THREE 0x55AE  /* reserved sound call for track 3 vol */
#define  TRACK_FOUR 0x55AF
#define  TRACK_FIVE 0x55B0
#define  TRACK_SIX 0x55B1
#define  TRACK_SEVEN 0x55B2

#define  PAN_ZERO 0x55BA     /* reserved sound call for track zero pan */
#define  PAN_ONE 0x55BB      /* reserved sound call for track one pan */
#define  PAN_TWO 0x55BC      /* reserved sound call for track two pan */
#define  PAN_THREE 0x55BD    /* reserved sound call for track three pan */
#define  PAN_FOUR 0x55BE
#define  PAN_FIVE 0x55BF
#define  PAN_SIX 0x55C0
#define  PAN_SEVEN 0x55C1

#define	DYN_VOLUME 0x55AB   /* dynamic track mode host track volume */	
#define	DYN_PAN 0x55AC		  /* dynamic track mode host track pan */
#define	DYN_STOP 0x55AE	  /* dynamic track mode host stop a track */

#define  STARTUP_VOLUME 103   /* sends this and defaults to it */
#define  MAXIMUM_VOLUME 255   /* maximum possible track and master volume */
#define  MAX_CALLS 10         /* maximum number of calls on a line + 2 */
#define  CENTER_PAN 127       /* 0=left 255=right */

/***** stuff for V+ via SPIEBUS ********************************************/

#define PRIMEBUS     0x0808064
#define DSPRUN	      0x8000    /* bit 15: 0 = hold and 1 = run */
#define DAENZ        0x2000    /* bit 13: 1 = disable bus... ALWAYS ON */

#define HSTDAT_IO		0x0000
#define HSTCTL_IO		0x1000
#define HSTADRL_IO	0x2000
#define HSTADRH_IO	0x3000
#define HODD			0x4000    /* bit 14: 0 = low word and 1 = high word */

/***** END of file defines.h ***********************************************/

