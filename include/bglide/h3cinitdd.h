/* -*-c++-*- */
/* $Header: /Releases/Banshee/GLOP/3Dfx/Devel/H3/cinit/h3cinitdd.h 1     9/02/98 12:35a Sapphire $ */
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
**
**
** $Revision: 1 $
** $Date: 9/02/98 12:35a $
*/


#ifndef __GOOSE__
#include <conio.h>
#endif
#include <stddef.h>

#if defined(__WATCOMC__)
#define _inp inp
#define _outp outp

#define _inpw inpw
#define _outpw outpw

#define _inpd inpd
#define _outpd outpd

#endif									 


#define SSTIOADDR(regName)      ((FxU16)offsetof(SstIORegs, regName))

#ifdef __GOOSE__


#define _outp(a,b) 	  *((FxU8 *) (a)) = (FxU8)(b)
#define _outpw(a,b)   *((FxU16 *) (a)) = (FxU16)(b)
#define _outpd(a,b)	  *((FxU32 *) (a)) = (FxU32)(b)
#define _inp(a)	 	  (FxU8)(*((FxU8 *) (a)))
#define _inpw(a) 	  (FxU16)(*((FxU16 *) (a)))
#define _inpd(a) 	  (FxU32)(*((FxU32 *) (a)))




static __inline__ void iset32(FxU32 *where,FxU32 what) {
volatile int i;
    *where = what;
	for (i=0;i<10;i++);
//    printf("iget32(%x):%x \n",(int)where,(int)what);
}

#define ISET32(addr, value) iset32((FxU32 *) ((FxU32) regBase + (FxU32) (SSTIOADDR(addr))),(FxU32) value) 


//#define ISET32(addr, value)\
//printf("ISET32(%x):%x\n",(FxU32) ((FxU32) regBase + (FxU32) (SSTIOADDR(addr))),(int)value); \
//_outpd((FxU32) ((FxU32) regBase + (FxU32) (SSTIOADDR(addr))), value) 


static __inline__ FxU32 iget32(FxU32 *where) {
//volatile int i;
    FxU32 what;
    what = *where;
//	for (i=0;i<100000;i++);
//    printf("iget32(%x):%x \n",(int)where,(int)what);
    return what;
}

#define IGET32(addr) iget32((FxU32 *)((FxU32) regBase + (FxU32) (SSTIOADDR(addr))))

//#define IGET32(addr) _inpd(((FxU32) regBase + (FxU32) (SSTIOADDR(addr))))


#else

#define ISET32(addr, value)     _outpd((FxU16) ((FxU16) regBase + (FxU16) (SSTIOADDR(addr))), value)
#define IGET32(addr)            _inpd((FxU16) ((FxU16) regBase + (FxU16) (SSTIOADDR(addr))))

#endif



#ifdef __GOOSE__



#define CHECKFORROOM 		do { ; } while (! (*(FxU32 *)regBase & 0x3ff))

//#define CHECKFORROOM 	while (!(_inp((FxU32) regBase) & (FxU32)(0x3ff)))
//#define CHECKFORROOM do { ; } while (! (*(FxU32 *)regBase & 0x3ff))
						
static __inline__ FxU8 iget8phys(FxU8 *where) {
    FxU8 what;
    what = *where;
    return what;
}

#define IGET8PHYS(a)   iget8phys((FxU8 *) ((FxU32) (regBase) + (FxU32) (a)))
//#define IGET8PHYS(a)   _inp((FxU32) ((FxU32) (regBase) + (FxU32) (a)))


static __inline__ FxU16 iget16phys(FxU16 *where) {
    FxU16 what;
    what = *where;
    return what;
}

#define IGET16PHYS(a) iget16phys((FxU16 *) ((FxU32) (regBase) + (FxU32)(a)))
//#define IGET16PHYS(a) _inpw((FxU32) ((FxU32) (regBase) + (FxU32)(a)))



#define ISET8PHYS(a,b) {FxU32 port = (FxU32) (regBase) + (FxU32) (a);\
						GDBG_INFO(120, "OUT8:  Port 0x%x Value 0x%x\n", port, b);\
						_outp(port, (FxU32) (b));}

#define ISET16PHYS(a,b) {FxU32 port = (FxU32)(regBase) + (FxU32)(a);\
						GDBG_INFO(120, "OUT16:  Port 0x%x Value 0x%x\n", port, b);\
						_outpw(port, (FxU32) (b));}

#else

#define CHECKFORROOM while (! (_inp((FxU16) regBase) & (FxU16)(0x3ff)))
#define IGET8PHYS(a)   	_inp((FxU16) ((FxU16) (regBase) + (FxU16) (a)))
#define IGET16PHYS(a)  	_inpw((FxU16) ((FxU16) (regBase) + (FxU16)(a)))
#define ISET8PHYS(a,b) {
FxU16 port = (FxU16) (regBase) + (FxU16) (a);\
GDBG_INFO(120, "OUT8:  Port 0x%x Value 0x%x\n", port, b);\
_outp(port, (FxU8) (b));}

#define ISET16PHYS(a,b) {
FxU16 port = (FxU16)(regBase) + (FxU16)(a);\
GDBG_INFO(120, "OUT16:  Port 0x%x Value 0x%x\n", port, b);\
_outpw(port, (FxU16) (b));}

#endif

#define MESSAGE GDBG_PRINTF

#ifdef OLD

#define ISET32(a,b)\
GDBG_INFO(120, "SET32: Register 0x%x Value 0x%x\n", (FxU32) (&((SstIORegs *)regBase)->a) - (FxU32) regBase, b); \
((FxU32) (((SstIORegs *) regBase)->a)) = (FxU32) b

#define IGET32(a) ((FxU32) (((SstIORegs *) regBase)->a))

#endif /* #ifdef OLD */
