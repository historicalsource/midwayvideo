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
** $Date: 9/02/98 12:35a $ 
**
*/

#ifndef _INITVGA_H_
#define _INITVGA_H_

#if defined(__WATCOMC__)
#define _inp inp
#define _outp outp
#define _outpw outpw
#endif

/* 
** Some Standard VGA Registers 
*/
/* Reading */
#define VGA_MISC_OUTPUT_READ    0x3cc

/* Writing */
#define VGA_MISC_OUTPUT_WRITE    0x3c2

/* Read/Write */
#define VGA_REGISTER_INPUT_STATUS_1_MONO                    0x3BA
#define VGA_REGISTER_INPUT_STATUS_1_COLOR                   0x3DA
#define VGA_INPUT_STATUS_1C                     0x3DA
#define VIS1C_PIXEL_DISPLAY_INACTIVE            BIT(0)
#define VIS1C_VERTICAL_RETRACE_ACTIVE           BIT(3)

#define VGA_REGISTER_CRTC                                   0x3D4
#define VR_CRTC_SERIAL_START_ADDRESS_HI_INDEX               0x0C
#define VR_CRTC_SERIAL_START_ADDRESS_LO_INDEX               0x0D
#define AR_CRTC_SERIAL_OVERFLOW_INDEX                       0x1C

/* General Port I/O */
#ifdef GDBG_INFO_ON

#define OUTP(port,val)\
GDBG_INFO((80, "%s:  Writing 0x%x to port 0x%x\n", FN_NAME, val, port));\
_outp(port, val)

#define OUTPW(port,val)\
GDBG_INFO((80, "%s:  Writing 0x%x to port 0x%x\n", FN_NAME, val, port));\
_outpw(port, val)


#define INP(port, val)\
val = _inp(port);\
GDBG_INFO((80, "%s:  Read 0x%x from port 0x%x\n", FN_NAME, val, port))

#define INPW(port, val)\
val = _inpW(port);\
GDBG_INFO((80, "%s:  Read 0x%x from port 0x%x\n", FN_NAME, val, port))

#else

#define OUTP(port, val) _outp(port, val)
#define INP(port, val)  val = _inp(port)

#define OUTPW(port, val) _outpw(port, val)
#define INPW(port, val)  val = _inpw(port)

#endif

/* Macros for Sequencer registers */
#define SEQU_INDEX               0x3c4
#define SEQU_DATA                0x3c5

/* Macros for CRTC registers */
#define CRTC_INDEX              0x3d4
#define CRTC_DATA               0x3d5

#ifdef GDBG_INFO_ON

#define SEQU_SET(reg,val)\
GDBG_INFO((80, "%s:  Writing 0x%x to Sequencer Index 0x%x\n", FN_NAME, val, reg));\
_outp(SEQU_INDEX, reg); _outp(SEQU_DATA, val)
#define SEQU_GET(reg,val)\
_outp(SEQU_INDEX, reg);val = _inp(SEQU_DATA);\
GDBG_INFO((80, "%s:  Read 0x%x from Sequencer Index 0x%x\n", val, reg));

#define CRTC_SET(reg,val)\
GDBG_INFO((80, "%s:  Writing 0x%x to CRTC Index 0x%x\n", FN_NAME, val, reg));\
_outp(CRTC_INDEX, reg); _outp(CRTC_DATA, val)
#define CRTC_GET(reg,val)\
_outp(CRTC_INDEX, reg); val = _inp(CRTC_DATA);\
GDBG_INFO((80, "%s:  Read 0x%x from CRTC Index 0x%x\n", FN_NAME, val, reg))

#else

#define SEQU_SET(reg,val) _outp(SEQU_INDEX, reg); _outp(SEQU_DATA, val)
#define SEQU_GET(reg,val) _outp(SEQU_INDEX, reg); val = _inp(SEQU_DATA)

#define CRTC_SET(reg,val) _outp(CRTC_INDEX, reg); _outp(CRTC_DATA, val)
#define CRTC_GET(reg,d)   _outp(CRTC_INDEX, reg); d = _inp(CRTC_DATA)

#endif


#endif /* _INITVGA_H_ */
