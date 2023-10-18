//
// Until the Lymie bastards fix it -- leave this on
//
#define	THE_PIECE_O_SHIT_PSYQ_BIOS_IS_BROKEN	1


// Turn this on if your going to do something really stupid like use a
// 16 bit compiler to build this stuff
//
// WARNING - If you do use a 16bit compiler you'll have to modify all of the
// assorted source files to for 16 bit support.  I've not done this because
// I use a real compiler with built in DPMI services (GNU).
#if defined(__GNUC__)
#define	USE_BULLSHIT_16BIT_COMPILER	0

#elif defined(__WATCOMC__)
#error	You are an idiot for using a compiler other than GNU
#define	USE_BULLSHIT_16BIT_COMPILER	1

#elif defined(_MSC_VER)
#error	You are really and idiot for using a microsoft compiler
#define	USE_BULLSHIT_16BIT_COMPILER	1

#endif

// Interrupts used to access DOS and the target
#define	DOS_INT			0x21
#define	PHOENIX_INT		0x7e

// Custom requests for the target defined by ME
#define	GET_PARTITION_TABLE	0xf8
#define	WRITE_SECTORS			0xf9
#define	READ_SECTORS			0xfa
#define	SET_FILE_TIMESTAMP	0xee
#define	GET_DISK_FREE			0xcb
#define	WRITE_MEMORY			0x1a
#define	READ_MEMORY				0x19
#define	GET_ID					0x11
#define	GET_INFO					0x06
#define	GET_VERSION				0x0d
#define	RESET_TARGET			0x22
#define	INIT_SERVER				0x1d
#define	GET_REGISTERS_ADDR	0x14
#define	NOP_COMMAND				0x10
#define	SOUND_WRITE				0xc0

// General purpose scsi command used for sending custom requests
#define	GENERAL_SCSI_CMD		0x1e
#define	ENABLE_COMMAND			0x1c

// SCSI unit number of the phoenix target
#define	PHOENIX_UNIT_NUMBER	0x0

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
	unsigned long	starting_block; 		// Logical starting block number
	unsigned long	num_blocks;			 	// Number of blocks in partition
	unsigned long	partition_type;	 	// Type of partition
} partition_info_t;

// Partition type definitions
#define	RAW_PARTITION		0				// Raw partition (No filesystem)
#define	FATFS_PARTITION	1				// FAT Filesystem partition
#define	EXT2FS_PARTITION	2				// Extended 2 Filesystem partition
#define	SWAP_PARTITION		3				// Swap Partition

// Format of the partition information stored on the disk at sector 0,
// head 0, cylinder 0.  This table occupies exactly 1 sector of the disk.
typedef struct partition_table
{
	unsigned long		magic_number; 		// The magic number 'PART'
	unsigned long		num_partitions;	// Number of partitions
	partition_info_t	partition[0];		// Partition info
} partition_table_t;

// Definition of what's in the magic number field above
#define	PART_MAGIC	(('P'<<24)|('A'<<16)|('R'<<8)|('T'<<0))

#if (!(USE_BULLSHIT_16BIT_COMPILER & 1))
typedef struct gen_scsi_cmd
{
	short		cmdlen;
	short		cmd_offset;
	short		cmd_segment;
	short		data_in_len;
	short		data_in_offset;
	short		data_in_segment;
	short		data_out_len;
	short		data_out_offset;
	short		data_out_segment;
} gen_scsi_cmd_t;
#else
typedef struct gen_scsi_cmd
{
	short			cmdlen;
	char far		*cmd_buffer;
	short			data_in_len;
	char far		*data_in_buffer;
	short			data_out_len;
	char far		*data_out_buffer;
} gen_scsi_cmd_t;
#endif

// Function prototypes
unsigned int _pd_close(int handle);
unsigned int _pd_commit(int handle);
unsigned int _pd_creat(const char *filename, unsigned int attr, int *handle);
unsigned int _pd_creatnew(const char *filename, unsigned int attr, int *handle);
int _pdexterr(struct _DOSERROR *p_error);
unsigned int _pd_findfirst(char *name, unsigned int attr, struct _find_t *result);
unsigned int _pd_findnext(struct _find_t *result);
void _pd_getdate(struct _dosdate_t *date);
unsigned int _pd_getdiskfree(unsigned int drive, struct _diskfree_t *diskspace);
void _pd_getdrive(unsigned int *p_drive);
unsigned int _pd_getfileattr(const char *filename, unsigned int *p_attr);
unsigned int _pd_getftime(int handle, unsigned int *p_date, unsigned int *p_time);
void _pd_gettime(struct _dostime_t *time);
unsigned int _pd_open(const char *filename, unsigned int mode, int *handle);
unsigned int _pd_read(int handle, void *buffer, unsigned int count, unsigned int *result);
unsigned int _pd_setdate(struct _dosdate_t *date);
void _pd_setdrive(unsigned int drive, unsigned int *p_drives);
unsigned int _pd_setfileattr(const char *filename, unsigned int attr);
unsigned int _pd_setftime(int handle, unsigned int date, unsigned int time);
unsigned int _pd_settime(struct _dostime_t *time);
unsigned int _pd_write(int handle, const void *buffer, unsigned int count, unsigned int *result);
int get_sys_by_name(const char *);
int get_sys(int);
int get_sys_by_drive(unsigned int);
void set_system(int, int);
char *get_fname_base(const char *);
int getptbl(partition_table_t *);
int write_sectors(int, const void *, unsigned int, unsigned int);
int read_sectors(int, const void *, unsigned int, unsigned int);
int check_driver(void);
int _fnsplit(const char *, char *, char *, char *, char *);
int psyq_mem_write(unsigned long, unsigned long);
int psyq_mem_read(unsigned long, unsigned long *);
int get_target_id(char *, char *);
int get_target_info(int *);
int get_target_version(int	*, int *);
int psyq_reset(void);
int psyq_init_server(void);
int psyq_halt(void);
int psyq_get_regs_addr(void);
int psyq_resume(void);
int psyq_blk_write(unsigned long, char *, int);
int psyq_blk_read(unsigned long, char *, int);
int psyq_enable(void);
int unlock_ioasic(void);
int pdremove(const char *);
int psyq_snd_write(char *, int);

