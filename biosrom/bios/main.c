//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 28 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>
#include	<ide.h>
#include	"find.h"

int	printf(const char *_format, ...);

char	__bios_version[] = {"$Revision: 28 $"};

void	(*exec_kernel)(char *, int) = (void (*))0xbfc00008;
void	(*debug_int)(int) = (void (*)(int))0x80000048;

void install_debug_hook(void);
int test_memory(void);

static int	ide_creg[22];

#if defined(TTY_DRIVER)
static const char	*banner_strs[] = {
"\n\n\n",
"Phoenix ",
#if (PHOENIX_SYS & SA1)
"SA1",
#elif (PHOENIX_SYS & SEATTLE)
"Seattle",
#elif (PHOENIX_SYS & FLAGSTAFF)
"Flagstaff",
#elif (PHOENIX_SYS & VEGAS)
"Vegas",
#endif
" R5000 ",
"BIOS  -  ",
"$Revision: 28 $\n",
"Copyright (c) 1997,1998 by Midway Video Inc.\n\n"
};

void puts(const char *str)
{
	char	cr = '\r';

	while(*str)
	{
		if(*str == '\n')
		{
			_write(1, &cr, 1);
		}
		_write(1, str, 1);
		++str;
	}
}

void show_banner(void)
{
	int	i;

	for(i = 0; i < sizeof(banner_strs)/sizeof(void *); i++)
	{
		puts(banner_strs[i]);
	}
}

#if (PHOENIX_SYS & VEGAS)
void show_banner1(void)
{
	int	i;

	for(i = 0; i < sizeof(banner_strs)/sizeof(void *); i++)
	{
		printf("%s", banner_strs[i]);
	}
}
#endif

#endif

void show_addr(int);

#define	show_data	show_addr

static void	(*reset_func)(void) = (void (*)(void))0xbfc00000;
static void	(*game_entry)(void) = (void (*)(void))0x80040000;

char	*get_rom_version(void)
{
	return(__bios_version);
}

static char	*exec_name = "game.bin";

#if defined(TEST)
void	(*post_exit)(void) = (void (*)(void))0xbfc00008;
#endif

void main(void)
{
	int	fd;
	int	dip;
	int	i;
	int	val;
	int	args[4] = {0, 0, 0, 0};
#ifndef NEW_TEST
#if (!(PHOENIX_SYS & SA1))
	void	(*post_test)(void) = (void (*)(void))0xbfc00008;
#endif
#endif


	// Install the debugging hooks
#if (!(PHOENIX_SYS & VEGAS))
	install_debug_hook();
#endif

	// If we have a tty driver - display the banner on the terminal
#if defined(TTY_DRIVER)
#if (!(PHOENIX_SYS & VEGAS))
	show_banner();
#endif
#endif

	// Indicate something on the LEDs
#if (PHOENIX_SYS & SA1)
	*((volatile int *)LED_ADDR) = 0x80;
#elif (PHOENIX_SYS & SEATTLE)
	*((volatile int *)LED_ADDR) = 0x3;
#elif (PHOENIX_SYS & VEGAS)
	*((volatile char *)LED_STATUS_REG_ADDR) = 0xf;
#endif


#if (!(PHOENIX_SYS & VEGAS))
	// Make sure th vertical retrace interrupt is cleared
	*((volatile int *)VRETRACE_RESET_REG) = 1;
#endif
	// Enable the IDE Disk interrupt
	enable_ip(IDE_DISK_INT);

#if (PHOENIX_SYS & VEGAS)
	enable_ip(SIO_INT);
	enable_ip(NILE4_INT);
	enable_ip(SCSI_INT);
#endif

//#if (!(PHOENIX_SYS & VEGAS))
#ifndef TEST

#if (!(PHOENIX_SYS & VEGAS))
	// Make sure the GT641010 interrupts are reset
	*((volatile int *)GT_INT_CAUSE) = 0;

	// Enable the GT64010 interrupt
	enable_ip(GALILEO_INT);
#endif

	// Attempt to open the scsi device
	if((fd = _open("scsi:", O_RDWR)) == -1)
	{
#if (!(PHOENIX_SYS & SA1))
		// Read the dipswitches
		dip = *((volatile int *)IOASIC_DIP_SWITCHES);

		// Should the power up tests be run at all ?
		if(dip & 0x4000)
		{
#if (!(PHOENIX_SYS & VEGAS))
			// Disable the GT64010 interrupts
			disable_ip(GALILEO_INT);
#endif

			// Save off the IDE controller Configuration registers
			for(i = 0; i < sizeof(ide_creg)/sizeof(int); i++)
			{
				pciGetConfigData(i*4, PC87415_DEVICE_NUMBER, &ide_creg[i]);
			}

			// Run the power on self tests
			post_test();

			// Disable the vertical retrace interrupt
			disable_ip(VERTICAL_RETRACE_INT);

			// Make sure the vertical retrace interrupt is cleared
			*((volatile int *)VRETRACE_RESET_REG) = 1;

			// Restore IDE controller registers that got trashed
			for(i = 0; i < sizeof(ide_creg)/sizeof(int); i++)
			{
				pciGetConfigData(i*4, PC87415_DEVICE_NUMBER, &val);
				if(val != ide_creg[i])
				{
					pciSetConfigData(i*4, 9, &ide_creg[i]);
				}
			}

			// Make sure no interrupts are pending
			(void)STATUS_REG;

			// Enable the IDE Disk interrupt
			enable_ip(IDE_DISK_INT);

#if (!(PHOENIX_SYS & VEGAS))
			// Make sure the GT641010 interrupts are reset
			*((volatile int *)GT_INT_CAUSE) = 0;

			// Enable the GT64010 interrupt
			enable_ip(GALILEO_INT);
#endif
		}
#endif

#ifdef ENABLE_WATCHDOG
		if((fd = _open("wdog:", O_RDONLY)) >= 0)
		{
			_ioctl(fd, FIOCWALKTHEDOG, 0);
			_close(fd);
		}
#endif

		dip = *((volatile int *)IOASIC_DIP_SWITCHES);
		if(!(dip & 0x8000))
		{
			exec_name = "diag.bin";
		}

		// Tell 'em we are booting
		puts("Booting ");
		puts(exec_name);
		puts("\r\n");


		// Execute the game
		exec(exec_name, 0x800c0000, args);

	}

	// Close the scsi driver
	_close(fd);

	// Tell 'em we are in debug mode
	puts("Debug Mode\r\n");

	// Finally - do the break to start debugger service
	__asm__("	break	0x402");
#else
	printf("\f");
#if (PHOENIX_SYS & VEGAS)
	show_banner1();
#else
	show_banner();
	printf("\n\nP.O.S.T. Test\n\n");
#endif
#if defined(TEST)
	post_exit();
#else
	printf("Ready for debugging\n");
	__asm__("	break	0x402");
#endif
#endif
}
