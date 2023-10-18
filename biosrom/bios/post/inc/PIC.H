#ifndef _PIC_H_
#define _PIC_H_
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    PIC.H                                                             บ
บ Author:  Jack Miller                                                       บ
บ Created: 20-Apr-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   PIC Microcontroller Manifest Definitions                                 บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  20Apr97 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/

/* PIC Microcontroller Handshake bits */
#define MC_REQ          0x0010
#define MC_ACK          0x0100
#define MC_RESP         0x0080
#define MC_MORE         0x00FF
#define MC_DONE         0x0000

/* PIC Microcontroller Command Types */
#define MC_IMMEDIATE    0x0001
#define MC_COMPLEX      0x0002

/* PIC Microcontroller Return Data Lengths */
#define MC_SN_LEN       16
#define MC_GET_TIME_LEN 7
#define MC_SET_TIME_LEN 14

/* PIC Microcontroller Commands */
enum {
  MC_NOP_0 = 0,
  MC_GET_SN,
  MC_UPDATE_TIME,
  MC_GET_TIME,
  MC_SET_TIME,
  MC_WR_NVRAM,
  MC_RD_NVRAM,
  MC_NOP_7,
  MC_NOP_8,
  MC_NOP_9,
  MC_NOP_A,
  MC_NOP_B,
  MC_NOP_C,
  MC_NOP_D,
  MC_NOP_E,
  MC_NOP_F
};

/* Real-Time Clock Data Buffer Offsets */
enum {
  MC_SEC = 0,
  MC_MIN,
  MC_HRS,
  MC_DAY,
  MC_DATE,
  MC_MON,
  MC_YEAR
};


/* Real-Time Clock NVRAM */
#define PIC_NVRAM_OFF       (8/sizeof(mxU32))
#define PIC_NVRAM_SZ        (64/sizeof(mxU32))

#endif
