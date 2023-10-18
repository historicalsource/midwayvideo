/*
 *		$Archive: /video/tools/buildscr/buildscr.c $
 *		$Revision: 4 $
 *		$Date: 8/24/98 10:31a $
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

#include <ctype.h>
#include <dirent.h>
#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "upio.h"
#include "update.h"

/*
 *		TYPEDEFS
 */

typedef struct file_list_tag {
	struct file_list_tag *next;
	char file_name[256];
} file_list;

typedef struct checksum_list_tag {
	struct checksum_list_tag *next;
	char file_name[256];
	ulong checksum;
} checksum_list;

typedef struct {
	struct ffblk file_block;
	ulong file_crc;
} fcheck_data;

/*
 *		STATIC PROTOTYPES
 */

static file_list *build_file_list(char *dir_name);
static checksum_list *build_checksum_list(char *dir_name);
static void build_bak_file_name(char *bak_file, char *orig_file);
static bool valid_file_name(char *fname);
static void destroy_file_list(file_list *head);
static void destroy_checksum_list(checksum_list *head);
static bool file_in_dir(file_list *head, char *file_name);
static bool str_similiar(char *a, char *b);
static bool strstr_similiar(char *s, char *find);
int strncmp_similiar(char *s1, char *s2, int n);
static bool get_checksum(checksum_list *head, char *file_name, ulong *crc);
static void build_path_filename(char *path, char *file_name, char *path_filename);
static bool files_are_diff(checksum_list *old_list, checksum_list *new_list, char *old_dir, char *new_dir, char *file_name);
static bool files_are_diff2(char *old_dir, char *new_dir, char *file_name);
static void check_error(int err, char *doing, char *where);

/*
 *		STATIC VARIABLES
 */

/*
 *		GLOBAL FUNCTIONS
 */

/* this tool will not generate a rename_file or exec_file command */
/* those must be added by hand */
int main(int argc, char *argv[])
{
	char script_file_name[13];
	char bak_file_name[13];
	int script_fd;
	char string[256];
	int cnt;
	int err;
	file_list *old_list;
	file_list *new_list;
	file_list *file;
	char old_revision_directory[256];
	char new_revision_directory[256];
	int eprom_size;
	char revision_name[256];
	int old_revision_level;
	int new_revision_level;
	int game_id;
	checksum_list *old_checksum;
	checksum_list *new_checksum;
	
	/* initalize the last char plus one to check for truncation */
	revision_name[28] = '\0';
	if (argc != 8) {
		fprintf(stderr, "Usage:%s \"old_rev_dir\" \"new_rev_dir\" eprom_size(4 or 8) \"rev_name\" old_rev_level new_rev_level game_id\n", argv[0]);
		return EXIT_FAILURE;
	} else {
		strcpy(old_revision_directory, argv[1]);
		strcpy(new_revision_directory, argv[2]);
		eprom_size = atoi(argv[3]);
		strcpy(revision_name, argv[4]);
		old_revision_level = atoi(argv[5]);
		new_revision_level = atoi(argv[6]);
		game_id = atoi(argv[7]);
	}
	
	/* validate the parameters */
	if (eprom_size != 4 && eprom_size != 8) {
		fprintf(stderr, "Invalid eprom size, defaulting to 8\n");
		eprom_size = 8;
	}

	if (revision_name[28] != '\0') {
		revision_name[28] = '\0';
		fprintf(stderr, "Truncating revision name to:%s\n", revision_name);
	}
	
	if (old_revision_level < 0 || old_revision_level > 65535) {
		fprintf(stderr, "Invalid old revision level, aborting\n");
		return EXIT_FAILURE;
	}
	
	if (new_revision_level < 0 || new_revision_level > 65535) {
		fprintf(stderr, "Invalid new revision level, aborting\n");
		return EXIT_FAILURE;
	}
	
	if (game_id < 0 || game_id > 65535) {
		fprintf(stderr, "Invalid game id, aborting\n");
		return EXIT_FAILURE;
	}

	/* create the script file */
	sprintf(script_file_name, "UPD%02d-%02d.SCR", old_revision_level & 0xFF, new_revision_level & 0xFF);
	err = file_create(script_file_name, &script_fd);
	check_error(err, "creating script file", "main");
	
	/* issue commands for the header parameter info */
	/* old_revision_directory "old"; */
	cnt = sprintf(string, "old_revision_directory \"%s\";\n", old_revision_directory);
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

	/* new_revision_directory "new"; */
	cnt = sprintf(string, "new_revision_directory \"%s\";\n", new_revision_directory);
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

		/* eprom_size eight_mb; or four_mb; */
	cnt = sprintf(string, "eprom_size %s;\n", eprom_size == 4 ? "four_mb" : "eight_mb");
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

		/* revision_name "blitz update one"; */
	cnt = sprintf(string, "revision_name \"%s\";\n", revision_name);
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

		/* old_revision_level 1; */
	cnt = sprintf(string, "old_revision_level %d;\n", old_revision_level);
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

		/* new_revision_level 2; */
	cnt = sprintf(string, "new_revision_level %d;\n", new_revision_level);
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

		/* game_id 444; */
	cnt = sprintf(string, "game_id %d;\n", game_id);
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");
		
		/* build a list of files in the old revision directory */
	fprintf(stderr, "Building old directory file cache...");
	old_list = build_file_list(old_revision_directory);
	fprintf(stderr, "done\n");
		
	/* build a list of files in the new revision directory */
	fprintf(stderr, "Building new directory file cache...");
	new_list = build_file_list(new_revision_directory);
	fprintf(stderr, "done\n");
	
#ifdef DISK_HAS_FILESYS_CHK
	/* build the list of old file checksums */
	fprintf(stderr, "Building old directory checksum cache...");
	old_checksum = build_checksum_list(old_revision_directory);
	fprintf(stderr, "done\n");
	/* build the list of new file checksums */
	fprintf(stderr, "Building new directory checksum cache...");
	new_checksum = build_checksum_list(new_revision_directory);
	fprintf(stderr, "done\n");
#else
	old_checksum = NULL;
	new_checksum = NULL;
#endif

	/* loop thru the old dir list */
	/* any file that is in the old dir but not the new dir output */
	/* delete_file "old.fil"; */
	fprintf(stderr, "Generating delete_file commands...");
	for (file = old_list; file != NULL; file = file->next) {
		if (!file_in_dir(new_list, file->file_name)) {
			/* delete_file "old.fil"; */
			cnt = sprintf(string, "delete_file \"%s\";\n", file->file_name);
			err = file_write(script_fd, string, cnt);
			check_error(err, "writing string to file", "main");
		}
	}
	fprintf(stderr, "done\n");
		
	/* loop thru the new dir list */
	/* any file that is in the new dir but not the old dir output */
	/* new_file "new.fil"; */
	fprintf(stderr, "Generating new_file commands...");
	for (file = new_list; file != NULL; file = file->next) {
		if (!file_in_dir(old_list, file->file_name)) {
			/* new_file "new.fil"; */
			cnt = sprintf(string, "new_file \"%s\";\n", file->file_name);
			err = file_write(script_fd, string, cnt);
			check_error(err, "writing string to file", "main");
			
#ifdef DISK_HAS_BAK_FILES
			/* copy_file "new.fil" "new.bak; */
			build_bak_file_name(bak_file_name, file->file_name);
			cnt = sprintf(string, "copy_file \"%s\" \"%s\";\n", file->file_name, bak_file_name);
			err = file_write(script_fd, string, cnt);
			check_error(err, "writing string to file", "main");
#endif
		}
	}
	fprintf(stderr, "done\n");
		
	/* loop thru the new dir list */
	/* any file that is in the new dir and the old dir, check for differences */
	/* if any patch the file */
	/* patch_file "file.nam" "tmp.nam"; */
	fprintf(stderr, "Generating patch_file commands...\n");
	for (file = new_list; file != NULL; file = file->next) {
		if (file_in_dir(old_list, file->file_name)) {
			if (files_are_diff(old_checksum, new_checksum, old_revision_directory, new_revision_directory, file->file_name)) {
				/* patch_file "file.nam" "tmp.nam"; */
				cnt = sprintf(string, "patch_file \"%s\" \"%s\";\n", file->file_name, "rename.tmp");
				err = file_write(script_fd, string, cnt);
				check_error(err, "writing string to file", "main");
				
#ifdef DISK_HAS_BAK_FILES
				/* copy_file "file.nam" "file.bak; */
				build_bak_file_name(bak_file_name, file->file_name);
				cnt = sprintf(string, "copy_file \"%s\" \"%s\";\n", file->file_name, bak_file_name);
				err = file_write(script_fd, string, cnt);
				check_error(err, "writing string to file", "main");
#endif
			}
		}
	}
	fprintf(stderr, "done\n");
		
	/* free the directory lists */
	destroy_file_list(old_list);
	destroy_file_list(new_list);
		
	/* free the checksum lists */
	destroy_checksum_list(old_checksum);
	destroy_checksum_list(new_checksum);
		
	/* add the end of script setinel */
	/* end_script; */
	cnt = sprintf(string, "end_script;\n");
	err = file_write(script_fd, string, cnt);
	check_error(err, "writing string to file", "main");

	/* close the script file */
	err = file_close(script_fd);
	check_error(err, "closing script file", "main");
	
	return EXIT_SUCCESS;
}  /* main */

/*
 *		STATIC FUNCTIONS
 */

static file_list *build_file_list(char *dir_name)
{
	DIR *dir;
	file_list *head, *entry;
	struct dirent *de;
	int err;
	
	head = NULL;
	dir = opendir(dir_name);
	for (de = readdir(dir); de != NULL; de = readdir(dir)) {
		if (valid_file_name(de->d_name)) {
			entry = malloc(sizeof(file_list));
			err = entry == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
			check_error(err, "mallocing file list", "build_file_list");
			entry->next = head;
			strcpy(entry->file_name, de->d_name);
			head = entry;
		}
	}
	closedir(dir);
	return head;
}  /* build_file_list */

static checksum_list *build_checksum_list(char *dir_name)
{
	char path_filename[256];
	checksum_list *head, *entry;
	struct ffblk file_block;
	ulong crc;
	int f_size;
	int num_file;
	int fd;
	int err;
	
	/* build the complete path filename */
	build_path_filename(dir_name, "filesys.chk", path_filename);
	
	/* open the filesys.chk */
	err = file_open(path_filename, &fd);
	check_error(err, "opening filesys.chk file", "build_checksum_list");

	/* get the file size */
	err = file_size(fd, NULL, &f_size);
	check_error(err, "getting file size", "build_checksum_list");

	/* determine the number of records */
	num_file = f_size / (sizeof(file_block) + sizeof(ulong));
	
	/* one at a time read the record, get filename and crc */
	head = NULL;
	while (num_file-- > 0) {
		entry = malloc(sizeof(checksum_list));
		err = entry == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
		check_error(err, "mallocing checksum list", "build_checksum_list");
		entry->next = head;
		
		/* read the file block */
		err = file_read(fd, &file_block, sizeof(file_block));
		check_error(err, "reading file", "build_checksum_list");
		
		/* read the file crc */
		err = file_read(fd, &crc, sizeof(crc));
		check_error(err, "reading file", "build_checksum_list");
		
		/* save the data */
		strcpy(entry->file_name, file_block.ff_name);
		entry->checksum = crc;
		head = entry;
	}
	
	/* close the file */
	err = file_close(fd);
	check_error(err, "closing file", "build_checksum_list");
	return head;
}  /* build_checksum_list */

static void build_bak_file_name(char *bak_file, char *orig_file)
{
	char *p;
	
	strcpy(bak_file, orig_file);
	p = strrchr(bak_file, '.');
	if (p != NULL) {
		*p = '\0';
		strcat(bak_file, ".bak");
	}
}  /* build_bak_file_name */

static bool valid_file_name(char *fname)
{
	bool ret;
	
	/* assume it is a valid file */
	ret = TRUE;
	if (fname == NULL
		/* ignore the current directory */
		|| str_similiar(".", fname)
		/* ignore the upper directory */
		|| str_similiar("..", fname)
		/* ignore the game revision file */
		|| str_similiar("gameinf.rev", fname)
		/* ignore backup files, we will generate a copy_file for them */
		|| strstr_similiar(fname, ".BAK"))
		ret = FALSE;
	return ret;
}  /* valid_file_name */

static void destroy_file_list(file_list *head)
{
	file_list *entry;
	
	while (head != NULL) {
		entry = head->next;
		free(head);
		head = entry;
	}
}  /* destroy_file_list */

static void destroy_checksum_list(checksum_list *head)
{
	checksum_list *entry;
	
	while (head != NULL) {
		entry = head->next;
		free(head);
		head = entry;
	}
}  /* destroy_checksum_list */

static bool file_in_dir(file_list *head, char *file_name)
{
	while (head != NULL) {
		if (str_similiar(file_name, head->file_name))
			return TRUE;
		head = head->next;
	}
	return FALSE;
}  /* file_in_dir */

static bool str_similiar(char *a, char *b)
{
	int i, a_len, b_len;
	bool ret;
	
	a_len = strlen(a);
	b_len = strlen(b);
	if (a_len == b_len) {
		ret = TRUE;
		for (i = 0; i < a_len; i++)
			if (toupper(a[i]) != toupper(b[i])) {
				ret = FALSE;
				break;
			}
	} else
		ret = FALSE;
	return ret;
}  /* str_similiar */

static bool strstr_similiar(char *s, char *find)
{
	char c, sc;
	int len;
	
	if ((c = *find++) != 0) {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return FALSE;
			} while (sc != c);
		} while (strncmp_similiar(s, find, len) != 0);
		s--;
	}
	return TRUE;
}  /* strstr_similiar */

int strncmp_similiar(char *s1, char *s2, int n)
{

	if (n == 0)
		return 0;
	do {
		if (toupper(*s1) != toupper(*s2++))
			return *(uchar *)s1 - *(uchar *)--s2;
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	return 0;
}  /* strncmp_similiar */

static bool get_checksum(checksum_list *head, char *file_name, ulong *crc)
{
	while (head != NULL) {
		if (str_similiar(file_name, head->file_name)) {
			*crc = head->checksum;
			return TRUE;
		}
		head = head->next;
	}
	return FALSE;
}  /* get_checksum */

static void build_path_filename(char *path, char *file_name, char *path_filename)
{
	strcpy(path_filename, path);
	strcat(path_filename, "\\");
	strcat(path_filename, file_name);
}  /* build_path_filename */

static bool files_are_diff(checksum_list *old_list, checksum_list *new_list, char *old_dir, char *new_dir, char *file_name)
{
	ulong old_sum;
	ulong new_sum;
	bool old_exist;
	bool new_exist;
	
	old_exist = get_checksum(old_list, file_name, &old_sum);
	new_exist = get_checksum(new_list, file_name, &new_sum);
	if (old_exist && new_exist)
		return old_sum != new_sum;
	else {
		fprintf(stderr, "%s being diffed via slow method!\n", file_name);
		return files_are_diff2(old_dir, new_dir, file_name);
	}
}  /* files_are_diff */

static bool files_are_diff2(char *old_dir, char *new_dir, char *file_name)
{
	char old_file[256];
	char new_file[256];
	int old_size;
	int new_size;
	uchar *old_buffer;
	uchar *new_buffer;
	int old_fd;
	int new_fd;
	int err;
	int num_bytes;
	bool ret;
	
	/* build a path to the old rev of the file */
	build_path_filename(old_dir, file_name, old_file);
	
	/* build a path to the new rev of the file */
	build_path_filename(new_dir, file_name, new_file);
	
	err = file_size(-1, old_file, &old_size);
	check_error(err, "getting old file size", "files_are_diff");
	
	err = file_size(-1, new_file, &new_size);
	check_error(err, "getting new file size", "files_are_diff");
	
	/* check the file sizes, if different then patch the file */
	if (old_size != new_size)
		return TRUE;
	
	old_buffer = malloc(128 * 1024);
	err = old_buffer == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	check_error(err, "mallocing old file buffer", "files_are_diff");
	
	new_buffer = malloc(128 * 1024);
	err = new_buffer == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	check_error(err, "mallocing new file buffer", "files_are_diff");
	
	/* open the old file */
	err = file_open(old_file, &old_fd);
	check_error(err, "opening old file", "files_are_diff");
	
	/* open the new file */
	err = file_open(new_file, &new_fd);
	check_error(err, "opening new file", "files_are_diff");
	
	ret = FALSE;
	while (old_size > 0) {
		/* determine the number of bytes to read */
		num_bytes = old_size > sizeof(old_buffer) ? sizeof(old_buffer) : old_size;
		/* update the number of bytes left to process */
		old_size -= num_bytes;
		
		err = file_read(old_fd, old_buffer, num_bytes);
		check_error(err, "reading old file", "files_are_diff");
		
		err = file_read(new_fd, new_buffer, num_bytes);
		check_error(err, "reading new file", "files_are_diff");
		
		if (memcmp(old_buffer, new_buffer, num_bytes) != 0) {
			ret = TRUE;
			break;
		}
	}
	/* close the files */
	err = file_close(old_fd);
	check_error(err, "closing old file", "files_are_diff");
	
	err = file_close(new_fd);
	check_error(err, "closing new file", "files_are_diff");
	
	/* free the buffers */
	free(old_buffer);
	free(new_buffer);
	return ret;
}  /* files_are_diff2 */

static void check_error(int err, char *doing, char *where)
{
	if (err != UPDATE_NO_ERROR) {
		fprintf(stderr, "Error while %s in %s\n", doing, where);
		abort();
	}
}  /* check_error */

/*
 *		$History: buildscr.c $
 * 
 * *****************  Version 4  *****************
 * User: Markg        Date: 8/24/98    Time: 10:31a
 * Updated in $/video/tools/buildscr
 * 
 * *****************  Version 3  *****************
 * User: Markg        Date: 8/23/98    Time: 8:32p
 * Updated in $/video/tools/buildscr
 * no longer spit out copy_file to .bak and do not use the filesys.chk
 * file any more
 * 
 * *****************  Version 2  *****************
 * User: Markg        Date: 10/06/97   Time: 8:28p
 * Updated in $/video/tools/buildscr
 * Make the diff command much smarter, uses the filesys.chk database for
 * very fast diffs.  Added progress information.  Ignore .BAK files and
 * generate them via copy_file statements.
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 4:54p
 * Created in $/video/tools/buildscr
 * the source file for the build script tool.
 */
