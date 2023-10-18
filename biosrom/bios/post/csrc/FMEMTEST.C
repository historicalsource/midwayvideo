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
** $Revision: 2 $
** $Date: 10/16/97 7:42p $
*/

/* Fast memory test for FBI memory */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <sst.h>
#include <3dfx.h>
#include <glide.h>
#include <sst1init.h>
#include <fxpci.h>

int fbiMemErrors[2][4];

static void drawRect(FxU32 *, FxU32, FxU32, FxU32, FxU32, FxU32);
static void setAllBuffers(FxU32 *, FxU32, FxU32, FxU32);
static FxBool checkFbiMemConst(FxU32 *, FxU32, FxU32, FxU32);
static FxBool verifyFbiMemConst(FxU32 *, FxU32, FxU32, FxU32, FxU32);
static FxBool verifyFbiMemRndm(FxU32 *, FxU32, FxU32, FxU32, FxU32, FxU32);
static FxBool checkFbiMemWhite(FxU32 *, FxU32, FxU32);
static FxBool checkFbiMemBlack(FxU32 *, FxU32, FxU32);
static FxBool checkFbiMem55aa(FxU32 *, FxU32, FxU32);
static FxBool checkFbiMemaa55(FxU32 *, FxU32, FxU32);
static FxBool checkFbiMemFlatRndm(FxU32 *, FxU32, FxU32);
static FxBool checkFbiMemRndm(FxU32 *, FxU32, FxU32);
static void printFbiMemErrors(void);

#define BIT_5TO8(val) ((val&0x1f)<<3) | ((val&0x1c)>>2)
#define BIT_6TO8(val) ((val&0x3f)<<2) | ((val&0x30)>>4)
#define COLOR16_TO_COLOR24(x)	((BIT_5TO8(x&0x1f)) | \
                                ((BIT_6TO8((x>>5)&0x3f))<<8) | \
                                ((BIT_5TO8((x>>11)&0x1f))<<16))
#define EXIT(X) exit(X)

FxBool testFBIMem(void)
{
	FxU32 *sstbase;
	volatile Sstregs *sst;
	sst1DeviceInfoStruct deviceInfo;
	int i, j, fbiMemSize = -1;
	FxU32 xDim, yDim;
	FxBool retVal;

    if(!(sstbase = sst1InitMapBoard(0))) return (FXFALSE);
    if(!(sst1InitRegisters(sstbase))) return (FXFALSE);
    if(!(sst1InitGamma(sstbase, 1.0))) return (FXFALSE);

    sst = (Sstregs *) sstbase;
    if(sst1InitGetDeviceInfo(sstbase, &deviceInfo) == FXFALSE) return (FXFALSE);

    if(fbiMemSize < 0) fbiMemSize = (int) deviceInfo.fbiMemSize;

    if(fbiMemSize == 2) {
        if(!(sst1InitVideo(sstbase, GR_RESOLUTION_512x400, GR_REFRESH_60Hz,
              (sst1VideoTimingStruct *) NULL))) return (FXFALSE);

        xDim = 512;
        yDim = 400;
    }

	/* Initialize error structure */
	for(j=0; j<2; j++) {
		for(i=0; i<4; i++)
			fbiMemErrors[j][i] = 0;
	}

	/* Turn off memory fifo so that if FBI memory is corrupt the diag will */
	/* still run without hanging */
	sst1InitIdle(sstbase);
	sst->fbiInit0 &= ~SST_MEM_FIFO_EN;
	sst1InitIdle(sstbase);

    sst->fbzColorPath = SST_CC_MONE;
	sst->fogMode = 0x0;

    retVal = FXTRUE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkFbiMemBlack(sstbase, xDim, yDim) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkFbiMemWhite(sstbase, xDim, yDim) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkFbiMem55aa(sstbase, xDim, yDim) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkFbiMemaa55(sstbase, xDim, yDim) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkFbiMemFlatRndm(sstbase, xDim, yDim) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkFbiMemRndm(sstbase, xDim, yDim) == FXFALSE)
		retVal = FXFALSE;

    printFbiMemErrors();

    sst1InitShutdown(sstbase);

    return (retVal);
}

static void printFbiMemErrors(void)
{
#if 0
	printf("\nFBI Error Summary:\n");
	printf("------------------\n");
    printf("fbiData[15:0]  --> Bank0: %d\t\tBank1: %d\n",
           fbiMemErrors[0][0], fbiMemErrors[1][0]);
    printf("fbiData[31:16] --> Bank0: %d\t\tBank1: %d\n",
           fbiMemErrors[0][1], fbiMemErrors[1][1]);
    printf("fbiData[47:32] --> Bank0: %d\t\tBank1: %d\n",
           fbiMemErrors[0][2], fbiMemErrors[1][2]);
    printf("fbiData[63:48] --> Bank0: %d\t\tBank1: %d\n",
           fbiMemErrors[0][3], fbiMemErrors[1][3]);
#endif
}

static FxBool checkFbiMemWhite(FxU32 *sstbase, FxU32 xDim, FxU32 yDim)
{
	return(checkFbiMemConst(sstbase, xDim, yDim, 0xffff));
}

static FxBool checkFbiMemBlack(FxU32 *sstbase, FxU32 xDim, FxU32 yDim)
{
	return(checkFbiMemConst(sstbase, xDim, yDim, 0x0));
}

static FxBool checkFbiMem55aa(FxU32 *sstbase, FxU32 xDim, FxU32 yDim)
{
	return(checkFbiMemConst(sstbase, xDim, yDim, 0x55aa));
}

static FxBool checkFbiMemaa55(FxU32 *sstbase, FxU32 xDim, FxU32 yDim)
{
	return(checkFbiMemConst(sstbase, xDim, yDim, 0xaa55));
}

static FxBool checkFbiMemFlatRndm(FxU32 *sstbase, FxU32 xDim, FxU32 yDim)
{
	FxU32 i, dataExpect = 0xdead;
	FxBool retVal = FXTRUE;

	for(i=0; i<5; i++) {
		if(checkFbiMemConst(sstbase, xDim, yDim, dataExpect) == FXFALSE)
			retVal = FXFALSE;
		dataExpect = (dataExpect + 0x5193) & 0xffff;
	}
	return(retVal);
}

static FxBool checkFbiMemConst(FxU32 *sstbase, FxU32 xDim, FxU32 yDim, FxU32 color16)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;
	FxBool retVal = FXTRUE;

	setAllBuffers(sstbase, xDim, yDim, 0x0);
	sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_FRONT;
	sst->zaColor = color16;
	drawRect(sstbase, 0, 0, xDim, yDim, COLOR16_TO_COLOR24(color16));
	if(verifyFbiMemConst(sstbase, xDim, yDim, 0, color16) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 1, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 2, 0x0) == FXFALSE)
		retVal = FXFALSE;

	setAllBuffers(sstbase, xDim, yDim, 0x0);
	sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_BACK;
	drawRect(sstbase, 0, 0, xDim, yDim, COLOR16_TO_COLOR24(color16));
	if(verifyFbiMemConst(sstbase, xDim, yDim, 0, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 1, color16) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 2, 0x0) == FXFALSE)
		retVal = FXFALSE;

	setAllBuffers(sstbase, xDim, yDim, 0x0);
	sst->fbzMode = SST_ZAWRMASK | SST_DRAWBUFFER_FRONT;
	sst->zaColor = color16;
	drawRect(sstbase, 0, 0, xDim, yDim, 0xdeadbeef);
	if(verifyFbiMemConst(sstbase, xDim, yDim, 0, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 1, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 2, color16) == FXFALSE)
		retVal = FXFALSE;

	return retVal;
}

static FxBool checkFbiMemRndm(FxU32 *sstbase, FxU32 xDim, FxU32 yDim)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;
	volatile unsigned long *lfbptr = (unsigned long *) sstbase;
	FxU32 x, y, data;
	FxBool retVal = FXTRUE;

	/* Test Buffer 0 */
	setAllBuffers(sstbase, xDim, yDim, 0x0);
	sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_FRONT;
	sst->lfbMode = SST_LFB_565 | SST_LFB_WRITEFRONTBUFFER;
	sst1InitIdle(sstbase);

	data = 0xbaddead;
	for(y=0; y<yDim; y++) {
		for(x=0; x<xDim; x+=2) {
			lfbptr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)] = data;
			data += 0x34972195;
		}
	}
	sst1InitIdle(sstbase);
	if(verifyFbiMemRndm(sstbase, xDim, yDim, 0, 0xbaddead, 0x34972195) ==
	  FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 1, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 2, 0x0) == FXFALSE)
		retVal = FXFALSE;

	/* Test Buffer 1 */
	setAllBuffers(sstbase, xDim, yDim, 0x0);
	sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_BACK;
	sst->lfbMode = SST_LFB_565 | SST_LFB_WRITEBACKBUFFER;
	sst1InitIdle(sstbase);

	data = 0xbaddead;
	for(y=0; y<yDim; y++) {
		for(x=0; x<xDim; x+=2) {
			lfbptr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)] = data;
			data += 0x74978193;
		}
	}
	sst1InitIdle(sstbase);
	if(verifyFbiMemConst(sstbase, xDim, yDim, 0, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemRndm(sstbase, xDim, yDim, 1, 0xbaddead, 0x74978193) ==
	  FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 2, 0x0) == FXFALSE)
		retVal = FXFALSE;

	/* Test Buffer 2 */
	setAllBuffers(sstbase, xDim, yDim, 0x0);
	sst->fbzMode = SST_ZAWRMASK;
	sst->lfbMode = SST_LFB_ZZ;
	sst1InitIdle(sstbase);

	data = 0xabddadd;
	for(y=0; y<yDim; y++) {
		for(x=0; x<xDim; x+=2) {
			lfbptr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)] = data;
			data += 0x77976393;
		}
	}
	sst1InitIdle(sstbase);
	if(verifyFbiMemConst(sstbase, xDim, yDim, 0, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemConst(sstbase, xDim, yDim, 1, 0x0) == FXFALSE)
		retVal = FXFALSE;
	if(verifyFbiMemRndm(sstbase, xDim, yDim, 2, 0xabddadd, 0x77976393) ==
	  FXFALSE)
		retVal = FXFALSE;

	return retVal;
}

static void setAllBuffers(FxU32 *sstbase, FxU32 xDim, FxU32 yDim, FxU32 color16)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;

	sst->fbzMode = SST_RGBWRMASK | SST_ZAWRMASK | SST_DRAWBUFFER_FRONT;
	sst->zaColor = color16;
	drawRect(sstbase, 0, 0, xDim, yDim, COLOR16_TO_COLOR24(color16));
	sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_BACK;
	drawRect(sstbase, 0, 0, xDim, yDim, COLOR16_TO_COLOR24(color16));
	sst1InitIdle(sstbase);
}

static void drawRect(FxU32 *sstbase, FxU32 x, FxU32 y, FxU32 xsize, FxU32 ysize,
	FxU32 color)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;
	FxU32 left = x;
	FxU32 right = (x + xsize);
	FxU32 lowy = y;
	FxU32 highy = (y + ysize);

	sst->clipLeftRight = (left<<SST_CLIPLEFT_SHIFT) | right;
	sst->clipBottomTop = (lowy<<SST_CLIPBOTTOM_SHIFT) | highy;
	sst->c1 = color;
	sst->fastfillCMD = 0x0;
	sst1InitIdle(sstbase);
}

static FxBool verifyFbiMemConst(FxU32 *sstbase, FxU32 xDim, FxU32 yDim, FxU32 buffer,
	FxU32 color16)
{
	volatile unsigned long *lfbptr = (unsigned long *) sstbase;
	volatile Sstregs *sst = (Sstregs *) sstbase;
	FxU32 x, y, data;
	int row, interleave, iLeave, rowStart;
	FxBool retVal = FXTRUE;

	if(buffer == 0)
		sst->lfbMode = SST_LFB_READFRONTBUFFER;
	else if(buffer == 1)
		sst->lfbMode = SST_LFB_READBACKBUFFER;
	else
		sst->lfbMode = SST_LFB_READDEPTHABUFFER;

	sst1InitIdle(sstbase);

	switch(xDim) {
		case 512:
			row = buffer * 64;
			break;
		case 640:
			row = buffer * 150;
			break;
		case 800:
			row = buffer * 247;
			break;
		default:
            return (FXFALSE);
    }
	row--;

	for(y=0; y<yDim; y++) {
		if(xDim == 800) {
			interleave = (y & 0x10) ? 1 : 0;
			if(!(y & 0xf)) {
				if(!(y & 0x1f)) {
					row++;
					rowStart = row;
				} else
					rowStart = row;
			} else
				row = rowStart;
		} else {
			interleave = 0;
			if(!(y & 0xf)) {
				row++;
				rowStart = row;
			} else
				row = rowStart;
		}
		for(x=0; x<xDim; x+=2) {
			if(!(x & 0x3f)) {
				if(x > 0) {
					/* 64-pixel tile boundary */
					/* Adjust running row and interleave counters */
					interleave ^= 0x1;
					if(xDim == 800) {
						if(((!(y & 0x10)) && !(x & 0x40)) ||
							(( (y & 0x10)) &&  (x & 0x40)))
							row++;
					} else {
						if(!(x & 0x40))
							row++;
					}
				}
#if 0
				printf("x:%d y:%d buffer:%d xDim:%d row:%d interleave:%d\n",
					x, y, buffer, xDim, row, interleave); fflush(stdout);
#endif
			}
			data = lfbptr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)];

			if((data & 0xffff) != color16) {
#if 0
				printf("ERROR! x:%d y:%d buffer:%d xDim:%d row:%d interleave:%d\n",
					x, y, buffer, xDim, row, interleave); fflush(stdout);
#endif
				iLeave = (buffer < 2) ? interleave : (interleave ^ 0x1);
				fbiMemErrors[((row > 511) ? 1 : 0)][(iLeave<<1)]++;
				retVal = FXFALSE;
			}
			if(((data >> 16) & 0xffff) != color16) {
#if 0
				printf("ERROR! x:%d y:%d buffer:%d xDim:%d row:%d interleave:%d\n",
					x, y, buffer, xDim, row, interleave); fflush(stdout);
#endif
				iLeave = (buffer < 2) ? interleave : (interleave ^ 0x1);
				fbiMemErrors[((row > 511) ? 1 : 0)][(iLeave<<1)+1]++;
				retVal = FXFALSE;
			}
		}
	}
	return retVal;
}

static FxBool verifyFbiMemRndm(FxU32 *sstbase, FxU32 xDim, FxU32 yDim, FxU32 buffer,
	FxU32 rndmStart, FxU32 rndmAdd)
{
	volatile unsigned long *lfbptr = (unsigned long *) sstbase;
	volatile Sstregs *sst = (Sstregs *) sstbase;
	FxU32 x, y, data;
	int row, interleave, iLeave, rowStart;
	FxU32 dataExpect = rndmStart;
	FxBool retVal = FXTRUE;

	if(buffer == 0)
		sst->lfbMode = SST_LFB_READFRONTBUFFER;
	else if(buffer == 1)
		sst->lfbMode = SST_LFB_READBACKBUFFER;
	else
		sst->lfbMode = SST_LFB_READDEPTHABUFFER;

	sst1InitIdle(sstbase);

	switch(xDim) {
		case 512:
			row = buffer * 64;
			break;
		case 640:
			row = buffer * 150;
			break;
		case 800:
			row = buffer * 247;
			break;
		default:
            return (FXFALSE);
    }
	row--;

	for(y=0; y<yDim; y++) {
		if(xDim == 800) {
			interleave = (y & 0x10) ? 1 : 0;
			if(!(y & 0xf)) {
				if(!(y & 0x1f)) {
					row++;
					rowStart = row;
				} else
					rowStart = row;
			} else
				row = rowStart;
		} else {
			interleave = 0;
			if(!(y & 0xf)) {
				row++;
				rowStart = row;
			} else
				row = rowStart;
		}
		for(x=0; x<xDim; x+=2) {
			if(!(x & 0x3f)) {
				if(x > 0) {
					/* 64-pixel tile boundary */
					/* Adjust running row and interleave counters */
					interleave ^= 0x1;
					if(xDim == 800) {
						if(((!(y & 0x10)) && !(x & 0x40)) ||
							(( (y & 0x10)) &&  (x & 0x40)))
							row++;
					} else {
						if(!(x & 0x40))
							row++;
					}
				}
#if 0
				printf("x:%d y:%d buffer:%d xDim:%d row:%d interleave:%d\n",
					x, y, buffer, xDim, row, interleave); fflush(stdout);
#endif
			}
			data = lfbptr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)];

			if((data & 0xffff) != (dataExpect & 0xffff)) {
#if 0
				printf("ERROR! x:%d y:%d buffer:%d xDim:%d row:%d interleave:%d\n",
					x, y, buffer, xDim, row, interleave); fflush(stdout);
#endif
				iLeave = (buffer < 2) ? interleave : (interleave ^ 0x1);
				fbiMemErrors[((row > 511) ? 1 : 0)][(iLeave<<1)]++;
				retVal = FXFALSE;
			}
			if(((data >> 16) & 0xffff) != ((dataExpect >> 16) & 0xffff)) {
#if 0
				printf("ERROR! x:%d y:%d buffer:%d xDim:%d row:%d interleave:%d\n",
					x, y, buffer, xDim, row, interleave); fflush(stdout);
#endif
				iLeave = (buffer < 2) ? interleave : (interleave ^ 0x1);
				fbiMemErrors[((row > 511) ? 1 : 0)][(iLeave<<1)+1]++;
				retVal = FXFALSE;
			}
			dataExpect += rndmAdd;
		}
	}
	return retVal;
}
