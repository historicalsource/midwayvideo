#ifndef __FXHAL_H__
#define __FXHAL_H__

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
** $Revision: 1 $
** $Date: 9/02/98 12:35a $
*/

#if defined(BUILD_HAL)
#define FX_DLL_DEFINITION
#endif
#include <fxdll.h>
#include <fxpci.h>

#include <h3info.h>

// Allow SourceSafe to track Revision values
#define HAL_H_REV "$Revision: 1 $"

// Just to unconfuse myself:
//
//      CHIP            FBI-REV TMU-REV DEV-ID
//      SST1-0.6u       1       0       1
//      SST1-0.5u       2       1       1
//      SST-96          2       (1)     2
//      H3 A0           1       4       3
//      H3 A1           2       4       3
//      H3 B0           3       4       3
//      H3Plus          1       4       4
//      H4              ?       ?       ?
//      SST2                            5
#define SST_DEVICE_ID_SST1      1
#define SST_DEVICE_ID_SST96     2
#define SST_DEVICE_ID_H3        3
#define SST_DEVICE_ID_H3PLUS    4
#define SST_DEVICE_ID_H4    	5

#define MBYTE(n)        (((FxU32)(n))<<20)
#define DEAD            0xDEAD

// Maximum number of boards and TMUs supported
#define HAL_MAX_BOARDS 4

//----------------------------------------------------------------------
// the root of all Hal information
//----------------------------------------------------------------------
typedef struct {
    int csim;           // TRUE if CSIM is enabled
    int hsim;           // TRUE if HSIM is enabled (actually a bitmask)
    int hw;             // TRUE if real HW is enabled (default)
    int csimio;         // TRUE if CSIM should intercept and mirror all HW/HSIM accesses
    FxU32 boardsFound;  // number of boards found
    FxDeviceInfo boardInfo[HAL_MAX_BOARDS];

    int pollLimit;      // number of pixels to poll msg Q after
    int pollCount;      // current pixel counter
    int video;          // video output enabled
    FxU32 csimLastRead; // holds the last data read (from CSIM)
} HalInfo;

//----------------------------------------------------------------------
/*
** SST Hardware Initialization routine protypes
**
** If all initialization routines are called, it is assumed they are called
** in the following order:
**   0. fxHalInit();
**   1. fxHalMapBoard();
**   2. fxHalInitRegisters();
**   3. fxHalInitGamma();
**   4. fxHalInitVideo();
**   5. fxHalShutdown();
**
** fxHalShutdown() is called at the end of an application to turn off
**  the graphics subsystem
**
*/

FX_ENTRY void   FX_CALL fxHalPutenv(char *buf);
FX_ENTRY HalInfo * FX_CALL fxHalInit(FxU32 flags);
FX_ENTRY FxU32  FX_CALL fxHalNumBoardsInSystem(void);
FX_ENTRY SstRegs * FX_CALL fxHalMapBoard(FxU32 boardNum);
FX_ENTRY FxBool FX_CALL fxHalInitCmdFifo( SstRegs *sst, int which, FxU32 fifoStart,
                  FxU32 size, FxBool directExec, FxBool disableHoles, FxBool agpEnable);
FX_ENTRY FxBool FX_CALL fxHalInitRegisters(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalInitRenderingRegisters(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalInitGuiRegisters(SstGRegs *sstg);
FX_ENTRY FxBool FX_CALL fxHalInitCmdAgpRegisters(SstCRegs *sstc);
FX_ENTRY FxBool FX_CALL fxHalInitGamma(SstRegs *sst, FxFloat gamma);
FX_ENTRY FxBool FX_CALL fxHalInitGammaRGB(SstRegs *sst, FxFloat r, FxFloat g, FxFloat b);
FX_ENTRY FxBool FX_CALL fxHalInitVideo(SstRegs *sst, FxU32 resolution,
                FxU32 refresh, FxVideoTimingInfo *);
FX_ENTRY FxBool FX_CALL fxHalIdle(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalIdleNoNop(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalIdle2(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalIdleNoNop2(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalInitUSWC(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalShutdown(SstRegs *sst);
FX_ENTRY void   FX_CALL fxHalShutdownAll(void);
FX_ENTRY FxBool FX_CALL fxHalGetDeviceInfo(SstRegs *sst, FxDeviceInfo *);

FX_ENTRY FxBool FX_CALL fxHalVsync(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalVsyncNot(SstRegs *sst);

FX_ENTRY void FX_CALL
fxHalInitVideoOverlaySurface(
    SstRegs *sst,               // pointer to hw
    FxU32 enable,               // 1=enable Overlay surface (OS), 1=disable
    FxU32 stereo,               // 1=enable OS stereo, 0=disable
    FxU32 horizScaling,         // 1=enable horizontal scaling, 0=disable
    FxU32 dudx,                 // horizontal scale factor (ignored if not
                                // scaling)
    FxU32 verticalScaling,      // 1=enable vertical scaling, 0=disable
    FxU32 dvdy,                 // vertical scale factor (ignored if not
                                // scaling)
    FxU32 filterMode,           // duh
    FxU32 tiled,                // 0=OS linear, 1=tiled
    FxU32 pixFmt,               // pixel format of OS
    FxU32 clutBypass,           // bypass clut for OS?
    FxU32 clutSelect,           // 0=lower 256 CLUT entries, 1=upper 256
    FxU32 startAddress,         // board address of beginning of OS
    FxU32 stride);              // distance between scanlines of the OS, in
                                // units of bytes for linear OS's and
                                // tiles for tiled OS's
// These are the CSIM interface routines
#ifdef HAL_CSIM 
        FX_ENTRY void FX_CALL csimInitDriver( FxU32 numBytes,
                        volatile FxU32 *memory, volatile FxU32 *hw );
        FX_ENTRY void FX_CALL csimInitMemory( FxU32 numBytes, volatile FxU32 *memory );
        FX_ENTRY void FX_CALL csimInitHwAddress( volatile FxU32 *hw );

        FX_ENTRY void  FX_CALL halStore8 (volatile void *, FxU8);
        FX_ENTRY void  FX_CALL halStore16(volatile void *, FxU16);
        FX_ENTRY void  FX_CALL halStore32(volatile void *, FxU32);
        FX_ENTRY void  FX_CALL halStore32f(volatile void *, FxFloat);
        FX_ENTRY FxU8  FX_CALL halLoad8 (volatile void *);
        FX_ENTRY FxU16 FX_CALL halLoad16(volatile void *);
        FX_ENTRY FxU32 FX_CALL halLoad32(volatile void *);

        // GMT: these no longer need to be DLL entry points since we
        // get to them via callbacks
        void  halOutPort8 (FxU16 port, FxU8 data);
        void  halOutPort16(FxU16 port, FxU16 data);
        void  halOutPort32(FxU16 port, FxU32 data);
        FxU8  halInPort8  (FxU16 port);
        FxU16 halInPort16 (FxU16 port);
        FxU32 halInPort32 (FxU16 port);

        // should be put into a driver
        FX_ENTRY void FX_CALL agpMemInit();
        FX_ENTRY FxU32 * FX_CALL agpMemAlloc(SstRegs *sst,unsigned sizeBytes);
        FX_ENTRY void  FX_CALL agpWriteMem32( FxU32 *virtAddr, FxU32 value);
        FX_ENTRY FxU32 FX_CALL agpReadMem32( FxU32 *virtAddr);
        FX_ENTRY void  FX_CALL agpPhysWriteMem32( FxU32 physAddrHi, FxU32 physAddrLo , FxU32 value);
        FX_ENTRY FxU32 FX_CALL agpPhysReadMem32(  FxU32 physAddrHi, FxU32 physAddrLo );
        FX_ENTRY FxU32 * FX_CALL agpPhysToVirt( FxU32 physAddrHi, FxU32 physAddrLo );
        FX_ENTRY void  FX_CALL agpVirtToPhys( FxU32 *virtAddr, FxU32 *physAddrHi, FxU32 *physAddrLo ); 
#endif

#if !defined(MSVC16) && !defined(THUNK32) && !defined(DIRECTX)

// Here are the macro defines for talking to hardware
#ifdef HAL_CSIM                 // CSIM macros
        #define GET8(s)         halLoad8 (&(s))
        #define GET16(s)        halLoad16(&(s))
        #define GET(s)          halLoad32(&(s))

        #define SET8(d,s)       halStore8 (&(d),s)
        #define SET16(d,s)      halStore16(&(d),s)
        #define SET(d,s)        halStore32(&(d),s)
        #define SETF(d,s)       halStore32f(&(d),s)

        #define AGPMEMINIT()       agpMemInit()
        #define AGPMEMALLOC(sst,size)       agpMemAlloc(sst,size)
        #define AGPWRV(v,d)        agpWriteMem32(&(v),d)
        #define AGPRDV(v)          agpReadMem32(&(v))
        #define AGPWRP(aHi,aLo,d)  AGPWRV( *agpPhysToVirt(aHi,aLo), d )
        #define AGPRDP(aHi,aLo)    AGPRDV( *agpPhysToVirt(aHi,aLo) )
#else  // #ifdef HAL_CSIM                          // REAL hw

#ifndef __GOOSE__
        #define GET8(s) s
        #define GET16(s) s
        #define GET(s) s
        #define SET8(d,s) d = s
        #define SET16(d,s) d = s
        #define SET(d,s) d = s
        #define SETF(d,s) (*(float *)&(d)) = s
#else
        #define GET8(s) s
        #define GET16(s) s
        #define GET(s) s
        #define SET8(d, s) (d) = (s)
        #define SET16(d, s) (d) = (s)
        #define SET(d, s) (d) = (s)
        #define SETF(d, s) (*(float *)&(d)) = (s)
#endif

        #define AGPMEMINIT()       GDBG_ERROR("AGPMEMINIT","nyi for hardware\n");
        #define AGPMEMALLOC(sst,size)     GDBG_ERROR("AGPMEMALLOC","nyi for hardware\n");
        #define AGPWRV(v,d)        (v) = (d)
        #define AGPRDV(v)          (v)
        #define AGPWRP(aHi,aLo,d)  AGPWRV( *agpPhysToVirt(aHi,aLo), d )
        #define AGPRDP(aHi,aLo)    AGPRDP( *agpPhysToVirt(aHi,aLo) )
#endif // #ifdef HAL_CSIM

#define GET_IO8(p)      pioInByte(p)
#define GET_IO16(p)     pioInWord(p)
#define GET_IO(p)       pioInLong(p)
#define SET_IO8(p,d)    pioOutByte(p,d)
#define SET_IO16(p,d)   pioOutWord(p,d)
#define SET_IO(p,d)     pioOutLong(p,d)

#endif // #if !defined(MSVC16) && !defined(THUNK32) && !defined(DIRECTX)

//---------------------------------------------------------------------------
// internal HAL stuff not meant for external use
//---------------------------------------------------------------------------
#if defined(BUILD_HAL) || defined(BUILD_DIAGS)

// GMT: Init code SET/GET always go thru subroutines (allows for P6 fencing)
#define IGET(A)   fxHalRead32((FxU32 *) &(A))
#define ISET(A,D) fxHalWrite32((FxU32 *) &(A), D)



// this is the FAKE address where the hardware lives
// we use a large address so attempts to write to it get an access violation
// and it has 28 zero bits so that we can easily figure out the board number
// and the offset into the board
#define SST_BAD_ADDRESS(a) (((FxU32)a&0xF0000000)==0 || ((FxU32)a&0x0C000000)!=0)
#define SST_FAKE_ADDRESS_MAKE(boardNum) (SstRegs *)(0x200000|((boardNum+1)<<28))
#define SST_FAKE_ADDRESS_GET_BOARD(a) ((((FxU32)a>>28)&0xF)-1)
#define SST_FAKE_ADDRESS_GET_CSIM(a) \
                halInfo.boardInfo[SST_FAKE_ADDRESS_GET_BOARD(a)].sstCSIM
#define SST_FAKE_ADDRESS_GET_OFFSET(a) ((FxU32)a&0x0FFFFFFF)
#define SST_FAKE_ADDRESS_GET_BASE_OFFSET(a) ((FxU32)a&0x01FFFFFF)
#define SST_FAKE_ADDRESS_GET_BASE(a) ( ((FxU32)a&BIT(25))>>25 )

#define SST_BAD_PORT(a) (((FxU32)a&0xF000)==0)
#define SST_FAKE_PORT_MAKE(boardNum) (FxU16)((boardNum+1)<<12)
#define SST_FAKE_PORT_GET_BOARD(a) ((((FxU16)a>>12)&0xF)-1)
#define SST_FAKE_PORT_GET_CSIM(a) \
                halInfo.boardInfo[SST_FAKE_PORT_GET_BOARD(a)].sstCSIM
#define SST_FAKE_PORT_GET_OFFSET(a) ((FxU16)a&0x0FFF)

// HSIM flags, passed in from the diag environment
#define HSIM_HW_SIMULATION              BIT(0)
#define HSIM_TREX_STANDALONE            BIT(1)
#define HSIM_TREX_BACKDOOR_TEXWRITES    BIT(2)
#define HSIM_SWIZZLE                    BIT(3)

extern HalInfo halInfo;

// internal HAL routines
FxU32 fxHalRead32(FxU32 *addr);
void fxHalWrite32(FxU32 *addr, FxU32 data);

void   fxHalResetBoardInfo( FxDeviceInfo *info );
FxBool fxHalFillDeviceInfo( SstRegs *sst );
// FxBool fxHalGetFbiInfo( SstRegs *sst, FxDeviceInfo *info );
// FxBool fxHalGetTmuInfo( SstRegs *sst, FxDeviceInfo *info );
FxBool fxHalVaddrToBoardNumber( SstRegs *sst, FxU32 *boardNumber );

// GUI interface
FX_ENTRY void FX_CALL guiNewViewWindow(FxU32 boardNumber, const char *name);
void guiReadMessageQueue(void);
FxBool guiOpen( FxU32 boardNumber );
void guiShutdown( SstRegs *sst );

#endif /* BUILD_HAL */

#endif /* !__FXHAL_H__ */
