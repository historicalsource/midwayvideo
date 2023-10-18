%{
/*
 *		$Archive: $
 *		$Revision: $
 *		$Date: $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

/* Due to the crummy filesystem we got stuck with all writes and seeks must be a multiple of 4 */

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "lex.h"
#include "update.h"
#include "upio.h"
#include "buildrom.h"

/*
 *		DEFINES
 */

#define GZIP_SCRIPT_FILENAME	"UPD.DAT"
#define ROUND_DOWN_TO_4(x)		((x / 4) * 4)

/*
 *		STATIC PROTOTYPES
 */

static void yyerror(char *err_string);
static int gzip_write_fname(gzFile gfd, char *fname);
static int gzip_write_opcode(gzFile gfd, uchar opcode);
static void build_path_filename(char *path, char *file_name, char *path_filename);
static bool check_error(int err, char *doing, char *func);
static bool check_params(void);
static void set_header(rom_header *h, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev, ushort rom_count, ushort rom_number);
static void set_trailer(rom_trailer *t);
static void set_checksum(rom_header *h, rom_trailer *t, void *data_block, int block_size);
static int build_rom_set(char *gzip_script_filename, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev);
static int build_patch_script(gzFile gfd, char *old_file_name, char *new_file_name);
static int patch_output_copy(gzFile gfd, int file_offset, int bytes_to_copy);
static int patch_output_new(gzFile gfd, uchar *new, int index, int num_bytes);
static int patch_output_end(gzFile gfd);

/*
 *		STATIC VARIABLES
 */

static ushort game_id;
static bool game_id_set;
static int eprom_size;
static ushort old_revision_level;
static bool old_revision_level_set;
static ushort new_revision_level;
static bool new_revision_level_set;
static char old_revision_directory[256];
static char new_revision_directory[256];
static char revision_name[256];
static bool exec_file_issued;
static gzFile gzfd;
%}

/*
 *		SEMANTIC STACK
 */

%union {
	int param[4];
	char *str;
	int ic;
}

/*
 *		TERMINAL SYMBOLS
 */

%token T_FOUR_MB T_EIGHT_MB T_OLD_REVISION_DIRECTORY
%token T_NEW_REVISION_DIRECTORY T_EPROM_SIZE
%token T_REVISION_NAME T_OLD_REVISION_LEVEL T_NEW_REVISION_LEVEL
%token T_GAME_ID T_NEW_FILE T_DELETE_FILE
%token T_RENAME_FILE T_EXEC_FILE T_PATCH_FILE T_COPY_FILE T_END_SCRIPT
%token T_EOF T_UNKNOWN_IDENT
%token <str>T_STR
%token <ic>T_IC

/*
 *		NONTERMINAL SYMBOLS
 */

%type <ic>rom_size
%type <param>params

%start script								/* start symbol */
%%
script:		empty1 script_file T_EOF
			{
				int err;
				
				/* close the output file */
				err = gzip_close(gzfd);
				if (check_error(err, "closing gzip file", "stript reduction"))
					YYABORT;
				
				err = build_rom_set(GZIP_SCRIPT_FILENAME, eprom_size, revision_name, game_id, old_revision_level, new_revision_level);
				if (check_error(err, "building EPROM set", "stript reduction"))
					YYABORT;
				
#if 0
				/* delete the temp script file */
				err = file_delete(GZIP_SCRIPT_FILENAME);
				if (check_error(err, "deleting temp script file", "stript reduction"))
					YYABORT;
#endif
				
				YYACCEPT;
			}
		|	T_EOF
			{
				fprintf(stderr, "The file `%s\' contains no script commands", in_file_name);
				YYACCEPT;
			}
		;

empty1:		/* empty */
			{
				int err;
				
				/* init the script parameters */
				game_id = 0;
				game_id_set = FALSE;
				eprom_size = 0;
				old_revision_level = 0;
				old_revision_level_set = FALSE;
				new_revision_level = 0;
				new_revision_level_set = FALSE;
				*old_revision_directory = '\0';
				*new_revision_directory = '\0';
				*revision_name = '\0';
				exec_file_issued = FALSE;
				
				/* create the output gzip file */
				err = gzip_create(GZIP_SCRIPT_FILENAME, &gzfd);
				if (check_error(err, "creating a gzip file", "empty1 reduction"))
					YYABORT;
			}
		;

script_file:
			script_statement ';'
		|	script_file script_statement ';'
		;

script_statement:
			T_OLD_REVISION_DIRECTORY T_STR
			{
				strcpy(old_revision_directory, $2);
				fprintf(stderr, "setting old_revision_directory = %s\n", old_revision_directory);
				free($2);
			}
		|	T_NEW_REVISION_DIRECTORY T_STR
			{
				strcpy(new_revision_directory, $2);
				fprintf(stderr, "setting new_revision_directory = %s\n", new_revision_directory);
				free($2);
			}
		|	T_EPROM_SIZE rom_size
			{
				eprom_size = $2;
				fprintf(stderr, "setting rom_size = %dK\n", eprom_size / 1024);
			}
		|	T_REVISION_NAME T_STR
			{
				strcpy(revision_name, $2);
				fprintf(stderr, "setting revision_name = %28s\n", revision_name);
				free($2);
			}
		|	T_OLD_REVISION_LEVEL T_IC
			{
				old_revision_level = $2;
				old_revision_level_set = TRUE;
				fprintf(stderr, "setting old_revision_level = %hu\n", old_revision_level);
			}
		|	T_NEW_REVISION_LEVEL T_IC
			{
				new_revision_level = $2;
				new_revision_level_set = TRUE;
				fprintf(stderr, "setting new_revision_level = %hu\n", new_revision_level);
			}
		|	T_GAME_ID T_IC
			{
				game_id = $2;
				game_id_set = TRUE;
				fprintf(stderr, "setting game_id = %hu\n", game_id);
			}
		|	T_NEW_FILE T_STR
			{
				uchar buffer[1024];
				char path_filename[256];
				int f_size, pad_size;
				int num_bytes;
				int err, fd, pad;
				
				/* NEW_FILE opcode
				 * filename file_name						name of new file
				 * int file_length							number of bytes to copy
				 * uchar file_bytes[file_length]			file data */
				
				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, NEW_FILE);
				if (check_error(err, "writing an opcode", "new_file reduction"))
					YYABORT;
				
				/* write out the new filename */
				err = gzip_write_fname(gzfd, $2);
				if (check_error(err, "writing a filename", "new_file reduction"))
					YYABORT;
				
				/* open the new file for copying into the script file */
				build_path_filename(new_revision_directory, $2, path_filename);
				err = file_open(path_filename, &fd);
				if (check_error(err, "opening a file", "new_file reduction"))
					YYABORT;
				
				/* get the size of the file */
				err = file_size(fd, NULL, &f_size);
				if (check_error(err, "getting a file size", "new_file reduction"))
					YYABORT;
				pad_size = ROUND_UP_TO_4(f_size) - f_size;

				/* write out the size to the script file */
				err = gzip_write(gzfd, &f_size, sizeof(f_size));
				if (check_error(err, "writing a file size", "new_file reduction"))
					YYABORT;
				
				while (f_size > 0) {
					/* determine the number of bytes to read */
					num_bytes = f_size > sizeof(buffer) ? sizeof(buffer) : f_size;
					/* update the number of bytes left to process */
					f_size -= num_bytes;
					err = file_read(fd, buffer, num_bytes);
					if (check_error(err, "reading a file buffer", "new_file reduction"))
						YYABORT;
					
					err = gzip_write(gzfd, buffer, num_bytes);
					if (check_error(err, "writing a file buffer", "new_file reduction"))
						YYABORT;					
				}
				
				/* pad the new file data out to four bytes */
				pad = 0;
				err = gzip_write(gzfd, &pad, pad_size);
				if (check_error(err, "writing a file buffer", "new_file reduction"))
					YYABORT;					

				err = file_close(fd);
				if (check_error(err, "closing a file buffer", "new_file reduction"))
					YYABORT;
				
				/* free the string storage */
				free($2);
			}
		|	T_DELETE_FILE T_STR
			{
				char path_filename[256];
				int err;
				
				/* DELETE_FILE opcode
				 * filename file_name						name of file to delete from disk */
				
				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, DELETE_FILE);
				if (check_error(err, "writing an opcode", "delete_file reduction"))
					YYABORT;
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, $2, path_filename);
				
				/* check to see if the file actually exists */
				if (file_exist(path_filename)) {
					err = gzip_write_fname(gzfd, $2);
					if (check_error(err, "writing a filename", "delete_file reduction"))
						YYABORT;
				} else {
					fprintf(stderr, "Error %s not a file in the %s directory\n", $2, old_revision_directory);
				}

				/* free the string storage */
				free($2);
			}
		|	T_RENAME_FILE T_STR T_STR
			{
				char path_filename[256];
				int err;
				
				/* RENAME_FILE opcode
				 * filename old_name						name of file to rename
				 * filename new_name						new file name */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, RENAME_FILE);
				if (check_error(err, "writing an opcode", "rename_file reduction"))
					YYABORT;
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, $2, path_filename);
				
				/* check to see if the file actually exists */
				if (file_exist(path_filename)) {
					err = gzip_write_fname(gzfd, $2);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
					err = gzip_write_fname(gzfd, $3);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
				} else {
					fprintf(stderr, "Error %s not a file in the %s directory\n", $2, old_revision_directory);
				}
				
				/* free the string storage */
				free($2);
				free($3);				
			}
		|	T_EXEC_FILE T_STR params
			{
				int i;
				int err;
				
				/* EXEC_FILE opcode
				 * filename file_name						name of file to exec
				 * int params[4] 							params to pass to program */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, EXEC_FILE);
				if (check_error(err, "writing an opcode", "exec_file reduction"))
					YYABORT;
				
				/* write out the filename to exec */
				err = gzip_write_fname(gzfd, $2);
				if (check_error(err, "writing a filename", "exec_file reduction"))
					YYABORT;

				/* write out the four int params */
				for (i = 0; i < 4; i++) {
					err = gzip_write(gzfd, &$3[i], sizeof(int));
					if (check_error(err, "writing an exec parameter", "exec_file reduction"))
						YYABORT;
				}
				
				/* free the string storage */
				free($2);
				
				/* warn if any commands follow this */
				exec_file_issued = TRUE;
			}
		|	T_COPY_FILE T_STR T_STR
			{
				char old_path_filename[256];
				char new_path_filename[256];
				int err;
				
				/* COPY_FILE opcode
				 * filename old_name						name of file to copy
				 * filename new_name						new file name */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, COPY_FILE);
				if (check_error(err, "writing an opcode", "copy_file reduction"))
					YYABORT;
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, $2, old_path_filename);
				build_path_filename(new_revision_directory, $2, new_path_filename);
				
				/* check to see if the file actually exists */
				if (file_exist(old_path_filename) || file_exist(new_path_filename)) {
					err = gzip_write_fname(gzfd, $2);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
					err = gzip_write_fname(gzfd, $3);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
				} else {
					fprintf(stderr, "Error %s not a file in the %s or %s directory\n", $2, old_revision_directory, new_revision_directory);
				}
				
				/* free the string storage */
				free($2);
				free($3);				
			}
		|	T_PATCH_FILE T_STR T_STR
			{
				char path_old_filename[256];
				char path_new_filename[256];
				int err;
				
				/* PATCH_FILE opcode
				 * filename file_name						file on disk to patch
				 * filename rename_file						name to rename the original to
				 *
				 * op codes
				 *		COPY_CHUNK							from the original file
				 *			int file_offset					offset in original file
				 *			int num_byte_to_copy			number of bytes to copy to new file
				 *		NEW_CHUNK							from the patch stream
				 *			int num_new_byte				number of bytes that follow
				 *			uchar chunk_bytes[num_new_byte]	new data to copy in file
				 *		END_PATCH							end of patch op code setinel */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, PATCH_FILE);
				if (check_error(err, "writing an opcode", "patch_file reduction"))
					YYABORT;

				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, $2, path_old_filename);
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(new_revision_directory, $2, path_new_filename);
				
				/* check to see if the files actually exists */
				if (!file_exist(path_old_filename)) {
					fprintf(stderr, "Error %s not a file in the %s directory\n", $2, old_revision_directory);
					YYABORT;
				}
				if (!file_exist(path_new_filename)) {
					fprintf(stderr, "Error %s not a file in the %s directory\n", $2, new_revision_directory);
					YYABORT;
				}
				
				/* write out the orginal file name */
				err = gzip_write_fname(gzfd, $2);
				if (check_error(err, "writing a filename", "patch_file reduction"))
					YYABORT;
				/* write out the rename filename */
				err = gzip_write_fname(gzfd, $3);
				if (check_error(err, "writing a filename", "patch_file reduction"))
					YYABORT;

				err = build_patch_script(gzfd, path_old_filename, path_new_filename);
				if (check_error(err, "writing a patch script", "patch_file reduction"))
					YYABORT;
				
				/* free the string storage */
				free($2);
				free($3);
			}
		|	T_END_SCRIPT
			{
				int err;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, END_SCRIPT);
				if (check_error(err, "writing an opcode", "end_script reduction"))
					YYABORT;
			}
		|	error
			{
				YYABORT;
			}
		;

rom_size:	T_FOUR_MB
			{
				/* 4Mb == 512K */
				$$ = (1024 * 1024 * 4) / 8;
			}
		|	T_EIGHT_MB
			{
				/* 8Mb == 1024K */
				$$ = (1024 * 1024 * 8) / 8;
			}
		;

params:		/* empty */
			{
				$$[0] = 0;
				$$[1] = 0;
				$$[2] = 0;
				$$[3] = 0;
			}
		|	T_IC
			{
				$$[0] = $1;
				$$[1] = 0;
				$$[2] = 0;
				$$[3] = 0;
			}
		|	T_IC ',' T_IC
			{
				$$[0] = $1;
				$$[1] = $3;
				$$[2] = 0;
				$$[3] = 0;
			}
		|	T_IC ',' T_IC ',' T_IC
			{
				$$[0] = $1;
				$$[1] = $3;
				$$[2] = $5;
				$$[3] = 0;
			}
		|	T_IC ',' T_IC ',' T_IC ',' T_IC
			{
				$$[0] = $1;
				$$[1] = $3;
				$$[2] = $5;
				$$[3] = $7;
			}
		;
%%
/*
 *		STATIC FUNCTIONS
 */

static void yyerror(char *err_string)
{
	fprintf (stderr, "%s at line %d\n", err_string, line_number);
}  /* yyerror */

/* write out a filename to the gzip script file, padded to 4 bytes */
static int gzip_write_fname(gzFile gfd, char *fname)
{
	int err;
	int len, pad;

	len = strlen(fname);
	err = gzip_write(gfd, &len, sizeof(len));
	if (err == UPDATE_NO_ERROR) {
		err = gzip_write(gfd, fname, len);
		if (err == UPDATE_NO_ERROR) {
			pad = 0;
			/* pad to a multiple of four! */
			err = gzip_write(gfd, &pad, ROUND_UP_TO_4(len) - len);
		}
	}
	return err;
}  /* gzip_write_fname */

/* write out an opcode to the gzip script file, padded to 4 bytes */
static int gzip_write_opcode(gzFile gfd, uchar opcode)
{
	int pad_opcode;
	
	/* pad the opcode to four bytes */
	pad_opcode = opcode;
	return gzip_write(gfd, &pad_opcode, sizeof(pad_opcode));
}  /* gzip_write_opcode */

static void build_path_filename(char *path, char *file_name, char *path_filename)
{
	strcpy(path_filename, path);
	strcat(path_filename, "\\");
	strcat(path_filename, file_name);
}  /* build_path_filename */
				
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

static bool check_params(void)
{
	int cnt;
	
	cnt = 0;
	if (!game_id_set)
		cnt = fprintf(stderr, "Use \'GAME_ID num;\' to set the games id\n");
	if (eprom_size == 0)
		cnt = fprintf(stderr, "Use \'EPROM_SIZE (FOUR_MB | EIGHT_MB);\' to set the EPROM size\n");
	if (!old_revision_level_set)
		cnt = fprintf(stderr, "Use \'OLD_REVISION_LEVEL num;\' to set the previous revision level\n");
	if (!new_revision_level_set)
		cnt = fprintf(stderr, "Use \'NEW_REVISION_LEVEL num;\' to set the new revision level\n");
	if (*old_revision_directory == '\0')
		cnt = fprintf(stderr, "Use \'OLD_REVISION_DIRECTORY str;\' to set the old revision directory\n");
	if (*new_revision_directory == '\0')
		cnt = fprintf(stderr, "Use \'NEW_REVISION_DIRECTORY str;\' to set the new revision directory\n");
	if (*revision_name == '\0')
		cnt = fprintf(stderr, "Use \'REVISION_NAME str;\' to set the revision name\n");
	if (old_revision_level >= new_revision_level)
		cnt = fprintf(stderr, "old revision level is >= new revision level!\n");
	if (exec_file_issued)
		cnt = fprintf(stderr, "commands following exec_file will never get executed!\n");
	return cnt > 0;
}  /* check_params */

static void set_header(rom_header *h, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev, ushort rom_count, ushort rom_number)
{
	int i;
	
	/* set the header signature */
	h->signature = HEADER_SIG;
	h->rom_size = rom_size;
	/* the checksum and inv_checksum will be calculated later */
	h->checksum_16 = 0;
	h->inv_checksum_16 = 0;
	
	/* copy over the revision name, may not be nul terminated */
	for (i = 0; i < sizeof(revision_desc) && rev_name[i] != '\0'; i++)
		h->rev_name[i] = rev_name[i];
	while (i < sizeof(revision_desc))
		h->rev_name[i++] = '\0';
	
	/* set the required level for update */
	h->required_level.game_id = game_id;
	h->required_level.rev_level = from_rev;
	
	/* set the new revision level */
	h->new_level.game_id = game_id;
	h->new_level.rev_level = to_rev;
	
	/* set the number of EPROMs in the set */
	h->rom_count = rom_count;
	
	/* set which EPROM number this is */
	h->rom_number = rom_number;
	
	/* unused/future expansion fields */
	h->flags = 0;
	h->unused1 = 0xDEADBEEF;
	h->unused2 = 0xDEADBEEF;
}  /* set_header */

static void set_trailer(rom_trailer *t)
{
	/* set up the trialer signature */
	t->i = TRAILER_SIG;
	
	/* calculate the adjustment later */
	t->s.checksum_adjust = 0;
}  /* set_trailer */

static void set_checksum(rom_header *h, rom_trailer *t, void *data_block, int block_size)
{
	ushort sum, temp_checksum, final_checksum;
	uchar lo_byte, rev_level, rom_number;
	
	/* get the low order nibble of the rom number */
	rom_number = h->rom_number & 0x000F;
	
	/* get the low order nibble of the revision level */
	rev_level = h->new_level.rev_level & 0x000F;
	
	/* encode the revision level and rom number in the low order byte of the checksum */
	lo_byte = (rev_level << 4) | rom_number;
		
	/* init the low order byte of the checksum and inv_checksum to final value, the revision level */
	h->checksum_16 = lo_byte;
	h->inv_checksum_16 = (~lo_byte) & 0x00FF;
	
	/* zero out the checksum tweak byte */
	t->s.checksum_adjust = 0;
	
	/* since checksum and inv_checksum are bitwise inverts of each other, and the checksum is an */
	/* additive calculation, the sum of the checksums low and high order bytes are 0xFF + 0xFF */
	/* the low order bytes are already set to final value, init a high order byte to 0xFF for */
	/* calculating the real checksum */
	h->checksum_16 |= 0xFF00;

	/* calculate the pretweaked checksum */
	sum = calculate_checksum(h, sizeof(rom_header));
	sum += calculate_checksum(data_block, block_size);
	sum += calculate_checksum(t, sizeof(rom_trailer));
	
	/* force the revision level and rom number on the lower byte of the checksum */
	temp_checksum = (sum & 0xFF00) | lo_byte;
	
	if (temp_checksum > sum) {
		/* the new checksum with the revision level forced on is bigger */
		/* add the difference to the tweak byte */
		/* to properly adjust the contents to make the checksum */

		/* determine the final checksum */
		final_checksum = temp_checksum;
		
		/* set the checksum adjustment byte */
		t->s.checksum_adjust = temp_checksum - sum;
	} else /* temp_checksum <= sum */{
		/* the new checksum is smaller */
		/* we need to keep the lower order byte as is, so bump up the high order byte by one */
		
		/* determine the final checksum */
		final_checksum = temp_checksum + (temp_checksum == sum ? 0x0000 : 0x0100);
		
		/* set the checksum adjustment byte */
		t->s.checksum_adjust = final_checksum - sum;
	}
	
	/* set the header fields */
	h->checksum_16 = final_checksum;
	h->inv_checksum_16 = ~final_checksum;
}  /* set_checksum */

/* given the filename of a gzipped script file on disk, split it into the EPROM images */
/* rom_size is 512 * 1024 for 27C040 or 1024 * 1024 for 27C080 */
/* rev_name is the text string descibing the update */
/* game_id is the id of the game the patch is for */
/* from_rev is the required revision that be present to do the patch */
/* to_rev is the new revision level after the patch */
static int build_rom_set(char *gzip_script_filename, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev)
{
	rom_header h;
	rom_trailer t;
	int script_file_size;
	uchar *block, *rom_name;
	int gfd, rfd;
	int num_rom, read_chunk;
	int block_size;
	int err;
	int i, j;
	
	block = malloc(rom_size);
	err = block == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	if (check_error(err, "mallocing the rom block", "build_rom_set"))
		return err;
	
	/* open the script file */
	err = file_open(gzip_script_filename, &gfd);
	if (check_error(err, "opening compressed script file", "build_rom_set"))
		return err;
	
	/* get the size of the gzipped script file in bytes */
	err = file_size(gfd, NULL, &script_file_size);
	if (check_error(err, "getting script file size", "build_rom_set"))
		return err;
	
	/* determine the number of EPROMs needed for the set */
	num_rom = script_file_size / rom_size;
	if (script_file_size % rom_size != 0)
		num_rom++;
	
	/* EPROMs are numbered one based */
	for (i = 1; i <= num_rom; i++) {
		/* clear out the EPROM bank */
		for (j = 0; j < rom_size; j++)
			block[j] = 0;
		
		block_size = rom_size - (sizeof(rom_header) + sizeof(rom_trailer));
		read_chunk = script_file_size > block_size ? block_size : script_file_size;
		script_file_size -= read_chunk;
		err = file_read(gfd, block, read_chunk);
		if (check_error(err, "reading from script file", "build_rom_set"))
			return err;
		
		/* fill in the header, trailer, and calculate the checksum */
		set_header(&h, rom_size, rev_name, game_id, from_rev, to_rev, num_rom, i);
		set_trailer(&t);
		set_checksum(&h, &t, block, read_chunk);
		
		/* create the EPROM image file */
		rom_name = build_piece_filename(to_rev, i);
		err = file_create(rom_name, &rfd);
		if (check_error(err, "creating ROM piece image file", "build_rom_set"))
			return err;
		
		err = file_write(rfd, &h, sizeof(rom_header));
		if (check_error(err, "writing ROM piece header", "build_rom_set"))
			return err;
		
		/* use block_size instead of read_chunk to pad out the last EPROM */
		err = file_write(rfd, block, block_size);
		if (check_error(err, "writing ROM piece data", "build_rom_set"))
			return err;
		
		err = file_write(rfd, &t, sizeof(rom_trailer));
		if (check_error(err, "writing ROM piece trailer", "build_rom_set"))
			return err;
		
		err = file_close(rfd);
		if (check_error(err, "closing ROM piece", "build_rom_set"))
			return err;
	}
	/* close the script file */
	err = file_close(gfd);
	if (check_error(err, "closing script file", "build_rom_set"))
		return err;
	
	/* free the data block */
	free(block);
	return err;
}  /* build_rom_set */

/* COPY_CHUNK						from the original file
 *	int file_offset					offset in original file
 *		file_offset must be a mult of four!
 *	int byte_to_copy				number of bytes to copy to new file
 *		byte_to_copy must be a mult of four!
 * NEW_CHUNK						from the patch stream
 *	int new_bytes					number of bytes that follow
 *	uchar chunk_bytes[new_bytes]	new data to copy in file
 *		new_bytes must be a mult of 4!
 * END_PATCH							end of patch op code setinel */
static int build_patch_script(gzFile gfd, char *old_file_name, char *new_file_name)
{
	uchar *old, *new;
	int fd;
	int old_size, new_size;
	int min_size;
	int top;
	int err;
	int i;
	
	/* open the old file */
	err = file_open(old_file_name, &fd);
	if (check_error(err, "opening old file", "build_patch_script"))
		return err;
	/* get the old files size */
	err = file_size(fd, NULL, &old_size);
	if (check_error(err, "getting file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}
	/* malloc space to read old file */
	old = malloc(ROUND_UP_TO_4(old_size));
	err = old == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	if (check_error(err, "mallocing old file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}
	/* read the old file in */
	err = file_read(fd, old, old_size);
	if (check_error(err, "reading old file buffer", "build_patch_script")) {
		file_close(fd);
		free(old);
		return err;
	}
	/* blank out the trailing pad */
	for (i = old_size; i < ROUND_UP_TO_4(old_size); i++)
		old[i] = 0;
	/* close the old file */
	err = file_close(fd);
	if (check_error(err, "closing old file", "build_patch_script")) {
		free(old);
		return err;
	}

	/* open the new file */
	err = file_open(new_file_name, &fd);
	if (check_error(err, "opening new file", "build_patch_script"))
		return err;
	/* get the new files size */
	err = file_size(fd, NULL, &new_size);
	if (check_error(err, "getting file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}
	/* malloc space to read new file */
	new = malloc(ROUND_UP_TO_4(new_size));
	err = new == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	if (check_error(err, "mallocing new file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}	
	/* read the new file */
	err = file_read(fd, new, new_size);
	if (check_error(err, "reading old file buffer", "build_patch_script")) {
		file_close(fd);
		free(new);
		return err;
	}	
	/* blank out the trailing pad */
	for (i = new_size; i < ROUND_UP_TO_4(new_size); i++)
		new[i] = 0;
	/* close the new file */
	err = file_close(fd);
	if (check_error(err, "closing old file", "build_patch_script")) {
		free(new);
		return err;
	}
	
	/* find the smaller size */
	min_size = old_size < new_size ? old_size : new_size;
	
	/* compare two files to find the top part that matches */
	for (top = 0; top < min_size; top++)
		if (old[top] != new[top])
			break;
	/* scale top back to a mult of four */
	top = ROUND_DOWN_TO_4(top);
	/* check to see if the number of bytes is worth using */
	if (top > 16) {
		/* tell to copy the matching top block */
		err = patch_output_copy(gfd, 0, top);
	} else {
		/* output the entire new file */
		top = 0;
	}
	/* output the rest of the new file */
	if (err == UPDATE_NO_ERROR)
		err = patch_output_new(gfd, new, top, ROUND_UP_TO_4(new_size - top));
	if (err == UPDATE_NO_ERROR)
		err = patch_output_end(gfd);
	/* free the file data */
	free(old);
	free(new);
	return err;
}  /* build_patch_script */

static int patch_output_copy(gzFile gfd, int file_offset, int bytes_to_copy)
{
	int err;
	
	if (file_offset % 4 != 0)
		printf("patch_output_copy:file_offset not a mult of four!\n");
	if (bytes_to_copy % 4 != 0)
		printf("patch_output_copy:bytes_to_copy not a mult of four!\n");
	err = gzip_write_opcode(gfd, COPY_CHUNK);
	if (err == UPDATE_NO_ERROR) {
		err = gzip_write(gfd, &file_offset, sizeof(file_offset));
		if (err == UPDATE_NO_ERROR)
			err = gzip_write(gfd, &bytes_to_copy, sizeof(bytes_to_copy));
	}
	return err;
}  /* patch_output_copy */

static int patch_output_new(gzFile gfd, uchar *new, int index, int num_bytes)
{
	int err;
	
	if (num_bytes % 4 != 0)
		printf("patch_output_new:num_bytes not a mult of four!\n");
	err = gzip_write_opcode(gfd, NEW_CHUNK);
	if (err == UPDATE_NO_ERROR) {
		err = gzip_write(gfd, &num_bytes, sizeof(num_bytes));
		if (err == UPDATE_NO_ERROR)
			err = gzip_write(gfd, &new[index], num_bytes);
	}
	return err;
}  /* patch_output_new */

static int patch_output_end(gzFile gfd)
{
	return gzip_write_opcode(gfd, END_PATCH);
}  /* patch_output_end */

/*
 *		$History: $
 */
