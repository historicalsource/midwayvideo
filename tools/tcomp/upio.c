/*
 *		$Archive: /video/tools/buildrom/upio.c $
 *		$Revision: 4 $
 *		$Date: 10/06/97 5:31p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

/*
 *		SYSTEM INCLUDES
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 *		USER INCLUDES
 */

#include "upio.h"
#include "update.h"

/*
 *		DEFINES
 */

#define SCRIPT_PIECE_FILENAME		"UPL%02XN%02X.ROM"	/* script piece from individual ROM */
														/* UPLrrNnn.ROM r is 2 digits hex revision number */
														/* n is 2 digit hex rom number */
/*
 *		STATIC VARIABLES
 */

static uchar buffer[512*256];

/*
 *		GLOBAL FUNCTIONS
 */

int file_open(char *fname, int *fd)
{
	/* open the file in binary read/write mode */
//	*fd = open(fname, O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	*fd = open(fname, O_RDONLY | O_BINARY, S_IRUSR|S_IWUSR);
	/* check for error condition and return */
	return *fd < 0 ? UPDATE_FILE_OPEN_ERROR : UPDATE_NO_ERROR;
}  /* file_open */

int file_create(char *fname, int *fd)
{
	int err;
	
	/* create the file with read/write permissions */
	if (file_exist(fname)) {
		err = file_delete(fname);
		if (err != UPDATE_NO_ERROR)
			return err;
	}
	*fd = open(fname, O_RDWR | O_BINARY | O_CREAT, S_IRUSR | S_IWUSR);
	/* check for error condition and return */
	return *fd < 0 ? UPDATE_FILE_CREATE_ERROR : UPDATE_NO_ERROR;
}  /* file_create */

int file_close(int fd)
{
	/* close the file, check for error condition and return */
	return close(fd) != 0 ? UPDATE_FILE_CLOSE_ERROR : UPDATE_NO_ERROR;
}  /* file_close */

bool file_exist(char *fname)
{
	int fd;
	bool exist;
	
	/* try and open the file */
	if (file_open(fname, &fd) == UPDATE_NO_ERROR) {
		exist = TRUE;
		close(fd);
	} else
		exist = FALSE;
	
	/* return the file status */
	return exist;
}  /* file_exist */

int file_delete(char *fname)
{
	/* delete the file. check for error condition and return */
	return unlink(fname) != 0 ? UPDATE_FILE_DELETE_ERROR : UPDATE_NO_ERROR;
}  /* file_delete */


int file_rename(char *old_name, char *new_name)
{
#if 0
	/* rename the file, check for error condition and return */
	return rename(old_name, new_name) != 0 ? UPDATE_FILE_RENAME_ERROR : UPDATE_NO_ERROR;
#else
	int	err;
	
	err = file_copy(old_name, new_name);
	if (err == UPDATE_NO_ERROR)
		file_delete(old_name);
	return err;
#endif
}  /* file_rename */

int file_copy(char *old_name, char *new_name)
{
	int	old_fd;
	int	new_fd;
	int	num_bytes;
	int	byte_count;
	int	err;
	
	err = file_open(old_name, &old_fd);
	if (err == UPDATE_NO_ERROR) {
		err = file_create(new_name, &new_fd);
		if (err == UPDATE_NO_ERROR) {
			err = file_size(old_fd, NULL, &byte_count);
			if (err == UPDATE_NO_ERROR) {
				while (byte_count > 0) {
					/* determine the number of bytes to read */
					num_bytes = byte_count > sizeof(buffer) ? sizeof(buffer) : byte_count;

					/* bump down the number of remaing bytes to read */
					byte_count -= num_bytes;
									
					/* read a chunk in */
					err = file_read(old_fd, buffer, num_bytes);
					if (err == UPDATE_NO_ERROR) {
						/* write chunk out to script file */
						err = file_write(new_fd, buffer, num_bytes);
						if (err != UPDATE_NO_ERROR)
							break;
					} else
						break;
				}
			}
			file_close(new_fd);
		}
		file_close(old_fd);
	}
	return err;
}  /* file_copy */

int file_read(int fd, void *buffer, int read_count)
{
	int n_read;
	int err;
	
	/* check if any action to do */
	if (read_count == 0)
		return UPDATE_NO_ERROR;
#if 0
	/* validate the parameter */
	if (read_count % 4 != 0)
		return UPDATE_DISKIO_PARAM_ERROR;
#endif
	/* read the data into buffer */
	n_read = read(fd, buffer, read_count);
	/* check for error conditions */
	if (n_read == read_count)
		err = UPDATE_NO_ERROR;
	else if (n_read == 0)
		err = UPDATE_FILE_READ_EOF_ERROR;
	else
		err = UPDATE_FILE_READ_ERROR;
	return err;
}  /* file_read */

int file_write(int fd, void *buffer, int write_count)
{
	int n_write;
	int err;
	
	/* check if any action to do */
	if (write_count == 0)
		return UPDATE_NO_ERROR;
#if 0
	/* validate the parameter */
	if (write_count % 4 != 0)
		return UPDATE_DISKIO_PARAM_ERROR;
#endif
	/* write the data to disk */
	n_write = write(fd, buffer, write_count);
	/* check for error conditions */
	if (n_write == write_count)
		err = UPDATE_NO_ERROR;
	else if (n_write == 0)
		err = UPDATE_FILE_WRITE_EOF_ERROR;
	else
		err = UPDATE_FILE_WRITE_ERROR;
	return err;
}  /* file_write */

int file_set_pos(int fd, int offset, int whence)
{
	/* set the file pointer position, check for error conditions and return */
	return lseek(fd, offset, whence) < 0 ? UPDATE_FILE_SEEK_ERROR : UPDATE_NO_ERROR;
}  /* file_set_pos */

int file_get_pos(int fd, int *pos)
{
	/* get the file pointer position */
	*pos = lseek(fd, 0, SEEK_CUR);
	/* check for error condition and return */
	return *pos < 0 ? UPDATE_FILE_SEEK_ERROR : UPDATE_NO_ERROR;
}  /* file_get_pos */

int file_size(int fd, char *fname, int *f_size)
{
	int save_pos;
	int err, err2;
	bool close_file;
	
	/* check for using a filename or existing file descriptor */
	if (fname != NULL) {
		close_file = TRUE;
		err = file_open(fname, &fd);
	} else {
		close_file = FALSE;
		err = UPDATE_NO_ERROR;
	}
	
	if (err == UPDATE_NO_ERROR) {
		/* remember where we are in the file */
		err = file_get_pos(fd, &save_pos);
		
		if (err == UPDATE_NO_ERROR) {
			/* goto the end of the file */
			err = file_set_pos(fd, 0, SEEK_END);
			
			if (err == UPDATE_NO_ERROR) {
				/* get the file pointer, it will be the size of the file in bytes */
				err = file_get_pos(fd, f_size);
				
				/* reset the file pointer to the original position */
				err2 = file_set_pos(fd, save_pos, SEEK_SET);
				if (err == UPDATE_NO_ERROR)
					err = err2;
			}
		}
		/* close the file if it is one we opened */
		if (close_file)
			file_close(fd);
	}
	return err;
}  /* file_size */

int gzip_open(char *fname, gzFile *zfd)
{
	*zfd = gzopen(fname, "rb");
	return *zfd == NULL ? UPDATE_GZIP_OPEN_ERROR : UPDATE_NO_ERROR;
}  /* gzip_open */

int gzip_create(char *fname, gzFile *zfd)
{
	*zfd = gzopen(fname, "wb9");
	return *zfd == NULL ? UPDATE_GZIP_CREATE_ERROR : UPDATE_NO_ERROR;
}  /* gzip_create */

int gzip_close(gzFile zfd)
{
	return gzclose(zfd) != Z_OK ? UPDATE_GZIP_CLOSE_ERROR : UPDATE_NO_ERROR;
}  /* gzip_close */

int gzip_read(gzFile zfd, void *buffer, int read_count)
{
	int n_read;
	int err;
	
	/* check if any action to do */
	if (read_count == 0)
		return UPDATE_NO_ERROR;
	/* get the uncompressed data from the compressed file */
	n_read = gzread(zfd, buffer, read_count);
	/* check for error conditions */
	if (n_read <= 0) {
		err = n_read < 0 ? UPDATE_GZIP_READ_ERROR : UPDATE_GZIP_EOF_ERROR;
	} else {
		err = n_read == read_count ? UPDATE_NO_ERROR : UPDATE_GZIP_READ_ERROR;
	}
	return err;
}  /* gzip_read */

int gzip_write(gzFile zfd, void *buffer, int write_count)
{
	/* check if any action to do */
	if (write_count == 0)
		return UPDATE_NO_ERROR;
	return gzwrite(zfd, buffer, write_count) != write_count ? UPDATE_GZIP_WRITE_ERROR : UPDATE_NO_ERROR;
}  /* gzip_write */

ushort calculate_checksum(void *buffer, int count)
{
	uchar *b;
	ushort checksum;
	
	b = (uchar *)buffer;
	checksum = 0;
	/* calculate the sumation checksum */
	while (count--)
		checksum += *b++;
	return checksum;
}  /* calculate_checksum */

char *build_piece_filename(int rev_level, int rom_number)
{
	static char filename_buffer[13];						/* 8.3 plus nul */
	
	/* build the piece file UPLrrNnn.ROM rr is hex rev num, nn is rom num */
	sprintf(filename_buffer, SCRIPT_PIECE_FILENAME, rev_level & 0xFF, rom_number & 0xFF);
	return filename_buffer;
}  /* build_piece_filename */

/*
 *		$History: upio.c $
 * 
 * *****************  Version 4  *****************
 * User: Markg        Date: 10/06/97   Time: 5:31p
 * Updated in $/video/tools/buildrom
 * added copy_file command
 * 
 * *****************  Version 3  *****************
 * User: Mlynch       Date: 10/01/97   Time: 6:38p
 * Updated in $/video/tools/buildrom
 * Workarounds for JOE's stupid ass fucked up file system.  Increased
 * buffer sizes.
 * 
 * *****************  Version 2  *****************
 * User: Mlynch       Date: 10/01/97   Time: 9:52a
 * Updated in $/video/tools/buildrom
 * Assorted changes to deal with bullshit 4 byte padding of target
 * filesystem.
 */
