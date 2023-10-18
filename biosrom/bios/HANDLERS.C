//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 16 $
//
// $Author: Mlynch $
//
#include	<system.h>

extern int	exregs[];
extern char	reg_runflag;
extern char	active;
extern char	reg_extype;
extern int	do_the_dog;
extern int	not_debug_mode;
extern void	(*user_int0_handler)(void);
extern void	(*user_into_handler)(void);

void ide_intr(void);
void kick_the_dog(void);
void feed_the_dog(void);
void ide_disk_timeout(void);

// Offsets to the various registers int the register save area

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
#define	CPO_INDEX		70
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


// Table of application level installed FPU exception handlers
int	(*fpu_handler[6])(int *) = {
0,0,0,0,0,0
};

// Table of application level installed exception vectors
int	(*exception_vectors[16])(int, int *) = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// Table of application level installed interrupt handler functions
int	(*interrupt_handler[8])(void) = {
0,					// Vertical retrace interrupt handler
0,					// System Controller interrupt handler
0,					// IDE Disk Controller interrupt handler
0,					// I/O Asic Interrupt
0,					// Unused Interrupt handler
0,					// Unused Interrupt handler
0,					// Unused Interrupt handler
0					// Unused Interrupt handler
};

// Routing table used to map IP0-IP7 to one of the user installed interrput
// handlers
int	int_route[8] = {
#if (PHOENIX_SYS & SA1)
-1,		// IP0 -> No interrupt handler
-1,		// IP1 -> No interrupt handler
1,			// IP2 -> System Controller interrupt handler
0,			// IP3 -> Vertical retrace interrupt handler
2,			// IP4 -> IDE Disk Interrupt
-1,		// IP5 -> No interrupt handler
-1,		// IP6 -> No interrupt handler
-1,		// IP7 -> No interrupt handler
#elif (PHOENIX_SYS & SEATTLE)
-1,		// IP0 -> No interrupt handler
-1,		// IP1 -> No interrupt handler
1,			// IP2 -> System Controller interrupt
3,  		// IP3 -> I/O Asic Physical interrupt
2,			// IP4 -> IDE Disk Interrupt
4,			// IP5 -> Hilink/NSS/Widget interrupt
0,			// IP6 -> Vertical Retrace interrupt
-1,		// IP7 -> No interrupt handler
#endif
};


// Table of application level installed system controller interrupt handlers
void	(*sys_exception_handler[32])(void) = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


// Prototypes for standard exception handlers
static void standard_exc_code0_handler(int, unsigned int *);
static void standard_exc_code1_handler(int, unsigned int *);
static void standard_exc_code2_handler(int, unsigned int *);
static void standard_exc_code3_handler(int, unsigned int *);
static void standard_exc_code4_handler(int, unsigned int *);
static void standard_exc_code5_handler(int, unsigned int *);
static void standard_exc_code6_handler(int, unsigned int *);
static void standard_exc_code7_handler(int, unsigned int *);
static void standard_exc_code8_handler(int, unsigned int *);
static void standard_exc_code9_handler(int, unsigned int *);
static void standard_exc_code10_handler(int, unsigned int *);
static void standard_exc_code11_handler(int, unsigned int *);
static void standard_exc_code12_handler(int, unsigned int *);
static void standard_exc_code13_handler(int, unsigned int *);
static void standard_exc_code14_handler(int, unsigned int *);
static void standard_exc_code15_handler(int, unsigned int *);


// Table of standard handlers that get called after any user installed
// handlers for a particular exception
static void	(*standard_handler[16])(int, unsigned int *) =
{
standard_exc_code0_handler,				// IP2 - IP7
standard_exc_code1_handler,
standard_exc_code2_handler,
standard_exc_code3_handler,
standard_exc_code4_handler,
standard_exc_code5_handler,
standard_exc_code6_handler,
standard_exc_code7_handler,
standard_exc_code8_handler,
standard_exc_code9_handler,
standard_exc_code10_handler,
standard_exc_code11_handler,
standard_exc_code12_handler,
standard_exc_code13_handler,
standard_exc_code14_handler,
standard_exc_code15_handler				// Floating point
};


static void standard_fpu_handler(int, int *);

// Table of standard FPU exception handlers
static void (*standard_fpu[6])(int, int *) = {
standard_fpu_handler,				// Inexact result
standard_fpu_handler,				// Underflow
standard_fpu_handler,				// Overflow
standard_fpu_handler,				// Divide by 0
standard_fpu_handler,				// Invalid operation
standard_fpu_handler					// Uniplemented operation
};

static void	(*user_tlb_func)(void) = (void (*)(void))0;
static void	(*user_xtlb_func)(void) = (void (*)(void))0;
static void	(*user_cache_error_func)(void) = (void (*)(void))0;


static void go_debug(int, unsigned int *);

static int	tick_counter = 0;
int			watchdog_enabled = 0;

static char	*gt_int_string[] = {
"INTERRUPT",
"CPU MEMORY DECODE OUT OF RANGE",
"DMA MEMORY ACCESS OUT OF RANGE",
"CPU MEMORY ACCESS OUT OF RANGE",
"DMA CHANNEL 0",
"DMA CHANNEL 1",
"DMA CHANNEL 2",
"DMA CHANNEL 3",
"TIMER 0",
"TIMER 1",
"TIMER 2",
"TIMER 3",
"MASTER READ PARITY ERROR",
"SLAVE WRITE PARITY ERROR",
"MASTER WRITE PARITY ERROR",
"SLAVE READ PARITY ERROR",
"ADDRESS PARITY ERROR",
"MEMORY PARITY ERROR",
"MASTER ABORT",
"TARGET ABORT",
"RETRY COUNTER EXPIRATION",
"CPU INT 0",
"CPU INT 1",
"CPU INT 2",
"CPU INT 3",
"CPU INT 4",
"PCI INT A",
"PCI INT B",
"PCI INT C",
"PCI INT D",
"CPU INTERRUPT",
"PCI INTERRUPT"
};

void init_user_vectors(void)
{
	int	i;

	// Clear out user installed exception vectors
	for(i = 0; i < sizeof(exception_vectors)/sizeof(void *); i++)
	{
		// General exceptions
		exception_vectors[i] = 0;
	}

	// Clear out user installed system controller interrupt vectors
	for(i = 0; i < sizeof(sys_exception_handler)/sizeof(void *); i++)
	{
		// GT64010 interrupts
		sys_exception_handler[i] = 0;
	}

	// Clear out user installed interrupt handlers
	for(i = 0; i < sizeof(interrupt_handler)/sizeof(void *); i++)
	{
		// Processor interrupts
		interrupt_handler[i] = 0;
	}

	// Clear out the user installed fpu exception handlers
	for(i = 0; i < sizeof(fpu_handler)/sizeof(void *); i++)
	{
		// FPU exceptions
		fpu_handler[i] = 0;
	}

	// TLB refill
	user_tlb_func = (void (*)(void))0;

	// XTLB refill
	user_xtlb_func = (void (*)(void))0;

	// Cache error
	user_cache_error_func = (void (*)(void))0;

	// Integer division by 0
	user_int0_handler = 0;

	// Integer division overflow
	user_into_handler = 0;
}

static char	*ip_strings[] = {"0", "1", "2", "3", "4", "5", "6", "7"};

// Hardware generated interrupt handler
void handle_interrupts(int cause, unsigned int *r_save)
{
	int	i;
	int	mask = 0x100;
	int	route;

	// Loop through and deal with all pending interrupts
	for(i = 0; i < 8; i++)
	{
		// Is this interrupt active ?
		if(cause & mask)
		{
			// Get the route for this interrupt
			route = int_route[i];

			// Is there a routing table entry for this interrupt ?
			if(route >= 0)
			{
				// Is there a user installed handler for this interrupt ?
				if(interrupt_handler[route])
				{
					// Call the users interrupt handler
					if(interrupt_handler[route]())
					{
						// Call the standard exception handler
						standard_exc_code0_handler(mask, r_save);
					}
				}

				// No user installed interrupt handler
				else
				{
					// Call the standard exception 0 handler
					standard_exc_code0_handler(mask, r_save);
				}
			}

			// No routing table entry: print a message and goto debugger
			else
			{
				puts("***** INTERRUPT: IP");
//				puts(ip_strings[i]);
//				puts(" *****\n");
show_addr(cause);
puts("\n");
				go_debug(cause, r_save);
				return;
			}

			// Clear the pending bit from the cause register
			r_save[CP0_CAUSE] &= ~mask;
		}

		// Next mask
		mask <<= 1;
	}
}

__asm__("
	.set	noreorder
	.globl	get_old_ips
get_old_ips:
	mfc0	$2,$12
	li		$8,0xff00
	and	$2,$2,$8
	not	$8
	and	$8,$2,$8
	ori	$8,1
	li		$9,6
	not	$9
	and	$8,$8,$9
	jr		$31
	mtc0	$8,$12
	.set	reorder

	.set	noreorder
	.globl	gc
gc:
	jr	$31
	mfc0	$2,$13
	.set	reorder
");

int get_old_ips(void);
int gc(void);


// Exception handler that gets called when the application has installed
// exception handlers.
static int	in_exc = 0;
void general_exception(int cause, unsigned int *r_save, int *sp)
{
	int	code = (cause >> 2) & 0x1f;

	// Are we already in exception service ?
	if(in_exc)
	{
		// YES - Put out a message
		dputs("\nException while in exeption handler\n");
		dputs("Reboot\n");

		// Reboot
		*((volatile int *)WATCHDOG_ADDR) = 0;
		while(1) ;
	}

	// Set in exception service flag
	in_exc = 1;

	// Turn on I/O ASIC led upon entry to exception handler
	*MAIN_CONTROL &= ~(1<<14);

	// Is the cause a hardware interrupt ?
	if(!code)
	{
		// YES - Deal with it
		handle_interrupts(cause, r_save);

		// Turn off I/O ASIC led when leaving exception handler
		*MAIN_CONTROL |= (1<<14);

		// Reset in exception service flag
		in_exc = 0;

		// Done
		return;
	}

	// Is there a user installed handler for this exception ?
	else if(exception_vectors[code])
	{
		// YES - Call it
		if(exception_vectors[code](cause, r_save))
		{
			// If user handler returns non-zero, call the standard handler too
			standard_handler[code](cause, r_save);
		}
	}

	// No user installed handler: just call the standard handler
	else
	{
		standard_handler[code](cause, r_save);
	}

	// Reset in exception service flag
	in_exc = 0;

	// Turn off I/O ASIC led when leaving exception handler
	*MAIN_CONTROL |= (1<<14);
}

#if (PHOENIX_SYS & SA1)
#define	FLASH_LED	0x80
#elif (PHOENIX_SYS & SEATTLE)
#define	FLASH_LED	0x2
#endif

// Processor Interrupts IP0 - IP7
static void standard_exc_code0_handler(int cause, unsigned int *r_save)
{
	int				i;
	int				gt_cause;
	unsigned char	*creg = (unsigned char *)(GT_INT_CAUSE - 1);
#if (PHOENIX_SYS & SA1)
	int	causes = 0;
#endif

	// Vertical retrace interrupt
	if(cause & VERTICAL_RETRACE_INT)
	{
#if (PHOENIX_SYS & SA1)
		// Set IP's
		causes |= VERTICAL_RETRACE_INT;
#endif

		// Increment the tick counter
		++tick_counter;

		// Turn the LED on/off every 8 ticks
		if(!(tick_counter & 7))
		{
			*((volatile int *)LED_ADDR) ^= FLASH_LED;
		}

		// If the watchdog timer has been enabled
		if(watchdog_enabled)
		{
			// Write the watchdog approximately 1 time per second
			if(!(tick_counter & 63))
			{
				*((volatile int *)WATCHDOG_ADDR) = 0;
			}
		}

		// Reset the vertical retrace interrupt
		*((volatile int *)VRETRACE_RESET_REG) = 0;
	}

#if (PHOENIX_SYS & SEATTLE)
	if(cause & IO_ASIC_INT)
	{
		// NOT SURE IF THIS IS RIGHT
		*((volatile int *)IOASIC_CONTROL) &= ~1;
	}

	if(cause & NSS_INT)
	{
		dputs("Hilink/NSS/Widget Interrupt - must be handled at user level\n");
	}
#endif

	if(cause & IDE_DISK_INT)
	{
		// Call the IDE disk interrupt handler
		ide_intr();
	}

#if (!(PHOENIX_SYS & VEGAS))
	// If a galileo interrupt - reset it
	if(cause & GALILEO_INT)
	{
#if (PHOENIX_SYS & SA1)
		// Set IP's
		causes |= GALILEO_INT;
#endif

		// Get the GT64010 interrupt cause register
		gt_cause = *((volatile int *)GT_INT_CAUSE);

		// We are only interested in the interrupts that are enabled
		gt_cause &= *((volatile int *)GT_INT_CPU_MASK);

		// Loop through and service ALL pending and enable interrupt from
		// the GT64010
		for(i = 0; i < sizeof(sys_exception_handler)/sizeof(void *); i++)
		{
			if(!(i & 7))
			{
				creg++;
			}
			if(gt_cause & 1)
			{
				*creg &= ~(1 << (i & 7));
			}

			// Only do this if NOT using debugger
#if (WDOG_TIMER == TIMER3_WDOG)
			if(i == 11 && (gt_cause & 1) && not_debug_mode)
#else
			if(i == 8 && (gt_cause & 1) && not_debug_mode)
#endif
			{
				feed_the_dog();
				ide_disk_timeout();
#if (WDOG_TIMER == TIMER0_WDOG)
				if(sys_exception_handler[8])
				{
					sys_exception_handler[8]();
				}
#endif
			}
			else if(sys_exception_handler[i] && (gt_cause & 1))
			{
				sys_exception_handler[i]();
			}
			else if(gt_cause & 1)
			{
				if(i < 8 || i > 11)
				{
					puts("GT INT:  ");
					puts(gt_int_string[i]);
					puts("\n");
				}
			}
			gt_cause >>= 1;
		}
	}
#endif

#if (PHOENIX_SYS & SA1)
	// Wait for the cause register to tell use that they are off
	while(get_cause() & causes)
	{
		// Reset the vertical retrace interrupt
		*((volatile int *)VRETRACE_RESET_REG) = 0;
	}
#endif
}


static void show_pc_and_inst(unsigned int *r_save)
{
	int	inst;
	int	good_pc;

	puts("PC:             0x");
	show_addr(r_save[PC]);
	puts("\n");
	puts("SP:             0x");
	show_addr(r_save[GP29]);
	puts("\n");
	if(!(r_save[PC] & 0x3))
	{
		if(r_save[PC] >= (unsigned int)0xbfc00000 && r_save[PC] < (unsigned int)0xbfc40000)
		{
			good_pc = 1;
		}
		else if(r_save[PC] >= (unsigned int)0x9fc00000 && r_save[PC] < (unsigned int)0x9fc40000)
		{
			good_pc = 1;
		}
		else if(r_save[PC] >= (unsigned int)0x80000000 && r_save[PC] < (unsigned int)0x80800000)
		{
			good_pc = 1;
		}
		else if(r_save[PC] >= (unsigned int)0xa0000000 && r_save[PC] < (unsigned int)0xa0800000)
		{
			good_pc = 1;
		}
		else
		{
			good_pc = 0;
		}
		if(!good_pc)
		{
			puts("**** BOGUS PC ****\n");
			return;
		}
		if(r_save[CP0_CAUSE] & 0x80000000)
		{
			inst = *((int *)(r_save[PC] + 4));
		}
		else
		{
			inst = *((int *)r_save[PC]);
		}
		puts("Instruction:    0x");
		show_addr(inst);
		puts("\n");
	}
	else
	{
		puts("**** PC is NOT word aligned ****\n");
		puts("Probable cause:  branch or jump\n");
	}
}

static void standard_exc_code1_handler(int cause, unsigned int *r_save)
{
	puts("TLB Modification\n");
	show_pc_and_inst(r_save);
	puts("Bad Address:    0x");
	show_addr(r_save[CP0_BADVADDR]);
	puts("\n");
	go_debug(cause, r_save);
}

static void standard_exc_code2_handler(int cause, unsigned int *r_save)
{
	int	inst;

	puts("TLB Exception (load or instruction fetch)\n");
	show_pc_and_inst(r_save);
	if(!(r_save[PC] & 0xe0000000))
	{
		puts("Program counter is in NOT in and KSEG\n");
	}
	puts("Bad Address:    0x");
	show_addr(r_save[CP0_BADVADDR]);
	puts("\n");
	go_debug(cause, r_save);
}

static void standard_exc_code3_handler(int cause, unsigned int *r_save)
{
	puts("TLB Exception (store)\n");
	show_pc_and_inst(r_save);
	puts("Bad Address:    0x");
	show_addr(r_save[CP0_BADVADDR]);
	puts("\n");
	go_debug(cause, r_save);
}

static void standard_exc_code4_handler(int cause, unsigned int *r_save)
{
	puts("Address Error (load or instruction fetch)\n");
	show_pc_and_inst(r_save);
	puts("Bad Address:    0x");
	show_addr(r_save[CP0_BADVADDR]);
	puts("\n");
	go_debug(cause, r_save);
}

static void standard_exc_code5_handler(int cause, unsigned int *r_save)
{
	puts("Address Error (store)\n");
	show_pc_and_inst(r_save);
	puts("Bad Address:    0x");
	show_addr(r_save[CP0_BADVADDR]);
	puts("\n");
	go_debug(cause, r_save);
}

static void standard_exc_code6_handler(int cause, unsigned int *r_save)
{
	puts("Bus error (instruction fetch\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code7_handler(int cause, unsigned int *r_save)
{
	puts("Bus error (data reference)\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code8_handler(int cause, unsigned int *r_save)
{
	puts("System Call\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code9_handler(int cause, unsigned int *r_save)
{
	int	inst;

	// Get the instruction
	inst = *((int *)r_save[PC]);

	// Shift down the code
	inst >>= 6;

	// And off the un-needed bits
	inst &= 0xfffff;

	// Is it an integer divide by 0
	if(inst == 0x1c00)
	{
		user_int0_handler();
	}

	// Is it an integer divide overflow
	else if(inst == 0x1800)
	{
		user_into_handler();
	}

	// Otherwise we don't deal with it
	else
	{
		puts("Breakpoint\n");
		show_pc_and_inst(r_save);
		go_debug(cause, r_save);
	}
}

static void standard_exc_code10_handler(int cause, unsigned int *r_save)
{
	puts("Reserved Instruction\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code11_handler(int cause, unsigned int *r_save)
{
	puts("Coprocessor Unusable\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code12_handler(int cause, unsigned int *r_save)
{
	puts("Arithmetic Overflow (interger)\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code13_handler(int cause, unsigned int *r_save)
{
	puts("Trap\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

static void standard_exc_code14_handler(int cause, unsigned int *r_save)
{
	puts("Reserved Exception\n");
	show_pc_and_inst(r_save);
	go_debug(cause, r_save);
}

// Floating point exception
static void standard_exc_code15_handler(int cause, unsigned int *r_save)
{
	int	i;

	// Loop through all of the fpu exception handlers
	for(i = 0; i < sizeof(standard_fpu)/sizeof(void *); i++)
	{
		// Is this fpu exception active ?
		if(r_save[FCR31] & ((1 << 12) << i))
		{
			// YES - has user installed a handler ?
			if(fpu_handler[i])
			{
				// YES - Do users function
				fpu_handler[i](r_save);
			}

			// No user handler installed
			else
			{
				// Do standard handler
				standard_fpu[i](r_save[FCR31], r_save);

				// Go to the debugger
				go_debug(cause, r_save);
			}
	
			// Turn off this fpu exception cause
			r_save[FCR31] &= ~((1<<12) << i);
		}
	}
}

// Standard FPU exception handler
static void standard_fpu_handler(int fpcsr, int *r_save)
{
	puts("\nFloating Point Execption\n");
	show_pc_and_inst(r_save);
	puts("\nFPCSR:  0x");
	show_addr(fpcsr);
	puts("\n");
}

// Function to copy the fast exception handler register save area to the
// debugger stub register save area and then go to debugger servce.
static void go_debug(int cause, unsigned int *r_save)
{
	int	i;

	if(get_scsi_device_number() < 0)
	{
		dputs("go_debug() - debug entry with no SCSI card\n");
		dputs("cause:  ");
		dphex(cause);
		dputs("\n");
		return;
	}
	for(i = 0; i < 200; i++)
	{
		exregs[i] = r_save[i];
	}
	reg_runflag = 0;
	active = 1;
	reg_extype = (char)((cause >> 2) & 0x1f);

	__asm__("	move	$4,$0");
	__asm__("	la		$31,restart");
	__asm__("	j		process_scsi");
	__asm__("	nop");

}

__asm__("
	.set	noreorder
	.globl	get_pc
get_pc:
	jr		$31
	mfc0	$2,$14
	.set	reorder

	.set	noreorder
	.globl	get_bv
get_bv:
	jr		$31
	mfc0	$2,$8
	.set	reorder
");

unsigned int *get_pc(void);
unsigned int *get_bv(void);

void tlb_exc(void)
{
	// Put out a message
	puts("\nTLB Refill Exception\n");

	// Are we already in exception service
	if(in_exc)
	{
		// YES - put out a message
		dputs("\nException while in exeption handler\n");
		dputs("Reboot\n");

		// Reboot
		*((volatile int *)WATCHDOG_ADDR) = 0;
		while(1) ;
	}

	// Is there a user installed TLB Refill exception handler ?
	if(user_tlb_func)
	{
		// YES - Call it
		user_tlb_func();
	}

	// Reboot
	*((volatile int *)WATCHDOG_ADDR) = 0;
	while(1) ;
}

void xtlb_exc(void)
{
	// Put out a message
	puts("\nXTLB Refill Exception\n");

	// Are we already in exception service ?
	if(in_exc)
	{
		// YES - Put out a message
		dputs("\nException while in exeption handler\n");
		dputs("Reboot\n");

		// Reboot
		*((volatile int *)WATCHDOG_ADDR) = 0;
		while(1) ;
	}

	// Is there a user installed XTLB Refill exception handler ?
	if(user_xtlb_func)
	{
		// YES - Call it
		user_xtlb_func();
	}

	// Reboot
	*((volatile int *)WATCHDOG_ADDR) = 0;
	while(1) ;
}

void cache_exc(void)
{
	// Put out a message
	puts("\nCache Error Exception\n");

	// Are we already in exception service
	if(in_exc)
	{
		// YES - Put out a message
		dputs("\nException while in exeption handler\n");
		dputs("Reboot\n");

		// Reboot
		*((volatile int *)WATCHDOG_ADDR) = 0;
		while(1) ;
	}

	// Is there a user installed Cache Error Exception handler ?
	if(user_cache_error_func)
	{
		// YES - Call it
		user_cache_error_func();
	}

	// Reboot
	*((volatile int *)WATCHDOG_ADDR) = 0;
	while(1) ;
}

#define	TIMER_START_VAL	0x00ffffff

void install_sys_handler(int num, void (*func)(void))
{
	volatile unsigned char *creg = (volatile unsigned char *)GT_INT_CAUSE + (num >> 3);

#if (WDOG_TIMER == TIMER3_WDOG)
	if(num < 0 || num >= sizeof(sys_exception_handler)/sizeof(void *) || num == 11)
#else
	if(num < 0 || num >= sizeof(sys_exception_handler)/sizeof(void *))
#endif
	{
		return;
	}
	if(func)
	{
		// Make sure the any pending interrupt from this is cleared before
		// Enabling the interrupt
		*creg &= ~(1 << (num & 7));

		// Enable the interrupt
		*((volatile int *)GT_INT_CPU_MASK) |= (1<<num);
	}
	else
	{
#if (WDOG_TIMER == TIMER0_WDOG)
		if(num != 8)
		{
#endif
			// Disable the interrupt
			*((volatile int *)GT_INT_CPU_MASK) &= ~(1<<num);

			// Make sure any pending from this are cleared
			*creg &= ~(1 << (num & 7));
#if (WDOG_TIMER == TIMER0_WDOG)
		}
#endif
	}
	sys_exception_handler[num] = func;
}

__asm__("
	.set	noreorder
	.globl	enable_fpu_exc
enable_fpu_exc:
	cfc1	$8,$31
	or		$4,$4,$8
	jr		$31
	ctc1	$4,$31
	.set	reorder

	.set	noreorder
	.globl	disable_fpu_exc
disable_fpu_exc:
	cfc1	$8,$31
	not	$4
	and	$4,$4,$8
	jr		$31
	ctc1	$4,$31
	.set	reorder
");

void install_fpu_handler(int num, int (*func)(int *))
{
	if(num < 0 || num >= sizeof(fpu_handler)/sizeof(void *))
	{
		return;
	}
	fpu_handler[num] = func;
	if(num < ((sizeof(fpu_handler)/sizeof(void *)) - 1))
	{
		// Enable the exception
		if(func)
		{
			enable_fpu_exc(0x80 << num);
		}

		// Disable the exception
		else
		{
			disable_fpu_exc(0x80 << num);
		}
	}
}

void install_int_div0_handler(void (*func)(void))
{
	user_int0_handler = func;
}

void install_int_divo_handler(void (*func)(void))
{
	user_into_handler = func;
}

void install_interrupt_handler(int num, int (*func)(void))
{
	int	i;

	if(num < 0 || num >= sizeof(interrupt_handler)/sizeof(void *))
	{
		return;
	}
	for(i = 0; i < sizeof(int_route)/sizeof(int); i++)
	{
		if(int_route[i] == num)
		{
			if(func)
			{
				// Is this the vertical retrace interrupt ?
				if(num == 0)
				{
					// Make sure the vertical retrace interrupt is cleared
					*((volatile int *)VRETRACE_RESET_REG) = 1;

					// YES - Enable it at the PLD
					*((volatile int *)ICPLD_INT_ENBL_REG) |= VSYNC_INT_ENABLE;
				}
				enable_ip(0x100 << i);
			}
			else
			{
				// Is this the vertical retrace interrupt ?
				if(num == 0)
				{
					// YES - Disable it at the PLD
					*((volatile int *)ICPLD_INT_ENBL_REG) &= ~VSYNC_INT_ENABLE;

					// Make sure the vertical retrace interrupt is cleared
					*((volatile int *)VRETRACE_RESET_REG) = 1;
				}
				disable_ip(0x100 << i);
			}
			break;
		}
	}
	if(i == sizeof(int_route)/sizeof(int))
	{
		return;
	}
	interrupt_handler[num] = func;
}

void install_tlb_handler(void (*func)(void))
{
	user_tlb_func = func;
}

void install_xtlb_handler(void (*func)(void))
{
	user_xtlb_func = func;
}

void install_cache_error_handler(void (*func)(void))
{
	user_cache_error_func = func;
}

void install_vector(int num, int (*func)(int, int *))
{
	if(num < 0 || num >= sizeof(exception_vectors)/sizeof(void *))
	{
		return;
	}
	exception_vectors[num] = func;
}
