#ifndef _GT64010_H_
#define _GT64010_H_

/*
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º                                                                            º
º File:    GT64010.H                                                         º
º Author:  Jack Miller                                                       º
º Created: 01-Apr-1996                                                       º
º                                                                            º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                            º
º     Manifest definitions for the GT64010 System Controller.                º
º                                                                            º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º HISTORY:                                                                   º
º                                                                            º
º  07Jan97 JVM  Modified for Galileo chip revision 1A.                       º
º  24Jul96 JVM  Add data structures for PCI configuration registers.         º
º  29May96 JVM  Changed structures to use 'compiler.h' defintions.           º
º  01Apr96 JVM  Creation                                                     º
º                                                                            º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
*/

#if 0
/*
** Be sure to change this when we get revision 1a.
*/
#define GT_64010_REV    1
#endif

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                  GT64010                                   ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

/* CPU Interface */
/*
** According to the GT-64010 Rev. 1 Silicon Errata List the following CPU
** interface registers, addresses 0x0000 - 0x0078, must be READ TWICE !!!
*/

#define GT_CPU_INTERFACE_CFG    0x0000

/* CPU Interface - Processor Address Space */
#define GT_RAS10_LO             0x0008
#define GT_RAS10_HI             0x0010
#define GT_RAS32_LO             0x0018
#define GT_RAS32_HI             0x0020

#define GT_CS20_LO              0x0028
#define GT_CS20_HI              0x0030
#define GT_CS3B_LO              0x0038
#define GT_CS3B_HI              0x0040

#define GT_PCI_IO_LO            0x0048
#define GT_PCI_IO_HI            0x0050

#define GT_PCI_MEM_LO           0x0058
#define GT_PCI_MEM_HI           0x0060

#define GT_INTERNAL             0x0068

#define GT_BUS_ERR_LO           0x0070
#define GT_BUS_ERR_HI           0x0078

/* DRAM and Device Address Space */
#define GT_RAS0_LO              0x0400
#define GT_RAS0_HI              0x0404
#define GT_RAS1_LO              0x0408
#define GT_RAS1_HI              0x040C
#define GT_RAS2_LO              0x0410
#define GT_RAS2_HI              0x0414
#define GT_RAS3_LO              0x0418
#define GT_RAS3_HI              0x041C

#define GT_CS0_LO               0x0420
#define GT_CS0_HI               0x0424
#define GT_CS1_LO               0x0428
#define GT_CS1_HI               0x042C
#define GT_CS2_LO               0x0430
#define GT_CS2_HI               0x0434
#define GT_CS3_LO               0x0438
#define GT_CS3_HI               0x043C

#define GT_CSB_LO               0x0440
#define GT_CSB_HI               0x0444

#define GT_ADDR_DECODE_ERR      0x0470

/* DRAM Configuration */
#define GT_DRAM_CONFIG          0x0448

/* DRAM Parameters */
#define GT_DRAM0_CONFIG         0x044C
#define GT_DRAM1_CONFIG         0x0450
#define GT_DRAM2_CONFIG         0x0454
#define GT_DRAM3_CONFIG         0x0458

/* Device Parameters */
#define GT_DEV0_CONFIG          0x045C
#define GT_DEV1_CONFIG          0x0460
#define GT_DEV2_CONFIG          0x0464
#define GT_DEV3_CONFIG          0x0468
#define GT_DEVB_CONFIG          0x046C

/* Device Parameters Bit Shift Definitions */
#define _TurnOff_s              0
#define _AccToFirst_s           3
#define _AccToNext_s            7
#define _ADStoWr_s              11
#define _WrActive_s             14
#define _WrHigh_s               17
#define _DevWidth_s             20
#define _DevLoc_s               23
#define _LatchFunct_s           25
#define _Parity_s               30

#define GT_DEV0_CFG_VALUE       ((0<<30)+(0<<25)+(0<<23)+(2<<20)+(4<<17)+(6<<14)+(3<<11)+(8<<7) +(8<<3) +(4<<0))
#define GT_DEV2_CFG_VALUE       ((0<<30)+(0<<25)+(0<<23)+(2<<20)+(7<<17)+(7<<14)+(7<<11)+(15<<7)+(15<<3)+(7<<0))
#define GT_DEV1_CFG_VALUE       ((0<<30)+(0<<25)+(0<<23)+(2<<20)+(4<<17)+(6<<14)+(3<<11)+(8<<7) +(8<<3) +(4<<0))
#define GT_DEV3_CFG_VALUE       ((0<<30)+(0<<25)+(0<<23)+(2<<20)+(4<<17)+(6<<14)+(3<<11)+(8<<7) +(8<<3) +(4<<0))


/* DMA Record */
#define GT_CHAN0_COUNT          0x0800
#define GT_CHAN1_COUNT          0x0804
#define GT_CHAN2_COUNT          0x0808
#define GT_CHAN3_COUNT          0x080C

#define GT_CHAN0_SOURCE         0x0810
#define GT_CHAN1_SOURCE         0x0814
#define GT_CHAN2_SOURCE         0x0818
#define GT_CHAN3_SOURCE         0x081C

#define GT_CHAN0_DEST           0x0820
#define GT_CHAN1_DEST           0x0824
#define GT_CHAN2_DEST           0x0828
#define GT_CHAN3_DEST           0x082C

#define GT_CHAN0_NEXT_REC       0x0830
#define GT_CHAN1_NEXT_REC       0x0834
#define GT_CHAN2_NEXT_REC       0x0838
#define GT_CHAN3_NEXT_REC       0x083C

/* DMA Channel Control */
#define GT_CHAN0_CTRL           0x0840
#define GT_CHAN1_CTRL           0x0844
#define GT_CHAN2_CTRL           0x0848
#define GT_CHAN3_CTRL           0x084C

/* DMA Arbiter */
#define GT_ARBITER_CTRL         0x0860

/* Timer/Counter */
#define GT_TIMER_COUNTER_0      0x0850
#define GT_TIMER_COUNTER_1      0x0854
#define GT_TIMER_COUNTER_2      0x0858
#define GT_TIMER_COUNTER_3      0x085C
#define GT_TIMER_COUNTER_CTRL   0x0864

/* PCI Internal */
#define GT_PCI_COMMAND          0x0C00
#define GT_PCI_TIMEOUT_RETRY    0x0C04
#define GT_PCI_RAS10_BANK_SIZE  0x0C08
#define GT_PCI_RAS32_BANK_SIZE  0x0C0C
#define GT_PCI_CS20_BANK_SIZE   0x0C10
#define GT_PCI_CS3B_BANK_SIZE   0x0C14

#define GT_PCI_SERR_MASK        0x0C28
#define GT_PCI_INT_ACK          0x0C34
#define GT_PCI_CONFIG_ADDR      0x0CF8
#define GT_PCI_CONFIG_DATA      0x0CFC

/* Interrupts */
#define GT_INT_CAUSE            0x0C18
#define GT_CPU_INT_MASK         0x0C1C
#define GT_PCI_INT_MASK         0x0C24

/* Interrupt Bits */
#define GT_INT_SUM              0x00000001
#define GT_MEM_OUT              0x00000002
#define GT_DMA_OUT              0x00000004
#define GT_CPU_OUT              0x00000008
#define GT_DMA0_COMP            0x00000010
#define GT_DMA1_COMP            0x00000020
#define GT_DMA2_COMP            0x00000040
#define GT_DMA3_COMP            0x00000080
#define GT_T0_EXP               0x00000100
#define GT_T1_EXP               0x00000200
#define GT_T2_EXP               0x00000400
#define GT_T3_EXP               0x00000800
#define GT_MAS_RD_ERR           0x00001000
#define GT_SLV_WR_ERR           0x00002000
#define GT_MAS_WR_ERR           0x00004000
#define GT_SLR_RD_ERR           0x00008000
#define GT_ADDR_ERR             0x00010000
#define GT_MEM_ERR              0x00020000
#define GT_MAS_ABORT            0x00040000
#define GT_TAR_ABORT            0x00080000
#define GT_RETRY_CTR            0x00100000
#define GT_CPU_INT              0x03E00000
#define GT_PCI_INT              0x3C000000
#define GT_CPU_SUM              0x40000000
#define GT_PCI_SUM              0x80000000
#define GT_ONCHIP_MASK          0x001FFFFE
#define GT_CPU_MASK             0x03E00000
#define GT_PCI_MASK             0x3C000000


/* PCI Configuration */

/*
** PCI Configuration Register Numbers
*/
#define PCI_CR0                 0
#define PCI_CR1                 1
#define PCI_CR2                 2
#define PCI_CR3                 3
#define PCI_CR4                 4
#define PCI_CR5                 5
#define PCI_CR6                 6
#define PCI_CR7                 7
#define PCI_CR8                 8
#define PCI_CR9                 9
#define PCI_CR10                10
#define PCI_CR11                11
#define PCI_CR12                12
#define PCI_CR13                13
#define PCI_CR14                14
#define PCI_CR15                15



/*
** According to the GT-64010 Rev. 1 Silicon Errata List the register
** numbers have to be left shifted twice before being written to the
** GT_PCI_CONFIG_ADDR register to access the PCI configuration registers.
*/
#define GT_PCI_DEVICE_VENDOR_ID 0
#define GT_PCI_STAT_CMND        1
#define GT_PCI_CLASS_CODE_REV   2
#define GT_PCI_HDR_LTMR_CACHE   3
#define GT_PCI_RAS10_BASE       4
#define GT_PCI_RAS32_BASE       5
#define GT_PCI_CS20_BASE        6
#define GT_PCI_CS3B_BASE        7
#define GT_PCI_INT_REG_MEM_BASE 8
#define GT_PCI_INT_IO_BASE      9



/*
** Device numbers
*/
#define GT64010_DEVICE_NUMBER       0
#define SST1_DEVICE_NUMBER          PCI_SLOT_2_DEVICE_NUMBER
#define PC87415_DEVICE_NUMBER       9
#define PCI_SLOT_2_DEVICE_NUMBER    8
#define PCI_SLOT_1_DEVICE_NUMBER    7
#define PCI_SLOT_0_DEVICE_NUMBER    6
#define GLINT300SX_DEVICE_NUMBER    PCI_SLOT_2_DEVICE_NUMBER



/*
** Miscellaneous definitions
*/
#define ENABLE          1
#define DISABLE         0
#define ALLOW_ACCESS    1
#define RESET           0
#define ACTIVE          1
#define NOT_ACTIVE      0



#ifdef CLANGUAGE
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                Structures                                  ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

#include <compiler.h>

/*
**    NOTE: The following structures are defined for LITTLE ENDIAN only !
*/

/* DMA Controller */

typedef union _dmaChannelControl {

  mxU32     reg;
  struct {
    mxU32 reserved0         : 1;    // (0)     must be 0
    mxU32 reserved1         : 1;    // (1)     must be 0
    mxU32 srcDir            : 2;    // (3:2)   00 = inc, 01 = dec, 11 = hold
    mxU32 destDir           : 2;    // (5:4)   00 = inc, 01 = dec, 11 = hold
    mxU32 datTransLim       : 3;    // (8:6)   000 = 4 bytes, 001 = 8 bytes
                                    //         011 = 16 bytes, 111 = 32 bytes
    mxU32 chainMod          : 1;    // (9)     0 = chained, 1 = non-chained
    mxU32 intMode           : 1;    // (10)
    mxU32 transMode         : 1;    // (11)    0 = demand, 1 = block
    mxU32 chanEnable        : 1;    // (12)    0 = disable, 1 = enable
    mxU32 fetNextRec        : 1;    // (13)    1 = force fetch
    mxU32 dmaStat           : 1;    // (14)    0 = inactive, 1 = active (read only)
    mxU32 uu0               : 17;   // (31:15) not used
  } s;

} _DMA_CHAN_CTRL;


typedef union _dmaArbiterControl {

  mxU32     reg;
  struct {
    mxU32   Chan10Pri       : 2;    // (1:0)   00 = round robin, 01 = 1 over 0
                                    //         10 = 0 over 1, 11 = reserved
    mxU32   Chan32Pri       : 2;    // (3:2)   00 = round robin, 01 = 3 over 2
                                    //         10 = 2 over 3, 11 = reserved
    mxU32   GroupPri        : 2;    // (5:4)   00 = round robin, 01 = 2/3 over 0/1
                                    //         10 = 0/1 over 2/3, 11 = reserved
    mxU32   priorityOpt     : 1;    // (6)     0 = relinquish, 1 = hold
    mxU32   uu0             : 25;   // (31:7)  not used
  } s;

} _DMA_ARBITER_CTRL;


typedef struct _timerCounterControl {

  mxU32     reg;
  struct {
    mxU32 tc0Enable         : 1;    // (0)      0 = diable, 1 = enable
    mxU32 tc0Select         : 1;    // (1)      0 = counter, 1 = timer
    mxU32 tc1Enable         : 1;    // (2)      0 = diable, 1 = enable
    mxU32 tc1Select         : 1;    // (3)      0 = counter, 1 = timer
    mxU32 tc2Enable         : 1;    // (4)      0 = diable, 1 = enable
    mxU32 tc2Select         : 1;    // (5)      0 = counter, 1 = timer
    mxU32 tc3Enable         : 1;    // (6)      0 = diable, 1 = enable
    mxU32 tc4Select         : 1;    // (7)      0 = counter, 1 = timer
    mxU32 unused            : 24;   // (31:8)   not used
  } s;

} _TIMER_COUNTER_CTRL;


/* PCI Internal */

typedef union _pciInternalCommand {

  mxU32     reg;
  struct {
    mxU32 byteSwap          : 1;    // (0)      1 = swap PCI data
    mxU32 syncMode          : 2;    // (2:1)    00 = PClk <= 33Mhz,
                                    //          01 = PClk > 0.5TClk
                                    //          1X = synchronized
    mxU32 unused            : 29;   // (31:3)   not used
  } s;

} _PCI_INTERNAL_COMMAND;

typedef union _pciInteralConfigAddr {

  mxU32     reg;
  struct {
    mxU32   uu0             : 2;    // (0:1)    unused
    mxU32   regNum          : 6;    // (2:7)    register number
    mxU32   funcType        : 3;    // (8:10)   function type
    mxU32   devNum          : 5;    // (11:15)  device number
    mxU32   busNum          : 8;    // (16:23)  bus number
    mxU32   uu1             : 7;    // (24:30)  unused
    mxU32   configEna       : 1;    // (31)     1 = generate config cycle on bus
  } s;

} _PCI_INTERNAL_CONFIG_ADDR;


/* PCI Configuration */

typedef union _pciConfigReg0 {

  mxU32   reg;
  struct {
    mxU32   vendorID        : 16;   // (0:15)   GT-64010 = 0x11AB
    mxU32   deviceID        : 16;   // (16:31)  GT-64010 = 0x0146
  } s;

} _PCI_CONFIG_REG0;


typedef union _pciConfigReg1 {

  mxU32     reg;
  struct {
    mxU32   enIO            : 1;    // (0)      0 = disable, 1 = enable
    mxU32   enMemory        : 1;    // (1)      0 = disable, 1 = enable
    mxU32   enMaster        : 1;    // (2)      0 = disable, 1 = enable
    mxU32   unused1         : 1;    // (3)      not used
    mxU32   enMemWRInv      : 1;    // (4)      0 = disable, 1 = enable
    mxU32   unused2         : 1;    // (5)      not used
    mxU32   enPErrPin       : 1;    // (6)      0 = disable, 1 = enable
    mxU32   unused3         : 1;    // (7)      not used
    mxU32   enSErrPin       : 1;    // (8)      0 = disable, 1 = enable
    mxU32   unused4         : 14;   // (22:9)   not used
    mxU32   tarFastBB       : 1;    // (23)     (read only) fast back-to-back PCI = 1
    mxU32   dataParErr      : 1;    // (24)     data parity error = 1
    mxU32   devSelTime      : 3;    // (27:25)  (read only) dev sel timing
    mxU32   targetAbort     : 1;    // (28)     1 = target abort
    mxU32   masterAbort     : 1;    // (29)     1 = master abort
    mxU32   systemErr       : 1;    // (30)     1 = system error
    mxU32   parityErr       : 1;    // (31)     1 = parity error (master or slave)
  } s;

} _PCI_CONFIG_REG1;


typedef union _pciConfigReg2 {

  mxU32     reg;
  struct {
    mxU32   revision        : 8;    // (7:0)    GT-64010 revision number
    mxU32   unused          : 8;    // (15:8)   not used
    mxU32   subClass        : 8;    // (23:16)  Subclass - 0 = host bridge
    mxU32   baseClass       : 8;    // (31:24)  Base Class - 6 = bridge device
  } s;

} _PCI_CONFIG_REG2;


typedef union _pciConfigReg3 {

  mxU32     reg;
  struct {
    mxU32 cacheLine         : 8;    // (7:0)    cache line size (= 8)
    mxU32 latTimer          : 8;    // (15:8)   latency timer
    mxU32 headType          : 8;    // (23:16)  byte layout (bytes 10H - 3FH)
    mxU32 bist              : 8;    // (31:24)  Built-In-Self-Test
  } s;

} _PCI_CONFIG_REG3;


typedef union _pciConfigReg15 {

  mxU32     reg;
  struct {
    mxU32 maxLat            : 8;    // (7:0)    PCI access repeat time
    mxU32 minGnt            : 8;    // (15:8)   burst period @ 33MHz
    mxU32 interruptPin      : 8;    // (23:16)  which pin is used
    mxU32 interruptLine     : 8;    // (31:24)  system int. controller input
  } s;

} _PCI_CONFIG_REG15;


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                            Function Prototypes                             ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
extern void     initGT64010(void);
extern int      getGT64010Rev(void);
extern mxU32    get_pci_config_reg(mxU32 devNum, mxU32 regNum);
extern void     put_pci_config_reg(mxU32 devNum, mxU32 regNum, mxU32 data);
extern void     get_pci_slot_configuration_info(mxU32 *to, mxU32 devNum);
extern void     disable_dma(mxU32 channel);

#endif  /* CLANGUAGE */

#endif
