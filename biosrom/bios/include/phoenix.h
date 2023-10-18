/* 
** constants for the WMS phoenix board (5579-14661-00)
**
** AMD 12/17/95
**
*/

#ifndef _PHOENIX7_
#define _PHOENIX7_

/* 
 * note that all of this can move around if the chips select registers
 * get hosed, either in startup, or running
 */

#define LOCAL_MEM	0x00000000 	/* Local on-board/private memory */
#define LOCAL_MEM_SIZE	0x10000000 	/* Local memory size 128Mb */

#define PCI_IO		0x08000000
#define PCI_IO_SIZE	0x02000000

#define PCI_MEM		0x0a000000
#define PCI_MEM_SIZE	0x02000000

#define GAL_RESET_REG_BASE	0xb4000000
#define GAL_REG_BASE	0x0c000000
#define GAL_REG_SIZE	0x01000000

#define EXPCS0		0x10000000	/* Daughterboard expansion area */
#define EXPCS0_SIZE	0x02000000	/* Daughterboard expansion area */

#define EXPCS1		0x12000000	/* Daughterboard expansion area */
#define EXPCS1_SIZE	0x02000000	/* Daughterboard expansion area */

#define EXPCS2		0x14000000	/* Daughterboard expansion area */
#define EXPCS2_SIZE	0x02000000	/* Daughterboard expansion area */

#define EXPCS3		0x18000000	/* Daughterboard expansion area */
#define EXPCS3_SIZE	0x07c00000	/* Daughterboard expansion area */

#define BOOT_PROM	0x1fc00000
#define BOOT_PROM_SIZE	0x00400000

#if defined(IOASIC_BASE)
#undef IOASIC_BASE
#endif
#define IOASIC_BASE	0x15000000
#define INTPLD_BASE	0x15080000
#define MISCPLD_BASE	0x15100000
#define WATCHDOG	0x15180000
#define UART0_BASE	0x15200000
#define UART1_BASE	0x15280000
#define PRT_BASE	0x15300000
#define ADC_BASE	0x15380000
#define SNDFIFO_BASE	0x15400000
#define NVRAM_BASE	0x15480000
#define IOASIC_RESET	0x15800000
#define PCI_RESET	0x15880000
#define LED_BASE	0x15900000


#define	INT_CONTROLLER_ADDRESS	0xb5080000

#define	INT_ENABLE_REG		(INT_CONTROLLER_ADDRESS+0)
#define	INT_MAPA_REG		(INT_CONTROLLER_ADDRESS+8)
#define	INT_MAPB_REG		(INT_CONTROLLER_ADDRESS+16)
#define	INT_ICAUSE_REG		(INT_CONTROLLER_ADDRESS+24)
#define	INT_ISTATUS_REG		(INT_CONTROLLER_ADDRESS+32)
#define	INT_GPSTAT_REG		(INT_CONTROLLER_ADDRESS+40)

/* Enable Register Bits */
#define	EXP_CON0_INT_ENABLE	0x0001
#define	EXP_CON1_INT_ENABLE	0x0002
#define	EXP_CON2_INT_ENABLE	0x0004
#define	EXP_CON3_INT_ENABLE	0x0008
#define	MISC_IO_CNT_INT_ENABLE	0x0010
#define	UART_16552_1_INT_ENABLE	0x0020
#define	UART_16552_2_INT_ENABLE	0x0040
#define	PAR_16552_INT_ENABLE	0x0080
#define	PCI_A_INT_ENABLE	0x0100
#define	PCI_B_INT_ENABLE	0x0200
#define	PCI_C_INT_ENABLE	0x0400
#define	PCI_D_INT_ENABLE	0x0800
#define	IDE_INT_ENABLE		0x1000
#define	IO_ASIC_INT_ENABLE	0x2000
#if (PHOENIX_SYS & SA1)
#define	A2D_INT_ENABLE		0x4000
#endif

/* Map Register Controls */
/* Galileo CPU Int is hard routed to CPU Int 0 */
/* Galileo PCI Int is hard routed to CPU Int 1 */
#define	ROUTE_INT_TO_2	0
#define	ROUTE_INT_TO_3	1
#define	ROUTE_INT_TO_4	2
#define	ROUTE_INT_TO_5	3

/* Map register A routing shifts */
#define	EXP_CON0_ROUTE_SHIFT		0
#define	EXP_CON1_ROUTE_SHIFT		2
#define	EXP_CON2_ROUTE_SHIFT		4
#define	EXP_CON3_ROUTE_SHIFT		6
#define	MISC_IO_CNT_ROUTE_SHIFT		8
#define	UART_16552_1_ROUTE_SHIFT	10
#define	UART_16552_2_ROUTE_SHIFT	12	
#define	PAR_16552_ROUTE_SHIFT		14

/* Map register B routing shifts */
#define	PCI_A_ROUTE_SHIFT		0
#define	PCI_B_ROUTE_SHIFT		2
#define	PCI_C_ROUTE_SHIFT		4
#define	PCI_D_ROUTE_SHIFT		6
#define	IDE_ROUTE_SHIFT			8
#define	IO_ASIC_ROUTE_SHIFT		10
#define	A2D_ROUTE_SHIFT			12

/* Cause & Status Register bits */
/* The difference is the cause register shows the interrupt if it is active */
/* and the interrupt is enabled.  The status register shows the interrupt */
/* source regardless of whether or not the interrupt is enabled. */
#define	EXP_CON0_INT_PENDING		0x0001
#define	EXP_CON1_INT_PENDING		0x0002
#define	EXP_CON2_INT_PENDING		0x0004
#define	EXP_CON3_INT_PENDING		0x0008
#define	MISC_IO_CNT_INT_PENDING		0x0010
#define	UART_16552_1_INT_PENDING	0x0020
#define	UART_16552_2_INT_PENDING	0x0040
#define	PAR_16552_INT_PENDING		0x0080
#define	PCI_A_INT_PENDING		0x0100
#define	PCI_B_INT_PENDING		0x0200
#define	PCI_C_INT_PENDING		0x0400
#define	PCI_D_INT_PENDING		0x0800
#define	IDE_INT_PENDING			0x1000
#define	IO_ASIC_INT_PENDING		0x2000
#define	A2D_INT_PENDING			0x4000

/* General Status Register Bits */
#define	AUX_OUT_0_11_RD			0x0fff
#define	WATCHDOG_NOT_STATUS		0x1000
#define	NMI_NOT_STATUS			0x2000
#define	BAT_NOT_LOW			0x4000
#define	SPARE				0x8000

#endif /* _PHOENIX7_ */
