/****************************************************************************/
/*                                                                          */
/* sprites.c - Sprite handling code for the phoenix system                  */
/*                                                                          */
/* Copyright (c) 1996 by Midway Video Inc.                                  */
/* All rights reserved                                                      */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* $Revision: 32 $                                                           */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<ioctl.h>
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<string.h>
#include	<glide.h>
#endif
#include	<goose/goose.h>

#ifdef VEGAS
#define GR_ASPECT_8x1	0
#define GR_ASPECT_4x1	1
#define GR_ASPECT_2x1	2
#define GR_ASPECT_1x1	3
#define GR_ASPECT_1x2	4
#define GR_ASPECT_1x4	5
#define GR_ASPECT_1x8	6

#if defined(VOODOO2)
void _grChromaRangeExt(GrColor_t, GrColor_t, GrChromaRangeMode_t);
void _grChromaModeExt(GrChromakeyMode_t);
#endif
#endif

char	goose_sprites_c_version[] = {"$Revision: 32 $"};

/* Turn this on if you want to display the sprite drawing timing */
/* info on the debug output terminal */
#define	DISPLAY_TIMING_INFO						0

/* Number of sprite nodes to allocate at a time */
#define	ALLOC_NUM	1000

#ifndef VEGAS
void grDrawTriangleDma(MidVertex *, MidVertex *, MidVertex *, int);
void grDrawQuadDma(MidVertex *);
#endif

#if (DISPLAY_TIMING_INFO & 1)
static int	gen_time;
static int	generated;
#endif

static void notify_check(sprite_info_t *sprite);
void set_sprite_mode(sprite_info_t *sprite);
int unlink_snode(sprite_node_t **sn_list_ptr, sprite_node_t *snode);

/* List of non-alpha channeled sprites to draw */
sprite_node_t	*sprite_node_list = (sprite_node_t *)0;

/* List of alpha channeled sprites to draw */
sprite_node_t	*alpha_sprite_node_list = (sprite_node_t *)0;

/* List of free sprite nodes */
sprite_node_t	*free_sprite_node_list = (sprite_node_t *)0;
int				is_low_res = 0;
extern int		scale_font_size;
extern int		resource_mode;

#ifdef VEGAS
static int				first_sprite = 0;
static state_info_t	last_sprite_state;
#endif

extern struct process_node	*cur_proc;

/* Transform matricies */
static float	x_mat[16];		/* X rotation matrix */
static float	y_mat[16];		/* Y rotation matrix */
static float	z_mat[16];		/* Z rotation matrix */
static float	c_mat[16];		/* Composite transform matrix */
static float	t_mat[16];		/* Translation matrix */

#ifndef VEGAS
/*
** memset() - This function sets "size" bytes of memory pointed to by "p"
** to the value specified by "data".
*/
static void memset(void *p, int data, int size)
{
	char	*b;

	b = (char *)p;
	while(size--)
	{
		*b++ = data;
	}
}
#endif

/*
** mxm() - This function performs a cross multiply on two 4x4 matrices
** (m1 and m2) into the the resultant matrix (r).
*/
static void mxm(register float *r, register float *m1, register float *m2)
{
	register int	i;

	i = 12;
	while(i)
	{
		i -= 4;
		r[i]   = m1[i]*m2[0] + m1[i+1]*m2[4] + m1[i+2]*m2[8];
		r[i+1] = m1[i]*m2[1] + m1[i+1]*m2[5] + m1[i+2]*m2[9];
		r[i+2] = m1[i]*m2[2] + m1[i+1]*m2[6] + m1[i+2]*m2[10];
		r[i+3] = m1[i]*m2[3] + m1[i+1]*m2[7] + m1[i+2]*m2[11];
	}
}

/*
** generate_sprite_verts() - This function does the calculations needed to
** generate the vertex information for a sprite.  This function MUST be called
** whenever the user moves a sprite.  It need not be called if automatic
** velocity adding is enabled for the sprite or the change_img function is
** called.
*/
static void _generate_sprite_verts(register sprite_info_t *sprite)
{
	register float			tx, ty;
#if defined(SEATTLE)
	register MidVertex	*vert = sprite->verts;
#elif defined(VEGAS)
	register SpriteVertex	*vert = sprite->verts;
#else
#error Environment variable TARGET_SYS not set
#endif

	// Is the Z position of the sprite < 1.0
	if(sprite->z < 1.0f)
	{
		// Set the sprites mode to hidded
		((sprite_node_t *)sprite->node_ptr)->mode |= HIDE_SPRITE;

		// Return because there is no point in calculating
		// this stuff for a hidden sprite
		return;
	}

	// Is sprite rotated ?
// This should work but doesn't - I don't know why - so I replaced it with
// the line below it
//	if(sprite->x_angle == sprite->y_angle == sprite->z_angle == 0.0f)
	if(sprite->x_angle == 0.0f && sprite->y_angle == 0.0f && sprite->z_angle == 0.0f)
	{
		register float			sw;
		register float			sh;
		register	float			oow;

		// Reset rotated mode bit
		sprite->mode &= ~ROTATED;

		// Calculate the OOW value for the sprite
		oow = 1.0F/sprite->z;

		// Calculate left X
		tx = sprite->x - (sprite->ii->ax * sprite->w_scale);

		// Calculate bottom Y
		ty = sprite->y + (sprite->ii->ay * sprite->h_scale);

		// Calculate right X
		sw = tx + (sprite->ii->w * sprite->w_scale);

		// Calculate top Y
		sh = ty + (sprite->ii->h * sprite->h_scale);

		// Set low res y position scaling factor
		if(is_low_res)
		{
			ty *= 0.66666666f;
			sh *= 0.66666666f;
		}

		// Snap vertices to .25 pixels
		tx += (1<<21);
		ty += (1<<21);
		sw += (1<<21);
		sh += (1<<21);

		tx -= (1<<21);
		ty -= (1<<21);
		sw -= (1<<21);
		sh -= (1<<21);

		// Set bottom left vertex
		vert[0].x = tx;
		vert[0].y = ty;

		// Set bottom right vertex
		vert[1].x = sw;
		vert[1].y = ty;

		// Set top right vertex
		vert[2].x = sw;
		vert[2].y = sh;

		// Set top left vertex
		vert[3].x = tx;
		vert[3].y = sh;
//#ifndef VEGAS
#if 1
		vert[0].oow =
			vert[1].oow =
			vert[2].oow =
			vert[3].oow =
			vert[0].tow =
			vert[1].tow =
			vert[2].tow =
			vert[3].tow =
			vert[0].sow =
			vert[1].sow =
			vert[2].sow =
			vert[3].sow = oow;
#else
		vert[0].oow =
			vert[1].oow =
			vert[2].oow =
			vert[3].oow =
			vert[0].tmuvtx[0].oow =
			vert[1].tmuvtx[0].oow =
			vert[2].tmuvtx[0].oow =
			vert[3].tmuvtx[0].oow =
			vert[0].tmuvtx[1].oow =
			vert[1].tmuvtx[1].oow =
			vert[2].tmuvtx[1].oow =
			vert[3].tmuvtx[1].oow =
			vert[0].tmuvtx[0].tow =
			vert[1].tmuvtx[0].tow =
			vert[2].tmuvtx[0].tow =
			vert[3].tmuvtx[0].tow =
			vert[0].tmuvtx[1].tow =
			vert[1].tmuvtx[1].tow =
			vert[2].tmuvtx[1].tow =
			vert[3].tmuvtx[1].tow =
			vert[0].tmuvtx[0].sow =
			vert[1].tmuvtx[0].sow =
			vert[2].tmuvtx[0].sow =
			vert[3].tmuvtx[0].sow =
			vert[0].tmuvtx[1].sow =
			vert[1].tmuvtx[1].sow =
			vert[2].tmuvtx[1].sow =
			vert[3].tmuvtx[1].sow = oow;
#endif
	}
	else
	{
		register int	i;
		register float	cos_a;
		register float	sin_a;
		register float	oow;
		float				x[4];
		float				y[4];

		// Normalize X axis rotation angle to <= 2PI && >= 2PI
		if(sprite->x_angle >= (2.0F * (float)M_PI))
		{
			sprite->x_angle -= (2.0F * (float)M_PI);
		}
		else if(sprite->x_angle <= (-2.0F * (float)M_PI))
		{
			sprite->x_angle += (2.0F * (float)M_PI);
		}

		// Normalize Y axis rotation angle to <= 2PI && >= 2PI
		if(sprite->y_angle >= (2.0F * (float)M_PI))
		{
			sprite->y_angle -= (2.0F * (float)M_PI);
		}
		else if(sprite->y_angle <= (-2.0F * (float)M_PI))
		{
			sprite->x_angle += (2.0F * (float)M_PI);
		}

		// Normalize Z axis rotation angle to <= 2PI && >= 2PI
		if(sprite->z_angle >= (2.0F * (float)M_PI))
		{
			sprite->z_angle -= (2.0F * (float)M_PI);
		}
		else if(sprite->z_angle <= (-2.0F * (float)M_PI))
		{
			sprite->z_angle += (2.0F * (float)M_PI);
		}

		// Set rotated mode bit
		sprite->mode |= ROTATED;

		// Now generate the vertices (scaled)
		x[1] = x[2] = sprite->w_scale * (float)(sprite->ii->w - sprite->ii->ax);
		x[0] = x[3] = x[1] - (sprite->w_scale * (float)sprite->ii->w);
	
		y[2] = y[3] = sprite->h_scale * (float)(sprite->ii->h + sprite->ii->ay);
		y[0] = y[1] = y[2] - (sprite->h_scale * (float)sprite->ii->h);

		/* Apply x, y, and z rotations to x0-x3, y0-y3, and z0-z3 */
		// X axis rotation matrix
		cos_a = fcos(-sprite->x_angle);
		sin_a = fsin(-sprite->x_angle);
		x_mat[5] = x_mat[10] = cos_a;
		x_mat[6] = sin_a;
		x_mat[9] = -sin_a;

		// Y axis rotation matrix
		cos_a = fcos(-sprite->y_angle);
		sin_a = fsin(-sprite->y_angle);
		y_mat[0] = y_mat[10] = cos_a;
		y_mat[8] = sin_a;
		y_mat[2] = -sin_a;

		// Z axis rotation matrix
		cos_a = fcos(-sprite->z_angle);
		sin_a = fsin(-sprite->z_angle);
		z_mat[0] = z_mat[5] = cos_a;
		z_mat[1] = sin_a;
		z_mat[4] = -sin_a;

		// Contatenate the rotation matrices
		mxm(c_mat, x_mat, y_mat);
		mxm(t_mat, c_mat, z_mat);

		// Apply the composite transform to the vertices
		for(i = 0; i < 4; i++)
		{
			// Calculate X
			tx = x[i]*t_mat[0] + y[i]*t_mat[4] + sprite->x;

			// Calculate Y
			ty = x[i]*t_mat[1] + y[i]*t_mat[5] + sprite->y;

			// Is it low res ?
			if(is_low_res)
			{
				// YES - scale the position
				ty *= 0.6666666f;
			}

			// Perspective projection
			if(sprite->mode & PER_PROJECT)
			{
				// Calculate OOW
				oow = 1.0F/(x[i]*t_mat[2] + y[i]*t_mat[6] + sprite->z);

				// Tell user this isn't supported yet
#if defined(DEBUG)
				fprintf(stderr, "Perspective Projection not yet supported for sprites\r\n");
				lockup();
#endif
				oow = 1.0F/sprite->z;
			}

			// Parallel projection
			else
			{
				// Calculate OOW
				oow = 1.0F/sprite->z;
			}

			// Snap x to .25
			tx += (float)(1<<21);
			tx -= (float)(1<<21);
	
			// Snap y to .25
			ty += (float)(1<<21);
			ty -= (float)(1<<21);
	
			// Set the vertex points
			vert[i].x = tx;
			vert[i].y = ty;
//#ifndef VEGAS
#if 1
			vert[i].oow = vert[i].tow = vert[i].sow = oow;
#else
			vert[i].oow = 
				vert[i].tmuvtx[0].oow = 
				vert[i].tmuvtx[0].tow = 
				vert[i].tmuvtx[0].sow = 
				vert[i].tmuvtx[1].oow = 
				vert[i].tmuvtx[1].tow = 
				vert[i].tmuvtx[1].sow = oow;
#endif
		}
	}

	// This is for fonts - if in low res mode we scale the position of the
	// character but NOT its size
	if(is_low_res == 3 && !scale_font_size)
	{
		vert[0].y = vert[1].y -= (float)((int)((sprite->ii->h - (vert[2].y - vert[0].y)) / 2.0f));
		vert[2].y = vert[3].y = vert[0].y + sprite->ii->h;
	}
}

void _generate_sprite_st(register sprite_info_t *sprite)
{
	register float				s_start, s_end;
	register float				t_start, t_end;
#if defined(SEATTLE)
	register MidVertex		*vert = sprite->verts;
#elif defined(VEGAS)
	register SpriteVertex		*vert = sprite->verts;
#else
#error Environment variable TARGET_SYS not set
#endif
	register image_info_t	*ii = sprite->ii;

	// Textured sprite
	if(sprite->tn)
	{
		// Check for tiling
		if(sprite->tile_x > 1.0F || sprite->tile_y > 1.0F)
		{
			sprite->state.texture_clamp_mode = GR_TEXTURECLAMP_WRAP;
		}
		else
		{
			sprite->state.texture_clamp_mode = GR_TEXTURECLAMP_CLAMP;
		}

		// Get starting and ending S and T values adjusted for tiling
		s_start = ii->s_start * sprite->tile_x;
		t_start = ii->t_start * sprite->tile_y;
		s_end = ii->s_end * sprite->tile_x;
		t_end = ii->t_end * sprite->tile_y;

		// Multiply out based on aspect ratio of texture
		switch(sprite->tn->texture_info.header.aspect_ratio)
		{
			case GR_ASPECT_8x1:
			{
				s_start *= 256.0F;
				t_start *= 32.0F;
				s_end *= 256.0F;
				t_end *= 32.0F;
				break;
			}
			case GR_ASPECT_4x1:
			{
				s_start *= 256.0F;
				t_start *= 64.0F;
				s_end *= 256.0F;
				t_end *= 64.0F;
				break;
			}
			case GR_ASPECT_2x1:
			{
				s_start *= 256.0F;
				t_start *= 128.0F;
				s_end *= 256.0F;
				t_end *= 128.0F;
				break;
			}
			case GR_ASPECT_1x1:
			{
				s_start *= 256.0F;
				t_start *= 256.0F;
				s_end *= 256.0F;
				t_end *= 256.0F;
				break;
			}
			case GR_ASPECT_1x2:
			{
				s_start *= 128.0F;
				t_start *= 256.0F;
				s_end *= 128.0F;
				t_end *= 256.0F;
				break;
			}
			case GR_ASPECT_1x4:
			{
				s_start *= 64.0F;
				t_start *= 256.0F;
				s_end *= 64.0F;
				t_end *= 256.0F;
				break;
			}
			case GR_ASPECT_1x8:
			{
				s_start *= 32.0F;
				t_start *= 256.0F;
				s_end *= 32.0F;
				t_end *= 256.0F;
				break;
			}
		}

		// Set T values if texture is flipped vertically
		if(!(sprite->mode & FLIP_TEX_V))
		{
//#ifndef VEGAS
#if 1
			vert[0].tow = vert[0].oow * t_end;
			vert[1].tow = vert[1].oow * t_end;
			vert[2].tow = vert[2].oow * t_start;
			vert[3].tow = vert[3].oow * t_start;
#else
			vert[0].tmuvtx[0].tow = vert[0].oow * t_end;
			vert[1].tmuvtx[0].tow = vert[1].oow * t_end;
			vert[2].tmuvtx[0].tow = vert[2].oow * t_start;
			vert[3].tmuvtx[0].tow = vert[3].oow * t_start;
#endif
		}

		// Set T values for texture that is NOT vertically flipped
		else
		{
//#ifndef VEGAS
#if 1
			vert[0].tow = vert[0].oow * t_start;
			vert[1].tow = vert[1].oow * t_start;
			vert[2].tow = vert[2].oow * t_end;
			vert[3].tow = vert[3].oow * t_end;
#else
			vert[0].tmuvtx[0].tow = vert[0].oow * t_start;
			vert[1].tmuvtx[0].tow = vert[1].oow * t_start;
			vert[2].tmuvtx[0].tow = vert[2].oow * t_end;
			vert[3].tmuvtx[0].tow = vert[3].oow * t_end;
#endif
		}

		// Set S values for horizontally flipped texture		
		if(!(sprite->mode & FLIP_TEX_H))
		{
//#ifndef VEGAS
#if 1
			vert[0].sow = vert[0].oow * s_start;
			vert[1].sow = vert[1].oow * s_end;
			vert[2].sow = vert[2].oow * s_end;
			vert[3].sow = vert[3].oow * s_start;
#else
			vert[0].tmuvtx[0].sow = vert[0].oow * s_start;
			vert[1].tmuvtx[0].sow = vert[1].oow * s_end;
			vert[2].tmuvtx[0].sow = vert[2].oow * s_end;
			vert[3].tmuvtx[0].sow = vert[3].oow * s_start;
#endif
		}

		// Set S values for texture that is NOT horizontally flipped
		else
		{
//#ifndef VEGAS
#if 1
			vert[0].sow = vert[0].oow * s_end;
			vert[1].sow = vert[1].oow * s_start;
			vert[2].sow = vert[2].oow * s_start;
			vert[3].sow = vert[3].oow * s_end;
#else
			vert[0].tmuvtx[0].sow = vert[0].oow * s_end;
			vert[1].tmuvtx[0].sow = vert[1].oow * s_start;
			vert[2].tmuvtx[0].sow = vert[2].oow * s_start;
			vert[3].tmuvtx[0].sow = vert[3].oow * s_end;
#endif
		}
//#ifdef VEGAS
#if 0
		vert[0].tmuvtx[1].sow = vert[0].tmuvtx[0].sow;
		vert[1].tmuvtx[1].sow = vert[1].tmuvtx[0].sow;
		vert[2].tmuvtx[1].sow = vert[2].tmuvtx[0].sow;
		vert[3].tmuvtx[1].sow = vert[3].tmuvtx[0].sow;
		vert[0].tmuvtx[1].tow = vert[0].tmuvtx[0].tow;
		vert[1].tmuvtx[1].tow = vert[1].tmuvtx[0].tow;
		vert[2].tmuvtx[1].tow = vert[2].tmuvtx[0].tow;
		vert[3].tmuvtx[1].tow = vert[3].tmuvtx[0].tow;
#endif
	}
}

void generate_sprite_verts(register sprite_info_t *sprite)
{
	_generate_sprite_verts(sprite);
	_generate_sprite_st(sprite);
}

/*
** check_for_notification() - This function is used to determine when the
** user defined limit for some field on a sprite is hit.
*/
static int check_for_notification(float *val, float *end_val, float *vel, int note_code)
{
	// Positive velocity
	if(*vel > 0.0F)
	{
		// Reached limit
		if(*val >= *end_val)
		{
			// YES - set velcity to 0
			*vel = 0.0F;

			// Glitch it to end position
			*val = *end_val;

			// Notify termination point reached
			return(note_code);
		}
	}

	// Negative velocity
	else if(*vel < 0.0F)
	{
		// Reached limit
		if(*val <= *end_val)
		{
			// YES - set velocity to 0
			*vel = 0.0F;

			// Glitch it to end position
			*val = *end_val;

			// Notify termination point reached
			return(note_code);
		}
	}

	// No notification for this item
	return(0);
}

/*
** notify_check() - This function is used when the user wants a function
** called when a sprite hits one or more user defined limits.
*/
static void notify_check(sprite_info_t *sprite)
{
	int	notifications;

	// Initialize notifications to be made
	notifications = 0;

	// X position notification
	if((sprite->notify_modes & NOTIFY_X_POS))
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->x,
			&sprite->x_end,
			&sprite->x_vel,
			NOTIFY_X_POS);
	}

	// Y position notification
	if((sprite->notify_modes & NOTIFY_Y_POS))
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->y,
			&sprite->y_end,
			&sprite->y_vel,
			NOTIFY_Y_POS);
	}

	// Z position notification
	if((sprite->notify_modes & NOTIFY_Z_POS))
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->z,
			&sprite->z_end,
			&sprite->z_vel,
			NOTIFY_Z_POS);
	}

	// X axis rotation notification
	if(sprite->notify_modes & NOTIFY_X_ROT)
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->x_angle,
			&sprite->x_angle_end,
			&sprite->x_ang_vel,
			NOTIFY_X_ROT);
	}

	// Y axis rotation notifiction
	if(sprite->notify_modes & NOTIFY_Y_ROT)
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->y_angle,
			&sprite->y_angle_end,
			&sprite->y_ang_vel,
			NOTIFY_Y_ROT);
	}

	// Z axis rotation notification
	if(sprite->notify_modes & NOTIFY_Z_ROT)
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->z_angle,
			&sprite->z_angle_end,
			&sprite->z_ang_vel,
			NOTIFY_Z_ROT);
	}

	// W scaling notification
	if(sprite->notify_modes & NOTIFY_W_SCALE)
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->w_scale,
			&sprite->w_scale_end,
			&sprite->w_scale_vel,
			NOTIFY_W_SCALE);
	}

	// H scaling notification
	if(sprite->notify_modes & NOTIFY_H_SCALE)
	{
		// YES - Check to see if sprite is at termination position
		notifications |= check_for_notification(&sprite->h_scale,
			&sprite->h_scale_end,
			&sprite->h_scale_vel,
			NOTIFY_H_SCALE);
	}

	// Turn off notification bits for conditions that are being
	// notified about at this time
	sprite->notify_modes &= ~notifications;

	// If there is no user installed notification function for
	// this sprite then we are done.
	if(!sprite->notify_func)
	{
		return;
	}

	// Notifications to be made
	if(notifications)
	{
		// YES - Call the user specified notification function for
		// this sprite.
		sprite->notify_func(sprite, notifications);
	}
}

/*
** insert_node() - This function is used to insert a sprite into the
** alpha channeled sprite draw list in descending Z order.
*/
static void insert_node(sprite_node_t *snode)
{
	register sprite_node_t	*sn_list = alpha_sprite_node_list;
	register sprite_node_t	*last_sn = alpha_sprite_node_list;

	// Nothing in list
	if(!sn_list)
	{
		// Set this node next and previous
		snode->next = (sprite_node_t *)0;
		snode->prev = (sprite_node_t *)0;

		// Set the alpha sprite node list pointer to this node
		alpha_sprite_node_list = snode;

		// Done
		return;
	}

	// Walk the list until we find a sprite with a Z greater than or
	// equal to the sprite we are adding to the list or we hit the
	// end of the list, whichever comes first
	while(sn_list && (sn_list->si->z >= snode->si->z))
	{
		// Save the pointer to the last node
		last_sn = sn_list;

		// Next node in list
		sn_list = sn_list->next;
	}

	// Check to see if insert is to done on front of list
	if(sn_list == alpha_sprite_node_list)
	{
		// Set this nodes next to the current alpha sprite list
		snode->next = alpha_sprite_node_list;

		// Set the nodes previous to NULL
		snode->prev = (sprite_node_t *)0;

		// Is there a next node
		if(snode->next)
		{
			// YES - Set its previous to the current
			snode->next->prev = snode;
		}

		// Set the alpha sprite node list pointer to the current node
		alpha_sprite_node_list = snode;
	}

	// Otherwise if sn_list is not NULL then we are inserting into
	// the middle of the list somewhere.
	else if(sn_list)
	{
		// Set the new node next to point at the current node
		snode->next = sn_list;

		// Set the new node previous to point at the current nodes previous
		snode->prev = sn_list->prev;

		// Set the current node previous to point at the new node
		sn_list->prev = snode;

		// Is there a previous node
		if(snode->prev)
		{
			// YES - set it's next to the current node
			snode->prev->next = snode;
		}
	}

	// Otherwise we are inserting at the end of the list
	else
	{
		// Set the last nodes next to the current node
		last_sn->next = snode;

		// Set the current node previous to the last node
		snode->prev = last_sn;

		// Set the current node next to NULL
		snode->next = (sprite_node_t *)0;
	}
}

/*
** change_sprite_alpha_state() - This function is used to change a sprite
** to or from being alpha channeled.
*/
void change_sprite_alpha_state(sprite_info_t *sprite, int mode)
{
	sprite_node_t	*snode;

	// Check to see if mode is really being changed
	if(!((sprite->mode & ALPHA_MODE) ^ mode))
	{
		// NOPE - Done
		return;
	}

	// Get the sprites node pointer
	snode = (sprite_node_t *)sprite->node_ptr;

	// Does a node exist after this one
	if(snode->next)
	{
		// YES - link it's previous to current previous
		snode->next->prev = snode->prev;
	}

	// Does a node exist before this one
	if(snode->prev)
	{
		// YES - link it's next to current next
		snode->prev->next = snode->next;
	}

	// Switching to alpha mode
	if(mode)
	{
		// Turn on the alpha mode bit
		sprite->mode |= ALPHA_MODE;

		// Does texture have alpha embedded in it
		if(sprite->tn->texture_info.header.format == GR_TEXFMT_RGB_332 ||
			sprite->tn->texture_info.header.format == GR_TEXFMT_INTENSITY_8 ||
			sprite->tn->texture_info.header.format == GR_TEXFMT_P_8 ||
			sprite->tn->texture_info.header.format == GR_TEXFMT_YIQ_422 ||
			sprite->tn->texture_info.header.format == GR_TEXFMT_RGB_565)
		{
			// NOPE - Set alpha source to constant alpha
			sprite->state.alpha_source = GR_ALPHASOURCE_CC_ALPHA;
		}

		// Texture with embedded alpha
		else
		{
			// Set alpha source to texture
			sprite->state.alpha_source = GR_ALPHASOURCE_TEXTURE_ALPHA_TIMES_CONSTANT_ALPHA;
		}

		// Set SRC RGB blending function
		sprite->state.alpha_rgb_src_function = GR_BLEND_SRC_ALPHA;

		// Set DST RGB blending function
		sprite->state.alpha_rgb_dst_function = GR_BLEND_ONE_MINUS_SRC_ALPHA;

		// Set SRC Alpha blending function
		sprite->state.alpha_a_src_function = GR_BLEND_ONE;

		// Set DST Alpha blending function
		sprite->state.alpha_a_dst_function = GR_BLEND_ZERO;

		// Set color combiner for alpha blending
		sprite->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA;

		// Turn off chroma keying
		// April 9th, 98 : Took this out so that I can fade strings...
//		sprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

		// Unlink it from the non-alpha list
		unlink_snode(&sprite_node_list, snode);

		// Insert the node into the alpha sprite list in descending Z order
		insert_node(snode);
	}

	// Switching FROM alpha mode
	else
	{

		// Turn off the alpha mode bit
		sprite->mode &= ~ALPHA_MODE;

		// Set the alpha source
		sprite->state.alpha_source = GR_ALPHASOURCE_CC_ALPHA;

		// Set the SRC RGB blending function
		sprite->state.alpha_rgb_src_function = GR_BLEND_ONE;

		// Set the DST RGB blending function
		sprite->state.alpha_rgb_dst_function = GR_BLEND_ZERO;

		// Set the SRC Alpha blending function
		sprite->state.alpha_a_src_function = GR_BLEND_ONE;

		// Set the DSt Alpha blending function
		sprite->state.alpha_a_dst_function = GR_BLEND_ZERO;

		// Set the color combiner to NOT alpha blend
		sprite->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;

		// Unlink the node from the alpha sprites list
		unlink_snode(&alpha_sprite_node_list, snode);

		// There are no nodes prior to this one
		snode->prev = (sprite_node_t *)0;

		// Set this nodes next to the sprite node list
		snode->next = sprite_node_list;

		// If there is a next, set its prev to this node
		if(snode->next)
		{
			snode->next->prev = snode;
		}

		// Reset the sprite node list pointer
		sprite_node_list = snode;
	}
}

void change_sprite_children_alpha(void *parent, int mode, int alpha)
{
	register sprite_node_t	*snode;
	register sprite_node_t	*next;

	snode = sprite_node_list;
	while(snode)
	{
		next = snode->next;
		if(snode->si->parent == parent)
		{
			change_sprite_alpha_state(snode->si, mode);
			snode->si->state.constant_color = alpha;
		}
		snode = next;
	}

#if 0
	snode = alpha_sprite_node_list;
	while(snode)
	{
		next = snode->next;
		if(snode->si->parent == parent)
		{
			change_sprite_alpha_state(snode->si, mode);
			snode->si->state.constant_color = alpha;
		}
		snode = next;
	}
#endif
}


/*
** beginobj() - This function is used to create a 2D object that does NOT
** have a constant alpha on the screen.
*/
sprite_info_t *beginobj(image_info_t *ii, float x, float y, float z, int tid)
{
	// Create the sprite using the current process
	// pointer as the parent pointer.
	return(_beginobj((void *)cur_proc, ii, x, y, z, tid));
}


static sprite_node_t *alloc_sprite_nodes(void)
{
	int							i;
	register sprite_node_t	*sn;
	register sprite_node_t	*last = (sprite_node_t *)0;
	register sprite_node_t	*rv = (sprite_node_t *)0;

	if(resource_mode != NO_FREE_MEMORY)
	{
		sn = (sprite_node_t *)malloc(sizeof(sprite_node_t));
		if(sn)
		{
			sn->next = (sprite_node_t *)0;
			sn->prev = (sprite_node_t *)0;
		}
		return(sn);
	}

	// Allocate a block of sprite nodes and initialize the link pointers
	for(i = 0; i < ALLOC_NUM; i++)
	{
		// Allocate memory for sprite node
		sn = (sprite_node_t *)malloc(sizeof(sprite_node_t));

		// First one ?
		if(!i)
		{
			// Set return value
			rv = sn;
		}

		// Did we get memory
		if(!sn)
		{
			break;
		}

		// First node ?
		if(!i)
		{
			// Set it's next and prev to NULL
			sn->prev = (sprite_node_t *)0;
			sn->next = sn->prev;
		}

		// Last node ?
		else if(i == (ALLOC_NUM-1))
		{
			// YES - Terminate list here
			sn->next = (sprite_node_t *)0;

			// Back link to last node
			sn->prev = last;

			// Set last nodes forward link
			sn->prev->next = sn;
		}

		// Node in middle
		else
		{
			// YES - Back link to last node
			sn->prev = last;

			// Set forward link
			sn->next = (sprite_node_t *)0;
			
			// Set last nodes forward link
			sn->prev->next = sn;
		}

		// Set last node pointer
		last = sn;
	}

	// Return new
	return(rv);
}

static sprite_node_t	*get_snode(void)
{
	register sprite_node_t			*snode;

	// Any sprite nodes available on the free list
	if(!free_sprite_node_list)
	{
		// Get sprite nodes
		free_sprite_node_list = alloc_sprite_nodes();

		// Allocation OK
		if(!free_sprite_node_list)
		{
			// NOPE - Inform user of allocation error
#if defined(DEBUG)
			fprintf(stderr, "Could not allocate memory for free sprites\r\n");
			lockup();
#endif
			return((sprite_node_t *)0);
		}

		// Initialize the newly allocated nodes
		snode = free_sprite_node_list;

		// Initialize the fields of the newly allocate sprite nodes
		while(snode)
		{
			snode->si = (sprite_info_t *)0;
			snode = snode->next;
		}
	}

	// Get a free sprite node from the beginning of the
	// free sprite node list
	snode = free_sprite_node_list;

	// Has memory already been allocated for the sprite information
	if(!snode->si)
	{
		// NOPE - allocate memory for the sprite information
		snode->si = malloc(sizeof(sprite_info_t));

		// Was memory allocated
		if(!snode->si)
		{
			// NOPE - inform user of allocation error
#if defined(DEBUG)
			fprintf(stderr, "Could not allocate memory for sprite information\r\n");
			lockup();
#endif
			return((sprite_node_t *)0);
		}

		snode->si->vert_mem = (void *)0;
	}

	return(snode);
}

static void sprite_clear(register sprite_info_t *s)
{
	register int	i = sizeof(sprite_info_t)/sizeof(int);
	register int	*ptr = (int *)s;

	while(i--)
	{
		*ptr++ = 0;
	}
}

/*
** _beginobj() - This function creates the sprite node and initialized all
** of the sprite info structure fields when a new 2D sprite is created.
*/
sprite_info_t *_beginobj(void *parent, image_info_t *ii, float x, float y, float z, int tid)
{
	register sprite_node_t			*snode;
	register sprite_info_t			*sprite;
	register struct texture_node	*tn = (struct texture_node *)0;
#if defined(SEATTLE)
	register MidVertex				*verts;
#elif defined(VEGAS)
	register SpriteVertex			*verts;
#else
#error Environment variable TARGET_SYS not set
#endif
	register int						i;
	register state_info_t			*state;

	// Get a sprite node
	snode = get_snode();

	// Did we get one ?
	if(!snode)
	{
		// NOPE - Return failure
		return((sprite_info_t *)0);
	}

	// Initialize the new sprites mode
	snode->mode = 0;

	// Set the sprite pointer
	sprite = snode->si;

	// Adjust the free sprite node list
	free_sprite_node_list = snode->next;

	// Any left on list
	if(free_sprite_node_list)
	{
		// YES - none before this one
		free_sprite_node_list->prev = (sprite_node_t *)0;
	}

	// Image information exists
	if(ii)
	{
		// Textured
		if(ii->texture_name)
		{
			// Create the texture
			tn = create_texture(ii->texture_name, 
				tid,
				0,
				CREATE_NORMAL_TEXTURE,
				GR_TEXTURECLAMP_CLAMP,
				GR_TEXTURECLAMP_CLAMP,
				GR_TEXTUREFILTER_BILINEAR,
				GR_TEXTUREFILTER_BILINEAR);

			// Texture created OK
			if(!tn)
			{
				// NOPE - inform user of texture creation error
#if defined(DEBUG)
				fprintf(stderr, "\nCould not create texture for sprite: %s\r\n", ii->texture_name);
				lockup();
#endif
				return((sprite_info_t *)0);
			}
		}
	}

	if(!sprite->vert_mem)
	{
		// Allocate memory for the vertices for the sprite
#if defined(SEATTLE)
		verts = (MidVertex *)malloc((sizeof(MidVertex) * 4) + 32);
#elif defined(VEGAS)
		verts = (SpriteVertex *)malloc((sizeof(SpriteVertex) * 4) + 32);
#else
#error Environment variable TARGET_SYS not set
#endif
	}
	else
	{
		verts = sprite->vert_mem;
	}

	// Allocated OK
	if(!verts)
	{
		// NOPE - delete texture if one was created
		if(tn)
		{
			delete_texture(tn);
		}

		// Return the sprite node to the beginning of the
		// free sprite node list
		snode->next = free_sprite_node_list;

		// Set list to point at node returned
		free_sprite_node_list = snode;

		// If a next node exists, back link it
		if(snode->next)
		{
			snode->next->prev = snode;
		}

		// Inform user of vertex memory allocation error
#if defined(DEBUG)
		fprintf(stderr, "\r\nCan not allocate memory for sprite vertices\r\n");
		lockup();
#endif
		return((sprite_info_t *)0);
	}

	// Clear out the sprite information
//	memset(sprite, 0, sizeof(sprite_info_t));
	sprite_clear(sprite);

	// Set the current sprite X position
	sprite->x = x;

	// Set the current sprite Y position
	sprite->y = y;

	// Set the current sprite Z position
	sprite->z = z;

	// Set the current sprite width scaling factor to 1
	sprite->w_scale = 1.0F;

	// Set the current sprite height scaling factor to 1
	sprite->h_scale = 1.0F;

	// Set the current sprite horizontal tiling factor to 1
	sprite->tile_x = 1.0F;

	// Set the current sprite vertical tiling factor to 1
	sprite->tile_y = 1.0F;

	// Set the sprites pointer to it's texture node
	sprite->tn = tn;

	// Set the sprites pointer to it's image information
	sprite->ii = ii;

	// Save the pointer to the allocated memory for the vertices
	sprite->vert_mem = (void *)verts;

	// Default the sprite id to 0
	sprite->id = 0;

	// Set the sprites parent pointer
	sprite->parent = parent;

	// Is the vertex memory cache page aligned
	if((int)verts & 0x1c)
	{
		// NOPE - Get it cache page aligned
		int	*ptr = (int *)verts;
		do
		{
			++ptr;
		} while((int)ptr & 0x1c);
#if defined(SEATTLE)
		verts = (MidVertex *)ptr;
#elif defined(VEGAS)
		verts = (SpriteVertex *)ptr;
#else
#error Environment variable TARGET_SYS not set
#endif
	}

	// Set the sprites vertex array pointer
	sprite->verts = verts;

#ifndef VEGAS
#if ((SPRITE_RGB & 1) || !(USE_MID_VERTEX & 1))
	// Default the vertex colors to white
	for(i = 0; i < 4; i++)
	{
		sprite->verts[i].r = 255.0f;
		sprite->verts[i].g = 255.0f;
		sprite->verts[i].b = 255.0f;
	}
#endif
#endif

	// Set pointer to sprite state information
	state = &sprite->state;

	// Default the sprite cull mode to OFF
	state->cull_mode = GR_CULL_DISABLE;

#if (!(MULTIPART_IMAGES & 1))
	// Set the default chroma keying color to BLACK
	state->chroma_color = 0;
#endif

	// Textured sprite
	if(tn)
	{
		// YES - default texture combiner to DECAL
		state->texture_combiner_function = GR_TEXTURECOMBINE_DECAL;
	}

	// Non-Textured sprite
	else
	{
		// NO - Default texture combiner to ONE
		state->texture_combiner_function = GR_TEXTURECOMBINE_ONE;
	}

	// Set default texture filter mode
	state->texture_filter_mode = GR_TEXTUREFILTER_BILINEAR;

	// Set default texture clamping mode
	state->texture_clamp_mode = GR_TEXTURECLAMP_CLAMP;

	// Set the default constant color to WHITE
	state->constant_color = 0xffffffff;

	// Generate the vertices for the sprite
	_generate_sprite_verts(sprite);


	// Textured sprite
	if(tn)
	{
		// Texture without embedded alpha information
		if(tn->texture_info.header.format == GR_TEXFMT_RGB_332 ||
			tn->texture_info.header.format == GR_TEXFMT_INTENSITY_8 ||
			tn->texture_info.header.format == GR_TEXFMT_P_8 ||
			tn->texture_info.header.format == GR_TEXFMT_YIQ_422 ||
			tn->texture_info.header.format == GR_TEXFMT_RGB_565)
		{
			// Set the default alpha source
			state->alpha_source = GR_ALPHASOURCE_CC_ALPHA;

			// Set the default SRC RGB blending function
			state->alpha_rgb_src_function = GR_BLEND_ONE;

			// Set the default DST RGB blending function
			state->alpha_rgb_dst_function = GR_BLEND_ZERO;

			// Set the default SRC Alpha blending function
			state->alpha_a_src_function = GR_BLEND_ONE;

			// Set the default DST Alpha blending function
			state->alpha_a_dst_function = GR_BLEND_ZERO;

			// Set the default color combiner function the DECAL
			state->color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;
	
			// Set the default chroma key mode to ON
			state->chroma_key_mode = GR_CHROMAKEY_ENABLE;
	
			// Link the new node to the the non-alpha blended sprite list
			snode->next = sprite_node_list;

			// No nodes prior to this node
			snode->prev = (sprite_node_t *)0;

			// Set the non-alpha sprite list to this node
			sprite_node_list = snode;
	
			// Is there a node after this one
			if(snode->next)
			{
				// Set its previous to the new node
				snode->next->prev = snode;
			}
		}
	
		// Textures with embedded alpha
		else
		{
			// Set the default alpha source
			state->alpha_source = GR_ALPHASOURCE_TEXTURE_ALPHA_TIMES_CONSTANT_ALPHA;
	
			// Set the default SRC RGB blending function
			state->alpha_rgb_src_function = GR_BLEND_SRC_ALPHA;

			// Set the default DST RGB blending function
			state->alpha_rgb_dst_function = GR_BLEND_ONE_MINUS_SRC_ALPHA;

			// Set the default SRC Alpha blending function
			state->alpha_a_src_function = GR_BLEND_SRC_ALPHA;

			// Set the default DST Alpha blending function
			state->alpha_a_dst_function = GR_BLEND_ONE_MINUS_SRC_ALPHA;

			// Set the color combiner to alpha blend
			state->color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA;

			// Set the default chroma key mode to OFF
			state->chroma_key_mode = GR_CHROMAKEY_DISABLE;
	
			// Insert the sprite into the alpha sprite list in descending Z order
			insert_node(snode);

			// Set the mode sprites mode to alpha mode
			sprite->mode |= ALPHA_MODE;
		}

		// Generate the S and T information for the sprite
		_generate_sprite_st(sprite);
	}

	// Non-textured sprite (constant color sprite)
	else
	{
		// Set the default alpha source
		state->alpha_source = GR_ALPHASOURCE_CC_ALPHA;

		// Set the default SRC RGB blending function
		state->alpha_rgb_src_function = GR_BLEND_ONE;

		// Set the default DST RGB blending function
		state->alpha_rgb_dst_function = GR_BLEND_ZERO;

		// Set the default SRC Alpha blending function
		state->alpha_a_src_function = GR_BLEND_ONE;

		// Set the default DST Alpha blending function
		state->alpha_a_dst_function = GR_BLEND_ZERO;

		// Set the default color combiner function to texture time ccrgb
#ifndef VEGAS
		state->color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
#else
		state->color_combiner_function = GR_COLORCOMBINE_CCRGB;
#endif
	
		// Set the default chroma key mode to OFF
		state->chroma_key_mode = GR_CHROMAKEY_DISABLE;

		// Set the default constant color to WHITE
		state->constant_color = WHITE;
	
		// Link the new node to the the non-alpha blended sprite list
		snode->next = sprite_node_list;

		// No nodes prior to this node
		snode->prev = (sprite_node_t *)0;

		// Generate dummy values for the S and T's to prevent fp exceptions
		for(i = 0; i < 4; i++)
		{
//#ifndef VEGAS
#if 1
			sprite->verts[i].sow = 1.0f;
			sprite->verts[i].tow = 1.0f;
#else
			sprite->verts[i].tmuvtx[0].sow = 1.0f;
			sprite->verts[i].tmuvtx[0].tow = 1.0f;
			sprite->verts[i].tmuvtx[1].sow = 1.0f;
			sprite->verts[i].tmuvtx[1].tow = 1.0f;
#endif
		}

		// Set the non-alpha sprite list to this node
		sprite_node_list = snode;
	
		// Is there a node after this one
		if(snode->next)
		{
			// Set its previous to the new node
			snode->next->prev = snode;
		}
	}
	
	// Set the node ptr for the sprite
	sprite->node_ptr = (void *)snode;

#if (MULTIPART_IMAGES & 1)
	// If this is a multipart image - create the sprites
	// for each of the additional parts.
	// NOTE - We assume that the animation points for each of the parts
	// is set such that the entire image will be put together in the
	// proper places based on the single point of control
	// NOTE - This makes this function recursive

	// Does image info exist
	if(ii)
	{
		// YES - Is this a multi-part image
		if(ii->next)
		{
			// YES - Create a child sprite
			sprite->mp_next = _beginobj(sprite, ii->next, x, y, z, tid);
		}
		else
		{
			// NO - Terminate the multi-part list
			sprite->mp_next = (sprite_info_t *)0;
		}
	}
#endif

	// Return the pointer to the sprite information to the caller
	return(sprite);
}

/* DOES SOMETHING INTERESTING - sort of a masked effect - try it and see
sprite->state.alpha_source = GR_ALPHASOURCE_TEXTURE_ALPHA;
sprite->state.alpha_rgb_src_function = GR_BLEND_SRC_COLOR;
sprite->state.alpha_rgb_dst_function = GR_BLEND_ONE_MINUS_SRC_ALPHA;
sprite->state.alpha_a_src_function = GR_BLEND_ONE;
sprite->state.alpha_a_dst_function = GR_BLEND_ZERO;
sprite->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA;
*/

/*
** unlink_snode() - This function is used to unlink a sprite node from a
** sprite node list.
**
*/
int unlink_snode(sprite_node_t **sn_list_ptr, sprite_node_t *snode)
{
	sprite_node_t	*sn_list;

	// Set list pointer
	sn_list = *sn_list_ptr;

	// Walk list
	while(sn_list)
	{
		// Does this node match
		if(sn_list == snode)
		{
			// YES - Is is it first node on the list
			if(snode == *sn_list_ptr)
			{
				// YES - Set the new list beginning
				*sn_list_ptr = snode->next;

				// Does a node exist after this one
				if(snode->next)
				{
					// YES - Set its previous to NULL
					snode->next->prev = (sprite_node_t *)0;
				}
			}

			// Node NOT first on list
			else
			{
				// Is there a node after this one
				if(snode->next)
				{
					// YES - Set its previous to current previous
					snode->next->prev = snode->prev;
				}

				// Is there a node before this one
				if(snode->prev)
				{
					// YES - Set its next to current next
					snode->prev->next = snode->next;
				}
			}

			// Done - return success
			return(1);
		}

		// Get next node on list
		sn_list = sn_list->next;
	}

	// Went through list without finding a match
	// Return failure
	return(0);
}

/*
** free_sprite() - This function is used to free up the memory used by a
** sprite when that sprite is deleted.
*/
void free_sprite_node(sprite_node_t *snode)
{
	// is this sprite 2d animatable ? Cos if it is, we need to free up the anim info
	// structure pointed at by	PARENT
	if ((snode->si->mode & ANIM_2D) != 0)
		free_anim_space(snode);

	// Textured sprite
	if(snode->si->tn)
	{
		// YES - Delete the texture
		delete_texture(snode->si->tn);
	}

	// Free the memory used for the vertices
//	free(snode->si->vert_mem);

//	snode->si->vert_mem = (void *)0;

	if(resource_mode == NO_FREE_MEMORY)
	{
		// Set next to point at free sprite node list
		snode->next = free_sprite_node_list;

		// None previous to this one
		snode->prev = (sprite_node_t *)0;

		// Does a next node exist
		if(snode->next)
		{
			// YES - Set its previous to the current node
			snode->next->prev = snode;
		}

		// Reset the free list pointer
		free_sprite_node_list = snode;
	}
	else
	{
		// Free memory the sprite info memory
		free(snode->si);

		snode->si = (void *)0;

		// Free the snode memory
		free(snode);
	}
}

/*
** delobj() - This function is used to delete a sprite from the specified
** sprite list.
*/
void delobj(sprite_info_t *sprite)
{
	// Is sprite on alpha sprite list
	if(sprite->mode & ALPHA_MODE)
	{
		// YES - Take it off of alpha sprite list
		if(!(unlink_snode(&alpha_sprite_node_list, (sprite_node_t *)sprite->node_ptr)))
		{
			// ERROR
			return;
		}
	}

	// Sprite on non-alpha sprite list
	else
	{
		// Take if off of non-alpha sprite list
		if(!(unlink_snode(&sprite_node_list, (sprite_node_t *)sprite->node_ptr)))
		{
			// ERROR
			return;
		}
	}

	// Free up the resource used by the sprite
	free_sprite_node((sprite_node_t *)sprite->node_ptr);

#if (MULTIPART_IMAGES & 1)
	// Multipart sprite
	if(sprite->mp_next)
	{
		// YES - Delete the next part
		delobj(sprite->mp_next);
	}
#endif
}

/*
** _del1c() - This function is used to delete multiple sprites specified by
** their sprite id from the sprite list specified by slist.
*/
void _del1c(sprite_node_t **snode_list_ptr, int id, int id_mask)
{
	register sprite_node_t	*snode = *snode_list_ptr;
	register sprite_node_t	*next;

	// Walk a sprite node list
	while(snode)
	{
		// Save the next node pointer
		next = snode->next;

		// Node we are looking for
		if((snode->si->id & id_mask) == id)
		{
			// YES - Take it off of the list
			unlink_snode(snode_list_ptr, snode);

			// Free up its resources
			free_sprite_node(snode);
		}

		// Get next node
		snode = next;
	}
}

/*
** del1c() - This function is used to delete all occurances of sprites with
** the specfied id from both of the sprite lists.
*/
void del1c(int id, int id_mask)
{
	// Delete all matching sprites from non-alpha sprite list
	_del1c(&sprite_node_list, id, id_mask);

	// Delete all matching sprites from alpha sprite list
	_del1c(&alpha_sprite_node_list, id, id_mask);
}

/*
** _del_child_obj() - This function is used to delete all sprites specified by
** the parent pointer from the sprite list specified by slist.
*/
static void _del_child_obj(sprite_node_t **snode_list_ptr, void *parent)
{
	register sprite_node_t	*snode = *snode_list_ptr;
	register sprite_node_t	*next;

	// Walk a sprite list
	while(snode)
	{
		// Save the next node pointer
		next = snode->next;

		// Node we are looking for ?
		if(snode->si->parent == parent)
		{
			// YES - Unlink from it's list
			unlink_snode(snode_list_ptr, snode);

			// Free up the sprite resources
			free_sprite_node(snode);
		}

		// Get the next node
		snode = next;
	}
}

/*
** del_child_obj() - This function is used to delete all sprites specified by
** the parent pointer from all sprite lists.
*/
void del_child_obj(void *parent)
{
	// Delete all child sprites from non-alpha sprite list
	_del_child_obj(&sprite_node_list, parent);

	// Delete all child sprites from alpha sprite list
	_del_child_obj(&alpha_sprite_node_list, parent);
}

/*
** delete_all_sprites() - This function is used to delete all sprites from
** all sprite lists.
*/
void delete_all_sprites(void)
{
	// Delete all sprites from the non-alpha list
	_del1c(&sprite_node_list, 0, 0);

	// Delete all sprites from the alpha sprite list
	_del1c(&alpha_sprite_node_list, 0, 0);
}

/*
** set_sprite_mode() - This function is used to set up the graphics engine
** to draw the sprite specified by sprite.
*/
#ifndef VEGAS
void set_sprite_mode(sprite_info_t *sprite)
{
	register state_info_t	*state;

	// Get pointer to state information
	state = &sprite->state;

	// Set the texture source
	if(sprite->tn)
	{
		guTexSource(sprite->tn->texture_handle);
	}

	// Set the texture combining function
	grTexCombineFunction(0, state->texture_combiner_function);

	/* Set the backface culling mode */
//	grCullMode(sprite->state.cull_mode);

	// Set the source for alpha information
	guAlphaSource(sprite->state.alpha_source);

	// Set the color to used for special modes
	grConstantColorValue(sprite->state.constant_color);

	// Set the chroma key color
#if (!(MULTIPART_IMAGES & 1))
	grChromakeyValue(sprite->state.chroma_color);
#else
	grChromaKeyValue(sprite->ii->chroma_color);
#endif

	// Set the texture filtering mode
	grTexFilterMode(0, sprite->state.texture_filter_mode, sprite->state.texture_filter_mode);

	// Set the texture clamp mode
	grTexClampMode(0, sprite->state.texture_clamp_mode, sprite->state.texture_clamp_mode);

	// Set the color combiner mode
	guColorCombineFunction(sprite->state.color_combiner_function);

	// Set the chroma key
	grChromakeyMode(sprite->state.chroma_key_mode);

	// Set the alpha blending functions
	grAlphaBlendFunction(sprite->state.alpha_rgb_src_function,
		sprite->state.alpha_rgb_dst_function,
		sprite->state.alpha_a_src_function,
		sprite->state.alpha_a_dst_function);
}
#else
void set_sprite_mode(sprite_info_t *sprite)
{
	register state_info_t	*state;


#if 0
grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                        GR_COMBINE_FACTOR_ONE,
                        GR_COMBINE_LOCAL_NONE,
                        GR_COMBINE_OTHER_TEXTURE,
                        FXFALSE );
guTexSource(sprite->tn->texture_handle);
return;
#endif


	// Get pointer to state information
	state = &sprite->state;

	// Set the texture source
	if(sprite->tn)
	{
		guTexSource(sprite->tn->texture_handle);
	}

	if(!first_sprite)
	{
		if(!memcmp(state, &last_sprite_state, sizeof(state_info_t)))
		{
			return;
		}
	}

	// Set the texture source
//	if(sprite->tn)
//	{
//		guTexSource(sprite->tn->texture_handle);
//	}

	// Set the texture combining function
	if((state->texture_combiner_function != last_sprite_state.texture_combiner_function) || first_sprite)
	{
		grTexCombineFunction(0, state->texture_combiner_function);
	}

	/* Set the backface culling mode */
	if((state->cull_mode != last_sprite_state.cull_mode) || first_sprite)
	{
//		grCullMode(state->cull_mode);
	}

	// Set the source for alpha information
	if((state->alpha_source != last_sprite_state.alpha_source) || first_sprite)
	{
		guAlphaSource(state->alpha_source);
	}

	// Set the color to used for special modes
	if((state->constant_color != last_sprite_state.constant_color) || first_sprite)
	{
		grConstantColorValue(state->constant_color);
	}

	// Set the chroma key color
#if (!(MULTIPART_IMAGES & 1))
	if((state->chroma_color != last_sprite_state.chroma_color) || first_sprite)
	{
#if defined(VOODOO2)
		_grChromaRangeExt(state->chroma_color, state->chroma_color, GR_CHROMARANGE_RGB_ALL);
#else
		grChromakeyValue(state->chroma_color);
#endif
	}
#else
	grChromakeyValue(sprite->ii->chroma_color);
#endif

	// Set the texture filtering mode
	if((state->texture_filter_mode != last_sprite_state.texture_filter_mode) || first_sprite)
	{
		grTexFilterMode(0, state->texture_filter_mode, state->texture_filter_mode);
	}

	// Set the texture clamp mode
	if((state->texture_clamp_mode != last_sprite_state.texture_clamp_mode) || first_sprite)
	{
		grTexClampMode(0, state->texture_clamp_mode, state->texture_clamp_mode);
	}

	// Set the color combiner mode
	if((state->color_combiner_function != last_sprite_state.color_combiner_function) || first_sprite)
	{
		guColorCombineFunction(state->color_combiner_function);
	}

	// Set the chroma key
	if((state->chroma_key_mode != last_sprite_state.chroma_key_mode) || first_sprite)
	{
		grChromakeyMode(state->chroma_key_mode);
#if defined(VOODOO2)
		if(state->chroma_key_mode == GR_CHROMAKEY_ENABLE)
		{
			_grChromaModeExt(GR_CHROMARANGE_ENABLE);
		}
		else
		{
			_grChromaModeExt(GR_CHROMARANGE_DISABLE);
		}
#else
		grChromakeyMode(state->chroma_key_mode);
#endif
	}

	// Set the alpha blending functions
	if((state->alpha_rgb_src_function != last_sprite_state.alpha_rgb_src_function) ||
		(state->alpha_rgb_dst_function != last_sprite_state.alpha_rgb_dst_function) ||
		(state->alpha_a_src_function != last_sprite_state.alpha_a_src_function) ||
		(state->alpha_a_dst_function != last_sprite_state.alpha_a_dst_function) ||
		first_sprite)
	{
		grAlphaBlendFunction(state->alpha_rgb_src_function,
			state->alpha_rgb_dst_function,
			state->alpha_a_src_function,
			state->alpha_a_dst_function);
	}

	
	memcpy(&last_sprite_state, state, sizeof(state_info_t));
	first_sprite = 0;
}
#endif

/*
** hide_sprite() - This function is used to hide a sprite
** from being drawn.
*/
void hide_sprite(sprite_info_t *sprite)
{
	register sprite_node_t	*snode;

	// Get the sprite node pointer
	snode = (sprite_node_t *)sprite->node_ptr;

	// Turn on hide bit
	snode->mode |= HIDE_SPRITE;
}

/*
** hide_sprites() - This function is used to hide all sprites on all sprite
** lists.
*/
void hide_sprites(void)
{
	register sprite_node_t	*snode;

	// Get pointer to non-alpha sprite list
	snode = sprite_node_list;

	// Walk the list
	while(snode)
	{
		// Set the hide bit
		snode->mode |= HIDE_SPRITE;

		// Get next node
		snode = snode->next;
	}

	// Get pointer to alpha sprite list
	snode = alpha_sprite_node_list;

	// Walk the list
	while(snode)
	{
		// Set the hide bit
		snode->mode |= HIDE_SPRITE;

		// Get next node
		snode = snode->next;
	}
}

/*
** unhide_sprite() - This function is used to unhide a sprite
** that was previously hidden with the hide_sprite call.
*/
void unhide_sprite(sprite_info_t *sprite)
{
	register sprite_node_t	*snode;

	// Get the node pointer
	snode = (sprite_node_t *)sprite->node_ptr;

	// Turn off hide bit
	snode->mode &= ~HIDE_SPRITE;
}

/*
** unhide_sprites() - This function is used to hide all sprites on all
** sprite lists.
*/
void unhide_sprites(void)
{
	register sprite_node_t	*snode;

	// Get pointer to non-alpha sprite list
	snode = sprite_node_list;

	// Walk the list
	while(snode)
	{
		// Turn off hide bit
		snode->mode &= ~HIDE_SPRITE;

		// Get next node
		snode = snode->next;
	}

	// Get pointer to alpha sprite list
	snode = alpha_sprite_node_list;

	// Walk the list
	while(snode)
	{
		// Turn off hide bit
		snode->mode &= ~HIDE_SPRITE;

		// Get next node
		snode = snode->next;
	}
}

/*
** do_sprite_veladd() - This function is used to do the automatic velocity
** add to all sprites on all sprite lists.
*/
void do_sprite_veladd(void)
{
#if 0
	register sprite_node_t	*snode;
	register sprite_info_t	*sprite;
	register int				alpha_z_change = 0;

	/* Do the non-alpha channeled sprite list */
	snode = sprite_node_list;
	while(snode)
	{
		if(snode->mode & DO_VEL_ADD)
		{
			sprite = snode->si;

			/* Add in the velocities */
			sprite->x += sprite->x_vel;
			sprite->y += sprite->y_vel;
			sprite->z += sprite->z_vel;
			sprite->x_angle += sprite->x_ang_vel;
			sprite->y_angle += sprite->y_ang_vel;
			sprite->z_angle += sprite->z_ang_vel;
			sprite->w_scale += sprite->w_scale_vel;
			sprite->h_scale += sprite->h_scale_vel;

			/* Check to see if any notifications need to be made */
			if(sprite->notify_modes)
			{
				notify_check(sprite);
			}

			/* Generate new vertices */
			generate_sprite_verts(sprite);
		}

		/* Get next sprite on the list */
		snode = snode->next;
	}

	/* Do the alpha channeled sprite list */
	snode = alpha_sprite_node_list;
	while(snode)
	{
		if(snode->mode & DO_VEL_ADD)
		{
			sprite = snode->si;

			/* Add in the velocities */
			sprite->x += sprite->x_vel;
			sprite->y += sprite->y_vel;
			sprite->z += sprite->z_vel;
			sprite->x_angle += sprite->x_ang_vel;
			sprite->y_angle += sprite->y_ang_vel;
			sprite->z_angle += sprite->z_ang_vel;
			sprite->w_scale += sprite->w_scale_vel;
			sprite->h_scale += sprite->h_scale_vel;

			if(sprite->z_vel)
			{
				alpha_z_change = 1;
			}

			/* Check to see if any notifications need to be made */
			if(sprite->notify_modes)
			{
				notify_check(sprite);
			}

			/* Generate the vertices for the sprite */
			generate_sprite_verts(sprite);
		}

		/* Get next sprite on the list */
		snode = snode->next;
	}

	// If any of the alpha sprite Z positions have changed the alpha sprites
	// list must be sorted in ascending Z order.
	if(alpha_z_change)
	{
//		sort_alpha_list();
	}
#else
return;
#endif
}

/*
** set_veladd_mode() - This function is used to enable
** or disable velocity adding for a sprite.
*/
void set_veladd_mode(sprite_info_t *sprite, int mode)
{
	register sprite_node_t	*snode;

	// Get the node pointer
	snode = (sprite_node_t *)sprite->node_ptr;

	// Enabling
	if(mode)
	{
		// YES - Turn on velocity add bit
		snode->mode |= DO_VEL_ADD;
	}
	else
	{
		// NO - Turn off velocity add bit
		snode->mode &= ~DO_VEL_ADD;
	}
}

/*
** _draw_sprites() - This function is used to draw all sprites on the sprite
** list specified by slist.
*/
#ifdef SPRITE_TIMES
__asm__("
	.set	noreorder
	.globl	___clr_count
___clr_count:
	jr	$31
	mtc0	$0,$9

	.globl	___get_count
___get_count:
	mfc0	$2,$9
	nop
	nop
	jr	$31
	nop
	.set	reorder
");

void ___clr_count(void);
int ___get_count(void);

unsigned int	avg_tot = 0;
unsigned int	avg_count = 0;
#endif

static void _draw_sprites(register sprite_node_t *s)
{
	register sprite_info_t	*sprite;
	register sprite_node_t	*snode = s;
#ifdef SPRITE_TIMES
register int	t = 0;
register int	num = 0;
#endif

#if (DISPLAY_TIMING_INFO & 1)
	gen_time = 0;
	generated = 0;
#endif

#ifdef VEGAS
	first_sprite = 1;
#endif

	// Walk the sprite node list
	while(snode)
	{
		// Get the sprite info pointer
		sprite = snode->si;

		// Velocity add on for this sprite
		if(snode->mode & DO_VEL_ADD)
		{
			// YES - Add X velocity
			sprite->x += sprite->x_vel;

			// Add Y Velocity
			sprite->y += sprite->y_vel;

			// Add Z Velocity
			sprite->z += sprite->z_vel;

			// Add X axis rotational velocity
			sprite->x_angle += sprite->x_ang_vel;

			// Add Y axis rotational velocity
			sprite->y_angle += sprite->y_ang_vel;

			// Add Z axis rotational velocity
			sprite->z_angle += sprite->z_ang_vel;

			// Add width scaling velocity
			sprite->w_scale += sprite->w_scale_vel;

			// Add height scaling velocity
			sprite->h_scale += sprite->h_scale_vel;

			// Notification modes enabled
			if(sprite->notify_modes)
			{
				// YES - Check for any notifications to be made
				notify_check(sprite);
			}

#if (DISPLAY_TIMING_INFO & 1)
			start_timer();
#endif
			// Generate new vertices for sprite
			generate_sprite_verts(sprite);
		}

		// Is the sprite mode set to hidden
		if(!(snode->mode & HIDE_SPRITE))
		{
			// Set up the drawing mode for the sprite
			set_sprite_mode(sprite);

#ifndef VEGAS
			//6 Feb 97 - took out if/else per Mike - mb
			// YES - Draw Triangle 1
			grDrawTriangleDma(&sprite->verts[0], &sprite->verts[1], &sprite->verts[2], sprite->state.cull_mode);
			// Draw Triangle 2
			grDrawTriangleDma(&sprite->verts[0], &sprite->verts[2], &sprite->verts[3], sprite->state.cull_mode);
#else
#ifdef SPRITE_TIMES
___clr_count();
#endif
			grDrawSprite((float *)sprite->verts);
#ifdef SPRITES_TIMES
t += ___get_count();
num++;
#endif
#endif

#if (DISPLAY_TIMING_INFO & 1)
			gen_time += get_time();
			generated += 2;
#endif

		}

		// Get the next node in the list
		snode = snode->next;
	}
#ifdef SPRITE_TIMES
if(num)
{
	avg_tot += (t/num);
	avg_count++;
}
#endif
#if (DISPLAY_TIMING_INFO & 1)
	fprintf(stderr, "Total time to draw %d triangles: %d\r\n", generated, gen_time*20);
	if(generated)
	{
#if 0
		float	time;

		time = (float)(gen_time*20);
		time /= (float)generated;
		time /= 1000000000.0F;
		time = 1.0F / time;
		if(time > 1000000.0F)
		{
			fprintf(stderr, "%fM", time / 1000000.0F);
		}
		else if(time > 100000.0F)
		{
			fprintf(stderr, "%fK", time / 100000.0F);
		}
		else
		{
			fprintf(stderr, "%f", time);
		}
		fprintf(stderr, " Triangles/second\r\n");
#else
		gen_time *= 20;
		gen_time /= generated;
		fprintf(stderr, "Avg/Tri: %d\r\n", gen_time);
#endif
	}
#endif
}

/*
** draw_sprites() - This is function is called by the
** process loop to draw the sprites generated and/or
** manipulated by the processes.
*/
void draw_sprites(void)
{
#ifdef VEGAS
	grVertexLayout(GR_PARAM_XY,  GR_VERTEX_X_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_RGB, GR_VERTEX_R_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_A,   GR_VERTEX_A_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Z,   GR_VERTEX_OOZ_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_W,   GR_VERTEX_OOW_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_ST0, GR_VERTEX_SOW_TMU0_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_Q0,  GR_VERTEX_OOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST1, GR_VERTEX_SOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q1,  GR_VERTEX_OOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
#endif

	// Turn off any alpha-testing
	grAlphaTestFunction( GR_CMP_ALWAYS );

	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);

    // KEENAN: added this Feb 10, 1998
	// Make sure the depth mask is on
	grDepthMask(FXTRUE);

	// Make sure the depth buffer is on
	grDepthBufferFunction(GR_CMP_LESS);

	// Make sure the depth bias is off
	grDepthBiasLevel(0.0f);

#ifdef VEGAS
	grCullMode(GR_CULL_NEGATIVE);
#endif

	_draw_sprites(sprite_node_list);
	_draw_sprites(alpha_sprite_node_list);

#ifdef SPRITE_TIMES
if(avg_count)
	fprintf(stderr, "%04d\r", avg_tot/avg_count);
#endif
}


/*
** change_img() - This function is used to change the image for the sprite
** specified by sprite to the image specified by ii.
*/
/*
** NOTE - This function needs to be modified to deal with multipart images
** and also needs to be modified to deal with sprites that change alpha
** state.
*/
void change_img(sprite_info_t *sprite, image_info_t *ii, int tid)
{
	// Does the sprite have a texture
	if(sprite->tn)
	{
		// YES - Delete it
		delete_texture(sprite->tn);
	}

	// Set the new image information for the sprite
	sprite->ii = ii;

	// Create the new texture
	sprite->tn = create_texture(ii->texture_name, 
		(short)tid,
		0,
		CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_CLAMP,
		GR_TEXTURECLAMP_CLAMP,
		GR_TEXTUREFILTER_BILINEAR,
		GR_TEXTUREFILTER_BILINEAR);

	// ERROR
	if(!sprite->tn)
	{
		// Tell user about texture creation error
#if defined(DEBUG)
		fprintf(stderr, "\r\nCould not create texture for sprite: %s\r\n", ii->texture_name);
		lockup();
#endif
		return;
	}
	else
	{
		// Generate new vertex information for the sprite
		generate_sprite_verts(sprite);
	}
}

/*
** init_sprite_verts() - This function is used to initialize the sprite
** drawing stuff.
*/
void init_sprites(void)
{
	int	i;

	// Initialize the transform matrices (identity matrices)
	for(i = 0; i < sizeof(x_mat)/sizeof(float); i++)
	{
		if(!(i % 5))
		{
			x_mat[i] = 1.0F;
			y_mat[i] = 1.0F;
			z_mat[i] = 1.0F;
			c_mat[i] = 1.0F;
			t_mat[i] = 1.0F;
		}
		else
		{
			x_mat[i] = 0.0F;
			y_mat[i] = 0.0F;
			z_mat[i] = 0.0F;
			c_mat[i] = 0.0F;
			t_mat[i] = 0.0F;
		}
	}

	// Initialize the none alpha sprite node list
	sprite_node_list = (sprite_node_t *)0;

	// Initialize the alpha sprite node list
	alpha_sprite_node_list = (sprite_node_t *)0;

	// Initialize the free sprite node list
	free_sprite_node_list = (sprite_node_t *)0;
}


//
// clear_free_sprite_list() - Function to clear out sprite free list
//
void clear_free_sprite_list(void)
{
	register sprite_node_t	*s;
	register sprite_node_t	*next;

	// Get pointer to free list
	s = free_sprite_node_list;

	while(s)
	{
		// Save next node pointer
		next = s->next;

		// Has a sprite info structure been allocated ?
		if(s->si)
		{
			// YES - Has vertex memory been allocated ?
			if(s->si->vert_mem)
			{
				// Free memory used for vertex data
				free(s->si->vert_mem);
			}

			// Free memory used for sprite info
			free(s->si);
		}

		// Free memory used for the sprite node
		free(s);

		// Next node
		s = next;
	}

	// Set the free sprite node list pointer
	free_sprite_node_list = (sprite_node_t *)0;
}


//
// sprite_exists() - Returns TRUE if sprite is on sprite_node-list or
// alpha_sprite_node_list, FALSE otherwise.
//
int sprite_exists( sprite_info_t *psprite )
{
	sprite_node_t	*ps;

	// check non-alpha
	for( ps = sprite_node_list; ps; ps = ps->next )
	{
		if (ps->si == psprite)
			return TRUE;
	}

	// check alpha
	for( ps = alpha_sprite_node_list; ps; ps = ps->next )
	{
		if (ps->si == psprite)
			return TRUE;
	}

	return FALSE;
}

//
// END sprite.c
//
