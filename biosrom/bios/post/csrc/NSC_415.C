/****************************************************************************
    Copyright (C) 1995 National Semiconductor Corp.  All Rights Reserved
*****************************************************************************
*
*   File:               NSC_415.C
*
*   Purpose:            Functions for handling the NSC PC87415 IDE
*                       controller chip.
*
*   Update History:      1/19/95 GDV Created get/set registers
*                        1/24/95 MF  Created calc registers
*                        4/23/96 DMS Change names of types
*                        07Jun96 JVM Convert to K. & R. style bracing and
*                                    generally try to make this code look
*                                    like it was written by a professional
*                                    programmer.
*
****************************************************************************/
#include <stddef.h>
#include <compiler.h>           /* Common definitions */
#include <machine/idtcpu.h>
#include <machine/seattle.h>
#include <machine/gt64010.h>    /* there is only one thing wanted out of here */
#include "nsc_pcic.h"           /* PCI Configuration Routines */
#include "nsc_415.h"            /* NSC PC87415 IDE Definitions */

#ifndef NUM_HDRIVES
# define NUM_HDRIVES	(1)        	/* default to number of IDE devices supported */
#endif

#define PHYS(x) ((mxU32)(x)&0x1FFFFFFF)

/* timing limits for the PC87415: */
#define DBA_CLKMIN              2       /* Data Byte Active */
#define DBA_CLKMAX              17
#define DBR_CLKMIN              1       /* Data Byte Recover */
#define DBR_CLKMAX              16

#define CBA_CLKMIN              3       /* Command Byte Active */
#define CBA_CLKMAX              17
#define CBR_CLKMIN              3       /* Command Byte Recover */
#define CBR_CLKMAX              18

NSC415_Regs     save_controller_regs;


/* Gets current '415 IDE register values */
void NSC415_GetCurrentRegs(DEVHANDLE devhand, NSC415_Regs *regptr, mxBool getdma)
{
    unsigned int    i;                  /* loop counter */
    mxU8            *dbptr;             /* pointer to bytes in structure */
    mxU16           baseadr;            /* base address for bus master regs */
    mxU32           tmp;

    dbptr = (mxU8 *)&(regptr->nsc_ConfigRegs);

    for (i = 0; i < sizeof(N415_CfigRegs); ++i, ++dbptr) {
        *dbptr = PCI_ReadConfigByte(devhand, (mxU8)i);
    }

    if (getdma) {

      /* BAR4 always reads back with bit 0 set */
        baseadr = ((mxU16)regptr->nsc_ConfigRegs.n415_BaseAddr4) & 0x0FFFE;

        dbptr = (mxU8 *)&(regptr->nsc_BusMastRegs);

        for (i = 0; i < sizeof(N415_MastRegs); ++i, ++dbptr, ++baseadr) {

            tmp =  *dbptr = (*(int *)((baseadr + PHYS_TO_K1(PCI_IO_BASE)) & 0xfffffffc));
            *dbptr = (mxU8)(tmp >> (8 * ((baseadr + PHYS_TO_K1(PCI_IO_BASE)) & 0x03)));
        }
    }
}

/* Sets actual '415 IDE controller regs */
void NSC415_SetRegValues(DEVHANDLE devhand, NSC415_Regs *regptr)
{
    N415_CfigRegs   *figptr;            /* pointer to configuration registers */

    figptr  = &(regptr->nsc_ConfigRegs);

    PCI_WriteConfigWord(devhand,  offsetof(N415_CfigRegs, n415_Command),       figptr->n415_Command);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_ProgIface),     figptr->n415_ProgIface);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_Latency),       figptr->n415_Latency);

#if 1
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_Control[0]),    figptr->n415_Control[0]);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_Control[1]),    figptr->n415_Control[1]);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_Control[2]),    figptr->n415_Control[2]);
#else
    PCI_WriteConfigDword(devhand, offsetof(N415_CfigRegs, n415_Control[0]),   *(mxU32*)figptr->n415_Control);
#endif
    PCI_WriteConfigDword(devhand, offsetof(N415_CfigRegs, n415_BaseAddr0),    figptr->n415_BaseAddr0);
    PCI_WriteConfigDword(devhand, offsetof(N415_CfigRegs, n415_BaseAddr1),    figptr->n415_BaseAddr1);
    PCI_WriteConfigDword(devhand, offsetof(N415_CfigRegs, n415_BaseAddr2),    figptr->n415_BaseAddr2);
    PCI_WriteConfigDword(devhand, offsetof(N415_CfigRegs, n415_BaseAddr3),    figptr->n415_BaseAddr3);
    PCI_WriteConfigDword(devhand, offsetof(N415_CfigRegs, n415_BaseAddr4),    figptr->n415_BaseAddr4);

    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C1D1_Dread),    figptr->n415_C1D1_Dread);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C1D1_Dwrite),   figptr->n415_C1D1_Dwrite);

    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C1D2_Dread),    figptr->n415_C1D2_Dread);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C1D2_Dwrite),   figptr->n415_C1D2_Dwrite);

    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C2D1_Dread),    figptr->n415_C2D1_Dread);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C2D1_Dwrite),   figptr->n415_C2D1_Dwrite);

    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C2D2_Dread),    figptr->n415_C2D2_Dread);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_C2D2_Dwrite),   figptr->n415_C2D2_Dwrite);

    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_CmdCtrl_RdWrt), figptr->n415_CmdCtrl_RdWrt);
    PCI_WriteConfigByte(devhand,  offsetof(N415_CfigRegs, n415_SectorSize),    figptr->n415_SectorSize);
}

/* Initializes values for '415 IDE regs */
void NSC415_InitRegValues(mxBool *dodrive, NSC415_Regs *regptr)
{
    N415_CfigRegs   *nptr;              /* pointer to configuration registers */
    N415_MastRegs   *bptr;              /* pointer to bus master registers */


    nptr = &(regptr->nsc_ConfigRegs);

    nptr->n415_Command     = 0x0145;    /* enable bus master and errors */
    nptr->n415_SectorSize  = 0xEE;      /* sector size = 512 bytes */

    nptr->n415_ProgIface   =  0x8A;     /* enable master IDE and legacy mode */
    nptr->n415_Control[0]  =  0x70;     /* No write to Vendor ID regs, mask INTA ... */
                                        /* map both ints to INTA, turn on PWR, turn off IDE resets */
    nptr->n415_Control[1] &= ~0x0B;     /* disable data phase watchdog, unmask both interrupts */
    nptr->n415_Control[1] |=  0x03;     /* mask both interrupts */


#define M0_RD_TIMING    (0xCD)
#define M0_WR_TIMING    (0xCD)
#define CC0_TIMING      (0xB7)

    if (dodrive[0] || dodrive[1]) {     /* configure the first channel */

        nptr->n415_ProgIface  &= ~0x01; /* use legacy mode, not BAR 0,1 */
        nptr->n415_Control[1] &= ~0x48; /* map IDE to BAR 0,1, disable watchdog */
        nptr->n415_Control[1] |=  0x11; /* mask int, buffer BAR 0,1 accesses */
        nptr->n415_Control[2] |=  0x01; /* enable buffers for first channel */
        nptr->n415_BaseAddr0   =  PHYS(PCI_IO_BASE) + 0x400;   /* set the base registers */
        nptr->n415_BaseAddr1   =  PHYS(PCI_IO_BASE) + 0x408;

        if (dodrive[0]) {
            nptr->n415_Control[2]  |= 0x10; /* use IORDY for first drive */
            nptr->n415_C1D1_Dread   = M0_RD_TIMING; /* use mode 0 timings */
            nptr->n415_C1D1_Dwrite  = M0_WR_TIMING;
        }

        if (dodrive[1]) {
            nptr->n415_Control[2]  |= 0x20; /* use IORDY for second drive */
            nptr->n415_C1D2_Dread   = M0_RD_TIMING;
            nptr->n415_C1D2_Dwrite  = M0_WR_TIMING;
        }
    }

    if (dodrive[2] || dodrive[3]) {     /* configure the second channel */

        nptr->n415_ProgIface  &= ~0x04;	/* use legacy mode, not BAR 2,3 */
        nptr->n415_Control[1] &= ~0x8C; /* map IDE to BAR 2,3, disable watchdog */
        nptr->n415_Control[1] |=  0x22; /* mask int, buffer BAR 2,3 accesses */
        nptr->n415_Control[2] |=  0x02; /* enable buffers for second channel */
        nptr->n415_BaseAddr2   = PHYS(PCI_IO_BASE) + 0x410;   /* set the base registers */
        nptr->n415_BaseAddr3   = PHYS(PCI_IO_BASE) + 0x418;
        nptr->n415_Control[1] &= ~0x0B; /* disable data phase watchdog, unmask both interrupts */

        if (dodrive[2]) {
            nptr->n415_Control[2]  |= 0x40; /* use IORDY for first drive */
            nptr->n415_C2D1_Dread   = M0_RD_TIMING;
            nptr->n415_C2D1_Dwrite  = M0_WR_TIMING;
        }

        if (dodrive[3]) {
            nptr->n415_Control[2]  |= 0x80; /* use IORDY for second drive */
            nptr->n415_C2D2_Dread   = M0_RD_TIMING;
            nptr->n415_C2D2_Dwrite  = M0_WR_TIMING;
        }
    }

    nptr->n415_CmdCtrl_RdWrt = CC0_TIMING;

#if 0
    nptr->n415_BaseAddr4 = PHYS(PCI_IO_BASE) + NSC415_DEFVAL_BAR4;

    bptr = &(regptr->nsc_BusMastRegs);

    bptr->n415_Mast1_Cmd  = 0x00;       /* stop any DMA transfers */
    bptr->n415_Mast1_Stat = 0x06;       /* reset error/interrupts */

    bptr->n415_Mast2_Cmd  = 0x00;       /* stop any DMA transfers */
    bptr->n415_Mast2_Stat = 0x06;       /* reset error/interrupts */
#endif
}


void setup_harddrive(void) {
    NSC415_Regs     disk_controller_regs;
    mxBool          drives[3];

/* grab registers */
    NSC415_GetCurrentRegs(PC87415_DEVICE_NUMBER, \
                        &save_controller_regs, (mxBool)FALSE);

    NSC415_GetCurrentRegs(PC87415_DEVICE_NUMBER, \
                        &disk_controller_regs, (mxBool)FALSE);

/* initialize the NS87415 */
    drives[0] = (NUM_HDRIVES > 0);
    drives[1] = (NUM_HDRIVES > 1);
    drives[2] = (NUM_HDRIVES > 2);
    drives[3] = (NUM_HDRIVES > 3);

    NSC415_InitRegValues(&drives[0], &disk_controller_regs);

/* store everything back */
    NSC415_SetRegValues(PC87415_DEVICE_NUMBER, &disk_controller_regs);
}

void restore_harddrive(void) {

  /* restore everything */
    NSC415_SetRegValues(PC87415_DEVICE_NUMBER, &save_controller_regs);

}

/***************************************************************************/
