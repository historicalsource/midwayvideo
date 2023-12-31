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
** $Date: 5/04/97 3:44p $ 
**
*/

#ifndef _FXPCI_H_
#define _FXPCI_H_

#define _3DFX_PCI_ID 0x121A

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

const char *pciGetErrorString( void );

FX_ENTRY FxBool FX_CALL pciOpen( void );
FX_ENTRY FxBool FX_CALL pciClose( void );

FX_ENTRY FxBool FX_CALL pciDeviceExists( FxU32 device_number );
#if 0
FX_ENTRY FxBool FX_CALL pciGetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );
FX_ENTRY FxBool FX_CALL pciSetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );
#endif

FX_ENTRY FxBool FX_CALL pciMapPhysicalToLinear( FxU32 *linear_addr, FxU32 physical_addr, FxU32 *length );
FX_ENTRY void   FX_CALL pciUnmapPhysical( FxU32 linear_addr, FxU32 length );

const char *pciGetVendorName( FxU16 vendor_id );
const char *pciGetClassName( FxU32 class_code , FxU32 deviceID);

FX_ENTRY FxBool FX_CALL pciFindCard(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum);
FX_ENTRY FxBool FX_CALL pciFindCardMulti(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum, FxU32 cardNum);
FX_ENTRY FxU32 * FX_CALL pciMapCard(FxU32 vendorID, FxU32 deviceID, FxI32 length, FxU32 *devNum);
FX_ENTRY FxU32 * FX_CALL pciMapCardMulti(FxU32 vendorID, FxU32 deviceID, FxI32 length, FxU32 *devNum, FxU32 cardNum);

#endif



