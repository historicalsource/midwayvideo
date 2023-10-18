/*
 * Templates onto the memory space of the duart.
 */

struct ns450dev
{
  unsigned int	udata;
  unsigned int	uintenab;
  unsigned int	uintstat;
  unsigned int	ulinectl;
  unsigned int	umdmctl;
  unsigned int	ulinestat;
  unsigned int	umdmstat;
  unsigned int	uscratch;
};

/* convert baudrate into a value for the BRG */
#define BRTCHI(clk, brate) (((clk/16) / brate) >> 8)
#define BRTCLO(clk, brate) (((clk/16) / brate) & 0x000000ff)

/* UART register defines */
#define UART_RCVHOLD	(0x0000 << 2)
#define UART_INTSTAT	(0x0002 << 2)
#define UART_LINESTAT	(0x0005 << 2)
#define UART_MDMSTAT	(0x0006 << 2)
#define UART_SCRATCH	(0x0007 << 2)

#define UART_XMITHOLD	(0x0000 << 2)
#define UART_INTENAB	(0x0001 << 2)
#define UART_FIFOCTL	(0x0002 << 2)
#define UART_LINECTL	(0x0003 << 2)
#define UART_MDMCTL	(0x0004 << 2)

#define UART_DIVLOW	(0x0000 << 2)
#define UART_DIVHIGH	(0x0001 << 2)

/* Line status register bits */
#define ULSR_THRE	0x20

/* Line Control register bits */
#define ULCR_DLAB	0x80
#define ULCR_BRK	0x40
#define ULCR_FORCE	0x20
#define ULCR_EVNP	0x10
#define ULCR_PAR	0x08
#define ULCR_8BIT	0x03
#define ULCR_7BIT	0x02

/* Modem Control Register bits */
#define UMCR_DTR	0x01
#define UMCR_RTS	0x02
#define UMCR_INTE	0x08
#define UMCR_LOOP	0x10

/* FIFO Control Register bits */
#define UFCR_ENAB	0x01
#define UFCR_RRST	0x02
#define UFCR_TRST	0x04
#define UFCR_LEV1	0x00
#define UFCR_LEV4	0x40
#define UFCR_LEV8	0x80
#define UFCR_LEV14	0xc0

