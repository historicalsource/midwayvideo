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
*/
#ifndef __FXDPMI_H__
#define __FXDPMI_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __3DFX_H__
#  include <3dfx.h>
#endif

#include <fxmemmap.h>

/*
** type definitions
*/
typedef FxI16 DpmiSelector_t;

typedef struct
{
  FxI32 EDI;
  FxI32 ESI;
  FxI32 EBP;
  FxI32 reserved;
  FxI32 EBX;
  FxI32 EDX;
  FxI32 ECX;
  FxI32 EAX;
  FxI16 flags;
  FxI16 ES, DS, FS, GS, IP, CS, SP, SS;
} DpmiRMI;

/*
** function prototypes
*/
FxU32
DpmiMapPhysicalToLinear( FxU32 paddr, FxU32 length );

FxBool
DpmiUnmapMemory(FxU32 pAddr, FxU32 length);

void
DpmiUnloadVxd(void);

void*
DpmiAllocDosMem( FxU16 size, DpmiSelector_t *pSel );

FxBool
DpmiFreeDosMem( DpmiSelector_t sel );

void
DpmiExecuteRealModeInterrupt( int intno, DpmiRMI *data );
 
void
DpmiExecuteRealModeProcedure( FxU16 proc_seg, FxU16 proc_off, DpmiRMI *data );

FxBool
DpmiSetMSR(FxU32 ins, FxU32 outs);

FxBool
DpmiGetMSR(FxU32 ins, FxU32 outs);

void
DpmiCheckVxd(FxBool *onWindows, FxU32 *vxdVersion);

FxBool
DpmiSetPassThroughBase(FxU32* const pBaseAddr, FxU32 hwBaseLen);

FxBool
DpmiOutputDebugString(const char* msgBuf);

FxBool
DpmiLinearRangeSetPermission(const FxU32 addrBase, const FxU32 addrLen, const FxBool writeableP);

#ifdef __cplusplus
}
#endif

#endif
