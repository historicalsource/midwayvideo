/***************************************************************************/
/*                                                                         */
/* SOUND.C                                                                 */
/*                                                                         */
/* Lower and middle level functions for DCS2 downloadable sound system.    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* by Matt Booty, Ed Keenan and Mike Lynch                                 */
/*                                                                         */
/* 06 Jan 97                                                               */
/*                                                                         */
/* MVB 04 Feb 97 - added functions for sample playback "engine" sounds     */
/* MVB 19 Feb 97 - streaming bugs fixed, better error handling             */
/* MVB 24 Mar 97 - snd call lockout while loading, correct load err hndl   */
/* MVB 01 Jul 97 - added separate debugging for normal, stream and engine  */
/* MVB 08 Jul 97 - added sound bank checksum routines, needs version       */
/*                 0118 of the sound system                                */
/* MVB 28 Aug 97 - fixed reset of snd_table_addr and snd_bank_addr         */
/* 15 Sep 97 MVB - fixed SOUNDSTATUS bug                                   */
/* 25 Nov 97 EJK - change snd_reset & snd_reset_ack to only modify bit 0   */
/* 30 Nov 97 MVB - support diff o/s files, support Eloff FIFO streaming    */
/*                                                                         */
/* $Revision: 42 $                                                         */
/*                                                                         */
/***************************************************************************/

/* set these to 1 to turn on verbose debugging info */

/* use this to trace bank loading and sound call sending */
#if 0
#if defined(RELEASE)
#define SND_DEBUG 0
#else
#define	SND_DEBUG	1
#endif
#else
#define SND_DEBUG 0
#endif

/* use this to trace streaming functions */
#define SND_DEBUG_STREAMING 0

/* use this to debug sample-playback engine functions */
#define SND_DEBUG_ENGINE 0

/* use to report data - added for hunting down NFL problem - 19 Sep 97 */
#define SND_DEBUG_ISR_TIMEOUT 0

/***** INCLUDES ************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>
#include <fcntl.h>
#if defined(SEATTLE)
#include <ioctl.h>
#endif
#include <goose/sound.h>
#include <goose/process.h>

unsigned long crc(unsigned char *, int);
#if defined(VEGAS)
unsigned int get_timer_val(void);
#endif

char	sound_sound_c_version[] = {"$Revision: 42 $"};

/***** LOCAL DEFINES *******************************************************/

#define OK 0
#define ERROR 0xEEEE
#define NULL 0
#define MAX_NAME 20
#define SND_BNK_EXT ".BNK"   /* all banks have .BNK extension */
#define SND_SND_EXT ".SND"   /* all stream files have .SND extension */

#define SND_BYTE0  0x000000FF
#define SND_BYTE1  0x0000FF00
#define SND_BYTE2  0x00FF0000
#define SND_BYTE3  0xFF000000
#define SND_MASK32 0xFFFFFFFF
#define SND_MASK16 0x0000FFFF
#define SND_MASK8  0x000000FF
#define SND_TOP16  0xFFFF0000

/* I/O ASIC addresses for Phoenix system */
//#define SND_IOA_MCTRL  IOASIC_CONTROL    			/* I/O ASIC main control register */
//#define SND_CONTROL    IOASIC_SOUND_CONTROL      /* I/O ASIC sound control register */   
//#define SND_HTS_DATA   IOASIC_SOUND_DATA_OUT     /* host-to-sound data register */
//#define SND_HTS_STATUS IOASIC_SOUND_STATUS  		/* host-to-sound status register */
//#define SND_STH_DATA   IOASIC_SOUND_DATA_IN      /* sound-to-host data register */

/* status register bit fields */
#define SND_MONITOR_READY  0x0A      /* ack returned by snd boot rom */
#define SND_OPSYS_READY    0x0C      /* ack returned by snd op sys */
#define STH_DATA_READY     0x0040    /* snd sys has data for us */
#define HTS_DATA_EMPTY     0x0080    /* OK to send data to snd sys */
#define HTS_DATA_REALTIME  0x8080	 /* realtime bit clear for load while playing */		

/* these are read from the sound-to-host status register */
/* notice only upper 8 bits are relevant */
/* error code and info signals back */
#define SND_INFO_SIGNAL		0x0100	 /* playlist signal - user data coming back */
#define SND_INFO_TRACK		0x0200	 /* track used and interrupted mask */	
#define SND_INFO_CHECKSUM	0x0300	 /* any number of checksums */	
#define SND_INFO_SYNC		0x0400	 /* host and sound synch up return queues */
#define SND_INFO_STREAM		0x0500	 /* flag any stream-related info back */
#define SND_INFO_BUSY		0x0600	 /* tried to hard set to a track that was in use */
#define SND_INFO_UNDEFINED	0x0700	 /* undefined as of 22 Sep 97 */

/* these are read from the sound-to-host status register */
/* notice only upper 8 bits are relevant */
#define SND_STATUS_REALTIME	0x8000		/* 1 == snd idle, 0 == snd busy */
#define SND_STATUS_FIFO			0x4000	/* FIFO related, see Eloff */
#define SND_SAFETY_NET			0x2000	/* decompression error safety kicked in */
#define SND_STATUS_SYSERROR	0x1000		/* any other stack or code related error */
#define SND_STATUS_UNDEFINED  0x0800	/* not defined 22 Sep 97 MVB */

/* delays and timeouts for values on order of microseconds */
/* this is for delay between word xfers */
#define SND_DELAY_MULTIPLIER 500000
#define SND_TIMEOUT_MULTIPLIER 500000

#if !defined(DEBUG)
   /* this is 25,000,000 nanosec	or 25 milliseconds */
	#define SND_TIMEOUT 50 * SND_TIMEOUT_MULTIPLIER
#else
   /* this is 250,000,000 nanosec or 0.25 seconds */
	#define SND_TIMEOUT 500 * SND_TIMEOUT_MULTIPLIER
#endif

/* for timeouts on order of seconds */
/* this is for certain real-time sound boot ROM diagnostics */
#define SND_LONG_TIMEOUT 2         

/* defines for sound boot ROM diagnostic functions */
#define SND_CMD_SRAM_TEST    0x3A
#define SND_CMD_DRAM0_TEST   0x4A
#define SND_CMD_DRAM1_TEST   0x5A
#define SND_CMD_ROM_VERSION  0x6A
#define SND_CMD_ASIC_VER     0x7A
#define SND_CMD_ECHO         0x8A
#define SND_CMD_PMINT_SUM    0x9A  
#define SND_CMD_BONG         0xAA

/* completion codes (CC) for above diags */
#define SND_RTN_SRAM_PASSED   0xCC01
#define SND_RTN_DRAM0_PASSED  0xCC02
#define SND_RTN_DRAM1_PASSED  0xCC03
#define SND_RTN_BONG_FINISHED 0xCC04
#define SND_RTN_SRAM1_FAILED  0xEE01 
#define SND_RTN_SRAM2_FAILED  0xEE02
#define SND_RTN_SRAM3_FAILED  0xEE03
#define SND_RTN_DRAM0_FAILED  0xEE04 
#define SND_RTN_DRAM1_FAILED  0xEE05 

/* defines for sound boot ROM load cmds */
#define SND_CMD_LOAD         0x1A          
#define SND_CMD_RUN          0x2A
#define SND_LOAD_TYPE_PM     0x00
#define SND_LOAD_TYPE_DM     0x01


/*** reserved system sound calls ***/

/* 2-word calls */
#define SND_CMD_MASTER_VOLUME    0x55AA  /* set snd master volume */
#define SND_CMD_TRACK_VOLUME     0x55AB  /* set track vol for a track */
#define SND_CMD_TRACK_PAN        0x55AC  /* set track pan for a track */
#define SND_CMD_TRACK_PRIORITY   0x55AD  /* set priority for a track */
#define SND_CMD_TRACK_STOP       0x55AE  /* stop a specific track */
#define SND_CMD_SET_RESERVED     0x55AF  /* set reserved track mask */
#define SND_CMD_SYNC             0x55B0  /* put a sync word in the queue */
#define SND_CMD_OUTPUT_MONO      0x55B1  /* set output mode to mono */
#define SND_CMD_OUTPUT_STEREO    0x55B2  /* set output mode to stereo */ 
#define SND_CMD_STEREO_SAFETY    0xAA55  /* safety 2nd word */
#define SND_CMD_MONO_SAFETY      0xBB44  /* safety 2nd word */

/* "engine" sound sample playback cmds */
#define SND_CMD_DRONE1_CTRL      0x55E3  /* params for drone snd #1 */
#define SND_CMD_DRONE2_CTRL      0x55E4  /* params for drone snd #2 */
#define SND_CMD_PLAYER_CTRL      0x55E0  /* params for player sound */

/* 1-word calls */
#define SND_CMD_LOAD_DRAM      0x55D0  /* download to D/RAM immediate */
#define SND_CMD_LOAD_PLAY      0x55D1  /* download to D/RAM while playing */
#define SND_CMD_FLUSH_CALLS    0x55D2  /* flush all pending sound calls */
#define SND_CMD_FLUSH_QUEUE    0x55D3  /* flush input and output queue */
#define SND_CMD_GET_VERSION    0x55D4  /* return snd opsys version */
#define SND_CMD_CHECKSUM_DRAM  0x55D5  /* return checksum of D/RAM range */

#define SND_MONO 0
#define SND_STEREO 1

/* a frame is 240 16-bit samples = 120 32-bit words */
#define SND_SAMPLE_RATE 31250  /* DCS sample rate is 31,250 Hz */
#define SND_MONO_FRAME 120     /* DCS frame is 240 samples (from FFT) */
#define SND_STEREO_FRAME 240   /* stereo frame is two mono frames */
#define SND_BUFFER_SIZE 240    /* enough to hold biggest */

#define SND_STATUS_PLAYERR  0x3333
#define SND_STATUS_SENDOK   0xA1A1

/* wait 20 x 16msec for sound streaming to empty out */
#define SND_STREAM_TIMEOUT 20

#define  TRUE  1
#define  FALSE 0


int snd_stream_stop (void);
int _ioctl(int, int, int);

/***** SOUND GLOBALS (to this module only) *********************************/

//volatile unsigned int *io_asic_control = (unsigned int *) SND_IOA_MCTRL;

//volatile unsigned int *snd_control = (unsigned int *) SND_CONTROL;
//volatile unsigned int *snd_hts_data = (unsigned int *) SND_HTS_DATA;
//volatile unsigned int *snd_hts_status = (unsigned int *) SND_HTS_STATUS;
//volatile unsigned int *snd_sth_data = (unsigned int *) SND_STH_DATA;

/* sound call table and D/RAM addresses */

#define SND_SCALL_SIZE   2               /* each entry is 2 16-bit words */
#define SND_TABLE_SIZE   0x4000          /* 4096 entries */
#define SND_TABLE_FIRST  SND_SCALL_SIZE  /* first entry always rsvd for 0 */
#if defined(VEGAS)
#define SND_DRAM_SIZE    0x400000        /* 4 megabytes of D/RAM */
#else
#define SND_DRAM_SIZE    0x200000        /* 2 megabytes of D/RAM */
#endif

static snd_bank_addr = SND_TABLE_SIZE;
static snd_table_addr = SND_TABLE_FIRST;

#define SAMPLE_ENTRY_SIZE 4      /* 4 bytes for each address */
#define SAMPLE_TABLE_SIZE SND_MAX_SAMPLES * (SAMPLE_ENTRY_SIZE * 2)
#define DCS2_PAGE_SIZE 0x400

/***** bank mgmt stuff ******************************************************/

typedef struct snd_bank_node {
 char name [MAX_NAME];           /* name of the bank, no .BNK extension */
 struct snd_bank_node *prev;     /* ptr to prev loaded bank */
 struct snd_bank_node *next;     /* ptr to next loaded bank */
 unsigned int dram_start;        /* first location in D/RAM */
 unsigned int dram_end;          /* last location in D/RAM */
 int scall_count;				 /* how many sound calls in this bank */			
 int scall_first;                /* first absolute scall in this bank */
 int scall_last;                 /* last absolute scall in this bank */
 } snd_bank_node_t;

snd_bank_node_t *snd_bank_list = NULL; /* start of bank list */

int snd_lock = FALSE;         /* lock out all sound calls */
int snd_lock_bank = FALSE;    /* only lock out calls made with snd_scall_bank() */

/***** engine sample stuff **************************************************/

typedef struct snd_sample
 {
 char *name;                 /* name passed in */
 FILE *file_handle;          /* file handle */
 unsigned int file_size;     /* file size in !!!bytes!!! */
 unsigned int *data;         /* ptr to data loaded from disk */
 } snd_sample_t;

/***** streaming audio stuff ***********************************************/

int snd_buffer_count = 0;    /* number of one-frame buffers avail */
FILE *snd_stream_handle;

/***** structure for loading different sound o/s files *********************/

enum {
	SND_OPSYS_TYPE_DM,		/* file goes into data memory */
	SND_OPSYS_TYPE_PM		/* file goes into program memory */
	};


typedef struct snd_opsys_file
	{
	char *filename;			/* full name of file on the disk */
	int type;				/* DM or PM */
	int start_address;		/* starting load address on sound DSP */
	int end_address;		/* ending load address on sound DSP */
	} snd_opsys_file_t;

/* "normal" version of opsys, latest is 0122 */
snd_opsys_file_t snd_opsys_0122_list[] = {
        {"f_osys.bin", SND_OPSYS_TYPE_PM, 0x0000, 0x01FF},
        {"f_comp.bin", SND_OPSYS_TYPE_PM, 0x2800, 0x3aFF},
        {"f_dm_ext.bin", SND_OPSYS_TYPE_DM, 0x0800, 0x37FF},
        {"f_dm_int.bin", SND_OPSYS_TYPE_DM, 0x3800, 0x38FF},
	{0, 0, 0, 0}
	};

/* newer Eloff FIFO streaming system */
snd_opsys_file_t snd_opsys_0223_list[] = {
	{"f_osys.bin", SND_OPSYS_TYPE_PM, 0x0000, 0x01FF},
        {"f_comp.bin", SND_OPSYS_TYPE_PM, 0x2800, 0x3AFF},
	{"f_dm_ext.bin", SND_OPSYS_TYPE_DM, 0x0800, 0x37FF},
	{"f_dm_int.bin", SND_OPSYS_TYPE_DM, 0x3800, 0x38FF},
	{0, 0, 0, 0}
	};

snd_opsys_file_t *snd_opsys_load_list[] = {
	snd_opsys_0122_list,
	snd_opsys_0223_list,
	0};

#if defined(SEATTLE)
static int	snd_use_crc = 0;
#elif defined(VEGAS)
static int	snd_use_crc = 1;
#else
#error Environment variable TARGET_SYS is not defined
#endif
static void	(*snd_crc_fail_audit_func)(void) = (void (*)(void))0;


/***** FUNCTION PROTOTYPES *************************************************/

void snd_dump_port (void);
int snd_init_from_memory(void *os, void *comp, void *dm_ext, void *dm_int);
int snd_load_opsys_from_memory(void *os, void *comp, void *dm_ext, void *dm_int);
int snd_load_pm_from_memory(unsigned int *buffer, int start, int end);
int snd_load_dm_from_memory (unsigned int *buffer, int start, int end);
int snd_load_bank_from_memory (unsigned int *data_buffer, int *num_sound_calls);
int snd_bank_load_from_memory (char *bank_name, void *bnk_data);

/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_init()                                                    */
/*                                                                         */
/* Quick wrapper for the functions needed to get the sound system running. */
/*                                                                         */
/* 1. resets the sound system and looks for ack from sound boot ROM        */
/*                                                                         */
/* 2. reads the sound O/S off the disk and downloads it to sound DSP       */
/*                                                                         */
/* 3. initializes the sound call table to all empty                        */
/*                                                                         */
/* 4. initializes the bank management scheme                               */
/*                                                                         */
/* Returns OK (0) or ERROR (non-zero).                                     */
/*                                                                         */
/* See also snd_clear().                                                   */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_init (void)

{ /* snd_init() */

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_init(): begin\r\n");
	#endif

	if (snd_reset_ack() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init(): ERROR could not reset sound system.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_opsys() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init(): ERROR could not load sound operating system.\r\n");
		#endif
		return ERROR;
		}

	if (snd_init_scall_table() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init(): ERROR could not initialize sound call table.\r\n");
		#endif
		return ERROR;
		}

	snd_bank_init();

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_init(): done\r\n");
	#endif

	return OK;
   
} /* snd_init() */

/***** END of snd_init() ***************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_init_multi()                                              */
/*                                                                         */
/* Same as snd_init(), except that the files to load and the DSP load      */
/* addresses are in a table defined at the top of this file.               */
/*                                                                         */
/* This was done so that we could switch on the fly between the "normal"   */
/* version of the sound system and the new FIFO-based streaming code.      */
/*                                                                         */
/* Once the new FIFO sound code is verified and made "official", then this */
/* function should be the standard way to load the sound opsys, since it   */
/* doesn't have anythign hard-coded.                                       */
/*                                                                         */
/* Pass in one of the enums SND_OPSYS_0122, SND_OPSYS_0223, etc. defined   */
/* in SOUND.H.                                                             */
/*                                                                         */
/* Copyright (c) 1997 Midway Games Inc.                                    */
/*                                                                         */
/* 30 Nov 97                                                               */
/*                                                                         */
/***************************************************************************/

int snd_init_multi (int opsys_type)

{ /* snd_init_multi() */

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_init_multi(): begin\r\n");
	#endif

	if (snd_reset_ack() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init_multi(): ERROR could not reset sound system.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_opsys_multi (opsys_type) != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init_multi(): ERROR could not load sound operating system.\r\n");
		#endif
		return ERROR;
		}

	if (snd_init_scall_table() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init_multi(): ERROR could not initialize sound call table.\r\n");
		#endif
		return ERROR;
		}

	snd_bank_init();

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_init_multi(): done\r\n");
	#endif

	return OK;
   
} /* snd_init_multi() */

/***** END of snd_init_multi() *********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_init_from_memory()                                        */
/*                                                                         */
/* Loads the sound operating system from memory instead of from disk       */
/* files. Used by the diagnostics.                                         */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_init_from_memory (void *os, void *comp, void *dm_ext, void *dm_int)

{ /* snd_init_from_memory() */

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_init_from_memory(): begin\r\n");
	#endif

	if (snd_reset_ack() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init_from_memory(): ERROR could not reset sound system.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_opsys_from_memory (os, comp, dm_ext, dm_int) != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init_from_memory(): ERROR could not load sound operating system.\r\n");
		#endif
		return ERROR;
		}

	if (snd_init_scall_table() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_init_from_memory(): ERROR could not initialize sound call table.\r\n");
		#endif
		return ERROR;
		}

	snd_bank_init();

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_init_from_memory(): done\r\n");
	#endif

	return OK;
   
} /* snd_init_from_memory() */

/***** END of snd_init_from_memory() ***************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_dbg_boot_info()                                           */
/*                                                                         */
/* Runs each of the available sound boot ROM diagnostic tests (except for  */
/* the 1 kHz sine wave). Shows the progress and results of the test using  */
/* the printf() output.                                                    */
/*                                                                         */
/* This function can be used to check out sound hardware when debugging    */
/* or as the start of the actual game diagnostics.                         */
/*                                                                         */
/* !!! Note that this function SLEEPS and must be called by a process. !!! */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

void snd_dbg_boot_info (void)

{ /* snd_dbg_boot_info() */

unsigned int snd_boot_version;
unsigned int snd_opsys_version;
unsigned int sdrc_version;
unsigned int pmint_sum;

int status;

/***************************************************************************/

 fprintf (stderr, "\r\n");
 fprintf (stderr, "snd_dbg_boot_info(): begin\r\n");

 /*** 0. reset the sound system to get the boot ROM running ***/

 snd_reset();


 /*** 1. request sound boot ROM version ***/

 snd_boot_version = snd_get_boot_version();

 fprintf (stderr, " sound boot rom version: ");
 if (snd_boot_version == ERROR)
   fprintf (stderr, "ERROR\r\n");
 else
   fprintf (stderr, "%04X\r\n", snd_boot_version & SND_MASK16);


 /*** 2. request SDRC ASIC version ***/

 sdrc_version = snd_get_sdrc_version();

 fprintf (stderr, " SDRC ASIC version: ");
 if (sdrc_version == ERROR)
   fprintf (stderr, "ERROR\r\n");
 else
   fprintf (stderr, "%04X\r\n", sdrc_version & SND_MASK16);


 /*** 3. do walking 1's and 0's test on sound port ***/

 status = snd_test_port();
 if (status == ERROR)
   fprintf (stderr, " ERROR sound port failed walking 0's and 1's test\r\n");
 else
   fprintf (stderr, " sound port walking 0's and 1's test OK\r\n");


 /*** 4. request sound DSP internal program memory checksum ***/

 pmint_sum = snd_get_pmint_sum();

 fprintf (stderr, " internal program memory checksum: ");
 if (pmint_sum == ERROR)
   fprintf (stderr, "ERROR\r\n");
 else
   fprintf (stderr, "%04X\r\n", pmint_sum & SND_MASK16);


 /*** 5. test the sound DSP static RAM ***/

 status = snd_send_command (SND_CMD_SRAM_TEST);

 status = snd_wait_for_completion();

 switch (status)
   {
   case SND_RTN_SRAM_PASSED:
     fprintf (stderr, " sound DSP static RAM OK\r\n");
     break;

   case SND_RTN_SRAM1_FAILED:
     fprintf (stderr, " ERROR sound DSP static RAM 1 failed\r\n");
     break;

   case SND_RTN_SRAM2_FAILED:
     fprintf (stderr, " ERROR sound DSP static RAM 2 failed\r\n");
     break;

   case SND_RTN_SRAM3_FAILED:
     fprintf (stderr, " ERROR sound DSP static RAM 3 failed\r\n");
     break;

   default:
     fprintf (stderr, " ERROR sound DSP static RAM, unknown error\r\n");
     break;

   }


 /*** 6. test the sound DSP dynamic RAM ***/

 /* note that the DCS2 system supports two banks of RAM */
 /* each bank has 16 megabits or 1 megabyte of D/RAM */
 /* production boards are only stuffed with 1 bank */

 status = snd_send_command (SND_CMD_DRAM0_TEST);

 status = snd_wait_for_completion();

 switch (status)
   {
   case SND_RTN_DRAM0_PASSED:
     fprintf (stderr, " sound DSP dynamic RAM bank 0 OK\r\n");
     break;

   case SND_RTN_DRAM0_FAILED:
     fprintf (stderr, " ERROR sound DSP dynamic RAM bank 0 failed\r\n");
     break;

   default:
     fprintf (stderr, " ERROR sound DSP dynamic RAM, unknown error\r\n");
     break;

   }


 /*** 7. make the ever-famous "BONG" sound ***/  

 fprintf (stderr, " listen for tone...\r\n");

 status = snd_send_command (SND_CMD_BONG);

 status = snd_wait_for_completion();

 if (status == SND_RTN_BONG_FINISHED)
   fprintf (stderr, " tone OK\r\n");
 else
   fprintf (stderr, " ERROR sound port failed tone test\r\n");


 /*** 8. load the actual operating system off disk ***/

 snd_load_opsys();

 fprintf (stderr, " loading sound operating system...\r\n");

 snd_opsys_version = snd_get_opsys_ver();
 fprintf (stderr, " sound operating system version: ");
 if (snd_opsys_version == ERROR)
   fprintf (stderr, "ERROR\r\n");
 else
   fprintf (stderr, "%04X\r\n", snd_opsys_version & SND_MASK16);

  fprintf (stderr, "snd_dbg_boot_info(): done\r\n");
  fprintf (stderr, "\r\n");

} /* snd_dbg_boot_info() */

/***** END of snd_dbg_boot_info() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_bank_delete()                                             */
/*                                                                         */
/* Deletes a sound bank from the linked list of loaded banks. Nothing is   */
/* changed on the sound system; this just lets the bank mgmt system know   */
/* that a certain range of sound D/RAM has been freed up for loading.      */
/*                                                                         */
/* Returns OK (0) or ERROR (non-zero).                                     */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_bank_delete (char * bank_name)

{ /* snd_bank_delete() */

snd_bank_node_t *bp;         /* bank node ptr */
snd_bank_node_t *bp_prev;    /* next bank in list */
snd_bank_node_t *bp_next;    /* prev bank in list */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_bank_delete(): begin\r\n");
 #endif

 bp = snd_bank_list;

 while (bp != NULL)
   {
   if (!strcmp (bp->name, bank_name))
     break;
   bp = bp->next;
   }

 if (bp == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_delete(): ERROR bank '%s' not found.\r\n", bank_name);
#endif
   return ERROR;
   }

 /* unlink the bank from the list */
 bp_prev = bp->prev;
 bp_next = bp->next;
 if (bp_prev)
    {
    bp_prev->next = bp_next;
    }
 if (bp_next)
    {
    bp_next->prev = bp_prev;
    }

 /* if this was the only bank, set the list to NULL */
 /* and reset the sound D/RAM address */
 if ((bp->next == NULL) && (bp->prev == NULL))
   {
   snd_bank_list = NULL;
   /* do not set this to SND_TABLE_SIZE because */
   /* there could be engine samples loaded... */
   /* we want this set to whatever the start load addr */
   /* was when the first bank got loaded */	   	
   snd_bank_addr = bp->dram_start;
   }

 /* if this was the first bank on the list, move the list head ptr */
 if (bp->prev == NULL)
	{
	/* next node will now be the first node */
    snd_bank_list = bp_next;
	}

 #if SND_DEBUG
  fprintf (stderr, "snd_bank_delete(): deleted bank '%s'\r\n", bp->name);
 #endif

 /* free the node */
 free (bp);

 #if SND_DEBUG
  fprintf (stderr, "snd_bank_delete(): done\r\n");
 #endif

 return (OK);

} /* snd_bank_delete() */

/***** snd_bank_delete() ***************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_bank_load()                                               */
/*                                                                         */
/* Loads a sound bank and inserts it into the linked list. The name passed */
/* in should be the name of the bank file on the game disk, without the    */
/* .BNK extension. This is also the name that will be used for all         */
/* subsequent references to sound calls in the bank.                       */
/*                                                                         */
/* Returns OK (0) or ERROR (non-zero).                                     */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_bank_load (char *bank_name)

{ /* snd_bank_load() */

snd_bank_node_t *bp_cur;	/* current bank node ptr */
snd_bank_node_t *bp_prev;   /* prev bank */
snd_bank_node_t *bp_new;    /* bank we are adding */

unsigned int dram_last;     /* ending D/RAM location of previous bank */
unsigned int scall_last;    /* ending scall location of previous bank */

char file_name [MAX_NAME];  /* name plus .BNK extension */
int num_sound_calls;		/* how many calls in this bank */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_bank_load(): begin\r\n");
 #endif

 /* need to add code to deal with loading a bank */
 /* in the middle */

 bp_prev = NULL;

 /* this is the first bank */
 if (snd_bank_list == NULL)
   {
   /* note that snd_bank_addr could have been */
   /* moved by the engine sound loaded */
   dram_last = snd_bank_addr;
   scall_last = SND_TABLE_FIRST;
   bp_prev = NULL;
   }
 else
   {
   /* find the last bank loaded */
   bp_cur = snd_bank_list;
   while (bp_cur != NULL)
     {
     /* check if the bank is already loaded */
     if (!strcmp (bp_cur->name, bank_name))
       {
#ifdef DEBUG
       fprintf (stderr, "snd_bank_load(): ERROR bank '%s' already loaded.\r\n", bank_name);
#endif
       return ERROR;
       }
     bp_prev = bp_cur;
     bp_cur = bp_cur->next;
     } 
   /* get the ending bounds from the last bank */
   dram_last = bp_prev->dram_end;
   scall_last = bp_prev->scall_last;

   } /* else */


 /* add a new bank on the end */
 bp_new = malloc (sizeof (snd_bank_node_t));
 if (bp_new == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load(): ERROR allocating bank node.\r\n");
#endif
   return ERROR;
   }

 /* setup the name */
 strcpy (bp_new->name, bank_name);

 /* set up the linked list */
 bp_new->next = NULL;     		/* make this last in list */
 bp_new->prev = bp_prev;   		/* link this back to the prev */
 /* if this is first in list, link to list ptr */
 if (bp_prev == NULL)
   {
   snd_bank_list = bp_new;
   }
 else
   {
   bp_prev->next = bp_new;   /* link prev to this */
   }

 /* set up the dram and scall table bounds */
 bp_new->dram_start = dram_last;
 bp_new->scall_first = scall_last;
 /* these are global to this module */
 /* and will get updated when we load the bank */
 snd_bank_addr = bp_new->dram_start;
 snd_table_addr = bp_new->scall_first;

 /* check if we over-ran the sound memory */
 /* check the D/RAM (most likely problem) */
 if ((snd_bank_addr > SND_DRAM_SIZE) || (snd_table_addr > SND_TABLE_SIZE))
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load(): ERROR bank '%s' did not fit in sound D/RAM.\r\n", bank_name);
   fprintf (stderr, "snd_bank_load(): ending address:%08X, max address:%08X\r\n",
                snd_bank_addr, SND_DRAM_SIZE);
#endif
   snd_bank_delete (bank_name);
   return ERROR;
   }

// I'm located above here...
//
// /* check if we over_ran the sound call table (less likely) */
// if (snd_table_addr > SND_TABLE_SIZE)
//   {
//#ifdef DEBUG
//   fprintf (stderr, "snd_bank_load(): ERROR bank '%s' overran sound call table.\r\n", bank_name);
//   fprintf (stderr, "snd_bank_load(): ending address:%08X, max address:%08X\r\n",
//                snd_bank_addr, SND_DRAM_SIZE);
//#endif
//   snd_bank_delete (bank_name);
//   return ERROR;
//   }


 strcpy (file_name, bank_name);
 strcat (file_name, SND_BNK_EXT);
  if (snd_load_bank (file_name, &num_sound_calls) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load(): ERROR loading bank '%s'.\r\n", file_name);
#endif
   /* if the bank did not actually get successfully */
   /* loaded then unlink it from the list */
   snd_bank_delete (bank_name);
   return ERROR;
   }

 /* now that the bank has been loaded, save the ending bounds */
 bp_new->dram_end = snd_bank_addr;
 bp_new->scall_last = snd_table_addr;
 bp_new->scall_count = num_sound_calls;
 	
 #if SND_DEBUG
  fprintf (stderr, "snd_bank_load(): done\r\n");
 #endif

 return OK;

} /* snd_bank_load() */



int snd_bank_load_from_memory (char *bank_name, void *bnk_data)
{ /* snd_bank_load_from_memory() */

snd_bank_node_t *bp_cur;	/* current bank node ptr */
snd_bank_node_t *bp_prev;   /* prev bank */
snd_bank_node_t *bp_new;    /* bank we are adding */

unsigned int dram_last;     /* ending D/RAM location of previous bank */
unsigned int scall_last;    /* ending scall location of previous bank */

char file_name [MAX_NAME];  /* name plus .BNK extension */
int num_sound_calls;		/* how many calls in this bank */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_bank_load_from_memory(): begin\r\n");
 #endif

 /* need to add code to deal with loading a bank */
 /* in the middle */

 bp_prev = NULL;

 /* this is the first bank */
 if (snd_bank_list == NULL)
   {
   /* note that snd_bank_addr could have been */
   /* moved by the engine sound loaded */
   dram_last = snd_bank_addr;
   scall_last = SND_TABLE_FIRST;
   bp_prev = NULL;
   }
 else
   {
   /* find the last bank loaded */
   bp_cur = snd_bank_list;
   while (bp_cur != NULL)
     {
     /* check if the bank is already loaded */
     if (!strcmp (bp_cur->name, bank_name))
       {
#ifdef DEBUG
       fprintf (stderr, "snd_bank_load_from_memory(): ERROR bank '%s' already loaded.\r\n", bank_name);
#endif
       return ERROR;
       }
     bp_prev = bp_cur;
     bp_cur = bp_cur->next;
     } 
   /* get the ending bounds from the last bank */
   dram_last = bp_prev->dram_end;
   scall_last = bp_prev->scall_last;

   } /* else */


 /* add a new bank on the end */
 bp_new = malloc (sizeof (snd_bank_node_t));
 if (bp_new == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load_from_memory(): ERROR allocating bank node.\r\n");
#endif
   return ERROR;
   }

 /* setup the name */
 strcpy (bp_new->name, bank_name);

 /* set up the linked list */
 bp_new->next = NULL;     		/* make this last in list */
 bp_new->prev = bp_prev;   		/* link this back to the prev */
 /* if this is first in list, link to list ptr */
 if (bp_prev == NULL)
   {
   snd_bank_list = bp_new;
   }
 else
   {
   bp_prev->next = bp_new;   /* link prev to this */
   }

 /* set up the dram and scall table bounds */
 bp_new->dram_start = dram_last;
 bp_new->scall_first = scall_last;
 /* these are global to this module */
 /* and will get updated when we load the bank */
 snd_bank_addr = bp_new->dram_start;
 snd_table_addr = bp_new->scall_first;

 /* check if we over-ran the sound memory */
 /* check the D/RAM (most likely problem) */
 if ((snd_bank_addr > SND_DRAM_SIZE) || (snd_table_addr > SND_TABLE_SIZE))
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load(): ERROR bank '%s' did not fit in sound D/RAM.\r\n", bank_name);
   fprintf (stderr, "snd_bank_load(): ending address:%08X, max address:%08X\r\n",
                snd_bank_addr, SND_DRAM_SIZE);
#endif
   snd_bank_delete (bank_name);
   return ERROR;
   }

// I'm located above here...
//
// /* check if we over_ran the sound call table (less likely) */
// if (snd_table_addr > SND_TABLE_SIZE)
//   {
//#ifdef DEBUG
//   fprintf (stderr, "snd_bank_load(): ERROR bank '%s' overran sound call table.\r\n", bank_name);
//   fprintf (stderr, "snd_bank_load(): ending address:%08X, max address:%08X\r\n",
//                snd_bank_addr, SND_DRAM_SIZE);
//#endif
//   snd_bank_delete (bank_name);
//   return ERROR;
//   }


 strcpy (file_name, bank_name);
 strcat (file_name, SND_BNK_EXT);

 /* load the bank */
  if (snd_load_bank_from_memory(bnk_data, &num_sound_calls) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load_from_memory(): ERROR loading bank '%s'.\r\n", file_name);
#endif
   /* if the bank did not actually get successfully */
   /* loaded then unlink it from the list */
   snd_bank_delete (bank_name);
   return ERROR;
   }

 /* now that the bank has been loaded, save the ending bounds */
 bp_new->dram_end = snd_bank_addr;
 bp_new->scall_last = snd_table_addr;
 bp_new->scall_count = num_sound_calls;
 	
 /* check if we over-ran the sound memory */
 /* check the D/RAM (most likely problem) */
 if (snd_bank_addr > SND_DRAM_SIZE)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load_from_memory(): ERROR bank '%s' did not fit in sound D/RAM.\r\n", bank_name);
   fprintf (stderr, "snd_bank_load_from_memory(): ending address:%08X, max address:%08X\r\n",
                snd_bank_addr, SND_DRAM_SIZE);
#endif
   return ERROR;
   }
 /* check if we over_ran the sound call table (less likely) */
 if (snd_table_addr > SND_TABLE_SIZE)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load_from_memory(): ERROR bank '%s' overran sound call table.\r\n", bank_name);
   fprintf (stderr, "snd_bank_load_from_memory(): ending address:%08X, max address:%08X\r\n",
                snd_bank_addr, SND_DRAM_SIZE);
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_bank_load_from_memory(): done\r\n");
 #endif

 return OK;

} /* snd_bank_load_from_memory() */

/***** END of snd_bank_load() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_bank_load_playing()                                       */
/*                                                                         */
/* Version of snd_bank_load_playing() that supports downloading sounds     */
/* while playing.                                                          */
/*                                                                         */
/* Note! There are a few constraints that the game-side code needs to heed */
/* when using this function, so "Call before you dig."                     */
/*                                                                         */
/* (This is a cut-and-paste from snd_bank_load(), and could be made more   */
/* modular, but I did not want to disrupt any of the current code while    */
/* trying to get this to work.)                                            */
/*                                                                         */
/* Returns OK (0) or ERROR (non-zero).                                     */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* 05 Sep 97 MVB                                                           */
/*                                                                         */
/***************************************************************************/

int snd_bank_load_playing (char *bank_name)

{ /* snd_bank_load_playing() */

snd_bank_node_t *bp_cur;	/* current bank node ptr */
snd_bank_node_t *bp_prev;   /* prev bank */
snd_bank_node_t *bp_new;    /* bank we are adding */

unsigned int dram_last;     /* ending D/RAM location of previous bank */
unsigned int scall_last;    /* ending scall location of previous bank */

char file_name [MAX_NAME];  /* name plus .BNK extension */
int num_sound_calls;		/* how many calls in this bank */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_bank_load_playing(): begin\r\n");
 #endif

 /* need to add code to deal with loading a bank */
 /* in the middle */

 bp_prev = NULL;

 /* this is the first bank */
 if (snd_bank_list == NULL)
   {
   /* note that snd_bank_addr could have been */
   /* moved by the engine sound loaded */
   dram_last = snd_bank_addr;
   scall_last = SND_TABLE_FIRST;
   bp_prev = NULL;
   }
 else
   {
   /* find the last bank loaded */
   bp_cur = snd_bank_list;
   while (bp_cur != NULL)
     {
     /* check if the bank is already loaded */
     if (!strcmp (bp_cur->name, bank_name))
       {
#ifdef DEBUG
       fprintf (stderr, "snd_bank_load_playing(): ERROR bank '%s' already loaded.\r\n", bank_name);
#endif
       return ERROR;
       }
     bp_prev = bp_cur;
     bp_cur = bp_cur->next;
     } 
   /* get the ending bounds from the last bank */
   dram_last = bp_prev->dram_end;
   scall_last = bp_prev->scall_last;

   } /* else */


 /* add a new bank on the end */
 bp_new = malloc (sizeof (snd_bank_node_t));
 if (bp_new == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load_playing(): ERROR allocating bank node.\r\n");
#endif
   return ERROR;
   }

 /* setup the name */
 strcpy (bp_new->name, bank_name);

 /* set up the linked list */
 bp_new->next = NULL;     		/* make this last in list */
 bp_new->prev = bp_prev;   		/* link this back to the prev */
 /* if this is first in list, link to list ptr */
 if (bp_prev == NULL)
   {
   snd_bank_list = bp_new;
   }
 else
   {
   bp_prev->next = bp_new;   /* link prev to this */
   }

 /* set up the dram and scall table bounds */
 bp_new->dram_start = dram_last;
 bp_new->scall_first = scall_last;
 /* these are global to this module */
 /* and will get updated when we load the bank */
 snd_bank_addr = bp_new->dram_start;
 snd_table_addr = bp_new->scall_first;

 /* check if we over-ran the sound memory */
 /* check the D/RAM (most likely problem) */
 if ((snd_bank_addr > SND_DRAM_SIZE) || (snd_table_addr > SND_TABLE_SIZE))
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load(): ERROR bank '%s' did not fit in sound D/RAM.\r\n", bank_name);
   fprintf (stderr, "snd_bank_load(): starting address:%08X, max address:%08X\r\n",
                snd_bank_addr, SND_DRAM_SIZE);
#endif
   snd_bank_delete (bank_name);
   return ERROR;
   }
// I'm located above here...
//
// /* check if we over_ran the sound call table (less likely) */
// if (snd_table_addr > SND_TABLE_SIZE)
//   {
//#ifdef DEBUG
//   fprintf (stderr, "snd_bank_load(): ERROR bank '%s' overran sound call table.\r\n", bank_name);
//   fprintf (stderr, "snd_bank_load(): ending address:%08X, max address:%08X\r\n",
//                snd_bank_addr, SND_DRAM_SIZE);
//#endif
//   snd_bank_delete (bank_name);
//   return ERROR;
//   }


 strcpy (file_name, bank_name);
 strcat (file_name, SND_BNK_EXT);

  if (snd_load_bank_playing (file_name, &num_sound_calls) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_bank_load_playing(): ERROR loading bank '%s'.\r\n", file_name);
#endif
   /* if the bank did not actually get successfully */
   /* loaded then unlink it from the list */
   snd_bank_delete (bank_name);
   return ERROR;
   }

 /* now that the bank has been loaded, save the ending bounds */
 bp_new->dram_end = snd_bank_addr;
 bp_new->scall_last = snd_table_addr;
 bp_new->scall_count = num_sound_calls;
 	
 #if SND_DEBUG
  fprintf (stderr, "snd_bank_load_playing(): done\r\n");
 #endif

 return OK;

} /* snd_bank_load_playing() */

/***** END of snd_bank_load_playing() **************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_scall_bank                                                */
/*                                                                         */
/* Given a bank name, searches the list of loaded banks and finds the base */
/* sound call number offset for that bank. Adds that offset to the scall   */
/* and then calls snd_scall_direct().                                      */
/*                                                                         */
/* Returns OK (0) or ERROR (non-zero).                                     */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* 21 Mar 97 MVB added snd_lock check                                      */
/* 12 Sep 97 MVB - added more error checking                               */
/* 22 Sep 97 MVB - added snd_lock to snd_lock_bank                         */
/*                                                                         */
/***************************************************************************/

int snd_scall_bank (char *bank_name, unsigned int scall, 
                   unsigned int volume, unsigned int pan, 
                   unsigned int priority)

{ /* snd_scall_bank() */

snd_bank_node_t *bp;       /* bank node ptr */
unsigned int scall_offset; /* first sound call for this bank */

/***************************************************************************/

 /* check for sound call lockout */
 if ((snd_lock) || (snd_lock_bank))
   {
   #if SND_DEBUG
      fprintf (stderr, "snd_scall_bank(): sound calls locked out, no call made\r\n");
   #endif
   return OK; /* should this be an ERROR? */
   }

 /* find the given bank */

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_scall_bank(): begin\r\n");
 #endif

 bp = snd_bank_list;
 while (bp != NULL)
   {
   if (!strcmp (bp->name, bank_name))
     break;
   bp = bp->next;
   }

 if (bp == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_bank(): ERROR bank '%s' not found.\r\n", bank_name);
#endif
   return ERROR;
   }

 /* the scall number is the addr div by size of a call */
 scall_offset = bp->scall_first / SND_SCALL_SIZE;

 #if SND_DEBUG
  fprintf (stderr, "snd_scall_bank(): bank: %s\r\n", bp->name);
  fprintf (stderr, "snd_scall_bank(): offset: %d (0x%08X)\r\n", scall_offset, 
               scall_offset);
  fprintf (stderr, "snd_scall_bank(): scall: %d (0x%08X)\r\n", scall, scall);

 #endif

 /* make the sound call and pass on the parameters */
 if (snd_scall_direct (scall + scall_offset, volume, pan, priority) != OK)
	{
#ifdef DEBUG
	fprintf (stderr, "snd_scall_bank(): ERROR sending sound call\n");
	fprintf (stderr, " bank: %s\r\n", bp->name);
	fprintf (stderr, " offset: %d (0x%08X)\r\n", scall_offset, scall_offset);
	fprintf (stderr, " scall: %d (0x%08X)\r\n", scall, scall);
	fprintf (stderr, " volume: %d (0x%04X)\r\n", volume, volume);
	fprintf (stderr, " pan: %d (0x%04X)\r\n", pan, pan);
	fprintf (stderr, " priority: %d (0x%04X)\r\n", priority, priority);
#endif
	return ERROR;
	}

 #if SND_DEBUG
  fprintf (stderr, "snd_scall_bank(): done\r\n");
 #endif

 return OK;

} /* snd_scall_bank() */

/***** END of snd_scall_bank() *********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_bank_showlist()                                           */
/*                                                                         */
/* Debugging function. Shows the list of currently loaded banks.           */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

void snd_bank_showlist (void)

{ /* snd_bank_showlist() */

snd_bank_node_t *bp;   /* bank node ptr */

int num_banks;

/***************************************************************************/

  fprintf (stderr, " \r\n");
  fprintf (stderr, "snd_bank_showlist():\r\n");

 bp = snd_bank_list;
 num_banks = 0;

 if (bp == NULL)
   { 
   fprintf (stderr, " \r\n");
    fprintf (stderr, " no banks loaded\r\n");
   fprintf (stderr, " \r\n");
   return;   
   }

#ifdef LONG_DISPLAY
 while (bp != NULL)
   {
   fprintf (stderr, " \r\n");
   fprintf (stderr, " bank name:   %s\r\n", bp->name);
   fprintf (stderr, " node addr:   %08X\r\n", (int)bp);
   fprintf (stderr, " prev addr:   %08X\r\n", (int)bp->prev);
   fprintf (stderr, " next addr:   %08X\r\n", (int)bp->next);
   fprintf (stderr, " dram_start:  %08X\r\n", bp->dram_start);
   fprintf (stderr, " dram_end:    %08X\r\n", bp->dram_end);
   fprintf (stderr, " scall_first: %08X\r\n", bp->scall_first);
   fprintf (stderr, " scall_last:  %08X\r\n", bp->scall_last);
   num_banks++;
   bp = bp->next;
   }
#else
   fprintf (stderr, " BANKNAME        DramStart DramEnd  ScallFirst ScallLast\r\n");
   fprintf (stderr, " ---------------+---------+--------+----------+---------+\r\n");
 while (bp != NULL)
   {
   fprintf (stderr, " \r\n \r\n");
   fprintf (stderr, " %15.15s  %08X %08X  %08X %08X\r\n", 
   bp->name,
   bp->dram_start,
   bp->dram_end,
   bp->scall_first,
   bp->scall_last);
   num_banks++;
   bp = bp->next;
   }
#endif

 fprintf (stderr, " \r\n");
 fprintf (stderr, " %d banks currently loaded\r\n", num_banks);
 fprintf (stderr, " \r\n");

} /* snd_bank_showlist() */

/***** END of snd_bank_showlist() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_bank_init()                                               */
/*                                                                         */
/* Initializes the bank mgmt system. Deletes all banks. Resets the         */
/* starting D/RAM load address.                                            */
/*                                                                         */
/* Note! If you are using the sample-playback engine stuff, then this will */
/* wipeout any loaded engine samples.                                      */
/*                                                                         */
/* See also snd_clear().                                                   */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

void snd_bank_init (void)

{ /* snd_bank_init() */

snd_bank_node_t *bp_cur;   /* current bank */
snd_bank_node_t *bp_temp;    /* save ptr to one that will get deleted */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_bank_init(): begin\r\n");
 #endif
 
 if (snd_bank_list == NULL)
   return;

 /* walk the list and delete the bank nodes */

 bp_cur = snd_bank_list;
 
 while (bp_cur != NULL)
   {
   bp_temp = bp_cur;

   #if SND_DEBUG
     fprintf (stderr, "snd_bank_init(): deleting bank '%s'\r\n", bp_temp->name);
   #endif

   bp_cur = bp_cur->next;

   free (bp_temp);       
   }

 /* tag the list as empty */	
 snd_bank_list = NULL;

 /* reset the starting load address for banks */
 /* to the first D/RAM address - this is important! */
 /* 28 Aug 97 MVB and BRE */
 snd_bank_addr = SND_TABLE_SIZE;


 #if SND_DEBUG
  fprintf (stderr, "snd_bank_init(): done\r\n");
 #endif

} /* snd_bank_init() */

/***** END of snd_bank_init() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_clear()                                                   */
/*                                                                         */
/* Does a "wipeout" of loaded sounds, but does NOT reset the sound O/S.    */
/* Use this when you want to clear out all loaded sounds but do not want   */
/* to reset the sound system.                                              */
/*                                                                         */
/* See also snd_bank_init() and snd_init().                                */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* 28 Aug 97 MVB                                                           */
/*                                                                         */
/***************************************************************************/

void snd_clear (void)

{ /* snd_clear() */

	#if SND_DEBUG
		fprintf (stderr, "snd_clear(): begin\r\n");
	#endif

	/* clear out the sound call table D/RAM ptrs */
	snd_init_scall_table();

	/* clear out the bank mgmt system and */
	/* clear out the bank D/RAM ptrs */
	snd_bank_init();


	#if SND_DEBUG
		fprintf (stderr, "snd_clear(): done\r\n");
	#endif

} /* snd_clear() */

/***** END of snd_clear() **************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_init_scall_table()                                        */
/*                                                                         */
/* The sound call table is the first 16 kbytes of the sound D/RAM. Each    */
/* sound call is 4 bytes, so this is 4 kbytes of sound calls or 4096 sound */
/* calls.                                                                  */
/*                                                                         */
/* On reset, the sound D/RAM will wake up in an unknown state. This        */
/* initializes the sound call table to all 0xFFFFFFFF's.                   */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/***************************************************************************/

int snd_init_scall_table (void)

{ /* snd_init_scall_table() */

int status;

volatile int i;
volatile int j;

unsigned int high_word;
unsigned int low_word;

unsigned int start;
unsigned int end;
unsigned int size;

unsigned int checksum;
unsigned int table_checksum;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_init_scall_table(): begin\r\n");
 #endif

 start = 0;
 /* 16384 bytes = 8192 16-bit words */
 end = SND_TABLE_SIZE / SND_SCALL_SIZE - 1;
 size = end - start + 1;                 

 #if SND_DEBUG
  fprintf (stderr, "snd_init_scall_table(): start addr: %d (0x%08X)\r\n", start, start);
  fprintf (stderr, "snd_init_scall_table(): end addr:   %d (0x%08X)\r\n", end, end);
  fprintf (stderr, "snd_init_scall_table(): size:       %d words\r\n", size);
 #endif

  if (snd_send_data (SND_CMD_LOAD_DRAM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_init_scall_table(): ERROR sending load D/RAM cmd.\r\n");
#endif
    return ERROR;           
   }

  /* send the starting load address */
  high_word = (start >> 16) & SND_MASK16;
  low_word = start & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_init_scall_table(): ERROR sending high start addr.\r\n");
#endif
    return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_init_scall_table(): ERROR sending low start addr.\r\n");
#endif
    return ERROR;         
   }

  /* send the ending load address */
  high_word = (end >> 16) & SND_MASK16;
  low_word = end & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_init_scall_table(): ERROR sending high end addr.\r\n");
#endif
    return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_init_scall_table(): ERROR sending low end addr.\r\n");
#endif
    return ERROR;         
   }


  /* now send the data */
  i = 0;
 /* checksum is a straight 16-bit sum */
  checksum = 0;

  i++;

 for (j= 0; j < size; j++)
    {
 
    status = snd_send_data (0xFFFF);

    if (status != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_init_scall_table(): ERROR sending data.\r\n");
#endif
        return ERROR;         
       }
       
     checksum += 0xFFFF;

    }  /* for */

  status = snd_get_data (&table_checksum);         

 #if SND_DEBUG
  fprintf (stderr, "snd_init_scall_table(): checksum: %04X\r\n", table_checksum);
 #endif

  if ((checksum & SND_MASK16) != table_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_init_scall_table(): ERROR reading checksum.\r\n");     
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               table_checksum);      
#endif
     return ERROR;       
   }


 /* reset the starting location for adding new */
 /* sound call table entries - this is important */
 /* 28 Aug 97 MVB and BRE */
 snd_table_addr = SND_TABLE_FIRST;


 #if SND_DEBUG
  fprintf (stderr, "snd_init_scall_table(): done\r\n");
 #endif

  return OK;


} /* snd_init_scall_table() */

/***** END of snd_init_scall_table() ***************************************/
#if defined(SEATTLE)

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_reset()                                                   */
/*                                                                         */
/* Toggles the sound DSP reset line. 'C31 write-back mode is needed to     */
/* make sure that the status bits in host-to-sound and sound-to-host get   */
/* cleared properly. See page the I/O ASIC doc for more info.              */
/*                                                                         */
/* Also see the function snd_reset_ack() below.                            */
/*                                                                         */
/* 25 Nov 97 EJK - modified reset to only affect reset bit.                */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

void snd_reset (void)

{ /* snd_reset() */

int val;

/***************************************************************************/

   /* reset bit is bit 0 of I/O ASIC sound control */
   /* 0 = reset, 1 = active, not reset */

   /* get the current sound control register */
   _ioctl (4, FIOCGETSOUNDCONTROL,(int)&val);
   snd_delay (2);

   /* clear bit 0 */
   val &= ~0x0001;
   
   /* update the sound control register with this new value */
   _ioctl (4, FIOCSETSOUNDCONTROL, val);
   snd_delay (2);
   
   /* get the current sound control register */
   _ioctl (4, FIOCGETSOUNDCONTROL,(int)&val);
   snd_delay (2);
   
   /* set bit 0 */
   val |= 0x0001;
   
   /* update the sound control register with this new value */
   _ioctl (4, FIOCSETSOUNDCONTROL, val);
   snd_delay (2);

   /* bit 15 of I/O ASIC main ctrl enables */
   /* 'C31 write back mode */
   // *io_asic_control = 0xC000;

   _ioctl (4, FIOCGETCONTROL, (int)&val);
	/* clear out bit 15 - turn off write back mode */
   val &= ~0x8000;
   /* turn on the I/O ASIC LED */
   val |= 0x4000;
   _ioctl (4, FIOCSETCONTROL, val);

} /* snd_reset() */

/***** snd_reset() *********************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_reset_ack()                                               */
/*                                                                         */
/* Toggles the sound DSP reset line. 'C31 write-back mode is needed to     */
/* make sure that the status bits in host-to-sound and sound-to-host get   */
/* cleared properly. See page the I/O ASIC doc for more info.              */
/*                                                                         */
/* When the sound system is brought out of reset, it should be running the */
/* sound boot monitor code. The sound system should return an ack saying   */
/* "I booted OK."                                                          */
/*                                                                         */
/* Returns OK if successful, or ERROR if the ack does not come back.       */
/*                                                                         */
/* Also see the function snd_reset() above.                                */
/*                                                                         */
/* 25 Nov 97 EJK - modified reset to only affect reset bit.                */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_reset_ack (void)

{ /* snd_reset_ack() */

int status;                /* whether or not timed out */
unsigned int snd_ack;      /* ack returned by sound boot ROM */
int val;

/***************************************************************************/

   /* reset bit is bit 0 of I/O ASIC sound control */
   /* 0 = reset, 1 = active, not reset */

   /* get the current sound control register */
   _ioctl (4, FIOCGETSOUNDCONTROL,(int)&val);
   snd_delay (2);

   /* clear bit 0 */
   val &= ~0x0001;
   
   /* update the sound control register with this new value */
   _ioctl (4, FIOCSETSOUNDCONTROL, val);
   snd_delay (2);
   
   /* get the current sound control register */
   _ioctl (4, FIOCGETSOUNDCONTROL,(int)&val);
   snd_delay (2);
   
   /* set bit 0 */
   val |= 0x0001;
   
   /* update the sound control register with this new value */
   _ioctl (4, FIOCSETSOUNDCONTROL, val);
   snd_delay (2);

   /* bit 15 of I/O ASIC main ctrl enables */
   /* 'C31 write back mode */
   // *io_asic_control = 0xC000;

   _ioctl (4, FIOCGETCONTROL, (int)&val);
	/* clear out bit 15 - turn off write back mode */
   val &= ~0x8000;
   /* turn on the I/O ASIC LED */
   val |= 0x4000;
   _ioctl (4, FIOCSETCONTROL, val);

	/* look for the ack */
	status = snd_get_data (&snd_ack);

   if (status != OK)
      return ERROR;

   if (snd_ack != SND_MONITOR_READY)
      return ERROR;

   return OK;

} /* snd_reset_ack() */

/***** snd_reset_ack() *****************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_get_data()                                                */
/*                                                                         */
/* Returns a single 16-bit word from the sound-to-host port.               */
/*                                                                         */
/* Returns OK if successful or an ERROR if it times out.                   */
/*                                                                         */
/* Modify the defines SND_TIMEOUT_MULTIPLIER and SND_TIMEOUT to change     */
/* the timeout window as needed.                                           */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_get_data (unsigned int *data)

{ /* snd_get_data() */

int   status;
int   time;

/***************************************************************************/

   /* wait for data to show up in port */

   _ioctl(5, FIOCSTARTTIMER1, 0);

   _ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);
    
   while (!(status & STH_DATA_READY))
      {

      _ioctl (5, FIOCGETTIMER1, (int)&time);

      if (time > SND_TIMEOUT)
		  {
#ifdef DEBUG
   	   	  fprintf (stderr, "snd_get_data(): ERROR time out (status:%04X)\n", status);
#endif

		  /* stop the timeout timer */	
		  _ioctl(5, FIOCSTOPTIMER1, 0);
	
          return ERROR;
		  }

      _ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

      }

   /* stop the timeout timer */	
   _ioctl(5, FIOCSTOPTIMER1, 0);

   /* read the data */
   _ioctl(4, FIOCGETSOUNDDATA, (int)data);

   /* In 'C31 mode a read from the sound DSP */
   /* must be followed by a write.    */           
   /* This clears the data ready bit.  */                        
   _ioctl(4, FIOCSETSTHDATA, 0);

   return OK;

} /* snd_get_data() */

/***** END of snd_get_data() ***********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_get_debug()                                               */
/*                                                                         */
/* Version of snd_get_data() that always prints out the value of the       */
/* I/O ASIC status register.                                               */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* 18 Sep 97 MVB                                                           */
/*                                                                         */
/***************************************************************************/

int snd_get_debug (unsigned int *data)

{ /* snd_get_debug() */

int   status;
int   time;

/***************************************************************************/

   /* wait for data to show up in port */

   _ioctl(5, FIOCSTARTTIMER1, 0);

   _ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);
    
   while (!(status & STH_DATA_READY))
      {

      _ioctl (5, FIOCGETTIMER1, (int)&time);

      if (time > SND_TIMEOUT)
		  {
#ifdef DEBUG
   	   	  fprintf (stderr, "snd_get_debug(): ERROR time out (status:%04X)\n", status);
#endif
		  /* stop the timeout timer */	
		  _ioctl(5, FIOCSTOPTIMER1, 0);
	
          return ERROR;
		  }

      _ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

      }

   /* stop the timeout timer */	
   _ioctl(5, FIOCSTOPTIMER1, 0);

   /* read the data */
   _ioctl(4, FIOCGETSOUNDDATA, (int)data);

   fprintf (stderr, "snd_get_debug(): data:%04X, status:%04X\n", *data, status);

   /* In 'C31 mode a read from the sound DSP */
   /* must be followed by a write.    */           
   /* This clears the data ready bit.  */                        
   _ioctl(4, FIOCSETSTHDATA, 0);

   return OK;

} /* snd_get_debug() */

/***** END of snd_get_debug() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_send_data()                                               */
/*                                                                         */
/* Sends a single 16-bit word to the sound system.                         */
/*                                                                         */
/* Returns OK if successful or an ERROR if it times out.                   */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 12 Sep 97 MVB - added better error checking                             */
/* 15 Sep 97 MVB - fixed SOUNDSTATUS bug (was checking wrong STATUS)       */
/*                                                                         */
/***************************************************************************/

int snd_send_data (unsigned int data)

{ /* snd_send_data() */

int   status;
int   time;

/***************************************************************************/

	/* start the timeout timer */
   _ioctl (5, FIOCSTARTTIMER1, 0);

	/* wait for the port to clear up */
   _ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

	while (!(status & HTS_DATA_EMPTY))
	   {
       _ioctl (5, FIOCGETTIMER1, (int)&time);
	   
	   if (time > SND_TIMEOUT)
   	   	  {
		  #ifdef DEBUG
   	      		fprintf (stderr, "snd_send_data(): ERROR time out (status:%04X, data:%04X)\n",
   	                     status, data);
		  #endif

	   	  /* stop the timeout timer */

	   	  _ioctl (5, FIOCSTOPTIMER1, 0);

		  #if SND_DEBUG_ISR_TIMEOUT
		  	#ifdef DEBUG
				/* !!! this will lock up the system !!! */
				snd_dump_port();
		  	#endif
		  #endif

       	  return ERROR;
   	   	  }

	   _ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);
	   }

	/* stop the timeout timer */
	_ioctl (5, FIOCSTOPTIMER1, 0);

	/* write the data */
   _ioctl (4, FIOCSETSOUNDDATA, data & SND_MASK16);

 return OK;

} /* snd_send_data() */

/***** END of snd_send_data() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_send_data_realtime()                                      */
/*                                                                         */
/* Checks the sound system "realtime" bit and waits until it clears up,    */
/* and then sends the data.                                                */
/*                                                                         */
/* This is used during "download while playing" to pace the download with  */
/* the realtime fluctuations of the sound system.                          */
/*                                                                         */
/* See also snd_send_data().                                               */
/*                                                                         */
/* Returns OK if successful or an ERROR if it times out.                   */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 05 Sep 97 MVB                                                           */
/* 12 Sep 97 MVB - fixed logic for checking _two_ bits of status, not one  */
/* 15 Sep 97 MVB - fixed SOUNDSTATUS bug (was checking wrong STATUS)       */
/*                                                                         */
/***************************************************************************/

int snd_send_data_realtime (unsigned int data)

{ /* snd_send_data_realtime() */

int   status;
int   time;

/***************************************************************************/

	/* wait for the port to clear up */

	_ioctl (5, FIOCSTARTTIMER1, 0);
	_ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

	while ((status & HTS_DATA_REALTIME) != HTS_DATA_REALTIME)
		{
		_ioctl(5, FIOCGETTIMER1, (int)&time);

	   if (time > SND_TIMEOUT)
	      {
		   #ifdef DEBUG
		   		fprintf (stderr, "snd_send_data_realtime(): ERROR time out (data:0x%04X, status:0x%04X)\n",
		                 data, status);
		   #endif

		   /* stop the timeout data */
		   _ioctl (5, FIOCSTOPTIMER1, 0);

		   #if SND_DEBUG_ISR_TIMEOUT
		        #ifdef DEBUG
		   			/* !!! this will lock up the system !!! */
		   			snd_dump_port();
				#endif
		   #endif

	       return ERROR;
		   }

		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);
		}

	/* stop the timeout data */
	_ioctl (5, FIOCSTOPTIMER1, 0);

   /* write the data */
   _ioctl(4, FIOCSETSOUNDDATA, data & SND_MASK16);

   return OK;

} /* snd_send_data_realtime() */

/***** END of snd_send_data_realtime() *************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_dump_port()                                               */
/*                                                                         */
/* Sits in a tight loop and repeatedly reads the sound-to-host data port.  */
/* Does not check any handshaking bits; just reads it back to back in a    */
/* loop.                                                                   */
/*                                                                         */
/* This is a debug function to grab info coming back from the sound        */
/* system in the event of a lockout/timeout situation.                     */
/*                                                                         */
/* (c) 1997 Midway Games Inc.                                              */
/*                                                                         */
/* 19 Sep 97 MVB                                                           */
/*                                                                         */
/***************************************************************************/

void snd_dump_port (void)

{ /* snd_dump_port() */

int data;
int status;

/***************************************************************************/


	while (1)
		{

	    /* read the data */
   		_ioctl(4, FIOCGETSOUNDDATA, (int)(&data));

		/* read the status */
		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);

		/* print them out */
		fprintf (stderr, "snd_dump_port(): data:%04X, status:%04X\n",
		         data, status);

		}


} /* snd_dump_port() */

/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_send_command()                                            */
/*                                                                         */
/* The sound monitor ROM code always returns an 0x0A whenever it is ready  */
/* for a command or has completed a command and is ready for another.      */
/* This function looks for the 0x0A and sends out a command.               */
/*                                                                         */
/* Note that the sound boot rom monitor code is not running interrupts,    */
/* so it sits with the completion code and will not                        */
/* issue another "ready for command" code until you finish off the         */
/* cycle by reading the completion code.                                   */
/*                                                                         */
/* Returns a 0 if successful, or a time out or non-zero error if not OK.   */
/*                                                                         */
/* See the doc for the list of available sound boot ROM functions and      */
/* completion codes.                                                       */
/*                                                                         */
/* In this code, 'C31 write-back is on. If you don't do the dummy write,   */
/* then the sound system will crash.                                       */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_send_command (unsigned int command)

{ /* snd_send_command() */

int   status;
int   time;

/***************************************************************************/

	/* wait for the monitor ready signal to show up */
	/* it should be there if we called this function, but wait */
	/* for it nonetheless */

	_ioctl (5, FIOCSTARTTIMER1, 0);
	_ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

	while (!(status & STH_DATA_READY))
		{

		_ioctl (5, FIOCGETTIMER1, (int)&time);

		if (time > SND_TIMEOUT)
			{
#ifdef DEBUG
		    fprintf (stderr, "snd_send_command(): ERROR time out (command:0x%04X, status:0x%04X)\n",
		             command, status);
#endif

			/* stop the timeout timer */
			_ioctl (5, FIOCSTOPTIMER1, 0);

			return ERROR;
			}

		_ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

		}

	/* stop the timeout timer */
	_ioctl (5, FIOCSTOPTIMER1, 0);

	/* if the ready bit has gone high, then data is ready */
	/* make sure it's the 0x0A */

	_ioctl (4, FIOCGETSOUNDDATA, (int)&status);
	if ((status & SND_MASK16) != SND_MONITOR_READY)
		return ERROR;

	/* dummy write-back */
	_ioctl (4, FIOCSETSTHDATA, 0);


	/* send the actual command */
	_ioctl (4, FIOCSETSOUNDDATA, command);

	return OK;

} /* snd_send_command() */
#endif
/***** END of snd_send_command() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_wait_for_completion()                                     */
/*                                                                         */
/* Each time the sound boot ROM completes a diagnostic test, it returns    */
/* a unique completion code. Each test will take a varying amount of time, */
/* so this function waits for the completion code.                         */
/*                                                                         */
/* Note that the timeout here is on the order of a second, since things    */
/* like the "BONG" command can take that long.                             */
/*                                                                         */
/* Returns the 16-bit completion code, unless there is a timeout error.    */
/*                                                                         */
/* !!! Note that this function SLEEPS and must be called by a process. !!! */
/*                                                                         */
/* Copyright (c) 1995 Midway Manufacturing Company - MVB                   */
/*                                                                         */
/***************************************************************************/
#if defined(SEATTLE)
unsigned int snd_wait_for_completion (void)

{ /* snd_wait_for_completion() */

volatile unsigned int completion_code;

int timeout;
int tick_count;
int status;

/***************************************************************************/

  tick_count = 0;

  timeout = 120 * SND_LONG_TIMEOUT;

  _ioctl (4, FIOCGETSOUNDSTATUS, (int)&status);

  while (!(status & STH_DATA_READY))
        {

        if (++tick_count > timeout)
				{
#ifdef DEBUG
			    fprintf (stderr, "snd_wait_for_completion(): ERROR time out\n");
#endif
            	return ERROR;
				}

        sleep (1);

        _ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);

        }

   // completion_code = (*snd_sth_data & SND_MASK16);
   _ioctl (4, FIOCGETSOUNDDATA, (int)&completion_code);
   completion_code &= SND_MASK16;

   /* required dummy write-back */
   // *snd_sth_data = 0x0000;
   _ioctl (4, FIOCSETSTHDATA, 0);

   return completion_code;

} /* snd_wait_for_completion() */
#else
extern void	*cur_proc;

unsigned int snd_wait_for_completion (void)
{
	volatile unsigned int completion_code;
	int timeout;
	int tick_count;

	tick_count = 0;
	timeout = 120 * SND_LONG_TIMEOUT;
	while(!(get_snd_status() & STH_DATA_READY))
	{
		if(cur_proc)
		{
			if(++tick_count > timeout)
			{
#ifdef DEBUG
				fprintf (stderr, "snd_wait_for_completion(): ERROR time out\n");
#endif
				return ERROR;
			}
			sleep (1);
		}
	}

   // completion_code = (*snd_sth_data & SND_MASK16);
	completion_code = get_snd_data();

	set_snd_data(0);

   return completion_code;

} /* snd_wait_for_completion() */
#endif

/***** END of snd_wait_for_completion() ************************************/
#if defined(SEATTLE)
/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_delay()                                                   */
/*                                                                         */
/* Delay proportional to value passed in. Used by sound reset and data     */
/* transfer functions, where a small (microseconds) delay is needed.       */
/*                                                                         */
/* Change SND_DELAY_MULTIPLIER to globally increase or decrease the delay. */
/* One time through the delay loop takes about 500,000 timer ticks.        */
/*                                                                         */
/* Copyright (c) 1995 Midway Manufacturing Company - MVB                   */
/*                                                                         */
/***************************************************************************/

void snd_delay (volatile int delay_time)

{ /* snd_delay() */

int   time;

/***************************************************************************/

	delay_time *= SND_DELAY_MULTIPLIER;

	// start_timer ();
	_ioctl (5, FIOCSTARTTIMER2, 0);

	do {
		_ioctl(5, FIOCGETTIMER2, (int)&time);
		} while (time < delay_time);

	_ioctl (5, FIOCSTOPTIMER2, 0);

} /* snd_delay() */

/***** END of snd_delay() **************************************************/
#endif

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_get_boot_version()                                        */
/*                                                                         */
/* Queries the sound system for the version of the sound boot ROM.         */
/*                                                                         */
/* The sound boot ROM version is a 16-bit number. Upper 8 bits are major   */
/* revision, lower 8 bits are minor revision, e.g. 0x0102 is version 1.02. */
/*                                                                         */
/* Returns the version or ERROR.                                           */
/*                                                                         */
/* !!! Note that this function SLEEPS and must be called by a process. !!! */
/*                                                                         */
/* Copyright (c) 1995 Midway Manufacturing Company - MVB                   */
/*                                                                         */
/***************************************************************************/

unsigned int snd_get_boot_version (void)

{ /* snd_get_boot_version() */

int status;
unsigned int version;

/***************************************************************************/

		/* request the version */
		status = snd_send_command (SND_CMD_ROM_VERSION);

		if (status != OK)
			return ERROR;

		/* wait for the version back */
		version = snd_wait_for_completion();

		if (version == ERROR)
			return ERROR;
		else
			return (version & SND_MASK16);

} /* snd_get_boot_version() */

/***** END of snd_get_boot_version() ***************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_get_sdrc_version()                                        */
/*                                                                         */
/* Queries the sound system for the version of the SDRC ASIC (Sound D/RAM  */
/* Controller).                                                            */
/*                                                                         */
/* The SDRC ASIC version number is a single byte.                          */
/*                                                                         */
/* Returns the version or ERROR.                                           */
/*                                                                         */
/* !!! Note that this function SLEEPS and must be called by a process. !!! */
/*                                                                         */
/* Copyright (c) 1995 Midway Manufacturing Company - MVB                   */
/*                                                                         */
/***************************************************************************/

unsigned int snd_get_sdrc_version (void)

{ /* snd_get_sdrc_version() */

int status;
unsigned int version;

/***************************************************************************/

		/* request the version */
		status = snd_send_command (SND_CMD_ASIC_VER);

		if (status != OK)
			return ERROR;

		/* wait for the version back */
		version = snd_wait_for_completion();

		if (version == ERROR)
			return ERROR;
		else
			return (version & SND_MASK16);

} /* snd_get_sdrc_version() */

/***** END of snd_get_sdrc_version() ***************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_get_pmint_sum()                                           */
/*                                                                         */
/* Requests the checksum of the sound DSP's internal program memory. This  */
/* can be used to check if the sound boot ROM got booted into the sound    */
/* program memory correctly.                                               */
/*                                                                         */
/* The checksum returned is a 16-bit value.                                */
/*                                                                         */
/* Returns the checksum or ERROR.                                          */
/*                                                                         */
/* !!! Note that this function SLEEPS and must be called by a process. !!! */
/*                                                                         */
/* Copyright (c) 1995 Midway Manufacturing Company - MVB                   */
/*                                                                         */
/***************************************************************************/

unsigned int snd_get_pmint_sum (void)

{ /* snd_get_pmint_sum() */

int status;
unsigned int pmint_sum;

/***************************************************************************/

		/* request the version */
		status = snd_send_command (SND_CMD_PMINT_SUM);

		if (status != OK)
			return ERROR;

		/* wait for the version back */

		pmint_sum = snd_wait_for_completion();

		if (pmint_sum == ERROR)
			return ERROR;
		else
			return (pmint_sum & SND_MASK16);

} /* snd_get_pmint_sum() */

/***** END of snd_get_pmint_sum() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_test_port()                                               */
/*                                                                         */
/* Does a walking ones and zeros test on the host-to-sound interface path. */
/* This is a 16-bit path with handshaking done through status bits in      */
/* I/O ASIC.                                                               */
/*                                                                         */
/* Note that the sound boot ROM "echo" function returns the complement of  */
/* the value sent.                                                         */
/*                                                                         */
/* !!! Note that this function SLEEPS and must be called by a process. !!! */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_test_port (void)

{ /* snd_test_port() */

int i;
int j;
int status;

unsigned int test_value;
unsigned int returned_value;

/***************************************************************************/

 j = 1;

 for (i=0; i < (16 + 1) ;i++)
    {

   /* shift a 1 through 16 locations */

   test_value = (j << i) & SND_MASK16;


   /* walking ones portion of test for this bit */

    status = snd_send_command (SND_CMD_ECHO);

   if (status != OK)
       return ERROR;

   status = snd_send_data (test_value);

   if (status != OK)
       return ERROR;

    returned_value = snd_wait_for_completion();


   /* if value read back does not equal value written */
    /* then set error status and save expected and actual */

   if ((test_value & SND_MASK16) != (~returned_value & SND_MASK16))
      return ERROR;


   /* walking zeros portion of test for this bit */

   test_value = (~test_value) & (SND_MASK16);

    status = snd_send_command (SND_CMD_ECHO);

   if (status != OK)
       return ERROR;

   status = snd_send_data (test_value);

   if (status != OK)
       return ERROR;

    returned_value = snd_wait_for_completion();


   /* if value read back does not equal value written then error */

   if ((test_value & SND_MASK16) != (~returned_value & SND_MASK16))
      return ERROR;

   } /* for i */

 return OK;

} /* snd_test_port() */

/***** END of snd_test_port() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_file()                                               */
/*                                                                         */
/* Simple wrapper for loading sound operating system files and sound bank  */
/* files. Loads a file from disk into malloc'ed memory.                    */
/*                                                                         */
/* Returns a pointer to the data, or returns NULL if an error.             */
/*                                                                         */
/***************************************************************************/

unsigned int *snd_load_file (char *file_name)

{ /* snd_load_file() */

FILE *file_handle;           /* file ptr */
unsigned int *data_buffer;   /* ptr to data read from file */
unsigned int file_size;        /* file size in !!!bytes!!! */
unsigned int num_read;       /* actual num read in 32-bit words */
struct ffblk   ffblk;
unsigned long	stored_crc;
unsigned long	gen_crc;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_file(): begin\r\n");
  fprintf (stderr, "snd_load_file(): file name: %s\r\n", file_name);
 #endif

   // Get the info about the file
   if(findfirst(file_name, &ffblk, 0))
   {
#ifdef DEBUG
      fprintf(stderr, "snd_load_file(): ERROR can not find file: %s\r\n", file_name);
#endif
      return(NULL);
   }
   
  /* open the file */
 file_handle = fopen (file_name, "rb");
 if (file_handle == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_file(): ERROR cannot open file %s for reading.\r\n", file_name);
#endif
   return NULL;
   }

	// Are we CRC checking ?
	if(snd_use_crc)
	{
		// YES - Set size to file size minus size of CRC value
		file_size = ffblk.ff_fsize - 4;
	}
	else
	{
		// NOPE - Set size of size of file
		file_size = ffblk.ff_fsize;
	}

	// Are we CRC checking ?
	if(snd_use_crc)
	{
		// YES - Read CRC from file
		if(fread(&stored_crc, sizeof(stored_crc), 1, file_handle) != 1)
		{
			fprintf(stderr, "snd_load_file() : ERROR can not read CRC from file: %s\n", file_name);
	        fclose (file_handle);
			return(NULL);
		}
	}

 #if SND_DEBUG
  fprintf (stderr, "snd_load_file(): file_size: %d bytes\r\n", file_size);
 #endif

 /* allocate memory to hold it */
 /* this is in bytes */  
 /* the arg to malloc() is bytes */
  data_buffer = malloc (file_size);
  if (data_buffer == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_file(): ERROR cannot malloc %d bytes.\r\n", file_size);
#endif
   fclose (file_handle);
   return NULL;  
   }

 /* read in the file */
  num_read = fread (data_buffer, 1, file_size, file_handle);
  if (num_read != file_size)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_file(): ERROR reading file %s.\r\n", file_name);
   fprintf (stderr, "file size:%d bytes, actually read:%d bytes\r\n", file_size, num_read);
#endif
   /* must be sure to free memory if error */
   free (data_buffer);
   fclose (file_handle);
   return NULL;  
   }

 /* close the file */
 fclose (file_handle);

	// Are we CRC checking ?
	if(snd_use_crc)
	{
		// YES - generate a CRC on the file read
		gen_crc = crc((unsigned char *)data_buffer, file_size);

		// Does the gnerated CRC match the stored CRC ?
		if(gen_crc != stored_crc)
		{
			// NOPE - ERROR
			fprintf(stderr, "CRC error detected in file: %s\n", file_name);

			// Is there an audit function
			if(snd_crc_fail_audit_func)
			{
				// YES - Call it
				snd_crc_fail_audit_func();
			}

			// Return fail
            fclose (file_handle);
	        free (data_buffer);
			return(NULL);
		}
	}

 #if SND_DEBUG
  fprintf (stderr, "snd_load_file(): done\r\n");
 #endif

 /* pass back the pointer to loaded data */
  return data_buffer;

} /* snd_load_file() */

/***** END of snd_load_file() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_opsys()                                              */
/*                                                                         */
/* When the sound system is reset, it is running the sound boot ROM code,  */
/* which contains diags and a monitor/loader. The actual DCS2 sound system */
/* code must be downloaded to the sound DSP before sounds can be played.   */
/*                                                                         */
/* The sound DSP has two kinds of memory: program RAM (PM) and data RAM    */
/* (DM). PM is 24-bit and DM is 16-bit, so there are separate load         */
/* routines for each.                                                      */
/*                                                                         */
/* The sound operating system consists of four files:                      */
/*                                                                         */
/* 1. _comp.bin - 24-bit external program memory image                     */
/*                                                                         */
/* 2. _osys.bin - 24-bit internal on-chip memory image                     */
/*                                                                         */
/* 3. dm_ext.bin - 16-bit external data memory image                       */
/*                                                                         */
/* 4. dm_int.bin - 16-bit internal on-chip memory image                    */
/*                                                                         */
/* These files must be on the game hard disk.                              */
/*                                                                         */
/* See also snd_load_pm() and snd_load_dm().                               */
/*                                                                         */
/* Returns OK if successful or a non-zero error if problems.               */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_load_opsys (void)

{ /* snd_load_opsys() */

int status;
unsigned int sound_ack;

/***************************************************************************/

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_load_opsys(): begin\r\n");
	#endif

	if (snd_reset_ack() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR resetting sound system.\r\n");
		#endif
		return ERROR;
		}

	/* The following load addresses are hard set based on         */
	/* the sound system memory map. See the sound system docs for */
	/* more information.                                          */

	if (snd_load_pm ("comp.bin", 0x2800, 0x37FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR loading PM external.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_pm ("osys.bin", 0x0000, 0x03FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR loading PM internal.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_dm ("dm_ext.bin", 0x0800, 0x37FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR loading DM external.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_dm ("dm_int.bin", 0x3800, 0x39FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR loading DM internal.\r\n");
		#endif
		return ERROR;
		}

	/* now that the code images have been loaded into memory by */
	/* the boot monitor, tell it to "run" the just-loaded opsys */
	status = snd_send_data (SND_CMD_RUN);
	if (status != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR sending run command.\r\n");
		#endif
		return ERROR;
		}

	/* if the code ran OK then there should be a 0x0C waiting */
	/* see sound system docs for more info on ack codes */
	status = snd_get_data (&sound_ack);
	if ((sound_ack & SND_MASK16) != SND_OPSYS_READY)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys(): ERROR getting opsys ready ack.\r\n");
		#endif
		return ERROR;
		}

	#if SND_DEBUG
		fprintf (stderr, "snd_load_opsys(): done\r\n");
	#endif

	return OK;

} /* snd_load_opsys() */

/***** END of snd_load_opsys() *********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_opsys_multi()                                        */
/*                                                                         */
/* Same as snd_load_opsys() except that the files to load and the DSP      */
/* load addresses come from a table instead of being hard-coded.           */
/*                                                                         */
/* This is so we can switch between the "normal" DCS system and the new    */
/* FIFO-based streaming.                                                   */
/*                                                                         */
/* Pass in one of the enums SND_OPSYS_0122 or SND_OPSYS_0223 etc. which    */
/* are defined in SOUND.H                                                  */
/*                                                                         */
/* Copyright (c) 1997 Midway Games Inc.                                    */
/*                                                                         */
/* 30 Nov 97                                                               */
/*                                                                         */
/***************************************************************************/

int snd_load_opsys_multi (int opsys_type)

{ /* snd_load_opsys_multi() */

int status;
unsigned int sound_ack;
int i;

/* load list pointer */
snd_opsys_file_t *ll;

/***************************************************************************/

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_load_opsys_multi(): begin\r\n");
	#endif

	if (snd_reset_ack() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_multi(): ERROR resetting sound system.\r\n");
		#endif
		return ERROR;
		}


	/* the load_list is global to this file */
	/* enums to index it are in SOUND.H */
	ll = snd_opsys_load_list [opsys_type];

	i = 0;

	while (ll[i].filename != NULL)
		{
		switch (ll[i].type)
			{

			case SND_OPSYS_TYPE_PM:
				if (snd_load_pm (ll[i].filename, ll[i].start_address, ll[i].end_address))
					{
					#ifdef DEBUG
						fprintf (stderr, "snd_load_opsys_multi(): ERROR loading PM %s.\r\n",
						         ll[i].filename);
					#endif
					return ERROR;
					}
				break;

			case SND_OPSYS_TYPE_DM:
				if (snd_load_dm (ll[i].filename, ll[i].start_address, ll[i].end_address))
					{
					#ifdef DEBUG
						fprintf (stderr, "snd_load_opsys_multi(): ERROR loading DM %s.\r\n",
						         ll[i].filename);
					#endif
					return ERROR;
					}
				break;

			default:
				#ifdef DEBUG
					fprintf (stderr, "snd_load_opsys_multi(): ERROR unknown type.\r\n");
				#endif
				return ERROR;
				break;

			} /* switch */

		/* move to the next in the list */
		i++;

		} /* while */


	/* now that the code images have been loaded into memory by */
	/* the boot monitor, tell it to "run" the just-loaded opsys */
	status = snd_send_data (SND_CMD_RUN);
	if (status != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_multi(): ERROR sending run command.\r\n");
		#endif
		return ERROR;
		}

	/* if the code ran OK then there should be a 0x0C waiting */
	/* see sound system docs for more info on ack codes */
	status = snd_get_data (&sound_ack);
	if ((sound_ack & SND_MASK16) != SND_OPSYS_READY)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_multi(): ERROR getting opsys ready ack.\r\n");
		#endif
		return ERROR;
		}

	#if SND_DEBUG
		fprintf (stderr, "snd_load_opsys_multi(): done\r\n");
	#endif

	return OK;

} /* snd_load_opsys_multi() */

/***** END of snd_load_opsys_multi() ***************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_opsys_from_memory()                                  */
/*                                                                         */
/* Same as snd_load_opsys, except that the sound code is loaded from a     */
/* memory location rather than from a disk file.                           */
/*                                                                         */
/* Used by the diagnostics.                                                */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_load_opsys_from_memory (void *os, void *comp, void *dm_ext, void *dm_int)

{ /* snd_load_opsys_from_memory() */

int status;
unsigned int sound_ack;

/***************************************************************************/

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_load_opsys_from_memory(): begin\r\n");
	#endif

	if (snd_reset_ack() != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR resetting sound system.\r\n");
		#endif
		return ERROR;
		}

	/* The following load addresses are hard set based on         */
	/* the sound system memory map. See the sound system docs for */
	/* more information.                                          */

	if (snd_load_pm_from_memory (comp, 0x2800, 0x37FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR loading PM external.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_pm_from_memory (os, 0x0000, 0x03FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR loading PM internal.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_dm_from_memory (dm_ext, 0x0800, 0x37FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR loading DM external.\r\n");
		#endif
		return ERROR;
		}

	if (snd_load_dm_from_memory (dm_int, 0x3800, 0x39FF))
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR loading DM internal.\r\n");
		#endif
		return ERROR;
		}

	/* now that the code images have been loaded into memory by */
	/* the boot monitor, tell it to "run" the just-loaded opsys */
	status = snd_send_data (SND_CMD_RUN);
	if (status != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR sending run command.\r\n");
		#endif
		return ERROR;
		}

	/* if the code ran OK then there should be a 0x0C waiting */
	/* see sound system docs for more info on ack codes */
	status = snd_get_data (&sound_ack);
	if ((sound_ack & SND_MASK16) != SND_OPSYS_READY)
		{
		#ifdef DEBUG
			fprintf (stderr, "snd_load_opsys_from_memory(): ERROR getting opsys ready ack.\r\n");
		#endif
		return ERROR;
		}

	#if SND_DEBUG
		fprintf (stderr, "snd_load_opsys_from_memory(): done\r\n");
	#endif

	return OK;

} /* snd_load_opsys_from_memory() */

/***** END of snd_load_opsys_from_memory() *********************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_pm()                                                 */
/*                                                                         */
/* When the sound system is reset, it is running the sound boot ROM code,  */
/* which contains diags and a monitor/loader. The actual DCS2 sound system */
/* code must be downloaded to the sound DSP before sounds can be played.   */
/*                                                                         */
/* The sound DSP has two kinds of memory: program RAM (PM) and data RAM    */
/* (DM). PM is 24-bit and DM is 16-bit, so there are separate load         */
/* routines for each.                                                      */
/*                                                                         */
/* The start and end addresses are primarily so that the separate internal */
/* and external memory segments can be addressed and loaded as needed.     */
/*                                                                         */
/* The format and handshaking between the host and the sound DSP is        */
/* described in more detail in the sound system docs (see Matt Booty or    */
/* Ed Keenan).                                                             */
/*                                                                         */
/* Also see snd_load_dm() and snd_load_opsys().                            */
/*                                                                         */
/* Returns OK if successful or ERROR if timeout or checksum errors.        */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_load_pm (char *file_name, int start, int end)

{ /* snd_load_pm() */

int size;                /* number of 16-bit words to send */

unsigned int data[8];    /* swizzle array for 32 to 24 to 16 convert */
unsigned int *buffer;    /* malloc'ed data buffer for disk data */

int i;                 /* counters for send loop */
int j;
int k;

int status;                    /* status of xfer to and from snd DSP */
unsigned int checksum;       /* our computed checksum as data sent */
unsigned int sound_checksum;  /* checksum returned from sound DSP */
unsigned int sound_ack;        /* monitor alive signal back from DSP */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_pm(): begin\r\n");
 #endif

 /* for each 24-bit word sent to the sound DSP */
  /* we actually send two 16-bit words */
 
 size = (end - start + 1) * 2;
 
 /* send the load command */
  /* format is command, start, end, type, data... */
                           
 if (snd_send_data (SND_CMD_LOAD) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending load cmd.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (start) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending start addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (end) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending end addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (SND_LOAD_TYPE_PM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending memory type.\r\n");     
#endif
    return ERROR;      
   }

 /* Here is some fun! We have 24-bit data, stored in 32-bit words          */
  /* _across_ word boundaries on the disk. We have to send this to the       */
 /* sound DSP in 16-bit chunks, with each 24-bit word requiring two        */
 /* 16-bit xfers.  The first multiple that fits an integer number of all   */
 /* of these is a block of four 32-bit words.  3 x 32-bit holds 4 x 24-bit */
  /* DSP words which is sent as 8 x 16-bit words.                          */

  /* Then there is MORE fun! We have to do an endian switch too. The         */
 /* swizzling here allows us to just copy the exact .BIN file from         */
  /* the PC to the game disk and load it here with no massaging in between.  */

 buffer = snd_load_file (file_name);

 if (buffer == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR loading file %s.\r\n", file_name);      
#endif
    return ERROR;      
   }

 i = 0;
 k = 0;
  checksum = 0;

 /* i counts the 32-bit words */
 /* k is the index into the eight word array */
 /* j is the number of 16-bit words sent to sound DSP */
 
 data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
 data[1] = (SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);

 data[2] = ((buffer[i] & SND_BYTE3) >> 16) | (buffer[i+1] & SND_BYTE0);
 data[3] = (SND_BYTE1) | ((buffer[++i] & SND_BYTE1) >> 8);

 data[4] = ((buffer[i] & SND_BYTE2) >> 8) | ((buffer[i] & SND_BYTE3) >> 24);
 data[5] = (SND_BYTE1) | (buffer[++i] & SND_BYTE0);

 data[6] = (buffer[i] & SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);
 data[7] = (SND_BYTE1) | ((buffer[i++] & SND_BYTE3) >> 24);


 for (j=0; j < size; j++)
   {                   
 
   if (snd_send_data (data[k]) != OK)
     {
      free (buffer);
#ifdef DEBUG
     fprintf (stderr, "snd_load_pm(): ERROR sending data.\r\n");
#endif
      return ERROR;
      }

   /* the checksum is just a straight sum of each 16-bit word sent */
     
   checksum += data[k];

    k++;

   if (k == 8)
     {

     k = 0;

     data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
     data[1] = (SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);

     data[2] = ((buffer[i] & SND_BYTE3) >> 16) | (buffer[i+1] & SND_BYTE0);
     data[3] = (SND_BYTE1) | ((buffer[++i] & SND_BYTE1) >> 8);

     data[4] = ((buffer[i] & SND_BYTE2) >> 8) | ((buffer[i] & SND_BYTE3) >> 24);
     data[5] = (SND_BYTE1) | (buffer[++i] & SND_BYTE0);

     data[6] = (buffer[i] & SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);
     data[7] = (SND_BYTE1) | ((buffer[i++] & SND_BYTE3) >> 24);

     } /* if k */

   } /* for j */

  free (buffer);


  /* after the data is loaded, the sound DSP will return its */
  /* checksum... make sure it matches ours */
 status = snd_get_data (&sound_checksum);   
 if ((checksum & SND_MASK16) != sound_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR reading checksum.\r\n");      
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               sound_checksum);      
#endif
    return ERROR;      
   }

 /* after the checksum, monitor should go back to ready mode */
 status = snd_get_data (&sound_ack);
  if (sound_ack != SND_MONITOR_READY)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR getting monitor ready ack.\r\n");     
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_pm(): done\r\n");
 #endif
 
   return OK;

} /* snd_load_pm() */




int snd_load_pm_from_memory(unsigned int *buffer, int start, int end)

{ /* snd_load_pm_from_memory() */

int size;                /* number of 16-bit words to send */

unsigned int data[8];    /* swizzle array for 32 to 24 to 16 convert */

int i;                 /* counters for send loop */
int j;
int k;

int status;                    /* status of xfer to and from snd DSP */
unsigned int checksum;       /* our computed checksum as data sent */
unsigned int sound_checksum;  /* checksum returned from sound DSP */
unsigned int sound_ack;        /* monitor alive signal back from DSP */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_pm_from_memory(): begin\r\n");
 #endif

 /* for each 24-bit word sent to the sound DSP */
  /* we actually send two 16-bit words */
 
 size = (end - start + 1) * 2;
 
 /* send the load command */
  /* format is command, start, end, type, data... */
                           
 if (snd_send_data (SND_CMD_LOAD) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR sending load cmd.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (start) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR sending start addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (end) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR sending end addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (SND_LOAD_TYPE_PM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR sending memory type.\r\n");     
#endif
    return ERROR;      
   }

 /* Here is some fun! We have 24-bit data, stored in 32-bit words          */
  /* _across_ word boundaries on the disk. We have to send this to the       */
 /* sound DSP in 16-bit chunks, with each 24-bit word requiring two        */
 /* 16-bit xfers.  The first multiple that fits an integer number of all   */
 /* of these is a block of four 32-bit words.  3 x 32-bit holds 4 x 24-bit */
  /* DSP words which is sent as 8 x 16-bit words.                          */

  /* Then there is MORE fun! We have to do an endian switch too. The         */
 /* swizzling here allows us to just copy the exact .BIN file from         */
  /* the PC to the game disk and load it here with no massaging in between.  */

 if (buffer == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR loading from memory.\r\n");      
#endif
    return ERROR;      
   }

 i = 0;
 k = 0;
  checksum = 0;

 /* i counts the 32-bit words */
 /* k is the index into the eight word array */
 /* j is the number of 16-bit words sent to sound DSP */
 
 data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
 data[1] = (SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);

 data[2] = ((buffer[i] & SND_BYTE3) >> 16) | (buffer[i+1] & SND_BYTE0);
 data[3] = (SND_BYTE1) | ((buffer[++i] & SND_BYTE1) >> 8);

 data[4] = ((buffer[i] & SND_BYTE2) >> 8) | ((buffer[i] & SND_BYTE3) >> 24);
 data[5] = (SND_BYTE1) | (buffer[++i] & SND_BYTE0);

 data[6] = (buffer[i] & SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);
 data[7] = (SND_BYTE1) | ((buffer[i++] & SND_BYTE3) >> 24);


 for (j=0; j < size; j++)
   {                   
 
   if (snd_send_data (data[k]) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_pm_from_memory(): ERROR sending data.\r\n");
#endif
      return ERROR;
      }

   /* the checksum is just a straight sum of each 16-bit word sent */
     
   checksum += data[k];

    k++;

   if (k == 8)
     {

     k = 0;

     data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
     data[1] = (SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);

     data[2] = ((buffer[i] & SND_BYTE3) >> 16) | (buffer[i+1] & SND_BYTE0);
     data[3] = (SND_BYTE1) | ((buffer[++i] & SND_BYTE1) >> 8);

     data[4] = ((buffer[i] & SND_BYTE2) >> 8) | ((buffer[i] & SND_BYTE3) >> 24);
     data[5] = (SND_BYTE1) | (buffer[++i] & SND_BYTE0);

     data[6] = (buffer[i] & SND_BYTE1) | ((buffer[i] & SND_BYTE2) >> 16);
     data[7] = (SND_BYTE1) | ((buffer[i++] & SND_BYTE3) >> 24);

     } /* if k */

   } /* for j */


  /* after the data is loaded, the sound DSP will return its */
  /* checksum... make sure it matches ours */
 status = snd_get_data (&sound_checksum);   
 if ((checksum & SND_MASK16) != sound_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR reading checksum.\r\n");      
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               sound_checksum);      
#endif
    return ERROR;      
   }

 /* after the checksum, monitor should go back to ready mode */
 status = snd_get_data (&sound_ack);
  if (sound_ack != SND_MONITOR_READY)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm_from_memory(): ERROR getting monitor ready ack.\r\n");     
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_pm_from_memory(): done\r\n");
 #endif
 
   return OK;

} /* snd_load_pm_from_memory() */


/***** END of snd_load_pm() ************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_dm()                                                 */
/*                                                                         */
/* When the sound system is reset, it is running the sound boot ROM code,  */
/* which contains diags and a monitor/loader. The actual DCS2 sound system */
/* code must be downloaded to the sound DSP before sounds can be played.   */
/*                                                                         */
/* The sound DSP has two kinds of memory: program RAM (PM) and data RAM    */
/* (DM). PM is 24-bit and DM is 16-bit, so there are separate load         */
/* routines for each.                                                      */
/*                                                                         */
/* The start and end addresses are primarily so that the separate internal */
/* and external memory segments can be addressed and loaded as needed.     */
/*                                                                         */
/* The format and handshaking between the host and the sound DSP is        */
/* described in more detail in the sound system docs (see Matt Booty or    */
/* Ed Keenan).                                                             */
/*                                                                         */
/* Also see snd_load_dp() and snd_load_opsys().                            */
/*                                                                         */
/* This is basically the same as snd_load_pm() except for the 32 to 24 to  */
/* 16 byte/word swizzle.                                                   */
/*                                                                         */
/* Returns OK if successful or ERROR if timeout or checksum errors.        */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_load_dm (char *file_name, int start, int end)

{ /* snd_load_dm() */

int size;                      /* number of 16-bit words to send */

unsigned int data[2];          /* swizzle array for 32 to 16 convert */
unsigned int *buffer;          /* malloc'ed data buffer for disk data */

int status;

int i;                       /* counters for send loop */
int j;
int k;

unsigned int checksum;       /* our computed checksum as data sent */
unsigned int sound_checksum; /* checksum returned from sound DSP */
unsigned int sound_ack;        /* monitor alive signal back from DSP */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_dm(): begin\r\n");
 #endif

 /* we are sending straight 16-bit words to the DSP */

 size = (end - start + 1);
 
  /* send the load command */
  /* format is command, start, end, type, data... */
                           
 if (snd_send_data (SND_CMD_LOAD) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending load cmd.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (start) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending start addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (end) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending end addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (SND_LOAD_TYPE_DM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending memory type.\r\n");     
#endif
    return ERROR;      
   }


 /* load the file */
 buffer = snd_load_file (file_name);


 /* i counts the 32-bit words */
 /* k is the index into the two word array */
 /* j is the number of 16-bit words sent to sound DSP */

  i = 0;
 k = 0;
  checksum = 0;

  /* for each 32-bit word on disk, pull out two 16-bit words to send */

 data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
 data[1] = ((buffer[i] & SND_BYTE2) >> 8)| ((buffer[i] & SND_BYTE3) >> 24);
 i++;

 
 for (j=0; j < size; j++)
   {                   
 
   if (snd_send_data (data[k]) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_dm(): ERROR sending data.\r\n");
#endif
      free (buffer);
      return ERROR;
      }

   checksum += data[k];

    k++;

   if (k == 2)
     {
     k = 0;
     data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
       data[1] = ((buffer[i] & SND_BYTE2) >> 8)| ((buffer[i] & SND_BYTE3) >> 24);
     i++;
     } /* if k */

   } /* for */

  free (buffer);


  /* after the data is loaded, the sound DSP will return its */
  /* checksum... make sure it matches ours */
 status = snd_get_data (&sound_checksum);   
 if ((checksum & SND_MASK16) != sound_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_dm(): ERROR reading checksum.\r\n");      
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               sound_checksum);      
#endif
    return ERROR;      
   }


 /* after the checksum, monitor should go back to ready mode */
 status = snd_get_data (&sound_ack);
  if (sound_ack != SND_MONITOR_READY)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_dm(): ERROR getting monitor ready ack.\r\n");     
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_dm(): done\r\n");
 #endif

   return OK;

} /* snd_load_dm() */


int snd_load_dm_from_memory (unsigned int *buffer, int start, int end)

{ /* snd_load_dm_from_memory() */

int size;                      /* number of 16-bit words to send */

unsigned int data[2];          /* swizzle array for 32 to 16 convert */

int status;

int i;                       /* counters for send loop */
int j;
int k;

unsigned int checksum;       /* our computed checksum as data sent */
unsigned int sound_checksum; /* checksum returned from sound DSP */
unsigned int sound_ack;        /* monitor alive signal back from DSP */

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_dm_from_memory(): begin\r\n");
 #endif

 /* we are sending straight 16-bit words to the DSP */

 size = (end - start + 1);
 
  /* send the load command */
  /* format is command, start, end, type, data... */
                           
 if (snd_send_data (SND_CMD_LOAD) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending load cmd.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (start) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending start addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (end) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending end addr.\r\n");      
#endif
    return ERROR;      
   }

 if (snd_send_data (SND_LOAD_TYPE_DM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_pm(): ERROR sending memory type.\r\n");     
#endif
    return ERROR;      
   }


 /* i counts the 32-bit words */
 /* k is the index into the two word array */
 /* j is the number of 16-bit words sent to sound DSP */

  i = 0;
 k = 0;
  checksum = 0;

  /* for each 32-bit word on disk, pull out two 16-bit words to send */

 data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
 data[1] = ((buffer[i] & SND_BYTE2) >> 8)| ((buffer[i] & SND_BYTE3) >> 24);
 i++;

 
 for (j=0; j < size; j++)
   {                   
 
   if (snd_send_data (data[k]) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_dm_from_memory(): ERROR sending data.\r\n");
#endif
      return ERROR;
      }

   checksum += data[k];

    k++;

   if (k == 2)
     {
     k = 0;
     data[0] = ((buffer[i] & SND_BYTE0) << 8) | ((buffer[i] & SND_BYTE1) >> 8);
       data[1] = ((buffer[i] & SND_BYTE2) >> 8)| ((buffer[i] & SND_BYTE3) >> 24);
     i++;
     } /* if k */

   } /* for */


  /* after the data is loaded, the sound DSP will return its */
  /* checksum... make sure it matches ours */
 status = snd_get_data (&sound_checksum);   
 if ((checksum & SND_MASK16) != sound_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_dm_from_memory(): ERROR reading checksum.\r\n");      
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               sound_checksum);      
#endif
    return ERROR;      
   }


 /* after the checksum, monitor should go back to ready mode */
 status = snd_get_data (&sound_ack);
  if (sound_ack != SND_MONITOR_READY)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_dm_from_memory(): ERROR getting monitor ready ack.\r\n");     
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_dm_from_memory(): done\r\n");
 #endif

   return OK;

} /* snd_load_dm_from_memory() */

/***** END of snd_load_dm() ************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_get_opsys_ver()                                           */
/*                                                                         */
/* Requests the version of the sound operating system.                     */
/*                                                                         */
/* The sound op sys version is a 16-bit number. Upper 8 bits are major     */
/* revision, lower 8 bits are minor revision, e.g. 0x0102 is version 1.02. */
/*                                                                         */
/* Returns the version or ERROR.                                           */
/*                                                                         */
/* 18 Sep 97 MVB - changed wait_for_completion() to get_data()             */
/*                                                                         */
/***************************************************************************/

unsigned int snd_get_opsys_ver (void)

{ /* snd_get_opsys_ver() */

unsigned int version;
int status;

/***************************************************************************/

	 /* clear any pending return info */	
	 snd_flush_queues();    	
	 snd_clear_latch();    	

	 if (snd_send_data (SND_CMD_GET_VERSION) != OK)
	    {
#ifdef DEBUG
	    fprintf (stderr, "snd_get_opsys_ver(): ERROR sending request cmd.\r\n");     
#endif
	    return ERROR;      
	    }

	 /* wait for the version back */
	 status = snd_get_data (&version);

	 if (version == ERROR)
	    {
#ifdef DEBUG
	    fprintf (stderr, "snd_get_opsys_ver(): ERROR getting version.\r\n");     
#endif
	    return ERROR;      
	    }
	 else
	   {	
	   return (version & SND_MASK16);
	   }

} /* snd_get_opsys_ver() */

/***** END of snd_get_opsys_ver() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_checksum_banks()                                          */
/*                                                                         */
/* Checksums a range of loaded banks. This is designed to check whether or */
/* not sound banks are still valid. Load the banks, get the checksum, and  */
/* then subsequently request the checksum to see if it has changed. If the */
/* checksum is the same, it can be assumed (but still an assumption) that  */
/* the sound bank data is valid, and does not need to be re-loaded.        */
/*                                                                         */
/* The args are the char names of the first and last banks that define the */
/* range to be checksummed. Therefore, the banks MUST have been loaded     */
/* using snd_bank_load() so that the names are in the list of loaded       */
/* banks.                                                                  */
/*                                                                         */
/* Returns the checksum of the range as a 16-bit int (i.e. sound system    */
/* returns a 16-bit value).                                                */
/*                                                                         */
/* NOTICE!!! This function SLEEPS while it waits for the checksum back.    */
/* It takes about 1.5 seconds to checksum the whole 2MB of sound D/RAM.    */
/*                                                                         */
/* NOTE: Once the checksum command is sent, the sound DSP spends 100% of   */
/* its realtime in a loop doing the checksum as fast as it can. Any sound  */
/* call that is sent during the checksum will cancel the checksum process  */
/* and immediately execute the sound call. If you interrupt the checksum   */
/* with a sound call, you will never get anything back (which is fine,     */
/* but just so you know). This is designed so you can do the checksum      */
/* during silent attract mode, but if the game starts, you can just blow   */
/* it off and start sending sound calls.                                   */
/*                                                                         */
/* Returns -1 if error.                                                    */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 2 July 97 MVB                                                           */
/*                                                                         */
/***************************************************************************/

int snd_checksum_banks (char *first_bank_name, char *last_bank_name)

{ /* snd_checksum_banks() */

snd_bank_node_t *bp;	/* bank ptr */

int dram_start;
int dram_end;

int checksum;

/***************************************************************************/

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_checksum_banks(): begin\r\n");
	#endif


	/* find the first bank in the list */
	/* snd_bank_list is the global bank list */
    bp = snd_bank_list;
    while (bp != NULL)
		{
	    if (!strcmp (bp->name, first_bank_name))
			break;
		bp = bp->next;
     	} 

	/* error if bank not found */
	/* otherwise save the start address */
	if (bp == NULL)
		{
#ifdef DEBUG
   		fprintf (stderr, "snd_checksum_banks(): ERROR bank %s not loaded.\r\n",
   		         first_bank_name);     
#endif
    	return -1;      
		}
	else
		{
		dram_start = bp->dram_start;
		#if SND_DEBUG
			fprintf (stderr, "\r\n");
			fprintf (stderr, "snd_checksum_banks(): bank %s starts at %04X\r\n",
			         bp->name, dram_start);
		#endif
		}


	/* find the last bank in the list */
	/* snd_bank_list is the global bank list */
    bp = snd_bank_list;
    while (bp != NULL)
		{
	    if (!strcmp (bp->name, last_bank_name))
			break;
		bp = bp->next;
     	} 

	/* error if bank not found */
	/* otherwise save the end address */
	if (bp == NULL)
		{
#ifdef DEBUG
   		fprintf (stderr, "snd_checksum_banks(): ERROR bank %s not loaded.\r\n",
   		         last_bank_name);     
#endif
    	return -1;      
		}
	else
		{
		dram_end = bp->dram_end;
		#if SND_DEBUG
			fprintf (stderr, "\r\n");
			fprintf (stderr, "snd_checksum_banks(): bank %s ends at %04X\r\n",
			         bp->name, dram_end);
		#endif
		}

	/* make sure that the addresses are valid, i.e. start is */
	/* actually less than end */
	if (dram_start >= dram_end)
		{
#ifdef DEBUG
   		fprintf (stderr, "snd_checksum_banks(): ERROR invalid memory range.\r\n");
   		fprintf (stderr, "                      bank '%s' starts at %04X\r\n", 
   		         first_bank_name, dram_start);
   		fprintf (stderr, "                      bank '%s' ends at %04X\r\n", 
   		         last_bank_name, dram_end);
#endif
    	return -1;      
		}

	/* all should be in order now -- actually get the checksum */
	/* the addresses in the bank list structure are in BYTE WISE format */
	/* the sound system expects WORD WISE addresses, so div by 2 */
	/* also the dram_end is actually the _next_ valid address, so sub 1 */
	checksum = snd_checksum_dram (dram_start/2, dram_end/2 - 1);
	if (checksum < 0)
		{
#ifdef DEBUG
   		fprintf (stderr, "snd_checksum_banks(): ERROR requesting checksum.\r\n");
#endif
    	return -1;      
		}

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_checksum_banks(): checksum: %04X\r\n", checksum);
		fprintf (stderr, "snd_checksum_banks(): done\r\n");
	#endif

	return checksum;

} /* snd_checksum_banks() */

/***** END of snd_checksum_banks() *****************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_checksum_dram()                                           */
/*                                                                         */
/* Given a starting address in sound D/RAM and an ending address in D/RAM, */
/* requests the 16-bit checksum of the range from the sound system. The    */
/* addresses are inclusive.                                                */
/*                                                                         */
/* Returns the checksum of the range as a 16-bit int (i.e. sound system    */
/* returns a 16-bit value).                                                */
/*                                                                         */
/* NOTICE!!! This function SLEEPS while it waits for the checksum back.    */
/* It takes about 1.5 seconds to checksum the whole 2MB of sound D/RAM.    */
/*                                                                         */
/* NOTE: Once the checksum command is sent, the sound DSP spends 100% of   */
/* its realtime in a loop doing the checksum as fast as it can. Any sound  */
/* call that is sent during the checksum will cancel the checksum process  */
/* and immediately execute the sound call. If you interrupt the checksum   */
/* with a sound call, you will never get anything back (which is fine,     */
/* but just so you know). This is designed so you can do the checksum      */
/* during silent attract mode, but if the game starts, you can just blow   */
/* it off and start sending sound calls.                                   */
/*                                                                         */
/* Returns -1 if error.                                                    */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 2 July 97 MVB                                                           */
/*                                                                         */
/***************************************************************************/

int snd_checksum_dram (int start_addr, int end_addr)

{ /* snd_checksum_dram() */

unsigned int high_word;
unsigned int low_word;

unsigned int dram_checksum;

/***************************************************************************/

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_checksum_dram(): begin\r\n");
  		fprintf (stderr, "start addr: %d (0x%08X)\r\n", start_addr, start_addr);
  		fprintf (stderr, "end addr: %d (0x%08X)\r\n", end_addr, end_addr);
	#endif

	/* send the cmd to request the checksum */
	if (snd_send_data (SND_CMD_CHECKSUM_DRAM) != OK)
		{
#ifdef DEBUG
		fprintf (stderr, "snd_checksum_dram(): ERROR sending checksum D/RAM cmd.\r\n");
#endif
		return -1;           
   		}

	/* these addresses are WORD WISE 16-bit values, NOT byte wise */

  	/* send the starting D/RAM address */
  	high_word = (start_addr >> 16) & SND_MASK16;
  	low_word = start_addr & SND_MASK16;
  	if (snd_send_data (high_word) != OK)
   		{
#ifdef DEBUG
   		fprintf (stderr, "snd_checksum_dram(): ERROR sending high start addr.\r\n");
#endif
     	return -1;         
   		}
	if (snd_send_data (low_word) != OK)
		{
#ifdef DEBUG
		fprintf (stderr, "snd_checksum_dram(): ERROR sending low start addr.\r\n");
#endif
		return -1;         
		}

	/* send the ending D/RAM address */
	high_word = (end_addr >> 16) & SND_MASK16;
	low_word = end_addr & SND_MASK16;
	if (snd_send_data (high_word) != OK)
		{
#ifdef DEBUG
		fprintf (stderr, "snd_checksum_dram(): ERROR sending high end addr.\r\n");
#endif
		return -1;         
		}
	if (snd_send_data (low_word) != OK)
		{
#ifdef DEBUG
		fprintf (stderr, "snd_checksum_dram(): ERROR sending low end addr.\r\n");
#endif
		return -1;         
		}

	/* wait for the checksum to come back */
	/* normally we would use snd_get_data(), but that function times out */
	/* on the order of microseconds; we need to sleep and wait for the */
	/* checksum, which could take as long as 2 seconds */
	dram_checksum = snd_wait_for_completion();         

	#if SND_DEBUG
		fprintf (stderr, "\r\n");
		fprintf (stderr, "snd_checksum_dram(): checksum: %04X\r\n", dram_checksum);
		fprintf (stderr, "snd_checksum_dram(): end\r\n");
	#endif

	/* pass back the value */
	return dram_checksum;

} /* snd_checksum_dram() */

/***** END of snd_checksum_dram() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_download_bank()                                           */
/*                                                                         */
/* A DCS2 sound bank consists of two parts: the sound call table entries   */
/* and the actual bank data (playlists and compressed audio).              */
/*                                                                         */
/* This downloads the bank data. It is called by snd_load_bank().          */
/*                                                                         */
/* *bank_data is a ptr to the data loaded from the .BNK file. start and    */
/* end determine where in the sound D/RAM the bank gets put.               */
/*                                                                         */
/* Notice the byte swizzling which is done so that the .BNK files can be   */
/* loaded directly off the game disk exactly as they would be on the PC.   */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_download_bank (unsigned int *bank_data, int start, int end)

{ /* snd_download_bank()*/

int status;

unsigned int size;
unsigned int buffer[2];

unsigned int high_word;
unsigned int low_word;

unsigned int i;
unsigned int j;
unsigned int k;

volatile unsigned int checksum;
unsigned int bank_checksum;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_download_bank(): begin\r\n");
 #endif

 size = end - start + 1;                 

 #if SND_DEBUG
  fprintf (stderr, "snd_download_bank(): start addr: %d (0x%08X)\r\n", start, start);
  fprintf (stderr, "snd_download_bank(): end addr:   %d (0x%08X)\r\n", end, end);
  fprintf (stderr, "snd_download_bank(): size:       %d words\r\n", size);
 #endif

 if (snd_send_data (SND_CMD_LOAD_DRAM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank(): ERROR sending load D/RAM cmd.\r\n");
#endif
    return ERROR;           
   }

  /* send the starting load address */
  high_word = (start >> 16) & SND_MASK16;
  low_word = start & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank(): ERROR sending high start addr.\r\n");
#endif
     return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank(): ERROR sending low start addr.\r\n");
#endif
     return ERROR;         
   }


  /* send the ending load address */
  high_word = (end >> 16) & SND_MASK16;
  low_word = end & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank(): ERROR sending high end addr.\r\n");
#endif
     return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank(): ERROR sending low end addr.\r\n");
#endif
     return ERROR;         
   }

  /* now send the data */
  k = 0;
  i = 0;
  /* checksum is a straight 16-bit sum */  
  checksum = 0;

  buffer[0] = ((bank_data[i] & SND_BYTE3) >> 24) |
             ((bank_data[i] & SND_BYTE2) >> 8);

  buffer[1] = ((bank_data[i+1] & SND_BYTE0) << 8) |
             ((bank_data[i+1] & SND_BYTE1) >> 8);

  i++;

 for (j= 0; j < size; j++)
     {
     status = snd_send_data (buffer[k]);

    if (status != OK)
     {
#ifdef DEBUG
       fprintf (stderr, "snd_download_bank(): ERROR sending data.\r\n");
#endif
        return ERROR;
     }

      checksum += buffer[k];

      k++;
        
     if (k==2)
       {
       k = 0;

        buffer[0] = ((bank_data[i] & SND_BYTE3) >> 24) |
                   ((bank_data[i] & SND_BYTE2) >> 8);

        buffer[1] = ((bank_data[i+1] & SND_BYTE0) << 8) |
                   ((bank_data[i+1] & SND_BYTE1) >> 8);
       i++;
       }

     } /* for */

  status = snd_get_data (&bank_checksum);         

 #if SND_DEBUG
  fprintf (stderr, "snd_download_bank(): computed checksum: %04X\r\n", checksum);
  fprintf (stderr, "snd_download_bank(): received checksum: %04X\r\n", bank_checksum);
 #endif

   if ((checksum & SND_MASK16) != bank_checksum)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_download_bank(): ERROR reading checksum.\r\n");      
     fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
                 bank_checksum);     
#endif
      return ERROR;      
     }

 #if SND_DEBUG
  fprintf (stderr, "snd_download_bank(): done\r\n");
 #endif

   return OK;

} /* snd_download_bank() */

/***** END of snd_download_bank() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_download_bank_playing()                                   */
/*                                                                         */
/* Version of snd_download_bank() for loading while playing.               */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_download_bank_playing (unsigned int *bank_data, int start, int end)

{ /* snd_download_bank_playing()*/

int status;

unsigned int size;
unsigned int buffer[2];

unsigned int high_word;
unsigned int low_word;

unsigned int i;
unsigned int j;
unsigned int k;

volatile unsigned int checksum;
unsigned int bank_checksum;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_download_bank_playing(): begin\r\n");
 #endif

 size = end - start + 1;                 

 #if SND_DEBUG
  fprintf (stderr, "snd_download_bank_playing(): start addr: %d (0x%08X)\r\n", start, start);
  fprintf (stderr, "snd_download_bank_playing(): end addr:   %d (0x%08X)\r\n", end, end);
  fprintf (stderr, "snd_download_bank_playing(): size:       %d words\r\n", size);
 #endif

 if (snd_send_data (SND_CMD_LOAD_PLAY) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank_playing(): ERROR sending load D/RAM cmd.\r\n");
#endif
    return ERROR;           
   }

  /* send the starting load address */
  high_word = (start >> 16) & SND_MASK16;
  low_word = start & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank_playing(): ERROR sending high start addr.\r\n");
#endif
     return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank_playing(): ERROR sending low start addr.\r\n");
#endif
     return ERROR;         
   }


  /* send the ending load address */
  high_word = (end >> 16) & SND_MASK16;
  low_word = end & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank_playing(): ERROR sending high end addr.\r\n");
#endif
     return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_bank_playing(): ERROR sending low end addr.\r\n");
#endif
     return ERROR;         
   }

  /* now send the data */
  k = 0;
  i = 0;
  /* checksum is a straight 16-bit sum */  
  checksum = 0;

  buffer[0] = ((bank_data[i] & SND_BYTE3) >> 24) |
             ((bank_data[i] & SND_BYTE2) >> 8);

  buffer[1] = ((bank_data[i+1] & SND_BYTE0) << 8) |
             ((bank_data[i+1] & SND_BYTE1) >> 8);

  i++;

 for (j= 0; j < size; j++)
     {

	/* wait until the sound DSP is not busy */

    status = snd_send_data_realtime (buffer[k]);

    if (status != OK)
     {
#ifdef DEBUG
       fprintf (stderr, "snd_download_bank_playing(): ERROR sending data.\r\n");
#endif
        return ERROR;
     }

      checksum += buffer[k];

      k++;
        
     if (k==2)
       {
       k = 0;

        buffer[0] = ((bank_data[i] & SND_BYTE3) >> 24) |
                   ((bank_data[i] & SND_BYTE2) >> 8);

        buffer[1] = ((bank_data[i+1] & SND_BYTE0) << 8) |
                   ((bank_data[i+1] & SND_BYTE1) >> 8);
       i++;
       }

     } /* for */

  status = snd_get_data (&bank_checksum);         

 #if SND_DEBUG
  fprintf (stderr, "snd_download_bank_playing(): computed checksum: %04X\r\n", checksum);
  fprintf (stderr, "snd_download_bank_playing(): received checksum: %04X\r\n", bank_checksum);
 #endif

   if ((checksum & SND_MASK16) != bank_checksum)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_download_bank_playing(): ERROR reading checksum.\r\n");      
     fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
                 bank_checksum);     
#endif
      return ERROR;      
     }

 #if SND_DEBUG
  fprintf (stderr, "snd_download_bank_playing(): done\r\n");
 #endif

   return OK;

} /* snd_download_bank_playing() */

/***** END of snd_download_bank_playing() **********************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_download_table()                                          */
/*                                                                         */
/* A DCS2 sound bank consists of two parts: the sound call table data      */
/* and the actual bank data (playlists and compressed audio). This         */
/* downloads the sound call table data. It is called by snd_load_bank().   */
/*                                                                         */
/* This function only updates the sound call table between the start and   */
/* end addresses given.                                                    */
/*                                                                         */
/* NOTE!                                                                   */
/*                                                                         */
/* The sound call table is the first 16 kbytes of the sound D/RAM. Each    */
/* entry is a 32-bit address (4 bytes) so there is room for 4096 sound     */
/* calls.                                                                  */
/*                                                                         */
/* An entry in the sound call table is really just the address of the      */
/* playlist in D/RAM, which means that the host must reconcile/relocate    */
/* each sound call table entry with the address that it got loaded into.   */
/* See snd_load_bank() and snd_download_bank() for details, and see the    */
/* sound system docs for more info.                                        */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_download_table (unsigned int *table_data, int start, int end)

{ /* snd_download_table()*/

int status;

volatile int i;
volatile int j;
volatile int k;

unsigned int high_word;
unsigned int low_word;

unsigned int size;
unsigned int buffer[2];

unsigned int checksum;
unsigned int table_checksum;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_download_table(): begin\r\n");
 #endif

 size = end - start + 1;                 

 #if SND_DEBUG
  fprintf (stderr, "snd_download_table(): start addr: %d (0x%08X)\r\n", start, start);
  fprintf (stderr, "snd_download_table(): end addr:   %d (0x%08X)\r\n", end, end);
  fprintf (stderr, "snd_download_table(): size:       %d words\r\n", size);
 #endif

  if (snd_send_data (SND_CMD_LOAD_DRAM) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table(): ERROR sending load D/RAM cmd.\r\n");
#endif
    return ERROR;           
   }

  /* send the starting load address */
  high_word = (start >> 16) & SND_MASK16;
  low_word = start & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table(): ERROR sending high start addr.\r\n");
#endif
    return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table(): ERROR sending low start addr.\r\n");
#endif
    return ERROR;         
   }

  /* send the ending load address */
  high_word = (end >> 16) & SND_MASK16;
  low_word = end & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table(): ERROR sending high end addr.\r\n");
#endif
    return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table(): ERROR sending low end addr.\r\n");
#endif
    return ERROR;         
   }


  /* now send the data */
  k = 0;
  i = 0;
 /* checksum is a straight 16-bit sum */
  checksum = 0;

  buffer[0] = (table_data[i] >> 16) & SND_MASK16;
  buffer[1] = table_data[i] & SND_MASK16;
  i++;

 for (j= 0; j < size; j++)
    {
 
    status = snd_send_data (buffer[k]);

    if (status != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_download_table(): ERROR sending data.\r\n");
#endif
        return ERROR;         
       }
       
     checksum += buffer[k];

     k++;
       
    if (k==2)
     {
     k = 0;
      buffer[0] = (table_data[i] >> 16) & SND_MASK16;
      buffer[1] = table_data[i] & SND_MASK16;
     i++;
     }

    }  /* for */

  status = snd_get_data (&table_checksum);         

 #if SND_DEBUG
  fprintf (stderr, "snd_download_table(): checksum: %04X\r\n", table_checksum);
 #endif

  if ((checksum & SND_MASK16) != table_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table(): ERROR reading checksum.\r\n");     
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               table_checksum);      
#endif
     return ERROR;       
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_download_table(): done\r\n");
 #endif

  return OK;

} /* snd_download_table()*/

/***** END of snd_download_table() *****************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_download_table_playing()                                  */
/*                                                                         */
/* Version of snd_download_table() for loading while playing.              */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 05 Sep 97 MVB - download while playing version                          */
/*                                                                         */
/***************************************************************************/

int snd_download_table_playing (unsigned int *table_data, int start, int end)

{ /* snd_download_table_playing()*/

int status;

volatile int i;
volatile int j;
volatile int k;

unsigned int high_word;
unsigned int low_word;

unsigned int size;
unsigned int buffer[2];

unsigned int checksum;
unsigned int table_checksum;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_download_table_playing(): begin\r\n");
 #endif

 size = end - start + 1;                 

 #if SND_DEBUG
  fprintf (stderr, "snd_download_table_playing(): start addr: %d (0x%08X)\r\n", start, start);
  fprintf (stderr, "snd_download_table_playing(): end addr:   %d (0x%08X)\r\n", end, end);
  fprintf (stderr, "snd_download_table_playing(): size:       %d words\r\n", size);
 #endif

  if (snd_send_data (SND_CMD_LOAD_PLAY) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table_playing(): ERROR sending load D/RAM cmd.\r\n");
#endif
    return ERROR;           
   }

  /* send the starting load address */
  high_word = (start >> 16) & SND_MASK16;
  low_word = start & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table_playing(): ERROR sending high start addr.\r\n");
#endif
    return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table_playing(): ERROR sending low start addr.\r\n");
#endif
    return ERROR;         
   }

  /* send the ending load address */
  high_word = (end >> 16) & SND_MASK16;
  low_word = end & SND_MASK16;
  if (snd_send_data (high_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table_playing(): ERROR sending high end addr.\r\n");
#endif
    return ERROR;         
   }
  if (snd_send_data (low_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table_playing(): ERROR sending low end addr.\r\n");
#endif
    return ERROR;         
   }


  /* now send the data */
  k = 0;
  i = 0;
 /* checksum is a straight 16-bit sum */
  checksum = 0;

  buffer[0] = (table_data[i] >> 16) & SND_MASK16;
  buffer[1] = table_data[i] & SND_MASK16;
  i++;

 for (j= 0; j < size; j++)
    {
 
    status = snd_send_data_realtime (buffer[k]);

    if (status != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_download_table_playing(): ERROR sending data.\r\n");
#endif
        return ERROR;         
       }
       
     checksum += buffer[k];

     k++;
       
    if (k==2)
     {
     k = 0;
      buffer[0] = (table_data[i] >> 16) & SND_MASK16;
      buffer[1] = table_data[i] & SND_MASK16;
     i++;
     }

    }  /* for */

  status = snd_get_data (&table_checksum);         

 #if SND_DEBUG
  fprintf (stderr, "snd_download_table_playing(): checksum: %04X\r\n", table_checksum);
 #endif

  if ((checksum & SND_MASK16) != table_checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_download_table_playing(): ERROR reading checksum.\r\n");     
   fprintf (stderr, "expected %04X, received %04X\r\n", (checksum & SND_MASK16),
               table_checksum);      
#endif
     return ERROR;       
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_download_table_playing(): done\r\n");
 #endif

  return OK;

} /* snd_download_table_playing()*/

/***** END of snd_download_table_playing() *********************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_bank()                                               */
/*                                                                         */
/* 1. Reads a .BNK file from the game disk.                                */
/*                                                                         */
/* 2. Does the relocation pass for each sound call table entry.            */
/*                                                                         */
/* 3. Downloads the sound call table data using snd_download_table().      */
/*                                                                         */
/* 4. Downloads the playlist and compressed audio data using               */
/*    snd_download_bank().                                                 */
/*                                                                         */
/* For detailed info on the .BNK file format and the load and relocation   */
/* scheme, see the sound system docs.                                      */
/*                                                                         */
/* Variables snd_bank_addr and snd_table_addr are global.                  */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 21 Mar 97 MVB added queue flush, latch clear, and sound call holdoff    */
/*                                                                         */
/***************************************************************************/

int snd_load_bank (char *bank_name, int *num_sound_calls)

{ /* snd_load_bank() */

unsigned int *sound_call_table;
unsigned int *data_buffer;

unsigned int temp1;
unsigned int temp2;

volatile int num_calls;
volatile int bank_size;

int i;
int k;

int status;


/***************************************************************************/

  /* load the .BNK file from the disk */

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_bank(): begin\r\n");
 #endif

 data_buffer = snd_load_file (bank_name);

 if (data_buffer == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_bank(): ERROR loading bank file %s.\r\n", bank_name);
#endif
   return ERROR;
   *num_sound_calls = 0;
   }

 /* there is a 128 byte header that we can ignore */
 /* the header mostly contains build info used by the sound tools */

  /* number of sound calls in this bank is stored in first data loc */
 num_calls = data_buffer[32] & SND_MASK16;
 /* num_sound_calls gets passed back */
 *num_sound_calls = num_calls;

 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank(): %d sound calls\r\n", num_calls);
 #endif


 /* size of bank is next */
 bank_size = (data_buffer[32] & SND_TOP16) >> 16 
           | (data_buffer[33] & SND_MASK16) << 16;

 if (bank_size + snd_bank_addr >= SND_DRAM_SIZE) {
#if DEBUG
	fprintf(stderr,"snd_load_bank(): Bank will not fit in D/RAM\n");
#endif
	free (data_buffer);
	return ERROR;
    }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank(): bank size: %d bytes\r\n", bank_size);
 #endif


 /* this is NOT the entire sound call table as it exists */
 /* on the sound DSP side, but rather only as much as we */
 /* need to hold the calls in this bank */

 sound_call_table = malloc (num_calls * 4);  

 if (sound_call_table == NULL)
    {
#ifdef DEBUG
   fprintf (stderr, "snd_load_bank(): cannot malloc %d bytes for sound call table.\r\n", num_calls * 4);
#endif
    free (data_buffer);
    return ERROR;
   }

  /* the table could be sparse, so set all entries to NULL */
 /* the sound system uses 0xFFFFFFFF as the flag for */
 /* "no sound call loaded for this entry" */

 for (i=0; i < num_calls; i++)
    sound_call_table[i] = SND_MASK32;


 k = 33;

  /* create the local, relocated sound call table */
 
 for (i=0; i < num_calls; i ++)
    {

    temp1 = (data_buffer[k] & SND_TOP16) >> 16
          | (data_buffer [k+1] & SND_MASK16) << 16;
    k++;

     /* relative to last ending point */

    if (temp1 != SND_MASK32)
       temp1 = temp1 + snd_bank_addr;

    sound_call_table[i] = temp1; 

    }

 /* starting location in D/RAM of sound calls to store */
 temp1 = snd_table_addr;
 /* location of last sound call to store */
 temp2 = snd_table_addr + (2 * num_calls) - 1;

 #if SND_DEBUG
   fprintf (stderr, "snd_load_bank(): sending sound call table...\r\n");
 #endif

 /* we are committed to a bank load at this point */
 /* flush any return info that may be */
 /* lingering from previous sound calls */

 snd_bank_lockout(TRUE);   /* lockout sound calls */
 snd_flush_pending ();      /* clear any pending sound calls */
 snd_flush_queues();    	/* clear any pending return info */
 snd_clear_latch();         /* clear out the I/O ASIC latch */

 /* download the sound call entries for this bank */
 status = snd_download_table (sound_call_table, temp1, temp2);
 free (sound_call_table);
 if (status != OK)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_load_bank(): ERROR downloading sound call table.\r\n");
#endif
    free (data_buffer);
	snd_bank_lockout(FALSE);
    return ERROR;
    }

 /* save the last loaded table entry for subsequent loads to use */
 /* This is a global variable! */
 snd_table_addr = temp2 + 1;

 /* starting address of D/RAM to load */
 /* temp1 and temp2 are in 16-bit word addresses */
 /* temp2 is inclusive */
 temp1 = snd_bank_addr / 2;
 temp2 = temp1 + (bank_size) / 2 - 1;


 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank(): sending bank data...\r\n");
 #endif

 /* k still holds the index into the buffer loaded from disk */
 /* download the bank data */
 status = snd_download_bank (data_buffer + k, temp1, temp2); 
 free (data_buffer);
 if (status != OK)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_load_bank(): ERROR downloading bank data.\r\n");
#endif
	snd_bank_lockout(FALSE);
    return ERROR;
    }

 /* OK for sound calls now */
 snd_bank_lockout (FALSE); 

 /* update the running D/RAM address */
 snd_bank_addr = (temp2 + 1) * 2;

 #if SND_DEBUG
   fprintf (stderr, "snd_load_bank(): done\r\n");
 #endif

 return OK;

} /* snd_load_bank() */



int snd_load_bank_from_memory (unsigned int *data_buffer, int *num_sound_calls)

{ /* snd_load_bank_from_memory() */

unsigned int *sound_call_table;

unsigned int temp1;
unsigned int temp2;

volatile int num_calls;
volatile int bank_size;

int i;
int k;

int status;


/***************************************************************************/

  /* load the .BNK file from the disk */

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_bank_from_memory(): begin\r\n");
 #endif

 if (data_buffer == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_bank_from_memory(): ERROR loading bank from memory.\r\n");
#endif
   return ERROR;
   *num_sound_calls = 0;
   }

 /* there is a 128 byte header that we can ignore */
 /* the header mostly contains build info used by the sound tools */

  /* number of sound calls in this bank is stored in first data loc */
 num_calls = data_buffer[32] & SND_MASK16;
 /* num_sound_calls gets passed back */
 *num_sound_calls = num_calls;

 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank_from_memory(): %d sound calls\r\n", num_calls);
 #endif


 /* size of bank is next */
 bank_size = (data_buffer[32] & SND_TOP16) >> 16 
           | (data_buffer[33] & SND_MASK16) << 16;

 if (bank_size + snd_bank_addr >= SND_DRAM_SIZE) {
#if DEBUG
	fprintf(stderr,"snd_load_bank_from_memory(): Bank will not fit in D/RAM\n");
#endif
	return ERROR;
    }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank_from_memory(): bank size: %d bytes\r\n", bank_size);
 #endif


 /* this is NOT the entire sound call table as it exists */
 /* on the sound DSP side, but rather only as much as we */
 /* need to hold the calls in this bank */

 sound_call_table = malloc (num_calls * 4);  

 if (sound_call_table == NULL)
    {
#ifdef DEBUG
   fprintf (stderr, "snd_load_bank_from_memory(): cannot malloc %d bytes for sound call table.\r\n", num_calls * 4);
#endif
    free (data_buffer);
    return ERROR;
   }

  /* the table could be sparse, so set all entries to NULL */
 /* the sound system uses 0xFFFFFFFF as the flag for */
 /* "no sound call loaded for this entry" */

 for (i=0; i < num_calls; i++)
    sound_call_table[i] = SND_MASK32;


 k = 33;

  /* create the local, relocated sound call table */
 
 for (i=0; i < num_calls; i ++)
    {

    temp1 = (data_buffer[k] & SND_TOP16) >> 16
          | (data_buffer [k+1] & SND_MASK16) << 16;
    k++;

     /* relative to last ending point */

    if (temp1 != SND_MASK32)
       temp1 = temp1 + snd_bank_addr;

    sound_call_table[i] = temp1; 

    }

 /* starting location in D/RAM of sound calls to store */
 temp1 = snd_table_addr;
 /* location of last sound call to store */
 temp2 = snd_table_addr + (2 * num_calls) - 1;

 #if SND_DEBUG
   fprintf (stderr, "snd_load_bank_from_memory(): sending sound call table...\r\n");
 #endif

 /* we are committed to a bank load at this point */
 /* flush any return info that may be */
 /* lingering from previous sound calls */

 snd_bank_lockout (TRUE);   /* lockout sound calls */
 snd_flush_pending ();      /* clear any pending sound calls */
 snd_flush_queues();    	/* clear any pending return info */
 snd_clear_latch();         /* clear out the I/O ASIC latch */

 /* download the sound call entries for this bank */
 status = snd_download_table (sound_call_table, temp1, temp2);
 free (sound_call_table);
 if (status != OK)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_load_bank_from_memory(): ERROR downloading sound call table.\r\n");
#endif
    free (data_buffer);
	snd_bank_lockout(FALSE);
    return ERROR;
    }

 /* save the last loaded table entry for subsequent loads to use */
 /* This is a global variable! */
 snd_table_addr = temp2 + 1;

 /* starting address of D/RAM to load */
 /* temp1 and temp2 are in 16-bit word addresses */
 /* temp2 is inclusive */
 temp1 = snd_bank_addr / 2;
 temp2 = temp1 + (bank_size) / 2 - 1;


 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank_from_memory(): sending bank data...\r\n");
 #endif

 /* k still holds the index into the buffer loaded from disk */
 /* download the bank data */
 status = snd_download_bank (data_buffer + k, temp1, temp2); 
 free (data_buffer);
 if (status != OK)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_load_bank_from_memory(): ERROR downloading bank data.\r\n");
#endif
	snd_bank_lockout(FALSE);
    return ERROR;
    }

 /* OK for sound calls now */
 snd_bank_lockout (FALSE); 

 /* update the running D/RAM address */
 snd_bank_addr = (temp2 + 1) * 2;

 #if SND_DEBUG
   fprintf (stderr, "snd_load_bank_from_memory(): done\r\n");
 #endif

 return OK;

} /* snd_load_bank_from_memory() */

/***** END of snd_load_bank() **********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_bank_playing()                                       */
/*                                                                         */
/* Special/copied version of snd_load_bank() with inline changes to        */
/* support loading while playing.                                          */
/*                                                                         */
/* See snd_load_bank() for more info.                                      */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 21 Mar 97 MVB added queue flush, latch clear, and sound call holdoff    */
/* 05 Sep 97 MVB this version for loading while playing                    */
/*                                                                         */
/***************************************************************************/

int snd_load_bank_playing (char *bank_name, int *num_sound_calls)

{ /* snd_load_bank_playing() */

unsigned int *sound_call_table;
unsigned int *data_buffer;

unsigned int temp1;
unsigned int temp2;

volatile int num_calls;
volatile int bank_size;

int i;
int k;

int status;


/***************************************************************************/

  /* load the .BNK file from the disk */

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_bank_playing(): begin\r\n");
 #endif

 data_buffer = snd_load_file (bank_name);

 if (data_buffer == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_bank_playing(): ERROR loading bank file %s.\r\n", bank_name);
#endif
   *num_sound_calls = 0;
   return ERROR;
   }

 /* there is a 128 byte header that we can ignore */
 /* the header mostly contains build info used by the sound tools */

  /* number of sound calls in this bank is stored in first data loc */
 num_calls = data_buffer[32] & SND_MASK16;
 /* num_sound_calls gets passed back */
 *num_sound_calls = num_calls;

 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank_playing(): %d sound calls\r\n", num_calls);
 #endif


 /* size of bank is next */
 bank_size = (data_buffer[32] & SND_TOP16) >> 16 
           | (data_buffer[33] & SND_MASK16) << 16;

 if (bank_size + snd_bank_addr >= SND_DRAM_SIZE) {
#if DEBUG
	fprintf(stderr,"snd_load_bank_playing(): Bank will not fit in D/RAM\n");
#endif
	return ERROR;
    }

 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank_playing(): bank size: %d bytes\r\n", bank_size);
 #endif


 /* this is NOT the entire sound call table as it exists */
 /* on the sound DSP side, but rather only as much as we */
 /* need to hold the calls in this bank */

 sound_call_table = malloc (num_calls * 4);  

 if (sound_call_table == NULL)
    {
#ifdef DEBUG
   fprintf (stderr, "snd_load_bank_playing(): cannot malloc %d bytes for sound call table.\r\n", num_calls * 4);
#endif
    free (data_buffer);
    return ERROR;
   }

  /* the table could be sparse, so set all entries to NULL */
 /* the sound system uses 0xFFFFFFFF as the flag for */
 /* "no sound call loaded for this entry" */

 for (i=0; i < num_calls; i++)
    sound_call_table[i] = SND_MASK32;


 k = 33;

  /* create the local, relocated sound call table */
 
 for (i=0; i < num_calls; i ++)
    {

    temp1 = (data_buffer[k] & SND_TOP16) >> 16
          | (data_buffer [k+1] & SND_MASK16) << 16;
    k++;

     /* relative to last ending point */

    if (temp1 != SND_MASK32)
       temp1 = temp1 + snd_bank_addr;

    sound_call_table[i] = temp1; 

    }

 /* starting location in D/RAM of sound calls to store */
 temp1 = snd_table_addr;
 /* location of last sound call to store */
 temp2 = snd_table_addr + (2 * num_calls) - 1;

 #if SND_DEBUG
   fprintf (stderr, "snd_load_bank_playing(): sending sound call table...\r\n");
 #endif

 /* we are committed to a bank load at this point */
 /* flush any return info that may be */
 /* lingering from previous sound calls */

 snd_bank_lockout (TRUE);   /* lockout sound calls */
 snd_flush_pending();       /* clear any pending sound calls */
 snd_flush_queues();    	/* clear any pending return info */
 snd_clear_latch();         /* clear out the I/O ASIC latch */

 /* download the sound call entries for this bank */
 status = snd_download_table_playing (sound_call_table, temp1, temp2);
 free (sound_call_table);
 if (status != OK)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_load_bank_playing(): ERROR downloading sound call table.\r\n");
#endif
    free (data_buffer);
	snd_bank_lockout(FALSE);
    return ERROR;
    }

 /* save the last loaded table entry for subsequent loads to use */
 /* This is a global variable! */
 snd_table_addr = temp2 + 1;

 /* starting address of D/RAM to load */
 /* temp1 and temp2 are in 16-bit word addresses */
 /* temp2 is inclusive */
 temp1 = snd_bank_addr / 2;
 temp2 = temp1 + (bank_size) / 2 - 1;


 #if SND_DEBUG
  fprintf (stderr, "snd_load_bank_playing(): sending bank data...\r\n");
 #endif

 /* k still holds the index into the buffer loaded from disk */
 /* download the bank data */
 status = snd_download_bank_playing (data_buffer + k, temp1, temp2); 
 free (data_buffer);
 if (status != OK)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_load_bank_playing(): ERROR downloading bank data.\r\n");
#endif
	snd_bank_lockout(FALSE);
    return ERROR;
    }

 /* OK for sound calls now */
 snd_bank_lockout (FALSE); 

 /* update the running D/RAM address */
 snd_bank_addr = (temp2 + 1) * 2;

 #if SND_DEBUG
   fprintf (stderr, "snd_load_bank_playing(): done\r\n");
 #endif

 return OK;

} /* snd_load_bank_playing() */

/***** END of snd_load_bank_playing() **************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_scall_quick()                                             */
/*                                                                         */
/* Simple function to quickly send a "normal" sound call.                  */
/*                                                                         */
/* This function does NOT add any offsets for which bank the call is in.   */
/* It is equivalent to snd_scall_direct() but it sets the track vol and    */
/* track pan to their defaults.                                            */
/*                                                                         */
/* Takes a sound call number and sets the track pan and track volume to    */
/* default values (pan==127==center, volume==255==max).                    */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 28 Aug 97 MVB - added sound lockout support                             */
/*                                                                         */
/***************************************************************************/

int snd_scall_quick (unsigned int scall)

{ /* snd_scall_quick() */

unsigned int track_pan;
unsigned int track_volume;
unsigned int temp;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_scall_quick(): begin\r\n");
 #endif

 /* lockout sound calls if disabled */
 if (snd_lock) 
   {
   #if SND_DEBUG
      fprintf (stderr, "snd_scall_quick(): sound calls locked out, no call made\r\n");
   #endif
   return OK;
   }

 /* set to default values */

 track_pan = 127;
 track_volume = 255;

 /* send the sound call number */
 if (snd_send_data (scall) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_quick(): ERROR sending sound call.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_scall_quick(): sent %d (0x%04X)\r\n", scall, scall);
 #endif

 /* send the track volume and track pan */
 temp = ((track_volume & SND_MASK8) << 8) | (track_pan & SND_MASK8);
 if (snd_send_data (0xFF00) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_quick(): ERROR sending track volume and pan.\r\n");
#endif
   return ERROR;
   }

 /* send the track mask and priority mask */
 /* the default value is (conveniently) zero */
 if (snd_send_data (0) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_quick(): ERROR sending track priority.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_scall_quick(): done\r\n");
 #endif

 return OK;

} /* snd_scall_quick() */

/***** END of snd_scall_quick() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_master_volume()                                           */
/*                                                                         */
/* Sends the reserved system sound call to update the master volume.       */
/*                                                                         */
/* 0 is mute                                                               */
/* 1 is softest                                                            */
/* 255 is loudest                                                          */
/*                                                                         */
/* This is a 2-word command. First the cmd is sent followed by the volume  */
/* OR'ed with the complement of the volume.                                */
/*                                                                         */
/* Returns OK or an error if it times out.                                 */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_master_volume (int volume)

{ /* snd_master_volume() */

unsigned int vol_word;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_master_volume(): begin\r\n");
 #endif

 if (snd_send_data (SND_CMD_MASTER_VOLUME) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_master_volume(): ERROR sending master volume cmd.\r\n");   
#endif
    return ERROR;
   }

 /* check bounds */
 if (volume > 255)
   volume = 255;
 if (volume < 0)
   volume = 0;

 vol_word = (volume & SND_MASK8) << 8 | (~volume & SND_MASK8);

 #if SND_DEBUG
  fprintf (stderr, "snd_master_volume(): master volume: %d\r\n", volume);
 #endif

 if (snd_send_data (vol_word) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_master_volume(): ERROR sending volume data.\r\n");   
#endif
    return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_master_volume(): done\r\n");
 #endif

  return OK;


} /* snd_master_volume() */

/***** END of snd_master_volume() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stop_all()                                                */
/*                                                                         */
/* Sends sound call 0, which stops all tracks and sets most of the sound   */
/* system back to its default value.                                       */
/*                                                                         */
/* Returns OK or an error if it times out.                                 */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stop_all (void)

{ /* snd_stop_all() */

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_master_volume(): begin\r\n");
 #endif

 if (snd_scall_quick (0) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_master_volume(): ERROR sending sound call 0.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_master_volume(): done\r\n");
 #endif

 return OK;

} /* snd_stop_all() */

/***** END of snd_stop_all() ***********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_scall_direct()                                            */
/*                                                                         */
/* Sends a sound call with the given number, volume, pan and priority.     */
/*                                                                         */
/* This function does NOT add any offsets for which bank the call is in.   */
/* It just sends the sound call directly to the sound system.              */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/* 28 Aug 97 MVB - added sound lockout support                             */
/* 12 Sep 97 MVB - added more error checking                               */
/*                                                                         */
/***************************************************************************/

int snd_scall_direct (unsigned int scall, unsigned int volume, 
                     unsigned int pan, unsigned int priority)

{ /* snd_scall_direct() */

unsigned int temp;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_scall_direct(): begin\r\n");
 #endif

 /* lockout sound calls if disabled */
 if (snd_lock) 
   {
   #if SND_DEBUG
      fprintf (stderr, "snd_scall_direct(): sound calls locked out, no call made\r\n");
   #endif
   return OK;
   }

 /* bounds check */

 if (volume > 255) 
   volume = 255;

 if (pan > 255) 
   pan = 255;

 if (scall > SND_TABLE_SIZE)
    {
#ifdef DEBUG
    fprintf (stderr, "snd_scall_direct(): ERROR call %d (0x%04X) out of range.\r\n",
                scall, scall);
#endif
    return ERROR;
    }  

 #if SND_DEBUG
  fprintf (stderr, "snd_scall_direct(): scall %d (0x%04X)\r\n", scall, scall);
  fprintf (stderr, "snd_scall_direct(): volume %d (0x%04X)\r\n", volume, volume);
  fprintf (stderr, "snd_scall_direct(): pan %d (0x%04X)\r\n", pan, pan);
  fprintf (stderr, "snd_scall_direct(): priority %d (0x%04X)\r\n", 
               priority, priority);
 #endif
    

 /* send the sound call number */
 if (snd_send_data (scall) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_direct(): ERROR sending sound call.\r\n");
#endif
   return ERROR;
   }

 /* send the track volume and track pan */
 temp = (volume & SND_MASK8) << 8 | (pan & SND_MASK8);
 if (snd_send_data (temp) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_direct(): ERROR sending track volume and pan.\r\n");
#endif
   return ERROR;
   }

 /* send the track mask and priority mask */
 if (snd_send_data (priority) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_direct(): ERROR sending track priority.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_scall_direct(): done\r\n");
 #endif

 return OK;

} /* snd_scall_direct() */

/***** END of snd_scall_direct() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_scall_return()                                            */
/*                                                                         */
/* Sends a sound call and waits for the return info.                       */
/*                                                                         */
/* The return information indicates which track(s) were used and which     */
/* track(s) were interrupted.                                              */
/*                                                                         */
/* The return value is TRACKS_USED|TRACKS_INTERRUPTED, where TRACKS_USED   */
/* is an 8-bit mask with bit 0 = track 0, bit 1 = track 1, etc. and the    */
/* same for TRACKS_INTERRUPTED. See the track defines in SOUND.H.          */
/*                                                                         */
/* The typical use for this function is to determine which track a sound   */
/* call got dynamically allocated to. That way you can come back later     */
/* and explicitly stop it or modify it (e.g. to kill a looping sound).     */
/*                                                                         */
/* This function explicitly waits until the return information shows up in */
/* the sound-to-host port. The track used/interrupted mask is in the data  */
/* port, and the 3-bit status will be set to indicate that this is sound   */
/* call return info (there are also other kinds of return info; see the    */
/* docs for more details).                                                 */
/*                                                                         */
/* The more correct way to do this would be to have a queue of information */
/* coming back from the sound system (interrupt driven would be best).     */
/* This function is mostly meant to be an example of how this could work.  */
/*                                                                         */
/* This uses the bank/call method; you could easily write another function */
/* that is equivalent to snd_scall_direct().                               */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* 28 Aug 97 MVB - added sound lockout support                             */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_scall_return (char *bank_name, unsigned int scall, 
                     unsigned int volume, unsigned int pan, 
                     unsigned int priority, unsigned int *return_info)

{ /* snd_scall_return() */

volatile unsigned int return_type;
volatile unsigned int return_data;
#if defined(SEATTLE)
int   status;
#endif
int   time;

#if SND_DEBUG
int i;
#endif

#if defined(VEGAS)
int	start_time = get_timer_val();
#endif
/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_scall_return(): begin\r\n");
 #endif

 /* check for sound call lockout */
 if ((snd_lock) || (snd_lock_bank))
   {
   #if SND_DEBUG
      fprintf (stderr, "snd_scall_return(): sound calls locked out, no call made\r\n");
   #endif
   return OK;
   }

 /* make the sound call */
 snd_scall_bank (bank_name, scall, volume, pan, priority);


 /* the latency here _should_ be less than ~7 milliseconds */
 /* which is one frame for the sound system */
 /* but that's not a guarantee since you can obviously think */
 /* up situations where it could span 3 frames, or ~21 milliseconds */
 
 /* the better way to do this would be to have the host get */
 /* interrupted whenever the sound system returns something, */
 /* and have it get queued up */

 /* wait for the return info */
// load_timer (1);
#if defined(SEATTLE)
	_ioctl(5, FIOCSTARTTIMER1, 0);
	_ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);
	while (!(status & STH_DATA_READY))
#elif defined(VEGAS)
	while(!(get_snd_status() & STH_DATA_READY))
#endif
	{
#if defined(SEATTLE)
		_ioctl(5, FIOCGETTIMER1, (int)&time);
#elif defined(VEGAS)
		time = start_time - get_timer_val();
#endif
		if(time > SND_TIMEOUT)
		{
#ifdef DEBUG
			fprintf (stderr, "snd_scall_return(): ERROR timeout waiting for return value.\r\n");
#endif
			*return_info = 0;
			return ERROR;
		}
#if defined(SEATTLE)
		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&status);
#endif
	}
#if defined(SEATTLE)
	_ioctl(5, FIOCSTOPTIMER1, 0);
#endif


	/* set up status, data and return */
	*return_info = 0;
#if defined(SEATTLE)
	_ioctl(4, FIOCGETSOUNDSTATUS, (int)&return_type);
	_ioctl(4, FIOCGETSOUNDDATA, (int)&return_data);
#elif defined(VEGAS)
	return_type = get_snd_status();
	return_data = get_snd_data();
#endif
	return_data &= SND_MASK16;
	/* clear the latch */
#if defined(SEATTLE)
	_ioctl(4, FIOCSETSTHDATA, 0);
#elif defined(VEGAS)
	set_snd_data(0);
#endif

 /* simple check to see if the sound call exists */
 if ((return_type & SND_ERROR_MASK) == SND_INFO_ERROR)
   {
   if (return_data == SND_INFO_NOCALL)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_scall_return(): ERROR sound call %d (0x%04X) in bank '%s' not found.\r\n",
                 scall, scall, bank_name);
#endif
     *return_info = return_data;
     return ERROR;
     }
   }

 
 /* for now, all we are looking for is tracks used and interrupted info */
 if ((return_type & SND_INFO_MASK) == SND_INFO_TRACKS)
   {

   #if SND_DEBUG

     fprintf (stderr, "snd_scall_return(): track info: %04X\r\n", return_data);

     fprintf (stderr, "tracks used:        ");
     for (i=0; i < 6; i++)
       {
       if ((return_data >> (8 + i)) & 0x0001)
         fprintf (stderr, "%d ", i);
       else
         fprintf (stderr, "- ");
         }
     fprintf (stderr, "\r\n");

     fprintf (stderr, "tracks interrupted: ");
     for (i=0; i < 6; i++)
       {
       if ((return_data >> i) & 0x0001)
         fprintf (stderr, "%d ", i);
       else
         fprintf (stderr, "- ");
         }
     fprintf (stderr, "\r\n");

   #endif

   *return_info = return_data;
   }
 else
   {
   /* did not get back track info */
   /* must be something else */
   #if SND_DEBUG
     fprintf (stderr, "snd_scall_return(): unexpected return flag %04X, data %04X.\r\n",
                  return_type, return_data);
   #endif
   *return_info = 0;
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_scall_return(): done\r\n");
 #endif

 return OK;

} /* snd_scall_return() */

/***** END of snd_scall_return() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_output_stereo()                                           */
/*                                                                         */
/* Sets the output mode to stereo.                                         */
/*                                                                         */
/* See also snd_output_mono().                                             */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_output_stereo (void)

{ /* snd_output_stereo() */

/***************************************************************************/

 if (snd_send_data (SND_CMD_OUTPUT_STEREO) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_output_stereo(): ERROR sending 1st word.\r\n");
#endif
    return ERROR;
   }

 if (snd_send_data (SND_CMD_STEREO_SAFETY) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_output_stereo(): ERROR sending 2nd word.\r\n");
#endif
    return ERROR;
   }

 return OK;

} /* snd_output_stereo() */

/***** END of snd_output_stereo() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_output_mono()                                             */
/*                                                                         */
/* Sets the output mode to mono.                                           */
/*                                                                         */
/* See also snd_output_stereo().                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_output_mono (void)

{ /* snd_output_mono() */

/***************************************************************************/

 if (snd_send_data (SND_CMD_OUTPUT_MONO) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_output_mono(): ERROR sending 1st word.\r\n");
#endif
    return ERROR;
   }

 if (snd_send_data (SND_CMD_MONO_SAFETY) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_output_mono(): ERROR sending 2nd word.\r\n");
#endif
    return ERROR;
   }

 return OK;

} /* snd_output_mono() */

/***** END of snd_output_mono() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_flush_pending()                                           */
/*                                                                         */
/* Flushes and removes any pending sound calls that the sound system has   */
/* received but not yet played.                                            */
/*                                                                         */
/* This does not stop any currently playing sound calls, but just clears   */
/* out the incoming queue.                                                 */
/*                                                                         */
/* This is intended mainly so that you can start a "download while         */
/* playing" sequence with the guarantee that no more sound calls will      */
/* start up.                                                               */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_flush_pending (void)

{ /* snd_flush_pending() */

 if (snd_send_data (SND_CMD_FLUSH_CALLS) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_flush_pending(): ERROR sending cmd.\r\n");
#endif
    return ERROR;
   }

 return OK;

} /* snd_flush_pending() */

/***** END of snd_flush_pending() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_flush_queues()                                            */
/*                                                                         */
/* Clears out the incoming host-to-sound queue on the sound DSP and also   */
/* the outgoing sound-to-host queue on the sound DSP.                      */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_flush_queues (void)

{ /* snd_flush_queues() */

 if (snd_send_data (SND_CMD_FLUSH_QUEUE) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_flush_queues(): ERROR sending cmd.\r\n");
#endif
    return ERROR;
   }

 return OK;

} /* snd_flush_queues() */

/***** END of snd_flush_queues() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_set_reserved()                                            */
/*                                                                         */
/* Sets the reserved track mask. Reserved tracks are not used by the       */
/* dynamic track allocator. The typical use of this is to reserve tracks   */
/* for music (typically 0 and 1) and let the allocator dynamically assign  */
/* the rest.                                                               */
/*                                                                         */
/* The track mask is one or more track bitfields as defined in SOUND.H.    */
/* Bit 0 set means reserve track 0, bit 1 set means reserve track 1, etc.  */
/*                                                                         */
/* Note: Sending a sound call zero "stop all" or doing an immediate bank   */
/* download will reset the track reserved mask.                            */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_set_reserved (unsigned int track_mask)

{ /* snd_set_reserved() */

 if (snd_send_data (SND_CMD_SET_RESERVED) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_set_reserved(): ERROR sending cmd.\r\n");
#endif
   return ERROR;
   }

 if (snd_send_data ((track_mask << 8) & 0xFF00) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_set_reserved(): ERROR sending track mask.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_set_reserved(): track mask: %02X\r\n", track_mask);
  if (track_mask & SND_TRACK_0)
      fprintf (stderr, "snd_set_reserved(): track 0 reserved\r\n");
  if (track_mask & SND_TRACK_1)
      fprintf (stderr, "snd_set_reserved(): track 1 reserved\r\n");
  if (track_mask & SND_TRACK_2)
      fprintf (stderr, "snd_set_reserved(): track 2 reserved\r\n");
  if (track_mask & SND_TRACK_3)
      fprintf (stderr, "snd_set_reserved(): track 3 reserved\r\n");
  if (track_mask & SND_TRACK_4)
      fprintf (stderr, "snd_set_reserved(): track 4 reserved\r\n");
  if (track_mask & SND_TRACK_5)
      fprintf (stderr, "snd_set_reserved(): track 5 reserved\r\n");
 #endif

 return OK;

} /* snd_set_reserved() */

/***** END of snd_set_reserved() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stop_track()                                              */
/*                                                                         */
/* Stops one or more tracks.                                               */
/*                                                                         */
/* To kill all tracks, use snd_stop_all();                                 */
/*                                                                         */
/* The track mask is one or more track bitfields as defined in SOUND.H.    */
/* Bit 0 set means reserve track 0, bit 1 set means reserve track 1, etc.  */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stop_track (unsigned int track_mask)

{ /* snd_stop_track() */

 if (snd_send_data (SND_CMD_TRACK_STOP) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stop_track(): ERROR sending cmd.\r\n");
#endif
    return ERROR;
   }

 if (snd_send_data ((track_mask << 8) & 0xFF00) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stop_track(): ERROR sending track mask.\r\n");
#endif
    return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_stop_track(): track mask: %02X\r\n", track_mask);
  if (track_mask & SND_TRACK_0)
      fprintf (stderr, "snd_stop_track(): track 0 stopped\r\n");
  if (track_mask & SND_TRACK_1)
      fprintf (stderr, "snd_stop_track(): track 1 stopped\r\n");
  if (track_mask & SND_TRACK_2)
      fprintf (stderr, "snd_stop_track(): track 2 stopped\r\n");
  if (track_mask & SND_TRACK_3)
      fprintf (stderr, "snd_stop_track(): track 3 stopped\r\n");
  if (track_mask & SND_TRACK_4)
      fprintf (stderr, "snd_stop_track(): track 4 stopped\r\n");
  if (track_mask & SND_TRACK_5)
      fprintf (stderr, "snd_stop_track(): track 5 stopped\r\n");
 #endif

 return OK;

} /* snd_stop_track() */

/***** END of snd_stop_track() *********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_track_volume()                                            */
/*                                                                         */
/* Sets the track volume for one or more tracks. The track volume is a     */
/* host-controlled volume designed to let the host fade things in and out  */
/* as needed during game play.                                             */
/*                                                                         */
/* 0 is the softest and 255 is the loudest.                                */
/*                                                                         */
/* The default level when a track starts playing is 255. If you are not    */
/* trying to do something specific, set it at 255 when you make the sound  */
/* call and then leave it.                                                 */
/*                                                                         */
/* Do not use this to adjust the master volume. Instead use                */
/* snd_master_volume().                                                    */
/*                                                                         */
/* The track mask is one or more track bitfields as defined in SOUND.H.    */
/* Bit 0 set means reserve track 0, bit 1 set means reserve track 1, etc.  */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_track_volume (unsigned int track_mask, unsigned int track_volume)

{ /* snd_track_volume() */

 if (snd_send_data (SND_CMD_TRACK_VOLUME) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_track_volume(): ERROR sending cmd.\r\n");
#endif
    return ERROR;
   }

 if (snd_send_data ((track_mask << 8) | track_volume) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_track_volume(): ERROR sending track mask and volume.\r\n");
#endif
    return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_track_volume(): track mask:%02X volume:%d (0x%02X)\r\n", 
              track_mask, track_volume, track_volume);
  if (track_mask & SND_TRACK_0)
      fprintf (stderr, "snd_track_volume(): track 0 set to %d\r\n", track_volume);
  if (track_mask & SND_TRACK_1)
      fprintf (stderr, "snd_track_volume(): track 1 set to %d\r\n", track_volume);
  if (track_mask & SND_TRACK_2)
      fprintf (stderr, "snd_track_volume(): track 2 set to %d\r\n", track_volume);
  if (track_mask & SND_TRACK_3)
      fprintf (stderr, "snd_track_volume(): track 3 set to %d\r\n", track_volume);
  if (track_mask & SND_TRACK_4)
      fprintf (stderr, "snd_track_volume(): track 4 set to %d\r\n", track_volume);
  if (track_mask & SND_TRACK_5)
      fprintf (stderr, "snd_track_volume(): track 5 set to %d\r\n", track_volume);
 #endif

 return OK;

} /* snd_track_volume() */

/***** snd_track_volume() **************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_track_pan()                                               */
/*                                                                         */
/* Sets the track pan for one or more tracks. This is designed for moving  */
/* a sound around after it has been called/started.                        */
/*                                                                         */
/* 0 is full left, 127 is center, 255 is full right.                       */
/*                                                                         */
/* The default pan when a track starts playing is 127. If you are not      */
/* trying to do something specific, set the pan when you first make the    */
/* sound call and leave it.                                                */
/*                                                                         */
/* The track mask is one or more track bitfields as defined in SOUND.H.    */
/* Bit 0 set means reserve track 0, bit 1 set means reserve track 1, etc.  */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_track_pan (unsigned int track_mask, unsigned int track_pan)

{ /* snd_track_pan() */

 if (snd_send_data (SND_CMD_TRACK_PAN) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_track_pan(): ERROR sending cmd.\r\n");
#endif
    return ERROR;
   }

 if (snd_send_data ((track_mask << 8) | track_pan) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_track_pan(): ERROR sending track mask and pan.\r\n");
#endif
    return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_track_pan(): track mask:%02X pan:%d (0x%02X)\r\n", 
              track_mask, track_pan, track_pan);
  if (track_mask & SND_TRACK_0)
      fprintf (stderr, "snd_track_pan(): track 0 set to %d\r\n", track_pan);
  if (track_mask & SND_TRACK_1)
      fprintf (stderr, "snd_track_pan(): track 1 set to %d\r\n", track_pan);
  if (track_mask & SND_TRACK_2)
      fprintf (stderr, "snd_track_pan(): track 2 set to %d\r\n", track_pan);
  if (track_mask & SND_TRACK_3)
      fprintf (stderr, "snd_track_pan(): track 3 set to %d\r\n", track_pan);
  if (track_mask & SND_TRACK_4)
      fprintf (stderr, "snd_track_pan(): track 4 set to %d\r\n", track_pan);
  if (track_mask & SND_TRACK_5)
      fprintf (stderr, "snd_track_pan(): track 5 set to %d\r\n", track_pan);
 #endif

 return OK;

} /* snd_track_pan() */

/***** snd_track_pan() *****************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_load_samples()                                            */
/*                                                                         */
/* Given a list of "engine" samples...                                     */
/*                                                                         */
/* 1. loads the samples from disk                                          */
/* 2. creates and downloads the engine sample table                        */
/* 3. downloads the actual sample data                                     */
/*                                                                         */
/* The "engine" samples are assumed to...                                  */
/*                                                                         */
/* 1. be on the game disk with names exactly as passed in                  */
/* 2. sample rate of 15,625 Hz (half usual DCS rate)                       */
/* 3. 16-bit little-endian PC format                                       */
/* 4. raw .SND file, no headers                                            */
/* 5. mono                                                                 */
/*                                                                         */
/* The "engine" sound functions are for playing back un-compressed, looped */
/* data that be sped up and slowed down for making engine-like noises.     */
/* Any other uses are encouraged, but completely unsupported. :-)          */
/*                                                                         */
/* This code should work in a general purpose situation, _but_, the sound  */
/* code expects there to be four samples loaded:                           */
/*                                                                         */
/* 1. sample #1, sample#2 and sample #3 make up player "engine" sound      */
/* 2. sample #4 makes up drone "engine" sound                              */
/*                                                                         */
/* Note: It's easy to mess this up. Read the docs on the sample playback   */
/* engine stuff for more info.                                             */
/*                                                                         */
/* Note!!! The "engine" samples MUST be loaded before any compressed data! */
/* The raw samples MUST be the first data after the sound call table!      */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* MVB 04 Feb 97                                                           */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int snd_load_samples (char **sample_names, int num_samples)

{ /* snd_load_samples() */

int i;
int k;
int j;
int num_read;                  /* num bytes actually read from file */

snd_sample_t *sample_list;   /* list of sample info structs */
int sample_list_size;          /* size of sample info list in bytes */
snd_sample_t *sp;              /* pointer to one sample info struct */

unsigned int begin_addr;     /* starting snd DRAM addr to load */
unsigned int end_addr;       /* ending snd DRAM addr to load */
unsigned int address_range;    /* how many 16-bit words we are sending */

unsigned int sample_start;   /* address of the sample in linear words */
unsigned int sample_end;

unsigned int addr_page;        /* address of the sample in page:offset */
unsigned int addr_offset;

unsigned int upper_word;     /* splitting 32-bit into 16-bit */
unsigned int lower_word;

unsigned int data_checksum;    /* checksum we compute */
unsigned int checksum;       /* checksum returned to us */

int status;
unsigned int data_word;

struct ffblk   ffblk;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_samples(): begin\r\n");
 #endif

 /* allocate space for sample structures */

 sample_list_size = sizeof(snd_sample_t) * num_samples;
 sample_list = malloc (sample_list_size);
 if (sample_list == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_load_samples(): ERROR could not allocate %d bytes for sample list.\r\n", sample_list_size);
#endif
   return ERROR;
   }

 /*** step 1: read in the sample data ***/

 sp = 0;

 for (i=0; i < num_samples; i++)
   {

   sp = &sample_list[i];

   sp->name = sample_names[i];

   #if SND_DEBUG
     fprintf (stderr, "\r\n");
     fprintf (stderr, "snd_load_samples(): sample %d: %s\r\n", i, sp->name);
     fprintf (stderr, "snd_load_samples(): name[i]: %s\r\n", sample_names[i]);
   #endif

   // Get the info about the file
   if(findfirst(sp->name, &ffblk, 0))
   {
#ifdef DEBUG
      fprintf(stderr, "snd_load_samples(): ERROR can not get info for file: %s\r\n", sp->name);
#endif
      free(sample_list);
      return(ERROR);
   }

   /* open the file */
   sp->file_handle = fopen (sp->name, "rb");
   if (sp->file_handle == NULL)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR cannot open file %s for reading.\r\n", 
                  sp->name);
#endif
     free (sample_list);
     return ERROR;
     }

   /* allocate memory to hold it */
   /* this is in bytes */  
   sp->file_size = ffblk.ff_fsize;

   #if SND_DEBUG
     fprintf (stderr, "snd_load_samples(): file_size: %d bytes\r\n", sp->file_size);
   #endif

   /* the arg to malloc() is bytes */
   sp->data = malloc (sp->file_size);
   if (sp->data == NULL)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR cannot malloc %d bytes for data.\r\n", 
                   sp->file_size);
#endif
     free (sample_list);
     return ERROR; 
     }

   /* read in the file */
   num_read = fread (sp->data, 1, sp->file_size, sp->file_handle);
   if (num_read != sp->file_size)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR reading file %s.\r\n", sp->name);
     fprintf (stderr, "file size:%d bytes, actually read:%d bytes\r\n", 
                  sp->file_size, num_read);
#endif
     free (sample_list);
     free (sp->data);
     return ERROR; 
     }

   /* close the file */
   fclose (sp->file_handle);

   } /* for i */


 /*** step 2: download the sample address table ***/

  sample_start = SND_TABLE_SIZE + SAMPLE_TABLE_SIZE;
  begin_addr = (SND_TABLE_SIZE/2);

 for (i=0; i < num_samples; i++)
   {

   sp = &sample_list[i];

   #if SND_DEBUG
     fprintf (stderr, "\r\n");
     fprintf (stderr, "snd_load_samples(): addr table entries for sample %d\r\n", i);
   #endif

    // a 32-bit byte address needs two 16-bit locations
    // total of 4 16-bit words for start and end
    end_addr = begin_addr + SAMPLE_ENTRY_SIZE - 1;
    
   data_checksum = 0x0000;

   /* send the start and end DRAM addresses to the sound DSP */

   /* send the load command */
     if (snd_send_data (SND_CMD_LOAD_DRAM) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending load D/RAM cmd.\r\n");
#endif
     return ERROR;           
     }

   /* send the starting load address */
   upper_word = (begin_addr >> 16) & SND_MASK16;
   lower_word = begin_addr & SND_MASK16;
   if (snd_send_data (upper_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending high begin addr.\r\n");
#endif
       return ERROR;         
     }
   if (snd_send_data (lower_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending low begin addr.\r\n");
#endif
       return ERROR;         
     }

   /* send the ending load address */
   upper_word = (end_addr >> 16) & SND_MASK16;
   lower_word = end_addr & SND_MASK16;
   if (snd_send_data (upper_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending high end addr.\r\n");
#endif
       return ERROR;         
     }
   if (snd_send_data (lower_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending low end addr.\r\n");
#endif
       return ERROR;         
     }

    /* send the starting address for the sample */
    /* the ending address needs to be the actual last sample */
    sample_end = sample_start + sp->file_size - 2;

   #if SND_DEBUG
     fprintf (stderr, "sample start addr:%x  sample end addr:%x\r\n", sample_start, sample_end);
     fprintf (stderr, "sample size:%x \r\n", sp->file_size);
   #endif  

    /* get the top 16 of the 32-bit word */
    upper_word = (sample_start >> 16) & 0x0000FFFF;
    /* get the bottom 16 of the 32-bit word */
    lower_word = sample_start & 0x0000FFFF;

    /* convert to upper:lower to page:offset */
    addr_page = (sample_start/2) / DCS2_PAGE_SIZE;
    addr_offset = (sample_start/2) % DCS2_PAGE_SIZE;

   #if SND_DEBUG
     fprintf (stderr, "sample start page:%x  sample start offset:%x\r\n", addr_page, addr_offset);
   #endif  

    /* send the offset */
    data_checksum += addr_offset; 
   if (snd_send_data (addr_offset) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending start addr offset.\r\n");
#endif
       return ERROR;         
     }

    /* send the page */ 
    data_checksum += addr_page; 
   if (snd_send_data (addr_page) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending start addr page.\r\n");
#endif
       return ERROR;         
     }


    /* send the ending address for the sample */

    /* get the top 16 of the 32-bit word */
    upper_word = (sample_end >> 16) & 0x0000FFFF;
    /* get the bottom 16 of the 32-bit word */
    lower_word = sample_start & 0x0000FFFF;

    /* convert to upper:lower to page:offset */
    addr_page = (sample_end/2) / DCS2_PAGE_SIZE;
    addr_offset = (sample_end/2) % DCS2_PAGE_SIZE;

    /* send the offset */
    data_checksum += addr_offset; 
   if (snd_send_data (addr_offset) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending end addr offset.\r\n");
#endif
       return ERROR;         
     }

    /* send the page */ 
    data_checksum += addr_page; 
   if (snd_send_data (addr_page) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending end addr page.\r\n");
#endif
       return ERROR;         
     }

   snd_get_data (&checksum);         

   if ((checksum & SND_MASK16) != data_checksum)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR reading checksum.\r\n");     
     fprintf (stderr, "expected %04X, received %04X\r\n", 
     data_checksum,
                 (checksum & SND_MASK16));     
#endif
     return ERROR;       
     }

    // sample_start and sample_end are _byte_ addrs
    // we are storing page and offset as _16-bit-word_ addrs
    // so we need to add 2-bytes, or one word
    // sample_end points to the last sample in the sound, NOT
    // to the first sample of the next sound
    sample_start = sample_end + 2;

   /* this is the DRAM load cmd address */
   /* _word_ address */
    begin_addr = end_addr + 1;

   } /* for i */


 /*** step 3: send the sample data ***/

 /* start loading sample data after the sound */
 /* call table and after the sample address table */
 /* this is in 8-bit mode */
 snd_bank_addr = SND_TABLE_SIZE + SAMPLE_TABLE_SIZE;


  for (i=0; i < num_samples; i++)
   {

   #if SND_DEBUG
     fprintf (stderr, "\r\n");
     fprintf (stderr, "snd_load_samples(): data for sample %d\r\n", i);
   #endif

    /* set a pointer to structure */
    sp = &sample_list[i];

   /* begin_addr and end_addr are in 16-bit mode */      
     begin_addr = snd_bank_addr / 2;
   /* file size is in bytes */
    address_range = sp->file_size / 2;
    end_addr = begin_addr + address_range - 1;


   #if SND_DEBUG
     fprintf (stderr, "addr setup: begin_addr:%x end_addr:%x\r\n", 
                   begin_addr, end_addr);
   #endif  

   /* send the load command */
     if (snd_send_data (SND_CMD_LOAD_DRAM) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending load D/RAM cmd.\r\n");
#endif
     return ERROR;           
     }

   /* send the starting load address */
   upper_word = (begin_addr >> 16) & SND_MASK16;
   lower_word = begin_addr & SND_MASK16;
   if (snd_send_data (upper_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending high begin addr.\r\n");
#endif
       return ERROR;         
     }
   if (snd_send_data (lower_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending low begin addr.\r\n");
#endif
       return ERROR;         
     }

   /* send the ending load address */
   upper_word = (end_addr >> 16) & SND_MASK16;
   lower_word = end_addr & SND_MASK16;
   if (snd_send_data (upper_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending high end addr.\r\n");
#endif
       return ERROR;         
     }
   if (snd_send_data (lower_word) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR sending low end addr.\r\n");
#endif
       return ERROR;         
     }

   /* note: I tried to get this to work by indexing sp->data[] */
   /* with j, and having j go from 0 to address_range, but the */
   /* stupid compiler optimizes it out, even if you use volatile, */
   /* so that's why I use the k index */

   /* send over the actual data */
    data_checksum = 0;
   k = 0;
   for (j=0; j < address_range/2; j++)
       {

       data_word = (sp->data[k] & SND_MASK16);
       data_checksum += data_word;
       status = snd_send_data (data_word);
       if (status != OK)
         {
#ifdef DEBUG
         fprintf (stderr, "snd_load_samples(): ERROR sending data word 1 %d\r\n", j);
#endif
         return ERROR;
         }

       data_word = ((sp->data[k++] >> 16) & SND_MASK16);
       data_checksum += data_word;
       status = snd_send_data (data_word);
       if (status != OK)
         {
#ifdef DEBUG
         fprintf (stderr, "snd_load_samples(): ERROR sending data word 2 %d\r\n", j);
#endif
         return ERROR;
         }

       } /* for j */

   status = snd_get_data (&checksum);         
   if (status != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR waiting for checksum.\r\n");
#endif
     return ERROR;
     }

   if ((data_checksum & SND_MASK16) != checksum)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_load_samples(): ERROR reading checksum.\r\n");     
     fprintf (stderr, "expected %04X, received %04X\r\n", (data_checksum & SND_MASK16),
                  checksum);     
#endif
     return ERROR;       
     }

   snd_bank_addr += sp->file_size;

    } /* for i */

 /*** step 4: clean up ***/

 // free the sample data
 // free the sample list
 for (i=0; i < num_samples; i++)
   {
   free (sample_list[i].data);
   }
 free (sample_list);


 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_load_samples(): done\r\n");
 #endif

 return OK;

} /* snd_load_samples() */

/***** END of snd_load_samples() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_scall_engine()                                            */
/*                                                                         */
/* Sends a command to control one of the three available "engine" sounds.  */
/*                                                                         */
/* cmd is one of:                                                          */
/*                                                                         */
/* SND_CMD_PLAYER_CTRL - player sound, samples #1, #2, #3, four octaves    */
/* SND_CMD_DRONE1_CTRL - drone #1 sound, sample #4, four octaves           */
/* SND_CMD_DRONE2_CTRL - drone #2 sound, sample #4, two octaves            */
/*                                                                         */
/* pitch - 0 (slow) to 255 (fast)                                          */
/*                                                                         */
/* volume - 0 (off), 1 (soft) to 255 (loud)                                */
/*                                                                         */
/* pan - 0 (left), 127 (center), 255 (right)                               */
/*                                                                         */
/* Note that you turn an engine sound "off" by setting its volume to zero. */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* MVB 05 Feb 97                                                           */
/* 28 Aug 97 MVB - added snd_lock support to disable sound calls           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_scall_engine (unsigned int cmd, unsigned int pitch, 
                     unsigned int volume, unsigned int pan)

{ /* snd_scall_engine() */

unsigned int temp;

/***************************************************************************/

 #if SND_DEBUG_ENGINE
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_scall_engine(): begin\r\n");
 #endif

 /* lockout sound calls if disabled */
 if (snd_lock) 
   {
   #if SND_DEBUG
      fprintf (stderr, "snd_scall_engine(): sound calls locked out, no call made\r\n");
   #endif
   return OK;
   }

 /* bounds check */

 if (pitch > 255)
   pitch = 255;

 if (volume > 255) 
   volume = 255;

 if (pan > 255) 
   pan = 255;

 #if SND_DEBUG_ENGINE
  fprintf (stderr, "snd_scall_engine(): command %d (0x%04X)\r\n", cmd, cmd);
  fprintf (stderr, "snd_scall_engine(): pitch %d (0x%04X)\r\n", pitch, pitch);
  fprintf (stderr, "snd_scall_engine(): volume %d (0x%04X)\r\n", volume, volume);
  fprintf (stderr, "snd_scall_engine(): pan %d (0x%04X)\r\n", pan, pan);
 #endif
    

 /* send the sound call command */
 if (snd_send_data (cmd) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_engine(): ERROR sending command.\r\n");
#endif
   return ERROR;
   }

 /* send the pitch and volume */
 temp = (pitch & SND_MASK8) << 8 | (volume & SND_MASK8);
 if (snd_send_data (temp) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_engine(): ERROR sending pitch and volume.\r\n");
#endif
   return ERROR;
   }

 /* send the pan */
 if (snd_send_data (pan & SND_MASK8) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_scall_engine(): ERROR sending pan.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG_ENGINE
  fprintf (stderr, "snd_scall_engine(): done\r\n");
 #endif

 return OK;

} /* snd_scall_engine() */

/***** END of snd_scall_engine() *******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stop_engine()                                             */
/*                                                                         */
/* Stops the specified engine sound. Valid arguments are                   */
/*                                                                         */
/*   SND_CMD_DRONE1_CTRL                                                   */
/*   SND_CMD_DRONE2_CTRL                                                   */
/*   SND_CMD_PLAYER_CTRL                                                   */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* MVB 21 Feb 97                                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stop_engine (unsigned int engine_select)

{ /* snd_stop_engine() */

 #if SND_DEBUG
  fprintf (stderr, "snd_stop_engine(): begin\r\n");
 #endif

   /* set the pitch and volume to zero */
   /* pan doesn't matter */
   if (snd_scall_engine (engine_select, 0, 0, 127) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_stop_engine(): ERROR stopping engine %04X.\r\n", engine_select);
#endif
     return ERROR;
     }
   else
     {
     #if SND_DEBUG
       fprintf (stderr, "snd_stop_engine(): engine %04X stopped.\r\n", engine_select);
     #endif
     } 
   
   return OK;

 #if SND_DEBUG
  fprintf (stderr, "snd_stop_engine(): done\r\n");
 #endif

} /* snd_stop_engine() */

/***** END of snd_stop_engine() ********************************************/

#if defined(SEATTLE)
/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_clear_latch()                                             */
/*                                                                         */
/* Does several reads from the sound system and explicitly clears the      */
/* latch to make sure that there is no lingering data in the sound-to-     */
/* host port.                                                              */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* MVB 05 Feb 97                                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

void snd_clear_latch (void)

{ /* snd_clear_latch() */

int i;
volatile unsigned int data;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_clear_latch(): begin\r\n");
 #endif

   for (i=0; i < 5; i++)
       {
	   // data = (*snd_sth_data & SND_MASK16);
       _ioctl (4, FIOCGETSOUNDDATA, (int) &data);
       }

   /* In 'C31 mode a read from the sound DSP */
   /* must be followed by a write. */           
   /* This clears the data ready bit. */                        

   // *snd_sth_data = 0x0000;
   _ioctl (4, FIOCSETSTHDATA, 0);

 #if SND_DEBUG
  fprintf (stderr, "snd_clear_latch(): done\r\n");
 #endif

} /* snd_clear_latch() */

/***** END of snd_clear_latch() ********************************************/
#endif

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_query_buffers()                                           */
/*                                                                         */
/* Used by streaming functions.                                            */
/*                                                                         */
/* Sends a command to the sound system requesting the number of empty      */
/* buffers available.                                                      */
/*                                                                         */
/* Returns the number of empty buffers in the variable passed in. Returns  */
/* a status of ERROR or OK.                                                */
/*                                                                         */
/* MVB 05 Feb 97                                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_query_buffers (int *buffers_available)

{ /* snd_query_buffers() */

int status;

/***************************************************************************/

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_query_buffers(): begin\r\n");
 #endif

 /* send the query command */
 if (snd_send_data (SND_CMD_STREAM_REQFREE) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_query_buffers(): ERROR sending query command.\r\n");
#endif
   return ERROR;
   }

  /* read the answer */
  status = snd_get_data (buffers_available);         
 if (status != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_query_buffers(): ERROR waiting for query reply.\r\n");
#endif
   return ERROR;
   }

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "%d buffers available\r\n", *buffers_available);
  fprintf (stderr, "snd_query_buffers(): done\r\n");
 #endif

  return (OK);

} /* snd_query_buffers() */

/***** END of snd_query_buffers() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stream_volume()                                           */
/*                                                                         */
/* Sends the reserved system sound call to update the volume of the audio  */
/* being streamed to the sound system.                                     */
/*                                                                         */
/* 0 is mute                                                               */
/* 1 is softest                                                            */
/* 255 is loudest                                                          */
/*                                                                         */
/* Functionally equivalent to snd_master_volume() but the streaming volume */
/* does not send the complement.                                           */
/*                                                                         */
/* The streaming volume is not affected by sending a sound call zero "stop */
/* all". On reset, the streaming volume is zero, but once you set it, it   */
/* stays set.                                                              */
/*                                                                         */
/* Returns OK or an error if it times out.                                 */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stream_volume (int volume)

{ /* snd_stream_volume() */

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_stream_volume(): begin\r\n");
 #endif

 if (snd_send_data (SND_CMD_STREAM_VOLUME) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_volume(): ERROR sending stream volume cmd.\r\n");   
#endif
    return ERROR;
   }

 /* check bounds */
 if (volume > 255)
   volume = 255;
 if (volume < 0)
   volume = 0;

 #if SND_DEBUG
  fprintf (stderr, "snd_stream_volume(): %d\r\n", volume);
 #endif

 if (snd_send_data (volume) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_volume(): ERROR sending volume data.\r\n");   
#endif
    return ERROR;
   }

 #if SND_DEBUG
  fprintf (stderr, "snd_stream_volume(): done\r\n");
 #endif

  return OK;

} /* snd_stream_volume() */

/***** END of snd_stream_volume() ******************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_read_buffer()                                             */
/*                                                                         */
/* Streaming audio function.                                               */
/*                                                                         */
/* Reads one buffer's worth of audio data from the file. The size of the   */
/* buffer will depend on whether the file is mono or stereo.               */
/*                                                                         */
/* Assumes the caller allocated the buffer.                                */
/*                                                                         */
/* Returns OK or an error if it times out.                                 */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_read_buffer (unsigned int *buffer, int frame_size, FILE *file_handle)

{ /* snd_read_buffer() */

int num_read;
int read_count;

/***************************************************************************/

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "snd_read_buffer(): begin\r\n");
 #endif

 /* read count is in bytes */
 /* frame_size is in 32-bit words per frame */
 read_count = 4 * frame_size;

  num_read = fread (buffer, 1, read_count, file_handle);

  if (num_read != read_count)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_read_buffer(): ERROR reading file.\r\n");
   fprintf (stderr, "expected:%d bytes, actually read:%d bytes\r\n", read_count, num_read);
#endif
   return NULL;  
   }

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "snd_read_buffer(): done\r\n");
 #endif

  return OK;

} /* snd_read_buffer() */

/***** END of snd_read_buffer() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_send_buffer()                                             */
/*                                                                         */
/* Sends one buffer's worth of audio data to the sound system. The buffer  */
/* size depends on whether the file is mono or stereo.                     */
/*                                                                         */
/* Note: As of 5 Feb 7, this code only deals with stereo files.            */
/*                                                                         */
/* Returns ERROR or OK.                                                    */
/*                                                                         */
/* The basic protocol is:                                                  */
/*                                                                         */
/* 1. host sends command to start buffer xfer                              */
/* 2. sound system echoes back OK or "no empty buffers avail" as error     */
/* 3. host sends buffer data                                               */
/* 4. sound system sends back checksum (simple 16-bit sum) for buffer      */
/*                                                                         */
/* Returns OK or an error if it times out.                                 */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_send_buffer (unsigned int *buffer, int frame_size)

{ /* snd_send_buffer() */

// 13 Feb 97 MVB - checksum no longer used as ver 0117 of sound opsys
//unsigned int checksum;
//unsigned int data_checksum;

unsigned int temp;
unsigned int status;
unsigned int data_word;
int j;
int k;

/***************************************************************************/

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "snd_send_buffer(): begin\r\n");
 #endif

 /* send the command to start the xfer */
 if (snd_send_data (SND_CMD_STREAM_SENDBUF) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_send_buffer(): ERROR sending buffer xfer cmd.\r\n");   
#endif
    return ERROR;
   }

 /* read the answer */
 status = snd_get_data (&temp);         
 if (status != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_send_buffer(): ERROR waiting for xfer cmd ack.\r\n");
#endif
   return ERROR;
   }

  if (temp != SND_STATUS_SENDOK)
     {
#ifdef DEBUG
    fprintf (stderr, "snd_send_buffer(): ERROR bad xfer cmd ack.\r\n");
    fprintf (stderr, "expected:%04x received:%04x\r\n", SND_STATUS_SENDOK, temp);
#endif
    return ERROR;
    }

 /* send over the buffer */
 /* frame size should be the number of 32-bit words per frame */
 /* which is one half the number of 16-bit samples */

 k = 0;
 for (j=0; j < frame_size; j++)
     {

     data_word = (buffer[k] & SND_MASK16);
     status = snd_send_data (data_word);
     if (status != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_send_buffer(): ERROR sending data word 1 %d\r\n", j);
#endif
       return ERROR;
       }

     data_word = ((buffer[k++] >> 16) & SND_MASK16);
     status = snd_send_data (data_word);
     if (status != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_send_buffer(): ERROR sending data word 2 %d\r\n", j);
#endif
       return ERROR;
       }

     } /* for j */

 /* no longer reading checksum */
 /* as of version 114 of sound system */
 /* 13 Feb 97 MVB */

 #if 0
 status = snd_get_data (&checksum);         
 if (status != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_send_buffer(): ERROR waiting for checksum.\r\n");
#endif
   return ERROR;
   }

  if ((data_checksum & SND_MASK16) != checksum)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_send_buffer(): ERROR reading checksum.\r\n");      
   fprintf (stderr, "expected %04X, received %04X\r\n", (data_checksum & SND_MASK16),
                checksum);     
#endif
   return ERROR;   
   }
 #endif

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "snd_send_buffer(): done\r\n");
 #endif

  return (OK);

} /* snd_send_buffer() */

/***** END of snd_send_buffer() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stream_init()                                             */
/*                                                                         */
/* Does setup for streaming audio from the game disk to the sound system.  */
/* This function does the setup, and snd_stream() does the actual xfer.    */
/*                                                                         */
/* The audio file format is 16-bit, stereo, 31250 Hz sample rate, straight */
/* raw audio (e.g. Sound Forge "raw" .SND file), no compression.           */
/*                                                                         */
/* The sound system has between 10 and 15 buffers available for streaming. */
/* Each buffer is one frame of stereo audio, which is 480 16-bit samples.  */
/*                                                                         */
/* This function pre-loads all the available buffers. You should then call */
/* snd_stream() to start playback and feed it buffers as the file plays.   */
/*                                                                         */
/* snd_buffer_count is global                                              */
/*                                                                         */
/* stream_handle is global                                                 */
/*                                                                         */
/* stream_name - file name without any extension                           */
/*                                                                         */
/* num_buffers - total number of buffers in the file, subsequently needs   */
/*               to be passed to snd_stream()                              */
/*                                                                         */
/* Returns OK if successful or ERROR if not.                               */
/*                                                                         */
/* MVB 05 Feb 97                                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stream_init (char *stream_name, int *num_buffers)

{ /* snd_stream_init() */

int buffers_available;
static char file_name [MAX_NAME];                /* name plus .SND extension */
static unsigned int buffer [SND_STEREO_FRAME];   /* one frame of stereo audio */

int i;
int status;

struct ffblk ffblk;

/***************************************************************************/

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_stream_init(): begin\r\n");
 #endif


 /* add the extension */
 strcpy (file_name, stream_name);
 strcat (file_name, SND_SND_EXT);

   // Get the info about the file
   if(findfirst(file_name, &ffblk, 0))
   {
#ifdef DEBUG
      fprintf(stderr, "snd_stream_init(): ERROR cannot get info for file %s\r\r\n", file_name);
#endif
      return(NULL);
   }

 /* open the file */
 snd_stream_handle = fopen (file_name, "rb");
 if (snd_stream_handle == NULL)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_init(): ERROR cannot open file %s for reading.\r\n", file_name);
#endif
   return NULL;
   }

 /* return the total number of buffers in this file */
 *num_buffers = ffblk.ff_fsize / (SND_STEREO_FRAME * 4);
 #if SND_DEBUG_STREAMING
  fprintf (stderr, "snd_stream_init(): file size: %d bytes\r\n", ffblk.ff_fsize);
  fprintf (stderr, "snd_stream_init(): %d buffers in file.\r\n", *num_buffers);
 #endif


 /* stop the sound system from sending return info */
 /* for normal tracks back - it could corrupt the */
 /* handshaking during streaming */
 if (snd_send_data (SND_CMD_DISABLE_QUEUE) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_init(): ERROR sending disable queue cmd.\r\n");   
#endif
   fclose (snd_stream_handle);
   return ERROR;
   }

 /* empty out all the streaming buffers */
 if (snd_send_data (SND_CMD_STREAM_FLUSH) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_init(): ERROR sending flush stream cmd.\r\n");    
#endif
   fclose (snd_stream_handle);
   return ERROR;
   }

  /* now that the queue is disabled and empty, */
  /* clear any data that might be sitting in the latch */
  /* data can stay latched even through a sound soft reset */
  /* so make sure to clear it */
  snd_clear_latch();


 /* find out how many buffers the sound system has */
 /* available to use for streaming */
  if (snd_query_buffers (&buffers_available) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_init(): ERROR requesting buffer count.\r\n");   
#endif
   fclose (snd_stream_handle);
   return ERROR;
   }

  if (buffers_available == 0)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream_init(): ERROR no buffers available.\r\n");    
#endif
   fclose (snd_stream_handle);
   return ERROR;
   }

 /* the sound system should have all of its buffers empty at this point */
 /* save this state so when we're done playing, we know when to send the */
 /* stop command */
 /* snd_buffer_count is global */
 snd_buffer_count = buffers_available;


 /* pre-load the empty buffers **/
  for (i=0; i < buffers_available; i++)
      {
      status = snd_read_buffer (buffer, SND_STEREO_FRAME, snd_stream_handle);
      if (status != OK)
         {
#ifdef DEBUG
         fprintf (stderr, "snd_stream_init(): ERROR reading pre-load buffer %d of %d.\r\n", 
                      i, buffers_available);
#endif
         fclose (snd_stream_handle);
         return ERROR; 
         } 

      status = snd_send_buffer (buffer, SND_STEREO_FRAME);
      if (status != OK)
         {
#ifdef DEBUG
         fprintf (stderr, "snd_stream_init(): ERROR sending pre-load buffer %d of %d.\r\n", 
                      i, buffers_available);
#endif
         fclose (snd_stream_handle);
         return ERROR; 
         } 

      } /* for */

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "snd_stream_init(): done\r\n");
 #endif

 return OK;

} /* snd_stream_init() */

/***** END of snd_stream_init() ********************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stream()                                                  */
/*                                                                         */
/* Note: This function sleeps. It must be called by a process.             */
/*                                                                         */
/* Note: I measured this function to take about 3 to 5 milliseconds per    */
/* frame to feed the sound system. About 1 to 2 msec of that is the disk   */
/* read; the rest is the xfer to the sound DSP. -mb 7 Feb 97 As of 19 Feb  */
/* 97, this is down to about 2 to 4 milliseconds per frame. - mb           */
/*                                                                         */
/* Call snd_stream_init() first to open the audio file. stream_handle is   */
/* a global variable. snd_buffer_count is also a global variable.          */
/*                                                                         */
/* The basic flow of this function is:                                     */
/*                                                                         */
/* 1. start playing                                                        */
/*                                                                         */
/* 2. A. request # of empty buffers                                        */
/*    B. while there are empty buffers, send a buffer over                 */
/*    C. repeat for length of file                                         */
/*                                                                         */
/* 3. wait for all buffers to empty out                                    */
/*                                                                         */
/* 4. send the stop command                                                */
/*                                                                         */
/* Returns ERROR or OK.                                                    */
/*                                                                         */
/* MVB 05 Feb 97                                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stream (int buffer_total)

{ /* snd_stream() */

int done;                /* have all buffers been sent? */
int buffers_available;   /* how many free buffers does snd sys have? */
int buffer_count;        /* total num of buffers we have sent over */
int timeout;             /* waiting for buffers to empty out */
int status;              /* result of buffer send */
int num_read;            /* _bytes_ read from file */  
int read_count;          /* how many _bytes_ to read */

static unsigned int buffer [SND_STEREO_FRAME];   /* one frame of stereo audio */

//uncommment this for timing
//#define STREAM_TIMING 1

#if STREAM_TIMING
 float ftime;
#endif

int tick_count;
float t1;
float t2;

#if defined(VEGAS)
#if STREAM_TIMING
int	start_time;
#endif
#endif

/***************************************************************************/

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_stream(): begin\r\n");
 #endif

 /* the function snd_stream_init() was called previously */
 /* to pre-load the buffers - sending this command starts */
 /* the actual audio playing */

 if (snd_send_data (SND_CMD_STREAM_START) != OK)
   {
#ifdef DEBUG
   fprintf (stderr, "snd_stream(): ERROR sending stream start cmd.\r\n");   
#endif
   snd_stream_stop();
   return ERROR;
   }

 /* keep track of the number of buffers we pre-loaded */
 /* snd_buffer_count is a global var */
 buffer_count = snd_buffer_count;

 done = FALSE;

 /* feed the sound system buffers as it needs them */
 /* each buffer is one DCS frame */
 /* each frame is 7.68 milliseconds worth of stereo audio */
 /* each frame is 240 audio samples */
 /* a stereo frame is 480 audio samples */
 /* a sample is a 16-bit word, or 2 bytes */
 /* there are two 16-bit samples per 32-bit word */
 read_count = 4 * SND_STEREO_FRAME; 

  tick_count = 0;

  while (!done)
      {

     #if STREAM_TIMING
//       load_timer (0);
#if defined(SEATTLE)
         _ioctl(5, FIOCSTARTTIMER2, 0);
#elif defined(VEGAS)
			start_time = get_timer_val();
#endif
     #endif

     /* as the sound system plays audio, the buffers will */
     /* free up, and as they free up, we want to fill them */
     if (snd_query_buffers (&buffers_available) != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): ERROR buffer query 1 failed.\r\n");
       fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
       snd_stream_stop();
       return ERROR;
       }

     /* each buffer of audio is 7.68 milliseconds long, */
     /* so each video frame == 2.08 audio buffers, */
     /* so we should only be sending 2 or 3 buffers each tick */ 
     if (buffers_available > 6)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): WARNING %d buffers empty\r\n", buffers_available);
#endif
       }

     if ((buffers_available == 0) && (buffer_count > snd_buffer_count))
       {
       #if SND_DBG
         fprintf (stderr, "snd_stream(): %d buffers available\r\n", buffers_available);
       #endif SND_DBG
       }

     /* if the number of available buffers exceeds the number */
     /* of buffers total, then something has gone wrong */
     if (buffers_available > snd_buffer_count)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): ERROR bad buffer count %d\r\n", buffers_available);
       fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
       snd_stream_stop();
       return ERROR;
       }

     /* if there are empty buffers, fill them */
     while ((buffers_available > 0) && (!done))
       {
       /* read in one buffer of audio from the disk */ 
       num_read = fread (buffer, 1, read_count, snd_stream_handle);
       if (num_read != read_count)
         {
#ifdef DEBUG
         fprintf (stderr, "snd_stream(): ERROR reading buffer %d of %d.\r\n", 
                     buffer_count, buffers_available);
         fprintf (stderr, "expected:%d bytes, actually read:%d bytes\r\n", read_count, num_read);
         fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
         snd_stream_stop();
         return ERROR;
         }

       /* send one buffer of audio to the sound system */  
       status = snd_send_buffer (buffer, SND_STEREO_FRAME);
       if (status != OK)
         {
#ifdef DEBUG
         fprintf (stderr, "snd_stream(): ERROR sending buffer %d of %d.\r\n", 
                   buffer_count, buffer_total);
         fprintf (stderr, "snd_stream(): buffers_available: %d\r\n", buffers_available);
         fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
         snd_stream_stop();
         return ERROR;
         } 

      /* keep track of what we've sent to the sound system */
      buffer_count++;

      #if SND_DEBUG_STREAMING
        fprintf (stderr, "\r\n");
        fprintf (stderr, "buffer %d sent OK\r\n", buffer_count);
      #endif

      /* check if we are at EOF */
      if (buffer_count >= buffer_total)
         done = TRUE;

      if (snd_query_buffers (&buffers_available) != OK)
        {
#ifdef DEBUG
        fprintf (stderr, "snd_stream(): ERROR buffer query 2 failed.\r\n");
        fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
        snd_stream_stop();
        return ERROR;
        }

      } /* while buffers avail */

      #if STREAM_TIMING
#if defined(SEATTLE)
      _ioctl(5, FIOCGETTIMER2, (int)&time);
#elif defined(VEGAS)
		time = start_time - get_timer_val();
#endif
       ftime = (float) time;   /* 20 nanosecond ticks */
       ftime /= 50000.0f;                /* convert to milliseconds */
       fprintf(stderr, "snd_stream(): %f msecs\r\r\n", ftime);
      #endif

     sleep (1);

     /* This is for error checking. It keeps track of how many seconds */
     /* have gone by, and checks that the equivalent amount of audio */
     /* has been sent to the sound system. This is a sanity check to */
     /* make sure that both the game and the sound system are running */
     /* at the right speed. */

     tick_count++;
     t1 = ((float) (tick_count)) * 16.67f;
     t2 = ((float) (buffer_count)) * 7.68f;

     /* If the game and the sound system get more than 50 milliseconds */
     /* apart, then complain. The sound system has 13 buffers of 7.68 msec */
     /* each, which is about 100 msec of slack. */

     if ((t1 - t2) > 100.0f)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): ERROR game and sound system not in sync\r\n");
       fprintf (stderr, "              host time t1:   %f msecs\r\n", t1);
       fprintf (stderr, "              stream time t2: %f msecs\r\n", t2);
       fprintf (stderr, "              buffer_count:   %d\r\n", buffer_count);
#endif
       }

     } /* while */

   /* we are done sending buffers, but the sound system */
   /* is not necessarily done playing them, so we wait */
   /* until all the buffers are empty */

   #if SND_DEBUG_STREAMING
     fprintf (stderr, "\r\n");
     fprintf (stderr, "snd_stream(): waiting for buffers to clear\r\n");
   #endif

  timeout = 0;

   do {

     /* are all buffers empty yet? */
     if (snd_query_buffers (&buffers_available) != OK)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): ERROR buffer query 3 failed.\r\n");
       fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
       snd_stream_stop();
       return ERROR;
       }

     /* if the number of available buffers exceeds the number */
     /* of buffers total, then something has gone wrong */
     if (buffers_available > snd_buffer_count)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): ERROR bad wait buffer count %d\r\n", buffers_available);
       fprintf (stderr, "snd_stream(): streaming stopped.\r\n");
#endif
       snd_stream_stop();
       return ERROR;
       }

     sleep (1);

     timeout++;

     if (timeout > SND_STREAM_TIMEOUT)
       {
#ifdef DEBUG
       fprintf (stderr, "snd_stream(): ERROR waiting for stream to end.\r\n");
       fprintf (stderr, "              buffer count %d\r\n", buffers_available);
#endif
       snd_stream_stop();
       return ERROR;
       } 

      } while (buffers_available != snd_buffer_count);

 /* all done */
 /* send stop cmd, flush queues, re-enable xmit */
 /* close file handle */
 snd_stream_stop();

 #if SND_DEBUG_STREAMING
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_stream(): done\r\n");
 #endif

 return OK;

} /* snd_stream() */

/***** END of snd_stream() ***************************************************/

/***************************************************************************/
/*                                                                         */
/* FUNCTION: snd_stream_stop()                                             */
/*                                                                         */
/* Call this function to stop streaming audio before the end of file is    */
/* reached.                                                                */
/*                                                                         */
/* 1. Sends the "stop streaming" command.                                  */
/*                                                                         */
/* 2. Flushes the sound-to-host transmit queue.                            */
/*                                                                         */
/* 3. Re-enables the sound-to-host transmit queue.                         */
/*                                                                         */
/* 4. Closes the global streaming file handle.                             */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* MVB 05 Feb 97                                                           */
/*                                                                         */
/* Copyright (c) 1997 Midway Games, Inc.                                   */
/*                                                                         */
/***************************************************************************/

int snd_stream_stop (void)

{ /* snd_stream_stop() */

int status;

/***************************************************************************/

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_stream_stop(): begin\r\n");
 #endif

   status = OK;

   /* all empty, so tell it to stop streaming */
   if (snd_send_data (SND_CMD_STREAM_STOP) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_stream_stop(): ERROR sending stream stop cmd.\r\n");   
#endif
     status = ERROR; 
     }

   /* clear out the return info queue */
   if (snd_send_data (SND_CMD_FLUSH_QUEUE) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_stream_stop(): ERROR sending flush queue cmd.\r\n");
#endif
     return ERROR;
     }

   /* turn back on the return info queue */
   if (snd_send_data (SND_CMD_ENABLE_QUEUE) != OK)
     {
#ifdef DEBUG
     fprintf (stderr, "snd_stream_stop(): ERROR sending enable queue cmd.\r\n");    
#endif
     status = ERROR; 
     }

   /* close up the file */
   fclose (snd_stream_handle);

   return status;

 #if SND_DEBUG
  fprintf (stderr, "\r\n");
  fprintf (stderr, "snd_stream_stop(): done\r\n");
 #endif

} /* snd_stream_stop() */

/***** END of snd_stream_stop() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* snd_stream_proc()                                                         */
/*                                                                           */
/* Simple test function for streaming audio.                                 */
/*                                                                           */
/* The file name passed in must be on the disk.                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 19 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/

void snd_stream_proc (int *args)

{ /* snd_stream_proc() */

int num_buffers; /* total number of buffers in the audio file */
int status;
char *file_name;

/*****************************************************************************/

 #if SND_DBG
   fprintf (stderr, "\r\n");
   fprintf (stderr, "snd_stream_proc(): begin\r\n");
 #endif

 /* extract the file name */
 file_name = (char *) args[0];

 /* the file must be on the disk */
 status = snd_stream_init (file_name, &num_buffers);

 if (status == OK)
     snd_stream (num_buffers);

 #if SND_DBG
   fprintf (stderr, "\r\n");
   fprintf (stderr, "snd_stream_proc(): done\r\n");
 #endif

 /* this function should go to sleep, */
 /* the proc that spawned it should kill it */
 suspend_self();

} /* snd_stream_proc() */

/***** END of snd_stream_proc() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_bank_lockout()                                              */
/*                                                                           */
/* Sets the global (to this module) variable snd_lock_BANK to mode (TRUE     */
/* or FALSE).                                                                */
/*                                                                           */
/* When this is set, any sound calls made using snd_scall_bank() will not    */
/* be sent, so as to not disturb the bank loading process.                   */
/*                                                                           */
/* You can still make sound calls using snd_scall_direct().                  */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 21 Mar 97                                                             */
/*                                                                           */
/*****************************************************************************/

void snd_bank_lockout (int mode)

{ /* snd_bank_lockout() */

 #if SND_DBG
   fprintf (stderr, "\r\n");
   fprintf (stderr, "snd_bank_lockout(): begin\r\n");
 #endif

   snd_lock_bank = mode;

 #if SND_DBG
   fprintf (stderr, "\r\n");
   fprintf (stderr, "snd_bank_lockout(): snd_lock_bank set to %d\r\n", snd_lock);
 #endif

} /* snd_bank_lockout() */

/***** END of snd_bank_lockout() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_lockout()                                                   */
/*                                                                           */
/* Sets the global (to this module) variable snd_lock to mode (TRUE or       */
/* FALSE).                                                                   */
/*                                                                           */
/* This will disable sending any and all sound calls, including using        */
/* snd_scall_direct(), snd_scall_bank(), snd_scall_quick(), and              */
/* snd_scall_return().                                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 22 Sep 97                                                             */
/*                                                                           */
/*****************************************************************************/

void snd_lockout (int mode)

{ /* snd_lockout() */

 #if SND_DBG
   fprintf (stderr, "\r\n");
   fprintf (stderr, "snd_lockout(): begin\r\n");
 #endif

   snd_lock = mode;

 #if SND_DBG
   fprintf (stderr, "\r\n");
   fprintf (stderr, "snd_lockout(): snd_lock set to %d\r\n", snd_lockout);
 #endif

} /* snd_lockout() */

/***** END of snd_lockout() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_get_first_sound()                                           */
/*                                                                           */
/* This will return the first sound call in the bank "name" passed. You can  */
/* use this along with the value from get_last_sound to determine the first  */
/* and last sound call in any given bank.                                    */
/*                                                                           */
/* Example: last_actual_sound = get_last_sound() - get_first_sound()         */
/* You would then know that the sound table for that bank goes from 0 to     */
/* last_actual_sound.                                                        */                 
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* BRE 21 AUG 97                                                             */
/*                                                                           */
/*****************************************************************************/

int snd_get_first_sound (char *bank_name)

{	/* snd_get_first_sound() */

snd_bank_node_t *bp;         /* bank node ptr */

/*****************************************************************************/

	bp = snd_bank_list;

	/* Find bank by name passed bank_name */
	while (bp != NULL)
		{
		if (!strcmp (bp->name, bank_name))
			break;
		bp = bp->next;
		}

	if (bp == NULL)
		{
		#ifdef DEBUG
		fprintf (stderr, "get_first_sound(): ERROR bank '%s' not found.\r\n", 
		         bank_name);
		#endif
		return ERROR;
		}

	/* scall addresses stored in the bank struct are */
	/* actual D/RAM locations in the sound memory */
	/* so we need to convert a mem location to a num */

	return (bp->scall_first / SND_SCALL_SIZE);	

} /* snd_get_first_sound() */

/***** END of get_first_sound() **********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_get_last_sound()                                            */
/*                                                                           */
/* This will return the last sound call in the bank "name" passed. You can   */
/* use this along with the value from get_first_sound to determine the first */
/* and last sound call in any given bank.                                    */
/*                                                                           */
/* Example: last_actual_sound = get_last_sound() - get_first_sound()         */
/* You would then know that the sound table for that bank goes from 0 to     */
/* last_actual_sound.                                                        */                 
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* BRE 21 AUG 97                                                             */
/*                                                                           */
/*****************************************************************************/

int snd_get_last_sound (char *bank_name)

{ /* snd_get_last_sound() */

snd_bank_node_t *bp;         /* bank node ptr */

/*****************************************************************************/

bp = snd_bank_list;

	/* Find bank by name passed bank_name */
	while (bp != NULL)
		{
		if (!strcmp (bp->name, bank_name))
			break;
		bp = bp->next;
		}

	if (bp == NULL)
		{
		#ifdef DEBUG
		fprintf (stderr, "get_last_sound(): ERROR bank '%s' not found.\r\n", 
		         bank_name);
		#endif
		return ERROR;
		}

	/* scall addresses stored in the bank struct are */
	/* actual D/RAM locations in the sound memory */
	/* so we need to convert a mem location to a num */

	return ((bp->scall_last / SND_SCALL_SIZE) - 1);	

} /* snd_get_last_sound() */

/***** END of get_last_sound() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_is_bank_loaded()                                            */
/*                                                                           */
/* This checks to see if the bank "name" passed is currently loaded in the   */
/* linked list of loaded banks. Note that this only checks the API-level     */
/* list; it has no way of knowing whether a bank is actually resident in the */
/* sound system D/RAM.                                                       */
/* 	                                                                         */
/* RETURNS:                                                                  */
/* 			(1) YES = If bank is loaded                                      */
/* 			(0) NO  = If bank is not loaded                                  */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* BRE 21 AUG 97                                                             */
/*                                                                           */
/*****************************************************************************/

int snd_is_bank_loaded (char *bank_name)

{ /* snd_is_bank_loaded() */

snd_bank_node_t *bp;         /* bank node ptr */

/*****************************************************************************/

	bp = snd_bank_list;

	/* Find bank by name passed bank_name */

	while (bp != NULL)
		{
	    if (!strcmp (bp->name, bank_name))
			break;
		bp = bp->next;
		}

	 if (bp == NULL)
  		return FALSE; 
	 else  
		return TRUE; 

} /* snd_is_bank_loaded() */

/***** END of snd_is_bank_loaded() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_report_banks()                                              */
/*                                                                           */
/* Walks the list of loaded banks and reports on the number, size and        */
/* contents of any loaded sound banks.                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Sep 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void snd_report_banks (void)

{ /* snd_report_banks() */

snd_bank_node_t *bp;         /* bank node ptr */

int num_banks;
int num_calls;
unsigned int total_size;
unsigned int bank_size;
float percentage;

/*****************************************************************************/

	fprintf (stderr, "\r\n");
	fprintf (stderr, "sound bank status:\r\n");
	fprintf (stderr, "\r\n");

	/* snd_bank_list is global */
	bp = snd_bank_list;

	if (bp == NULL)
		{
		fprintf (stderr, "no sound banks loaded\r\n");
		fprintf (stderr, "\r\n");
		return;
		}

	num_banks = 0;
	num_calls = 0;
	total_size = 0;

	fprintf (stderr, "bank name    size (bytes)  sound calls  start addr\r\n");
	fprintf (stderr, "-----------  ------------  -----------  ----------\r\n");

	while (bp != NULL)
		{
		/* this will be the size in _bytes_ */
		bank_size = bp->dram_end - bp->dram_start;

		/* keep running totals */
		total_size += bank_size;
		num_calls += bp->scall_count;
		num_banks++;

		/* print info */
		fprintf (stderr, "%-11s  %12d  %11d    0x%06X\r\n",
		         bp->name, bank_size, bp->scall_count, bp->dram_start);

		bp = bp->next;
		}

	percentage = ((float) total_size) / ((float) (SND_DRAM_SIZE - SND_TABLE_SIZE));
	percentage *= 100.0f;

	fprintf (stderr, "\r\n");

	fprintf (stderr, "%d banks, %d sound calls, %d bytes, %4.1f%% full\r\n", 
	         num_banks, num_calls, total_size, percentage);

	fprintf (stderr, "\r\n");

} /* snd_report_banks() */

/***** END of snd_report_banks() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_get_lockout()                                               */
/*                                                                           */
/* Reporst state of snd_lock flag. See also snd_lockout().                   */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 07 Apr 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int snd_get_lockout (void)

{ /* snd_get_lockout() */

   return (snd_lock);

} /* snd_get_lockout() */

/***** END of snd_get_lockout() **********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_get_bank_lockout()                                          */
/*                                                                           */
/* Reporst state of snd_lock_bank flag. See also snd_bank_lockout().         */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 07 Apr 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int snd_get_bank_lockout (void)

{ /* snd_get_bank_lockout() */

   return (snd_lock_bank);

} /* snd_get_bank_lockout() */

/***** END of snd_get_bank_lockout() *****************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_crc_check_enable()                                          */
/*                                                                           */
/* Enables CRC checking of files for sound library.                          */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 02 Jun 98 MJL                                                             */
/*                                                                           */
/*****************************************************************************/

void snd_crc_check_enable (void)

{ /* snd_crc_check_enable() */

   snd_use_crc = 1;

} /* snd_crc_check_enable() */

/***** END of snd_crc_check_enable() *****************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: snd_crc_fail_audit()                                            */
/*                                                                           */
/* Installs function to be called upon detection of CRC error in sound file  */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 02 Jun 98 MJL                                                             */
/*                                                                           */
/*****************************************************************************/

void snd_crc_fail_audit(void (*func)(void))

{ /* snd_crc_fail_audit() */

   snd_crc_fail_audit_func = func;

} /* snd_crc_fail_audit() */

/***** END of snd_crc_fail_audit() *******************************************/

/***** END of SOUND.C ********************************************************/

