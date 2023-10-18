//
// ioctl.h - IOCTL header file
//
// $Revision: 8 $
//
#ifndef __IOCTL_H__
#define __IOCTL_H__

//
// general ioctl's
//
#define	FIOCNBLOCK		(('f'<<8)|1)	// set non-blocking io
#define	FIOCSCAN			(('f'<<8)|2)	// scan device for pending input
#define	FIOCINTBRK		(('f'<<8)|3)	// enable break interrupt
#define	FIOCINTBRKNOT	(('f'<<8)|4)	// disable break interrupt
#define	FIOCCLRINT		(('f'<<8)|5)	// clear external interrupt

//
// "tty" ioctl's
//
#define	TIOCRAW		(('t'<<8)|1)	// no special chars on char devices
#define	TIOCFLUSH	(('t'<<8)|2)	// flush input
#define	TIOCPROTO	(('t'<<8)|3)	// control pseudo console access
#define	TIOCREOPEN	(('t'<<8)|4)	// reopen to effect baud rate chg
#define	TIOCBAUD		(('t'<<8)|5)	// baud rate chg
#define	TIOCKEYHIT	(('t'<<8)|6)	// key hit

//
// file ioctl's
//
#define	FIOCSETDATE	(('f'<<8)|10)	// Set file time and date
#define	FIOCGETDATE	(('f'<<8)|11)	// Get file time and date
#define	FIOCSETATTR	(('f'<<8)|12)	// Set file attribute
#define	FIOCGETATTR	(('f'<<8)|13)	// Get file attribute
#define	FIOCFLUSH	(('f'<<8)|14)	// Flush the file system buffers

#if (!(PHOENIX_SYS & VEGAS))
//
// gt64010 ioctl's
//
#define	FIOCGETGTREG			(('s'<<8)|0)	// Get one of the registers
#define	FIOCSETGTREG			(('s'<<8)|1)	// Set one of the registers
#endif
#define	FIOCGETPCICONFIGREG	(('s'<<8)|2)	// Get a pci config register
#define	FIOCSETPCICONFIGREG	(('s'<<8)|3)	// Set a pci config register

typedef struct pci_reg_info
{
	int	slot_num;
	int	reg_num;
	int	data;
} pci_reg_info_t;

#if (!(PHOENIX_SYS & VEGAS))
typedef struct gt_reg_info
{
	int	reg_addr;
	int	data;
} gt_reg_info_t;
#endif

// Error codes returned by ioctls
#define	PCI_INVALID_IOCTL		-1
#define	PCI_INVALID_REG_NUM	-2
#define	PCI_INVALID_SLOT_NUM	-3


//
// I/O Asic ioctl's
//
#define	FIOCGETDIPSWITCHES	(('i'<<8)|0)	// Get the dipswitches
#define	FIOCGETCOININPUTS		(('i'<<8)|1)	// Get the coin inputs
#define	FIOCGETPLAYER12		(('i'<<8)|2)	// Get the player 1 & 2 inputs
#define	FIOCGETPLAYER34		(('i'<<8)|3)	// Get the player 3 & 4 inputs
#define	FIOCGETCOINMETERS		(('i'<<8)|4)	// Read the coin meters
#define	FIOCSETCOINMETERS		(('i'<<8)|5)	// Write the coin meters
#define	FIOCSETSOUNDCONTROL	(('i'<<8)|6)	// Write the sound control reg
#define	FIOCGETSOUNDCONTROL	(('i'<<8)|7)	// Read the sound control reg
#define	FIOCSETSOUNDDATA		(('i'<<8)|8)	// Write the sound data reg
#define	FIOCGETSOUNDSTATUS	(('i'<<8)|9)	// Get the sound status reg
#define	FIOCGETSOUNDDATA		(('i'<<8)|10)	// Get the sound data reg
#define	FIOCSETPICCOMMAND		(('i'<<8)|11)	// Set the pic command register
#define	FIOCGETPICCOMMAND		(('i'<<8)|12)	// Get the pic command register
#define	FIOCGETPICDATA			(('i'<<8)|13)	// Get the data register
#define	FIOCGETSTATUS			(('i'<<8)|14)	// Get the main status reg
#define	FIOCSETCONTROL			(('i'<<8)|15)	// Set the main control reg
#define	FIOCGETCONTROL			(('i'<<8)|16)	// Get the main control reg
#define	FIOCSETSTHDATA			(('i'<<8)|17)	// Write the sound to host reg
#define	FIOCSETAUXPORT			(('i'<<8)|18)	// Write to the aux out port

//
// Cmos ioctl's
//
#define	FIOCSETCMOSADDR		(('c'<<8)|0)	// Set rd/wr transfer address
#define	FIOCGETCMOSADDR		(('c'<<8)|1)	// Get cmos base address
#define	FIOCGETCMOSSIZE		(('c'<<8)|2)	// Get size of cmos
#define	FIOCGETCMOSDUMPRECORD	(('c'<<8)|3)	// Get CMOS dump record

// Cmos ioctl return values
#define	CMOS_INVALID_ADDRESS	-2					// Transfer address out of range

//
// Watchdog ioctl's
//
#define	FIOCGETWDOGSTATUS		(('w'<<8)|0)	// Get reset status
#define	FIOCWALKTHEDOG			(('w'<<8)|1)	// Walk the dog


//
// Timer ioctl's
//
#define	FIOCSTARTTIMER0			(('t'<<8)|0)	// Start timer 0
#define	FIOCSTARTTIMER1			(('t'<<8)|1)	// Start timer 1
#define	FIOCSTARTTIMER2			(('t'<<8)|2)	// Start timer 2
#define	FIOCSTARTTIMER3			(('t'<<8)|3)	// Start timer 3
#define	FIOCGETTIMER0				(('t'<<8)|4)	// Get timer 0
#define	FIOCGETTIMER1				(('t'<<8)|5)	// Get timer 1
#define	FIOCGETTIMER2				(('t'<<8)|6)	// Get timer 2
#define	FIOCGETTIMER3				(('t'<<8)|7)	// Get timer 3
#define	FIOCSTOPTIMER0				(('t'<<8)|8)	// Stop timer 0
#define	FIOCSTOPTIMER1				(('t'<<8)|9)	// Stop timer 1
#define	FIOCSTOPTIMER2				(('t'<<8)|10)	// Stop timer 2
#define	FIOCSTOPTIMER3				(('t'<<8)|11)	// Stop timer 3

//
// PIC Chip ioctl's
//
#define	FIOCGETSERIALNUMBER		(('p'<<8)|0)	// Get the serial number
#define	FIOCGETGAMENUMBER			(('p'<<8)|1)	// Get the game number
#define	FIOCGETDOM					(('p'<<8)|2)	// Get the date of manufacture
#define	FIOCSETSDATE				(('p'<<8)|3)	// Set the date
#define	FIOCGETSDATE				(('p'<<8)|4)	// Get the date
#define	FIOCSETSTIME				(('p'<<8)|5)	// Set the time
#define	FIOCGETSTIME				(('p'<<8)|6)	// Get the time


//
// Structures for sound system IOCTL's
//
typedef struct sound_data_buffer
{
	unsigned char	*buffer;			// Buffer to move data from/to
	unsigned int	sound_saddr;	// Where to get/send data
	unsigned int	sound_eaddr;	// Ending address
	unsigned int	size;				// Number of bytes of data
	unsigned int	checksum;		// Returned checksum
} sound_data_buffer_t;

//
// Sound system ioctl's
//
#define	FIOCSNDRESET				(('s'<<8)|0)	// Reset the sound system
#define	FIOCSNDRESETACK			(('s'<<8)|1)	// Reset sound with acknowledge
#define	FIOCSNDGETDATA				(('s'<<8)|2)	// Get sound data
#define	FIOCSNDSENDDATA			(('s'<<8)|3)	// Send sound data
#define	FIOCSNDSENDCMD				(('s'<<8)|4)	// Send sound command
#define	FIOCSNDSENDPMDATA			(('s'<<8)|5)	// Send data to program memory
#define	FIOCSNDSENDDRAMDATA		(('s'<<8)|6)	// Send data to dram
#define	FIOCSNDDELAY				(('s'<<8)|7)	// Delay a bit

//
// Coin driver ioctl's
//
#define	CIOCCLEARCOUNTS			(('c'<<8)|0)	// Clear all of the counters
#define	CIOCADDCOUNT0				(('c'<<8)|1)	// Add to counter 0
#define	CIOCADDCOUNT1				(('c'<<8)|2)	// Add to counter 2
#define	CIOCADDCOUNT2				(('c'<<8)|3)	// Add to counter 3
#define	CIOCADDCOUNT3				(('c'<<8)|4)	// Add to counter 4

//
// Prototype for ioctl function
//
int _ioctl(int, int, int);

//
// Interrupt handler numbers used for calls to install_handler().  This is
// the master list of all of the interrupts that can be generated by
// devices on the VEGAS system.
//
typedef enum
{
// Exception handlers
CPU_GENERAL_EXCEPTION_HANDLER_NUM,						// General exception
CPU_TLB_EXCEPTION_HANDLER_NUM,							// TLB exception
CPU_XTLB_EXCEPTION_HANDLER_NUM,							// XTLB exception
CPU_CACHE_ERROR_EXCEPTION_HANDLER_NUM,					// Cache error exception

// Hardware generated interrupts
CPU_IP0_HANDLER_NUM,											// CPU - IP0
CPU_IP1_HANDLER_NUM,											// CPU - IP1
CPU_IP2_HANDLER_NUM,											// CPU - IP2
IDE_HANDLER_NUM = CPU_IP2_HANDLER_NUM,					// PCI IDE disk interrupt
CPU_INT0_HANDLER_NUM = CPU_IP2_HANDLER_NUM,			// CPU - Int 0
CPU_IP3_HANDLER_NUM,											// CPU - IP3
SIO_HANDLER_NUM = CPU_IP3_HANDLER_NUM,					// SIO Board interrupt
CPU_INT1_HANDLER_NUM = CPU_IP3_HANDLER_NUM,			// CPU - Int 1
CPU_IP4_HANDLER_NUM,											// CPU - IP4
NILE4_HANDLER_NUM = CPU_IP4_HANDLER_NUM,				// NILE IV Interrupt
CPU_INT2_HANDLER_NUM = CPU_IP4_HANDLER_NUM,			// CPU - Int 2
CPU_IP5_HANDLER_NUM,											// CPU - IP5
CPU_INT3_HANDLER_NUM = CPU_IP5_HANDLER_NUM,			// CPU - Int 3
CPU_IP6_HANDLER_NUM,											// CPU - IP6
CPU_INT4_HANDLER_NUM = CPU_IP6_HANDLER_NUM,			// CPU - Int 4
CPU_IP7_HANDLER_NUM,											// CPU - IP7
DEBUG_HANDLER_NUM = CPU_IP7_HANDLER_NUM,				// Debugging interrupt
CPU_INT5_HANDLER_NUM = CPU_IP7_HANDLER_NUM,			// CPU - Int 5

// Interrupts from the processor
CPU_INT_HANDLER_NUM,											// CPU Interrupt
CPU_TLBM_HANDLER_NUM,										// CPU TLB Mod
CPU_TLBL_HANDLER_NUM,										// CPU TLB Load
CPU_TLBS_HANDLER_NUM,										// CPU TLB Store
CPU_ADEL_HANDLER_NUM,										// CPU Address Error (load)
CPU_ADES_HANDLER_NUM,										// CPU Address Error (store)
CPU_IBE_HANDLER_NUM,											// CPU Bus Error (fetch)
CPU_DBE_HANDLER_NUM,											// CPU Bus Error (data)
CPU_SYSCALL_HANDLER_NUM,									// CPU Syscall
CPU_BREAKPOINT_HANDLER_NUM,								// CPU Breakpoint
CPU_RESERVED_INSTRUCTION_HANDLER_NUM,					// CPU Reserved Instruction
CPU_CPU_HANDLER_NUM,											// CPU Coprocessor unusable
CPU_OVERFLOW_HANDLER_NUM,									// CPU Arithmethic overflow
CPU_TRAP_HANDLER_NUM,										// CPU Software trap
CPU_RESERVED_HANDLER_NUM,									// CPU Reserved
CPU_FPE_HANDLER_NUM,											// CPU Floating point

// Interrupts from the floating point co-processor
COP1_INEXACT_OPERATION_HANDLER_NUM,						// FPU Inexact operation
COP1_UNDERFLOW_HANDLER_NUM,								// FPU Underflow
COP1_OVERFLOW_HANDLER_NUM,									// FPU Overflow
COP1_DIVIDE_BY_ZERO_HANDLER_NUM,							// FPU Divide by 0 (zero)
COP1_INVALID_OPERATION_HANDLER_NUM,						// FPU Invalid Operation
COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM,				// FPU Unimplemented operation


// Interrupts from the SIO board
SIO_WDOG_TIMER_HANDLER_NUM,								// SIO - Watchdog timer
SIO_A2D_HANDLER_NUM,											// SIO - Analog to digital convertor
SIO_IOASIC_HANDLER_NUM,										// SIO - I/O ASIC
SIO_WIDGET_HANDLER_NUM,										// SIO - Widget board
SIO_HILINK_HANDLER_NUM = SIO_WIDGET_HANDLER_NUM,	// SIO - Hi-Link board
SIO_NSS_HANDLER_NUM = SIO_HILINK_HANDLER_NUM,		// SIO - NSS board
SIO_ETHERNET_HANDLER_NUM,									// SIO - Ethernet controller
SIO_VSYNC_HANDLER_NUM,										// SIO - Vertical sync

// Interrupts from the I/O ASIC on the SIO board
IOASIC_FORCE_INT_HANDLER_NUM,								// SIO - IOASIC Force interrupt
IOASIC_MICRO_ACK_HANDLER_NUM,								// SIO - IOASIC Microcontroller ACK
IOASIC_FIFO_EMPTY_HANDLER_NUM,							// SIO - IOASIC FIFO empty
IOASIC_FIFO_HALF_FULL_HANDLER_NUM,						// SIO - IOASIC FIFO half full
IOASIC_FIFO_FULL_HANDLER_NUM,								// SIO - IOASIC FIFO full
IOASIC_SND_STH_DATA_FULL_HANDLER_NUM,					// SIO - IOASIC Sound STH data full
IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM,					// SIO - IOASIC Sound HTS data empty
IOASIC_UART_BREAK_HANDLER_NUM,							// SIO - IOASIC UART Break detect
IOASIC_UART_FERROR_HANDLER_NUM,							// SIO - IOASIC UART Framing error
IOASIC_UART_OVERRUN_HANDLER_NUM,							// SIO - IOASIC UART Rx Overrun
IOASIC_UART_RX_FULL_HANDLER_NUM,							// SIO - IOASIC UART Rx Full
IOASIC_UART_RX_CHAR_HANDLER_NUM,							// SIO - IOASIC UART Rx char avail
IOASIC_UART_TX_EMPTY_HANDLER_NUM,						// SIO - IOASIC UART Tx Empty

// Interrupts from the NILE IV Chip
NILE4_CPU_PARITY_ERROR_HANDLER_NUM,						// NILE4 - CPU Parity error
NILE4_CPU_NO_TARGET_DECODE_HANDLER_NUM,				// NILE4 - CPU no-target decode
NILE4_MEMORY_CHECK_ERROR_HANDLER_NUM,					// NILE4 - Memory check error
NILE4_DMA_CONTROLLER_HANDLER_NUM,						// NILE4 - DMA controller
NILE4_UART_HANDLER_NUM,										// NILE4 - UART
NILE4_WATCHDOG_HANDLER_NUM,								// NILE4 - Watchdog timer
NILE4_GPT_HANDLER_NUM,										// NILE4 - General purpose timer
NILE4_LBT_HANDLER_NUM,										// NILE4 - Local bus timeout
NILE4_PCI_INTA_HANDLER_NUM,								// NILE4 - PCI inta
NILE4_PCI_INTB_HANDLER_NUM,								// NILE4 - PCI intb
NILE4_PCI_INTC_HANDLER_NUM,								// NILE4 - PCI intc
NILE4_PCI_INTD_HANDLER_NUM,								// NILE4 - PCI intd
NILE4_PCI_INTE_HANDLER_NUM,								// NILE4 - PCI inte
NILE4_RESERVED_HANDLER_NUM,								// NILE4 - Reserved
NILE4_PCI_SERR_HANDLER_NUM,								// NILE4 - PCI SERR
NILE4_PCI_INTERNAL_ERROR_HANDLER_NUM,					// NILE4 - PCI internal error

// Interrupts from the SMC Ethernet Chip
SIO_ETHERNET_RX_INT_HANDLER_NUM,							// SMC Receiver Int
SIO_ETHERNET_TX_INT_HANDLER_NUM,							// SMC Transmitter Int
SIO_ETHERNET_TX_EMPTY_INT_HANDLER_NUM,					// SMC Transmitter Empty Int
SIO_ETHERNET_ALLOC_INT_HANDLER_NUM,						// SMC Allocation Complete Int
SIO_ETHERNET_RX_OVERRUN_INT_HANDLER_NUM,				// SMC Receiver overrun Int
SIO_ETHERNET_EPH_INT_HANDLER_NUM,						// SMC Ethernet Protocol Int
SIO_ETHERNET_EARLY_RX_INT_HANDLER_NUM,					// SMC Early receive Int

ETHERNET_EPH_LINK_OK_HANDLER_NUM,						// SMC EPH - Link OK Int
ETHERNET_EPH_COUNTER_ROLLOVER_HANDLER_NUM,			// SMC EPH - Stats counter rollover Int
ETHERNET_EPH_TXENA_HANDLER_NUM,							// SMC EPH - Fatal TX error Int

ETHERNET_EPH_TXENA_TXUNRN_HANDLER_NUM,					// SMC EPH TXENA Transmit Underrun Int
ETHERNET_EPH_TXENA_SQET_HANDLER_NUM,					// SMC EPH TXENA SQE Error Int
ETHERNET_EPH_TXENA_LOST_CARR_HANDLER_NUM,				// SMC EPH TXENA Lost carrier Int
ETHERNET_EPH_TXENA_LATCOL_HANDLER_NUM,					// SMC EPH TXENA Late collision Int
ETHERNET_EPH_TXENA_16COL_HANDLER_NUM,					// SMC EPH TXENA 16 collisions Int

// Interrupts from the Snaphat Chip
SNAPHAT_ALARM_HANDLER_NUM,									// Alarm Clock from Snaphat
SNAPHAT_WATCHDOG_HANDLER_NUM,								// Watchdog from Snaphat

// End of handlers numbers marker
LAST_HANDLER_NUM												// End of list mark
} int_handler_num_t;

int set_handler(int, int (*)(int, unsigned int *));

#define	CMOS_RESERVE	2048

typedef struct exc_info {
	unsigned int	exc_code;
	unsigned int	pc;
	unsigned int	inst;
	unsigned int	gp_regs[32];
} exc_info_t;

typedef struct cmos_dump_record_header {
	unsigned long	crc;
	unsigned int	watchdog_count;
} cmos_dump_record_header_t;

#define	MAX_EXC_INFO_RECORDS	((CMOS_RESERVE-sizeof(cmos_dump_record_header_t))/sizeof(exc_info_t))

typedef struct	cmos_dump_record {
	cmos_dump_record_header_t	header;
	exc_info_t						exc_info[MAX_EXC_INFO_RECORDS];
} cmos_dump_record_t;


typedef enum {
SWID_P1_UP,
SWID_P1_DOWN,
SWID_P1_LEFT,
SWID_P1_RIGHT,
SWID_P1_A,
SWID_P1_B,
SWID_P1_C,
SWID_P1_D,

SWID_P2_UP,
SWID_P2_DOWN,
SWID_P2_LEFT,
SWID_P2_RIGHT,
SWID_P2_A,
SWID_P2_B,
SWID_P2_C,
SWID_P2_D,

SWID_P3_UP,
SWID_P3_DOWN,
SWID_P3_LEFT,
SWID_P3_RIGHT,
SWID_P3_A,
SWID_P3_B,
SWID_P3_C,
SWID_P3_D,

SWID_P4_UP,
SWID_P4_DOWN,
SWID_P4_LEFT,
SWID_P4_RIGHT,
SWID_P4_A,
SWID_P4_B,
SWID_P4_C,
SWID_P4_D,

SWID_LEFT_COIN,
SWID_RIGHT_COIN,
SWID_P1_START,
SWID_SLAM_TILT,
SWID_TEST,
SWID_P2_START,
SWID_CREDIT,
SWID_CENTER_COIN,
SWID_EXTRA_COIN,
SWID_P3_START,
SWID_P4_START,
SWID_VOL_DOWN,
SWID_VOL_UP,
SWID_CD_NOT_USED,
SWID_INTERLOCK,
SWID_DBV,

SWID_DIP0_0,
SWID_DIP0_1,
SWID_DIP0_2,
SWID_DIP0_3,
SWID_DIP0_4,
SWID_DIP0_5,
SWID_DIP0_6,
SWID_DIP0_7,

SWID_DIP1_0,
SWID_DIP1_1,
SWID_DIP1_2,
SWID_DIP1_3,
SWID_DIP1_4,
SWID_DIP1_5,
SWID_DIP1_6,
SWID_DIP1_7
} switch_id_t;

//
// Assorted other definitions
//
#define	MAX_PLAYERS					4
#define	MAX_SWITCHES_PER_PLAYER	8
#define	MAX_DIPS						2
#define	MAX_SWITCHES_PER_DIP		8
#define	MAX_GUNS						2
#define	MAX_SWITCHES_PER_GUN		8
#define	MAX_COIN_DOOR_SWITCHES	16
#define	MAX_PLAYER_SWITCHES		(MAX_PLAYERS*MAX_SWITCHES_PER_PLAYER)
#define	MAX_DIP_SWITCHES			(MAX_DIPS*MAX_SWITCHES_PER_DIP)
#define	MAX_GUN_SWITCHES			(MAX_GUNS*MAX_SWITCHES_PER_GUN)
#define	MAX_SWITCHES				(MAX_PLAYER_SWITCHES+MAX_DIP_SWITCHES+MAX_GUN_SWITCHES+MAX_COIN_DOOR_SWITCHES)
#define	MAX_A2D_CHANNELS			8

typedef struct idata
{
	unsigned short	p12;
	unsigned short	p34;
	unsigned short	coin;
	unsigned short	dip;
	unsigned int	on_transitions[MAX_SWITCHES];
	unsigned int	a2d[MAX_A2D_CHANNELS];
} idata_t;
	
#endif
