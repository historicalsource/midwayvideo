//
// sa1.h - Phoenix SA1 system specific definitions
//
// $Revision: 6 $
//
// This file should contain definitions specific to the Phoenix SA1 system
// only.
//
#ifndef __SA1_H__
#define __SA1_H__

// Definitions of stuff on this system
#define	SERIAL_CHANNEL_TYPE		NS450
#define	TTY_DRIVER
#define	SYSTEM_CONTROLLER			GT64010
#define	PC87415_DEVICE_NUMBER	9

// Gross System Addresses
#define	PCI_MEM_BASE_ADDR			0xa8000000
#define	PCI_IO_BASE_ADDR			0xaa000000
#define	GT_64010_BASE				0xac000000
#define	IO_BASE_ADDR				0xb5000000
#define	BOOT_ROM_ADDR				0xbfc00000
//#define	SST_BASE_ADDR				PCI_MEM_BASE_ADDR
#define	SST_BASE_ADDR				0xa8000000

// Base Address of the I/O ASIC
#define	IO_ASIC_BASE 				(IO_BASE_ADDR + 0)

// Base Address of the interrupt Control PAL
#define	INTERRUPT_CONTROL			(IO_BASE_ADDR + 0x80000)

// Base Address of the Miscellaneous Control PAL
#define	MISC_PLD						(IO_BASE_ADDR + 0x100000)

// Base Address of the watchdog timer
#define	WATCHDOG_ADDR				(IO_BASE_ADDR + 0x180000)

// Base address of Serial port 1
#define	SERIAL_0_ADDR				(IO_BASE_ADDR + 0x200000)

// Base address of Serial port 2
#define	SERIAL_1_ADDR				(IO_BASE_ADDR + 0x280000)

// Base address of the parallel port
#define	PARALLEL_ADDR				(IO_BASE_ADDR + 0x300000)

// Base address of the Analog to digital convertor
#define	A2D_ADDR						(IO_BASE_ADDR + 0x380000)

// Base adderss of the sound system fifos
#define	SOUND_FIFO_ADDR			(IO_BASE_ADDR + 0x400000)

// Base address of the CMOS Ram
#define	CMOS_RAM_ADDR				(IO_BASE_ADDR + 0x480000)

// Base address of the CMOS Ram unlock
#define	CMOS_UNLOCK_ADDR			(IO_BASE_ADDR + 0x500000)

// Base address of the I/O ASIC reset
#define	IOASIC_RESET_ADDR			(IO_BASE_ADDR + 0x800000)

// Base address of PCI reset
#define	PCI_RESET_ADDR				(IO_BASE_ADDR + 0x880000)

// Base Address of the LEDs
#define	LED_ADDR						(IO_BASE_ADDR + 0x100010)

// Address of the register used to reset the vertical retrace interrupt
#define	VRETRACE_RESET_REG		ICPLD_STATUS_REG

// Base Address of the IDC Controller
#define	IDE_BASE_ADDRESS			PCI_IO_BASE_ADDR

// CMOS Memory Cell Spacing (bytes)
#define	CMOS_CELL_SPACE			4

// Size of the CMOS ram (bytes)
#define	CMOS_SIZE					8192

// Registers in the interrupt control PAL
#define	ICPLD_INT_ENBL_REG		(INTERRUPT_CONTROL)
#define	ICPLD_INT_MAPA_REG		(INTERRUPT_CONTROL + 0x08)
#define	ICPLD_INT_MAPB_REG		(INTERRUPT_CONTROL + 0x10)
#define	ICPLD_CAUSE_REG			(INTERRUPT_CONTROL + 0x18)
#define	ICPLD_STATUS_REG			(INTERRUPT_CONTROL + 0x20)
#define	ICPLD_GPSTATUS_REG		(INTERRUPT_CONTROL + 0x28)
#define	ICPLD_NMICNTRL_REG		(INTERRUPT_CONTROL + 0x30)


// I/O ASIC Registers
#define	IOASIC_DIP_SWITCHES		(IO_ASIC_BASE + 0x00)
#define	IOASIC_MISC_INPUT			(IO_ASIC_BASE + 0x08)
#define	IOASIC_COIN_INPUT			(IOASIC_MISC_INPUT)
#define	IOASIC_PLAYER_12			(IO_ASIC_BASE + 0x10)
#define	IOASIC_PLAYER_34			(IO_ASIC_BASE + 0x18)
#define	IOASIC_UART_CONTROL		(IO_ASIC_BASE + 0x20)
#define	IOASIC_UART_TX				(IO_ASIC_BASE + 0x28)
#define	IOASIC_UART_RX				(IO_ASIC_BASE + 0x30)
#define	IOASIC_UART_STATUS		(IOASIC_UART_RX)
#define	IOASIC_COIN_METERS		(IO_ASIC_BASE + 0x38)
#define	IOASIC_SOUND_CONTROL		(IO_ASIC_BASE + 0x40)
#define	IOASIC_SOUND_DATA_OUT	(IO_ASIC_BASE + 0x48)
#define	IOASIC_SOUND_STATUS		(IO_ASIC_BASE + 0x50)
#define	IOASIC_SOUND_DATA_IN		(IO_ASIC_BASE + 0x58)
#define	IOASIC_PIC_COMMAND		(IO_ASIC_BASE + 0x60)
#define	IOASIC_PIC_DATA_IN		(IO_ASIC_BASE + 0x68)
#define	IOASIC_STATUS				(IO_ASIC_BASE + 0x70)
#define	IOASIC_CONTROL				(IO_ASIC_BASE + 0x78)

#define	VERTICAL_RETRACE_INT		IP_3
#define	IDE_DISK_INT				IP_4
#define	GALILEO_INT					IP_2
#define	DEBUG_SWITCH_INT			IP_7
#define	SCSI_INT						IP_7


#ifndef __GT64010_H__
#include	<gt64010.h>
#endif

#endif	// __SA1_H__
