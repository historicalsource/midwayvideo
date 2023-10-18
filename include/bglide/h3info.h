#ifndef __H3INFO_H__
#define __H3INFO_H__

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

#if defined(__unix__) && ! defined(__H3REGS_H__)
// basic data types
#define FxU8  unsigned char
#define FxU16 unsigned short
#define FxU32 unsigned long
#define FxBool int
// defn of registers not reqd, treat (SstRegs *) as (void *)
typedef void SstRegs;
#endif

#ifdef SST2
#define MAX_NUM_TMUS 4
#elif defined(H4)
#define MAX_NUM_TMUS 2
#else
#define MAX_NUM_TMUS 1
#endif

/*
** H3 Device Information Structure
**
*/

#define MAX_NUM_TMUS_SUPPORTED 4
#if MAX_NUM_TMUS > MAX_NUM_TMUS_SUPPORTED
#   error "need to increase MAX_NUM_TMUS_SUPPORTED"
#endif

typedef struct {		// H3 Device Information Structure
    FxU32 size;                 // size of this structure
    SstRegs *virtAddr[2];	// virtual memory base address
    FxU32 physAddr[2];		// physical memory base address
    FxU16 virtPort;             // virtual i/o port base address
    FxU16 physPort;             // physical i/o port base address
    FxU32 deviceNumber;		// PCI device number
    FxU32 vendorID;		// PCI vendor ID
    FxU32 deviceID;		// PCI device ID
    FxU32 fbiRevision;		// FBI revision number
    FxU32 fbiConfig;		// FBI strapping pins
    FxU32 fbiMemType;		// FBI memory type (poweron strapping bits)
    FxU32 fbiVideoWidth;	// FBI video display X-resolution
    FxU32 fbiVideoHeight;	// FBI video display Y-resolution
    FxU32 fbiVideoRefresh;	// FBI video refresh rate
    FxU32 fbiMemoryFifoEn;	// FBI memory fifo enabled
    FxU32 tmuRevision;		// TMU revision number (for all TMUs)
    FxU32 numberTmus;		// number of TMUs installed
    FxU32 tmuConfig;		// TMU configuration bits
    FxU32 fbiMemSize;		// FBI frame buffer memory (in MBytes)
    FxU32 tmuMemSize[MAX_NUM_TMUS_SUPPORTED]; 	// TMU texture memory (in MBytes)
#ifndef CVG
    FxU8 *agpMem;               // AGP true base address
    FxU8 *agpVirtAddr;          // AGP virtual base address
    FxU32 agpSizeInBytes;       // AGP memory size (in Bytes)
    FxU32 agpBaseAddrH;         // upper 4 bits of AGP physical base address
    FxU32 agpBaseAddrL;         // lower 32 bits of AGP physical base address
    FxU32 agpRqDepth;           // AGP request depth
#endif

    // These cannot be read from the hardware, so we shadow them here
    FxU32 tmuInit0[MAX_NUM_TMUS_SUPPORTED];
    FxU32 tmuInit1[MAX_NUM_TMUS_SUPPORTED];

    // Misc
    FxU32 initGrxClkDone;

    // CSIM specific
    SstRegs *sstCSIM;		// pointer to CSIM structure
    SstRegs *sstHW;		// pointer to HW
} FxDeviceInfo;

#endif /* !__H3INFO_H__ */
