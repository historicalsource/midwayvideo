/* 	$Id: nsc_415.h,v 1.1 1996/05/07 00:08:36 shepperd Exp $	 */
/****************************************************************************
    Copyright (C) 1995 National Semiconductor Corp.  All Rights Reserved
*****************************************************************************
*
*   File:               NSC_415.H
*
*   Purpose:            Definitions for the PC87415 IDE controller chip.
*
*   Update History:      1/10/95 GDV Created
*
****************************************************************************/

#ifndef NSC_415_H
#define NSC_415_H

#include <compiler.h>

#define NSC415_VENDOR_ID    0x100B
#define NSC415_DEVICE_ID    0x0002
#define NSC415_PART_ID      (((long)NSC415_DEVICE_ID << 16) + NSC415_VENDOR_ID)

#define NSC415_DEFVAL_BAR4  0x0F00      /* default I/O address of BAR4 register */

typedef struct                      /* PCI configuration registers */
{
    mxU16           n415_VendorID;      /* RO 00-01: vendor ID (value = 100Bh) */
    mxU16           n415_DeviceID;      /* RW 02-03: device ID (value = 0002h) */

    mxU16           n415_Command;       /* RW 04-05: command register */
    mxU16           n415_Status;        /* RW 06-07: status register */

    mxU8            n415_RevisionID;    /* R  08: revision ID (value = 00h) */
    mxU8            n415_ProgIface;     /* RW 09: programming interface (PIF) */

    mxU8            n415_SubClass;      /* R  0A: sub-class code (01 = IDE) */
    mxU8            n415_ClassCode;     /* R  0B: class code (01 = storage) */
    mxU8            n415_Unused1;       /*    0C: unused */
    mxU8            n415_Latency;       /* RW 0D: latency time in PCI clocks */
    mxU8            n415_HeadType;      /* R  0E: header type (value = 00h) */
    mxU8            n415_BIST;          /* R  0F: ?? (value = 00h) */

    mxU32           n415_BaseAddr0;     /* RW 10-13: base address register 0 */
    mxU32           n415_BaseAddr1;     /* RW 14-17: base address register 1 */
    mxU32           n415_BaseAddr2;     /* RW 18-1B: base address register 2 */
    mxU32           n415_BaseAddr3;     /* RW 1C-1F: base address register 3 */
    mxU32           n415_BaseAddr4;     /* RW 20-23: base address register 4 */

    mxU8            n415_Unused2[24];   /*    24-3B: unused */
    mxU8            n415_IntLine;       /* RW 3C: interrupt line (0Eh) */
    mxU8            n415_IntPin;        /* R  3D: interrupt pin (value = 01) */
    mxU8            n415_Unused3[2];    /*    3E-3F: unused */

    mxU8            n415_Control[3];    /* RW 40-42: control register (000000h) */
    mxU8            n415_WbuffStat;     /* R  43: write buffer status */

    mxU8            n415_C1D1_Dread;    /* RW 44: channel 1 device 1 data read */
    mxU8            n415_C1D1_Dwrite;   /* RW 45: channel 1 device 1 data write */
    mxU8            n415_Unused4[2];    /*    46-47: unused */

    mxU8            n415_C1D2_Dread;    /* RW 48: channel 1 device 2 data read */
    mxU8            n415_C1D2_Dwrite;   /* RW 49: channel 1 device 2 data write */
    mxU8            n415_Unused5[2];    /*    4A-4B: unused */

    mxU8            n415_C2D1_Dread;    /* RW 4C: channel 2 device 1 data read */
    mxU8            n415_C2D1_Dwrite;   /* RW 4D: channel 2 device 1 data write */
    mxU8            n415_Unused6[2];    /*    4E-4F: unused */

    mxU8            n415_C2D2_Dread;    /* RW 50: channel 2 device 2 data read */
    mxU8            n415_C2D2_Dwrite;   /* RW 51: channel 2 device 2 data write */
    mxU8            n415_Unused7[2];    /*    52-53: unused */

    mxU8            n415_CmdCtrl_RdWrt; /* RW 54: command/control read/write */
    mxU8            n415_SectorSize;    /* RW 55: sector size */
}
N415_CfigRegs;

typedef struct                      /* DMA bus mastering registers */
{
    mxU8            n415_Mast1_Cmd;     /* RW 00: bus master command (00h) */
    mxU8            n415_Reserved1;     /*    01: reserved */
    mxU8            n415_Mast1_Stat;    /* RW 02: bus master status (00h) */
    mxU8            n415_Reserved2;     /*    03: reserved */
    mxU32           n415_Mast1_Prd;     /* RW 04-07: PRD table address (00000000h) */

    mxU8            n415_Mast2_Cmd;     /* RW 08: bus master command (00h) */
    mxU8            n415_Reserved3;     /*    09: reserved */
    mxU8            n415_Mast2_Stat;    /* RW 0A: bus master status (00h) */
    mxU8            n415_Reserved4;     /*    0B: reserved */
    mxU32           n415_Mast2_Prd;     /* RW 0C-0F: PRD table address (00000000h) */
}
N415_MastRegs;

typedef struct                  /* All of the NSC PC87415 registers */
{
    N415_CfigRegs   nsc_ConfigRegs;     /* configuration registers */
    N415_MastRegs   nsc_BusMastRegs;    /* DMA bus mastering registers */
}
NSC415_Regs;

void NSC415_GetCurrentRegs          /* Gets current '415 IDE register values */
(
    DEVHANDLE       devhand,            /* handle for device access */
    NSC415_Regs     *regptr,            /* ptr to returned PC87415 reg values */
    mxBool          getdma              /* TRUE to get bus master DMA registers */
);

void NSC415_InitRegValues           /* Initializes values for '415 IDE regs */
(
    mxBool          *dodrive,           /* TRUE for each drive to initialize */
    NSC415_Regs     *regptr             /* ptr to returned PC87415 reg values */
);

/* PUBLIC void NSC415_CalcRegValues */    /* Calculate new values for '415 IDE regs */
/* ( */
/*    UINT            bus_speed, */          /* calculated bus speed in Mhz */
/*    IDE_IDinfo **   infoptr,   */         /* ptr to info vars for all drives */
/*    DRV_CmdOpts *   optptr,    */         /* ptr to command line overrides */
/*    DRV_ProgVars *  progptr,   */         /* ptr to driver vars for all drives */
/*     NSC415_Regs *   regptr    */          /* ptr to PC87415 controller regs */
/* ); */

/* extern void NSC415_ShowRegValues */    /* Display '415 IDE register values */
/* ( */
/*    NSC415_Regs *   regptr, */             /* ptr to PC87415 controller regs */
/*     DRV_DispStr_Fun dspfun, */             /* ptr to routine to display output */
/*    BOOL            verbose,  */          /* TRUE for verbose output */
/*     BOOL *          dodrive  */           /* TRUE for each drive to show info */
/* ); */

extern void NSC415_SetRegValues     /* Sets actual '415 IDE controller regs */
(
    DEVHANDLE       devhand,            /* handle for device access */
    NSC415_Regs     *regptr             /* ptr to values to set PC87415 regs */
);

#endif
