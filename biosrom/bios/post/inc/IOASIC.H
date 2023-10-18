#ifndef _IOASIC_H_
#define _IOASIC_H_
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    IOASIC.H                                                          บ
บ Author:  Jack Miller                                                       บ
บ Created: 17-Sep-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   I/O-ASIC Manifest Definitions                                            บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  17Sep97 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <compiler.h>
#include <machine/idtcpu.h>


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                          System Level Definitions                          ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/

#define OFFSET_SHIFT            2
#define ASIC_WORD               ((mxVU16 *)PHYS_TO_K1(IO_ASIC_BASE))

#define IO_DIPSWITCH            (0x000B << OFFSET_SHIFT)    // 16-bits R
#define IO_MISC                 (0x000A << OFFSET_SHIFT)    // 16-bits R
#define IO_PLAYER2_1            (0x0009 << OFFSET_SHIFT)    // 16-bits R
#define IO_PLAYER4_3            (0x0008 << OFFSET_SHIFT)    // 16-bits R
#define IO_UART_CTRL            (0x0000 << OFFSET_SHIFT)    // 16-bits RW
#define IO_UART_XMIT            (0x0001 << OFFSET_SHIFT)    //  8-bits RW
#define IO_UART_STAT_RECV       (0x0002 << OFFSET_SHIFT)    // 16-bits R
#define IO_METER                (0x0003 << OFFSET_SHIFT)    //  5-bits RW
#define IO_HOST_SOUND_CTRL      (0x0007 << OFFSET_SHIFT)    // 16-bits RW
#define IO_HOST_SOUND_DATA      (0x0006 << OFFSET_SHIFT)    // 16-bits RW
#define IO_HOST_SOUND_STAT      (0x0005 << OFFSET_SHIFT)    // 16-bits R
#define IO_SOUND_HOST_DATA      (0x0004 << OFFSET_SHIFT)    // 16-bits R
#define IO_HOST_PIC_CMND        (0x000F << OFFSET_SHIFT)    //  4-bits RW
#define IO_PIC_HOST_DATA        (0x000E << OFFSET_SHIFT)    //  8-bits R
#define IO_MAIN_STAT            (0x000D << OFFSET_SHIFT)    // 16-bits R
#define IO_MAIN_CTRL            (0x000C << OFFSET_SHIFT)    // 16-bits RW


#ifdef CLANGUAGE
typedef struct _ASICRegs {
  mxVU16    asicUartCtrl    __attribute__ ((aligned(4)));
  mxVU16    asicUartXmit    __attribute__ ((aligned(4)));
  union {
    mxVU16  asicUartStat;
    mxVU16  asicUartRecv;
  } usr                     __attribute__ ((aligned(4)));
  mxVU16    asicMeter       __attribute__ ((aligned(4)));
  mxVU16    asicSndHostData __attribute__ ((aligned(4)));
  mxVU16    asicHostSndStat __attribute__ ((aligned(4)));
  mxVU16    asicHostSndData __attribute__ ((aligned(4)));
  mxVU16    asicHostSndCtrl __attribute__ ((aligned(4)));
  mxVU16    asicP21         __attribute__ ((aligned(4)));
  mxVU16    asicP43         __attribute__ ((aligned(4)));
  mxVU16    asicMisc        __attribute__ ((aligned(4)));
  mxVU16    asicDipSw       __attribute__ ((aligned(4)));
  mxVU16    asicMainCtrl    __attribute__ ((aligned(4)));
  mxVU16    asicMainStat    __attribute__ ((aligned(4)));
  mxVU16    asicPicHostData __attribute__ ((aligned(4)));
  mxVU16    asicHostPicCmnd __attribute__ ((aligned(4)));
} _ASIC_REGS;
#endif

#endif
