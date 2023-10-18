
#if !defined(_CONFIG_H_)
#define _CONFIG_H_
#if !defined(_CONSTANTS_H_)
#define _CONSTANTS_H_
#define M68000 (0x1)
#define M68010 (0x2)
#define M68EC020 (0x3)
#define M68020 (0x3)
#define ASAP (0x10)
#define MIPS3000 (0x20)	/* MIPS 30x1 processor, big endian */
#define MIPS30x1 (0x21)	/* MIPS 30x1 processor, big endian */
#define MIPS30x1L (0x22)	/* MIPS 30x1 processor, little endian */
#define MIPS4000 (0x30)	/* MIPS 4000 processor, big endian */
#define MIPS4000L (0x31)	/* MIPS 4000 processor, little endian */
#define MIPS4600 (0x32)	/* MIPS 4600 processor, big endian */
#define MIPS4600L (0x33)	/* MIPS 4600 processor, little endian */
#define MIPS4650 (0x34)	/* MIPS 4650 processor, big endian */
#define MIPS4650L (0x35)	/* MIPS 4650 processor, little endian */
#define MIPS4700 (0x36)	/* MIPS 4700 processor, big endian */
#define MIPS4700L (0x37)	/* MIPS 4700 processor, little endian */
#define MIPS5000 (0x38)	/* MIPS 5000 processor, big endian */
#define MIPS5000L (0x39)	/* MIPS 5000 processor, little endian */
#define EC020cojag (0x1)	/* McKee's low cost EC020 host board, CoJag version */
#define EC020zoid10 (0x2)	/* McKee's low cost EC020 host board, Zoid 10 version */
#define EC020zoid20 (0x3)	/* McKee's low cost EC020 host board, Zoid 20 version */
#define MCUBE (0x4)	/* Mokris's 68k based host board */
#define ASCLEAP (0x5)	/* The ASAP based ASCLEAP */
#define LCR3K (0x6)	/* Low cost R3k host board */
#define IDT3xEVAL (0x7)	/* IDT's 3000 eval board with XBUS adapter */
#define IDT4xEVAL (0x8)	/* IDT's 4000 eval board with XBUS adapter */
#define MB4600 (0x9)	/* Senthil and Mark's 4600 MathBox board */
#define HCR4K (0x0A)	/* Senthil and Mark's 4600 Host board */
#define PHOENIX (0x0B)	/* WMS Host board */
#define ZOID10_V (0x1)	/* Zoid 10 stack		*/
#define ZOID20_V (0x2)	/* Zoid 20 stack 	*/
#define COJAG_V (0x3)	/* CoJag stack 		*/
#define GX1_V (0x4)	/* FSG42 board		*/
#define GX2_V (0x5)	/* GX2 board		*/
#define GT_V (0x6)	/* GT board		*/
#define SST_V (0x7)	/* 3DFX video board	*/
#define COJAG_PROTO (0x1)	/* Non-Game specific running on COJAG */
#define COJAG_HERO (0x2)	/* Hero on COJAG */
#define COJAG_AREA51 (0x4)	/* Area 51 on COJAG */
#define COJAG_RAGE (0x8)	/* RAGE 2 on COJAG */
#define COJAG_FISH (0x10)	/* Tropical Fish on COJAG */
#define ZOID_PROTO (0x1)	/* Non-Game specific running on ZOID */
#define ZOID_HOCKEY (0x2)	/* Wayne Gretzky Hocky running on ZOID */
#define ZOID_GAUNTLET (0x4)	/* 3D Gauntlet running on ZOID */
#define ZOID20_DIAG (0x8)	/* Mike Albaugh developing ZOID 20 */
#define ZOID_RUSH (0x10)	/* SF Rush running on ZOID */
#define ZOID_MACE (0x20)	/* Mace running on ZOID */
#define ZOID20_DMS (0x40)	/* Dave Shepperd test ZOID */
#define SST_PROTO (0x1)	/* Non-Game specific running on 3DFX */
#define SST_DMS (0x2)	/* Dave Shepperd test 3DFX */
#define SST_RUSH (0x4)	/* RUSH 3DFX */
#define SST_HOCKEY (0x8)	/* Hockey 3DFX */
#define SST_MACE (0x10)	/* Mace 3DFX */
#define TRUE (0x1)
#define FALSE (0x0)
#define ABORT (0x0)
#define FAIL (0x0FFFFFFFF)
#define IDE_COJAG (0x1)	/* For IDE on COJAG boards */
#define IDE_STREAM (0x2)	/* For IDE on STREAM boards */
#define IDE_PCI (0x3)	/* For IDE on PCI bus */
#define ANSI_OK (0x1)
#endif			/* _CONSTANTS_H_ */
#define PROCESSOR (0x32)
#if !defined(_PPTYPES_H_)
#define _PPTYPES_H_
#define NO_LONGLONG (0x0)
#ifndef __VS32_TYPE_DEFINED
#define __VS32_TYPE_DEFINED
typedef volatile long VS32;
#endif /* __VS32_TYPE_DEFINED */
#ifndef __VS16_TYPE_DEFINED
#define __VS16_TYPE_DEFINED
typedef volatile short VS16;
#endif /* __VS16_TYPE_DEFINED */
#ifndef __VS8_TYPE_DEFINED
#define __VS8_TYPE_DEFINED
typedef volatile char VS8;
#endif /* __VS8_TYPE_DEFINED */
#ifndef __VS08_TYPE_DEFINED
#define __VS08_TYPE_DEFINED
typedef volatile char VS08;
#endif /* __VS08_TYPE_DEFINED */
#ifndef __VU32_TYPE_DEFINED
#define __VU32_TYPE_DEFINED
typedef volatile unsigned long VU32;
#endif /* __VU32_TYPE_DEFINED */
#ifndef __VU16_TYPE_DEFINED
#define __VU16_TYPE_DEFINED
typedef volatile unsigned short VU16;
#endif /* __VU16_TYPE_DEFINED */
#ifndef __VU8_TYPE_DEFINED
#define __VU8_TYPE_DEFINED
typedef volatile unsigned char VU8;
#endif /* __VU8_TYPE_DEFINED */
#ifndef __VU08_TYPE_DEFINED
#define __VU08_TYPE_DEFINED
typedef volatile unsigned char VU08;
#endif /* __VU08_TYPE_DEFINED */
#ifndef __VF32_TYPE_DEFINED
#define __VF32_TYPE_DEFINED
typedef volatile float VF32;
#endif /* __VF32_TYPE_DEFINED */
#ifndef __VF64_TYPE_DEFINED
#define __VF64_TYPE_DEFINED
typedef volatile double VF64;
#endif /* __VF64_TYPE_DEFINED */
#ifndef __m_int_TYPE_DEFINED
#define __m_int_TYPE_DEFINED
typedef int m_int;
#endif /* __m_int_TYPE_DEFINED */
#ifndef __m_uint_TYPE_DEFINED
#define __m_uint_TYPE_DEFINED
typedef unsigned int m_uint;
#endif /* __m_uint_TYPE_DEFINED */
#ifndef __U8_TYPE_DEFINED
#define __U8_TYPE_DEFINED
typedef unsigned char U8;
#endif /* __U8_TYPE_DEFINED */
#ifndef __U08_TYPE_DEFINED
#define __U08_TYPE_DEFINED
typedef unsigned char U08;
#endif /* __U08_TYPE_DEFINED */
#ifndef __S8_TYPE_DEFINED
#define __S8_TYPE_DEFINED
typedef signed char S8;
#endif /* __S8_TYPE_DEFINED */
#ifndef __S08_TYPE_DEFINED
#define __S08_TYPE_DEFINED
typedef signed char S08;
#endif /* __S08_TYPE_DEFINED */
#ifndef __U16_TYPE_DEFINED
#define __U16_TYPE_DEFINED
typedef unsigned short U16;
#endif /* __U16_TYPE_DEFINED */
#ifndef __S16_TYPE_DEFINED
#define __S16_TYPE_DEFINED
typedef short S16;
#endif /* __S16_TYPE_DEFINED */
#ifndef __U32_TYPE_DEFINED
#define __U32_TYPE_DEFINED
typedef unsigned long U32;
#endif /* __U32_TYPE_DEFINED */
#ifndef __S32_TYPE_DEFINED
#define __S32_TYPE_DEFINED
typedef long S32;
#endif /* __S32_TYPE_DEFINED */
#ifndef __F32_TYPE_DEFINED
#define __F32_TYPE_DEFINED
typedef float F32;
#endif /* __F32_TYPE_DEFINED */
#ifndef __F64_TYPE_DEFINED
#define __F64_TYPE_DEFINED
typedef double F64;
#endif /* __F64_TYPE_DEFINED */
#ifndef __RD_TYP_TYPE_DEFINED
#define __RD_TYP_TYPE_DEFINED
typedef struct rdb RD_TYP;
#endif /* __RD_TYP_TYPE_DEFINED */
#ifndef __RR_TYP_TYPE_DEFINED
#define __RR_TYP_TYPE_DEFINED
typedef struct rrb RR_TYP;
#endif /* __RR_TYP_TYPE_DEFINED */
#ifndef __MN_TYP_TYPE_DEFINED
#define __MN_TYP_TYPE_DEFINED
typedef struct menub MN_TYP;
#endif /* __MN_TYP_TYPE_DEFINED */
#ifndef __PB_TYP_TYPE_DEFINED
#define __PB_TYP_TYPE_DEFINED
typedef struct pconfigp PB_TYP;
#endif /* __PB_TYP_TYPE_DEFINED */
#ifndef __CR_TYP_TYPE_DEFINED
#define __CR_TYP_TYPE_DEFINED
typedef struct creditsb CR_TYP;
#endif /* __CR_TYP_TYPE_DEFINED */
struct menu_d {
	char	*mn_label;		    /* menu item label		*/
	int	(*mn_call)(const struct menu_d*); /* menu item routine call	*/
};
struct menub {
	char		*mn_label;	/* menu item label		*/
	void		(*mn_call)();	/* menu item routine call	*/
};
struct creditsb {
	unsigned short	crd_whole;	/* Integer part of coins	*/
	unsigned char	crd_num;	/* numerator			*/
	unsigned char	crd_denom;	/* denominator			*/
};
struct rdb {
unsigned long * rd_base;	/*  Starting address  */
unsigned long rd_len;	/*  Length in bytes  */
unsigned long rd_misc;	/*  Which bits exist */
};
#define PM_TEXT_SIZE (80)	/*Up to 80 bytes of postmortem text*/
struct pm_general {
const char * pm_msg;	/* Pointer to message */
char pm_text[PM_TEXT_SIZE];	/* Local copy of text message */
U32* pm_stack;	/* Stack pointer in target's address space */
U32* pm_stkupper;	/* Stack upper limit in target's address space */
U32* pm_stklower;	/* Stack lower limit in target's address space */
U32* pm_stkrelative;	/* Stack pointer in host's address space */
S32 pm_cntr;	/* Post mortem flag */
U32 pm_pc;	/* Program counter */
U32 pm_sr;	/* Status register */
U32 pm_regs[32];	/* ASAP/R3K/R4K have 32. 68k only uses 16 of these */
U32 pm_cause;	/* R3K/R4K cause register */
U32 pm_badvaddr;	/* R3K/R4K bad virtual address register */
};
# define _PM_GENERAL_STRUCT_	0	/* Disable the definition in st_proto.h */
# define PM_68K_SIZE	(sizeof(struct pm_general)-(18*4))
# define PM_RxK_SIZE	(sizeof(struct pm_general))
struct rrb {
unsigned long * rr_addr;	/*  Where it choked  */
unsigned long rr_expected;	/*  What it wanted  */
unsigned long rr_got;	/*  What it got */
int rr_test_no;	/*  Which test  */
};
#define B_NO_ROM_TEST (0x0)	/* bit # in p_debug_options to skip ROM checksum	*/
#define NO_ROM_TEST (0x1)
#define B_NO_RAM_TEST (0x1)	/* bit # in p_debug_options to skip RAM test	*/
#define NO_RAM_TEST (0x2)
#define B_NO_LOG_RESET (0x2)	/* bit # in p_debug_options to skip logging RESET*/
#define NO_LOG_RESET (0x4)
#endif			/* _PPTYPES_H_ */
#define HOST_BOARD (0x0B)
#define VIDEO_BOARD (0x7)
#define VIS_H_PIX (512)
#define VIS_V_PIX (400)
#define SST_RESOLUTION			GR_RESOLUTION_512x400
#define SST_REFRESH_RATE		GR_REFRESH_60Hz
#define SST_COLOR_FORMAT		GR_COLORFORMAT_ARGB
#define SST_ORIGIN			GR_ORIGIN_LOWER_LEFT
#define NO_ANTIALIAS_MODE		1
#define USE_ISPRINTF (1)	/* Use isprintf instead of nisprintf */
#define MENU_X_DEFAULT (-1)	/* Default X position for menu items */
#define INCLUDE_FEXCP (1)	/* Include floating point exception handler */
#define ICELESS_IO (0)
#define HAS_CAGE (0x1)	/* has a CAGE sound bd	*/
#define HAS_4MSINT (0x1)	/* 4 millisecond interrupt */
#define COIN_DOORS (0x0)	/* # of coin doors(2 mechs per)	*/
#define XBUSMON_BASE (0x0B4400000)	/* XBUS monitor board lives here */
#define CPU_SPEED (100000000)	/* CPU clock speed in Hertz */
#define BUS_SPEED (50000000)	/* Bus (Galileo) clock speed in Hertz */
#define FAKE_XBUSMON_COINS (1)	/* Fake coin switches */
#define SST_GAME (8)	/* Hockey game */
#define ICELESS_LVL (1)
#define ICELESS_MANY (15)	/* Bit mask for allowable interrupts */
#define DEBUG		1
#define HANDSHAKE	(J_UP)
#ifndef HDW_INIT
# define HDW_INIT(x) do { \
	extern void prc_init_vecs(void), hdw_init(int); \
	prc_init_vecs(); \
	hdw_init(x);\
} while (0)
#endif
#define IDE_TYPE (3)	/* PCI I/O */
#define NUCLEUS_OS (0x0)
#define WORD0_OFFS (0x0)
#define WORD32_OFFS (0x4)
#define SHORT0_OFFS (0x0)
#define SHORT16_OFFS (0x2)
#define BYTE0_OFFS (0x0)
#define BYTE8_OFFS (0x1)
#define BYTE16_OFFS (0x2)
#define BYTE24_OFFS (0x3)
#define HWORD0_OFFS (0x0)
#define HWORD32_OFFS (0x4)
#define HSHORT0_OFFS (0x0)
#define HSHORT16_OFFS (0x2)
#define HSHORT32_OFFS (0x4)
#define HSHORT48_OFFS (0x6)
#define HBYTE0_OFFS (0x0)
#define HBYTE8_OFFS (0x1)
#define HBYTE16_OFFS (0x2)
#define HBYTE24_OFFS (0x3)
#define HBYTE32_OFFS (0x4)
#define HBYTE40_OFFS (0x5)
#define HBYTE48_OFFS (0x6)
#define HBYTE56_OFFS (0x7)
#define MEM_KUSEG (0x0)	/* -0x7FFFFFFF (  2GB, mapped, cached)	*/
#define MEM_KSEG0 (0x80000000)	/* -0x9FFFFFFF phys 00000000-1FFFFFFF (512MB, unmapped, cached)	*/
#define MEM_KSEG1 (0x0A0000000)	/* -0xBFFFFFFF phys 00000000-1FFFFFFF (512MB, unmapped, uncached)	*/
#define MEM_KSSEG (0x0C0000000)	/* -0xDFFFFFFF (512MB, mapped)		*/
#define MEM_KSEG3 (0x0E0000000)	/* -0xFFFFFFFF (512MB, mapped)		*/
#define DRAM_BASE (0x80000000)	/* DRAM phys 0x00000000-0x07FFFFFF */
#define DRAM_BASEnc (0x0A0000000)	/* DRAM phys 0x00000000-0x07FFFFFF (uncached) */
#define DRAM_SIZE (0x800000)	/* DRAM size (8Mb) */
#define PCI_MEM_BASE (0x0A8000000)	/* -0x29FFFFFF PCI Memory bus (uncached) */
#define PCI_IO_BASE (0x0AA000000)	/* -0x2BFFFFFF PCI I/O bus (uncached) */
#define GALILEO_BOOT_BASE (0x0B4000000)	/* -0xB40FFFFF GALILEO registers right after reset */
#define GALILEO_BASE (0x0AC000000)	/* -0x2C0FFFFF GALILEO registers (uncached) */
#define EXPAN0_BASE (0x0B0000000)	/* -0x31FFFFFF Expansion connector 0 (PCS0, uncached) */
#define EXPAN1_BASE (0x0B2000000)	/* -0x33FFFFFF Expansion connector 1 (PCS1, uncached) */
#define EXPAN2_BASE (0x0B4000000)	/* -0x34FFFFFF Expansion connector 2 (PCS2, uncached) */
#define OBIO_BASE (0x0B5000000)	/* -0x35FFFFFF On board I/O base (uncached) */
#define EXPAN3_BASE (0x0B8000000)	/* -0x3FBFFFFF Expansion connector 3 (PCS3, uncached) */
#define PROM_BASEnc (0x0BFC00000)	/* -0xBFFFFFFF PROM phys (uncached) */
#define PROM_BASE (0x9FC00000)	/* -0x9FFFFFFF PROM phys (cached) */
#define CACHE_MEM_BASE (0x81000000)
#define IOASIC_BASE (0x0B5000000)
#define IO_DIPSW (0x0B5000000)	/* bits 15:0 (ro) = dip switches */
#define DIPSW0 (0x1)	/* dip switch 0 */
#define DIPSW1 (0x2)	/* dip switch 1 */
#define DIPSW2 (0x4)	/* dip switch 2 */
#define DIPSW3 (0x8)	/* dip switch 3 */
#define DIPSW4 (0x10)	/* dip switch 4 */
#define DIPSW5 (0x20)	/* dip switch 5 */
#define DIPSW6 (0x40)	/* dip switch 6 */
#define DIPSW7 (0x80)	/* dip switch 7 */
#define DIPSW8 (0x100)	/* dip switch 8 */
#define DIPSW9 (0x200)	/* dip switch 9 */
#define DIPSW10 (0x400)	/* dip switch 10 */
#define DIPSW11 (0x800)	/* dip switch 11 */
#define DIPSW12 (0x1000)	/* dip switch 12 */
#define DIPSW13 (0x2000)	/* dip switch 13 */
#define DIPSW14 (0x4000)	/* dip switch 14 */
#define DIPSW15 (0x8000)	/* dip switch 15 */
#define IO_MISC (0x0B5000008)	/* bits 15:0 (ro) = misc inputs */
#define MI0 (0x1)	/* miscellaneos input 0 */
#define MI1 (0x2)	/* miscellaneos input 1 */
#define MI2 (0x4)	/* miscellaneos input 2 */
#define MI3 (0x8)	/* miscellaneos input 3 */
#define MI4 (0x10)	/* miscellaneos input 4 */
#define MI5 (0x20)	/* miscellaneos input 5 */
#define MI6 (0x40)	/* miscellaneos input 6 */
#define MI7 (0x80)	/* miscellaneos input 7 */
#define MI8 (0x100)	/* miscellaneos input 8 */
#define MI9 (0x200)	/* miscellaneos input 9 */
#define MI10 (0x400)	/* miscellaneos input 10 */
#define MI11 (0x800)	/* miscellaneos input 11 */
#define MI12 (0x1000)	/* miscellaneos input 12 */
#define MI13 (0x2000)	/* miscellaneos input 13 */
#define MI14 (0x4000)	/* miscellaneos input 14 */
#define MI15 (0x8000)	/* miscellaneos input 15 */
#define IO_PLAYER_21 (0x0B5000010)	/* bits 15:0 (ro) = player 1 and player 2 inputs */
#define P1_0 (0x1)	/* player 1 switch 0 */
#define P1_1 (0x2)	/* player 1 switch 1 */
#define P1_2 (0x4)	/* player 1 switch 2 */
#define P1_3 (0x8)	/* player 1 switch 3 */
#define P1_4 (0x10)	/* player 1 switch 4 */
#define P1_5 (0x20)	/* player 1 switch 5 */
#define P1_6 (0x40)	/* player 1 switch 6 */
#define P1_7 (0x80)	/* player 1 switch 7 */
#define P2_0 (0x100)	/* player 2 switch 0 */
#define P2_1 (0x200)	/* player 2 switch 1 */
#define P2_2 (0x400)	/* player 2 switch 2 */
#define P2_3 (0x800)	/* player 2 switch 3 */
#define P2_4 (0x1000)	/* player 2 switch 4 */
#define P2_5 (0x2000)	/* player 2 switch 5 */
#define P2_6 (0x4000)	/* player 2 switch 6 */
#define P2_7 (0x8000)	/* player 2 switch 7 */
#define IO_PLAYER_43 (0x0B5000018)	/* bits 15:0 (ro) = player 3 and player 4 inputs */
#define P3_0 (0x1)	/* player 3 switch 0 */
#define P3_1 (0x2)	/* player 3 switch 1 */
#define P3_2 (0x4)	/* player 3 switch 2 */
#define P3_3 (0x8)	/* player 3 switch 3 */
#define P3_4 (0x10)	/* player 3 switch 4 */
#define P3_5 (0x20)	/* player 3 switch 5 */
#define P3_6 (0x40)	/* player 3 switch 6 */
#define P3_7 (0x80)	/* player 3 switch 7 */
#define P4_0 (0x100)	/* player 4 switch 0 */
#define P4_1 (0x200)	/* player 4 switch 1 */
#define P4_2 (0x400)	/* player 4 switch 2 */
#define P4_3 (0x800)	/* player 4 switch 3 */
#define P4_4 (0x1000)	/* player 4 switch 4 */
#define P4_5 (0x2000)	/* player 4 switch 5 */
#define P4_6 (0x4000)	/* player 4 switch 6 */
#define P4_7 (0x8000)	/* player 4 switch 7 */
#define IO_UART_CTL (0x0B5000020)	/* bits 15:0 (rw) = UART control bits */
#define IO_UART_TX (0x0B5000028)	/* bits  7:0 (rw) = UART transmit register */
#define IO_UART_RCV (0x0B5000030)	/* bits 15:0 (ro) = UART receive/status registers */
#define IO_METER (0x0B5000038)	/* bits  4:0 (rw) = meter control register */
#define IO_H2SND_CTL (0x0B5000040)	/* bits 15:0 (rw) = Host to Sound control register */
#define IO_H2SND_DTA (0x0B5000048)	/* bits 15:0 (rw) = Host to Sound data register */
#define IO_SND_STS (0x0B5000050)	/* bits 15:0 (ro) = Host Sound status */
#define IO_SND2H_DTA (0x0B5000058)	/* bits 15:0 (ro) = Sound to Host data register */
#define IO_H2MIC_CMD (0x0B5000060)	/* bits  3:0 (rw) = Host to microcontroller control */
#define IO_MIC2H_DTA (0x0B5000068)	/* bits  7:0 (ro) = MIC to host data register */
#define IO_MAIN_STS (0x0B5000070)	/* bits 15:0 (ro) = Main status register */
#define IO_MAIN_CTL (0x0B5000078)	/* bits 15:0 (rw) = Main control register */
#define INTCTL_BASE (0x0B5080000)
#define INTCTL_IE (0x0B5080000)	/* bits 15:0 (rw) = Interrupt enable register */
#define INTCTL_MAPA (0x0B5080008)	/* bits 15:0 (rw) = Interrupt Map register A */
#define INTCTL_MAPB (0x0B5080010)	/* bits 15:0 (rw) = Interrupt Map register B */
#define INTCTL_CAUSE (0x0B5080018)	/* bits 15:0 (ro) = Interrupt cause register */
#define INTCTL_STS (0x0B5080020)	/* bits 15:0 (ro) = Interrupt polled status */
#define INTCTL_GPSTS (0x0B5080028)	/* bits 15:0 (rw) = Interrupt GP register */
#define INTCTL_NMI (0x0B5080030)	/* bits  3:0 (rw) = NMI select/enable register */
#define MISCPLD_BASE (0x0B5100000)
#define MISC_HCTL (0x0B5100000)	/* bits 15:0 (ro) = Host control register */
#define MISC_HCMD (0x0B5100004)	/* bits 15:0 (ro) = Host command register */
#define MISC_HWFIFO (0x0B5100008)	/* bits 15:0 (ro) = Host write data FIFO */
#define MISC_HRFIFO (0x0B510000C)	/* bits 15:0 (wo) = Host read data FIFO */
#define MISC_TCTL (0x0B5100010)	/* bits 15:0 (rw) = Target control register */
#define MISC_TRESP (0x0B5100014)	/* bits 15:0 (rw) = Target response register */
#define MISC_TSTS (0x0B5100018)	/* bits 15:0 (ro) = Target status register */
#define MISC_TTESTP (0x0B510001C)	/* bits 15:0 (rw) = Target test point register */
#define NSS_CTL (0x0B5100020)	/* bits 15:0 (rw) = NSS control register */
#define NSS_GAME_CTL (0x0B5100024)	/* bits 15:0 (ro) = NSS game control register */
#define NSS_FIFO_STS (0x0B5100028)	/* bits 15:0 (ro) = NSS FIFO status */
#define NSS_GAMEINT (0x0B510002C)	/* bits 15:0 (ro) = NSS Game interrupt status */
#define NSS_FIFO_DTA (0x0B5100030)	/* bits 15:0 (rw) = NSS read/write FIFO */
#define NSS_FIFO_RST (0x0B5100034)	/* bits 15:0 (wo) = NSS FIFO reset register */
#define MISC_STATUS (0x0B5100038)	/* bits 15:0 (ro) = Misc status register */
#define MISC_CONFIG (0x0B510003C)	/* bits 15:0 (rw) = Misc configuration reg */
#define MISC_CONFIG_NSS_RESET (0x10)	/* NSS reset bit */
#define WATCHDOG (0x0B5180000)	/* bits  x:x (wo) = Watch dog timer */
#define COMBO_UART0 (0x0B5200000)	/* bits  7:0 (rw) = 16552 UART channel 0 */
#define COMBO_UART1 (0x0B5280000)	/* bits  7:0 (rw) = 16552 UART channel 1 */
#define COMBO_PARA (0x0B5300000)	/* bits  7:0 (rw) = 16552 UART parallel port */
#define A_TO_D (0x0B5380000)	/* bits  7:0 (rw) = A/D converter */
#define SND_STREAM (0x0B5400000)	/* bits 15:0 (wo) = Sound system FIFO */
#define BRAM_BASE (0x0B5480000)	/* bits  7:0 (rw) = NVRAM bytes */
#define BRAM_SIZE (0x20000)	/* BRAM size (32k words)					*/
#define BRAM_UNLK (0x0B5500000)	/* bits  x:x (wo) = BRAM unlock */
#define IO_RESET (0x0B5800000)	/* bits  0:0 (rw) = IOASIC reset bit */
#define PCI_RESET (0x0B5880000)	/* bits  0:0 (rw) = PCI bus reset */
#define RAMROM_BASE (0x0B4000000)	/* 8MB of RamRom starts here */
#define GALILEO_CPUINTFC (0x0AC000000)	/*CPU Interface configuration*/
#define GALILEO_CPUINTFC_CacheOpMap_b (0x0)
#define GALILEO_CPUINTFC_CachePres_b (0x9)
#define GALILEO_CPUINTFC_WriteMod_b (0x0B)
#define GALILEO_CPUINTFC_Endian_b (0x0C)
#define GALILEO_CPUINTFC_CacheOpMap_m (0x1)
#define GALILEO_CPUINTFC_CachePres_m (0x200)
#define GALILEO_CPUINTFC_WriteMod_m (0x800)
#define GALILEO_CPUINTFC_Endian_m (0x1000)
#define GALILEO_RAS10_LOW (0x0AC000008)	/* RAS[1:0] Low decode Address */
#define GALILEO_RAS10_HIGH (0x0AC000010)	/* RAS[1:0] High decode Address */
#define GALILEO_RAS32_LOW (0x0AC000018)	/* RAS[3:2] Low decode Address */
#define GALILEO_RAS32_HIGH (0x0AC000020)	/* RAS[3:2] High decode Address */
#define GALILEO_CS20_LOW (0x0AC000028)	/* CS[2:0] Low decode Address */
#define GALILEO_CS20_HIGH (0x0AC000030)	/* CS[2:0] High decode Address */
#define GALILEO_CS3BOOT_LOW (0x0AC000038)	/* CS[3] & Boot Low decode Address */
#define GALILEO_CS3BOOT_HIGH (0x0AC000040)	/* CS[3] & Boot High decode Address */
#define GALILEO_PCIIO_LOW (0x0AC000048)	/* PCI I/O Low decode Address */
#define GALILEO_PCIIO_HIGH (0x0AC000050)	/* PCI I/O High decode Address */
#define GALILEO_PCIMEM_LOW (0x0AC000058)	/* PCI Memory Low decode Address */
#define GALILEO_PCIMEM_HIGH (0x0AC000060)	/* PCI Memory High decode Address */
#define GALILEO_Internal (0x0AC000068)	/* Galileo internal space decode Address */
#define GALILEO_BUSERR_LOW (0x0AC000070)	/* Address bits 31:0 after Bus Error */
#define GALILEO_BUSERR_HIGH (0x0AC000078)	/* Address bits 35:32 after Bus Error */
#define GALILEO_RAS0_LOW (0x0AC000400)	/* RAS[0] Low decode Address */
#define GALILEO_RAS0_HIGH (0x0AC000404)	/* RAS[0] High decode Address */
#define GALILEO_RAS1_LOW (0x0AC000408)	/* RAS[1] Low decode Address */
#define GALILEO_RAS1_HIGH (0x0AC00040C)	/* RAS[1] High decode Address */
#define GALILEO_RAS2_LOW (0x0AC000410)	/* RAS[2] Low decode Address */
#define GALILEO_RAS2_HIGH (0x0AC000414)	/* RAS[2] High decode Address */
#define GALILEO_RAS3_LOW (0x0AC000418)	/* RAS[3] Low decode Address */
#define GALILEO_RAS3_HIGH (0x0AC00041C)	/* RAS[3] High decode Address */
#define GALILEO_CS0_LOW (0x0AC000420)	/* CS[0] Low decode Address */
#define GALILEO_CS0_HIGH (0x0AC000424)	/* CS[0] High decode Address */
#define GALILEO_CS1_LOW (0x0AC000428)	/* CS[1] Low decode Address */
#define GALILEO_CS1_HIGH (0x0AC00042C)	/* CS[1] High decode Address */
#define GALILEO_CS2_LOW (0x0AC000430)	/* CS[2] Low decode Address */
#define GALILEO_CS2_HIGH (0x0AC000434)	/* CS[2] High decode Address */
#define GALILEO_CS3_LOW (0x0AC000438)	/* CS[3] Low decode Address */
#define GALILEO_CS3_HIGH (0x0AC00043C)	/* CS[3] High decode Address */
#define GALILEO_BOOT_LOW (0x0AC000440)	/* CS[3] Low decode Address */
#define GALILEO_BOOT_HIGH (0x0AC000444)	/* CS[3] High decode Address */
#define GALILEO_DRAM_CFG (0x0AC000448)	/* DRAM configuration */
#define GALILEO_DRAM_CFG_RefIntCnt_b (0x0)	/* Refresh interval count */
#define GALILEO_DRAM_CFG_StagRef_b (0x10)	/* Staggered Refresh */
#define GALILEO_DRAM_CFG_ADSFunct_b (0x11)	/* ADS pin function */
#define GALILEO_DRAM_CFG_DRAMLatch_b (0x12)	/* DRAM latch operation */
#define GALILEO_DRAM_CFG_RefIntCnt_m (0x1)	/* Refresh interval count */
#define GALILEO_DRAM_CFG_StagRef_m (0x10000)	/* Staggered Refresh */
#define GALILEO_DRAM_CFG_ADSFunct_m (0x20000)	/* ADS pin function */
#define GALILEO_DRAM_CFG_DRAMLatch_m (0x40000)	/* DRAM latch operation */
#define GALILEO_DRAM0_CFG (0x0AC00044C)	/* DRAM Bank 0 configuration */
#define GALILEO_DRAMBK_CFG_CASWr_b (0x0)	/* Number of CAS cycles during writes */
#define GALILEO_DRAMBK_CFG_RAStoCASWr_b (0x1)	/* Number of cycles between RAS and CAS during writes */
#define GALILEO_DRAMBK_CFG_CASRd_b (0x2)	/* Number of CAS cycles during read */
#define GALILEO_DRAMBK_CFG_RAStoCASRd_b (0x3)	/* Number of cycles between RAS and CAS during reads */
#define GALILEO_DRAMBK_CFG_Refresh_b (0x4)	/* Refresh type */
#define GALILEO_DRAMBK_CFG_BankWidth_b (0x6)	/* Width (32/64) */
#define GALILEO_DRAMBK_CFG_BankLoc_b (0x7)	/* Even/Odd side if width = 32 */
#define GALILEO_DRAMBK_CFG_Parity_b (0x8)	/* Parity support */
#define GALILEO_DRAMBK_CFG_CASWr_m (0x1)	/* Number of CAS cycles during writes */
#define GALILEO_DRAMBK_CFG_RAStoCASWr_m (0x2)	/* Number of cycles between RAS and CAS during writes */
#define GALILEO_DRAMBK_CFG_CASRd_m (0x4)	/* Number of CAS cycles during read */
#define GALILEO_DRAMBK_CFG_RAStoCASRd_m (0x8)	/* Number of cycles between RAS and CAS during reads */
#define GALILEO_DRAMBK_CFG_Refresh_m (0x10)	/* Refresh type */
#define GALILEO_DRAMBK_CFG_BankWidth_m (0x40)	/* Width (32/64) */
#define GALILEO_DRAMBK_CFG_BankLoc_m (0x80)	/* Even/Odd side if width = 32 */
#define GALILEO_DRAMBK_CFG_Parity_m (0x100)	/* Parity support */
#define GALILEO_DRAM1_CFG (0x0AC000450)	/* DRAM Bank 1 configuration */
#define GALILEO_DRAM2_CFG (0x0AC000454)	/* DRAM Bank 2 configuration */
#define GALILEO_DRAM3_CFG (0x0AC000458)	/* DRAM Bank 3 configuration */
#define GALILEO_DEV0_CFG (0x0AC00045C)	/* Device Bank 0 (PCS0) Configuration */
#define GALILEO_DEV_CFG_TurnOff_b (0x0)	/* Cycles between OE's */
#define GALILEO_DEV_CFG_AccToFirst_b (0x3)	/* Cycles between CS's */
#define GALILEO_DEV_CFG_AccToNext_b (0x7)	/* Cycles between CS's */
#define GALILEO_DEV_CFG_ADStoWr_b (0x0B)	/* Cycles between ADS and WR */
#define GALILEO_DEV_CFG_WrActive_b (0x0E)	/* Cycles Wr held assetred */
#define GALILEO_DEV_CFG_WrHigh_b (0x11)	/* Cycles between assertions of Wr */
#define GALILEO_DEV_CFG_DevWidth_b (0x14)	/* Device bus width */
#define GALILEO_DEV_CFG_DevLoc_b (0x17)	/* Even/Odd side if width != 64*/
#define GALILEO_DEV_CFG_LatchFunct_b (0x19)	/* Latch enable function */
#define GALILEO_DEV_CFG_Parity_b (0x1E)	/* Parity support */
#define GALILEO_DEV_CFG_TurnOff_m (0x1)	/* Cycles between OE's */
#define GALILEO_DEV_CFG_AccToFirst_m (0x8)	/* Cycles between CS's */
#define GALILEO_DEV_CFG_AccToNext_m (0x80)	/* Cycles between CS's */
#define GALILEO_DEV_CFG_ADStoWr_m (0x800)	/* Cycles between ADS and WR */
#define GALILEO_DEV_CFG_WrActive_m (0x4000)	/* Cycles Wr held assetred */
#define GALILEO_DEV_CFG_WrHigh_m (0x20000)	/* Cycles between assertions of Wr */
#define GALILEO_DEV_CFG_DevWidth_m (0x100000)	/* Device bus width */
#define GALILEO_DEV_CFG_DevLoc_m (0x800000)	/* Even/Odd side if width != 64*/
#define GALILEO_DEV_CFG_LatchFunct_m (0x2000000)	/* Latch enable function */
#define GALILEO_DEV_CFG_Parity_m (0x40000000)	/* Parity support */
#define GALILEO_DEV1_CFG (0x0AC000460)	/* Device Bank 0 (PCS0) Configuration */
#define GALILEO_DEV2_CFG (0x0AC000464)	/* Device Bank 0 (PCS0) Configuration */
#define GALILEO_DEV3_CFG (0x0AC000468)	/* Device Bank 0 (PCS0) Configuration */
#define GALILEO_BOOT_CFG (0x0AC00046C)	/* Boot Device (BOOTCS) Configuration */
#define GALILEO_ERRADDR (0x0AC000470)	/* Address of accesses to invalid ranges */
#define GALILEO_DMA0_BC (0x0AC000800)	/* DMA channel 0 byte count */
#define GALILEO_DMA1_BC (0x0AC000804)	/* DMA channel 1 byte count */
#define GALILEO_DMA2_BC (0x0AC000808)	/* DMA channel 2 byte count */
#define GALILEO_DMA3_BC (0x0AC00080C)	/* DMA channel 3 byte count */
#define GALILEO_DMA0_SRC (0x0AC000810)	/* DMA channel 0 Source address */
#define GALILEO_DMA1_SRC (0x0AC000814)	/* DMA channel 1 Source address */
#define GALILEO_DMA2_SRC (0x0AC000818)	/* DMA channel 2 Source address */
#define GALILEO_DMA3_SRC (0x0AC00081C)	/* DMA channel 3 Source address */
#define GALILEO_DMA0_DST (0x0AC000820)	/* DMA channel 0 Destination address */
#define GALILEO_DMA1_DST (0x0AC000824)	/* DMA channel 1 Destination address */
#define GALILEO_DMA2_DST (0x0AC000828)	/* DMA channel 2 Destination address */
#define GALILEO_DMA3_DST (0x0AC00082C)	/* DMA channel 3 Destination address */
#define GALILEO_DMA0_NXTRCD (0x0AC000830)	/* DMA channel 0 Next Record pointer */
#define GALILEO_DMA1_NXTRCD (0x0AC000834)	/* DMA channel 1 Next Record pointer */
#define GALILEO_DMA2_NXTRCD (0x0AC000838)	/* DMA channel 2 Next Record pointer */
#define GALILEO_DMA3_NXTRCD (0x0AC00083C)	/* DMA channel 3 Next Record pointer */
#define GALILEO_TIMER0 (0x0AC000850)	/* TIMER/COUNTER 0 */
#define GALILEO_TIMER1 (0x0AC000854)	/* TIMER/COUNTER 0 */
#define GALILEO_TIMER2 (0x0AC000858)	/* TIMER/COUNTER 0 */
#define GALILEO_TIMER3 (0x0AC00085C)	/* TIMER/COUNTER 0 */
#define GALILEO_TIMER_CTL (0x0AC000864)	/* Timer/Counter control */
#define GALILEO_PCI_CMD (0x0AC000C00)	/* PCI Command register */
#define GALILEO_PCI_CMD_SWAP (0x0AC000000)	/* PCI byte swap */
#define GALILEO_PCI_CMD_SYNC (0x0AC000001)	/* Sync mode: 0=normal, 1=Pclk*/
#define GALILEO_PCI_RETRY (0x0AC000C04)	/* Timeout and retry register */
#define GALILEO_PCI_RETRY_TO0_b (0x0)	/* Timeout 0, 8 bits (default to 0xF) */
#define GALILEO_PCI_RETRY_TO1_b (0x8)	/* Timeout 1, 8 bits (default to 0x7) */
#define GALILEO_PCI_RETRY_CTR_b (0x10)	/* Number of retries, 24 bits */
#define GALILEO_PCI_RETRY_TO0_m (0x1)	/* Timeout 0, 8 bits (default to 0xF) */
#define GALILEO_PCI_RETRY_TO1_m (0x100)	/* Timeout 1, 8 bits (default to 0x7) */
#define GALILEO_PCI_RETRY_CTR_m (0x10000)	/* Number of retries, 24 bits */
#define GALILEO_PCI_CFG (0x0AC000CF8)	/* PCI Configuration register */
#define GALILEO_PCI_DTA (0x0AC000CFC)	/* PCI Configuration data register */
#define GALILEO_INT_CAUSE (0x0AC000C18)	/* Galileo Interrupt Cause register */
#define GALILEO_INT_DMA0_b (0x4)	/* DMA 0 interrupt */
#define GALILEO_INT_DMA1_b (0x5)	/* DMA 1 interrupt */
#define GALILEO_INT_DMA2_b (0x6)	/* DMA 2 interrupt */
#define GALILEO_INT_DMA3_b (0x7)	/* DMA 3 interrupt */
#define GALILEO_INT_DMA0_m (0x10)	/* DMA 0 interrupt */
#define GALILEO_INT_DMA1_m (0x20)	/* DMA 1 interrupt */
#define GALILEO_INT_DMA2_m (0x40)	/* DMA 2 interrupt */
#define GALILEO_INT_DMA3_m (0x80)	/* DMA 3 interrupt */
#define GALILEO_INT_TIMER0_b (0x8)	/* Timer 0 interrupt */
#define GALILEO_INT_TIMER1_b (0x9)	/* Timer 1 interrupt */
#define GALILEO_INT_TIMER2_b (0x0A)	/* Timer 2 interrupt */
#define GALILEO_INT_TIMER3_b (0x0B)	/* Timer 3 interrupt */
#define GALILEO_INT_TIMER0_m (0x100)	/* Timer 0 interrupt */
#define GALILEO_INT_TIMER1_m (0x200)	/* Timer 1 interrupt */
#define GALILEO_INT_TIMER2_m (0x400)	/* Timer 2 interrupt */
#define GALILEO_INT_TIMER3_m (0x800)	/* Timer 3 interrupt */
#define GALILEO_CPU_INT_m (0x3E00000)	/* Galileo CPU Interrupt Mask (to PCI) */
#define GALILEO_PCI_INT_m (0x3C000000)	/* Galileo PCI Interrupt Mask (to CPU) */
#define GALILEO_CPU_I_ENA (0x0AC000C1C)	/* Galileo CPU Interrupt Mask register */
#define GALILEO_PCI_I_ENA (0x0AC000C24)	/* Galileo PCI Interrupt Mask register */
#  define FLUSH_WB() do { U32 junk; junk = *(VU32 *)GALILEO_PCI_CFG; } while (0)
#define SWINT0_LVL (0)	/* Software interrupt 0 */
#define SWINT1_LVL (1)	/* Software interrupt 1 */
#define INT0_LVL (2)	/* Interrupt Control PLD (pin 81) */
#define INT1_LVL (3)	/* Interrupt Control PLD (pin 85) */
#define INT2_LVL (4)	/* Interrupt Control PLD (pin 82) */
#define INT3_LVL (5)	/* Interrupt Control PLD (pin 83) */
#define INT4_LVL (6)	/* Interrupt Control PLD (pin 86) */
#define INT5_LVL (7)	/* Interrupt Control PLD (pin 87) */
#define XBUS0_LVL (8)	/* Fake levels */
#define XBUS1_LVL (9)
#define XBUS2_LVL (10)
#define XBUS3_LVL (11)
#define B_PIC_XS0 (0)	/* Phoenix Interrupt Control */
#define XS0_NOTES (0x1)
#define B_PIC_XS1 (1)	/* Phoenix Interrupt Control */
#define XS1_NOTES (0x2)
#define B_PIC_XS2 (2)	/* Phoenix Interrupt Control */
#define XS2_NOTES (0x4)
#define B_PIC_XS3 (3)	/* Phoenix Interrupt Control */
#define XS3_NOTES (0x8)
#define B_PIC_IDE (12)	/* Phoenix Interrupt Control */
#define IDE_NOTES (0x1000)
#define B_GIC_TM3 (11)	/* Galileo Interrupt Control */
#define TM3_NOTES (0x800)
#define INTCTL_MAPA_INIT (0x0FF)	/* Phoenix Interrupt Map Register A */
#define INTCTL_MAPB_INIT (0x200)	/* Phoenix Interrupt Map Register B */
#define PHOENIX_INTCTL_MASK (0x100F)	/* Phoenix Interrupt Control */
#define GALILEO_INTCTL_MASK (0x800)	/* Galileo Interrupt Control */
#define INTS_ON (0x0FF01)	/* Enable all interrupts */
#define INTS_OFF (0x0FF00)	/* Disable all interrupts */
struct diag_params {
U32 actual_msb;	/* Actual data read from mem (upper 32 bits) */
U32 actual_lsb;	/* Actual data read from mem (lower 32 bits) */
U32 expected_msb;	/* Expected data (upper 32 bits) */
U32 expected_lsb;	/* Expected data (Lower 32 bits) */
U32 bad_address;	/* Failing address */
U32 subtest;	/* Subtest number */
};
#define WDOG *(VU32 *)WATCHDOG
extern U32 WRAM[8388608];	/* Working RAM	*/
#define WRAM_ADDR (0x80000000)
extern U8 EEPROM[131072];	/* EEPROM 		*/
#define EEPROM_ADDR (0x0B5480000)
#define BRAM ((U8*)(0x0B5480000))	/* BRAM	(non-cached)	*/
#define UNLK_EP (*(VU32*)(0x0B5500000))	/* BRAM unlock */
#define RST_VEC (0x0BFC00000)	/* reset and NMI vector */
#define TLB_VEC (0x0BFC00200)	/* tlb refill vector */
#define XTLB_VEC (0x0BFC00280)	/* XTLB refill vevtor (64 bit) */
#define CACHE_VEC (0x0BFC00300)	/* CACHE Error vector */
#define EXCEPT_VEC (0x0BFC00380)	/* Normal exception vector */
#define K0BASE (0x80000000)
#define K0SIZE (0x20000000)
#define K1BASE (0x0A0000000)
#define K1SIZE (0x20000000)
#define K2BASE (0x0C0000000)
#define K2SIZE (0x20000000)
#define KSBASE (0x0E0000000)
#define KSSIZE (0x20000000)
#define KUBASE (0x0)
#define KUSIZE (0x80000000)
#define T_VEC (0x80000000)	/* tlbmiss vector */
#define X_VEC (0x80000080)	/* xtlbmiss vector */
#define C_VEC (0x80000100)	/* cache error vector */
#define E_VEC (0x80000180)	/* exception vector */
#define R_VEC (0x0BFC00000)	/* reset vector */
#define 	CAST(as) (as)
#define	K0_TO_K1(x)	(CAST(unsigned)(x)|0xA0000000)	/* kseg0 to kseg1 */
#define	K1_TO_K0(x)	(CAST(unsigned)(x)&0x9FFFFFFF)	/* kseg1 to kseg0 */
#define	K0_TO_PHYS(x)	(CAST(unsigned)(x)&0x1FFFFFFF)	/* kseg0 to physical */
#define	K1_TO_PHYS(x)	(CAST(unsigned)(x)&0x1FFFFFFF)	/* kseg1 to physical */
#define	PHYS_TO_K0(x)	(CAST(unsigned)(x)|0x80000000)	/* physical to kseg0 */
#define	PHYS_TO_K1(x)	(CAST(unsigned)(x)|0xA0000000)	/* physical to kseg1 */
#define MINCACHE (0x800)	/* 2*1024     2k   */
#define MAXCACHE (0x40000)	/* 256*1024   256k */
#define CFG_CM (0x80000000)	/* Master-Checker mode */
#define CFG_ECMASK (0x70000000)	/* System Clock Ratio */
#define CFG_ECBY2 (0x0)	/* divide by 2 */
#define CFG_ECBY3 (0x10000000)	/* divide by 3 */
#define CFG_ECBY4 (0x20000000)	/* divide by 4 */
#define CFG_EPMASK (0x0F000000)	/* Transmit data pattern */
#define CFG_EPD (0x0)	/* D */
#define CFG_EPDDX (0x1000000)	/* DDX */
#define CFG_EPDDXX (0x2000000)	/* DDXX */
#define CFG_EPDXDX (0x3000000)	/* DXDX */
#define CFG_EPDDXXX (0x4000000)	/* DDXXX */
#define CFG_EPDDXXXX (0x5000000)	/* DDXXXX */
#define CFG_EPDXXDXX (0x6000000)	/* DXXDXX */
#define CFG_EPDDXXXXX (0x7000000)	/* DDXXXXX */
#define CFG_EPDXXXDXXX (0x8000000)	/* DXXXDXXX */
#define CFG_SBMASK (0x0C00000)	/* Secondary cache block size */
#define CFG_SBSHIFT (0x16)
#define CFG_SB4 (0x0)	/* 4 words */
#define CFG_SB8 (0x400000)	/* 8 words */
#define CFG_SB16 (0x800000)	/* 16 words */
#define CFG_SB32 (0x0C00000)	/* 32 words */
#define CFG_SS (0x200000)	/* Split secondary cache */
#define CFG_SW (0x100000)	/* Secondary cache port width */
#define CFG_EWMASK (0x0C0000)	/* System port width */
#define CFG_EWSHIFT (0x12)
#define CFG_EW64 (0x0)	/* 64 bit */
#define CFG_EW32 (0x10000)	/* 32 bit */
#define CFG_SC (0x20000)	/* Secondary cache absent */
#define CFG_SM (0x10000)	/* Dirty Shared mode disabled */
#define CFG_BE (0x8000)	/* Big Endian */
#define CFG_EM (0x4000)	/* ECC mode enable */
#define CFG_EB (0x2000)	/* Block ordering */
#define CFG_ICMASK (0x0E00)	/* Instruction cache size */
#define CFG_ICSHIFT (0x9)
#define CFG_DCMASK (0x1C0)	/* Data cache size */
#define CFG_DCSHIFT (0x6)
#define CFG_IB (0x20)	/* Instruction cache block size */
#define CFG_DB (0x10)	/* Data cache block size */
#define CFG_CU (0x8)	/* Update on Store Conditional */
#define CFG_K0MASK (0x7)	/* KSEG0 coherency algorithm */
#define CFG_C_UNCACHED (0x2)
#define CFG_C_NONCOHERENT (0x3)
#define CFG_C_COHERENTXCL (0x4)
#define CFG_C_COHERENTXCLW (0x5)
#define CFG_C_COHERENTUPD (0x6)
#define Index_Invalidate_I (0x0)	/* 0       0 */
#define Index_Writeback_Inv_D (0x1)	/* 0       1 */
#define Index_Invalidate_SI (0x2)	/* 0       2 */
#define Index_Writeback_Inv_SD (0x3)	/* 0       3 */
#define Index_Load_Tag_I (0x4)	/* 1       0 */
#define Index_Load_Tag_D (0x5)	/* 1       1 */
#define Index_Load_Tag_SI (0x6)	/* 1       2 */
#define Index_Load_Tag_SD (0x7)	/* 1       3 */
#define Index_Store_Tag_I (0x8)	/* 2       0 */
#define Index_Store_Tag_D (0x9)	/* 2       1 */
#define Index_Store_Tag_SI (0x0A)	/* 2       2 */
#define Index_Store_Tag_SD (0x0B)	/* 2       3 */
#define Create_Dirty_Exc_D (0x0D)	/* 3       1 */
#define Create_Dirty_Exc_SD (0x0F)	/* 3       3 */
#define Hit_Invalidate_I (0x10)	/* 4       0 */
#define Hit_Invalidate_D (0x11)	/* 4       1 */
#define Hit_Invalidate_SI (0x12)	/* 4       2 */
#define Hit_Invalidate_SD (0x13)	/* 4       3 */
#define Hit_Writeback_Inv_D (0x15)	/* 5       1 */
#define Hit_Writeback_Inv_SD (0x17)	/* 5       3 */
#define Fill_I (0x14)	/* 5       0 */
#define Hit_Writeback_D (0x19)	/* 6       1 */
#define Hit_Writeback_SD (0x1B)	/* 6       3 */
#define Hit_Writeback_I (0x18)	/* 6       0 */
#define Hit_Set_Virtual_SI (0x1E)	/* 7       2 */
#define Hit_Set_Virtual_SD (0x1F)	/* 7       3 */
#define N_TLB_ENTRIES (0x30)
#define TLBHI_VPN2MASK (0x0FFFFE000)
#define TLBHI_PIDMASK (0x0FF)
#define TLBHI_NPID (0x100)
#define TLBLO_PFNMASK (0x3FFFFFC0)
#define TLBLO_PFNSHIFT (0x6)
#define TLBLO_D (0x4)	/* writeable */
#define TLBLO_V (0x2)	/* valid bit */
#define TLBLO_G (0x1)	/* global access bit */
#define TLBLO_CMASK (0x38)	/* cache algorithm mask */
#define TLBLO_CSHIFT (0x3)
#define TLBLO_UNCACHED (0x10)
#define TLBLO_NONCOHERENT (0x18)
#define TLBLO_COHERENTXCL (0x20)
#define TLBLO_COHERENTXCLW (0x28)
#define TLBLO_COHERENTUPD (0x30)
#define TLBINX_PROBE (0x80000000)
#define TLBINX_INXMASK (0x3F)
#define TLBRAND_RANDMASK (0x3F)
#define TLBCTXT_BASEMASK (0x0FF800000)
#define TLBCTXT_BASESHIFT (0x17)
#define TLBCTXT_VPN2MASK (0x7FFFF0)
#define TLBCTXT_VPN2SHIFT (0x4)
#define TLBPGMASK_MASK (0x1FFE000)
#define SR_CUMASK (0x0F0000000)	/* coproc usable bits */
#define SR_CU3 (0x80000000)	/* Coprocessor 3 usable */
#define SR_CU2 (0x40000000)	/* Coprocessor 2 usable */
#define SR_CU1 (0x20000000)	/* Coprocessor 1 usable */
#define SR_CU0 (0x10000000)	/* Coprocessor 0 usable */
#define SR_RP (0x8000000)	/* Reduced power operation */
#define SR_FR (0x4000000)	/* Additional floating point registers */
#define SR_RE (0x2000000)	/* Reverse endian in user mode */
#define SR_BEV (0x400000)	/* Use boot exception vectors */
#define SR_TS (0x200000)	/* TLB shutdown */
#define SR_SR (0x100000)	/* Soft reset */
#define SR_CH (0x40000)	/* Cache hit */
#define SR_CE (0x20000)	/* Use cache ECC  */
#define SR_DE (0x10000)	/* Disable cache exceptions */
#define SR_IMASK (0x0FF00)	/* Interrupt mask */
#define SR_IMASK8 (0x0)	/* mask level 8 */
#define SR_IMASK7 (0x8000)	/* mask level 7 */
#define SR_IMASK6 (0x0C000)	/* mask level 6 */
#define SR_IMASK5 (0x0E000)	/* mask level 5 */
#define SR_IMASK4 (0x0F000)	/* mask level 4 */
#define SR_IMASK3 (0x0F800)	/* mask level 3 */
#define SR_IMASK2 (0x0FC00)	/* mask level 2 */
#define SR_IMASK1 (0x0FE00)	/* mask level 1 */
#define SR_IMASK0 (0x0FF00)	/* mask level 0 */
#define SR_IMASKSHIFT (0x8)
#define SR_IBIT8 (0x8000)	/* bit level 8 */
#define SR_IBIT7 (0x4000)	/* bit level 7 */
#define SR_IBIT6 (0x2000)	/* bit level 6 */
#define SR_IBIT5 (0x1000)	/* bit level 5 */
#define SR_IBIT4 (0x800)	/* bit level 4 */
#define SR_IBIT3 (0x400)	/* bit level 3 */
#define SR_IBIT2 (0x200)	/* bit level 2 */
#define SR_IBIT1 (0x100)	/* bit level 1 */
#define SR_KX (0x80)	/* TLB vector offset = 0x80 */
#define SR_SX (0x40)	/* Supervisor mode 64 bit */
#define SR_UX (0x20)	/* User mode 64 bit */
#define SR_KSMASK (0x18)	/* Kernel mode mask */
#define SR_KSUSER (0x10)	/* User mode */
#define SR_KSSUPER (0x8)	/* Supervisor mode */
#define SR_KSKERNEL (0x0)	/* Kernel mode */
#define SR_ERL (0x4)	/* Error level */
#define SR_EXL (0x2)	/* Exception level */
#define SR_IE (0x1)	/* Interrupts enabled */
#define SR_IEC (0x1)	/* Interrupts enabled */
#define CAUSE_BD (0x80000000)	/* Branch delay slot */
#define CAUSE_CEMASK (0x30000000)	/* coprocessor error */
#define CAUSE_CESHIFT (0x1C)
#define CAUSE_IPMASK (0x0FF00)	/* Pending interrupt mask */
#define CAUSE_IPSHIFT (0x8)
#define CAUSE_EXCMASK (0x7C)	/* Cause code bits */
#define CAUSE_EXCSHIFT (0x2)
#define C1_FCSR_FS (16777216)	/* Flush denormalized results to 0*/
#define C1_FCSR_C (8388608)	/* Condition bit */
#define C1_FCSR_CA_E (131072)	/* Unimplemented operation exception */
#define C1_FCSR_CA_V (65536)	/* Invalid operation exception */
#define C1_FCSR_CA_Z (32768)	/* Divide by 0 exception */
#define C1_FCSR_CA_O (16384)	/* Overflow exception */
#define C1_FCSR_CA_U (8192)	/* Undeflow exception */
#define C1_FCSR_CA_I (4096)	/* Inexact exception */
#define C1_FCSR_EN_V (2048)	/* Enable invalid operation exception */
#define C1_FCSR_EN_Z (1024)	/* Enable divide by 0 exception */
#define C1_FCSR_EN_O (512)	/* Enable overflow exception */
#define C1_FCSR_EN_U (256)	/* Enable underflow exception */
#define C1_FCSR_EN_I (128)	/* Enable inexact exception */
#define C1_FCSR_FLG_V (64)	/* Invalid operation */
#define C1_FCSR_FLG_Z (32)	/* Divide by 0 */
#define C1_FCSR_FLG_O (16)	/* Overflow */
#define C1_FCSR_FLG_U (8)	/* Undeflow */
#define C1_FCSR_FLG_I (4)	/* Inexact */
#define C1_FCSR_RM_RN (0)	/* Round to nearest representable */
#define C1_FCSR_RM_RZ (1)	/* Round toward 0 */
#define C1_FCSR_RM_RP (2)	/* Round toward +infinity */
#define C1_FCSR_RM_RM (3)	/* Round toward -infinity */
struct cpu_params {
U32 cpu_type;	/* cpu type (3041, 3051, etc) */
U32 cpu_prid;	/* copy of C0_PRID register */
U32 cpu_icache;	/* size of I cache in bytes */
U32 cpu_dcache;	/* size of D cache in bytes */
U32 cpu_icache_ls;	/* linesize of I cache in bytes */
U32 cpu_dcache_ls;	/* linesize of D cache in bytes */
};
#define CPU_ID_TYPE (0)	/* Index into cpu_ident[] for TYPE */
#define CPU_ID_PRID (1)	/* Index into cpu_ident[] for PRID */
#define CPU_ID_ICACHE (2)	/* Index into cpu_ident[] for ICACHE size */
#define CPU_ID_DCACHE (3)	/* Index into cpu_ident[] for DCACHE size */
struct ROM_VECTOR_STR {
U32 ROMV_JUMP;	/* Jump (or branch) to start address */
U32 ROMV_JNOP;	/* Delay slot after jump */
U32 ROMV_SENTINEL;	/* FEEDFACE or FEEDFADE sentinel */
U32 ROMV_TSIZE;	/* Size of data to copy */
U32 ROMV_STUB_INIT;	/* Entry point to stub's init */
U32 ROMV_STUB_FLAG;	/* Stub init marker */
U32 ROMV_STUB_EH;	/* Stub exception handler entry */
U32 ROMV_STUB_VECS;	/* Ptr to list of functions provided by stub */
U32 ROMV_RAMVBR;	/* Pointer to RAM based array of pointers to functions */
U32 ROMV_STACK;	/* Last usable address in SRAM */
U32 ROMV_ENTRY;	/* reset entry point */
U32 ROMV_EXCEPT;	/* Pointer to game's exception handler */
U32 ROMV_STUB_REASON;	/* Pointer to game's exception message (or 0) */
};
struct RAM_VECTOR_STR {
U32 RAMV_IRQ0;	/*  Interrupt 0 */
U32 RAMV_IRQ1;	/*  Interrupt 1 */
U32 RAMV_IRQ2;	/*  Interrupt 2 */
U32 RAMV_IRQ3;	/*  Interrupt 3 */
U32 RAMV_IRQ4;	/*  Interrupt 4 */
U32 RAMV_IRQ5;	/*  Interrupt 5 */
U32 RAMV_IRQ6;	/*  Interrupt 6 */
U32 RAMV_IRQ7;	/*  Interrupt 7 */
U32 RAMV_TLB;	/*  TLB exception  */
U32 RAMV_TMP;	/* Not used anymore, remains only for compatibility with old stub */
U32 RAMV_NORMAL;	/* Not used anymore, remains only for compatibility with old stub */
U32 RAMV_CHEAP;	/* Not used anymore, remains only for compatibility with old stub */
U32 RAMV_TLBM;	/* TLB modification exception address */
U32 RAMV_TLBL;	/* TLB Load exception address */
U32 RAMV_TLBS;	/* TLB store exception address */
U32 RAMV_ADDRL;	/* Address exception on load or I-fetch */
U32 RAMV_ADDRS;	/* Address exception on store */
U32 RAMV_BUSERRI;	/* Bus error on I-fetch */
U32 RAMV_BUSERRD;	/* Bus error on data load */
U32 RAMV_SYSCALL;	/* SYSCALL exception */
U32 RAMV_BREAK;	/* Breakpoint exception */
U32 RAMV_RESERV;	/* Reserved instruction exception */
U32 RAMV_COPROC;	/* Coprocessor unusable exception */
U32 RAMV_OVERFL;	/* Integer overflow exception */
U32 RAMV_TRAPV;	/* Trap exception */
U32 RAMV_FILL0;	/* Reserved entry */
U32 RAMV_FPE;	/* Floating point exception */
};
struct IIO_VECTOR_STR {
U32 STUBVEC_PKTINIT;	/* Ptr to pktinit function (not used) */
U32 STUBVEC_PKTPOLL;	/* Ptr to pktpoll function */
U32 STUBVEC_PKTQRECV;	/* Ptr to pktquerecv function */
U32 STUBVEC_PKTQSEND;	/* Ptr to pktquesend function */
U32 STUBVEC_PKTIOCTL;	/* Ptr to pktioctl function */
U32 STUBVEC_NMI;	/* Ptr to stub's NMI handler */
U32 STUBVEC_FAKE_EH;	/* Ptr to stub's fake exception handler */
};
enum vecnums {
	VN_IRQ0,
	VN_IRQ1,
	VN_IRQ2,
	VN_IRQ3,
	VN_IRQ4,
	VN_IRQ5,
	VN_IRQ6,
	VN_IRQ7,
	VN_TLB,
	VN_TMP,
	VN_NORMAL,
	VN_CHEAP,
	VN_TLBM,
	VN_TLBL,
	VN_TLBS,
	VN_ADDRL,
	VN_ADDRS,
	VN_BUSERRI,
	VN_BUSERRD,
	VN_SYSCALL,
	VN_BREAK,
	VN_RESERV,
	VN_COPROC,
	VN_OVERFL,
	VN_TRAPV,
	VN_FILL0,
	VN_FPE,
	VN_MAX
};
#define NUM_RVECS VN_MAX
#define REGISTER_SIZE (8)	/* Size of the registers in bytes */
#define MIPS_REGSIZE (8)	/* Size of the registers in bytes */
#define ZERO_REGNUM (0)	/* read-only register, always 0 */
#define ZERO_REGNUM_W (0)	/* read-only register, always 0 */
#define ZERO_REGNUM_U (1)	/* read-only register, always 0 */
#define AT_REGNUM (1)	/* reg used by assembler */
#define AT_REGNUM_W (2)	/* reg used by assembler */
#define AT_REGNUM_U (3)	/* reg used by assembler */
#define V0_REGNUM (2)	/* Function integer return value */
#define V0_REGNUM_W (4)	/* Function integer return value */
#define V0_REGNUM_U (5)	/* Function integer return value */
#define A0_REGNUM (4)	/* Loc of first arg during a subr call */
#define A0_REGNUM_W (8)	/* Loc of first arg during a subr call */
#define A0_REGNUM_U (9)	/* Loc of first arg during a subr call */
#define SP_REGNUM (29)	/* Contains address of top of stack */
#define SP_REGNUM_W (58)	/* Contains address of top of stack */
#define SP_REGNUM_U (59)	/* Contains address of top of stack */
#define S8_REGNUM (30)	/* S8/FP register */
#define S8_REGNUM_W (60)	/* S8/FP register */
#define S8_REGNUM_U (61)	/* S8/FP register */
#define RA_REGNUM (31)	/* Contains return address value */
#define RA_REGNUM_W (62)	/* Contains return address value */
#define RA_REGNUM_U (63)	/* Contains return address value */
#define PS_REGNUM (32)	/* Contains processor status */
#define PS_REGNUM_W (64)	/* Contains processor status */
#define PS_REGNUM_U (65)	/* Contains processor status */
#define HI_REGNUM (34)	/* Multiply/divide high byte */
#define HI_REGNUM_W (68)	/* Multiply/divide high byte */
#define HI_REGNUM_U (69)	/* Multiply/divide high byte */
#define LO_REGNUM (33)	/* Multiply/divide low byte */
#define LO_REGNUM_W (66)	/* Multiply/divide low byte */
#define LO_REGNUM_U (67)	/* Multiply/divide low byte */
#define BADVADDR_REGNUM (35)	/* bad vaddr for addressing exception */
#define BADVADDR_REGNUM_W (70)	/* bad vaddr for addressing exception */
#define BADVADDR_REGNUM_U (71)	/* bad vaddr for addressing exception */
#define CAUSE_REGNUM (36)	/* describes last exception */
#define CAUSE_REGNUM_W (72)	/* describes last exception */
#define CAUSE_REGNUM_U (73)	/* describes last exception */
#define PC_REGNUM (37)	/* Contains program counter */
#define PC_REGNUM_W (74)	/* Contains program counter */
#define PC_REGNUM_U (75)	/* Contains program counter */
#define FP0_REGNUM (38)	/* Floating point register 0 (single float) */
#define FP0_REGNUM_W (76)	/* Floating point register 0 (single float) */
#define FP0_REGNUM_U (77)	/* Floating point register 0 (single float) */
#define FCRCS_REGNUM (70)	/* FP control/status */
#define FCRCS_REGNUM_W (140)	/* FP control/status */
#define FCRCS_REGNUM_U (141)	/* FP control/status */
#define FCRIR_REGNUM (71)	/* FP implementation/revision */
#define FCRIR_REGNUM_W (142)	/* FP implementation/revision */
#define FCRIR_REGNUM_U (143)	/* FP implementation/revision */
#define FP_REGNUM (72)	/* Pseudo register that contains true address of executing stack frame */
#define FP_REGNUM_W (144)	/* Pseudo register that contains true address of executing stack frame */
#define FP_REGNUM_U (145)	/* Pseudo register that contains true address of executing stack frame */
#define FIRST_EMBED_REGNUM (73)	/* First supervisor register for embedded use */
#define FIRST_EMBED_REGNUM_W (146)	/* First supervisor register for embedded use */
#define FIRST_EMBED_REGNUM_U (147)	/* First supervisor register for embedded use */
#define INX_REGNUM (73)
#define INX_REGNUM_W (146)
#define INX_REGNUM_U (147)
#define RAND_REGNUM (74)
#define RAND_REGNUM_W (148)
#define RAND_REGNUM_U (149)
#define TLBLO0_REGNUM (75)
#define TLBLO0_REGNUM_W (150)
#define TLBLO0_REGNUM_U (151)
#define TLBLO1_REGNUM (76)
#define TLBLO1_REGNUM_W (152)
#define TLBLO1_REGNUM_U (153)
#define CTXT_REGNUM (77)
#define CTXT_REGNUM_W (154)
#define CTXT_REGNUM_U (155)
#define PAGEMASK_REGNUM (78)
#define PAGEMASK_REGNUM_W (156)
#define PAGEMASK_REGNUM_U (157)
#define WIRED_REGNUM (79)
#define WIRED_REGNUM_W (158)
#define WIRED_REGNUM_U (159)
#define COUNT_REGNUM (80)
#define COUNT_REGNUM_W (160)
#define COUNT_REGNUM_U (161)
#define TLBHI_REGNUM (81)
#define TLBHI_REGNUM_W (162)
#define TLBHI_REGNUM_U (163)
#define COMPARE_REGNUM (82)
#define COMPARE_REGNUM_W (164)
#define COMPARE_REGNUM_U (165)
#define EPC_REGNUM (83)
#define EPC_REGNUM_W (166)
#define EPC_REGNUM_U (167)
#define PRID_REGNUM (84)
#define PRID_REGNUM_W (168)
#define PRID_REGNUM_U (169)
#define CONFIG_REGNUM (85)
#define CONFIG_REGNUM_W (170)
#define CONFIG_REGNUM_U (171)
#define LLADDR_REGNUM (86)
#define LLADDR_REGNUM_W (172)
#define LLADDR_REGNUM_U (173)
#define XCTXT_REGNUM (89)
#define XCTXT_REGNUM_W (178)
#define XCTXT_REGNUM_U (179)
#define ECC_REGNUM (90)
#define ECC_REGNUM_W (180)
#define ECC_REGNUM_U (181)
#define CACHEERR_REGNUM (91)
#define CACHEERR_REGNUM_W (182)
#define CACHEERR_REGNUM_U (183)
#define TAGLO_REGNUM (92)
#define TAGLO_REGNUM_W (184)
#define TAGLO_REGNUM_U (185)
#define TAGHI_REGNUM (93)
#define TAGHI_REGNUM_W (186)
#define TAGHI_REGNUM_U (187)
#define ERRPC_REGNUM (94)
#define ERRPC_REGNUM_W (188)
#define ERRPC_REGNUM_U (189)
#define LAST_EMBED_REGNUM (94)	/* Last one */
#define LAST_EMBED_REGNUM_W (188)	/* Last one */
#define LAST_EMBED_REGNUM_U (189)	/* Last one */
#define NUM_REGS (95)	/* Number of machine registers */
#define REGISTER_BYTES (760)	/* Total number of register bytes */
#define MAX_IRQS (8)	/* RxK has only 8 possible interrupts */
#define MAX_EXCEPTIONS (16)	/* R4k has only 16 possible exceptions */



#define IDE_REG_OFFSET (0x1F0)
#define IDE_REG_ALT_STATUS_OFFSET (0x3F6)	/*R*/
#define IDE_REG_FIXED_DISK_OFFSET (0x3F6)	/*W*/

struct ide_ctl {
    union {
	U32 ldata;
	struct { U16 data; U16 pad; } wdata;
	struct { U8 data; U8 precomp_error; U8 scnt; U8 snum; } bdata;
    } overlaid;
    U8 lcylinder;
    U8 hcylinder;
    U8 drive_head;
    U8 csr;
}; 
#define IDE_CMD_RECALIBRATE (0x10)
#define IDE_CMD_SREAD (0x20)
#define IDE_CMD_SWRITE (0x30)
#define IDE_CMD_SVERIFY (0x40)
#define IDE_CMD_FORMAT (0x50)
#define IDE_CMD_SEEK (0x70)
#define IDE_CMD_DIAGNOSTICS (0x90)
#define IDE_CMD_INITPARMS (0x91)
#define IDE_CMD_MREAD (0x0C4)
#define IDE_CMD_MWRITE (0x0C5)
#define IDE_CMD_MULTIMODE (0x0C6)
#define IDE_CMD_BREAD (0x0E4)
#define IDE_CMD_BWRITE (0x0E8)
#define IDE_CMD_IDENTIFY (0x0EC)
#define IDE_CMD_BUFFERMODE (0x0EF)
#define IDE_STB_BUSY (0x80)
#define IDE_STB_READY (0x40)
#define IDE_STB_WRFAULT (0x20)
#define IDE_STB_SEEKDONE (0x10)
#define IDE_STB_DATAREQ (0x8)
#define IDE_STB_CORRDATA (0x4)
#define IDE_STB_INDEX (0x2)
#define IDE_STB_ERROR (0x1)
#define IDE_ERB_BADBLOCK (0x80)
#define IDE_ERB_UNCDATA (0x40)
#define IDE_ERB_IDNFOUND (0x10)
#define IDE_ERB_ABORTCMD (0x4)
#define IDE_ERB_TK0NFOUND (0x2)
#define IDE_ERB_AMNFOUND (0x1)
#define DRIVE_HEAD_INFO (0x0A0)	/*sector size and master select */
#define BYTES_PER_SECTOR (0x200)
#define WORDS_PER_SECTOR (0x100)
#define LONGS_PER_SECTOR (0x80)
#define IDE_INTS_OFF (0x0)
#define IDE_INTS_ON (0x1)
#define IDE_DEVICE_INVALID (0x0)
#define IDE_DEVICE_CONNECTED (0x1)
#define IDE_BSS_BUFPTR (0x0)
#define IDE_BSS_CNTPTR (0x1)
#define IDE_BSS_RDPEND (0x2)
#define IDE_BSS_DEVPTR (0x3)
#define IDE_BSS_ERROR (0x4)

int ide_init(void);
int ide_set_device(int);
int ide_reset(void);
int ide_identify(unsigned int *);
int ide_check_devstat(void);
U32 ide_get_rdstatus(void);
U32 ide_get_errstatus(void);
int ide_get_rpm(void);
int ide_get_hdinfo(unsigned short *, unsigned short *, unsigned short *);
int ide_hread_sectors(unsigned int *, int, int, int, int);
int ide_write_sectors(unsigned int *, int, int, int, int);




#if !defined(_XBUSMON_H_)
#define _XBUSMON_H_


#define XBUSMON0_LED (*(VU32*)(0x0B4400000))
#define XBUSMON0_SW (*(VU32*)(0x0B4400000))
#define XBUSMON0_LCD (*(VU32*)(0x0B4400100))

#define XBUSMON1_LED (*(VU32*)(0x0B4400200))
#define XBUSMON1_SW (*(VU32*)(0x0B4400200))
#define XBUSMON1_LCD (*(VU32*)(0x0B4400300))

#define XBUSMON_SW1_UP (0x8000)
#define XBUSMON_SW1_DOWN (0x4000)
#define XBUSMON_SW1_LEFT (0x2000)
#define XBUSMON_SW1_RIGHT (0x1000)
#define XBUSMON_SW1_START (0x800)
#define XBUSMON_SW1_STOP (0x400)

#define XBUSMON_SW0_UP (0x80)
#define XBUSMON_SW0_DOWN (0x40)
#define XBUSMON_SW0_LEFT (0x20)
#define XBUSMON_SW0_RIGHT (0x10)
#define XBUSMON_SW0_START (0x8)
#define XBUSMON_SW0_STOP (0x4)
#define XBUSMON_SW0_SERIN (0x1)

#define XBUSMON_SEROUT (0x1)

extern void lcd_wait(int);
extern void lcd_update(void);
extern char *lcd_reset(void);
# if defined(LCD_GUTS_ONLY)
extern VU32 lcd_time;
extern char lcd_buf[81];
# endif
#endif			/* _XBUSMON_H_ */

#define INPUTS (*(VU16*)(0x0B5000010))
#define EXTEND (*(VU16*)(0x0B5000018))
#define MISC (*(VU16*)(0x0B5000008))
#define DIAG (*(VU16*)(0x0B5000000))
#define TEST (*(VU16*)(0x0B5000010))
#define B_TEST (0x0E)
#define SH_INPUTS (0x0)
#define SH_EXTEND (0x0)
#define SH_MISC (0x10)
#define SH_DIAG (0x10)
#define INPUT_SWITCHES ( INPUTS << SH_INPUTS )
#define EXTEND_SWITCHES ( EXTEND << SH_EXTEND )
#define MISC_SWITCHES ( MISC << SH_MISC )
#define DIAG_SWITCHES ( DIAG << SH_DIAG )
#define READ_RAW_SWITCHES(x) ( (x) ?\
			    ~( EXTEND_SWITCHES | DIAG_SWITCHES ) :\
			    ~( INPUT_SWITCHES | MISC_SWITCHES ) )
#define SW_EDGES (0x0FFFFFFFF)
#define SW_LEVELS (0x0)
#define J1_UP (0x1)	/* SW07 */
#define J1_DOWN (0x2)	/* SW06 */
#define J1_LEFT (0x4)	/* SW05 */
#define J1_RIGHT (0x8)	/* SW04 */
#define J2_UP (0x0)
#define J2_DOWN (0x0)
#define J2_LEFT (0x0)
#define J2_RIGHT (0x0)
#define J3_UP (0x0)
#define J3_DOWN (0x0)
#define J3_LEFT (0x0)
#define J3_RIGHT (0x0)
#define J_UP (0x1)
#define J_DOWN (0x2)
#define DEBUG0 (0x10000)
#define DEBUG1 (0x20000)
#define DEBUG2 (0x40000)
#define DEBUG3 (0x80000)
#define DEBUG4 (0x100000)
#define DEBUG5 (0x200000)
#define DEBUG6 (0x400000)
#define DEBUG7 (0x800000)
#define DEBUG8 (0x1000000)
#define DEBUG9 (0x2000000)
#define DEBUG10 (0x4000000)
#define DEBUG11 (0x8000000)
#define DEBUG12 (0x10000000)
#define DEBUG13 (0x20000000)
#define DEBUG14 (0x40000000)
#define DEBUG15 (0x80000000)
#define SW01 (0x20)	/* SW01 */
#define SW08 (0x200000)	/* SW08 */
#define SW09 (0x100)	/* SW09 */
#define SW10 (0x200)	/* SW10 */
#define SW11 (0x400)	/* SW11 */
#define SW12 (0x800)	/* SW12 */
#define SW13 (0x1000)	/* SW13 */
#define SW14 (0x2000)	/* SW14 */
#define SW15 (0x4000)	/* SW15 (Self Test) */
#define J1_VERT (0x3)
#define J1_BITS (0x0F)
#define J2_VERT (0x0)
#define J2_BITS (0x0)
#define J3_VERT (0x0)
#define J3_BITS (0x0)
#define SW_GREEN (0x10)	/* SW03 */
#define SW_WHITE (0x40000)	/* SW02 */
#define SW_BLACK (0x0)
#define J_LEFT (0x4)
#define J_RIGHT (0x8)
#define JOY_BITS (0x0F)
#define JOY_VERT (0x3)
#define SW_NEXT (0x10)	/* GREEN */
#define SW_ACTION (0x40000)	/* WHITE */
#define SW_EXTRA (0x0)	/* BLACK */
#define JOY_ALL (0x4001F)
#define SW_ALL (0x40010)
#define SW_BITS (0x0FFFFFFFF)
#if 0
extern U32 ctl_mod_latch(int);
#endif

#define LM_EMC_L	1
#define LM_EMC_R	2
#if 0
# define COUNTER_ON(x)		ctl_mod_latch(emc_map[x])
# define COUNTER_OFF(x)	ctl_mod_latch(~emc_map[x])
#endif

#define B_COINL		(11+SH_INPUTS)	/* COIN LEFT */
#define B_COINR		(8+SH_INPUTS)	/* COIN RIGHT */
#define BC_AUX_LFT	(10+SH_INPUTS)	/* AUX LEFT Coin switch */
#define BC_AUX_RGT	(9+SH_INPUTS)	/* AUX RIGHT Coin switch */
#define CN_STATUS	(INPUTS<<SH_INPUTS)
#define MECH_ORDER  { LFT_M, RGT_M, AUX_M, AUX_M }
#ifndef HDW_INIT
# define HDW_INIT(x) do { extern void prc_init_vecs(void); prc_init_vecs(); } while (0)
#endif
#define SST_BASE (0x0A8000000)	/* Address of 3DFX board */
#define TOT_H_PIX (0x400)	/* pixel count to next line	*/
#define TOT_V_PIX (0x400)	/* Total vertical screen size	*/
#define RED_LSB (0x400)	/* RED gun LSB		*/
#define GRN_LSB (0x20)	/* GREEN gun LSB		*/
#define BLU_LSB (0x1)	/* BLUE gun LSB		*/
#define ALL_LSB (0x421)
#define INTEN_MSK (0x8000)	/* INTENSITY MASK	*/
#define RED_MSK (0x7C00)	/* RED INTENSITY MASK	*/
#define GRN_MSK (0x3E0)	/* GREEN INTENSITY MASK	*/
#define BLU_MSK (0x1F)	/* BLUE INTENSITY MASK	*/
#define RED_SHF (0x0A)	/* RED INTENSITY SHIFT	*/
#define GRN_SHF (0x5)	/* GREEN INTENSITY SHIFT	*/
#define BLU_SHF (0x0)	/* BLUE INTENSITY SHIFT	*/
#define ALL_MSK (0x7FFF)
#define BITS_PER_GUN (0x5)	/* # of bits/gun	*/
#define YEL_MSK (0x7FE0)
#define CYN_MSK (0x3FF)
#define VIO_MSK (0x7C1F)
#define RED_QTR (0x2000)
#define GRN_QTR (0x100)
#define BLU_QTR (0x8)
#define WHT_FUL (0x5EF7)
#define SLT_FUL (0x3DEF)
#define GRY_FUL (0x1CE7)
#define GRY_BAK (0x1CE70000)
#define BLK_SLT (0x3DEF)	/* Slate on black	*/
#define BLK_WHT (0x5EF7)	/* White on black	*/
#define BLK_YEL (0x7FE0)	/* Yellow on Black	*/
#define BLK_RED (0x7C00)	/* Red on black		*/
#define BLK_GRN (0x3E0)	/* Green on Black	*/
#define BLK_CYN (0x3FF)	/* Cyan on black		*/
#define BLK_VIO (0x7C1F)	/* Violet on black	*/
#define BLK_BLU (0x1F)	/* Blue on black		*/
#define GRY_SLT (0x1CE73DEF)
#define GRY_WHT (0x1CE75EF7)
#define GRY_YEL (0x1CE77FE0)
#define GRY_RED (0x1CE77C00)
#define GRY_GRN (0x1CE703E0)
#define GRY_CYN (0x1CE703FF)
#define GRY_VIO (0x1CE77C1F)
#define GRY_BLU (0x1CE7001F)
#define AN_VIS_COL (0x40)	/* AN visible stamps horiz.	*/
#define AN_VIS_ROW (0x32)	/* AN visible stamps vert.	*/
#define AN_A_STMP (0x47)	/* code for 'A' stamp		*/
#define AN_SQUARE (0x3)	/* AN stamp # completely filled	*/
#define AN_BORDER (0x3)
#define AN_VOL_BAR (0x166)	/* AN stamp for volume stamp	*/
#define BGBIT (0x8000)
#define AN_PAL_MSK (0x7C00)
#define AN_PAL_SHF (0x0A)
#define AN_NXT_PAL (0x400)
#define GRY_PAL (0x0)	/* GREY is palette 0	*/
#define GRY_PALB (0x8000)
#define BLU_PAL (0x400)	/* BLUE is palette 1	*/
#define BLU_PALB (0x8400)
#define GRN_PAL (0x800)	/* GREEN is palette 2	*/
#define GRN_PALB (0x8800)
#define CYN_PAL (0x0C00)	/* CYAN is palette 3	*/
#define CYN_PALB (0x8C00)
#define RED_PAL (0x1000)	/* RED is palette 4	*/
#define RED_PALB (0x9000)
#define VIO_PAL (0x1400)	/* VIOLET is palette 5	*/
#define VIO_PALB (0x9400)
#define YEL_PAL (0x1800)	/* YELLOW is palette 6	*/
#define YEL_PALB (0x9800)
#define WHT_PAL (0x1C00)	/* WHITE is palette 7	*/
#define WHT_PALB (0x9C00)
#define BOLD_FONT (0x1)
#define UL_FONT (0x2)
#define REVERSE_FONT (0x4)
#define NORMAL_FONT (0x0)
#define MNORMAL_PAL (0x0)
#define TITLE_PAL (0x9000)
#define INSTR_PAL (0x8800)
#define BORDER_PAL (0x1000)
#define VERSION_PAL (0x8C00)
#define MHILITE_PAL (0x9800)
#define MNORMAL_PAL (0x0)
#define ERROR_PAL (0x9000)
#define MAX_MESS (0x3)	/* maximum # of special message types	*/
#define BIG_CHAR (0x80)	/* BIG character bit offset		*/
#define COLMSK (0x0FC00)	/* palette mask+bgbit	*/
#define SETMSK (0x3)	/* character set mask	*/
#define AN_SET0 (0x0)
#define AN_SET1 (0x1)
#define AN_SET2 (0x2)
#define AN_BIG_SET (0x3)
#define POT_CNT (0x0)
#define VCR_RECORD (0x0FFFFFFFF)	/* turn on vcr RECORD		*/
#define VCR_STOP (0x0)	/* STOP vcr			*/
#define VCR_HOLD_TIME (20)
#define TEST_DWELL (900)	/* How long to wait for NEXT in P.O.R*/
#define WDI_MASK (0x3FF)	/* longwords to try to clear btw wdogs*/
#undef TEST
#define TEST (~ctl_read_sw(0))
#undef B_TEST
#define B_TEST 20
#undef CN_STATUS
#define CN_STATUS (~(ctl_read_sw(0)>>8))
#if !defined(MECH_ORDER)
#define MECH_ORDER  { NO_MECH, NO_MECH, NO_MECH, NO_MECH, NO_MECH, LFT_M, RGT_M }
#endif
struct pconfigb {
void (*p_reset)();	/*  Points to code  */
const unsigned char * p_optmenu;	/*  game option menu  */
const unsigned char * p_coinmenu;	/*  coin opt menu  */
const char * p_linktime;	/*  Main program link time  */
char p_trapmode;	/*  trap type 0:STOP +:68K -:68010  */
char p_debug_options;	/*  Normally 0 */
void * p_oblist;	/* game-allocated object list memory */
unsigned long p_oblen;	/* length in bytes of above */
unsigned long p_vers_valid;	/* pconfigb version and validation */
const void * p_bss_start;	/* start of working ram */
const void * p_bss_end;	/* end of working ram */
const void * p_stats;	/*  pointer to game stats  */
};
struct pconfigp {
long p_validate;	/*  validation word	 */
struct pconfigb *p_configb;	/*  configuration pointer	 */
};
#define PB_VALID (0x0DEADBEEF)	/*value to validate PTR	*/
#endif				/* _CONFIG_H_ */
