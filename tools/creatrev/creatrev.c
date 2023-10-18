/*
 *		$Archive: /video/tools/creatrev/creatrev.c $
 *		$Revision: 1 $
 *		$Date: 10/01/97 9:46a $
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

static int create_revision_info(revision_id *rom_set_id, revision_desc rom_set_name);

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	revision_id rev_id;
	revision_desc rev_desc;
	int err, i;
	
	if (argc != 4) {
		fprintf(stderr, "Usage:%s game_id game_rev rev_desc\n", argv[0]);
		err = 1;
	} else {
		rev_id.game_id = atoi(argv[1]);
		rev_id.rev_level = atoi(argv[2]);
		strncpy(rev_desc, argv[3], 28);
		for (i = 0; i < 28; i++)
			if (rev_desc[i] == '_')
				rev_desc[i] = ' ';

		printf("game_id = %hu, rev_level = %hu, %28s\n", rev_id.game_id, rev_id.rev_level, rev_desc);
		err = create_revision_info(&rev_id, rev_desc);
	}
	return err == UPDATE_NO_ERROR ? EXIT_SUCCESS : EXIT_FAILURE;
}  /* main */

/*
 *		STATIC PROTOTYPES
 */

static int create_revision_info(revision_id *rom_set_id, revision_desc rom_set_name)
{
	int fd;
	int err, err2;
	
	/* delete the revision file if it already exists */
	if (file_exist(REVISION_ID_FILENAME))
		file_delete(REVISION_ID_FILENAME);
	
	/* create the new revision file */
	err = file_create(REVISION_ID_FILENAME, &fd);
	if (err == UPDATE_NO_ERROR) {
		/* write out the game id and software revision level */
		err = file_write(fd, rom_set_id, sizeof(revision_id));
		if (err == UPDATE_NO_ERROR)
			/* write out the ROM set name */
			err = file_write(fd, rom_set_name, sizeof(revision_desc));
		err2 = file_close(fd);
		if (err == UPDATE_NO_ERROR)
			err = err2;
	}
	return err;
}  /* create_revision_info */

/*
 *		$History: creatrev.c $
 * 
 * *****************  Version 1  *****************
 * User: Mlynch       Date: 10/01/97   Time: 9:46a
 * Created in $/video/tools/creatrev
 * Source for the creatrev tool
 */
