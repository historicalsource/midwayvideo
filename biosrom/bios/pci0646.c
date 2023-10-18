/****************************************************************************/
/*                                                                          */
/* pci0646.c - Source for PCI0606 PCI IDE Disk Driver                       */
/*                                                                          */
/* Written by:  Dave Sheppard (ATARI Games Inc.)                            */
/* Modified by: Michael J. Lynch (Midway Video Inc.)                        */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 15 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<system.h>
#include	<stdproto.h>

static unsigned long	bar4_base_address;
static unsigned long	ide_base_address;
static unsigned long	alt_base_address;

#include	<pci0646.h>

//#define	IDE_DEBUG
//#define	IDE_DEBUG1
//#define	IDE_DEBUG2

#ifdef IDE_DEBUG
void dputs(char *);
void dphex(int);
#endif

#define	DMA
#define	DISK_CACHE
#define	DISK_INT

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
#if defined(DISK_CACHE)
void init_disk_cache(void);			// Initialize the disk cache
#endif
#if defined(DISK_INT)
int disable_ip(int);						// Disable an interrupt
void enable_ip(int);						// Enable an interrupt
#endif
void hit_invalidate_dcache(void *, int);
void hit_writeback_invalidate_dcache(void *, int);


/****************************************************************************/
/* Statis data for this module                                              */
/****************************************************************************/
static unsigned long	ide_sector_buffer[LONGS_PER_SECTOR];	// temporary sector buffer
static unsigned long	partition_buffer[LONGS_PER_SECTOR+8];		// buffer for holding partition data
static DeviceDesc		device_list;									// drive information
static int				initialized = 0;								// initialization flag
static int				disk_active = 0;								// Active flag
static int				transfer_error = 0;

int	ide_drive_status = 0;

// Callback function pointer
void				(*callback)(int) = (void (*)(int))0;	// Callback function

// The read queue
volatile static struct queue	queue = {0, 0};
volatile static struct queue_data	qd[MAX_REQUESTS];

// The prd entries used by the DMA
// These are in an uncached segment so as to eliminate some code.
// The first item is initialized here to get the section attribute to work
// properly.
volatile prd_entry_t	prd[MAX_REQUESTS*2] __attribute__ ((section ("uncached.bss"))) = {
{0, 0},
};
volatile	prd_entry_t	*_prd_ptr;


// Pointer to the partition table information
//static partition_table_t	*part_table = (partition_table_t *)partition_buffer;
static partition_table_t	*part_table;
static partition_info_t		*part_info = (partition_info_t *)0;


/****************************************************************************/
/* Prototypes for static functions in this module                           */
/****************************************************************************/
static int	ide_wait_bsy(void);
static int	ide_soft_reset(void);
static void	ide_hread_data(register unsigned int *);
static int	ide_hread_sectors(unsigned int *, int, int, int, int);
void	prc_delay(int);
static void	setup_harddrive(void);
static unsigned char PCI_ReadConfigByte(int, unsigned char);
static void PCI_WriteConfigByte(int, unsigned char, unsigned char);
static void PCI_WriteConfigWord(int, unsigned char, unsigned short);
static void PCI_WriteConfigDword(int, unsigned char, unsigned long);
static short que_trans(unsigned long sec, unsigned long *buf, unsigned long num, int type);
static void queued_req(volatile struct queue_data *qd);

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
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;

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

static int	pio_active_clocks[] = {
0x0,	// 0 clks (padding)
0x10,	// 1 clk (30 ns)
0x20,	// 2 clks (60 ns)
0x30,	// 3 clks (90 ns)
0x40,	// 4 clks (120 ns)
0x50,	// 5 clks (150 ns)
0x60,	// 6 clks (180 ns)
0x70,	// 7 clks (210 ns)
0x80,	// 8 clks (240 ns)
0x90,	// 9 clks (270 ns)
0xa0,	// 10 clks (300 ns)
0xb0,	// 11 clks (330 ns)
0xc0,	// 12 clks (360 ns)
0xd0,	// 13 clks (390 ns)
0xe0,	// 14 clks (420 ns)
0xf0,	// 15 clks (450 ns)
0x00	// 16 clks (480 ns)
};

static int	pio_recovery_clocks[] = {
0x0,	// 0 clks (padding)
0xf,	// 1 clks (30 ns)
0x1,	// 2 clks (60 ns)
0x2,	// 3 clks (90 ns)
0x3,	// 4 clks (120 ns)
0x4,	// 5 clks (150 ns)
0x5,	// 6 clks (180 ns)
0x6,	// 7 clks (210 ns)
0x7,	// 8 clks (240 ns)
0x8,	// 9 clks (270 ns)
0x9,	// 10 clks (300 ns)
0xa,	// 11 clks (330 ns)
0xb,	// 12 clks (360 ns)
0xc,	// 13 clks (390 ns)
0xd,	// 14 clks (420 ns)
0xe,	// 15 clks (450 ns)
0x0	// 16 clks (480 ns)
};

static void PCI0646_GetCurrentRegs(int devhand, PCI0646_Regs *regptr, int getdma);
static void PCI0646_SetRegValues(int devhand, PCI0646_Regs *regptr);

void ide_allow_init(void)
{
	initialized = 0;
}

//
// ide_init() - This function is used to initialize the ide disk drive
//

int ide_init(void)
{
	register unsigned char	*ide_base;
	register volatile unsigned char	*alt_base;

	int				eer_rtc;
	PCI0646_Regs	disk_controller_regs;
	int				recovery_time;
	int				active_time;
	int				pio_mode = 0;
	int				tmp;
	int				*t;

#ifdef IDE_DEBUG
	dputs("ide_init()\n");
#endif

	// Already initialized ?
	if(initialized)
	{
#ifdef IDE_DEBUG
		dputs("ide_init() - IDE DRIVE ALREADY INITIALIZED\n");
#endif
		// Return status
		return(device_list.status);
	}

	// Make sure pointer for partition table is cache page aligned
	t = (int *)&partition_buffer[0];
	while((int)t & 0x1f)
	{
		t++;
	}
	part_table = (partition_table_t *)t;

	// Initialize the disk caches
#if defined(DISK_CACHE)
	init_disk_cache();
#endif

	// Setup the controller
	setup_harddrive();

ide_soft_reset();

	// Set the base addresses used for macros
	ide_base = (unsigned char *)ide_base_address;
	alt_base = (volatile unsigned char *)alt_base_address;

	// Initialize the control structure information
	device_list.select = 0;
	device_list.busy = 0;
	device_list.dma_timing = 0;
	device_list.pio_timing = 0;
	device_list.cyls = 0;
	device_list.heads = 0;
	device_list.sectors = 0;
	device_list.status = IDE_DEVICE_INVALID;	// assume invalid

	// Initialize the scatter gather list pointer

	// Get pointer to scatter gather list
	_prd_ptr = prd;

	// Make it an int
	tmp = (int)_prd_ptr;

	// Do we have enough room for 16 entries without crossing a 64k boundary ?
	if((tmp & 0xffff) != ((int)(prd + 16) & 0xffff))
	{
		// Nope - Increment the pointer to align on a 64k boundary
		while(tmp & 0xffff)
		{
			tmp++;
		}
	}

	// Set the scatter gather list pointer
	_prd_ptr = (prd_entry_t *)tmp;

	// Set the device
	ide_set_device(0);

	// Status == 0xff ?
	if(ALT_STATUS_REG == 0xff)
	{
#ifdef IDE_DEBUG
		dputs("ALT STATUS ADDR:  ");
		dphex(alt_base_address + ALT_STATUS_OFFSET);
		dputs("\n");
		dputs("ALT_STATUS_REG:  ");
		dphex((int)ALT_STATUS_REG);
		dputs("\n");
		dputs("ide_init() - No drive connected\n");
#endif
		// Assume no drive connected
		return(device_list.status);
	}


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
			dputs("ide_int() - TIMEOUT WAITING FOR DRIVE BUSY BEFORE IDENTIFY COMMAND\n");
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

			// Get the current configuration registers
			PCI0646_GetCurrentRegs(PCI0646_DEVICE_NUMBER, &disk_controller_regs, (int)FALSE);

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
			dputs("ide_init() - PIO MODE: ");
			dphex(pio_mode);
			dputs("\n");
#endif

			// Get the minimum active time for this mode
			active_time = piod_active_times[pio_mode];
			active_time += 29;
			active_time /= 30;

			// Calculate the recovery time based on mode cycle time and mode
			// minimum active time
			recovery_time = ((DriveID *)ide_sector_buffer)->eide_pio_iordy - (active_time * 30);
			recovery_time += 29;
			recovery_time /= 30;

			// Or 'em together to get value to put into the register
			active_time = (pio_active_clocks[active_time] | pio_recovery_clocks[recovery_time]);

			// Set the data read and write cycle times
			disk_controller_regs.pci0646_ConfigRegs.pci0646_DACKTiming_0 = active_time;
			disk_controller_regs.pci0646_ConfigRegs.pci0646_DACKTiming_1 = active_time;

			// Get the minimum command active time for this PIO mode
			active_time = pioc_active_times[pio_mode];
			active_time += 29;
			active_time /= 30;

			// Calculate the recovery time based on mode cycle time and mode
			// minimum active time
			recovery_time = ((DriveID *)ide_sector_buffer)->eide_pio_iordy - (active_time * 30);
			recovery_time += 29;
			recovery_time /= 30;

			// Or 'em together to get value to put into the register
			active_time = (pio_active_clocks[active_time] | pio_recovery_clocks[recovery_time]);

			// Set the command read and write cycle times
			disk_controller_regs.pci0646_ConfigRegs.pci0646_CmdTiming = active_time;

			// Set the registers
			PCI0646_SetRegValues(PCI0646_DEVICE_NUMBER, &disk_controller_regs);

			if(!ide_wait_bsy())
			{
#ifdef IDE_DEBUG
				dputs("ide_init() - TIMEOUT WAITING FOR BUSY BEFORE SETTING PIO MODE\n");
#endif
				return(device_list.status);
			}
#ifdef IDE_DEBUG
			dputs("ide_init() - ISSUING PIO MODE COMMAND\n");
#endif
			PRECOMP_ERROR_REG = 0x3;
			SECTOR_COUNT_REG = 0xb;
			DRIVE_HEAD_REG = 0xa0;
			COMMAND_REG = IDE_CMD_BUFFERMODE;

			// Enable multi-sector transfers
			if(!ide_wait_bsy())
			{
#ifdef IDE_DEBUG
				dputs("ide_init() - TIMEOUT WAITING FOR BUSY BEFORE SETTING MULTI-SECTOR TRANSFER SIZE\n");
#endif
				return(device_list.status);
			}
#ifdef IDE_DEBUG
			dputs("ide_init() - ISSUING MULTI-SECTOR TRANSFER SIZE COMMAND\n");
#endif
			SECTOR_COUNT_REG = device_list.max_multsect;
			DRIVE_HEAD_REG = 0xa0;
			COMMAND_REG = IDE_CMD_MULTIMODE;


			// Set initialized flag
			initialized = 1;

#ifdef IDE_DEBUG
			dputs("ide_init() - READING PARTITION TABLE\n");
#endif

			// Read the partition table
			eer_rtc = PART_READ_TIMEOUT;
			while(ide_hread_sectors((unsigned int *)part_table, 0, 0, 1, 1) && --eer_rtc)
			{
				prc_delay(0);
			}

			// Did we read the partition table OK
			if(!eer_rtc)
			{
#ifdef IDE_DEBUG
				dputs("ide_init() - TIMEOUT READING PARTITION TABLE\n");
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
				dputs("ide_init() - INVALID PARTITION TABLE\n");
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

#if defined(DISK_INT)
#ifdef IDE_DEBUG
			dputs("ide_init() - ENABLING DISK INTERRUPTS\n");
#endif
			// Clear any possible pending interrupts
			*((volatile char *)(bar4_base_address + 1)) = 0xd;
			*((volatile char *)(bar4_base_address + 2)) = 0x6;

			// Enable the IDE Disk interrupt
			enable_ip(IDE_DISK_INT);
#endif

		}
#ifdef IDE_DEBUG
		else
		{
			dputs("ide_init() - IDENDTIFY COMMAND FAILURE\n");
		}
	}
	else
	{
		dputs("ide_init() - TIMEOUT WAITING FOR POWER UP RESET\n");
#endif
	}

	// Give a little extra time before exiting
	prc_delay(0);

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
	register volatile unsigned char	*alt_base = (volatile unsigned char *)alt_base_address;
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
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;
	register volatile unsigned char	*alt_base = (volatile unsigned char *)alt_base_address;
	register int	nloops;

#ifdef IDE_DEBUG
	dputs("ide_hread_sectors()\n");
#endif

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
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;
	register volatile unsigned char	*alt_base = (volatile unsigned char *)alt_base_address;
	int 		tmp;
	int 		i;
	DriveID	*id;
	int		eer_rtc;

#ifdef IDE_DEBUG
	dputs("ide_identify()\n");
#endif

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
// ide_wait_busy() - This function is used to wait for the drive to be NOT
// busy or timeout, whichever come first.
//
static int ide_wait_bsy(void)
{
	register volatile unsigned char	*alt_base = (volatile unsigned char *)alt_base_address;
	int	eer_rtc = (BUSY_TIMEOUT * 500);

	while((ALT_STATUS_REG & IDE_STB_BUSY) && --eer_rtc) ;

	return(eer_rtc);
}


//
// ide_soft_reset() - This function is used to software reset the drive.
//
static int ide_soft_reset(void)
{
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;
	register volatile unsigned char	*alt_base = (volatile unsigned char *)alt_base_address;
	int	timer;

#ifdef IDE_DEBUG
	dputs("ide_soft_reset()\n");
#endif

	// Set the reset bit until timeout or no master aborts
	*((volatile char *)(bar4_base_address + 1)) |= 0x40;

	// Hold reset for 1/4 second
	prc_delay(15);

	// Reset the reset bit until timeout or no master abort
	*((volatile char *)(bar4_base_address + 1)) &= ~0x40;

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
#ifdef IDE_DEBUG
		if(!timer)
		{
			dputs("ide_soft_reset(): Timeout waiting for drive to go ready\n");
			dputs("ide_soft_reset(): Alt status:  ");
			dphex((int)ALT_STATUS_REG);
			dputs("\n");
		}
#endif
	}
#ifdef IDE_DEBUG
	else
	{
		dputs("ide_soft_reset():  Timeout waiting for drive to go NOT busy\n");
	}
#endif

	// Did we NOT timeout
	if(timer)
	{
		// YES - return sucess
#ifdef IDE_DEBUG
		dputs("ide_soft_reset():  Drive reset\n");
#endif
		return(0);
	}

	// Return fail
#ifdef IDE_DEBUG
		dputs("ide_soft_reset():  Drive reset TIMEOUT\n");
#endif
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

#ifdef IDE_DEBUG
	dputs("_SecReads()\n");
#endif

	return(que_trans(sec, buf, num, 0));
}

#if defined(DISK_CACHE)
void disk_cache_read(int sec, unsigned long *buf, int num);
#endif


short SecReads(unsigned long sec, unsigned long *buf, unsigned long num)
{
#ifdef IDE_DEBUG
	dputs("SecReads()\n");
#endif

	// If NOT initialized - return fail
	if(!initialized)
	{
#ifdef IDE_DEBUG2
		dputs("SecReads() - Disk NOT initialized\n");
#endif
		return(0);
	}

	// Check to make sure we are accessing valid blocks in the partition
	if((sec + num) >= part_info->num_blocks)
	{
#ifdef IDE_DEBUG2
		dputs("SecReads() - Sector number out of range\n");
#endif
		return(0);
	}

	// Offset the sector by the starting block of the active partition
	sec += part_info->starting_block;

	// Read from the disk cache.  If a miss occurs, _SecReads (below) gets
	// called to satisfy the request.
#if defined(DISK_CACHE)
#ifdef IDE_DEBUG2
	dputs("SecReads() - Disk cache read\n");
#endif
	disk_cache_read(sec, buf, num);
#else
	_SecReads(sec, buf, num);
#endif
	if(transfer_error)
	{
		return(0);
	}
	return(1);
}


//
// SecWrites() - This function is used to write sectors to the drive using
// logical block addressing mode.
//
short _SecWrites(unsigned long sec, unsigned long *buf, unsigned long num)
{
#ifdef IDE_DEBUG
	dputs("_SecWrites()\n");
#endif

#ifdef IDE_DEBUG2
	dputs("_SecWrites() - Queued write\n");
#endif
	return(que_trans(sec, buf, num, 1));
}

#if defined(DISK_CACHE)
void disk_cache_write(int sec, unsigned long *buf, int num);
#endif

short SecWrites(unsigned long sec, unsigned long *buf, unsigned long num)
{
#ifdef IDE_DEBUG
	dputs("SecWrites()\n");
#endif

	// If NOT initialized - return fail
	if(!initialized)
	{
#ifdef IDE_DEBUG2
		dputs("SecWrites() - Driver not initialized\n");
#endif
		return(0);
	}

	// Check to make sure we are accessing valid blocks in the partition
	if((sec + num) >= part_info->num_blocks)
	{
#ifdef IDE_DEBUG2
		dputs("SecWrites() - Sector out of range\n");
#endif
		return(0);
	}

	// Offset the sector by the starting block of the active partition
	sec += part_info->starting_block;

	// Make sure we maintain coherency of the disk cache
#if defined(DISK_CACHE)
#ifdef IDE_DEBUG2
	dputs("SecWrites() - Cached write\n");
#endif
	disk_cache_write(sec, buf, num);
#else
	_SecWrites(sec, buf, num);
#endif
	if(transfer_error)
	{
		return(0);
	}
	return(1);
}


//
// PCI0646_GetCurrentRegs() - This function retrieves the values of all of
// the configuration registers for the NSC87415 PCI IDE controller.
//
static void PCI0646_GetCurrentRegs(int devhand, PCI0646_Regs *regptr, int getdma)
{
	unsigned int	i;
	unsigned char	*dbptr;
	unsigned short	baseadr;
	unsigned long	tmp;

	dbptr = (unsigned char *)&(regptr->pci0646_ConfigRegs);

	for(i = 0; i < sizeof(PCI0646_CfigRegs); ++i, ++dbptr)
	{
		*dbptr = PCI_ReadConfigByte(devhand, (unsigned char)i);
	}
}


//
// PCI0646_SetRegValues() - This function sets all of the configuration
// registers for the NSC87415 PCI IDE controller.
//
static void PCI0646_SetRegValues(int devhand, PCI0646_Regs *regptr)
{
	PCI0646_CfigRegs	*figptr;

	figptr = &regptr->pci0646_ConfigRegs;

	PCI_WriteConfigWord(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_Command) - (unsigned long)figptr),
		figptr->pci0646_Command);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_ProgIface) - (unsigned long)figptr),
		figptr->pci0646_ProgIface);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_Latency) - (unsigned long)figptr),
		figptr->pci0646_Latency);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_CmdTiming) - (unsigned long)figptr),
		figptr->pci0646_CmdTiming);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_DACKTiming_0) - (unsigned long)figptr),
		figptr->pci0646_DACKTiming_0);
	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_DACKTiming_1) - (unsigned long)figptr),
		figptr->pci0646_DACKTiming_1);

	PCI_WriteConfigByte(devhand,
		(unsigned char)((unsigned long)&(figptr->pci0646_Control_0_1) - (unsigned long)figptr),
		figptr->pci0646_Control_0_1);
}


//
// PCI0646_InitRegValues() - This function initializes the configuration
// registers for the NSC87415 PCI IDE controller.
//
static void PCI0646_InitRegValues(PCI0646_Regs *regptr)
{
	PCI0646_CfigRegs	*nptr;

	nptr = &regptr->pci0646_ConfigRegs;

	nptr->pci0646_Command = 0x105;		// enable, bus master and sys error check

	// Enable master IDE and native mode
	nptr->pci0646_ProgIface = 0x8f;

	// Enable drive 0 read ahead - disable channel 2
	nptr->pci0646_Control_0_1 = 4;
}


//
// setup_harddrive() - This function sets up the NSC87415 PCI IDE controller
//
#define CMD_DEF_BAR (0x400)
#define CMD_PCI_CFR		(0x50/4) /* PCI configuration registers */
#define CMD_PCI_DRWTIM0		(0x54/4) /* PCI Drive 0 R/W or DACK timing */
#define CMD_PCI_DRWTIM2		(0x58/4) /* PCI Drive 2 R/W or DACK timing */

static void setup_harddrive(void)
{
	PCI0646_Regs	disk_controller_regs;
	PCI0646_MastRegs	*bptr;

#ifdef IDE_DEBUG
	dputs("setup_harddrive()\n");
#endif

	ide_base_address = get_pci_config_reg(PCI0646_DEVICE_NUMBER, 4);
	ide_base_address &= ~0x3;
	ide_base_address |= 0xa0000000;
	alt_base_address = get_pci_config_reg(PCI0646_DEVICE_NUMBER, 5);
	alt_base_address &= ~0x3;
	alt_base_address |= 0xa0000000;
	bar4_base_address = get_pci_config_reg(PCI0646_DEVICE_NUMBER, 8);
	bar4_base_address &= ~0xf;
	bar4_base_address |= 0xa0000000;

	// Get configuration registers
	PCI0646_GetCurrentRegs(PCI0646_DEVICE_NUMBER, &disk_controller_regs, (int)FALSE);

	// Initialize the configuration registers
	PCI0646_InitRegValues(&disk_controller_regs);

	// Write configuration registers
	PCI0646_SetRegValues(PCI0646_DEVICE_NUMBER, &disk_controller_regs);

	bptr = (PCI0646_MastRegs *)(bar4_base_address);

	bptr->pci0646_BMIDECR0 = 0x00;			// stop any DMA transfers
	bptr->pci0646_MRDMODE  = 0x0c;			// reset pending interrupt bits
	bptr->pci0646_BMIDESR0 = 0x06;			// reset error/interrupt bits

	bptr->pci0646_BMIDECR1 = 0x00;			// stop any DMA transfers
	bptr->pci0646_BMIDESR1 = 0x06;			// reset error/interrupts bits
}



//
// PCI_ReadConfigByte() - This function reads a byte from a configuration
// register.
//
static int	last_reg_read = -1;
static int	last_reg_data = 0;

static unsigned char PCI_ReadConfigByte(int devhand, unsigned char regaddr)
{
	int	i;

	if(last_reg_read == (regaddr >> 2))
	{
		i = last_reg_data;
	}
	else
	{
		i = get_pci_config_reg(devhand, (regaddr >> 2));
		last_reg_data = i;
		last_reg_read = (regaddr >> 2);
	}
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
#if defined(DISK_INT)
void	ide_intr(void);
#endif


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

#ifdef IDE_DEBUG
	dputs("add_queue()\n");
#endif

	// If the request is a write - block until all previous transactions have
	// finished
	if(qdata->request != 0)
	{
#ifdef IDE_DEBUG
		dputs("add_queue() - Write\n");
#endif
		// Wait for the queue to drain
		while(queue.head != queue.tail) ;
	}

#if defined(DISK_INT)
	// Disallow disk interrupts while adding stuff to the queue
	disable_ip(IDE_DISK_INT);
#endif

	// Get status of queue
	is_not_empty = queue.head - queue.tail;

	head = queue.head + 1 >= sizeof(queue.qd)/sizeof(void *) ? 0 : queue.head + 1;
	if(head == queue.tail)
	{
#if defined(DISK_INT)
		// Enable the disk interrupts
		enable_ip(IDE_DISK_INT);
#endif

		// Wait here
		do
		{
			head = queue.head + 1 >= sizeof(queue.qd)/sizeof(void *) ? 0 : queue.head + 1;
		} while(head == queue.tail);

#if defined(DISK_INT)
		// Disallow disk interrupts while adding stuff to the queue
		disable_ip(IDE_DISK_INT);
#endif

		// Get status of queue
		is_not_empty = queue.head - queue.tail;
	}

	// Add request to queue
	queue.qd[queue.head] = qdata;

	// Increment the head index
	queue.head = head;

	if(!is_not_empty)
	{
		queued_req(qdata);
	}

	enable_ip(IDE_DISK_INT);

	// Return the status of queue before the addition
	return(is_not_empty);
}

//
// que_trans() - This function is called by _SecReads and _SecWrites.
//
static short que_trans(unsigned long sec, unsigned long *buf, unsigned long num, int type)
{
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;
	volatile struct queue_data	*qdata;
	int								wait_time;

#ifdef IDE_DEBUG
	dputs("que_trans()\n");
#endif

	// Initialize the data in the qdata structure
	qdata = &qd[queue.head];
	qdata->sec = sec;
	qdata->buf = buf;
	qdata->num = num;
	qdata->request = type;
	qdata->retry_count = 0;

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

	// Add the request to the queue.
	add_queue(qdata);

retry:
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
			suspend();
		}

		// Not a process
		else
		{
			// Has a callback function been installed ?
			if(callback)
			{
				// YES - return
				return(0);
			}

			// No callback function - wait here for the interrupt
			while(!(qdata->proc)) ;
		}
	}

	// No pointers yet so just wait for interrupt
	else if(callback)
	{
#ifdef IDE_DEBUG
		dputs("que_trans() - Callback function installed - returning\n");
#endif
		// YES - return
		return(0);
	}

	else
	{
#ifdef IDE_DEBUG
		dputs("que_trans() - No Callback - waiting for interrupt\n");
#endif
		// No callback function - wait here for the interrupt
		wait_time = 10000000;
		while(!qdata->proc && --wait_time)
		{
			delay_us(1);
		}

		if(!wait_time)
		{
			transfer_error = 1;

			// Stop the DMA, reset the interrupt and error
			*((volatile char *)(bar4_base_address + 0)) = 0;
			*((volatile char *)(bar4_base_address + 1)) = 0xd;
			*((volatile char *)(bar4_base_address + 2)) = 0x6;

			(void)STATUS_REG;

			if(qdata->retry_count < 4)
			{
				ide_soft_reset();
				++qdata->retry_count;
				queued_req(qdata);
				goto retry;
			}
		}
#ifdef IDE_DEBUG
		dputs("que_trans() - Interrupt received\n");
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
	return(qdata->num);
}

//
// This is the interrupt handler for the ide disk.  This interrupt is received
// whenever data is ready to be transferred for a read or write request.
//
#if defined(DISK_INT)
void ide_intr(void)
{
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;
	volatile register struct queue_data	*qdata = queue.qd[queue.tail];
	register char								dma_status;
	register char								status;
	register int								error = 0;
	register int								bytes;

*((volatile char *)LED_ADDR) |= 0x40;
#ifdef IDE_DEBUG
	dputs("ide_intr()\n");
#endif

	// Acknowledge the interrupt from the drive
	status = STATUS_REG;

	// If bogus pointer - return
	if(!qdata)
	{
#ifdef IDE_DEBUG1
		dputs("ide_intr() - Bogus qdata pointer\n");
#endif
		return;
	}

	// Get the status of the DMA transfer
	dma_status = *((volatile char *)(bar4_base_address + 2));

	// Stop the DMA, reset the interrupt and error
	*((volatile char *)(bar4_base_address + 0)) = 0;
	*((volatile char *)(bar4_base_address + 1)) = 0xd;
	*((volatile char *)(bar4_base_address + 2)) = 0x6;

	// Did an error occur while DMA'ing the data
	if(dma_status & 2)
	{
		// YES - Set error flag
		error |= 1;
	}

	// Did a command error occur ?
	if(status & IDE_STB_ERROR)
	{
		// YES - Set error flag
		error |= 2;
	}

	// Did an error occur ?
	if(error)
	{
		// YES - Retry the request ?
		if(qdata->retry_count < 16)
		{
			// YES - Increment the retry count
			qdata->retry_count++;

			// Re-issue the request
			queued_req(queue.qd[queue.tail]);

			// Done
			return;
		}			

		// Was this request from a process ?
		if(qdata->proc)
		{
			// YES - resume the process (this is GOOSE resume_process())
			resume((void *)qdata->proc);
		}

		// Not from process - is there a callback function installed ?
		else if(callback)
		{
			// YES - do it
			callback((int)dma_status);
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

	// Decrement count of number of sectors left to transfer
	qdata->num -= qdata->num_2_trans;

	// Is there more to transfer for this request ?
	if(qdata->num)
	{
		// Is this a read ?
		if(!qdata->request)
		{
			// YES - How many bytes in this transfer
			bytes = qdata->num_2_trans * 512;

			// YES - Invalidate the data caches in this range
			hit_invalidate_dcache(qdata->buf, bytes);
		}

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
		// Was this a read ?
		if(!qdata->request)
		{
			// YES - How many bytes in this transfer
			bytes = qdata->num_2_trans * 512;

			// Invalidate the data caches in this range
			hit_invalidate_dcache(qdata->buf, bytes);
		}

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
			resume((void *)qdata->proc);
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
#endif	// DISK INT

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
static void queued_req(volatile struct queue_data *qd)
{
	register unsigned char	*ide_base = (unsigned char *)ide_base_address;
	register	int			num;
	register prd_entry_t	*prd_ptr;
	register int			bytes;
	register int			max_bytes;

*((volatile char *)LED_ADDR) &= ~0x40;
#ifdef IDE_DEBUG
	dputs("queued_req()\n");
#endif
	// Set the prd pointer
	qd->prd = _prd_ptr;

	transfer_error = 0;

	// Make sure
	// Figure out number of sectors to tranfer
	qd->num_2_trans = qd->num;

	// The maximum number of sectors that can be transferred in one DMA
	// command is 256 sectors.  Therefore we limit the number of sectors
	// to transfer in one command sequence to 256.
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
		return;
	}

	// Setup the command to transfer multiple sectors (LBA addressing mode)
	DRIVE_HEAD_REG = (unsigned char)(0x40|device_list.select|DRIVE_HEAD_INFO|((qd->sec >> 24) & 0xf));
	LOW_CYLINDER_REG = (unsigned char)((qd->sec >> 8) & 0xff);
	HIGH_CYLINDER_REG = (unsigned char)((qd->sec >> 16) & 0xff);
	SECTOR_NUM_REG = (unsigned char)(qd->sec & 0xff);
	SECTOR_COUNT_REG = (qd->num_2_trans & 0xff);

	// Each dma transfer can only transfer upto 65536 bytes.  Therefore,
	// each entry in the scatter gather list is limited to 65536 bytes.  If
	// more than 65536 bytes are to be transferred, multiple entries into the
	// scatter gather list are made to complete the transfer.  NOTE - VERY
	// IMPORTANT - No single transfer may cross a 64k boundary.  This is a
	// bad thing and will cause all sorts of nasty problems.

	// Get pointer to the prd table for this request
	prd_ptr = (prd_entry_t *)qd->prd;

	// Get number of sectors to transfer
	num = qd->num_2_trans;

	// Set first prd entry transfer address
	prd_ptr->paddr = (int)qd->buf & 0x1fffffff;

	// Figure out how many bytes we are going to transfer with with request
	bytes = num * 512;

	// Set up the scatter gather entries
	while(bytes)
	{
		// Do not allow a single entry to transfer data across a 64k boundary
		max_bytes = 65536 - ((int)prd_ptr->paddr & 0xffff);

		// Limit transfer length to number of bytes left
		if(max_bytes > bytes)
		{
			max_bytes = bytes;
		}

		// Set the byte count to maximum
		prd_ptr->byte_count = max_bytes;

		// Set the next entries transfer address
		(prd_ptr + 1)->paddr = prd_ptr->paddr + max_bytes;

		// Increment to the next entry
		++prd_ptr;

		// Decrement count of bytes left to transfer
		bytes -= max_bytes;
	}

	// Terminate the scatter gather list
	(prd_ptr-1)->byte_count |= 0x80000000;

	// Set the address of the PRD table for the DMA transfer (physical)
	*((volatile int *)(bar4_base_address + 4)) = (int)qd->prd & 0x1fffffff;

	// Send the transfer multiple sectors command
	// If using the DMA to transfer the data, this becomes and IDE_CMD_DMAREAD
	// and the control block used for the transfer must be filled in with
	// the physical address and byte count for the request.
	if(!qd->request)
	{
#ifdef IDE_DEBUG
		dputs("queued_req() - Read\n");
#endif
		// Send the DMA Read Command
		COMMAND_REG = IDE_CMD_DREAD;

		// Start the DMA in master write mode
		*((volatile char *)(bar4_base_address + 0)) = 0x9;
	}
	else
	{
#ifdef IDE_DEBUG
		dputs("queued_req() - Write - sector: ");
		dphex(qd->sec);
		dputs("  num2trans:  ");
		dphex(qd->num_2_trans);
		dputs("\n");
#endif
		if((qd->num_2_trans * 512) >= 32768)
		{
			// Writeback and invalidate the data caches
			writeback_cache();
		}
		else
		{
			hit_writeback_dcache(qd->buf, qd->num_2_trans * 512);
		}

		// Send the DMA Write Command
		COMMAND_REG = IDE_CMD_DWRITE;

		// Start the DMA in master read mode
		*((volatile char *)(bar4_base_address)) = 1;
	}
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

int disk_queue_empty(void)
{
	if(queue.head == queue.tail)
	{
		return(1);
	}
	return(0);
}
