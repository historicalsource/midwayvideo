/****************************************************************************/
/*                                                                          */
/* ide.c - Source for IDE disk functions.                                   */
/*                                                                          */
/* Written by:  Dave Sheppard (ATARI Games Inc.)                            */
/* Modified by: Michael J. Lynch (Midway Video Inc.)                        */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 20 $                                                           */
/*                                                                          */
/****************************************************************************/
#include	<system.h>
#include	<stdproto.h>
#include	<ide.h>

//#define	IDE_DEBUG

#ifdef IDE_DEBUG
void dputs(char *);
void dphex(int);
#endif

#define	MAX_REQUESTS	128	// Maximum number of requests that can be queued

// Definition of an entry in the PRD table
typedef struct prd_entry
{
	unsigned int	paddr;			// Physical address
	int				byte_count;		// Number of bytes to transfer
} prd_entry_t;

// Defintion of structure for queue read requests
struct queue_data
{
	unsigned long	sec;				// Starting sector number to transfer from/to
	unsigned long	*buf;				// Address to transfer from/to
	unsigned long	num;				// Number of sectors to transfer
	int				request;			// Request type 0 = read, 1 = write
	void				*proc;			// Process pointer that made the request
	int				num_2_trans;	// Number of sectors to transfer
	int				retry_count;	// Number of retries
	volatile prd_entry_t	*prd;		// Pointer to the prd entry for this trans
};

// Definition of structure for queued reads
struct queue
{
	int				 				head;						// Head of the queue
	int				 				tail;						// Tail of the queue
	volatile struct queue_data	*qd[MAX_REQUESTS];	// Queue'd requests
};

/****************************************************************************/
/* External Data References                                                 */
/****************************************************************************/


/****************************************************************************/
/* External function references                                             */
/****************************************************************************/
void reset_count(void);					// Resets the processor count register
int get_count_reg(void);				// Get the current value of the count reg
void init_disk_cache(void);			// Initialize the disk cache
int disable_ip(int);						// Disable an interrupt
void enable_ip(int);						// Enable an interrupt
void reset_disk_que(void);				// Reset the disk request queue
void hit_invalidate_dcache(void *, int);
void hit_writeback_invalidate_dcache(void *, int);
void flush_disk_queue(void);
void ide_enable_timeout(void);
void ide_disable_timeout(void);
void ide_disk_timeout(void);



/****************************************************************************/
/* Statis data for this module                                              */
/****************************************************************************/
static unsigned long	ide_sector_buffer[LONGS_PER_SECTOR];	// temporary sector buffer
static unsigned long	partition_buffer[LONGS_PER_SECTOR];		// buffer for holding partition data
static DeviceDesc		device_list;									// drive information
static int				initialized = 0;								// initialization flag
static int				disk_active = 0;								// Active flag
static unsigned		max_sector = 0;
static int				ide_timeout_enable = 0;
static int				ide_timeout_time = 0;
int						ide_timer_ints = 0;
extern int				ide_timer_start;

int	ide_drive_status = 0;

// Callback function pointer
void				(*callback)(int) = (void (*)(int))0;	// Callback function
void				(*audit)(void) = (void (*)(void))0;

// The read queue
volatile static struct queue	queue = {0, 0};
volatile static struct queue_data	qd[MAX_REQUESTS];

// The prd entries used by the DMA
// These are in an uncached segment so as to eliminate some code.
// The first item is initialized here to get the section attribute to work
// properly.
//volatile prd_entry_t	prd[MAX_REQUESTS*2] __attribute__ ((section ("uncached.bss"))) = {
//{0, 0},
//};
//volatile prd_entry_t	*prd = (volatile prd_entry_t *)0xa007fff8;

extern	char	ex_stack;
volatile prd_entry_t	*prd;


// Pointer to the partition table information
static partition_table_t	*part_table = (partition_table_t *)partition_buffer;
static partition_info_t		*part_info = (partition_info_t *)0;


/****************************************************************************/
/* Prototypes for static functions in this module                           */
/****************************************************************************/
static int	ide_wait_bsy(void);
static int	ide_wait_drdy(void);
static int	ide_soft_reset(void);
static void	ide_hread_data(register unsigned int *);
static int	ide_hread_sectors(unsigned int *, int, int, int, int);
static int	ide_write_sectors(unsigned int *, int, int, int, int);
void	prc_delay(int);
static void	setup_harddrive(void);
static unsigned char PCI_ReadConfigByte(int, unsigned char);
static void PCI_WriteConfigByte(int, unsigned char, unsigned char);
static void PCI_WriteConfigWord(int, unsigned char, unsigned short);
static void PCI_WriteConfigDword(int, unsigned char, unsigned long);
static short que_trans(unsigned long sec, unsigned long *buf, unsigned long num, int type);
static int queued_req(volatile struct queue_data *qd);

/****************************************************************************/
/* Global Data                                                              */
/****************************************************************************/
// NOTE - The proc value below will be NULL whenever the debugger interrupt
// stub is running.  The value at proc will be NULL whenever the application
// calls read from outside the process loop and will contain a valid process
// pointer whenever an application level process is making the call.
// These values get set by the application using BIOS services.
int	**proc = (int **)0;			// pointer to current process pointer
int	**proc_save = (int **)0;	// save area used by debugger stub
void	(*suspend)(void);				// pointer to suspend_self() function
void	(*resume)(int *);				// pointer to resume() function

extern int	in_debugger;


/****************************************************************************/
/* Beginning of code                                                        */
/****************************************************************************/
//
// prc_delay() - This function is used for timing delays
//
void prc_delay(int amount)
{
	amount++;
	amount *= TICKS_100HZ;
	reset_count();
	do
	{
		;
	} while(get_count_reg() < amount);
}


//
// ide_set_device() - This function sets the current drive to the drive
// number specified by the device argument passed to it.
// LEGACY SHIT
//
int ide_set_device(int device)
{
	DRIVE_HEAD_REG &= ~(1<<4);
	DRIVE_HEAD_REG |= device_list.select;
	return(0);
}


//
// ide_set_partition() - This function set the active partition
//
int ide_set_partition(int partition)
{
	// Is this a valid partition ?
	if(partition > part_table->num_partitions)
	{
		// NOPE - return error
		return(IDE_INVALID_PARTITION);
	}

	// Set the partition number
	part_info = &part_table->partition[partition];

	// Return success
	return(0);
}


//
// ide_get_partition_size() - This function returns the size (in sectors)
// of the currently active partition.
//
unsigned long ide_get_partition_size(void)
{
	return(part_info->num_blocks);
}


//
// ide_get_partition_table() - This function returns a pointer to the
// partition table.
//
partition_table_t *ide_get_partition_table(void)
{
	return(part_table);
}

static int	piod_active_times[] = {
165,	// Mode 0
125,	// Mode 1
100,	// Mode 2
80,	// Mode 3
70,	// Mode 4
};

static int	pioc_active_times[] = {
290,	// Mode 0
290,	// Mode 1
290,	// Mode 2
80,	// Mode 3
70,	// Mode 4
};

static int	piod_active_clocks[] = {
0xf,	// 2 clks (60 ns)
0xf,	// 2 clks (60 ns)
0xf,	// 2 clks (60 ns)
0xe,	// 3 clks (90 ns)
0xd,	// 4 clks (120 ns)
0xc,	// 5 clks (150 ns)
0xb,	// 6 clks (180 ns)
0xa,	// 7 clks (210 ns)
0x9,	// 8 clks (240 ns)
0x8,	// 9 clks (270 ns)
0x7,	// 10 clks (300 ns)
0x6,	// 11 clks (330 ns)
0x5,	// 12 clks (360 ns)
0x4,	// 13 clks (390 ns)
0x3,	// 14 clks (420 ns)
0x2,	// 15 clks (450 ns)
0x1,	// 16 clks (480 ns)
0x0	// 17 clks (510 ns)
};

static int	piod_recovery_clocks[] = {
0xf0,	// 1 clks (30 ns)
0xf0,	// 1 clks (30 ns)
0xe0,	// 2 clks (60 ns)
0xd0,	// 3 clks (90 ns)
0xc0,	// 4 clks (120 ns)
0xb0,	// 5 clks (150 ns)
0xa0,	// 6 clks (180 ns)
0x90,	// 7 clks (210 ns)
0x80,	// 8 clks (240 ns)
0x70,	// 9 clks (270 ns)
0x60,	// 10 clks (300 ns)
0x50,	// 11 clks (330 ns)
0x40,	// 12 clks (360 ns)
0x30,	// 13 clks (390 ns)
0x20,	// 14 clks (420 ns)
0x10,	// 15 clks (450 ns)
0x00,	// 16 clks (480 ns)
};

static int	pioc_active_clocks[] = {
0xe,	// 3 clks (90 ns)
0xe,	// 3 clks (90 ns)
0xe,	// 3 clks (90 ns)
0xe,	// 3 clks (90 ns)
0xd,	// 4 clks (120 ns)
0xc,	// 5 clks (150 ns)
0xb,	// 6 clks (180 ns)
0xa,	// 7 clks (210 ns)
0x9,	// 8 clks (240 ns)
0x8,	// 9 clks (270 ns)
0x7,	// 10 clks (300 ns)
0x6,	// 11 clks (330 ns)
0x5,	// 12 clks (360 ns)
0x4,	// 13 clks (390 ns)
0x3,	// 14 clks (420 ns)
0x2,	// 15 clks (450 ns)
0x1,	// 16 clks (480 ns)
0x0,	// 17 clks (510 ns)
};

static int	pioc_recovery_clocks[] = {
0xf0,	// 3 clks (90 ns)
0xf0,	// 3 clks (90 ns)
0xf0,	// 3 clks (90 ns)
0xe0,	// 4 clks (120 ns)
0xd0,	// 5 clks (150 ns)
0xc0,	// 6 clks (180 ns)
0xb0,	// 7 clks (210 ns)
0xa0,	// 8 clks (240 ns)
0x90,	// 9 clks (270 ns)
0x80,	// 10 clks (300 ns)
0x70,	// 11 clks (330 ns)
0x60,	// 12 clks (360 ns)
0x50,	// 13 clks (390 ns)
0x40,	// 14 clks (420 ns)
0x30,	// 15 clks (450 ns)
0x20,	// 16 clks (480 ns)
0x10,	// 17 clks (510 ns)
0x00,	// 18 clks (540 ns)
};


static void NSC415_GetCurrentRegs(int devhand, NSC415_Regs *regptr, int getdma);
static void NSC415_SetRegValues(int devhand, NSC415_Regs *regptr);


//
// ide_init() - This function is used to initialize the ide disk drive
//
int ide_init(void)
{
	int			eer_rtc;
	unsigned int	tmp;
	NSC415_Regs	disk_controller_regs;
	int			recovery_time;
	int			active_time;
	int			pio_mode = 0;

#ifdef IDE_DEBUG
	dputs("IDE INIT\n");
#endif

	// Already initialized ?
	if(initialized)
	{
#ifdef IDE_DEBUG
		dputs("IDE DRIVE ALREADY INITIALIZED\n");
#endif
		// Return status
		return(device_list.status);
	}

	// Set up the pointer to the scatter gather table
	tmp = (unsigned int)&ex_stack & 0xffff;
	if((0x10000 - tmp) < (128 * sizeof(prd_entry_t)))
	{
		tmp = (unsigned int)&ex_stack + 0xffff;
		tmp &= ~0xffff;
	}
	else
	{
		tmp = (unsigned int)&ex_stack;
	}

	// Make sure pointer is aligned on a cache line boundary
	// I don't know why, but the controller seems to like this better
	while(tmp & 0x1f) tmp++;

	// Set the scatter-gather list pointer
	prd = (volatile prd_entry_t *)tmp;

	// Setup the controller
	setup_harddrive();

	// Initialize the control structure information
	device_list.select = 0;
	device_list.busy = 0;
	device_list.dma_timing = 0;
	device_list.pio_timing = 0;
	device_list.cyls = 0;
	device_list.heads = 0;
	device_list.sectors = 0;
	device_list.status = IDE_DEVICE_INVALID;	// assume invalid

	// Set the device
	ide_set_device(0);

	// Status == 0xff ?
	if(ALT_STATUS_REG == 0xff)
	{
		// Assume no drive connected
		return(device_list.status);
	}

	//kludge for quantum fireball ex firmware bug
	//the following drives function with this kludge:
	// - quantum fireball ex, el, se, tm
	// - maxtor 90340d2
	ide_soft_reset();

	//
	// Wait for drive to come out of powerup reset
	//
	eer_rtc = POWERUP_TIMEOUT;
	while((ALT_STATUS_REG & IDE_STB_BUSY) && eer_rtc--)
	{
		// Delay 1 tick before checking again
		prc_delay(0);
	}

	// Did powerup reset wait timeout ?
	if(!(ALT_STATUS_REG & IDE_STB_BUSY))
	{
		// NOPE - Wait for the drive to be not busy
		if(!ide_wait_bsy())
		{
#ifdef IDE_DEBUG
			dputs("IDE_DRIVE: - TIMEOUT WAITING FOR DRIVE BUSY BEFORE IDENTIFY COMMAND\n");
#endif
			return(device_list.status);
		}

		// Read the information from the drive
		if(!ide_identify((unsigned int *)ide_sector_buffer))
		{
			device_list.cyls         = ((DriveID *)ide_sector_buffer)->cyls;
			device_list.heads        = ((DriveID *)ide_sector_buffer)->heads;
			device_list.sectors      = ((DriveID *)ide_sector_buffer)->sectors;
			device_list.dma_timing   = ((DriveID *)ide_sector_buffer)->eide_dma_min;
			device_list.pio_timing   = ((DriveID *)ide_sector_buffer)->eide_pio;
			device_list.max_multsect = ((DriveID *)ide_sector_buffer)->max_multsect;

			// Set maximum sector number
			max_sector = (unsigned)device_list.cyls;
			max_sector *= (unsigned)device_list.heads;
			max_sector *= (unsigned)device_list.sectors;

			// Get the current configuration registers
			NSC415_GetCurrentRegs(PC87415_DEVICE_NUMBER, &disk_controller_regs, (int)FALSE);

			// Are advanced PIO modes supported ?
			if(((DriveID *)ide_sector_buffer)->field_valid & 2)
			{
				// YES - How about PIO Mode 4 ?
				if(((DriveID *)ide_sector_buffer)->eide_pio_modes & 2)
				{
					// YES - set it
					pio_mode = 4;
				}

				// PIO Mode 3 ?
				else if(((DriveID *)ide_sector_buffer)->eide_pio_modes & 1)
				{
					// Set it
					pio_mode = 3;
				}

				// Mode 3 or 4 not supported - get mode from other area
				else
				{
					pio_mode = ((DriveID *)ide_sector_buffer)->tPIO;
				}
			}

			// No advanced PIO modes - get mode from other area
			else
			{
				pio_mode = ((DriveID *)ide_sector_buffer)->tPIO;
			}
#ifdef IDE_DEBUG
			dputs("IDE DRIVE - PIO MODE: ");
			dphex(pio_mode);
			dputs("\n");
#endif

			// Get the minimum active time for this mode
			active_time = piod_active_times[pio_mode];
			active_time += 29;
			active_time /= 30;

			// Calculate the recovery time based on mode cycle time and mode
			// minimum active time
			recovery_time = ((DriveID *)ide_sector_buffer)->eide_pio_iordy - (((piod_active_clocks[active_time] ^ 0xf) * 30) + 60);
			recovery_time += 29;
			recovery_time /= 30;

			// Or 'em together to get value to put into the register
			active_time = (piod_active_clocks[active_time] | piod_recovery_clocks[recovery_time]);

			// Set the data read and write cycle times
			disk_controller_regs.nsc_ConfigRegs.n415_C1D1_Dread = active_time;
			disk_controller_regs.nsc_ConfigRegs.n415_C1D1_Dwrite = active_time;

			// Get the minimum command active time for this PIO mode
			active_time = pioc_active_times[pio_mode];
			active_time += 29;
			active_time /= 30;

			// Calculate the recovery time based on mode cycle time and mode
			// minimum active time
			recovery_time = ((DriveID *)ide_sector_buffer)->eide_pio_iordy - (((pioc_active_clocks[active_time] ^ 0xf) * 30) + 60);
			recovery_time += 29;
			recovery_time /= 30;

			// Or 'em together to get value to put into the register
			active_time = (pioc_active_clocks[active_time] | pioc_recovery_clocks[recovery_time]);

			// Set the command read and write cycle times
			disk_controller_regs.nsc_ConfigRegs.n415_CmdCtrl_RdWrt = active_time;

			// Set the registers
			NSC415_SetRegValues(PC87415_DEVICE_NUMBER, &disk_controller_regs);

			if(!ide_wait_bsy())
			{
#ifdef IDE_DEBUG
				dputs("IDE DRIVE - TIMEOUT WAITING FOR BUSY BEFORE SETTING PIO MODE\n");
#endif
				return(device_list.status);
			}
#ifdef IDE_DEBUG
			dputs("IDE DRIVE - ISSUING PIO MODE COMMAND\n");
#endif
			PRECOMP_ERROR_REG = 0x3;
			SECTOR_COUNT_REG = 0xb;
			DRIVE_HEAD_REG = 0xa0;
			COMMAND_REG = IDE_CMD_BUFFERMODE;

			// Enable multi-sector transfers
			if(!ide_wait_bsy())
			{
#ifdef IDE_DEBUG
				dputs("IDE DRIVE - TIMEOUT WAITING FOR BUSY BEFORE SETTING MULTI-SECTOR TRANSFER SIZE\n");
#endif
				return(device_list.status);
			}
#ifdef IDE_DEBUG
			dputs("IDE DRIVE - ISSUING MULTI-SECTOR TRANSFER SIZE COMMAND\n");
#endif
			SECTOR_COUNT_REG = device_list.max_multsect;
			DRIVE_HEAD_REG = 0xa0;
			COMMAND_REG = IDE_CMD_MULTIMODE;

#ifdef IDE_DEBUG
			dputs("IDE_DRIVE - READING PARTITION TABLE\n");
#endif
			// Read the partition table
			eer_rtc = PART_READ_TIMEOUT;
			while(ide_hread_sectors((unsigned int *)partition_buffer, 0, 0, 1, 1) && --eer_rtc)
			{
				prc_delay(0);
			}

			// Did we read the partition table OK
			if(!eer_rtc)
			{
#ifdef IDE_DEBUG
				dputs("IDE_DRIVE - TIMEOUT READING PARTITION TABLE\n");
#endif
				return(device_list.status);
			}

			// Fill in the information for partition 0 (RAW PARTITION)
			part_table->partition[0].partition_type = RAW_PARTITION;
			part_table->partition[0].num_blocks = device_list.cyls *
				device_list.heads *
				device_list.sectors;
			part_table->partition[0].starting_block = 0;

			// Set device is connected status
			device_list.status = IDE_DEVICE_CONNECTED;

			// Is partition table valid ?
			if(part_table->magic_number != PART_MAGIC)
			{
#ifdef IDE_DEBUG
				dputs("IDE_DRIVE - INVALID PARTITION TABLE\n");
#endif
				// NOPE - Set number of partitions
				part_table->num_partitions = 1;

				// Set magic number
				part_table->magic_number = PART_MAGIC;

				// Set the active partition
				ide_set_partition(0);
			}
			else
			{
				// Set the active partition to the first user partition
				ide_set_partition(1);
			}

			// Initialize the queue pointers
			queue.head = queue.tail = 0;

			// Make sure no interrupts are pending
			(void)STATUS_REG;

#if (PHOENIX_SYS & SA1)
			// Enable IDE interrupt on IP4
			*((volatile int *)ICPLD_INT_MAPB_REG) |= (0 << 8);
			*((volatile int *)ICPLD_INT_ENBL_REG) |= (1 << 12);
#endif
#ifdef IDE_DEBUG
			dputs("IDE_DRIVE - ENABLING DISK INTERRUPTS\n");
#endif

			// Enable the IDE Disk interrupt
			enable_ip(IDE_DISK_INT);

		}
#ifdef IDE_DEBUG
		else
		{
			dputs("IDE DRIVE - IDENDTIFY COMMAND FAILURE\n");
		}
	}
	else
	{
		dputs("IDE DRIVE - TIMEOUT WAITING FOR POWER UP RESET\n");
#endif
	}

	// Initialize the disk caches
	init_disk_cache();

	// Give a little extra time before exiting
	prc_delay(0);

#ifdef IDE_DEBUG
	dputs("IDE INIT - entry\n");
#endif

	ide_drive_status = device_list.status;

	// Return the drive status
	return(device_list.status);
}


//
// ide_reset() - This function is used to perform a software reset of the
// ide drive.
//
int ide_reset(void)
{
	return(ide_soft_reset());
}


//
// ide_check_devstat() - This function is used to determin whether or not a
// drive is actually connected.
//
int ide_check_devstat(void)
{
	unsigned char	alt_status;
	int				eer_rtc = 0;

	// Loop waiting for status of ! 0xff and ! 0x00 or timeout (whichever
	// comes first).
	do
	{
		// Read the alternate status register
		alt_status = ALT_STATUS_REG;
 
		// Is status NOT 0xff or 0x00
		if((alt_status != 0xff) && (alt_status != 0x00))
		{
			// YES - return connected
			return(IDE_DEVICE_CONNECTED);
		}

		prc_delay(0);

	} while(eer_rtc++ < BUSY_TIMEOUT);

	// Timed out - return NOT connected
	return(IDE_DEVICE_INVALID);
}


//
// ide_hread_sectors() - This function is used to read the number of sectors
// specified by the count argument from the head, cylinder, and sector
// arguments into the buffer specified by the rdbuf argument.
//
static int ide_hread_sectors(unsigned int *rdbuf, int head, int cylinder, int sector, int count)
{
	register int	nloops;

	// Wait for drive to be NOT busy
	if(!ide_wait_bsy())
	{
#ifdef IDE_DEBUG
		dputs("ide_hread_sectors() - TIMEOUT WAITING FOR BUSY\n");
#endif
		return(1);
	}

	// Setup the command to read sectors
	DRIVE_HEAD_REG = (unsigned char)(device_list.select|DRIVE_HEAD_INFO|(head & 0x000f));
	LOW_CYLINDER_REG = (unsigned char)(cylinder & 0x00ff);
	HIGH_CYLINDER_REG = (unsigned char)((cylinder >> 8) & 0x00ff);
	SECTOR_NUM_REG = (unsigned char)sector;
	SECTOR_COUNT_REG = count;

	// Send the read sectors command
	COMMAND_REG = IDE_CMD_SREAD;

	while(count--)
	{
		// Wait for the busy bit to tell use its ok to read the data
		if(!ide_wait_bsy())
		{
#ifdef IDE_DEBUG
			dputs("ide_hread_sectors() - TIMEOUT WAITING FOR DATA\n");
#endif
			(void)STATUS_REG;
			return(1);
		}

		// Acknowledge the interrupt
		(void)STATUS_REG;

		// Read a sector of data
		nloops = (LONGS_PER_SECTOR / 16);
		while(nloops--)
		{
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
			*rdbuf++ = DATA_REG;
		}

		// If an error occurred
		if(ALT_STATUS_REG & IDE_STB_ERROR)
		{
#ifdef IDE_DEBUG
			dputs("ide_hread_sectors() - DATA ERROR\n");
#endif
			// Return the error
			return(PRECOMP_ERROR_REG);
		}
	}

	// Return success
	return(0);
}



//
// ide_hread_data() - This function is used to read 1 sectors worth of data
// into the buffer specified by the rdbuf argument passed to it.
//
// NOTE - This is a leaf function and is NOT allowed to call other functions
//
static void ide_hread_data(register unsigned int *rdbuf)
{
	register int				nloops asm ("$8") = 8;
	register	unsigned int	*buf asm("$9") = rdbuf;

	// Read 1 sectors worth of data
	while(nloops--)
	{
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
		*buf++ = DATA_REG;
	}
}


//
// ide_identify() - This function is used to read the drive information
//
int ide_identify(unsigned int *rdbuf)
{
	int 		tmp;
	int 		i;
	DriveID	*id;
	int		eer_rtc;

	// Don't do anything if there's no device!
	if(ide_check_devstat() == IDE_DEVICE_INVALID)
	{
#ifdef IDE_DEBUG
		dputs("ide_identify() - DEVICE INVALID\n");
#endif
		return(IDE_ERB_ABORTCMD);
	}

	// Send the command to get the drive data
	COMMAND_REG = IDE_CMD_IDENTIFY;

	// Wait for the drive to indicate data is waiting to be read
	eer_rtc = DREQ_TIMEOUT;
	while(((ALT_STATUS_REG & IDE_STB_DATAREQ) != IDE_STB_DATAREQ) && --eer_rtc)
	{
		prc_delay(0);
	}

	// Read Id information if we did NOT timeout
	if(eer_rtc)
	{
		// acknowledge the HD interrupt and read the data
		(void)STATUS_REG;

		// Read the data
		ide_hread_data(rdbuf);

		// Set pointer to data read
		id = (DriveID *)rdbuf;

		// Swap bytes in the serial number field
		for(i = 0; i < sizeof(id->serial_no); i += 2)
		{
			tmp = id->serial_no[i];
			id->serial_no[i] = id->serial_no[i+1];
			id->serial_no[i+1] = tmp;
		}

		// Swap bytes in the model field
		for(i = 0; i < sizeof(id->model); i += 2)
		{
			tmp = id->model[i];
			id->model[i] = id->model[i+1];
			id->model[i+1] = tmp;
		}
	}
#ifdef IDE_DEBUG
	else
	{
		dputs("ide_identify() - TIMEOUT WAITING FOR IDENTIFY DATA\n");
	}
#endif

	// Error ?
	if(ALT_STATUS_REG & IDE_STB_ERROR)
	{
#ifdef IDE_DEBUG
		dputs("ide_identify() - ERROR IN IDENTIFY DATA\n");
#endif

		// YES - return it
		return((int)PRECOMP_ERROR_REG);
	}

	// Return success
	return(0);
}


//
// ide_get_hdinfo() - This function is used to get the number of heads,
// sectors, and cylinders for the drive.
//
int ide_get_hdinfo(unsigned short *nheads, unsigned short *ncylinders, unsigned short *nsectors)
{
	int status = IDE_DEVICE_INVALID;	

	// initialize hard drive if it hasn't been done yet
	if(ide_init() == IDE_DEVICE_CONNECTED)
	{
		// Initialized OK ?
		if((status = ide_check_devstat()) == IDE_DEVICE_CONNECTED)
		{
			// YES - Set the values
			*nheads     = device_list.heads;
			*ncylinders = device_list.cyls;
			*nsectors   = device_list.sectors;
		}
	}

	// Return the status
	return(status);
}


//
// ide_write_sectors() - This function is used to write sector data to the
// drive.
//
static int ide_write_sectors(unsigned int *wrbuf, int head, int cylinder, int sector, int count)
{
	int	nloops;
	int	eer_rtc;
	int	old_vec;

	// Don't do anything if there's no device!
	if(ide_check_devstat() == IDE_DEVICE_INVALID)
	{
		return(IDE_ERB_ABORTCMD);
	}

	// Disable the debugger interrupt
	old_vec = disable_ip(SCSI_INT);

	while(count-- > 0)
	{
		// Wait for drive to indicate it's NOT busy
		if(!ide_wait_bsy())
		{
#ifdef IDE_DEBUG
			dputs("ide_write_sectors() - TIMEOUT WAITING FOR BUSY\n");
#endif
			return(IDE_ERB_ABORTCMD);
		}

		// Set up the write sector command
		DRIVE_HEAD_REG = (unsigned char)(device_list.select|(head & 0x000f));
		LOW_CYLINDER_REG = (unsigned char)(cylinder & 0x00ff);
		HIGH_CYLINDER_REG = (unsigned char)((cylinder >> 8) & 0x00ff);
		SECTOR_NUM_REG = (unsigned char)sector;
		SECTOR_COUNT_REG = 1;

		// Issue the command
		COMMAND_REG = IDE_CMD_SWRITE;

		// Wait for the drive to indicate data is ready to transfer
		if(!ide_wait_bsy())
		{
#ifdef IDE_DEBUG
			dputs("ide_write_sectors() - TIMEOUT WAITING TO WRITE DATA\n");
#endif
			return(IDE_ERB_ABORTCMD);
		}

		// Error occur ?
		if((ALT_STATUS_REG & IDE_STB_ERROR) == 0)
		{
			// NOPE - Make sure data is available
			eer_rtc = DREQ_TIMEOUT;
			while(!(ALT_STATUS_REG & IDE_STB_DATAREQ) && --eer_rtc)
			{
				prc_delay(0);
			}

			// Timeout ?
			if(!eer_rtc)
			{
				// YES - quit
				break;
			}

			// Write 1 sectors worth of data
			nloops = LONGS_PER_SECTOR / 16;
			while(nloops--)
			{
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
				DATA_REG = *wrbuf++;
			}

			// Wait until write is done then acknowledge HD interrupt
			if(!ide_wait_bsy())
			{
#ifdef IDE_DEBUG
				dputs("ide_write_sectors() - TIMEOUT WAITING TO WRITE NEXT SECTOR\n");
#endif
				(void)STATUS_REG;
				return(IDE_ERB_ABORTCMD);
			}

			// Acknowledge the interrupt
			(void)STATUS_REG;

			// Calculate next cylinder, head, and sector to transfer
			++sector;
			if(sector > device_list.sectors)
			{
				sector = 1;
				++head;
				if(head >= device_list.heads)
				{
					head = 0;
					++cylinder;
					if(cylinder >= device_list.cyls)
					{
						break;
					}
				}
			}
		}
	}

	// Re-enable the debugger interrupt if it was already on
	if(old_vec & SCSI_INT)
	{
		enable_ip((old_vec | SCSI_INT));
	}

	// Are we really done transferring data ?
	if(ALT_STATUS_REG & IDE_STB_DATAREQ)
	{
		// NOPE - Error
		return(0xff);
	}

	// Was there some other sort of error ?
	if(ALT_STATUS_REG & IDE_STB_ERROR)
	{
		// YES - return it
		return(PRECOMP_ERROR_REG);
	}

	// Return sucess
	return 0;
}


//
// ide_wait_busy() - This function is used to wait for the drive to be NOT
// busy or timeout, whichever come first.
//
static int ide_wait_bsy(void)
{
	int	eer_rtc = (BUSY_TIMEOUT * 500);

	while((ALT_STATUS_REG & IDE_STB_BUSY) && --eer_rtc) ;

	return(eer_rtc);
}

static int ide_wait_drdy(void)
{
	int	eer_rtc = (BUSY_TIMEOUT * 500);

	while(!(ALT_STATUS_REG & IDE_STB_READY) && --eer_rtc) ;

	return(eer_rtc);
}

//
// ide_soft_reset() - This function is used to software reset the drive.
//
static int ide_soft_reset(void)
{
	int	timer;

	// Set the reset bit until timeout or no master aborts
	timer = RESET_TIMEOUT;
	do
	{
		DEVICE_CONTROL_REG = 0x04;
	} while(--timer);

	// Hold reset for 1 second
	prc_delay(60);

	// Reset the reset bit until timeout or no master abort
	timer = RESET_TIMEOUT;
	do
	{
		DEVICE_CONTROL_REG = 0;
	} while(--timer);

	// Wait for the drive to say it is NOT busy or timeout, whichever comes
	// first
	timer = RESET_TIMEOUT;
	while((ALT_STATUS_REG & IDE_STB_BUSY) && --timer)
	{
		// Wait 1 tick
		prc_delay(0);
	}

	// Continue only if we did NOT timeout
	if(timer)
	{
		// Wait for the drive to say it is ready or timeout, whichever comes
		// first.
		while(!(ALT_STATUS_REG & IDE_STB_READY) && --timer)
		{
			// Wait 1 tick
			prc_delay(0);
		}
	}

	// Did we NOT timeout
	if(timer)
	{
		// YES - return sucess
		return(0);
	}

	// Return fail
	return(1);
}

//
// SecReads() - This function is used to read sectors from the drive using
// logical block addressing mode.
//
volatile int	disk_int = 0;

short _SecReads(unsigned long sec, unsigned long *buf, unsigned long num)
{
	register int	num_2_read;

	// In the debugger stub ?
	if(!in_debugger)
	{
#ifdef IDE_DEBUG
		dputs("_SecReads - queued\n");
#endif
		// NOPE - que the transfer
		if(que_trans(sec, buf, num, 0))
		{
			// ERROR
			return(-1);
		}

		// NO ERROR
		return(num);
	}

#ifdef IDE_DEBUG
	dputs("_SecReads - non-queued\n");
#endif
	while(num)
	{
		// Figure out number to read
		num_2_read = num;
		if(num_2_read > device_list.max_multsect)
		{
			num_2_read = device_list.max_multsect;
		}

		// Wait for drive to be NOT busy
		if(!ide_wait_bsy())
		{
#ifdef IDE_DEBUG
			dputs("_SecReads() - TIMEOUT WAITING FOR BUSY\n");
#endif
			return(0);
		}

		// Setup the command to read sectors
		DRIVE_HEAD_REG = (unsigned char)(0x40|device_list.select|DRIVE_HEAD_INFO|((sec >> 24) & 0xf));
		LOW_CYLINDER_REG = (unsigned char)((sec >> 8) & 0xff);
		HIGH_CYLINDER_REG = (unsigned char)((sec >> 16) & 0xff);
		SECTOR_NUM_REG = (unsigned char)(sec & 0xff);
		SECTOR_COUNT_REG = num_2_read;

		// Send the read sectors command
		COMMAND_REG = IDE_CMD_MREAD;

		// Decrement the number to transfer
		num -= num_2_read;

		// Increment the sector number
		sec += num_2_read;

		// Figure out how many loops to do
		num_2_read *= (LONGS_PER_SECTOR / 16);

		// Wait for the busy bit to tell use its ok to read the data
		if(!ide_wait_bsy())
		{
#ifdef IDE_DEBUG
			dputs("_SecReads() - TIMEOUT WAITING FOR DATA\n");
#endif
			(void)STATUS_REG;
			return(0);
		}
	
		// Acknowledge the interrupt
		(void)STATUS_REG;

		while(num_2_read--)
		{
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
			*buf++ = DATA_REG;
		}

		// If an error occurred
		if(ALT_STATUS_REG & IDE_STB_ERROR)
		{
#ifdef IDE_DEBUG
			dputs("_SecReads() - DATA ERROR\n");
#endif
			// Return the error
			return(0);
		}
	}

	// Return success
	return(1);
}

short disk_cache_read(int sec, unsigned long *buf, int num);


short SecReads(unsigned long sec, unsigned long *buf, unsigned long num)
{
	// Check to make sure we are accessing valid blocks in the partition
	if((sec + num) >= part_info->num_blocks)
	{
		return(-1);
	}

	// Offset the sector by the starting block of the active partition
	sec += part_info->starting_block;

	// Read from the disk cache.  If a miss occurs, _SecReads (below) gets
	// called to satisfy the request.
	if(!in_debugger)
	{
		return(disk_cache_read(sec, buf, num));
	}
	return(_SecReads(sec, buf, num));
}


//
// SecWrites() - This function is used to write sectors to the drive using
// logical block addressing mode.
//
short _SecWrites(unsigned long sec, unsigned long *buf, unsigned long num)
{
	int	head;
	int	cylinder;
	int	sector;

	// In debugger stub
	if(!in_debugger)
	{
		// NO - que the request
		if(que_trans(sec, buf, num, 1))
		{
			return(-1);
		}
		return(num);
	}

	// Calculate the actual sector, head, and cylinder to use
	sector = sec % device_list.sectors;
	sector++;
	head = (sec / device_list.sectors) % device_list.heads;
	cylinder = sec / (device_list.sectors * device_list.heads);

	// Write the requested sectors
	ide_write_sectors((unsigned int *)buf, head, cylinder, sector, num);

	// Return success
	return(1);
}

short disk_cache_write(int sec, unsigned long *buf, int num);

short SecWrites(unsigned long sec, unsigned long *buf, unsigned long num)
{
	// Check to make sure we are accessing valid blocks in the partition
	if((sec + num) >= part_info->num_blocks)
	{
		return(-1);
	}

	// Offset the sector by the starting block of the active partition
	sec += part_info->starting_block;

	// Make sure we maintain coherency of the disk cache
	if(!in_debugger)
	{
		return(disk_cache_write(sec, buf, num));
	}
	return(_SecWrites(sec, buf, num));
}


//
// NSC415_GetCurrentRegs() - This function retrieves the values of all of
// the configuration registers for the NSC87415 PCI IDE controller.
//
static void NSC415_GetCurrentRegs(int devhand, NSC415_Regs *regptr, int getdma)
{
	unsigned int	i;
	unsigned char	*dbptr;
	unsigned short	baseadr;
	unsigned long	tmp;

	dbptr = (unsigned char *)&(regptr->nsc_ConfigRegs);

	for(i = 0; i < sizeof(N415_CfigRegs); ++i, ++dbptr)
	{
		*dbptr = PCI_ReadConfigByte(devhand, (unsigned char)i);
	}

	if(getdma)
	{
		// BAR4 always reads back with bit 0 set
		baseadr = ((unsigned short)regptr->nsc_ConfigRegs.n415_BaseAddr4) & 0xfffe;

		dbptr = (unsigned char *)&regptr->nsc_BusMastRegs;

		for(i = 0; i < sizeof(N415_MastRegs); ++i, ++dbptr, ++baseadr)
		{
			tmp =  *dbptr = (*(int *)((baseadr + IDE_BASE_ADDRESS) & 0xfffffffc));
			*dbptr = (unsigned char)(tmp >> (8 * ((baseadr + IDE_BASE_ADDRESS) & 0x03)));
		}
	}
}


//
// NSC415_SetRegValues() - This function sets all of the configuration
// registers for the NSC87415 PCI IDE controller.
//
static void NSC415_SetRegValues(int devhand, NSC415_Regs *regptr)
{
	N415_CfigRegs	*figptr;

	figptr = &regptr->nsc_ConfigRegs;

	PCI_WriteConfigWord(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_Command) - (unsigned long)figptr),
		figptr->n415_Command);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_ProgIface) - (unsigned long)figptr),
		figptr->n415_ProgIface);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_Latency) - (unsigned long)figptr),
		figptr->n415_Latency);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_Control[0]) - (unsigned long)figptr),
		figptr->n415_Control[0]);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_Control[1]) - (unsigned long)figptr),
		figptr->n415_Control[1]);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_Control[2]) - (unsigned long)figptr),
		figptr->n415_Control[2]);

	PCI_WriteConfigDword(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_BaseAddr0)  - (unsigned long)figptr),
		figptr->n415_BaseAddr0);
	PCI_WriteConfigDword(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_BaseAddr1)  - (unsigned long)figptr),
		figptr->n415_BaseAddr1);
	PCI_WriteConfigDword(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_BaseAddr2)  - (unsigned long)figptr),
		figptr->n415_BaseAddr2);
	PCI_WriteConfigDword(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_BaseAddr3)  - (unsigned long)figptr),
		figptr->n415_BaseAddr3);
	PCI_WriteConfigDword(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_BaseAddr4)  - (unsigned long)figptr),
		figptr->n415_BaseAddr4);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C1D1_Dread) - (unsigned long)figptr),
		figptr->n415_C1D1_Dread);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C1D1_Dwrite) - (unsigned long)figptr),
		figptr->n415_C1D1_Dwrite);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C1D2_Dread) - (unsigned long)figptr),
		figptr->n415_C1D2_Dread);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C1D2_Dwrite) - (unsigned long)figptr),
		figptr->n415_C1D2_Dwrite);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C2D1_Dread) - (unsigned long)figptr),
		figptr->n415_C2D1_Dread);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C2D1_Dwrite) - (unsigned long)figptr),
		figptr->n415_C2D1_Dwrite);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C2D2_Dread) - (unsigned long)figptr),
		figptr->n415_C2D2_Dread);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_C2D2_Dwrite) - (unsigned long)figptr),
		figptr->n415_C2D2_Dwrite);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_CmdCtrl_RdWrt) - (unsigned long)figptr),
		figptr->n415_CmdCtrl_RdWrt);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->n415_SectorSize) - (unsigned long)figptr),
		figptr->n415_SectorSize);
}


//
// NSC415_InitRegValues() - This function initializes the configuration
// registers for the NSC87415 PCI IDE controller.
//
static void NSC415_InitRegValues(NSC415_Regs *regptr)
{
	N415_CfigRegs	*nptr;
	N415_MastRegs	*bptr;

	nptr = &regptr->nsc_ConfigRegs;

	nptr->n415_Command = 0x105;	// enable bus master and sys error check
	nptr->n415_SectorSize = 0xee;		// sector size = 512 bytes

	nptr->n415_ProgIface   =  0x8a;	// enable master IDE and legacy mode
												// map both ints to INTA, turn on PWR,
												// turn off IDE resets
	// Legacy mode, soft resets off, power on, INTA unmasked, no-write to vend-id
	nptr->n415_Control[0] = 0x30;

	nptr->n415_Control[1] &= ~0x0b;	// disable data phase watchdog, unmask both interrupts
	nptr->n415_Control[1] |=  0x03;	// mask both interrupts


	nptr->n415_ProgIface  &= ~0x01;	// use legacy mode, not BAR 0,1
	nptr->n415_Control[1] &= ~0x48;	// map IDE to BAR 0,1, disable watchdog
	// Chan 1 int unmasked, buffer BAR 0,1 accesses
	nptr->n415_Control[1] = 0x12;

	nptr->n415_Control[2] |=  0x01;	// enable buffers for first channel
	nptr->n415_BaseAddr0 = PHYS(IDE_BASE_ADDRESS) + 0x400;
	nptr->n415_BaseAddr1 = PHYS(IDE_BASE_ADDRESS) + 0x408;

	nptr->n415_Control[2] &= ~0x10;		// use IORDY for first drive
	nptr->n415_C1D1_Dread  = M0_TIMING;	// use mode 0 timings
	nptr->n415_C1D1_Dwrite = M0_TIMING;
	nptr->n415_CmdCtrl_RdWrt = CC_TIMING;

	nptr->n415_BaseAddr4 = PHYS(IDE_BASE_ADDRESS) + NSC415_DEFVAL_BAR4;

	bptr = &regptr->nsc_BusMastRegs;

	bptr->n415_Mast1_Cmd  = 0x00;			// stop any DMA transfers
	bptr->n415_Mast1_Stat = 0x06;			// reset error/interrupts

	bptr->n415_Mast2_Cmd  = 0x00;			// stop any DMA transfers
	bptr->n415_Mast2_Stat = 0x06;			// reset error/interrupts
}


//
// setup_harddrive() - This function sets up the NSC87415 PCI IDE controller
//
static void setup_harddrive(void)
{
	NSC415_Regs	disk_controller_regs;

	// Get configuration registers
	NSC415_GetCurrentRegs(PC87415_DEVICE_NUMBER, &disk_controller_regs, (int)FALSE);

	// Initialize the configuration registers
	NSC415_InitRegValues(&disk_controller_regs);

	// Write configuration registers
	NSC415_SetRegValues(PC87415_DEVICE_NUMBER, &disk_controller_regs);
}



//
// PCI_ReadConfigByte() - This function reads a byte from a configuration
// register.
//
static unsigned char PCI_ReadConfigByte(int devhand, unsigned char regaddr)
{
	int	i;

	i = get_pci_config_reg(devhand, (regaddr >> 2));
	i = i >> (8 * (regaddr & 0x03));
	i = i & 0xff;
	return((unsigned char)i);
}


//
// PCI_WriteConfigByte() - This function writes a byte to a configuration
// register.
//
static void PCI_WriteConfigByte(int devhand, unsigned char regaddr, unsigned char regval)
{
	int	i;
	int	j;
	int	mask;

	i = get_pci_config_reg(devhand, (regaddr >> 2));
	j = (int) (regval << (8 * (regaddr & 0x03)));
	mask = ~(0xff << (8 * (regaddr & 0x03)));
	i = (i & mask) | j;
	put_pci_config_reg(devhand, (regaddr>>2), i);
}


//
// PCI_WriteConfigWord() - This function writes a short to a configuration
// register.
//
static void PCI_WriteConfigWord(int devhand, unsigned char regaddr, unsigned short regval)
{
	int	i;
	int	j;
	int	mask;

	i = get_pci_config_reg(devhand, (regaddr >> 2));
	j = (int) (regval << (16 * (regaddr & 0x02)));
	mask = ~(0xffff << (16 * (regaddr & 0x02)));
	i = (i & mask) | j;
	put_pci_config_reg(devhand, (regaddr >> 2), i);
}


//
// PCI_WriteConfigDword() - This function writes a long to a configuration
// register.
//
static void PCI_WriteConfigDword(int devhand, unsigned char regaddr, unsigned long regval)
{
	put_pci_config_reg(devhand, regaddr>>2, regval);
}

__asm__("
	.set	noreorder
	.globl	__get_sr
__get_sr:
	jr		$31
	mfc0	$2,$12
	.set	reorder
");

__asm__("
	.set	noreorder
	.globl	__get_cause
__get_cause:
	jr	$31
	mfc0	$2,$13
	.set	reorder
");

int	__get_sr(void);
int	__get_cause(void);
void	ide_intr(void);


/****************************************************************************/
/*                                                                          */
/*                                                                          */
/* BEGINNING OF THE INTERRUPT DRIVEN DISK READ CODE (UNTESTED SO FAR)       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
// This function gets called whenever a read or write disk request is made.
// This function simply queues the requests onto a ring buffer (FIFO) and is
// serviced by the interrupt handler.
//
// Add a read request to the request queue
//
static int add_queue(volatile struct queue_data *qdata)
{
	register int	head;
	register int	is_not_empty;
	register void	*old_callback;

	// If the request is a write - block until all previous transactions have
	// finished
	if(qdata->request != 0)
	{
		// Has this been called while we are in an interrupt service routine ?
		if(__get_sr() & 2)
		{
			// YES - Then we must NOT simply block here because we will never
			// see the interrupt from the disk.  We must service the disk
			// interrupts manually at this point.

			old_callback = callback;
			callback = (void *)0;
			while(queue.head != queue.tail)
			{
				// Wait for the cause register to tell us that the disk interrupt
				// is active
				while(!(__get_cause() & IDE_DISK_INT)) ;

				// Once active - manually service the interrupt
				ide_intr();

				// Make sure the interrupt went inactive
				while((__get_cause() & IDE_DISK_INT)) ;
			}
			callback = old_callback;
		}
		else
		{
			// Wait for the queue to drain
			while(queue.head != queue.tail) ;
		}
	}

	// Disallow interrupts from the disk
	disable_ip(IDE_DISK_INT);

	// Get status of queue
	is_not_empty = queue.head - queue.tail;

	// If we are NOT in an interrupt service routine AND the queue is full
	// then we block here until at least 1 queue slot becomes available
	if(!(__get_sr() & 2))
	{
		// Wait here if queue is full
		do
		{
			head = queue.head + 1 >= sizeof(queue.qd)/sizeof(void *) ? 0 : queue.head + 1;
		} while(head == queue.tail);
	}
	else
	{
		// Save the callback function pointer
		old_callback = callback;
		callback = (void *)0;

		// Figure out how many are available
		head = queue.head + 1 >= sizeof(queue.qd)/sizeof(void *) ? 0 : queue.head + 1;

		// Wait here if queue is full
		while(head == queue.tail)
		{
			// Wait for the cause register to tell us that the disk interrupt
			// is active
			while(!(__get_cause() & IDE_DISK_INT)) ;

			// Once active - manually service the interrupt
			ide_intr();

			// Make sure the interrupt went inactive
			while((__get_cause() & IDE_DISK_INT)) ;

			// Figure out how many are available
			head = queue.head + 1 >= sizeof(queue.qd)/sizeof(void *) ? 0 : queue.head + 1;
		}

		// Reset the callback function
		callback = old_callback;
	}

	// Add request to queue
	queue.qd[queue.head] = qdata;

	// Increment the head index
	queue.head = head;

	// Return the status of queue before the addition
	return(is_not_empty);
}

//
// que_trans() - This function is called by _SecReads and _SecWrites.
//
static short que_trans(unsigned long sec, unsigned long *buf, unsigned long num, int type)
{
	volatile struct queue_data	*qdata;
	unsigned long	timeout;

	// Is the sector number in range ?
	if(sec >= max_sector)
	{
		// NO - Is there a callback function ?
		if(callback)
		{
			// YES - Call it
			callback(num);
		}

		// Return error
		return((short)num);
	}

	// Attempt to transfer beyond end of drive ?
	if((sec + num) >= max_sector)
	{
		// YES - Is there a callback function ?
		if(callback)
		{
			// YES - Call it
			callback(num);
		}

		// Return error
		return((short)num);
	}

	// Initialize the data in the qdata structure
	qdata = &qd[queue.head];
	qdata->sec = sec;
	qdata->buf = buf;
	qdata->num = num;
	qdata->request = type;
	qdata->retry_count = 0;

	// Is buffer in a cached segment ?
	if(!((int)qdata->buf & 0x20000000))
	{
		// YES - Writeback and invalidate the data cache so processor flushes
		// the data caches to memory
		writeback_cache();
	}

	// Has application installed pointers ?
	if(proc)
	{
#ifdef IDE_DEBUG
		dputs("que_trans() - Processes issued transfer\n");
#endif
		// YES - Get process pointer
		qdata->proc = *proc;
	}

	// No application installed pointers
	else
	{
		// Set process pointer to NULL
		qdata->proc = 0;
	}

	// Add the request to the queue.  If add_queue returns 0 it means the
	// queue was empty and the command needs to be sent to the disk to start
	// the transfer.  The disk interrupt handler guarentees that no interrupts
	// are pending (or possible) if the queue is empty.
#ifdef IDE_DEBUG
	dputs("Adding request to queue\n");
#endif
	if(!add_queue(qdata))
	{
#ifdef IDE_DEBUG
		dputs("Issuing queued request\n");
#endif
		// Send the read command to the disk to start the whole works
		// Did it fail ?
		if(!queued_req(qdata))
		{
			// YES - flush the disk queue because at if we got here we
			// know the que was empty when the request was added
			reset_disk_que();

			// If a callback function is installed at this point -- call it
			// so it knows about the error
			if(callback)
			{
				callback(num);
			}

			// Return error
			return(num);
		}
	}

	// If we get here it means that the request was properly queued and IF
	// it was started, it was started without error

	// Has the application installed the pointers yet ?
	if(proc)
	{
		// YES - Is a process making the read request ?
		if(*proc)
		{
			// YES - Are there requests in the queue ?
			if(queue.head == queue.tail)
			{
				// NOPE - Return to the calling process
				return(0);
			}

			// YES - Suspend the process (this is GOOSE suspend_self())
			// Turn ON the GREEN LED
			suspend();
		}

		// Not a process
		else
		{
			// Has a callback function been installed ?
			if(callback)
			{
				// YES - return (interrupt calls function)
				return(0);
			}

			// No callback function - wait here for the interrupt
			timeout = 100000000;
			while(!(qdata->proc) && timeout)
			{
				--timeout;
			}

			// Did we timeout ?
			if(!timeout)
			{
				// YES - is there a user installed audit function ?
				if(audit)
				{
					// YES - call it
					audit();
				}

				// Show something on debug terminal
				dputs("Disk timeout\n");

				// Flush the disk caches
				flush_disk_cache();
			}
		}
	}

	// No pointers yet so just wait for interrupt
	else
	{
#ifdef IDE_DEBUG
		dputs("DONE");
#endif
		// Has a callback function been installed ?
		if(callback)
		{
#ifdef IDE_DEBUG
			dputs("Callback function installed - returning\n");
#endif
			// YES - return (interrupt calls function)
			return(0);
		}

#ifdef IDE_DEBUG
		dputs("No Callback - waiting for interrupt\n");
#endif
		// No callback function - wait here for the interrupt
		timeout = 100000000;
		while(!(qdata->proc) && timeout && qdata->retry_count < 4)
		{
			--timeout;
		}

		// Did we timeout ?
		if(!timeout)
		{
			// YES - Is there a user installed audit function ?
			if(audit)
			{
				// YES - Call it
				audit();
			}

			// Show something on the debug terminal
			dputs("Disk timeout\n");

			// Flush the disk caches
			flush_disk_cache();
		}
#ifdef IDE_DEBUG
		dputs("Interrupt received\n");
#endif
	}

	// In the case of no pointers installed or read request being made by
	// a non-process, we get here when the proc pointer field of the queue
	// data structure gets set to non-null by the interrupt handler.
	// 
	// In the case of a process making the read request, we get here when
	// the process is resumed by the disk interrupt handler.  We return the
	// number of sectors NOT transferred to the caller.

	// Return number of sectors NOT transferred
#ifdef IDE_DEBUG
	dputs("Returning\n");
#endif
	return(qdata->num);
}

static char	*disk_error[] = {
"Address mark not found\n",
"Track 0 not found\n",
"Aborted command\n",
"Media change\n",
"ID not found\n",
"Media changed\n",
"Uncorrectable data error\n"
};

static void show_abort_info(void)
{
	int	i;
	char	error;
	int	sec;
	int	sectors;

	sec = (DRIVE_HEAD_REG & 0xf) << 24;
	sec |= ((HIGH_CYLINDER_REG & 0xff) << 16);
	sec |= ((LOW_CYLINDER_REG & 0xff) << 8);
	sec |= (SECTOR_NUM_REG & 0xff);
	sectors = SECTOR_COUNT_REG & 0xff;
	dputs("Disk Error - ");
	dputs("Sector: ");
	dphex(sec);
	dputs(" - Num Sectors: ");
	dphex(sectors);
	dputs("\n");
	error = PRECOMP_ERROR_REG;
	for(i = 0; i < 7; i++)
	{
		if(error & (1<<i))
		{
			dputs(disk_error[i]);
		}
	}
}
		
		

//
// This is the interrupt handler for the ide disk.  This interrupt is received
// whenever data is ready to be transferred for a read or write request.
//
void ide_intr(void)
{
	volatile register struct queue_data	*qdata = queue.qd[queue.tail];
	register int								dma_status;
	register int								bytes;

	if(in_debugger)
	{
		(void)STATUS_REG;
		return;
	}

	// Turn OFF the RED led
	*((volatile int *)LED_ADDR) |= 1;

	// Disble the timeout timer
	ide_disable_timeout();

	// Did an error occur
	if(ALT_STATUS_REG & IDE_STB_ERROR)
	{
		if(*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) & 0x00010000)
		{
			// Stop the DMA, reset the interrupt and error (see erata for the NSC415)
			*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x0020000e;
		}

		// Should we retry ?
		if(qdata->retry_count < 4)
		{
			// Acknowledge the interrupt
			(void)STATUS_REG;

			// Increment the retry count
			qdata->retry_count++;

			// Re-issue the request
			queued_req(qdata);

			// Done
			return;
		}

		// YES - Is was this request from a process ?
		if(qdata->proc)
		{
			// YES - resume the process (this is GOOSE resume_process())
			resume(qdata->proc);
		}

		// Not from process - is there a callback function installed ?
		else if(callback)
		{
			// YES - do it
			callback(IDE_STB_ERROR);
		}

		// Set the disk interrupt recieved flag
		qdata->proc = (void *)-1;

		// Show the info
		show_abort_info();

		// Adjust the tail index
		queue.tail++;

		// Make sure it is valid
		queue.tail &= (MAX_REQUESTS - 1);

		// Are there more requests pending
		if(queue.tail != queue.head)
		{
			// YES - Issue disk transfer for request
			queued_req(queue.qd[queue.tail]);
		}

		// Acknowledge the interrupt
		(void)STATUS_REG;

		// Done
		return;
	}

	// Acknowledge the interrupt
	(void)STATUS_REG;

	// Get the status of the DMA transfer
	dma_status = *((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4));

	// Stop the DMA, reset the interrupt and error (see erata for the NSC415)
	*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x6;

	// Did an error occur while DMA'ing the data
	if(dma_status & 0x00020000)
	{
		// Should we retry ?
		if(qdata->retry_count < 4)
		{
			// Increment the retry count
			qdata->retry_count++;

			// Re-issue the request
			queued_req(qdata);

			// Done
			return;
		}

		// Was this request from a process ?
		if(qdata->proc)
		{
			// YES - resume the process (this is GOOSE resume_process())
			// Turn OFF the GREEN LED
			resume(qdata->proc);
		}

		// Not from process - is there a callback function installed ?
		else if(callback)
		{
			// YES - do it
			callback(dma_status);
		}

		// Set the disk interrupt recieved flag
		qdata->proc = (void *)-1;

		// Adjust the tail index
		queue.tail++;

		// Make sure it is valid
		queue.tail &= (MAX_REQUESTS - 1);

		// Are there more requests pending
		if(queue.tail != queue.head)
		{
			// YES - Issue disk transfer for request
			queued_req(queue.qd[queue.tail]);
		}

		// Done
		return;
	}

	// Decrement count of number of sectors left
	qdata->num -= qdata->num_2_trans;

	// Is there more to transfer for this request ?
	if(qdata->num)
	{
		// Increment the sector number
		qdata->sec += qdata->num_2_trans;

		// Increment the buffer address (ints)
		qdata->buf += (qdata->num_2_trans * LONGS_PER_SECTOR);

		// Send another transfer command
		queued_req(qdata);
	}

	// No more left to transfer for this request
	else
	{
		// Adjust the tail index
		queue.tail++;

		// Make sure it is valid
		queue.tail &= (MAX_REQUESTS - 1);

		// Are there more requests pending
		if(queue.tail != queue.head)
		{
			// YES - Issue disk transfer for request
			queued_req(queue.qd[queue.tail]);
		}

		// Was this request from a process ?
		if(qdata->proc)
		{
			// YES - Wakeup the process
			// Turn OFF the GREEN LED
			resume(qdata->proc);
		}

		// Not from process - is there a callback function installed ?
		else if(callback)
		{
			// YES - do it
			callback(0);
		}

		// Set the disk interrupt received flag
		qdata->proc = (void *)-1;
	}
}

//
// This is the queued disk transfer function.  Ideally (if the IDE DMA can be
// made to transfer data without errors) this function should do a DMA
// transfer.  This will allow upto 256 sectors to be transferred in one shot and
// the interrupt handler will NOT have to do the data transfer.  However,
// data will have to be transferred to the physical address and the data
// caches for those pages will have to be invalidated by the disk interrupt
// routine.  Alternately, the data could be transferred to a secondary buffer
// and then copied from that buffer to the users buffer by the processor but
// that would pretty much defeat the purpose of using DMA.
//

static int queued_req(volatile struct queue_data *qd)
{
	register	int			num;
	register int			max_bytes;
	register prd_entry_t	*prd_ptr;
	int						i;

	// Turn ON the RED led
	*((volatile int *)LED_ADDR) &= ~1;

	// Make sure the disk interrupt is enabled
	enable_ip(IDE_DISK_INT);

	// Figure out number of sectors to tranfer
	qd->num_2_trans = qd->num;

	// Limit to maximum number of sectors for a multisector transfer
	// NOTE - if using the DMA to transfer the data this limit can go to
	// 256 sectors.
	// This is 254 because the field used to specify the number of bytes to
	// transfer with the DMA is 16 bits.  If you multiply 128 by the sector
	// size (512 bytes) you get 65536.  This is 0x10000 (too many bits).  So
	// The maximum number of bytes that can be transferred with the IDE
	// controllers DMA is 127 * 512 (65024 bytes).
	if(qd->num_2_trans > 256)
	{
		qd->num_2_trans = 256;
	}

	// Wait for drive to be NOT busy (the drive should NEVER be busy here)
	if(!ide_wait_bsy())
	{
#ifdef IDE_DEBUG
		dputs("queued_req() - TIMEOUT WAITING FOR BUSY\n");
#endif
		return(0);
	}

	// Setup the command to transfer multiple sectors (LBA addressing mode)
	DRIVE_HEAD_REG = (unsigned char)(0x40|device_list.select|DRIVE_HEAD_INFO|((qd->sec >> 24) & 0xf));

	// Set the scatter gather list pointer
	qd->prd = prd;

	// Get pointer to the prd table for this request
	prd_ptr = (prd_entry_t *)qd->prd;

	// Get number of bytes to transfer
	num = qd->num_2_trans * 512;

	// Set first prd entry transfer address
	prd_ptr->paddr = (int)qd->buf & 0x1fffffff;

	// Set remaining prd entry byte counts and next prd entry transfer entry
	while(num)
	{
		max_bytes = 65536 - (prd_ptr->paddr & 0xffff);

		if(max_bytes > num)
		{
			max_bytes = num;
		}

		// Set the count for this entry to max
		prd_ptr->byte_count = max_bytes & 0xffff;

		// Set the next entry's transfer address
		(prd_ptr + 1)->paddr = prd_ptr->paddr + max_bytes;

		// Go to next prd entry
		++prd_ptr;

		// Decrement number
		num -= max_bytes;
	}

	// Set the byte count for the last prd entry and terminate the transfer list
	(prd_ptr-1)->byte_count |= 0x80000000;

	LOW_CYLINDER_REG = (unsigned char)((qd->sec >> 8) & 0xff);
	HIGH_CYLINDER_REG = (unsigned char)((qd->sec >> 16) & 0xff);
	SECTOR_NUM_REG = (unsigned char)(qd->sec & 0xff);
	SECTOR_COUNT_REG = (qd->num_2_trans & 0xff);

	// Set the address of the PRD table for the DMA transfer (physical)
	*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4 + 4)) = (int)qd->prd & 0x1fffffff;

	// Send the transfer multiple sectors command
	// If using the DMA to transfer the data, this becomes and IDE_CMD_DMAREAD
	// and the control block used for the transfer must be filled in with
	// the physical address and byte count for the request.
	if(!qd->request)
	{
		// Set the DMA to read from disk/write to memory
		*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x0020000e;

		// Start the DMA
		*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x00200009;

		// Enable the timeout timer
		ide_enable_timeout();

		// Send the DMA Read Command
		COMMAND_REG = IDE_CMD_DREAD;
	}
	else
	{
		// Set the DMA to read from memory/write to disk
		*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x00200006;

		// Start the DMA
		*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x00200001;

		// Enable the timeout timer
		ide_enable_timeout();

		// Send the DMA Write Command
		COMMAND_REG = IDE_CMD_DWRITE;
	}

	// Return OK
	return(1);
}



// This function gets called by the debugger stub reboot request to reset
// the disk queues.
void reset_disk_que(void)
{
	queue.head = queue.tail = 0;
}

void *get_ide_ident_info(void)
{
	return((void *)ide_sector_buffer);
}


void install_disk_callback(void (*func)(int))
{
	int	timeout = 100000000;

	// Are there q'd requests ?
	if(queue.head != queue.tail)
	{
		// YES - wait for the queue to empty
		while(queue.head != queue.tail && --timeout) ;

		// Did we timeout ?
		if(!timeout)
		{
			// YES - flush disk cache and disk que
			flush_disk_queue();
			flush_disk_cache();
		}
	}

	// Set the callback function
	callback = func;
}

void install_disk_info(int **p, void (*sus)(void), void (*res)(int *))
{
	if(!p)
	{
		if(queue.head != queue.tail)
		{
			dputs("install_disk_info() - Uninstall while requests are queued\n");
		}
		while(queue.head != queue.tail) ;
	}
	proc = p;
	suspend = sus;
	resume = res;
}

void flush_disk_queue(void)
{
	reset_disk_que();
}

void install_disk_timeout_audit(void (*func)(void))
{
	audit = func;
}

void ide_enable_timeout(void)
{
	if(proc || callback)
	{
		ide_timeout_time = 0;
		ide_timeout_enable = 1;
		ide_timer_ints = 0;
	}
	else
	{
		ide_timeout_enable = 0;
	}
}

void ide_disable_timeout(void)
{
	ide_timeout_enable = 0;
	ide_timeout_time = 0;
	ide_timer_ints = 0;
}

#if defined(SPACE)
#define	TIMER_PERIOD	21
#else
#define	TIMER_PERIOD	20
#endif

#define	IDE_TIMEOUT_MS		250

#if 0
static void show_ide_info(volatile struct queue_data *qdata)
{
	volatile prd_entry_t	*prd_ptr = qdata->prd;
	int			i = 1;
	int			total_bytes = 0;
	int			sectors;
	unsigned char	pci_status;

	dputs("\n");
	do
	{
		dputs("PRD ENTRY:  ");
		dphex(i);
		dputs("\n");
		dputs("paddr:  ");
		dphex(prd_ptr->paddr);
		dputs("\nBytes:  ");
		dphex(prd_ptr->byte_count & 0x7fffffff);
		dputs("\n");
		total_bytes += (prd_ptr->byte_count & 0x7fffffff) == 0 ? 65536 : (prd_ptr->byte_count & 0x7fffffff);
		prd_ptr++;
		i++;
	} while(prd_ptr->byte_count >= 0);

	dputs("PRD ENTRY:  ");
	dphex(i);
	dputs("\n");
	dputs("paddr:  ");
	dphex(prd_ptr->paddr);
	dputs("\nBytes:  ");
	dphex(prd_ptr->byte_count & 0x7fffffff);
	dputs("\n");
	total_bytes += (prd_ptr->byte_count & 0x7fffffff) == 0 ? 65536 : (prd_ptr->byte_count & 0x7fffffff);

	sectors = (int)SECTOR_COUNT_REG;
	if(!sectors)
	{
		sectors = 256;
	}
	dputs("Sectors:  ");
	dphex(sectors);
	dputs("\n");

	pci_status = PCI_ReadConfigByte(PC87415_DEVICE_NUMBER, 7);
	if(pci_status & 1)
	{
		dputs("PCI Data parity error detected\n");
	}
	if(pci_status & 8)
	{
		dputs("PCI Target abort signaled\n");
	}
	if(pci_status & 16)
	{
		dputs("PCI Target abort received\n");
	}
	if(pci_status & 32)
	{
		dputs("PCI Master abort received\n");
	}
	if(pci_status & 64)
	{
		dputs("PCI SERR signaled\n");
	}
	if(pci_status & 128)
	{
		dputs("PCI PERR detected\n");
	}

	if(total_bytes != (sectors * 512))
	{
		dputs("DMA byte count != sector count\n");
	}
}
#endif

void ide_disk_timeout(void)
{
	volatile register struct queue_data	*qdata = queue.qd[queue.tail];
	int											status;
	int											divisor = (IDE_TIMEOUT_MS * 1000000) / (ide_timer_start * TIMER_PERIOD);

	// Is the disk timeout enabled ?
	if(!ide_timeout_enable)
	{
		// NOPE - return;
		return;
	}

	// Make sure divisor is good
	if(divisor <= 0)
	{
		divisor = 1;
	}

	// Increment the timer ints
	++ide_timer_ints;

	// Time to increment the timeout timer ?
	if(!(ide_timer_ints % divisor))
	{
		// YES - Reset ide timer interrupt count
		ide_timer_ints = 0;

		// YES - Has 250 ms passed ?
		if(++ide_timeout_time > IDE_TIMEOUT_MS)
		{
			// YES - Have we retried < 4 times ?
			if(++qdata->retry_count < 5)
			{
				// NOPE - Reset the timeout time
				ide_timeout_time = 0;
	
				// Try to figure out why we timed out
				status = *((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4));

				// DMA error
				if(status & 0x00020000)
				{
					dputs("ide_disk_timeout() - DMA error\n");
				}
	
				// Transferred more than 4 dwords less than transfer size ?
				// DMA inactive, Int inactive
				if(!(status & 0x00050000))
				{
					// YES
					dputs("ide_disk_timeout() - byte count > 4 dwords less than transfer size\n");
				}
	
				// Transferred more than 1 and less than 4 dwords less than transfer size ?
				// DMA inactive, Int inactive
				else if(!(status & 0x00010000) && (status & 0x00040000))
				{
					// YES
					dputs("ide_disk_timeout() - byte count < 4 and more than 1 dwords less than transfer size\n");
				}
	
				// Byte count more than transfer size ?
				// DMA active, Int active
				else if((status & 0x00050000) == 0x00050000)
				{
					// YES
					dputs("ide_disk_timeout() - byte count > transfer size\n");
				}

				// Unknown reason for timeout
				// For this case the drive is probably just responding really
				// slowly because it may be doing retries or may be in the middle
				// of a thermal recalibration at the time of the request or
				// doing something else that takes a very long time.  In this
				// case, we should NOT reissue the command.
				// DMA active, Int inactive
				else
				{
					dputs("ide_disk_timeout() - dma timeout\n");
					qdata->retry_count--;
					if(qdata->retry_count < 0)
					{
						qdata->retry_count = 0;
					}
					ide_enable_timeout();
					return;
				}
	
				// Is the DMA active ?	
				if(status & 0x00010000)
				{
					// YES - Stop the DMA, reset the interrupt and error (see erata for the NSC415)
					*((volatile int *)(IDE_BASE_ADDRESS + NSC415_DEFVAL_BAR4)) = 0x0020000e;
				}

				// Re-issue the request
				queued_req(qdata);
			}

			else
			{
				if(audit)
				{
					// YES - call it
					audit();
				}

				dputs("ide_disk_timeout() - reboot\n");

				// Wait for the watchdog to catch
				while(1) ;
			}
		}
	}
}
