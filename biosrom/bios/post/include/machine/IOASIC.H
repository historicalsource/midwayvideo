#ifndef _IOASIC_H_
#define _IOASIC_H_
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    IOASIC.H                                                          บ
บ Author:  Jack Miller                                                       บ
บ Created: 20-Jan-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   I/O-ASIC UART Manifest Definitions                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  20Jan97 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <compiler.h>
#include <machine/idtcpu.h>


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                       Application Level Definitions                        ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
/* Player21 register bit definitions */
#define	P1_U		0x0001
#define	P1_D		0x0002
#define	P1_L		0x0004
#define	P1_R		0x0008
#define	P1_LP		0x0010
#define	P1_FR		0x0020
#define	P1_RP		0x0040
#define P1_XX8X     0x0080
#define	P2_U		0x0100
#define	P2_D		0x0200
#define	P2_L		0x0400
#define	P2_R		0x0800
#define	P2_LP		0x1000
#define	P2_FR		0x2000
#define	P2_RP		0x4000
#define P2_8XXX     0x8000


/* Player43 register bit definitions */
#define P1_LK		0x00010000
#define P1_RK		0x00020000
#define P1_TH		0x00040000

#define P2_LK		0x00100000
#define P2_RK		0x00200000
#define P2_TH		0x00400000


/* Misc. register bit definitions */
#define SW_COIN1    0x0001
#define SW_COIN2	0x0002
#define P1_START	0x0004
#define SW_TILT		0x0008
#define SW_TEST		0x0010
#define P2_START	0x0020
#define SW_CREDIT	0x0040
#define SW_COIN3	0x0080
#define SW_COIN4    0x0100
#define P3_START    0x0200
#define P4_START    0x0400
#define SW_VOLDN    0x0800
#define SW_VOLUP    0x1000
#define SW_INTRLK   0x2000
#define SW_BILL     0x4000
#define SW_DOLLAR   0x8000

/* Multi-button/key bit definitions */
#define UPKEYS      (P1_U|P1_LP|P1_RP|P2_U|P2_LP|P2_RP)
#define DNKEYS      (P1_D|P1_LK|P1_RK|P2_D|P2_LK|P2_RK)

#define UPJOY       (P1_U|P2_U)
#define DNJOY       (P1_D|P2_D)
#define RTJOY       (P1_R|P2_R)
#define LTJOY       (P1_L|P2_L)

#define ANYKEYS     (~(UPJOY|DNJOY|RTJOY|LTJOY))

#define ENTKEYS     (P1_TH|P1_FR|P2_TH|P2_FR)
#define EXITKEYS    (SW_CREDIT|P1_START|P2_START)


/* Global function prototypes */
#ifdef CLANGUAGE
extern void     unlock_ioasic(void);
extern mxU16    get_dipswitches(void);
extern mxU16    get_misc(void);
extern mxU32    get_player12(void);
extern void     (*initInput(void(*callBack)(mxU32, mxU32, mxU32))) (mxU32, mxU32, mxU32);
extern void     updateCoinCounters(mxU32 count1, mxU32 count2, mxU32 count3, mxU32 count4);
extern void     wrNVRam(mxU32 memval, mxU32 offset);
extern mxU32    rdNVRam(mxU32 offset);
#endif

/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                          System Level Definitions                          ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/

#define OFFSET_SHIFT            2
#define ASIC_WORD               ((mxVU16 *)PHYS_TO_K1(IO_ASIC_BASE))

#define IO_DIPSWITCH            (0x0000 << OFFSET_SHIFT)    // 16-bits R
#define IO_MISC                 (0x0001 << OFFSET_SHIFT)    // 16-bits R
#define IO_PLAYER2_1            (0x0002 << OFFSET_SHIFT)    // 16-bits R
#define IO_PLAYER4_3            (0x0003 << OFFSET_SHIFT)    // 16-bits R
#define IO_UART_CTRL            (0x0004 << OFFSET_SHIFT)    // 16-bits RW
#define IO_UART_XMIT            (0x0005 << OFFSET_SHIFT)    //  8-bits RW
#define IO_UART_STAT_RECV       (0x0006 << OFFSET_SHIFT)    // 16-bits R
#define IO_METER                (0x0007 << OFFSET_SHIFT)    //  5-bits RW
#define IO_HOST_SOUND_CTRL      (0x0008 << OFFSET_SHIFT)    // 16-bits RW
#define IO_HOST_SOUND_DATA      (0x0009 << OFFSET_SHIFT)    // 16-bits RW
#define IO_HOST_SOUND_STAT      (0x000A << OFFSET_SHIFT)    // 16-bits R
#define IO_SOUND_HOST_DATA      (0x000B << OFFSET_SHIFT)    // 16-bits R
#define IO_HOST_PIC_CMND        (0x000C << OFFSET_SHIFT)    //  4-bits RW
#define IO_PIC_HOST_DATA        (0x000D << OFFSET_SHIFT)    //  8-bits R
#define IO_MAIN_STAT            (0x000E << OFFSET_SHIFT)    // 16-bits R
#define IO_MAIN_CTRL            (0x000F << OFFSET_SHIFT)    // 16-bits RW


#ifdef CLANGUAGE
typedef struct _ASICRegs {
  mxVU16    asicDipSw       __attribute__ ((aligned(4)));
  mxVU16    asicMisc        __attribute__ ((aligned(4)));
  mxVU16    asicP21         __attribute__ ((aligned(4)));
  mxVU16    asicP43         __attribute__ ((aligned(4)));
  mxVU16    asicUartCtrl    __attribute__ ((aligned(4)));
  mxVU16    asicUartXmit    __attribute__ ((aligned(4)));
  union {
    mxVU16  asicUartStat;
    mxVU16  asicUartRecv;
  } usr                     __attribute__ ((aligned(4)));
  mxVU16    asicMeter       __attribute__ ((aligned(4)));
  mxVU16    asicHostSndCtrl __attribute__ ((aligned(4)));
  mxVU16    asicHostSndData __attribute__ ((aligned(4)));
  mxVU16    asicHostSndStat __attribute__ ((aligned(4)));
  mxVU16    asicSndHostData __attribute__ ((aligned(4)));
  mxVU16    asicHostPicCmnd __attribute__ ((aligned(4)));
  mxVU16    asicPicHostData __attribute__ ((aligned(4)));
  mxVU16    asicMainStat    __attribute__ ((aligned(4)));
  mxVU16    asicMainCtrl    __attribute__ ((aligned(4)));
} _ASIC_REGS;
#endif

#define ASIC_REG(reg) ((mxVU16 *)&(reg))


/*
** UART Control Register
*/

#define UCR_CTS_OUT         0x8000
#define UCR_CTS_IN          0x4000
#define UCR_BRK_OUT         0x2000
#define UCR_XMIT_DIS        0x1000
#define UCR_LOOPBACK_ENA    0x0800
#define UCR_ERROR_ENA       0x0400
#define UCR_UART_ENA        0x0200
#define UCR_BIT_8_UNUSED    0x0100
#define UCR_BIT_7_UNUSED    0x0080
#define UCR_BR_MASK         0x007F

/*
** UART Transmit Register
*/

#define UTR_XMIT_MASK       0x00FF


/*
** UART Status/Receive Register
*/

#define USR_CTS_OUT_TRUE    0x8000
#define USR_CTS_IN_TRUE     0x4000
#define USR_XMIT_EMPTY      0x2000
#define USR_XCVR_READY      0x1000
#define USR_XCVR_FULL       0x0800
#define USR_OVERRUN_ERR     0x0400
#define USR_FRAMING_ERR     0x0200
#define USR_BRK_DETECT      0x0100
#define USR_ERROR_MASK      0x0700
#define USR_XCVR_MASK       0x00FF


/*
** Main Status Register - UART bits
*/

#define MSR_CTS_OUT_TRUE    0x8000
#define MSR_CTS_IN_TRUE     0x4000
#define MSR_XMIT_EMPTY      0x2000
#define MSR_XCVR_READY      0x1000
#define MSR_XCVR_FULL       0x0800
#define MSR_OVERRUN_ERR     0x0400
#define MSR_FRAMING_ERR     0x0200
#define MSR_BRK_DETECT      0x0100
#define MSR_INT_GLOBAL      0x0001


/*
** Main Control Register - UART bits
*/

#define MCR_TI320CX_MODE_ENA    0x8000
#define MCR_XMIT_EMTY_INT_ENA   0x2000
#define MCR_XCVR_READY_INT_ENA  0x1000
#define MCR_XCVR_FULL_INT_ENA   0x0800
#define MCR_OVERRUN_ERR_INT_ENA 0x0400
#define MCR_FRAMING_ERR_INT_ENA 0x0200
#define MCR_BRK_DETECT_INT_ENA  0x0100
#define MCR_GLOBAL_INT_ENA      0x0001


/*
** Host To Sound Control Register
*/
#define HSCR_MAX_BAUD_RATE  0x0010


/* Convert baudrate into a value for the Baud Rate Select Register */
#define BR_DIV(clk, brate)  (clk / (brate * 32))


#define PIC_MAX_WRITES      100
#define PIC_READY           0x0001

#define MC_SN_LEN           16

#define IOASIC_LED_BIT      0x4000
#define IOASIC_LED_OFF      0
#define IOASIC_LED_ON       1

#define IOASIC_METER_ENABLE 0x0010
#define IOASIC_METER_BITS   0x000F
#define IOASIC_COIN_METER1  0x0001
#define IOASIC_COIN_METER2  0x0002
#define IOASIC_COIN_METER3  0x0004
#define IOASIC_COIN_METER4  0x0008

#endif
