// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 6 $
//
// $Author: Mlynch $
//

//
// NILE IV - Uart Driver
//
#include	<system.h>

#if defined(TTY_DRIVER)
#include <io.h>
#include <phoenix.h>
#include	<ioctl.h>

#ifdef TTY_INTERRUPTS
//#define	ALLOW_BREAK_INT
#endif

//
// Function prototypes for externs
//
extern void ttyinput(struct circ_buf *, char);
extern void scandevs(void);
extern char circ_getc(struct circ_buf *);
extern void circ_putc(int, register struct circ_buf *);


extern int	nile4_version;

//
// Routines known to the outside world which are living here.
//
int ttyinit(void);
int ttyioctl(struct iocntb *, int, int);
int ttyopen(struct iocntb *);
int ttyread(register struct iocntb *, char *, int);
int ttywrite(register struct iocntb *, char *, int);
void dputs(char *str);
int dputc(char c);


//
// Internal routines
//
#ifndef TTY_INTERRUPTS
int UARTgetc(void);
int UARTputc(char);
#else
static int UARTgetc(void);
static int UARTputc(char);
#endif

//
// Data definitions
//
static struct circ_buf NILE4Queue;
#ifdef TTY_INTERRUPTS
static struct circ_buf NILE4OQueue;
#endif
static int initialized = 0;

typedef struct nile4_uart_init_data
{
	unsigned			addr;
	unsigned	char	data;
} nile4_uart_init_data_t;


#define	BR_DLL(clk, brate)	(((clk) / (brate)) & 0xff)
#define	BR_DLM(clk, brate)	((((clk) / (brate)) >> 8) & 0xff)

#define	CLK_SPEED		PROCESSOR_CLOCK_SPEED
#define	UART_CLK_RES	((CLK_SPEED/12)/16)
#define	BAUD_RATE		57600
//#define	BAUD_RATE		9600
//#define	BAUD_RATE		38400
//#define	BAUD_RATE		115200
//#define	CHAR_TICKS		((CLK_SPEED/BAUD_RATE)*25)
#define	CHAR_TICKS		((CLK_SPEED/BAUD_RATE)*12)

static nile4_uart_init_data_t	nile4_uart_init_table[] = {
{NILE4_UART_LCR, ULCR_DLAB},
{NILE4_UART_DLL, BR_DLL(UART_CLK_RES, BAUD_RATE)},
{NILE4_UART_DLM, BR_DLM(UART_CLK_RES, BAUD_RATE)},
{NILE4_UART_LCR, 0},
{NILE4_UART_LCR, ULCR_8BIT},
{NILE4_UART_MCR, (UMCR_DTR | UMCR_RTS)},
{NILE4_UART_IER, 0},
{NILE4_UART_FCR, 0},
{NILE4_UART_IER, 1}
};

//
// ttyioctl() - ioctl function for I/O ASIC driver
//
int ttyioctl(struct iocntb *io, int cmd, int arg)
{
	int	c;
  
	switch(cmd) 
	{
		case FIOCSCAN:
		{
			while(c = UARTgetc())
			{
				ttyinput(&NILE4Queue, c);
			}
			break;
		}
		case FIOCINTBRK:
		case FIOCINTBRKNOT:
		case TIOCBAUD:
		{
			break;
		}
		case TIOCRAW:
		{
			if(arg)
			{
				NILE4Queue.cb_flags |= CB_RAW;
			}
			else
			{
				NILE4Queue.cb_flags &= ~CB_RAW;
			}
			break;
		}
		case TIOCKEYHIT:
		{
			if(CIRC_EMPTY(&NILE4Queue))
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

//
// ttyinit() - init function for I/O ASIC driver.
//
int ttyinit(void)
{
	int				i;

	// Clear the Queue
	NILE4Queue.cb_flags = 0;
	NILE4Queue.cb_in = NILE4Queue.cb_out = 0;

#ifdef TTY_INTERRUPTS
	NILE4OQueue.cb_flags = 0;
	NILE4OQueue.cb_in = NILE4OQueue.cb_out = 0;
#endif

	// Turn on the UART reset bit
	*((volatile int *)NILE4_UART_SCR) = 0x1a5;
	for(i = 0; i < 500; i++)
	{
		delay_us(1);
	}

	// Turn the UART reset bit off
	*((volatile int *)NILE4_UART_SCR) = 0x5a;
	for(i = 0; i < 500; i++)
	{
		delay_us(1);
	}

	// Initialize the UART
	for(i = 0; i < sizeof(nile4_uart_init_table)/sizeof(nile4_uart_init_data_t); i++)
	{
		*((volatile unsigned char *)nile4_uart_init_table[i].addr) = nile4_uart_init_table[i].data;
	}

	for(i = 0; i < 1000000; i++)
	{
		delay_us(1);
	}

	// Flush the queue
	CIRC_FLUSH(&NILE4Queue);

	// Clear the queue
	CIRC_CLEAR(&NILE4Queue);

#ifdef TTY_INTERRUPTS
	// Flush the queue
	CIRC_FLUSH(&NILE4OQueue);

	// Clear the queue
	CIRC_CLEAR(&NILE4OQueue);
#endif

	// Set initialized flag
	initialized = 1;

	// Return OK
	return(0);
}

//
// ttyopen() - Device open for I/O ASIC driver.
//
int ttyopen(struct iocntb *io)
{
#ifndef TTY_INTERRUPTS
	// Enable I/O scanning
	io->icb_flags |= F_SCAN;
#endif

	// Flush the queue
	CIRC_FLUSH(&NILE4Queue);

	// Clear the queue
	CIRC_CLEAR(&NILE4Queue);

#ifdef TTY_INTERRUPTS
	if((io->icb_flags & (F_READ|F_WRITE)) == F_READ)
	{
		io->icb_flags |= F_NBLOCK;
	}
	else
	{
		io->icb_flags &= ~F_NBLOCK;
	}

	// Flush the queue
	CIRC_FLUSH(&NILE4OQueue);

	// Clear the queue
	CIRC_CLEAR(&NILE4OQueue);

	// Enable the Rx interrupts
#ifndef ALLOW_BREAK_INT
	*((volatile unsigned char *)NILE4_UART_IER) = 1;
#else
	*((volatile unsigned char *)NILE4_UART_IER) = 5;
#endif
#endif

	// Return success
	return(0);
}

//
// ttyread() - read function for I/O ASIC driver.
//
int ttyread(register struct iocntb *io, char *buf, int count)
{
#ifndef TTY_INTERRUPTS
	register	c;
#endif
	int		ocount = count;

	// Set up the IO control block information
	io->icb_addr = buf;
	io->icb_count = count;

	// Get characters until count is exhausted
	while(io->icb_count > 0)
	{
#ifndef TTY_INTERRUPTS
		// Shove characters into the queue as long as they are available
		while(c = UARTgetc())
		{
			ttyinput(&NILE4Queue, c);
		}

		// Should we block ?
		if(!(io->icb_flags & F_NBLOCK))
		{
			// YES - Wait for the queue to be NOT empty
			while(CIRC_EMPTY(&NILE4Queue))
			{
				// Scan devices until we get characters
				scandevs();
			}
		}

		// Is queu empty ?
		if(CIRC_EMPTY(&NILE4Queue))
		{
			// YES - return number of characters read
			return(ocount - io->icb_count);
		}

		// Increment buffer address
		*io->icb_addr++ = circ_getc(&NILE4Queue);

		// Decrement count
		io->icb_count--;
#else
		// Should we block ?
		if(!(io->icb_flags & F_NBLOCK))
		{
			// YES - Wait for characters
			while(CIRC_EMPTY(&NILE4Queue)) ;
		}

		// Are there characters in the queue ?
		else if(CIRC_EMPTY(&NILE4Queue))
		{
			return(ocount - io->icb_count);
		}

		// Get a character from the queue and put in user buffer
		*io->icb_addr++ = circ_getc(&NILE4Queue);

		// Decrement count of available characters
		io->icb_count--;
#endif
	}

	// Return characters read
	return(ocount);
}

#ifdef TTY_INTERRUPTS
int circ_full(register struct circ_buf *cb)
{
	char *cp;

	cp = cb->cb_in + 1 >= &cb->cb_buf[sizeof(cb->cb_buf)] ? cb->cb_buf : cb->cb_in + 1;
	if(cp == cb->cb_out)
	{
		return(1);
	}
	return(0);
}
#endif

extern volatile int	exception_level;

//
// ttywrite() - write function for I/O ASIC driver.
//
int ttywrite(register struct iocntb *io, char *buf, int count)
{
	register char	c;
#ifdef TTY_INTERRUPTS
	register int	ocount = 0;
#endif

	// Are we in exception processing ?
	if(exception_level)
	{
#ifdef TEST
		int ocount;
#endif
		ocount = count;
		while(count--)
		{
			dputc(*buf++);
		}

		// Done
		return(ocount);
	}

	// Set up I/O control block information
	io->icb_addr = buf;
	io->icb_count = count;

#ifndef TTY_INTERRUPTS
	// Send as many characters as requested
	while(io->icb_count-- > 0)
	{
		UARTputc(*io->icb_addr++);
	}

	// Return count of number sent
	return(count);
#else
	while(count--)
	{
		while(circ_full(&NILE4OQueue))
		{
			if(!(*((volatile unsigned char *)NILE4_UART_IER) & 2))
			{
#ifndef ALLOW_BREAK_INT
				*((volatile unsigned char *)NILE4_UART_IER) = 3;
#else
				*((volatile unsigned char *)NILE4_UART_IER) = 7;
#endif
			}
			if(io->icb_flags & F_NBLOCK)
			{
				return(ocount);
			}
		}
		circ_putc(*io->icb_addr++, &NILE4OQueue);
		ocount++;
	}
	if(!CIRC_EMPTY(&NILE4OQueue))
	{
		if(!(*((volatile unsigned char *)NILE4_UART_IER) & 2))
		{
#ifndef ALLOW_BREAK_INT
			*((volatile unsigned char *)NILE4_UART_IER) = 3;
#else
			*((volatile unsigned char *)NILE4_UART_IER) = 7;
#endif
		}
	}
	return(ocount);
#endif
}

#ifdef TTY_INTERRUPTS
//
// ttyintr() - Function that gets called when an interrupt from the UART
// happens
//
void ttyintr(void)
{
	register unsigned char	status;
	register unsigned char	c;

	status = *((volatile unsigned char *)NILE4_UART_IIR);
	if(status & 1)
	{
		// Spurious interrupt
		return;
	}
	status >>= 1;
	status &= 7;
	switch(status)
	{
		case 0:	// Modem status interrupt
		{
			break;
		}
		case 1:	// Tx Holding register empty interrupt
		{
			if(CIRC_EMPTY(&NILE4OQueue))
			{
				// Disable Tx interrupts, Leave Rx interrupts on
#ifndef ALLOW_BREAK_INT
				*((volatile unsigned char *)NILE4_UART_IER) = 1;
#else
				*((volatile unsigned char *)NILE4_UART_IER) = 5;
#endif
			}
			else
			{
				*((volatile unsigned char *)NILE4_UART_THR) = (unsigned char)circ_getc(&NILE4OQueue);
			}
			break;
		}
		case 2:	// Rx Data available interrupt
		{
			c = *((volatile unsigned char *)NILE4_UART_RBR);
			circ_putc((int)c, &NILE4Queue);
			break;
		}
		case 3:	// Rx Line status interrupt
		{
#ifdef ALLOW_BREAK_INT
			status = *((volatile unsigned char *)NILE4_UART_LSR);
			if(status & 0x10)
			{
				*((volatile char *)LED_ADDR) ^= 0x20;
			}
			c = *((volatile unsigned char *)NILE4_UART_RBR);
#endif
			break;
		}
		case 6:	// Character timeout
		{
			break;
		}
		default:	// Unknown
		{
			break;
		}
	}
}
#endif

//
// UARTgetc() - If character waiting, read it, else return -1.
//
#ifdef TTY_INTERRUPTS
static int UARTgetc(void)
#else
int UARTgetc(void)
#endif
{
#if 0
	unsigned char val;

	// Wait 175 us
	delay_us(175);

	// Get UART status  
	val = (unsigned char)*((volatile unsigned char *)NILE4_UART_LSR);
  
	// Is data available ?
	if(val & ULSR_DR)
	{
		val = (unsigned char)*((volatile unsigned char *)NILE4_UART_RBR);
		return((val & 0x00ff) | 0x0100);
	}

	// No data - return 0
	return(0);
#else
	unsigned char	status;

	status = *((volatile unsigned char *)NILE4_UART_IIR);
	if(!(status & 1))
	{
		status >>= 1;
		if((status & 7) == 2)
		{
			status = *((volatile unsigned char *)NILE4_UART_RBR);
			return((int)status | 0x100);
		}
	}
	return(0);
#endif
}


static void out_delay(void)
{
	if(nile4_version < 2)
	{
		clear_count();
		while(gcount() < CHAR_TICKS) ;
	}
}

//
// UARTputc() - If character can be output, do it.
//

#ifdef TTY_INTERRUPTS
static int UARTputc(char c)
#else
int UARTputc(char c)
#endif
{
	unsigned char	status;

	if(nile4_version < 2)
	{
		out_delay();
	}
	else
	{
		while(!(*((volatile char *)NILE4_UART_LSR) & ULSR_THRE)) delay_us(175);
	}

	// Transmit the character
	*((volatile unsigned char *)NILE4_UART_THR) = c;
	delay_us(175);

	// Return success
	return(0);
}

int dputc(char c)
{
	unsigned char	status;

	// Has UART been initialized ?
	if(!initialized)
	{
		// NOPE - Initialize it
		ttyinit();

		// Still NOT initialized
		if(!initialized)
		{
			// Return fail
			return(0);
		}
	}

	// If a '\n' do a '\r' first
	if(c == '\n')
	{
		dputc('\r');
	}

	if(nile4_version < 2)
	{
		out_delay();
	}
	else
	{
		while(!(*((volatile char *)NILE4_UART_LSR) & ULSR_THRE)) delay_us(175);
	}

	// Transmit the character
	*((volatile unsigned char *)NILE4_UART_THR) = c;
	delay_us(175);

	return(1);
}

void dputs(char *str)
{
	while(*str)
	{
		dputc(*str);
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
		dputc(v);
	}
}

#endif /* TTY_DRIVER */
