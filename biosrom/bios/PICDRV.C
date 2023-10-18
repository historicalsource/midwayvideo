// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 7 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

typedef struct dosdate
{
	unsigned char	day __attribute__((packed));
	unsigned char	month __attribute__((packed));
	unsigned short	year __attribute__((packed));
	unsigned char	dayofweek __attribute__((packed));
} dosdate_t;

typedef struct dostime
{
	unsigned char	hour;
	unsigned char	minute;
	unsigned char	second;
	unsigned char	hsecond;
} dostime_t;

typedef struct pic_dt
{
	unsigned char	seconds;				// 0 - 59
	unsigned char	minutes;				// 0 - 59
	unsigned char	hours;				// 0 - 23
	unsigned char	day_of_week;		// 1 - 7
	unsigned char	day_of_month;		// 1 - 31
	unsigned char	month;				// 1 - 12
	unsigned char	year;					// 0 - 99
#if (PHOENIX_SYS & VEGAS)
	unsigned char	hyear;				// 0 - 99
#endif
} pic_dt_t;

typedef union pic_dt_raw
{
	pic_dt_t			dt_struct;
	unsigned char	dt_data[sizeof(pic_dt_t)];
} pic_dt_raw_t;

typedef struct serial_data
{
	unsigned char	e;
	unsigned char	d;
	unsigned char	c;
	unsigned char	i;
	unsigned char	h;
	unsigned char	g;
	unsigned char	f;
	unsigned char	l;
	unsigned char	k;
	unsigned char	j;
	unsigned char	q;
	unsigned char	r;
	unsigned char	x;
	unsigned char	y;
	unsigned char	s;
	unsigned char	t;
} serial_data_t;

unsigned int	serial_number;
unsigned int	game_number;
unsigned int	date_of_manufacture;

#define	SERIAL_GET	1
#define	TIME_PREP	2
#define	TIME_GET		3
#define	TIME_SET		4

//#define	ACK_DELAY_TIME	5000000
#define	ACK_DELAY_TIME	50000000

#define	PIC_ACK		0x100
#define	PIC_REQ		0x010

static int wait_pic_ack(void)
{
	unsigned int	delay;
	unsigned int	status;

	for(delay = 0; delay < ACK_DELAY_TIME; delay++)
	{
#if (!(PHOENIX_SYS & VEGAS))
		status = *((volatile int *)IOASIC_PIC_DATA_IN);
#else
		status = *((volatile short *)IOASIC_PIC_DATA_IN);
#endif
		if(status & PIC_ACK)
		{
			break;
		}
	}
	return(status);
}

static int wait_pic_nack(void)
{
	unsigned int	delay;
	unsigned int	status;

	for(delay = 0; delay < ACK_DELAY_TIME; delay++)
	{
#if (!(PHOENIX_SYS & VEGAS))
		status = *((volatile int *)IOASIC_PIC_DATA_IN);
#else
		status = *((volatile short *)IOASIC_PIC_DATA_IN);
#endif
		if(!(status & PIC_ACK))
		{
			return(0);
		}
	}
	return(-1);
}

static int write_pic_command(unsigned char cmd)
{
	int	status;

	cmd &= 0xf;
	cmd |= 0x80;
#if (!(PHOENIX_SYS & VEGAS))
	*((volatile int *)IOASIC_PIC_COMMAND) = cmd | PIC_REQ;
#else
	*((volatile short *)IOASIC_PIC_COMMAND) = cmd | PIC_REQ;
#endif
	status = wait_pic_ack();
#if (!(PHOENIX_SYS & VEGAS))
	*((volatile int *)IOASIC_PIC_COMMAND) = 0;
#else
	*((volatile short *)IOASIC_PIC_COMMAND) = 0;
#endif
	if(!(status & PIC_ACK))
	{
		return(-1);
	}
	if((status & 0xff) != cmd)
	{
		return(-1);
	}
	return(wait_pic_nack());
}

static int write_nibbles(unsigned char c)
{
	int	status;

	if((status = write_pic_command(c & 0xf)) < 0)
	{
		return(status);
	}
	return(write_pic_command((c >> 4) & 0xf));
}

static int read_pic_data(unsigned char *data)
{
	int	status;

#if (!(PHOENIX_SYS & VEGAS))
	*((volatile int *)IOASIC_PIC_COMMAND) = PIC_REQ;
#else
	*((volatile short *)IOASIC_PIC_COMMAND) = PIC_REQ;
#endif
	status = wait_pic_ack();
	if(!(status & PIC_ACK))
	{
		return(-1);
	}
	*data = (status & 0xff);
#if (!(PHOENIX_SYS & VEGAS))
	*((volatile int *)IOASIC_PIC_COMMAND) = 0;
#else
	*((volatile short *)IOASIC_PIC_COMMAND) = 0;
#endif
	return(wait_pic_nack());
}

static void bcdtohex(char *v)
{
	int	val;

	val = (*v & 0xf);
	val += (((*v >> 4) & 0xf) * 10);
	*v = val;
}

static void hextobcd(char *v)
{
	int	val;

	val = *v % 10;
	val |= (((*v / 10) % 10) << 4);
	*v = val;
}

static int read_pic_serial(serial_data_t *sd)
{
	int	i;
	char	*buf;

	if(write_pic_command(SERIAL_GET))
	{
		return(-1);
	}
	buf = (char *)sd;
	for(i = 0; i < 16; i++)
	{
		if(read_pic_data(buf))
		{
			return(-1);
		}
		++buf;
	}
	return(0);
}

#if (!(PHOENIX_SYS & VEGAS))
static int read_pic_date_time(pic_dt_raw_t *dt)
{
	int	i;

	if(write_pic_command(TIME_PREP))
	{
		return(-1);
	}
	if(write_pic_command(TIME_GET))
	{
		return(-1);
	}
	for(i = 0; i < 7; i++)
	{
		if(read_pic_data(&dt->dt_data[i]))
		{
			return(-1);
		}
		bcdtohex(&dt->dt_data[i]);
	}
	return(0);
}

static int write_pic_date_time(pic_dt_raw_t *dt)
{
	int	status = 0;
	int	i;

	if(write_pic_command(TIME_SET))
	{
		return(-1);
	}
	for(i = 0; i < 7; i++)
	{
		hextobcd(&dt->dt_data[i]);
		if(write_nibbles(dt->dt_data[i]) < 0)
		{
			status = -1;
		}
	}
	return(status);
}
#else

static int write_pic_date_time(pic_dt_raw_t *dt)
{
	int				i;
	unsigned char	cntl;

	// Grab current control register
	cntl = *((unsigned char *)RTC_CONTROL_REG);

	// Turn on WRITE bit
	cntl |= RTC_W_BIT;

	// Unlock Device write lock
	*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

	// Write control register
	*((unsigned char *)RTC_CONTROL_REG) = cntl;

	// Write the eight registers
	for(i = 0; i < 8; i++)
	{
		// Unlock Device write lock
		*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

		// Convert the data to bcd
		hextobcd(&dt->dt_data[i]);

		if(i < 7)
		{
			// Write the data for this register
			*((unsigned char *)(RTC_SECONDS_REG + i)) = dt->dt_data[i];
		}
		else
		{
			// Write the data to the century register
			*((unsigned char *)RTC_ALARM_YEAR_REG) = dt->dt_data[i];
		}
	}

	// Turn off WRITE bit
	cntl &= ~RTC_W_BIT;

	// Unlock Device write lock
	*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

	// Write control register
	*((unsigned char *)RTC_CONTROL_REG) = cntl;
}

static int read_pic_date_time(pic_dt_raw_t *dt)
{
	int				i;
	unsigned char	cntl;

	// Grab current control register
	cntl = *((unsigned char *)RTC_CONTROL_REG);

	// Turn on READ bit
	cntl |= RTC_R_BIT;

	// Unlock Device write lock
	*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

	// Write control register
	*((unsigned char *)RTC_CONTROL_REG) = cntl;

	// Read the eight registers
	for(i = 0; i < 8; i++)
	{
		if(i < 7)
		{
			dt->dt_data[i] = *((unsigned char *)(RTC_SECONDS_REG + i));
		}
		else
		{
			dt->dt_data[i] = *((unsigned char *)RTC_ALARM_YEAR_REG);
		}
		bcdtohex(&dt->dt_data[i]);
	}

	// Turn off READ bit
	cntl &= ~RTC_R_BIT;

	// Unlock Device write lock
	*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

	// Write control register
	*((unsigned char *)RTC_CONTROL_REG) = cntl;
}
#endif

int get_pic_date(dosdate_t *date)
{
	pic_dt_raw_t	dt;

	if(read_pic_date_time(&dt) < 0)
	{
		return(-1);
	}
	date->day = dt.dt_struct.day_of_month;
	date->month = dt.dt_struct.month;
#if (!(PHOENIX_SYS & VEGAS))
	date->year = dt.dt_struct.year + 1980;
#else
	date->year = (short)dt.dt_struct.hyear * 100;
	date->year += (short)dt.dt_struct.year;
#endif
	date->dayofweek = dt.dt_struct.day_of_week - 1;
	return(0);
}

int set_pic_date(dosdate_t *date)
{
	pic_dt_raw_t	dt;

	if(read_pic_date_time(&dt) < 0)
	{
		return(-1);
	}
	dt.dt_struct.day_of_month = date->day;
	dt.dt_struct.month = date->month;
#if (!(PHOENIX_SYS & VEGAS))
	dt.dt_struct.year = date->year - 1980;
#else
	dt.dt_struct.hyear = date->year / 100;
	dt.dt_struct.year = date->year % 100;
#endif
	dt.dt_struct.day_of_week = date->dayofweek + 1;
	if(write_pic_date_time(&dt))
	{
		return(-1);
	}
	return(0);
}

int get_pic_time(dostime_t *time)
{
	pic_dt_raw_t	dt;

	if(read_pic_date_time(&dt) < 0)
	{
		return(-1);
	}
	time->hour = dt.dt_struct.hours;
	time->minute = dt.dt_struct.minutes;
	time->second = dt.dt_struct.seconds;
	time->hsecond = 0;
	return(0);
}

int set_pic_time(dostime_t *time)
{
	pic_dt_raw_t	dt;

	if(read_pic_date_time(&dt) < 0)
	{
		return(-1);
	}
	dt.dt_struct.hours = time->hour;
	dt.dt_struct.minutes = time->minute;
	dt.dt_struct.seconds = time->second;
	if(write_pic_date_time(&dt) < 0)
	{
		return(-1);
	}
	return(0);
}

static int bcdtod(char *buf, int num)
{
	int	val = 0;
	int	mult = 1;

	buf += (num - 1);

	while(num--)
	{
		val += (*buf * mult);
		mult *= 10;
		--buf;
	}
	return(val);
}


static void	itoa(unsigned int value, char *string)
{
	char		tmp[33];
	char		*tp = tmp;
	int		i;
	unsigned	v;
	char		*sp;

	v = (unsigned)value;

	while(v || tp == tmp)
	{
		i = v % 10;
		v = v / 10;
		if(i < 10)
		{
			*tp++ = i+'0';
		}
		else
		{
			*tp++ = i + 'a' - 10;
		}
	}

	sp = string;

	while(tp > tmp)
	{
		*sp++ = *--tp;
	}
	*sp = 0;
}

static int strlen(char *str)
{
	int	i = 0;

	while(*str)
	{
		++i;
		++str;
	}
	return(i);
}

static void add_lead_zeros(char *str, int min)
{
	int	i;
	char	*tmp;
	char	buffer[16];

	if(strlen(str) == min)
	{
		return;
	}
	else
	{
		for(i = 0; i < min - strlen(str); i++)
		{
			buffer[i] = '0';
		}
		tmp = str;
		while(i < min)
		{
			buffer[i] = *tmp++;
			i++;
		}
		for(i = 0; i < min; i++)
		{
			str[i] = buffer[i];
		}
		str[i] = 0;
	}
}

static void decode_serial(serial_data_t *sd)
{
	int	val;
	char	convert[12];
	char	str[16];

	val = (sd->c << 16) | (sd->d << 8) | sd->e;
	val &= 0xffffff;
	val -= 15732;
	val /= 581;
	val -= (int)(sd->x);
	itoa((unsigned)val, str);
	add_lead_zeros(str, 4);
	convert[0xb] = str[0] - '0';
	convert[0x3] = str[1] - '0';
	convert[0x5] = str[2] - '0';
	convert[0x9] = str[3] - '0';

	val = (sd->f << 24) | (sd->g << 16) | (sd->h << 8) | sd->i;
	val &= 0xffffffff;
	val -= 7463513;
	val /= 4223;
	val -= sd->y;
	val -= sd->y;
	val -= (sd->x);
	itoa((unsigned)val, str);
	add_lead_zeros(str, 5);
	convert[0x2] = str[0] - '0';
	convert[0xa] = str[1] - '0';
	convert[0x0] = str[2] - '0';
	convert[0x8] = str[3] - '0';
	convert[0x6] = str[4] - '0';

	val = (sd->j << 16) | (sd->k << 8) | sd->l;
	val &= 0xffffff;
	val -= 127984;
	val /= 7117;
	val -= (sd->y * 5);
	itoa((unsigned)val, str);
	add_lead_zeros(str, 3);
	convert[0x1] = str[0] - '0';
	convert[0x7] = str[1] - '0';
	convert[0x4] = str[2] - '0';

	serial_number = bcdtod(&convert[3], 6);
	game_number = bcdtod(convert, 3);

	date_of_manufacture = (sd->q << 8) | (sd->r);
	date_of_manufacture &= 0xffff;
}

static int get_serial_number(void)
{
	serial_data_t	sd;

	if(read_pic_serial(&sd))
	{
		return(-1);
	}
	decode_serial(&sd);
	return(0);
}

int pic_init(void)
{
	if(get_serial_number())
	{
		return(-1);
	}
	return(0);
}


int pic_ioctl(register struct iocntb *io, int cmd, int arg)
{
	switch(cmd)
	{
		case FIOCGETSERIALNUMBER:
		{
			*((volatile unsigned int *)arg) = serial_number;
			break;
		}
		case FIOCGETGAMENUMBER:
		{
			*((volatile unsigned int *)arg) = game_number;
			break;
		}
		case FIOCGETDOM:
		{
			*((volatile unsigned int *)arg) = date_of_manufacture;
			break;
		}
		case FIOCSETSDATE:
		{
			return(set_pic_date((dosdate_t *)arg));
		}
		case FIOCGETSDATE:
		{
			return(get_pic_date((dosdate_t *)arg));
		}
		case FIOCSETSTIME:
		{
			return(set_pic_time((dostime_t *)arg));
		}
		case FIOCGETSTIME:
		{
			return(get_pic_time((dostime_t *)arg));
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}
