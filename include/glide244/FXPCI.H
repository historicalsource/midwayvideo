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
** $Revision: 2 $ 
** $Date: 3/31/98 4:01p $ 
**
*/

#ifndef _FXPCI_H_
#define _FXPCI_H_

#define _3DFX_PCI_ID 0x121A

/* 
    16 Busses ( of possible 256, I am making the 
                assumption that busses are numbered 
                in increasing order and that no
                PC will have more than 16 busses )
    32 Slots Per Bus

    Device Number = Bus Number * 32 + Slot Number
*/
//#define MAX_PCI_DEVICES 512
#define MAX_PCI_DEVICES 10

typedef int PciMemType;
#define PciMemTypeUncacheable           0
#define PciMemTypeWriteCombining        1
#define PciMemTypeWriteThrough          4
#define PciMemTypeWriteProtected        5
#define PciMemTypeWriteback             6

typedef enum {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
} PciIOFlag;

typedef struct {
    FxU32              regAddress;
    FxU32              sizeInBytes;
    PciIOFlag          rwFlag;
} PciRegister;

#ifndef KERNEL
extern const PciRegister PCI_VENDOR_ID;     
extern const PciRegister PCI_DEVICE_ID;     
extern const PciRegister PCI_COMMAND;       
extern const PciRegister PCI_STATUS;
extern const PciRegister PCI_REVISION_ID;
extern const PciRegister PCI_CLASS_CODE;
extern const PciRegister PCI_CACHE_LINE_SIZE;
extern const PciRegister PCI_LATENCY_TIMER;
extern const PciRegister PCI_HEADER_TYPE;
extern const PciRegister PCI_BIST;
extern const PciRegister PCI_BASE_ADDRESS_0;
extern const PciRegister PCI_BASE_ADDRESS_1;
extern const PciRegister PCI_BASE_ADDRESS_2;
extern const PciRegister PCI_BASE_ADDRESS_3;
extern const PciRegister PCI_BASE_ADDRESS_4;
extern const PciRegister PCI_BASE_ADDRESS_5;
extern const PciRegister PCI_INTERRUPT_LINE;
extern const PciRegister PCI_INTERRUPT_PIN;
extern const PciRegister PCI_MIN_GNT;
extern const PciRegister PCI_MAX_LAT;
#endif /* #ifndef KERNEL */

const char *
pciGetErrorString( void );

FxU32 
pciGetErrorCode( void );

FX_ENTRY FxBool FX_CALL 
pciOpen( void );

FX_ENTRY FxBool FX_CALL 
pciClose( void );

FX_ENTRY FxBool FX_CALL 
pciDeviceExists( FxU32 device_number );

FX_ENTRY FxBool FX_CALL 
pciGetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );

FX_ENTRY FxBool FX_CALL 
pciSetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );

FX_ENTRY FxBool FX_CALL 
pciMapPhysicalToLinear(FxU32 *linear_addr, FxU32 physical_addr,FxU32 *length);

FX_ENTRY void   FX_CALL 
pciUnmapPhysical( FxU32 linear_addr, FxU32 length );

const char *
pciGetVendorName( FxU16 vendor_id );
const char *
pciGetClassName( FxU32 class_code , FxU32 deviceID);

FX_ENTRY FxBool FX_CALL 
pciFindCard(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum);

FX_ENTRY FxBool FX_CALL 
pciFindCardMulti(FxU32 vID, FxU32 dID, FxU32 *devNum, FxU32 cardNum);

FX_ENTRY FxU32 * FX_CALL 
pciMapCard(FxU32 vID, FxU32 dID, FxI32 len, FxU32 *devNo, FxU32 addrNo);

FX_ENTRY FxU32 * FX_CALL
pciMapCardMulti(FxU32 vID,FxU32 dID,FxI32 l,FxU32 *dNo,FxU32 cNo,FxU32 aNo);

FX_ENTRY FxBool FX_CALL
pciFindMTRRMatch(FxU32 pBaseAddrs, FxU32 psz, PciMemType type, FxU32 *mtrrNum);

FX_ENTRY FxBool FX_CALL
pciFindFreeMTRR(FxU32 *mtrrNum);

FX_ENTRY FxBool FX_CALL
pciSetMTRR(FxU32 mtrrNo, FxU32 pBaseAddr, FxU32 psz, PciMemType type);

#define PCI_ERR_NOERR           0
#define PCI_ERR_WINRTINIT       1
#define PCI_ERR_MEMMAPVXD       2
#define PCI_ERR_MAPMEMDRV       3
#define PCI_ERR_GENPORT         4
#define PCI_ERR_NO_BUS          5
#define PCI_ERR_NOTOPEN         6
#define PCI_ERR_NOTOPEN2        7
#define PCI_ERR_NOTOPEN3        8
#define PCI_ERR_OUTOFRANGE      9
#define PCI_ERR_NODEV           10
#define PCI_ERR_NODEV2          11
#define PCI_ERR_WRITEONLY       12
#define PCI_ERR_READONLY        13
#define PCI_ERR_PHARLAP         14
#define PCI_ERR_WRONGVXD        15
#define PCI_ERR_MEMMAP          16
#define PCI_ERR_MAPMEM          17
#define PCI_ERR_WINRT           18
#define PCI_ERR_VXDINUSE        19

#endif /* _FXPCI_H_ */
