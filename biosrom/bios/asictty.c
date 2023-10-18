// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 9 $
//
// $Author: Mlynch $
//

//
// I/O ASIC Uart Driver
//
#include	<system.h>

#if (PHOENIX_SYS & VEGAS)
#define	INT	short
#else
#define	INT	int
#endif

#if defined(TTY_DRIVER)
#include <io.h>
#include <phoenix.h>
#include	<ioctl.h>


//
// Function prototypes for externs
//
extern void ttyinput(struct circ_buf *, char);
extern void scandevs(void);
extern char circ_getc(struct circ_buf *);

//
// Routines known to the outside world which are living here.
//
int ttyinit(void);
int ttyioctl(struct iocntb *, int, int);
int ttyopen(struct iocntb *);
int ttyread(register struct iocntb *, char *, int);
int ttywrite(register struct iocntb *, char *, int);


//
// Internal routines
//
static int UARTgetc(void);
int UARTputc(char);

//
// Data definitions
//
static struct circ_buf IOAQueue;
static int IOASICStatus = 0;
static int initialized = 0;

char SpeedTable [4] = {
IOASIC_UART_38400_BAUD,		// 38,461.5 Baud
IOASIC_UART_19200_BAUD,		// 19,230.8 Baud
IOASIC_UART_9600_BAUD,		//  9,615.4 Baud
IOASIC_UART_4800_BAUD		//  4,807.7 Baud
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
				ttyinput(&IOAQueue, c);
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
				IOAQueue.cb_flags |= CB_RAW;
			}
			else
			{
				IOAQueue.cb_flags &= ~CB_RAW;
			}
			break;
		}
		case TIOCKEYHIT:
		{
			if(CIRC_EMPTY(&IOAQueue))
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
	unsigned int	delay;
	unsigned short	value;

	// Clear the Queue
	IOAQueue.cb_flags = 0;
	IOAQueue.cb_in = IOAQueue.cb_out = 0;
  
	// Reset the UART
	*((volatile INT *)IOASIC_UART_CONTROL) = IOASIC_UART_RESET;
  
	for(delay = 0; delay < 1000; delay++) ;
  
	*((volatile INT *)IOASIC_UART_CONTROL) = (IOASIC_UART_INIT|SpeedTable[2]);

	// Enable the UART
	*((volatile INT *)IOASIC_UART_CONTROL) |= IOASIC_UART_ENABLE;

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
	// Enable I/O scanning
	io->icb_flags |= F_SCAN;

	// Flush the queue
	CIRC_FLUSH(&IOAQueue);

	// Clear the queue
	CIRC_CLEAR(&IOAQueue);

	// Return success
	return(0);
}

//
// ttyread() - read function for I/O ASIC driver.
//
int ttyread(register struct iocntb *io, char *buf, int count)
{
	register	c;
	int		ocount = count;

	// Set up the IO control block information
	io->icb_addr = buf;
	io->icb_count = count;

	// Get characters until count is exhausted
	while(io->icb_count > 0)
	{
		// Shove characters into the queue as long as they are available
		while(c = UARTgetc())
		{
			ttyinput(&IOAQueue, c);
		}

		// Should we block ?
		if(!(io->icb_flags & F_NBLOCK))
		{
			// YES - Wait for the queue to be NOT empty
			while(CIRC_EMPTY(&IOAQueue))
			{
				// Scan devices until we get characters
				scandevs();
			}
		}

		// Is queu empty ?
		if(CIRC_EMPTY(&IOAQueue))
		{
			// YES - return number of characters read
			return(ocount - io->icb_count);
		}

		// Increment buffer address
		*io->icb_addr++ = circ_getc(&IOAQueue);

		// Decrement count
		io->icb_count--;
	}

	// Return characters read
	return(ocount);
}

//
// ttywrite() - write function for I/O ASIC driver.
//
int ttywrite(register struct iocntb *io, char *buf, int count)
{
	register char	c;

	// Set up I/O control block information
	io->icb_addr = buf;
	io->icb_count = count;

	// Send as many characters as requested
	while(io->icb_count-- > 0)
	{
		UARTputc(*io->icb_addr++);
	}

	// Return count of number sent
	return(count);
}

//
// UARTgetc() - If character waiting, read it, else return -1.
//
static int UARTgetc(void)
{
	unsigned short val;

	// Get UART status  
	val = *((volatile INT *)IOASIC_STATUS);
  
	// Is data available ?
	if(val & IOASIC_UART_RCV_CHAR) 
	{
		val = *((volatile INT *)IOASIC_UART_RX);
		return((val & 0x00ff) | 0x0100);
	}

	// No data - return 0
	return(0);
}

//
// UARTputc() - If character can be output, do it.
//

int UARTputc(char c)
{
	unsigned short status;
	unsigned short	output = 0;

	// Has the transmitter been stopped ?
	if(CIRC_STOPPED(&IOAQueue)) 
	{
		// YES - Wait for it to resume
		while(CIRC_STOPPED(&IOAQueue))
		{
			// Scan devices with F_SCAN while waiting
			scandevs();
		}
	}

	// Get status
	status = *((volatile INT *)IOASIC_STATUS);

	// Transmit register empty ?
	if(!(status & IOASIC_UART_XMT_EMPTY))
	{
		// NOPE - Wait for it to empty
		while(!(status & IOASIC_UART_XMT_EMPTY))
		{
			// Scan devices with F_SCAN while waiting
			scandevs();
			status = *((volatile INT *)IOASIC_STATUS);
		}
	}

	// Transmit the character
	output = c;
	*((volatile INT *)IOASIC_UART_TX) = output;

	// Return success
	return(0);
}

int dputc(char c)
{
	unsigned short status;
	unsigned short	output = 0;

	if(!initialized)
	{
		if(unlock_ioasic() < 0)
		{
			if(unlock_ioasic() < 0)
			{
				return;
			}
		}
		ttyinit();
		if(!initialized)
		{
			return(0);
		}
	}

	// If a '\n' do a '\r' first
	if(c == '\n')
	{
		dputc('\r');
	}

	// Get status
	status = *((volatile INT *)IOASIC_STATUS);

	// Transmit register empty ?
	if(!(status & IOASIC_UART_XMT_EMPTY))
	{
		// NOPE - Wait for it to empty
		while(!(status & IOASIC_UART_XMT_EMPTY))
		{
			status = *((volatile INT *)IOASIC_STATUS);
		}
	}

	// Transmit the character
	output = c;
	*((volatile INT *)IOASIC_UART_TX) = output;

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
