/*
** Copyright (c) 1996, 3Dfx Interactive, Inc.
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
** $Date: 10/16/97 8:18p $
*/

/* Fast memory test for TMU memory */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sst.h>
#include <3dfx.h>
#include <sst1init.h>

int tmuMemErrors[2][2][4];

static void clearScreen(FxU32 *, FxU32);
static void drawRect(FxU32 *, FxU32, FxU32, FxU32, FxU32, FxU32);
static void drawRectUsingTris(FxU32 *, int x, int y, int);
static FxBool checkTmuMemConst(FxU32 *, int, int, FxU32);
static FxBool checkTmuMemWhite(FxU32 *, int, int);
static FxBool checkTmuMemBlack(FxU32 *, int, int);
static FxBool checkTmuMem55aa(FxU32 *, int, int);
static FxBool checkTmuMemaa55(FxU32 *, int, int);
static FxBool checkTmuMemFlatRndm(FxU32 *, int, int);
static FxBool checkTmuMemRndm(FxU32 *, int, int);
static void printTmuMemErrors(int numTmus);

FxBool testTMUMem(void)
{
	int i, j, k;
	FxU32 *sstbase;
	volatile Sstregs *sst;
	sst1DeviceInfoStruct deviceInfo;
	int numberTmus = -1, tmuMemSize = -1;
	FxBool retVal;

//    numberTmus = 1;
//    tmuMemSize = 4;

    if(!(sstbase = sst1InitMapBoard(0))) return (FXFALSE);
    if(!(sst1InitRegisters(sstbase))) return (FXFALSE);
    if(!(sst1InitGamma(sstbase, 1.0))) return (FXFALSE);

    sst = (Sstregs *) sstbase;
    if(sst1InitGetDeviceInfo(sstbase, &deviceInfo) == FXFALSE) return (FXFALSE);

    if(numberTmus < 0) numberTmus = (int) deviceInfo.numberTmus;
	if(tmuMemSize < 0) tmuMemSize = (int) deviceInfo.tmuMemSize[0];

    if(!(sst1InitVideo(sstbase, GR_RESOLUTION_512x400, GR_REFRESH_60Hz,
          (sst1VideoTimingStruct *) NULL))) return (FXFALSE);

    /* Turn off memory fifo so that if FBI memory is corrupt the diag will */
	/* still run without hanging */
	sst1InitIdle(sstbase);
	sst->fbiInit0 &= ~SST_MEM_FIFO_EN;
	sst1InitIdle(sstbase);

	/* Initialize error structure */
	for(k=0; k<2; k++) {
		for(j=0; j<2; j++) {
			for(i=0; i<4; i++)
				tmuMemErrors[k][j][i] = 0;
		}
	}

    retVal = FXTRUE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkTmuMemBlack(sstbase, numberTmus, tmuMemSize) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkTmuMemWhite(sstbase, numberTmus, tmuMemSize) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkTmuMemaa55(sstbase, numberTmus, tmuMemSize) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkTmuMem55aa(sstbase, numberTmus, tmuMemSize) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkTmuMemFlatRndm(sstbase, numberTmus, tmuMemSize) == FXFALSE)
		retVal = FXFALSE;
    if (getinput(0, 0x0064)) return (FXTRUE);
    if(checkTmuMemRndm(sstbase, numberTmus, tmuMemSize) == FXFALSE)
        retVal = FXFALSE;

    printTmuMemErrors(numberTmus);

    sst1InitShutdown(sstbase);

    return (retVal);
}

static void printTmuMemErrors(numTmus)
{
#if 0
    printf("\nTMU0 Error Summary:\n");
	printf("-------------------\n");
    printf("tex_data_0 --> Bank0: %d\t\tBank1: %d\n",
        tmuMemErrors[0][0][0], tmuMemErrors[0][1][0]);
    printf("tex_data_1 --> Bank0: %d\t\tBank1: %d\n",
        tmuMemErrors[0][0][1], tmuMemErrors[0][1][1]);
    printf("tex_data_2 --> Bank0: %d\t\tBank1: %d\n",
        tmuMemErrors[0][0][2], tmuMemErrors[0][1][2]);
    printf("tex_data_3 --> Bank0: %d\t\tBank1: %d\n",
        tmuMemErrors[0][0][3], tmuMemErrors[0][1][3]);

    if(numTmus > 1) {
		printf("\nTMU1 Error Summary:\n");
		printf("-------------------\n");
		printf("tex_data_0 --> Bank0: %d\t\tBank1: %d\n", 
			tmuMemErrors[1][0][0], tmuMemErrors[1][1][0]);
		printf("tex_data_1 --> Bank0: %d\t\tBank1: %d\n", 
			tmuMemErrors[1][0][1], tmuMemErrors[1][1][1]);
		printf("tex_data_2 --> Bank0: %d\t\tBank1: %d\n", 
			tmuMemErrors[1][0][2], tmuMemErrors[1][1][2]);
		printf("tex_data_3 --> Bank0: %d\t\tBank1: %d\n", 
			tmuMemErrors[1][0][3], tmuMemErrors[1][1][3]);
    }
#endif
}
static void clearScreen(FxU32 *sstbase, FxU32 color)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;

    sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_FRONT;
    drawRect(sstbase, 0, 0, 512, 400, color);
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

static void drawRectUsingTris(FxU32 *sstbase, int x, int y, int tSize)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;

	sst->vA.x = (x<<SST_XY_FRACBITS);
	sst->vA.y = (y<<SST_XY_FRACBITS);
	sst->vB.x = ((x+tSize)<<SST_XY_FRACBITS);
	sst->vB.y = (y<<SST_XY_FRACBITS);
	sst->vC.x = ((x+tSize)<<SST_XY_FRACBITS);
	sst->vC.y = ((y+tSize)<<SST_XY_FRACBITS);
	sst->s = 0;
	sst->t = 0;
	sst->w = 0;
	sst->r = (0xff<<SST_XY_INTBITS);
	sst->g = 0;
	sst->b = 0;
	sst->dsdx = (0x1<<SST_ST_FRACBITS);
	sst->dtdx = 0;
	sst->dwdx = 0;
	sst->dsdy = 0;
	sst->dtdy = (0x1<<SST_ST_FRACBITS);
	sst->dwdy = 0;
	sst->triangleCMD = 0;
	sst->vB.x = (x<<SST_XY_FRACBITS);
	sst->vB.y = ((y+tSize)<<SST_XY_FRACBITS);
	sst->triangleCMD = 0xFFFFFFFF;
}

static FxBool checkTmuMemWhite(FxU32 *sstbase, int numberTmus, int tmuMemSize)
{
	return(checkTmuMemConst(sstbase, numberTmus, tmuMemSize, 0xffffffff));
}

static FxBool checkTmuMemBlack(FxU32 *sstbase, int numberTmus, int tmuMemSize)
{
	return(checkTmuMemConst(sstbase, numberTmus, tmuMemSize, 0x0));
}

static FxBool checkTmuMem55aa(FxU32 *sstbase, int numberTmus, int tmuMemSize)
{
	return(checkTmuMemConst(sstbase, numberTmus, tmuMemSize, 0x55aaaa55));
}

static FxBool checkTmuMemaa55(FxU32 *sstbase, int numberTmus, int tmuMemSize)
{
	return(checkTmuMemConst(sstbase, numberTmus, tmuMemSize, 0xaa5555aa));
}

static FxBool checkTmuMemFlatRndm(FxU32 *sstbase, int numberTmus, int tmuMemSize)
{
	FxU32 i, dataExpect = 0xbaddead;
	FxBool retVal = FXTRUE;

	for(i=0; i<5; i++) {
		if(checkTmuMemConst(sstbase, numberTmus, tmuMemSize, dataExpect) ==
		  FXFALSE)
			retVal = FXFALSE;
		dataExpect += 0x74978193;
	}

	return(retVal);
}

static FxBool checkTmuMemConst(FxU32 *sstbase, int numberTmus, int tmuMemSize,
	FxU32 dataExpect)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;
	FxBool retVal = FXTRUE;
	int numTmu;
	FxU32 texBaseAddr;
	volatile FxU32 *lfbAddr, *texAddr;
	FxU32 x, y, i, dataRead;

	for(numTmu = 0; numTmu<numberTmus; numTmu++) {
		for(texBaseAddr = 0; texBaseAddr < ((FxU32 ) tmuMemSize << 20);
		  texBaseAddr+=(1<<17)) {

			clearScreen(sstbase, 0x0);

			for(i=0; i<2; i++) {
				if(!i) {
					/* Fill Texture with Random Values */
					sst->textureMode = SST_RGB565 | SST_TC_REPLACE | SST_TCA_ZERO;
					sst->tLOD = 0x0;
					sst->tDetail = 0x0;
					sst->texBaseAddr = (texBaseAddr>>3);
					texAddr = (numTmu<<(21-2)) + (FxU32 *)SST_TEX_ADDRESS(sst);
					for(y=0; y<256; y++) {
						for(x=0; x<256;x+=2) {
							texAddr[(y<<7)+(x>>1)] = (y<<8) + x;
						}
						texAddr += (0x200 >> 2);
					}
				} else {
					/* Download Texture */
					sst->textureMode = SST_RGB565 | SST_TC_REPLACE | SST_TCA_ZERO;
					sst->tLOD = 0x0;
					sst->tDetail = 0x0;
					sst->texBaseAddr = (texBaseAddr>>3);
					texAddr = (numTmu<<(21-2)) + (FxU32 *)SST_TEX_ADDRESS(sst);
					for(y=0; y<256; y++) {
						for(x=0; x<256; x+=2) {
							texAddr[(x>>1)] = dataExpect;
							/* texAddr[(x>>1)] = 0x07e007e0; */
						}
						texAddr += (0x200 >> 2);
					}
				}
			}

			/* Render Rectangle for testing (two tris) */
			if(numTmu == 0) {
				sst->textureMode = SST_RGB565 | SST_TC_REPLACE | SST_TCA_ZERO;
			} else if(numTmu == 1) {
				/* Force downstream TMU to passthrough upstream data */
				SST_TREX(sst,0)->textureMode = SST_RGB565 | SST_TC_PASS |
				  SST_TCA_PASS;
				SST_TREX(sst,1)->textureMode = SST_RGB565 | SST_TC_REPLACE |
				  SST_TCA_ZERO;
			}
			sst->tLOD = 0x0;
			sst->tDetail = 0x0;
			sst->texBaseAddr = (texBaseAddr>>3);
			sst->fogMode = 0x0;
			sst->alphaMode = 0x0;
			sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_FRONT;
			sst->fbzColorPath = SST_RGBSEL_TREXOUT | SST_CC_PASS |
				SST_ENTEXTUREMAP;
			drawRectUsingTris(sstbase, 0, 0, 256);
			sst1InitIdle(sstbase);

    		sst->lfbMode = SST_LFB_565 | SST_LFB_READFRONTBUFFER;
			sst1InitIdle(sstbase);
			lfbAddr = (unsigned long *) sstbase;
			for(y=0; y<256; y++) {
				for(x=0; x<256; x+=2) {
					dataRead = lfbAddr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)];
					/* dataExpect = 0x07e007e0; */
					if(dataRead != dataExpect) {
						FxU32 bank = (texBaseAddr < (2<<20)) ? 0 : 1;
						FxU32 offset = (y & 1) ? 2 : 0;
						retVal = FXFALSE;
						if((dataRead&0xffff) != (dataExpect&0xffff))
							tmuMemErrors[numTmu][bank][offset]++;
						if(((dataRead>>16)&0xffff) != ((dataExpect>>16)&0xffff))
							tmuMemErrors[numTmu][bank][offset+1]++;
					}
				}
			}
		}
	}
	return retVal;
}

static FxBool checkTmuMemRndm(FxU32 *sstbase, int numberTmus, int tmuMemSize)
{
	volatile Sstregs *sst = (Sstregs *) sstbase;
	FxBool retVal = FXTRUE;
	int numTmu;
	FxU32 texBaseAddr;
	volatile FxU32 *lfbAddr, *texAddr;
	FxU32 x, y, i, dataRead, dataExpect;

	for(numTmu = 0; numTmu<numberTmus; numTmu++) {
		for(texBaseAddr = 0; texBaseAddr < ((FxU32 ) tmuMemSize << 20);
		  texBaseAddr+=(1<<17)) {

			clearScreen(sstbase, 0x0);

			for(i=0; i<2; i++) {
				if(!i) {
					/* Fill Texture with Random Values */
					sst->textureMode = SST_RGB565 | SST_TC_REPLACE | SST_TCA_ZERO;
					sst->tLOD = 0x0;
					sst->tDetail = 0x0;
					sst->texBaseAddr = (texBaseAddr>>3);
					texAddr = (numTmu<<(21-2)) + (FxU32 *)SST_TEX_ADDRESS(sst);
					for(y=0; y<256; y++) {
						for(x=0; x<256;x+=2) {
							texAddr[(y<<7)+(x>>1)] = (y<<8) + x;
						}
						texAddr += (0x200 >> 2);
					}
				} else {
					/* Download Texture */
					sst->textureMode = SST_RGB565 | SST_TC_REPLACE | SST_TCA_ZERO;
					sst->tLOD = 0x0;
					sst->tDetail = 0x0;
					sst->texBaseAddr = (texBaseAddr>>3);
					texAddr = (numTmu<<(21-2)) + (FxU32 *)SST_TEX_ADDRESS(sst);
					dataExpect = 0xbaddead;
					for(y=0; y<256; y++) {
						for(x=0; x<256; x+=2) {
							texAddr[(x>>1)] = dataExpect;
							dataExpect += 0x34972195;
						}
						texAddr += (0x200 >> 2);
					}
				}
			}

			/* Render Rectangle for testing (two tris) */
			if(numTmu == 0) {
				sst->textureMode = SST_RGB565 | SST_TC_REPLACE | SST_TCA_ZERO;
			} else if(numTmu == 1) {
				/* Force downstream TMU to passthrough upstream data */
				SST_TREX(sst,0)->textureMode = SST_RGB565 | SST_TC_PASS |
				  SST_TCA_PASS;
				SST_TREX(sst,1)->textureMode = SST_RGB565 | SST_TC_REPLACE |
				  SST_TCA_ZERO;
			}
			sst->tLOD = 0x0;
			sst->tDetail = 0x0;
			sst->texBaseAddr = (texBaseAddr>>3);
			sst->fogMode = 0x0;
			sst->alphaMode = 0x0;
			sst->fbzMode = SST_RGBWRMASK | SST_DRAWBUFFER_FRONT;
			sst->fbzColorPath = SST_RGBSEL_TREXOUT | SST_CC_PASS |
				SST_ENTEXTUREMAP;
			drawRectUsingTris(sstbase, 0, 0, 256);
			sst1InitIdle(sstbase);

    		sst->lfbMode = SST_LFB_565 | SST_LFB_READFRONTBUFFER;
			sst1InitIdle(sstbase);
			lfbAddr = (unsigned long *) sstbase;
			dataExpect = 0xbaddead;
			for(y=0; y<256; y++) {
				for(x=0; x<256; x+=2) {
					dataRead = lfbAddr[((SST_LFB_ADDR+(x<<1)+(y<<11))>>2)];
					if(dataRead != dataExpect) {
						FxU32 bank = (texBaseAddr < (2<<20)) ? 0 : 1;
						FxU32 offset = (y & 1) ? 2 : 0;
						retVal = FXFALSE;
						if((dataRead&0xffff) != (dataExpect&0xffff))
							tmuMemErrors[numTmu][bank][offset]++;
						if(((dataRead>>16)&0xffff) != ((dataExpect>>16)&0xffff))
							tmuMemErrors[numTmu][bank][offset+1]++;
					}
					dataExpect += 0x34972195;
				}
			}
		}
	}
	return retVal;
}
