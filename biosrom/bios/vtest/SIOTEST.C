#include	<system.h>
#include	<ioctl.h>
#include	<glide.h>

#define SND_CMD_SRAM_TEST    0x3A
#define SND_CMD_DRAM0_TEST   0x4A
#define SND_CMD_DRAM1_TEST   0x5A
#define SND_CMD_ROM_VERSION  0x6A
#define SND_CMD_ASIC_VER     0x7A
#define SND_CMD_ECHO         0x8A
#define SND_CMD_PMINT_SUM    0x9A  
#define SND_CMD_BONG         0xAA

/* completion codes (CC) for above diags */
#define SND_RTN_SRAM_PASSED   0xCC01
#define SND_RTN_DRAM0_PASSED  0xCC02
#define SND_RTN_DRAM1_PASSED  0xCC03
#define SND_RTN_BONG_FINISHED 0xCC04
#define SND_RTN_SRAM1_FAILED  0xEE01 
#define SND_RTN_SRAM2_FAILED  0xEE02
#define SND_RTN_SRAM3_FAILED  0xEE03
#define SND_RTN_DRAM0_FAILED  0xEE04 
#define SND_RTN_DRAM1_FAILED  0xEE05 

extern int (*myputc)(char);
int	gputc(char);

void delay_us(int);
extern int	__memory_size;
extern int	do_comprehensive_tests;
extern int	burn_loop;

struct dostime_t
{
	unsigned char	hour;
	unsigned char	minute;
	unsigned char	second;
	unsigned char	hsecond;
};

struct dosdate_t
{
	unsigned char	day __attribute__((packed));
	unsigned char	month __attribute__((packed));
	unsigned short	year __attribute__((packed));
	unsigned char	dayofweek __attribute__((packed));
};

static int	pass_number = 1;

static char	*dip_sw_name[] = {
"U13-1",
"U13-2",
"U13-3",
"U13-4",
"U13-5",
"U13-6",
"U13-7",
"U13-8",
"U12-1",
"U12-2",
"U12-3",
"U12-4",
"U12-5",
"U12-6",
"U12-7",
"U12-8"
};

static char	*coin_sw_name[] = {
"L-COIN  ",
"R-COIN  ",
"P1-STRT ",
"SLAM    ",
"TEST    ",
"P2-STRT ",
"CREDIT  ",
"C-COIN  ",
"E-COIN  ",
"P3-STRT ",
"P4-STRT ",
"VOL-DN  ",
"VOL-UP  ",
"RSVD    ",
"INTERLCK",
"BILL    "
};

static char	*player12_sw_name[] = {
"P1-U",
"P1-D",
"P1-L",
"P1-R",
"P1-A",
"P1-B",
"P1-C",
"P1-D",
"P2-U",
"P2-D",
"P2-L",
"P2-R",
"P2-A",
"P2-B",
"P2-C",
"P2-D"    
};

static char	*player34_sw_name[] = {
"P3-U",
"P3-D",
"P3-L",
"P3-R",
"P3-A",
"P3-B",
"P3-C",
"P3-D",
"P4-U",
"P4-D",
"P4-L",
"P4-R",
"P4-A",
"P4-B",
"P4-C",
"P4-D"
};

extern int	serial_number;
extern int	game_number;
extern int	date_of_manufacture;

static void pictest(void)
{
	set_fcolor(0xffff);
	printf("Serial Number:");
	if(serial_number == 123791)
	{
		set_fcolor(0x07e0);
		printf(" %d", serial_number);
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}
	set_fcolor(0xffff);
	printf("   Game Number: ");
	if(game_number == 528)
	{
		set_fcolor(0x07e0);
		printf("%d", game_number);
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}
	set_fcolor(0xffff);
	printf("   DOM Code: ");
	if(date_of_manufacture == 5255)
	{
		set_fcolor(0x07e0);
		printf("%d\n", date_of_manufacture);
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}
	set_fcolor(0xffff);
}

static void cmosbatterytest(void)
{
	unsigned char	val;

	set_fcolor(0xffff);
	printf("CMOS Battery test - ");
	val = *((unsigned char *)RTC_FLAGS_REG);
	if(val & 0x10)
	{
		set_fcolor(0xf800);
		printf("FAIL - BATTERY LOW\n");
		set_fcolor(0xffff);
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void cmoswalkonetest(void)
{
	int	data;
	int	tmp;
	unsigned char	old_data;

	set_fcolor(0xffff);
	printf("CMOS Walking 1 data test - ");
	old_data = *((volatile char *)CMOS_RAM_ADDR);
	for(data = 1; data < 0x100; data++)
	{
		*((volatile unsigned char *)CMOS_UNLOCK_ADDR) = 0;
		*((volatile unsigned char *)CMOS_RAM_ADDR) = (unsigned char)data;
		tmp = (int)*((volatile unsigned char *)CMOS_RAM_ADDR);
		if(tmp != data)
		{
			*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
			*((volatile char *)CMOS_RAM_ADDR) = old_data;
			set_fcolor(0xf800);
			printf("ERROR - Data Failure 0x%02.2X -> 0x%02.2X\n", data, tmp);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)CMOS_RAM_ADDR) = old_data;
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void cmoswalkzerotest(void)
{
	int	data;
	int	tmp;
	unsigned char	old_data;

	set_fcolor(0xffff);
	printf("CMOS Walking 0 data test - ");
	old_data = *((volatile char *)CMOS_RAM_ADDR);
	for(data = 0x0fffff7f; data > 0x000fffff; data >>= 1)
	{
		*((volatile unsigned char *)CMOS_UNLOCK_ADDR) = 0;
		*((volatile unsigned char *)CMOS_RAM_ADDR) = (unsigned char)data;
		tmp = (int)*((volatile unsigned char *)CMOS_RAM_ADDR);
		if((tmp & 0xff) != (data & 0xff))
		{
			*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
			*((volatile char *)CMOS_RAM_ADDR) = old_data;
			set_fcolor(0xf800);
			printf("ERROR - Data Failure 0x%02.2X -> 0x%02.2X\n", data, tmp);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)CMOS_RAM_ADDR) = old_data;
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static char	cmos_data[CMOS_SIZE];

static void save_cmos(void)
{
	int	i;

	for(i = 0; i < CMOS_SIZE; i++)
	{
		cmos_data[i] = *((volatile char *)(CMOS_RAM_ADDR + i));
	}
}

static void restore_cmos(void)
{
	int	i;

	for(i = 0; i < CMOS_SIZE; i++)
	{
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		*((volatile char *)(CMOS_RAM_ADDR + i)) = cmos_data[i];
	}
}

static void cmostest(void)
{
	int				i;
	unsigned char	data = 1;
	unsigned char	tmp;
	unsigned char	*addr = (unsigned char *)CMOS_RAM_ADDR;

	save_cmos();
	set_fcolor(0xffff);
	printf("Non-destructive CMOS test - ");
	for(i = 0; i < CMOS_SIZE; i++)
	{
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		*addr++ = data;
		++data;
		if(!data)
		{
			data = 1;
		}
	}
	addr = (unsigned char *)CMOS_RAM_ADDR;
	data = 1;
	for(i = 0; i < CMOS_SIZE; i++)
	{
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		tmp = *addr++;
		if(tmp != data)
		{
			restore_cmos();
			set_fcolor(0xf800);
			printf("ERROR - Data Failure @ 0x%08.8X\n", (int)addr);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
		++data;
		if(!data)
		{
			data = 1;
		}
	}
	restore_cmos();
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void watchdogtest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Watchdog Timer Test - ");
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0;
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0x5;
	for(i = 0; i < 500000; i++)
	{
		delay_us(1);
		if(*((volatile char *)RTC_FLAGS_REG) & 0x80)
		{
			*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
			*((volatile char *)RTC_WATCHDOG_REG) = 0;
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0;
	set_fcolor(0xf800);
	printf("FAIL\n");
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
	set_fcolor(0xffff);
}

static int	watchdog_int_received;

static int watchdog_handler(int cause, unsigned int *r_save)
{
	set_handler(SNAPHAT_WATCHDOG_HANDLER_NUM, (void *)0);
	watchdog_int_received = 1;
	return(0);
}

static void watchdoginttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Watchdog Timer Interrupt Test - ");
	watchdog_int_received = 0;
	set_handler(SNAPHAT_WATCHDOG_HANDLER_NUM, watchdog_handler);
	for(i = 0; i < 10000; i++)
	{
		delay_us(1000);
		if(watchdog_int_received)
		{
			set_handler(SNAPHAT_WATCHDOG_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(SNAPHAT_WATCHDOG_HANDLER_NUM, (void *)0);
	set_fcolor(0xf800);
	printf("FAIL\n");
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
	set_fcolor(0xffff);
}

static void swtest(void)
{
	int				i;
	unsigned short	cur_dip;
	unsigned short	cur_player12;
	unsigned short	cur_player34;
	unsigned short	cur_coin;
	unsigned short	init_dip;
	unsigned short	init_coin;
	unsigned short	init_player12;
	unsigned short	init_player34;
	unsigned short	checked_dip = 0;
	unsigned short	checked_coin = 0;
	unsigned short	checked_player12 = 0;
	unsigned short	checked_player34 = 0;
	unsigned short	last_dip = -1;
	unsigned short	last_coin = -1;
	unsigned short	last_player12 = -1;
	unsigned short	last_player34 = -1;
	int	done = 0;

	last_dip = cur_dip = init_dip = ~*((volatile short *)IOASIC_DIP_SWITCHES);
	cur_player12 = init_player12 = ~*((volatile short *)IOASIC_PLAYER_12);
	cur_player34 = init_player34 = ~*((volatile short *)IOASIC_PLAYER_34);
	cur_coin = init_coin = ~*((volatile short *)IOASIC_COIN_INPUT);
	while(!done)
	{
		cur_dip = ~*((volatile short *)IOASIC_DIP_SWITCHES);
		cur_player12 = ~*((volatile short *)IOASIC_PLAYER_12);
		cur_player34 = ~*((volatile short *)IOASIC_PLAYER_34);
		cur_coin = ~*((volatile short *)IOASIC_COIN_INPUT);
		if(cur_dip != last_dip ||
			cur_coin != last_coin ||
			cur_player12 != last_player12 ||
			cur_player34 != last_player34)
		{
			last_coin = cur_coin;
			last_player12 = cur_player12;
			last_player34 = cur_player34;
			if(!((cur_dip ^ last_dip) & checked_dip))
			{
				checked_dip |= (cur_dip ^ last_dip);
			}
			last_dip = cur_dip;
			if(!((cur_coin ^ init_coin) & checked_coin))
			{
				checked_coin |= ((cur_coin ^ init_coin) & ~checked_coin);
			}
			if(!((cur_player12 ^ init_player12) & checked_player12))
			{
				checked_player12 |= ((cur_player12 ^ init_player12) & ~checked_player12);
			}
			if(!((cur_player34 ^ init_player34) & checked_player34))
			{
				checked_player34 |= ((cur_player34 ^ init_player34) & ~checked_player34);
			}
	
			printf("\fSwitch Test\n\n");
			for(i = 0; i < 16; i++)
			{
				if(checked_dip & (1 << i))
				{
					set_fcolor(0x07e0);
				}
				else if(cur_dip & (1 << i))
				{
					set_fcolor(0xf7e0);
				}
				else
				{
					set_fcolor(0xffff);
				}
				printf("%s  \t", dip_sw_name[i]);
	
				if(checked_coin & (1 << i))
				{
					set_fcolor(0x07e0);
				}
				else if(cur_coin & (1 << i))
				{
					set_fcolor(0xf7e0);
				}
				else
				{
					set_fcolor(0xffff);
				}
				printf("%s\t", coin_sw_name[i]);
	
				if(checked_player12 & (1 << i))
				{
					set_fcolor(0x07e0);
				}
				else if(cur_player12 & (1 << i))
				{
					set_fcolor(0xf7e0);
				}
				else
				{
					set_fcolor(0xffff);
				}
				printf("%s   \t", player12_sw_name[i]);
	
				if(checked_player34 & (1 << i))
				{
					set_fcolor(0x07e0);
				}
				else if(cur_player34 & (1 << i))
				{
					set_fcolor(0xf7e0);
				}
				else
				{
					set_fcolor(0xffff);
				}
				printf("%s\n", player34_sw_name[i]);
			}
	
			set_fcolor(0x07e0);
			printf("\n\nGREEN  - Switch checked\n");
			set_fcolor(0xf7e0);
			printf("YELLOW - Switch ON but NOT checked\n");
			set_fcolor(0xffff);
			printf("WHITE  - Switch OFF but NOT checked\n\n");
			printf("You MUST test all switches to exit");

			if(checked_dip == 0xffff && checked_coin == 0xffff && checked_player12 == 0xffff &&
				checked_player34 == 0xffff)
			{
				done = 1;
			}
		}
	}
	printf("\f");
}

static void rtctest(void)
{
	struct dostime_t	t;
	struct dosdate_t	d;
	unsigned char	day = 0;
	unsigned char	month = 0;
	unsigned short	year = 0;
	unsigned char	dayofweek = 0;
	unsigned char	hour = 0;
	unsigned char	minute = 0;
	unsigned char	second = 0;

	set_fcolor(0xffff);
	printf("Real Time Clock Test - ");
	for(dayofweek = 0; dayofweek < 7; dayofweek++)
	{
		d.dayofweek = dayofweek;
		d.year = year;
		d.month = month;
		d.day = day;
		t.second = 0;
		t.minute = minute;
		t.hour = hour;
		_setdate(&d);
		_settime(&t);
		_getdate(&d);
		_gettime(&t);
		if(d.dayofweek != dayofweek)
		{
			set_fcolor(0xf800);
			printf("FAIL - DAY OF WEEK - %02.2X -> %02.2X\n", dayofweek, d.dayofweek);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	dayofweek = 6;
	for(year = 1980; year < 2010; year++)
	{
		d.dayofweek = dayofweek;
		d.year = year;
		d.month = month;
		d.day = day;
		t.second = 0;
		t.minute = minute;
		t.hour = hour;
		_setdate(&d);
		_settime(&t);
		_getdate(&d);
		_gettime(&t);
		if(d.year != year)
		{
			set_fcolor(0xf800);
			printf("FAIL - YEAR - %04d -> %04d\n", year, d.year);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	year = 2009;
	for(month = 0; month < 12; month++)
	{
		d.dayofweek = dayofweek;
		d.year = year;
		d.month = month;
		d.day = day;
		t.second = 0;
		t.minute = minute;
		t.hour = hour;
		_setdate(&d);
		_settime(&t);
		_getdate(&d);
		_gettime(&t);
		if(d.month != month)
		{
			set_fcolor(0xf800);
			printf("FAIL - MONTH - %02d -> %02d\n", month, d.month);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	month = 11;
	for(day = 0; day < 31; day++)
	{
		d.dayofweek = dayofweek;
		d.year = year;
		d.month = month;
		d.day = day;
		t.second = 0;
		t.minute = minute;
		t.hour = hour;
		_setdate(&d);
		_settime(&t);
		_getdate(&d);
		_gettime(&t);
		if(d.day != day)
		{
			set_fcolor(0xf800);
			printf("FAIL - DAY OF MONTH - %02d -> %02d\n", day, d.day);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	day = 30;
	second = 59;
	for(minute = 0; minute < 60; minute++)
	{
		d.dayofweek = dayofweek;
		d.year = year;
		d.month = month;
		d.day = day;
		t.second = second;
		t.minute = minute;
		t.hour = hour;
		_setdate(&d);
		_settime(&t);
		_getdate(&d);
		_gettime(&t);
		if(t.minute != minute)
		{
			set_fcolor(0xf800);
			printf("FAIL - MINUTES - %02d -> %02d\n", minute, t.minute);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	minute = 59;
	for(hour = 0; hour < 24; hour++)
	{
		d.dayofweek = dayofweek;
		d.year = year;
		d.month = month;
		d.day = day;
		t.second = second;
		t.minute = minute;
		t.hour = hour;
		_setdate(&d);
		_settime(&t);
		_getdate(&d);
		_gettime(&t);
		if(t.hour != hour)
		{
			set_fcolor(0xf800);
			printf("FAIL - HOURS - %02d -> %02d\n", hour, t.hour);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	hour = 23;
	_gettime(&t);
	second = t.second;
	delay_us(1000000);
	delay_us(1000000);
	delay_us(1000000);
	_gettime(&t);
	if(t.second == second)
	{
		set_fcolor(0xf800);
		printf("FAIL - CLOCK NOT RUNNING\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static volatile int	ioasic_int_received;

static int inttest_handler(int cause, unsigned int *regs)
{
	ioasic_int_received = 1;
	return(0);
}

static int uart_rx_handler(int cause, unsigned int *regs)
{
	ioasic_int_received = 1;
	(void)*((volatile short *)IOASIC_UART_RX);
	return(0);
}

static int uart_break_handler(int cause, unsigned int *regs)
{
	ioasic_int_received = 1;
	*((volatile short *)IOASIC_UART_CONTROL) &= ~0x2000;
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	return(0);
}

static int uart_rx_full_handler(int cause, unsigned int *regs)
{
	ioasic_int_received = 1;
	while(*((volatile short *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
	{
		(void)*((volatile short *)IOASIC_UART_RX);
	}
	return(0);
}

static int uart_rx_overrun_handler(int cause, unsigned int *regs)
{
	ioasic_int_received = 1;
	while(*((volatile short *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
	{
		(void)*((volatile short *)IOASIC_UART_RX);
	}
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	return(0);
}

static int asicputc(char c)
{
	unsigned short status;
	unsigned short	output = 0;

	// Get status
	status = *((volatile short *)IOASIC_STATUS);

	// Transmit register empty ?
	if(!(status & IOASIC_UART_XMT_EMPTY))
	{
		// NOPE - Wait for it to empty
		while(!(status & IOASIC_UART_XMT_EMPTY))
		{
			status = *((volatile short *)IOASIC_STATUS);
		}
	}

	// Transmit the character
	output = c;
	*((volatile short *)IOASIC_UART_TX) = output;
}

static void asicputs(char *str)
{
	while(*str)
	{
		asicputc(*str);
		++str;
	}
}

static int uart_tx_handler(int cause, unsigned int *regs)
{
	ioasic_int_received = 1;
	*((volatile short *)IOASIC_UART_CONTROL) |= 0x1000;
	asicputc(0);
	return(0);
}

static void ioasicinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("I/O ASIC Interrupt Test - ");
	ioasic_int_received = 0;
	set_handler(IOASIC_FORCE_INT_HANDLER_NUM, inttest_handler);
	*((volatile short *)IOASIC_CONTROL) |= 2;
	for(i = 0; i < 10000; i++)
	{
		if(ioasic_int_received)
		{
			set_handler(IOASIC_FORCE_INT_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(IOASIC_FORCE_INT_HANDLER_NUM, (void *)0);
	set_fcolor(0xf800);
	printf("FAIL\n");
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
	set_fcolor(0xffff);
}

static void a2dtest(void)
{
	int	chan_low[8] = {0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100};
	int	chan_hi[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int	chan_cur[8];
	int	chan_last[8] = {0xaa5555aa, 0xaa5555aa, 0xaa5555aa, 0xaa5555aa, 0xaa5555aa, 0xaa5555aa, 0xaa5555aa, 0xaa5555aa};
	int	i;

	printf("\f");
	printf("\f\n\nAnalog to Digital Convertor Test\n\n");
	set_fcolor(0x07e0);
	printf("Channel in GREEN - Tested\n");
	set_fcolor(0xffff);
	printf("Channel in WHITE - Un-tested\n\n");
	printf("All channels MUST be GREEN to continue\n\n");
	printf("0  1  2  3  4  5  6  7\n");
	while(1)
	{
		for(i = 0; i < sizeof(chan_cur)/sizeof(int); i++)
		{
			*((volatile char *)A2D_ADDR) = i + 8;
			delay_us(1000);
			chan_cur[i] = *((volatile unsigned char *)A2D_ADDR);
		}
		if((chan_cur[0] != chan_last[0]) ||
			(chan_cur[1] != chan_last[1]) ||
			(chan_cur[2] != chan_last[2]) ||
			(chan_cur[3] != chan_last[3]) ||
			(chan_cur[4] != chan_last[4]) ||
			(chan_cur[5] != chan_last[5]) ||
			(chan_cur[6] != chan_last[6]) ||
			(chan_cur[7] != chan_last[7]))
		{
			set_fcolor(0xffff);
			for(i = 0; i < sizeof(chan_cur)/sizeof(int); i++)
			{
				chan_last[i] = chan_cur[i];
				if(chan_last[i] < chan_low[i])
				{
					chan_low[i] = chan_last[i];
				}
				else if(chan_last[i] > chan_hi[i])
				{
					chan_hi[i] = chan_last[i];
				}
				if(chan_hi[i] == 0xff && chan_low[i] == 0)
				{
					set_fcolor(0x07e0);
				}
				else
				{
					set_fcolor(0xffff);
				}
				printf("%02.2X ", chan_cur[i]);
			}
			printf("\r");
		}
		if((chan_low[0] == 0) &&
			(chan_low[1] == 0) &&
			(chan_low[2] == 0) &&
			(chan_low[3] == 0) &&
			(chan_low[4] == 0) &&
			(chan_low[5] == 0) &&
			(chan_low[6] == 0) &&
			(chan_low[7] == 0) &&
			(chan_hi[0] == 0xff) &&
			(chan_hi[1] == 0xff) &&
			(chan_hi[2] == 0xff) &&
			(chan_hi[3] == 0xff) &&
			(chan_hi[4] == 0xff) &&
			(chan_hi[5] == 0xff) &&
			(chan_hi[6] == 0xff) &&
			(chan_hi[7] == 0xff))
		{
			return;
		}
	}
	set_fcolor(0xffff);
}

static volatile int	a2d_int_received = 0;
static int				chan;

static int a2d_handler(int cause, unsigned int *regs)
{
	(void)*((volatile char *)(A2D_ADDR + chan));
	a2d_int_received = 1;
	return(0);
}

static void a2dinttest(void)
{
	set_fcolor(0xffff);
	printf("Analog to Digital Convertor Int Test - ");
	for(chan = 0; chan < 8; chan++)
	{
		(void)*((volatile char *)(A2D_ADDR + chan));
	}
	set_handler(SIO_A2D_HANDLER_NUM, a2d_handler);
	for(chan = 0; chan < 8; chan++)
	{
		a2d_int_received = 0;
		*((volatile char *)(A2D_ADDR + chan)) = 0;
		delay_us(1000);
		if(a2d_int_received && chan == 7)
		{
			set_handler(SIO_A2D_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(SIO_A2D_HANDLER_NUM, (void *)0);
	set_fcolor(0xf800);
	printf("FAIL - Channel %d\n", chan);
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void ethertest(void)
{
	set_fcolor(0xffff);
	printf("Ethernet Local Loopback Test - ");
	printf("NOT IMPLEMENTED\n");
	set_fcolor(0xffff);
}

static void etherinttest(void)
{
	set_fcolor(0xffff);
	printf("Ethernet Int Test - ");
	printf("NOT IMPLEMENTED\n");
	set_fcolor(0xffff);
}

static void hilinktest(void)
{
	set_fcolor(0xffff);
	printf("HI-LINK Board Test - ");
	printf("NOT IMPLEMENTED\n");
	set_fcolor(0xffff);
}

static void hilinkinttest(void)
{
	set_fcolor(0xffff);
	printf("HI-LINK Board Int Test - ");
	printf("NOT IMPLEMENTED\n");
	set_fcolor(0xffff);
}

static void vretracetest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Vertical Retrace Interrupt Test - ");
	ioasic_int_received = 0;
	set_handler(SIO_VSYNC_HANDLER_NUM, inttest_handler);
	for(i = 0; i < 100000; i++)
	{
		if(ioasic_int_received)
		{
			set_handler(SIO_VSYNC_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
		delay_us(1);
	}
	set_handler(SIO_VSYNC_HANDLER_NUM, (void *)0);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void snd_reset(void)
{
	short	control;

	control = *((volatile short *)IOASIC_SOUND_CONTROL);
	delay_us(10000*2);
	control &= ~1;
	*((volatile short *)IOASIC_SOUND_CONTROL) = control;
	delay_us(10000*2);
	control = *((volatile short *)IOASIC_SOUND_CONTROL);
	delay_us(10000*2);
	control |= 1;
	*((volatile short *)IOASIC_SOUND_CONTROL) = control;
	delay_us(10000*2);
	control = *((volatile short *)IOASIC_SOUND_CONTROL);
	delay_us(10000*2);
	control &= ~0x8000;
	*((volatile short *)IOASIC_SOUND_CONTROL) = control;
	delay_us(10000*2);
}

static int snd_send_command(unsigned int command)
{
	int	timeout;

	timeout = 1000;
	while(!(*((volatile short *)IOASIC_SOUND_STATUS) & 0x0040))
	{
		delay_us(1);
		--timeout;
	}
	if(!timeout)
	{
		return(0xeeee);
	}
	if(*((volatile short *)IOASIC_SOUND_DATA_IN) != 0xa)
	{
		return(0xeeee);
	}
	*((volatile short *)IOASIC_SOUND_DATA_IN) = 0;
	*((volatile short *)IOASIC_SOUND_DATA_OUT) = command;
	return(0);
}
	

static unsigned int snd_wait_for_completion(void)
{
	int	timeout;
	volatile unsigned int	completion_code;

	timeout = 2000000;
	while(!(*((volatile short *)IOASIC_SOUND_STATUS) & 0x0040))
	{
		--timeout;
		delay_us(1);
	}
	if(!timeout)
	{
		return(0xeeee);
	}
	completion_code = *((volatile short *)IOASIC_SOUND_DATA_IN) & 0xffff;
	*((volatile short *)IOASIC_SOUND_DATA_IN) = 0;
	return(completion_code);
}	

static unsigned int snd_get_boot_version(void)
{
	int				status;
	unsigned int	version;

	status = snd_send_command(0x6a);	// GET ROM VERSION
	if(status)
	{
		return(0xeeee);
	}
	version = snd_wait_for_completion();
	if(version == 0xeeee)
	{
		return(0xeeee);
	}
	return((version & 0xffff));
}


static void soundtest(void)
{
	int	boot_version;
	int	sram_status;
	int	dram_status;
	int	bong_status;
	int	error = 0;

	snd_reset();

	boot_version = snd_get_boot_version() & 0xffff;
	set_fcolor(0xffff);
	printf("Sound Boot Version:  ");
	if(boot_version != 0xd104)
	{
		set_fcolor(0xf800);
		printf("FAIL - %X\n", boot_version);
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}
	else
	{
		set_fcolor(0x07e0);
		printf("%X.%02X\n", boot_version >> 8, boot_version & 0xff);
	}		

	set_fcolor(0xffff);
	printf("Sound SRAM Test - ");
	snd_send_command(SND_CMD_SRAM_TEST);
	sram_status = snd_wait_for_completion();
	sram_status &= 0xffff;
	if(sram_status == SND_RTN_SRAM_PASSED)
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
	}
	else if(SND_RTN_SRAM1_FAILED)
	{
		set_fcolor(0xf800);
		printf("FAIL - SRAM1\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}
	else if(SND_RTN_SRAM2_FAILED)
	{
		set_fcolor(0xf800);
		printf("FAIL - SRAM2\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}
	else if(SND_RTN_SRAM3_FAILED)
	{
		set_fcolor(0xf800);
		printf("FAIL - SRAM2\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL - SRAM\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}

	set_fcolor(0xffff);
	printf("Sound DRAM Test - ");
	snd_send_command(SND_CMD_DRAM0_TEST);
	dram_status = snd_wait_for_completion();
	dram_status &= 0xffff;
	if(dram_status == SND_RTN_DRAM0_FAILED)
	{
		set_fcolor(0xf800);
		printf("FAIL - DRAM0\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}
	else if(dram_status == SND_RTN_DRAM0_PASSED)
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL - DRAM\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}

	set_fcolor(0xffff);
	printf("Sound BONG Test - ");
	snd_send_command(SND_CMD_BONG);
	bong_status = snd_wait_for_completion();
	bong_status &= 0xffff;
	if(bong_status == SND_RTN_BONG_FINISHED)
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
	}

	snd_reset();
	set_fcolor(0xffff);
}

static void asicuarttest(void)
{
	int	data;
	int	i;
	int	tmp;
	unsigned short status;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART Local Loopback Test - ");
	while(*((volatile short *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
	{
		(void)*((volatile short *)IOASIC_UART_RX);
	}
	*((volatile short *)IOASIC_UART_CONTROL) |= (1<<11);
	for(data = 0; data < 0x100; data++)
	{
		// Get status
		status = *((volatile short *)IOASIC_STATUS);

		// Transmit register empty ?
		if(!(status & IOASIC_UART_XMT_EMPTY))
		{
			// NOPE - Wait for it to empty
			while(!(status & IOASIC_UART_XMT_EMPTY))
			{
				status = *((volatile short *)IOASIC_STATUS);
			}
		}

		// Transmit the character
		*((volatile short *)IOASIC_UART_TX) = data;

		for(i = 0; i < 100000; i++)
		{
			if(*((volatile short *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
			{
				break;
			}
		}
		if(i == 100000)
		{
			set_fcolor(0xf800);
			printf("FAIL - No characters received\n");
			set_fcolor(0xffff);
			while(1) ;
		}
		tmp = *((volatile short *)IOASIC_UART_RX) & 0xff;
		if(tmp != data)
		{
			set_fcolor(0xf800);
			printf("FAIL - Data error:  0x%02.2X -> 0x%02.2X\n", data, tmp);
			set_fcolor(0xffff);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}


static void asicuartrxinttest(void)
{
	int	data;
	int	i;
	int	tmp;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART Rx Int Test - ");
	*((volatile short *)IOASIC_UART_CONTROL) |= (1<<11);
	ioasic_int_received = 0;
	set_handler(IOASIC_UART_RX_CHAR_HANDLER_NUM, uart_rx_handler);
	asicputc(data);
	for(i = 0; i < 1000; i++)
	{
		delay_us(1);
		if(ioasic_int_received)
		{
			set_handler(IOASIC_UART_RX_CHAR_HANDLER_NUM, (void *)0);
			*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(IOASIC_UART_RX_CHAR_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void asicuartbreakinttest(void)
{
	int	data;
	int	i;
	int	tmp;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART Break Int Test - ");

	while(*((volatile short *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
	{
		(void)*((volatile short *)IOASIC_UART_RX);
	}

	*((volatile short *)IOASIC_UART_CONTROL) |= (1<<11);
	ioasic_int_received = 0;
	set_handler(IOASIC_UART_BREAK_HANDLER_NUM, uart_break_handler);
	*((volatile short *)IOASIC_UART_CONTROL) |= 0x2000;
	for(i = 0; i < 10000; i++)
	{
		delay_us(1);
		if(ioasic_int_received)
		{
			*((volatile short *)IOASIC_UART_CONTROL) &= ~0x2000;
			set_handler(IOASIC_UART_BREAK_HANDLER_NUM, (void *)0);
			*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	*((volatile short *)IOASIC_UART_CONTROL) &= ~0x2000;
	set_handler(IOASIC_UART_BREAK_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	*((volatile short *)IOASIC_UART_CONTROL) ^= (1<<10);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void asicuartferrorinttest(void)
{
	int	data;
	int	i;
	int	tmp;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART FERROR Int Test - ");
	*((volatile short *)IOASIC_UART_CONTROL) |= (1<<11);
	ioasic_int_received = 0;
	set_handler(IOASIC_UART_FERROR_HANDLER_NUM, uart_break_handler);
	*((volatile short *)IOASIC_UART_CONTROL) |= 0x2000;
	for(i = 0; i < 1000; i++)
	{
		delay_us(1);
		if(ioasic_int_received)
		{
			set_handler(IOASIC_UART_FERROR_HANDLER_NUM, (void *)0);
			*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	*((volatile short *)IOASIC_UART_CONTROL) &= ~0x2000;
	set_handler(IOASIC_UART_FERROR_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void asicuartrxfullinttest(void)
{
	int	data;
	int	i;
	int	tmp;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART Rx Full Int Test - ");
	*((volatile short *)IOASIC_UART_CONTROL) |= (1<<11);
	ioasic_int_received = 0;
	set_handler(IOASIC_UART_RX_FULL_HANDLER_NUM, uart_rx_full_handler);
	asicputs("Testing");
	for(i = 0; i < 1000; i++)
	{
		delay_us(1);
		if(ioasic_int_received)
		{
			set_handler(IOASIC_UART_RX_FULL_HANDLER_NUM, (void *)0);
			*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(IOASIC_UART_RX_FULL_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void asicuartrxoverruninttest(void)
{
	int	data;
	int	i;
	int	tmp;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART Rx Overrun Int Test - ");
	*((volatile short *)IOASIC_UART_CONTROL) |= (1<<11);
	ioasic_int_received = 0;
	set_handler(IOASIC_UART_OVERRUN_HANDLER_NUM, uart_rx_overrun_handler);
	asicputs("Testtest");
	for(i = 0; i < 1000; i++)
	{
		delay_us(1);
		if(ioasic_int_received)
		{
			set_handler(IOASIC_UART_OVERRUN_HANDLER_NUM, (void *)0);
			*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(IOASIC_UART_OVERRUN_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_UART_CONTROL) &= ~(1<<11);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void asicuarttxinttest(void)
{
	int	data;
	int	i;
	int	tmp;
	
	set_fcolor(0xffff);
	printf("I/O ASIC UART Tx Empty Int Test - ");
	ioasic_int_received = 0;
	*((volatile short *)IOASIC_UART_CONTROL) |= 0x1000;
	asicputc(0);
	set_handler(IOASIC_UART_TX_EMPTY_HANDLER_NUM, uart_tx_handler);
	*((volatile short *)IOASIC_UART_CONTROL) &= ~0x1000;
	for(i = 0; i < 1000; i++)
	{
		delay_us(1);
		if(ioasic_int_received)
		{
			set_handler(IOASIC_UART_TX_EMPTY_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	set_handler(IOASIC_UART_TX_EMPTY_HANDLER_NUM, (void *)0);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static int fifos_exist = 0;
static int fifo_size = 0;

static void fifotest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Sound FIFO Test - ");

	// Hold sound system in reset
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<0);

	// Enable the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<1);

	// Reset the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);

	// Is status empty, NOT half-full, and NOT full ?
	if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) == 1)
	{
		// YES - write 1 word to FIFOs
		*((volatile short *)SOUND_FIFO_ADDR) = 5;

		// Is status NOT empty, NOT half-full, and NOT full ?
		if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) != 0)
		{
			// NOPE - FIFO don't exist or are broken
			fifos_exist = 0;
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<0);
			printf("NO FIFOS INSTALLED\n");
			set_fcolor(0xffff);
			return;
		}

		// Reset the FIFO's
		*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
		*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);


		// Fill FIFO's with 128 words
		for(i = 0; i < 128; i++)
		{
			*((volatile short *)SOUND_FIFO_ADDR) = 0;
		}

		// Does status indicate FIFO's are half full ?
		if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) == 2)
		{
			// YES - FIFO's are 256 words
			fifo_size = 256;
		}

		else
		{
			// Fill FIFO's with another 128 words
			for(i = 0; i < 128; i++)
			{
				*((volatile short *)SOUND_FIFO_ADDR) = 0;
			}

			// Does status indicate FIFO's are half full ?
			if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) == 2)
			{
				// YES - FIFO's are 512 words
				fifo_size = 512;
			}
			else
			{
				// Fill FIFO's with another 256 words
				for(i = 0; i < 256; i++)
				{
					*((volatile short *)SOUND_FIFO_ADDR) = 0;
				}

				// Does status indicate FIFO's are half full ?
				if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) == 2)
				{
					// YES - FIFO's are 512 words
					fifo_size = 1024;
				}
				else
				{
					// NOPE - FIFO's are broken
					fifos_exist = 0;
					*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
					*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
					*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
					*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<0);
					set_fcolor(0xf800);
					printf("FAIL - HALF FULL\n");
					set_fcolor(0xffff);
					if(do_comprehensive_tests)
					{
						while(1) ;
					}
					return;
				}
			}
		}

		// Fill FIFO's the reset of the way
		for(i = 0; i < (fifo_size >> 1); i++)
		{
			*((volatile short *)SOUND_FIFO_ADDR) = 0;
		}

		// Does status indicate FIFO's are full ?
		if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) != 6)
		{
			// NOPE
			fifos_exist = 0;
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<0);
			set_fcolor(0xf800);
			printf("FAIL - FULL\n");
			set_fcolor(0xffff);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}

		// Reset the FIFO's
		*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
		*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);

		// Does status indicate FIFO's empty ?
		if(((*((volatile short *)IOASIC_SOUND_STATUS) >> 3) & 7) != 1)
		{
			// NOPE
			fifos_exist = 0;
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<0);
			set_fcolor(0xf800);
			printf("FAIL - EMPTY\n");
			set_fcolor(0xffff);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}

		set_fcolor(0x07e0);
		printf("PASS - FIFO SIZE %d WORDS\n", fifo_size);
		set_fcolor(0xffff);
		fifos_exist = 1;
		*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<0);
		return;
	}

	fifos_exist = 0;
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<0);
	printf("NO FIFOS INSTALLED\n");
	set_fcolor(0xffff);
}

static int fifo_int_received = 0;

static int fifo_empty_handler(int cause, unsigned int *regs)
{
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	*((volatile short *)IOASIC_CONTROL) ^= 0x8;
	while(*((volatile short *)IOASIC_STATUS) & 0x8)
	{
		*((volatile short *)SOUND_FIFO_ADDR) = 0;
	}
	*((volatile short *)IOASIC_CONTROL) ^= 0x8;
	fifo_int_received = 1;
	return(0);
}

static void fifoemptyinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Sound FIFO Empty Int Test - ");

	// Hold sound system in reset
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<0);

	// Enable the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<1);

	// Set flow through mode
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<3);

	// Reset the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);

	*((volatile short *)SOUND_FIFO_ADDR) = 0;

	fifo_int_received = 0;
	set_handler(IOASIC_FIFO_EMPTY_HANDLER_NUM, fifo_empty_handler);

	// Reset the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);

	for(i = 0; i < 10000; i++)
	{
		if(fifo_int_received)
		{
			set_handler(IOASIC_FIFO_EMPTY_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
			return;
		}
		delay_us(1);
	}

	set_handler(IOASIC_FIFO_EMPTY_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}


static int fifo_half_full_handler(int cause, unsigned int *regs)
{
	*((volatile short *)IOASIC_CONTROL) ^= 0x10;
	while(*((volatile short *)IOASIC_STATUS) & 0x10)
	{
		*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
		*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	}
	*((volatile short *)IOASIC_CONTROL) ^= 0x10;
	fifo_int_received = 1;
	return(0);
}

static void fifohalffullinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Sound FIFO Half Full Int Test - ");

	// Hold sound system in reset
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<0);

	// Enable the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<1);

	// Set flow through mode
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<3);

	// Reset the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);

	fifo_int_received = 0;
	set_handler(IOASIC_FIFO_HALF_FULL_HANDLER_NUM, fifo_half_full_handler);

	for(i = 0; i < (fifo_size >> 1); i++)
	{
		*((volatile short *)SOUND_FIFO_ADDR) = 0;
	}

	for(i = 0; i < 10000; i++)
	{
		if(fifo_int_received)
		{
			set_handler(IOASIC_FIFO_HALF_FULL_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
			return;
		}
		delay_us(1);
	}

	set_handler(IOASIC_FIFO_HALF_FULL_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static int fifo_full_handler(int cause, unsigned int *regs)
{
	*((volatile short *)IOASIC_CONTROL) ^= 0x20;
	while(*((volatile short *)IOASIC_STATUS) & 0x20)
	{
		*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
		*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	}
	*((volatile short *)IOASIC_CONTROL) ^= 0x20;
	fifo_int_received = 1;
	return(0);
}

static void fifofullinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Sound FIFO Full Int Test - ");

	// Hold sound system in reset
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<0);

	// Enable the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<1);

	// Set flow through mode
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<3);

	// Reset the FIFOs
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);

	fifo_int_received = 0;
	set_handler(IOASIC_FIFO_FULL_HANDLER_NUM, fifo_full_handler);

	for(i = 0; i < fifo_size; i++)
	{
		*((volatile short *)SOUND_FIFO_ADDR) = 0;
	}

	for(i = 0; i < 10000; i++)
	{
		if(fifo_int_received)
		{
			set_handler(IOASIC_FIFO_FULL_HANDLER_NUM, (void *)0);
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
			return;
		}
		delay_us(1);
	}

	set_handler(IOASIC_FIFO_FULL_HANDLER_NUM, (void *)0);
	*((volatile short *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile short *)IOASIC_SOUND_CONTROL) |= (1<<2);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static volatile int sound_int_received = 0;

static int sound_hts_handler(int cause, unsigned int *regs)
{
printf("HTS HAND\n");
	set_handler(IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM, (void *)0);
	sound_int_received = 1;
	return(0);
}

static void soundhtsinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Host to sound Data Empty Int Test - ");
	sound_int_received = 0;
	disable_ip(SIO_INT);
	set_handler(IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM, sound_hts_handler);
	enable_ip(SIO_INT);
	for(i = 0; i < 100000; i++)
	{
		if(sound_int_received)
		{
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
		delay_us(1);
	}
	set_handler(IOASIC_SND_HTS_DATA_EMPTY_HANDLER_NUM, (void *)0);
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static int sound_sth_handler(int cause, unsigned int *regs)
{
	set_handler(IOASIC_SND_STH_DATA_FULL_HANDLER_NUM, (void *)0);
	sound_int_received = 1;
	return(0);
}

static void soundsthinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Sound to host data full Int Test - ");
	sound_int_received = 0;
	disable_ip(SIO_INT);
	set_handler(IOASIC_SND_STH_DATA_FULL_HANDLER_NUM, sound_sth_handler);
	enable_ip(SIO_INT);
	for(i = 0; i < 100000; i++)
	{
		if(sound_int_received)
		{
			set_handler(IOASIC_SND_STH_DATA_FULL_HANDLER_NUM, (void *)0);
			snd_reset();
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
		delay_us(1);
	}
	set_handler(IOASIC_SND_STH_DATA_FULL_HANDLER_NUM, (void *)0);
	snd_reset();
	set_fcolor(0xf800);
	printf("FAIL\n");
	set_fcolor(0xffff);
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
}

static void ledtest(void)
{
	int	i;
	char	tmp;

	set_fcolor(0xffff);
	printf("SIO LED Test - ");
	for(i = 0; i < 0x10; i++)
	{
		*((volatile char *)LED_STATUS_REG_ADDR) = i;
		tmp = *((volatile char *)LED_STATUS_REG_ADDR) & 0xf;
		if(tmp != (char)i)
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void asicttyinit(void)
{
	unsigned int	delay;

	// Reset the UART
	*((volatile short *)IOASIC_UART_CONTROL) = IOASIC_UART_RESET;
  
	for(delay = 0; delay < 1000; delay++) ;
  
	*((volatile short *)IOASIC_UART_CONTROL) = (IOASIC_UART_INIT|IOASIC_UART_38400_BAUD);

	// Enable the UART
	*((volatile short *)IOASIC_UART_CONTROL) |= IOASIC_UART_ENABLE;
}

void siotest(void)
{
	printf("\fInitializing SIO board for testing...\n");
	printf("Unlocking I/O ASIC...");
	unlock_ioasic();
	printf("\nCalling sysinit...");
	sysinit();
	printf("\nCalling pic_init...");
	pic_init();
//	ttyinit();
	printf("\nCalling asicttyinit...");
	asicttyinit();
	printf("\nCalling sound_init...");
	sound_init();
	printf("\nEnabling SIO interrupts...");
	enable_ip(SIO_INT);
	printf("\fSIO Board Tests\n\n");
//	printf("Pass Number: %d\n", pass_number);
//	pass_number++;
	ledtest();
	pictest();
	cmoswalkonetest();
	cmoswalkzerotest();
	cmostest();
	cmosbatterytest();
	rtctest();
	watchdogtest();
	watchdoginttest();
	ioasicinttest();
	asicuarttest();
	asicuartrxinttest();
	asicuartrxfullinttest();
	asicuartrxoverruninttest();
//	asicuartbreakinttest();
	asicuartferrorinttest();
	asicuarttxinttest();
	fifotest();
	if(fifos_exist)
	{
		fifoemptyinttest();
		fifohalffullinttest();
		fifofullinttest();
	}
	soundtest();
//	soundhtsinttest();
//	soundsthinttest();
	vretracetest();
	hilinktest();
	hilinkinttest();
	a2dinttest();
//	printf("\f");
	ethertest();
	etherinttest();
	if(do_comprehensive_tests && !burn_loop)
	{
		a2dtest();
		swtest();
	}
}
