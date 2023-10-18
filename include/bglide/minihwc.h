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
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header: /releases/banshee/glop/3dfx/devel/h3/minihwc/minihwc.h 2     9/21/98 4:47p Dow $
** $Log: /releases/banshee/glop/3dfx/devel/h3/minihwc/minihwc.h $
** 
** 2     9/21/98 4:47p Dow
** Stuff for vidmode
** 
** 22    9/17/98 3:58p Dow
** Vidmode Stuff
** 
** 21    8/02/98 5:00p Dow
** Glide Surface Extension
** 
** 20    7/29/98 3:09p Dow
** SDRAM Fixes
** 
** 19    7/24/98 2:02p Dow
** AGP Stuff
** 
** 18    7/23/98 1:18a Dow
** Bump & Grind
** 
** 17    7/18/98 12:21a Jdt
** Added state buffer
** 
** 16    7/16/98 10:26p Dow
** GIW Stuf
** 
** 15    7/16/98 3:05p Dow
** Removed useless colBufferAddr from surface info
** 
** 14    7/15/98 4:09p Dow
** GIW Stuff & DOS Protection
** 
** 13    7/13/98 10:35p Jdt
** Added hwcWinFifo, hwcAllocWinFifo, hwcexecuteWinFifo
** 
** 12    7/02/98 12:11p Dow
** LFB fixes
** 
** 11    6/16/98 6:11p Dow
** Rearranged texture memory
** 
** 10    6/12/98 9:22a Peter
** lfb read for triple buffering
** 
** 9     6/11/98 7:44p Jdt
** Win98/NT5 Multimon 1st pass
** 
** 8     6/10/98 9:49a Peter
** lfb buffer addressing
** 
** 7     5/08/98 10:58a Dow
** Volatile declarations
** 
** 6     4/16/98 10:14p Dow
** EXT_HWC is default init method
** 
** 5     4/07/98 10:40p Dow
** LFB Fixes
** 
** 4     3/28/98 10:51a Dow
** Fixes for FIFO bug
** 
** 3     3/20/98 1:11p Dow
** Now checking revision of chip
** 
** 2     3/11/98 8:27p Dow
** WinGlide
** 
** 1     3/04/98 4:13p Dow
**
*/
#ifndef MINIHWC_H
#define MINIHWC_H

#ifdef HWC_EXT_INIT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <3dfx.h>
#include <h3.h>
#include <fxvid.h>

/* 
**  Constants
*/
#define HWC_MAX_BOARDS          4 /* Max # boards we can handle */
#define HWC_NUM_BASE_ADDR       9 /* Max # base addresses */

#define HWC_TILE_WIDTH          128
#define HWC_TILE_HEIGHT         32


typedef FxU32   hwcBuffer;
#define HWC_BUFFER_FRONTBUFFER 0x0
#define HWC_BUFFER_BACKBUFFER  0x1
#define HWC_BUFFER_AUXBUFFER   0x2
#define HWC_BUFFER_DEPTHBUFFER 0x3
#define HWC_BUFFER_ALPHABUFFER 0x4
#define HWC_BUFFER_TRIPLEBUFFER 0x5
#define HWC_BUFFER_FIFOBUFFER      0x6
#define HWC_BUFFER_SCREENBUFFER    0x7
#define HWC_BUFFER_TEXTUREBUFFER   0x8
#define HWC_BUFFER_AUXRENDER    0x9
#define HWC_BUFFER_NONE            0xff

typedef FxU32   hwcOriginLocation;
#define HWC_ORIGIN_UPPER_LEFT  0x0
#define HWC_ORIGIN_LOWER_LEFT  0x1

typedef FxI32   hwcColorFormat;
#define HWC_COLORFORMAT_ARGB   0x0
#define HWC_COLORFORMAT_ABGR   0x1
#define HWC_COLORFORMAT_RGBA   0x2
#define HWC_COLORFORMAT_BGRA   0x3

typedef FxU32 hwcSwapType;
#define HWC_SWAP_FLIP 0x0
#define HWC_SWAP_BLT  0x1

typedef FxU32   hwcControl;
#define HWC_CONTROL_ACTIVATE   0x1
#define HWC_CONTROL_DEACTIVATE 0x2
#define HWC_CONTROL_RESIZE     0x3
#define HWC_CONTROL_MOVE       0x4

/*
**  Data Structures
*/

typedef struct hwcPCIInfo_s {
  FxBool
    initialized;
  FxU32
    vendorID,                   /* PCI Vendor ID */
    deviceID,                   /* PCI Device ID */
    pciBaseAddr[HWC_NUM_BASE_ADDR];
} hwcPCIInfo;  

typedef struct hwcLinearInfo_s {
  FxBool
    initialized;
  FxU32
    linearAddress[HWC_NUM_BASE_ADDR];
} hwcLinearInfo;

typedef struct hwcRegInfo_s {
  FxBool
    initialized;
  volatile FxU32
    ioMemBase,                  /* mem base for I/O aliases */
    cmdAGPBase,                 /* CMD/AGP register base */
    waxBase,                    /* 2D register base */
    sstBase,                    /* 3D register base */
    lfbBase,                    /* 3D lfb base */
    rawLfbBase,                 /* Raw LFB base (base address 1) */
	biosBase;							// BIOS base

#ifndef __GOOSE__
  volatile FxU16
    ioPortBase,                 /* I/O base address */
    pad;                        /* Keep things aligned */
#else
  volatile FxU32 ioPortBase;
#endif
} hwcRegInfo;

typedef struct hwcFifoInfo_s {
  FxBool
    agpFifo,
    initialized;
  FxU32
    agpVirtAddr,
    agpPhysAddr,
    agpSize,
    fifoStart,                  /* Beg of fifo (offset from base) */
    fifoLength;                 /* Fifo size in bytes */
} hwcFifoInfo;

/*
**  CHD:  Reconcile the following two types
*/

typedef struct hwcBufferDesc_s {
  FxU32        bufMagic;
  hwcBuffer    bufType;
  FxU32        bufOffset;
  FxI32        bufStride;
  FxU32        bufSize;
  FxU32        bufBPP;          /* bits per pixel */
  FxU32        width, height;
  FxBool       tiled;           /* Is it tiled? */
} hwcBufferDesc;

typedef struct hwcBufferInfo_s {
  FxBool
    initialized;
  
  FxU32
    bufSize,                    /* size of buffer in bytes */
    bufSizeInTiles,             /* Buffer Size in tiles */
    bufStride,                  /* stride of buffer in bytes */
    bufStrideInTiles,           /* stride of buffer in tiles */
    bufHeightInTiles,           /* height of buffer in tiles */
    nColBuffers,                /* # color buffers */
    colBuffStart[3],            /* Beg of color buffers */
    colBuffEnd[3],              /* End of color buffers */
    lfbBuffAddr[4],             /* Start address of lfb (tiled relative) buffers.  
                                 * NB: This only includes enough space
                                 * for 3 color buffers and 1 aux buffer.  
                                 */
    nAuxBuffers,                /* # aux buffer (0 or 1) */
    auxBuffStart,               /* End of aux buffer */
    auxBuffEnd;                 /* End of aux buffer */
  void
    *colBuffPriv,               /* Private color buffer info */
    *auxBuffPriv;               /* Private aux buffer info */
  hwcBufferDesc
    buffers[6];                 /* 3 col + 1 aux + 1 tex + 1 fifo */
} hwcBufferInfo;

typedef struct hwcVidInfo_s {
  FxU32
    hWnd;                       /* Window Handle */
  FxBool
    initialized,
    tiled;                      /* Is it tiled or linear? */
  GrScreenResolution_t
    sRes;
  GrScreenRefresh_t
    vRefresh;
  FxU32
    stride,                     /* Stride in bytes */
    xRes,                       /* X resolution */
    yRes,                       /* Y resolution */
    refresh;                    /* refresh rate */
} hwcVidInfo;

typedef struct hwcSurfaceInfo_s {
  FxU32
    lpSurface,
    width,
    height,
    tileBase,
    pciStride,
    lpLFB,
    hwStride,
    strideInBytes;
  FxBool
    isTiled;
} hwcSurfaceInfo;

typedef struct hwcAGPInfo_s {
  FxU32
    linAddr,                    /* Linear address of AGP memory */
    physAddr,                   /* physical address of AGP memory */
    size;                       /* size of physical memory */
} hwcAGPInfo;


/* Here's the wrapper in which we keep all of the above: */
typedef struct hwcBoardInfo_s {
  FxBool
    sdRAM;                      /* Is the board SDRAM? */
  FxU32  
    hdc;                        /* This is really an HDC */
  void *                        /* this is an HMONITOR */
    hMon;
  FxU32
    extContextID,               /* HWC_EXT Context ID (if needed) */
    devRev,                     /* Device Revision */
    tramOffset,                 /* Offset to texture memory */
    tramSize,                   /* Size of texure memory */
    fbOffset,                   /* Frame Buffer Offset (in bytes) */
    h3Rev,                      /* Banshee Revision Number */
    h3Mem,                      /* Megs of RAM on board */
    boardNum,                   /* Board ID for hwc */
    deviceNum;                  /* Device ID for PCILib */
  hwcPCIInfo
    pciInfo;
  hwcLinearInfo
    linearInfo;
  hwcRegInfo
    regInfo;
  hwcFifoInfo
    fifoInfo;
  hwcBufferInfo
    buffInfo;
  hwcVidInfo
    vidInfo;
  void *
    deviceConfigData;
  hwcAGPInfo
    agpInfo;
} hwcBoardInfo;

/* Now, let's tie it all together */
typedef struct hwcInfo_s {
  FxU32
    nBoards;
  hwcBoardInfo
    boardInfo[HWC_MAX_BOARDS];
} hwcInfo;

typedef struct {
    FxU32 cmdBuffer;
    FxU32 cmdBufferSize;
    FxU32 stateBuffer;
    FxU32 stateBufferSize;
} HwcWinFifo;


/*
**  Function Prototypes
*/

hwcInfo *
hwcInit(FxU32 vID, FxU32 dID);

FxBool
hwcMapBoard(hwcBoardInfo *bInfo, FxU32 bAddrMask);

FxBool
hwcInitRegisters(hwcBoardInfo *bInfo);

FxBool
hwcAllocBuffers(hwcBoardInfo *bInfo, FxU32 nColBuffers, FxU32 nAuxBuffers); 

FxBool
hwcInitFifo(hwcBoardInfo *bInfo);

FxBool
hwcInitVideo(hwcBoardInfo *bInfo, FxBool tiled, FxVideoTimingInfo
             *vidTiming, FxBool overlay);

FxBool
hwcRestoreVideo(hwcBoardInfo *bInfo);

char *
hwcGetErrorString(void);

FxBool
hwcCheckMemSize(hwcBoardInfo *bInfo, FxU32 xres, FxU32 yres, FxU32 nColBuffers,
                FxU32 nAuxBuffers, FxBool tiled);

#ifdef __WIN32__
FxBool 
hwcAllocWinFifo( FxU32 hdc, HwcWinFifo *fifo );

FxBool 
hwcExecuteWinFifo( FxU32 hdc, HwcWinFifo *fifo );

void
hwcGetSurfaceInfo(FxU32 *sfc, hwcSurfaceInfo *ret);

FxU32 
hwcInitAGPFifo(hwcBoardInfo *bInfo);

FxBool
hwcAllocAuxRenderingBuffer(hwcBoardInfo *bInfo, hwcBufferDesc *bp, int width,
                           int height);

#endif

#endif                          /* MINIHWC_H not defined */
