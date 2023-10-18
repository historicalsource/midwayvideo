/*
 *		$Archive: /video/tools/c_hack/pack.c $
 *		$Revision: 1 $
 *		$Date: 7/12/98 4:29p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_pack_c = "$Workfile: pack.c $ $Revision: 1 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include "pack.h"
#include "bagger.h"
#include "util.h"

/*
 *		DEFINES
 */

#define NOT_IN_PAGE		-1

/*
 *		TYPEDEFS
 */

typedef struct _block_list_t {
	struct _block_list_t *list_next;	/* next block in list */
	block_t *list_data;					/* pointer to block data structure */
} block_list_t;

enum {
	MIN_WIDTH,
	MIN_HEIGHT,
	MAX_WIDTH,
	MAX_HEIGHT,
	CLOSEST_WIDTH,
	CLOSEST_HEIGHT,
	EQ_WIDTH,
	EQ_HEIGHT,
	LE_WIDTH,
	LE_HEIGHT
};

/*
 *		STATIC PROTOTYPES
 */

static int load_blocks(int num_blocks, block_t *blocks);
static void unload_blocks(void);
static void pack_blocks(void);
static block_list_t *find_best_block_list(int *y_level_map, int top, int left, int *axis);
static block_list_t *fill_entire_span(int height);
static block_list_t *fill_span_height_bias(int width, int prefer_height, int max_height, int *best_pix_count);
static block_list_t *fill_span_height_bias_sub(int width, int height, int *best_pix_count);
static block_list_t *fill_span_width_bias(int width, int prefer_height, int max_height, int *best_pix_count);
static block_list_t *fill_span_width_bias_sub(int width, int height, int *best_pix_count);
static block_list_t *pick_list(block_list_t *h_list, block_list_t *w_list, int h_pix, int w_pix, int *axis);
static void reset_page(int *curr_pack_page, int *wrap_count, int *y_level_map, int *top, int *left);
static bool valid_block(block_t *block);
static int get_span_width(int *y_level_map, int start_index);
static void reset_left(int *y_level_map, int *left);
static void set_span_height(int *y_level_map, int start_index, int span_width, int value);
static void find_left_right_span(int *y_level_map, int start_index, int span_width, int *left, int *right);
static block_list_t *find_block_list(block_list_t *src_list, int which_blocks, int *value);
static void fill_bad_spans(int *y_level_map);
static bool width_eq(block_t *block_data, int value);
static bool height_eq(block_t *block_data, int value);
static bool width_le(block_t *block_data, int value);
static bool height_le(block_t *block_data, int value);
static block_list_t *build_block_list(block_list_t *src_list, bool (*cmp)(block_t *block_data, int value), int value);
static void destroy_block_list(block_list_t *src_list);
static int count_block_list(block_list_t *src_list);
static void list_to_bagger(block_list_t *src_list, int which, int *num_value, int **value);
static block_list_t *set_to_list(set *result, int goal, int num_value, int *value, block_list_t *src_list, int which);
static void calc_stats(pack_stats *stats);

/*
 *		STATIC VARIABLES
 */

static pack_params *params;				/* global reference to packing parameters */
static int page_log_width;				/* logical width of texture page */
static int page_log_height;				/* logical height of texture page */
static int page_left;					/* logical packing left */
static int page_top;					/* logical packing top */
static int page_bottom;					/* logical packing bottom, meaningless in LINE_MODE */
static int page_right;					/* logical packing right */
static int max_block_height;			/* tallest block height */
static int max_block_width;				/* widest block width */
static block_list_t *block_list;		/* list of all blocks */
static block_list_t **width_table;		/* table of block lists sorted by width */
static block_list_t **height_table;		/* table of block lists sorted by height */
static uint filled_pix;					/* total number of pixels wasted in filled spans */
static uint page_pix;					/* total number of pixels in current page */
static uint end_page_pix;				/* total number of pixels wasted at the end of a page */
static uint last_page_pix;				/* number of pixels wasted at the end of the last page */
static uint boarder_pix;				/* number of pixels wasted on the inset border */

/*
 *		GLOBAL FUNCTIONS
 */

int pack(int num_blocks, block_t *blocks, pack_params *packing_params, pack_stats *packing_stats)
{
	int align_left_inset;
	int err;
	
	/* be sure we got passed a parameter block */
	if (packing_params == NULL)
		return PACK_NO_PARAM_BLOCK;
	
	/* check there is a valid number of blocks */
	if (num_blocks < 0)
		return PACK_BAD_BLOCK_COUNT;
	
	/* quick exit if no data to pack */
	if (num_blocks == 0)
		return PACK_NO_ERR;
	
	/* be sure we got passed a block list */
	if (blocks == NULL)
		return PACK_NO_BLOCK_LIST;
	
	/* make the packing parameters global */
	params = packing_params;
	
	/* validate the params */
	if (params->pack_mode != PAGE_MODE && params->pack_mode != LINE_MODE)
		return PACK_BAD_PACK_MODE;
	if (params->block_align != 1 && params->block_align != 2 && params->block_align != 4)
		return PACK_BAD_BLOCK_ALIGN;
	if (params->page_phy_width < 1)
		return PACK_BAD_PAGE_PHY_WIDTH;
	if (params->page_phy_height < 1)
		return PACK_BAD_PAGE_PHY_HEIGHT;
	if (params->left_inset < 0 || params->left_inset >= params->page_phy_width)
		return PACK_BAD_LEFT_INSET;
	if (params->top_inset < 0 || params->top_inset >= params->page_phy_height)
		return PACK_BAD_TOP_INSET;
	if (params->right_inset < 0 || params->right_inset >= params->page_phy_width)
		return PACK_BAD_RIGHT_INSET;
	if (params->bottom_inset < 0 || params->bottom_inset >= params->page_phy_height)
		return PACK_BAD_BOTTOM_INSET;
	
	/* start the left side at the proper alignment */
	align_left_inset = ROUND2(params->left_inset, params->block_align);
	
	/* calculate the logical size of the texture page */
	page_log_width = params->page_phy_width - (align_left_inset + params->right_inset);
	page_log_height = params->page_phy_height - (params->top_inset + params->bottom_inset);
	
	/* calculate the fill boundarys */
	page_left = align_left_inset;
	page_right = params->page_phy_width - params->right_inset;
	page_top = params->top_inset;
	page_bottom = params->page_phy_height - params->bottom_inset;
	
	/* initialize the stat tracking variables */
	filled_pix = 0;
	page_pix = 0;
	end_page_pix = 0;
	last_page_pix = 0;
	boarder_pix = 0;
	
	err = load_blocks(num_blocks, blocks);
	if (err == PACK_NO_ERR) {
		pack_blocks();
		if (packing_stats != NULL)
			calc_stats(packing_stats);
	}
	unload_blocks();
	return err;
}  /* pack */

/*
 *		STATIC FUNCTIONS
 */

static int load_blocks(int num_blocks, block_t *blocks)
{
	block_list_t *new_elem;
	int i;
	
	/* start the list empty */
	block_list = NULL;
	
	for (i = 0; i < num_blocks; i++) {
		blocks[i].block_texture_page = -1;
		blocks[i].block_x_offset = -1;
		blocks[i].block_y_offset = -1;
		blocks[i].block_align_width = ROUND2(blocks[i].block_width, params->block_align);
		
		new_elem = util_xmalloc(sizeof(block_list_t));
		new_elem->list_next = block_list;
		new_elem->list_data = &blocks[i];
		block_list = new_elem;
	}
	
	max_block_width = 0;
	max_block_height = 0;
	for (new_elem = block_list; new_elem != NULL; new_elem = new_elem->list_next) {
		/* check for valid width */
		if ((new_elem->list_data->block_align_width > page_log_width) || (new_elem->list_data->block_align_width < 1))
			return PACK_BAD_BLOCK_WIDTH;
		/* check for valid height */
		if (new_elem->list_data->block_height < 1)
			return PACK_BAD_BLOCK_HEIGHT;
		if ((params->pack_mode != LINE_MODE) && (new_elem->list_data->block_height > page_log_height))
				return PACK_BAD_BLOCK_HEIGHT;
		/* find max block width */
		if (new_elem->list_data->block_align_width > max_block_width)
			max_block_width = new_elem->list_data->block_align_width;
		/* find max block height */
		if (new_elem->list_data->block_height > max_block_height)
			max_block_height = new_elem->list_data->block_height;
	}
	return PACK_NO_ERR;
}  /* load_blocks */

static void unload_blocks(void)
{
	block_list_t *block;
	block_list_t *next;
	
	for (block = block_list; block != NULL; block = next) {
		next = block->list_next;
		free(block);
	}
	block_list = NULL;
}  /* unload_blocks */

/* calculate the positions for all of the blocks */
static void pack_blocks(void)
{
	block_list_t *next_block, *blk;
	int *y_level_map;					/* contour map of the bottom of the page */
	int i;								/* loop index */
	int top, left;						/* the top and left of the spanning being filled */
	int curr_pack_page;					/* which texture page we are currently filling up */
	int axis;							/* which axis the block list packs along */
	int wrap_count;						/* counter to check when page is filled */
	
	/* allocate the height contour map */
	y_level_map = util_xmalloc(params->page_phy_width * sizeof(int));
	
	/* start with the first texture page, reset_page will bump it to 0 */
	curr_pack_page = -1;
	
	/* initialize the packing parameters */
	reset_page(&curr_pack_page, &wrap_count, y_level_map, &top, &left);
	
	/* generate the width sorted table */
	/* width table is used when, we have a span, but not block of the exact height to fill it */
	/* use a bagger routine to get the best set of blocks with the same width to stack in */
	/* that span which best approximate the desired height */
	width_table = util_xmalloc((max_block_width + 1) * sizeof(block_list_t *));
	width_table[0] = NULL;
	for (i = 1; i < max_block_width + 1; i++)
		width_table[i] = find_block_list(block_list, EQ_WIDTH, &i);
	
	/* generate the height sorted table */
	/* height table is used to find the tallest, widest span of blocks to place */
	height_table = util_xmalloc((max_block_height + 1) * sizeof(block_list_t *));
	height_table[0] = NULL;
	for (i = 1; i < max_block_height + 1; i++)
		height_table[i] = find_block_list(block_list, EQ_HEIGHT, &i);

	while (count_block_list(block_list) > 0) {
		/* elimate any spans that can not be filled by a block */
		fill_bad_spans(y_level_map);
		
		/* check to see if fill_bad_spans extended our span behind us */
		reset_left(y_level_map, &left);
		
		/* find the current top */
		top = y_level_map[left];
		
		/* select the next best set of block to be placed */
		next_block = find_best_block_list(y_level_map, top, left, &axis);
		if (next_block != NULL) {
			if (axis == EQ_WIDTH) {
				/* stack the blocks side by side */
				for (blk = next_block; blk != NULL; blk = blk->list_next) {
					/* place the block */
					blk->list_data->block_texture_page = curr_pack_page;
					blk->list_data->block_x_offset = left;
					blk->list_data->block_y_offset = top;
					
					/* update the contour map */
					set_span_height(y_level_map, left, blk->list_data->block_align_width, top + blk->list_data->block_height);
					/* update stats */
					page_pix += blk->list_data->block_align_width * blk->list_data->block_height;
					
					/* output debug info */
					if (params->debug) {
						if (blk != next_block)
							printf("\t");
						printf("WIDTH:left %d->%d, top %d->%d\n", left, left + blk->list_data->block_align_width - 1, top, top + blk->list_data->block_height - 1);
					}
					
					/* update the left */
					left += blk->list_data->block_align_width;
				}
			} else /* axis == EQ_HEIGHT */{
				int block_width;
				
				block_width = 0;
				/* stack the blocks one on top of the other */
				for (blk = next_block; blk != NULL; blk = blk->list_next) {
					block_width = blk->list_data->block_align_width;
					
					/* place the block */
					blk->list_data->block_texture_page = curr_pack_page;
					blk->list_data->block_x_offset = left;
					blk->list_data->block_y_offset = top;
					
					/* update the contour map */
					set_span_height(y_level_map, left, block_width, top + blk->list_data->block_height);
					
					/* update stats */
					page_pix += blk->list_data->block_align_width * blk->list_data->block_height;
					
					/* output debug info */
					if (params->debug) {
						if (blk != next_block)
							printf("\t");
						printf("HEIGHT:left %d->%d, top %d->%d\n", left, left + block_width - 1, top, top + blk->list_data->block_height - 1);
					}
					
					/* update the top */
					top += blk->list_data->block_height;
				}
				/* skip to the next span */
				left += block_width;
			}
			
			/* placed a block, reset the wrap count */
			wrap_count = 0;
			destroy_block_list(next_block);
		} else {
			/* output debug info */
			if (params->debug)
				printf("skipping span %d->%d, %d\n", left, left + get_span_width(y_level_map, left) - 1, top);
			
			/* skip this span */
			left += get_span_width(y_level_map, left);
		}
		
		/* check to see if the span ended at the edge of the page */
		if (left == page_right) {
			left = page_left;
			wrap_count++;
		}
		
		/* detect when the texture page is full */
		/* if we have wrapped the left pointer twice without */
		/* placing a block, then we could not place a block */
		/* in any of the remaining spans, and the page is full */
		if (wrap_count == 2) {
			/* update stats */
			end_page_pix += (params->page_phy_width * params->page_phy_height) - page_pix;
			
			/* output debug info */
			if (params->debug)
				printf("spilling to new page!\n");
			
			reset_page(&curr_pack_page, &wrap_count, y_level_map, &top, &left);
		}
	}
	
	/* free all the work buffers */
	for (i = 0; i < max_block_width + 1; i++)
		destroy_block_list(width_table[i]);
	for (i = 0; i < max_block_height + 1; i++)
		destroy_block_list(height_table[i]);
	free(y_level_map);
	free(width_table);
	free(height_table);
}  /* pack_blocks */

static block_list_t *find_best_block_list(int *y_level_map, int top, int left, int *axis)
{
	block_list_t *best_list;
	int left_level, right_level;
	int span_width;
	int max_height;
	
	best_list = NULL;
	
	/* find the width of the next span */
	span_width = get_span_width(y_level_map, left);
	
	/* find the height of the adjacent spans */
	find_left_right_span(y_level_map, left, span_width, &left_level, &right_level);
	
	/* determine the tallest block that should be searched for */
	max_height = params->pack_mode == LINE_MODE ? max_block_height : page_bottom - top;
	
	if (left_level == NOT_IN_PAGE && right_level == NOT_IN_PAGE) {
		/* span is the width of the entire page */
		best_list = fill_entire_span(max_height);
		*axis = EQ_WIDTH;
		
		/* output debug info */
		if (params->debug)
			printf("ENTIRE SPAN:");
		
	} else if (left_level == NOT_IN_PAGE || right_level == NOT_IN_PAGE) {
		/* span is at the left or right edge of page */
		block_list_t *h_list, *w_list;
		int h_pix, w_pix;
		int height;
		
		height = left_level == NOT_IN_PAGE ? right_level : left_level;
		/* ensure the span is actually lower than adjacent span */
		if (height > top) {
			h_list = fill_span_height_bias(span_width, height - top, max_height, &h_pix);
			w_list = fill_span_width_bias(span_width, height - top, max_height, &w_pix);
			best_list = pick_list(h_list, w_list, h_pix, w_pix, axis);
		}
		
		/* output debug info */
		if (params->debug)
			printf("EDGE:");
		
	} else {
		/* span is between two blocks */
		block_list_t *left_list, *right_list;
		int left_pix, right_pix;
		int laxis, raxis;
		
		left_list = NULL;
		right_list = NULL;
		left_pix = 0;
		right_pix = 0;
		/* ensure the span is actually lower than left span */
		if (left_level > top) {
			block_list_t *h_list, *w_list;
			int h_pix, w_pix;
				
			h_list = fill_span_height_bias(span_width, left_level - top, max_height, &h_pix);
			w_list = fill_span_width_bias(span_width, left_level - top, max_height, &w_pix);
			left_list = pick_list(h_list, w_list, h_pix, w_pix, &laxis);
		}
		
		/* ensure the span is actually lower than right span */
		if (right_level > top) {
			block_list_t *h_list, *w_list;
			int h_pix, w_pix;
				
			h_list = fill_span_height_bias(span_width, right_level - top, max_height, &h_pix);
			w_list = fill_span_width_bias(span_width, right_level - top, max_height, &w_pix);
			right_list = pick_list(h_list, w_list, h_pix, w_pix, &raxis);
		}
		if (left_list == NULL && right_list == NULL) {
			/* can happen if span is taller than left and right blocks */
			best_list = NULL;
		} else if (left_list == NULL || right_list == NULL) {
			if (left_list != NULL) {
				best_list = left_list;
				*axis = laxis;
			} else {
				best_list = right_list;
				*axis = raxis;
			}
		} else {
			int dummy;
			
			best_list = pick_list(left_list, right_list, left_pix, right_pix, &dummy);
			*axis = best_list == left_list ? laxis : raxis;
		}
		
		/* output debug info */
		if (params->debug)
			printf("MIDDLE:");
	}
	return best_list;
}  /* find_best_block_list */

/* span is the entire width of the page */
/* pick a set of blocks that are: */
/* 1) all the same height */
/* 2) the total of their widths comes as close as possible to the page width */
/* 3) choose the set whose summed widths is maximized(with same height) */
/* 4) in case of tie on summed widths, go with the taller set of blocks */
static block_list_t *fill_entire_span(int max_height)
{
	block_list_t *new_list, *best_list;
	int new_delta, best_delta;
	int i;	
	
	best_list = NULL;
	best_delta = page_log_width + 1;

	if (max_height > max_block_height)
		max_height = max_block_height;

	/* search all of the height, tallest to shortest */
	/* need to ensure that the blocks will fit in the page */
	for (i = max_height; i >= 1; i--) {
		int num_value;
		int *value;
		int ret;
		set result;

		/* build a list of blocks widths to feed to the bagger routine */
		list_to_bagger(height_table[i], EQ_WIDTH, &num_value, &value);
		
		if (num_value > 0) {
			/* find the set of widths that comes as close to the page width as possible */
			ret = bagger(page_log_width, num_value, value, &result);
			
			/* build a set of blocks from the width values */
			new_list = set_to_list(&result, page_log_width, num_value, value, height_table[i], EQ_WIDTH);
			free(value);
			
			/* check how good of a fit the set is */
			new_delta = page_log_width - result.total;
			
			if (ret == +1) {
				/* tallest perfect solution */
				if (best_list != NULL)
					destroy_block_list(best_list);
				best_delta = new_delta;
				best_list = new_list;
				break;
			} else if (ret == 0) {
				/* got a nonperfect solution, see if it is a better width fit than before */
				/* if so, choose this, it is a better fit widthwise, but shortest */
				/* the goal is to maximize the width to make the fewest possible unfillable spans */
				if (new_delta < best_delta) {
					if (best_list != NULL)
						destroy_block_list(best_list);
					best_delta = new_delta;
					best_list = new_list;
				}
			}
		}
	}
	return best_list;
}  /* fill_entire_span */

/* find the set of blocks that will best fill the span using blocks that are the same height */
/* placed side by side */
static block_list_t *fill_span_height_bias(int width, int prefer_height, int max_height, int *best_pix_count)
{
	block_list_t *list;
	
	list = fill_span_height_bias_sub(width, prefer_height, best_pix_count);
	if (list == NULL) {
		list = fill_span_height_bias_sub(width, max_height, best_pix_count);
		*best_pix_count = -*best_pix_count;
	}
	return list;
}  /* fill_span_height_bias */

static block_list_t *fill_span_height_bias_sub(int width, int height, int *best_pix_count)
{
	block_list_t *new_list, *best_list;
	int new_pix_count;
	int i;	
	
	/* start with no best choice */
	best_list = NULL;
	*best_pix_count = 0;
	
	if (height > max_block_height)
		height = max_block_height;
	
	/* search all of the height, tallest to shortest */
	for (i = height; i >= 1; i--) {
		set result;
		block_list_t *blk;
		int *value;
		int num_value;
		int ret;
		
		list_to_bagger(height_table[i], EQ_WIDTH, &num_value, &value);
		if (num_value > 0) {
			ret = bagger(width, num_value, value, &result);
			new_list = set_to_list(&result, width, num_value, value, height_table[i], EQ_WIDTH);
			free(value);
			
			if (ret >= 0) {
				new_pix_count = 0;
				for (blk = new_list; blk != NULL; blk = blk->list_next)
					new_pix_count += blk->list_data->block_align_width * blk->list_data->block_height;
				/* use >= so we can trade up to a shorter span */
				if (new_pix_count >= *best_pix_count) {
					if (best_list != NULL)
						destroy_block_list(best_list);
					*best_pix_count = new_pix_count;
					best_list = new_list;
				}
			}
		}
	}
	return best_list;
}  /* fill_span_height_bias_sub */

/* find the set of blocks that will best fill the span using blocks that are the same width */
/* placed on top of each other */
static block_list_t *fill_span_width_bias(int width, int prefer_height, int max_height, int *best_pix_count)
{
	block_list_t *list;
	
	list = fill_span_width_bias_sub(width, prefer_height, best_pix_count);
	if (list == NULL) {
		list = fill_span_width_bias_sub(width, max_height, best_pix_count);
		*best_pix_count = -*best_pix_count;
	}
	return list;
}  /* fill_span_width_bias */

/* find the set of blocks that fill the highest percentage of space */
/* blocks will all be the same width, stack on top of each other */
static block_list_t *fill_span_width_bias_sub(int width, int height, int *best_pix_count)
{
	block_list_t *new_list, *best_list;
	int new_pix_count;
	int i;	
	
	/* start with no best choice */
	best_list = NULL;
	*best_pix_count = 0;
	
	if (width > max_block_width)
		width = max_block_width;

	/* search all of the widths, widest to narrowest */
	for (i = width; i >= 1; i--) {
		set result;
		block_list_t *blk;
		int *value;
		int num_value;
		int ret;
		
		list_to_bagger(width_table[i], EQ_HEIGHT, &num_value, &value);
		if (num_value > 0) {
			ret = bagger(height, num_value, value, &result);
			new_list = set_to_list(&result, height, num_value, value, width_table[i], EQ_HEIGHT);
			free(value);
			
			if (ret >= 0) {
				new_pix_count = 0;
				for (blk = new_list; blk != NULL; blk = blk->list_next)
					new_pix_count += blk->list_data->block_align_width * blk->list_data->block_height;
				if (new_pix_count > *best_pix_count) {
					if (best_list != NULL)
						destroy_block_list(best_list);
					*best_pix_count = new_pix_count;
					best_list = new_list;
				}
			}
		}
	}
	return best_list;
}  /* fill_span_width_bias_sub */

/* pick the better list of blocks to use.  pix_a and pix_b are the number of pixels covered by the */
/* list of blocks, if negative the blocks fill more the ideal span height. */
/* if either are 0% choose the other one */
/* if both are < 100% coverage select the larger coverage list */
/* if one is > 100% choose the < 100% list */
/* if both are > 100% choose the larger coverage list */
static block_list_t *pick_list(block_list_t *h_list, block_list_t *w_list, int h_pix, int w_pix, int *axis)
{
	block_list_t *best;
	
	if (h_pix == 0 || w_pix == 0) {
		best = h_pix == 0 ? w_list : h_list;
	} else if (h_pix > 0 && w_pix > 0) {
		best = h_pix > w_pix ? h_list : w_list;
	} else if (h_pix > 0 || w_pix > 0) {
		best = h_pix > 0 ? h_list : w_list;
	} else /* h_pix < 0 && w_pix < 0 */{
		h_pix = ABS(h_pix);
		w_pix = ABS(w_pix);
		best = h_pix > w_pix ? h_list : w_list;
	}
	if (best == h_list) {
		*axis = EQ_WIDTH;
		destroy_block_list(w_list);
	} else {
		*axis = EQ_HEIGHT;
		destroy_block_list(h_list);
	}
	return best;
}  /* pick_list */

static void reset_page(int *curr_pack_page, int *wrap_count, int *y_level_map, int *top, int *left)
{
	/* bump up the texture page index */
	(*curr_pack_page)++;
	
	/* reset the wrap_count */
	*wrap_count = 0;
	
	/* reset the contour map */
	set_span_height(y_level_map, page_left, page_log_width, page_top);
	
	/* start packing at the top left corner */
	*top = page_top;
	*left = page_left;
	
	/* start with the boarder pixel count */
	if (params->pack_mode == LINE_MODE) {
		page_pix = 0;
	} else {
		page_pix = page_top * params->page_phy_width;
		page_pix += (params->page_phy_height - page_bottom) * params->page_phy_width;
		page_pix += (page_top + params->page_phy_height - page_bottom) * (params->page_phy_width - page_log_width);
		boarder_pix += page_pix;
	}
}  /* reset_page */

static bool valid_block(block_t *block)
{
	if (block == NULL) {
		fprintf(stderr, "valid_block was passed a NULL pointer\n");
		abort();
	}
	return block->block_texture_page == -1;
}  /* valid_block */

/* find the width of the span starting at start_index in the contour map */
static int get_span_width(int *y_level_map, int start_index)
{
	int i, start_level;
	
	i = start_index;
	start_level = y_level_map[start_index];
	while ((i < page_right) && (start_level == y_level_map[i]))
		i++;
	return i - start_index;
}  /* get_span_width */

static void reset_left(int *y_level_map, int *left)
{
	int i, start_level;
	
	i = *left;
	start_level = y_level_map[i];
	while ((i > page_left) && (start_level == y_level_map[i - 1]))
		i--;
	
	/* output debug info */
	if (params->debug) {
		if (i != *left)
			printf("backedup %d->%d, %d\n", *left, i, start_level);
	}
	
	*left = i;
}  /* reset_left */

/* set the span starting at start_index and span_width wide to the specified height value */
static void set_span_height(int *y_level_map, int start_index, int span_width, int value)
{
	int i;
	
	if ((start_index + span_width) > page_right) {
		fprintf(stderr, "valid_block was passed a NULL pointer\n");
		abort();
	}
	for (i = start_index; i < start_index + span_width; i++)
		y_level_map[i] = value;
}  /* set_span_height */

/* find the height of the span to the left and right of the passed span */
static void find_left_right_span(int *y_level_map, int start_index, int span_width, int *left, int *right)
{
	*left = start_index == page_left ? NOT_IN_PAGE : y_level_map[start_index - 1];
	*right = (start_index + span_width) == page_right ? NOT_IN_PAGE : y_level_map[start_index + span_width];
}  /* find_left_right_span */

/* build and return a list of blocks from the src_list */
/* MIN_WIDTH value = smallest width, return list of blocks with that width */
/* MIN_HEIGHT value = smallest height, return list of blocks with that height */
/* MAX_WIDTH value = largest width, return list of blocks with that width */
/* MAX_HEIGTH value = largest height, return list of blocks with that height */
/* CLOSEST_WIDTH value = width closest to passed width, return list of blocks with that width */
/* CLOSEST_HEIGHT value = width closest to passed height, return list of blocks with that height */
/* EQ_WIDTH value = passed in width, return list of blocks equal to the passed width */
/* EQ_HEIGHT value = passed in height, return list of blocks equal to the passed height */
/* LE_WIDTH value = passed in width, return list of blocks less than or equal to passed width */
/* LE_HEIGTH value = passed in height, return list of blocks less than or equal to passed height */
static block_list_t *find_block_list(block_list_t *src_list, int which_blocks, int *value)
{
	block_list_t *new_list_head;
	block_list_t *block;
	int tmp_value, delta, new_delta;
	
	new_list_head = NULL;
	switch (which_blocks) {
		/* find a list of blocks which all have the smallest width */
	case MIN_WIDTH:
		/* find the smallest width */
		*value = page_log_width + 1;
		for (block = src_list; block != NULL; block = block->list_next) {
			if (valid_block(block->list_data) && (block->list_data->block_align_width < *value))
				*value = block->list_data->block_align_width;
		}
		/* build the list of blocks sharing that width */
		new_list_head = build_block_list(src_list, width_eq, *value);
		break;

		/* find a list of blocks which all have the smallest height */
	case MIN_HEIGHT:
		/* find the smallest height */
		*value = params->pack_mode == LINE_MODE ? max_block_height + 1 : page_bottom + 1;
		for (block = src_list; block != NULL; block = block->list_next) {
			if (valid_block(block->list_data) && (block->list_data->block_height < *value))
				*value = block->list_data->block_height;
		}
		/* build the list of blocks sharing that height */
		new_list_head = build_block_list(src_list, height_eq, *value);
		break;

		/* return a list of blocks which all have the largest width */
	case MAX_WIDTH:
		/* find the largest width */
		*value = 0;
		for (block = src_list; block != NULL; block = block->list_next) {
			if (valid_block(block->list_data) && (block->list_data->block_align_width > *value))
				*value = block->list_data->block_align_width;
		}
		/* build the list of blocks sharing that width */
		new_list_head = build_block_list(src_list, width_eq, *value);
		break;

		/* find a list of blocks which all have the largest height */
	case MAX_HEIGHT:
		/* find the largest height */
		*value = 0;
		for (block = src_list; block != NULL; block = block->list_next) {
			if (valid_block(block->list_data) && (block->list_data->block_height > *value))
				*value = block->list_data->block_height;
		}
		/* build the list of blocks sharing that height */
		new_list_head = build_block_list(src_list, height_eq, *value);
		break;

		/* find a list of blocks which have a width closest to but not greater */
		/* than the passed in value */
	case CLOSEST_WIDTH:
		/* find the closest width */
		tmp_value = 0;
		delta = *value;
		for (block = src_list; block != NULL; block = block->list_next) {
			if (valid_block(block->list_data) && (block->list_data->block_align_width <= *value)) {
				new_delta = *value - block->list_data->block_align_width;
				if (new_delta < delta) {
					tmp_value = block->list_data->block_align_width;
					delta = new_delta;
				}
			}
		}
		*value = tmp_value;
		/* build the list of blocks sharing that width */
		new_list_head = build_block_list(src_list, width_eq, *value);
		break;

		/* find a list of blocks which have a height closest to but not greater */
		/* than the passed in value */
	case CLOSEST_HEIGHT:
		/* find the closest height */
		tmp_value = 0;
		delta = *value;
		for (block = src_list; block != NULL; block = block->list_next) {
			if (valid_block(block->list_data) && (block->list_data->block_height <= *value)) {
				new_delta = *value - block->list_data->block_height;
				if (new_delta < delta) {
					tmp_value = block->list_data->block_height;
					delta = new_delta;
				}
			}
		}
		*value = tmp_value;
		/* build the list of blocks sharing that width */
		new_list_head = build_block_list(src_list, height_eq, *value);
		break;

		/* find a list of blocks which all have the width equal to the value */
		/* passed in */
	case EQ_WIDTH:
		new_list_head = build_block_list(src_list, width_eq, *value);
		break;

		/* find a list of blocks which all have the height equal to the value */
		/* passed in */
	case EQ_HEIGHT:
		new_list_head = build_block_list(src_list, height_eq, *value);
		break;

		/* find a list of blocks which all have the width equal to or less than the */
		/* value passed in */
	case LE_WIDTH:
		new_list_head = build_block_list(src_list, width_le, *value);
		break;

		/* find a list of blocks which all have the height equal to or less than the */
		/* value passed in */
	case LE_HEIGHT:
		new_list_head = build_block_list(src_list, height_le, *value);
		break;
	}
	return new_list_head;
}  /* find_block_list */

/* eaxmine the y_level contour map and remove and spans that are too narrow to fit any of the */
/* availiable blocks */
static void fill_bad_spans(int *y_level_map)
{
	block_list_t *width_list, *height_list;
	int i, max_width, max_height;
	int curr_y_level, new_y_level;
	int left, right;
	int curr_span_width;
	bool filled_span;
	
	do {
		/* have not filled in any spans yet */
		filled_span = FALSE;
		
		/* scan through the contour map examine every span */
		for (i = page_left; i < page_right; i += curr_span_width) {
			/* find the next span width */
			curr_span_width = get_span_width(y_level_map, i);
			
			/* check to see if at least one block will fit */
			max_width = curr_span_width;
			width_list = find_block_list(block_list, LE_WIDTH, &max_width);
			max_height = params->pack_mode == LINE_MODE ? max_block_height : page_bottom - y_level_map[i];
			height_list = find_block_list(width_list, LE_HEIGHT, &max_height);
			
			if (count_block_list(height_list) == 0) {
				/* find the height of the span */
				curr_y_level = y_level_map[i];
				
				/* find the heights of the span to the left and right */
				find_left_right_span(y_level_map, i, curr_span_width, &left, &right);
				
				/* determine the new height, it is the shorter of the adjacent spans */
				if (left == NOT_IN_PAGE)
					new_y_level = right == NOT_IN_PAGE ? curr_y_level : right;
				else
					new_y_level = right == NOT_IN_PAGE ? left : MIN(left, right);
				
				/* update the contour map if needed */
				if (curr_y_level < new_y_level) {
					/* update stats */
					filled_pix += curr_span_width * (new_y_level - curr_y_level);
					page_pix += curr_span_width * (new_y_level - curr_y_level);
					
					/* output debug info */
					if (params->debug)
						printf("filling span (%c) left %d->%d top %d->%d\n", new_y_level == left ? 'L' : 'R', i, i + curr_span_width - 1, curr_y_level, new_y_level - 1);
					
					set_span_height(y_level_map, i, curr_span_width, new_y_level);
					filled_span = TRUE;
				}
			}
			destroy_block_list(width_list);
			destroy_block_list(height_list);
		}
	} while (filled_span);
}  /* fill_bad_spans */

/* a compare function passed to build_block_list */
/* return TRUE for blocks whose width match the passed parameter */
static bool width_eq(block_t *block_data, int value)
{
	return block_data->block_align_width == value;
}  /* width_eq */

/* a compare function passed to build_block_list */
/* return TRUE for blocks whose height match the passed parameter */
static bool height_eq(block_t *block_data, int value)
{
	return block_data->block_height == value;
}  /* height_eq */

/* a compare function passed to build_block_list */
/* return TRUE for blocks whose width is less than or equal to the passed parameter */
static bool width_le(block_t *block_data, int value)
{
	return block_data->block_align_width <= value;
}  /* width_le */

/* a compare function passed to build_block_list */
/* return TRUE for blocks whose height is less than or equal to the passed parameter */
static bool height_le(block_t *block_data, int value)
{
	return block_data->block_height <= value;
}  /* height_le */

/* given a source list of blocks, a compare function, and a value build and return a new list */
/* of blocks based on the compare routine */
static block_list_t *build_block_list(block_list_t *src_list, bool (*cmp)(block_t *block_data, int value), int value)
{
	block_list_t *dst_list;				/* new list of blocks */
	block_list_t *block;				/* current block being considered */
	block_list_t *new_element;			/* new element in list */
	
	dst_list = NULL;
	for (block = src_list; block != NULL; block = block->list_next)
		/* make sure the block is not already placed */
		if (valid_block(block->list_data) && cmp(block->list_data, value)) {
			/* add to new list */
			new_element = util_xmalloc(sizeof(block_list_t));
			new_element->list_next = dst_list;
			new_element->list_data = block->list_data;
			dst_list = new_element;
		}
	return dst_list;
}  /* build_block_list */

/* free the storage used by a linked list of blocks */
static void destroy_block_list(block_list_t *src_list)
{
	block_list_t *block;
	block_list_t *next;
	
	for (block = src_list; block != NULL; block = next) {
		next = block->list_next;
		free(block);
	}
}  /* destroy_block_list */

/* return the number of elements in a block list */
static int count_block_list(block_list_t *src_list)
{
	block_list_t *block;
	int num_block;
	
	num_block = 0;
	for (block = src_list; block != NULL; block = block->list_next)
		/* make sure the block is not already placed */
		if (valid_block(block->list_data))
			num_block++;
	return num_block;
}  /* count_block_list */

/* give a list of blocks, and EQ_WIDTH or EQ_HEIGHT make the parameter for a call to bagger */
static void list_to_bagger(block_list_t *src_list, int which, int *num_value, int **value)
{
	block_list_t *block;
	int i;
		
	*num_value = count_block_list(src_list);
	if (*num_value > 0) {
		*value = util_xmalloc(*num_value * sizeof(int));
		i = 0;
		for (block = src_list; block != NULL; block = block->list_next)
			/* make sure the block is not already placed */
			if (valid_block(block->list_data))
				(*value)[i++] = (which == EQ_WIDTH) ? block->list_data->block_align_width : block->list_data->block_height;
	} else
		*value = NULL;
}  /* list_to_bagger */

/* given a set from bagger, original block list build a sub list of the items in the set */
static block_list_t *set_to_list(set *result, int goal, int num_value, int *value, block_list_t *src_list, int which)
{
	block_list_t *dst_list;
	block_list_t *new_elem;
	block_list_t *block;
	bool found;
	int i;
	
	/* output debug info */
	if (params->debug)
		printf("bagger considering %4d items, wants %4d, got %4d\n", num_value, goal, result->total);
	
	dst_list = NULL;
	for (i = 0; i < num_value; i++) {
		if (util_bit_test(result->mask, i)) {
			/* value[i] is in the set, find that block and add to the new list */
			for (block = src_list; block != NULL; block = block->list_next) {
				if (valid_block(block->list_data)) {
					if (which == EQ_WIDTH)
						found = value[i] == block->list_data->block_align_width;
					else
						found = value[i] == block->list_data->block_height;
					if (found) {
						new_elem = util_xmalloc(sizeof(block_list_t));
						new_elem->list_next = dst_list;
						new_elem->list_data = block->list_data;
						dst_list = new_elem;
						/* set texture_page to -2 so it can not be found again, could have */
						/* two of the same value represented in the set and need to get */
						/* the seperate blocks not two references to the first one */
						new_elem->list_data->block_texture_page = -2;
						break;
					}
				}
			}
		}
	}
	free(result->mask);
	result->mask = NULL;
	
	/* reset the flag value back to -1 */
	for (block = dst_list; block != NULL; block = block->list_next)
		block->list_data->block_texture_page = -1;
	
	return dst_list;
}  /* set_to_list */

static void calc_stats(pack_stats *stats)
{
	block_list_t *list;
	block_t *b;
	int i;
	uint total_pix, total_align_pix;
	float pix_count;
	
	if (params->pack_mode == LINE_MODE) {
		/* find the number of lines */
		stats->line_count = 0;
		for (list = block_list; list != NULL; list = list->list_next) {
			b = list->list_data;
			if (b->block_y_offset + b->block_height > stats->line_count)
				stats->line_count = b->block_y_offset + b->block_height;
		}
		/* add in the bottom inset */
		i = stats->line_count;
		stats->line_count += params->page_phy_height - page_bottom;
		stats->total_line_count = stats->line_count;
		page_bottom = i;
		
		/* only one page */
		stats->page_count = 1;
		
		/* calculate the number of pixels wasted on the boarder */
		boarder_pix += page_top * params->page_phy_width;
		boarder_pix += (stats->line_count - page_bottom) * params->page_phy_width;
		boarder_pix += (page_top + stats->line_count - page_bottom) * (params->page_phy_width - page_log_width);
	} else {
		/* line_count is the page_phy_height */
		stats->line_count = params->page_phy_height;
		stats->total_line_count = 0;
		for (list = block_list; list != NULL; list = list->list_next) {
			b = list->list_data;
			i = (b->block_texture_page * stats->line_count) + b->block_y_offset + b->block_height;
			if (i > stats->total_line_count)
				stats->total_line_count = i;
		}
		
		/* find the number of pages */
		stats->page_count = 0;
		for (list = block_list; list != NULL; list = list->list_next) {
			b = list->list_data;
			if (b->block_texture_page > stats->page_count)
				stats->page_count = b->block_texture_page;
		}
		stats->page_count++;
	}
	
	/* calculate the number of pixels unused at the end of the last page or on last scan line */
	last_page_pix = (params->page_phy_width * stats->line_count) - page_pix;
	if (params->pack_mode == LINE_MODE)
		last_page_pix -= boarder_pix;
	
	total_pix = 0;
	total_align_pix = 0;
	for (list = block_list; list != NULL; list = list->list_next) {
		b = list->list_data;
		total_pix += b->block_width * b->block_height;
		total_align_pix += (b->block_align_width - b->block_width) * b->block_height;
	}
	
	pix_count = (float)(stats->page_count * params->page_phy_width * stats->line_count);
	
	stats->pack_efficiency = ((float)total_pix / pix_count) * 100.0F;
	stats->filled_spans = ((float)filled_pix / pix_count) * 100.0F;
	stats->filled_alignment = ((float)total_align_pix / pix_count) * 100.0F;
	stats->filled_boarder = ((float)boarder_pix / pix_count) * 100.0F;
	if (params->pack_mode == LINE_MODE) {
		stats->filled_end_page = 0.0F;
		stats->filled_end_last_page = ((float)last_page_pix / pix_count) * 100.0F;
	} else {
		stats->filled_end_page = ((float)end_page_pix / pix_count) * 100.0F;
		stats->filled_end_last_page = ((float)last_page_pix / pix_count) * 100.0F;
	}
}  /* calc_stats */

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */
