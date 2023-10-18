#include	<stdio.h>
#include	<ioctl.h>
#include	<goose/switch.h>

char	goose_switch_c_version[] = {"$Revision: 18 $"};

static int	__use_49way = 0;		// set non-zero for Blitz '98 control panel configurations
int			I40_present = 0;		// non-static, but should only be written by functions in switch.c

static int get_mplex_stick( int low_n, int high_n );
static void __burn_200ns( void );
static void blitz_stick_decode( int *, int * );


/*
** Index with raw 49way to get direction #'s corresponding as:
**   UL  .  .  U  .  . UR   =   00 01 02 03 04 05 06
**    .  -  .  -  .  -  .   =   07 08 09 10 11 12 13
**    .  .  -  -  -  .  .   =   14 15 24 24 24 19 20
**    L  -  -  C  -  -  R   =   21 22 24 24 24 26 27
**    .  .  -  -  -  .  .   =   28 29 24 24 24 33 34
**    .  -  .  -  .  -  .   =   35 36 37 38 39 40 41
**   DL  .  .  D  .  . DR   =   42 43 44 45 46 47 48
*/
char xlat_49way[] = {
	24, 24, 24, 22, 22, 22, 22, 21, 24, 24, 24, 24, 24, 24, 26, 27,
	24, 24, 24, 15, 15, 15, 15, 14, 24, 24, 24, 24, 24, 24, 19, 20,
	24, 24, 24, 15, 15, 15, 15, 14, 24, 24, 24, 24, 24, 24, 19, 20,
	10,  9,  9,  8,  8,  8,  8,  7, 10, 10, 10, 10, 11, 11, 12, 13,
	10,  9,  9,  8,  8,  8,  8,  7, 10, 10, 10, 10, 11, 11, 12, 13,
	10,  9,  9,  8,  8,  8,  8,  7, 10, 10, 10, 10, 11, 11, 12, 13,
	10,  9,  9,  8,  8,  8,  8,  7, 10, 10, 10, 10, 11, 11, 12, 13,
	 3,  2,  2,  1,  1,  1,  1,  0,  3,  3,  3,  3,  4,  4,  5,  6,
	24, 24, 24, 22, 22, 22, 22, 21, 24, 24, 24, 24, 24, 24, 26, 27,
	24, 24, 24, 22, 22, 22, 22, 21, 24, 24, 24, 24, 24, 24, 26, 27,
	24, 24, 24, 22, 22, 22, 22, 21, 24, 24, 24, 24, 24, 24, 26, 27,
	24, 24, 24, 22, 22, 22, 22, 21, 24, 24, 24, 24, 24, 24, 26, 27,
	24, 24, 24, 29, 29, 29, 29, 28, 24, 24, 24, 24, 24, 24, 33, 34,
	24, 24, 24, 29, 29, 29, 29, 28, 24, 24, 24, 24, 24, 24, 33, 34,
	38, 37, 37, 36, 36, 36, 36, 35, 38, 38, 38, 38, 39, 39, 40, 41,
	45, 44, 44, 43, 43, 43, 43, 42, 45, 45, 45, 45, 46, 46, 47, 48
};

static int	stuck_psw;

/*
** Index with raw 8way to get direction #'s corresponding as:
**   UL  U  UR   =   00 03 06
**    L  C   R   =   21 24 27
**   DL  D  DR   =   42 45 48
*/
char xlat_8way[] = {
	24, 24, 24, 24, 24, 48,  6, 27, 24, 42,  0, 21, 24, 45,  3, 24
};

/*
** Index with 49way direction # to get corresponding 8way RLDU
*/
char xlat_49to8[] = {
	0x5, 0x5, 0x1, 0x1, 0x1, 0x9, 0x9,
	0x5, 0x5, 0x1, 0x1, 0x1, 0x9, 0x9,
	0x4, 0x4, 0x0, 0x0, 0x0, 0x8, 0x8,
	0x4, 0x4, 0x0, 0x0, 0x0, 0x8, 0x8,
	0x4, 0x4, 0x0, 0x0, 0x0, 0x8, 0x8,
	0x6, 0x6, 0x2, 0x2, 0x2, 0xa, 0xa,
	0x6, 0x6, 0x2, 0x2, 0x2, 0xa, 0xa,
};

#if defined(VEGAS)
void get_inputs(idata_t *);
static idata_t	input_data;
static int		coin_drops[] = {0, 0, 0, 0, 0};
#endif

static void null_sw_handler(int, int);

/* Player input stack */
static int	p1234sw_FIFO[2];
static int	p1234sw_cur;
static int	p1234sw_close;
static int	p1234sw_open;
static int	p12xxsw_cur;		// 49-way switches, p1-4

static int	p5sw_FIFO[2];		// extra switches on I40 board
static int	p5sw_cur;
static int	p5sw_close;
static int	p5sw_open;

/* Coin and Dipswitch input stack */
static int	dip_coin_FIFO[2];
static int	dip_coin_cur;
static int	dip_coin_close;
static int	dip_coin_open;

/* User installed handlers for player switches */
static void	(*p1234sw_func[32])(int, int);

/* User installed handlers for dip and coin switches */
static void	(*dcsw_func[32])(int, int);

struct coin_handler_info {
	int	ticks_2_recheck;
	int	current_count;
	void	(*func)(int, int);
};

static void null_sw_handler(int sig, int id)
{
}

void *set_psw_handler(int sw_id, void (*h_func)(int, int))
{
	void	*old_func;

	old_func = (void *)p1234sw_func[sw_id];
	p1234sw_func[sw_id] = h_func;
	return(old_func);
}

void *set_dcsw_handler(int sw_id, void (*h_func)(int, int))
{
	void	*old_func;

	old_func = (void *)dcsw_func[sw_id];
	dcsw_func[sw_id] = h_func;
	return(old_func);
}

void init_sw_handlers(void)
{
	int	i;

	for(i = 0; i < sizeof(p1234sw_func)/sizeof(void *); i++)
	{
		p1234sw_func[i] = null_sw_handler;
		dcsw_func[i] = null_sw_handler;
	}
}

void switch_init(void)
{
	int	low_16;
	int	high_16;
	int	p12xx;

#if defined(SEATTLE)
	_ioctl(4, FIOCGETPLAYER12, (int)&low_16);
	_ioctl(4, FIOCGETPLAYER34, (int)&high_16);
#elif defined(VEGAS)
	get_inputs(&input_data);
	high_16 = input_data.p34;
	low_16 = input_data.p12;
	coin_drops[0] =
		coin_drops[1] = 
		coin_drops[2] = 
		coin_drops[3] = 
		coin_drops[4] = 0;
#endif
	high_16 <<= 16;
	stuck_psw = p1234sw_FIFO[0] = p1234sw_FIFO[1] = p1234sw_cur = (high_16 | low_16);
#if defined(SEATTLE)
	_ioctl(4, FIOCGETCOININPUTS, (int)&low_16);
	_ioctl(4, FIOCGETDIPSWITCHES, (int)&high_16);
#elif defined(VEGAS)
	low_16 = input_data.coin;
	high_16 = input_data.dip;
#endif
	high_16 <<= 16;
	dip_coin_FIFO[0] = dip_coin_FIFO[1] = dip_coin_cur = (high_16 | low_16);

	if( __use_49way )
	{
		p5sw_FIFO[0] = p5sw_FIFO[1] = p5sw_cur = get_mplex_stick( 8, 9 );
		blitz_stick_decode( &stuck_psw, &p12xx );
//			stuck_psw = ~(
//					((int)xlat_49to8[(p12xx >> 8) & P1_SWITCH_MASK] << 8) |
//					((int)xlat_49to8[ p12xx       & P1_SWITCH_MASK]) |
//					((~p1234sw_cur) & (P1_ABCD_MASK | P2_ABCD_MASK)) |
//					(((~p1234sw_cur) & (P1_SWITCH_MASK | P2_SWITCH_MASK)) << 16)
//					);
//		}
	}

	init_sw_handlers();
}

static void do_player_signals(void)
{
	int	i;
	int	mask = 1;

	for(i = 0; i < 32; i++)
	{
		if(p1234sw_close & mask)
		{
			p1234sw_func[i](SWITCH_CLOSE, i);
		}
		if(p1234sw_open & mask)
		{
			p1234sw_func[i](SWITCH_OPEN, i);
		}
		mask <<= 1;
	}
}

static void do_dip_coin_signals(void)
{
	int	i;
	int	mask = 1;

	for(i = 0; i < 32; i++)
	{
#if defined(VEGAS)
		if(i == LEFT_COIN_SWID ||
			i == RIGHT_COIN_SWID ||
			i == CENTER_COIN_SWID ||
			i == EXTRA_COIN_SWID ||
			i == BILL_VALIDATOR_SWID)
		{
			mask <<= 1;
			continue;
		}
#endif
		if(dip_coin_close & mask)
		{
			dcsw_func[i](SWITCH_CLOSE, i);
		}
		if(dip_coin_open & mask)
		{
			dcsw_func[i](SWITCH_OPEN, i);
		}
		mask <<= 1;
	}
#if defined(VEGAS)
	{
		register int	swid = LEFT_COIN_SWID;

		for(i = 0; i < 5; i++)
		{
			if(coin_drops[i])
			{
				if(i == 0)
				{
					swid = LEFT_COIN_SWID;
				}
				else if(i == 1)
				{
					swid = RIGHT_COIN_SWID;
				}
				else if(i == 2)
				{
					swid = CENTER_COIN_SWID;
				}
				else if(i == 3)
				{
					swid = EXTRA_COIN_SWID;
				}
				else if(i == 4)
				{
					swid = BILL_VALIDATOR_SWID;
				}
				while(coin_drops[i] > 0)
				{
					dcsw_func[swid](SWITCH_CLOSE, swid);
					--coin_drops[i];
				}
			}
		}
	}
#endif
}

void scan_switches(void)
{
	int p1234, p12xx;
	int	hi, lo;

	/* Get coin and dipswitch inputs */
	dip_coin_FIFO[1] = dip_coin_FIFO[0];
	dip_coin_FIFO[0] = dip_coin_cur;
#if defined(SEATTLE)
	_ioctl(4, FIOCGETDIPSWITCHES, (int)&hi);
	_ioctl(4, FIOCGETCOININPUTS, (int)&lo);
#elif defined(VEGAS)
	get_inputs(&input_data);
	coin_drops[0] += input_data.on_transitions[SWID_LEFT_COIN];
	coin_drops[1] += input_data.on_transitions[SWID_RIGHT_COIN];
	coin_drops[2] += input_data.on_transitions[SWID_CENTER_COIN];
	coin_drops[3] += input_data.on_transitions[SWID_EXTRA_COIN];
	coin_drops[4] += input_data.on_transitions[SWID_DBV];
	hi = input_data.dip;
	lo = input_data.coin;
#endif
	hi <<= 16;
	dip_coin_cur = (hi | lo);

	/* Get player 1, 2, 3, and 4 inputs */
	p1234sw_FIFO[1] = p1234sw_FIFO[0];
	p1234sw_FIFO[0] = p1234sw_cur;
#if defined(SEATTLE)
	_ioctl(4, FIOCGETPLAYER34, (int)&hi);
	_ioctl(4, FIOCGETPLAYER12, (int)&lo);
#elif defined(VEGAS)
	hi = input_data.p34;
	lo = input_data.p12;
#endif
	hi <<= 16;
	p1234 = (hi | lo);

	if(__use_49way)
	{
		blitz_stick_decode( &p1234, &p12xx );
		p5sw_FIFO[1] = p5sw_FIFO[0];
		p5sw_FIFO[0] = p5sw_cur;
		p5sw_cur = get_mplex_stick( 8, 9 );
		p5sw_close = (p5sw_FIFO[0] & p5sw_FIFO[1]) & ~p5sw_cur;
		p5sw_open = ~(p5sw_FIFO[0] & p5sw_FIFO[1]) & p5sw_cur;
	}
	else
	{
		p12xx =	((int)xlat_8way[(p1234 & P2_RLDU_MASK) >> 8] << 8) |
				((int)xlat_8way[(p1234 & P1_RLDU_MASK)]);
	}

	p1234sw_cur = p1234 | ~stuck_psw;
	p12xxsw_cur = p12xx;

	/* Check for up and down transitions on player 1,2,3, and 4 inputs */
	/* Close transitions */
	p1234sw_close = (p1234sw_FIFO[0] & p1234sw_FIFO[1]) & ~p1234sw_cur;
	/* Open Transitions */
	p1234sw_open = ~(p1234sw_FIFO[0] | p1234sw_FIFO[1]) & p1234sw_cur;

	/* Check for up/down transitions on coin and dipswitch inputs */
	/* Close transitions */
	dip_coin_close = (dip_coin_FIFO[0] & dip_coin_FIFO[1]) & ~dip_coin_cur;
	/* Open Transitions */
	dip_coin_open = ~(dip_coin_FIFO[0] | dip_coin_FIFO[1]) & dip_coin_cur;

	/* Call any user installed signal handlers for the player inputs */
	do_player_signals();

	/* Do system level and user level functions for dip and coin inputs */
	do_dip_coin_signals();
}

// read inputs from multiplexer board first used in Blitz '98
// write selector nibble to aux out port, wait 200+ns, read
// selected data from p4 UDLR.  low_n and high_n are the
// bank select nibbles for the low and high halves of the
// byte we'll return.
int get_mplex_stick( int low_n, int high_n )
{
	int		lo,hi,r;

#if defined(VEGAS)
	_ioctl(4, FIOCSETAUXPORT, low_n);
#elif defined(SEATTLE)
	*((volatile int *)0xb6000038) = low_n;
#endif
	__burn_200ns();
	_ioctl(4, FIOCGETPLAYER34, (int)&lo);
	lo = (lo & 0xf00) >> 8;

#if defined(VEGAS)
	_ioctl(4, FIOCSETAUXPORT, high_n);
#elif defined(SEATTLE)
	*((volatile int *)0xb6000038) = high_n;
#endif
	__burn_200ns();
	_ioctl(4, FIOCGETPLAYER34, (int)&hi);
	hi = (hi & 0xf00) >> 4;
	
	r = lo|hi;

	return r;
}

// Blitz stick decoding
static void blitz_stick_decode( int *p1234_p, int *p12xx_p )
{
	int	s0,s1,s2,s3;
	union
	{
		dip_inputs_t	di;
		int				val;
	} dip_in;
	int		p1234, p12xx;

	p1234 = *p1234_p;
	p12xx = *p12xx_p;

	dip_in.val = dip_coin_cur;
	if( dip_in.di.reserved1 == DIP_STICK_TYPE_8_WAY )
	{
		// 49-way sticks
		if( I40_present )
		{
			// read from I40 board, 2 or 4-player
			s0 = xlat_49way[get_mplex_stick( 0, 1 )];
			s1 = xlat_49way[get_mplex_stick( 2, 3 )];
			s2 = xlat_49way[get_mplex_stick( 4, 5 )];
			s3 = xlat_49way[get_mplex_stick( 6, 7 )];

			p12xx = s0 | (s1<<8) | (s2<<16) | (s3<<24);

			s0 = xlat_49to8[s0];
			s1 = xlat_49to8[s1];
			s2 = xlat_49to8[s2];
			s3 = xlat_49to8[s3];

			// mask off any stick data
			p1234 = (~p1234) & 0xf0f0f0f0;
			p1234 |= (s0 | (s1<<8) | (s2<<16) | (s3<<24));
			p1234 = ~p1234;
		}
		else
		{
			if( dip_in.di.reserved2 == DIP_2PLAYER_PANEL )
			{
				// Problem.  Dip switches say we have 4 49-way
				// sticks, but there's no I40 board.  Since there's
				// no hope of getting useful data from the I40, just
				// act like there's eight-way sticks out there.
				p12xx =	((int)xlat_8way[(p1234 & P2_RLDU_MASK) >> 8] << 8) |
					   	((int)xlat_8way[(p1234 & P1_RLDU_MASK)] |
					   	((int)xlat_8way[(p1234 & P3_RLDU_MASK) >> 16] << 16) |
					   	((int)xlat_8way[(p1234 & P4_RLDU_MASK) >> 24] << 24));
			}
			else
			{
				// original blitz harness
				p12xx =	((int)xlat_49way[(p1234 >> 24) & P1_SWITCH_MASK] << 8) |
						((int)xlat_49way[(p1234 >> 16) & P1_SWITCH_MASK]);
				p1234 = ~(
						((int)xlat_49to8[(p12xx >> 8) & P1_SWITCH_MASK] << 8) |
						((int)xlat_49to8[ p12xx       & P1_SWITCH_MASK]) |
						((~p1234) & (P1_ABCD_MASK | P2_ABCD_MASK)) |
						(((~p1234) & (P1_SWITCH_MASK | P2_SWITCH_MASK)) << 16));
			}
		}
	}
	else
	{
		// 8-way sticks (2 or 4)
		p12xx = 	((int)xlat_8way[(p1234 & P2_RLDU_MASK) >> 8] << 8) |
					((int)xlat_8way[(p1234 & P1_RLDU_MASK)] |
					((int)xlat_8way[(p1234 & P3_RLDU_MASK) >> 16] << 16) |
					((int)xlat_8way[(p1234 & P4_RLDU_MASK) >> 24] << 24));
	}

	*p1234_p = p1234;
	*p12xx_p = p12xx;
}

int get_player_49way_current(void)
{
	return(p12xxsw_cur);
}

int get_player_sw_current(void)
{
	return(~p1234sw_cur);
}

int diag_get_player_sw_current(void)
{
	int	low_16;
	int	high_16;
	int	result, s0, s1, s2, s3;
	union
	{
		dip_inputs_t	di;
		int				val;
	} dip_in;

	dip_in.val = dip_coin_cur;

	_ioctl(4, FIOCGETPLAYER12, (int)&low_16);
	_ioctl(4, FIOCGETPLAYER34, (int)&high_16);

	result = ~((high_16 << 16) | low_16);
	if(__use_49way) {

		result &= 0xF0F0F0F0;

		// 49-way sticks
		if (I40_present) {

			s0 = xlat_49to8[p12xxsw_cur & 0xff];
			s1 = xlat_49to8[(p12xxsw_cur >> 8) & 0xff];
			s2 = xlat_49to8[(p12xxsw_cur >> 16) & 0xff];
			s3 = xlat_49to8[(p12xxsw_cur >> 24) & 0xff];
			result |= (s0 | (s1<<8) | (s2<<16) | (s3<<24));

		} else if (dip_in.di.reserved2 != DIP_2PLAYER_PANEL)
			
			// original blitz harness
			result |= (
						((int)xlat_49to8[(p12xxsw_cur >> 8) & P1_SWITCH_MASK] << 8) |
						((int)xlat_49to8[ p12xxsw_cur       & P1_SWITCH_MASK]) | 0 );
//						((~p1234) & (P1_ABCD_MASK | P2_ABCD_MASK)) |
//						(((~p1234) & (P1_SWITCH_MASK | P2_SWITCH_MASK)) << 16));


	}

	return(result);
}

int get_p5_sw_current(void)
{
	return(~p5sw_cur);
}

int get_dip_coin_current(void)
{
	return(~dip_coin_cur);
}

int diag_get_dip_coin_current(void)
{
	int	low_16;
	int	high_16;

	_ioctl(4, FIOCGETCOININPUTS, (int)&low_16);
	_ioctl(4, FIOCGETDIPSWITCHES, (int)&high_16);
	high_16 <<= 16;
	return(~(high_16|low_16));
}

int get_player_sw_close(void)
{
	return(p1234sw_close);
}

int get_p5_sw_close(void)
{
	return(p5sw_close);
}

int get_player_sw_open(void)
{
	return(p1234sw_open);
}

int get_p5_sw_open(void)
{
	return(p5sw_open);
}

int get_dip_coin_close(void)
{
	return(dip_coin_close);
}

int get_dip_coin_open(void)
{
	return(dip_coin_open);
}

int read_dip_direct(void)
{
	int	val;

	_ioctl(4, FIOCGETDIPSWITCHES, (int)&val);
	return(val);
}

int read_coin_direct(void)
{
	int	val;

	_ioctl(4, FIOCGETCOININPUTS, (int)&val);
	return(val);
}

int read_player12_direct(void)
{
	int	val;

	_ioctl(4, FIOCGETPLAYER12, (int)&val);
	return(val);
}

int read_player34_direct(void)
{
	int	val;

	_ioctl(4, FIOCGETPLAYER34, (int)&val);
	return(val);
}

int clear_stuck_psw( void )
{
	int		old;

	old = stuck_psw;
	stuck_psw = 0xffffffff;

	return old;
}

// 2 sticks (Original Blitz), mode = 1
// 4 sticks (Blitz '98), mode = 2
extern int i40;
void set_49way(int mode)
{
	int		val;

	__use_49way = mode;
	I40_present = 1;

	// check for I40 board
#if defined(VEGAS)
	_ioctl(4, FIOCSETAUXPORT, 0xa);
#elif defined(SEATTLE)
	*((volatile int *)0xb6000038) = 0xa;
#endif
	__burn_200ns();
	_ioctl(4, FIOCGETPLAYER34, (int)&val);
	if( (val & 0xF00) != 0xA00 )
		I40_present = 0;

#if defined(VEGAS)
	_ioctl(4, FIOCSETAUXPORT, 0xb);
#elif defined(SEATTLE)
	*((volatile int *)0xb6000038) = 0xb;
#endif
	__burn_200ns();
	_ioctl(4, FIOCGETPLAYER34, (int)&val);
	if( (val & 0xF00) != 0xB00 )
		I40_present = 0;

#if defined(VEGAS)
	_ioctl(4, FIOCSETAUXPORT, 0xc);
#elif defined(SEATTLE)
	*((volatile int *)0xb6000038) = 0xc;
#endif
	__burn_200ns();
	_ioctl(4, FIOCGETPLAYER34, (int)&val);
	if( (val & 0xF00) != 0xC00 )
		I40_present = 0;
}

// run off around 40 instructions, which is around 200ns on a 200MHz CPU
static void __burn_200ns( void )
{
asm("
		ori		$8,$0,13
__burn_top:
		addi	$8,$8,-1
		bne		$0,$8,__burn_top
		nop
		jr		$31
");
}
