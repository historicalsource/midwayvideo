/****************************************************************************/
/*                                                                          */
/* anim2d.c - Sprite 2D animation system for the phoenix system             */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All rights reserved                                                      */
/*                                                                          */
/* Written by:  Jake Simpson                                                */
/* $Revision: 15 $                                                           */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#include	<goose/process.h>
#include	<goose/texture.h>
#include	<goose/object.h>
#include	<goose/sprites.h>
#include <goose/anim2d.h>
#include	<goose/lockup.h>

char	goose_anim2d_c_version[] = {"$Revision: 15 $"};

extern sprite_node_t	*sprite_node_list;
extern sprite_node_t	*alpha_sprite_node_list;

// do the actual animation on individual sprites
void animate_2d(register sprite_node_t *s)
{
	register sprite_info_t	*sprite;
	register sprite_node_t	*snode = s;
	register ani_2d_info_t	*anim;
	void  (*func_call)(sprite_info_t*);
	int	command;
	int	deleted_obj;

	// Walk the sprite node list
	while(snode)
	{
		// Get the sprite info pointer
		sprite = snode->si;
		deleted_obj = 0;
		if ((sprite->mode & ANIM_2D) != 0 )
		{
			anim = sprite->parent;
			anim->frame_delay--;
			if (anim->frame_delay == 0)
			{
				while ((anim->frame_delay == 0) && (deleted_obj != 1))
			  	{
					command = *anim->a_2d_script++;
					switch(command)
					{
						// call a function embedded in the script
						case A_2D_CALL:
							func_call = (void (*)(sprite_info_t*)) *anim->a_2d_script++;
							func_call(sprite);
							break;

						// set up an animation loop
						case A_2D_LOOP:
							anim->loop_count = *anim->a_2d_script++;
							anim->a_loop_script = anim->a_2d_script;
							break;

						// perform the animation loop
						case A_2D_LOOP_BACK:
							if (anim->loop_count == 0)
								break;
							anim->a_2d_script	= anim->a_loop_script;
							anim->loop_count--;
							break;

						// destroy this obj
						case A_KILL_OBJ:
							deleted_obj = 1;
							snode = snode->next;
							delobj(sprite);
							break;

						// goto a specific label in the animation
						case A_2D_GOTO:
							anim->a_2d_script	= (int *) *anim->a_2d_script;
							break;

						//	just a new image, with a delay, and a tid for the image so we can locate it in the texture list
						default:
							anim->frame_delay = command;
							if (*anim->a_2d_script != 0)
							{
								change_img(sprite, (image_info_t *) *anim->a_2d_script++, *anim->a_2d_script++);
							}
							else
							{
								generate_sprite_verts(sprite);
								anim->a_2d_script++;
								anim->a_2d_script++;
							}
							break;
					}
				}
			}
			if (((int)anim->func_call != -1) && (deleted_obj != 1))
				anim->func_call(sprite);

		}
		if (deleted_obj != 1)
			{
			if (snode != 0)
				{
				// get next sprite
				snode = snode->next;
				}
			}

	}
}

// proc that calls the anim process on all sprites
void animate_2d_proc(int *data)
{
	while(1)
	{
		animate_2d(sprite_node_list);
		animate_2d(alpha_sprite_node_list);
		sleep(1);
	}
}

// for debugging purposes, walk a specific 2d animation object list
void walk_2danim_list(register sprite_node_t *s)
{
	register sprite_info_t	*sprite;
	register sprite_node_t	*snode = s;

	// Walk the sprite node list
	while(snode)
	{
		// Get the sprite info pointer
		sprite = snode->si;
		if ((sprite->mode & ANIM_2D) != 0 )
		{
			printf("2d animatable obj %x, id %d\n",(int)sprite,sprite->id);
		}
		snode = snode->next;
	}
}

// for debugging purposes, look throught the 2d object list and see if you can find animatable objs
void check_2danim_list(void)
{
 	walk_2danim_list(sprite_node_list);
 	walk_2danim_list(alpha_sprite_node_list);

}

// make an animating sprite a normal sprite again, using the supplied frame
void unhook_sprite_anim(sprite_info_t *sprite, image_info_t *frame, int frame_tid)
{
	if ((sprite->mode & ANIM_2D) == 0)
	{
		return;
	}

	sprite->mode &= ~ANIM_2D;

	free(sprite->parent);
	sprite->parent = 0;

	change_img(sprite, frame, frame_tid);

	return;
	}

// make a sprite an animatable object. If it already is, just returns
void make_sprite_anim(sprite_info_t *sprite, int* script)
{
	ani_2d_info_t *anim;

	if ((sprite->mode & ANIM_2D) == 0)
	{

		sprite->mode |= ANIM_2D;
		sprite->parent = malloc (sizeof(ani_2d_info_t));

		// Was memory allocated
		if(!sprite->parent)
		{
			// NOPE - inform user of allocation error
#if defined(DEBUG)
			fprintf(stderr, "Could not allocate memory for sprite 2d animation information\r\n");
			lockup();
#endif
			return;
		}
	}

	anim = sprite->parent;
	anim->a_2d_script = script;
	anim->frame_delay = 1;
	anim->loop_count = 0;
 	anim->func_call = (void (*)(sprite_info_t*)) -1;
	
}

// give a function to be performed on a sprite every tick
void add_func_to_sprite(sprite_info_t *sprite, int func_addr )
	{
	ani_2d_info_t *anim;
	anim = sprite->parent;
 	anim->func_call = (void (*)(sprite_info_t*)) func_addr;
	}

// make an animating obj stop animating, while retaining all the anim info
void pause_sprite_anim(sprite_info_t *sprite)
{
	sprite->mode &= ~ANIM_2D;
}

// make a paused animating obj resume animating
void unpause_sprite_anim(sprite_info_t *sprite)
{
	sprite->mode |= ANIM_2D;
}



// when deleting an obj, if its animatable, free up space used by anim info
void free_anim_space(sprite_node_t *snode)
{
	if(snode->si->parent)
	{
		free(snode->si->parent);
	}
}
