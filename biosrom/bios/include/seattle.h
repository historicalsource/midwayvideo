//
// seattle.h - Phoenix seattle system specific definitions
//
// $Revision: 12 $
//
// This file should contain definitions specific to the Phoenix SEATTLE system
// only.
//
#ifndef __SEATTLE_H__
#define __SEATTLE_H__

#ifndef __GT64010_H__
#include	<gt64010.h>
#endif

// Definitions of stuff on this system
#define	SERIAL_CHANNEL_TYPE		IOASIC
#define	TTY_DRIVER
#define	SYSTEM_CONTROLLER			GT64010
#define	PC87415_DEVICE_NUMBER	9

// Gross System Addresses
#define	PCI_MEM_BASE_ADDR			0xa8000000
#define	PCI_IO_BASE_ADDR			0xaa000000
#define	GT_64010_BASE				0xac000000
#define	IO_BASE_ADDR				0xb6000000
#define	BOOT_ROM_ADDR				0xbfc00000
#define	SST_BASE_ADDR				PCI_MEM_BASE_ADDR

// Base Address of the I/O ASIC
#define	IO_ASIC_BASE				(IO_BASE_ADDR + 0)

// Base Address of the CMOS RAM
#define	CMOS_RAM_ADDR 				(IO_BASE_ADDR + 0x100000)

// Base Address of the Interrupt Control PAL
#define	INTERRUPT_CONTROL			(IO_BASE_ADDR + 0x1200000)

// Base Address for CMOS unlock
#define	CMOS_UNLOCK_ADDR			(IO_BASE_ADDR + 0x1000000)

// Base Address for the watchdog timer
#define	WATCHDOG_ADDR				(IO_BASE_ADDR + 0x1100000)

// Base Address for the sound system fifos
#define	SOUND_FIFO_ADDR			0xb3000000

// Base Address for the LEDs
#define	LED_ADDR						(IO_BASE_ADDR + 0x1900000)

// Base Address for I/O ASIC reset
#define	IOASIC_RESET_ADDR			(IO_BASE_ADDR + 0x1f00000)

// Base Address for PCI reset
#define	PCI_RESET_ADDR				(IOASIC_RESET_ADDR)

// Base Address NMI Select/Enable Register
#define	NMI_SELECT_ENABLE			(IO_BASE_ADDR + 0x01200000)

// Base Address of the IDC Controller
#define	IDE_BASE_ADDRESS			PCI_IO_BASE_ADDR

// CMOS Memory Cell Spacing (bytes)
#define	CMOS_CELL_SPACE			4

// Size of the CMOS Ram (bytes)
#define	CMOS_SIZE					32768

// PCI Arbitor Control PAL address
#define	PCI_ARBITOR					(IO_BASE_ADDR + 0x1800000)

// PCI Arbitor Conrol PAL Bit definitions
#define	PCI_ARB_PARK_ON_GALILEO	0
#define	PCI_ARB_ROUND_ROBIN		1
#define	PCI_ARB_SPARE0				2
#define	PCI_ARB_SPARE1				4

// I/O ASIC Registers (I/O Map 0)
#define	IOASIC_MAP0_DIP_SWITCHES		(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP0_MISC_INPUT			(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP0_COIN_INPUT			(IOASIC_MAP0_MISC_INPUT)
#define	IOASIC_MAP0_PLAYER_12			(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP0_PLAYER_34			(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP0_UART_CONTROL		(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP0_UART_TX				(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP0_UART_RX				(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP0_UART_STATUS			(IOASIC_MAP0_UART_RX)
#define	IOASIC_MAP0_COIN_METERS			(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP0_SOUND_CONTROL		(IO_ASIC_BASE + (0x8<<2))
#define	IOASIC_MAP0_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP0_SOUND_STATUS		(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP0_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP0_PIC_COMMAND			(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP0_PIC_DATA_IN			(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP0_STATUS				(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP0_CONTROL				(IO_ASIC_BASE + (0xf<<2))

// I/O ASIC Registers (I/O Map 1)
#define	IOASIC_MAP1_DIP_SWITCHES		(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP1_MISC_INPUT			(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP1_COIN_INPUT			(IOASIC_MAP1_MISC_INPUT)
#define	IOASIC_MAP1_PLAYER_12			(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP1_PLAYER_34			(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP1_UART_CONTROL		(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP1_UART_TX				(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP1_UART_RX				(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP1_UART_STATUS			(IOASIC_MAP1_UART_RX)
#define	IOASIC_MAP1_COIN_METERS			(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP1_SOUND_CONTROL		(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP1_SOUND_DATA_OUT		(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP1_SOUND_STATUS		(IO_ASIC_BASE + (0x8<<2))
#define	IOASIC_MAP1_SOUND_DATA_IN		(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP1_PIC_COMMAND			(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP1_PIC_DATA_IN			(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP1_STATUS				(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP1_CONTROL				(IO_ASIC_BASE + (0xf<<2))

// I/O ASIC Registers (I/O Map 2)
#define	IOASIC_MAP2_DIP_SWITCHES		(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP2_MISC_INPUT			(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP2_COIN_INPUT			(IOASIC_MAP2_MISC_INPUT)
#define	IOASIC_MAP2_PLAYER_12			(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP2_PLAYER_34			(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP2_UART_CONTROL		(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP2_UART_TX				(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP2_UART_RX				(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP2_UART_STATUS			(IOASIC_MAP2_UART_RX)
#define	IOASIC_MAP2_COIN_METERS			(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP2_SOUND_CONTROL		(IO_ASIC_BASE + (0x8<<2))
#define	IOASIC_MAP2_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP2_SOUND_STATUS		(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP2_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP2_PIC_COMMAND			(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP2_PIC_DATA_IN			(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP2_STATUS				(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP2_CONTROL				(IO_ASIC_BASE + (0xf<<2))

// I/O ASIC Registers (I/O Map 3)
#define	IOASIC_MAP3_DIP_SWITCHES		(IO_ASIC_BASE + (0xf<<2))
#define	IOASIC_MAP3_MISC_INPUT			(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP3_COIN_INPUT			(IOASIC_MAP3_MISC_INPUT)
#define	IOASIC_MAP3_PLAYER_12			(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP3_PLAYER_34			(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP3_UART_CONTROL		(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP3_UART_TX				(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP3_UART_RX				(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP3_UART_STATUS			(IOASIC_MAP3_UART_RX)
#define	IOASIC_MAP3_COIN_METERS			(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP3_SOUND_CONTROL		(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP3_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x8<<2))
#define	IOASIC_MAP3_SOUND_STATUS		(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP3_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP3_PIC_COMMAND			(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP3_PIC_DATA_IN			(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP3_STATUS				(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP3_CONTROL				(IO_ASIC_BASE + (0x0<<2))

// I/O ASIC Registers (I/O Map 4)
#define	IOASIC_MAP4_DIP_SWITCHES		(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP4_MISC_INPUT			(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP4_COIN_INPUT			(IOASIC_MAP4_MISC_INPUT)
#define	IOASIC_MAP4_PLAYER_12			(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP4_PLAYER_34			(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP4_UART_CONTROL		(IO_ASIC_BASE + (0xf<<2))
#define	IOASIC_MAP4_UART_TX				(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP4_UART_RX				(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP4_UART_STATUS			(IOASIC_MAP4_UART_RX)
#define	IOASIC_MAP4_COIN_METERS			(IO_ASIC_BASE + (0x8<<2))
#define	IOASIC_MAP4_SOUND_CONTROL		(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP4_SOUND_DATA_OUT		(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP4_SOUND_STATUS		(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP4_SOUND_DATA_IN		(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP4_PIC_COMMAND			(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP4_PIC_DATA_IN			(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP4_STATUS				(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP4_CONTROL				(IO_ASIC_BASE + (0x3<<2))

// I/O ASIC Registers (I/O Map 5)
#define	IOASIC_MAP5_DIP_SWITCHES		(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP5_MISC_INPUT			(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP5_COIN_INPUT			(IOASIC_MAP5_MISC_INPUT)
#define	IOASIC_MAP5_PLAYER_12			(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP5_PLAYER_34			(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP5_UART_CONTROL		(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP5_UART_TX				(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP5_UART_RX				(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP5_UART_STATUS			(IOASIC_MAP5_UART_RX)
#define	IOASIC_MAP5_COIN_METERS			(IO_ASIC_BASE + (0xf<<2))
#define	IOASIC_MAP5_SOUND_CONTROL		(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP5_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP5_SOUND_STATUS		(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP5_SOUND_DATA_IN		(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP5_PIC_COMMAND			(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP5_PIC_DATA_IN			(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP5_STATUS				(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP5_CONTROL				(IO_ASIC_BASE + (0x8<<2))

// I/O ASIC Registers (I/O Map 6)
#define	IOASIC_MAP6_DIP_SWITCHES		(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP6_MISC_INPUT			(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP6_COIN_INPUT			(IOASIC_MAP6_MISC_INPUT)
#define	IOASIC_MAP6_PLAYER_12			(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP6_PLAYER_34			(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP6_UART_CONTROL		(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP6_UART_TX				(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP6_UART_RX				(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP6_UART_STATUS			(IOASIC_MAP6_UART_RX)
#define	IOASIC_MAP6_COIN_METERS			(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP6_SOUND_CONTROL		(IO_ASIC_BASE + (0xc<<2))
#define	IOASIC_MAP6_SOUND_DATA_OUT		(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP6_SOUND_STATUS		(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP6_SOUND_DATA_IN		(IO_ASIC_BASE + (0xf<<2))
#define	IOASIC_MAP6_PIC_COMMAND			(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP6_PIC_DATA_IN			(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP6_STATUS				(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP6_CONTROL				(IO_ASIC_BASE + (0x8<<2))

// I/O ASIC Registers (I/O Map 7)
#define	IOASIC_MAP7_DIP_SWITCHES		(IO_ASIC_BASE + (0xb<<2))
#define	IOASIC_MAP7_MISC_INPUT			(IO_ASIC_BASE + (0xa<<2))
#define	IOASIC_MAP7_COIN_INPUT			(IOASIC_MAP7_MISC_INPUT)
#define	IOASIC_MAP7_PLAYER_12			(IO_ASIC_BASE + (0x9<<2))
#define	IOASIC_MAP7_PLAYER_34			(IO_ASIC_BASE + (0x8<<2))
#define	IOASIC_MAP7_UART_CONTROL		(IO_ASIC_BASE + (0x0<<2))
#define	IOASIC_MAP7_UART_TX				(IO_ASIC_BASE + (0x1<<2))
#define	IOASIC_MAP7_UART_RX				(IO_ASIC_BASE + (0x2<<2))
#define	IOASIC_MAP7_UART_STATUS			(IOASIC_MAP7_UART_RX)
#define	IOASIC_MAP7_COIN_METERS			(IO_ASIC_BASE + (0x3<<2))
#define	IOASIC_MAP7_SOUND_CONTROL		(IO_ASIC_BASE + (0x7<<2))
#define	IOASIC_MAP7_SOUND_DATA_OUT		(IO_ASIC_BASE + (0x6<<2))
#define	IOASIC_MAP7_SOUND_STATUS		(IO_ASIC_BASE + (0x5<<2))
#define	IOASIC_MAP7_SOUND_DATA_IN		(IO_ASIC_BASE + (0x4<<2))
#define	IOASIC_MAP7_PIC_COMMAND			(IO_ASIC_BASE + (0xf<<2))
#define	IOASIC_MAP7_PIC_DATA_IN			(IO_ASIC_BASE + (0xe<<2))
#define	IOASIC_MAP7_STATUS				(IO_ASIC_BASE + (0xd<<2))
#define	IOASIC_MAP7_CONTROL				(IO_ASIC_BASE + (0xc<<2))



// I/O ASIC Registers
#if	defined(NBA)
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

#elif	defined(NFL)
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

#elif	defined(B99)
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


// Interrupt Control Registers
#define	ICPLD_NMICNTRL_REG		(INTERRUPT_CONTROL)
#define	ICPLD_INT_ENBL_REG		(INTERRUPT_CONTROL + 0x100000)
#define	ICPLD_INT_MAP_REG			(INTERRUPT_CONTROL + 0x200000)
#define	ICPLD_INT_CAUSE_REG		(INTERRUPT_CONTROL + 0x300000)
#define	ICPLD_GPSTATUS_REG		(INTERRUPT_CONTROL + 0x400000)

#define	VRETRACE_RESET_REG		(INTERRUPT_CONTROL + 0x500000)

// Reset Register Bit definitions
#define	RST_UNUSED						(1<<0)
#define	RST_IOASIC						(1<<1)
#define	RST_PCI							(1<<2)
#define	RST_3DFX							(1<<3)
#define	RST_NSS							(1<<4)
#define	RST_SPARE						(1<<5)

// Control Latch Output Register Bit definitions
#define	ARBITER_PARK_GT64010			(0<<0)
#define	ARBITER_PARK_ROUND_ROBIN	(1<<0)
#define	TEST_POINT_1					(1<<1)
#define	TEST_POINT_2					(1<<2)

// LED Register Bit Definitions
#define	LED_2								(1<<0)
#define	LED_3								(1<<1)
#define	LED_4								(1<<2)
#define	RED_LED							(LED_2)
#define	YELLOW_LED						(LED_3)
#define	GREEN_LED						(LED_4)

// Interrupt Enable Register bit definitions
#define	UNUSED_INT_ENABLE				(1<<0)
#define	NSS_INT_ENABLE					(1<<1)
#define	RESERVED_INT_ENABLE			(1<<2)
#define	PCI_INT_ENABLE					(1<<3)
#define	A2D_INT_ENABLE					(1<<4)
#define	RESERVED2_INT_ENABLE			(1<<5)
#define	DEBUG_SW_INT_ENABLE			(1<<6)
#define	VSYNC_INT_ENABLE				(1<<7)
#define	VSYNC_NEGATIVE_POLARITY		(0<<8)
#define	VSYNC_POSITIVE_POLARITY		(1<<8)

// Interrupt Map Register bit definitions
#define	MAP_UNUSED_TO_INT3			(1<<0)
#define	MAP_UNUSED_TO_INT4			(2<<0)
#define	MAP_UNUSED_TO_INT5			(3<<0)
#define	MAP_NSS_TO_INT3				(1<<2)
#define	MAP_NSS_TO_INT4				(2<<2)
#define	MAP_NSS_TO_INT5				(3<<2)
#define	MAP_RESERVED_TO_INT3			(1<<4)
#define	MAP_RESERVED_TO_INT4			(2<<4)
#define	MAP_RESERVED_TO_INT5			(3<<4)
#define	MAP_PCI_TO_INT3				(1<<6)
#define	MAP_PCI_TO_INT4				(2<<6)
#define	MAP_PCI_TO_INT5				(3<<6)
#define	MAP_A2D_TO_INT3				(1<<8)
#define	MAP_A2D_TO_INT4				(2<<8)
#define	MAP_A2D_TO_INT5				(3<<8)
#define	MAP_RESERVED1_TO_INT3		(1<<10)
#define	MAP_RESERVED1_TO_INT4		(2<<10)
#define	MAP_RESERVED1_TO_INT5		(3<<10)
#define	MAP_DEBUG_SW_TO_INT3			(1<<12)
#define	MAP_DEBUG_SW_TO_INT4			(2<<12)
#define	MAP_DEBUG_SW_TO_INT5			(3<<12)
#define	MAP_VSYNC_TO_INT3				(1<<14)
#define	MAP_VSYNC_TO_INT4				(2<<14)
#define	MAP_VSYNC_TO_INT5				(3<<14)

// Interrupt cause register bit definitions
#define	INT_CAUSE_UNUSED				(1<<0)
#define	INT_CAUSE_NSS					(1<<1)
#define	INT_CAUSE_RESERVED			(1<<2)
#define	INT_CAUSE_PCI					(1<<3)
#define	INT_CAUSE_A2D					(1<<4)
#define	INT_CAUSE_RESERVED1			(1<<5)
#define	INT_CAUSE_DEBUG_SW			(1<<6)
#define	INT_CAUSE_VSYNC				(1<<7)

// NMI Select/Enable register bit definitions
#define	DISABLE_NMI						0
#define	NMI_SOURCE_UNUSED				1
#define	NMI_SOURCE_NSS					2
#define	NMI_SOURCE_DEBUG_SW			3

// General purpose status register bit definitions
#define	INT_SOURCE_EXPANSION_SLOT	(1<<0)
#define	INT_SOURCE_NSS					(1<<1)
#define	INT_SOURCE_RESERVED			(1<<2)
#define	INT_SOURCE_PCI					(1<<3)
#define	INT_SOURCE_A2D					(1<<4)
#define	INT_SOURCE_RESERVED1			(1<<5)
#define	INT_SOURCE_DEBUG_SW			(1<<6)
#define	INT_SOURCE_VSYNC				(1<<7)
#define	CURRENT_VSYNC					(1<<8)
#define	BATTERY_LOW						(1<<9)
#define	WATCHDOG_TIMEOUT				(1<<10)
#define	AUX_LATCH_BIT_11				(1<<11)
#define	NMI_INTERRUPT					(1<<12)

#define	GALILEO_INT					IP_2
#define	IO_ASIC_INT					IP_3
#define	IDE_DISK_INT				IP_4
#define	NSS_INT						IP_5
#define	VERTICAL_RETRACE_INT		IP_6
#define	DEBUG_SWITCH_INT			IP_7
#define	SCSI_INT						IP_7

// Timer used to strobe the watchdog
#define	TIMER0_WDOG	0
#define	TIMER3_WDOG	1

#define	WDOG_TIMER	TIMER0_WDOG

#endif	// __SEATTLE_H__
