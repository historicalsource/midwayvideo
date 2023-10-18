#if defined(GLIDE3)
#include	<stdio.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<glide.h>

#define	TO_BOLD			(1<<0)
#define	TO_UNDERSCORE	(1<<1)
#define	TO_BLINK			(1<<2)
#define	TO_REVERSE		(1<<3)
#define	TO_CONCEALED	(1<<4)

#define	FONT_WIDTH			8
#define	FONT_HEIGHT			8
#define	FONT_LEADING		2

#define	TO_RGB_BLACK 		0
#define	TO_RGB_LT_RED		(0x1f<<11)
#define	TO_RGB_LT_GREEN	(0x3f<<5)
#define	TO_RGB_LT_BLUE		(0x1f)
#define	TO_RGB_LT_YELLOW	(TO_RGB_LT_RED|TO_RGB_LT_GREEN)
#define	TO_RGB_LT_MAGENTA	(TO_RGB_LT_RED|TO_RGB_LT_BLUE)
#define	TO_RGB_LT_CYAN		(TO_RGB_LT_GREEN|TO_RGB_LT_BLUE)
#define	TO_RGB_LT_GRAY		((0x18<<11)|(0x30<<5)|(0x18))
#define	TO_RGB_WHITE		(TO_RGB_LT_RED|TO_RGB_LT_GREEN|TO_RGB_LT_BLUE)
#define	TO_RGB_DK_RED		(0xf<<11)
#define	TO_RGB_DK_GREEN	(0x1f<<5)
#define	TO_RGB_DK_BLUE		(0xf)
#define	TO_RGB_DK_YELLOW	(TO_RGB_DK_RED|TO_RGB_DK_GREEN)
#define	TO_RGB_DK_MAGENTA	(TO_RGB_DK_RED|TO_RGB_DK_BLUE)
#define	TO_RGB_DK_CYAN		(TO_RGB_DK_GREEN|TO_RGB_DK_BLUE)
#define	TO_RGB_DK_GRAY		(TO_RGB_DK_RED|TO_RGB_DK_GREEN|TO_RGB_DK_BLUE)

#define	TO_BLACK			0
#define	TO_LT_RED		1
#define	TO_LT_GREEN		2
#define	TO_LT_BLUE		3
#define	TO_LT_YELLOW	4
#define	TO_LT_MAGENTA	5
#define	TO_LT_CYAN		6
#define	TO_LT_GRAY		7
#define	TO_WHITE			8
#define	TO_DK_RED		9
#define	TO_DK_GREEN		10
#define	TO_DK_BLUE		11
#define	TO_DK_YELLOW	12
#define	TO_DK_MAGENTA	13
#define	TO_DK_CYAN		14
#define	TO_DK_GRAY		15

static
#include	"font8x8.c"

static void	to_putc_direct(char);
static void	to_putc_buffer(char);

extern float	hres, vres;

static unsigned int		*text_overlay_buffer;
static int					text_overlay_buffer_size = 0;
static int					text_overlay_buffer_dirty = 0;
static int					screen_width = 0;
static int					screen_height = 0;
static int					cur_column = 0;
static int					cur_row = 0;
static int					cur_fcolor = TO_WHITE;
static int					cur_bcolor = TO_BLACK;
static int					cur_attr = 0;
static int					save_row = 0;
static int					save_column = 0;
static void					(*to_putc)(char) = to_putc_direct;

static unsigned short	colors[16] = {
TO_RGB_BLACK,
TO_RGB_LT_RED,
TO_RGB_LT_GREEN,
TO_RGB_LT_YELLOW,
TO_RGB_LT_BLUE,
TO_RGB_LT_MAGENTA,
TO_RGB_LT_CYAN,
TO_RGB_WHITE,
TO_RGB_LT_GRAY,
TO_RGB_DK_RED,
TO_RGB_DK_GREEN,
TO_RGB_DK_BLUE,
TO_RGB_DK_YELLOW,
TO_RGB_DK_MAGENTA,
TO_RGB_DK_CYAN,
TO_RGB_DK_GRAY
};

extern float	hres, vres;

void init_text_overlay(int);
static void clear_text_buffer(void);
static void draw_char(int, int, unsigned char, unsigned short, unsigned short);
static void clr_eol(void);


void init_text_overlay(int mode)
{
	cur_row = 0;
	cur_column = 0;
	cur_fcolor = TO_WHITE;
	cur_bcolor = TO_BLACK;
	cur_attr = 0;
	if(!mode)
	{
		to_putc = to_putc_direct;
		screen_width = (int)hres / FONT_WIDTH;
		screen_height = (int)vres / (FONT_HEIGHT + FONT_LEADING);
		if(text_overlay_buffer)
		{
			free(text_overlay_buffer);
			text_overlay_buffer_size = 0;
			text_overlay_buffer = NULL;
		}
		return;
	}
	if(text_overlay_buffer)
	{
		return;
	}
hres = 512.0f;
vres = 384.0f;
	screen_width = (int)hres / FONT_WIDTH;
	screen_height = (int)vres / FONT_HEIGHT;
	text_overlay_buffer_size = sizeof(int) * screen_width * screen_height;
	text_overlay_buffer = (unsigned int *)malloc(text_overlay_buffer_size);
	if(!text_overlay_buffer_size)
	{
		return;
	}
	screen_height = (int)vres / (FONT_HEIGHT + FONT_LEADING);
	to_putc = to_putc_buffer;
}

static void clear_text_buffer(void)
{
	memset(text_overlay_buffer, 0, text_overlay_buffer_size);
	text_overlay_buffer_dirty = 0;
}

static void clear_screen(void)
{
	if(text_overlay_buffer)
	{
		clear_text_buffer();
	}
	else
	{
		grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
		grBufferSwap(1);
	}
	cur_row = 0;
	cur_column = 0;
}

static void clr_eol(void)
{
	if(text_overlay_buffer)
	{
		memset(&text_overlay_buffer[(cur_row * screen_width) + cur_column - 1], 0, ((screen_width - 1) - cur_column) * sizeof(int));
	}
	else
	{
	}
}

void draw_text_overlay(void)
{
	unsigned int	*data = text_overlay_buffer;
	unsigned short	fcolor;
	unsigned short	bcolor;
	unsigned int	row;
	unsigned int	column;

	if(!data || !text_overlay_buffer_dirty)
	{
		return;
	}
	for(row = 0; row < screen_height; row++)
	{
		for(column = 0; column < screen_width; column++)
		{
			if(*data)
			{
				fcolor = (*data >> 8) & 0xf;
				bcolor = (*data >> 12) & 0xf;
				fcolor = colors[fcolor];
				bcolor = colors[bcolor];
				draw_char(row*(FONT_HEIGHT+FONT_LEADING), column*FONT_WIDTH, *data & 0xff, fcolor, bcolor);
			}
			++data;
		}
	}
}

static short	pix_buffer[FONT_WIDTH*FONT_HEIGHT];

static void draw_char(int row, int column, unsigned char val, unsigned short fcolor, unsigned short bcolor)
{
	unsigned char	byte;
	int				r, c;

	grLfbReadRegion(GR_BUFFER_BACKBUFFER,
		column + FONT_WIDTH, row + FONT_HEIGHT,
		FONT_WIDTH, FONT_HEIGHT,
		16,
		pix_buffer);

	for(r = 0; r < FONT_HEIGHT; r++)
	{
		byte = font8x8[(val * FONT_WIDTH) + r];

		for(c = 0; c < FONT_WIDTH; c++)
		{
			if(byte & 0x80)
			{
				pix_buffer[(r * FONT_WIDTH) + c] = fcolor;
			}
			else
			{
//				pix_buffer[(r * FONT_WIDTH) + c] = bcolor;
			}
			byte <<= 1;
		}
	}

	grLfbWriteRegion(GR_BUFFER_BACKBUFFER,
		column + FONT_WIDTH, row + FONT_HEIGHT,
		GR_LFB_SRC_FMT_565,
		FONT_WIDTH, FONT_HEIGHT,
#if defined(GLIDE3)
		FXFALSE,
#endif
		16,
		pix_buffer);
}


static int	val[16];

static int get_vals(char *str)
{
	char	conv_buffer[8];
	int	i;
	int	num_vals = 0;

	while(*str)
	{
		i = 0;
		while(*str &&
			*str >= '0' &&
			*str <= '9' &&
			i < sizeof(conv_buffer))
		{
			conv_buffer[i++] = *str++;
		}
		if(!i || i > (sizeof(conv_buffer) - 1))
		{
			return(0);
		}
		conv_buffer[i] = 0;
		val[num_vals++] = atoi(conv_buffer);
		if(num_vals > sizeof(val)/sizeof(int))
		{
			return(0);
		}
		if(*str == ';')
		{
			++str;
		}
	}
	return(num_vals);
}

static char *decode_esc(char *str)
{
	char	conv_buffer[32];
	char	*tmp = str;
	int	i;
	int	num_vals;
	int	old_attr;
	int	old_fcolor;
	int	old_bcolor;
	int	error = 0;

	old_attr = cur_attr;
	old_fcolor = cur_fcolor;
	old_bcolor = cur_bcolor;
#ifndef TEST
	if(*tmp == '')
#else
	if(*tmp == 'e')
#endif
	{
		tmp++;
		if(*tmp == '[')
		{
			tmp++;
			i = 0;
			while(*tmp &&
				*tmp != 'H' &&
				*tmp != 'f' &&
				*tmp != 'A' &&
				*tmp != 'B' &&
				*tmp != 'C' &&
				*tmp != 'D' &&
				*tmp != 's' &&
				*tmp != 'u' &&
				*tmp != 'J' &&
				*tmp != 'K' &&
				*tmp != 'm' &&
				i < sizeof(conv_buffer))
			{
					conv_buffer[i++] = *tmp++;
			}
			conv_buffer[i] = 0;
			num_vals = get_vals(conv_buffer);
			switch(*tmp)
			{
				case 'H':				// Set cursor position [PL;PcH (none = 0, 0)
				case 'f':
				{
					if(!num_vals)
					{
						cur_row = 0;
						cur_column = 0;
						str = tmp + 1;
					}
					else if(num_vals <= 2)
					{
						cur_row = val[0];
						if(cur_row < 0)
						{
							cur_row = 0;
						}
						else if(cur_row >= screen_height)
						{
							cur_row = screen_height - 1;
						}
						if(num_vals == 2)
						{
							cur_column = val[1];
							if(cur_column < 0)
							{
								cur_column = 0;
							}
							else if(cur_column >= screen_width)
							{
								cur_column = screen_width - 1;
							}
						}
						str = tmp + 1;
					}
					break;
				}
				case 'A':				// Cursor up [PnA (up Pn lines)
				{
					if(val[0] > 0 && num_vals == 1)
					{
						cur_row -= val[0];
						if(cur_row < 0)
						{
							cur_row = 0;
						}
						str = tmp + 1;
					}
					break;
				}
				case 'B':				// Cursor down [PnB (down Pn lines)
				{
					if(val[0] > 0 && num_vals == 1)
					{
						cur_row += val[0];
						if(cur_row >= screen_height)
						{
							cur_row = screen_height - 1;
						}
						str = tmp + 1;
					}
					break;
				}
				case 'C':				// Cursor forward [PnC (right Pn columns)
				{
					if(val[0] > 0 && num_vals == 1)
					{
						cur_column += val[0];
						if(cur_column >= screen_width)
						{
							cur_column = screen_width - 1;
						}
						str = tmp + 1;
					}
					break;
				}
				case 'D':				// Cursor backward [PnD (left Pn columns)
				{
					if(val[0] > 0 && num_vals == 1)
					{
						cur_column -= val[0];
						if(cur_column < 0)
						{
							cur_column = 0;
						}
						str = tmp + 1;
					}
					break;
				}
				case 's':				// Save cursor position [s
				{
					if(i == 0)
					{
						save_row = cur_row;
						save_column = cur_column;
						str = tmp + 1;
					}
					break;
				}
				case 'u':				// Restore cursor position [u
				{
					if(i == 0)
					{
						cur_row = save_row;
						cur_column = save_column;
						str = tmp + 1;
					}
					break;
				}
				case 'J':				// Erase display 2J
				{
					if(conv_buffer[0] == '2')
					{
						clear_screen();
						str = tmp + 1;
					}
					break;
				}
				case 'K':				// Clear to end of line [K
				{
					if(i == 0)
					{
						clr_eol();
						str = tmp + 1;
					}
					break;
				}
				case 'm':				// Graphics mode [Ps;...;Psm (set modes)
				{
					while(num_vals)
					{
						--num_vals;
						switch(val[num_vals])
						{
							case 0:		// Attributes off
							{
								cur_attr = 0;
								break;
							}
							case 1:		// Bold on
							{
								cur_attr |= TO_BOLD;
								break;
							}
							case 4:		// Underscore
							{
								cur_attr |= TO_UNDERSCORE;
								break;
							}
							case 5:		// Blink on
							{
								cur_attr |= TO_BLINK;
								break;
							}
							case 7:		// Reverse video on
							{
								cur_attr |= TO_REVERSE;
								break;
							}
							case 8:		// Concealed on
							{
								cur_attr |= TO_CONCEALED;
								break;
							}
							case 30:		// Foreground BLACK
							case 31:		// Foreground RED
							case 32:		// Foreground GREEN
							case 33:		// Foreground YELLOW
							case 34:		// Foreground BLUE
							case 35:		// Foreground MAGENTA
							case 36:		// Foreground CYAN
							case 37:		// Foreground WHITE
							{
								cur_fcolor = val[num_vals] - 30;
								break;
							}
							case 40:		// Background BLACK
							case 41:		// Background RED
							case 42:		// Background GREEN
							case 43:		// Background YELLOW
							case 44:		// Background BLUE
							case 45:		// Background MAGENTA
							case 46:		// Background CYAN
							case 47:		// Background WHITE
							{
								cur_bcolor = val[num_vals] - 40;
								break;
							}
							default:
							{
								error = 1;
								break;
							}
						}
						if(error)
						{
							cur_attr = old_attr;
							cur_fcolor = old_fcolor;
							cur_bcolor = old_bcolor;
							break;
						}
					}
					if(!error)
					{
						str = tmp + 1;
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
	return(str);
}

static void to_putc_buffer(char ch)
{
	text_overlay_buffer[(cur_row * screen_width) + cur_column] = (cur_bcolor<<12)|(cur_fcolor<<8)|ch;
	text_overlay_buffer_dirty = 1;
	++cur_column;
	if(cur_column > (screen_width - 1))
	{
		cur_column = screen_width - 1;
	}
}

static void to_putc_direct(char ch)
{
	unsigned char	byte;
	int				r, c;

	for(r = 0; r < FONT_HEIGHT; r++)
	{
		byte = font8x8[(ch * FONT_WIDTH) + r];

		for(c = 0; c < FONT_WIDTH; c++)
		{
			if(byte & 0x80)
			{
				pix_buffer[(r * FONT_WIDTH) + c] = colors[cur_fcolor];
			}
			else
			{
				pix_buffer[(r * FONT_WIDTH) + c] = colors[cur_bcolor];
			}
			byte <<= 1;
		}
	}

	grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
		(cur_column * FONT_WIDTH),
		(cur_row * (FONT_HEIGHT + FONT_LEADING)),
		GR_LFB_SRC_FMT_565,
		FONT_WIDTH, FONT_HEIGHT,
#if defined(GLIDE3)
		FXFALSE,
#endif
		16,
		pix_buffer);

	++cur_column;
	if(cur_column > (screen_width - 1))
	{
		cur_column = screen_width - 1;
	}
}

static void to_puts(char *str)
{
	while(*str)
	{
		switch(*str)
		{
			case '\n':			// Newline
			{
				cur_row++;
				if(cur_row > screen_height - 1)
				{
					cur_row = screen_height - 1;
				}
				cur_column = 0;
				clr_eol();
				break;
			}
			case '\r':			// Return
			{
				cur_column = 0;
				break;
			}
			case '\t':			// Tab
			{
				cur_column += 4;
				if(cur_column > screen_width - 1)
				{
					cur_column = screen_width - 1;
				}
				break;
			}
			case '\b':			// Backspace
			{
				cur_column--;
				if(cur_column < 0)
				{
					cur_column = 0;
				}
				break;
			}
			case '\f':			// Formfeed
			{
				clear_screen();
				break;
			}
			default:
			{
				to_putc(*str);
				break;
			}
		}
		++str;
	}
}

static char	line_buffer[32][128];
static char	str_buffer[32*128];

void to_printf(char *fmt, ...)
{
	int	i;
	char	*tmp;
	char	*tmp1;
	int	num_strings = 0;

	vsprintf(str_buffer, fmt, (&fmt)+1);
	tmp = str_buffer;
	while(*tmp)
	{
		i = 0;
		if(*tmp != '')
		{
			while(*tmp && *tmp != '' && i < (sizeof(line_buffer[0]) - 1))
			{
				line_buffer[num_strings][i++] = *tmp++;
			}
			line_buffer[num_strings][i] = 0;
		}
		else
		{
			while(*tmp &&
				*tmp != 'H' &&
				*tmp != 'f' &&
				*tmp != 'A' &&
				*tmp != 'B' &&
				*tmp != 'C' &&
				*tmp != 'D' &&
				*tmp != 's' &&
				*tmp != 'u' &&
				*tmp != 'J' &&
				*tmp != 'K' &&
				*tmp != 'm' &&
				i < sizeof(line_buffer[0]) - 1)
			{
				line_buffer[num_strings][i++] = *tmp++;
			}
			line_buffer[num_strings][i++] = *tmp++;
			line_buffer[num_strings][i] = 0;
		}
		num_strings++;
	}

	for(i = 0; i < num_strings; i++)
	{
		if(line_buffer[i][0] == '')
		{
			tmp1 = decode_esc(line_buffer[i]);
			if(tmp1 == line_buffer[i])
			{
				to_puts(line_buffer[i]);
			}
		}
		else
		{
			to_puts(line_buffer[i]);
		}
	}
}


int get_to_rows(void)
{
	return(screen_height);
}

int get_to_columns(void)
{
	return(screen_width);
}
#endif
