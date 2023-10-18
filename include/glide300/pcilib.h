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
** $Revision: 5 $ 
** $Date: 9/29/97 6:57p $ 
**
*/

#ifndef _PCILIB_H_
#define _PCILIB_H_

#define CONFIG_ADDRESS_PORT 0xCF8
#define CONFIG_DATA_PORT    0xCFC
#define CONFIG_ADDRESS_ENABLE_BIT 0x80000000UL

#define CONFIG_MAPPING_ENABLE_BYTE 0x80
#define CONFIG_MAPPING_DISABLE_BYTE 0x00

#define CONFIG_SPACE_ENABLE_PORT 0xCF8
#define FORWARD_REGISTER_PORT    0xCFA
#define CONFIG_MAPPING_OFFSET    0xC000

#define PCI_INTERRUPT  0x1A
#define PCI_FUNC_CODE  0xB1
#define PCI_PHYS_ADDR  0x000FFE6E

#define BYTE0(a) ((a) & 0xff)
#define BYTE1(a) (((a) >> 8) & 0xff)
#define VXDREFCOUNT(a) (((a) >> 16) & 0xff)

/* PRIVATE DATA */
FxU32   pciVxdVer;
FxU32   pciErrorCode;
FxBool  pciLibraryInitialized;

FxBool	pciInitializeDDio(void);
void	pciUnmapPhysicalDD( FxU32 linear_addr, FxU32 length ) ;

FxU8    pioInByte ( unsigned short port );              /* inp */
FxU16   pioInWord ( unsigned short port );              /* inpw */
FxU32   pioInLong ( unsigned short port );              /* inpd */

FxBool  pioOutByte ( unsigned short port, FxU8 data );  /* outp */
FxBool  pioOutWord ( unsigned short port, FxU16 data ); /* outpw */
FxBool  pioOutLong ( unsigned short port, FxU32 data ); /* outpd */

#endif /* _PCILIB_H_ */
