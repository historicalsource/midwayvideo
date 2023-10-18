/*
** Copyright (c) 1997, 3Dfx Interactive, Inc.
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
** $Date: 3/11/98 2:25p $ 
**
*/
#include <glide.h>

#define RED_565    0xF800
#define GRN_565    0x07E0
#define BLU_565    0x001F
#define INVBLU_565 0xFFE0
#define INVGRN_565 0xF81F
#define INVRED_565 0x07FF
#define WHT_565    0xFFFF
#define BLK_565    0x0000

#define NUM_X_BLCKS 7
#define NUM_Y_BLCKS 7

extern GrHwConfiguration hwconfig;

void qatest01(void)
{
	GrScreenResolution_t	resolution = GR_RESOLUTION_512x384;
	float						scrWidth   = 512.0f;
	float						scrHeight  = 384.0f;
	int						frames     = 57;
	GrLfbInfo_t				myLfbInfo;
	int						nBlckWdth;
	int						nBlckHght;

	FxU16  yBlckClrs[NUM_Y_BLCKS][NUM_X_BLCKS] = {
		{RED_565, GRN_565, BLU_565, INVRED_565, INVGRN_565, INVBLU_565, WHT_565},
		{RED_565, GRN_565, BLU_565, INVRED_565, INVGRN_565, INVBLU_565, WHT_565},
		{RED_565, GRN_565, BLU_565, INVRED_565, INVGRN_565, INVBLU_565, WHT_565},
		{RED_565, GRN_565, BLU_565, INVRED_565, INVGRN_565, INVBLU_565, WHT_565},
		{RED_565, GRN_565, BLU_565, INVRED_565, INVGRN_565, INVBLU_565, WHT_565},
		{RED_565, GRN_565, BLU_565, INVRED_565, INVGRN_565, INVBLU_565, WHT_565},
		{BLK_565, BLK_565, BLK_565, BLK_565,    BLK_565,    BLK_565,    BLK_565}
		};


	/* Initializations */
	nBlckWdth = ((int) scrWidth / NUM_X_BLCKS);
	nBlckHght = ((int) scrHeight / NUM_Y_BLCKS);
  
	myLfbInfo.size = sizeof(GrLfbInfo_t);
	if(!grLfbLock(GR_LFB_WRITE_ONLY, GR_BUFFER_BACKBUFFER, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &myLfbInfo))
	{
		frames=0;
	}
  
	while(frames--)
	{
		int	i, j, k, l;
		FxU16	*lpPxl;
		FxU32	stride = myLfbInfo.strideInBytes;
		void	*tmpLfb;

		tmpLfb = (void*)((FxU16*)myLfbInfo.lfbPtr + 1 + 2*stride/2);
		grBufferClear(0xffffff, 0, GR_WDEPTHVALUE_FARTHEST);
    
		for(i=0; i < NUM_Y_BLCKS; ++i)
		{
			for(j=0; j < NUM_X_BLCKS; ++j)
			{
				lpPxl = ((FxU16 *) tmpLfb + j*nBlckWdth) + (i*nBlckHght*stride/2);
				for(k=0; k < nBlckHght; ++k)
				{
					for(l=0; l < nBlckWdth; ++l)
					{
						*lpPxl = yBlckClrs[i][j];
						++lpPxl;
					}
					lpPxl += (stride/2 - nBlckWdth);  /* inc in pixel space */
				}
			}
		}
   
		grBufferSwap(1);
		if(hwconfig.SSTs[0].type == GR_SSTTYPE_SST96)
		{
			grClipWindow(0, 0, (FxU32)scrWidth, (FxU32)scrHeight);
		} 
	}
	grLfbUnlock(GR_LFB_WRITE_ONLY, GR_BUFFER_BACKBUFFER); 
	printf("\f");
}
