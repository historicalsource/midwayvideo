#ifndef _DCS2DEF_H_
#define _DCS2DEF_H_

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    DCS2DEF.H                                                         บ
บ Author:  Scott Patterson                                                   บ
บ Created: 29-Aug-1996                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ     DCS2 Sound System Defines                                              บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  16Sep96 JVM  Add #ifdef around contents of this module. Add #ifdef        บ
บ               around definitions of TRUE and FALSE in case they are        บ
บ               already defined.                                             บ
บ  29Aug96 SSP Created based on MVB code and documentation                   บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/

/*
**  DCS2 Driver Operating Modes
*/
#define DCS2_MODE_DIAG  1
#define DCS2_MODE_GAME  2


// ---------------------------Sound Code Masks and IDs

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define OK    0
#define ERROR 1

#define BYTE0 0x000000FF

#define BYTE1 0x0000FF00

#define BYTE2 0x00FF0000

#define BYTE3 0xFF000000

#define MASK8  0x000000FF
#define MASK16 0x0000FFFF
#define MASK32 0xFFFFFFFF
#define TOP16 0xFFFF0000

#define TI320CX_MODE_ENABLE 0x8000

#define DCS2_DSP_RESET      0xFFFE
#define DCS2_DSP_ENABLE     0x0001

#define STH_DATA_RDY        0x0040

#define HTS_DATA_EMPTY      0x0080

#define SND_LOAD_TYPE_PM    0x00000000

#define SND_LOAD_TYPE_DM    0x00000001

#define OPSYS_READY         0x000C

// ---------------------------Sound Code Defines

#define SOUND_CALL_TABLE_SIZE 0x4000

#define MAXIMUM_VOLUME 255

// ---------------------------Sound Boot Rom Commands

#define SOUND_BOOT_ROM_READY_SIGNAL				0x000A
#define SOUND_BOOT_ROM_LOAD						0x001A
#define SOUND_BOOT_ROM_RUN						0x002A
#define SOUND_BOOT_ROM_SRAM_TEST				0x003A
#define SOUND_BOOT_ROM_DRAM_BANK0_TEST			0x004A
#define SOUND_BOOT_ROM_DRAM_BANK1_TEST			0x005A
#define SOUND_BOOT_ROM_VERSION_NUMBER			0x006A
#define SOUND_BOOT_ROM_ASIC_VERSION_NUMBER		0x007A
#define SOUND_BOOT_ROM_ECHO_COMPLEMENT			0x008A
#define SOUND_BOOT_ROM_INT_PRAM_CHECKSUM		0x009A
#define SOUND_BOOT_ROM_BONG_1_TIME_COMMAND		0x00AA
#define SOUND_BOOT_ROM_START_1KHZ_TEST_TONE		0x00BA
#define SOUND_BOOT_ROM_STOP_1KHZ_TEST_TONE		0x00CA

// ---------------------------Sound Boot Rom Return Codes

#define SOUND_BOOT_ROM_RETURN_SRAM_PASSED		0xCC01
#define SOUND_BOOT_ROM_RETURN_DRAM_BANK0_PASSED	0xCC02
#define SOUND_BOOT_ROM_RETURN_DRAM_BANK1_PASSED	0xCC03
#define SOUND_BOOT_ROM_RETURN_BONG_FINISHED		0xCC04
#define SOUND_BOOT_ROM_RETURN_TONE_STARTED		0xCC05
#define SOUND_BOOT_ROM_RETURN_TONE_ENDED		0xCC06

#define SOUND_BOOT_ROM_RETURN_SRAM1_FAILED		0xEE01
#define SOUND_BOOT_ROM_RETURN_SRAM2_FAILED		0xEE01
#define SOUND_BOOT_ROM_RETURN_SRAM3_FAILED		0xEE01
#define SOUND_BOOT_ROM_RETURN_DRAM_BANK0_FAILED	0xEE01
#define SOUND_BOOT_ROM_RETURN_DRAM_BANK1_FAILED	0xEE01

// ---------------------------Sound Operating System Commands

// ---------------------------Single Word Sound Calls

#define SOUND_CALL_FLUSH_ALL_PENDING			0x55D2		// flush all pending sound calls
#define SOUND_CALL_FLUSH_INPUT_AND_OUTPUT		0x55D3		// flush input and output queues
#define SOUND_CALL_RETURN_VERSION				0x55D4		// version returned as 0xMMNN (0x0108 is 1.8)

// ---------------------------Double Word Sound Calls

#define SOUND_CALL_UPDATE_MASTER_VOLUME			0x55AA		// Second Word: [M_VOL  | ~M_VOL]
#define SOUND_CALL_UPDATE_TRACK_VOLUME			0x55AB		// Second Word: [T_MASK | T_VOL]
#define SOUND_CALL_UPDATE_TRACK_PAN				0x55AC		// Second Word: [T_MASK | T_PAN]
#define SOUND_CALL_UPDATE_TRACK_PRIORITY		0x55AD		// Second Word: [T_MASK | T_PRIOR]
#define SOUND_CALL_STOP_TRACKS					0x55AE		// Second Word: [T_MASK | JUNK]
#define SOUND_CALL_SET_RESERVED_TRACK_MASK		0x55AF		// Second Word: [T_MASK | JUNK]
#define SOUND_CALL_SYNCHRONIZATION_WORD			0x55B0		// Second Word: [VALUE]
#define SOUND_CALL_MONO_OUTPUT_MODE				0x55B1		// Second Word: [0xAA55]
#define SOUND_CALL_STEREO_OUTPUT_MODE			0x55B2		// Second Word: [0xBB44]

// M_VOL	MASTER VOLUME: 8-bit, 0 is off, 1 is softest, 255 is loudest
// T_VOL    TRACK  VOLUME: 8-bit, 0 is off, 1 is softest, 255 is loudest
// T_PAN    TRACK  PAN   : 8-bit, 0 is left, 127 is center, 255 is right
// T_PRIOR  TRACK  PRIORITY: 0 is highest, 127 is lowest
// T_MASK   TRACK  MASK: set one or more bits 0 thru 5 to affect corresponding tracks 0 thru 5
// JUNK     DON'T CARE bits

// ---------------------------Triple Word Sound Calls

#define SOUND_CALL_STOP_ALL_SOUND				0x0000		// Second Word: [0x0000] Third Word: [0x0000]

// Normal Sound Calls are like this:
//
// First Word: [SOUND_NUMBER] Second Word: [T_VOL | T_PAN] Third Word: [PRIORITY_WORD]
// 
// SOUND_NUMBER is 0x0001 thru 0x55A9

// PRIORITY WORD:
//
// BIT 15   1 = EXPLICITLY SET TRACK NUMBER, 0 = DYNAMICALLY ALLOCATE TRACK NUMBER
//
// BIT 14	1 = CHECK AND CONFORM TO PRIORITY WHEN EXPLICITLY SETTING TRACK NUMBER
//			0 = OVERRIDE PRIORITY WHEN EXPLICITLY SETTING TRACK NUMBER
//
// BIT 13	1 = TRACKS DO NOT GET INTERRUPTED BY INCOMING CALLS WITH EQUAL PRIORITY
//			0 = TRACKS ARE INTERRUPTED BY INCOMING CALLS WITH EQUAL PRIORITY
//
// BITS 12,11,10		NOT USED
// BITS 9,8,7			IF BIT 15 SET, THESE BITS DETERMINE TRACK NUMBER 0 THRU 5
// BITS 6,5,4,3,2,1,0	PRIORITY, 0 is the highest, 127 is the lowest

// Sound Call Return Information:
//
// Return Decription			3-bit STATUS			16-bit DATA
//
// Playlist Signal				0x01					generic value from playlist signal
// Allocation Info				0x02					TRACK_USED | TRACK_INTERRUPTED
// Download Checksum			0x03					16-bit DRAM LOAD CHECKSUM
// Allocation Error				0x04					0xEEEE
// not used						0x05
// not used						0x06
// System Error                 0x07                    not yet defined
//
#define SET_TRACK_NUMBER    0x8000
#define CHECK_PRIORITY      0x4000
#define DO_NOT_INTERRUPT    0x2000

// ---------------------------Five Word Sound Calls

#define SOUND_CALL_START_IMMEDIATE_DOWNLOAD		0x55D0		// no sounds can play during this download
//
// Second Word: [ 0x00                         | 24BIT_START_ADDRESS_HIGH_BYTE]
// Third Word:  [ 24BIT_START_ADDRESS_MID_BYTE | 24BIT_START_ADDRESS_LOW_BYTE ]
// Fourth Word: [ 0x00                         | 24BIT_END_ADDRESS_HIGH_BYTE  ]
// Fifth Word:  [ 24BIT_END_ADDRESS_MID_BYTE   | 24BIT_END_ADDRESS_LOW_BYTE   ]
//
// Returns a STATUS: 0x03 DATA: 16-bit checksum

#define SOUND_CALL_START_DOWNLOAD_WHILE_PLAYING	0x55D1		// download while sounds are playing
//
// Second Word: [ 0x00                         | 24BIT_START_ADDRESS_HIGH_BYTE]
// Third Word:  [ 24BIT_START_ADDRESS_MID_BYTE | 24BIT_START_ADDRESS_LOW_BYTE ]
// Fourth Word: [ 0x00                         | 24BIT_END_ADDRESS_HIGH_BYTE  ]
// Fifth Word:  [ 24BIT_END_ADDRESS_MID_BYTE   | 24BIT_END_ADDRESS_LOW_BYTE   ]
//
// Returns a STATUS: 0x03 DATA: 16-bit checksum

#define SOUND_CALL_RETURN_CHECKSUM				0x55D5		// returns a checksum of a specified SRAM range 
//
// Second Word: [ 0x00                         | 24BIT_START_ADDRESS_HIGH_BYTE]
// Third Word:  [ 24BIT_START_ADDRESS_MID_BYTE | 24BIT_START_ADDRESS_LOW_BYTE ]
// Fourth Word: [ 0x00                         | 24BIT_END_ADDRESS_HIGH_BYTE  ]
// Fifth Word:  [ 24BIT_END_ADDRESS_MID_BYTE   | 24BIT_END_ADDRESS_LOW_BYTE   ]
//
// Returns a STATUS: 0x03 DATA: 16-bit checksum

#endif
