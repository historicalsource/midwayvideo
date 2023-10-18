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
** $Header: /Releases/Banshee/GLOP/3Dfx/Devel/H3/minihwc/hwcio.h 1     9/02/98 12:35a Sapphire $
** $Log: /Releases/Banshee/GLOP/3Dfx/Devel/H3/minihwc/hwcio.h $
** 
** 1     9/02/98 12:35a Sapphire
** 
** 2     3/11/98 8:27p Dow
** WinGlide
** 
** 1     3/04/98 4:13p Dow
**
*/
#ifndef HWCIO_H
#define HWCIO_H

extern char *h3SstIORegNames[];
extern char *h3SstcmdAGPRegNames[];
extern char *waxRegNames[];
extern char *h3SstRegNames[];

/*
**  I/O Macros
*/
#define HWC_IO_STORE(regInfo, reg, val)\
GDBG_INFO(120,"Storing 0x%x to IO Register %s\n", val,\
  h3SstIORegNames[(offsetof(SstIORegs, reg)) >> 2]);\
((SstIORegs *) regInfo.ioMemBase)->reg = val

#define HWC_IO_LOAD(regInfo, reg, val)\
val = ((SstIORegs *) regInfo.ioMemBase)->reg;\
GDBG_INFO(120,"Loaded 0x%x from IO Register %s\n", val,\
  h3SstIORegNames[(offsetof(SstIORegs, reg)) >> 2]);

#define HWC_CAGP_STORE(regInfo, reg, val)\
GDBG_INFO(120, "Storing 0x%x to CAGP Register %s\n", val,\
  h3SstcmdAGPRegNames[(offsetof(SstCRegs, reg)) >> 2]);\
((SstCRegs *) (regInfo.cmdAGPBase))->reg = val

#define HWC_CAGP_LOAD(regInfo, reg, val)\
val = ((SstCRegs *) (regInfo).cmdAGPBase)->reg;\
GDBG_INFO(120, "Loaded 0x%x from CAGP Register %s\n", val,\
  h3SstcmdAGPRegNames[(offsetof(SstCRegs, reg)) >> 2]);
  
#if 0
#define HWC_WAX_STORE(regInfo, reg, val)\
GDBG_INFO(120, "Storing 0x%x to WAX Register %s\n", val,\
  waxRegnames[(offsetof(SstGRegs, reg)) >> 2]);\
((SstGRegs *) regInfo->waxRegs)->reg = val

#define HWC_WAX_LOAD(regInfo, reg, val)\
val = ((SstGRegs *) regInfo->waxRegs)->reg;\
GDBG_INFO(120, "Loaded 0x%x from WAX Register %s\n", val,\
  waxRegnames[(offsetof(SstGRegs, reg)) >> 2]);
#endif

#define HWC_SST_STORE(regInfo, reg, val)\
GDBG_INFO(120, "Storing 0x%x to 3D Register %s\n", val,\
 h3SstRegNames[(offsetof(SstRegs, reg)) >> 2]);\
((SstRegs *) regInfo->sstRegs)->reg = val

#define HWC_SST_LOAD(regInfo, reg, val)\
val = ((SstRegs *) regInfo->sstRegs)->reg;\
GDBG_INFO(120, "Loaded 0x%x from WAX Register %s\n", val,\
 h3SstRegNames[(offsetof(SstRegs, reg)) >> 2]);

#endif                          /* HWCIO_H not defined */
