/*
 *		$Archive: /video/tools/printrev/printrev.c $
 *		$Revision: 1 $
 *		$Date: 10/02/97 11:39a $
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

#define REVISION_ID_FILENAME		"GAMEINF.REV"		/* disk file to save current revision id in */

/*
 *		STATIC PROTOTYPES
 */

static int read_revision_info(char *file_name, revision_id *curr_rom_set_id, revision_desc curr_rom_set_name);

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	revision_id rev_id;
	revision_desc rev_desc;
	char *fname;
	int err;
	
	if (argc != 1 && argc != 2) {
		fprintf(stderr, "Usage:%s [rev_file]\n", argv[0]);
		err = 1;
	} else {
		fname = argc == 1 ? REVISION_ID_FILENAME : argv[1];
		err = read_revision_info(fname, &rev_id, rev_desc);
		if (err == UPDATE_NO_ERROR)
			printf("game_id = %hu, rev_level = %hu, %28s\n", rev_id.game_id, rev_id.rev_level, rev_desc);
		else
			printf("Error reading revision file information from \'%s\'\n", fname);
	}
	return err == UPDATE_NO_ERROR ? EXIT_SUCCESS : EXIT_FAILURE;
}  /* main */

/*
 *		STATIC PROTOTYPES
 */

static int read_revision_info(char *file_name, revision_id *curr_rom_set_id, revision_desc curr_rom_set_name)
{
	int fd;
	int err, err2;
	
	/* open the revision file */
	err = file_open(file_name, &fd);
	if (err == UPDATE_NO_ERROR) {
		/* read the ROM id:game number and software revision level */
		err = file_read(fd, curr_rom_set_id, sizeof(revision_id));

		if (err == UPDATE_NO_ERROR)
			/* read the ROM set name */
			err = file_read(fd, curr_rom_set_name, sizeof(revision_desc));

		/* close the file */
		err2 = file_close(fd);
		if (err == UPDATE_NO_ERROR)
			err = err2;
	}
	return err;
}  /* read_revision_info */

/*
 *		$History: printrev.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 11:39a
 * Created in $/video/tools/printrev
 * Tool for prinit the contents of a GAMEREV.INF file
 */
