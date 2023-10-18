/*
 *		$Archive: $
 *		$Revision: $
 *		$Date: $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __PACK_H__
#define __PACK_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_pack_h = "$Workfile: $ $Revision: $";
#endif

/*
 *		USER INCLUDES
 */
#ifndef __SYSTEM_H__
#include "system.h"
#endif

/*
 *		DEFINES
 */

#define PACK_NO_ERR					0
#define PACK_NO_PARAM_BLOCK			(-1)
#define PACK_BAD_BLOCK_COUNT		(-2)
#define PACK_NO_BLOCK_LIST			(-3)
#define PACK_BAD_PACK_MODE			(-4)
#define PACK_BAD_BLOCK_ALIGN		(-5)
#define PACK_BAD_PAGE_PHY_WIDTH		(-6)
#define PACK_BAD_PAGE_PHY_HEIGHT	(-7)
#define PACK_BAD_LEFT_INSET			(-8)
#define PACK_BAD_TOP_INSET			(-9)
#define PACK_BAD_RIGHT_INSET		(-10)
#define PACK_BAD_BOTTOM_INSET		(-11)
#define PACK_BAD_BLOCK_WIDTH		(-12)
#define PACK_BAD_BLOCK_HEIGHT		(-13)

/*
 *		TYPEDEFS
 */

enum {
	PAGE_MODE,						/* pack blocks into N page_phy_width by page_phy_height texture pages */
	LINE_MODE						/* pack blocks into N page_phy_width texture lines */
};

typedef struct {
									/* input */
	int block_width;				/* width of image data */
	int block_height;				/* height of image data */
	void *block_data;				/* free use for application */
									/* output */
	int block_texture_page;			/* texture page number block packed into, zero based */
	int block_x_offset;				/* x offset into the texture page, zero based */
	int block_y_offset;				/* y offset into the texture page, zero based */
									/* private data */
	int block_align_width;			/* width padded to alignment size */
} block_t;

typedef struct {
	int pack_mode;					/* PAGE_MODE or LINE_MODE */
	int block_align;				/* 1, 2, or 4 byte alignment boundary */
	int page_phy_width;				/* physical width of texture page */
	int page_phy_height;			/* physical height of texture page, ignored in LINE_MODE */
	int left_inset;					/* number of pixel inset on left boarder of texture page */
	int top_inset;					/* number of pixel inset on top boarder of texture page */
	int right_inset;				/* number of pixel inset on right boarder of texture page */
	int bottom_inset;				/* number of pixel inset on bottom boarder of texture page */
	bool debug;						/* enable debug info */
} pack_params;

typedef struct {
	int line_count;					/* PAGE_MODE its page_phy_height, LINE_MODE its number of lines used */
	int total_line_count;			/* PAGE_MODE it total lines used, LINE_MODE is same as line_count */
	int page_count;					/* number of texture pages used */
	float pack_efficiency;			/* block packing percentage */
	float filled_spans;				/* percentage of space used for unfillable spans */
	float filled_alignment;			/* percentage of space used for block alignment */
	float filled_boarder;			/* percentage of space used for inset boarder */
	float filled_end_page;			/* percentage of space unused at end of texture pages */
	float filled_end_last_page;		/* percentage of space unused at end of last page */
} pack_stats;

/*
 *		GLOBAL PROTOTYPES
 */

int pack(int num_blocks, block_t *blocks, pack_params *packing_params, pack_stats *packing_stats);

/*
 *	Local Variables:
 *	tab-width:4
 *	End:
 */

#endif
