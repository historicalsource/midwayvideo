/*
 *		$Archive: /video/tools/pack/test.c $
 *		$Revision: 1 $
 *		$Date: 10/02/97 6:10p $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_test_c = "$Workfile: test.c $ $Revision: 1 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdlib.h>
#include <stdio.h>

/*
 *		USER INCLUDES
 */

#include "pack.h"
#include "getopt.h"
#include "as.h"
#include "util.h"

/*
 *		STATIC PROTOTYPES
 */

static void usage(char *prog_name);
static void dump_blocks(int num_blocks, block_t *blocks);
static void verify_data(int num_blocks, block_t *blocks, pack_params *params, pack_stats *stats);

/*
 *		GLOBAL FUNCTIONS
 */

int main(int argc, char *argv[])
{
	int opt;
	int tmp;
	int num_blocks, i, err;
	int rand_seed;
	block_t *list;
	pack_params params;
	pack_stats stats;
	
	/* set up the packing parameter defaults */
	params.pack_mode = PAGE_MODE;			/* 256 by 256 texture page */
	params.block_align = 1;					/* no block alignment or inset */
	params.page_phy_width = 256;
	params.page_phy_height = 256;
	params.left_inset = 0;
	params.top_inset = 0;
	params.right_inset = 0;
	params.bottom_inset = 0;
	params.debug = FALSE;

	rand_seed = 2084;
	num_blocks = 1000;
	while ((opt = getopt(argc, argv, "a:dmn:w:h:t:l:b:r:s:?")) != -1) {
		switch (opt) {
		case 'a':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp == 1 || tmp == 2 || tmp == 4)
				params.block_align = tmp;
			else {
				printf("-a block alignment must be 1, 2, or, 4\n");
				return EXIT_FAILURE;
			}
			break;
		case 'd':
			params.debug = TRUE;
			break;
		case 'm':
			params.pack_mode = LINE_MODE;
			break;
		case 'n':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp > 0)
				num_blocks = tmp;
			else {
				printf("-n number of blocks must be greater than zero\n");
				return EXIT_FAILURE;
			}
			break;
		case 'w':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp > 0)
				params.page_phy_width = tmp;
			else {
				printf("-w width must be greater than zero\n");
				return EXIT_FAILURE;
			}
			break;
		case 'h':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp > 0)
				params.page_phy_height = tmp;
			else {				
				printf("-h height must be greater than zero\n");
				return EXIT_FAILURE;
			}
			break;
		case 't':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp >= 0)
				params.top_inset = tmp;
			else {				
				printf("-t top_inset must be positive\n");
				return EXIT_FAILURE;
			}
			break;
		case 'l':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp >= 0)
				params.left_inset = tmp;
			else {				
				printf("-l left_inset must be positive\n");
				return EXIT_FAILURE;
			}
			break;
		case 'b':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp >= 0)
				params.bottom_inset = tmp;
			else {				
				printf("-b bottom_inset must be positive\n");
				return EXIT_FAILURE;
			}
			break;
		case 'r':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp >= 0)
				params.right_inset = tmp;
			else {				
				printf("-r right_inset must be postivie\n");
				return EXIT_FAILURE;
			}
			break;
		case 's':
			tmp = -1;
			if (getopt_arg != NULL)
				tmp = atoi(getopt_arg);
			if (tmp >= 0)
				rand_seed = tmp;
			else {
				printf("-s rand_seed must be positive\n");
				return EXIT_FAILURE;
			}
			break;
		case '?':
			usage(argv[0]);
			return EXIT_FAILURE;
			break;
		default:
			as_abort("uncaught case");
			break;
		}
	}
	
	/* build a set of random sized blocks */
	list = util_xmalloc(num_blocks * sizeof(block_t));
	srand(rand_seed);
	for (i = 0; i < num_blocks; i++) {
		list[i].block_width = rand() % 96 + rand() % 4 + 1;
		list[i].block_height = rand() % 64 + rand() % 4 + 1;
		list[i].block_data = NULL;
	}
	
	/* pack the blocks */
	err = pack(num_blocks, list, &params, &stats);
	
	if (err == PACK_NO_ERR) {
		dump_blocks(num_blocks, list);
		verify_data(num_blocks, list, &params, &stats);
		
		printf("\n");
		printf("%5d page%s used\n", stats.page_count, stats.page_count > 1 ? "s" : "");
		printf("%5d line%s per page\n", stats.line_count, stats.line_count > 1 ? "s" : "");
		printf("%5d line%s used\n", stats.total_line_count, stats.line_count > 1 ? "s" : "");
		printf("\n");
		printf("%5.2f%% packing efficiency\n", stats.pack_efficiency);
		if (stats.filled_spans > 0.0F)
			printf("%5.2f%% wasted on filled spans\n", stats.filled_spans);
		if (stats.filled_alignment > 0.0F)
			printf("%5.2f%% wasted on alignment\n", stats.filled_alignment);
		if (stats.filled_boarder > 0.0F)
			printf("%5.2f%% wasted on boarders\n", stats.filled_boarder);
		if (stats.filled_end_page > 0.0F)
			printf("%5.2f%% wasted at end of page\n", stats.filled_end_page);
		if (stats.filled_end_last_page > 0.0F)
			printf("%5.2f%% wasted at end of last page\n", stats.filled_end_last_page);
	} else {
		switch (err) {
		case PACK_NO_PARAM_BLOCK:
			printf("No parameter block passed\n");
			break;			
		case PACK_BAD_BLOCK_COUNT:
			printf("Invalid block count passed\n");
			break;			
		case PACK_NO_BLOCK_LIST:
			printf("No block list passed\n");
			break;
		case PACK_BAD_PACK_MODE:
			printf("Bad packing mode\n");
			break;
		case PACK_BAD_BLOCK_ALIGN:
			printf("Bad block alignment\n");
			break;
		case PACK_BAD_PAGE_PHY_WIDTH:
			printf("Bad page width\n");
			break;
		case PACK_BAD_PAGE_PHY_HEIGHT:
			printf("Bad page height\n");
			break;
		case PACK_BAD_LEFT_INSET:
			printf("Bad left inset\n");
			break;
		case PACK_BAD_TOP_INSET:
			printf("Bad top inset\n");
			break;
		case PACK_BAD_RIGHT_INSET:
			printf("Bad right inset\n");
			break;
		case PACK_BAD_BOTTOM_INSET:
			printf("Bad bottom inset\n");
			break;
		case PACK_BAD_BLOCK_WIDTH:
			printf("Bad block width\n");
			break;
		case PACK_BAD_BLOCK_HEIGHT:
			printf("Bad bock height\n");
			break;
		default:
			printf("Unknown error occurred\n");
			break;
		}
	}
	free(list);
	return EXIT_SUCCESS;
}  /* main */

/*
 *		STATIC FUNCTIONS
 */

static void usage(char *prog_name)
{
	printf("Usage:%s [options] file\n", prog_name);
	printf("\t-m              line mode defaults to page mode\n");
	printf("\t-d              enable debugging info\n");
	printf("\t-s rand_seed    random number seed\n");
	printf("\t-a 1 | 2 | 4    block alignment defaults to 1\n");
	printf("\t-n num_blocks   number of blocks to pack defaults to 1000\n");
	printf("\t-w page_width   page width defaults to 256\n");
	printf("\t-h page_height  page height defaults to 256\n");
	printf("\t-t top_inset    top page inset defaults to 0\n");
	printf("\t-l left_inset   left page inset defaults to 0\n");
	printf("\t-b bottom_inset bottom page inset defaults to 0\n");
	printf("\t-r right_inset  right page inset defaults to 0\n");
}  /* usage */

static void dump_blocks(int num_blocks, block_t *blocks)
{
	int i;
	
	for (i = 0; i < num_blocks; i++)
		printf("width = %4d, height = %4d, page = %4d, xoff = %4d, yoff = %4d\n", blocks[i].block_width, blocks[i].block_height, blocks[i].block_texture_page, blocks[i].block_x_offset, blocks[i].block_y_offset);
}  /* dump_blocks */

static void verify_data(int num_blocks, block_t *blocks, pack_params *params, pack_stats *stats)
{
	uchar **page_list;
	uchar *data, *page;
	block_t *b;
	int i, j, k;
	int page_left, page_top, page_right, page_bottom;
	
	page_left = ROUND2(params->left_inset, params->block_align);
	page_right = params->page_phy_width - params->right_inset;
	page_top = params->top_inset;
	if (params->pack_mode == LINE_MODE)
		page_bottom = stats->line_count - params->bottom_inset;
	else
		page_bottom = params->page_phy_height - params->bottom_inset;
	
	/* allocate the pages */
	page_list = util_xmalloc(stats->page_count * sizeof(uchar *));
	for (i = 0; i < stats->page_count; i++) {
		page_list[i] = util_xmalloc(params->page_phy_width * stats->line_count * sizeof(uchar));
		for (j = 0; j < params->page_phy_width * stats->line_count; j++)
			page_list[i][j] = 0xFF;
	}
	printf("texture space allocated\n");
	
	/* generate data for all of the blocks */
	for (i = 0; i < num_blocks; i++) {
		b = &blocks[i];
		data = util_xmalloc(b->block_width * b->block_height * sizeof(uchar));
		b->block_data = data;
		srand((uint)b->block_data);
		for (j = 0; j < b->block_width * b->block_height; j++)
			data[j] = rand() % 256;
	}
	printf("data generated\n");
	
	/* copy the data into the pages */
	for (i = 0; i < num_blocks; i++) {
		b = &blocks[i];
		data = (uchar *)b->block_data;
		page = page_list[b->block_texture_page] + (b->block_y_offset * params->page_phy_width + b->block_x_offset);
		for (j = 0; j < b->block_height; j++) {
			for (k = 0; k < b->block_width; k++) {
				page[k] = data[k];
			}
			data += b->block_width;
			page += params->page_phy_width;
		}
	}
	printf("data copied\n");
	
	/* verify that the data is the same in the page as the block */
	for (i = 0; i < num_blocks; i++) {
		b = &blocks[i];
		data = (uchar *)b->block_data;
		page = page_list[b->block_texture_page] + (b->block_y_offset * params->page_phy_width + b->block_x_offset);
		for (j = 0; j < b->block_height; j++) {
			for (k = 0; k < b->block_width; k++) {
				if (page[k] != data[k])
					as_abort("compare error!\n");
			}
			data += b->block_width;
			page += params->page_phy_width;
		}
	}
	printf("data verified\n");
	
	/* insure the inset border is empty */
	for (i = 0; i < stats->page_count; i++) {
		/* top inset */
		for (j = 0; j < page_top; j++) {
			page = page_list[i] + (j * params->page_phy_width);
			for (k = 0; k < params->page_phy_width; k++) {
				if (page[k] != 0xFF)
					as_abort("top inset border corrupted!\n");
			}
		}
		/* bottom inset */
		for (j = page_bottom; j < stats->line_count; j++) {
			page = page_list[i] + (j * params->page_phy_width);
			for (k = 0; k < params->page_phy_width; k++) {
				if (page[k] != 0xFF)
					as_abort("bottom inset border corrupted!\n");
			}
		}
		/* left and right inset */
		for (j = 0; j < stats->line_count; j++) {
			page = page_list[i] + (j * params->page_phy_width);
			/* left */
			for (k = 0; k < page_left; k++) {
				if (page[k] != 0xFF)
					as_abort("left inset border corrupted!\n");
			}
			/* right */
			for (k = page_right; k < params->page_phy_width; k++) {
				if (page[k] != 0xFF)
					as_abort("right inset border corrupted!\n");
			}
		}
	}
	printf("inset border data verified\n");
	
	/* free the data for all of the blocks */
	for (i = 0; i < num_blocks; i++) {
		b = &blocks[i];
		free(b->block_data);
		b->block_data = NULL;
	}
	printf("data freed\n");

	/* free up the memeory used in the tests */
	for (i = 0; i < stats->page_count; i++)
		free(page_list[i]);
	free(page_list);
	printf("texture space deallocated\n");
}  /* verify_data */

/*
 *		$History: test.c $
 * 
 * *****************  Version 1  *****************
 * User: Markg        Date: 10/02/97   Time: 6:10p
 * Created in $/video/tools/pack
 * the sample packer test source file
 */
