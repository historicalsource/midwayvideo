#include	<glide.h>
#include	<system.h>


void get_mem_size(void);

#if defined(TEST)

//#define	GDEBUG

extern int (*myputc)(char);

void qatest01(void);

extern unsigned char	font8x8[];

#define FONT_WIDTH  8
#define FONT_HEIGHT 8
#define FONT_LEADING	2
#define FONT_SZ     ((FONT_WIDTH*FONT_HEIGHT)/8)

static void writeChar8(int, int, unsigned long, unsigned long, int, unsigned char);
static void putPixel(int, int, unsigned long);
int gputc(char ch);
void set_bcolor(int col);
void set_fcolor(int col);
void exit(int level);

static int	fcolor = -1;
static int	bcolor = 0;
static int	chars_per_line;
static int	rows_per_screen;
int	screen_width = 512;
#ifndef CARN
int	screen_height = 384;
#else
int	screen_height = 256;
#endif
int	graphics_initialized = 0;


//#define	GDEBUG

// Static variables only used here
GrHwConfiguration hwconfig;

static int find_3dfx(void)
{
	int	slot;
	int	val;

#if (PHOENIX_SYS & VEGAS)
	for(slot = 0; slot < 6; slot++)
#else
	for(slot = 0; slot < 10; slot++)
#endif
	{
		val = get_pci_config_reg(slot, 0);
		if((val & 0xffff) == 0x121a)
		{
			return(1);
		}
	}
	return(0);
}

void graphics_init(void)
{
	int	status;

	graphics_initialized = 0;

	// Check to see if there are any 3dfx cards in the system
	if(!find_3dfx())
	{
		printf("Graphics Card NOT detected\n");
		return;
	}

	printf("Graphics Card Detected\n");

	// Initialize the library
#if defined(GDEBUG)
	printf("Calling grGlideInit()\n");
#endif
	grGlideInit();

	// Figure out what hardware is really out there
#if defined(GDEBUG)
	printf("Calling grSstQueryHardware()\n");
#endif
	if(!grSstQueryHardware(&hwconfig))
	{
		printf("\ngraphics_init(): Graphics Hardware initialization failure\n");
		return;
	}

	// Select graphics card 0
#if defined(GDEBUG)
	printf("Calling grSstSelect()\n");
#endif
	grSstSelect(0);

	// Open the graphics library and set resolution - NOTE:  The values passed
	// here are ignored because the grSstVideMode() call is used above.
#if defined(GDEBUG)
	printf("Calling grSstWinOpen()\n");
#endif
#if defined(NFL)
	status = grSstWinOpen(0, GR_RESOLUTION_512x384, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
#elif defined(NBA)
	status = grSstWinOpen(0, GR_RESOLUTION_512x384, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
#elif defined(SPACE)
	status = grSstWinOpen(0, GR_RESOLUTION_512x384, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
#elif defined(B99)
	status = *((volatile int *)IOASIC_DIP_SWITCHES);
	if(!(status & 0x0200))
	{
		status = grSstWinOpen(0, GR_RESOLUTION_512x256, GR_REFRESH_54Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
		screen_height = 256;
	}
	else
	{
		status = grSstWinOpen(0, GR_RESOLUTION_512x384, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
		screen_height = 384;
	}
#elif defined(CARN)
	status = grSstWinOpen(0, GR_RESOLUTION_512x256, GR_REFRESH_54Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
#elif defined(DEVELOPMENT)
	status = grSstWinOpen(0, GR_RESOLUTION_512x384, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1);
#else
#error Game is NOT defined
#endif

	// Did resolution setting fail ?
	if(!status)
	{
		printf("\ngraphics_init(): ERROR graphics subsystem initialization failure\n");
		return;
	}

	// Initialize the state of the rendering engine
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);

	chars_per_line = (screen_width - (FONT_WIDTH*2)) / FONT_WIDTH;
	rows_per_screen = (screen_height - ((FONT_HEIGHT + FONT_LEADING)*2)) / (FONT_HEIGHT + FONT_LEADING);

	myputc = gputc;

	graphics_initialized = 1;
}

void exit(int level)
{
	char	tmp = *((volatile char *)LED_ADDR);

	while(1)
	{
		*((volatile char *)LED_ADDR) = 0xff;
		delay_us(250000);
		*((volatile char *)LED_ADDR) = tmp;
		delay_us(250000);
	}		
}

char *getenv(char *str)
{
	return((void *)0);
}

int sscanf(void)
{
	return(-1);
}

int atoi(char *str)
{
	return(0);
}

int sprintf(char *buf, char *fmt)
{
	return(-1);
}

float pow(void)
{
	return(0.0f);
}

float exp(void)
{
	return(0.0f);
}

void *malloc(int amount)
{
	return((void *)0);
}

void vfprintf(void)
{
}

void fclose(void)
{
}

void fflush(void)
{
}

void strcat(void)
{
}

void *fopen(void)
{
	return((void *)0);
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 == *s2)
	{
		if(*s1 == 0)
		{
			return 0;
		}
		s1++;
		s2++;
	}
	return(*(unsigned const char *)s1 - *(unsigned const char *)(s2));
}

int fgetc(void)
{
	return(-1);
}

void __dj_ctype_toupper(void)
{
}

void strtok(void)
{
}

void __dj_ctype_tolower(void)
{
}


void fprintf(void)
{
}

int	__dj_stdout = 1;
int	__dj_stderr = 0;

float atof(char *str)
{
	return(0.0f);
}


// Draw a character using the 8x8 character font by plotting the individual
// pixels of each character. Pixels corresponding to character information
// are plotted in 'fColor', background pixels are plotted in 'bColor'.
//
// x, y   - Position to begin drawing string.
// fColor - Foreground pixel color.
// bColor - Background pixel color.
// ch     - Character to draw

static short	char_data[8*8];

static void writeChar8(int x, int y, unsigned long fColor, unsigned long bColor, int bg, unsigned char ch)
{
	unsigned char	byte;
	int				r, c;
	unsigned char	*font;
	int				i;

	if(!graphics_initialized)
	{
		return;
	}

	font = font8x8;

	for(r = 0; r < FONT_HEIGHT; r++)
	{
		byte = *(font + (ch * FONT_SZ) + r);

		for(c = 0; c < FONT_WIDTH; c++)
		{
			if(byte & 0x80)
			{
				putPixel(c, r, fColor);
			}
			else
			{
				putPixel(c, r, bColor);
			}
			byte <<= 1;
		}
	}

	grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
		x + FONT_WIDTH, y + FONT_HEIGHT,
		GR_LFB_SRC_FMT_565,
		8, 8,
		16,
		char_data);
}


// Light the 'pixel' at (x, y) in screen buffer.
// NOTE: _screenPtr must be set up prior to calling this routine.
static void putPixel(int x, int y, unsigned long pixel)
{
	char_data[(y*8)+x] = pixel;
}


static int row = 0;
static int col = 0;

int gputc(char ch)
{
	if(ch == '\n')
	{
		row++;
		if(row >= rows_per_screen)
		{
			row = 0;
		}
		col = 0;
	}
	else if(ch == '\r')
	{
		col = 0;
	}
	else if(ch == '\t')
	{
		col += 4;
		if(col >= chars_per_line)
		{
			col = 0;
			row++;
			if(row >= rows_per_screen)
			{
				row = 0;
			}
		}
	}
	else if(ch == '\f')
	{
		grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
		grBufferSwap(1);
		grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
		grBufferSwap(1);
		row = 0;
		col = 0;
	}
	else
	{
		writeChar8(col * FONT_WIDTH, row * (FONT_HEIGHT + FONT_LEADING), fcolor, bcolor, 1, ch);
		col++;
		if(col >= chars_per_line)
		{
			col = 0;
			row++;
			if(row >= rows_per_screen)
			{
				row = 0;
			}
		}
	}
}

void set_fcolor(int col)
{
	fcolor = col;
}

void set_bcolor(int col)
{
	bcolor = col;
}

void draw_hline(int x1, int y1, int x2, int y2)
{
	int	num_pix;
	int	i;

	if(!graphics_initialized)
	{
		return;
	}

	// Reject any non-horizontal lines
	if(y1 != y2)
	{
		return;
	}

	// Reject horizontal lines off top of screen
	if(y1 < 0)
	{
		return;
	}

	// Reject horizontal lines off bottom of screen
	else if(y1 >= screen_height)
	{
		return;
	}

	// Limit line to screen width
	if(x1 < 0)
	{
		x1 = 0;
	}
	else if(x1 >= screen_width)
	{
		x1 = screen_width - 1;
	}
	if(x2 < 0)
	{
		x2 = 0;
	}
	else if(x2 >= screen_width)
	{
		x2 = screen_width - 1;
	}

	// Reject any 0 length lines
	if(x1 == x2)
	{
		return;
	}

	// Left to right draw
	if(x1 < x2)
	{
		while(x1 != x2)
		{
			if(x1 & 1)
			{
				num_pix = 1;

				for(i = 0; i < num_pix; i++)
				{
					putPixel(i, 0, fcolor);
				}

				grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
					x1, y1,
					GR_LFB_SRC_FMT_565,
					num_pix, 1,
					16,
					char_data);
	
				x1 += num_pix;
			}

			if((x2 - x1) > 8)
			{
				num_pix = 8;
			}
			else
			{
				num_pix = x2 - x1;
			}
			for(i = 0; i < num_pix; i++)
			{
				putPixel(i, 0, fcolor);
			}

			grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
				x1, y1,
				GR_LFB_SRC_FMT_565,
				num_pix, 1,
				16,
				char_data);

			x1 += num_pix;
		}
	}

	// Right to left draw
	else
	{
		while(x2 != x1)
		{
			if(x2 & 1)
			{
				num_pix = 1;

				for(i = 0; i < num_pix; i++)
				{
					putPixel(i, 0, fcolor);
				}

				grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
					x2, y1,
					GR_LFB_SRC_FMT_565,
					num_pix, 1,
					16,
					char_data);
	
				x2 += num_pix;
			}

			if((x1 - x2) > 8)
			{
				num_pix = 8;
			}
			else
			{
				num_pix = x1 - x2;
			}
			for(i = 0; i < num_pix; i++)
			{
				putPixel(i, 0, fcolor);
			}

			grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
				x2, y1,
				GR_LFB_SRC_FMT_565,
				num_pix, 1,
				16,
				char_data);

			x2 += num_pix;
		}
	}
}

void draw_vline(int x1, int y1, int x2, int y2)
{
	int	num_pix;
	int	i;

	if(!graphics_initialized)
	{
		return;
	}

	// Reject any non-vertical lines
	if(x1 != x2)
	{
		return;
	}

	// Reject vertical lines off left side of screen
	if(x1 < 0)
	{
		return;
	}

	// Reject vertical lines off right side of screen
	else if(x1 >= screen_width)
	{
		return;
	}

	// Limit line to screen height
	if(y1 < 0)
	{
		y1 = 0;
	}
	else if(y1 >= screen_height)
	{
		y1 = screen_height - 1;
	}
	if(y2 < 0)
	{
		y2 = 0;
	}
	else if(y2 >= screen_height)
	{
		y2 = screen_height - 1;
	}

	// Reject any 0 length lines
	if(y1 == y2)
	{
		return;
	}

	// Top to bottom
	if(y1 < y2)
	{
		while(y1 != y2)
		{
			if((y2 - y1) > 8)
			{
				num_pix = 8;
			}
			else
			{
				num_pix = y2 - y1;
			}
			for(i = 0; i < num_pix; i++)
			{
				putPixel(i, 0, fcolor);
			}

			grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
				x1, y1,
				GR_LFB_SRC_FMT_565,
				1, num_pix,
				2,
				char_data);

			y1 += num_pix;
		}
	}

	// Bottom to top
	else
	{
		while(y2 != y1)
		{
			if((y1 - y2) > 8)
			{
				num_pix = 8;
			}
			else
			{
				num_pix = y1 - y2;
			}
			for(i = 0; i < num_pix; i++)
			{
				putPixel(i, 0, fcolor);
			}

			grLfbWriteRegion(GR_BUFFER_FRONTBUFFER,
				x2, y1 - num_pix,
				GR_LFB_SRC_FMT_565,
				1, num_pix,
				2,
				char_data);

			y1 -= num_pix;
		}
	}
}

void draw_polyline(int num, int *pnts)
{
	if(!num)
	{
		return;
	}
	num >>= 1;
	while(num--)
	{
		if(pnts[0] == pnts[2])
		{
			draw_vline(pnts[0], pnts[1], pnts[2], pnts[3]);
		}
		else if(pnts[1] == pnts[3])
		{
			draw_hline(pnts[0], pnts[1], pnts[2], pnts[3]);
		}
		pnts += 2;
	}
}

void draw_rect(tlx, tly, w, h)
{
	draw_hline(tlx, tly, tlx+w, tly);
	draw_hline(tlx, tly+h, tlx+w, tly+h);
	draw_vline(tlx, tly, tlx, tly+h);
	draw_vline(tlx+w, tly, tlx+w, tly+h);
}

void filled_rect(tlx, tly, w, h)
{
	while(h--)
	{
		draw_hline(tlx, tly, tlx+w, tly);
		tly++;
	}
}

#else
void graphics_init(void)
{
	get_mem_size();
}
void set_fcolor(int col)
{
}
void __show_processor_info(void)
{
}
void __show_pci_devices(void)
{
}
#endif
