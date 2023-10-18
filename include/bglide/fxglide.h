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
** $Header: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/fxglide.h 3     9/21/98 5:41p Atai $
** $Log: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/fxglide.h $
** 
** 3     9/21/98 5:41p Atai
** sync to top of tree
** 
** 58    9/11/98 10:45p Jdt
** Switch over to statically allocated in-memory fifo.
** 
** 57    9/04/98 11:35a Peter
** re-open fix for nt (thanks to taco/rob/nt bob)
** 
** 56    8/31/98 10:33a Peter
** asm w/ debugging
** 
** 55    8/30/98 10:54p Jdt
** promote INVALIDATE macro to global dominion
** 
** 54    8/30/98 1:34p Dow
** State & other optimizations
** 
** 53    8/29/98 8:12p Dow
** Clip optimization
** 
** 52    8/29/98 4:34p Dow
** thread optimization stuff
** 
** 51    8/27/98 9:27p Atai
** fix env variable for glide3x
** 
** 50    7/01/98 8:40a Jdt
** removed gc arg from trisetup functions
** 
** 49    8/03/98 6:34a Jdt
** Changes for multi-thread
** 
** 48    8/02/98 5:01p Dow
** Glide Surface Extension
** 
** 46    7/21/98 7:41p Jdt
** fixed palettes
** 
** 45    7/18/98 1:45p Jdt
** Removed TACO_MEMORY_FIFO_HACK
** 
** 44    7/18/98 12:25a Jdt
** Some clean up and new shadow register structure to support state
** restoration.
**
*/
            
/*                                               
** fxglide.h
**  
** Internal declarations for use inside Glide.
**
** GLIDE_LIB:        Defined if building the Glide Library.  This macro
**                   should ONLY be defined by a makefile intended to build
**                   GLIDE.LIB or glide.a.
**
** HAL_CSIM:         Defined if GLIDE should use the software simulator.  An
**                   application is responsible for defining this macro.
**
** GLIDE_NUM_TMU:    Number of physical TMUs installed.  Valid values are 1
**                   and 2.  If this macro is not defined by the application
**                   it is automatically set to the value 2.
**
*/

#ifndef __FXGLIDE_H__
#define __FXGLIDE_H__

/* -----------------------------------------------------------------------
   INCLUDE FILES
   ----------------------------------------------------------------------- */
/* standard */
#include <limits.h>
#include <math.h>
#include <stddef.h>

#ifndef __ROM__
#include <stdio.h>
#include <stdlib.h>
#endif

/* 3dfx */
#include <3dfx.h>
#include <glidesys.h>
#include <gdebug.h>
#include <h3.h>

/* local */
#include "gsfc.h"

/* conditional */

#if defined( GLIDE_INIT_HAL )
#include <fxhal.h>
#else
#include <minihwc.h>
//#define HWC_BASE_ADDR_MASK 0x04UL
#define HWC_BASE_ADDR_MASK 0x0fUL

#endif /* defined ( GLIDE_INIT_HAL ) */

/* -----------------------------------------------------------------------
   Manifest Constants
   ----------------------------------------------------------------------- */
#define MAX_NUM_SST                  4
#define MAX_NUM_CONTEXTS             16

#define VOODOO_GAMMA_TABLE_SIZE      32
#define SST1_BITS_DEPTH              16
#define SST1_ZDEPTHVALUE_NEAREST     0xFFFF
#define SST1_ZDEPTHVALUE_FARTHEST    0x0000
#define SST1_WDEPTHVALUE_NEAREST     0x0000
#define SST1_WDEPTHVALUE_FARTHEST    0xFFFF

#define GR_MAX_RESOLUTION  15
#define GR_MAX_REFRESH      8
#define GR_MAX_COLOR_BUF    3
#define GR_MAX_AUX_BUF      1
#define GR_MIN_RESOLUTION   0
#define GR_MIN_REFRESH      0
#define GR_MIN_COLOR_BUF    2
#define GR_MIN_AUX_BUF      0

#define GR_AA_ORDERED_OGL               0x00010000
#define GR_AA_ORDERED_POINTS_OGL        GR_AA_ORDERED_OGL+1
#define GR_AA_ORDERED_LINES_OGL         GR_AA_ORDERED_OGL+2
#define GR_AA_ORDERED_TRIANGLES_OGL     GR_AA_ORDERED_OGL+3

#define GR_AA_ORDERED_POINTS_MASK       0x01
#define GR_AA_ORDERED_LINES_MASK        0x02
#define GR_AA_ORDERED_TRIANGLES_MASK    0x04

/* -----------------------------------------------------------------------
   Code Macros
   ----------------------------------------------------------------------- */
#if defined( __MSC__ ) 
#define P6FENCE {_asm xchg eax, _GlideRoot.p6Fencer}
#elif __GOOSE__ 
//#include <fxhal.h>
#define P6FENCE	__asm__("	sync")
#else
#include <fxhal.h>
#error "P6 Fencing in-line assembler code needs to be added for this compiler"
#endif /* defined ( __MSC__ ) */

#undef  GETENV
#define GETENV getenv

#define GR_CDECL

/* -----------------------------------------------------------------------
   Internal Enumerated Types
   ----------------------------------------------------------------------- */
typedef int GrSstType;
#define GR_SSTTYPE_VOODOO    0
#define GR_SSTTYPE_SST96     1
#define GR_SSTTYPE_AT3D      2
#define GR_SSTTYPE_Voodoo2   3
#define GR_SSTTYPE_Banshee   4

/* -----------------------------------------------------------------------
   Internal Structures
   ----------------------------------------------------------------------- */
typedef struct GrTMUConfig_St {
  int    tmuRev;                /* Rev of Texelfx chip */
  int    tmuRam;                /* 1, 2, or 4 MB */
} GrTMUConfig_t;

typedef struct GrVoodooConfig_St {
  int    fbRam;                         /* 1, 2, or 4 MB */
  int    fbiRev;                        /* Rev of Pixelfx chip */
  int    nTexelfx;                      /* How many texelFX chips are there? */
  FxBool sliDetect;                     /* Is it a scan-line interleaved board? */
  GrTMUConfig_t tmuConfig[GLIDE_NUM_TMU];   /* Configuration of the Texelfx chips */
} GrVoodooConfig_t;

typedef struct GrSst96Config_St {
  int   fbRam;                  /* How much? */
  int   nTexelfx;
  GrTMUConfig_t tmuConfig;
} GrSst96Config_t;

typedef GrVoodooConfig_t GrVoodoo2Config_t;

typedef struct GrAT3DConfig_St {
  int   rev;
} GrAT3DConfig_t;

typedef struct {
  int num_sst;                  /* # of HW units in the system */
  struct {
    GrSstType type;             /* Which hardware is it? */
    union SstBoard_u {
      GrVoodooConfig_t  VoodooConfig;
      GrSst96Config_t   SST96Config;
      GrAT3DConfig_t    AT3DConfig;
      GrVoodoo2Config_t Voodoo2Config;
    } sstBoard;
  } SSTs[MAX_NUM_SST];          /* configuration for each board */
} GrHwConfiguration;

/*
** -----------------------------------------------------------------------
** STUFF FOR STRIPS
** -----------------------------------------------------------------------
*/

#define GR_COLOR_OFFSET_RED     (0 << 2)
#define GR_COLOR_OFFSET_GREEN   (1 << 2)
#define GR_COLOR_OFFSET_BLUE    (2 << 2)
#define GR_COLOR_OFFSET_ALPHA   (3 << 2)

#define GR_VERTEX_OFFSET_X      (0 << 2)
#define GR_VERTEX_OFFSET_Y      (1 << 2)
#define GR_VERTEX_OFFSET_Z      (2 << 2)
#define GR_VERTEX_OFFSET_WFBI   (3 << 2)

#define GR_TEXTURE_OFFSET_S     (0 << 2)
#define GR_TEXTURE_OFFSET_T     (1 << 2)
#define GR_TEXTURE_OFFSET_W     (2 << 2)

#define GR_DLIST_END            0x00
#define GR_VTX_PTR              0x00
#define GR_VTX_PTR_ARRAY        0x01
#define GR_SCALE_OOW            0x00
#define GR_SCALE_COLOR          0x01
#define GR_SCALE_STW            0x02

typedef struct {
  FxU32
    mode;                       /* enable / disable */
  FxI32
    offset;                     /* offset to the parameter data */
} GrVParamInfo;

typedef struct  {
  GrVParamInfo
    vertexInfo,          /* xy */
    zInfo,               /* z(ooz) */
    wInfo,               /* w(oow) */
    aInfo,               /* a float */
    fogInfo,             /* fog */
    rgbInfo,             /* rgb float */
    pargbInfo,           /* pargb byte */
    st0Info,             /* st0 */
    st1Info,             /* st1 */
    qInfo,               /* q */
    q0Info,              /* q0 */
    q1Info;              /* q1 */
  FxU32
    vStride,             /* vertex stride */
    vSize;               /* vertex size */
  FxU32
   colorType;           /* float or byte */
} GrVertexLayout;

/*============================================================
 **  State Monster Stuff:
 **============================================================*/
#define GR_FLUSH_STATE() \
  if (gc->state.invalid) _grValidateState();
      

/* Look in distate.c:grValidateState (NOTVALID macro) to see how these
   are used I wanted to keep the mixed-case register names here, and
   that's why they are mixed case */
#define alphaModeBIT            FXBIT(0)
#define fbzColorPathBIT         FXBIT(1)
#define fbzModeBIT              FXBIT(2)
#define chromaKeyBIT            FXBIT(3)
#define clipRegsBIT             FXBIT(4)
#define zaColorBIT              FXBIT(5)
#define fogModeBIT              FXBIT(6)
#define fogColorBIT             FXBIT(7)
#define lfbModeBIT              FXBIT(8)
#define c0c1BIT                 FXBIT(9)
#define chromaRangeBIT          FXBIT(10)
/*
** lazy evaluate vertexlayout.
** it is not part of the registers so we add the bit in MSB
*/ 
#define vtxlayoutBIT            FXBIT(31)

/*============================================================
 **  Video Stuff:
 **============================================================*/
#define VRETRACEMASK            0x00000fff
#define HRETRACEPOS             16


/*--------------------------------------------------------------------------
  State Restoration Buffer
  --------------------------------------------------------------------------*/
typedef struct {
    FxU32 pkt4Hdr_0;

#define OOII_IIOO_OIII_IIII  0x3C7f
#define SR_MASK_0            OOII_IIOO_OIII_IIII
#define SR_ADDR_0            ((FxU32) &((( SstRegs* )0)->fbzColorPath))
    FxU32 fbzColorPath;    /* 0x104 (  0 ) */
    FxU32 fogMode;         /* 0x108 (  1 ) */
    FxU32 alphaMode;       /* 0x10C (  2 ) */
    FxU32 fbzMode;         /* 0x110 (  3 ) */
    FxU32 lfbMode;         /* 0x114 (  4 ) */
    FxU32 clipLeftRight;   /* 0x118 (  5 ) */
    FxU32 clipBottomTop;   /* 0x11C (  6 ) */
    /* space */
    FxU32 fogColor;        /* 0x12C ( 10 ) */
    FxU32 zaColor;         /* 0x130 ( 11 ) */
    FxU32 chromaKey;       /* 0x134 ( 12 ) */
    FxU32 chromaRange;     /* 0x138 ( 13 ) */

    /* ----------- end packet -------------*/

#define OOOO_OOOO_OOOO_OIII  0x0007
#define SR_MASK_1            OOOO_OOOO_OOOO_OIII
#define SR_ADDR_1            ((FxU32) &((( SstRegs* )0)->stipple))
    FxU32 pkt4Hdr_1;
    FxU32 stipple;         /* 0x140 ( 15 ) */
    FxU32 color0;          /* 0x144 ( 16 ) */
    FxU32 color1;          /* 0x148 ( 17 ) */

    /* ----------- end packet -------------*/
#define SR_WORDS_2       32
#define SR_ADDR_2        ((FxU32) &((( SstRegs* )0)->fogTable[0]))
    FxU32 pkt1Hdr_2;

    FxU32 fogTable[32];

    /* ----------- end packet -------------*/

#define OOOO_OOOO_OOOO_IIII  0x000f
#define SR_MASK_3            OOOO_OOOO_OOOO_IIII
#define SR_ADDR_3            ((FxU32) &((( SstRegs* )0)->colBufferAddr))
    FxU32 pkt4Hdr_3;

    FxU32 colBufferAddr;   /* 0x1CC (  0 ) */
    FxU32 colBufferStride; /* 0x1D0 (  1 ) */
    FxU32 auxBufferAddr;   /* 0x1D4 (  2 ) */
    FxU32 auxBufferStride; /* 0x1D8 (  3 ) */
    
    /* ----------- end packet -------------*/

#define OOOO_OOOO_OIII_IIII  0x007f
#define SR_MASK_4            OOOO_OOOO_OIII_IIII
#define SR_ADDR_4            ((FxU32) &((( SstRegs* )0)->textureMode))
    FxU32 pkt4Hdr_4;

    FxU32 textureMode;     /* 0x300 (  0 ) */
    FxU32 tLOD;            /* 0x304 (  1 ) */
    FxU32 tDetail;         /* 0x308 (  2 ) */
    FxU32 texBaseAddr;     /* 0x30C (  3 ) */
    FxU32 texBaseAddr_1;   /* 0x310 (  4 ) */
    FxU32 texBaseAddr_2;   /* 0x314 (  5 ) */
    FxU32 texBaseAddr_3_8; /* 0x318 (  6 ) */

    /* ----------- end packet -------------*/
    
#define SR_WORDS_5       24
#define SR_ADDR_5        ((FxU32) &((( SstRegs* )0)->nccTable0[0]))
    FxU32 pkt1Hdr_5;
    
    FxU32 nccTable0[12];
    FxU32 nccTable1[12];

    /* ----------- end packet -------------*/

    struct PaletteRow {
#define SR_WORDS_P       8
#define SR_ADDR_P        ((FxU32) &((( SstRegs* )0)->nccTable0[4]))
        FxU32 pkt1Hdr_P;
        FxU32 data[8];
    } paletteRow[32];

} GrStateBuffer;   


/*==========================================================================*/
/*
** GrState
**
** This structure comprises the entire queryable state in Glide.
** 
** Two types of data qualify for inclusion here:
**
** API State  - cull-mode
** Chip State - hw register state
**
** Not included:
** any volatile data: eg fifo setup, colBufferAddr, etc
*/

typedef struct {
    FxU32 cull_mode;           /* cull neg, cull pos, don't cull   */
  
    FxU32 paramIndex;          /* Index into array containing
                                  parameter indeces to be sent ot the
                                  triangle setup code    */
    FxU32 tmuMask;             /* Tells the paramIndex updater which
                                  TMUs need values */

    GrStateBuffer shadow;      /* shadow of all hw state registers */

    struct PerTmuState {
        float s_scale;
        float t_scale;
        FxU32 mmMode;
        FxU32 smallLod;
        FxU32 largeLod;
        FxU32 evenOdd;
        FxU32 nccTable;
    } per_tmu[GLIDE_NUM_TMU];

  
    FxBool                       /* Values needed to determine which */
    ac_requires_it_alpha,      /*   parameters need gradients computed */
        ac_requires_texture,       /*   when drawing triangles      */
        cc_requires_it_rgb,
    cc_requires_texture,
    allowLODdither,            /* allow LOD dithering            */
    checkFifo;                 /* Check fifo status as specified by hints */
  
  FxU32
    lfb_constant_depth;        /* Constant value for depth buffer (LFBs) */
  GrAlpha_t
    lfb_constant_alpha;        /* Constant value for alpha buffer (LFBs) */
  
  FxU32
    num_buffers;               /* 2 or 3 */
  
  GrColorFormat_t
    color_format;              /* ARGB, RGBA, etc. */
  
  GrOriginLocation_t           /* lower left, upper left */
    origin;                     

  GrTexTable_t tex_table;      /* Current palette type - ncc vs pallette */
  
  float
    clipwindowf_xmin, clipwindowf_ymin, /* Clipping info */
    clipwindowf_xmax, clipwindowf_ymax;
  FxU32
    screen_width, screen_height; /* Screen width and height */
  
  /* viewport and clip space coordinate related stuff */
  
  struct {
    float
      n, f;
    FxFloat
      ox, oy, oz;
    FxFloat
      hwidth, hheight, hdepth;
  } Viewport;
  
  
  /* Strip Stuff */
  GrVertexLayout vData;

  /*============================================================
  **  State Monster Stuff:
  **============================================================*/
  /*  
  **   The following DWORD is used to determine what state (if any) needs to
  **   be flushed when a rendering primative occurs.
  */
  FxU32
    invalid;
  /* invalid contains bits representing:
     alphaMode register:
        modified by grAlphaBlendFunction, grAlphaTestFunction,
        grAlphaTestReferenceValue  

     fbzColorPath register:
        modified by grAlphaCombine, grAlphaControlsITRGBLighting,
        grColorCombine

     fbzMode register:
        modified by grChromaKeyMode, grDepthBufferFunction,
        grDeptBufferMode, grDepthMask, grDitherMode, grRenderBuffer,
        grSstOrigin, grColorMask 

     chromaKey register:
        modified by grChromaKeyValue

     clipLeftRight, clipBottomTop registers:
        modified by grClipWindow

     zaColor register:
        modified by grDepthBiasLevel

     fogMode register:
        modified by grFogMode

     fogColor register:
        modified by grFocColorValue

     lfbMode register:
        modified by grLfbWriteColorFormat, grLfbWriteColorSwizzle 

     c0 & c1 registers:
        modified by grConstanColorValue
   */

  /*
  **  Argument storage for State Monster:
  **
  **  NOTE that the data structure element names are IDENTICAL to the function
  **  argment names.  This is very important, as there are macros in distate.c
  **  that require that.
  */
  struct {
    struct {
      GrAlphaBlendFnc_t rgb_sf;
      GrAlphaBlendFnc_t rgb_df;
      GrAlphaBlendFnc_t alpha_sf;
      GrAlphaBlendFnc_t alpha_df;
    } grAlphaBlendFunctionArgs;
    struct {
      GrCmpFnc_t fnc;
    } grAlphaTestFunctionArgs;
    struct {
      GrAlpha_t value;
    } grAlphaTestReferenceValueArgs; 
    struct {
      GrCombineFunction_t function;
      GrCombineFactor_t factor;
      GrCombineLocal_t local;
      GrCombineOther_t other;
      FxBool invert;
    } grAlphaCombineArgs;
    struct {
      FxBool enable;
    } grAlphaControlsITRGBLightingArgs;
    struct {
      GrCombineFunction_t function;
      GrCombineFactor_t factor;
      GrCombineLocal_t local;
      GrCombineOther_t other;
      FxBool invert;
    } grColorCombineArgs;
    struct {
      FxBool rgb;
      FxBool alpha;
    } grColorMaskArgs;
    struct {
      GrChromakeyMode_t mode;
    } grChromakeyModeArgs;
    struct {
      GrColor_t color;
    } grChromakeyValueArgs;
    struct {
      GrColor_t range;
      GrChromaRangeMode_t mode;
      GrChromaRangeMode_t match_mode;
    } grChromaRangeArgs;
    struct {
      FxBool enable;
    } grDepthMaskArgs;
    struct {
      GrCmpFnc_t fnc;
    } grDepthBufferFunctionArgs;
    struct {
      GrDepthBufferMode_t mode;
    } grDepthBufferModeArgs;
    struct {
      GrDitherMode_t mode;
    } grDitherModeArgs;
    struct {
      GrBuffer_t buffer;
    } grRenderBufferArgs;
    struct {
      GrOriginLocation_t origin;
    } grSstOriginArgs;
    struct {
      FxU32 minx;
      FxU32 miny;
      FxU32 maxx;
      FxU32 maxy;
    } grClipWindowArgs;
    struct {
      FxU32 level;
    } grDepthBiasLevelArgs;
    struct {
      GrFogMode_t mode;
    } grFogModeArgs;
    struct {
      GrColor_t color;
    } grFogColorValueArgs;
    struct {
      GrColorFormat_t colorFormat;
    } grLfbWriteColorFormatArgs;
    struct {
      FxBool swizzleBytes;
      FxBool swapWords;
    } grLfbWriteColorSwizzleArgs;
    struct {
      GrColor_t color;
    } grConstantColorValueArgs;
  } stateArgs;
  struct{
    GrEnableMode_t primitive_smooth_mode;
    GrEnableMode_t shameless_plug_mode;
    GrEnableMode_t video_smooth_mode;
  } grEnableArgs;
  struct{
    GrCoordinateSpaceMode_t coordinate_space_mode;
  } grCoordinateSpaceArgs;
} GrState;

typedef struct GrGC_s
{
  struct {
    FxU32       bufferSwaps;    /* number of buffer swaps       */
    FxU32       pointsDrawn;
    FxU32       linesDrawn;
    FxU32       trisProcessed;
    FxU32       trisDrawn;
    FxU32       othertrisDrawn;

    FxU32       texDownloads;   /* number of texDownload calls   */
    FxU32       texBytes;       /* number of texture bytes downloaded   */

    FxU32       palDownloads;   /* number of palette download calls     */
    FxU32       palBytes;       /* number of palette bytes downloaded   */

    FxU32       nccDownloads;   /* # of NCC palette download calls */
    FxU32       nccBytes;       /* # of NCC palette bytes downloaded */

#if USE_PACKET_FIFO
    FxU32       fifoWraps;
    FxU32       fifoWrapDepth;
    FxU32       fifoStalls;
    FxU32       fifoStallDepth;
#endif /* USE_PACKET_FIFO */
  } stats;

  struct {
    float  ftemp1, ftemp2;       /* temps to convert floats to ints */
  } pool;

#if GLIDE_HW_TRI_SETUP
  FxI32 curVertexSize;          /* Size in bytes of a single vertex's parameters */
#endif

  FxI32 curTriSize;             /* the size in bytes of the current triangle */

  FxU32
    orgSW, orgSH;               /* Original Screen width & Height */
  FxU32
    totBuffers,
    strideInTiles,
    heightInTiles,
    bufferStride,
    bufSizeInTiles,
    bufSize,
    fbOffset,
    tramOffset,
    tramSize,
    *base_ptr,                  /* base address of SST */
    *reg_ptr,                   /* pointer to base of SST registers */
    *tex_ptr,                   /* texture memory address */
    *lfb_ptr,                   /* linear frame buffer address */
    *slave_ptr;                 /* Scanline Interleave Slave address */
  
#ifdef GLIDE_INIT_HWC
  hwcBoardInfo
    *bInfo;
#endif

#if GLIDE_MULTIPLATFORM
  GrGCFuncs
    gcFuncs;
#endif  

#define kMaxVertexParam (20 + (12 * GLIDE_NUM_TMU) + 3)

#ifndef GLIDE3
  struct dataList_s {
    int      i;
    FxFloat* addr;
  } regDataList[kMaxVertexParam];
  int tsuDataList[kMaxVertexParam];
#endif
  int tsuDataList[kMaxVertexParam];
#ifdef GLIDE3_SCALER
  int tsuDataListScaler[kMaxVertexParam];
#endif

  GrState
    state;                      /* state of Glide/SST */

  /* Here beginneth the Swap Pending Workaround (tm) */
#define MAX_BUFF_PENDING        0x7
  FxU32
    swapsPending,               /* swaps in unexecuted region of FIFO */
    lastSwapCheck,              /* Position at last check */
    curSwap,                    /* Position in the array below */
    bufferSwaps[MAX_BUFF_PENDING];/* Position in FIFO of buffer swaps */
  /* Here endeth the Swap Pending Workaround */

  struct cmdTransportInfo {
    FxU32  triPacketHdr; /* Pre-computed packet header for
                          * independent triangles. 
                          */
    
    FxU32  cullStripHdr; /* Pre-computed packet header for generic
                          * case of packet 3 triangles. This needs
                          * command type and # of vertices to be complete.
                          */
    
    FxU32  paramMask;    /* Mask for specifying parameters of
                          * non-triangle packets.  The parameter
                          * bits[21:10] mimic the packet3 header
                          * controlling which fields are sent, and
                          * pc[28] controls whether any color
                          * information is sent as packed.
                          */
    
    /* Basic command fifo characteristics. These should be
     * considered logically const after their initialization.
     */
    FxU32* fifoStart;    /* Virtual address of start of fifo */
    FxU32* fifoEnd;      /* Virtual address of fba fifo */
    FxU32  fifoOffset;   /* Offset from hw base to fifo start */
    FxU32  fifoSize;     /* Size in bytes of the fifo */
    FxU32  fifoJmpHdr;   /* Type0 packet for jmp to fifo start */

    
    FxU32* fifoPtr;      /* Current write pointer into fifo */
    FxU32  fifoRead;     /* Last known hw read ptr. 
                          * This is the sli master, if enabled.
                          */

    FxU32* stateBuffer;
    
#if GLIDE_USE_DEBUG_FIFO && !HAL_CSIM
    FxU32* fifoShadowBase; /* Buffer that shadows the hw fifo for debugging */
    FxU32* fifoShadowPtr;
#endif /* GLIDE_USE_DEBUG_FIFO */
    
    /* Fifo checking information. In units of usuable bytes until
     * the appropriate condition.
     */
    FxI32  fifoRoom;     /* Space until next fifo check */
    FxI32  roomToReadPtr;/* Bytes until last known hw ptr */
    FxI32  roomToEnd;    /* # of bytes until last usable address before fifoEnd */

    FxBool lfbLockCount; /* Have we done an lfb lock? Count of the locks. */
    
  } cmdTransportInfo;
  FxI32 (FX_CALL *triSetupProc)(const void *a, const void *b, const void *c);
  
  SstIORegs
    *ioRegs;                    /* I/O remap regs */
  SstCRegs
    *cRegs;                     /* AGP/Cmd xfer/misc regs */
  SstGRegs
    *gRegs;                     /* 2D regs */
  SstRegs
    *sstRegs;                   /* Graphics Regs (3D Regs) */
  FxU32
    *rawLfb,
    nBuffers,
    curBuffer,
    frontBuffer,
    backBuffer,
    buffers[3],
    lfbBuffers[4];              /* Tile relative addresses of the color/aux
                                 * buffers for lfbReads.
                                 */
  
  FxU32 lockPtrs[2];        /* pointers to locked buffers */
  FxU32 fbStride;

  struct {
    FxU32             freemem_base;
    FxU32             total_mem;
    FxU32             next_ncc_table;
    GrMipMapId_t      ncc_mmids[2];
    const GuNccTable *ncc_table[2];
  } tmu_state[GLIDE_NUM_TMU];

  int
    grSstRez,                   /* Video Resolution of board */
    grSstRefresh,               /* Video Refresh of board */
    fbuf_size,                  /* in MB */
    num_tmu,                    /* number of TMUs attached */
    grColBuf,
    grAuxBuf,
    grHwnd;

  FxBool
    scanline_interleaved;

#ifndef GLIDE3_ALPHA
  struct {
    GrMipMapInfo data[MAX_MIPMAPS_PER_SST];
    GrMipMapId_t free_mmid;
  } mm_table;                   /* mip map table */
#endif

  FxBool tmuLodDisable[GLIDE_NUM_TMU];

  /* DEBUG and SANITY variables */
  FxI32 myLevel;                /* debug level */
  FxI32 counter;                /* counts bytes sent to HW */
  FxI32 expected_counter;       /* the number of bytes expected to be sent */

  FxU32 checkCounter;
  FxU32 checkPtr;
   
  FxVideoTimingInfo* vidTimings;/* init code overrides */

  FxBool open;                  /* Has GC Been Opened? */
  FxBool auxRendering;          /* Is an aux rendering surface current? */
  FxBool windowed;              /* is this a fullscreen or windowed gc */
  FxBool hwInitP;               /* Has the hw associated w/ GC been initted and
                                   mapped?  This is managed in
                                   _grDetectResources:gpci.c the first time
                                   that the board is detected, and in
                                   grSstWinOpen:gsst.c if the hw has been
                                   shutdown in a call to grSstWinClose.
                                   */

#ifdef GLIDE_INIT_HWC
  hwcBufferDesc
    tBuffer,                    /* Texture Buuffer */
    *arBuffer;                  /* Aux Rendering Buffer */
#endif

/* <=32k bytes -- don't want segment overflow in 16-bit driver*/
#define WINDOW_FIFO_SIZE_IN_DWORDS 0x2000
  FxU32         windowedFifo[WINDOW_FIFO_SIZE_IN_DWORDS];
  GrStateBuffer windowedState;

} GrGC;


/*
**  The Root Of All EVIL!
**
**  The root of all Glide data, all global data is in here
**  stuff near the top is accessed a lot
*/
struct _GlideRoot_s {
  int p6Fencer;                 /* xchg to here to keep this in cache!!! */
  FxU32
    tlsIndex,
    tlsOffset;

  int current_sst;
  FxU32 CPUType;
  FxBool
    OSWin95,
    windowsInit;        /* Is the Windows part of glide initialized? */


#if !GLIDE_HW_TRI_SETUP || !GLIDE_PACKET3_TRI_SETUP
  FxU32 paramCount;
  FxI32 curTriSizeNoGradient;   /* special for _trisetup_nogradients */
#endif /* !GLIDE_HW_TRI_SETUP || !GLIDE_PACKET3_TRI_SETUP */

#if GLIDE_MULTIPLATFORM
  GrGCFuncs
    curGCFuncs;                 /* Current dd Function pointer table */
#endif
  int initialized;

  struct {                      /* constant pool (minimizes cache misses) */
    float  f0;
    float  fHalf;
    float  f1;
    float  f255;

#if GLIDE_PACKED_RGB
#define kPackBiasA _GlideRoot.pool.fBiasHi
#define kPackBiasR _GlideRoot.pool.fBiasHi
#define kPackBiasG _GlideRoot.pool.fBiasHi
#define kPackBiasB _GlideRoot.pool.fBiasLo

#define kPackShiftA 16UL
#define kPackShiftR 8UL
#define kPackShiftG 0UL
#define kPackShiftB 0UL

#define kPackMaskA  0x00FF00UL
#define kPackMaskR  0x00FF00UL
#define kPackMaskG  0x00FF00UL
#define kPackMaskB  0x00FFUL

    float  fBiasHi;
    float  fBiasLo;
#endif /* GLIDE_PACKED_RGB */
  } pool;

  struct {                      /* environment data             */
    FxBool ignoreReopen;
    FxBool triBoundsCheck;      /* check triangle bounds        */
    FxBool noSplash;            /* don't draw it                */
    FxBool shamelessPlug;       /* translucent 3Dfx logo in lower right */
    FxI32  swapInterval;        /* swapinterval override        */
    FxI32  swFifoLWM;
    FxU32  snapshot;            /* register trace snapshot      */
    FxBool disableDitherSub;    /* Turn off dither subtraction? */
    FxBool texLodDither;        /* Always do lod-dithering      */
    FxI32  tmuMemory;           /* tmuMemory */

    /* Force alternate buffer strategy */
    FxI32  nColorBuffer;
    FxI32  nAuxBuffer;
  } environment;

  GrHwConfiguration     hwConfig;
  
  FxU32                 gcNum;                  /* # of actual boards mapped */
  FxU32                 gcMap[MAX_NUM_SST];     /* Logical mapping between selectable
                                                 * sst's and actual boards.
                                                 */
  GrGC                  GCs[MAX_NUM_SST];       /* one GC per board     */
  GrGC                  surfaceGCs[MAX_NUM_CONTEXTS];
  GrGC                  *surfaceGCHeap[MAX_NUM_CONTEXTS];
};

extern struct _GlideRoot_s GR_CDECL _GlideRoot;
#if GLIDE_MULTIPLATFORM
extern GrGCFuncs _curGCFuncs;
#endif
/*==========================================================================*/
/* Macros for declaring functions */
#define GR_DDFUNC(name, type, args) \
   type FX_CSTYLE name args

#define GR_EXT_ENTRY( name, type, args ) \
   type FX_CSTYLE name args

#define GR_ENTRY(name, type, args) \
   FX_EXPORT type FX_CSTYLE name args

#define GR_FAST_ENTRY(name, type, args) \
   __declspec naked FX_EXPORT type FX_CSTYLE name args

#define GR_DIENTRY(name, type, args) \
   type FX_CSTYLE name args

#ifdef GLIDE3
#define GR_STATE_ENTRY(name, type, args) \
   type _##name## args
#else
#define GR_STATE_ENTRY(name, type, args) \
   GR_ENTRY(name, type, args)
#endif

/*==========================================================================*/

#define STATE_REQUIRES_IT_DRGB        FXBIT(0)
#define STATE_REQUIRES_IT_DRGB_SHIFT        0
#define STATE_REQUIRES_IT_ALPHA       FXBIT(1)
#define STATE_REQUIRES_IT_ALPHA_SHIFT       1
#define STATE_REQUIRES_OOZ            FXBIT(2)
#define STATE_REQUIRES_OOZ_SHIFT            2
#define STATE_REQUIRES_OOW_FBI        FXBIT(3)
#define STATE_REQUIRES_OOW_FBI_SHIFT        3
#define STATE_REQUIRES_W_TMU0         FXBIT(4)
#define STATE_REQUIRES_W_TMU0_SHIFT         4
#define STATE_REQUIRES_ST_TMU0        FXBIT(5)
#define STATE_REQUIRES_ST_TMU0_SHIFT        5
#define STATE_REQUIRES_W_TMU1         FXBIT(6)
#define STATE_REQUIRES_W_TMU1_SHIFT         6
#define STATE_REQUIRES_ST_TMU1        FXBIT(7)
#define STATE_REQUIRES_ST_TMU1_SHIFT        7
#define STATE_REQUIRES_W_TMU2         FXBIT(8)
#define STATE_REQUIRES_W_TMU2_SHIFT         8
#define STATE_REQUIRES_ST_TMU2        FXBIT(9)
#define STATE_REQUIRES_ST_TMU2_SHIFT        9

#define GR_TMUMASK_TMU0 FXBIT(GR_TMU0)
#define GR_TMUMASK_TMU1 FXBIT(GR_TMU1)
#define GR_TMUMASK_TMU2 FXBIT(GR_TMU2)

/*
**  Parameter gradient offsets
**
**  These are the offsets (in bytes)of the DPDX and DPDY registers from
**  from the P register
*/
#ifdef GLIDE_USE_ALT_REGMAP
#define DPDX_OFFSET 0x4
#define DPDY_OFFSET 0x8
#else
#define DPDX_OFFSET 0x20
#define DPDY_OFFSET 0x40
#endif

#if   (GLIDE_PLATFORM & GLIDE_HW_SST1)
#define GLIDE_DRIVER_NAME "Voodoo Graphics"
#elif (GLIDE_PLATFORM & GLIDE_HW_SST96)
#define GLIDE_DRIVER_NAME "Voodoo Rush"
#elif (GLIDE_PLATFORM & GLIDE_HW_CVG)
#define GLIDE_DRIVER_NAME "Voodoo^2"
#elif (GLIDE_PLATFORM & GLIDE_HW_H3)
#define GLIDE_DRIVER_NAME "Banshee"
#else 
#define GLIDE_DRIVER_NAME "HOOPTI???"
#endif

/*==========================================================================*/
#ifndef FX_GLIDE_NO_FUNC_PROTO

void _grMipMapInit(void);

/* NB: These routines are generally in asm (see xdraw2.inc), and have
 * their own internally defined calling conventions. The gc pointer
 * will be setup by the caller and will be in edx and that stack will
 * be configured as: return address, vertex a, vertex b, vertex c.  
 */
FxI32 FX_CSTYLE
_trisetup_cull(const void *va, const void *vb, const void *vc );
FxI32 FX_CSTYLE
_trisetup(const void *va, const void *vb, const void *vc );

FxI32 FX_CSTYLE
_trisetup_cull_noclip(const void *va, const void *vb, const void *vc );
FxI32 FX_CSTYLE
_trisetup_noclip(const void *va, const void *vb, const void *vc );

FxI32 FX_CSTYLE
_trisetup_cull_valid(const void *va, const void *vb, const void *vc );
FxI32 FX_CSTYLE
_trisetup_valid(const void *va, const void *vb, const void *vc );

FxI32 FX_CSTYLE
_trisetup_cull_noclip_valid(const void *va, const void *vb, const void *vc );
FxI32 FX_CSTYLE
_trisetup_noclip_valid(const void *va, const void *vb, const void *vc );


#define TRISETUP_NORGB(__cullMode) (((__cullMode) == GR_CULL_DISABLE) \
                                    ? _trisetup \
                                    : _trisetup_cull)

#define TRISETUP_NORGB_NOCLIP(__cullMode) (((__cullMode) == GR_CULL_DISABLE) \
                                    ? _trisetup_noclip \
                                    : _trisetup_cull_noclip)

#define TRISETUP_RGB(__cullMode)   TRISETUP_NORGB(__cullMode)
#define TRISETUP_ARGB(__cullMode)  TRISETUP_NORGB(__cullMode)
#define TRISETUP \
  __asm { mov edx, gc }; \
  (*gc->triSetupProc)

void
_grValidateState();

void FX_CSTYLE
_grDrawVertexList(FxU32 pktype, FxU32 type, FxI32 mode, FxI32 count, void *pointers);

void
_grAlphaBlendFunction(
                     GrAlphaBlendFnc_t rgb_sf,   GrAlphaBlendFnc_t rgb_df,
                     GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df
                     );
void
_grAlphaTestFunction( GrCmpFnc_t function );

void
_grAlphaTestReferenceValue( GrAlpha_t value );

void
_grAlphaCombine(
               GrCombineFunction_t function, GrCombineFactor_t factor,
               GrCombineLocal_t local, GrCombineOther_t other,
               FxBool invert
               );

void
_grAlphaControlsITRGBLighting( FxBool enable );

void
_grColorCombine(
               GrCombineFunction_t function, GrCombineFactor_t factor,
               GrCombineLocal_t local, GrCombineOther_t other,
               FxBool invert );

void FX_CALL 
_grChromaModeExt(GrChromaRangeMode_t mode);

void FX_CALL 
_grChromaRangeExt( GrColor_t min, GrColor_t max , GrChromaRangeMode_t mode);

void FX_CALL 
_grTexChromaModeExt(GrChipID_t tmu, GrTexChromakeyMode_t mode);

void FX_CALL 
_grTexChromaRangeExt(GrChipID_t tmu, GrColor_t min, GrColor_t max, GrTexChromakeyMode_t mode);

void
_grChromaRange( GrColor_t max , GrChromaRangeMode_t mode);

void
_grChromaMode( GrChromaRangeMode_t mode );

void
_grChromakeyMode( GrChromakeyMode_t mode );

void
_grChromakeyValue( GrColor_t color );

void
_grDepthMask( FxBool mask );

void
_grDepthBufferFunction( GrCmpFnc_t function );

void
_grDepthBufferMode( GrDepthBufferMode_t mode );

void
_grDitherMode( GrDitherMode_t mode );

void
_grRenderBuffer( GrBuffer_t buffer );

void
_grColorMask( FxBool rgb, FxBool a );

void
_grSstOrigin(GrOriginLocation_t  origin);

void
_grClipWindow( FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy );

void
_grDepthBiasLevel( FxI32 level );

void
_grFogMode( GrFogMode_t mode );

void
_grFogColorValue( GrColor_t fogcolor );

void
_grLfbWriteColorFormat(GrColorFormat_t colorFormat);

void
_grLfbWriteColorSwizzle(FxBool swizzleBytes, FxBool swapWords);

void
_grConstantColorValue( GrColor_t value );

void FX_CSTYLE
_grDrawPoints(FxI32 mode, FxI32 count, void *pointers);

void FX_CSTYLE
_grDrawLineStrip(FxI32 mode, FxI32 count, FxI32 ltype, void *pointers);

void FX_CSTYLE
_grDrawTriangles(FxI32 mode, FxI32 count, void *pointers);

void FX_CSTYLE
_grAADrawPoints(FxI32 mode, FxI32 count, void *pointers);

void FX_CSTYLE
_grAADrawLineStrip(FxI32 mode, FxI32 ltype, FxI32 count, void *pointers);

void FX_CSTYLE
_grAADrawLines(FxI32 mode, FxI32 count, void *pointers);

void FX_CSTYLE
_grAADrawTriangles(FxI32 mode, FxI32 ttype, FxI32 count, void *pointers);

void FX_CSTYLE
_grAAVpDrawTriangles(FxI32 mode, FxI32 ttype, FxI32 count, void *pointers);

void FX_CSTYLE
_grAADrawVertexList(FxU32 type, FxI32 mode, FxI32 count, void *pointers);

void FX_CSTYLE
_guTexMemReset(void);

int FX_CSTYLE
_grBufferNumPending(void);

FxBool FX_CSTYLE
_grSstIsBusy(void);

void FX_CSTYLE
_grSstResetPerfStats(void);

void FX_CSTYLE
_grResetTriStats(void);

FxU32 FX_CSTYLE
_grSstStatus(void);

FxU32 FX_CSTYLE
_grSstVideoLine(void);

FxBool FX_CSTYLE
_grSstVRetraceOn(void);

#endif /* FX_GLIDE_NO_FUNC_PROTO */

/*==========================================================================*/
/* 
**  thread stuff
*/

#if (GLIDE_PLATFORM & GLIDE_OS_WIN32)
#define W95_TEB_PTR                     0x18
#define W95_TEB_TLS_OFFSET              0x88
#define W95_TLS_INDEX_TO_OFFSET(i)      ((i)*sizeof(DWORD)+W95_TEB_TLS_OFFSET)

#define WNT_TEB_PTR                     0x18
#define WNT_TEB_TLS_OFFSET              0xE10
#define WNT_TLS_INDEX_TO_OFFSET(i)      ((i)*sizeof(DWORD)+WNT_TEB_TLS_OFFSET)

#define __GR_GET_TLSC_VALUE() \
__asm { \
   __asm mov eax, DWORD PTR fs:[WNT_TEB_PTR] \
   __asm add eax, DWORD PTR _GlideRoot.tlsOffset \
   __asm mov eax, DWORD PTR [eax] \
}

#pragma warning (4:4035)        /* No return value */
__inline FxU32
getThreadValueFast() {
  __asm {
    __asm mov eax, DWORD PTR fs:[WNT_TEB_PTR] 
    __asm add eax, DWORD PTR _GlideRoot.tlsOffset 
    __asm mov eax, DWORD PTR [eax] 
  }
}
#pragma warning (3:4035)
#endif

#define INVALIDATE(regset) {\
  gc->state.invalid |= ##regset##BIT;\
}
//KEENAN PORT
//  if(gc->state.grCoordinateSpaceArgs.coordinate_space_mode == GR_CLIP_COORDS)\
//    gc->triSetupProc = _trisetup_cull;\
//  else\
//    gc->triSetupProc = _trisetup_cull_noclip;\
//}

//KEENAN PORT
//void 
//initThreadStorage( void );
//
void 
setThreadValue( FxU32 value );

FxU32
getThreadValueSLOW( void );

void 
initCriticalSection( void );

void 
beginCriticalSection( void );

void 
endCriticalSection( void );


/*==========================================================================*/
/* 
**  function prototypes
*/

void
_grClipNormalizeAndGenerateRegValues(FxU32 minx, FxU32 miny, FxU32 maxx,
                                     FxU32 maxy, FxU32 *clipLeftRight,
                                     FxU32 *clipBottomTop);



void 
_grSwizzleColor(GrColor_t *color);

void
_grDisplayStats(void);

void
_GlideInitEnvironment(void);

void FX_CSTYLE
_grColorCombineDelta0Mode(FxBool delta0Mode);

void
_doGrErrorCallback(const char *name, const char *msg, FxBool fatal);

void _grErrorDefaultCallback(const char *s, FxBool fatal);

#ifdef __WIN32__
void _grErrorWindowsCallback(const char *s, FxBool fatal);
#endif /* __WIN32__ */

extern void
(*GrErrorCallback)(const char *string, FxBool fatal);

void GR_CDECL
_grFence(void);

int
_guHeapCheck(void);

void FX_CSTYLE
_grRebuildDataList(void);

void
_grReCacheFifo(FxI32 n);

FxI32 GR_CDECL
_grSpinFifo(FxI32 n);

void
_grShamelessPlug(void);

FxBool
_grSstDetectResources(void);

FxU16
_grTexFloatLODToFixedLOD(float value);

void FX_CSTYLE
_grTexDetailControl(GrChipID_t tmu, FxU32 detail);

void FX_CSTYLE
_grTexDownloadNccTable(GrChipID_t tmu, FxU32 which,
                        const GuNccTable *ncc_table,
                        int start, int end);

void FX_CSTYLE
_grTexDownloadPalette(GrChipID_t   tmu, 
                       GuTexPalette *pal,
                       int start, int end);

FxU32
_grTexCalcBaseAddress(
                      FxU32 start_address, GrLOD_t largeLod,
                      GrAspectRatio_t aspect, GrTextureFormat_t fmt,
                      FxU32 odd_even_mask); 

void
_grTexForceLod(GrChipID_t tmu, int value);

FxU32
_grTexTextureMemRequired(GrLOD_t small_lod, GrLOD_t large_lod, 
                           GrAspectRatio_t aspect, GrTextureFormat_t format,
                           FxU32 evenOdd);
void FX_CSTYLE
_grUpdateParamIndex(void);

/* ddgump.c */
void FX_CSTYLE
_gumpTexCombineFunction(int virtual_tmu);

/* disst.c - this is an un-documented external for arcade developers */
extern FX_ENTRY void FX_CALL
grSstVidMode(FxU32 whichSst, FxVideoTimingInfo* vidTimings);

/* glfb.c */
extern FxBool
_grLfbWriteRegion(FxBool pixPipelineP,
                  GrBuffer_t dst_buffer, FxU32 dst_x, FxU32 dst_y, 
                  GrLfbSrcFmt_t src_format, 
                  FxU32 src_width, FxU32 src_height, 
                  FxI32 src_stride, void *src_data);

/* gglide.c - Flushes the current state in gc->state.fbi_config to the hw.
 */
extern void
_grFlushCommonStateRegs(void);

/* gsst.c */
extern void 
initGC( GrGC *gc );
extern void 
assertDefaultState( void );


/*==========================================================================*/
/*  GMT: have to figure out when to include this and when not to
*/
#if defined(GLIDE_DEBUG) || defined(GLIDE_ASSERT) || defined(GLIDE_SANITY_ASSERT) || defined(GLIDE_SANITY_SIZE)
  #define DEBUG_MODE 1
  #include <assert.h>
#endif


#if 0
#define GR_DCL_GC GrGC *gc = (GrGC*)getThreadValueSLOW()
#else
//#define GR_DCL_GC GrGC *gc = (GrGC*)getThreadValueFast()
// KEENAN PORT
//#define GR_DCL_GC GrGC* gc = _GlideRoot.GCs + _GlideRoot.current_sst
#define GR_DCL_GC register GrGC* gc = _GlideRoot.GCs


#endif

#if GLIDE_INIT_HAL
#define GR_DCL_HW SstRegs *hw = (SstRegs *)gc->sstRegs
#else
//#define GR_DCL_HW SstRegs *hw = (SstRegs *)gc->sstRegs
#define GR_DCL_HW register SstRegs *hw = (SstRegs *)gc->sstRegs
#endif
#ifdef DEBUG_MODE
#define ASSERT(exp) GR_ASSERT(exp)

#define GR_BEGIN_NOFIFOCHECK(name,level) \
                GR_DCL_GC;      \
                GR_DCL_HW;      \
                const FxI32 saveLevel = gc->myLevel; \
                static char myName[] = name;  \
                GR_ASSERT(gc != NULL);  \
                gc->myLevel = level; \
                gc->checkPtr = (FxU32)gc->cmdTransportInfo.fifoPtr; \
                GDBG_INFO(gc->myLevel,myName); \
                FXUNUSED(saveLevel); \
                FXUNUSED(hw)

#define GR_TRACE_EXIT(__n) \
                gc->myLevel = saveLevel; \
                GDBG_INFO(281, "%s --done---------------------------------------\n", __n)
#define GR_TRACE_RETURN(__l, __n, __v) \
                gc->myLevel = saveLevel; \
                GDBG_INFO((__l), "%s() => 0x%x---------------------\n", (__n), (__v), (__v))
#else /* !DEBUG_MODE */
#define ASSERT(exp)
#define GR_BEGIN_NOFIFOCHECK(name,level) \
                GR_DCL_GC;      \
                GR_DCL_HW;      \
                FXUNUSED(hw)
#define GR_TRACE_EXIT(__n)
#define GR_TRACE_RETURN(__l, __n, __v) 
#endif /* !DEBUG_MODE */

#define GR_BEGIN(name,level,size, packetNum) \
                GR_BEGIN_NOFIFOCHECK(name,level); \
                GR_SET_EXPECTED_SIZE(size, packetNum)

#define GR_END()        {GR_CHECK_SIZE(); GR_TRACE_EXIT(myName);}

#define GR_RETURN(val) \
                if (GDBG_GET_DEBUGLEVEL(gc->myLevel)) { \
                  GR_CHECK_SIZE(); \
                } \
                else \
                  GR_END(); \
                GR_TRACE_RETURN(gc->myLevel, myName, val); \
                return val

#if defined(GLIDE_SANITY_ASSERT)
#define GR_ASSERT(exp) ((void)((!(exp)) ? (_grAssert(#exp,  __FILE__, __LINE__),0) : 0xFFFFFFFF))
#else
#define GR_ASSERT(exp) ((void)(0 && ((FxU32)(exp))))
//#define GR_ASSERT(exp) ((void)((!(exp)) ? (fprintf(stderr, ">>>>>>>>>>ASSERT: %s %d\n", __FILE__, __LINE__),0) : 0xFFFFFFFF))
//#define GR_ASSERT(exp) ((void)((!(exp)) ? (_grAssert(#exp,  __FILE__, __LINE__),0) : 0xFFFFFFFF))
#endif

#define INTERNAL_CHECK(__name, __cond, __msg, __fatalP) \
    if (__cond) _doGrErrorCallback(__name, __msg, __fatalP)

#if defined(GLIDE_DEBUG)
#define GR_CHECK_F(name,condition,msg) INTERNAL_CHECK(name, condition, msg, FXTRUE)
#define GR_CHECK_W(name,condition,msg) INTERNAL_CHECK(name, condition, msg, FXFALSE)
#else
#define GR_CHECK_F(name,condition,msg)
#define GR_CHECK_W(name,condition,msg)
#endif

#if GLIDE_CHECK_COMPATABILITY
#define GR_CHECK_COMPATABILITY(__name, __cond, __msg) INTERNAL_CHECK(__name, __cond, __msg, FXTRUE)
#else
#define GR_CHECK_COMPATABILITY(__name, __cond, __msg) GR_CHECK_F(__name, __cond, __msg)
#endif /* !GLIDE_CHECK_COMPATABILITY */

/* macro define some basic and common GLIDE debug checks */
#define GR_CHECK_TMU(name,tmu) \
  GR_CHECK_COMPATABILITY(name, tmu < GR_TMU0 || tmu >= gc->num_tmu , "invalid TMU specified")

void
_grAssert(char *, char *, int);

#if ASSERT_FAULT
#define ASSERT_FAULT_IMMED(__x) if (!(__x)) { \
                                 *(FxU32*)NULL = 0; \
                                 _grAssert(#__x, __FILE__, __LINE__); \
                              }
#else
#define ASSERT_FAULT_IMMED(__x) GR_ASSERT(__x)
#endif

/* Offsets to 'virtual' addresses in the hw */
#if (GLIDE_PLATFORM & GLIDE_HW_CVG)
#define HW_REGISTER_OFFSET      SST_3D_OFFSET
#define HW_FIFO_OFFSET          0x00200000UL    
#elif (GLIDE_PLATFORM & GLIDE_HW_H3)
#define HW_IO_REG_REMAP         SST_IO_OFFSET
#define HW_CMD_AGP_OFFSET       SST_CMDAGP_OFFSET
#define HW_2D_REG_OFFSET        SST_2D_OFFSET
#define HW_3D_REG_OFFSET        SST_3D_OFFSET
#define HW_REGISTER_OFFSET      HW_3D_REG_OFFSET
#else
#error "Must define virtual address spaces for this hw"
#endif

#define HW_FIFO_OFFSET          0x00200000UL
#define HW_LFB_OFFSET           SST_LFB_OFFSET
#define HW_TEXTURE_OFFSET       SST_TEX_OFFSET

#if (GLIDE_PLATFORM & GLIDE_HW_CVG) || (GLIDE_PLATFORM & GLIDE_HW_H3)
#define HW_BASE_PTR(__b)        (__b)
#else
#error "Need HW_BASE_PTR to convert hw address into board address."
#endif
   
#define HW_REG_PTR(__b)        ((FxU32*)(((FxU32)(__b)) + HW_REGISTER_OFFSET))
#define HW_LFB_PTR(__b)        ((FxU32*)(((FxU32)(__b)) + HW_LFB_OFFSET))
#define HW_TEX_PTR(__b)        ((FxU32*)(((FxU32)(__b)) + HW_TEXTURE_OFFSET))   

/* access a floating point array with a byte index */
#define FARRAY(p,i)    (*(float *)((i)+(int)(p)))
#define ArraySize(__a) (sizeof(__a) / sizeof((__a)[0]))

void rle_decode_line_asm(FxU16 *tlut,FxU8 *src,FxU16 *dest);

extern FxU16 rle_line[256];
extern FxU16 *rle_line_end;

#define RLE_CODE                        0xE0
#define NOT_RLE_CODE            31

#ifdef  __WATCOMC__
#pragma aux rle_decode_line_asm parm [edx] [edi] [esi] value [edi] modify exact [eax ebx ecx edx esi edi] = \
"  next_pixel:                   "  \
"  xor   ecx,ecx                 "  \
"  mov   al,byte ptr[edi]        "  \
"  mov   cl,byte ptr[edi]        "  \
"  inc   edi                     "  \
"                                "  \
"  and   al,0xE0                 "  \
"  cmp   al,0xE0                 "  \
"  jne   unique                  "  \
"                                "  \
"  and   cl,0x1F                 "  \
"  mov   al,cl                   "  \
"  jz    done_rle                "  \
"                                "  \
"  mov   cl,byte ptr[edi]        "  \
"  inc   edi                     "  \
"  mov   bx,word ptr[edx+ecx*2]  "  \
"                                "  \
"  copy_block:                   "  \
"  mov   word ptr[esi],bx        "  \
"  add   esi,0x2                 "  \
"  dec   al                      "  \
"  jz    next_pixel              "  \
"  jmp   copy_block              "  \
"                                "  \
"  unique:                       "  \
"  mov   bx,word ptr[edx+ecx*2]  "  \
"  mov   word ptr[esi],bx        "  \
"  add   esi,0x2                 "  \
"  jmp   next_pixel              "  \
"  done_rle:                     "; 
#endif /* __WATCOMC__ */

#if GDBG_INFO_ON
/* cvg.c */
extern void
_grErrorCallback(const char* const procName,
                 const char* const format,
                 va_list           args);
#endif

extern FxU32 GR_CDECL
_cpu_detect_asm(void);

extern void GR_CDECL 
single_precision_asm(void);

extern void GR_CDECL 
double_precision_asm(void);


/*
** The lod and aspect ratio changes will be done after we split the tree.
** Currently, we change the definition but patch it back to the original value
** so it is the same glide2.
** To smooth the transition from glide2 defs to glide3 defs, we introduce the 
** translation layer.
*/
#if defined(GLIDE3) && defined(GLIDE3_ALPHA)
#ifndef GLIDE3_DEBUG
/* #define GLIDE3_DEBUG 1 */
#endif
#ifdef GLIDE3_DEBUG
#define TEX_INFO(ptr,field)                         ptr->field
#define G3_LOD_TRANSLATE(lod)                       (lod)
#define G3_ASPECT_TRANSLATE(aspect)                 (aspect)
#else
#define TEX_INFO(ptr,field)                         ptr->field##Log2
#define G3_LOD_TRANSLATE(lod)                       (0x8-lod)
#define G3_ASPECT_TRANSLATE(aspect)                 (0x3-(aspect))
#endif /* GLIDE3_DEBUG */
#else
#define TEX_INFO(ptr,field)                         ptr->field
#define G3_LOD_TRANSLATE(lod)                       (lod)
#define G3_ASPECT_TRANSLATE(aspect)                 (aspect)
#endif

#endif /* __FXGLIDE_H__ */

