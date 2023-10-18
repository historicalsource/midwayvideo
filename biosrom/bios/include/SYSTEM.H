//
// system.h
//
// System address definintions
//
// $Revision: 6 $
//
// This file should contain only those things that are common to all systems.
// System dependent stuff should be in one of the include files included
// by this file.
//
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

// Definitions of systems
#define	SA1			1
#define	SEATTLE		2
#define	FLAGSTAFF	4
#define	VEGAS			8

// Definition of the types of serial channels
#define	NS450		1
#define	IOASIC	2

// Definition of the system controller types
#define	GT64010	1
#define	NILE4		2

// Processor clock speed and count to get 100Hz
#define	PCLK			200000000		// Processor clock speed
#define	ICLK			(PCLK/2)			// 1/2 instruction issue rate
#define	TICKS_100HZ	(ICLK/100)		// counts for 100HZ

#if ! defined(PHOENIX_SYS)
#define	PHOENIX_SYS	SA1
#endif

#if (PHOENIX_SYS & SA1)

// Include file for ths SA1 system
#include "/video/biosrom/bios/include/sa1.h"

#elif (PHOENIX_SYS & SEATTLE)

// Include file for the Seattle system
#include "/video/biosrom/bios/include/seattle.h"

#elif (PHOENIX_SYS & FLAGSTAFF)

// Include file for the Flagstaff system
#error Flagstaff not supported yet

#elif (PHOENIX_SYS & VEGAS)

// Include file for the Vegas system
#include "/video/biosrom/bios/include/vegas.h"

#endif

#ifndef SYSTEM_CONTROLLER
#error SYSTEM_CONTROLLER is not defined
#endif

#ifndef SERIAL_CHANNEL_TYPE
#error SERIAL_CHANNEL_TYPE is not defined
#endif

// Alternate register names for I/O ASIC
#define	MAIN_CONTROL    			((volatile unsigned short *)(IOASIC_CONTROL))
#define	ASIC_WORD 					((unsigned short *)(IO_ASIC_BASE))

// I/O ASIC Status Register Bits
#define	PIC_READY			      (1<<0)

// Definitions of bit positions for player switches
// If the bit positions of the player switches changes, this is what must be
// changed to reflect those changes.
#define	P_UP_SHIFT					0
#define	P_DOWN_SHIFT				1
#define	P_LEFT_SHIFT				2
#define	P_RIGHT_SHIFT				3
#define	P_A_SHIFT					4
#define	P_B_SHIFT					5
#define	P_C_SHIFT					6
#define	P_D_SHIFT					7

#define	PLAYER_BITS					8

// Byte position of player inputs
// If the byte positions of the player switches changes, this is what must be
// changed to reflect those changes
#define	P1_BYTE						0
#define	P2_BYTE						1
#define	P3_BYTE						2
#define	P4_BYTE						3

// Definitions of Bits for player12 and player34 data registers
#define	P_UP							(1 << (P_UP_SHIFT))
#define	P_DOWN						(1 << (P_DOWN_SHIFT))
#define	P_LEFT						(1 << (P_LEFT_SHIFT))
#define	P_RIGHT						(1 << (P_RIGHT_SHIFT))
#define	P_A							(1 << (P_A_SHIFT))
#define	P_B							(1 << (P_B_SHIFT))
#define	P_C							(1 << (P_C_SHIFT))
#define	P_D							(1 << (P_D_SHIFT))

// Definitions of player shifts
#define	P1_SHIFT						(P1_BYTE * PLAYER_BITS)
#define	P2_SHIFT						(P2_BYTE * PLAYER_BITS)
#define	P3_SHIFT						(P3_BYTE * PLAYER_BITS)
#define	P4_SHIFT						(P4_BYTE * PLAYER_BITS)

// Definitions of shift amounts for player switches
#define	P1_UP_SWID					(P_UP_SHIFT    + P1_SHIFT)
#define	P1_DOWN_SWID				(P_DOWN_SHIFT  + P1_SHIFT)
#define	P1_LEFT_SWID				(P_LEFT_SHIFT  + P1_SHIFT)
#define	P1_RIGHT_SWID				(P_RIGHT_SHIFT + P1_SHIFT)
#define	P1_A_SWID					(P_A_SHIFT     + P1_SHIFT)
#define	P1_B_SWID					(P_B_SHIFT     + P1_SHIFT)
#define	P1_C_SWID					(P_C_SHIFT     + P1_SHIFT)
#define	P1_D_SWID					(P_D_SHIFT     + P1_SHIFT)

#define	P2_UP_SWID					(P_UP_SHIFT    + P2_SHIFT)
#define	P2_DOWN_SWID				(P_DOWN_SHIFT  + P2_SHIFT)
#define	P2_LEFT_SWID				(P_LEFT_SHIFT  + P2_SHIFT)
#define	P2_RIGHT_SWID				(P_RIGHT_SHIFT + P2_SHIFT)
#define	P2_A_SWID					(P_A_SHIFT     + P2_SHIFT)
#define	P2_B_SWID					(P_B_SHIFT     + P2_SHIFT)
#define	P2_C_SWID					(P_C_SHIFT     + P2_SHIFT)
#define	P2_D_SWID					(P_D_SHIFT     + P2_SHIFT)

#define	P3_UP_SWID					(P_UP_SHIFT    + P3_SHIFT)
#define	P3_DOWN_SWID				(P_DOWN_SHIFT  + P3_SHIFT)
#define	P3_LEFT_SWID				(P_LEFT_SHIFT  + P3_SHIFT)
#define	P3_RIGHT_SWID				(P_RIGHT_SHIFT + P3_SHIFT)
#define	P3_A_SWID					(P_A_SHIFT     + P3_SHIFT)
#define	P3_B_SWID					(P_B_SHIFT     + P3_SHIFT)
#define	P3_C_SWID					(P_C_SHIFT     + P3_SHIFT)
#define	P3_D_SWID					(P_D_SHIFT     + P3_SHIFT)

#define	P4_UP_SWID					(P_UP_SHIFT    + P4_SHIFT)
#define	P4_DOWN_SWID				(P_DOWN_SHIFT  + P4_SHIFT)
#define	P4_LEFT_SWID				(P_LEFT_SHIFT  + P4_SHIFT)
#define	P4_RIGHT_SWID				(P_RIGHT_SHIFT + P4_SHIFT)
#define	P4_A_SWID					(P_A_SHIFT     + P4_SHIFT)
#define	P4_B_SWID					(P_B_SHIFT     + P4_SHIFT)
#define	P4_C_SWID					(P_C_SHIFT     + P4_SHIFT)
#define	P4_D_SWID					(P_D_SHIFT     + P4_SHIFT)

// Definitions of the player switches as they are returned
#define	P1_UP							(1 << P1_UP_SWID)
#define	P1_DOWN						(1 << P1_DOWN_SWID)
#define	P1_LEFT						(1 << P1_LEFT_SWID)
#define	P1_RIGHT						(1 << P1_RIGHT_SWID)
#define	P1_A							(1 << P1_A_SWID)
#define	P1_B							(1 << P1_B_SWID)
#define	P1_C							(1 << P1_C_SWID)
#define	P1_D							(1 << P1_D_SWID)

#define	P2_UP							(1 << P2_UP_SWID)
#define	P2_DOWN						(1 << P2_DOWN_SWID)
#define	P2_LEFT						(1 << P2_LEFT_SWID)
#define	P2_RIGHT						(1 << P2_RIGHT_SWID)
#define	P2_A							(1 << P2_A_SWID)
#define	P2_B							(1 << P2_B_SWID)
#define	P2_C							(1 << P2_C_SWID)
#define	P2_D							(1 << P2_D_SWID)

#define	P3_UP							(1 << P3_UP_SWID)
#define	P3_DOWN						(1 << P3_DOWN_SWID)
#define	P3_LEFT						(1 << P3_LEFT_SWID)
#define	P3_RIGHT						(1 << P3_RIGHT_SWID)
#define	P3_A							(1 << P3_A_SWID)
#define	P3_B							(1 << P3_B_SWID)
#define	P3_C							(1 << P3_C_SWID)
#define	P3_D							(1 << P3_D_SWID)

#define	P4_UP							(1 << P4_UP_SWID)
#define	P4_DOWN						(1 << P4_DOWN_SWID)
#define	P4_LEFT						(1 << P4_LEFT_SWID)
#define	P4_RIGHT						(1 << P4_RIGHT_SWID)
#define	P4_A							(1 << P4_A_SWID)
#define	P4_B							(1 << P4_B_SWID)
#define	P4_C							(1 << P4_C_SWID)
#define	P4_D							(1 << P4_D_SWID)

#define	P_SWITCH_MASK				(P_UP|P_DOWN|P_RIGHT|P_LEFT|P_A|P_B|P_C|P_D)
#define	P1_SWITCH_MASK				(P_SWITCH_MASK << (P1_SHIFT))
#define	P2_SWITCH_MASK				(P_SWITCH_MASK << (P2_SHIFT))
#define	P3_SWITCH_MASK				(P_SWITCH_MASK << (P3_SHIFT))
#define	P4_SWITCH_MASK				(P_SWITCH_MASK << (P4_SHIFT))

#define	P_RLDU_MASK					(P_UP|P_DOWN|P_RIGHT|P_LEFT)
#define	P1_RLDU_MASK				(P_RLDU_MASK << (P1_SHIFT))
#define	P2_RLDU_MASK				(P_RLDU_MASK << (P2_SHIFT))
#define	P3_RLDU_MASK				(P_RLDU_MASK << (P3_SHIFT))
#define	P4_RLDU_MASK				(P_RLDU_MASK << (P4_SHIFT))

#define	P_ABCD_MASK					(P_A|P_B|P_C|P_D)
#define	P1_ABCD_MASK				(P_ABCD_MASK << (P1_SHIFT))
#define	P2_ABCD_MASK				(P_ABCD_MASK << (P2_SHIFT))
#define	P3_ABCD_MASK				(P_ABCD_MASK << (P3_SHIFT))
#define	P4_ABCD_MASK				(P_ABCD_MASK << (P4_SHIFT))

// Definitions of the coin switch bit positions
#define	LEFT_COIN_SHIFT				0
#define	RIGHT_COIN_SHIFT				1
#define	P1_START_SHIFT					2
#define	SLAM_SHIFT						3
#define	TEST_SHIFT						4
#define	P2_START_SHIFT					5
#define	SERVICE_CREDIT_SHIFT			6
#define	CENTER_COIN_SHIFT				7
#define	EXTRA_COIN_SHIFT				8
#define	P3_START_SHIFT					9
#define	P4_START_SHIFT					10
#define	VOLUME_DOWN_SHIFT				11
#define	VOLUME_UP_SHIFT				12
#define	COINDOOR_INTERLOCK_SHIFT	14
#define	BILL_VALIDATOR_SHIFT			15

// Definitions of the the word positions of coins and dipswitches
#define	COIN_SHIFT						0
#define	DIP_SHIFT						16

// Defintions of the coin switch ids
#define	LEFT_COIN_SWID					(LEFT_COIN_SHIFT + COIN_SHIFT)
#define	RIGHT_COIN_SWID				(RIGHT_COIN_SHIFT + COIN_SHIFT)
#define	P1_START_SWID					(P1_START_SHIFT + COIN_SHIFT)
#define	SLAM_SWID						(SLAM_SHIFT + COIN_SHIFT)
#define	TEST_SWID						(TEST_SHIFT + COIN_SHIFT)
#define	P2_START_SWID					(P2_START_SHIFT + COIN_SHIFT)
#define	SERVICE_CREDIT_SWID			(SERVICE_CREDIT_SHIFT + COIN_SHIFT)
#define	CENTER_COIN_SWID				(CENTER_COIN_SHIFT + COIN_SHIFT)
#define	EXTRA_COIN_SWID				(EXTRA_COIN_SHIFT + COIN_SHIFT)
#define	P3_START_SWID					(P3_START_SHIFT + COIN_SHIFT)
#define	P4_START_SWID					(P4_START_SHIFT + COIN_SHIFT)
#define	VOLUME_DOWN_SWID				(VOLUME_DOWN_SHIFT + COIN_SHIFT)
#define	VOLUME_UP_SWID					(VOLUME_UP_SHIFT + COIN_SHIFT)
#define	COINDOOR_INTERLOCK_SWID		(COINDOOR_INTERLOCK_SHIFT + COIN_SHIFT)
#define	BILL_VALIDATOR_SWID			(BILL_VALIDATOR_SHIFT + COIN_SHIFT)

// Definitions of the bits on the miscellaneous input port
#define	LEFT_COIN_SW				(1 << (LEFT_COIN_SWID))
#define	RIGHT_COIN_SW				(1 << (RIGHT_COIN_SWID))
#define	P1_START_SW					(1 << (P1_START_SWID))
#define	SLAM_SW						(1 << (SLAM_SWID))
#define	TEST_SW						(1 << (TEST_SWID))
#define	P2_START_SW					(1 << (P2_START_SWID))
#define	SERVICE_CREDIT_SW			(1 << (SERVICE_CREDIT_SWID))
#define	CENTER_COIN_SW				(1 << (CENTER_COIN_SWID))
#define	EXTRA_COIN_SW				(1 << (EXTRA_COIN_SWID))
#define	P3_START_SW					(1 << (P3_START_SWID))
#define	P4_START_SW					(1 << (P4_START_SWID))
#define	VOLUME_DOWN_SW				(1 << (VOLUME_DOWN_SWID))
#define	VOLUME_UP_SW				(1 << (VOLUME_UP_SWID))
#define	COINDOOR_INTERLOCK_SW	(1 << (COINDOOR_INTERLOCK_SWID))
#define	BILL_VALIDATOR_SW			(1 << (BILL_VALIDATOR_SWID))
#define	MENU_UP_SW					VOLUME_UP_SW
#define	MENU_DOWN_SW				VOLUME_DOWN_SW
#define	MENU_ESCAPE_SW				SERVICE_CREDIT_SW
#define	MENU_SELECT_SW				TEST_SW
#define	TILT_SW						SLAM_SW


#define	PLAYER_STARTS_MASK		(P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)
#define	COIN_INPUTS_MASK			(LEFT_COIN_SW|RIGHT_COIN_SW|CENTER_COIN_SW|EXTRA_COIN_SW)
#define	VOLUME_SW_MASK				(VOLUME_UP_SW|VOLUME_DOWN_SW)
#define	MENU_CONTROL_MASK			(MENU_UP_SW|MENU_DOWN_SW|MENU_ESCAPE_SW|MENU_SELECT_SW)


// Definitions of dipswitch bit shifts
#define	DIP_BIT_0_SHIFT			0
#define	DIP_BIT_1_SHIFT			1
#define	DIP_BIT_2_SHIFT			2
#define	DIP_BIT_3_SHIFT			3
#define	DIP_BIT_4_SHIFT			4
#define	DIP_BIT_5_SHIFT			5
#define	DIP_BIT_6_SHIFT			6
#define	DIP_BIT_7_SHIFT			7
#define	DIP_BIT_8_SHIFT			8
#define	DIP_BIT_9_SHIFT			9
#define	DIP_BIT_10_SHIFT			10
#define	DIP_BIT_11_SHIFT			11
#define	DIP_BIT_12_SHIFT			12
#define	DIP_BIT_13_SHIFT			13
#define	DIP_BIT_14_SHIFT			14
#define	DIP_BIT_15_SHIFT			15

// Definitions of dipswitch ids
#define	DIP_BIT_0_SWID				(DIP_BIT_0_SHIFT + DIP_SHIFT)
#define	DIP_BIT_1_SWID				(DIP_BIT_1_SHIFT + DIP_SHIFT)
#define	DIP_BIT_2_SWID				(DIP_BIT_2_SHIFT + DIP_SHIFT)
#define	DIP_BIT_3_SWID				(DIP_BIT_3_SHIFT + DIP_SHIFT)
#define	DIP_BIT_4_SWID				(DIP_BIT_4_SHIFT + DIP_SHIFT)
#define	DIP_BIT_5_SWID				(DIP_BIT_5_SHIFT + DIP_SHIFT)
#define	DIP_BIT_6_SWID				(DIP_BIT_6_SHIFT + DIP_SHIFT)
#define	DIP_BIT_7_SWID				(DIP_BIT_7_SHIFT + DIP_SHIFT)
#define	DIP_BIT_8_SWID				(DIP_BIT_8_SHIFT + DIP_SHIFT)
#define	DIP_BIT_9_SWID				(DIP_BIT_9_SHIFT + DIP_SHIFT)
#define	DIP_BIT_10_SWID			(DIP_BIT_10_SHIFT + DIP_SHIFT)
#define	DIP_BIT_11_SWID			(DIP_BIT_11_SHIFT + DIP_SHIFT)
#define	DIP_BIT_12_SWID			(DIP_BIT_12_SHIFT + DIP_SHIFT)
#define	DIP_BIT_13_SWID			(DIP_BIT_13_SHIFT + DIP_SHIFT)
#define	DIP_BIT_14_SWID			(DIP_BIT_14_SHIFT + DIP_SHIFT)
#define	DIP_BIT_15_SWID			(DIP_BIT_15_SHIFT + DIP_SHIFT)

// Definitions of Dipswitch Inputs
#define	DIP_BIT_0					(1 << DIP_BIT_0_SWID)
#define	DIP_BIT_1					(1 << DIP_BIT_1_SWID)
#define	DIP_BIT_2					(1 << DIP_BIT_2_SWID)
#define	DIP_BIT_3					(1 << DIP_BIT_3_SWID)
#define	DIP_BIT_4					(1 << DIP_BIT_4_SWID)
#define	DIP_BIT_5					(1 << DIP_BIT_5_SWID)
#define	DIP_BIT_6					(1 << DIP_BIT_6_SWID)
#define	DIP_BIT_7					(1 << DIP_BIT_7_SWID)
#define	DIP_BIT_8					(1 << DIP_BIT_8_SWID)
#define	DIP_BIT_9					(1 << DIP_BIT_9_SWID)
#define	DIP_BIT_10					(1 << DIP_BIT_10_SWID)
#define	DIP_BIT_11					(1 << DIP_BIT_11_SWID)
#define	DIP_BIT_12					(1 << DIP_BIT_12_SWID)
#define	DIP_BIT_13					(1 << DIP_BIT_13_SWID)
#define	DIP_BIT_14					(1 << DIP_BIT_14_SWID)
#define	DIP_BIT_15					(1 << DIP_BIT_15_SWID)

// Definition of where bits end up in long word
#define	DSW1_BIT0					DIP_BIT_0
#define	DSW1_BIT1					DIP_BIT_1
#define	DSW1_BIT2					DIP_BIT_2
#define	DSW1_BIT3					DIP_BIT_3
#define	DSW1_BIT4					DIP_BIT_4
#define	DSW1_BIT5					DIP_BIT_5
#define	DSW1_BIT6					DIP_BIT_6
#define	DSW1_BIT7					DIP_BIT_7

#define	DSW2_BIT0					DIP_BIT_0
#define	DSW2_BIT1					DIP_BIT_1
#define	DSW2_BIT2					DIP_BIT_2
#define	DSW2_BIT3					DIP_BIT_3
#define	DSW2_BIT4					DIP_BIT_4
#define	DSW2_BIT5					DIP_BIT_5
#define	DSW2_BIT6					DIP_BIT_6
#define	DSW2_BIT7					DIP_BIT_7

#define	DIP_SW1_BITS				(DSW1_BIT0|DSW1_BIT1|DSW1_BIT2|DSW1_BIT3|DSW1_BIT4|DSW1_BIT5|DSW1_BIT6|DSW1_BIT7)
#define	DIP_SW2_BITS				(DSW2_BIT0|DSW2_BIT1|DSW2_BIT2|DSW2_BIT3|DSW2_BIT4|DSW2_BIT5|DSW2_BIT6|DSW2_BIT7)
#define	DIP_BITS						(DIP_SW1_BITS|DIP_SW2_BITS)

// Assorted definitions used for exception codes fro CPU cause register
#define	EXCEPTION_CODE_MASK		0x7c
#define	EXCEPTION_BIT_SHIFT		2

// Processor Exception Codes
#define	INTERRUPT_EXCEPTION		(0<<EXCEPTION_BIT_SHIFT)
#define	TLB_MOD_EXCEPTION			(1<<EXCEPTION_BIT_SHIFT)
#define	TLB_LF_EXCEPTION			(2<<EXCEPTION_BIT_SHIFT)
#define	TLB_STORE_EXCEPTION		(3<<EXCEPTION_BIT_SHIFT)
#define	ADDR_LOAD_EXCEPTION		(4<<EXCEPTION_BIT_SHIFT)
#define	ADDR_STORE_EXCEPTION		(5<<EXCEPTION_BIT_SHIFT)
#define	BUS_FETCH_EXCEPTION		(6<<EXCEPTION_BIT_SHIFT)
#define	BUS_DATA_EXCEPTION		(7<<EXCEPTION_BIT_SHIFT)
#define	SYSCALL_EXCEPTION			(8<<EXCEPTION_BIT_SHIFT)
#define	BREAKPOINT_EXCEPTION		(9<<EXCEPTION_BIT_SHIFT)
#define	RESERVED_INST_EXCEPTION	(10<<EXCEPTION_BIT_SHIFT)
#define	CPU_UNUSABLE_EXCEPTION	(11<<EXCEPTION_BIT_SHIFT)
#define	OVERFLOW_EXCEPTION		(12<<EXCEPTION_BIT_SHIFT)
#define	TRAP_EXCEPTION				(13<<EXCEPTION_BIT_SHIFT)
#define	VCOH_INST_EXCEPTION		(14<<EXCEPTION_BIT_SHIFT)
#define	FPU_EXCEPTION				(15<<EXCEPTION_BIT_SHIFT)
#define	WATCH_EXCEPTION			(23<<EXCEPTION_BIT_SHIFT)
#define	VCOH_DATA_EXCEPTION		(31<<EXCEPTION_BIT_SHIFT)

// CPU Interrupt Cause Register Bits
#define	IP_0							0x100
#define	IP_1							0x200
#define	IP_2							0x400		// Int 0
#define	IP_3							0x800		// Int 1
#define	IP_4							0x1000	// Int 2
#define	IP_5							0x2000	// Int 3
#define	IP_6							0x4000	// Int 4
#define	IP_7							0x8000	// Timer/Int 5

#define	INT_CAUSE_MASK				(IP_0|IP_1|IP_2|IP_3|IP_4|IP_5|IP_6|IP_7)

// Definition of PCI Configuration register numbers
#define	PCI_CONFIG_REG0			0
#define	PCI_CONFIG_REG1			1
#define	PCI_CONFIG_REG2			2
#define	PCI_CONFIG_REG3			3
#define	PCI_CONFIG_REG4			4
#define	PCI_CONFIG_REG5			5
#define	PCI_CONFIG_REG6			6
#define	PCI_CONFIG_REG7			7
#define	PCI_CONFIG_REG8			8
#define	PCI_CONFIG_REG9			9
#define	PCI_CONFIG_REG10			10
#define	PCI_CONFIG_REG11			11
#define	PCI_CONFIG_REG12			12
#define	PCI_CONFIG_REG13			13
#define	PCI_CONFIG_REG14			14
#define	PCI_CONFIG_REG15			15


// Definitions of I/O ASIC UART Control register bits
//#define IOASIC_UART_GP_OUT     0x0080
//#define IOASIC_UART_ENABLE     0x0200
//#define IOASIC_UART_ENABLE_ERR 0x0400
//#define IOASIC_UART_LOOP_BACK  0x0800
//#define IOASIC_UART_XMIT_DIS   0x1000
//#define IOASIC_UART_SEND_BREAK 0x2000
//#define IOASIC_UART_TX_CTS     0x4000
//#define IOASIC_UART_RX_CTS     0x8000

// Definitions of I/O ASIC UART Status bits
#define IOASIC_UART_XMT_EMPTY  0x2000
#define IOASIC_UART_RCV_CHAR   0x1000
#define IOASIC_UART_RCV_FULL   0x0800
#define IOASIC_UART_OVER_RUN   0x0400
#define IOASIC_UART_FRAME      0x0200
#define IOASIC_UART_GOT_BREAK  0x0100


/*
 * The following manifest constant 0x0633 has the following meaning:
 *
 * Bit   15   0    -    CTS Output NOT forced false.
 * Bit   14   0    -    CTS Transmitter doesn't ignore CTS Input.
 * Bit   13   0    -    BREAK not being forced on UART.
 * Bit   12   0    -    Enable internal Transmitter data output.
 *
 * Bit   11   0    -    Disables Loop-Back
 * Bit   10   1    -    Error flags enabled
 * Bit    9   1    -    Enable/Reset internal UART
 * Bit    8   0    -    NC
 *
 * Bit    7   0    -    NC
 * Bit  6-0 0x33   -    9600 BAUD
 */ 
#define	IOASIC_UART_CTS_FALSE			(1<<15)
#define	IOASIC_UART_CTS_IGNORE			(1<<14)
#define	IOASIC_UART_SEND_BREAK			(1<<13)
#define	IOASIC_UART_INT_TX_ENABLE		(1<<12)
#define	IOASIC_UART_LOOP_BACK			(1<<11)
#define	IOASIC_UART_ERR_FLAG_ENABLE	(1<<10)
#define	IOASIC_UART_ENABLE				(1<<9)
#define	IOASIC_UART_57600_BAUD			(8)
#define	IOASIC_UART_38400_BAUD			(13)
#define	IOASIC_UART_19200_BAUD			(26)
#define	IOASIC_UART_9600_BAUD			(53)
#define	IOASIC_UART_4800_BAUD			(107)

#define	IOASIC_UART_RESET	IOASIC_UART_9600_BAUD
#define	IOASIC_UART_INIT	(IOASIC_UART_ERR_FLAG_ENABLE|\
	IOASIC_UART_CTS_IGNORE|IOASIC_UART_9600_BAUD)
//#define IOASIC_UART_INIT	0x0633
//#define IOASIC_UART_SPEED	0x0400

/*
 * This constant will reset the Internal UART in the I/O ASIC.
 */

//#define IOASIC_UART_RESET	~(IOASIC_UART_ENABLE)

#endif	// __SYSTEM_H__
