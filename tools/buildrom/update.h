/*
 *		$Archive: /video/tools/buildrom/update.h $
 *		$Revision: 3 $
 *		$Date: 10/06/97 5:31p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __UPDATE_H__
#define __UPDATE_H__

/*
 *		USER INCLUDES
 */

#ifndef __TYPES_H__
#include "types.h"
#endif

/*
 *		DEFINES
 */

#define HEADER_SIG					0x55545250			/* expected signature in header block */
#define TRAILER_SIG					0x47434D00			/* expected signature in trailer block */
#define TRAILER_SIG_MASK			0xFFFFFF00			/* mask off the checksum adjust byte */

#define UPDATE_NO_ERROR						0			/* operation completed successfully */
														/* target hard disk related errors */
#define UPDATE_FILE_OPEN_ERROR				(-1)		/* error opening a target hard disk file */
#define UPDATE_FILE_CREATE_ERROR			(-2)		/* error creating a target hard disk file */
#define UPDATE_FILE_CLOSE_ERROR				(-3)		/* error closing a target hard disk file */
#define UPDATE_FILE_DELETE_ERROR			(-4)		/* error deleting a target hard disk file */
#define UPDATE_FILE_RENAME_ERROR			(-5)		/* error renaming a target hard disk file */
#define UPDATE_FILE_READ_EOF_ERROR			(-6)		/* got EOF reading from a target hard disk file */
#define UPDATE_FILE_READ_ERROR				(-7)		/* error reading from a target hard disk file */
#define UPDATE_FILE_WRITE_EOF_ERROR			(-8)		/* got EOF writing to a target hard disk file */
#define UPDATE_FILE_WRITE_ERROR				(-9)		/* error writing to a target hard disk file */
#define UPDATE_FILE_SEEK_ERROR				(-10)		/* error seeking in a target hard disk file */
														/* EPROM related errors */
#define UPDATE_EPROM_NOT_PRESENT_ERROR		(-11)		/* EPROM not in socket */
#define UPDATE_EPROM_READ_ERROR				(-12)		/* error reading from EPROM address space */
#define UPDATE_EPROM_CHECKSUM_ERROR			(-13)		/* bad EPROM checksum */
														/* */
#define UPDATE_WRONG_GAME_ERROR				(-14)		/* update EPROM is for different game */
#define UPDATE_WRONG_REVISON_LEVEL_ERROR	(-15)		/* update EPROM is for differnet revision of game */
														/* */
#define UPDATE_UP_TO_DATE					(-16)		/* update EPROM found, but game is at that revision */
#define UPDATE_ROM_ALREADY_PROCESSED		(-17)		/* update EPROM piece already loaded */

#define UPDATE_SCRIPTFILE_CREATE_ERROR		(-18)		/* error creating the concated script file */

#define UPDATE_MORE_ROMS_NEEDED				(-19)		/* more ROMs need to complete this patch kit */
#define UPDATE_HAVE_ALL_PIECES				(-20)		/* got all the data, time to update the disk */

#define UPDATE_GZIP_OPEN_ERROR				(-21)		/* error opening gzip file */
#define UPDATE_GZIP_CREATE_ERROR			(-22)		/* error creating a gzip file */
#define UPDATE_GZIP_CLOSE_ERROR				(-23)		/* error closing gzip file */
#define UPDATE_GZIP_READ_ERROR				(-24)		/* error reading from gzip file */
#define UPDATE_GZIP_WRITE_ERROR				(-25)		/* error writing to gzip file */
#define UPDATE_GZIP_EOF_ERROR				(-26)		/* got EOF reading from gzip script file */
#define UPDATE_GZIP_BUFFER_SMALL_ERROR		(-27)		/* a buffer was too small to read data into */
#define UPDATE_GZIP_UNKNOWN_OPCODE			(-28)		/* an unknown opcode found in script file */
#define UPDATE_PATCH_UNKNOWN_OPCODE			(-29)		/* an unknown opcode found in patch script */
#define UPDATE_MALLOC_ERROR					(-30)		/* error mallocing a block of memory */
#define UPDATE_DISKIO_PARAM_ERROR			(-31)		/* the read/write count must be mult of 4 */

enum {									/* update script opcodes */
	EXEC_FILE,							/* execute the name file */
	NEW_FILE,							/* extract a new file to hard disk */
	DELETE_FILE,						/* delete the named file from the hard disk */
	RENAME_FILE,						/* rename the file on hard disk */
	PATCH_FILE,							/* patch the specified file on hard disk */
	END_SCRIPT,							/* op code to signify the end of the script file */
	COPY_FILE							/* copy a file to a new filename */
};

enum {
	COPY_CHUNK,							/* copy data from the original file(file_offset, copy_count) */
	NEW_CHUNK,							/* copy data from the patch file(copy_count, data) */
	END_PATCH							/* end the path setinel */
};
/*
 *		TYPEDEFS
 */

typedef struct {						/* ROM set description  */
	ushort game_id;						/* game id, 0 = NFL Blitz, 1 = Space Race, etc */
	ushort rev_level;					/* game revision level, L1, L2, etc */
} revision_id;

typedef uchar revision_desc[28];		/* text string describing revision level, not nul terminated */

typedef struct {						/* first 64 bytes in every ROM */
	uint signature;						/*  4 HEADER_SIG used to id the address space as an update ROM */
	int rom_size;						/*  4 this ROM size in bytes: 512K or 1024K */
	ushort checksum_16;					/*  2 checksum for this ROM, low byte is the rev level/rom num */
	ushort inv_checksum_16;				/*  2 bitwise inverse of the checksum */
	revision_desc rev_name;				/* 28 this revision level description */
	revision_id required_level;			/*  4 current revision level required to be present to update from */
	revision_id new_level;				/*  4 this revision level id */
	ushort rom_count;					/*  2 number of ROMs in this revision patch set */
	ushort rom_number;					/*  2 this ROM number in the set, 1 based */
	int flags;							/*  4 update flags, not currently used */
	int unused1;						/*  8 unused space for future expansion space */
	int unused2;
} rom_header;

typedef union {							/* last 4 bytes in every ROM */
	struct {							/* data viewed as a group of bytes */
		uchar checksum_adjust;			/* a checksum tweak byte, used to encode the rev level and rom num */
		uchar signature[3];				/* TRAILER_SIG used to id the address space as an update ROM */
	} s;
	int i;								/* data viewed as a long */
} rom_trailer;

#ifdef __cplusplus
extern "C" {
#endif

/*
 *		GLOBAL PROTOTYPES
 */
int update_rom_present(void);
int update_process_rom(void);

#ifdef __cplusplus
}
#endif

/*
 *		$History: update.h $
 * 
 * *****************  Version 3  *****************
 * User: Markg        Date: 10/06/97   Time: 5:31p
 * Updated in $/video/tools/buildrom
 * added copy_file command
 * 
 * *****************  Version 2  *****************
 * User: Mlynch       Date: 10/01/97   Time: 9:52a
 * Updated in $/video/tools/buildrom
 * Assorted changes to deal with bullshit 4 byte padding of target
 * filesystem.
 */

#endif
