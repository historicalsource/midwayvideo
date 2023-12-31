/* -*-c++-*- */
/* $Header: /Releases/Banshee/GLOP/3Dfx/Devel/H3/cinit/modetabl.h 1     9/02/98 12:35a Sapphire $ */
/*
** Copyright (c) 1995, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished  -
** rights reserved under the Copyright Laws of the United States.
**
**
** $Revision: 1 $
** $Date: 9/02/98 12:35a $
**
** $History: modetabl.h $
** 
** *****************  Version 1  *****************
** User: Sapphire     Date: 9/02/98    Time: 12:35a
** Created in $/Releases/Banshee/GLOP/3Dfx/Devel/H3/cinit
** 
** *****************  Version 14  *****************
** User: Andrew       Date: 8/31/98    Time: 11:26p
** Updated in $/devel/h3/Win95/dx/minivdd
** John's fixes for 800x600
** 
** *****************  Version 13  *****************
** User: Andrew       Date: 8/20/98    Time: 10:11p
** Updated in $/devel/h3/Win95/dx/minivdd
** Updated 320x240@60,72 400x300 @ 72 & 85, 1792x1392 changed from 72 to
** 75.
** 
** *****************  Version 12  *****************
** User: Andrew       Date: 7/27/98    Time: 11:12a
** Updated in $/devel/h3/Win95/dx/minivdd
** Added updates for 400x300 modes since SCANLINEDBL was not set in xls
** file
** 
** *****************  Version 11  *****************
** User: Andrew       Date: 7/21/98    Time: 2:42p
** Updated in $/devel/h3/Win95/dx/minivdd
** Modified to support the new final mode list -- add new refreshs
** 320x200, 320x240, 400x300, 512x384, and 1152x864.
** 
** *****************  Version 10  *****************
** User: Andrew       Date: 7/04/98    Time: 10:37a
** Updated in $/devel/h3/Win95/dx/minivdd
** Got modetabl.h the right way.....Differences  hsync skew on 1920x1440
** and clocks on low-rez modes
** 
** *****************  Version 9  *****************
** User: Andrew       Date: 6/29/98    Time: 10:58a
** Updated in $/devel/h3/Win95/dx/minivdd
** Changed Dot Clock on 1792x1344 and Refresh Rate from 70 to 72.  Changed
** CR04 on 1792x1344 @ 60 Hz
** 
** *****************  Version 8  *****************
** User: Andrew       Date: 6/24/98    Time: 9:32a
** Updated in $/devel/h3/Win95/dx/minivdd
** New mode additions for 1792x1344 and 1856x1392 plus 1152x864 @ 100
** 
** *****************  Version 7  *****************
** User: Andrew       Date: 5/19/98    Time: 6:12p
** Updated in $/devel/h3/Win95/dx/minivdd
** changed 1800 to 1808 and changed timing to 1808
** 
** *****************  Version 6  *****************
** User: Andrew       Date: 5/07/98    Time: 11:24a
** Updated in $/devel/h3/Win95/dx/minivdd
** Added modes 1792x1440 and 1920x1440.  1800x1440 was updated but still
** does not work
** 
** *****************  Version 5  *****************
** User: Andrew       Date: 4/22/98    Time: 2:58p
** Updated in $/devel/h3/Win95/dx/minivdd
** Changed clock at 1280x960 @ 75 hz and 1600x1200 @ 85 Hz and added
** broken 1800x1440
** 
** *****************  Version 4  *****************
** User: Ken          Date: 4/15/98    Time: 6:42p
** Updated in $/devel/h3/win95/dx/minivdd
** added unified header to all files, with revision, etc. info in it
**
*/
//
// The structure of this data is as follows:
//
//typedef struct mode_table_data
// hres;
// vres;
// vfreq;
//	htot_lsb;
//	hvis_lsb;
//	hblnk_start_lsb;
//	hblnk_end_lsb;
//	hsync_start_lsb;
//	hsync_end_lsb;
//	vtot_lsb;
//	misc;
//	lines_per_char;
//	vsync_start_lsb;
//	vsync_end_lsb;
//	vvis_lsb;
//	vblnk_start_lsb;
//	vblnk_end_lsb;
//	dunno
//	dunno
//	sync_polarities,
//	seq_register,
//	pll_lsb,
//	pll_msb,
//	dac_mode
//} mode_table_data_t;
//

#ifndef __GOOSE__
{320, 200, 70, 0x2f, 0x27, 0x27, 0x93, 0x2a, 0x8e, 0xbb, 0x1f, 0x40, 0x91,
	0x24, 0x8f, 0x8f, 0xbc, 0x80, 0x00, 0x4f, 0x21, 0xee, 0xdb, 0x00},
{320, 200, 85, 0x2f, 0x27, 0x27, 0x93, 0x2a, 0x8e, 0xbb, 0x1f, 0x40, 0x91,
	0x24, 0x8f, 0x8f, 0xbc, 0x80, 0x00, 0x4f, 0x21, 0x37, 0x82, 0x00},
{320, 240, 60, 0x2d, 0x27, 0x27, 0x8f, 0x29, 0x8e, 0x0b, 0x3e, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xfc, 0x80, 0x00, 0xcf, 0x21, 0xea, 0xd1, 0x00},
{320, 240, 72, 0x2f, 0x27, 0x27, 0x93, 0x2a, 0x8e, 0x06, 0x3e, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xfc, 0x80, 0x00, 0xcf, 0x21, 0x37, 0x82, 0x00},
{320, 240, 85, 0x2f, 0x27, 0x27, 0x93, 0x2a, 0x8e, 0xfb, 0x1f, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xfc, 0x80, 0x00, 0xcf, 0x21, 0x3f, 0xa9, 0x00},
{400, 300, 60, 0x3d, 0x31, 0x31, 0x81, 0x34, 0x1c, 0x72, 0xf0, 0x60, 0x59,
	0x2d, 0x57, 0x57, 0x73, 0xa0, 0x00, 0x0f, 0x21, 0xf4, 0x56, 0x00},
{400, 300, 72, 0x3c, 0x31, 0x31, 0x80, 0x34, 0x1b, 0x98, 0xf0, 0x60, 0x7d,
	0x23, 0x57, 0x57, 0x99, 0xa0, 0x00, 0x0f, 0x21, 0xf4, 0x6c, 0x00},
{400, 300, 85, 0x3d, 0x31, 0x31, 0x80, 0x33, 0x1b, 0x75, 0xf0, 0x60, 0x59,
	0x2c, 0x57, 0x57, 0x76, 0xa0, 0x00, 0x0f, 0x21, 0xb8, 0x5d, 0x00},
#endif
{512, 256, 57, 0x4c, 0x3f, 0x41, 0x90, 0x41, 0x06, 0x16, 0x1d, 0x47, 0x04,
   0x28, 0xff, 0x00, 0x16, 0x20, 0x00, 0xcf, 0x21, 0x86, 0x63, 0x00},

// 60 
//{512, 384, 60, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x06, 0x9d, 0x1f, 0x47, 0x82,
//	0x26, 0x7f, 0x80, 0x9b, 0x20, 0x00, 0xcf, 0x21, 0x1f, 0x51, 0x00},
{512, 384, 60, 0x4e, 0x3f, 0x40, 0x92, 0x41, 0x04, 0x9f, 0x1f, 0x47, 0x85,
	0x0a, 0x7f, 0x7f, 0x9f, 0x20, 0x00, 0xcf, 0x21, 0xd6, 0xfd, 0x00},


// 59 Hz
//{512, 384, 59, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x06, 0xa5, 0x1f, 0x47, 0x8a,
//	0x26, 0x7f, 0x88, 0xa3, 0x20, 0x00, 0xcf, 0x21, 0x1f, 0x51, 0x00},
{512, 384, 59, 0x4e, 0x3f, 0x40, 0x92, 0x41, 0x04, 0xa7, 0x1f, 0x47, 0x85,
	0x0a, 0x7f, 0x7f, 0xa7, 0x20, 0x00, 0xcf, 0x21, 0xd6, 0xfd, 0x00},

// 58 Hz
//{512, 384, 58, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x06, 0xab, 0x1f, 0x47, 0x90,
//	0x26, 0x7f, 0x8e, 0xa9, 0x20, 0x00, 0xcf, 0x21, 0x1f, 0x51, 0x00},
{512, 384, 58, 0x4e, 0x3f, 0x40, 0x92, 0x41, 0x04, 0xae, 0x1f, 0x47, 0x85,
	0x0a, 0x7f, 0x7f, 0xae, 0x20, 0x00, 0xcf, 0x21, 0xd6, 0xfd, 0x00},

// 57 Hz
//{512, 384, 57, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x06, 0xb3, 0x1f, 0x47, 0x9a,
//	0x26, 0x7f, 0x98, 0xb3, 0x20, 0x00, 0xcf, 0x21, 0x1f, 0x51, 0x00},
{512, 384, 57, 0x4e, 0x3f, 0x40, 0x92, 0x41, 0x04, 0xb5, 0x1f, 0x47, 0x85,
	0x0a, 0x7f, 0x7f, 0xb5, 0x20, 0x00, 0xcf, 0x21, 0xd6, 0xfd, 0x00},

// 56 Hz
//{512, 384, 56, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x06, 0xbb, 0x1f, 0x47, 0xa0,
//	0x26, 0x7f, 0x9e, 0xb9, 0x20, 0x00, 0xcf, 0x21, 0x1f, 0x51, 0x00},
{512, 384, 56, 0x4e, 0x3f, 0x40, 0x92, 0x41, 0x04, 0xbd, 0x1f, 0x47, 0x85,
	0x0a, 0x7f, 0x7f, 0xbd, 0x20, 0x00, 0xcf, 0x21, 0xd6, 0xfd, 0x00},

// 55 Hz
//{512, 384, 55, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x06, 0xc2, 0x1f, 0x47, 0xa7,
//	0x26, 0x7f, 0xa5, 0xc0, 0x20, 0x00, 0xcf, 0x21, 0x1f, 0x51, 0x00},
{512, 384, 55, 0x4e, 0x3f, 0x40, 0x92, 0x41, 0x04, 0xc5, 0x1f, 0x47, 0x85,
	0x0a, 0x7f, 0x7f, 0xc5, 0x20, 0x00, 0xcf, 0x21, 0xd6, 0xfd, 0x00},

#ifndef __GOOSE__
{512, 384, 60, 0x4f, 0x3f, 0x3f, 0x93, 0x41, 0x0a, 0x24, 0xf5, 0x60, 0x04,
	0x2a, 0xff, 0xff, 0x25, 0x20, 0x00, 0xcf, 0x21, 0x5e, 0xe1, 0x00},
{512, 384, 72, 0x4e, 0x3f, 0x3f, 0x92, 0x41, 0x0a, 0x24, 0xf5, 0x60, 0x03,
	0x29, 0xff, 0xff, 0x25, 0x20, 0x00, 0xcf, 0x21, 0xbc, 0x82, 0x00},
{512, 384, 75, 0x4d, 0x3f, 0x3f, 0x91, 0x41, 0x07, 0x1e, 0xf5, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x1f, 0x20, 0x00, 0x0f, 0x21, 0x2a, 0x82, 0x00},
{512, 384, 85, 0x51, 0x3f, 0x3f, 0x95, 0x43, 0x09, 0x26, 0xf5, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x27, 0x20, 0x00, 0x0f, 0x21, 0x36, 0xc4, 0x00},
{640, 350, 85, 0x63, 0x4f, 0x4f, 0x87, 0x54, 0x9c, 0xbb, 0x1f, 0x40, 0x7e,
	0x21, 0x5d, 0x5d, 0xbc, 0x20, 0x00, 0x8f, 0x21, 0x36, 0x82, 0x00},
{640, 400, 70, 0x63, 0x4f, 0x4f, 0x87, 0x54, 0x9c, 0xbb, 0x1f, 0x40, 0x91,
	0x24, 0x8f, 0x8f, 0xbc, 0x20, 0x00, 0x4f, 0x21, 0xed, 0xdb, 0x00},
{640, 400, 85, 0x63, 0x4f, 0x4f, 0x87, 0x54, 0x9c, 0xbb, 0x1f, 0x40, 0x91,
	0x24, 0x8f, 0x8f, 0xbc, 0x20, 0x00, 0x4f, 0x21, 0x36, 0x82, 0x00},
{640, 480, 60, 0x5f, 0x4f, 0x4f, 0x83, 0x52, 0x9e, 0x0b, 0x3e, 0x40, 0xea,
	0x2c, 0xdf, 0xdf, 0x0c, 0x20, 0x00, 0xcf, 0x21, 0x7d, 0x72, 0x00},
{640, 480, 72, 0x63, 0x4f, 0x4f, 0x87, 0x56, 0x9b, 0x06, 0x3e, 0x40, 0xe9,
	0x2c, 0xdf, 0xdf, 0x07, 0x20, 0x00, 0xcf, 0x21, 0x36, 0x82, 0x00},
{640, 480, 75, 0x64, 0x4f, 0x4f, 0x88, 0x51, 0x99, 0xf2, 0x1f, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xf3, 0x20, 0x00, 0xcf, 0x21, 0x36, 0x82, 0x00},
{640, 480, 85, 0x63, 0x4f, 0x4f, 0x87, 0x56, 0x9e, 0xfb, 0x1f, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xfc, 0x20, 0x00, 0xcf, 0x21, 0x3e, 0xa9, 0x00},
{640, 480, 100, 0x63, 0x4f, 0x4f, 0x87, 0x56, 0x9e, 0xfb, 0x1f, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xfc, 0x20, 0x00, 0xcf, 0x21, 0x58, 0x45, 0x00},
{640, 480, 120, 0x63, 0x4f, 0x4f, 0x87, 0x56, 0x9e, 0xfb, 0x1f, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0xfc, 0x20, 0x00, 0xcf, 0x21, 0x48, 0x45, 0x00},
{720, 400, 72, 0x70, 0x59, 0x59, 0x94, 0x5e, 0x87, 0xbc, 0x1f, 0x40, 0x91,
	0x24, 0x8f, 0x8f, 0xbd, 0xa0, 0x00, 0x4f, 0x21, 0x20, 0x13, 0x00},
{720, 400, 85, 0x70, 0x59, 0x59, 0x94, 0x5e, 0x87, 0xbc, 0x1f, 0x40, 0x91,
	0x24, 0x8f, 0x8f, 0xbd, 0xa0, 0x00, 0x4f, 0x21, 0xb8, 0x75, 0x00},
{720, 480, 60, 0x70, 0x59, 0x59, 0x94, 0x5e, 0x87, 0x15, 0x3e, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0x16, 0xa0, 0x00, 0x4f, 0x21, 0xc4, 0x69, 0x00},
{720, 480, 72, 0x70, 0x59, 0x59, 0x94, 0x5e, 0x87, 0x15, 0x3e, 0x40, 0xe1,
	0x24, 0xdf, 0xdf, 0x16, 0xa0, 0x00, 0x4f, 0x21, 0xd8, 0x8b, 0x00},
{720, 576, 72, 0x70, 0x59, 0x59, 0x94, 0x5e, 0x87, 0x80, 0xf0, 0x60, 0x41,
	0x24, 0x3f, 0x3f, 0x81, 0xa0, 0x00, 0x4f, 0x21, 0xb0, 0x89, 0x00},
{720, 576, 100, 0x70, 0x59, 0x59, 0x94, 0x5e, 0x87, 0x80, 0xf0, 0x60, 0x41,
	0x24, 0x3f, 0x3f, 0x81, 0xa0, 0x00, 0x4f, 0x21, 0xec, 0xfe, 0x00},
{800, 600, 56, 0x7b, 0x63, 0x63, 0x9f, 0x69, 0x99, 0x6f, 0xf0, 0x60, 0x59,
	0x2b, 0x57, 0x57, 0x70, 0xa0, 0x00, 0x0f, 0x21, 0x3e, 0xa9, 0x00},
{800, 600, 60, 0x7b, 0x63, 0x63, 0x9f, 0x6a, 0x94, 0x6c, 0xf0, 0x60, 0x59,
	0x2c, 0x57, 0x57, 0x6d, 0xa0, 0x00, 0x0f, 0x21, 0x06, 0x1e, 0x00},
{800, 600, 72, 0x7d, 0x63, 0x63, 0x81, 0x68, 0x17, 0x98, 0xf0, 0x60, 0x7d,
	0x23, 0x57, 0x57, 0x99, 0x80, 0x00, 0x0f, 0x21, 0x06, 0x28, 0x00},
{800, 600, 75, 0x7f, 0x63, 0x63, 0x83, 0x65, 0x0f, 0x6f, 0xf0, 0x60, 0x59,
	0x2c, 0x57, 0x57, 0x70, 0x80, 0x00, 0x0f, 0x21, 0x55, 0x9d, 0x00},
{800, 600, 85, 0x7e, 0x63, 0x63, 0x82, 0x67, 0x0f, 0x75, 0xf0, 0x60, 0x59,
	0x2c, 0x57, 0x57, 0x76, 0x80, 0x00, 0x0f, 0x21, 0x31, 0x6c, 0x00},
{800, 600, 100, 0x7e, 0x63, 0x63, 0x82, 0x66, 0x0e, 0x75, 0xf0, 0x60, 0x59,
	0x2c, 0x57, 0x57, 0x76, 0x80, 0x00, 0x0f, 0x21, 0x8c, 0xa9, 0x00},
{800, 600, 120, 0x7e, 0x63, 0x63, 0x82, 0x66, 0x0e, 0x75, 0xf0, 0x60, 0x59,
	0x2c, 0x57, 0x57, 0x76, 0x80, 0x00, 0x0f, 0x21, 0x24, 0x3b, 0x00},
{1152, 864, 60, 0xb4, 0x8f, 0x8f, 0x98, 0x94, 0x80, 0x8e, 0xff, 0x60, 0x61,
	0x24, 0x5f, 0x5f, 0x8f, 0x80, 0x00, 0x0f, 0x21, 0x84, 0xc4, 0x00},
{1152, 864, 75, 0xc3, 0x8f, 0x8f, 0x87, 0x98, 0x08, 0x82, 0xff, 0x60, 0x61,
	0x24, 0x5f, 0x5f, 0x83, 0xa0, 0x00, 0x0f, 0x21, 0x50, 0xa4, 0x00},
{1152, 864, 85, 0xc0, 0x8f, 0x8f, 0x84, 0x98, 0x08, 0x89, 0xff, 0x60, 0x61,
	0x24, 0x5f, 0x5f, 0x8a, 0xa0, 0x00, 0x0f, 0x21, 0x6c, 0xf4, 0x00},
{1152, 864, 100, 0x5f, 0x47, 0x47, 0x83, 0x4c, 0x93, 0x82, 0xff, 0x60, 0x61,
	0x24, 0x5f, 0x5f, 0x83, 0x20, 0x00, 0x0f, 0x21, 0x40, 0xb3, 0x01},
{1024, 768, 60, 0xa3, 0x7f, 0x7f, 0x87, 0x83, 0x94, 0x24, 0xf5, 0x60, 0x04,
	0x2a, 0xff, 0xff, 0x25, 0x00, 0x00, 0xcf, 0x21, 0x5d, 0xe1, 0x00},
{1024, 768, 70, 0xa1, 0x7f, 0x7f, 0x85, 0x83, 0x94, 0x24, 0xf5, 0x60, 0x03,
	0x29, 0xff, 0xff, 0x25, 0x00, 0x00, 0xcf, 0x21, 0x4d, 0xda, 0x00},
{1024, 768, 72, 0xa1, 0x7f, 0x7f, 0x85, 0x83, 0x94, 0x24, 0xf5, 0x60, 0x03,
	0x29, 0xff, 0xff, 0x25, 0x00, 0x00, 0xcf, 0x21, 0x74, 0xa5, 0x00},
{1024, 768, 75, 0x9f, 0x7f, 0x7f, 0x83, 0x82, 0x8e, 0x1e, 0xf5, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x0f, 0x21, 0x29, 0x82, 0x00},
{1024, 768, 85, 0xa7, 0x7f, 0x7f, 0x8b, 0x86, 0x92, 0x26, 0xf5, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x27, 0x00, 0x00, 0x0f, 0x21, 0x35, 0xc4, 0x00},
{1024, 768, 100, 0xa7, 0x7f, 0x7f, 0x8b, 0x86, 0x92, 0x26, 0xf5, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x27, 0x00, 0x00, 0x0f, 0x21, 0x3c, 0x82, 0x00},
{1024, 768, 120, 0xa7, 0x7f, 0x7f, 0x8b, 0x86, 0x92, 0x26, 0xf5, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x27, 0x00, 0x00, 0x0f, 0x21, 0x50, 0xcb, 0x00},
{1280, 960, 60, 0xdc, 0x9f, 0x9f, 0x80, 0xac, 0x9a, 0xe6, 0xff, 0x60, 0xc1,
	0x24, 0xbf, 0xbf, 0xe7, 0xa0, 0x00, 0x0f, 0x21, 0x50, 0xa4, 0x00},
{1280, 960, 75, 0xcd, 0x9f, 0x9f, 0x91, 0xa4, 0x16, 0xe6, 0xff, 0x60, 0xc1,
	0x24, 0xbf, 0xbf, 0xe7, 0xa0, 0x00, 0x0f, 0x21, 0x0c, 0x2a, 0x00},
{1280, 960, 85, 0x67, 0x4f, 0x4f, 0x8b, 0x54, 0x9e, 0xf1, 0xff, 0x60, 0xc1,
	0x24, 0xbf, 0xbf, 0xf2, 0x20, 0x00, 0x0f, 0x21, 0x38, 0xa4, 0x01},
{1280, 1024, 60, 0xce, 0x9f, 0x9f, 0x92, 0xa6, 0x14, 0x28, 0x5a, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x29, 0xa0, 0x41, 0x0f, 0x21, 0x50, 0xa4, 0x00},
{1280, 1024, 75, 0xce, 0x9f, 0x9f, 0x92, 0xa2, 0x14, 0x28, 0x5a, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x29, 0xa0, 0x41, 0x0f, 0x21, 0x30, 0x82, 0x00},
{1280, 1024, 85, 0x67, 0x4f, 0x4f, 0x8b, 0x54, 0x9e, 0x2e, 0x5a, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x2f, 0x20, 0x41, 0x0f, 0x21, 0x28, 0x82, 0x01},
{1280, 1024, 100, 0x67, 0x4f, 0x4f, 0x8b, 0x54, 0x9e, 0x2e, 0x5a, 0x60, 0x01,
	0x24, 0xff, 0xff, 0x2f, 0x20, 0x41, 0x0f, 0x21, 0x3c, 0xda, 0x01},
{1600, 1200, 60, 0x82, 0x63, 0x63, 0x86, 0x68, 0x14, 0xe0, 0x10, 0x40, 0xb1,
	0x24, 0xaf, 0xaf, 0xe1, 0x80, 0x55, 0x0f, 0x21, 0x44, 0xd5, 0x01},
{1600, 1200, 65, 0x82, 0x63, 0x63, 0x86, 0x68, 0x14, 0xe0, 0x10, 0x40, 0xb1,
	0x24, 0xaf, 0xaf, 0xe1, 0x80, 0x55, 0x0f, 0x21, 0x44, 0xe7, 0x01},
{1600, 1200, 70, 0x82, 0x63, 0x63, 0x86, 0x68, 0x14, 0xe0, 0x10, 0x40, 0xb1,
	0x24, 0xaf, 0xaf, 0xe1, 0x80, 0x55, 0x0f, 0x21, 0x34, 0xc4, 0x01},
{1600, 1200, 75, 0x82, 0x63, 0x63, 0x86, 0x68, 0x14, 0xe0, 0x10, 0x40, 0xb1,
	0x24, 0xaf, 0xaf, 0xe1, 0x80, 0x55, 0x0f, 0x21, 0x30, 0xc4, 0x01},
{1600, 1200, 80, 0x82, 0x63, 0x63, 0x86, 0x68, 0x14, 0xe0, 0x10, 0x40, 0xb1,
	0x24, 0xaf, 0xaf, 0xe1, 0x80, 0x55, 0x0f, 0x21, 0x24, 0xa4, 0x01},
{1600, 1200, 85, 0x82, 0x63, 0x63, 0x86, 0x68, 0x14, 0xe0, 0x10, 0x40, 0xb1,
	0x24, 0xaf, 0xaf, 0xe1, 0x80, 0x55, 0x0f, 0x21, 0x0c, 0x4e, 0x01},
{1792, 1344, 60, 0x8e, 0x6f, 0x6f, 0x92, 0x71, 0x1f, 0x74, 0x1f, 0x40, 0x43,
	0x29, 0x3f, 0x3f, 0x75, 0x80, 0x55, 0x0f, 0x21, 0x30, 0xbf, 0x01},
{1792, 1344, 75, 0x94, 0x6f, 0x6f, 0x98, 0x74, 0x01, 0x87, 0x1f, 0x40, 0x4a,
	0x4d, 0x3f, 0x3f, 0x88, 0x00, 0x55, 0x0f, 0x21, 0x0c, 0x59, 0x01},
{1856, 1392, 60, 0x92, 0x73, 0x73, 0x96, 0x7c, 0x03, 0xa4, 0x1f, 0x40, 0x73,
	0x29, 0x6f, 0x6f, 0xa5, 0x00, 0x55, 0x0f, 0x21, 0x3c, 0xf7, 0x01},
{1920, 1440, 60, 0x9d, 0x77, 0x77, 0x81, 0x7a, 0x87, 0xda, 0x1f, 0x40, 0xa1,
	0x24, 0x9f, 0x9f, 0xdb, 0x00, 0x55, 0x0f, 0x21, 0x30, 0xe3, 0x01},
#endif
