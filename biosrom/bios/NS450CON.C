//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 2 $
//
// $Author: Mlynch $
//
/*
 * ns16450 console device
 */

#include	<system.h>

#if defined(TTY_DRIVER)
#include <io.h>
#include <ns450con.h>
#include <phoenix.h>
#include	<ioctl.h>

void ttyinput(register struct circ_buf *, register char);
void scandevs(void);
int circ_getc(register struct circ_buf *);


static consgetc();
static consputc(char c, int unit);

#define	PHYS_TO_K1(x)	((unsigned)(x)|0xA0000000)	/* physical to kseg1 */

#define NUM_UNIT 2
 
typedef volatile struct ns450dev *ns450dev;

static struct circ_buf ns450buf[NUM_UNIT];

static const int ns450clk = 8192000;

static int dbaud[NUM_UNIT];

#define CHANA	1
#define CHANB	0

static const struct ns450dev *ns450dp[NUM_UNIT] = {
(const struct ns450dev *)PHYS_TO_K1(UART1_BASE),
(const struct ns450dev *)PHYS_TO_K1(UART0_BASE)
};

const int	brate[] = {
  115200,			/* 115200 */
  57600,			/* 57600 */
  38400,			/* 38400 */
  19200,			/* 19200 */
  9600,				/* 9600  */
  4800,				/* 4800  */
  2400,				/* 2400  */
  2000,				/* 2000  */
  1800,				/* 1800  */
  1200,				/* 1200  */
  600,				/* 600   */
  300,				/* 300   */
  150,				/* 150   */
  110,				/* 110   */
  0				/* No change */
  };
char *const br[] = {
  "115200",
  "57600",
  "38400",
  "19200",
  "9600 ",
  "4800 ",
  "2400 ",
  "2000 ",
  "1800 ",
  "1200 ",
  "600  ",
  "300  ",
  "150  ",
  "110  ",
  "NoChg"
};
const int NUM_BAUDS = sizeof(br) / sizeof(br[0]);

void wbflush(void)
{
	__asm__("	sync");
}

/*
 * internal routines to access uart registers
 */
void ns450putreg(unsigned int *reg, int val)
{
	*reg = (val & 0xff);
	wbflush();
}

int ns450getreg(unsigned int *reg)
{
	return(*reg & 0xff);
}


int ttyinit(void)
{
	int	i;
	const struct ns450dev *dp;

	for(i = 0; i < NUM_UNIT; i++)
	{
		dbaud[i] = 38400;
		ns450buf[i].cb_flags = 0;
		ns450buf[i].cb_in = ns450buf[i].cb_out = 0;

		dp = ns450dp[i];

		/*
		 * global device initialisation
		 */

		ns450putreg((unsigned int *)&dp->ulinectl, 0x83);
		ns450putreg((unsigned int *)&dp->udata, 0x34);
		ns450putreg((unsigned int *)&dp->uintenab, 0);
		ns450putreg((unsigned int *)&dp->ulinectl, 0x3);

		ns450putreg((unsigned int *)&dp->umdmctl, 0x3);
		ns450putreg((unsigned int *)&dp->uintenab, 0);
	}
	return(1);
}

static void ns450setbaud(struct ns450dev *dp, int baudrate)
{
	ns450putreg((unsigned int *)&dp->ulinectl, 0x83);

	ns450putreg((unsigned int *)&dp->udata, (500000/baudrate));
	ns450putreg((unsigned int *)&dp->uintenab, ((500000/baudrate)>>8));

	ns450putreg((unsigned int *)&dp->ulinectl, 0x3);
}

/*
 * ns450setline - set parameters for unit
 */
void ns450setline(int unit)
{
	const struct ns450dev *dp = ns450dp[unit];

	/*
	 * See the ns450 manual for details.
	 * set baud, N81, and RTS/DTR
	 */
	ns450putreg((unsigned int *)&dp->uintenab, 0x00);
	ns450putreg((unsigned int *)&dp->ulinectl, ULCR_8BIT);
	ns450putreg((unsigned int *)&dp->umdmctl, UMCR_DTR | UMCR_RTS | UMCR_INTE);
	ns450putreg((unsigned int *)&dp->uintstat, 0x00);
	ns450setbaud((struct ns450dev *)dp, dbaud[unit]);

	/* clear status indications */
	ns450getreg((unsigned int *)&dp->uintstat);
	ns450getreg((unsigned int *)&dp->umdmstat);
}

/*
** ttyopen -- initialize duart
*/
int ttyopen(struct iocntb *io)
{
	unsigned unit;

	unit = io->icb_di->dev_unit;
	if(unit > NUM_UNIT) 
	{
		return(-1);
	}
	io->icb_flags |= F_SCAN;
	ns450setline(unit);
	CIRC_FLUSH(&ns450buf[unit]);	/* flush all pending input */
	CIRC_CLEAR(&ns450buf[unit]);	/* clear buf processing flags */
	return(0);
}

/*
** ttyread-- perform read operation 
*/
int ttyread(register struct iocntb *io, char *buf, int count)
{
	register struct circ_buf *cb;
	register c;
	int ocnt = count;

	io->icb_addr = buf;
	io->icb_count = count;
	cb = &ns450buf[io->icb_di->dev_unit];
	while(io->icb_count > 0)
	{
		while( (c = consgetc(io->icb_di->dev_unit)))
		{
			ttyinput(cb, c);
		}
		if((io->icb_flags & F_NBLOCK) == 0)
		{
			while(CIRC_EMPTY(cb))
			{
				scandevs();
			}
		}
		if(CIRC_EMPTY(cb))
		{
			return(ocnt - io->icb_count);
		}
		*io->icb_addr++ = circ_getc(cb);
		io->icb_count--;
	}
	return(ocnt);
}

/*
** ttywrite-- perform write operation 
*/
int ttywrite(register struct iocntb *io, char *buf, int count)
{
	io->icb_addr = buf;
	io->icb_count = count;
	while(io->icb_count-- > 0)
	{
		consputc(*io->icb_addr++, io->icb_di->dev_unit);
	}
	return(count);
}

/*
** ioctl routine - does all those device perculiar things
*/
int ttyioctl(struct iocntb *io, int cmd, int arg)
{
	unsigned unit = io->icb_di->dev_unit;
	struct circ_buf *cb = &ns450buf[unit];
	const struct ns450dev *dp = ns450dp[unit];
	int c;

	switch(cmd)
	{
		case FIOCSCAN:
		{
			while((c = consgetc(unit)))
			{
				ttyinput(cb, c);
			}
			break;
		}
		case FIOCINTBRK:
		{
			break;
		}
		case FIOCINTBRKNOT:
		{
			break;
		}
		case FIOCCLRINT:
		{
			break;
		}
		case TIOCBAUD:
		{
			if(brate[arg])
			{
				dbaud[unit] = brate[arg];
				ns450setbaud((struct ns450dev *)dp, dbaud[unit]);
			}
			break;		
		}
		case TIOCRAW:
		{
			if(arg)
			{
				cb->cb_flags |= CB_RAW;
			}
			else
			{
				cb->cb_flags &= ~CB_RAW;
			}
			break;		
		}
		case TIOCKEYHIT:
		{
			if(CIRC_EMPTY(cb))
			{
				*((int *)arg) = 0;
			}
			else
			{
				*((int *)arg) = 1;
			}
			break;
		}
		default:
		{
			return(-1);
		}
	}
	return(0);
}

static int consgetc(int unit)
{
	const struct ns450dev *dp = ns450dp[unit];
	int c;

	if((ns450getreg((unsigned int *)&dp->ulinestat) & 0x01) == 0)
	{
		return(0);
	}
	c = ns450getreg((unsigned int *)&dp->udata);
	return(0x100 | c);
}

static int consputc(char c, int unit)
{
	register struct circ_buf *cb;
	const struct ns450dev *dp = ns450dp[unit];

	cb = &ns450buf[unit];
	while(CIRC_STOPPED(cb))
	{
		scandevs();
	}
	while((ns450getreg((unsigned int *)&dp->ulinestat) & ULSR_THRE) == 0)
	{
		scandevs();
	}
	ns450putreg((unsigned int *)&dp->udata, c);
	wbflush();
	return(1);
}

void dputs(char *str)
{
	while(*str)
	{
		if(*str == '\n')
		{
			consputc('\r', 1);
		}
		consputc(*str, 1);
		++str;
	}
}

void dphex(int val)
{
	int	i;
	int	v;
	char	c;

	dputs("0x");
	for(i = 28; i >= 0; i -= 4)
	{
		v = (val >> i) & 0xf;
		if(v < 10)
		{
			v |= 0x30;
		}
		else
		{
			v += 0x37;
		}
		consputc(v, 1);
	}
}

#endif
