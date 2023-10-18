/***************************************************************************/
/*                                                                         */
/* SOUND.H                                                                 */
/*                                                                         */
/* Sound defines and function prototypes.                                  */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* by Matt Booty and Ed Keenan                                             */
/*                                                                         */
/* 07 Jan 97                                                               */
/*                                                                         */
/* MVB 04 Feb 97 - added functions for sample playback "engine" sounds     */
/* MVB 08 Jul 97 - added sound bank checksum routines, needs version       */
/*                 0118 of the sound system                                */
/*                                                                         */
/* $Revision: 10 $                                                          */
/*                                                                         */
/***************************************************************************/

#ifndef	__SOUND_H__
#define	__SOUND_H__

#ifdef VERSIONS
char	goose_sound_h_version[] = {"$Revision: 10 $"};
#endif

#ifndef	__dj_include_stdio_h_
#include	<stdio.h>
#endif

/***** TRACK DEFINES *******************************************************/

/* Most track commands can modify one or more tracks at once. */
/* So track numbers are specified using bit fields in a mask */
/* there are 6 tracks, numbered 0 through 5. OR them together */
/* to modify multiple tracks. */

#define SND_TRACK_0 0x01    /* bit 0 */
#define SND_TRACK_1 0x02    /* bit 1 */
#define SND_TRACK_2 0x04    /* bit 2 */
#define SND_TRACK_3 0x08    /* bit 3 */
#define SND_TRACK_4 0x10    /* bit 4 */    
#define SND_TRACK_5 0x20    /* bit 5 */

/***** RETURN INFO DEFINES *************************************************/

/* When the sound system sends back return information, the */
/* upper byte of the status register indicates the category */

#define SND_INFO_SIGNAL     0x0100
#define SND_INFO_TRACKS     0x0200
#define SND_INFO_CHECKSUM   0x0300
#define SND_INFO_SYNC       0x0400
#define SND_INFO_STREAM     0x0500
#define SND_INFO_REALTIME   0x8000
#define SND_INFO_ERROR      0x1900
#define SND_INFO_NOCALL     0xEEEE
#define SND_ERROR_MASK      0x00007F00
#define SND_INFO_MASK       0x00000700


/***** SOUND CALL PRIORITY DEFINES *****************************************/

/* These get OR'ed together to create the 16-bit priority word.    */
/* The default value is zero, which is:                            */
/* - dynamically allocate tracks                                   */
/* - if explicitly setting track #, override any priority          */
/* - tracks with equal priority interrupt each other               */
/* See the sound system docs for more details.                     */

#define SND_PRI_SET_NOINT   0xE000      // hard set to trk, check pri of set trk, and = pri no int.

#define SND_PRI_SET        0x8000      /* explicitly set the track number */
#define SND_PRI_TRACK0     (0 << 7)    /* force call to track 0 */    
#define SND_PRI_TRACK1     (1 << 7)    /* force call to track 1 */
#define SND_PRI_TRACK2     (2 << 7)    /* force call to track 2 */
#define SND_PRI_TRACK3     (3 << 7)    /* force call to track 3 */ 
#define SND_PRI_TRACK4     (4 << 7)    /* force call to track 4 */ 
#define SND_PRI_TRACK5     (5 << 7)    /* force call to track 5 */ 

/***** VOLUME ADJUSTMENT DEFINES *******************************************/

#define SND_VOLUME_MAX 255
#define SND_VOLUME_MIN 0

/***** ENGINE SAMPLES ******************************************************/

/* first three samples are for player sound */
/* fourth sample is for drone sounds */

#define SND_MAX_SAMPLES 4           

/* "engine" sound sample playback cmds */
/* needed by external functions */
#define SND_CMD_DRONE1_CTRL      0x55E3  /* params for drone snd #1 */
#define SND_CMD_DRONE2_CTRL      0x55E4  /* params for drone snd #2 */
#define SND_CMD_PLAYER_CTRL      0x55E0  /* params for player sound */


/***** NEW OPSYS FILE LOAD LISTS ********************************************/

enum {
	SND_OPSYS_0122,			/* latest non-FIFO version */
	SND_OPSYS_0223			/* version with FIFO streaming */
	};

/***** STREAMING COMMANDS ***************************************************/

#define SND_CMD_STREAM_START   0x55D6   /* start playing streaming data */
#define SND_CMD_STREAM_STOP    0x55D7   /* stop playing streaming data */
#define SND_CMD_STREAM_VOLUME  0x55B3   /* set streaming 'track' volume */
#define SND_CMD_STREAM_FLUSH   0x55D8   /* flush all streaming buffers */
#define SND_CMD_STREAM_REQFREE 0x55D9   /* get # of empty buffers */
#define SND_CMD_STREAM_SENDBUF 0x55DA   /* start buffer xfer protocol */
#define SND_CMD_DISABLE_QUEUE  0x55DB   /* disable return info for normal calls */
#define SND_CMD_ENABLE_QUEUE   0x55DC   /* enable return info for normal calls */
#define SND_CMD_FIFO_ENABLE	   0x55E7	/* tell sound DSP to read FIFO's */	

/***** FUNCTION PROTOTYPES *************************************************/

/* low-level stuff */
int snd_init (void);
void snd_reset (void);                                         
int snd_reset_ack (void);
int snd_get_data (unsigned int *data);
int snd_send_data (unsigned int data);
int snd_send_command (unsigned int command);
void snd_delay (int delay_time);
unsigned int snd_wait_for_completion (void);
int snd_send_data_realtime (unsigned int data);

/* simple diagnostic tests */
void snd_dbg_boot_info (void);
unsigned int snd_get_boot_version (void);
unsigned int snd_get_sdrc_version (void);
unsigned int snd_get_pmint_sum (void);
int snd_test_port (void);

/* op sys loading stuff */
int snd_load_opsys (void);
int snd_load_dm (char *file_name, int start, int end);
int snd_load_pm (char *file_name, int start, int end);
unsigned int *snd_load_file (char *file_name);
unsigned int snd_get_opsys_ver (void);

/* bank and sound table loading */
int snd_init_scall_table (void);
void snd_clear (void);
int snd_download_bank (unsigned int *bank_data, int start, int end);
int snd_download_bank_playing (unsigned int *bank_data, int start, int end);
int snd_download_table (unsigned int *table_data, int start, int end);
int snd_download_table_playing (unsigned int *table_data, int start, int end);
int snd_load_bank (char *bank_name, int *num_sound_calls);
int snd_load_bank_playing (char *bank_name, int *num_sound_calls);

/* sending sound calls */
int snd_master_volume (int volume);
int snd_stop_all (void);
int snd_scall_quick (unsigned int scall);
int snd_scall_direct (unsigned int scall, unsigned int volume, 
           unsigned int pan, unsigned int priority);
int snd_output_mono (void);

int snd_output_stereo (void);
int snd_flush_pending (void);
int snd_flush_queues (void);
int snd_set_reserved (unsigned int track_mask);
int snd_stop_track (unsigned int track_mask);
int snd_track_volume (unsigned int track_mask, unsigned int track_volume);
int snd_track_pan (unsigned int track_mask, unsigned int track_pan);

/* bank management stuff */
void snd_bank_init (void);
int snd_bank_load (char *bank_name);
int snd_bank_load_playing (char *bank_name);
int snd_bank_delete (char * bank_name);

int snd_scall_bank (char *bank_name, unsigned int scall, 
         unsigned int volume, unsigned int pan, 
         unsigned int priority);

void snd_bank_showlist (void);

int snd_scall_return (char *bank_name, unsigned int scall, 
           unsigned int volume, unsigned int pan, 
           unsigned int priority, unsigned int *return_info);

void snd_bank_lockout (int mode);
void snd_lockout (int mode);
void snd_report_banks (void);
int snd_get_lockout (void);
int snd_get_bank_lockout (void);


/* engine sample stuff */
int snd_load_samples (char **sample_list, int num_samples);
int snd_scall_engine (unsigned int cmd, unsigned int pitch, 
           unsigned int volume, unsigned int pan);
int snd_stop_engine (unsigned int engine_select);

/* streaming stuff */
void snd_clear_latch (void);
int snd_query_buffers (int *buffers_available);
int snd_stream_volume (int volume);
int snd_stream_init (char *stream_name, int *num_buffers);
int snd_read_buffer (unsigned int *buffer, int frame_size, FILE *);
int snd_send_buffer (unsigned int *buffer, int frame_size);
int snd_stream (int buffer_total);
int snd_stream_stop (void);
void snd_stream_proc (int *args);

/* D/RAM and bank checksum functions */
int snd_checksum_dram (int start_addr, int end_addr);
int snd_checksum_banks (char *first_bank_name, char *last_bank_name);

/* bank parsing stuff */
int snd_get_first_sound (char *bank_name);
int snd_get_last_sound (char *bank_name);
int snd_is_bank_loaded (char *bank_name);

/* new list-based opsys loader */
int snd_init_multi (int opsys_type);
int snd_load_opsys_multi (int opsys_type);

/* Stuff for CRC checking sound files */
void snd_crc_check_enable (void);
void snd_crc_fail_audit(void (*func)(void));

short get_snd_status(void);
short get_snd_data(void);
void set_snd_data(short);

/***** END of SOUND.H ******************************************************/

#endif
