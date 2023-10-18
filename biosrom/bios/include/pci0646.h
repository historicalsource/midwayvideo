/****************************************************************************/
/*                                                                          */
/* pci0646.h - Header file for use the PCI0646 IDE Driver                   */
/*                                                                          */
/* Modified by: Michael J. Lynch (Midway Video Inc.)                        */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 1 $                                                             */
/*                                                                          */
/****************************************************************************/
#ifndef __SYSTEM_H__
#include	<system.h>
#endif
#ifndef __PCI0646_H__
#define __PCI0646_H__

// IDE Disk Commands
#define	IDE_CMD_RECALIBRATE	0x10		// Recalibrate drive command
#define	IDE_CMD_SREAD			0x20		// Read sector(s) command
#define	IDE_CMD_SWRITE			0x30		// Write sector(s) command
#define	IDE_CMD_SVERIFY		0x40		// Verify sector(s) command
#define	IDE_CMD_FORMAT			0x50		// Format drive command
#define	IDE_CMD_SEEK			0x70		// Seek command
#define	IDE_CMD_DIAGNOSTICS	0x90		// Drive diagnostics command
#define	IDE_CMD_INITPARMS		0x91		// Initialize parameters command
#define	IDE_CMD_MREAD			0xC4		// Read multiple sectors command
#define	IDE_CMD_MWRITE			0xC5		// Write multiple sectors command
#define	IDE_CMD_MULTIMODE		0xC6		// Multisector transfer mode command
#define	IDE_CMD_BREAD			0xE4		// Read long sector command
#define	IDE_CMD_BWRITE			0xE8		// Write long sector command
#define	IDE_CMD_IDENTIFY		0xEC		// Identify drive command
#define	IDE_CMD_BUFFERMODE	0xEF		// Set features command
#define	IDE_CMD_DREAD			0xC8		// Read DMA
#define	IDE_CMD_DWRITE			0xCA		// Write DMA

// Status and alternate status register bits
#define	IDE_STB_BUSY			0x80		// Drive busy
#define	IDE_STB_READY			0x40		// Drive ready
#define	IDE_STB_WRFAULT		0x20		// Write fault
#define	IDE_STB_SEEKDONE		0x10		// Seek completed
#define	IDE_STB_DATAREQ		0x08		// Data request
#define	IDE_STB_CORRDATA		0x04		// Corrected data
#define	IDE_STB_INDEX			0x02		// Index mark
#define	IDE_STB_ERROR			0x01		// Error occurred

// Error register bits
#define	IDE_ERB_BADBLOCK		0x80		// Bad block
#define	IDE_ERB_UNCDATA		0x40		// Uncorrectable data
#define	IDE_ERB_IDNFOUND		0x10		// Id not found
#define	IDE_ERB_ABORTCMD		0x04		// Command aborted
#define	IDE_ERB_TK0NFOUND		0x02		// Track 0 not found
#define	IDE_ERB_AMNFOUND		0x01		// Address mark not found

#define	DRIVE_HEAD_INFO		0xA0		//sector size and master select

// Size of sectors
#define	BYTES_PER_SECTOR		0x200		// Number of bytes in a sector
#define	WORDS_PER_SECTOR		0x100		// Number of shorts in a sector
#define	LONGS_PER_SECTOR		0x80		// Number of ints in a sector

// Interrupt control
#define	IDE_INTS_OFF 			0x0		// Disable interrupts
#define	IDE_INTS_ON				0x1		// Enable interrupts

// Assorted status information
#define	IDE_DEVICE_INVALID	 	0x0	// Drive not connected
#define	IDE_DEVICE_CONNECTED		0x1	// Drive connected
#define	IDE_INVALID_PARTITION	0x2	// Invalid partition
#define	IDE_INVALID_BLOCK			0x3	// Invalide block for partition

/****************************************************************************/
/* Assorted Definitions                                                     */
/****************************************************************************/

// Offsets to the assorted registers in the disk controller
#define	DATA_OFFSET				0x0						// Data register
#define	PRECOMP_ERROR_OFFSET	0x1						// Error register
#define	SECTOR_COUNT_OFFSET	0x2						// Sector count register
#define	SECTOR_NUM_OFFSET		0x3						// Sector number register
#define	LOW_CYLINDER_OFFSET	0x4						// LSB of cylinder register
#define	HIGH_CYLINDER_OFFSET	0x5						// MSB of cylinder register
#define	DRIVE_HEAD_OFFSET		0x6						// Drive and head register
#define	COMMAND_OFFSET			0x7						// Command register
#define	STATUS_OFFSET			COMMAND_OFFSET			// Status register
#define	ALT_STATUS_OFFSET		0x2						// Alternate status register
#define	DEV_CONTROL_OFFSET	ALT_STATUS_OFFSET		// Device control register

// Addresses of the disk controller registers
#define	DRIVE_HEAD_REG			ide_base[DRIVE_HEAD_OFFSET]
#define	LOW_CYLINDER_REG		ide_base[LOW_CYLINDER_OFFSET]
#define	HIGH_CYLINDER_REG		ide_base[HIGH_CYLINDER_OFFSET]
#define	SECTOR_COUNT_REG		ide_base[SECTOR_COUNT_OFFSET]
#define	SECTOR_NUM_REG			ide_base[SECTOR_NUM_OFFSET]
#define	COMMAND_REG				ide_base[COMMAND_OFFSET]
#define	STATUS_REG				ide_base[STATUS_OFFSET]
#define	DATA_REG					(*((volatile unsigned int	*)(ide_base_address+DATA_OFFSET)))
#define	PRECOMP_ERROR_REG		ide_base[PRECOMP_ERROR_OFFSET]
#define	ALT_STATUS_REG			alt_base[ALT_STATUS_OFFSET]
#define	DEVICE_CONTROL_REG	alt_base[DEV_CONTROL_OFFSET]

// Assorted delta times (ticks) used in this module
#define DREQ_TIMEOUT    		(2*100)	// timeout for wait for DREQ
#define BUSY_TIMEOUT    		(5*100)	// timeout for ide_wait_bsy()
#define RESET_TIMEOUT   		(10*100)	// timeout for ide_soft_reset()
#define POWERUP_TIMEOUT 		(30*100)	// timeout for drive powerup
#define PART_READ_TIMEOUT		(25)		// timeout for reading partition table

#define PHYS(x)					((x) & 0x1fffffff)
#define M0_TIMING					0x85
#define CC_TIMING					0xb7

// Structure of information kept around to be able to use the drive
typedef struct device_desc
{
	int	select;			// drive select bit
	int	status;			// status of device connected
	int	cyls;				// number of cylinders
	int	heads;			// number of heads
	int	sectors;			// number of sectors per track
	int	max_multsect;	// maximum number of sectors on multi-sector r/w
	int	busy;				// drive currently busy
	int	dma_timing;		// nanosecs for DMA read/write
	int	pio_timing;		// nanosecs for PIO read/write
} DeviceDesc;


// Format of the data returned by the drive identify command
typedef struct hd_driveid
{
	unsigned short	config;				// Lots of obsolete bit flags
	unsigned short	cyls;					// Physical number of cylinders
	unsigned short	reserved2;			// Reserved
	unsigned short	heads;				// Physical number of heads
	unsigned short	track_bytes;		// Unformatted bytes per track
	unsigned short	sector_bytes;		// Unformatted bytes per sector
	unsigned short	sectors;				// Physical sectors per track
	unsigned short	vendor0;				// Vendor unique
	unsigned short	vendor1;				// Vendor unique
	unsigned short	vendor2;				// Vendor unique
	unsigned char	serial_no[20];		// Serial number ([0,1] == 0, not specified)
	unsigned short	buf_type;			// Vendor unique
	unsigned short	buf_size;			// 512 byte increments; 0 = not_specified
	unsigned short	vs_bytes;			// Number of vendor specific bytes on r/w longs
	unsigned char	fw_rev[8];			// Firmware revision
	unsigned char	model[40];			// Model number
	unsigned char	max_multsect;		// LSB max sectors on a r/w multiple; 0=not_implemented
	unsigned char	vendor3;				// MSB
	unsigned short	reserved48;			// Reserved
	unsigned char	vendor4;				// LSB
	unsigned char	capability;			// MSB bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup
	unsigned short	reserved50;			// Reserved
	unsigned char	vendor5;				// LSB
	unsigned char	tPIO;					// MSB 0=slow, 1=medium, 2=fast
	unsigned char	vendor6;				// LSB vendor unique
	unsigned char	tDMA;					// MSB 0=slow, 1=medium, 2=fast
	unsigned short	field_valid;		// Bits 0:cur_ok 1:eide_ok
	unsigned short	cur_cyls;			// Number of logical cylinders
	unsigned short	cur_heads;			// Number of logical heads
	unsigned short	cur_sectors;		// Number of logical sectors per track
	unsigned short	cur_capacity0;		// LSW logical total sectors on drive
	unsigned short	cur_capacity1;		// MSW logical total sectors on drive
	unsigned char	multsect;			// LSB current multiple sector count
	unsigned char	multsect_valid;	// MSB when (bit0==1) multsect is ok
	unsigned int	lba_capacity;		// Total number of sectors
	unsigned short	dma_1word;			// Single-word dma info
	unsigned short	dma_mword;			// Multiple-word dma info
	unsigned short	eide_pio_modes;	// Bits 0:mode3 1:mode4
	unsigned short	eide_dma_min;		// Min mword dma cycle time (ns)
	unsigned short	eide_dma_time;		// Recommended mword dma cycle time (ns)
	unsigned short	eide_pio;			// Min cycle time (ns), no IORDY
	unsigned short	eide_pio_iordy;	// Min cycle time (ns), with IORDY
} DriveID;


// NOTES about partitions:
// If a partition table exists on the drive:
//		Partition number 0 ALWAYS starts at block 0 and includes ALL blocks.
//		This partition is reserved for raw access to the drive.
//
//		All other partitions are usable by the user
//
// Format of the partition info structure
typedef struct partition_info
{
	int	starting_block;				// Logical starting block number
	int	num_blocks;						// Number of blocks in partition
	int	partition_type;				// Type of partition
} partition_info_t;

// Partition type definitions
#define	RAW_PARTITION		0			// Raw partition (No filesystem)
#define	FATFS_PARTITION	1			// FAT Filesystem partition
#define	EXT2FS_PARTITION	2			// Extended 2 Filesystem partition
#define	SWAP_PARTITION		3			// Swap Partition

// Format of the partition information stored on the disk at sector 0,
// head 0, cylinder 0.  This table occupies exactly 1 sector of the disk.
typedef struct partition_table
{
	int					magic_number; 		// The magic number 'PART'
	int					num_partitions;	// Number of partitions
	partition_info_t	partition[0];		// Partition info
} partition_table_t;

#define	PART_MAGIC	(('P'<<24)|('A'<<16)|('R'<<8)|('T'<<0))


// Function prototypes
int	ide_init(void);
int	ide_set_device(int);
int	ide_set_partition(int);
int	ide_reset(void);
int	ide_identify(unsigned int *);
int	ide_check_devstat(void);
int	ide_get_rpm(void);
int	ide_get_hdinfo(unsigned short *, unsigned short *, unsigned short *);
unsigned long ide_get_partition_size(void);
partition_table_t *ide_get_partition_table(void);
short	SecReads(unsigned long, unsigned long *, unsigned long);
short	SecWrites(unsigned long, unsigned long *, unsigned long);


typedef struct
{
	unsigned short	pci0646_VendorID __attribute__((packed));			// vendor ID
	unsigned short	pci0646_DeviceID __attribute__((packed));			// device ID
	unsigned short	pci0646_Command __attribute__((packed));			// command register
	unsigned short	pci0646_Status __attribute__((packed));			// status register
	unsigned char	pci0646_RevisionID __attribute__((packed));		// revision ID
	unsigned char	pci0646_ProgIface __attribute__((packed));		// programming interface
	unsigned char	pci0646_SubClass __attribute__((packed));			// sub-class code
	unsigned char	pci0646_ClassCode __attribute__((packed));		// class code
	unsigned char	pci0646_CacheLineSize __attribute__((packed));	// Cache Line Size
	unsigned char	pci0646_Latency __attribute__((packed));			// latency time in PCI clocks
	unsigned char	pci0646_HeadType __attribute__((packed));			// header type
	unsigned char	pci0646_BIST __attribute__((packed));				// Built in self test
	unsigned long	pci0646_BaseAddr0 __attribute__((packed));		// base address register 0
	unsigned long	pci0646_BaseAddr1 __attribute__((packed));		// base address register 1
	unsigned long	pci0646_BaseAddr2 __attribute__((packed));		// base address register 2
	unsigned long	pci0646_BaseAddr3 __attribute__((packed));		// base address register 3
	unsigned long	pci0646_BaseAddr4 __attribute__((packed));		// base address register 4
	unsigned long	pci0646_BaseAddr5 __attribute__((packed));		// base address register 5
	unsigned long	pci0646_Unused1[2] __attribute__((packed)); 		// NOT Used
	unsigned long	pci0646_RomExp __attribute__((packed));			// ROM Expansion
	unsigned long	pci0646_Unused2[2] __attribute__((packed));		// NOT Used
	unsigned char	pci0646_IntLine __attribute__((packed));			// interrupt line
	unsigned char	pci0646_IntPin __attribute__((packed));			// interrupt pin
	unsigned char	pci0646_MinGnt __attribute__((packed));			// Minimum Grant
	unsigned char	pci0646_MaxLat __attribute__((packed));			// Maximum Latency
	unsigned long	pci0646_Unused3[4] __attribute__((packed));		// NOT Used
	unsigned char	pci0646_Config __attribute__((packed));			// Configuration
	unsigned char	pci0646_Control_0_1 __attribute__((packed));		// Drive 0/1 Control
	unsigned char	pci0646_CmdTiming __attribute__((packed));		// Command Timing
	unsigned char	pci0646_AddrSetup_0 __attribute__((packed));		// Address Setup Timing (drive 0)
	unsigned char	pci0646_DACKTiming_0 __attribute__((packed));	// DACK Timing (drive 0)
	unsigned char	pci0646_AddrSetup_1 __attribute__((packed));		// Address Setup Timing (drive 1)
	unsigned char	pci0646_DACKTiming_1 __attribute__((packed));	// DACK Timing (drive 1)
	unsigned char	pci0646_Control_2_3 __attribute__((packed));		// Drive 2/3 Control
	unsigned char	pci0646_DACKTiming_2 __attribute__((packed));	// DACK Timing (drive 2)
	unsigned short	pci0646_ReadAheadCount __attribute__((packed));	// Read ahead count
	unsigned char	pci0646_DACKTiming_3 __attribute__((packed));	// DACK Timing (drive 3)
	unsigned long	pci0646_Unused4[5] __attribute__((packed));		// NOT Used
} PCI0646_CfigRegs;


typedef struct
{
	unsigned char	pci0646_BMIDECR0 __attribute__((packed));			// Bus Master IDE Command Register 0
	unsigned char	pci0646_MRDMODE __attribute__((packed));			// DMA Master Read Mode Select
	unsigned char	pci0646_BMIDESR0 __attribute__((packed));			// Bus Master IDE Status Register 0
	unsigned char	pci0646_Unused5 __attribute__((packed));			// NOT Used
	unsigned long	pci0646_DTPR0 __attribute__((packed));				// Desciptor Table Pointer 0
	unsigned char	pci0646_BMIDECR1 __attribute__((packed));			// Bus Master IDE Command Register 1
	unsigned char	pci0646_Unused6 __attribute__((packed));			// NOT Used
	unsigned char	pci0646_BMIDESR1 __attribute__((packed));			// Bus Master IDE Status Register 1
	unsigned char	pci0646_Unused7 __attribute__((packed));			// NOT Used
	unsigned long	pci0646_DTPR1 __attribute__((packed));				// Desciptor Table Pointer 1
} PCI0646_MastRegs;


typedef struct
{
	PCI0646_CfigRegs	pci0646_ConfigRegs;		// configuration registers
	PCI0646_MastRegs	pci0646_BusMastRegs;		// DMA bus mastering registers
} PCI0646_Regs;

#ifndef FALSE
#define	FALSE	0
#endif

#endif	// __PCI0646_H__
