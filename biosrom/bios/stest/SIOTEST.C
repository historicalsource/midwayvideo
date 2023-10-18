#include	<system.h>
#include	<ioctl.h>
#include	<glide.h>
#include	"test.h"

extern chip_draw_info_t	cpu_chips[];

extern int cpu_rom_chip_num;
extern int cpu_expansion_rom_chip_num;
extern int cpu_cpu_chip_num;
extern int cpu_galileo_chip_num;
extern int cpu_sdram1_chip_num;
extern int cpu_sdram2_chip_num;
extern int cpu_sdram3_chip_num;
extern int cpu_sdram4_chip_num;
extern int cpu_ide_chip_num;
extern int cpu_pic_chip_num;
extern int cpu_ioasic_chip_num;
extern int cpu_rtc_clock_chip_num;
extern int cpu_cmos_ram_chip_num;
extern int cpu_fbi_chip_num;
extern int cpu_tmu_chip_num;
extern int cpu_fbi_mem0_chip_num;
extern int cpu_fbi_mem1_chip_num;
extern int cpu_fbi_mem2_chip_num;
extern int cpu_fbi_mem3_chip_num;
extern int cpu_tmu_mem0_chip_num;
extern int cpu_tmu_mem1_chip_num;
extern int cpu_tmu_mem2_chip_num;
extern int cpu_tmu_mem3_chip_num;
extern int cpu_tmu_mem4_chip_num;
extern int cpu_tmu_mem5_chip_num;
extern int cpu_tmu_mem6_chip_num;
extern int cpu_tmu_mem7_chip_num;
extern int cpu_fifo0_chip_num;
extern int cpu_fifo1_chip_num;
extern int cpu_sound_dsp_chip_num;

extern int (*myputc)(char);
int	gputc(char);
int	get_buttons(void);

int	pic_status = -1;

void delay_us(int);
extern int	__memory_size;

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

extern int	serial_number;
extern int	game_number;
extern int	date_of_manufacture;

static void pictest(void)
{
	set_fcolor(0xffff);
	printf("Serial Number:  ");
	if(!pic_status)
	{
		set_fcolor(0x07e0);
		printf("%d\n", serial_number);
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		cpu_chips[cpu_pic_chip_num].status = CHIP_STATUS_FAILED;
	}
	set_fcolor(0xffff);
	printf("Game Number:  ");
	if(!pic_status)
	{
		set_fcolor(0x07e0);
		printf("%d\n", game_number);
	}
	else
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		cpu_chips[cpu_pic_chip_num].status = CHIP_STATUS_FAILED;
	}
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
			printf("FAIL\n");
			cpu_chips[cpu_cmos_ram_chip_num].status = CHIP_STATUS_FAILED;
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
			printf("FAIL\n");
			cpu_chips[cpu_cmos_ram_chip_num].status = CHIP_STATUS_FAILED;
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
		cmos_data[i] = *((volatile int *)(CMOS_RAM_ADDR + (i<<2)));
	}
}

static void restore_cmos(void)
{
	int	i;

	for(i = 0; i < CMOS_SIZE; i++)
	{
		*((volatile int *)CMOS_UNLOCK_ADDR) = 0;
		*((volatile int *)(CMOS_RAM_ADDR + (i<<2))) = cmos_data[i];
	}
}

static void cmostest(void)
{
	int				i;
	unsigned char	data = 1;
	unsigned char	tmp;
	volatile unsigned int	*addr = (unsigned int *)CMOS_RAM_ADDR;

	save_cmos();
	set_fcolor(0xffff);
	printf("Non-destructive CMOS test - ");
	disable_interrupts();
	for(i = 0; i < CMOS_SIZE; i++)
	{
		*((volatile int *)CMOS_UNLOCK_ADDR) = 0;
		*addr = data;
		tmp = *addr++ & 0xff;
		if(tmp != data)
		{
			restore_cmos();
			set_fcolor(0xf800);
			printf("WR FAIL\n");
//			printf("WR FAIL - offset: %x - %02.2X -> %02.2X\n", i, data, tmp);
			cpu_chips[cpu_cmos_ram_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
		++data;
		if(!data)
		{
			data = 1;
		}
	}
	addr = (unsigned int *)CMOS_RAM_ADDR;
	data = 1;
	for(i = 0; i < CMOS_SIZE; i++)
	{
		tmp = *addr++ & 0xff;
		if(tmp != data)
		{
			restore_cmos();
			set_fcolor(0xf800);
			printf("RD FAIL\n");
			cpu_chips[cpu_cmos_ram_chip_num].status = CHIP_STATUS_FAILED;
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

static void rtctest(void)
{
	struct dostime_t	t;
	struct dosdate_t	d;
	struct dostime_t	ot;
	struct dosdate_t	od;
	unsigned char	day = 0;
	unsigned char	month = 0;
	unsigned short	year = 0;
	unsigned char	dayofweek = 0;
	unsigned char	hour = 0;
	unsigned char	minute = 0;
	unsigned char	second = 0;
	int				i;

	_getdate(&od);
	_gettime(&ot);
	set_fcolor(0xffff);
	printf("Real Time Clock Test - ");
	for(dayofweek = 0; dayofweek < 7; dayofweek++)
	{
		if(get_buttons())
		{
			goto rout;
		}
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
			printf("FAIL\n");
			cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
			goto rout;
		}
	}
	dayofweek = 6;
	for(year = 1980; year < 2010; year++)
	{
		if(get_buttons())
		{
			goto rout;
		}
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
			printf("FAIL\n");
			cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
			goto rout;
		}
	}
	year = 2009;
	for(month = 0; month < 12; month++)
	{
		if(get_buttons())
		{
			goto rout;
		}
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
			printf("FAIL\n");
			cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
			goto rout;
		}
	}
	month = 11;
	for(day = 0; day < 31; day++)
	{
		if(get_buttons())
		{
			goto rout;
		}
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
			printf("FAIL\n");
			cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
			goto rout;
		}
	}
	day = 30;
	second = 59;
	for(minute = 0; minute < 60; minute++)
	{
		if(get_buttons())
		{
			goto rout;
		}
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
			printf("FAIL\n");
			cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
			goto rout;
		}
	}
	minute = 59;
	for(hour = 0; hour < 24; hour++)
	{
		if(get_buttons())
		{
			goto rout;
		}
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
			printf("FAIL\n");
			cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
			goto rout;
		}
	}
	hour = 23;
	_gettime(&t);
	second = t.second;
	for(i = 0; i < 3000000; i++)
	{
		if(get_buttons())
		{
			goto rout;
		}
		delay_us(1);
	}
	_gettime(&t);
	if(t.second == second)
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_FAILED;
		goto rout;
	}
	else
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
		set_fcolor(0xffff);
	}
rout:
	_settime(&ot);
	_setdate(&od);
}

static void snd_reset(void)
{
	short	control;

	control = *((volatile int *)IOASIC_SOUND_CONTROL);
	delay_us(10000*2);
	control &= ~1;
	*((volatile int *)IOASIC_SOUND_CONTROL) = control;
	delay_us(10000*2);
	control = *((volatile int *)IOASIC_SOUND_CONTROL);
	delay_us(10000*2);
	control |= 1;
	*((volatile int *)IOASIC_SOUND_CONTROL) = control;
	delay_us(10000*2);
	control = *((volatile int *)IOASIC_SOUND_CONTROL);
	delay_us(10000*2);
	control &= ~0x8000;
	*((volatile int *)IOASIC_SOUND_CONTROL) = control;
	delay_us(10000*2);
}

static int snd_send_command(unsigned int command)
{
	int	timeout;

	timeout = 1000;
	while(!(*((volatile int *)IOASIC_SOUND_STATUS) & 0x0040))
	{
		delay_us(1);
		--timeout;
	}
	if(!timeout)
	{
		return(0xeeee);
	}
//	if(*((volatile int *)IOASIC_SOUND_DATA_IN) != 0xa)
//	{
//printf("\nsnd_send_command() - bogus data code\n");
//while(1) ;
//		return(0xeeee);
//	}
//	*((volatile int *)IOASIC_SOUND_DATA_IN) = 0;
	*((volatile int *)IOASIC_SOUND_DATA_OUT) = command;
	return(0);
}
	

static unsigned int snd_wait_for_completion(void)
{
	int	timeout;
	volatile unsigned int	completion_code;

	timeout = 2000000;
	while(!(*((volatile int *)IOASIC_SOUND_STATUS) & 0x0040))
	{
		--timeout;
		delay_us(1);
	}
	if(!timeout)
	{
		return(0xeeee);
	}
	completion_code = *((volatile int *)IOASIC_SOUND_DATA_IN) & 0xffff;
	*((volatile int *)IOASIC_SOUND_DATA_IN) = 0;
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

	*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
	*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<0);
	set_fcolor(0xffff);
	printf("Sound System Test - ");
	snd_reset();
	boot_version = snd_get_boot_version() & 0xffff;
	if(boot_version == 0xeeee)
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
	}
	else
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
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
	printf("UART Local Loopback Test - ");
	if(myputc != gputc)
	{
		printf("BYPASSED\n");
		return;
	}
	while(*((volatile int *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
	{
		(void)*((volatile int *)IOASIC_UART_RX);
	}
	*((volatile int *)IOASIC_UART_CONTROL) |= (1<<11);
	for(data = 0; data < 0x100; data++)
	{
		// Get status
		status = *((volatile int *)IOASIC_STATUS);

		// Transmit register empty ?
		if(!(status & IOASIC_UART_XMT_EMPTY))
		{
			// NOPE - Wait for it to empty
			while(!(status & IOASIC_UART_XMT_EMPTY))
			{
				status = *((volatile int *)IOASIC_STATUS);
			}
		}

		// Transmit the character
		*((volatile int *)IOASIC_UART_TX) = data;

		for(i = 0; i < 100000; i++)
		{
			if(*((volatile int *)IOASIC_STATUS) & IOASIC_UART_RCV_CHAR)
			{
				break;
			}
		}
		if(i == 100000)
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ioasic_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
		tmp = *((volatile int *)IOASIC_UART_RX) & 0xff;
		if(tmp != data)
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ioasic_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	*((volatile int *)IOASIC_UART_CONTROL) &= ~(1<<11);
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static int fifos_exist = 0;
static int fifo_size = 0;

static void fifotest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Sound FIFO Test - ");

	// Hold sound system in reset
	*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<0);

	// Enable the FIFOs
	*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<1);

	// Reset the FIFOs
	*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);

	// Is status empty, NOT half-full, and NOT full ?
	if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) == 1)
	{
		// YES - write 1 word to FIFOs
		*((volatile int *)SOUND_FIFO_ADDR) = 5;

		// Is status NOT empty, NOT half-full, and NOT full ?
		if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) != 0)
		{
			// NOPE - FIFO don't exist or are broken
			fifos_exist = 0;
			*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);
			*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
			*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<0);
			printf("NOT INSTALLED\n");
			set_fcolor(0xffff);
			return;
		}

		// Reset the FIFO's
		*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
		*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);

		// Fill FIFO's with 129 words
		for(i = 0; i < 129; i++)
		{
			*((volatile int *)SOUND_FIFO_ADDR) = 0;
		}

		// Does status indicate FIFO's are half full ?
		if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) == 2)
		{
			// YES - FIFO's are 256 words
			fifo_size = 256;
		}

		else
		{
			// Fill FIFO's with another 128 words
			for(i = 0; i < 128; i++)
			{
				*((volatile int *)SOUND_FIFO_ADDR) = 0;
			}

			// Does status indicate FIFO's are half full ?
			if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) == 2)
			{
				// YES - FIFO's are 512 words
				fifo_size = 512;
			}
			else
			{
				// Fill FIFO's with another 256 words
				for(i = 0; i < 256; i++)
				{
					*((volatile int *)SOUND_FIFO_ADDR) = 0;
				}

				// Does status indicate FIFO's are half full ?
				if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) == 2)
				{
					// YES - FIFO's are 512 words
					fifo_size = 1024;
				}
				else
				{
					// NOPE - FIFO's are broken
					fifos_exist = 0;
					*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
					*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);
					*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
					*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<0);
					set_fcolor(0xf800);
					printf("FAIL\n");
					set_fcolor(0xffff);
					return;
				}
			}
		}

		// Fill FIFO's the reset of the way
		for(i = 0; i < ((fifo_size - 1) >> 1); i++)
		{
			*((volatile int *)SOUND_FIFO_ADDR) = 0;
		}

		// Does status indicate FIFO's are full ?
		if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) != 6)
		{
			// NOPE
			fifos_exist = 0;
			*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);
			*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_fifo0_chip_num].status = CHIP_STATUS_FAILED;
			cpu_chips[cpu_fifo1_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}

		// Reset the FIFO's
		*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
		*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);

		// Does status indicate FIFO's empty ?
		if(((*((volatile int *)IOASIC_SOUND_STATUS) >> 3) & 7) != 1)
		{
			// NOPE
			fifos_exist = 0;
			*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
			*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);
			*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_fifo0_chip_num].status = CHIP_STATUS_FAILED;
			cpu_chips[cpu_fifo1_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}

		set_fcolor(0x07e0);
		printf("OK - %d WORDS\n", fifo_size);
		set_fcolor(0xffff);
		fifos_exist = 1;
		*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<0);
		*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
		return;
	}

	fifos_exist = 0;
	*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<2);
	*((volatile int *)IOASIC_SOUND_CONTROL) |= (1<<2);
	*((volatile int *)IOASIC_SOUND_CONTROL) &= ~(1<<1);
	printf("NOT INSTALLED\n");
	set_fcolor(0xffff);
}

void siotest(void)
{
	pic_status = pic_init();
	if(get_buttons())
	{
		return;
	}
	ttyinit();
	if(get_buttons())
	{
		return;
	}
	sound_init();
	if(get_buttons())
	{
		return;
	}
	cpu_chips[cpu_pic_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_pic_chip_num]);
	pictest();
	if(cpu_chips[cpu_pic_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_pic_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_pic_chip_num]);
	if(get_buttons())
	{
		return;
	}
	cpu_chips[cpu_cmos_ram_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_cmos_ram_chip_num]);
	cmoswalkonetest();
	if(get_buttons())
	{
		return;
	}
	cmoswalkzerotest();
	if(get_buttons())
	{
		return;
	}
	cmostest();
	if(cpu_chips[cpu_cmos_ram_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_cmos_ram_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_cmos_ram_chip_num]);
	if(get_buttons())
	{
		return;
	}
	cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_rtc_clock_chip_num]);
	rtctest();
	if(cpu_chips[cpu_rtc_clock_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_rtc_clock_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_rtc_clock_chip_num]);
	if(get_buttons())
	{
		return;
	}
	cpu_chips[cpu_ioasic_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_ioasic_chip_num]);
	asicuarttest();
	if(cpu_chips[cpu_ioasic_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_ioasic_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_ioasic_chip_num]);
	if(get_buttons())
	{
		return;
	}
	cpu_chips[cpu_fifo0_chip_num].status = CHIP_STATUS_TESTING;
	cpu_chips[cpu_fifo1_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_fifo0_chip_num]);
	draw_chip(&cpu_chips[cpu_fifo1_chip_num]);
	fifotest();
	if(fifo_size)
	{
		if(cpu_chips[cpu_fifo0_chip_num].status != CHIP_STATUS_FAILED &&
			cpu_chips[cpu_fifo1_chip_num].status != CHIP_STATUS_FAILED)
		{
			cpu_chips[cpu_fifo0_chip_num].status = CHIP_STATUS_GOOD;
			cpu_chips[cpu_fifo1_chip_num].status = CHIP_STATUS_GOOD;
		}
	}
	else
	{
		cpu_chips[cpu_fifo0_chip_num].status = CHIP_STATUS_NOT_STUFFED;
		cpu_chips[cpu_fifo1_chip_num].status = CHIP_STATUS_NOT_STUFFED;
	}
	draw_chip(&cpu_chips[cpu_fifo0_chip_num]);
	draw_chip(&cpu_chips[cpu_fifo1_chip_num]);
	if(get_buttons())
	{
		return;
	}
	cpu_chips[cpu_sound_dsp_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_sound_dsp_chip_num]);
	soundtest();
	if(cpu_chips[cpu_sound_dsp_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_sound_dsp_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_sound_dsp_chip_num]);
}
