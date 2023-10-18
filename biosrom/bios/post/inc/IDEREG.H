#ifndef _NSC_IDEREG_H_
#define _NSC_IDEREG_H_

#include <compiler.h>

#ifndef NUM_HDRIVES
 #define NUM_HDRIVES    (1)        /* number of IDE devices supported */
#endif

typedef volatile struct nsc415_cmd {
  unsigned char cmd;            /* command */
  unsigned char fill0;          /* reserved */
  unsigned char status;         /* status */
  unsigned char fill1;          /* reserved */
  unsigned long prd_addr;       /* Prd table address */
} NSC415Cmd;

#define DMA_CMD_start	0x01	/* start DMA */
#define DMA_CMD_write	0x08	/* write DMA */
#define DMA_STS_busy	0x01	/* DMA channel is busy */
#define DMA_STS_error	0x02	/* DMA got an error */
#define DMA_STS_int     0x04    /* Interrupt reported from drive */
#define DMA_STS_dma1	0x20	/* drive 1 is DMA capable */
#define DMA_STS_dma2	0x40	/* drive 2 is DMA capable */

typedef volatile struct nsc415_pdt {
  unsigned long     phys;       /* point to physical memory (must be even) */
  unsigned short    byte_cnt;   /* number of bytes (must be even) */
  unsigned short    filler:15;  /* reserved bits */
  unsigned short    eot:1;      /* if 1, no more entries */
} NSC415Pdt;

typedef struct device_desc{
  volatile struct ide_ctl   *ideptr;    /* points to h/w */
  volatile unsigned char    *alt_sts;   /* points to alternate status byte */
  NSC415Cmd                 *dma;       /* pointer to DMA control registers */
  NSC415Pdt                 pdt[4];     /* give 'em a little scatter ability */
//  void (*complete)(struct device_desc *); /* pointer to user completion routine */
//  void (*irq)(struct device_desc *);    /* pointer to interrupt service routine */
//  volatile int              timeout;    /* timeout flag */
//  struct tq                 timer;      /* gross timer for I/O */

  int                       select;     /* drive select bit */
  int                       status;     /* status of device connected */
  int                       cyls;       /* number of cylinders */
  int                       heads;      /* number of heads */
  int                       sectors;    /* number of sectors per track */
  int                       spc;        /* sectors per cylinder */
  int                       lba_capacity; /* total number of sectors per disk */
  int                       max_multsect; /* maximum number of sectors on multi-sector r/w */
  int                       busy;       /* drive currently busy */
  int                       caps;       /* device capabilities */
  int                       dma_timing; /* value to stuff into the timing register */
  int                       pio_timing; /* value to stuff into the timing register */
  int                       dma_ns;     /* nanosecs for DMA read/write */
  int                       pio_ns;     /* nanosecs for PIO read/write */
} DeviceDesc;

/* structure returned by HDIO_GET_IDENTITY, as per ANSI ATA2 rev.2j spec */
/* (VU) = Vendor Unique							 */
typedef struct hd_driveid {
  unsigned short    config;             /*  0 lots of obsolete bit flags */
  unsigned short    cyls;               /*  1 "physical" cyls */
  unsigned short    reserved2;          /*  2 reserved (word 2) */
  unsigned short    heads;              /*  3 "physical" heads */
  unsigned short    track_bytes;        /*  4 (VU) unformatted bytes per track */
  unsigned short    sector_bytes;       /*  5 (VU) unformatted bytes per sector */
  unsigned short    sectors;            /*  6 "physical" sectors per track */
  unsigned short    vendor0;            /*  7 vendor unique */
  unsigned short    vendor1;            /*  8 vendor unique */
  unsigned short    vendor2;            /*  9 vendor unique */
  unsigned char     serial_no[20];      /* 10-19 Serial number ([0,1] == 0, not specified) */
  unsigned short    buf_type;           /* 20 (VU) */
  unsigned short    buf_size;           /* 21 (VU) 512 byte increments; 0 = not_specified */
  unsigned short    vs_bytes;           /* 22 number of vendor specific bytes on r/w longs */
  unsigned char     fw_rev[8];          /* 23-26 Firmware revision */
  unsigned char     model[40];          /* 27-46 Model number */
  unsigned char     max_multsect;       /* 47lsb max sectors on a r/w multiple; 0=not_implemented */
  unsigned char     vendor3;            /* 47msb (VU) */
  unsigned short    reserved48;         /* 48 reserved */
  unsigned char     vendor4;            /* 49lsb (VU) */
  unsigned char     capability;         /* 49msb bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup*/
  unsigned short    reserved50;         /* 50 reserved */
  unsigned char     vendor5;            /* 51lsb (VU) */
  unsigned char     tPIO;               /* 51msb 0=slow, 1=medium, 2=fast */
  unsigned char     vendor6;            /* 52lsb (VU) vendor unique */
  unsigned char     tDMA;               /* 52msb 0=slow, 1=medium, 2=fast */
  unsigned short    field_valid;        /* 53 bits 0:cur_ok 1:eide_ok */
  unsigned short    cur_cyls;           /* 54 logical cylinders */
  unsigned short    cur_heads;          /* 55 logical heads */
  unsigned short    cur_sectors;        /* 56 logical sectors per track */
  unsigned short    cur_capacity0;      /* 57 logical total sectors on drive (lsb) */
  unsigned short    cur_capacity1;      /* 58   (2 words, misaligned int) (msb)    */
  unsigned char     multsect;           /* 59lsb current multiple sector count */
  unsigned char     multsect_valid;     /* 59msb when (bit0==1) multsect is ok */
  unsigned int      lba_capacity;       /* 60+61 total number of sectors */
  unsigned short    dma_1word;          /* 62 single-word dma info */
  unsigned short    dma_mword;          /* 63 multiple-word dma info */
  unsigned short    eide_pio_modes;     /* 64 bits 0:mode3 1:mode4 */
  unsigned short    eide_dma_min;       /* 65 min mword dma cycle time (ns) */
  unsigned short    eide_dma_time;      /* 66 recommended mword dma cycle time (ns) */
  unsigned short    eide_pio;           /* 67 min cycle time (ns), no IORDY  */
  unsigned short    eide_pio_iordy;     /* 68 min cycle time (ns), with IORDY */
  unsigned short    reserved69;         /* 69 reserved */
  unsigned short    reserved70;         /* 70 reserved */
  /* unsigned short reservedxx[57];*/   /* reserved (words 71-127) */
  /* unsigned short vendor7  [32];*/    /* vendor unique (words 128-159) */
  /* unsigned short reservedyy[96];*/   /* reserved (words 160-255) */
} DriveID;

#define PC87415_DEVICE_NUMBER   9       /* PCI device number on Phoenix */
#define NSC415_VENDOR_ID        0x100B
#define NSC415_DEVICE_ID        0x0002
#define NSC415_PART_ID          (((long)NSC415_DEVICE_ID << 16) + NSC415_VENDOR_ID)

#define NSC415_DEFVAL_BAR4      0x0F00  /* default I/O address of BAR4 register */

/* PCI configuration registers */
typedef struct
{
  mxU16 n415_VendorID;      /* RO 00-01: vendor ID (value = 100Bh) */
  mxU16 n415_DeviceID;      /* RW 02-03: device ID (value = 0002h) */

  mxU16 n415_Command;       /* RW 04-05: command register */
  mxU16 n415_Status;        /* RW 06-07: status register */

  mxU8  n415_RevisionID;    /* R  08: revision ID (value = 00h) */
  mxU8  n415_ProgIface;     /* RW 09: programming interface (PIF) */
  mxU8  n415_SubClass;      /* R  0A: sub-class code (01 = IDE) */
  mxU8  n415_ClassCode;     /* R  0B: class code (01 = storage) */

  mxU8  n415_Unused1;       /*    0C: unused */
  mxU8  n415_Latency;       /* RW 0D: latency time in PCI clocks */
  mxU8  n415_HeadType;      /* R  0E: header type (value = 00h) */
  mxU8  n415_BIST;          /* R  0F: ?? (value = 00h) */

  mxU32 n415_BaseAddr0;     /* RW 10-13: base address register 0 */
  mxU32 n415_BaseAddr1;     /* RW 14-17: base address register 1 */
  mxU32 n415_BaseAddr2;     /* RW 18-1B: base address register 2 */
  mxU32 n415_BaseAddr3;     /* RW 1C-1F: base address register 3 */
  mxU32 n415_BaseAddr4;     /* RW 20-23: base address register 4 */

  mxU8  n415_Unused2[24];   /*    24-3B: unused */
  mxU8  n415_IntLine;       /* RW 3C: interrupt line (0Eh) */
  mxU8  n415_IntPin;        /* R  3D: interrupt pin (value = 01) */
  mxU8  n415_Unused3[2];    /*    3E-3F: unused */

  mxU8  n415_Control[3];    /* RW 40-42: control register (000000h) */
  mxU8  n415_WbuffStat;     /* R  43: write buffer status */

  mxU8  n415_C1D1_Dread;    /* RW 44: channel 1 device 1 data read */
  mxU8  n415_C1D1_Dwrite;   /* RW 45: channel 1 device 1 data write */
  mxU8  n415_Unused4[2];    /*    46-47: unused */

  mxU8  n415_C1D2_Dread;    /* RW 48: channel 1 device 2 data read */
  mxU8  n415_C1D2_Dwrite;   /* RW 49: channel 1 device 2 data write */
  mxU8  n415_Unused5[2];    /*    4A-4B: unused */

  mxU8  n415_C2D1_Dread;    /* RW 4C: channel 2 device 1 data read */
  mxU8  n415_C2D1_Dwrite;   /* RW 4D: channel 2 device 1 data write */
  mxU8  n415_Unused6[2];    /*    4E-4F: unused */

  mxU8  n415_C2D2_Dread;    /* RW 50: channel 2 device 2 data read */
  mxU8  n415_C2D2_Dwrite;   /* RW 51: channel 2 device 2 data write */
  mxU8  n415_Unused7[2];    /*    52-53: unused */

  mxU8  n415_CmdCtrl_RdWrt; /* RW 54: command/control read/write */
  mxU8  n415_SectorSize;    /* RW 55: sector size */
}
N415_CfigRegs;

/* DMA bus mastering registers */
typedef struct
{
  mxU8  n415_Mast1_Cmd;     /* RW 00: bus master command (00h) */
  mxU8  n415_Reserved1;     /*    01: reserved */
  mxU8  n415_Mast1_Stat;    /* RW 02: bus master status (00h) */
  mxU8  n415_Reserved2;     /*    03: reserved */
  mxU32 n415_Mast1_Prd;     /* RW 04-07: PRD table address (00000000h) */

  mxU8  n415_Mast2_Cmd;     /* RW 08: bus master command (00h) */
  mxU8  n415_Reserved3;     /*    09: reserved */
  mxU8  n415_Mast2_Stat;    /* RW 0A: bus master status (00h) */
  mxU8  n415_Reserved4;     /*    0B: reserved */
  mxU32 n415_Mast2_Prd;     /* RW 0C-0F: PRD table address (00000000h) */
}
N415_MastRegs;

/* All of the NSC PC87415 registers */
typedef struct
{
    N415_CfigRegs   nsc_ConfigRegs;     /* configuration registers */
    N415_MastRegs   nsc_BusMastRegs;    /* DMA bus mastering registers */
}
NSC415_Regs;

#define PHYS(x) ((mxU32)(x)&0x1FFFFFFF)

#endif
