/*
 *		$Archive: /video/tools/dumprom/dumprom.c $
 *		$Revision: 1 $
 *		$Date: 10/13/97 4:22p $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "update.h"
#include "upio.h"

/*
 *		DEFINES
 */

#define UPDATE_SCRIPT_FILENAME		"UPD.DAT"			/* concatenated compressed update script */

/*
 *		STATIC PROTOTYPES
 */

static void print_script_header_info(rom_header *header);
static void print_header(rom_header *header, char *fname);
static void print_trailer(rom_trailer *trailer, char *fname);
static int file_read_header(int fd, rom_header *header);
static int file_read_trailer(int fd, int rom_size, rom_trailer *trailer);
static int file_validate_data(int fd, char *fname);
static bool check_error(int err, char *doing, char *func);
static int gzip_read_fname(gzFile zfd, void *buffer, int max_size);
static bool piece_exist(int rev_level, int rom_number);
static void check_piece_set(int rev_level, int rom_count, int *num_pieces_still_needed, int *next_rom_needed);
static int build_script_file(rom_header *curr_header);
static int gzip_skip_bytes(gzFile gzip_file, int num_bytes);
static int process_script_file(void);

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	rom_header header;
	char *fname;
	int fd;
	int rev_level;
	int rom_count;
	int num_missing;
	int next;
	int err;
	
	if (argc != 2) {
		fprintf(stderr, "Usage:%s rev_level\n", argv[0]);
		err = 1;
	} else {
		/* get the update revision level */
		rev_level = atoi(argv[1]);
		
		/* build a file name for the first ROM image in the update set */
		fname = build_piece_filename(rev_level, 1);
		
		/* open the first ROM file of the set */
		err = file_open(fname, &fd);
		if (check_error(err, "opening ROM file", "main"))
			return EXIT_FAILURE;
		
		/* read the header information */
		err = file_read_header(fd, &header);
		if (check_error(err, "reading ROM file header", "main")) {
			err = file_close(fd);
			return EXIT_FAILURE;
		}
		
		/* close the first ROM image */
		err = file_close(fd);
		if (check_error(err, "closing ROM file", "main"))
			return EXIT_FAILURE;
		
		/* get the number of ROMs in this update set */
		rom_count = header.rom_number;
		
		/* check to see if all of the ROM set is present */
		printf("Checking ROM set\n");
		check_piece_set(rev_level, rom_count, &num_missing, &next);
		if (num_missing != 0) {
			printf("Incomplete ROM set, missing %d ROM images", num_missing);
			return EXIT_FAILURE;
		}
		
		/* create the gzipped script file */
		printf("Building script file\n");
		err = build_script_file(&header);
		if (check_error(err, "build_script_file", "main"))
			return EXIT_FAILURE;
		printf("Dumping script file\n");
		print_script_header_info(&header);
		err = process_script_file();
		if (check_error(err, "process_script_file", "main"))
			return EXIT_FAILURE;
		/* delete the temp script file */
		err = file_delete(UPDATE_SCRIPT_FILENAME);
	}
	return err == UPDATE_NO_ERROR ? EXIT_SUCCESS : EXIT_FAILURE;
}  /* main */

/*
 *		STATIC PROTOTYPES
 */

static void print_script_header_info(rom_header *header)
{
	char rev[29];
	char *str;
	
	printf("old_revision_directory \"old\";\n");
	printf("new_revision_directory \"new\";\n");
	if (header->rom_size == 1024L * 1024L)
		str = "eight_mb";
	else if (header->rom_size == 512L * 1024L)
		str = "four_mb";
	else
		str = "INVALID";
	printf("eprom_size %s;\n", str);
	strncpy(rev, header->rev_name, 28);
	rev[28] = '\0';
	printf("revision_name \"%s\";\n", rev);
	printf("old_revision_level %d;\n", header->required_level.rev_level);
	printf("new_revision_level %d;\n", header->new_level.rev_level);
	printf("game_id %d;\n", header->required_level.game_id);
}  /* print_script_header_info */

static void print_header(rom_header *header, char *fname)
{
	char *str;
	
	printf("ROM header from file \"%s\"\n", fname);
	printf("signature is %s\n", header->signature == HEADER_SIG ? "valid" : "INVALID");
	if (header->rom_size == 1024L * 1024L)
		str = "1024K";
	else if (header->rom_size == 512L * 1024L)
		str = "512K";
	else
		str = "INVALID";
	printf("rom_size is %s\n", str);
	printf("checksum_16 = 0x%04hX\n", header->checksum_16);
	printf("inv_checksum_16 = 0x%04hX\n", header->inv_checksum_16);
	printf("rev_name = %28s\n", header->rev_name);
	printf("game_id = %hu\n", header->required_level.game_id);
	printf("required_level = %hu\n", header->required_level.rev_level);
	printf("new_level = %hu\n", header->new_level.rev_level);
	printf("rom_count = %hu\n", header->rom_count);
	printf("rom_number = %hu\n", header->rom_number);
	printf("\n");
}  /* print_header */

static void print_trailer(rom_trailer *trailer, char *fname)
{
	printf("ROM trailer from file \"%s\"\n", fname);
	printf("signature is %s\n", (trailer->i & TRAILER_SIG_MASK) == TRAILER_SIG ? "valid" : "INVALID");
	printf("\n");
}  /* print_trailer */

static int file_read_header(int fd, rom_header *header)
{
	int save_pos;
	int err, err2;
	
	/* save the file pointer position */
	err = file_get_pos(fd, &save_pos);
	if (err != UPDATE_NO_ERROR)
		return err;
	
	/* set file pointer to the beginning of the file */
	err = file_set_pos(fd, 0, SEEK_SET);
	if (err != UPDATE_NO_ERROR)
		return err;
	
	/* read the header */
	err = file_read(fd, header, sizeof(rom_header));
	if (err == UPDATE_NO_ERROR) {
		/* check for a valid signature */
		if (header->signature != HEADER_SIG)
			err = UPDATE_EPROM_NOT_PRESENT_ERROR;
	}
	/* reset the file pointer */
	err2 = file_set_pos(fd, save_pos, SEEK_SET);
	if (err == UPDATE_NO_ERROR)
		err = err2;
	return err;
}  /* file_read_header */

static int file_read_trailer(int fd, int rom_size, rom_trailer *trailer)
{
	int save_pos;
	int err, err2;
	
	/* save the file pointer position */
	err = file_get_pos(fd, &save_pos);
	if (err != UPDATE_NO_ERROR)
		return err;
	
	/* set the file pointer to the end of file before the trailer */
	err = file_set_pos(fd, rom_size - sizeof(rom_trailer), SEEK_SET);
	if (err != UPDATE_NO_ERROR)
		return err;
	
	/* read the trailer */
	err = file_read(fd, trailer, sizeof(rom_trailer));
	if (err == UPDATE_NO_ERROR) {
		/* check for a valid signature */
		if ((trailer->i & TRAILER_SIG_MASK) != TRAILER_SIG)
			err = UPDATE_EPROM_NOT_PRESENT_ERROR;
	}
	/* reset the file pointer */
	err2 = file_set_pos(fd, save_pos, SEEK_SET);
	if (err == UPDATE_NO_ERROR)
		err = err2;
	
	return err;
}  /* file_read_trailer */

static int file_validate_data(int fd, char *fname)
{
	uchar buffer[1024];
	rom_header h;
	rom_trailer t;
	int save_pos;
	int err, err2;
	int count, num_bytes;
	bool close_file;
	ushort checksum;
	
	if (fname != NULL) {
		close_file = TRUE;
		err = file_open(fname, &fd);
	} else {
		close_file = FALSE;
		err = UPDATE_NO_ERROR;
	}
	
	/* check for using a filename or existing file descriptor */
	if (err == UPDATE_NO_ERROR) {
		/* get the header */
		err = file_read_header(fd, &h);
		if (err == UPDATE_NO_ERROR) {
			print_header(&h, fname);
			/* get the trailer */
			err = file_read_trailer(fd, h.rom_size, &t);
			if (err == UPDATE_NO_ERROR) {
				print_trailer(&t, fname);
				/* save the file pos */
				err = file_get_pos(fd, &save_pos);
				if (err == UPDATE_NO_ERROR) {
					/* start at the begining of the file */
					err = file_set_pos(fd, 0, SEEK_SET);
					if (err == UPDATE_NO_ERROR) {
						/* the number of bytes to read and checksum */
						count = h.rom_size;
						/* init the checksum */
						checksum = 0;
						while (count > 0) {
							/* determine the number of bytes to read */
							num_bytes = count > sizeof(buffer) ? sizeof(buffer) : count;
							/* update the number of bytes left to process */
							count -= num_bytes;
							
							/* read a chunk in */
							err = file_read(fd, buffer, num_bytes);
							if (err != UPDATE_NO_ERROR)
								break;
							
							/* calc the checksum on chunk */
							checksum += calculate_checksum(buffer, num_bytes);
						}
						/* verify the checksum */
						if (err == UPDATE_NO_ERROR && ((checksum != h.checksum_16) || ((ushort)~checksum != h.inv_checksum_16)))
							err = UPDATE_EPROM_CHECKSUM_ERROR;
						/* reset the file pos */
						err2 = file_set_pos(fd, save_pos, SEEK_SET);
						if (err == UPDATE_NO_ERROR)
							err = err2;
					}
				}
			}
		}
		/* close the file if necessary */
		if (close_file)
			file_close(fd);
	}
	return err;
}  /* file_validate_data */

static bool check_error(int err, char *doing, char *func)
{
	char *str;
	
	if (err != UPDATE_NO_ERROR) {
		switch (err) {
		case UPDATE_NO_ERROR:
			str = "UPDATE_NO_ERROR";
			break;
		case UPDATE_FILE_OPEN_ERROR:
			str = "UPDATE_FILE_OPEN_ERROR";
			break;
		case UPDATE_FILE_CREATE_ERROR:
			str = "UPDATE_FILE_CREATE_ERROR";
			break;
		case UPDATE_FILE_CLOSE_ERROR:
			str = "UPDATE_FILE_CLOSE_ERROR";
			break;
		case UPDATE_FILE_DELETE_ERROR:
			str = "UPDATE_FILE_DELETE_ERROR";
			break;
		case UPDATE_FILE_RENAME_ERROR:
			str = "UPDATE_FILE_RENAME_ERROR";
			break;
		case UPDATE_FILE_READ_EOF_ERROR:
			str = "UPDATE_FILE_READ_EOF_ERROR";
			break;
		case UPDATE_FILE_READ_ERROR:
			str = "UPDATE_FILE_READ_ERROR";
			break;
		case UPDATE_FILE_WRITE_EOF_ERROR:
			str = "UPDATE_FILE_WRITE_EOF_ERROR";
			break;
		case UPDATE_FILE_WRITE_ERROR:
			str = "UPDATE_FILE_WRITE_ERROR";
			break;
		case UPDATE_FILE_SEEK_ERROR:
			str = "UPDATE_FILE_SEEK_ERROR";
			break;
		case UPDATE_EPROM_NOT_PRESENT_ERROR:
			str = "UPDATE_EPROM_NOT_PRESENT_ERROR";
			break;
		case UPDATE_EPROM_READ_ERROR:
			str = "UPDATE_EPROM_READ_ERROR";
			break;
		case UPDATE_EPROM_CHECKSUM_ERROR:
			str = "UPDATE_EPROM_CHECKSUM_ERROR";
			break;
		case UPDATE_WRONG_GAME_ERROR:
			str = "UPDATE_WRONG_GAME_ERROR";
			break;
		case UPDATE_WRONG_REVISON_LEVEL_ERROR:
			str = "UPDATE_WRONG_REVISON_LEVEL_ERROR";
			break;
		case UPDATE_UP_TO_DATE:
			str = "UPDATE_UP_TO_DATE";
			break;
		case UPDATE_ROM_ALREADY_PROCESSED:
			str = "UPDATE_ROM_ALREADY_PROCESSED";
			break;
		case UPDATE_SCRIPTFILE_CREATE_ERROR:
			str = "UPDATE_SCRIPTFILE_CREATE_ERROR";
			break;
		case UPDATE_MORE_ROMS_NEEDED:
			str = "UPDATE_MORE_ROMS_NEEDED";
			break;
		case UPDATE_HAVE_ALL_PIECES:
			str = "UPDATE_HAVE_ALL_PIECES";
			break;
		case UPDATE_GZIP_OPEN_ERROR:
			str = "UPDATE_GZIP_OPEN_ERROR";
			break;
		case UPDATE_GZIP_CREATE_ERROR:
			str = "UPDATE_GZIP_CREATE_ERROR";
			break;
		case UPDATE_GZIP_CLOSE_ERROR:
			str = "UPDATE_GZIP_CLOSE_ERROR";
			break;
		case UPDATE_GZIP_READ_ERROR:
			str = "UPDATE_GZIP_READ_ERROR";
			break;
		case UPDATE_GZIP_WRITE_ERROR:
			str = "UPDATE_GZIP_WRITE_ERROR";
			break;
		case UPDATE_GZIP_EOF_ERROR:
			str = "UPDATE_GZIP_EOF_ERROR";
			break;
		case UPDATE_GZIP_BUFFER_SMALL_ERROR:
			str = "UPDATE_GZIP_BUFFER_SMALL_ERROR";
			break;
		case UPDATE_GZIP_UNKNOWN_OPCODE:
			str = "UPDATE_GZIP_UNKNOWN_OPCODE";
			break;
		case UPDATE_PATCH_UNKNOWN_OPCODE:
			str = "UPDATE_PATCH_UNKNOWN_OPCODE";
			break;
		case UPDATE_MALLOC_ERROR:
			str = "UPDATE_MALLOC_ERROR";
			break;
		case UPDATE_DISKIO_PARAM_ERROR:
			str = "UPDATE_DISKIO_PARAM_ERROR";
			break;
		default:
			str = "UNKNOWN ERROR";
			break;
		}
		fprintf(stderr, "%s while %s in %s\n", str, doing, func);
	}
	return err != UPDATE_NO_ERROR;
}  /* check_error */

/* uchar name_length
 * uchar name[name_length] */
static int gzip_read_fname(gzFile zfd, void *buffer, int max_size)
{
	int len;
	int err;
	
	/* read the filename length byte */
	err = gzip_read(zfd, &len, sizeof(len));
	
	/* check for error reading or buffer size too small */
	if (err == UPDATE_NO_ERROR && len >= max_size)
		err = UPDATE_GZIP_BUFFER_SMALL_ERROR;
	
	/* read the filename bytes, padded to mult of four */
	if (err == UPDATE_NO_ERROR)
		err = gzip_read(zfd, buffer, ROUND_UP_TO_4(len));
	
	/* nul temrinate the string */
	if (err == UPDATE_NO_ERROR)
		((uchar *)buffer)[len] = '\0';
	else
		*(uchar *)buffer = '\0';
	
	return err;
}  /* gzip_read_fname */

static bool piece_exist(int rev_level, int rom_number)
{
	/* check if a ROM piece is already on the disk */
	return file_exist(build_piece_filename(rev_level, rom_number));
}  /* piece_exist */

static void check_piece_set(int rev_level, int rom_count, int *num_pieces_still_needed, int *next_rom_needed)
{
	int i;
	
	*num_pieces_still_needed = 0;
	*next_rom_needed = 0;
	
	/* EPROMs are numbered 1 based */
	for (i = rom_count; i >= 1; i--) {
		/* see if the piece exist on the disk yet */
		if (!piece_exist(rev_level, i)) {
			/* bump up the needed count */
			(*num_pieces_still_needed)++;
			/* find the lowest number EPROM still needed */
			*next_rom_needed = i;
		}
	}
}  /* check_piece_set */

/* concatenate all of the ERPOM pieces into one gzip file.  the pieces are validated before being processed */
/* after sucessfully building the compressed script file, the pieces are deleted */
static int build_script_file(rom_header *curr_header)
{
	uchar buffer[1024];
	rom_header piece_header;
	char *fname;
	int script_fd, piece_fd;
	int byte_count, num_bytes;
	int err, err2, i;
	
	/* loop through all of the piece files and verify the contents on disk */
	for (i = 1; i <= curr_header->rom_count; i++) {
		/* build the piece files name */
		fname = build_piece_filename(curr_header->new_level.rev_level, i);
		/* validate the piece data */
		err = file_validate_data(-1, fname);
		if (err != UPDATE_NO_ERROR)
			break;
	}
	if (err == UPDATE_NO_ERROR) {
		/* create the script file */
		err = file_create(UPDATE_SCRIPT_FILENAME, &script_fd);
		if (err == UPDATE_NO_ERROR) {
			/* copy piece by piece into the file without including the header or trailer */
			/* remeber that pieces are one based, a 3 EPROM set is numbered one through three */
			for (i = 1; i <= curr_header->rom_count; i++) {
				/* build the EPROM filename */
				fname = build_piece_filename(curr_header->new_level.rev_level, i);
				/* open the disk based file */
				err = file_open(fname, &piece_fd);
				if (err == UPDATE_NO_ERROR) {
					/* get the header */
					err = file_read_header(piece_fd, &piece_header);
					if (err == UPDATE_NO_ERROR) {
						/* calculate the number of bytes to copy */
						byte_count = piece_header.rom_size - (sizeof(rom_header) + sizeof(rom_trailer));
						/* skip past the header */
						err = file_set_pos(piece_fd, sizeof(rom_header), SEEK_SET);
						if (err == UPDATE_NO_ERROR) {
							/* loop reading until all data copied from piece file to script file */
							while (byte_count > 0) {
								/* determine the number of bytes to read */
								num_bytes = byte_count > sizeof(buffer) ? sizeof(buffer) : byte_count;
								/* bump down the number of remaing bytes to read */
								byte_count -= num_bytes;
									
								/* read a chunk in */
								err = file_read(piece_fd, buffer, num_bytes);
								if (err == UPDATE_NO_ERROR) {
									/* write chunk out to script file */
									err = file_write(script_fd, buffer, num_bytes);
									if (err != UPDATE_NO_ERROR)
										break;
								} else
									break;
							}
						}
					}
					/* close the disk based file */
					err2 = file_close(piece_fd);
					if (err == UPDATE_NO_ERROR)
						err = err2;
				}
			}
			/* close the script file */
			err2 = file_close(script_fd);
			if (err == UPDATE_NO_ERROR)
				err = err2;
		}
	} else {
		if (err == UPDATE_EPROM_CHECKSUM_ERROR) {
			if (i <= curr_header->rom_count) {
				fname = build_piece_filename(curr_header->new_level.rev_level, i);
				printf("ROM number %d is invalid\n", i);
			}
		}
	}
	return err;
}  /* build_script_file */

static int gzip_skip_bytes(gzFile gzip_file, int num_bytes)
{
	uchar buffer[1024];
	int read_count;
	int err;
	
	while (num_bytes > 0) {
		/* find out how many bytes to copy */
		read_count = num_bytes > sizeof(buffer) ? sizeof(buffer) : num_bytes;
		/* bump down the remain byte count */
		num_bytes -= read_count;
		/* read the data from the script file */
		err = gzip_read(gzip_file, buffer, read_count);
		if (err != UPDATE_NO_ERROR)
			break;
	}
	return err;
}  /* gzip_skip_bytes */

static int process_script_file(void)
{
	uchar old_filename[256];
	uchar new_filename[256];
	gzFile gzip_file;
	int err, err2, done, done2, op_code;
	int args[4], i, file_offset, file_len;
	
	/* open the script file */
	err = gzip_open(UPDATE_SCRIPT_FILENAME, &gzip_file);
	if (err == UPDATE_NO_ERROR) {
		done = FALSE;
		while (!done) {
			/* read the next op code */
			err = gzip_read(gzip_file, &op_code, sizeof(op_code));
			if (err == UPDATE_NO_ERROR) {
				/* switch off to routine to handle op code */
				switch (op_code) {
				case EXEC_FILE:
					/* get the filename to exec */
					err = gzip_read_fname(gzip_file, old_filename, sizeof(old_filename));
					if (err == UPDATE_NO_ERROR) {
						/* read the parameters */
						for (i = 0; i < 4; i++) {
							err = gzip_read(gzip_file, &args[i], sizeof(int));
							if (err != UPDATE_NO_ERROR)
								break;
						}
						if (err == UPDATE_NO_ERROR)
							printf("exec_file \"%s\" %d %d %d %d;\n", old_filename, args[0], args[1], args[2], args[3]);
					}
					break;
				case NEW_FILE:
					err = gzip_read_fname(gzip_file, new_filename, sizeof(new_filename));
					if (err == UPDATE_NO_ERROR) {
						err = gzip_read(gzip_file, &file_len, sizeof(file_len));
						if (err == UPDATE_NO_ERROR) {
							printf("new_file \"%s\";\n", new_filename);
							printf("#file length = %d\n", file_len);
							err = gzip_skip_bytes(gzip_file, ROUND_UP_TO_4(file_len));
						}
					}
					break;
				case DELETE_FILE:
					/* get the filename to delete */
					err = gzip_read_fname(gzip_file, old_filename, sizeof(old_filename));
					if (err == UPDATE_NO_ERROR)
						printf("delete_file \"%s\";\n", old_filename);
					break;
				case RENAME_FILE:
					/* get the current filename */
					err = gzip_read_fname(gzip_file, old_filename, sizeof(old_filename));
					if (err == UPDATE_NO_ERROR) {
						/* get the new filename */
						err = gzip_read_fname(gzip_file, new_filename, sizeof(new_filename));
						if (err == UPDATE_NO_ERROR)
							printf("rename_file \"%s\" \"%s\";\n", old_filename, new_filename);
					}
					break;
				case PATCH_FILE:
					/* get the current filename */
					err = gzip_read_fname(gzip_file, old_filename, sizeof(old_filename));
					if (err == UPDATE_NO_ERROR) {
						/* get the tmp filename */
						err = gzip_read_fname(gzip_file, new_filename, sizeof(new_filename));
						if (err == UPDATE_NO_ERROR) {
							printf("patch_file \"%s\" \"%s\";\n", old_filename, new_filename);
							
							done2 = FALSE;
							while (!done2) {
								err = gzip_read(gzip_file, &op_code, sizeof(op_code));
								if (err == UPDATE_NO_ERROR) {
									switch (op_code) {
									case COPY_CHUNK:
										err = gzip_read(gzip_file, &file_offset, sizeof(file_offset));
										if (err == UPDATE_NO_ERROR) {
											err = gzip_read(gzip_file, &file_len, sizeof(file_len));
											if (err == UPDATE_NO_ERROR)
												printf("#patch_copy_chunk %d offset %d bytes\n", file_offset, file_len);
										}
										break;
									case NEW_CHUNK:
										err = gzip_read(gzip_file, &file_len, sizeof(file_len));
										if (err == UPDATE_NO_ERROR) {
											printf("#patch_new_chunk %d bytes\n", file_len);
											err = gzip_skip_bytes(gzip_file, file_len);
										}
										break;
									case END_PATCH:
										printf("#end_patch\n");
										done2 = TRUE;
										break;
									default:
										err = UPDATE_PATCH_UNKNOWN_OPCODE;
										break;
									}
								}
								if (err != UPDATE_NO_ERROR)
									done2 = TRUE;
							}
						}
					}
					break;
				case COPY_FILE:
					/* get the current filename */
					err = gzip_read_fname(gzip_file, old_filename, sizeof(old_filename));
					if (err == UPDATE_NO_ERROR) {
						/* get the new filename */
						err = gzip_read_fname(gzip_file, new_filename, sizeof(new_filename));
						if (err == UPDATE_NO_ERROR)
							printf("copy_file \"%s\" \"%s\";\n", old_filename, new_filename);
					}
					break;
				case END_SCRIPT:
					printf("end_script;\n");
					done = TRUE;
					break;
				default:
					err = UPDATE_GZIP_UNKNOWN_OPCODE;
					break;
				}
			}
			if (err != UPDATE_NO_ERROR)
				done = TRUE;
		}
		
		err2 = gzip_close(gzip_file);
		if (err == UPDATE_NO_ERROR)
			err = err2;
	}
	return err;
}  /* process_script_file */

/*
 *		$History: dumprom.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/13/97   Time: 4:22p
 * Created in $/video/tools/dumprom
 * Source for dumprom tool
 */
