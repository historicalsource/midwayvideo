//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 7 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>
#include	<ide.h>
#include	"find.h"

int	printf(const char *_format, ...);

char	__bios_version[] = {"$Revision: 7 $"};

extern int	debug_capable;

static const char	*banner_strs[] = {
"\n\n\n",
#ifndef TEST
"Phoenix Vegas R5000 BIOS - $Revision: 7 $\n",
#else
"Phoenix Vegas R5000 POST - $Revision: 7 $\n",
#endif
__DATE__,
"  ",
__TIME__,
"\n",
"Copyright (c) 1997,1998,1999 by Midway Video Inc.\n",
"All rights reserved\n\n"
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

#ifndef TEST
extern volatile unsigned long long	vsync_timestamp;

void show_help(void)
{
	printf("Commands\n");
	printf("B - Boot game from disk\n");
	printf("D - Disk Utilities\n");
	printf("G - Use GDB Debugging\n");
	printf("\n");
}

void show_disk_help(void)
{
	printf("\nDisk Commands\n");
	printf("P - Partitioning tool\n");
	printf("F - Format partition\n");
	printf("Q - Back to main menu\n");
	printf("\n");
}

void show_partition_help(void)
{
	printf("\nParitioning Commands\n");
	printf("S - Show partitioning info\n");
	printf("D - Delete last partition\n");
	printf("A - Add partition\n");
	printf("W - Write partition info\n");
	printf("Q - Return to disk menu\n");
	printf("\n");
}

#ifdef TTY_INTERRUPTS
int UARTgetc(void)
{
	unsigned char	c;

	if(!_read(0, &c, 1))
	{
		return(0);
	}
	return((int)c);
}
#endif

#define	LONG_MIN	0x80000000
#define	LONG_MAX	0x7fffffff

#define __dj_ISALNUM 	0x0001
#define __dj_ISALPHA	0x0002
#define __dj_ISCNTRL	0x0004
#define __dj_ISDIGIT	0x0008
#define __dj_ISGRAPH	0x0010
#define __dj_ISLOWER	0x0020
#define __dj_ISPRINT	0x0040
#define __dj_ISPUNCT	0x0080
#define __dj_ISSPACE	0x0100
#define __dj_ISUPPER	0x0200
#define __dj_ISXDIGIT	0x0400

extern unsigned short __dj_ctype_flags[];

#define isspace(c) (__dj_ctype_flags[((c)&0xff)+1] & __dj_ISSPACE)
#define isdigit(c) (__dj_ctype_flags[((c)&0xff)+1] & __dj_ISDIGIT)
#define isalpha(c) (__dj_ctype_flags[((c)&0xff)+1] & __dj_ISALPHA)
#define isupper(c) (__dj_ctype_flags[((c)&0xff)+1] & __dj_ISUPPER)

static long strtol(const char *nptr, int base)
{
  const char *s = nptr;
  unsigned long acc;
  int c;
  unsigned long cutoff;
  int neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-')
  {
    neg = 1;
    c = *s++;
  }
  else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X'))
  {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */
  cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++)
  {
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else
    {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0)
  {
    acc = neg ? LONG_MIN : LONG_MAX;
  }
  else if (neg)
    acc = -acc;
  return acc;
}

int get_dinput(int min, int max)
{
	char	str[24];
	int	i = 0;
	int	val = 0;
	int	c;

	while(1)
	{
		while((c = UARTgetc()) == 0) ;
		c &= 0xff;
		if(c == 0xd || c == 0xa)
		{
			printf("\n");
			str[i] = 0;
			val = strtol(str, 10);
			if(val < min)
			{
				printf("Error - Input must be greater than %d\nRetry:  ", min);
			}
			else if(val > max)
			{
				printf("Error - Input must be less than %d\nRetry:  ", max);
			}
			return(val);
		}
		else if(c < '0' || c >= '9')
		{
			printf("\nError - Input must be in range of 0 - 9\nRetry:  ");
			i = 0;
		}
		else
		{
			printf("%c", c);
			str[i] = c;
			i++;
			if(i >= sizeof(str) - 1)
			{
				i = sizeof(str) - 2;
			}
		}
	}
}

void do_partition_menu(void)
{
	partition_table_t	*pt = ide_get_partition_table();
	int					c = 0;
	int					i;
	unsigned int		total_blocks;
	unsigned int		num_blocks;

	show_partition_help();
	while(1)
	{
		printf("Partitioning Command: ");
		while((c = UARTgetc()) == 0) ;
		c &= 0xff;
		if(c >= 'a' && c <= 'z')
		{
			c &= ~0x20;
		}
		printf("%c\n", c);
		switch(c)
		{
			case 'D':	// Delete a partition
			{
				if(pt->num_partitions > 1)
				{
					pt->num_partitions--;
					printf("\nPartition %d deleted\n", pt->num_partitions);
				}
				else
				{
					printf("\nNo partitions left to delete\n");
				}
			}
			case 'S':	// Show current partitioning info
			{
show_info:
				printf("\nCurrent partition info\n");
				if(pt->num_partitions > 1)
				{
					for(i = 1; i < pt->num_partitions; i++)
					{
						printf("# %d - Mbytes: %04d - type: %s\n",
							i,
							(pt->partition[i].num_blocks * 512)/(1024*1024),
							pt->partition[i].partition_type == RAW_PARTITION ? "RAW" : "FATFS");
					}
				}
				else
				{
					printf("There are no partitions on this drive\n");
				}
				printf("\n");
				break;
			}
			case 'A':	// Add a partition
			{
				printf("\nAdd parition\n");
				total_blocks = 16;
				for(i = 1; i < pt->num_partitions; i++)
				{
					total_blocks += pt->partition[i].num_blocks;
				}
				total_blocks = pt->partition[0].num_blocks - total_blocks;
				printf("Blocks available %d - %d Mb\n",
					total_blocks,
					(total_blocks*512)/(1024*1024));
				printf("Enter desired size (blocks - 2=1k): ");
				num_blocks = get_dinput(0, total_blocks);
				if(pt->num_partitions == 1)
				{
					pt->partition[pt->num_partitions].starting_block = 16;
				}
				else
				{
					pt->partition[pt->num_partitions].starting_block =
						pt->partition[pt->num_partitions-1].starting_block +
						pt->partition[pt->num_partitions-1].num_blocks;
				}
				pt->partition[pt->num_partitions].num_blocks = num_blocks;
				total_blocks = 16;
				for(i = 1; i <= pt->num_partitions; i++)
				{
					total_blocks += pt->partition[i].num_blocks;
				}
				if(total_blocks >= pt->partition[0].num_blocks)
				{
					printf("\nTotal blocks used is more than drive capacity\n\n");
				}
				else
				{
					printf("Partition type (R = raw, F = File, S = Swap, Q = Quit):  ");
					while((c = UARTgetc()) == 0) ;
					c &= 0xff;
					printf("%c\n", c);
					if(c == 'r' || c == 'R')
					{
						pt->partition[pt->num_partitions].partition_type = RAW_PARTITION;
					}
					else if (c == 'f' || c == 'F')
					{
						pt->partition[pt->num_partitions].partition_type = FATFS_PARTITION;
					}
					else if (c == 's' || c == 'S')
					{
						pt->partition[pt->num_partitions].partition_type = SWAP_PARTITION;
					}
					else if (c == 'q' || c == 'Q')
					{
						printf("\n\n");
						goto show_info;
					}
					else
					{
						printf("Invalid entry - must be R, F, S, or Q\n\n");
						goto show_info;
					}
					pt->num_partitions++;
					printf("\n\n");
				}
				goto show_info;
			}
			case 'W':	// Write the new partition info
			{
				printf("\nWriting partition table...");
				_SecWrites(0, pt, 1);
				ide_allow_init();
				ide_init();
				FSInit();
				printf("\nPartition table written\n\n");
				break;
			}
			case 'Q':	// Done
			{
				ide_allow_init();
				ide_init();
				FSInit();
				return;
			}
			default:
			{
				printf("\n");
				show_partition_help();
				break;
			}
		}
	}
}

void format_disk_partition(void)
{
	partition_table_t	*pt = ide_get_partition_table();
	int					i;
	int					c;

	printf("\n");
	printf("Formatable partition numbers:");
	for(i = 1; i < pt->num_partitions; i++)
	{
		if(pt->partition[i].partition_type == FATFS_PARTITION)
		{
			printf(" %d", i);
			if(i < (pt->num_partitions-1))
			{
				printf(",");
			}
		}
	}
ptentry_retry:
	printf("\nEnter partition number to format (q to quit): ");
	while((c = UARTgetc()) == 0) ;
	c &= 0xff;
	printf("%c\n", c);
	if(c >= 'a' && c <= 'z')
	{
		c &= ~0x20;
		if(c == 'Q')
		{
			return;
		}
	}
	else if(c >= 'A' && c <= 'Z')
	{
		if(c == 'Q')
		{
			return;
		}
	}
	else if(c <= '0' || c > '9')
	{
		printf("\nInvalid entry: %c\n");
		goto ptentry_retry;
	}
	c &= 0xf;
	if(c <= 0 || c >= pt->num_partitions)
	{
		printf("\nInvalid partition number: %d\n", c);
		goto ptentry_retry;
	}

	printf("Formatting partition %d...", c);

	ide_set_partition(c);
	FSFormat();

	ide_allow_init();
	ide_init();
	FSInit();

	printf("\nPartition %d formatted\n", c);
}
	

void do_disk_menu(void)
{
	int	c = 0;

	show_disk_help();
	while(1)
	{
		printf("Disk Command: ");
		while((c = UARTgetc()) == 0) ;
		c &= 0xff;
		if(c >= 'a' && c <= 'z')
		{
			c &= ~0x20;
		}
		printf("%c\n", c);
		switch(c)
		{
			case 'P':	// Partitioning tool
			{
				do_partition_menu();
				show_disk_help();
				break;
			}
			case 'F':	// Disk formatter
			{
				format_disk_partition();
				show_disk_help();
				break;
			}
			case 'Q':	// Done
			{
				return;
			}
			default:
			{
				printf("\n");
				show_disk_help();
				break;
			}
		}
	}
}

static char	str_buf[32];

char *get_string(void)
{
	int	i = 0;
	int	c;

	while(1)
	{
		while((c = UARTgetc()) == 0) ;
		c &= 0xff;
		if(c == 0xd || c == 0xa)
		{
			printf("\n");
			str_buf[i] = 0;
			return(str_buf);
		}
		printf("%c", c);
		str_buf[i] = c;
		i++;
		if(i >= sizeof(str_buf) - 1)
		{
			i = sizeof(str_buf) - 2;
		}
	}
}


void do_menu(void)
{
	int	c = 0;
	int	args[4] = {0, 0, 0, 0};
	char	*str;

	show_help();
	printf("Ready for commands or PSYQ debugging\n");
	while(1)
	{
		printf("Main Command: ");
		while((c = UARTgetc()) == 0) ;
		c &= 0xff;
		if(c >= 'a' && c <= 'z')
		{
			c &= ~0x20;
		}
		printf("%c\n", c);
		switch(c)
		{
			case 'B':	// Boot game from disk
			{
				printf("Enter name:  ");
				str = get_string();
				printf("Booting %s\n\n", str);
				exec(str, 0x800c0000, args);
				break;
			}
			case 'D':	// Disk utility
			{
				do_disk_menu();
				show_help();
				break;
			}
			default:
			{
				printf("\n\n");
				show_help();
				break;
			}
		}
	}
}
#endif

#ifdef TTY_INTERRUPTS
extern int	(*myputc)(int);

int putc(int c)
{
	char	ch;

	ch = c;
	return(_write(1, &ch, 1));
}

void init_tty(void)
{
	myputc = putc;
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= ((1<<19));
}
#endif

void main(void)
{
#ifndef TEST
	int	fd;
	int	dip;
	int	i;
	int	val;
	int	args[4] = {0, 0, 0, 0};

	// Reset the vsync timestamp
	vsync_timestamp = 0L;

	// Enable the NILE4 Interrupts
	enable_ip(NILE4_INT);

#ifdef TTY_INTERRUPTS
	init_tty();
#endif

	// If we have a tty driver - display the banner on the terminal
	show_banner();

	// Enable the Vertical sync interrupt from the SIO board
	*((volatile char *)INT_ENBL_REG_ADDR) |= (1 << (SIO_VSYNC_HANDLER_NUM - SIO_WDOG_TIMER_HANDLER_NUM));

	// Installing vsync handler
	// YES - Make sure polarity is set to Negative
	*((volatile char *)RESET_REG_ADDR) &= ~0x10;

	// Clear any possible pending vsync interrupts
	*((volatile char *)RESET_REG_ADDR) &= ~0x8;

	// Allow it to occur again
	*((volatile char *)RESET_REG_ADDR) |= 0x8;

	// Make sure the interrupt is enabled through the NILE IV
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= (1 << 11);

	// Turn on the SIO interrupt at the processor
	enable_ip(SIO_INT);

	// Check to see if the SCSI card is installed
	if(!debug_capable)
	{
		// No SCSI device installed - Check to see if we should boot diag or game
		dip = *((volatile short *)IOASIC_DIP_SWITCHES);
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

	// Finally - do the break to start debugger service
	do_menu();
#else
	printf("\f");
	printf("\n\nERROR - POST EXIT TO MAIN\n\n");
	while(1) ;
#endif
}

#ifndef TEST
static unsigned int	__bios_flags = 0;

void write_bios_flags(unsigned int val)
{
	__bios_flags = val;
}

unsigned int read_bios_flags(void)
{
	return(__bios_flags);
}
#endif
