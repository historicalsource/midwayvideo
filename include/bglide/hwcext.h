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
** $Header: /releases/banshee/glop/3dfx/devel/h3/minihwc/hwcext.h 2     9/21/98 4:43p Dow $
**
*/
#ifndef HWCEXT_H
#define HWCEXT_H

#ifndef WINNT

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

/*
** D3D notification function
*/

extern void D3notify(void);

#endif /* WINNT */

/*
**  Constants
*/
#define HWCEXT_PROTOCOLREV              0x1

#define HWCEXT_MAX_BASEADDR             0x9
#define HWCEXT_MAX_COLORBUFFERS         0x3

/* The HWC escape */
#define EXT_HWC                         0xfd3

/********************************************************************
**  Here's the protocol:
*********************************************************************/

/*
**  HWCEXT_GETDRIVERVERION
**
**  INPUT:      Nothing
**  OUTPUT:     Driver Major & Minor Version
*/
#define HWCEXT_GETDRIVERVERSION        0x0
typedef struct hwcExtDriverVersionRes_s {
  FxU32
    major,                      /* Driver major version */
    minor;                      /* Driver minor version */
} hwcExtDriverVersionRes_t;

/* 
**  HWCEXT_ALLOCCONTEXT
**
**  INPUT:      Nothing
**  OUTPUT:     Context ID
*/
#define HWCEXT_ALLOCCONTEXT             0x1


#define HWCEXT_ABAPPTYPE_FSEM           0x0 /* Full-screen app */
#define HWCEXT_ABAPPTYPE_WIND           0x1 /* Windowed app */

/* Given to HWCEXT_ALLOCCONTEXT */
typedef struct hwcExtAllocContextReq_s {
  FxU32
    protocolRev,                /* Revision of this protocol */
    appType;                    /* FSEM or WIND */
} hwcExtAllocContextReq_t;

/* Returned from  HWCEXT_ALLOCCONTEXT*/
typedef struct hwcExtAllocContextRes_s {
  FxU32
    contextID;
} hwcExtAllocContextRes_t;

/* 
**  HWCEXT_GETDEVICECONFIG
**  
**  INPUT:      Device number (0 for first, 1 for second....)
**  OUTPUT:     VendorID, deviceID, fbRAM, chip rev
*/
#define HWCEXT_GETDEVICECONFIG          0x2
/* Given to HWCEXT_GETDEVICECONFIG */
typedef struct hwcExtDeviceConfigReq_s {
  HDC
    dc;                         /* Device context */
  FxU32
    devNo;                      /* Device number (0, 1, ...) */
} hwcExtDeviceConfigReq_t;

/* Returned from HWCEXT_GETDEVICECONFIG */
typedef struct hwcExtDeviceConfigRes_s {
  FxU32
    devNum,                     /* Device Number */
    vendorID,                   /* PCI Vendor ID */
    deviceID,                   /* PCI Device ID */
    fbRam,                      /* How much frame buuffer RAM  */
    chipRev;                    /* Chip Revision */
  FxU32 pciStride;              /* 1024, 2048, 4096, 8192, 16384 */
  FxU32 hwStride;               /* hardware x-translation stride  */
  FxU32 tileMark;               /* hardware tile addressing watermark */
} hwcExtDeviceConfigRes_t;

/* 
**  HWCEXT_GETLINEARADDR
**
**  INPUT:      Device number
**  OUTPUT:     Number of base addresses, base address list
*/
#define HWCEXT_GETLINEARADDR            0x3
/* Given to HWCEXT_GETLINEARADDR */
typedef struct hwcExtLinearAddrReq_s {
  FxU32 devNum;                 /* Device Number */
  FxU32 pHandle;                /* Process Handle */
} hwcExtLinearAddrReq_t;

/* Returned from HWCEXT_GETLINEARADDR */
typedef struct hwcExtLinearAddrRes_s {
  FxU32
    numBaseAddrs,               /* # base addresses */
    baseAddresses[HWCEXT_MAX_BASEADDR]; /* linear Addresses  */
} hwcExtLinearAddrRes_t;

/*
**  HWCEXT_ALLOCFIFO
**  
**  INPUT:      App type, nColor buffs, nAux buffs
**  OUTPUT:     nColor buffs, nAux Buffs, buffer offsets, fifo ptr,
**              fifo size, fifo type.
*/
#define HWCEXT_ALLOCFIFO             0x4

/* Returned from HWCEXT_ALLOCFIFO */
#define HWCEXT_FIFOFB           0x1 /* FIFO in frame buffer */
#define HWCEXT_FIFOHOST         0x2 /* FIFO in host memory */
#define HWCEXT_FIFOAGP          0x3 /* FIFO in AGP */

typedef struct hwcExtAllocFIFORes_s {
  FxU32
    commandBuffer,              /* Command buffer */
    commandBufferSz,            /* Size of command buffer */
    psBuffer,                   /* Persistent state buffer */
    psBufferSz;                 /* Size of persistent state buffer */
} hwcExtAllocFIFORes_t;

/*  
**  HWCEXT_EXECUTEFIFO
**
**  INPUT:      ptr to FIFO buffer, fifosize, persistent state FIFO & size
**  OUTPUT:     Status only.
**
*/
#define HWCEXT_EXECUTEFIFO      0x5

/* Given to HWCEXT_EXECUTEFIFO */
typedef struct hwcExtExecuteFifoReq_s {
  FxU32
    fifoPtr,                    /* Ptr to FIFO buffer to execute */
    fifoSizeInDWORDS,           /* Size of FIFO in DWORDS */
    persistentState,            /* Ptr to persistent state FIFO */
    psSizeInDWORDS;             /*  */
} hwcExtExecuteFifoReq_t;

/* 
**  HWCEXT_QUERYCONTEXT
**
**  INPUT:      Context ID
**  OUTPUT:     Status 
*/
#define HWCEXT_QUERYCONTEXT     0x6

#define HWC_CONTEXT_ACTIVE      0x0
#define HWC_CONTEXT_LOST        0x1

/*
**  HWCEXT_RELEASECONTEXT
**  
**  INPUT:      Context ID
**  OUTPUT:     Status
*/
#define HWCEXT_RELEASECONTEXT   0x7

/* Given to HWCEXT_RELEASECONTEXT */
typedef struct hwcExtReleaseContextReq_s {
  FxU32
    contextID;                  /* duh */
} hwcExtReleaseContextReq_t;

#define HWCEXT_HWCSETEXCLUSIVE  0x8

#define HWCEXT_HWCRLSEXCLUSIVE  0x9


/*
**  HWCEXT_I2C_WRITE_REQ
**
**  INPUT: i2c address, register number, dwValue to write
**  OUTPUT: status   (FXTRUE if OK, FXFALSE if fail, I2C_BUSY if busy)
*/

#define HWCEXT_I2C_WRITE_REQ    0xa

typedef struct hwcExtI2CwriteReq_s {
        FxU32 deviceAddress;    /* bus address of device */
        FxU32 i2c_regNum;       /* sometimes called subaddress */
        FxU32 i2c_regValue;     /* the value to write */
} hwcExtI2CwriteReq_t;

/*
**  HWCEXT_I2C_READ_REQ
**
**  INPUT: i2c address, register number
**  OUTPUT: status   (FXTRUE if OK, FXFALSE if fail, I2C_BUSY if busy)
**          value read: optRes.i2c_ReadRes.i2c_regValue
*/

#define HWCEXT_I2C_READ_REQ     0xb

typedef struct hwcExtI2CreadReq_s {
        FxU32 deviceAddress;    /* bus address of device */
        FxU32 i2c_regNum;       /* sometimes called subaddress */
} hwcExtI2CreadReq_t;

#define HWCEXT_I2C_READ_RES     0xc

typedef struct hwcExtI2CreadRes_s {
        FxU32 i2c_regValue;     /* the value read from reg above */
} hwcExtI2CreadRes_t;

/*
**  HWCEXT_I2C_WRITE_REQ
**
**  INPUT: i2c address, register number, dwValue to write
**  OUTPUT: status   (FXTRUE if OK, FXFALSE if fail, I2C_BUSY if busy)
*/

#define HWCEXT_I2C_MULTI_WRITE_REQ    0xd
typedef struct hwcExtI2CmultiWriteReq_s {
        FxU32 deviceAddress;    /* bus address of device */
        FxU32 i2c_regNum;       /* sometimes called subaddress */
        FxU8  *i2c_regValues;   /* the value to write */
        FxU32 i2c_numBytes;     /* number of bytes to xfer */
} hwcExtI2CmultiWriteReq_t;


/*
**  HWCEXT_GETAGPINFO
**
**  Get the relavent AGP info:  linear address, physical address, size.
*/
#define HWCEXT_GETAGPINFO            0xe
typedef struct hwcExtAGPInfoRes_s {
  FxU32
    lAddr,                      /* Linear Address of AGP memory */
    pAddr,                      /* Physical Address of AGP memory */
    size;                       /* Size */
} hwcExtAGPInfoRes_t;

/*
**  HWCEXT_VIDTIMING
**
**  Send the FxVidTiming structure to the driver.
**
*/
#define HWCEXT_VIDTIMING        0xf
typedef struct hwcExtVidTimingReq_s {
  void *vidTiming;
} hwcExtVidTimingReq_t;

/*
**  The Request structure (given as input data to ExtEscape() )
*/
typedef struct hwcExtRequest_s {
  FxU32
    contextID,                  /* ID of this context */
    which;                      /* Which request */
  union reqOptData {
    hwcExtAllocContextReq_t     allocContextReq;
    hwcExtDeviceConfigReq_t     deviceConfigReq;
    hwcExtLinearAddrReq_t       linearAddrReq;
    hwcExtExecuteFifoReq_t      executeFifoReq;
    hwcExtReleaseContextReq_t   releaseContextReq;
    hwcExtI2CwriteReq_t         i2c_WriteReq;
    hwcExtI2CreadReq_t          i2c_ReadReq;
    hwcExtI2CmultiWriteReq_t    i2c_MultiWriteReq;
    hwcExtVidTimingReq_t        vidTimingReq;
  } optData;
} hwcExtRequest_t;

typedef struct hwcExtResult_s {
  FxI32
    resStatus;
  union resOptData {
    hwcExtDriverVersionRes_t    driverVersionRes;
    hwcExtAllocContextRes_t     allocContextRes;
    hwcExtDeviceConfigRes_t     deviceConfigRes;    
    hwcExtLinearAddrRes_t       linearAddressRes;
    hwcExtAllocFIFORes_t        allocFifoRes;
    hwcExtI2CreadRes_t          i2c_ReadRes;
    hwcExtAGPInfoRes_t          agpInfoRes;
  } optData;
} hwcExtResult_t;

#ifdef WINNT
/*
 * Global structure used to store Glide state, used by the display driver
 *
 *   This will need changing if one driver supports multiple boards.
 *
 */

typedef struct
{
    LONG        glideGDIFlags;                  /* For cooperation with GDI.  (Exclusive flag) */
    HANDLE      glideProcessHandle;     /* Handle of glide process that last mapped membases */
    BYTE*       glideRegBase;           /* Saved glide register base address */
    BYTE*       glideLfbBase;           /* Saved glide Lfb base address - for mapping only */
    BYTE*       glideScreenBase;        /* Saved glide screen base address - used by glide. */

    ULONG       vidProcCfg;
    ULONG       vidDesktopOverlayStride;
} GLIDESTATE;

extern GLIDESTATE glideState;
#endif                                                  /* WINNT */

#endif                          /* HWCEXT_H not defined */
