/*
 *		$Archive: /video/tools/buildrom/upio.h $
 *		$Revision: 4 $
 *		$Date: 10/06/97 5:31p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __UPIO_H__
#define __UPIO_H__

/*
 *		USER INCLUDES
 */

#ifndef __TYPES_H__
#include "types.h"
#endif
#include "zlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *		DEFINES
 */

#define ROUND_UP_TO_4(x)		(((x) + 3) & ~3)

/*
 *		GLOBAL PROTOTYPES
 */

int file_open(char *fname, int *fd);
int file_create(char *fname, int *fd);
int file_close(int fd);
bool file_exist(char *fname);
int file_delete(char *fname);
int file_rename(char *old_name, char *new_name);
int file_copy(char *old_name, char *new_name);
int file_read(int fd, void *buffer, int read_count);
int file_write(int fd, void *buffer, int write_count);
int file_set_pos(int fd, int offset, int whence);
int file_get_pos(int fd, int *pos);
int file_size(int fd, char *fname, int *f_size);
int gzip_open(char *fname, gzFile *zfd);
int gzip_create(char *fname, gzFile *zfd);
int gzip_close(gzFile zfd);
int gzip_read(gzFile zfd, void *buffer, int read_count);
int gzip_write(gzFile zfd, void *buffer, int write_count);
ushort calculate_checksum(void *buffer, int count);
char *build_piece_filename(int rev_level, int rom_number);

#ifdef __cplusplus
}
#endif

/*
 *		$History: upio.h $
 * 
 * *****************  Version 4  *****************
 * User: Markg        Date: 10/06/97   Time: 5:31p
 * Updated in $/video/tools/buildrom
 * added copy_file command
 * 
 * *****************  Version 3  *****************
 * User: Markg        Date: 10/01/97   Time: 10:39a
 * Updated in $/video/tools/buildrom
 * Changed the name of a macro.
 * 
 * *****************  Version 2  *****************
 * User: Mlynch       Date: 10/01/97   Time: 9:52a
 * Updated in $/video/tools/buildrom
 * Assorted changes to deal with bullshit 4 byte padding of target
 * filesystem.
 */

#endif
