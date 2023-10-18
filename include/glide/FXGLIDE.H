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
**
** $Header: /video/include/glide/FXGLIDE.H 1     5/04/97 3:44p Mlynch $
** $Log: /video/include/glide/FXGLIDE.H $
 * 
 * 1     5/04/97 3:44p Mlynch
 * Glide library include files
 * 
 * 2     10/21/96 5:35p Mlynch
 * Replaced with this version for Glide version 2.03h3
 * 
 * 85    9/06/96 1:40p Dow
 * Changed packer hack to new convention
 *                                 Removed chip field from FBI writes
 * Removed P6 fence stuff (it moved to sst1init.h)
 * Changed the way trisetup is referenced
 * Added ref to regStruct
 * Mods for FIFO level checking
 * 
 * 84    9/05/96 1:23a Dow
 * Packer Bug Workaround update
 * 
 * 82    8/27/96 8:56a Dow
 * Changed _SST global ref to local gc, changed GrSST type to GrGC
 * 
 * 81    8/15/96 10:47p Dow
 * Removed glide state associated with HWM
 * 
 * 80    8/15/96 10:11p Dow
 * Changed src arg to trisetup to unsigned long *
 * 
 * 79    8/14/96 10:15a Dow
 * Removed ref to file that isn't in tree yet
 * 
 * 78    8/13/96 5:28p Dow
 * Some debugging stuff (the start of it) and support for FIFO checking
 * 
 * 77    7/20/96 11:28a Dow
 * Changes for multihead; got rid of prototypes for nonexistent functions.
 * 
 * 76    7/19/96 12:15p Sellers
 * fixed problems with initializing video for multiple SSTs in a system
 * moved globals which keep state of current video mode into
 * hw configuration data structure
 * 
 * 75    6/19/96 3:05p Sellers
 * fixed problems with splash when users repeatedly call glideInit() and
 * glideShutdown()
 * now grQueryHardware() properly returns FXFALSE and doesn't die if 3Dfx
 * hardware is not detected
 * 
 * 74    6/18/96 1:27p Dow
 * got rid of #if 0'd code
 * 
 * 73    6/14/96 5:17p Sellers
 * fixed for splash screen and SLI
 * fixed grBufferSwap() to be independent of .5 or .6 micron FBI
 * 
 * 72    6/12/96 4:15p Dow
 * state.clipLowYHighY -> clipBottomTop
 * got rid of state.clockwizearea, as it wasn't being used
 * 
 * 71    6/10/96 12:07p Dow
 * Changed declaration of trisetup routines to get rid of MSVC warnings.
 * 
 * 70    6/10/96 12:27a Jdt
 * Added new state to GrGlideTmuConfig for low level texture memory
 * manager
 * Exported aspect_bit_table to glide library objects.
 * Added _grTexDownloadPalette
 * Fixed prototype for _grTexCalcNextAddress
 * 
 * 69    6/08/96 4:15p Dow
 *      Added arg to trisetup code, 
 *      Changed name of trisetup init routine, 
 *      changed ordering of GrVertexReg struct, 
 *      cleaned out stale, commented code
 *      Added hints, removed wireframe from _GrState_s
 *      Changed around STATE_REQUIRES_* macros
 *      #ifdefed some 3tmu-related code
 *      Added proto for  state validation routine
 *      Scott's SLI changes
 * 
 * 68    5/30/96 11:04a Dow
 * removed packer hack around state setting (it's slow, likely unecessary,
 * and problematic)
 * 
 * 67    5/29/96 9:59p Dow
 * Removed #if 0'd code; added packerhack stuff; put packer hack around
 * state setting routines
 * 
 * 66    5/28/96 4:47p Paik
 * Move GrErrorCallback to private header file
 * 
 * 65    5/22/96 1:35a Dow
 * added some indeces for trisetup, made trisetup, trisetup_asm return
 * FxBool
 * 
 * 64    5/14/96 8:12a Dow
 * Added Hints to glide state.
 * 
 * 63    5/09/96 7:04a Paik
 * Glide DLL support
 * 
 * 62    5/08/96 3:07p Paik
 * Clean up of build warnings
 * 
 * 61    3/25/96 7:58p Paik
 * Move fxglide.h out of glide.h
 * 
 * 60    3/25/96 2:46p Paik
 * Add prototype for new state routines.
 * 
 * 59    3/20/96 8:07a Paik
 * Use single macro for TMUs
 * 
 * 58    3/18/96 3:58p Paik
 * Fix ups for djgpp
 * 
 * 57    3/01/96 2:06p Paik
 * State optimization
 * 
 * 56    3/01/96 9:30a Paik
 * Remove state extra state stuff.
 * 
 * 55    3/01/96 2:37a Paik
 * Reads from TMU 1 went to the wrong shadow.
 * 
 * 54    2/29/96 4:40p Paik
 * Missed one
 * 
 * 53    2/29/96 4:21p Paik
 * Prepare to slim down the state
 * 
 * 52    2/28/96 4:41p Dow
 * Minor Adjustments
 * 
 * 51    2/20/96 2:32p Paik
 * DJGPP fix
 * 
 * 50    2/09/96 4:41p Dow
 * Glide state stuff
 * 
 * 49    2/06/96 9:13p Garymct
 * grSstGetConfigFbi was not defined when compiling for the simulator, so
 * I added it.
 * 
 * 48    2/06/96 6:33a Dow
 * Changed drawing primitives
 * 
 * 47    2/05/96 3:08p Dow
 * Cleaned up grSstGetConfig*
 * 
 * 46    2/05/96 11:36a Dow
 * Updated grGetConfig*
 * 
 * 45    2/02/96 11:34p Paik
 * New fields for texture combine
 * 
 * 46    1/31/96 6:03p Paik
 * New fields for texture combine
 * 
 * 45    1/31/96 4:09p Paik
 * 
 * 44    1/22/96 10:39a Dow
 * Added macros that actually read the sst registers, changed declaration
 * of assembly language trisetup stuff
 * 
 * 43    1/19/96 5:12p Paik
 * Hardware bringup modifications
 * 
 * 42    1/15/96 6:29p Dow
 * Hardware Bringup mods
 * 
 * 41    1/14/96 5:31p Dow
 * 
 * 40    1/14/96 1:53p Dow
 * GLDIE_STATE_PAD -> GLIDE_STATE_PAD_SIZE
 * 
 * 39    1/14/96 12:29p Dow
 * AAMode stuff & GLIDE_STATE_PAD_SIZE fix
 * 
 * 37    1/13/96 2:43p Dow
 * Random fixes, and emacs helping stuff
 * 
 * 36    1/12/96 2:28p Paik
 * [sw-glide/225, sw-glide/242]
 * Executive summary
 * 1. created grAlphaCombine and grColorCombine which fully expose acu and
 * ccu.
 * 2. renamed grColorCombineFunction and grAlphaSource to
 * guColorCombineFunction and guAlphaSource, reimplement on top of
 * grColorCombine and grAlphaCombine.
 * 
 * 33    1/12/96 12:55p Dow
 * Hardware bringup shit
 * 
 * 31    1/11/96 9:37p Dow
 * Fixed bogus tmu type stuff
 * Moved GrChipID_t in from fxglide.h
 * 
 * 30    1/11/96 2:26a Dow
 * Added GrTMU_t, changed functions taking tmus to use it.
 * Added tmuMask to state, use it to better control state setting when
 * moving between texture mapped and non texture mapped color combine
 * modes.
 * 
 * 29    1/10/96 11:12p Dow
 * more 1 & 3 tmu, virtual tmu removal fixes
 * 
 * 28    1/10/96 9:43p Dow
 * fixed proto of grSstConfigPipeline
 * 
 * 27    1/09/96 9:39a Dow
 * tabs->spaces && added Rev. Control keywords
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
** GLIDE_SIMULATOR:  Defined if GLIDE should use the software simulator.  An
**                   application is responsible for defining this macro.
**                   This macro is mutually exclusive with GLIDE_HARDWARE.
**
** GLIDE_HARDWARE:   Defined if GLIDE should use the actual SST hardware.  An
**                   application is responsible for defining this macro.
**                   This macro is mutually exclusive with GLIDE_SIMULATOR.
**
** GLIDE_EMULATOR:   Defined if GLIDE should emulate the hardware on an
**                   SGI workstation.  This macro is mutually exclusive
**                   with GLIDE_SIMULATOR and GLIDE_HARDWARE.  This macro
**                   should only be defined on IRIX systems.
**
** GLIDE_NUM_TMU:    Number of physical TMUs installed.  Valid values are 1
**                   and 2.  If this macro is not defined by the application
**                   it is automatically set to the value 2.
**
** GLIDE_NUM_TMU:    Number of physical TMUs installed.  Valid values are 1
**                   and 2.  If this macro is not defined by the application
**                   it is automatically set to the value 2.
*/

/*
** -----------------------------------------------------------------------
** INCLUDE FILES
** -----------------------------------------------------------------------
*/

#ifndef __FXGLIDE_H__
#define __FXGLIDE_H__

#include <glide/gsstdef.h>

#if (GLIDE_PLATFORM & GLIDE_SST_HW)
#include <glide/sst1init.h>
#endif

/* 
** The following macros provide support for debugging messages
*/
#ifndef FX_BIT
#define FX_BIT(a)       (1 << a)
#endif

#define GLIDE_DEBUG_MASK_DRAW           FXBIT(0)
#define GLIDE_DEBUG_MASK_FBI_STATE      FXBIT(1)
#define GLIDE_DEBUG_MASK_TMU_STATE      FXBIT(2)
#define GLIDE_DEBUG_MASK_TEX_STATE      FXBIT(3)

#define GLIDE_DEBUG_LEVEL_NONE          0 /* No Debugging output */
#define GLIDE_DEBUG_LEVEL_CALLS         1 /* Call level output */
#define GLIDE_DEBUG_LEVEL_ARGS          2 /* Calls + args */
#define GLIDE_DEBUG_LEVEL_RET           3 /* Calls + args + returns */
#define GLIDE_DEBUG_LEVEL_VERBOSE       4 /* Prints out all data
                                             (including values written
                                             to TRAM and data given to
                                             guFb{Read,Write}Region
                                           */

/*
** The following functions are for access to hardware registers.
** An application using Glide should never have to call any of
** these instructions.
*/
#if ( ( GLIDE_PLATFORM & GLIDE_SST ) && !(GLIDE_PLATFORM & GLIDE_SST_HW) && !defined(GLIDE_BENCHMARK)   )
#  ifdef GLIDE_LIB
   void   grSstWriteFixedAll( GrSstRegister r, FxI32 value );
   void   grSstWriteFloatAll( GrSstRegister r, float value );
   void   grSstWriteConfigAll( GrSstRegister r, FxU32 value );
   void   grSstWriteAbsolute16( FxU32 address, FxU16 value );
   void   grSstWriteAbsolute32( FxU32 address, FxU32 value );

   void   grSstWriteFixedFbi( GrSstRegister r, FxI32 value );
   void   grSstWriteFloatFbi( GrSstRegister r, float value );
   void   grSstWriteConfigFbi( GrSstRegister r, FxU32 value );
   FxU32  grSstGetConfigFbi( GrSstRegister r );

   void   grSstWriteFixedTMU( GrChipID_t tmu, GrSstRegister r, FxI32 value );
   void   grSstWriteFloatTMU( GrChipID_t tmu, GrSstRegister r, float value );
   void   grSstWriteConfigTMU( GrChipID_t tmu, GrSstRegister r, FxU32 value );

#    if ( GLIDE_PLATFORM & GLIDE_SST_SIM )
extern void   sstStore16( void *, FxU16 value );
extern void   sstStore32( void *, FxU32 value );
extern void   sstStore32f( void *, float value );
extern FxU32  sstLoad32(void *reg);

#    endif
#  endif
#else /* We're benchmarking or using the hardware */

extern FxU32 
#ifdef __WATCOMC__
__cdecl
#endif
  packerHackAddress;
extern FxU32 
#ifdef __WATCOMC__
__cdecl
#endif
lastAddress;
extern FxU32 
#ifdef __WATCOMC__
__cdecl
#endif
packerHacksAfterParams;

/*
**  Here's the infamous Packer Hack:   
**  XOR the two addresses together to find out which bits are different.
**  AND the result with the bits that represent the chip field of the 
**  SST address.  If ANY of them are different, then do the packer hack.
**  Save this address as the last with which we compared.
*/  
#define PACKERWORKAROUND *((FxU32 *) packerHackAddress) = packerHackAddress

extern FxU32
#ifdef __WATCOMC__
__cdecl
#endif
  CPUType;
#define PACKERHACK(a)\
  if ( (((FxU32) a) ^ ((FxU32) lastAddress)) & SST_CHIP_SELECT_MASK ) {\
    if (CPUType == 6) {\
      P6FENCE;\
      PACKERWORKAROUND;\
      P6FENCE;\
    } else\
      PACKERWORKAROUND;\
  }\
  lastAddress = (FxU32)(a)

#define CALCALLADDRESS(reg) (gc->base_ptr + (reg))
#define CALCCHIPADDRESS(chp,reg) (gc->base_ptr + SST_CHIP_ADDR(chip) + (reg))
#define CALCTMUADDRESS(tmu,reg) (gc->base_ptr + SST_TMU_ADDR(tmu) + (reg))
#define CALCFBIADDRESS(reg) (gc->base_ptr + SST_FBI_ADDR + (reg))

#  define grSstWriteAbsolute16(a,b)  \
  PACKERHACK((FxU32)(a));\
  *((nFxU16 *)(a)) = (b)

#  define grSstWriteAbsolute32(a,b)  \
  PACKERHACK((FxU32)(a));\
  *((FxU32 *)(a)) = (b)

#  define grSstGetAddressAbsolute(a) (a);

#  define grSstWriteFixedAll(a,b) \
  *((FxI32 *)CALCALLADDRESS(a))= (b)

#  define grSstWriteFloatAll(a,b) \
  *((float *)CALCALLADDRESS(a)) = (b)

#  define grSstWriteConfigAll(a,b)\
  *((FxU32 *)CALCALLADDRESS(a)) = (b)

#  define grSstGetAddressAll(a)      CALCALLADDRESS(a)

#  define grSstWriteFixedChip(chip,reg,value)\
  PACKERHACK((FxU32)(CALCCHIPADDRESS(chip,reg)));\
  *((FxI32 *)CALCCHIPADDRESS(chip,reg)) = (value)

#  define grSstWriteFloatChip(chip,reg,value)\
  PACKERHACK((FxU32)(CALCCHIPADDRESS(chip,reg)));\
  *((float *)CALCCHIPADDRESS(chip,reg)) = (value)

#  define grSstWriteConfigChip(chip,reg,value)\
  PACKERHACK((FxU32)(CALCCHIPADDRESS(chip,reg)));\
  *((FxU32 *)CALCCHIPADDRESS(chip,reg)) = (value)

#  define grSstGetAddressChip(chip,reg)\
  CALCCHIPADDRESS(chip,reg)

#  define grSstWriteFixedFbi(a, b)\
  *((FxI32 *) CALCALLADDRESS(a)) = (b)

#  define grSstWriteFloatFbi(a, b)\
  *((float *)CALCALLADDRESS(a)) = (b)

#  define grSstWriteConfigFbi(a, b)\
  *((FxU32 *)CALCALLADDRESS(a)) = (b)

#  define grSstGetAddressFbi(a)  CALCFBIADDRESS(a)

#  define grSstGetConfigFbi(a)   *((FxU32 *)gc->base_ptr + (a))


#  define grSstWriteFixedTMU(tmu,reg,value)\
  PACKERWORKAROUND;\
  *((FxI32 *)CALCTMUADDRESS(tmu,reg)) = (value);\
  PACKERWORKAROUND


#  define grSstWriteFloatTMU(tmu,reg,value)\
  PACKERWORKAROUND;\
  *((float *)CALCTMUADDRESS(tmu,reg)) = (value);\
  PACKERWORKAROUND


#  define grSstWriteConfigTMU(tmu,reg,value)\
  PACKERWORKAROUND;\
  *((FxU32 *)CALCTMUADDRESS(tmu,reg)) = (value);\
  PACKERWORKAROUND

#  define grSstGetAddressTMU(tmu,reg) CALCTMUADDRESS(tmu,reg)

#ifndef GLIDE_DONT_OPTIMIZE_TRISETUP
void _grInitTriSetupCode(void);

FxBool 
#if !defined(__DJGPP__)
__cdecl
#endif
#ifndef GLIDE_USE_C_TRISETUP
_trisetup_asm(
              const float *va, const float *vb, const float *vc,
              FxU32 *src,  GrCullMode_t cullMode,
              FxU32 paramHints
              );
#define TRISETUP _trisetup_asm
#else
_trisetup(
          const float *va, const float *vb, const float *vc,
          FxU32 *src,  GrCullMode_t cullMode, FxU32 paramHints
         );
#define TRISETUP _trisetup
#endif /* GLIDE_USE_C_TRISETUP */
#endif /* GLIDE_DONT_OPTIMIZE_TRISETUP */
#endif /* ( ( GLIDE_PLATFORM & GLIDE_SST ) && !(GLIDE_PLATFORM & GLIDE_SST_HW) &&   !defined(GLIDE_BENCHMARK)   ) */

/*
**  These are where registers are stored to be sent to the trisetup routine
**  If you change this, be sure to change the C & assembly language triseup
**  routins.
*/
typedef struct {
  float *sow;                   /* address of sow register */
  float *tow;                   /* address of tow register */  
  float *oow;                   /* address of oow register */
}  GrTmuVertexReg;

typedef struct {
  float *x, *y, *z;             /* address of x, y, and z (NOT!) registers */
  float *r, *g, *b;             /* addrss of r, g, b, and a registers */
  float *ooz;                   /* address of ooz register */
  float *a;                     /* adddress of a register */
  float *oow;                   /* address of oow register */
  GrTmuVertexReg  tmuvtx[GLIDE_NUM_TMU]; /* see above */
} GrVertexReg;

extern GrVertexReg
#ifdef __WATCOMC__
__cdecl
#endif
  regStruct;

typedef struct
{
  FxU32             freemem_base;
  FxU32             total_mem;
  FxU32             next_ncc_table;
  GrMipMapId_t      ncc_mmids[2];
  const GuNccTable *ncc_table[2];
} GrTMU;


/*
** GrState
**
** If something changes in here, then go into glide.h, and look for a 
** declaration of the following form:
**
** #define GLIDE_STATE_PAD_SIZE N
** #ifndef GLIDE_LIB
** typedef struct {
**   char pad[GLIDE_STATE_PAD_SIZE];
** }  GrState;
** #endif
** 
** Then change N to sizeof(GrState) AS DECLARED IN THIS FILE!
**
*/

typedef struct
{
    FxU32   fbzColorPath;
    FxU32   fogMode;
    FxU32   alphaMode;
    FxU32   fbzMode;
    FxU32   lfbMode;
    FxU32   clipLeftRight;
    FxU32   clipBottomTop;
    
    FxU32   fogColor;
    FxU32   zaColor;
    FxU32   chromaKey;
    
    FxU32   stipple;
    FxU32   color0;
    FxU32   color1;

} GrGlideFbiConfig;

typedef struct
{
    FxU32   textureMode;
    FxU32   tLOD;
    FxU32   tDetail;
    FxU32   texBaseAddr;
    FxU32   texBaseAddr_1;
    FxU32   texBaseAddr_2;
    FxU32   texBaseAddr_3_8;
    GrMipMapMode_t mmMode;              /* saved to allow MM en/dis */
    GrLOD_t        smallLod, largeLod;  /* saved to allow MM en/dis */
    FxU32          evenOdd;
    GrNCCTable_t   nccTable;
} GrGlideTmuConfig;


struct _GrState_s {
   GrGlideFbiConfig
     fbi_config;                /* fbi register shadow */
   GrGlideTmuConfig
     tmu_config[GLIDE_NUM_TMU]; /* tmu register shadow */

   float
     clipwindowf_xmin, clipwindowf_ymin, /* Clipping info */
     clipwindowf_xmax, clipwindowf_ymax;

   FxU16
     lfb_constant_depth;        /* Constant value for depth buffer (LFBs) */
   GrAlpha_t
     lfb_constant_alpha;        /* Constant value for alpha buffer (LFBs) */
   
   FxBool                       /* Values needed to determine which */
     ac_requires_it_alpha,      /*   parameters need gradients computed */
     ac_requires_texture,       /*   when drawing triangles */
     cc_requires_it_rgb,
     cc_requires_texture,
     db_requires_w;

   FxBool
     cc_delta0mode;             /* Use constants for flat shading */

   FxU32
     paramIndex,                /* Index into array containing
                                   parameter indeces to be sent ot the
                                   triangle setup code */
     tmuMask;                   /* Tells the paramIndex updater which
                                   TMUs need values */

   GrColorFormat_t
     color_format;              /* ARGB, RGBA, etc. */
   int
     num_buffers;               /* 2 or 3 */

   GrCullMode_t
     cull_mode;                 /* cull neg, cull pos, don't cull */

   GrMipMapId_t
     current_mm[GLIDE_NUM_TMU]; /* Which guTex** thing is the TMU set
                                   up for? THIS NEEDS TO GO!!! */
   float
     screen_width, screen_height; /* Screen widht and height */

   float
     a, r, g, b;                /* Constant color values for Delta0 mode */

   GrHint_t
     paramHints;                /* Tells us if we need to pointcast a
                                   parameter to a specific chip */
   FxBool
     checkFifo;                 /* Check fifo status as specified by hints */
   
   FxI32
     swFifoLWM;                 /* Point where we need to start
                                   polling the fifo before rendering
                                   any more triangles  */
   FxU32
     fifoFree;                  /* # free entries in FIFO */
};

typedef struct {
  int
    grSstRez,                   /* Video Resolution of board */
    grSstRefresh,               /* Video Refresh of board */
    fbuf_size,                  /* in MB */
    num_tmu;                    /* number of TMUs attached */

  FxBool
    scanline_interleaved;

  FxU32
    *base_ptr,                  /* base address of SST */
    *tex_ptr,                   /* texture memory address */
    *lfb_ptr,                   /* linear frame buffer address */
    *slave_ptr;                 /* Scanline Interleave Slave address */

  GrMipMapTable
    mm_table;                   /* mip map table */

  GrState
    state;                      /* state of Glide/SST */

  GrTMU
    tmu_state[GLIDE_NUM_TMU];

} GrGC;

#define GETGC GrGC *gc = &_GCs[_GLIDE_STATE.current_sst]


#define STATE_REQUIRES_IT_DRGB  FXBIT(0)
#define STATE_REQUIRES_IT_ALPHA FXBIT(1)
#define STATE_REQUIRES_OOZ      FXBIT(2)
#define STATE_REQUIRES_OOW_FBI  FXBIT(3)
#define STATE_REQUIRES_W_TMU0   FXBIT(4)
#define STATE_REQUIRES_ST_TMU0  FXBIT(5)
#define STATE_REQUIRES_W_TMU1   FXBIT(6)
#define STATE_REQUIRES_ST_TMU1  FXBIT(7)
#define STATE_REQUIRES_W_TMU2   FXBIT(8)
#define STATE_REQUIRES_ST_TMU2  FXBIT(9)

#if (GLIDE_NUM_TMU > 2)
#define GLIDE_PARMINDEX_BITS    10
#else
#define GLIDE_PARMINDEX_BITS    8
#endif

#define GR_TMUMASK_TMU0 (1<< SST_TMU_CHIP_TO_INDEX(GR_TMU0))
#define GR_TMUMASK_TMU1 (1<< SST_TMU_CHIP_TO_INDEX(GR_TMU1))
#define GR_TMUMASK_TMU2 (1<< SST_TMU_CHIP_TO_INDEX(GR_TMU2))

#if (GLIDE_NUM_TMU > 2)
#define GR_MAX_STW_HINTS    (GR_STWHINT_ST_DIFF_TMU2 << 1)
#else
#define GR_MAX_STW_HINTS    (GR_STWHINT_ST_DIFF_TMU1 << 1)
#endif

/*
**  Parameter gradient offsets
**
**  These are the offsets of the DPDX and DPDY registers from
**  from the P register
*/
#ifdef GLIDE_USE_ALT_REGMAP
#define DPDX_OFFSET 0x4
#define DPDY_OFFSET 0x8
#else
#define DPDX_OFFSET 0x20
#define DPDY_OFFSET 0x40
#endif

typedef struct
{
  int current_sst;
} GrGlideState;

/*
**  References to global data
*/
extern GrHwConfiguration        _HW_CONFIGURATION;
extern GrGlideState             _GLIDE_STATE;
extern GrGC                     _GCs[MAX_NUM_SST];
extern FxU32                    aspect_bit_table[];

/* 
**  Function Prototypes
*/
extern void
(*GrErrorCallback)(const char *string, FxBool fatal );

void
_grUpdateParamIndex(void);

void
grColorCombineDelta0Mode( FxBool delta0mode );
void
_grTexDetailControl( GrChipID_t tmu, FxU32 detail );
void
_grTexDownloadNccTable(
                       GrChipID_t tmu, FxU32 which,
                       const GuNccTable *ncc_table );
void _grTexDownloadPalette( GrChipID_t   tmu, 
                            GuTexPalette *pal );


void
_grTexLodBiasValue( GrChipID_t tmu, FxU32 lod_bias );
FxU32
_grTexCalcBaseAddress(
                      FxU32 start_address, GrLOD_t lodmax,
                      GrAspectRatio_t aspect, GrTextureFormat_t fmt,
                      FxU32 odd_even_mask ); 
FxU32
_grTexCalcNextAddress(
                       FxU32 base_address, GrLOD_t lodmin,
                       GrAspectRatio_t aspect, GrTextureFormat_t fmt,
                       FxU32 odd_even_mask ); 
void
_grTexForceLod( GrChipID_t tmu, int value );
int
_guHeapCheck( void );

void
_grColorCombineDelta0Mode(FxBool delta0Mode);

void
_grSetupRegStructByBoardAndHints();

#if ( GLIDE_PLATFORM & GLIDE_SST_SIM )
/* PCI stuff */
FxU32 *_grPciFindSST( void );
#endif

#if (GLIDE_PLATFORM & GLIDE_SST_HW)
void            _grSstDetermineResources( GrGC *gc );
#endif

void
_grDrawTriangleNormal(const GrVertex *a, const GrVertex *b, const GrVertex *c);

FX_ENTRY void
FX_CALL grSstConfigPipeline(GrChipID_t chip, GrSstRegister reg, FxU32 value);


#endif
