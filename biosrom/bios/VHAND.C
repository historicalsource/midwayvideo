//
// vhand.c - Interrupt handlers for the VEGAS system
//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 16 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<ioctl.h>

#define	EXC_DEBUG

//
// Uncomment this to enable nested interrupt processing
//
//#define	NESTED_INTERRUPTS

#define	SINGLE_PREC_FLOAT	16
#define	DOUBLE_PREC_FLOAT	17

//
// External variables
//
extern char	active;
extern char	istat;
extern int	exregs[];
extern char	reg_runflag;
extern char	reg_extype;
extern int	__memory_size;
extern int	(*myputc)(char);
#ifndef TEST
extern int	debug_capable;
#endif

extern void coinintr(void);
extern void update_cmos(void);

typedef struct int_priority_info
{
	int	int_bit;
	int	higher_pri;
	void	(*func)(int, unsigned int *);
} int_priority_info_t;


//
// Global variables
//
unsigned long	*except_regs;

//
// Global Function prototypes
//
void reset_ip0(void);
void reset_ip1(void);
unsigned int *get_epc(void);
int disasm(unsigned *addr, int mode, int regset);
int	printf(const char *_format, ...);
void clear_user_handlers(void);
void copy_regs(unsigned char *to, unsigned char *from, int num);


#ifndef TEST
void setup_debug_service(void);
static void start_debug(unsigned int *regs, int status);
void a2d_intr(void);
#endif
int	UARTputc(char);
int	dputc(char);
void set_fcolor(int);
void enable_cop1_int(int bit);
void disable_cop1_int(int mask);
void unhandled_exception(char *str);
#ifdef TTY_INTERRUPTS
void ttyintr(void);
#endif

#ifdef VMM
void vmm_tlbm_handler(int, unsigned int *);
void vmm_tlbl_handler(int, unsigned int *);
void vmm_tlbs_handler(int, unsigned int *);
void vmm_tlb_exception_handler(int, unsigned int *);
#endif

//
// Local Function prototypes
//
static void ioasic_force_int_handler(int, unsigned int *);
static void ioasic_micro_ack_handler(int, unsigned int *);
static void ioasic_fifo_empty_handler(int, unsigned int *);
static void ioasic_fifo_half_full_handler(int, unsigned int *);
static void ioasic_fifo_full_handler(int, unsigned int *);
static void ioasic_snd_sth_data_full_handler(int, unsigned int *);
static void ioasic_snd_hts_data_empty_handler(int, unsigned int *);
static void ioasic_uart_break_handler(int, unsigned int *);
static void ioasic_uart_ferror_handler(int, unsigned int *);
static void ioasic_uart_overrun_handler(int, unsigned int *);
static void ioasic_uart_rx_full_handler(int, unsigned int *);
static void ioasic_uart_rx_char_handler(int, unsigned int *);
static void ioasic_uart_tx_empty_handler(int, unsigned int *);
static void sio_snaphat_handler(int, unsigned int *);
static void sio_a2d_handler(int, unsigned int *);
static void sio_ioasic_handler(int, unsigned int *);
static void sio_widget_handler(int, unsigned int *);
static void sio_ethernet_handler(int, unsigned int *);
static void sio_vsync_handler(int, unsigned int *);
static void nile4_cpu_parity_error_handler(int, unsigned int *);
static void nile4_cpu_no_target_decode_handler(int, unsigned int *);
static void nile4_memory_check_error_handler(int, unsigned int *);
static void nile4_dma_controller_handler(int, unsigned int *);
static void nile4_uart_handler(int, unsigned int *);
static void nile4_watchdog_handler(int, unsigned int *);
static void nile4_gpt_handler(int, unsigned int *);
static void nile4_lbt_handler(int, unsigned int *);
static void nile4_pci_inta_handler(int, unsigned int *);
static void nile4_pci_intb_handler(int, unsigned int *);
static void nile4_pci_intc_handler(int, unsigned int *);
static void nile4_pci_intd_handler(int, unsigned int *);
static void nile4_pci_inte_handler(int, unsigned int *);
static void nile4_pci_serr_handler(int, unsigned int *);
static void nile4_pci_internal_error_handler(int, unsigned int *);
static void cpu_ip0_handler(int, unsigned int *);
static void cpu_ip1_handler(int, unsigned int *);
static void cpu_ip2_handler(int, unsigned int *);
static void cpu_ip3_handler(int, unsigned int *);
static void cpu_ip4_handler(int, unsigned int *);
static void cpu_ip5_handler(int, unsigned int *);
static void cpu_ip6_handler(int, unsigned int *);
static void cpu_ip7_handler(int, unsigned int *);
static void fpu_inexact_handler(int, unsigned int *);
static void fpu_underflow_handler(int, unsigned int *);
static void fpu_overflow_handler(int, unsigned int *);
static void fpu_div0_handler(int, unsigned int *);
static void fpu_invalid_operation_handler(int, unsigned int *);
static void fpu_unimplemented_operation_handler(int, unsigned int *);
static void cpu_int_handler(int, unsigned int *);
static void cpu_tlbm_handler(int, unsigned int *);
static void cpu_tlbl_handler(int, unsigned int *);
static void cpu_tlbs_handler(int, unsigned int *);
static void cpu_adel_handler(int, unsigned int *);
static void cpu_ades_handler(int, unsigned int *);
static void cpu_ibe_handler(int, unsigned int *);
static void cpu_dbe_handler(int, unsigned int *);
static void cpu_syscall_handler(int, unsigned int *);
static void cpu_breakpoint_handler(int, unsigned int *);
static void cpu_reserved_instruction_handler(int, unsigned int *);
static void cpu_cpu_handler(int, unsigned int *);
static void cpu_overflow_handler(int, unsigned int *);
static void cpu_trap_handler(int, unsigned int *);
static void cpu_fpe_handler(int, unsigned int *);
static void no_handler(int, unsigned int *);
static void ethernet_eph_txena_transmit_underrun_handler(int, unsigned int *);
static void ethernet_eph_txena_sqet_handler(int, unsigned int *);
static void ethernet_eph_txena_lost_carrier_handler(int, unsigned int *);
static void ethernet_eph_txena_late_collision_handler(int, unsigned int *);
static void ethernet_eph_txena_16collision_handler(int, unsigned int *);
static void ethernet_eph_link_ok_handler(int, unsigned int *);
static void ethernet_eph_counter_rollover_handler(int, unsigned int *);
static void ethernet_eph_txena_handler(int, unsigned int *);
static void ethernet_rx_int_handler(int, unsigned int *);
static void ethernet_tx_int_handler(int, unsigned int *);
static void ethernet_tx_empty_int_handler(int, unsigned int *);
static void ethernet_alloc_int_handler(int, unsigned int *);
static void ethernet_rx_overrun_int_handler(int, unsigned int *);
static void ethernet_eph_int_handler(int, unsigned int *);
static void ethernet_early_rx_int_handler(int, unsigned int *);
static void	snaphat_alarm_handler(int, unsigned int *);
static void	snaphat_watchdog_handler(int, unsigned int *);


//
// Definitions of interrupt priorities
//
#define	PRIORITY_1_INT	NILE4_INT
#define	PRIORITY_2_INT	IDE_DISK_INT
#define	PRIORITY_3_INT	SIO_INT
#define	PRIORITY_4_INT	SCSI_INT
#define	PRIORITY_5_INT	IP_7
#define	PRIORITY_6_INT	IP_5
#define	PRIORITY_7_INT	IP_1
#define	PRIORITY_8_INT	IP_0

//
// Definitions of interrupt service functions
//
#define	priority_1_func	cpu_ip4_handler
#define	priority_2_func	cpu_ip2_handler
#define	priority_3_func	cpu_ip3_handler
#define	priority_4_func	cpu_ip6_handler
#define	priority_5_func	cpu_ip7_handler
#define	priority_6_func	cpu_ip5_handler
#define	priority_7_func	cpu_ip1_handler
#define	priority_8_func	cpu_ip0_handler

//
// Definitions of interrupts that are higher priority for each interrupt
//
#define	PRIORITY_1_HIGHER	0
#define	PRIORITY_2_HIGHER	(PRIORITY_1_HIGHER|PRIORITY_1_INT)
#define	PRIORITY_3_HIGHER	(PRIORITY_2_HIGHER|PRIORITY_2_INT)
#define	PRIORITY_4_HIGHER	(PRIORITY_3_HIGHER|PRIORITY_3_INT)
#define	PRIORITY_5_HIGHER	(PRIORITY_4_HIGHER|PRIORITY_4_INT)
#define	PRIORITY_6_HIGHER	(PRIORITY_5_HIGHER|PRIORITY_5_INT)
#define	PRIORITY_7_HIGHER	(PRIORITY_6_HIGHER|PRIORITY_6_INT)
#define	PRIORITY_8_HIGHER	(PRIORITY_7_HIGHER|PRIORITY_7_INT)

//
// Table of interrupt priorities in order of highest to lowest priority
//
static int_priority_info_t	int_priority_info[] = {
{PRIORITY_1_INT, PRIORITY_1_HIGHER, priority_1_func},
{PRIORITY_2_INT, PRIORITY_2_HIGHER, priority_2_func},
{PRIORITY_3_INT, PRIORITY_3_HIGHER, priority_3_func},
{PRIORITY_4_INT, PRIORITY_4_HIGHER, priority_4_func},
{PRIORITY_5_INT, PRIORITY_5_HIGHER, priority_5_func},
{PRIORITY_6_INT, PRIORITY_6_HIGHER, priority_6_func},
{PRIORITY_7_INT, PRIORITY_7_HIGHER, priority_7_func},
{PRIORITY_8_INT, PRIORITY_8_HIGHER, priority_8_func},
};

unsigned int splx(int);


//
// Local variables
//
static volatile unsigned long long	vsync_count = 0L;
static volatile unsigned long long	elapsed_time = 0L;
volatile unsigned long			 		watchdog_count = 0;
#ifndef TEST
volatile unsigned long long			vsync_timestamp = 0L;
#endif

//
// Table of application installed interrupt handlers
//
static int	(*user_handler[LAST_HANDLER_NUM])(int, unsigned int *);

//
// Table of functions used to service interrupts from the Snaphat
//
static void	(*snaphat_func[SNAPHAT_WATCHDOG_HANDLER_NUM - SNAPHAT_ALARM_HANDLER_NUM + 1])(int, unsigned int *) = {
snaphat_alarm_handler,
snaphat_watchdog_handler
};

//
// Table of functions used to service SMC Ethernet Controll EPH TXEN interrupts
//
static void	(*ether_eph_txena_func[ETHERNET_EPH_TXENA_16COL_HANDLER_NUM - ETHERNET_EPH_TXENA_TXUNRN_HANDLER_NUM + 1])(int, unsigned int *) = {
ethernet_eph_txena_transmit_underrun_handler,
ethernet_eph_txena_sqet_handler,
ethernet_eph_txena_lost_carrier_handler,
ethernet_eph_txena_late_collision_handler,
ethernet_eph_txena_16collision_handler
};

//
// Table of functions used to service SMC Ethernet Controller EPH Interrupts
//
static void	(*ether_eph_func[ETHERNET_EPH_TXENA_HANDLER_NUM - ETHERNET_EPH_LINK_OK_HANDLER_NUM + 1])(int, unsigned int *) = {
ethernet_eph_link_ok_handler,
ethernet_eph_counter_rollover_handler,
ethernet_eph_txena_handler
};

//
// Table of functions used to service SMC Ethernet Controller Interrupts
//
static void	(*ether_func[SIO_ETHERNET_EARLY_RX_INT_HANDLER_NUM - SIO_ETHERNET_RX_INT_HANDLER_NUM + 1])(int, unsigned int *) = {
ethernet_rx_int_handler,
ethernet_tx_int_handler,
ethernet_tx_empty_int_handler,
ethernet_alloc_int_handler,
ethernet_rx_overrun_int_handler,
ethernet_eph_int_handler,
ethernet_early_rx_int_handler
};

//
// Table of functions used to service I/O ASIC interrupts
//
static void (*ioasic_func[IOASIC_UART_TX_EMPTY_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM + 1])(int, unsigned int *) = {
ioasic_force_int_handler,
ioasic_micro_ack_handler,
ioasic_fifo_empty_handler,
ioasic_fifo_half_full_handler,
ioasic_fifo_full_handler,
ioasic_snd_sth_data_full_handler,
ioasic_snd_hts_data_empty_handler,
ioasic_uart_break_handler,
ioasic_uart_ferror_handler,
ioasic_uart_overrun_handler,
ioasic_uart_rx_full_handler,
ioasic_uart_rx_char_handler,
ioasic_uart_tx_empty_handler
};

//
// Functions used to handle interrupts from the SIO board
//
static void (*sio_func[SIO_VSYNC_HANDLER_NUM - SIO_WDOG_TIMER_HANDLER_NUM + 1])(int, unsigned int *) = {
sio_snaphat_handler,
sio_a2d_handler,
sio_ioasic_handler,
sio_widget_handler,
sio_ethernet_handler,
sio_vsync_handler
};

//
// Functions used to service interrupts from the NILE IV chip
//
static void (*nile4_func[NILE4_PCI_INTERNAL_ERROR_HANDLER_NUM - NILE4_CPU_PARITY_ERROR_HANDLER_NUM + 1])(int, unsigned int *) = {
nile4_cpu_parity_error_handler,
nile4_cpu_no_target_decode_handler,
nile4_memory_check_error_handler,
nile4_dma_controller_handler,
nile4_uart_handler,
nile4_watchdog_handler,
nile4_gpt_handler,
nile4_lbt_handler,
nile4_pci_inta_handler,
nile4_pci_intb_handler,
nile4_pci_intc_handler,
nile4_pci_intd_handler,
nile4_pci_inte_handler,
no_handler,
nile4_pci_serr_handler,
nile4_pci_internal_error_handler
};

//
// Functions used to handle individual CPU interrupts.  MUST be in the same
// order as the handler numbers in ioctl.h
//
static void (*int_func[CPU_IP7_HANDLER_NUM - CPU_IP0_HANDLER_NUM + 1])(int, unsigned int *) = {
cpu_ip0_handler,
cpu_ip1_handler,
cpu_ip2_handler,
cpu_ip3_handler,
cpu_ip4_handler,
cpu_ip5_handler,
cpu_ip6_handler,
cpu_ip7_handler
};

//
// Functions used to handle FPU exceptions
//
static void (*fpe_handler[COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM - COP1_INEXACT_OPERATION_HANDLER_NUM + 1])(int, unsigned int *) = {
fpu_inexact_handler,
fpu_underflow_handler,
fpu_overflow_handler,
fpu_div0_handler,
fpu_invalid_operation_handler,
fpu_unimplemented_operation_handler
};

//
// Functions used to handle individual CPU exceptions.  MUST be in the same
// order as the handler numbers in ioctl.h
//
static void (*exc_func[CPU_FPE_HANDLER_NUM - CPU_INT_HANDLER_NUM + 1])(int, unsigned int *) = {
cpu_int_handler,
cpu_tlbm_handler,
cpu_tlbl_handler,
cpu_tlbs_handler,
cpu_adel_handler,
cpu_ades_handler,
cpu_ibe_handler,
cpu_dbe_handler,
cpu_syscall_handler,
cpu_breakpoint_handler,
cpu_reserved_instruction_handler,
cpu_cpu_handler,
cpu_overflow_handler,
cpu_trap_handler,
no_handler,
cpu_fpe_handler
};

//
// Flag used to determine whether or not debug service has been entered
//
volatile int	debug_service = 0;

static char	*reg_names[] = {
"zero:", "at:", "v0:", "v1:", "a0:", "a1:", "a2:", "a3:",
"t0:", "t1:", "t2:", "t3:", "t4:", "t5:", "t6:", "t7:", 
"s0:", "s1:", "s2:", "s3:", "s4:", "s5:", "s6:", "s7:",
"t8:", "t9:", "k0:", "k1:", "gp:", "sp:", "fp:", "ra:"
};

static char	*short_reg_names[] = {
"00:", "01:", "02:", "03:", "04:", "05:", "06:", "07:",
"08:", "09:", "10:", "11:", "12:", "13:", "14:", "15:", 
"16:", "17:", "18:", "19:", "20:", "21:", "22:", "23:",
"24:", "25:", "26:", "27:", "28:", "29:", "30:", "31:"
};

static char *creg_names[] = {
"index:",
"random:",
"entrylo0:",
"entrylo1:",
"context:",
"pagemask:",
"wired:",
"NOT USED:",
"badvaddr:",
"count:",
"entryHi:",
"compare:",
"status:",
"cause:",
"epc:",
"prid:",
"config:",
"lladdr:",
"NOT USED:",
"NOT USED:",
"XContext:",
"NOT USED:",
"NOT USED:",
"NOT USED:",
"NOT USED:",
"NOT USED:",
"ecc:",
"cacheerr:",
"taglo:",
"taghi:",
"errorpc:",
"NOT USED:"
};


//
// Function used to display information about the execption that occurred.
//
extern int	exc_stack_end[];

static char	*exc_type_str[] = {
"Interrupt",
"TLB Modification",
"TLB Load",
"TLB Store",
"Address Error Load",
"Address Error Store",
"Bus Error Fetch",
"Bus Error Data",
"System call",
"Breakpoint",
"Reserved Instruction",
"Coprocessor Unusable",
"Arithmetic Overflow",
"Trap",
"Reserved",
"Floating Point"
};


//
// Definition of a register on the exception stack
//
typedef struct long_reg
{
	unsigned int			lo;
	unsigned int			hi;
} long_reg_t;


//
// Definition of an exception stack entry
//
typedef struct exc_stack_regs
{
	long_reg_t		gp[32];
	long_reg_t		lo;
	long_reg_t		hi;
	long_reg_t		pc;
	long_reg_t		cp0[32];
	long_reg_t		fp[32];
	unsigned int	fcr0;
	unsigned int	fcr31;
} exc_stack_regs_t;

//
// Nested exception level
//
volatile int							exception_level = 0;

//
// Registers for each exception level
//
static volatile exc_stack_regs_t	exception_regs[16];

static void show_exc_info(char *str)
{
	unsigned int	*pc;
	unsigned int	*disasm_pc;
	unsigned int	*sp;
	int				i;
	int				is_uart = 0;
	unsigned int	*r_save;
	int				exc_type;
	int				con;
	int				cause;

	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0;

#ifdef TEST
	if(myputc == dputc)
	{
		is_uart = 1;
	}
#else
	is_uart = 1;
#endif

	for(con = 0; con < exception_level; con++)
	{
		r_save = (unsigned int *)&exception_regs[con];

		cause = r_save[CP0_CAUSE];
		pc = (unsigned int *)r_save[PC];
		disasm_pc = pc;
		if(cause & 0x80000000)
		{
			pc++;
		}
		sp = (unsigned int *)r_save[GP29];
		if(!is_uart)
		{
			printf("\f");
			set_fcolor(0xf800);
		}
		else
		{
			printf("\n");
			printf("******************************************************************************\n");
		}
		exc_type = (cause >> 2) & 0xf;
		printf("\nException frame: %d - %p\n", con, r_save);
		if(exc_type)
		{
			printf("Exception:  %s\n\n", exc_type_str[exc_type]);
		}
		else
		{
			cause &= r_save[CP0_STATUS];
			printf("Exception:  Interrupt -");
			for(i = 0; i < 8; i++)
			{
				if(cause & (0x100 << i))
				{
					printf(" IP%d", i);
				}
			}
			printf("\n\n");
		}
		if(!is_uart)
		{
			set_fcolor(-1);
		}

		except_regs = (unsigned long *)r_save;
		if(!is_uart)
		{
			set_fcolor(0xffe0);
		}

		if(!((int)pc & 3) && (unsigned int)pc >= (unsigned int)0x80000000)
		{
			disasm(disasm_pc, 3, 0);
			if(!is_uart)
			{
				set_fcolor(-1);
			}
			printf("\n\n");
		}
		else
		{
			printf("Program Count: 0x%08.8X is BOGUS\n", (int)pc);
		}

		printf("General Purpose Registers");
		for(i = 0; i < 32; i++)
		{
			if(!(i & 3))
			{
				printf("\n");
			}
			if(is_uart)
			{
				printf("%-5s", reg_names[i]);
			}
			else
			{
				set_fcolor(0x07ff);
				printf("%-4s", short_reg_names[i]);
				set_fcolor(-1);
			}
			if(!i)
			{
				printf("00000000  ");
			}
			else
			{
				printf("%08X  ", r_save[(i << 1) + GP0]);
			}
		}

		if(is_uart)
		{
			printf("\n");
			printf("\n");

			printf("CP0 Registers");
			for(i = 0; i < 32; i++)
			{
				if(!(i & 3))
				{
					printf("\n");
				}
				printf("%-9s%08X  ", creg_names[i], r_save[(i<<1) + CP0_INDEX]);
			}
		}

		printf("\n\n");
		sp -= 64;
		if(((unsigned int)sp >> 27) >= 0x8)
		{
			printf("Stack");
			for(i = 0; i < 64; i++)
			{
				if(!(i & 3))
				{
					if(!is_uart)
					{
						set_fcolor(0xffe0);
					}
					printf("\n0x%08X  ", (int)sp);
					if(!is_uart)
					{
						set_fcolor(-1);
					}
				}
				printf("0x%08X ", *sp);
				sp++;
			}
			printf("\n");
		}
		else
		{
			printf("Stack pointer is NOT valid\n");
		}
		if(!is_uart)
		{
			break;
		}
	}
	if(is_uart)
	{
		if(exception_level)
		{
			printf("\n******************************************************************************\n");
		}
		else
		{
			printf("\n\n******* %s *******\n\n", str);
		}
	}
}


//
// Functions used to reset the IP0 and IP1 interrupts
//
__asm__("
	.set	noreorder
	.globl	reset_ip0
reset_ip0:
	mfc0	$8,$13
	li		$9,0x100
	not	$9
	and	$8,$8,$9
	jr		$31
	mtc0	$8,$13
	.set	reorder

	.set	noreorder
	.globl	reset_ip1
reset_ip1:
	mfc0	$8,$13
	li		$9,0x200
	not	$9
	and	$8,$8,$9
	jr		$31
	mtc0	$8,$13
	.set	reorder
");

//
// Software caused interrupt
//
static void cpu_ip0_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the interrupt
	//
	reset_ip0();

	//
	// Is there a user installed handler for this interrupt ?
	//
	if(user_handler[CPU_IP0_HANDLER_NUM])
	{
		//
		// YES - Call it - Did it return non-null ?
		//
		if(user_handler[CPU_IP0_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call the system handler
			//
			unhandled_exception("Interrupt IP0");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - Call system handler
	//
	unhandled_exception("Interrupt IP0");
}

//
// Software caused interrupt
//
static void cpu_ip1_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the interrupt
	//
	reset_ip1();

	//
	// Is there a user installed handler for this interrupt ?
	//
	if(user_handler[CPU_IP1_HANDLER_NUM])
	{
		//
		// YES - Call it - Did it return non-null ?
		//
		if(user_handler[CPU_IP1_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call the system handler
			//
			unhandled_exception("Interrupt IP1");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - Call system handler
	//
	unhandled_exception("Interrupt IP1");
}


//
// CPU Int 0 (IP2) Handler - IDE Disk interrupt
//
static void cpu_ip2_handler(int cause, unsigned int *r_save)
{
	//
	// Call the IDE Disk interrupt handler
	//
	ide_intr();

	//
	// Is there a user installed interrupt handler ?
	//
	if(user_handler[CPU_IP2_HANDLER_NUM])
	{
		//
		// YES - Call it
		//
		user_handler[CPU_IP2_HANDLER_NUM](cause, r_save);
	}
}


//
// Interrupt handler for interrupts from the alarm section of the
// SNAPHAT device.
//
static void snaphat_alarm_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the alarm interrupt
	//

	//
	// Is there an user installed handler ?
	//
	if(user_handler[SNAPHAT_ALARM_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SNAPHAT_ALARM_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call the system interrupt handler
			//
			unhandled_exception("Interrupt - SNAPHAT Alarm");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SNAPHAT Alarm");
}


//
// Interrupt handler for interrupts froom the watchdog section of the
// SNAPHAT device.
//
static void snaphat_watchdog_handler(int cause, unsigned int *r_save)
{
	unsigned char	tmp;

	//
	// Clear the watchdog interrupt
	//
	tmp = *((volatile char *)RTC_WATCHDOG_REG);
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0;
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = tmp;

	//
	// Is there an user installed handler ?
	//
	if(user_handler[SNAPHAT_WATCHDOG_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SNAPHAT_WATCHDOG_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Interrupt - SNAPHAT Watchdog");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Interrupt - SNAPHAT Watchdog");
}


//
// Function called upon detection of interrupt from the SNAPHAT device
// on the SIO board.
//
static void sio_snaphat_handler(int cause, unsigned int *r_save)
{
	int				i;
	unsigned char	snaphat_cause;

	//
	// Grab the interrupt pending flags
	//
	snaphat_cause = ((*((volatile char *)RTC_FLAGS_REG) >> 6) & 0x3);

	//
	// Is the alarm interrupt enabled ?
	//
	if(*((volatile char *)RTC_INT_ENABLE_REG) & 0x80)
	{
		//
		// YES - allow it to be looked at
		//
		snaphat_cause &= 3;
	}
	else
	{
		//
		// NO - Don't bother with alarm interrupt if it is pending
		//
		snaphat_cause &= 2;
	}

	//
	// Service all of the pending AND enable interrupts from the device
	//
	for(i = 0; i <= (SNAPHAT_WATCHDOG_HANDLER_NUM - SNAPHAT_ALARM_HANDLER_NUM); i++)
	{
		//
		// Is the interrupt pending AND enabled ?
		//
		if(snaphat_cause & (1 << i))
		{
			//
			// YES - service it
			//
			snaphat_func[i](cause, r_save);
		}
	}
}


//
// Handler used for interrupts from the A2D convertor section of the
// SIO board.
//
static void sio_a2d_handler(int cause, unsigned int *r_save)
{
#ifndef TEST
	//
	// Reset the interrupt from the A2D
	//
	a2d_intr();

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[SIO_A2D_HANDLER_NUM])
	{
		//
		// YES - Call it - Did it return non-null ?
		user_handler[SIO_A2D_HANDLER_NUM](cause, r_save);
	}
#else
	unhandled_exception("Interrupt - SIO A2D");
#endif
}


//
// Handler used for interrupts from the Forced Interrupt from the I/O ASIC
// section of the SIO board.
//
static void ioasic_force_int_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the force interrupt from the I/O ASIC
	//
	*((volatile short *)IOASIC_CONTROL) &= ~(2 << (IOASIC_FORCE_INT_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM));

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_FORCE_INT_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_FORCE_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_FORCE_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC Forced Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC Forced Int");
}


//
// Handler used for the Microcontroller ACK interrupt section of the
// SIO board.
//
static void ioasic_micro_ack_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the mico-ack interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_MICRO_ACK_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_MICRO_ACK_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_MICRO_ACK_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC Micro Ack Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC Micro Ack Int");
}


//
// Handler used for the FIFO empty interrupt section of the
// SIO board.
//
static void ioasic_fifo_empty_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the FIFO empty interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_FIFO_EMPTY_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_FIFO_EMPTY_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_FIFO_EMPTY_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC FIFO Empty Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC FIFO Empty Int");
}


//
// Handler used for the FIFO half full interrupt section of the
// SIO board.
//
static void ioasic_fifo_half_full_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the FIFO half full interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_FIFO_HALF_FULL_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_FIFO_HALF_FULL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_FIFO_HALF_FULL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC FIFO Half Empty Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC FIFO Half Empty Int");
}


//
// Handler used for the FIFO full interrupt section of the
// SIO board.
//
static void ioasic_fifo_full_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the FIFO Full Interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_FIFO_FULL_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_FIFO_FULL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_FIFO_FULL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC FIFO Full Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC FIFO Full Int");
}


//
// Handler used for the Sound to Host data full interrupt section
// of the SIO board.
//
static void ioasic_snd_sth_data_full_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the Sound to Host Data Full Interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_SND_STH_DATA_FULL_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_SND_STH_DATA_FULL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_SND_STH_DATA_FULL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC Sound To Host Data Full Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC Sound To Host Data Full Int");
}


//
// Handler used for the Host to Sound data Empty interrupt section of
// the SIO board.
//
static void ioasic_snd_hts_data_empty_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the Host to Sound Data Empty interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC Host to Sound Data Empty Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC Host to Sound Data Empty Int");
}


//
// Handler used for the I/O ASIC UART break interrupt section of the
// SIO board.
//
static void ioasic_uart_break_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the break if being generated by me
	//
	if(*((volatile short *)IOASIC_UART_CONTROL) & 0x2000)
	{
		*((volatile short *)IOASIC_UART_CONTROL) &= ~0x2000;
	}

	//
	// Reset the Break interrupt from the I/O ASIC UART
	//
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_UART_BREAK_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_UART_BREAK_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_UART_BREAK_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC UART Break Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC UART Break Int");
}


//
// Handler used for the I/O ASIC UART framing error interrupt section of the
// SIO board.
//
static void ioasic_uart_ferror_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the I/O ASIC UART Framing Error interrupt
	//
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_UART_FERROR_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_UART_FERROR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_UART_FERROR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC UART Framing Error Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC UART Framing Error Int");
}


//
// Handler used for the I/O ASIC UART overrun error interrupt section of the
// SIO board.
//
static void ioasic_uart_overrun_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the I/O ASIC UART Overrun Error interrupt
	//
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_UART_OVERRUN_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_UART_OVERRUN_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_UART_OVERRUN_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC UART Overrun Error Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC UART Overrun Error Int");
}


//
// Handler used for the I/O ASIC UART rx full interrupt section of the
// SIO board.
//
static void ioasic_uart_rx_full_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the I/O ASIC UART Rx Full Interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_UART_RX_FULL_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_UART_RX_FULL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_UART_RX_FULL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC UART Rx Full Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC UART Rx Full Int");
}


//
// Handler used for the I/O ASIC UART rx interrupt section of the
// SIO board.
//
static void ioasic_uart_rx_char_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the I/O ASIC UART Rx Character Interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_UART_RX_CHAR_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_UART_RX_CHAR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_UART_RX_CHAR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC UART Rx Char Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC UART Rx Char Int");
}


//
// Handler used for the I/O ASIC UART tx empty interrupt section of the
// SIO board.
//
static void ioasic_uart_tx_empty_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the I/O ASIC UART Tx Empty Interrupt
	//

	//
	// Wait for the cause register to indicate the interrupt has been cleared
	//
//	while(*((volatile short *)IOASIC_STATUS) & (2 << (IOASIC_UART_TX_EMPTY_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM))) ;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[IOASIC_UART_TX_EMPTY_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[IOASIC_UART_TX_EMPTY_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler too
			//
			unhandled_exception("Interrupt - SIO I/O ASIC UART Tx Empty Int");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO I/O ASIC UART Tx Empty Int");
}


//
// Handler used for interrupts from the I/O ASIC device section of then
// SIO board.
//
static void sio_ioasic_handler(int cause, unsigned int *r_save)
{
	int				i;
	unsigned short	ioasic_cause;

	//
	// Get the cause of the interrupt
	//
	ioasic_cause = *((volatile unsigned short *)IOASIC_STATUS);

	//
	// Mask out any interrupts that are NOT enabled
	//
	ioasic_cause &= *((volatile unsigned short *)IOASIC_CONTROL);

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (IOASIC_UART_TX_EMPTY_HANDLER_NUM - IOASIC_FORCE_INT_HANDLER_NUM); i++)
	{
		//
		// Is the interrupt pending ?
		//
		if(ioasic_cause & (2 << i))
		{
			//
			// YES - service it
			//
			ioasic_func[i](cause, r_save);
		}
	}
}


//
// SIO Widget board interrupt handler
//
static void sio_widget_handler(int cause, unsigned int *r_save)
{
	//
	// Reset the interrupt from the Widget board
	//

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[SIO_WIDGET_HANDLER_NUM])
	{
		//
		// YES - Call it - Did it return non-null ?
		if(user_handler[SIO_WIDGET_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Do exception handling
			//
			unhandled_exception("Interrupt - SIO Widget Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	unhandled_exception("Interrupt - SIO Widget Int.");
}


//
// Ethernet Rx Interrupt Handler
//
static void ethernet_rx_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[SIO_ETHERNET_RX_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SIO_ETHERNET_RX_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet Rx Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet Rx Int.");
}


//
// Ethernet Tx interrupt handler
//
static void ethernet_tx_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[SIO_ETHERNET_TX_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SIO_ETHERNET_TX_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet Tx Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet Tx Int.");
}


//
// Ethernet Tx Empty interrupt handler
//
static void ethernet_tx_empty_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[SIO_ETHERNET_TX_EMPTY_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SIO_ETHERNET_TX_EMPTY_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet Tx Empty Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet Tx Empty Int.");
}


//
// Ethernet buffer allocation interrupt handler
//
static void ethernet_alloc_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[SIO_ETHERNET_ALLOC_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SIO_ETHERNET_ALLOC_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet Alloc Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet Alloc Int.");
}


//
// Ethernet Rx overrun interrupt handler
//
static void ethernet_rx_overrun_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[SIO_ETHERNET_RX_OVERRUN_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SIO_ETHERNET_RX_OVERRUN_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet Rx Overrun Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet Rx Overrun Int.");
}


//
// Ethernet Link OK interrupt handler
//
static void ethernet_eph_link_ok_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_LINK_OK_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_LINK_OK_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH Link Ok Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH Link Ok Int.");
}


//
// Ethernet counter rollover interrupt handler
//
static void ethernet_eph_counter_rollover_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_COUNTER_ROLLOVER_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_COUNTER_ROLLOVER_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH Counter Rollover Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH Counter Rollover Int.");
}


//
// Ethernet Tx underrun interrupt handler
//
static void ethernet_eph_txena_transmit_underrun_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_TXENA_TXUNRN_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_TXENA_TXUNRN_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH TXENA Tx Underrun Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH TXENA Tx Underrun Int.");
}


//
// Ethernet SQE Error interrupt handler
//
static void ethernet_eph_txena_sqet_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_TXENA_SQET_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_TXENA_SQET_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH TXENA SQE Error Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH TXENA SQE Error Int.");
}


//
// Ethernet Lost carrier interrupt handler
//
static void ethernet_eph_txena_lost_carrier_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_TXENA_LOST_CARR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_TXENA_LOST_CARR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH TXENA Lost Carrier Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH TXENA Lost Carrier Int.");
}


//
// Ethernet Late collision interrupt handler
//
static void ethernet_eph_txena_late_collision_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_TXENA_LATCOL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_TXENA_LATCOL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH TXENA Late Collision Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH TXENA Late Collision Int.");
}


//
// Ethernet 16 collisions interrupt handler
//
static void ethernet_eph_txena_16collision_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[ETHERNET_EPH_TXENA_16COL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[ETHERNET_EPH_TXENA_16COL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet EPH TXENA 16 Collisions Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet EPH TXENA 16 Collisions Int.");
}


//
// Ethernet Tx error interrupt handler
//
static void ethernet_eph_txena_handler(int cause, unsigned int *r_save)
{
	int	i;
	int	ether_eph_txena_cause = 0;

	//
	// Is this interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & TXUNRN_BIT)
	{
		//
		// YES - Set pending bit
		//
		ether_eph_txena_cause |= 1;
	}

	//
	// Is this interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & SQET_BIT)
	{
		//
		// YES - Set pending bit
		//
		ether_eph_txena_cause |= 2;
	}

	//
	// Is this interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & LOST_CAR_BIT)
	{
		//
		// YES - Set pending bit
		//
		ether_eph_txena_cause |= 4;
	}

	//
	// Is this interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & LATCOL_BIT)
	{
		//
		// YES - Set pending bit
		//
		ether_eph_txena_cause |= 8;
	}

	//
	// Is this interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & SIXTEENCOL_BIT)
	{
		//
		// YES - Set pending bit
		//
		ether_eph_txena_cause |= 16;
	}

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (ETHERNET_EPH_TXENA_16COL_HANDLER_NUM - ETHERNET_EPH_TXENA_TXUNRN_HANDLER_NUM); i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(ether_eph_txena_cause & (1 << i))
		{
			//
			// YES - Call it's service function
			//
			ether_eph_txena_func[i](cause, r_save);
		}
	}
}


//
// Ethernet EPH interrupt handler
//
static void ethernet_eph_int_handler(int cause, unsigned int *r_save)
{
	int	i;
	int	ether_eph_cause = 0;

	//
	// Link OK interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & LINK_OK_BIT)
	{
		//
		// YES - Set cause bit
		//
		ether_eph_cause |= 1;
	}

	//
	// Counter rollover interrupt pending ?
	//
	if(SMC_EPH_STATUS_REG & CTR_ROL_BIT)
	{
		//
		// YES - Set cause bit
		//
		ether_eph_cause |= 2;
	}

	//
	// Fatal Tx error interrupt pending ?
	//
	if((SMC_TRANSMIT_CONTROL_REG & TXENA_BIT))
	{
		//
		// YES - Set cause bit
		//
		ether_eph_cause |= 4;
	}

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (ETHERNET_EPH_TXENA_HANDLER_NUM - ETHERNET_EPH_LINK_OK_HANDLER_NUM); i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(ether_eph_cause & (1 << i))
		{
			//
			// YES - Call service function
			//
			ether_eph_func[i](cause, r_save);
		}
	}
}


//
// Ethernet Early Rx interrupt handler
//
static void ethernet_early_rx_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[SIO_ETHERNET_EARLY_RX_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[SIO_ETHERNET_EARLY_RX_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("SIO Ethernet Early Rx Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("SIO Ethernet Early Rx Int.");
}


//
// Ethernet chip interrupts from SIO board
//
static void sio_ethernet_handler(int cause, unsigned int *r_save)
{
	int	i;
	int	ether_cause = 0;

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (SIO_ETHERNET_EARLY_RX_INT_HANDLER_NUM - SIO_ETHERNET_RX_INT_HANDLER_NUM); i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(ether_cause & (1 << i))
		{
			//
			// YES - Call service function
			//
			ether_func[i](cause, r_save);
		}
	}
}


__asm__("
	.set	noreorder
	.globl	enable_ints
enable_ints:
	mfc0	$8,$12
	ori	$8,1
	li		$9,6
	not	$9
	and	$8,$8,$9
	jr		$31
	mtc0	$8,$12
	.set	reorder
");

void enable_ints(void);

//
// SIO Vertical Sync Interrupt handler
//
static void sio_vsync_handler(int cause, unsigned int *r_save)
{
	static unsigned int	vsync_int_count = 0;

	//
	// Clear the interrupt
	//
	*((volatile char *)RESET_REG_ADDR) &= ~0x8;

	//
	// Allow it to occur again
	//
	*((volatile char *)RESET_REG_ADDR) |= 0x8;

	//
	// Is there a user installed interrupt handler for this interrupt ?
	//
	if(user_handler[SIO_VSYNC_HANDLER_NUM])
	{
		//
		// Call users handler (process dispatcher)
		//
		user_handler[SIO_VSYNC_HANDLER_NUM](cause, r_save);
	}

	//
	// Have 8 ticks gone by ?
	//
	if(vsync_int_count == 8)
	{
		//
		// YES - Toggle the Decimal point LED
		//
		*((volatile char *)LED_ADDR) ^= 0x01;

		//
		// Reset vsync interrupt count
		//
		vsync_int_count = 0;
	}
	else
	{
		vsync_int_count++;
	}

	//
	// Increment the vsync count
	//
	vsync_count++;
}


//
// Service function for SIO board interrupts
//
static void sio_handler(int cause, unsigned int *r_save)
{
	int	i;
	int	sio_cause;

	//
	// Get the cause of the interrupt from the SIO board
	//
	sio_cause = *((volatile unsigned char *)INT_CAUSE_REG_ADDR);

	//
	// Mask out any interrupts that are NOT enabled
	//
	sio_cause &= *((volatile unsigned char *)INT_ENBL_REG_ADDR);

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (SIO_VSYNC_HANDLER_NUM - SIO_WDOG_TIMER_HANDLER_NUM); i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(sio_cause & (1 << i))
		{
			//
			// YES - service it
			//
			sio_func[i](cause, r_save);
		}
	}
}
		

//
// CPU Int 1 (IP3) Handler - SIO board interrupts
//
static void cpu_ip3_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed interrupt handler
	//
	if(user_handler[CPU_IP3_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_IP3_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call the system handler
			//
			sio_handler(cause, r_save);
		}

		//
		// Done
		//
		return;
	}

	//
	// No user installed handler - call system handler
	//
	sio_handler(cause, r_save);
}


//
// NILE IV CPU Parity Error Interrupt handler
//
static void nile4_cpu_parity_error_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_CPU_PARITY_ERROR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_CPU_PARITY_ERROR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV CPU Parity Error Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV CPU Parity Error Int.");
}


//
// NILE IV No Target Detect Interrupt handler
//
static void nile4_cpu_no_target_decode_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_CPU_NO_TARGET_DECODE_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_CPU_NO_TARGET_DECODE_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV CPU No Target Detected Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV CPU No Target Detected Int.");
}


//
// NILE IV Memory Check Error Interrupt handler
//
static void nile4_memory_check_error_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_MEMORY_CHECK_ERROR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_MEMORY_CHECK_ERROR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV Memory Check Error Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV Memory Check Error Int.");
}


//
// NILE IV DMA Controller Interrupt handler
//
static void nile4_dma_controller_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_DMA_CONTROLLER_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_DMA_CONTROLLER_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV DMA Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV DMA Int.");
}


//
// NILE IV UART Interrupt handler
//
static void nile4_uart_handler(int cause, unsigned int *r_save)
{
#ifndef TTY_INTERRUPTS
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_UART_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_UART_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV UART Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV UART Int.");
#else
	ttyintr();
#endif
}


//
// NILE IV Watchdog Timer Interrupt handler
//
static void nile4_watchdog_handler(int cause, unsigned int *r_save)
{
	//
	// Increment the watchdog counter
	//
	++watchdog_count;

#ifndef TEST
	//
	// Check for process loop stall if debugging capability is not installed
	//
	if(!debug_capable)
	{
		if(vsync_timestamp)
		{
			if(watchdog_count > vsync_timestamp)
			{
				exception_level = -1;
				unhandled_exception("Process system stall");
			}
		}
	}
#endif

	//
	// Has 50ms elapsed ?
	//
	if(!(watchdog_count % 50))
	{
		//
		// YES - Strobe the coin counters
		//
		coinintr();
	}

	//
	// Has 250 ms elapsed ?
	//
	if(!(watchdog_count % 250))
	{
		//
		// YES - Toggle the Decimal point LED
		//
		*((volatile char *)LED_ADDR) ^= 0x80;

		//
		// Strobe the hardware watchdog
		//
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		*((volatile char *)RTC_WATCHDOG_REG) = 0x96;
	}

	//
	// Update the CMOS Ram if needed
	//
	update_cmos();

	//
	// 1 second elapsed ?
	//
	if(!(watchdog_count % 1000))
	{
		//
		// Increment the elapsed time
		//
		elapsed_time++;
	}

#ifndef TEST
	//
	// Scan the inputs
	//
	scan_inputs();
#endif

	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_WATCHDOG_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		user_handler[NILE4_WATCHDOG_HANDLER_NUM](cause, r_save);
	}
}


//
// NILE IV General Purpose Timer Interrupt handler
//
static void nile4_gpt_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_GPT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_GPT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV General Purpose Timer Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV General Purpose Timer Int.");
}


//
// NILE IV Local Bus Timeout Interrupt handler
//
static void nile4_lbt_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_LBT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_LBT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV Local Bus Timeout Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV Local Bus Timeout Int.");
}


//
// NILE IV PCI Int A Interrupt handler
//
static void nile4_pci_inta_handler(int cause, unsigned int *r_save)
{
	//
	// Clear the INTA status from the NILE IV too
	//
	*((volatile int *)NILE4_INT_CLR_ADDR) |= (1 << 8);

	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_INTA_HANDLER_NUM])
	{
		//
		// YES - Call it
		//
		user_handler[NILE4_PCI_INTA_HANDLER_NUM](cause, r_save);
	}
}


//
// NILE IV PCI Int B Interrupt handler
//
static void nile4_pci_intb_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_INTB_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_PCI_INTB_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV PCI INT B Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV PCI INT B Int.");
}


//
// NILE IV PCI Int C Interrupt handler
//
static void nile4_pci_intc_handler(int cause, unsigned int *r_save)
{
	//
	// Clear the INTC status from the NILE IV too
	//
	*((volatile int *)NILE4_INT_CLR_ADDR) |= (1 << 10);

	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_INTC_HANDLER_NUM])
	{
		//
		// YES - Call it
		//
		user_handler[NILE4_PCI_INTC_HANDLER_NUM](cause, r_save);
	}
}


//
// NILE IV PCI Int D Interrupt handler
//
static void nile4_pci_intd_handler(int cause, unsigned int *r_save)
{
	//
	// Clear the INTD status from the NILE IV too
	//
	*((volatile int *)NILE4_INT_CLR_ADDR) |= (1 << 11);

	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_INTD_HANDLER_NUM])
	{
		//
		// YES - Call it
		//
		user_handler[NILE4_PCI_INTD_HANDLER_NUM](cause, r_save);
	}
}


//
// NILE IV PCI Int E Interrupt handler
//
static void nile4_pci_inte_handler(int cause, unsigned int *r_save)
{
	//
	// Clear the INTE status from the NILE IV
	//
	*((volatile int *)NILE4_INT_CLR_ADDR) |= (1 << 12);

	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_INTE_HANDLER_NUM])
	{
		//
		// YES - Call it
		//
		user_handler[NILE4_PCI_INTE_HANDLER_NUM](cause, r_save);
	}
}


//
// NILE IV PCI SERR Interrupt handler
//
static void nile4_pci_serr_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_SERR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_PCI_SERR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV PCI SERR Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV PCI SERR Int.");
}


//
// NILE IV PCI Internal Error Interrupt handler
//
static void nile4_pci_internal_error_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user handler ?
	//
	if(user_handler[NILE4_PCI_INTERNAL_ERROR_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[NILE4_PCI_INTERNAL_ERROR_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Nile IV PCI Internal Error Int.");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Nile IV PCI Internal Error Int.");
}


//
// NILE IV Interrupt Handler
//
static void nile4_handler(int cause, unsigned int *r_save)
{
	register int	i;
	register int	nile4_cause;
	register int	nile4_enables = 0;
	register int	tmp;

	//
	// Get the cause of the interrupt
	//
	tmp = *((volatile unsigned int *)NILE4_INT_STAT0_LO_ADDR);
	nile4_cause = tmp & 0xffff;
	nile4_cause |= ((tmp >> 16) & 0xffff);
	tmp = *((volatile unsigned int *)NILE4_INT_STAT0_HI_ADDR);
	nile4_cause |= (tmp & 0xffff);
	nile4_cause |= ((tmp >> 16) & 0xffff);
	tmp = *((volatile unsigned int *)NILE4_INT_STAT1_LO_ADDR);
	nile4_cause |= (tmp & 0xffff);
	nile4_cause |= ((tmp >> 16) & 0xffff);

	//
	// Figure out which interrupts are actually enabled
	//
	tmp = *((volatile unsigned int *)NILE4_INT_CTRL_LO_ADDR);
	for(i = 0; i < 8; i++)
	{
		if(tmp & (8 << (i*4)))
		{
			nile4_enables |= (1<<i);
		}
	}

	//
	// Only deal with interrupts that are actually enabled
	//
	nile4_cause &= nile4_enables;

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (NILE4_PCI_INTERNAL_ERROR_HANDLER_NUM - NILE4_CPU_PARITY_ERROR_HANDLER_NUM); i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(nile4_cause & (1 << i))
		{
			//
			// YES - Service it
			//
			nile4_func[i](cause, r_save);

			//
			// Reset the cause
			//
			*((volatile int *)NILE4_INT_CLR_ADDR) |= (1 << i);
		}
	}
}


//
// CPU Int 2 (IP4) Handler - NILE IV Interrupt
//
static void cpu_ip4_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed interrupt handler ?
	//
	if(user_handler[CPU_IP4_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_IP4_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			nile4_handler(cause, r_save);
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	nile4_handler(cause, r_save);
}


//
// CPU Int 3 (IP5) Handler - un-assigned
//
static void cpu_ip5_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_IP5_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_IP5_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Interrupt IP5 (int 3)");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Interrupt IP5 (int 3)");
}


#ifndef TEST
//
// Function used to start PSYQ debug service
//
static void start_debug(unsigned int *regs, int status)
{
	// Disable the vertical sync interrupt
	*((volatile char *)RESET_REG_ADDR) &= ~0x8;

	//
	// Turn off wait vsync timeout
	//
	vsync_timestamp = 0;

	//
	// Set active flag
	//
	active = 1;

	//
	// Set SCSI status
	//
	istat = status;

	//
	// Copy regs to save area
	//
	copy_regs((unsigned char *)exregs, (unsigned char *)regs, sizeof(exc_stack_regs_t));

	//
	// Clear run flag
	//
	reg_runflag = 0;

	//
	// Set the exception type for the debugger
	//
	reg_extype = regs[CP0_CAUSE];
	reg_extype >>= 2;
	reg_extype &= 0x1f;

	//
	// Is this a trap ?
	//
	if(reg_extype == 13)
	{
		//
		// YES - fool debugger into thinking it's a breakpoint
		//
		reg_extype = 9;
	}

	//
	// Allow other interrupts to occur
	//
	setup_debug_service();

	//
	// Call process scsi
	//
	process_scsi(status);

	//
	// Reset the active flag
	//
	active = 0;

	//
	// Set the exception type
	//
	reg_extype = 1;

	//
	// Flush the caches in case new data and/or code was loaded
	//
	flush_cache();

	//
	// Copy back the registers
	//
	copy_regs((unsigned char *)regs, (unsigned char *)exregs, sizeof(exc_stack_regs_t));

	//
	// Re-enable the debugging interrupt
	//
	if(!status)
	{
		enable_ip(SCSI_INT);
	}

	// Enable the vertical sync interrupt
	*((volatile char *)RESET_REG_ADDR) |= 0x8;
}


//
// Function called by IP6 handler
//
static void debug_handler(int cause, unsigned int *r_save)
{
	int	status;

	//
	// Is the PSYQ debug card installed ?
	//
	if(!debug_capable)
	{
		//
		// NO - call system handler
		//
		unhandled_exception("Interrupt IP6 (int 4)");

		//
		// Done
		//
		return;
	}

	//
	// Are we already in debug service ?
	//
	if(!active)
	{
		//
		// NO - get status from PSYQ debug card
		//
		status = *((volatile int *)0xa9000000);

		//
		// Any SCSI interrupts pending ?
		//
		if(status & 7)
		{
			//
			// YES - Start debug
			//
			start_debug(r_save, (status & 7));
		}
		else
		{
			//
			// NO - Start debug
			//
			start_debug(r_save, 0);
		}
	}
}
#endif

//
// CPU Int 4 (IP6) Handler - PSYQ Debugging interrupt
//
static void cpu_ip6_handler(int cause, unsigned int *r_save)
{
	//
	// Clear the INTA status from the NILE IV too
	//
	*((volatile int *)NILE4_INT_CLR_ADDR) |= (1 << 8);

	//
	// Is a user handler installed ?
	//
	if(user_handler[CPU_IP6_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_IP6_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Start PSYQ debugging service
			//
#ifndef TEST
			debug_handler(cause, r_save);
#else
			unhandled_exception("Interrupt IP6 (int 4)");
#endif
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - Start PSYQ debug service
	//
#ifndef TEST
	debug_handler(cause, r_save);
#else
	unhandled_exception("Interrupt IP6 (int 4)");
#endif
}


//
// CPU Int 5 (IP7) handler
//
static void cpu_ip7_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_IP7_HANDLER_NUM])
	{
		//
		// YES - call it - did it return non-null ?
		//
		if(user_handler[CPU_IP7_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - call system handler
			//
			unhandled_exception("Interrupt IP7 (int 5)");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Interrupt IP7 (int 5)");
}


//
// CPU IP0 - IP7 Handler
//
static void cpu_ip_handler(int cause, unsigned int *r_save)
{
	register int	i;

	//
	// Only deal with interrupts that are both pending AND enabled
	//
	cause &= r_save[CP0_STATUS];

	//
	// Loop through and deal with the highest priority pending interrupt
	//
	for(i = 0; i < 8; i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(cause & int_priority_info[i].int_bit)
		{
			//
			// YES - Disable this and all lower priority interrupts.
			// Enable all higher priority interrupts and allow interrupt
			// processing.
			//
#ifdef NESTED_INTERRUPTS
			r_save[CP0_STATUS] &= ~0xff00;
			r_save[CP0_STATUS] |= splx(int_priority_info[i].higher_pri);
#endif

			//
			// Call the service function for this interrupt
			//
			int_priority_info[i].func(cause, r_save);

			//
			// Done
			//
			return;
		}
	}
}

#ifdef NESTED_INTERRUPTS
__asm__("
	.set		noreorder
	.globl	splx
splx:
	mfc0		$2,$12			# Get the status register
	move		$8,$2				# Save it
	andi		$2,0xff00		# Mask off all but interrupt enable bits
	li			$9,0xff01		# Turn off interrupt enable bits and IE
	not		$9
	and		$8,$8,$9
	mtc0		$8,$12			# Write back status register
	or			$8,$8,$4			# Or in new interrupt enable bits
	li			$9,6				# Turn off ERL and EXL bits
	not		$9
	and		$8,$8,$9
	ori		$8,1				# Turn on IE bit
	jr			$31				# Return to caller
	mtc0		$8,$12			# Write back status register (BDSLOT)
	.set		reorder
");
#endif


//
// CPU Interrupt Handler
//
static void cpu_int_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_INT_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_INT_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			cpu_ip_handler(cause, r_save);
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	cpu_ip_handler(cause, r_save);
}


//
// If this exception occurs it means that the virtual page number exists in
// the TLB, the page frame number exists in the TLB, AND that the page being
// written is write protected.
//
static void cpu_tlbm_handler(int cause, unsigned int *r_save)
{
#ifndef VMM
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_TLBM_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_TLBM_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("TLB Modification");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("TLB Modification");
#else
	vmm_tlbm_handler(cause, r_save);
#endif
}

//
// If this exception occurs, it means that the virtual page number exists in
// the TLB but the page frame is NOT valid.
//
static void cpu_tlbl_handler(int cause, unsigned int *r_save)
{
#ifndef VMM
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_TLBL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_TLBL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("TLB Load");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("TLB Load");
#else
	vmm_tlbl_handler(cause, r_save);
#endif
}


//
// If this exception occurs, it means that the virtual page number exists in
// the TLB but the page frame is NOT valid.
//
static void cpu_tlbs_handler(int cause, unsigned int *r_save)
{
#ifndef VMM
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_TLBS_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_TLBS_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("TLB Store");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("TLB Store");
#else
	vmm_tlbs_handler(cause, r_save);
#endif
}


//
// Address Error (load) handler
//
static void cpu_adel_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_ADEL_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_ADEL_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Address Error (Load)");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Address Error (Load)");
}


//
// Address Error (store) handler)
//
static void cpu_ades_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_ADES_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_ADES_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Address Error (Store)");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Address Error (Store)");
}


//
// Bus Error (instruction fetch) handler
//
static void cpu_ibe_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_IBE_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_IBE_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Bus Error (Instruction Fetch)");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Bus Error (Instruction Fetch)");
}


//
// Bus Error (data reference) handler
//
static void cpu_dbe_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_DBE_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_DBE_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Bus Error (Data Reference)");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Bus Error (Data Reference)");
}


//
// System call handler
//
static void cpu_syscall_handler(int cause, unsigned int *r_save)
{
	unhandled_exception("cpu_syscall_handler() - SYSCALL not recognized\n");
}


// Cold restart of the system
static void cold_start(void)
{
	void	(*cold_entry)(void) = (void (*)(void))0xbfc00000;

	// Set all of the reset control bits

	// Wait a little bit

	// Jump to the start of the ROM
	cold_entry();
}


//
// Breakpoint handler
//
static void cpu_breakpoint_handler(int cause, unsigned int *r_save)
{
#ifndef TEST
	int	i;
	int	code;
	int	status;

	// Get the instruction
	code = *((volatile int *)r_save[PC]);
	code >>= 6;

	// Mask off the unused bits
	code &= 0xfffff;

	// Is code in the range used by debugger service stuff ?
	if(code >= 0x400 && code <= 0x409 && debug_capable)
	{
		// YES - Deal with the specific code
		switch(code)
		{
			case 0x400:		// Poll the PC
			{
				// Are we in debug service ?
				if(active)
				{
					// YES - just return there's nothing to do
					return;
				}

				// Get the status from the SCSI
				status = *((volatile int *)0xa9000000);

				// Are any of it's interrupts pending ?
				if(!(status & 7))
				{
					// NOPE - just return - there is nothing to do
					return;
				}

				// Start up debugging service
				start_debug(r_save, 0);

				break;
			}
			case 0x401:		// Cold Start
			{
				cold_start();
				break;
			}
			case 0x402:		// Warm Start
			{
				// Store the exception type
				reg_extype = -1;

				// Start debugging service
				start_debug(r_save, 0);

				// Done
				break;
			}
			case 0x403:		// Set interrupt status
			{
				// This is no longer used - just return
				break;
			}
			case 0x404:		// Set cache
			{
				// This is not needed on Phoenix systems - return
				break;
			}
			case 0x405:		// Unhook vectors from downloader stub
			{
				clear_user_handlers();
				break;
			}
			case 0x406:		// Set interrupt vectors to be hooked by debugger
			{
				// This is not used - return
				break;
			}
			case 0x407:		// Pause
			{
				// Set the exception type
				reg_extype = 9;

				// Start debugging service
				start_debug(r_save, 0);

				// Done
				break;
			}
			case 0x408:		// Get reinstall
			{
				// Not used - return
				break;
			}
			case 0x409:		// Hook vector
			{
				// Not used - return
				break;
			}
			default:			// Unrecognized code
			{
				break;
			}
		}
		return;
	}
	else if(code == 0x1c00)
	{
		// Integer division by 0
		unhandled_exception("Integer Divide by 0");
	}
	else if(code == 0x1800)
	{
		// Integer overflow
		unhandled_exception("Integer Overflow");
	}
	else if(debug_capable)
	{
		// Set the exception type
		reg_extype = 9;

		// Start debugger service
		start_debug(r_save, 0);

		// Done
		return;
	}
#endif
	// If we get here it's because we shouldn't be here
	unhandled_exception("Breakpoint");
}


//
// Reserved instruction handler
//
static void cpu_reserved_instruction_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_RESERVED_INSTRUCTION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_RESERVED_INSTRUCTION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Reserved Instruction");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Reserved Instruction");
}


//
// Co-processor unusable handler
//
static void cpu_cpu_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_CPU_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_CPU_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Coprocessor Unusable");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Coprocessor Unusable");
}


//
// Arithmetic overflow handler
//
static void cpu_overflow_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_OVERFLOW_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_OVERFLOW_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Arithmetic Overflow");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Arithmetic Overflow");
}


//
// Trap handler
//
static void cpu_trap_handler(int cause, unsigned int *r_save)
{
	unsigned int	pc;

#ifndef TEST
	//
	// Is system debug capable ?
	//
	if(debug_capable)
	{
		//
		// YES - Save the pc
		//
		pc = r_save[PC];

		//
		// Fool the debugger into thinking this is a breakpoint
		//
		reg_extype = 9;
	}
#endif

	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_TRAP_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_TRAP_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Trap");
		}

#ifndef TEST
		if(debug_capable && pc == r_save[PC])
		{
			r_save[PC] += 4;
		}
#endif

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Trap");

	//
	// If debug capable and pc is same as when we entered adjust PC
	// NOTE - this should always be the case unless new code was downloaded
	// while servicing the trap
	//
#ifndef TEST
	if(debug_capable && pc == r_save[PC])
	{
		r_save[PC] += 4;
	}
#endif
}


//
// Reserved exception handler
//
static void no_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_RESERVED_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_RESERVED_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Reserved Exception");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Reserved Exception");
}


//
// FPU Inexact result handler
//
static void fpu_inexact_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[COP1_INEXACT_OPERATION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[COP1_INEXACT_OPERATION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("FPE Inexact Operation");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("FPE Inexact Operation");
}


//
// FPU Underflow handler
//
static void fpu_underflow_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[COP1_UNDERFLOW_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[COP1_UNDERFLOW_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("FPE Underflow");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("FPE Underflow");
}


//
// FPU Divide by 0 handler
//
static void fpu_overflow_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[COP1_OVERFLOW_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[COP1_OVERFLOW_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("FPE Overflow");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("FPE Overflow");
}


//
// System FPU divide by 0 handler
//
static void div0_handler(int *r_save)
{
	int	instruction;
	int	ft_reg;
	int	reg_val;
	int	fmt;

	// At this point what should happen is we should decode the divide
	// instruction that caused the exception and replace the 0 value in
	// the ft register with the smallest non-zero value that can be used
	// making sure to perserve the sign of the 0 value on the new value.
	// Then we reset the EPC to the instruction that caused the exception
	// and let 'er rip.  This should be a graceful recovery from the nasty
	// divide by zero situation that can (and does) happen in 3D projections


	// Get the correct instruction if in delayed branch     
	if(r_save[CP0_CAUSE] & 0x80000000)
	{
		instruction = *((unsigned *)(r_save[PC] + 4));
	}
	else
	{
		instruction = *((unsigned *)r_save[PC]);
	}

	if((instruction >> 26) != 0x11)
	{
		unhandled_exception("FPE DIV 0 - Not COP1");
		return;
	}
	if((instruction & 0x3f) == 3)		// div.fmt
	{
		fmt = instruction >> 21;
		fmt &= 0x1f;

		ft_reg = instruction >> 16;
		ft_reg &= 0x1f;
		ft_reg <<= 1;
		ft_reg += FGR0;
		reg_val = r_save[ft_reg];

		if(fmt == SINGLE_PREC_FLOAT)
		{
			r_save[ft_reg] = 0x3727c5ac;	// 0.000001
			if(reg_val < 0)
			{
				r_save[ft_reg] |= 0x80000000;	// Negative
			}
		}
		else if(fmt == DOUBLE_PREC_FLOAT)
		{
			reg_val = r_save[ft_reg+1];
			r_save[ft_reg] = 0xa0b5ed8d;
			r_save[ft_reg+1] = 0x3eb0c6f7;
			if(reg_val < 0)
			{
				r_save[ft_reg+1] |= 0x80000000;
			}
		}
		else
		{
			unhandled_exception("FPE DIV 0 - Unrecognized format for div");
			return;
		}
	}
	else if((instruction & 0x3f) == 0x15)	// recip.fmt
	{
		fmt = instruction >> 21;
		fmt &= 0x1f;

		ft_reg = instruction >> 11;
		ft_reg &= 0x1f;
		ft_reg <<= 1;
		ft_reg += FGR0;
		reg_val = r_save[ft_reg];

		if(fmt == SINGLE_PREC_FLOAT)
		{
			r_save[ft_reg] = 0x3727c5ac;	// 0.000001
			if(reg_val < 0)
			{
				r_save[ft_reg] |= 0x80000000;	// Negative
			}
		}
		else if(fmt == DOUBLE_PREC_FLOAT)
		{
			reg_val = r_save[ft_reg+1];
			r_save[ft_reg] = 0xa0b5ed8d;
			r_save[ft_reg+1] = 0x3eb0c6f7;
			if(reg_val < 0)
			{
				r_save[ft_reg+1] |= 0x80000000;
			}
		}
		else
		{
			unhandled_exception("FPE DIV 0 - Unrecognized format for recip");
			return;
		}
	}
	else if((instruction & 0x3f) == 0x16)	// rsqrt.fmt
	{
		fmt = instruction >> 21;
		fmt &= 0x1f;

		ft_reg = instruction >> 11;
		ft_reg &= 0x1f;
		ft_reg <<= 1;
		ft_reg += FGR0;
		reg_val = r_save[ft_reg];

		if(fmt == SINGLE_PREC_FLOAT)
		{
			r_save[ft_reg] = 0x3727c5ac;	// 0.000001
			if(reg_val < 0)
			{
				r_save[ft_reg] |= 0x80000000;	// Negative
			}
		}
		else if(fmt == DOUBLE_PREC_FLOAT)
		{
			reg_val = r_save[ft_reg+1];
			r_save[ft_reg] = 0xa0b5ed8d;
			r_save[ft_reg+1] = 0x3eb0c6f7;
			if(reg_val < 0)
			{
				r_save[ft_reg+1] |= 0x80000000;
			}
		}
		else
		{
			unhandled_exception("FPE DIV 0 - Unrecognized format for rsqrt");
			return;
		}
	}
	else
	{
		unhandled_exception("FPE DIV 0 - Unrecognized instruction");
	}
}

//
// FPU Divide by 0 handler
//
static void fpu_div0_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			div0_handler(r_save);
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	div0_handler(r_save);
}


//
// FPU Invalid operation handler
//
static void fpu_invalid_operation_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[COP1_INVALID_OPERATION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[COP1_INVALID_OPERATION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("FPE Invalid Operation");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("FPE Invalid Operation");
}


static void unimplemented_handler(int *r_save)
{
	unsigned	instruction;
	int		reg;
	int		fr_val;
	int		ft_val;
	int		fs_val;
	int		fd_val;
	int		in_bd_slot;
	char *	psz = (char *)0;

	// At this point we determine whether or not the instruction is a
	// COP1 instruction and if so we look specifically for a madd or
	// msub instruction.  If either of these instructions caused the
	// exception we attempt to determine if the exception was possibly
	// caused by an intermediate de-normalized result.  (E.G.  The result
	// of the multiply portion of the instruction resulted in a denormalized
	// result).  If so we, simply move the fr register to the fd register
	// and set the PC to the next instruction.

	if(r_save[CP0_CAUSE] & 0x80000000)
	{
		instruction = *((unsigned *)(r_save[PC] + 4));
		in_bd_slot = 1;
	}
	else
	{
		instruction = *((unsigned *)r_save[PC]);
		in_bd_slot = 0;
	}

	switch(instruction >> 26)
	{
		case 0x00:									// Special
		{
			if((instruction & 0x3f) != 1)
			{
				psz = "FPE Uniplemented operation - NOT AN FPU INSTRUCTION";
			}
			else
			{
				psz = "FPE Uniplemented operation - Special Instruction";
			}
			break;
		}
		case 0x11:									// COP1
		{
			// Check operand format to determine how to handle this
			if(((instruction >> 21) & 0x1f) == 16)	// .S single precision?
			{
				reg = instruction & 0x3f;

				switch(reg)
				{
					case 0:							// ADD.S
					case 1:							// SUB.S
					case 2:							// MUL.S
					case 3:							// DIV.S
					{
						static char * sz[] = {
							"FPE Uniplemented operation - unable to fix ADD.S qNaN",
							"FPE Uniplemented operation - unable to fix SUB.S qNaN",
							"FPE Uniplemented operation - unable to fix MUL.S qNaN",
							"FPE Uniplemented operation - unable to fix DIV.S qNaN"
						};

						// Get ft and fs indecies, values
						fr_val = ((instruction>>(16-1))&(31<<1))+FGR0;
						fd_val = ((instruction>>(11-1))&(31<<1))+FGR0;
						ft_val = r_save[fr_val];
						fs_val = r_save[fd_val];

						// Continue only if ft and fs are not qNaN
						if(((ft_val >> 23) & 0xff) != 0xff && ((fs_val >> 23) & 0xff) != 0xff)
						{
							if(((ft_val >> 23) & 0xff) == 0)
							{
								// Value in ft is denormalized or zero
								// Make it a same-sign 0
								r_save[fr_val] = ft_val & 0x80000000;
							}
							if(((fs_val >> 23) & 0xff) == 0)
							{
								// Value in fs is denormalized or zero
								// Make it a same-sign 0
								r_save[fd_val] = fs_val & 0x80000000;
							}

							// Go try again
							return;
						}

						// Value in ft and/or fs is quiet NaN
						psz = sz[reg];
						break;
					}
					case 4:							// SQRT.S
					case 5:							// ABS.S
					case 7:							// NEG.S
					case 12:						// ROUND.W.S
					case 13:						// TRUNC.W.S
					case 14:						// CEIL.W.S
					case 15:						// FLOOR.W.S
					case 33:						// CVT.D.S
					case 36:						// CVT.W.S
					{
						fr_val = ((instruction>>(11-1))&(31<<1))+FGR0;
						fs_val = r_save[fr_val];

						if(((fs_val >> 23) & 0xff) == 0)
						{
							// Value in fs is denormalized
							// Make it a same-sign 0 & go try again
							r_save[fr_val] = fs_val & 0x80000000;
							return;
						}

						// Value in fs is quiet NaN
						if(reg ==  4)
						{
							psz = "FPE Uniplemented operation - unable to fix SQRT.S qNaN";
						}
						else if(reg ==  5)
						{
							psz = "FPE Uniplemented operation - unable to fix ABS.S qNaN";
						}
						else if(reg ==  7)
						{
							psz = "FPE Uniplemented operation - unable to fix NEG.S qNaN";
						}
						else if(reg == 12)
						{
							psz = "FPE Uniplemented operation - unable to fix ROUND.W.S qNaN";
						}
						else if(reg == 13)
						{
							psz = "FPE Uniplemented operation - unable to fix TRUNC.W.S qNaN";
						}
						else if(reg == 14)
						{
							psz = "FPE Uniplemented operation - unable to fix CEIL.W.S qNaN";
						}
						else if(reg == 15)
						{
							psz = "FPE Uniplemented operation - unable to fix FLOOR.W.S qNaN";
						}
						else if(reg == 33)
						{
							psz = "FPE Uniplemented operation - unable to fix CVT.D.S qNaN";
						}
						else if(reg == 36)
						{
							psz = "FPE Uniplemented operation - unable to fix CVT.W.S qNaN";
						}

						break;
					}
					case 32:						// CVT.S.S
					{
						if(!in_bd_slot)
						{
							// Not in branch delay slot
							// Copy fs to fd & skip the instruction
							r_save[((instruction>>(6-1))&(31<<1))+FGR0] = r_save[((instruction>>(11-1))&(31<<1))+FGR0];

							r_save[PC] += 4;
							return;
						}

						// In branch delay slot
						psz = "FPE Uniplemented operation - unable to fix CVT.S.S in a BDS";
						break;
					}
					default:
					{
						psz = "FPE Uniplemented operation - reserved value in COP1 .S function field";
						break;
					}
				}
			}
			else if(((instruction >> 21) & 0x1f) == 17)	// .D double precision?
			{
				psz = "FPE Uniplemented operation - unable to fix COP1 .D format";
			}
			else if(((instruction >> 21) & 0x1f) == 20)	// .W fixed-point?
			{
				psz = "FPE Uniplemented operation - unable to fix COP1 .W format";
			}
			else
			{
				psz = "FPE Uniplemented operation - reserved value in COP1 format field";
			}
			break;
		}
		case 0x13:									// COP1X
		{
			reg = instruction & 0x3f;

			if (reg == 0x20 ||						// MADD.S
				reg == 0x28 ||						// MSUB.S
				reg == 0x30 ||						// NMADD.S
				reg == 0x38)						// NMSUB.S
			{
				// Get ft value, exponent
				fr_val = r_save[((instruction>>(16-1))&(31<<1))+FGR0];
				ft_val = ((fr_val >> 23) & 0xff) - 127;

				// Get fs value, exponent
				fd_val = r_save[((instruction>>(11-1))&(31<<1))+FGR0];
				fs_val = ((fd_val >> 23) & 0xff) - 127;

				if(!in_bd_slot)
				{
					// Not in branch delay slot
					// If multiply product would be denormalized, copy to the
					// destination register the value that would have been
					// added/subtracted; otherwise abort with no clue
					ft_val += fs_val;
					if(ft_val <= -127)
					{
						// Get fr value
						fr_val = r_save[((instruction>>(21-1))&(31<<1))+FGR0];

						// Flip the sign if MSUB.S or  NMADD.S
						if (reg == 0x28 || reg == 0x30) fr_val ^= 0x80000000;

						// Set fd value
						r_save[((instruction>>(6-1))&(31<<1))+FGR0] = fr_val;

						r_save[PC] += 4;
						return;
					}

					// No clue
					if(reg == 0x20)
					{
						psz = "FPE Uniplemented operation - unable to fix MADD.S";
					}
					else if(reg == 0x28)
					{
						psz = "FPE Uniplemented operation - unable to fix MSUB.S";
					}
					else if(reg == 0x30)
					{
						psz = "FPE Uniplemented operation - unable to fix NMADD.S";
					}
					else if(reg == 0x38)
					{
						psz = "FPE Uniplemented operation - unable to fix NMSUB.S";
					}
				}
				else
				{
					// In branch delay slot
					// If fs exp <= ft exp & abs(fs) != 0 set fs to same-sign
					// 0 otherwise set ft to same-sign 0
					if(fs_val <= ft_val && (fd_val & 0x7fffffff))
					{
						// Set fs index
						reg = ((instruction>>(11-1))&(31<<1))+FGR0;
						fr_val = r_save[reg] & 0x80000000;
					}
					else
					{
						// Set ft index
						reg = ((instruction>>(16-1))&(31<<1))+FGR0;
						fr_val = r_save[reg] & 0x80000000;
					}
					r_save[reg] = fr_val;

					// NOW when the eret instruction is executed, the branch
					// instruction is returned to and the fp instruction in
					// the bd slot that caused this mess gets re-executed.  If
					// the exception gets thrown again, the other register
					// (s or t) will get zero'd and the whole mess will occur
					// again.  If both regs get set to 0, the intermediate
					// result can NOT cause a denormalized result and therefore
					// we should go on.
					return;
				}
			}
			else if(reg == 0x21)					// MADD.D
			{
				psz = "FPE Uniplemented operation - unable to fix MADD.D";
			}
			else if(reg == 0x29)					// MSUB.D
			{
				psz = "FPE Uniplemented operation - unable to fix MSUB.D";
			}
			else if(reg == 0x31)					// NMADD.D
			{
				psz = "FPE Uniplemented operation - unable to fix NMADD.D";
			}
			else if(reg == 0x39)					// NMSUB.D
			{
				psz = "FPE Uniplemented operation - unable to fix NMSUB.D";
			}
			else if(reg == 0x00)					// LWXC1
			{
				psz = "FPE Uniplemented operation - unable to fix LWXC1";
			}
			else if(reg == 0x01)					// LDXC1
			{
				psz = "FPE Uniplemented operation - unable to fix LDXC1";
			}
			else if(reg == 0x08)					// SWXC1
			{
				psz = "FPE Uniplemented operation - unable to fix SWXC1";
			}
			else if(reg == 0x09)					// SDXC1
			{
				psz = "FPE Uniplemented operation - unable to fix SDXC1";
			}
			else if(reg == 0x0f)					// PREFIX
			{
				psz = "FPE Uniplemented operation - unable to fix PREFIX";
			}
			break;
		}
		case 0x31:									// LWC1
		{
			psz = "FPE Uniplemented operation - LWC1 Instruction";
			break;
		}
		case 0x35:									// LDC1
		{
			psz = "FPE Uniplemented operation - LDC1 Instruction";
			break;
		}
		case 0x39:									// SWC1
		{
			psz = "FPE Uniplemented operation - SWC1 Instruction";
			break;
		}
		case 0x3d:									// SDC1
		{
			psz = "FPE Uniplemented operation - SDC1 Instruction";
			break;
		}
		default:
		{
			psz = "FPE Uniplemented operation - NOT FPU Instruction";
			break;
		}
	}

	if(psz)
	{
		unhandled_exception(psz);
	}
}


//
// FPU Unimplemented operation exception handler
//
static void fpu_unimplemented_operation_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unimplemented_handler(r_save);
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unimplemented_handler(r_save);
}


//
// Floating point exception handler
//
static void cpu_fpe_handler(int cause, unsigned int *r_save)
{
	int	i;
	int	fpu_cause;
	int	fpu_enable;

	//
	// Get the FPE cause and enables
	//
	fpu_enable = fpu_cause = r_save[FCR31];

	//
	// Shift enables to cause positions
	//
	fpu_enable <<= 5;

	//
	// Mask causes that are NOT enabled
	//
	fpu_cause &= (fpu_enable|0x20000);

	//
	// Loop through and service all pending AND enabled interrupts
	//
	for(i = 0; i <= (COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM - COP1_INEXACT_OPERATION_HANDLER_NUM); i++)
	{
		//
		// Is this interrupt pending ?
		//
		if(fpu_cause & (1 << (i + 12)))
		{
			//
			// YES - service it
			//
			fpe_handler[i](cause, r_save);

			//
			// Reset the cause bit in the cause register
			//
			r_save[FCR31] &= ~(1 << (i + 12));
		}
	}
}

//
// Exception handler that gets called whenever a general exception occurs.
//
static void cpu_general_exception_handler(int cause, unsigned int *r_save)
{
	int	code = (cause >> 2) & 0x1f;

	//
	// Call the exception handler
	//
	exc_func[code](cause, r_save);
}

//
// General exception handler called from assembly language
//
__asm__("
	.set	noreorder
	.globl	exl_on
exl_on:
	mfc0		$8,$12
	ori		$8,2
	li			$9,1
	not		$9
	and		$8,$8,$9
	mtc0		$8,$12
	nop
	jr			$31
	nop
	.set		reorder
");

extern unsigned int	exc_stack[];

	
void *general_exception(int cause)
{
	//
	// Copy the registers
	//
	copy_regs((unsigned char *)&exception_regs[exception_level], (unsigned char *)exc_stack, sizeof(exc_stack_regs_t));

	//
	// Increment the exception_level index
	//
	exception_level++;

	//
	// Is a user handler installed ?
	//
	if(user_handler[CPU_GENERAL_EXCEPTION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_GENERAL_EXCEPTION_HANDLER_NUM](cause, (unsigned int *)&exception_regs[exception_level-1]))
		{
			//
			// YES - call system handler
			//
			cpu_general_exception_handler(cause, (unsigned int *)&exception_regs[exception_level-1]);
		}

		//
		// Turn on the EXL bit in the status register to disable interrupts
		//
		exl_on();

		//
		// Decrement the exception_level index
		//
		exception_level--;

		//
		// Done - Tell assembly language where to restore registers from
		//
		return((void *)&exception_regs[exception_level]);
	}

	//
	// No user handler - call system handler
	//
	cpu_general_exception_handler(cause, (unsigned int *)&exception_regs[exception_level-1]);

	//
	// Turn on EXL bit in status register to disable interrupts
	//
	exl_on();

	//
	// Decrement exception_level index
	//
	exception_level--;

	//
	// Done - Tell assembly language where to restore registers from
	//
	return((void *)&exception_regs[exception_level]);
}


static void cpu_tlb_exception_handler(int cause, unsigned int *r_save)
{
#ifndef VMM
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_TLB_EXCEPTION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_TLB_EXCEPTION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("TLB Refill");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("TLB Refill");
#else
	vmm_tlb_exception_handler(cause, r_save);
#endif
}

//
// If this exception occurs, it means that the virtual page number does NOT
// exist in the TLB.  This is the function called from the assembly level.
//
void *tlb_exception(int cause)
{
	//
	// Get where to save the registers to
	//
	unsigned int	*r_save = (unsigned int *)&exception_regs[exception_level];

	//
	// Copy the registers
	//
	copy_regs((unsigned char *)r_save, (unsigned char *)exc_stack, sizeof(exc_stack_regs_t));

	//
	// Increment the exception_level index
	//
	exception_level++;

	//
	// Call the system handler
	//
	cpu_tlb_exception_handler(cause, r_save);

	//
	// Turn on EXL bit in status register to disable interrupts
	//
	exl_on();

	//
	// Decrement exception_level index
	//
	exception_level--;

	//
	// Done - Tell assembly language where to restore registers from
	//
	return(r_save);
}


//
// XTLB Exception handler
//
static void cpu_xtlb_exception_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_XTLB_EXCEPTION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_XTLB_EXCEPTION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("XTLB Refill");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("XTLB Refill");
}


//
// XTLB Refill Exception handler.  This is the code called from the
// assembly level.
//
void *xtlb_exception(int cause)
{
	//
	// Get where to save the registers to
	//
	unsigned int	*r_save = (unsigned int *)&exception_regs[exception_level];

	//
	// Copy the registers
	//
	copy_regs((unsigned char *)r_save, (unsigned char *)exc_stack, sizeof(exc_stack_regs_t));

	//
	// Increment the exception_level index
	//
	exception_level++;

	//
	// Call system XTLB exception handler
	//
	cpu_xtlb_exception_handler(cause, r_save);

	//
	// Turn on EXL bit in status register to disable interrupts
	//
	exl_on();

	//
	// Decrement exception_level index
	//
	exception_level--;

	//
	// Done - Tell assembly language where to restore registers from
	//
	return(r_save);
}


//
// Cache Error handler
//
static void cpu_cache_error_exception_handler(int cause, unsigned int *r_save)
{
	//
	// Is there a user installed handler ?
	//
	if(user_handler[CPU_CACHE_ERROR_EXCEPTION_HANDLER_NUM])
	{
		//
		// YES - Call it - did it return non-null ?
		//
		if(user_handler[CPU_CACHE_ERROR_EXCEPTION_HANDLER_NUM](cause, r_save))
		{
			//
			// YES - Call system handler
			//
			unhandled_exception("Cache Error");
		}

		//
		// Done
		//
		return;
	}

	//
	// No user handler - call system handler
	//
	unhandled_exception("Cache Error");
}


//
// Cache error exception handler.  This is the function called from the
// assembly level.
//
void *cache_error_exception(int cause)
{
	//
	// Get where to save the registers to
	//
	unsigned int	*r_save = (unsigned int *)&exception_regs[exception_level];

	//
	// Copy the registers
	//
	copy_regs((unsigned char *)r_save, (unsigned char *)exc_stack, sizeof(exc_stack_regs_t));

	//
	// Increment the exception_level index
	//
	exception_level++;

	//
	// Call system Cache Error exception handler
	//
	cpu_cache_error_exception_handler(cause, r_save);

	//
	// Turn on EXL bit in status register to disable interrupts
	//
	exl_on();

	//
	// Decrement exception_level index
	//
	exception_level--;

	//
	// Done - Tell assembly language where to restore registers from
	//
	return(r_save);
}


//
// Functions used to enable and disable FPU interrupts
//
__asm__("
	.set		noreorder
	.globl	enable_cop1_int
enable_cop1_int:
	mfc1	$2,$f31
	nop
	nop
	or		$4,$2,$4
	jr		$31
	mtc1	$4,$f31
	.set		reorder

	.set		noreorder
	.globl	disable_cop1_int
disable_cop1_int:
	mfc1	$2,$f31
	nop
	nop
	and	$4,$2,$4
	jr		$31
	mtc1	$4,$f31
	.set	reorder

	.set	noreorder
	.globl	get_int_enables
get_int_enables:
	mfc0	$2,$12		# Get status reg
	jr		$31			# Return
	andi	$2,0xff00	# Return interrupt enables
	.set	reorder

	.set	noreorder
	.globl	ip_on
ip_on:
	mfc0	$8,$12
	li		$9,0xff00
	not	$9
	and	$8,$8,$9
	or		$8,$8,$4
	jr		$31
	mtc0	$8,$12
");

int get_int_enables(void);

//
// Function used to install/de-install handlers
//
int set_handler(int num, int (*func)(int, unsigned int *))
{
	void				*addr;
	int				shift;
	unsigned char	tmp;
	int				int_enables;

	// Get current enabled interrupts
	int_enables = get_int_enables();

	if(num == CPU_GENERAL_EXCEPTION_HANDLER_NUM)
	{
		// Nothing to do
	}
	else if(num == CPU_TLB_EXCEPTION_HANDLER_NUM)
	{
		// Nothing to do
	}
	else if(num == CPU_XTLB_EXCEPTION_HANDLER_NUM)
	{
		// Nothing to do
	}
	else if(num == CPU_CACHE_ERROR_EXCEPTION_HANDLER_NUM)
	{
		// Nothing to do
	}
	else if(num >= CPU_INT_HANDLER_NUM && num <= CPU_FPE_HANDLER_NUM)
	{
#ifndef TEST
		// Is this a trap handler and no debug capability ?
		if(num == CPU_TRAP_HANDLER_NUM && !debug_capable)
		{
			// YES - Don't allow handler to get installed
			ip_on(int_enables);
			return(1);
		}
#endif
		// Else nothing to do
	}
	else if(num >= CPU_IP0_HANDLER_NUM && num <= CPU_IP7_HANDLER_NUM)
	{
		// Enable the interrupt at the CPU if it is NOT already AND func != 0
		// If func == 0 disable the interrupt at the processor if it is NOT
		// IP2, IP3, IP4, or IP6
		if(func)
		{
			int_enables |= (0x100 << (num - CPU_IP0_HANDLER_NUM));
		}
		else if(!func &&
			(num != CPU_IP2_HANDLER_NUM) &&
			(num != CPU_IP3_HANDLER_NUM) &&
			(num != CPU_IP4_HANDLER_NUM) &&
			(num != CPU_IP6_HANDLER_NUM))
		{
			int_enables &= ~(0x100 << (num - CPU_IP0_HANDLER_NUM));
		}
	}
	else if(num >= COP1_INEXACT_OPERATION_HANDLER_NUM && num <= COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM)
	{
		// Enable the FPU interrupt if it is not already AND func != 0
		// If func = 0, disable the interrupt at the FPU
		if(num != COP1_UNIMPLEMENTED_OPERATION_HANDLER_NUM && num != COP1_DIVIDE_BY_ZERO_HANDLER_NUM)
		{
			if(!func)
			{
				disable_cop1_int(~(0x80 << (num - COP1_INEXACT_OPERATION_HANDLER_NUM)));
			}
			else
			{
				enable_cop1_int(0x80 << (num - COP1_INEXACT_OPERATION_HANDLER_NUM));
			}
		}
	}
	else if(num >= SIO_WDOG_TIMER_HANDLER_NUM && num <= SIO_VSYNC_HANDLER_NUM)
	{
		// Enable the SIO interrupt if it is not already AND func != 0
		// If func = 0, disable the interrupt from the SIO board
		if(!func && num != SIO_VSYNC_HANDLER_NUM && num != SIO_A2D_HANDLER_NUM)
		{
			*((volatile char *)INT_ENBL_REG_ADDR) &= ~(1 << (num - SIO_WDOG_TIMER_HANDLER_NUM));
		}
		else if(num != SIO_VSYNC_HANDLER_NUM && num != SIO_A2D_HANDLER_NUM)
		{
			*((volatile char *)INT_ENBL_REG_ADDR) |= (1 << (num - SIO_WDOG_TIMER_HANDLER_NUM));
		}
	}
	else if(num >= IOASIC_FORCE_INT_HANDLER_NUM && num <= IOASIC_UART_TX_EMPTY_HANDLER_NUM)
	{
		// Enable the SIO I/O ASIC interrupt if it is not already AND func != 0
		// If func = 0, disable the interrupt from the I/O ASIC
		if(!func)
		{
			*((volatile short *)IOASIC_CONTROL) &= ~(2 << (num - IOASIC_FORCE_INT_HANDLER_NUM));
		}
		else if(num != IOASIC_FORCE_INT_HANDLER_NUM)
		{
			*((volatile short *)IOASIC_CONTROL) |= (2 << (num - IOASIC_FORCE_INT_HANDLER_NUM));
		}

		// Are any of the I/O ASIC Interrupts enabled ?
		if(*((volatile short *)IOASIC_CONTROL) & 0x3ff7 || (num == IOASIC_FORCE_INT_HANDLER_NUM && func))
		{
			// YES - Make sure the I/O ASIC global interrupt enable is on
			*((volatile short *)IOASIC_CONTROL) |= 1;

			// Enable the I/O ASIC interrupts from the SIO board
			*((volatile char *)INT_ENBL_REG_ADDR) |= SIO_IOASIC_INT_ENABLE;
		}
		else
		{
			// NO - Turn off the I/O ASIC global interrupt enable 
			*((volatile short *)IOASIC_CONTROL) &= ~1;

			// Disable the I/O ASIC interrupts from the SIO board
			*((volatile char *)INT_ENBL_REG_ADDR) &= ~SIO_IOASIC_INT_ENABLE;
		}

		// If ANY of the interrupts from the SIO board are enabled then enable
		// the interrupt at the processor, otherwise disable the interrupt
		if(*((volatile char *)INT_ENBL_REG_ADDR) & 0x3f)
		{
			// Make sure the interrupt is enabled through the NILE IV
			*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= (1 << 11);

			// Enable the interrupt at the processor
			int_enables |= SIO_INT;
		}
		else
		{
			// Disable the interrupt at the processor
			int_enables &= ~SIO_INT;

			// Make sure the interrupt is disabled at the NILE IV
			*((volatile int *)NILE4_INT_CTRL_HI_ADDR) &= ~(1 << 11);
		}
	}
	else if(num >= NILE4_CPU_PARITY_ERROR_HANDLER_NUM && num <= NILE4_PCI_INTERNAL_ERROR_HANDLER_NUM)
	{
#ifdef TTY_INTERRUPTS
		if(num == NILE4_UART_HANDLER_NUM)
		{
			ip_on(int_enables);
			return(1);
		}
#endif
		// Is this one of the PCI interrupts
		if((num >= NILE4_PCI_INTA_HANDLER_NUM && num <= NILE4_PCI_INTE_HANDLER_NUM) || num == NILE4_WATCHDOG_HANDLER_NUM)
		{
			// Are we shutting it off ?
			if(!func)
			{
				// YES - Do NOT allow - PCI interrupts are ALWAYS on
				user_handler[num] = func;
				return(1);
			}
		}

		// Figure out which interrupt control register we need to access
		if(num > NILE4_LBT_HANDLER_NUM)
		{
			addr = (void *)NILE4_INT_CTRL_HI_ADDR;
		}
		else
		{
			addr = (void *)NILE4_INT_CTRL_LO_ADDR;
		}

		// Zero base the handler number
		shift = num - NILE4_CPU_PARITY_ERROR_HANDLER_NUM;

		// Generate the bit number
		shift *= 4;
		shift += 3;
		if(shift > 32)
		{
			shift -= 32;
		}

		// Installing ?
		if(!func)
		{
			// NOPE - Disable interrupt
			*((volatile int *)addr) &= ~(1 << shift);
		}
		else
		{
			// YES - Enable interrupt
			*((volatile int *)addr) |= (1 << shift);
		}
	}
	else if(num >= SIO_ETHERNET_RX_INT_HANDLER_NUM && num <= SIO_ETHERNET_EARLY_RX_INT_HANDLER_NUM)
	{
		// Enable top level SMC Ethernet controller interrupts and associated
		// sio board interrupt
	}
	else if(num >= ETHERNET_EPH_LINK_OK_HANDLER_NUM && num <= ETHERNET_EPH_TXENA_HANDLER_NUM)
	{
		// Enable Mid level SMC Ethernet controller interrupts, associated
		// top level interrupt, and associated sio board interrupt
	}
	else if(num >= ETHERNET_EPH_TXENA_TXUNRN_HANDLER_NUM && num <= ETHERNET_EPH_TXENA_16COL_HANDLER_NUM)
	{
		// Enable low level SMC Ethernet controller txena interrupts,
		// associated mid level interrupt, associated top level interrupt,
		// and associated sio board interrupt.
	}
	else if(num >= SNAPHAT_ALARM_HANDLER_NUM && num <= SNAPHAT_WATCHDOG_HANDLER_NUM)
	{
		// Enable the interrupt in the snaphat device, enable the associated,
		// interrupt in the FPGA if it is not already.  If func pointer is
		// NULL disable the snaphat interrupt and if all snaphat interrupts
		// have been disabled, disable the associated FPGA interrupt.
		if(func)
		{
			if((num - SNAPHAT_ALARM_HANDLER_NUM) == 0)
			{
				// Enable Alarm interrupt
				tmp = *((volatile char *)RTC_INT_ENABLE_REG);
				tmp |= 0x80;
				*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
				*((volatile char *)RTC_INT_ENABLE_REG) = tmp;
			}
			else
			{
				// Enable Watchdog interrupt
				*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
				*((volatile char *)RTC_WATCHDOG_REG) = 0;
				*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
				*((volatile char *)RTC_WATCHDOG_REG) = 0x16;	// 5 seconds
			}
		}
		else
		{
			if((num - SNAPHAT_ALARM_HANDLER_NUM) == 0)
			{
				// Disable Alarm interrupt
				tmp = *((volatile char *)RTC_INT_ENABLE_REG);
				tmp &= ~0x80;
				*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
				*((volatile char *)RTC_INT_ENABLE_REG) = tmp;
			}
			else
			{
				// Disable Watchdog interrupt
				*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
				*((volatile char *)RTC_WATCHDOG_REG) = 0;
			}
		}

		if(*((volatile char *)RTC_INT_ENABLE_REG) & 0x80 || *((volatile char *)RTC_WATCHDOG_REG))
		{
			// Enable the interrupt in the SIO int register
			*((volatile char *)INT_ENBL_REG_ADDR) |= SIO_WATCHDOG_INT_ENABLE;
		}
		else
		{
			// Disable the interrupt in the SIO int register
			*((volatile char *)INT_ENBL_REG_ADDR) &= ~SIO_WATCHDOG_INT_ENABLE;
		}
	}
	else
	{
		// ERROR - bogus handler number
		return(0);
	}
	user_handler[num] = func;
	ip_on(int_enables);
	return(1);
}

//
// Function used to clear out ALL user installed handlers
//
void clear_user_handlers(void)
{
	int	i;

	for(i = 0; i < LAST_HANDLER_NUM; i++)
	{
		set_handler(i, (void *)0);
	}
}


//
// Function used to copy registers to/from save area used by PSYQ and
// GDB debugging stubs.
//
void copy_regs(unsigned char *to, unsigned char *from, int num)
{
	while(num--)
	{
		*to++ = *from++;
	}
}

#ifndef TEST
static void dump_to_cmos(char *str)
{
	cmos_dump_record_t		cd;
	register int				i;
	register int				rec_index;
	register unsigned int	*r_save = (unsigned int *)&exception_regs[exception_level-1];

	//
	// Disable interrupts to make sure watchdog does NOT get fed
	//
	disable_interrupts();

	//
	// Disable the watchdog so it does not catch while we are doing
	// this stuff.
	//
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0;

	//
	// Show info on debugging terminal
	//
	show_exc_info(str);

	//
	// Get the current cmos dump record
	//
	if(get_cmos_dump_record(&cd) < 0)
	{
		// ERROR
		printf("ERROR detected trying to dump exc info to CMOS\n");
	}

	else
	{
		//
		// Get record index
		//
		rec_index = cd.watchdog_count;
		rec_index &= (MAX_EXC_INFO_RECORDS-1);

		if(exception_level >= 0)
		{
			//
			// Fill in the GP and FP registers
			//
			for(i = 0; i < 32; i++)
			{
				cd.exc_info[rec_index].gp_regs[i] = r_save[GP0 + (i<<1)];
			}

			//
			// Get the PC
			//
			cd.exc_info[rec_index].pc = r_save[PC];

			//
			// Get the exception code
			//
			cd.exc_info[rec_index].exc_code = r_save[CP0_CAUSE];
			cd.exc_info[rec_index].exc_code >>= 2;
			cd.exc_info[rec_index].exc_code &= 0x1f;

			//
			// If the PC is valid - grab the instruction
			//
			if(!(r_save[PC] & 3) && r_save[PC] >= 0x80000000 && r_save[PC] < 0x807ffffc)
			{
				if(r_save[CP0_CAUSE] & 0x80000000)
				{
					cd.exc_info[rec_index].inst = *((unsigned int *)(r_save[PC] + 4));
				}
				else
				{
					cd.exc_info[rec_index].inst = *((unsigned int *)r_save[PC]);
				}
			}
			else
			{
				cd.exc_info[rec_index].inst = 0xdeadbeef;
			}
		}
		else
		{
			cd.exc_info[rec_index].exc_code = EXC_TYPE_TIMEOUT;
		}

		//
		// Fill in the string field
		//
		strcpy(cd.exc_info[rec_index].string, str);

		//
		// Increment the watchdog count
		//
		cd.watchdog_count++;
	}

	//
	// Write the cmos dump record
	//
	write_cmos_dump_record(&cd);
}
#endif


//
// unhandled_exception() - This function gets called whenever an error is
// detected in an exception handler or when an unexpected exception occurs.
// The function does one of three things depending on whether or not the
// PSYQ debugging card is installed, the GDB debugger is being used, or
// the application is running in release mode.  If either of the debuggers
// are connected, the debug service stub is entered.  If not, the data is
// logged into the CMOS, an infinite loop is entered, and the watchdog is
// allowed to catch to cause the system to be reset.
//
void unhandled_exception(char *str)
{
#ifdef TEST
	show_exc_info(str);
	while(1) ;
#else
	register unsigned int	*r_save = (unsigned int *)&exception_regs[exception_level-1];

	//
	// Does the PSYQ debugging device exist ?
	//
	if(debug_capable)
	{
		//
		// Show info on debugging terminal
		//
		show_exc_info(str);

		//
		// Enter PSYQ debug service
		//
		start_debug(r_save, 0);
	}

	//
	// Default - Application is running with no debugging
	//
	else
	{
		//
		// Dump the info into CMOS
		//
		dump_to_cmos(str);

		//
		// Enable the watchdog
		//
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		*((volatile char *)RTC_WATCHDOG_REG) = 0x96;

		//
		// Go into infinite loop and allow watchdog to reset us
		//
		while(1) ;
	}
#endif
}

void wait_vsync(int sync_count)
{
	unsigned long	start_time = watchdog_count;

#ifndef TEST
	//
	// Set the timestamp
	//
	vsync_timestamp = start_time + 16000;
#endif

	//
	// Wait for sync_count number of vertical sync interrupts
	//
	while(vsync_count < (int)sync_count)
	{
		//
		// Has too much time passed waiting for the vertical sync interrupts ?
		//
		if((watchdog_count - start_time) > ((sync_count*18)+100))
		{
			//
			// YES - Timeout waiting for vertical sync interrupt
			// Here we should log information to the CMOS and allow system
			// reset to occur.
			//
			exception_level = -1;
			unhandled_exception("Timeout waiting for vertical sync interrupt");
		}
	}

	//
	// Reset the vertical sync interrupt count
	//
	vsync_count = 0L;
}

void disable_pstall_detect(void)
{
}

unsigned long long get_elapsed_time(void)
{
	return(elapsed_time);
}

unsigned long long get_timer_val(void)
{
	unsigned long long	val;

	// Disable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 0;

	// Get current count
	val = (unsigned long long)*((volatile unsigned long *)NILE4_WDOG_COUNT_LO_ADDR);

	// Calculate time ticks
	val = (unsigned long long)(1000000/NANOS_PER_TICK) - val;

	// Convert to nanoseconds
	val *= (unsigned long long)NANOS_PER_TICK;

	// Add in current watchdog value (ns)
	val += ((unsigned long long)watchdog_count * 1000000L);

	// Enable the timer
	*((volatile int *)NILE4_WDOG_CNTL_HI_ADDR) = 1;

	return(val);
}
