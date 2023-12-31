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
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/gsstdef.h 1     9/02/98 12:36a Sapphire $
** $Log: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/gsstdef.h $
** 
** 1     9/02/98 12:36a Sapphire
 * 
 * 1     1/16/98 4:29p Atai
 * create glide 3 src
 * 
 * 4     5/27/97 1:16p Peter
 * Basic cvg, w/o cmd fifo stuff. 
 * 
 * 3     5/21/97 6:05a Peter
**
*/
#ifndef __GSSTDEF_H__
#define __GSSTDEF_H__


#if (GLIDE_PLATFORM & GLIDE_HW_CVG)
#include <cvg.h>
#else
#include <sst.h>
#endif

/*----------------- SST chip layout -----------------------*/
typedef enum
{
   SSTR_STATUS,
   SSTR_RESERVED0,
   SSTR_VAX,
   SSTR_VAY,
   SSTR_VBX,
   SSTR_VBY,
   SSTR_VCX,
   SSTR_VCY,
#ifdef GLIDE_USE_ALT_REGMAP
   SSTR_R,
   SSTR_DRDX,
   SSTR_DRDY,

   SSTR_G,
   SSTR_DGDX,
   SSTR_DGDY,

   SSTR_B,
   SSTR_DBDX,
   SSTR_DBDY,

   SSTR_Z,
   SSTR_DZDX,
   SSTR_DZDY,

   SSTR_A,
   SSTR_DADX,
   SSTR_DADY,

   SSTR_S,
   SSTR_DSDX,
   SSTR_DSDY,

   SSTR_T,
   SSTR_DTDX,
   SSTR_DTDY,

   SSTR_W,
   SSTR_DWDX,
   SSTR_DWDY,
#else
   SSTR_R,
   SSTR_G,
   SSTR_B,
   SSTR_Z,
   SSTR_A,
   SSTR_S,
   SSTR_T,
   SSTR_W,

   SSTR_DRDX,
   SSTR_DGDX,
   SSTR_DBDX,
   SSTR_DZDX,
   SSTR_DADX,
   SSTR_DSDX,
   SSTR_DTDX,
   SSTR_DWDX,

   SSTR_DRDY,
   SSTR_DGDY,
   SSTR_DBDY,
   SSTR_DZDY,
   SSTR_DADY,
   SSTR_DSDY,
   SSTR_DTDY,
   SSTR_DWDY,
#endif
   SSTR_TRIANGLECMD,
   SSTR_RESERVED1,

   SSTR_FVAX,
   SSTR_FVAY,
   SSTR_FVBX,
   SSTR_FVBY,
   SSTR_FVCX,
   SSTR_FVCY,
#ifdef GLIDE_USE_ALT_REGMAP
   SSTR_FR,
   SSTR_FDRDX,
   SSTR_FDRDY,
                
   SSTR_FG,
   SSTR_FDGDX,
   SSTR_FDGDY,

   SSTR_FB,
   SSTR_FDBDX,
   SSTR_FDBDY,

   SSTR_FZ,
   SSTR_FDZDX,
   SSTR_FDZDY,

   SSTR_FA,
   SSTR_FDADX,
   SSTR_FDADY,

   SSTR_FS,
   SSTR_FDSDX,
   SSTR_FDSDY,

   SSTR_FT,
   SSTR_FDTDX,
   SSTR_FDTDY,

   SSTR_FW,
   SSTR_FDWDX,
   SSTR_FDWDY,
#else 
   SSTR_FR,
   SSTR_FG,
   SSTR_FB,
   SSTR_FZ,
   SSTR_FA,
   SSTR_FS,
   SSTR_FT,
   SSTR_FW,

   SSTR_FDRDX,
   SSTR_FDGDX,
   SSTR_FDBDX,
   SSTR_FDZDX,
   SSTR_FDADX,
   SSTR_FDSDX,
   SSTR_FDTDX,
   SSTR_FDWDX,

   SSTR_FDRDY,
   SSTR_FDGDY,
   SSTR_FDBDY,
   SSTR_FDZDY,
   SSTR_FDADY,
   SSTR_FDSDY,
   SSTR_FDTDY,
   SSTR_FDWDY,
#endif
   SSTR_FTRIANGLECMD,
   SSTR_FBZCOLORPATH,
   SSTR_FOGMODE,
   SSTR_ALPHAMODE,
   SSTR_FBZMODE,
   SSTR_LFBMODE,
   SSTR_CLIPLEFTRIGHT,
   SSTR_CLIPBOTTOMTOP,

   SSTR_NOPCMD,
   SSTR_FASTFILLCMD,
   SSTR_SWAPBUFFERCMD,
   SSTR_FOGCOLOR,
   SSTR_ZACOLOR,
   SSTR_CHROMAKEY,
   SSTR_RESERVED2,
   SSTR_RESERVED3,

   SSTR_STIPPLE,
   SSTR_C0,
   SSTR_C1,

   SSTR_FBIPIXELSIN,
   SSTR_FBICHROMAFAIL,
   SSTR_FBIZFUNCFAIL,
   SSTR_FBIAFUNCFAIL,
   SSTR_FBIPIXELSOUT,

   SSTR_FOGTABLE,
   SSTR_RESERVED8 = SSTR_FOGTABLE + 32,

   SSTR_FBIINIT4 = SSTR_RESERVED8 + 8,
   SSTR_VRETRACE,
   SSTR_BACKPORCH,
   SSTR_VIDEODIMENSIONS,
   SSTR_FBIINIT0,
   SSTR_FBIINIT1,
   SSTR_FBIINIT2,
   SSTR_FBIINIT3,

   SSTR_HSYNC,
   SSTR_VSYNC,
   SSTR_CLUTDATA,
   SSTR_DACDATA,
   SSTR_MAX_RGB_DELTA,
   SSTR_RESERVED51,

   SSTR_TEXTUREMODE = SSTR_RESERVED51 + 51,
   SSTR_TLOD,
   SSTR_TDETAIL,
   SSTR_TEXBASEADDR,
   SSTR_TEXBASEADDR1,
   SSTR_TEXBASEADDR2,
   SSTR_TEXBASEADDR38,
   SSTR_TEXINIT0,
   SSTR_TEXINIT1,

   SSTR_NCCTABLE0,
   SSTR_NCCTABLE1 = SSTR_NCCTABLE0 + 12,
   SSTR_END_OF_REGISTER_SET
} GrSstRegister;

#endif /* __GSSTDEF_H__ */
