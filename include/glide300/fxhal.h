/*-*-c++-*-*/
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
** $Revision: 13 $
** $Date: 3/10/98 2:33p $
*/

#if defined(BUILD_HAL)
#define FX_DLL_DEFINITION
#endif
#include <fxdll.h>

#include <cvginfo.h>

// Allow SourceSafe to track Revision values
#define HAL_H_REV "$Revision: 13 $"

// Just to unconfuse myself:
//
//      CHIP            FBI-REV TMU-REV DEV-ID
//      SST1-0.6u       1       0       1
//      SST1-0.5u       2       1       1
//      SST-96          2       (1)     2
//      H3              3       4       4
//	H4		?	?	4?
//	SST2				5
#define SST_DEVICE_ID_SST1      1
#define SST_DEVICE_ID_SST96     2
#define SST_DEVICE_ID_H3        3
#define SST_DEVICE_ID_SST2      5

#define MBYTE(n)	(((FxU32)(n))<<20)
#define DEAD		0xDEAD

// Maximum number of boards and TMUs supported
#define HAL_MAX_BOARDS 4

//----------------------------------------------------------------------
// the root of all Hal information
//----------------------------------------------------------------------
typedef struct {
    int csim;		// TRUE if CSIM is enabled
    int hsim;		// TRUE if HSIM is enabled (actually a bitmask)
    int hw;		// TRUE if real HW is enabled (default)
    FxU32 boardsFound;	// number of boards found
    FxDeviceInfo boardInfo[HAL_MAX_BOARDS];

    int pollLimit;	// number of pixels to poll msg Q after
    int pollCount;	// current pixel counter
    int video;		// video output enabled
    FxU32 csimLastRead;	// holds the last data read (from CSIM)
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
FX_ENTRY FxBool FX_CALL fxHalInitCmdFifo( SstRegs *sst, int which, 
                                          FxU32 fifoStart, FxU32 size, 
                                          FxBool directExec, FxBool disableHoles,
                                          FxSet32Proc set32);
FX_ENTRY FxBool FX_CALL fxHalInitRegisters(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalInitRenderingRegisters(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalInitGamma(SstRegs *sst, FxFloat gamma);
FX_ENTRY FxBool FX_CALL fxHalInitGammaRGB(SstRegs *sst, FxFloat r, FxFloat g, FxFloat b);
FX_ENTRY FxBool FX_CALL fxHalInitVideo(SstRegs *sst, FxU32 resolution,
					FxU32 refresh, sst1VideoTimingStruct *);
FX_ENTRY FxBool FX_CALL fxHalIdle(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalIdleNoNOP(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalIdleFBINoNOP( SstRegs *sst );
FX_ENTRY FxBool FX_CALL fxHalShutdown(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalInitUSWC(SstRegs *sst);
FX_ENTRY void   FX_CALL fxHalShutdownAll(void);
FX_ENTRY FxBool FX_CALL fxHalGetDeviceInfo(SstRegs *sst, FxDeviceInfo *);
FX_ENTRY FxBool FX_CALL fxHalInitSLI(SstRegs *sst, SstRegs *sst1);

FX_ENTRY FxBool FX_CALL fxHalVsync(SstRegs *sst);
FX_ENTRY FxBool FX_CALL fxHalVsyncNot(SstRegs *sst);


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

#endif

// Here are the macro defines for talking to hardware
#ifdef HAL_CSIM			// CSIM macros
	#define GET8(s)		halLoad8 (&(s))
	#define GET16(s)	halLoad16(&(s))
	#define GET(s)		halLoad32(&(s))

	#define SET8(d,s)	halStore8 (&(d),s)
	#define SET16(d,s)	halStore16(&(d),s)
	#define SET(d,s)	halStore32(&(d),s)
	#define SETF(d,s)	halStore32f(&(d),s)
#else				// REAL hw
	#define GET8(s) s
	#define GET16(s) s
	#define GET(s) s
	#define SET8(d,s) d = s
	#define SET16(d,s) d = s
	#define SET(d,s) d = s
	#define SETF(d,s) (*(float *)&(d)) = s
#endif

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
#define SST_FAKE_ADDRESS_MAKE(i) (SstRegs *)((i+1)<<28)
#define SST_FAKE_ADDRESS_GET_BOARD(a) ((((FxU32)a>>28)&0xF)-1)
#define SST_FAKE_ADDRESS_GET_CSIM(a) \
		halInfo.boardInfo[SST_FAKE_ADDRESS_GET_BOARD(a)].sstCSIM
#define SST_FAKE_ADDRESS_GET_OFFSET(a) ((FxU32)a&0x0FFFFFFF)
#define SST_FAKE_ADDRESS_GET_BASE(a) ((FxI32)a&~0x0FFFFFFF)

// HSIM flags, passed in from the diag environment
#define HSIM_HW_SIMULATION              BIT(0)
#define HSIM_TREX_STANDALONE            BIT(1)
#define HSIM_TREX_BACKDOOR_TEXWRITES    BIT(2)

extern HalInfo halInfo;

// internal HAL routines
FxU32 fxHalRead32(FxU32 *addr);
void fxHalWrite32(FxU32 *addr, FxU32 data);

FxBool fxHalIdleFBI( SstRegs *sst );

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
