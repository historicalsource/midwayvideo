/****************************************************************************/
/*                                                                          */
/* bgnd.c - Background sprites source code for the phoenix system           */
/*                                                                          */
/* Copyright (c) 1996 by Midway Video Inc.                                  */
/* All rights reserved                                                      */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* $Revision: 10 $                                                            */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#include	<goose/goose.h>

extern int	resource_mode;

char	goose_bgnd_c_version[] = {"$Revision: 10 $"};

/* Number of sprite nodes to allocate at a time */
#define	ALLOC_NUM	32

#ifndef VEGAS
void grDrawTriangleDma(MidVertex *, MidVertex *, MidVertex *, int);
#endif

void notify_check(sprite_info_t *);
void set_sprite_mode(sprite_info_t *);
int unlink_snode(sprite_node_t **, sprite_node_t *);
void free_sprite_node(sprite_node_t *);
void _del1c(sprite_node_t **, int, int);

/* List of sprites to use on the background */
/* NOTE - If the pointer to this list is NULL, the system does a standard */
/* screen clear.  If there are sprites on this list they are drawn with   */
/* the depth buffer disabled and the list of sprites is assumed to cover  */
/* the entire screen. */
sprite_node_t	*background_sprite_node_list = (sprite_node_t *)0;

// Flag used to determine whether background draw/screen clear should be done
// at all.
int	do_draw_background = 1;

/* List of free sprite nodes (from sprites.c) */
extern sprite_node_t			*free_sprite_node_list;
extern int						background_color;
extern struct process_node	*cur_proc;
extern int						is_low_res;

void memset(void *p, int data, int size);
void mxm(register float *r, register float *m1, register float *m2);
int check_for_notification(float *val, float *end_val, float *vel, int note_code);
void notify_check(sprite_info_t *sprite);

static sprite_info_t *_beginbobj(void *parent, image_info_t *ii, float x, float y, float z, int tid);

/*
** generate_background_sprite_verts() - This function generates geometry fro
** background sprites.  NOTE - It does NOT support anything other than
** translation.
*/
static void _generate_background_sprite_verts(register sprite_info_t *sprite)
{
	register int	i;

	// Now generate the vertices (scaled)
	sprite->verts[0].x = sprite->x - (sprite->ii->ax * sprite->w_scale);
	sprite->verts[0].y = sprite->y + (sprite->ii->ay * sprite->h_scale);
	sprite->verts[0].oow = 
		sprite->verts[1].oow = 
		sprite->verts[2].oow = 
		sprite->verts[3].oow = 1.0f/60000.0f/*(float)GR_WDEPTHVALUE_FARTHEST*/;
	
	sprite->verts[1].x = sprite->verts[0].x + (sprite->ii->w * sprite->w_scale);
	sprite->verts[1].y = sprite->verts[0].y;
	
	sprite->verts[2].x = sprite->verts[1].x;
	sprite->verts[2].y = sprite->verts[0].y + (sprite->ii->h * sprite->h_scale);
	
	sprite->verts[3].x = sprite->verts[0].x;
	sprite->verts[3].y = sprite->verts[2].y;
	
	for(i = 0; i < 4; i++)
	{
		// Snap x to .25
		sprite->verts[i].x += (float)(1<<21);
		sprite->verts[i].x -= (float)(1<<21);

		if(is_low_res)
		{
			sprite->verts[i].y *= 0.6666f;
		}

		// Snap y to .25
		sprite->verts[i].y += (float)(1<<21);
		sprite->verts[i].y -= (float)(1<<21);

	}
}


void generate_background_sprite_verts(register sprite_info_t *sprite)
{
	_generate_background_sprite_verts(sprite);
	_generate_sprite_st(sprite);
}



/*
** beginbobj() - This function is used to create a 2D object that does NOT
** have a constant alpha on the screen.
*/
sprite_info_t *beginbobj(image_info_t *ii, float x, float y, float z, int tid)
{
	// Create the sprite using the current process
	// pointer as the parent pointer.
	return(_beginbobj((void *)cur_proc, ii, x, y, z, tid));
}

/*
** _beginobj() - This function creates the sprite node and initialized all
** of the sprite info structure fields when a new 2D sprite is created.
*/
static sprite_info_t *_beginbobj(void *parent, image_info_t *ii, float x, float y, float z, int tid)
{
	sprite_node_t			*snode;
	sprite_info_t			*sprite;
	struct texture_node	*tn;
#if defined(SEATTLE)
	MidVertex				*verts;
#elif defined(VEGAS)
	SpriteVertex				*verts;
#else
#error Environment variable TARGET_SYS not set
#endif
	int						i;

	// Any sprite nodes available on the free list
	if(!free_sprite_node_list)
	{
		if(resource_mode == NO_FREE_MEMORY)
		{
			// NOPE - Allocate a block of sprite nodes
			free_sprite_node_list = malloc(sizeof(sprite_node_t) * ALLOC_NUM);
		}
		else
		{
			// NOPE - Allocate a block of sprite nodes
			free_sprite_node_list = malloc(sizeof(sprite_node_t));
		}

		// Allocation OK
		if(!free_sprite_node_list)
		{
			// NOPE - Inform user of allocation error
#if defined(DEBUG)
			fprintf(stderr, "Could not allocate memory for free sprites\r\n");
			lockup();
#endif
			return((sprite_info_t *)0);
		}

		// Initialize the newly allocated nodes
		snode = free_sprite_node_list;

		if(resource_mode == NO_FREE_MEMORY)
		{
			// Initialize the new block of free sprite nodes
			for(i = 0; i < ALLOC_NUM; i++)
			{
				// First node
				if(!i)
				{
					// YES - none prior to this one
					snode->prev = (sprite_node_t *)0;
				}
				else
				{
					// NO - set previous to previous node
					snode->prev = (snode - 1);
				}
	
				// Last node
				if(i < (ALLOC_NUM - 1))
				{
					// NO - Set next to next node
					snode->next = snode + 1;
				}
				else
				{
					// YES - None after this one
					snode->next = (sprite_node_t *)0;
				}
	
				// Set the free nodes sprite info pointer
				snode->si = (sprite_info_t *)0;
	
				// Next node
				snode = snode->next;
			}
		}
		else
		{
			snode->prev = (sprite_node_t *)0;
			snode->next = (sprite_node_t *)0;
	
			// Set the free nodes sprite info pointer
			snode->si = (sprite_info_t *)0;
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
			return((sprite_info_t *)0);
		}
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
				GR_TEXTURECLAMP_WRAP,
				GR_TEXTURECLAMP_WRAP,
				GR_TEXTUREFILTER_BILINEAR,
				GR_TEXTUREFILTER_BILINEAR);

			// Texture created OK
			if(!tn)
			{
				// NOPE - inform user of texture creation error
#if defined(DEBUG)
				fprintf(stderr, "\r\nCould not create texture for background sprite: %s\r\n", ii->texture_name);
				lockup();
#endif
				return((sprite_info_t *)0);
			}
		}

		// Not Textured
		else
		{
			// Set texture node to NULL
			tn = (struct texture_node *)0;
		}
	}

	// No image information
	else
	{
		// Set texture node to NULL
		// This is a NULL node and nothing will be drawn
		tn = (struct texture_node *)0;
	}

	if(!sprite->vert_mem)
	{
		// Allocate memory for the vertices for the sprite
#if defined(SEATTLE)
		verts = (MidVertex *)malloc((sizeof(MidVertex) * 4) + 32);
#elif defined(VEGAS)
		verts = (SpriteVertex *)malloc((sizeof(SpriteVertex) * 4) + 32);
#else
#error Environment variable TARGET_SYS not defined
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
	memset(sprite, 0, sizeof(sprite_info_t));

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

	// Is the vertex memory cache page aligned
	if((int)verts & 0x18)
	{
		// NOPE - Get it cache page aligned
		int	*ptr = (int *)verts;
		do
		{
			++ptr;
		} while((int)ptr & 0x18);
#if defined(SEATTLE)
		verts = (MidVertex *)ptr;
#elif defined(VEGAS)
		verts = (SpriteVertex *)ptr;
#else
#error Environment variable TARGET_SYS not defined
#endif

	}

	// Set the sprites vertex array pointer
	sprite->verts = verts;

	// Default the sprite id to 0
	sprite->id = 0;

	// Set the sprites parent pointer
	sprite->parent = parent;

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

	// Default the sprite cull mode to OFF
	sprite->state.cull_mode = GR_CULL_DISABLE;

#if (!(MULTIPART_IMAGES & 1))
	// Set the default chroma keying color to BLACK
	sprite->state.chroma_color = 0;
#endif

	// YES - default texture combiner to DECAL
	sprite->state.texture_combiner_function = GR_TEXTURECOMBINE_DECAL;

	// Set default texture filter mode
	sprite->state.texture_filter_mode = GR_TEXTUREFILTER_BILINEAR;

	// Set default texture clamping mode
	sprite->state.texture_clamp_mode = GR_TEXTURECLAMP_WRAP;

	// Set the default constant color to WHITE
	sprite->state.constant_color = 0xffffffff;

	// Set the default alpha source
	sprite->state.alpha_source = GR_ALPHASOURCE_CC_ALPHA;

	// Set the default SRC RGB blending function
	sprite->state.alpha_rgb_src_function = GR_BLEND_ONE;

	// Set the default DST RGB blending function
	sprite->state.alpha_rgb_dst_function = GR_BLEND_ZERO;

	// Set the default SRC Alpha blending function
	sprite->state.alpha_a_src_function = GR_BLEND_ONE;

	// Set the default DST Alpha blending function
	sprite->state.alpha_a_dst_function = GR_BLEND_ZERO;

	// Set the default color combiner function the DECAL
	sprite->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;
	
	// Set the default chroma key mode to ON
	sprite->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;

	// Link the new node to the the non-alpha blended sprite list
	snode->next = background_sprite_node_list;

	// No nodes prior to this node
	snode->prev = (sprite_node_t *)0;

	// Set the non-alpha sprite list to this node
	background_sprite_node_list = snode;

	// Is there a node after this one
	if(snode->next)
	{
		// Set its previous to the new node
		snode->next->prev = snode;
	}

	// Generate the geometry for the sprite
	generate_background_sprite_verts(sprite);

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


/*
** delobj() - This function is used to delete a sprite from the specified
** sprite list.
*/
void delbobj(sprite_info_t *sprite)
{
	// Take if off of non-alpha sprite list
	if(!(unlink_snode(&background_sprite_node_list, (sprite_node_t *)sprite->node_ptr)))
	{
		// ERROR
		return;
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
** delb1c() - This function is used to delete all occurances of sprites with
** the specfied id from the background sprite list.
*/
void delb1c(int id, int id_mask)
{
	// Delete all matching sprites from the background sprite list
	_del1c(&background_sprite_node_list, id, id_mask);
}

/*
** delete_all_background_sprites() - This function is used to delete all
** all sprites from the background sprite list.
*/
void delete_all_background_sprites(void)
{
	// Delete all sprites from the background list
	_del1c(&background_sprite_node_list, 0, 0);
}


/*
** draw_background_sprites() - This is function is called by the
** process loop to draw the sprites generated and/or
** manipulated by the processes.
*/
void draw_background(void)
{
	register	sprite_node_t	*snode;
	register	sprite_info_t	*sprite;

	if(!do_draw_background)
	{
		return;
	}

	if(background_sprite_node_list)
	{
		grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ONE, GR_BLEND_ZERO);
		grDepthBufferFunction(GR_CMP_ALWAYS);

		// Get pointer to the list
		snode = background_sprite_node_list;
		while(snode)
		{
			// Get pointer to sprite info
			sprite = snode->si;

			// Velocity add on for this sprite
			if(snode->mode & DO_VEL_ADD)
			{
				// YES - Add X velocity
				sprite->x += sprite->x_vel;

				// Add Y Velocity
				sprite->y += sprite->y_vel;

				// Generate new vertices for sprite
				generate_background_sprite_verts(sprite);
			}

			// Set mode
			set_sprite_mode(sprite);

#ifndef VEGAS
			// Draw Triangle 1
			grDrawTriangleDma(&sprite->verts[0], &sprite->verts[1], &sprite->verts[2], sprite->state.cull_mode);

			// Draw Triangle 2
			grDrawTriangleDma(&sprite->verts[0], &sprite->verts[2], &sprite->verts[3], sprite->state.cull_mode);
#else
			grDrawSprite((float *)sprite->verts);
#endif

			// Get the next node in the list
			snode = snode->next;
		}

		grDepthBufferFunction(GR_CMP_LESS);
	}
	else
	{
    	
        /* KEENAN 21 Jan 98 */
        /* If the draw system left grDepthMask(FXFALSE)  */
        /* clear to the depth buffer wouldn't happen.    */
        /* Set the grDepthMask to TRUE so that the clear */
        /* to the depth buffer always happens.           */
        grDepthMask(FXTRUE);
        
		// If the list do NOT exist just do a buffer clear
		grBufferClear(background_color, 0, GR_WDEPTHVALUE_FARTHEST);
        
	}
}
