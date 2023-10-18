//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 20 $
//
// $Author: Mlynch $
//

#include	<system.h>
#include <io.h>
#include	<ide.h>
#include	"find.h"
#include "scsi.h"
#include "pci_8xx.h"
#include "script.out"   /* the scsi chip script */

#if (PHOENIX_SYS & FLAGSTAFF)
#error psyq.c - FLAGSTAFF not supported yet
#endif

/* Error codes to be returned in sense block */
#define NOSENSE			0x00
#define BADCOMMAND		0x40
#define BADUNIT			0x41
#define ILLEGALPARMS		0x42
#define SCSIPARITY		0x43
#define SCSIERROR			0x44
#define UNITBUSY			0x45

/* Sense buffer index */
#define SENS_ERRTYPE		0	/* error type code $70 => current error */
#define SENS_SEGNO		1	/* segment number (NU) */
#define SENS_KEY			2	/* sense key (others or 9 for DOS codes) */
#define SENS_INFO			3	/* 'Information' (NU) */
#define SENS_ADDLEN		7	/* additional sense length (sens_len-7) */
#define SENS_COMSPEC		8	/* command specific info (NU) */
#define SENS_ASC			12	/* Additional Sense Code (DOS error) */
#define SENS_ASCQ			13	/* Additional Sense Code Qualifier */
#define SENS_FRUC			14	/* field replaceable unit code */
#define SENS_SENSSPEC	15	/* sense key specific */
#define SENS_ADDBYTES	18	/* additional sense bytes */
#define SENS_HANDLE     18
#define SENS_FILELENGTH 20
#define SENS_ATTRIBUTES 24
#define SENS_HWSTATUS   26
#define SENS_LEN			28	/* length of sense block */

// Convert logical address to physical address
#define RAMADDR(x) 		(((unsigned long) &(x)) & 0x1fffffff)

// Convert address to uncached address
#define UNCACHED(x) 		(((unsigned int) &(x)) | 0xa0000000)

// Size of the memory buffer in psyqdbg.s
#define MEM_BUF_SIZE 	2048

#define RELOCATE(lab, value) for (i = 0; i < sizeof(E_ ## lab ## _Used)/sizeof(ULONG); i++) \
                             * (unsigned long *) UNCACHED(SCRIPT[E_ ## lab ## _Used[i]]) = (value)
			
#define SETBUF(buf, addr, len) buffer_table[buf].count = len; buffer_table[buf].address = addr

// Size of the file buffer
#define FILE_BUF_LEN		32768

enum offsets {
	msg_out_buf1 = 0,
	msg_out_buf2,
	synch_neg_buf,
	wide_neg_buf,
	cmd_buf,
	stat_buf,
	stat_ok_buf,
	stat_interm_buf,
	stat_chk_buf,
	msg_in_buf,
	msg_cmp_buf,
	msg_rej_buf,
	id_buf,
	sns_buf,
	sns_badlun_buf,
	rgs_buf,
	inq_buf,
	inq_badlun_buf,
	run_buf,
	mem_buf,
};

struct _table {
	unsigned long   count;
	unsigned long   address;
};

typedef struct _table table;

// Base address of the SCSI Controller
#if (!(PHOENIX_SYS & VEGAS))
volatile unsigned char *scsi_regs = (volatile unsigned char *) SCSI_BASE_ADDR;
#else
volatile unsigned char *scsi_regs = (volatile unsigned char *)0;
unsigned long scsi_base_addr;
#endif

// All of this stuff is declared is psyqdbg.s
extern unsigned char reg_runflag;
extern unsigned int exregs[], reg_extype;
extern unsigned int reg_cp0_status;
extern unsigned int reg_cp0_cause;
extern unsigned int proc, proc_save;
extern void			  (*callback)(int);
extern int				reg_fcr31;
extern int				user_int0_handler;
extern int				user_into_handler;
#if (PHOENIX_SYS & VEGAS)
//#ifdef VEGAS
int						debug_capable = 0;
#endif

extern volatile table buffer_table[];
extern volatile unsigned char command_buf[];
extern volatile unsigned char sense_buf[];
extern volatile unsigned char sense_badlun_buf[];
extern volatile unsigned char message_out_buf1[];
extern volatile unsigned char message_out_buf2[];
extern volatile unsigned char inquiry_buf[];
extern volatile unsigned char inquiry_badlun_buf[];
extern volatile unsigned char runstate_buf[];
extern volatile unsigned char memory_buf[];
extern volatile unsigned int regs_buf;
extern volatile unsigned char filebuff[];


// Function prototypes for external functions
void install_debug_hook(void);
void memcpy_fast (void *, void *, int);
void writeback_cache (void);
void flush_cache(void);
void show_banner(void);
void disable_interrupts(void);
void prc_delay(int);
void init_user_vectors(void);
void reset_disk_que(void);
void clear_user_drivers(void);
static void start_send_memory(void);
static void setup_dta(struct find_t *f);
static void setup_df(diskfree_t *);
void reinitialize_disk_cache(void);


// Messages
static unsigned char command_complete[] = {0x00};
static unsigned char linked_command_complete[] = {0x0a};
static unsigned char linked_flagged_command_complete[] = {0x0b};
static unsigned char message_reject[] = {0x07};
static unsigned int	sr_reg_save;


// Status values
static unsigned char status_good[] = {0x00};
static unsigned char status_interm[] = {0x10};
static unsigned char status_check[] = {0x02};
static int				scsi_initialized = 0;
static int				fi_is_valid = 0;


// Static values for this module
#if (PHOENIX_SYS & SA1)
static unsigned char id_string[] = "R5000       PHOENIX1.00 ";
#elif (PHOENIX_SYS & SEATTLE)
static unsigned char id_string[] = "R5000       SEATTLE1.00 ";
#elif (PHOENIX_SYS & FLAGSTAFF)
static unsigned char id_string[] = "R5000     FLAGSTAFF1.00 ";
#else
static unsigned char id_string[] = "R5000       MGVEGAS1.00 ";
#endif
static unsigned long mem_len, mem_addr, trans_len, copy_recvd, len_read;
static unsigned char connected, performing_file_op;
static unsigned char performing_sound_op;
static unsigned char linked, flagged;

static struct find_t	fi;

#if ((PHOENIX_SYS & VEGAS))
void disable_write_merge(void);
#endif

#if (!(PHOENIX_SYS & VEGAS))
int						in_debugger = 0;
#endif


// putchar function used here only
static void putchar(int val)
{
	_write(1, &val, 1);
}

char *get_platform_id(void)
{
	return(id_string);
}


// Search the PCI bus for the slot number of the SCSI card we are using
int get_scsi_device_number (void)
{
	int	i;
	int	id;


#if (!(PHOENIX_SYS & VEGAS))
	for(i = 6; i < 10; i++)
#else
	for(i = 0; i < 6; i++)
#endif
	{
		id = get_pci_config_reg(i, 0);
		if((id & 0xffff) == 0x1000)		/* 0x1000 = vendor id of symbios 53c810 */
		{
#if (PHOENIX_SYS & VEGAS)
			debug_capable = 1;
#endif
			return(i);
		}
	}
	return(-1);
}


// Display all of the SCSI registers
static void dump_scsi_regs(void)
{
	int i;

	puts("Scsi regs:\n");
	for(i = 0; i < 0x60; i++)
	{
		if((i & 15) == 0)
		{
			puts("\n");
			show_addr(i);
			puts(": ");
		}
		else if ((i & 7) == 0)
		{
			putchar(' ');
		}
		show_addr(scsi_regs[i]);
		putchar(' ');
	}
	puts("\n\n");
}


// Write a long to a register on the SCSI card
static void write_scsi_long(int reg, unsigned long value)
{
	*((volatile unsigned long *)&scsi_regs[reg]) = value;	
}


// Read a long from a register on the SCSI card
static unsigned long read_scsi_long(int reg)
{
	unsigned long	data;

	// Read the data
	data =  *((volatile unsigned long *)&scsi_regs[reg]);

	// Check for a parity error, tell user if it occurred, and reset it
#if (!(PHOENIX_SYS & VEGAS))
	while(*((volatile int *)0xac000c18) & 0x1000)
	{
		*((volatile int *)0xac000c18) = 0;
		data =  *((volatile unsigned long *)&scsi_regs[reg]);
	}
#endif

	// Return the data
	return(data);
}


// Read a byte from a register on the SCSI card
static unsigned char read_scsi_byte (int reg)
{
	unsigned long regval = read_scsi_long(reg & ~3);

	return(regval >> ((reg & 3) * 8)) & 0xff;
}


// Reset the SCSI card
static void init_scsi_chip(void)
{
	int i;
	
	// Set reset bit
	scsi_regs[ISTAT] |= 0x40;

	// Wait a bit
	for (i = 0; i < 1000; i++)
	{
		;
	}

	// Reset the reset bit
	scsi_regs[ISTAT] &= ~0x40;

	// Set target mode parity checking
	scsi_regs[SCNTL0] = 0xc9;

	// Disable halt on parity error or ATN
	scsi_regs[SCNTL1] |= 0x20;

	// Set up clock dividers based on 50MHz clock
	scsi_regs[SCNTL3] = 0x33;

	// Set ID, enable response to selection
	scsi_regs[SCID] = 0x20 | SCSI_ID;

	// Set asynchronous
	scsi_regs[SXFER] = 0x00;

	// Set 8-transfer burst and read line
	scsi_regs[DMODE] = 0x88;

	// Enable DMA transfers
	scsi_regs[DIEN] = 0x7d;

	// Set 700 compatibility
	scsi_regs[DCNTL] = 0x01;

	// Enable SCSI interrupts
	scsi_regs[SIEN0] = 0x83;

	// Set response ID
	scsi_regs[RESPID0] = 1 << SCSI_ID;

	// Enable tolerant and disable single initiator response
	scsi_regs[STEST3] = 0x90;

	write_scsi_long(DSA, RAMADDR(buffer_table));

	// Set system to NOT connected
	connected = 0;
}


// Initialize the SCSI communications system
int scsi_init(void)
{
	int	i;
	int	id;

	// Reset the initialized flag
	scsi_initialized = 0;

	// The the PCI slot number of the SCSI controller
	id = get_scsi_device_number();

	if(id >= 0)
	{
#if (!(PHOENIX_SYS & VEGAS))
#if USE_SCSI_IO
		put_pci_config_reg(id, 4, (SCSI_IO_BASE_ADDR & 0x1fffffff));
		put_pci_config_reg(id, 1, 0x001d);
#else
		put_pci_config_reg(id, 5, (SCSI_MEM_BASE_ADDR & 0x1fffffff));
		put_pci_config_reg(id, 1, 0x001e);
#endif
#else
		put_pci_config_reg(id, 1, 0x001f);
		scsi_base_addr = get_pci_config_reg(id, 5);
		scsi_base_addr &= ~0xf;
		scsi_base_addr |= PCI_MEM_BASE;
		scsi_regs = (volatile unsigned char *)scsi_base_addr;
#endif

		// Initialize the SCSI card
		init_scsi_chip();
		
		regs_buf = ((((unsigned int) &exregs) & 0xff) << 24) + ((((unsigned int) &exregs) & 0xff00) << 8) + ((((unsigned int) &exregs) & 0xff0000) >> 8) + ((((unsigned int) &exregs) & 0xff000000) >> 24);

		// Initialize all of the assorted buffers
		memset(inquiry_buf, 0, 96);
		inquiry_buf[0] = 3;
		inquiry_buf[2] = 2;
		inquiry_buf[3] = 2;
		inquiry_buf[4] = 92;
		inquiry_buf[7] = 8;
		memcpy(inquiry_buf + 8, "PSY-Q   ", 8);
#if (PHOENIX_SYS & SA1)
		memcpy(inquiry_buf + 16, "SA1             ", 16);
#elif (PHOENIX_SYS & SEATTLE)
		memcpy(inquiry_buf + 16, "SEATTLE         ", 16);
#elif (PHOENIX_SYS & FLAGSTAFF)
		memcpy(inquiry_buf + 16, "FLAGSTAFF       ", 16);
#elif (PHOENIX_SYS & VEGAS)
		memcpy(inquiry_buf + 16, "MGVEGAS         ", 16);
#else
		memcpy(inquiry_buf + 16, "PHOENIX         ", 16);
#endif
		memcpy(inquiry_buf + 32, "1.00", 4);

		memcpy(inquiry_badlun_buf, inquiry_buf, 96);
		inquiry_badlun_buf[0] = 0x60;

		memset(sense_buf, 0, SENS_LEN);
		sense_buf[SENS_ERRTYPE] = 0x70;
		sense_buf[SENS_ADDLEN] = SENS_LEN - 7;

		memset(sense_badlun_buf, 0, SENS_LEN);
		sense_badlun_buf[SENS_ERRTYPE] = 0x70;
		sense_badlun_buf[SENS_KEY] = 0x05;			/* Illegal request */
		sense_badlun_buf[SENS_ADDLEN] = SENS_LEN - 7;
		sense_badlun_buf[SENS_ASC] = 0x25;			/* Logical unit not supported */
		sense_badlun_buf[SENS_ASCQ] = 0x00;
		sense_badlun_buf[SENS_ADDBYTES] = BADUNIT;	/* Psy-Q error code */

#if (!(PHOENIX_SYS & VEGAS))
		RELOCATE(chipreg_scratcha1, (SCSI_BASE_ADDR & 0x1fffffff) + SCRATCHA1);
		RELOCATE(chipreg_scratchb0, (SCSI_BASE_ADDR & 0x1fffffff) + SCRATCHB0);
		RELOCATE(chipreg_scratchb1, (SCSI_BASE_ADDR & 0x1fffffff) + SCRATCHB1);
		RELOCATE(chipreg_scratchb2, (SCSI_BASE_ADDR & 0x1fffffff) + SCRATCHB2);
		RELOCATE(chipreg_scratchb3, (SCSI_BASE_ADDR & 0x1fffffff) + SCRATCHB3);
#else
		RELOCATE(chipreg_scratcha1, (scsi_base_addr & 0x1fffffff) + SCRATCHA1);
		RELOCATE(chipreg_scratchb0, (scsi_base_addr & 0x1fffffff) + SCRATCHB0);
		RELOCATE(chipreg_scratchb1, (scsi_base_addr & 0x1fffffff) + SCRATCHB1);
		RELOCATE(chipreg_scratchb2, (scsi_base_addr & 0x1fffffff) + SCRATCHB2);
		RELOCATE(chipreg_scratchb3, (scsi_base_addr & 0x1fffffff) + SCRATCHB3);
#endif
		RELOCATE(command_buf_1, RAMADDR(command_buf) + 1);
		RELOCATE(command_buf_4, RAMADDR(command_buf) + 4);
		RELOCATE(command_buf_5, RAMADDR(command_buf) + 5);
		RELOCATE(command_buf_6, RAMADDR(command_buf) + 6);
		RELOCATE(command_buf_7, RAMADDR(command_buf) + 7);
		RELOCATE(command_buf_8, RAMADDR(command_buf) + 8);
		RELOCATE(command_buf_9, RAMADDR(command_buf) + 9);
		RELOCATE(command_buf_11, RAMADDR(command_buf) + 11);
		RELOCATE(inq_badlun_buf_count, RAMADDR(buffer_table[inq_badlun_buf].count));
		RELOCATE(sns_badlun_buf_count, RAMADDR(buffer_table[sns_badlun_buf].count));
		RELOCATE(sense_key, RAMADDR(sense_buf[SENS_KEY]));
		RELOCATE(sense_asc, RAMADDR(sense_buf[SENS_ASC]));
		RELOCATE(sense_ascq, RAMADDR(sense_buf[SENS_ASCQ]));
		RELOCATE(sense_addbytes, RAMADDR(sense_buf[SENS_ADDBYTES]));
		RELOCATE(reg_runflag, RAMADDR(reg_runflag));
		RELOCATE(reg_extype, RAMADDR(reg_extype));
		RELOCATE(runstate0, RAMADDR(runstate_buf[0]));
		RELOCATE(runstate1, RAMADDR(runstate_buf[1]));

		SETBUF(msg_out_buf1, RAMADDR(message_out_buf1), 1);
		SETBUF(msg_out_buf2, RAMADDR(message_out_buf2), 1);
		SETBUF(synch_neg_buf, RAMADDR(message_out_buf2), 2);
		SETBUF(wide_neg_buf, RAMADDR(message_out_buf2), 1);
		SETBUF(cmd_buf, RAMADDR(command_buf), 6);
		SETBUF(stat_buf, RAMADDR(status_good), 1);
		SETBUF(stat_ok_buf, RAMADDR(status_good), 1);
		SETBUF(stat_interm_buf, RAMADDR(status_interm), 1);
		SETBUF(stat_chk_buf, RAMADDR(status_check), 1);
		SETBUF(msg_in_buf, RAMADDR(command_complete), 1);
		SETBUF(msg_cmp_buf, RAMADDR(command_complete), 1);
		SETBUF(msg_rej_buf, RAMADDR(message_reject), 1);
		SETBUF(id_buf, RAMADDR(id_string), 24);
		SETBUF(sns_buf, RAMADDR(sense_buf), 20);
		SETBUF(sns_badlun_buf, RAMADDR(sense_badlun_buf), 20);
		SETBUF(rgs_buf, RAMADDR(regs_buf), 4);
		SETBUF(inq_buf, RAMADDR(inquiry_buf), 96);
		SETBUF(inq_badlun_buf, RAMADDR(inquiry_badlun_buf), 96);
		SETBUF(run_buf, RAMADDR(runstate_buf), 4);

		// Enable PCI INTA scsi interrupt on IP7
#if (PHOENIX_SYS & SA1)
		*((volatile int *)ICPLD_INT_MAPB_REG) |= 3;			/* Map PCI INTA to interrupt level 5 */
		*((volatile int *)ICPLD_INT_ENBL_REG) |= 1 << 8;	/* Enable interrupt */

		// Enable interrupt switch on IP7
		*((volatile int *)ICPLD_INT_MAPA_REG) |= 3;		/* Map Expansion slot 0 Interrupt to level 5 */
		*((volatile int *)ICPLD_INT_ENBL_REG) |= 1;		/* Enable it */

#elif (PHOENIX_SYS & SEATTLE)
		// Hilink/NSS/Widget on IP5 (INT3)
		// Veritcal retrace on IP6 (INT4)
		// PCI slot on IP7 (INT5)
		// Debug switch on IP7 (INT5)
		*((volatile int *)ICPLD_INT_MAP_REG) |= ((3<<6)|(3<<12)|(2<<14)|(1<<2));
		*((volatile int *)ICPLD_INT_ENBL_REG) |= ((1<<3)|(1<<6)|(1<<7)|(1<<1));
#elif (PHOENIX_SYS & FLAGSTAFF)
#error psyq.c - FLAGSTAFF not support yet
#elif (PHOENIX_SYS & VEGAS)
#else
#error psyq.c - PHOENIX_SYS not set
#endif
		
		// Enable the interrupts on the processor
		enable_ip(SCSI_INT);

		write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_wait_select);

		// Set the initialized flag
		scsi_initialized = 1;
	}

#if (PHOENIX_SYS & SEATTLE)
	// Map and enable the vertical retrace interrupt regardless of whether
	// or not the scsi card initializes properly
	// Hilink/NSS/Widget on IP5 (INT3)
	// Veritcal retrace on IP6 (INT4)
	*((volatile int *)ICPLD_INT_MAP_REG) |= ((2<<14)|(1<<2));
	*((volatile int *)ICPLD_INT_ENBL_REG) |= ((1<<7)|(1<<1));
#endif

	// Return success
	return(0);
}


// Set the status to be sent for a file operation request
static void set_status_fileop(unsigned char stat, unsigned short handle, unsigned int len, unsigned short attr, unsigned short hwstat)
{
	memset(sense_buf, 0, SENS_LEN);
	
	sense_buf[SENS_ERRTYPE] = 0x70;
	sense_buf[SENS_KEY] = 9;
	sense_buf[SENS_ASC] = stat | 0x80;
	sense_buf[SENS_ADDLEN] = SENS_LEN - 7;
	sense_buf[SENS_HANDLE] = (handle >> 8) & 0xff;
	sense_buf[SENS_HANDLE+1] = handle & 0xff;
	sense_buf[SENS_FILELENGTH] = (len >> 24) & 0xff;
	sense_buf[SENS_FILELENGTH+1] = (len >> 16) & 0xff;
	sense_buf[SENS_FILELENGTH+2] = (len >> 8) & 0xff;
	sense_buf[SENS_FILELENGTH+3] = len & 0xff;
	sense_buf[SENS_ATTRIBUTES] = (attr >> 8) & 0xff;
	sense_buf[SENS_ATTRIBUTES+1] = attr & 0xff;
	sense_buf[SENS_HWSTATUS] = (hwstat >> 8) & 0xff;
	sense_buf[SENS_HWSTATUS+1] = hwstat & 0xff;

	buffer_table[stat_buf].address = RAMADDR(status_check);
	buffer_table[msg_in_buf].address = RAMADDR(command_complete);

	scsi_regs[SCRATCHA1] = 0;	/* will force a disconnect */
	connected = 0;
}


// Send the status for a file operation request
static void send_status_fileop(unsigned char stat, unsigned short handle, unsigned int len, unsigned short attr, unsigned short hwstat)
{
	set_status_fileop(stat, handle, len, attr, hwstat);
	write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_stat_mess);
}


// Set up a bad status response
static void set_status_bad(int psyq_stat)
{
	memset(sense_buf, 0, SENS_LEN);
	
	sense_buf[SENS_ERRTYPE] = 0x70;
	sense_buf[SENS_ADDLEN] = SENS_LEN - 7;
	sense_buf[SENS_ADDBYTES] = psyq_stat;

	buffer_table[stat_buf].address = RAMADDR(status_check);
	buffer_table[msg_in_buf].address = RAMADDR(command_complete);

	// Force a disconnect
	scsi_regs[SCRATCHA1] = 0;
	connected = 0;
}


// Send a bad status response
static void send_status_bad(int psyq_stat)
{
	set_status_bad(psyq_stat);
	write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_stat_mess);
}


// Set up an OK status response
static void set_status_ok(void)
{
	memset(sense_buf, 0, SENS_LEN);
	
	sense_buf[SENS_ERRTYPE] = 0x70;
	sense_buf[SENS_ADDLEN] = SENS_LEN - 7;

	if(linked)
	{
		buffer_table[stat_buf].address = RAMADDR(status_interm);
		if(flagged)
		{
			buffer_table[msg_in_buf].address = RAMADDR(linked_flagged_command_complete);
		}
		else
		{
			buffer_table[msg_in_buf].address = RAMADDR(linked_command_complete);
		}
	}
	else
	{
		buffer_table[stat_buf].address = RAMADDR(status_good);
		buffer_table[msg_in_buf].address = RAMADDR(command_complete);
		connected = 0;
	}
}


// Send an OK status response
static void send_status_ok(void)
{
	set_status_ok();
	write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_stat_mess);
}


// Reinitialize the SCSI controller
static void restart_scsi(void)
{
	init_scsi_chip();
	write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_wait_select);
}

#define M_READ 1
#define M_WRITE 2
#define M_PCI 4


// This table defines the memory that the debugger has access to and what
// type of accesses to make to that memory.
struct
{
	unsigned long start, end;
	int attr;
} mem_attr[] = {
				{0x00000000, 0x0003ffff, 0},							/* Protected NO Access */
				{0x00040000, 0x007fffff, M_READ+M_WRITE},			/* TLB cache */
				{0x00800000, 0x7fffffff, 0},							/* Nothing */
				{0x80000000, 0x83ffffff, M_READ+M_WRITE},			/* Cached RAM */
				{0x84000000, 0x8fffffff, 0},							/* Nothing */
				{0x90000000, 0x9fbfffff, M_READ+M_WRITE},			/* Cached I/O */
				{0x9fc00000, 0x9fffffff, M_READ},					/* Cached ROM */
				{0xa0000000, 0xa3ffffff, M_READ+M_WRITE+M_PCI},	/* Uncached RAM */
				{0xa4000000, 0xa7ffffff, 0},							/* Nothing */
				{0xa8000000, 0xa8ffffff, M_READ+M_WRITE},			/* 3DFX Card */
				{0xa9000000, 0xabffffff, 0},							/* Nothing */
				{0xac000000, 0xac000cff, M_READ+M_WRITE},			/* GT64010 */
				{0xac000d00, 0xafffffff, 0},							/* Nothing */
				{0xb0000000, 0xbfbfffff, M_READ+M_WRITE},			/* I/O */
				{0xbfc00000, 0xbfffffff, M_READ},					/* BOOT ROM */
				{0xc0000000, 0xffffffff, 0}							/* Nothing */
};


// Calculate transfers
static void calc_transfer(int rw)
{
	unsigned int i = 0;

	// Find the entry in the memory access table
	while(mem_addr > mem_attr[i].end)
	{
		i++;
	}

	// Figure out the maximum that can be tranferred from the memory zone
	trans_len = (mem_attr[i].end + 1) - mem_addr;

	// If amount requested is more than the amount available in the zone
	// then limit the tranfer length to all that is left
	if(trans_len > mem_len)
	{
		trans_len = mem_len;
	}


	// Is the memory for this zone accessible ?
	if((mem_attr[i].attr & rw) == 0)
	{
		// NOPE - Limit the transfer length to the size of the memory buffer
		if(trans_len > MEM_BUF_SIZE)
		{
			trans_len = MEM_BUF_SIZE;
		}

		// Was a read requested ? 
		if(rw == M_READ)
		{
			// YES - Clear out the memory buffer
			memset(memory_buf, 0, trans_len);
		}

		// Otherwise - write but don't bother copying the data to the target
		// address.
		else
		{
			copy_recvd = 0;
		}

		// Set up the transfer to/from address
		buffer_table[mem_buf].address = RAMADDR(memory_buf);
	}

	// Is this memory zone accessible from the PCI bus ?
	else if ((mem_attr[i].attr & M_PCI) == 0)
	{
		// NOPE - Limit the transfer to the size of the memory buffer
		if(trans_len > MEM_BUF_SIZE)
		{
			trans_len = MEM_BUF_SIZE;
		}
	
		// Was a read requested ?
		if(rw == M_READ)
		{
			// YES - Copy the data from the requested address to the memory buffer
			memcpy_fast((void *)memory_buf, (void *) mem_addr, trans_len);
		}

		// Write - Copy the data from the memory buffer to the requested address
		// after it has been received
		else
		{
			copy_recvd = 1;
		}

		// Set up the transfer to/from address
		buffer_table[mem_buf].address = RAMADDR(memory_buf);
	}

	// Memory is accessible from PCI bus so let DMA do the move
	else
	{
		// Don't copy the data - Let DMA do it
		copy_recvd = 0;

		// Set up the transfer to/from address
		buffer_table[mem_buf].address = mem_addr & 0x1fffffff;
	}

	// Set up the amount to transfer
	buffer_table[mem_buf].count = trans_len;

	// Decrement the amount transferred for this request
	mem_len -= trans_len;
}

// Finish up a sound operation
void complete_sound_op(void)
{
	int	len;
	int	timeout;
	unsigned short	*data_p;

	switch(command_buf[0])
	{
		case 0xc0:	// send sound data
		{
			// Get the length
			len = (command_buf[2] << 8) + command_buf[3];

			// Set the pointer
			data_p = (unsigned short *)filebuff;

			// Transfer the data to the sound section
			while(len)
			{
				// Wait for sound DSP to be NOT busy
				timeout = 100000;
				while((*((volatile int *)IOASIC_SOUND_STATUS) & 0x8080) != 0x8080)
				{
					if(!(--timeout))
					{
						send_status_bad(0x80);
						return;
					}
				}

				// Write the data
				*((volatile int *)IOASIC_SOUND_DATA_OUT) = *data_p++;

				// Decrement the count
				len -= 2;
			}

			// Done with the operation
			performing_sound_op = 0;

			// Send OK status
			send_status_ok();

			// Done
			break;
		}
		default:
		{
			send_status_bad(BADCOMMAND);
			break;
		}
	}
}

// Finish up a requested file operation
void complete_file_op(void)
{
	int	partition;
	int	sector;
	int	num_sectors;
	int	handle;
	int	mode;
	int	len;
	int	amount_written;

	switch(command_buf[0])
	{
#if 0
		case 0xc3:		// Exec
		{
			send_status_ok();
			exec(filebuff);
			break;
		}
#endif
		case 0xc6:		// Set working directory
		{
			send_status_ok();		// Dummy - No directories on target
			break;
		}
		case 0xcb:		// Get Disk free
		{
			send_status_ok();
			break;
		}
		case 0xe0:		// Create file
		{
			handle = _open((char *)filebuff, O_WRONLY);
			if(handle >= 0)
			{
				send_status_fileop(0, handle, 0, 0, 0);
			}
			else
			{
				send_status_fileop(4, 0, 0, 0, 0);	// 4 = out of handles
			}
			performing_file_op = 0;
			break;
		}
		case 0xe1:		// Open file
		{
			mode = (command_buf[4] << 24) + (command_buf[5] << 16) + (command_buf[6] << 8) + command_buf[7];
			handle = _open((char *)filebuff, mode);
			if(handle >= 0)
			{
				send_status_fileop(0, handle, 0, 0, 0);
			}
			else
			{
				send_status_fileop(4, 0, 0, 0, 0);	// 84 = out of handles
			}
			performing_file_op = 0;
			break;
		}
		case 0xe4:		// Write file
		case 0xe6:
		{
			handle = (command_buf[2] << 8) + command_buf[3];
			len = (command_buf[4] << 24) + (command_buf[5] << 16) + (command_buf[6] << 8) + command_buf[7];
			mode = len;
			len += 3;
			len &= ~3;
			if(_write(handle, (char *)filebuff, len) < 0)
			{
				send_status_fileop(-_get_errno(), 0, 0, 0, 0);
			}
			else
			{
				send_status_fileop(0, 0, mode, 0, 0);
			}
			performing_file_op = 0;
			break;
		}
		case 0xe3:		// Read file
		case 0xe5:
		{
			// Return length actually read
			send_status_fileop(0, 0, len_read, 0, 0);
			performing_file_op = 0;
			break;
		}
		case 0xe7:		// Delete file
		{
			if((handle = FSDelete((char *)filebuff)))
			{
				send_status_bad(0x80 | handle);
			}
			else
			{
				send_status_ok();
			}
			performing_file_op = 0;
			break;
		}
#if 0
		case 0xe8:		// Send file attributes
		{
			handle = get_file_attributes(filebuff);
			if(handle >= 0)
			{
				send_status_fileop(0, 0, 0, handle, 0);
			}
			else
			{
				send_status_fileop(-_get_errno(), 0, 0, 0, 0);
			}
			break;
		}
#endif
		case 0xea:		// Find first
		{
			if(!_findfirst((char *)filebuff, 0, &fi))
			{
				fi_is_valid = 1;
				send_status_fileop(0, 1, 0, 0, 0);
			}
			else
			{
				fi_is_valid = 0;
				send_status_fileop(-1, 0, 0, 0, 0);
			}
			performing_file_op = 0;
			break;
		}
#if 0
		case 0xe9:		// Set file attributes
		{
			// Get the old file attributes
			h = get_file_attributes(filebuff);

			if(set_file_attr(filebuff))
			{
				send_status_fileop(-_get_errno(), 0, 0, 0, 0);
			}
			else
			{
				// Send back old attributes
				send_status_fileop(0, 0, 0, h, 0);
			}
			performing_file_op = 0;
			break;
		}
#endif
		case 0xeb:		// Find next
		{
			send_status_fileop(0, 0, 0, 0, 0);
			if(_findnext(&fi))
			{
				fi_is_valid = 0;
			}
			else
			{
				fi_is_valid = 1;
			}
			performing_file_op = 0;
			break;
		}
#if 0
		case 0xec:		// Rename file
		{
			char *newname = filebuff;
			
			while(*newname++) ;
				
			if(rename(filebuff, newname))
			{
				send_status_bad(0x80 | -_get_errno());
			}
			else
			{
				send_status_ok();
			}
			performing_file_op = 0;
			break;
		}
#endif
		case 0xed:
		{
			send_status_ok();
			performing_file_op = 0;
			break;
		}
		case 0xf8:		// Get partition table
		{
			performing_file_op = 0;
			send_status_ok();
			break;
		}
		case 0xf9:		// Write Sectors
		{
			partition = command_buf[2];
			ide_set_partition(partition);
			sector = (command_buf[7] << 24) + (command_buf[6] << 16) + (command_buf[5] << 8) + command_buf[4];
			num_sectors = command_buf[8];
			if(!SecWrites(sector, (unsigned long *)filebuff, num_sectors))
			{
				set_status_bad(0x80);
			}
			ide_set_partition(1);
			send_status_ok();
			performing_file_op = 0;
			break;
		}
		case 0xfa:		// Read Sectors
		{
			performing_file_op = 0;
			send_status_ok();
			break;
		}
		case 0xff:		// Send the get disk free info
		{
			performing_file_op = 0;
			send_status_ok();
			break;
		}
		default:
		{
			send_status_bad(BADCOMMAND);
			performing_file_op = 0;
			break;
		}
	}
}

// Start receiving data from the SCSI card
static void start_recv_memory(void)
{
	// More data to receive ?
	if(mem_len)
	{
		// YES - Figure out the memory access mode
		calc_transfer(M_WRITE);

		// Get some data
		write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_recv_memory);
	}

	// Doing a sound operation ?
	else if(performing_sound_op)
	{
		complete_sound_op();
	}

	// Doing a file operation ?
	else if(performing_file_op)
	{
		// YES - Finish it up
		complete_file_op();
	}

	// No more data to transfer and NOT doing a file operation
	else
	{
		// YES - Send OK status
		send_status_ok();
	}
}


// Start sending memory
static void start_send_memory(void)
{
	// Data left to send ?
	if(mem_len)
	{
		// YES - Figure out the memory access mode
		calc_transfer(M_READ);

		// Write the data
		write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_send_memory);
	}

	// Doing a file operation ?
	else if(performing_file_op)
	{
		// Finish it up
		complete_file_op();
	}

	// No more data to transfer and NOT doing a file operation
	else
	{
		// YES - Send OK status
		send_status_ok();
	}
}


// Copy the information from a find_t structure to the file buffer so
// it can be sent to the PC
static void setup_dta(struct find_t *f)
{
	filebuff[0x15] = f->attrib;
	memcpy_fast((void *)&filebuff[0x16], &f->wr_time, 2);
	memcpy_fast((void *)&filebuff[0x18], &f->wr_date, 2);
	memcpy_fast((void *)&filebuff[0x1a], &f->size, 4);
	memcpy_fast((void *)filebuff + 0x1e, &f->file_name, 13);
}

extern int reg_pc;

//
// Process requests from the SCSI Card
// This is the main loop entered whenever some sort of exception occurs that
// is NOT being dealt with by the application level code.  This function will
// ALWAYS be entered when the SCSI card causes and interrupt OR the debug
// switch is pressed.  It will also be entered whenever an exception occurs
// that the application level has not installed a handler for.  Finally, it
// can be entered by the application by issuing a break 0x400 instruction
// with an outstanding SCSI interrupt pending.
//
void process_scsi(int istat)
{
	int				i;
	int				find_index;
	int				handle;
	char				*tmp;
	char				*tmp1;
	int				partition;
	int				num_sectors;
	int				sector;
	unsigned char	sist0;
	unsigned char	sist1;
	unsigned char	dstat;
	unsigned long	dsps;
	int				date;
	int				time;
	unsigned long	timestamp;

#if (!(PHOENIX_SYS & VEGAS))
//#ifndef VEGAS
	if(get_scsi_device_number() < 0)
#else
	if(!debug_capable)
#endif
	{
		*((volatile int *)WATCHDOG_ADDR) = 0;
		while(1) ;
	}

	// Save the proc value
	proc_save = proc;

	// Set the proc value
	proc = 0;

	// Set the in debugger flag
#if (!(PHOENIX_SYS & VEGAS))
	in_debugger = 1;
#endif

	// Save the status register
	sr_reg_save = reg_cp0_status;

	// Fool debugger into think processor is in 32 fp register mode
	reg_cp0_status |= 0x04000000;

	do
	{
		// Wait for an interrupt if there wasn't one passed in
		while((istat & 7) == 0)
		{
			istat = read_scsi_byte(ISTAT);
		}

		// INTFLY interrupt - MUST be dealt with first
		if(istat & 4)
		{
			scsi_regs[ISTAT] = read_scsi_byte(ISTAT) | 4;
			puts("INTFLY\n");
		}

		// DMA Interrupts
		if(istat & 1)
		{
			// Get the DMA status register
			dstat = read_scsi_byte(DSTAT);

			// Get the request
			dsps = read_scsi_long(DSPS);

			// Illegal DMA instruction ?
			if(dstat & 1)
			{
				// YES
				puts("DMA: Illegal instruction detected\n");
				puts("Script offset ");
				show_addr(read_scsi_long(DSP) - RAMADDR(SCRIPT) - 8);
				puts("\n");
				for(i = 0; i < 11; i++)
				{
					show_addr(buffer_table[i].address);
					putchar(' ');
					show_addr(buffer_table[i].count);
					puts("\n");
				}
				dump_scsi_regs();
			}

			// Valid DMA instruction ?
			if(dstat & 4)
			{
				// Figure out what the request was
				switch(dsps)
				{
					case A_processor_command:
					{
						connected = 1;
						linked = read_scsi_byte(SCRATCHA1) & 1;
						flagged = read_scsi_byte(SCRATCHA1) & 2;
						switch(command_buf[0])
						{
							case 0x00:		// test unit ready
							{
								send_status_ok();
								break;
							}
							case 0x02:		// send id
							{
								set_status_ok();
								write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_send_id);
								break;
							}
							case 0x03:		// request sense
							{
								buffer_table[sns_buf].count = command_buf[4];
								write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_send_sense);
								break;
							}
							case 0x05:		// nop
							{
								send_status_ok();
								break;
							}
							case 0x06:		// make safe
							{
								reg_cp0_status = 0;
								reg_extype = 0;
								reg_runflag = 0;
								send_status_ok();
								break;
							}
							case 0x0c:		// disable
							{
								reg_runflag = 0;
								reg_extype = 0;
								send_status_ok();
								break;
							}
							case 0x0d:		// enable
							{
								reg_runflag = -1;
								send_status_ok();
								break;
							}
							case 0x0e:		// send regs address
							{
								set_status_ok();
								write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_send_regs_addr);
								break;
							}
							case 0x0f:		// disable2
							{
								reg_runflag = 0;
								send_status_ok();
								break;
							}
							case 0x10:		// send run state
							{
								runstate_buf[0] = reg_runflag;
								runstate_buf[1] = reg_extype;
								runstate_buf[2] = 0;
								runstate_buf[3] = 0;
								set_status_ok();
								write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_send_runstate);
								break;
							}
							case 0x12:		// inquiry
							{
								buffer_table[inq_buf].count = command_buf[4];
								write_scsi_long(DSP, RAMADDR(SCRIPT) + Ent_send_inquiry);
								break;
							}
							case 0xc2:		// reboot
							{
#if (!(PHOENIX_SYS & VEGAS))
								// Send OK status
								send_status_ok();

								// Disable all interrupts
								disable_interrupts();

								// Unhook any application installed vectors
								unhook_vectors();

								// Disconnect any user installed vectors
								init_user_vectors();

								// Close all of the open files
								closeall();

								// delay a bit before resetting the SCSI controller
								// so it can finish sending the OK status
								prc_delay(0);

								// Restart the SCSI Controller
								restart_scsi();

								// Reinitialize a few of things
								fi_is_valid = 0;
								performing_file_op = 0;
								performing_sound_op = 0;
								proc = 0;
								proc_save = 0;

								// Reset the interrupt hooks
								install_debug_hook();

								// Show the banner
								show_banner();

								// Turn on the connected flag
								connected = 1;

								// Close all possibly open files
								closeall();

								// Reset the disk queues
								reset_disk_que();

								// Disable the vertical retrace interrupts
								reg_cp0_status &= ~VERTICAL_RETRACE_INT;

								// Make sure we are in 16 fp register mode
								reg_cp0_status &= ~0x04000000;

								// Make sure the vertical retrace interrupt is cleared
								*((volatile int *)VRETRACE_RESET_REG) = 1;

								// Disable the GT64010 DMA's
								*((volatile int *)GT_DMA_CHAN0_CONTROL) &= ~(1<<12);
								*((volatile int *)GT_DMA_CHAN1_CONTROL) &= ~(1<<12);
								*((volatile int *)GT_DMA_CHAN2_CONTROL) &= ~(1<<12);
								*((volatile int *)GT_DMA_CHAN3_CONTROL) &= ~(1<<12);

								// Disable the GT64010 Timers
								*((volatile int *)GT_TC_CONTROL) = 0;

								// Make sure the GT64010 interrupts are cleared
								*((volatile int *)GT_INT_CAUSE) = 0;

								// Initialize the disk callback function
								callback = (void (*)(int))0;	// Callback function

								// Remove any user installed drivers
								clear_user_drivers();

								// Clear possible pending interrupts from cause reg
								reg_cp0_cause &= ~(VERTICAL_RETRACE_INT|GALILEO_INT);
								// Re-initialize the disk caches
								reinitialize_disk_cache();

								// Clear any possible FPU exceptions (leave FS bit on)
								reg_fcr31 &= 0x01000003;

								// Make sure we don't turn on the FR bit
								sr_reg_save &= ~0x04000000;

								// Unistall any user installed integer div handlers
								user_int0_handler = 0;
								user_into_handler = 0;

#else
								// Send OK status
								send_status_ok();

								// Unhook any application installed vectors
								clear_user_handlers();

								// Close all of the open files
								closeall();

								// delay a bit before resetting the SCSI controller
								// so it can finish sending the OK status
								prc_delay(0);

								// Restart the SCSI Controller
								restart_scsi();

								// Reinitialize a few of things
								fi_is_valid = 0;
								performing_file_op = 0;
								performing_sound_op = 0;
								proc = 0;
								proc_save = 0;

								// Show the banner
								show_banner();

								// Turn on the connected flag
								connected = 1;

								// Reset the disk queues
								reset_disk_que();

								// Make sure we are in 16 fp register mode
								reg_cp0_status &= ~0x04000000;

								// Initialize the disk callback function
								callback = (void (*)(int))0;	// Callback function

								// Remove any user installed drivers
								clear_user_drivers();

								// Re-initialize the disk caches
								reinitialize_disk_cache();

								// Clear any possible FPU exceptions (leave FS bit on)
								reg_fcr31 &= (0x01000003|(0x1f<<7));

								// Make sure we don't turn on the FR bit
								sr_reg_save &= ~0x04000000;

								// Turn off write merging
								disable_write_merge();
#endif
								// Fini
								break;
							}
							case 0xea:		// Find first
						/*	case 0xc3:		 Exec (file name) */
							case 0xc6:		// Set working directory - dummy
							case 0xe0:		// Create file
							case 0xe1:		// Open file
							case 0xe7:		// Delete file
						/*	case 0xec:		 Rename file (file names) */
						/*	case 0xe8:		 Send file attributes (filename) */
						/*	case 0xe9:		 Set file attributes (filename) */
							{
								// Length of file name
								mem_len = (command_buf[2] << 8) + command_buf[3];
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 1;
								start_recv_memory();
								break;
							}
							case 0xee:		// Set file date
							{
								handle = (command_buf[2] << 8) + command_buf[3];
								date = (command_buf[4] << 8) + command_buf[5];
								time = (command_buf[6] << 8) + command_buf[7];
								if(_setftime(handle, date, time))
								{
									send_status_bad(0x81);
								}
								else
								{
									send_status_ok();
								}
								break;
							}
							case 0xc7:		// Send working directory - dummy
							{
								*filebuff = 0;
								mem_len = (command_buf[2] << 8) + command_buf[3];
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 0;
								start_send_memory();
								break;
							}
							case 0xc8:		// Init server
							{
								break;
							}
							case 0xcb:		// Get disk free - CHECK
							{
								FSGetDiskFree((diskfree_t *)filebuff);
								mem_len = 8;
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 1;
								start_send_memory();
								break;
							}
							case 0xcc:		// Find close
							{
								send_status_ok();
								break;
							}
							case 0xe2:		// Close file
							{
								handle = (command_buf[2] << 8) + command_buf[3];
								if(_close(handle))
								{
									send_status_bad(0x80 | -_get_errno());
								}
								else
								{
									send_status_ok();
								}
								break;
							}
							case 0xe3:		// Read file
							case 0xe5:
							{
								handle = (command_buf[2] << 8) + command_buf[3];
								mem_len = (command_buf[4] << 24) + (command_buf[5] << 16) + (command_buf[6] << 8) + command_buf[7];
								if(mem_len > FILE_BUF_LEN)
								{
									mem_len = FILE_BUF_LEN;
								}
								if(_read(handle, (char *)filebuff, ((mem_len + 3) & ~3)) < 0)
								{
									send_status_bad(0x80 | -_get_errno());
								}
								else
								{
									len_read = mem_len;
									mem_addr = (unsigned long)filebuff;
									performing_file_op = 1;
									start_send_memory();
								}
								break;
							}
							case 0xe4:		// Write file
							case 0xe6:
							{
								mem_len = (command_buf[4] << 24) + (command_buf[5] << 16) + (command_buf[6] << 8) + command_buf[7];
								if(mem_len > FILE_BUF_LEN)
								{
									mem_len = FILE_BUF_LEN;
								}
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 1;
								start_recv_memory();
								break;
							}
							case 0xeb:		// Find next
							{
								if(fi_is_valid)
								{
									setup_dta(&fi);
									mem_len = 128;
									mem_addr = (unsigned long)filebuff;
									performing_file_op = 1;
									start_send_memory();
								}
								else
								{
									send_status_fileop(-1, 0, 0, 0, 0);
								}
								break;
							}
							case 0xed:		// Send file date - CHECK
							{
								handle = (command_buf[2] << 8) + command_buf[3];
								if(_getftime(handle, &date, &time))
								{
									send_status_bad(0x80 | -_get_errno());
								}
								else
								{
									*((unsigned short *)filebuff) = (date & 0xffff);
									*((unsigned short *)(filebuff + 2)) = (time & 0xffff);
									mem_len = 4;
									mem_addr = (unsigned long)filebuff;
									performing_file_op = 1;
									start_send_memory();
								}
								break;
							}
#if 0
							case 0xef:		// Seek
							{
								handle = handles[(command_buf[2] << 8) + command_buf[3]];
								mode = command_buf[4];
								offset = (command_buf[5] << 24) + (command_buf[6] << 16) + (command_buf[7] << 8) + command_buf[8];
								if(fseek(handle, offset, mode))
								{
									send_status_bad(0x80 | -_get_errno());
								}
								else
								{
									*(int *)filebuff = ftell(handle);
									mem_len = 4;
									mem_addr = (unsigned long)filebuff;
									performing_file_op = 0;
									start_send_memory();
								}
								break;
							}
#endif
							case 0xf0:		// recv memory
							case 0xf2:		// recv memory blind
							{
								mem_len = (command_buf[2] << 16) + (command_buf[3] << 8) + command_buf[4];
								mem_addr = (command_buf[5] << 24) + (command_buf[6] << 16) + (command_buf[7] << 8) + command_buf[8];
								performing_file_op = 0;
								start_recv_memory();
								break;
							}
							case 0xf1:		// send memory
							case 0xf3:		// send memory blind
							{
								mem_len = (command_buf[2] << 16) + (command_buf[3] << 8) + command_buf[4];
								mem_addr = (command_buf[5] << 24) + (command_buf[6] << 16) + (command_buf[7] << 8) + command_buf[8];
								performing_file_op = 0;
								start_send_memory();
								break;
							}
							case 0xf8:		// Send partition table
							{
								mem_len = 512;
								tmp = (char *)ide_get_partition_table();
								tmp1 = (char *)filebuff;
								for(i = 0; i < 512; i++)
								{
									*tmp1++ = *tmp++;
								}
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 1;
								start_send_memory();
								break;
							}
							case 0xf9:		// Write Sectors
							{
								partition = command_buf[2];
								sector = (command_buf[7] << 24) + (command_buf[6] << 16) + (command_buf[5] << 8) + command_buf[4];
								num_sectors = command_buf[8];
								mem_len = num_sectors * 512;
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 1;
								start_recv_memory();
								break;
							}
							case 0xfa:		// Read Sectors
							{
								partition = command_buf[2];
								ide_set_partition(partition);
								sector = (command_buf[7] << 24) + (command_buf[6] << 16) + (command_buf[5] << 8) + command_buf[4];
								num_sectors = command_buf[8];
								if(!SecReads(sector, (unsigned long *)filebuff, num_sectors))
								{
									send_status_bad(0x81);
								}
								ide_set_partition(1);
								mem_addr = (unsigned long)filebuff;
								mem_len = num_sectors * 512;
								performing_file_op = 1;
								start_send_memory();
								break;
							}
							case 0xfb:		// Set current drive
							case 0xfc:		// Get current drive
							case 0xfd:		// Set time
							case 0xfe:		// Get time
							{
								break;
							}
							case 0xff:		// Send the get free disk info
							{
								mem_len = 8;
								mem_addr = (unsigned long)filebuff;
								performing_file_op = 1;
								start_send_memory();
								break;
							}
							case 0xc0:		// Send data to sound section
							{
								mem_len = (command_buf[2] << 8) + command_buf[3];
								mem_addr = (unsigned long)filebuff;
								performing_sound_op = 0;
								start_recv_memory();
								break;
							}
							default:
							{
								send_status_bad(BADCOMMAND);
								break;
							}
						}
						break;
					}

					// If the message in not recognized - restart the SCSI card
					case A_non_handled_msg:
					{
						restart_scsi();
						break;
					}

					// If the message is a bad extende message - restart the SCSI card
					case A_bad_extended_msg:
					{
						restart_scsi();
						break;
					}

					// If the command is not recognized - restart the SCSI card
					case A_non_handled_cmd:
					{
						restart_scsi();
						break;
					}

					// If a SIGP was received - restart the SCSI card
					case A_got_SIGP:
					{
						restart_scsi();
						break;
					}

					// If the PC just wants to know if communications is working
					// send OK status
					case A_inquiry_sent:
					case A_req_sense_sent:
					{
						send_status_ok();
						break;
					}

					// If a block of memory has just finish being send to the PC
					case A_memory_sent:
					{
						// Increment the address
						mem_addr += trans_len;

						// Go send some more if there is more to send
						start_send_memory();
						break;
					}

					// If the PC sent some data
					case A_memory_recvd:
					{
						// Are we supposed to copy it to it's final location ?
						if(copy_recvd)
						{
							// YES - Copy the data to the final address
							memcpy_fast((void *)mem_addr, (void *)memory_buf, trans_len);

							// Flush the caches too
							flush_cache();
						}

						// Increment the address
						mem_addr += (trans_len);

						// Get more if there is more to get
						start_recv_memory();
						break;
					}

					// If the interrupt receive is NOT recognized
					default:
					{
						puts("Unexpected interrupt ");
						show_addr(dsps);
						puts(" from scsi chip\n");
						restart_scsi();
						break;
					}
				}
			}

			// Is this a single step (I don't believe this is ever used)
			if(dstat & 8)
			{
				puts("DMA: Single step interrupt\n");
				puts("Script offset ");
				show_addr(read_scsi_long(DSP) - RAMADDR(SCRIPT) - 8);
				puts("\n");
				dump_scsi_regs();
			}

			// Was the DMA aborted for one reason or another ?
			if(dstat & 16)
			{
				puts("DMA: Aborted\n");
			}

			// Was a bus fault detected ?
			if(dstat & 32)
			{
				puts("DMA: Bus Fault\n");
				puts("Script offset ");
				show_addr(read_scsi_long(DSP) - RAMADDR(SCRIPT) - 8);
				puts("\n");
				dump_scsi_regs();
			}

			// Was a parity error detected ?
			if(dstat & 64)
			{
				puts("DMA: Master Data Parity Error\n");
			}
		}

		// SCSI controller interrupt
		if(istat & 2)
		{
			// These clear when read so we'll have to read them in one operation
			unsigned long scsi_sists = read_scsi_long(SIST0 & ~3);
			
			sist0 = (scsi_sists >> ((SIST0 & 3) * 8)) & 0xff;
			sist1 = (scsi_sists >> ((SIST1 & 3) * 8)) & 0xff;
			
			if(sist0 & 1)
			{
				puts("SCSI: Parity Error\n");
			}
			if(sist0 & 2)
			{
				restart_scsi();
			}
			if(sist0 & 4)
			{
				puts("SCSI: Unexpected Disconnect\n");
			}
			if(sist0 & 8)
			{
				puts("SCSI: Gross Error\n");
			}
			if(sist0 & 16)
			{
				puts("SCSI: Reselected\n");
			}
			if(sist0 & 32)
			{
				puts("SCSI: Selected\n");
			}
			if(sist0 & 64)
			{
				puts("SCSI: Function Complete\n");
			}
			if(sist0 & 128)
			{
				puts("SCSI: Phase Mismatch / ATN received\n");
			}
			if(sist1 & 1)
			{
				puts("SCSI: Handshake-to-Handshake Timer Expired\n");
			}
			if(sist1 & 2)
			{
				puts("SCSI: General Purpose Timer Expired\n");
			}
			if(sist1 & 4)
			{
				puts("SCSI: Selection/Reselection Timeout\n");
			}
		}

		// Reset the interrupt status
		istat = 0;

	// Keep going until disconnected or told to exit by request
	} while(connected || !reg_runflag);

	// If we got here because of a floating point exception, clear
	// the exception and adjust the PC
	if(exregs[199] & ((1<<12)|(1<<13)|(1<<14)|(1<<15)|(1<<16)|(1<<17)))
	{
		// Clear any and all floating point exceptions
		exregs[199] &= ~((1<<12)|(1<<13)|(1<<14)|(1<<15)|(1<<16)|(1<<17));

		// Did the exception occur in a branch delay slot?
		if(exregs[96] & 0x80000000)
		{
			// YES - Then set then figure out what kind of branch instruction
			// it was and set the pc to where it was to branch to
			// jr r31
			exregs[68] = exregs[62];
		}

		// Not in branch delay slot so just set pc to next instruction
		else
		{
			// Adjust the pc
			exregs[68] += 4;
		}
	}

	// Reset the in debugger flag
#if (!(PHOENIX_SYS & VEGAS))
	in_debugger = 0;
#endif

	// Restore the proc value
	proc = proc_save;

	// Restore the FR status bit to what it was
	if(sr_reg_save & 0x04000000)
	{
		reg_cp0_status |= 0x04000000;
	}
	else
	{
		reg_cp0_status &= ~0x04000000;
	}
}

int scsiopen(struct iocntb *io)
{
	if(scsi_initialized)
	{
		return(0);
	}
	return(-1);
}
