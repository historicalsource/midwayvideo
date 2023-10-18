//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 15 $
//
// $Author: Mlynch $
//

#include	<system.h>
#include	<ioctl.h>
#include <io.h>
#include	<filesys.h>
#include	<ide.h>

#define	USE_CRC

extern void				(*callback)(int);
extern unsigned int	proc, proc_save;
#if (!(PHOENIX_SYS & VEGAS))
extern int				in_debugger;
#endif
extern int				do_the_dog;
extern int				ints2handle;
extern int				user_int0_handler;
extern int				user_into_handler;

static FSFile			fs_file;
static struct ffblk	ffblk;

#ifdef USE_CRC
unsigned long crc(unsigned char *data, int len);
#endif

static int				args[4];
#ifdef USE_CRC
static unsigned long	stored_crc;
static unsigned long	gen_crc;
#endif
static unsigned long	oldp;

void	setup_tlb(void);
int get_scsi_device_number (void);

#if (!(PHOENIX_SYS & VEGAS))
int exec(char *file, unsigned int entry_address, int *arg_ptr)
{
	void	(*start_up)(void) = (void (*)(void))entry_address;
	unsigned long	exe_address;
	char	*tmp = file;

	// Start feeding the watchdog if not in debug mode
	start_dog_feed();

	args[0] = arg_ptr[0];
	args[1] = arg_ptr[1];
	args[2] = arg_ptr[2];
	args[3] = arg_ptr[3];

	// Make sure the filename is uppercase
	while(*tmp)
	{
		if(*tmp >= 'a' && *tmp <= 'z')
		{
			*tmp &= ~0x20;
		}
		++tmp;
	}

	// Set the process pointer
	proc = 0;

	// Fake it into thinkin' we are in the debugger
	in_debugger = 1;

	// Shut down the hardware interrupts
	if(!do_the_dog)
	{
		disable_ip(SCSI_INT);
		disable_ip(IDE_DISK_INT);
		disable_ip(IO_ASIC_INT);
		disable_ip(NSS_INT);
	}
	else
	{
		disable_ip(VERTICAL_RETRACE_INT);
		disable_ip(SCSI_INT);
		disable_ip(IDE_DISK_INT);
		disable_ip(IO_ASIC_INT);
		disable_ip(NSS_INT);
	}

	enable_ip(GALILEO_INT);

	// Unhook the application level interrupt vectors
	unhook_vectors();

	// Reinitialize the user vector table
	init_user_vectors();

	// Close all open files
	closeall();

	// Reset the disk queues
	reset_disk_que();

	// Turn off the vertical retrace interrupts
	disable_ip(VERTICAL_RETRACE_INT);

	// Make sure they are unlatched from the latch too
	*((volatile int *)VRETRACE_RESET_REG) = 1;

	// Disable the GT64010 DMA's
	*((volatile int *)GT_DMA_CHAN0_CONTROL) &= ~(1<<12);
	*((volatile int *)GT_DMA_CHAN1_CONTROL) &= ~(1<<12);
	*((volatile int *)GT_DMA_CHAN2_CONTROL) &= ~(1<<12);
	*((volatile int *)GT_DMA_CHAN3_CONTROL) &= ~(1<<12);

	// Initialize the disk callback function
	callback = (void (*)(int))0;	// Callback function

	install_disk_timeout_audit(0);

	// Remove any user installed drivers
	clear_user_drivers();

	// Unistall any user installed integer div handlers
	user_int0_handler = 0;
	user_into_handler = 0;

	// Get the information about the file
	if(FSGetFFblk(file, &ffblk))
	{
		return(0);
	}

	// Open the file
	if(FSOpen(&fs_file, file))
	{
		return(1);
	}

#ifdef USE_CRC
	// Read the CRC value
	if(FSRead(&fs_file, &stored_crc, 1) != 4)
	{
		FSClose(&fs_file);
		return(5);
	}
#endif

	// Read the execute address
	if(FSRead(&fs_file, &exe_address, 1) != 4)
	{
		FSClose(&fs_file);
		return(2);
	}

	start_up = (void (*)(void))exe_address;

#ifdef USE_CRC
	// Seek Back 1 word
	FSSeek(&fs_file, 1, &oldp);
#endif

	// Read the file into the entry address
	if(FSRead(&fs_file, (unsigned long *)exe_address, (ffblk.ff_fsize - 4) / 4) != (ffblk.ff_fsize - 4))
	{
		FSClose(&fs_file);
		return(2);
	}

	// Close the file
	FSClose(&fs_file);

	// Flush the caches
	flush_cache();

#ifdef USE_CRC
	// Generate the CRC
	gen_crc = crc((unsigned char *)exe_address, (ffblk.ff_fsize - 4));

	// Check the CRC
	if(stored_crc != gen_crc)
	{
		return(6);
	}

	memcpy((void *)exe_address, (void *)(exe_address+4), ffblk.ff_fsize - 8);
#endif

	in_debugger = 0;

	// Jump to the code
	start_up();

	// Dummy return
	return(3);
}
#else
#ifndef TEST
extern volatile unsigned long long	vsync_timestamp;
#endif

int exec(char *file, unsigned int entry_address, int *arg_ptr)
{
	void	(*start_up)(void) = (void (*)(void))entry_address;
	unsigned long	exe_address;
	char	*tmp = file;

#ifndef TEST
	vsync_timestamp = 0L;
#endif

printf("exec(%s, %x, %p)\n", file, entry_address, arg_ptr);

	args[0] = arg_ptr[0];
	args[1] = arg_ptr[1];
	args[2] = arg_ptr[2];
	args[3] = arg_ptr[3];

	// Disable the debugging interrupt
	disable_ip(SCSI_INT);

	// Make sure the filename is uppercase
	while(*tmp)
	{
		if(*tmp >= 'a' && *tmp <= 'z')
		{
			*tmp &= ~0x20;
		}
		++tmp;
	}

	// Set the process pointer
	proc = 0;

	// Clear out any user installed interrupt handlers
	clear_user_handlers();

	// Close all open files
	closeall();

	// Reset the disk queues
	reset_disk_que();

	// Initialize the disk callback function
	callback = (void (*)(int))0;	// Callback function

	// Remove any user installed drivers
	clear_user_drivers();

	// Get the information about the file
	if(FSGetFFblk(file, &ffblk))
	{
		printf("Can not find file: %s", file);
		while(1) ;
		return(0);
	}

	// Open the file
	if(FSOpen(&fs_file, file))
	{
		printf("Can not open file: %s", file);
		while(1) ;
		return(1);
	}

	// Read the CRC value
	if(FSRead(&fs_file, &stored_crc, 1) != 4)
	{
		FSClose(&fs_file);
		return(5);
	}

	// Read the execute address
	if(FSRead(&fs_file, &exe_address, 1) != 4)
	{
		printf("Can not read file execute address\n");
		FSClose(&fs_file);
		while(1) ;
		return(2);
	}

	// Set the start up pointer
	start_up = (void (*)(void))exe_address;

	// Seek Back 1 word
	FSSeek(&fs_file, 1, &oldp);

printf("Execute address: %X\n", exe_address);

	// Read the file into the entry address
	if(FSRead(&fs_file, (unsigned long *)exe_address, ffblk.ff_fsize / 4) != (ffblk.ff_fsize - 4))
	{
		printf("Can not read all of file: %s\n", file);
		FSClose(&fs_file);
		while(1) ;
		return(2);
	}

	// Close the file
	FSClose(&fs_file);

	// Invalidate the instruction caches
	invalidate_icache();

	// Generate the CRC
	gen_crc = crc((unsigned char *)exe_address, (ffblk.ff_fsize - 4));

	// Check the CRC
	if(stored_crc != gen_crc)
	{
		return(6);
	}

	memcpy((void *)exe_address, (void *)(exe_address+4), ffblk.ff_fsize - 8);

printf("Executing: %s\n", file);

	disable_write_merge();

	// Jump to the code
	start_up();

	printf("WHOA - Startup should NOT return\n");
	while(1) ;

	// Dummy return
	return(3);
}
#endif


int rom_diag_exec(unsigned int entry_address, int *arg_ptr)
{
	void	(*start_up)(void) = (void (*)(void))entry_address;
	int	*to;
	int	*from;
	int	i;

	// Initialize the translation lookaside buffers
	setup_tlb();

#if defined(TTY_DRIVER)
	// Open the standard input device
	_open("con:", O_RDONLY);			// stdin device

	// Open the standard output device
	_open("con:", O_WRONLY);			// Stdout device

	// Open the standard error device
	_open("con:", O_WRONLY);			// stderr device
#endif
	// Open the standard cmos device
	i = _open("cmos:", O_RDWR);

	// Open the standard asic device
	i = _open("ioasic:", O_RDWR);

	// Open the standard timer device
	i = _open("timer:", O_RDWR);

	args[0] = arg_ptr[0];
	args[1] = arg_ptr[1];
	args[2] = arg_ptr[2];
	args[3] = arg_ptr[3];

	// Set the process pointer
	proc = 0;

	// Fake it into thinkin' we are in the debugger
#if (!(PHOENIX_SYS & VEGAS))
	in_debugger = 1;
#endif

	// Shut down the hardware interrupts
	if(!do_the_dog)
	{
		disable_interrupts();
	}
	else
	{
#if (!(PHOENIX_SYS & VEGAS))
		disable_ip(VERTICAL_RETRACE_INT);
		disable_ip(SCSI_INT);
		disable_ip(IDE_DISK_INT);
#endif
	}

	// Unhook the application level interrupt vectors
#if (!(PHOENIX_SYS & VEGAS))
	ints2handle = 0xffff0200;

	// Reinitialize the user vector table
	init_user_vectors();
#else
	clear_user_handlers();
#endif

	// Close all open files
	closeall();

	// Reset the disk queues
	reset_disk_que();

	// Turn off the vertical retrace interrupts
#if (!(PHOENIX_SYS & VEGAS))
	disable_ip(VERTICAL_RETRACE_INT);

	// Make sure they are unlatched from the latch too
	*((volatile int *)VRETRACE_RESET_REG) = 1;
#endif

#if (!(PHOENIX_SYS & VEGAS))
	// Disable the GT64010 DMA's
	*((volatile int *)GT_DMA_CHAN0_CONTROL) &= ~(1<<12);
	*((volatile int *)GT_DMA_CHAN1_CONTROL) &= ~(1<<12);
	*((volatile int *)GT_DMA_CHAN2_CONTROL) &= ~(1<<12);
	*((volatile int *)GT_DMA_CHAN3_CONTROL) &= ~(1<<12);

	// Make sure the GT64010 interrupts are cleared
	*((volatile int *)GT_INT_CAUSE) &= ~0x100;
#endif

	// Initialize the disk callback function
	callback = (void (*)(int))0;	// Callback function

	// Remove any user installed drivers
	clear_user_drivers();

	// Unistall any user installed integer div handlers
#if (!(PHOENIX_SYS & VEGAS))
	user_int0_handler = 0;
	user_into_handler = 0;
#endif

	// Copy the data from the second ROM to memory
	to = (int *)(entry_address | 0xa0000000);
	from = (int *)0xbfd00000;
	for(i = 0; i < (1<<17); i++)
	{
		*to++ = *from++;
	}

	// Flush the caches
	flush_cache();

#if (!(PHOENIX_SYS & VEGAS))
	in_debugger = 0;
#endif

	// Jump to the code
	start_up();

	// Dummy return
	return(3);
}


int *get_arg_ptr(void)
{
	return(args);
}


static unsigned long crctab[] = {
	0x0,
	0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6,
	0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
	0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f,
	0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a,
	0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58,
	0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033,
	0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
	0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4,
	0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5,
	0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
	0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c,
	0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1,
	0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b,
	0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698,
	0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
	0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f,
	0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80,
	0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
	0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629,
	0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c,
	0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e,
	0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65,
	0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
	0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2,
	0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74,
	0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
	0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a,
	0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087,
	0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d,
	0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce,
	0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
	0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09,
	0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf,
	0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


/*
** crc() - This function is used to calculate a crc32 value on an array
** of data in memory.
*/
unsigned long crc(unsigned char *data, int len)
{
	register int		i;
	register unsigned long	crc;

	crc = 0;
	for(i = 0; i < len; i++)
	{
		crc = crc << 8 ^ crctab[crc >> 24 ^ (*data)];
		data++;
	}
	while(len)
	{
		crc = crc << 8 ^ crctab[crc >> 24 ^ (len & 0xff)];
		len >>= 8;
	}
	return(~crc);
}

