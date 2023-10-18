//
// vegas.h - Phoenix seattle system specific definitions
//
// $Revision: 13 $
//
// This file should contain definitions specific to the Phoenix VEGAS system
// only.
//
#ifndef __VEGAS_H__
#define __VEGAS_H__

#define	VMM		// Install Virtual memory driver

#define	SERIAL_CHANNEL_TYPE		IOASIC
#define	SYSTEM_CONTROLLER			NILEIV
#define	TTY_DRIVER

#define	CS2_ADDR						0xa1000000
#define	CS3_ADDR						0xa1200000
#define	CS4_ADDR						0xa1400000
#define	CS5_ADDR						0xa1600000
#define	CS6_ADDR						0xa1800000
#define	CS7_ADDR						0xa1a00000
#define	CS8_ADDR						0xa2000000

// CPU Registers
#define	LED_ADDR						(CS5_ADDR + 0)
#define	CLK_MISC_ADDR				(CS5_ADDR + 1)
#define	JAMMA2_SENSE_ADDR			(CS5_ADDR + 2)
#define	CPU_RESET_ADDR				(CS5_ADDR + 3)

// CPU Reset Register bits
#define	CPU_GP_OUT_BIT				(1<<0)
#define	CPU_SIO_RST_BIT			(1<<1)

#define	PCIW0_ADDR					0x84000000
#define	PCIW1_ADDR					0x88000000

#define	PCI_IO_BASE_ADDR			(PCIW0_ADDR | 0xa0000000)

#define	PCI_MEM_BASE	(PCIW1_ADDR | 0xa0000000)
#define	PCI_IO_BASE		(PCIW0_ADDR | 0xa0000000)

// Bit of the PCI INIT control registers
#define	PCI_CMD_TYPE_MASK			(7<<1)
#define	PCI_INT_ACK_CMD_TYPE		(0<<1)
#define	PCI_IO_CMD_TYPE			(1<<1)
#define	PCI_RSVD_CMD_TYPE			(2<<1)
#define	PCI_MEM_CMD_TYPE			(3<<1)
#define	PCI_RSVD1_CMD_TYPE		(4<<1)
#define	PCI_CONFIG_CMD_TYPE		(5<<1)
#define	PCI_DUAL_CMD_TYPE			(6<<1)
#define	PCI_LINE_CMD_TYPE			(7<<1)

#define	PCI_ADDR_21_TO_31_MASK	(0x7ff<<21)

#define	PCI_32BIT					(1<<4)

#define	PCI_IDSEL_SHIFT			21

#define	NILE4_BASE_ADDR			0xbfa00000

#define	NILE4_PCI_INIT0_LO_ADDR	(NILE4_BASE_ADDR + 0xf0)
#define	NILE4_PCI_INIT0_HI_ADDR	(NILE4_BASE_ADDR + 0xf4)

#define	NILE4_PCI_INIT1_LO_ADDR	(NILE4_BASE_ADDR + 0xf8)
#define	NILE4_PCI_INIT1_HI_ADDR	(NILE4_BASE_ADDR + 0xfc)

#define	NILE4_PCI_CNTL_LO_ADDR	(NILE4_BASE_ADDR + 0xe0)
#define	NILE4_PCI_CNTL_HI_ADDR	(NILE4_BASE_ADDR + 0xe4)

#define	NILE4_INTERNAL_PCI		(NILE4_BASE_ADDR + 0x200)

#define	NILE4_PCIW0_LO_ADDR		(NILE4_BASE_ADDR + 0x60)

#define	NILE4_GPT_CNTL_LO_ADDR	(NILE4_BASE_ADDR + 0x1e0)
#define	NILE4_GPT_CNTL_HI_ADDR	(NILE4_BASE_ADDR + 0x1e4)
#define	NILE4_GPT_COUNT_LO_ADDR	(NILE4_BASE_ADDR + 0x1e8)

#define	NILE4_WDOG_CNTL_LO_ADDR	(NILE4_BASE_ADDR + 0x1f0)
#define	NILE4_WDOG_CNTL_HI_ADDR	(NILE4_BASE_ADDR + 0x1f4)
#define	NILE4_WDOG_COUNT_LO_ADDR	(NILE4_BASE_ADDR + 0x1f8)

#define	NILE4_INT_CTRL_LO_ADDR	(NILE4_BASE_ADDR + 0x88)
#define	NILE4_INT_CTRL_HI_ADDR	(NILE4_BASE_ADDR + 0x8c)

#define	NILE4_INT_STAT0_LO_ADDR	(NILE4_BASE_ADDR + 0x90)
#define	NILE4_INT_STAT0_HI_ADDR	(NILE4_BASE_ADDR + 0x94)
#define	NILE4_INT_STAT1_LO_ADDR	(NILE4_BASE_ADDR + 0x98)
#define	NILE4_INT_STAT1_HI_ADDR	(NILE4_BASE_ADDR + 0x9c)

#define	NILE4_PCI_INT_CTRL_ADDR	(NILE4_BASE_ADDR + 0xa8)
#define	NILE4_INT_CLR_ADDR		(NILE4_BASE_ADDR + 0xa0)

#define	NILE4_PCI_BAR0_ADDR		(NILE4_BASE_ADDR + 0x218)
#define	NILE4_PCI_BAR1_ADDR		(NILE4_BASE_ADDR + 0x220)
#define	NILE4_PCI_BAR2_ADDR		(NILE4_BASE_ADDR + 0x240)
#define	NILE4_PCI_BAR3_ADDR		(NILE4_BASE_ADDR + 0x248)
#define	NILE4_PCI_BAR4_ADDR		(NILE4_BASE_ADDR + 0x250)
#define	NILE4_PCI_BAR5_ADDR		(NILE4_BASE_ADDR + 0x258)
#define	NILE4_PCI_BAR6_ADDR		(NILE4_BASE_ADDR + 0x260)
#define	NILE4_PCI_BAR7_ADDR		(NILE4_BASE_ADDR + 0x268)
#define	NILE4_PCI_BAR8_ADDR		(NILE4_BASE_ADDR + 0x270)
#define	NILE4_PCI_BARB_ADDR		(NILE4_BASE_ADDR + 0x278)
#define	NILE4_PCI_BARC_ADDR		(NILE4_BASE_ADDR + 0x210)

#define	NILE4_PCI_ARBLO_ADDR		(NILE4_BASE_ADDR + 0x0e8)
#define	NILE4_PCI_ARBHI_ADDR		(NILE4_BASE_ADDR + 0x0ec)

#define	NILE4_PCI_COMMAND_ADDR	(NILE4_BASE_ADDR + 0x204)

#define	NILE4_UART_THR				(NILE4_BASE_ADDR + 0x300)
#define	NILE4_UART_RBR				(NILE4_BASE_ADDR + 0x300)
#define	NILE4_UART_DLL				(NILE4_BASE_ADDR + 0x300)
#define	NILE4_UART_IER				(NILE4_BASE_ADDR + 0x308)
#define	NILE4_UART_DLM				(NILE4_BASE_ADDR + 0x308)
#define	NILE4_UART_IIR				(NILE4_BASE_ADDR + 0x310)
#define	NILE4_UART_FCR				(NILE4_BASE_ADDR + 0x310)
#define	NILE4_UART_LCR				(NILE4_BASE_ADDR + 0x318)
#define	NILE4_UART_MCR				(NILE4_BASE_ADDR + 0x320)
#define	NILE4_UART_LSR				(NILE4_BASE_ADDR + 0x328)
#define	NILE4_UART_MSR				(NILE4_BASE_ADDR + 0x330)
#define	NILE4_UART_SCR				(NILE4_BASE_ADDR + 0x338)

/* Interrupt enable register */
#define	UIER_ERBI					0x01
#define	UIER_ETBEI					0x02
#define	UIER_ELSI					0x04
#define	UIER_EDSSI					0x08

/* Interrupt ID register */
#define	UIIR_PEND					0x01
#define	UIIR_RLS						0x06
#define	UIIR_RDA						0x04
#define	UIIR_CTI						0x0C
#define	UIIR_THRE					0x02
#define	UIIR_MS						0x00

/* FIFO Control Register bits */
#define	UFCR_ENAB					0x01
#define	UFCR_RRST					0x02
#define	UFCR_TRST					0x04
#define	UFCR_LEV1					0x00
#define	UFCR_LEV4					0x40
#define	UFCR_LEV8					0x80
#define	UFCR_LEV14  				0xC0

/* Line Control register bits */
#define	ULCR_5BIT					0x00
#define	ULCR_6BIT					0x01
#define	ULCR_7BIT					0x02
#define	ULCR_8BIT					0x03
#define	ULCR_STB						0x04
#define	ULCR_PEN						0x08
#define	ULCR_EPS						0x10
#define	ULCR_FORCE					0x20
#define	ULCR_BRK						0x40
#define	ULCR_DLAB					0x80

/* Modem Control Register bits */
#define	UMCR_DTR						0x01
#define	UMCR_RTS						0x02
#define	UMCR_OUT1					0x04
#define	UMCR_OUT2					0x08
#define	UMCR_LOOP					0x10

/* Line status register bits */
#define	ULSR_DR						0x01
#define	ULSR_OE						0x02
#define	ULSR_PE						0x04
#define	ULSR_FE						0x08
#define	ULSR_BI						0x10
#define	ULSR_THRE					0x20
#define	ULSR_TEMT					0x40
#define	ULSR_RFERR					0x80

#define	ULSR_ERRMASK				(ULSR_OE|ULSR_PE|ULSR_FE|ULSR_BI)

/* Modem status register */
#define	UMSR_DCTS					0x01
#define	UMSR_DDSR					0x02
#define	UMSR_TERI					0x04
#define	UMSR_DDCD					0x08
#define	UMSR_CTS						0x10
#define	UMSR_DSR						0x20
#define	UMSR_RI						0x40
#define	UMSR_DCD						0x80

/* Scratch register */
#define	USC_RESET					0x100

#define	N4_DCS2_LO					(NILE4_BASE_ADDR + 0x0010)
#define	N4_LCST2					  	(NILE4_BASE_ADDR + 0x0110)

#define	N4_PDAR_WIDTH_MASK		0x000000C0
#define	N4_PDAR_32BIT				0x00000080
#define	N4_LCL_CSON					0x00000001
#define	N4_LCL_CSPOL_NEG			0x00000000
#define	N4_LCL_CONPOL_NEG			0x00000000

#define	N4_LCL_CONSET_SHIFT		1
#define	N4_LCL_CONWID_SHIFT		3
#define	N4_LCL_SUBSCWID_SHIFT	9
#define	N4_LCL_CSOFF_SHIFT		15
#define	N4_LCL_COFHOLD_SHIFT		17
#define	N4_LCL_BUSIDLE_SHIFT		19
#define	N4_LCL_CONOFF_SHIFT		24

#define	VEGAS_SIOPLD_CONFIG		(CS5_ADDR + 0x01)
#define	VEGAS_SIOPLD_STATUS		(CS5_ADDR + 0x02)

#define	SIOPLD_CONFIGN				0x01
#define	SIOPLD_STATUS				0x02
#define	SIOPLD_CFGDONE				0x01

#define	VEGAS_RESET_LOCALBUS_REV_1	0x01
#define	VEGAS_RESET_LOCALBUS_REV_0	0x02
#define	VEGAS_RESET_IDE				0x02
#define	VEGAS_RESET_PCI				0x04
#define	VEGAS_RESET_LOCALBUS			VEGAS_RESET_LOCALBUS_REV_0

#define	RESET_REG_ADDR				(CS2_ADDR + (0 << 12))
#define	INT_ENBL_REG_ADDR			(CS2_ADDR + (1 << 12))
#define	INT_CAUSE_REG_ADDR		(CS2_ADDR + (2 << 12))
#define	INT_STATUS_REG_ADDR		(CS2_ADDR + (3 << 12))
#define	LED_STATUS_REG_ADDR		(CS2_ADDR + (4 << 12))

// Interrupt enables for SIO board
#define	SIO_WATCHDOG_INT_ENABLE	(1<<0)
#define	SIO_A2D_INT_ENABLE		(1<<1)
#define	SIO_IOASIC_INT_ENABLE	(1<<2)
#define	SIO_HILINK_INT_ENABLE	(1<<3)
#define	SIO_NSS_INT_ENABLE		SIO_HILINK_INT_ENABLE
#define	SIO_WIDGET_INT_ENABLE	SIO_HILINK_INT_ENABLE
#define	SIO_ETHERNET_INT_ENABLE	(1<<4)
#define	SIO_VSYNC_INT_ENABLE		(1<<5)

#define	VEGAS_DCS2_BASE			CS2_ADDR

#define	VEGAS_RESET_REG			RESET_REG_ADDR

#define	GUN_BASE_ADDR				(CS2_ADDR + (5 << 12))
#define	GUN1_HLOW_ADDR				(GUN_BASE_ADDR + 0)
#define	GUN1_HHIGH_ADDR			(GUN_BASE_ADDR + 1)
#define	GUN1_VLOW_ADDR				(GUN_BASE_ADDR + 2)
#define	GUN1_VHIGH_ADDR			(GUN_BASE_ADDR + 3)
#define	GUN2_HLOW_ADDR				(GUN_BASE_ADDR + 4)
#define	GUN2_HHIGH_ADDR			(GUN_BASE_ADDR + 5)
#define	GUN2_VLOW_ADDR				(GUN_BASE_ADDR + 6)
#define	GUN2_VHIGH_ADDR			(GUN_BASE_ADDR + 7)

#define	CMOS_UNLOCK_ADDR			(CS2_ADDR + (6 << 12))

#define	WATCHDOG_ADDR				(CS2_ADDR + (7 << 12))

#define	A2D_ADDR						(CS3_ADDR + (0 << 12))

#define	LCD_ADDR						(CS3_ADDR + (1 << 12))

#define	CMOS_RAM_ADDR				(CS4_ADDR + (0 << 12))

#define	CMOS_CELL_SPACE			1

#define	CMOS_SIZE					(32768 - 16)

#define	RTC_FLAGS_REG				(CS4_ADDR + CMOS_SIZE)
#define	RTC_ALARM_YEAR_REG		(RTC_FLAGS_REG + 1)
#define	RTC_ALARM_SECONDS_REG	(RTC_ALARM_YEAR_REG + 1)
#define	RTC_ALARM_MINUTES_REG	(RTC_ALARM_SECONDS_REG + 1)
#define	RTC_ALARM_HOURS_REG		(RTC_ALARM_MINUTES_REG + 1)
#define	RTC_ALARM_DATE_REG		(RTC_ALARM_HOURS_REG + 1)
#define	RTC_INT_ENABLE_REG		(RTC_ALARM_DATE_REG + 1)
#define	RTC_WATCHDOG_REG			(RTC_INT_ENABLE_REG + 1)
#define	RTC_CONTROL_REG			(RTC_WATCHDOG_REG + 1)
#define	RTC_SECONDS_REG			(RTC_CONTROL_REG + 1)
#define	RTC_MINUTES_REG			(RTC_SECONDS_REG + 1)
#define	RTC_HOURS_REG				(RTC_MINUTES_REG + 1)
#define	RTC_DAY_REG					(RTC_HOURS_REG + 1)
#define	RTC_DATE_REG				(RTC_DAY_REG + 1)
#define	RTC_MONTH_REG				(RTC_DATE_REG + 1)
#define	RTC_YEAR_REG				(RTC_MONTH_REG + 1)

// RTC Control Reg Bits
#define	RTC_W_BIT					(1 << 7)
#define	RTC_R_BIT					(1 << 6)
#define	RTC_S_BIT					(1 << 5)

// RTC Seconds Bits
#define	RTC_ST_BIT					(1 << 7)

// RTC Day Bits
#define	RTC_FT_BIT					(1 << 6)


// Base Address of the I/O ASIC
#define	IO_ASIC_BASE				(CS6_ADDR + (0 << 12))

#define	IOASIC_RESET_ADDR			RESET_REG_ADDR

#define	SOUND_FIFO_ADDR			(CS6_ADDR + (1 << 12))

#define	NSS_EXP_ADDR				(CS7_ADDR + (0 << 12))

#define	ETHER_ADDR					(CS7_ADDR + (1 << 12))


// I/O ASIC Registers (I/O Map 0)
#define	IOASIC_MAP0_DIP_SWITCHES		(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP0_MISC_INPUT			(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP0_COIN_INPUT			(IOASIC_MAP0_MISC_INPUT)
#define	IOASIC_MAP0_PLAYER_12			(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP0_PLAYER_34			(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP0_UART_CONTROL		(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP0_UART_TX				(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP0_UART_RX				(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP0_UART_STATUS			(IOASIC_MAP0_UART_RX)
#define	IOASIC_MAP0_COIN_METERS			(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP0_SOUND_CONTROL		(IO_ASIC_BASE + (0x8<<1))
#define	IOASIC_MAP0_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP0_SOUND_STATUS		(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP0_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP0_PIC_COMMAND			(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP0_PIC_DATA_IN			(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP0_STATUS				(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP0_CONTROL				(IO_ASIC_BASE + (0xf<<1))

// I/O ASIC Registers (I/O Map 1)
#define	IOASIC_MAP1_DIP_SWITCHES		(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP1_MISC_INPUT			(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP1_COIN_INPUT			(IOASIC_MAP1_MISC_INPUT)
#define	IOASIC_MAP1_PLAYER_12			(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP1_PLAYER_34			(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP1_UART_CONTROL		(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP1_UART_TX				(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP1_UART_RX				(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP1_UART_STATUS			(IOASIC_MAP1_UART_RX)
#define	IOASIC_MAP1_COIN_METERS			(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP1_SOUND_CONTROL		(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP1_SOUND_DATA_OUT		(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP1_SOUND_STATUS		(IO_ASIC_BASE + (0x8<<1))
#define	IOASIC_MAP1_SOUND_DATA_IN		(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP1_PIC_COMMAND			(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP1_PIC_DATA_IN			(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP1_STATUS				(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP1_CONTROL				(IO_ASIC_BASE + (0xf<<1))

// I/O ASIC Registers (I/O Map 2)
#define	IOASIC_MAP2_DIP_SWITCHES		(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP2_MISC_INPUT			(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP2_COIN_INPUT			(IOASIC_MAP2_MISC_INPUT)
#define	IOASIC_MAP2_PLAYER_12			(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP2_PLAYER_34			(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP2_UART_CONTROL		(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP2_UART_TX				(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP2_UART_RX				(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP2_UART_STATUS			(IOASIC_MAP2_UART_RX)
#define	IOASIC_MAP2_COIN_METERS			(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP2_SOUND_CONTROL		(IO_ASIC_BASE + (0x8<<1))
#define	IOASIC_MAP2_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP2_SOUND_STATUS		(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP2_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP2_PIC_COMMAND			(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP2_PIC_DATA_IN			(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP2_STATUS				(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP2_CONTROL				(IO_ASIC_BASE + (0xf<<1))

// I/O ASIC Registers (I/O Map 3)
#define	IOASIC_MAP3_DIP_SWITCHES		(IO_ASIC_BASE + (0xf<<1))
#define	IOASIC_MAP3_MISC_INPUT			(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP3_COIN_INPUT			(IOASIC_MAP3_MISC_INPUT)
#define	IOASIC_MAP3_PLAYER_12			(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP3_PLAYER_34			(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP3_UART_CONTROL		(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP3_UART_TX				(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP3_UART_RX				(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP3_UART_STATUS			(IOASIC_MAP3_UART_RX)
#define	IOASIC_MAP3_COIN_METERS			(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP3_SOUND_CONTROL		(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP3_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x8<<1))
#define	IOASIC_MAP3_SOUND_STATUS		(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP3_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP3_PIC_COMMAND			(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP3_PIC_DATA_IN			(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP3_STATUS				(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP3_CONTROL				(IO_ASIC_BASE + (0x0<<1))

// I/O ASIC Registers (I/O Map 4)
#define	IOASIC_MAP4_DIP_SWITCHES		(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP4_MISC_INPUT			(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP4_COIN_INPUT			(IOASIC_MAP4_MISC_INPUT)
#define	IOASIC_MAP4_PLAYER_12			(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP4_PLAYER_34			(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP4_UART_CONTROL		(IO_ASIC_BASE + (0xf<<1))
#define	IOASIC_MAP4_UART_TX				(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP4_UART_RX				(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP4_UART_STATUS			(IOASIC_MAP4_UART_RX)
#define	IOASIC_MAP4_COIN_METERS			(IO_ASIC_BASE + (0x8<<1))
#define	IOASIC_MAP4_SOUND_CONTROL		(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP4_SOUND_DATA_OUT		(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP4_SOUND_STATUS		(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP4_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP4_PIC_COMMAND			(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP4_PIC_DATA_IN			(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP4_STATUS				(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP4_CONTROL				(IO_ASIC_BASE + (0x3<<1))

// I/O ASIC Registers (I/O Map 5)
#define	IOASIC_MAP5_DIP_SWITCHES		(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP5_MISC_INPUT			(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP5_COIN_INPUT			(IOASIC_MAP5_MISC_INPUT)
#define	IOASIC_MAP5_PLAYER_12			(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP5_PLAYER_34			(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP5_UART_CONTROL		(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP5_UART_TX				(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP5_UART_RX				(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP5_UART_STATUS			(IOASIC_MAP5_UART_RX)
#define	IOASIC_MAP5_COIN_METERS			(IO_ASIC_BASE + (0xf<<1))
#define	IOASIC_MAP5_SOUND_CONTROL		(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP5_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP5_SOUND_STATUS		(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP5_SOUND_DATA_IN		(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP5_PIC_COMMAND			(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP5_PIC_DATA_IN			(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP5_STATUS				(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP5_CONTROL				(IO_ASIC_BASE + (0x8<<1))

// I/O ASIC Registers (I/O Map 6)
#define	IOASIC_MAP6_DIP_SWITCHES		(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP6_MISC_INPUT			(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP6_COIN_INPUT			(IOASIC_MAP6_MISC_INPUT)
#define	IOASIC_MAP6_PLAYER_12			(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP6_PLAYER_34			(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP6_UART_CONTROL		(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP6_UART_TX				(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP6_UART_RX				(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP6_UART_STATUS			(IOASIC_MAP6_UART_RX)
#define	IOASIC_MAP6_COIN_METERS			(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP6_SOUND_CONTROL		(IO_ASIC_BASE + (0xc<<1))
#define	IOASIC_MAP6_SOUND_DATA_OUT		(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP6_SOUND_STATUS		(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP6_SOUND_DATA_IN		(IO_ASIC_BASE + (0xf<<1))
#define	IOASIC_MAP6_PIC_COMMAND			(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP6_PIC_DATA_IN			(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP6_STATUS				(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP6_CONTROL				(IO_ASIC_BASE + (0x8<<1))

// I/O ASIC Registers (I/O Map 7)
#define	IOASIC_MAP7_DIP_SWITCHES		(IO_ASIC_BASE + (0xb<<1))
#define	IOASIC_MAP7_MISC_INPUT			(IO_ASIC_BASE + (0xa<<1))
#define	IOASIC_MAP7_COIN_INPUT			(IOASIC_MAP7_MISC_INPUT)
#define	IOASIC_MAP7_PLAYER_12			(IO_ASIC_BASE + (0x9<<1))
#define	IOASIC_MAP7_PLAYER_34			(IO_ASIC_BASE + (0x8<<1))
#define	IOASIC_MAP7_UART_CONTROL		(IO_ASIC_BASE + (0x0<<1))
#define	IOASIC_MAP7_UART_TX				(IO_ASIC_BASE + (0x1<<1))
#define	IOASIC_MAP7_UART_RX				(IO_ASIC_BASE + (0x2<<1))
#define	IOASIC_MAP7_UART_STATUS			(IOASIC_MAP7_UART_RX)
#define	IOASIC_MAP7_COIN_METERS			(IO_ASIC_BASE + (0x3<<1))
#define	IOASIC_MAP7_SOUND_CONTROL		(IO_ASIC_BASE + (0x7<<1))
#define	IOASIC_MAP7_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x6<<1))
#define	IOASIC_MAP7_SOUND_STATUS		(IO_ASIC_BASE + (0x5<<1))
#define	IOASIC_MAP7_SOUND_DATA_IN		(IO_ASIC_BASE + (0x4<<1))
#define	IOASIC_MAP7_PIC_COMMAND			(IO_ASIC_BASE + (0xf<<1))
#define	IOASIC_MAP7_PIC_DATA_IN			(IO_ASIC_BASE + (0xe<<1))
#define	IOASIC_MAP7_STATUS				(IO_ASIC_BASE + (0xd<<1))
#define	IOASIC_MAP7_CONTROL				(IO_ASIC_BASE + (0xc<<1))



// I/O ASIC Registers
#if	defined(NFL)
#define	IOASIC_DIP_SWITCHES		IOASIC_MAP7_DIP_SWITCHES
#define	IOASIC_MISC_INPUT			IOASIC_MAP7_MISC_INPUT
#define	IOASIC_COIN_INPUT			IOASIC_MAP7_COIN_INPUT
#define	IOASIC_PLAYER_12			IOASIC_MAP7_PLAYER_12
#define	IOASIC_PLAYER_34			IOASIC_MAP7_PLAYER_34
#define	IOASIC_UART_CONTROL		IOASIC_MAP7_UART_CONTROL
#define	IOASIC_UART_TX				IOASIC_MAP7_UART_TX
#define	IOASIC_UART_RX				IOASIC_MAP7_UART_RX
#define	IOASIC_UART_STATUS		IOASIC_MAP7_UART_STATUS
#define	IOASIC_COIN_METERS		IOASIC_MAP7_COIN_METERS
#define	IOASIC_SOUND_CONTROL		IOASIC_MAP7_SOUND_CONTROL
#define	IOASIC_SOUND_DATA_OUT	IOASIC_MAP7_SOUND_DATA_OUT
#define	IOASIC_SOUND_STATUS		IOASIC_MAP7_SOUND_STATUS
#define	IOASIC_SOUND_DATA_IN		IOASIC_MAP7_SOUND_DATA_IN
#define	IOASIC_PIC_COMMAND		IOASIC_MAP7_PIC_COMMAND
#define	IOASIC_PIC_DATA_IN		IOASIC_MAP7_PIC_DATA_IN
#define	IOASIC_STATUS				IOASIC_MAP7_STATUS
#define	IOASIC_CONTROL				IOASIC_MAP7_CONTROL

#elif	defined(SPACE)
#define	IOASIC_DIP_SWITCHES		IOASIC_MAP1_DIP_SWITCHES
#define	IOASIC_MISC_INPUT			IOASIC_MAP1_MISC_INPUT
#define	IOASIC_COIN_INPUT			IOASIC_MAP1_COIN_INPUT
#define	IOASIC_PLAYER_12			IOASIC_MAP1_PLAYER_12
#define	IOASIC_PLAYER_34			IOASIC_MAP1_PLAYER_34
#define	IOASIC_UART_CONTROL		IOASIC_MAP1_UART_CONTROL
#define	IOASIC_UART_TX				IOASIC_MAP1_UART_TX
#define	IOASIC_UART_RX				IOASIC_MAP1_UART_RX
#define	IOASIC_UART_STATUS		IOASIC_MAP1_UART_STATUS
#define	IOASIC_COIN_METERS		IOASIC_MAP1_COIN_METERS
#define	IOASIC_SOUND_CONTROL		IOASIC_MAP1_SOUND_CONTROL
#define	IOASIC_SOUND_DATA_OUT	IOASIC_MAP1_SOUND_DATA_OUT
#define	IOASIC_SOUND_STATUS		IOASIC_MAP1_SOUND_STATUS
#define	IOASIC_SOUND_DATA_IN		IOASIC_MAP1_SOUND_DATA_IN
#define	IOASIC_PIC_COMMAND		IOASIC_MAP1_PIC_COMMAND
#define	IOASIC_PIC_DATA_IN		IOASIC_MAP1_PIC_DATA_IN
#define	IOASIC_STATUS				IOASIC_MAP1_STATUS
#define	IOASIC_CONTROL				IOASIC_MAP1_CONTROL

#elif defined(CARN)
#define	IOASIC_DIP_SWITCHES		IOASIC_MAP6_DIP_SWITCHES
#define	IOASIC_MISC_INPUT			IOASIC_MAP6_MISC_INPUT
#define	IOASIC_COIN_INPUT			IOASIC_MAP6_COIN_INPUT
#define	IOASIC_PLAYER_12			IOASIC_MAP6_PLAYER_12
#define	IOASIC_PLAYER_34			IOASIC_MAP6_PLAYER_34
#define	IOASIC_UART_CONTROL		IOASIC_MAP6_UART_CONTROL
#define	IOASIC_UART_TX				IOASIC_MAP6_UART_TX
#define	IOASIC_UART_RX				IOASIC_MAP6_UART_RX
#define	IOASIC_UART_STATUS		IOASIC_MAP6_UART_STATUS
#define	IOASIC_COIN_METERS		IOASIC_MAP6_COIN_METERS
#define	IOASIC_SOUND_CONTROL		IOASIC_MAP6_SOUND_CONTROL
#define	IOASIC_SOUND_DATA_OUT	IOASIC_MAP6_SOUND_DATA_OUT
#define	IOASIC_SOUND_STATUS		IOASIC_MAP6_SOUND_STATUS
#define	IOASIC_SOUND_DATA_IN		IOASIC_MAP6_SOUND_DATA_IN
#define	IOASIC_PIC_COMMAND		IOASIC_MAP6_PIC_COMMAND
#define	IOASIC_PIC_DATA_IN		IOASIC_MAP6_PIC_DATA_IN
#define	IOASIC_STATUS				IOASIC_MAP6_STATUS
#define	IOASIC_CONTROL				IOASIC_MAP6_CONTROL

#else
#define	IOASIC_DIP_SWITCHES		IOASIC_MAP0_DIP_SWITCHES
#define	IOASIC_MISC_INPUT			IOASIC_MAP0_MISC_INPUT
#define	IOASIC_COIN_INPUT			IOASIC_MAP0_COIN_INPUT
#define	IOASIC_PLAYER_12			IOASIC_MAP0_PLAYER_12
#define	IOASIC_PLAYER_34			IOASIC_MAP0_PLAYER_34
#define	IOASIC_UART_CONTROL		IOASIC_MAP0_UART_CONTROL
#define	IOASIC_UART_TX				IOASIC_MAP0_UART_TX
#define	IOASIC_UART_RX				IOASIC_MAP0_UART_RX
#define	IOASIC_UART_STATUS		IOASIC_MAP0_UART_STATUS
#define	IOASIC_COIN_METERS		IOASIC_MAP0_COIN_METERS
#define	IOASIC_SOUND_CONTROL		IOASIC_MAP0_SOUND_CONTROL
#define	IOASIC_SOUND_DATA_OUT	IOASIC_MAP0_SOUND_DATA_OUT
#define	IOASIC_SOUND_STATUS		IOASIC_MAP0_SOUND_STATUS
#define	IOASIC_SOUND_DATA_IN		IOASIC_MAP0_SOUND_DATA_IN
#define	IOASIC_PIC_COMMAND		IOASIC_MAP0_PIC_COMMAND
#define	IOASIC_PIC_DATA_IN		IOASIC_MAP0_PIC_DATA_IN
#define	IOASIC_STATUS				IOASIC_MAP0_STATUS
#define	IOASIC_CONTROL				IOASIC_MAP0_CONTROL
#endif

// Reset Register bits
#define	RST_IOASIC					(1<<0)
#define	PIC_RST_BIT					(RST_IOASIC)
#define	SOUND_RST_BIT				(RST_IOASIC)
#define	HILINK_RST_BIT				(1<<1)
#define	NSS_RST_BIT					(HILINK_RST_BIT)
#define	ETHER_RST_BIT				(1<<2)
#define	VSYNC_RST_BIT				(1<<3)
#define	VSYNC_POLARITY_POS		(1<<4)

// PCI Slot definitions
#define	PCI0646_DEVICE_NUMBER	5

#define	IDE_DISK_INT				IP_2
#define	SIO_INT						IP_3
#define	NILE4_INT					IP_4
#define	SCSI_INT						IP_6
#define	DEBUG_SW_INT				SCSI_INT


// Definitions of stuff for the SMC Ethernet controller
#define	SMC_EPH_STATUS_REG			0
#define	SMC_TRANSMIT_CONTROL_REG	1
#define	SMC_INTERRUPT_STATUS_REG	2
#define	SMC_INTERRUPT_MASK_REG		3

// Definitions of bits int the EPH Status Register of the SMC Ethernet Chip
#define	TX_SUC_BIT					(1<<0)
#define	SNGLCOL_BIT					(1<<1)
#define	MULCOL_BIT					(1<<2)
#define	LTX_MULT_BIT				(1<<3)
#define	SIXTEENCOL_BIT				(1<<4)
#define	SQET_BIT						(1<<5)
#define	LTX_BRD_BIT					(1<<6)
#define	TX_DEFR_BIT					(1<<7)
#define	LATCOL_BIT					(1<<9)
#define	LOST_CAR_BIT				(1<<10)
#define	EXC_DEF_BIT					(1<<11)
#define	CTR_ROL_BIT					(1<<12)
#define	RX_OVRN_BIT					(1<<13)
#define	LINK_OK_BIT					(1<<14)
#define	TXUNRN_BIT					(1<<15)

// Definitions of bits in the Transmit Control register of the SMC Chip
#define	TXENA_BIT					(1<<0)
#define	LOOP_BIT						(1<<1)
#define	FORCOL_BIT					(1<<2)
#define	PAD_EN_BIT					(1<<7)
#define	NOCRC_BIT					(1<<8)
#define	MON_CSN_BIT					(1<<10)
#define	FDUPLX_BIT					(1<<11)
#define	STP_SQET_BIT				(1<<12)
#define	EPH_LOOP_BIT				(1<<13)

// Definitions of bits in the interrupt status register of the SMC Chip
#define	RX_INT_BIT					(1<<0)
#define	TX_INT_BIT					(1<<1)
#define	TX_EMPTY_INT_BIT			(1<<2)
#define	ALLOC_INT_BIT				(1<<3)
#define	RX_OVRN_INT_BIT			(1<<4)
#define	EPH_INT_BIT					(1<<5)
#define	ERCV_INT_BIT				(1<<6)

// Definitions of bits in the interrupt acknowledge register of the SMC Chip
#define	TX_INT_ACK_BIT				(1<<1)
#define	TX_EMPTY_INT_ACK_BIT		(1<<2)
#define	RX_OVRN_INT_ACK_BIT		(1<<4)
#define	ERCV_INT_ACK_BIT			(1<<6)

// Definitions of bits in the interrupt mask register of the SMC Chip
#define	RX_INT_ENABLE_BIT			(1<<0)
#define	TX_INT_ENABLE_BIT			(1<<1)
#define	TX_EMPTY_INT_ENABLE_BIT	(1<<2)
#define	ALLOC_INT_ENABLE_BIT		(1<<3)
#define	RX_OVRN_INT_ENABLE_BIT	(1<<4)
#define	EPH_INT_ENABLE_BIT		(1<<5)
#define	ERCV_INT_ENABLE_BIT		(1<<6)

// Defintions of PCI Clock Speeds
#define	PCI_CLOCK_SPEED_1_3			0		// 1/3 Sysclock rate
#define	PCI_CLOCK_SPEED_2_3			1		// 2/3 Sysclock rate
#define	PCI_CLOCK_SPEED_1_2			2		// 1/2 Sysclock rate
#define	PCI_CLOCK_SPEED_1_1			3		// Sysclock rate
#define	PCI_CLOCK_SPEED_EXTERNAL	4		// External rate

// 30 MHz for 180 MHz Proc - 26.667 Mhz for 160 Mhz Proc - 33.33 Mhz for 200 Mhz Proc
#define	PCI_CLOCK_SPEED				PCI_CLOCK_SPEED_1_3

//#define	PROCESSOR_CLOCK_SPEED		90000000
#define	PROCESSOR_CLOCK_SPEED		100000000
#define	NANOS_PER_TICK					(1000000000/PROCESSOR_CLOCK_SPEED)

#define	PAGE_SIZE	(1<<16)		// 65536 byte pages
#define	PAGE_MASK	(0xf<<13)


//
// Definitions for stuff to deal with virtual memory managment
//

#define	VIRTUAL_MEMORY_SIZE	(1<<25)	// 32 Megabytes
#define	MAX_TLB_ENTRIES		48			// Maximum TLB entries in processor
//#define	VMM_PAGE_SIZE			65536		// Page frame size
#define	VMM_PAGE_SIZE			16384		// Page frame size
//#define	VMM_PAGE_SIZE			4096		// Page frame size

//#if (VMM_PAGE_SIZE == 4096)
//#define	PAGE_MASK_BITS	0
//#define	VMM_PAGE_MASK		0x0000
//#elseif (VMM_PAGE_SIZE == 16384)
#define	PAGE_MASK_BITS	2
#define	VMM_PAGE_MASK		0x0003
//#elseif (VMM_PAGE_SIZE == 65536)
//#define	PAGE_MASK_BITS	4
//#define	VMM_PAGE_MASK		0x000f
//#elseif (VMM_PAGE_SIZE == 262144)
//#define	PAGE_MASK_BITS	6
//#define	VMM_PAGE_MASK		0x003f
//#elseif (VMM_PAGE_SIZE == 1048576)
//#define	PAGE_MASK_BITS	8
//#define	VMM_PAGE_MASK		0x00ff
//#elseif (VMM_PAGE_SIZE == 4194304)
//#define	PAGE_MASK_BITS	10
//#define	VMM_PAGE_MASK		0x03ff
//#elseif (VMM_PAGE_SIZE == 16777216)
//#define	PAGE_MASK_BITS	12
//#define	VMM_PAGE_MASK		0x0fff
//#else
//#error Invalid VMM Page Size
//#endif

#define	VPN_BITS				(19 - PAGE_MASK_BITS)
#define	VPN_SHIFT			(13 + PAGE_MASK_BITS)
#define	ASID_BITS			VPN_SHIFT
//#define	PFN_BITS			(VPN_BITS - 1)
#define	PFN_BITS				(24 - PAGE_MASK_BITS)
#define	PFN_UNUSED_BITS	PAGE_MASK_BITS
#define	PFN_SHIFT			(VPN_SHIFT - 1)
//#define	VPN_SIZE			(1 << VPN_BITS)
//#define	PFN_SIZE			(1 << PFN_BITS)
#define	PF_SIZE				VMM_PAGE_SIZE
#define	VP_SIZE				(PF_SIZE << 1)
#define	PF_MASK				(PF_SIZE - 1)
#define	VP_MASK				(VP_SIZE - 1)

typedef struct entry_hi
{
	unsigned asid	: ASID_BITS;	// Not used
	unsigned vpn2	: VPN_BITS;		// Virtual page number / 2
} entry_hi_t;

typedef struct entry_lo
{
	unsigned g		: 1;						// Global
	unsigned v		: 1;						// Valid
	unsigned d		: 1;						// Dirty
	unsigned c		: 3;						// Cache algorithm
#if PAGE_MASK_BITS
	unsigned nu1	: PFN_UNUSED_BITS;	// Not used
#endif
	unsigned pfn	: PFN_BITS;				// Page frame number
	unsigned	nu2	: 2;						// Not used
} entry_lo_t;

typedef struct tlb_entry
{
	entry_hi_t	entry_hi;
	entry_lo_t	entry_lo0;
	entry_lo_t	entry_lo1;
} tlb_entry_t;

#define	PF_VALID			1
#define	PF_DIRTY			1
#define	PF_WRITEABLE	PF_DIRTY
#define	PF_GLOBAL		1
#define	PF_NONCOHERENT	3

//
// Virtual page states
//
#define	VP_IN_USE		0
#define	VP_IN_MEMORY	1
#define	VP_ON_DISK		2
#define	VP_NOT_USED		3

#define	VP_STATE_SHIFT	30

#define	VP_STATE_MASK	(3<<VP_STATE_SHIFT)

#define	DO_SWAP			1
#define	NO_SWAP			0

//
// Deifinitions of register offsets in save area
//

// CPU general purpose registers
#define	GP0	0
#define	GP1	2
#define	GP2	4
#define	GP3	6
#define	GP4	8
#define	GP5	10
#define	GP6	12
#define	GP7	14
#define	GP8	16
#define	GP9	18
#define	GP10	20
#define	GP11	22
#define	GP12	24
#define	GP13	26
#define	GP14	28
#define	GP15	30
#define	GP16	32
#define	GP17	34
#define	GP18	36
#define	GP19	38
#define	GP20	40
#define	GP21	42
#define	GP22	44
#define	GP23	46
#define	GP24	48
#define	GP25	50
#define	GP26	52
#define	GP27	54
#define	GP28	56
#define	GP29	58
#define	GP30	60
#define	GP31	62


// Integer multiply/divide registers
#define	MFLO	64
#define	MFHI	66

// The program counter
#define	PC		68

// Coprocessor 0 registers
#define	CP0_INDEX		70
#define	CP0_RANDOM		72
#define	CP0_ENTRYLO0	74
#define	CP0_ENTRYHI1	76
#define	CP0_CONTEXT		78
#define	CPO_PAGEMASK	80
#define	CP0_WIRED		82
#define	CP0_7				84
#define	CP0_BADVADDR	86
#define	CPO_COUNT		88
#define	CP0_ENTRYHI		90
#define	CPO_COMPARE		92
#define	CP0_STATUS		94
#define	CP0_CAUSE		96
#define	CP0_EPC			98
#define	CP0_PRID			100
#define	CP0_CONFIG		102
#define	CP0_LLADDR		104
#define	CP0_WATCHLO		106
#define	CP0_WATCHHI		108
#define	CPO_20			110
#define	CP0_21			112
#define	CP0_22			114
#define	CP0_23			116
#define	CPO_24			118
#define	CP0_25			120
#define	CP0_ECC			122
#define	CP0_CACHEERR	124
#define	CP0_TAGLO		126
#define	CP0_TAGHI		128
#define	CP0_ERRORPC		130
#define	CP0_31			132


// Coprocessor 1 (FPU) general purpose register
#define	FGR0				134
#define	FGR1				136
#define	FGR2				138
#define	FGR3				140
#define	FGR4				142
#define	FGR5				144
#define	FGR6				146
#define	FGR7				148
#define	FGR8				150
#define	FGR9				152
#define	FGR10				154
#define	FGR11				156
#define	FGR12				158
#define	FGR13				160
#define	FGR14				162
#define	FGR15				164
#define	FGR16				166
#define	FGR17				168
#define	FGR18				170
#define	FGR19				172
#define	FGR20				174
#define	FGR21				176
#define	FGR22				178
#define	FGR23				180
#define	FGR24				182
#define	FGR25				184
#define	FGR26				186
#define	FGR27				188
#define	FGR28				190
#define	FGR29				192
#define	FGR30				194
#define	FGR31				196

// FPU Control and Status Registers
#define	FCR0				198
#define	FCR31				199

// FPU Control/Status Register Bits
#define	FPU_ROUND_MODE_NEAREST	0			// Round to nearest value
#define	FPU_ROUND_MODE_TO_ZERO	1			// Round toward zero
#define	FPU_ROUND_MODE_TO_PINF	2			// Round toward + infinity
#define	FPU_ROUND_MODE_TO_MINF	3			// Round toward - infinity
#define	FPU_FLAG_INEXACT			(1<<2)	// Inexact result flag
#define	FPU_FLAG_UNDERFLOW		(1<<3)	// Underflow flag
#define	FPU_FLAG_OVERFLOW			(1<<4)	// Overflow flag
#define	FPU_FLAG_DIV0				(1<<5)	// Divide by zero flag
#define	FPU_FLAG_INVALID			(1<<6)	// Invalid operation flag
#define	FPU_ENABLE_INEXACT		(1<<7)	// Enable Inexact result exception
#define	FPU_ENABLE_UNDERFLOW		(1<<8)	// Enable Underflow exception
#define	FPU_ENABLE_OVERFLOW		(1<<9)	// Enable Overflow exception
#define	FPU_ENABLE_DIV0			(1<<10)	// Enable Divide by zero exception
#define	FPU_ENABLE_INVALID		(1<<11)	// Enable Invalid operation exception
#define	FPU_CAUSE_INEXACT			(1<<12)	// Inexact result
#define	FPU_CAUSE_UNDERFLOW		(1<<13)	// Underflow
#define	FPU_CAUSE_OVERFLOW		(1<<14)	// Overflow
#define	FPU_CAUSE_DIV0				(1<<15)	// Divide by zero
#define	FPU_CAUSE_INVALID			(1<<16)	// Invalid operation
#define	FPU_CAUSE_UNIMPLEMENTED	(1<<17)	// Uniplemented operation
#define	FPU_FCC_0					(1<<23)	// Condition code 0
#define	FPU_FLUSH_TO_ZERO			(1<<24)	// Flush denormalized result to zero (MIPS III & IV only)
#define	FPU_FCC_1					(1<<25)	// Condition code 1 (MIPS IV only)
#define	FPU_FCC_2					(1<<26)	// Condition code 1 (MIPS IV only)
#define	FPU_FCC_3					(1<<27)	// Condition code 1 (MIPS IV only)
#define	FPU_FCC_4					(1<<28)	// Condition code 1 (MIPS IV only)
#define	FPU_FCC_5					(1<<29)	// Condition code 1 (MIPS IV only)
#define	FPU_FCC_6					(1<<30)	// Condition code 1 (MIPS IV only)
#define	FPU_FCC_7					(1<<31)	// Condition code 1 (MIPS IV only)

#ifndef TEST
#define	TTY_INTERRUPTS
#endif


#endif
