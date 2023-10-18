//
// trans.c - Source for transitions
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<dir.h>
#include	<goose/goose.h>

char	goose_trans_c_version[] = {"$Revision: 4 $"};

extern float	hres, vres;

#ifndef VEGAS
typedef struct wms_header
{
	int				version;
	float				bias;
	GrMipMapMode_t	filter_mode;
	FxBool			tri_mode;
	Gu3dfHeader		header;
} wms_header_t;
#endif

volatile static transition_control_t	transition_control = {
NULL,					// Transition piece array pointer
NULL,					// Transition textrure buffer pointer
NULL,					// Transition image pointer
NULL,					// Transition texture node pointer
NULL,					// Transition texture name pointer
0,						// Transition pieces
NULL,					// Transition sound function
TRANSITION_ON,		// Transition direction
TRANSITION_DONE,	// Transition state
0,						// Transition stagger
0						// Transition stagger count
};

static char	texture_name[32] = {""};

static image_info_t	timg = {
texture_name,
0.0f,
0.0f,
0.0f,
0.0f,
0.0f,
1.0f,
0.0f,
1.0f
};

// Function prototypes for bar initialization functions
static void _rl_wipe_init(void);
static void _rl_slide_init(void);
static void _lr_wipe_init(void);
static void _lr_slide_init(void);
static void _h_interleave_init(void);
static void _htb_slide_init(void);
static void _hbt_slide_init(void);
static void _vrl_slide_init(void);
static void _vlr_slide_init(void);
static void _tb_wipe_init(void);
static void _tb_slide_init(void);
static void _bt_wipe_init(void);
static void _bt_slide_init(void);
static void _v_interleave_init(void);

// Bar initialization functions
static void	(*init_bars[])(void) =
{
_rl_wipe_init,
_rl_slide_init,
_lr_wipe_init,
_lr_slide_init,
_h_interleave_init,
_htb_slide_init,
_hbt_slide_init,
_vrl_slide_init,
_vlr_slide_init,
_tb_wipe_init,
_tb_slide_init,
_bt_wipe_init,
_bt_slide_init,
_v_interleave_init
};

// Pointer to texture header
static wms_header_t	*whdr;

void config_transition(char *tex, int bars)
{
	int				do_tex_load = 0;
	struct ffblk	ffblk;
	FILE				*fp;

	// Set the texture name
	strcpy(texture_name, tex);

	// Set the image pointer
	transition_control.image = &timg;

	// Set the transition sound function
	transition_control.snd = NULL;

	// Does a texture already exist ?
	if(!transition_control.tex_name)
	{
		// Nope - load the texture into memory
		do_tex_load = 1;
	}

	// Is the existing texture the same as the new texture ?
	else if(transition_control.tex_name != timg.texture_name)
	{
		// Nope - Free the old texture buffer (if it exists)
		if(transition_control.tbuffer)
		{
			free(transition_control.tbuffer);
		}

		// Load the texture into memory
		do_tex_load = 1;
	}

	if(do_tex_load)
	{
		// Does the texture file exist ?
		if(!findfirst(timg.texture_name, &ffblk, 0))
		{
			// YES - Allocate memory for it
			if((transition_control.tbuffer = (void *)malloc(ffblk.ff_fsize)) == NULL)
			{
				// ERROR - No memory
#ifdef DEBUG
				fprintf(stderr, "Can not allocate system memory for transition texture: %s\r\n", timg.texture_name);
				lockup();
#endif
				return;
			}

			// Open the texture file
			if((fp = fopen(timg.texture_name, "rb")) == (FILE *)0)
			{
				free(transition_control.tbuffer);
				transition_control.tbuffer = NULL;
#ifdef DEBUG
				fprintf(stderr, "Can not open transition texture file: %s\r\n", timg.texture_name);
				lockup();
#endif
				return;
			}

			// Read the texture file
			if(fread(transition_control.tbuffer, sizeof(char), ffblk.ff_fsize, fp) != ffblk.ff_fsize)
			{
				free(transition_control.tbuffer);
				transition_control.tbuffer = NULL;
#ifdef DEBUG
				fprintf(stderr, "Can not read transition texture file: %s\r\n", timg.texture_name);
				lockup();
#endif
				return;
			}

			// Close the texture file
			fclose(fp);

			// Figure out the size of the texture and set the width and height
			// fields of the timg structure.
			whdr = (wms_header_t *)transition_control.tbuffer;
			timg.w = whdr->header.width;
			timg.h = whdr->header.height;
		}
		else
		{
#ifdef DEBUG
			fprintf(stderr, "Can not find transition texture: %s\r\n", timg.texture_name);
			lockup();
#endif
			return;
		}

		// Set the texture name pointer
		transition_control.tex_name = timg.texture_name;
	}

	// Set the number of pieces for the transition
	transition_control.num_pieces = bars;

	// Is there alreay memory allocated for transition pieces
	if(transition_control.tpiece)
	{
		// YES - Free it
		free(transition_control.tpiece);
		transition_control.tpiece = NULL;
	}

	// Allocate memory for the transition pieces
	if((transition_control.tpiece = (transition_piece_t *)malloc(bars * sizeof(transition_piece_t))) == NULL)
	{
		// ERROR
	}
}

static void make_bars(int fn)
{
	int	bar_width;
	int	bar_height;
	int	i;

	if(fn < TRANSITION_FUNCTION_VRL_SLIDE)
	{
		bar_width = hres;
		bar_height = (int)(vres / (float)transition_control.num_pieces);
		while((bar_height * transition_control.num_pieces) < vres)
		{
			bar_height++;
		}
		timg.w = whdr->header.width;
		timg.h = bar_height;
	}
	else
	{
		bar_height = vres;
		bar_width = (int)(hres / (float)transition_control.num_pieces);
		while((bar_width * transition_control.num_pieces) < hres)
		{
			bar_width++;
		}
		timg.h = whdr->header.height;
		timg.w = bar_width;
	}


	// Set the width, height, and sound controls for each bar
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].w = bar_width;
		transition_control.tpiece[i].h = bar_height;
		transition_control.tpiece[i].make_sound = 1;

		// Set the oow, sow, and tow fields of all of the vertices
		transition_control.tpiece[i].vert[0].oow =
			transition_control.tpiece[i].vert[1].oow =
			transition_control.tpiece[i].vert[2].oow =
			transition_control.tpiece[i].vert[3].oow = 1.0f;
//#ifndef VEGAS
#if 1
		transition_control.tpiece[i].vert[0].sow =
			transition_control.tpiece[i].vert[1].sow =
			(timg.s_start * ((float)transition_control.tpiece[i].h / timg.h)) * 256.0f;

		transition_control.tpiece[i].vert[2].sow =
			transition_control.tpiece[i].vert[3].sow =
			(timg.s_end * ((float)transition_control.tpiece[i].h / timg.h)) * 256.0f;

		transition_control.tpiece[i].vert[0].tow = 
			transition_control.tpiece[i].vert[3].tow = 
			(timg.t_start * ((float)transition_control.tpiece[i].w / timg.w)) * 256.0f;

		transition_control.tpiece[i].vert[1].tow = 
			transition_control.tpiece[i].vert[2].tow = 
			(timg.t_end * ((float)transition_control.tpiece[i].w / timg.w)) * 256.0f;
#else
		transition_control.tpiece[i].vert[0].tmuvtx[0].sow =
			transition_control.tpiece[i].vert[1].tmuvtx[0].sow =
			transition_control.tpiece[i].vert[0].tmuvtx[1].sow =
			transition_control.tpiece[i].vert[1].tmuvtx[1].sow =
			(timg.s_start * ((float)transition_control.tpiece[i].h / timg.h)) * 256.0f;

		transition_control.tpiece[i].vert[2].tmuvtx[0].sow =
			transition_control.tpiece[i].vert[3].tmuvtx[0].sow =
			transition_control.tpiece[i].vert[2].tmuvtx[1].sow =
			transition_control.tpiece[i].vert[3].tmuvtx[1].sow =
			(timg.s_end * ((float)transition_control.tpiece[i].h / timg.h)) * 256.0f;

		transition_control.tpiece[i].vert[0].tmuvtx[0].tow = 
			transition_control.tpiece[i].vert[3].tmuvtx[0].tow = 
			transition_control.tpiece[i].vert[0].tmuvtx[1].tow = 
			transition_control.tpiece[i].vert[3].tmuvtx[1].tow = 
			(timg.t_start * ((float)transition_control.tpiece[i].w / timg.w)) * 256.0f;

		transition_control.tpiece[i].vert[1].tmuvtx[0].tow = 
			transition_control.tpiece[i].vert[2].tmuvtx[0].tow = 
			transition_control.tpiece[i].vert[1].tmuvtx[1].tow = 
			transition_control.tpiece[i].vert[2].tmuvtx[1].tow = 
			(timg.t_end * ((float)transition_control.tpiece[i].w / timg.w)) * 256.0f;
#endif
	}

	// Initialize the vertices, start, end, and current positions, and sound
	// enables for each bar based on the transition function type.
	init_bars[fn]();
}

void transition_on(int ticks, int fn, void (*on_sound)(void))
{
	register int	i;

	// Is function number valid ?
	if(fn < 0 || fn > TRANSITION_FUNCTION_V_INTERLEAVE)
	{
#ifdef DEBUG
		fprintf(stderr, "transition_on():  Invalid function number: %d\r\n", fn);
		lockup();
#endif
		return;
	}

	// Create the texture node for the bars
	transition_control.texture = create_texture_from_memory(transition_control.tex_name,
		transition_control.tbuffer,
		0,
		0,
		CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP,
		GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR,
		GR_TEXTUREFILTER_BILINEAR);

	// Did texture create OK
	if(!transition_control.texture)
	{
#ifdef DEBUG
		fprintf(stderr, "transition_on():  Could not create memory based texture\r\n");
		lockup();
#endif
		return;
	}

	// Set the transition sound function
	transition_control.snd = on_sound;

	// Set the transition direction
	transition_control.direction = TRANSITION_ON;

	// Set the stagger count
	transition_control.stagger_count = 1;

	// Set the starting, ending, current positions, velocities, and sizes of
	// each of the bars based on the function type
	make_bars(fn);

	// Set the velocities of the pieces
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].dx =
			(transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start) / ticks;
		if(transition_control.staggered)
		{
			transition_control.tpiece[i].dx *= transition_control.num_pieces;
		}
		if(!transition_control.tpiece[i].dx)
		{
			if((transition_control.tpiece[i].x_end -
				transition_control.tpiece[i].x_start) < 0)
			{
				transition_control.tpiece[i].dx = -1;
			}
			else
			{
				transition_control.tpiece[i].dx = 1;
			}
		}
		transition_control.tpiece[i].dy =
			(transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start) / ticks;
		if(transition_control.staggered)
		{
			transition_control.tpiece[i].dy *= transition_control.num_pieces;
		}
		if(!transition_control.tpiece[i].dy)
		{
			if((transition_control.tpiece[i].y_end -
				transition_control.tpiece[i].y_start) < 0)
			{
				transition_control.tpiece[i].dy = -1;
			}
			else
			{
				transition_control.tpiece[i].dy = 1;
			}
		}
	}

	// Set the transition state to start running
	transition_control.state = TRANSITION_START;

	// Wait for the transition to complete
	do
	{
		sleep(1);
	} while(transition_control.state != TRANSITION_DONE);

	// Disable drawing until transition on occurs
	draw_enable(0);
}

void transition_off(int ticks, void (*off_sound)(void))
{
	int	i;

	// Create the texture node for the bars
	transition_control.texture = create_texture_from_memory(transition_control.tex_name,
		transition_control.tbuffer,
		0,
		0,
		CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP,
		GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR,
		GR_TEXTUREFILTER_BILINEAR);

	// Did texture create OK
	if(!transition_control.texture)
	{
#ifdef DEBUG
		fprintf(stderr, "transition_on():  Could not create memory based texture\r\n");
		lockup();
#endif
		return;
	}

	// Set the transition sound function
	transition_control.snd = off_sound;

	// Set the transition direction
	transition_control.direction = TRANSITION_OFF;

	// Set the velocities of each of the bars based on each bars starting and
	// ending positions
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].dx =
			(transition_control.tpiece[i].x_start -
			transition_control.tpiece[i].x_end) / ticks;
		if(transition_control.staggered)
		{
			transition_control.tpiece[i].dx *= transition_control.num_pieces;
		}
		if(!transition_control.tpiece[i].dx)
		{
			if((transition_control.tpiece[i].x_start -
				transition_control.tpiece[i].x_end) < 0)
			{
				transition_control.tpiece[i].dx = -1;
			}
			else
			{
				transition_control.tpiece[i].dx = 1;
			}
		}
		transition_control.tpiece[i].dy =
			(transition_control.tpiece[i].y_start -
			transition_control.tpiece[i].y_end) / ticks;
		if(transition_control.staggered)
		{
			transition_control.tpiece[i].dy *= transition_control.num_pieces;
		}
		if(!transition_control.tpiece[i].dy)
		{
			if((transition_control.tpiece[i].y_start -
				transition_control.tpiece[i].y_end) < 0)
			{
				transition_control.tpiece[i].dy = -1;
			}
			else
			{
				transition_control.tpiece[i].dy = 1;
			}
		}

		// Is sound enabled for this piece ?
		if(transition_control.tpiece[i].snd_enable)
		{
			// Allow sound to be made
			transition_control.tpiece[i].make_sound = 1;
		}
	}

	// Set the transition state to start running
	transition_control.state = TRANSITION_START;

	// Enable drawing
	draw_enable(1);

	// Wait for the transition to complete
	do
	{
		sleep(1);
	} while(transition_control.state != TRANSITION_DONE);

	// Delete the texture
	delete_texture(transition_control.texture);
}


static void _draw_normal(void)
{
	register int						i;
	register transition_piece_t	*tp;
	register int						x_term;
	register int						y_term;
	register	int						num_at_end;
	register int						on_screen;
	register int						vert;
	register int						vel_status;

	tp = transition_control.tpiece;
	num_at_end = 0;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		// Start off screen
		on_screen = 0;

		// Do the velocity add for the vertices
		for(vert = 0; vert < 4; vert++)
		{
			tp->vert[vert].x += (float)tp->dx;
			tp->vert[vert].y += (float)tp->dy;
			if(tp->vert[vert].x >= 0.0f && tp->vert[vert].x < hres)
			{
				if(tp->vert[vert].y >= 0.0f && tp->vert[vert].y < vres)
				{
					on_screen = 1;
				}
			}
		}

		// Is the transition direction to ON screen ?
		if(transition_control.direction == TRANSITION_ON)
		{
			x_term = tp->x_end;
			y_term = tp->y_end;
		}
		else
		{
			x_term = tp->x_start;
			y_term = tp->y_start;
		}

		// YES - Has this piece reached it's ending x position ?
		vel_status = 0;
		if(tp->dx == 0)
		{
			vel_status |= 1;
		}
		if(tp->dy == 0)
		{
			vel_status |= 2;
		}
		if(tp->dx < 0)
		{
			if(tp->vert[0].x <= x_term)
			{
				// YES - Set it's x velocity to 0
				tp->dx = 0;

				// Snap it to it's end position
				tp->vert[0].x = tp->vert[3].x = x_term;
				tp->vert[1].x = tp->vert[2].x = tp->vert[0].x + tp->w;
			}
		}
		else if(tp->dx > 0)
		{
			if(tp->vert[0].x >= x_term)
			{
				// YES - Set it's x velocity to 0
				tp->dx = 0;

				// Snap it to it's end position
				tp->vert[0].x = tp->vert[3].x = x_term;
				tp->vert[1].x = tp->vert[2].x = tp->vert[0].x + tp->w;
			}
		}

		// YES - Has this piece reached it's ending x position ?
		if(tp->dy < 0)
		{
			if(tp->vert[0].y <= y_term)
			{
				// YES - Set it's y velocity to 0
				tp->dy = 0;
	
				// Snap it to it's end position
				tp->vert[0].y = tp->vert[1].y = y_term;
				tp->vert[2].y = tp->vert[3].y = tp->vert[0].y + tp->h;
			}
		}
		else if(tp->dy > 0)
		{
			if(tp->vert[0].y >= y_term)
			{
				// YES - Set it's y velocity to 0
				tp->dy = 0;
	
				// Snap it to it's end position
				tp->vert[0].y = tp->vert[1].y = y_term;
				tp->vert[2].y = tp->vert[3].y = tp->vert[0].y + tp->h;
			}
		}

		// Is the piece on screen at all
		if(on_screen)
		{
			if(tp->make_sound)
			{
				if(transition_control.snd)
				{
					transition_control.snd();
				}
				tp->make_sound = 0;
			}

#ifndef VEGAS
			// YES - Draw the 2 triangles
			grDrawTriangleDma(&tp->vert[0], &tp->vert[1], &tp->vert[2], 0);
			grDrawTriangleDma(&tp->vert[0], &tp->vert[2], &tp->vert[3], 0);
#else
			// YES - Draw the 2 triangles
			grDrawTriangle(&tp->vert[0], &tp->vert[1], &tp->vert[2]);
			grDrawTriangle(&tp->vert[0], &tp->vert[2], &tp->vert[3]);
#endif
		}

		// Is it at its final position ?
		if(vel_status == 3)
		{
			num_at_end++;
		}

		// Next piece
		tp++;
	}

	// Have all of the pieces finished moving ?
	if(num_at_end >= transition_control.num_pieces)
	{
		// YES - Transition is done
		transition_control.state = TRANSITION_DONE;
	}
}


static void _draw_staggered_on(void)
{
	register int						i;
	register transition_piece_t	*tp;
	register int						x_term;
	register int						y_term;
	register	int						num_at_end;
	register int						on_screen;
	register int						vert;
	register int						vel_status;

	num_at_end = 0;		

	tp = transition_control.tpiece;
	for(i = 0; i < transition_control.stagger_count; i++)
	{
		// Start off screen
		on_screen = 0;

		// Do the velocity add for the vertices
		for(vert = 0; vert < 4; vert++)
		{
			tp->vert[vert].x += (float)tp->dx;
			tp->vert[vert].y += (float)tp->dy;
			if(tp->vert[vert].x >= 0.0f && tp->vert[vert].x < hres)
			{
				if(tp->vert[vert].y >= 0.0f && tp->vert[vert].y < vres)
				{
					on_screen = 1;
				}
			}
		}

		x_term = tp->x_end;
		y_term = tp->y_end;

		// Has this piece reached it's ending x position ?
		vel_status = 0;
		if(tp->dx == 0)
		{
			vel_status |= 1;
		}
		if(tp->dy == 0)
		{
			vel_status |= 2;
		}
		if(tp->dx < 0)
		{
			if(tp->vert[0].x <= x_term)
			{
				// YES - Set it's x velocity to 0
				tp->dx = 0;

				// Snap it to it's end position
				tp->vert[0].x = tp->vert[3].x = x_term;
				tp->vert[1].x = tp->vert[2].x = tp->vert[0].x + tp->w;
			}
		}
		else if(tp->dx > 0)
		{
			if(tp->vert[0].x >= x_term)
			{
				// YES - Set it's x velocity to 0
				tp->dx = 0;

				// Snap it to it's end position
				tp->vert[0].x = tp->vert[3].x = x_term;
				tp->vert[1].x = tp->vert[2].x = tp->vert[0].x + tp->w;
			}
		}

		// YES - Has this piece reached it's ending x position ?
		if(tp->dy < 0)
		{
			if(tp->vert[0].y <= y_term)
			{
				// YES - Set it's y velocity to 0
				tp->dy = 0;
	
				// Snap it to it's end position
				tp->vert[0].y = tp->vert[1].y = y_term;
				tp->vert[2].y = tp->vert[3].y = tp->vert[0].y + tp->h;
			}
		}
		else if(tp->dy > 0)
		{
			if(tp->vert[0].y >= y_term)
			{
				// YES - Set it's y velocity to 0
				tp->dy = 0;
	
				// Snap it to it's end position
				tp->vert[0].y = tp->vert[1].y = y_term;
				tp->vert[2].y = tp->vert[3].y = tp->vert[0].y + tp->h;
			}
		}

		// Is the piece on screen at all
		if(on_screen)
		{
			if(tp->make_sound)
			{
				if(transition_control.snd)
				{
					transition_control.snd();
				}
				tp->make_sound = 0;
			}

#ifndef VEGAS
			// YES - Draw the 2 triangles
			grDrawTriangleDma(&tp->vert[0], &tp->vert[1], &tp->vert[2], 0);
			grDrawTriangleDma(&tp->vert[0], &tp->vert[2], &tp->vert[3], 0);
#else
			// YES - Draw the 2 triangles
			grDrawTriangle(&tp->vert[0], &tp->vert[1], &tp->vert[2]);
			grDrawTriangle(&tp->vert[0], &tp->vert[2], &tp->vert[3]);
#endif
		}

		// Is it at its final position ?
		if(vel_status == 3)
		{
			num_at_end++;
			if(num_at_end == transition_control.stagger_count)
			{
				transition_control.stagger_count++;
				break;
			}
		}

		// Next piece
		tp++;
	}

	// Have all of the pieces finished moving ?
	if(num_at_end >= transition_control.num_pieces)
	{
		// YES - Transition is done
		transition_control.state = TRANSITION_DONE;
	}
}

static void _draw_staggered_off(void)
{
	register int						i;
	register transition_piece_t	*tp;
	register int						x_term;
	register int						y_term;
	register	int						num_at_end;
	register int						on_screen;
	register int						vert;
	register int						vel_status;

	num_at_end = transition_control.num_pieces;

	tp = transition_control.tpiece;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		// Start off screen
		on_screen = 0;

		// Do the velocity add for the vertices
		if(i == (transition_control.stagger_count - 2))
		{
			for(vert = 0; vert < 4; vert++)
			{
				tp->vert[vert].x += (float)tp->dx;
				tp->vert[vert].y += (float)tp->dy;
				if(tp->vert[vert].x >= 0.0f && tp->vert[vert].x < hres)
				{
					if(tp->vert[vert].y >= 0.0f && tp->vert[vert].y < vres)
					{
						on_screen = 1;
					}
				}
			}
		}
		else
		{
			for(vert = 0; vert < 4; vert++)
			{
				if(tp->vert[vert].x >= 0.0f && tp->vert[vert].x < hres)
				{
					if(tp->vert[vert].y >= 0.0f && tp->vert[vert].y < vres)
					{
						on_screen = 1;
					}
				}
			}
		}

		x_term = tp->x_start;
		y_term = tp->y_start;

		// Has this piece reached it's ending x position ?
		vel_status = 0;
		if(tp->dx == 0)
		{
			vel_status |= 1;
		}
		if(tp->dy == 0)
		{
			vel_status |= 2;
		}
		if(tp->dx < 0)
		{
			if(tp->vert[0].x <= x_term)
			{
				// YES - Set it's x velocity to 0
				tp->dx = 0;

				// Snap it to it's end position
				tp->vert[0].x = tp->vert[3].x = x_term;
				tp->vert[1].x = tp->vert[2].x = tp->vert[0].x + tp->w;
			}
		}
		else if(tp->dx > 0)
		{
			if(tp->vert[0].x >= x_term)
			{
				// YES - Set it's x velocity to 0
				tp->dx = 0;

				// Snap it to it's end position
				tp->vert[0].x = tp->vert[3].x = x_term;
				tp->vert[1].x = tp->vert[2].x = tp->vert[0].x + tp->w;
			}
		}

		// YES - Has this piece reached it's ending x position ?
		if(tp->dy < 0)
		{
			if(tp->vert[0].y <= y_term)
			{
				// YES - Set it's y velocity to 0
				tp->dy = 0;
	
				// Snap it to it's end position
				tp->vert[0].y = tp->vert[1].y = y_term;
				tp->vert[2].y = tp->vert[3].y = tp->vert[0].y + tp->h;
			}
		}
		else if(tp->dy > 0)
		{
			if(tp->vert[0].y >= y_term)
			{
				// YES - Set it's y velocity to 0
				tp->dy = 0;
	
				// Snap it to it's end position
				tp->vert[0].y = tp->vert[1].y = y_term;
				tp->vert[2].y = tp->vert[3].y = tp->vert[0].y + tp->h;
			}
		}

		// Is the piece on screen at all
		if(on_screen)
		{
#ifndef VEGAS
			// YES - Draw the 2 triangles
			grDrawTriangleDma(&tp->vert[0], &tp->vert[1], &tp->vert[2], 0);
			grDrawTriangleDma(&tp->vert[0], &tp->vert[2], &tp->vert[3], 0);
#else
			// YES - Draw the 2 triangles
			grDrawTriangle(&tp->vert[0], &tp->vert[1], &tp->vert[2]);
			grDrawTriangle(&tp->vert[0], &tp->vert[2], &tp->vert[3]);
#endif
		}

		// Is it at its final position ?
		if(vel_status == 3)
		{
			num_at_end--;
			if(num_at_end == (transition_control.stagger_count - 2))
			{
				if(tp->make_sound)
				{
					if(transition_control.snd)
					{
						transition_control.snd();
					}
					tp->make_sound = 0;
				}
				transition_control.stagger_count--;
			}
		}

		// Next piece
		tp++;
	}

	// Have all of the pieces finished moving ?
	if(num_at_end <= 0)
	{
		// YES - Transition is done
		transition_control.state = TRANSITION_DONE;
	}
}


// Function used to draw the transitions
void draw_transition(void)
{
	if(transition_control.state == TRANSITION_START)
	{
		// Set the state of the rendering engine
		grDepthBufferFunction(GR_CMP_ALWAYS);
		grChromakeyMode(GR_CHROMAKEY_DISABLE);
		guTexSource(transition_control.texture->texture_handle);
		grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ONE, GR_BLEND_ZERO);

		if(!transition_control.staggered)
		{
			_draw_normal();
		}
		else if(transition_control.direction == TRANSITION_ON)
		{
			_draw_staggered_on();
		}
		else
		{
			_draw_staggered_off();
		}

		// Restore rendering engine state
		grDepthBufferFunction(GR_CMP_LESS);
	}
}

static void _rl_wipe_init(void)
{
	register	int	i;

	transition_control.staggered = 0;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			(int)hres;
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start =
			transition_control.tpiece[i].y_end =
			i * (int)timg.h;
		transition_control.tpiece[i].x_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Only do a sound on the first bar
		if(!i)
		{
			transition_control.tpiece[i].snd_enable = 1;
		}
		else
		{
			transition_control.tpiece[i].snd_enable = 1;
		}

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _rl_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			(int)hres;
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start =
			transition_control.tpiece[i].y_end =
			i * (int)timg.h;
		transition_control.tpiece[i].x_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Do sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _lr_wipe_init(void)
{
	register	int	i;

	transition_control.staggered = 0;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			(int)-hres;
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start =
			transition_control.tpiece[i].y_end =
			i * (int)timg.h;
		transition_control.tpiece[i].x_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Only do a sound on the first bar
		if(!i)
		{
			transition_control.tpiece[i].snd_enable = 1;
		}
		else
		{
			transition_control.tpiece[i].snd_enable = 1;
		}

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _lr_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			(int)-hres;
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start =
			transition_control.tpiece[i].y_end =
			i * (int)timg.h;
		transition_control.tpiece[i].x_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _h_interleave_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		if(!(i & 1))
		{
			transition_control.tpiece[i].x =
				transition_control.tpiece[i].x_start =
				(int)hres;
			transition_control.tpiece[i].y =
				transition_control.tpiece[i].y_start =
				transition_control.tpiece[i].y_end =
				i * (int)timg.h;
			transition_control.tpiece[i].x_end = 0;
	
			transition_control.tpiece[i].dx =
				transition_control.tpiece[i].x_end -
				transition_control.tpiece[i].x_start;
	
			transition_control.tpiece[i].dy =
				transition_control.tpiece[i].y_end -
				transition_control.tpiece[i].y_start;
		}
		else
		{
			transition_control.tpiece[i].x =
				transition_control.tpiece[i].x_start =
				(int)-hres;
			transition_control.tpiece[i].y =
				transition_control.tpiece[i].y_start =
				transition_control.tpiece[i].y_end =
				i * (int)timg.h;
			transition_control.tpiece[i].x_end = 0;
	
			transition_control.tpiece[i].dx =
				transition_control.tpiece[i].x_end -
				transition_control.tpiece[i].x_start;
	
			transition_control.tpiece[i].dy =
				transition_control.tpiece[i].y_end -
				transition_control.tpiece[i].y_start;
		}
	
		// Only do a sound on the first bar
		if(!i)
		{
			transition_control.tpiece[i].snd_enable = 1;
		}
		else
		{
			transition_control.tpiece[i].snd_enable = 1;
		}

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _htb_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_end = 
			transition_control.tpiece[i].x_start = 0;
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start = vres + (i * timg.h);
		transition_control.tpiece[i].y_end = (i * timg.h);

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _hbt_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_end = 
			transition_control.tpiece[i].x_start = 0;
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start = -((i + 1) * timg.h);
		transition_control.tpiece[i].y_end = vres - ((i + 1) * timg.h);

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _vrl_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start = hres + (i * timg.w);
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_end = 
			transition_control.tpiece[i].y_start = 0;
		transition_control.tpiece[i].x_end = (i * timg.w);

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _vlr_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start = -((i + 1) * timg.w);
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_end = 
			transition_control.tpiece[i].y_start = 0;
		transition_control.tpiece[i].x_end = hres - ((i + 1) * timg.w);

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _tb_wipe_init(void)
{
	register	int	i;

	transition_control.staggered = 0;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			transition_control.tpiece[i].x_end = (i * timg.w);
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start = vres;
		transition_control.tpiece[i].y_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Only do a sound on the first bar
		if(!i)
		{
			transition_control.tpiece[i].snd_enable = 1;
		}
		else
		{
			transition_control.tpiece[i].snd_enable = 1;
		}

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _tb_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			transition_control.tpiece[i].x_end = (i * timg.w);
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start = vres;
			transition_control.tpiece[i].y_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _bt_wipe_init(void)
{
	register	int	i;

	transition_control.staggered = 0;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			transition_control.tpiece[i].x_end = (i * timg.w);
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start = -vres;
		transition_control.tpiece[i].y_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Only do a sound on the first bar
		if(!i)
		{
			transition_control.tpiece[i].snd_enable = 1;
		}
		else
		{
			transition_control.tpiece[i].snd_enable = 1;
		}

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _bt_slide_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		transition_control.tpiece[i].x =
			transition_control.tpiece[i].x_start =
			transition_control.tpiece[i].x_end = (i * timg.w);
		transition_control.tpiece[i].y =
			transition_control.tpiece[i].y_start = -vres;
			transition_control.tpiece[i].y_end = 0;

		transition_control.tpiece[i].dx =
			transition_control.tpiece[i].x_end -
			transition_control.tpiece[i].x_start;

		transition_control.tpiece[i].dy =
			transition_control.tpiece[i].y_end -
			transition_control.tpiece[i].y_start;

		// Sound on each bar
		transition_control.tpiece[i].snd_enable = 1;

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}

static void _v_interleave_init(void)
{
	register	int	i;

	transition_control.staggered = 1;
	for(i = 0; i < transition_control.num_pieces; i++)
	{
		if(!(i & 1))
		{
			transition_control.tpiece[i].x =
				transition_control.tpiece[i].x_start =
				transition_control.tpiece[i].x_end = (i * timg.w);
			transition_control.tpiece[i].y =
				transition_control.tpiece[i].y_start = -vres;
			transition_control.tpiece[i].y_end = 0;

			transition_control.tpiece[i].dx =
				transition_control.tpiece[i].x_end -
				transition_control.tpiece[i].x_start;

			transition_control.tpiece[i].dy =
				transition_control.tpiece[i].y_end -
				transition_control.tpiece[i].y_start;
		}
		else
		{
			transition_control.tpiece[i].x =
				transition_control.tpiece[i].x_start =
				transition_control.tpiece[i].x_end = (i * timg.w);
			transition_control.tpiece[i].y =
				transition_control.tpiece[i].y_start = vres;
			transition_control.tpiece[i].y_end = 0;

			transition_control.tpiece[i].dx =
				transition_control.tpiece[i].x_end -
				transition_control.tpiece[i].x_start;

			transition_control.tpiece[i].dy =
				transition_control.tpiece[i].y_end -
				transition_control.tpiece[i].y_start;
		}

		// Only do a sound on the first bar
		if(!i)
		{
			transition_control.tpiece[i].snd_enable = 1;
		}
		else
		{
			transition_control.tpiece[i].snd_enable = 1;
		}

		// Initialize the x and y positions of the verticies
		transition_control.tpiece[i].vert[0].x =
			transition_control.tpiece[i].vert[3].x =
			transition_control.tpiece[i].x_start;
		transition_control.tpiece[i].vert[1].x =
			transition_control.tpiece[i].vert[2].x =
			transition_control.tpiece[i].vert[0].x + transition_control.tpiece[i].w;

		transition_control.tpiece[i].vert[0].y =
			transition_control.tpiece[i].vert[1].y =
			transition_control.tpiece[i].y_start;
		transition_control.tpiece[i].vert[2].y =
			transition_control.tpiece[i].vert[3].y =
			transition_control.tpiece[i].vert[0].y + transition_control.tpiece[i].h;
	}
}


