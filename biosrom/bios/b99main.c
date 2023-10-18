//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>
#include	<ide.h>
#include	"find.h"

int	printf(const char *_format, ...);

char	__bios_version[] = {"$Revision: 4 $"};

void	(*exec_kernel)(char *, int) = (void (*))0xbfc00008;
void	(*debug_int)(int) = (void (*)(int))0x80000048;

void install_debug_hook(void);

static const char	*banner_strs[] = {
"\n\n\n",
"Phoenix Seattle R5000 BIOS for Blitz 99 - $Revision: 4 $\n",
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
#if defined(TEST)
		printf("%s", banner_strs[i]);
#else
		puts(banner_strs[i]);
#endif
	}
}

char	*get_rom_version(void)
{
	return(__bios_version);
}

void main(void)
{
#if defined(TEST)
	void	(*post_exit)(void) = (void (*)(void))0xbfc00008;
#else
	int	fd;
	int	dip;
	int	i;
	int	val;
	int	args[4] = {0, 0, 0, 0};
#endif

#ifndef TEST
	// Make sure the leds are off
	*MAIN_CONTROL |= (1<<14);
	*((volatile int *)LED_ADDR) = 7;

	// Install the debugging hooks
	install_debug_hook();

	// If we have a tty driver - display the banner on the terminal
	show_banner();

	// Make sure th vertical retrace interrupt is cleared
	*((volatile int *)VRETRACE_RESET_REG) = 1;

	// Enable the IDE Disk interrupt
	enable_ip(IDE_DISK_INT);

	// Make sure the GT641010 interrupts are reset
	*((volatile int *)GT_INT_CAUSE) = 0;

	// Enable the GT64010 interrupt
	enable_ip(GALILEO_INT);

	// Make sure the vertical retrace interrupt is disabled
	*((volatile int *)ICPLD_INT_ENBL_REG) &= ~VSYNC_INT_ENABLE;

	// Make sure the vertical retrace interrupt is cleared
	*((volatile int *)VRETRACE_RESET_REG) = 1;

	// Start feeding the watchdog
	start_dog_feed();

	// Attempt to open the scsi device
	if((fd = _open("scsi:", O_RDWR)) == -1)
	{
		dip = *((volatile int *)IOASIC_DIP_SWITCHES);
		if(!(dip & 0x8000))
		{
			puts("Booting diag.bin\r\n");
			exec("diag.exe", 0x800c0000, args);
		}

		// Tell 'em we are booting
		puts("Booting game.exe\r\n");

		// Execute the game
		exec("game.exe", 0x800c0000, args);

	}

	// Close the scsi driver
	_close(fd);

	// Tell 'em we are in debug mode
	puts("Debug Mode\r\n");

	// Finally - do the break to start debugger service
	__asm__("	break	0x402");

#else
	post_exit();
	printf("\f");
	show_banner();
	printf("\n\nP.O.S.T. Test\n\n");
#endif
}
