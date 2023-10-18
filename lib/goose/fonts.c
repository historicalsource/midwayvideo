/* $Revision: 8 $ */
#include	<stdio.h>
#include	<stdlib.h>
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#include	<goose/texture.h>
#include	<goose/sprites.h>
#include	<goose/fonts.h>
#include	<goose/process.h>
#include	<goose/lockup.h>

char	goose_fonts_c_version[] = {"$Revision: 8 $"};

/* This is the loaded fonts list */
font_node_t	*font_list = (font_node_t *)0;


font_node_t *create_font(font_info_t *fi, int id)
{
	font_node_t	*fn;
	int			i;
	int			j;
	int			tex_count = 0;
	char			*tex_names[16];

	// Walk the font list and see if the font being created already exists
	// and has the same id
	fn = font_list;
	while(fn)
	{
		// Does the font info pointers and id's match the font we are creating ?
		if(fn->font_info == fi && fn->id == id)
		{
			// YES - Return the pointer to the existing font
			return(fn);
		}

		// Check the next font
		fn = fn->next;
	}

	// Walk the the image info structures for the font and find out how
	// many unique textures there are
	for(i = 0; i < fi->num_characters; i++)
	{
		// Is this a valid character ?
		if(fi->characters[i])
		{
			// YES - Search found texture names to see if this texture name
			// is unique
			for(j = 0; j < tex_count; j++)
			{
				if((int)fi->characters[i]->texture_name == (int)tex_names[j])
				{
					break;
				}
			}

			// Was it a unique texture name
			if(j == tex_count)
			{
				// YES - Add it to the list of textures
				tex_names[tex_count] = fi->characters[i]->texture_name;
				tex_count++;
				if(tex_count >= (sizeof(tex_names)/sizeof(void *)))
				{
#if defined(DEBUG)
					fprintf(stderr, "create_font(): Too many textures for font\r\n");
					lockup();
#endif
					return((font_node_t *)0);
				}
			}
		}
	}

	// Are there any textures ?
	if(!tex_count)
	{
#if defined(DEBUG)
		fprintf(stderr, "create_font(): No textures for font\r\n");
		lockup();
#endif
		return((font_node_t *)0);
	}

	// Allocate memory for the font node
	fn = (font_node_t *)malloc(sizeof(font_node_t));
	if(!fn)
	{
#if defined(DEBUG)
		fprintf(stderr, "create_font(): Can not allocate memory for font node\r\n");
		lockup();
#endif
		return(fn);
	}

	// Loop to create all of the textures for the font
	for(i = 0; i < tex_count; i++)
	{
		/* Create the texture for this font */
		fn->tn[i] = create_texture(tex_names[i],
			0,
			0,
			CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP,
			GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_POINT_SAMPLED,
			GR_TEXTUREFILTER_POINT_SAMPLED);
#if 0
		if(!fn->tn[i])
		{
#if defined(DEBUG)
			fprintf(stderr, "create_font(): Could not create texture: %s\r\n", tex_names[i]);
			lockup();
#endif
			// Delete the texture that were created (if any)
			while(--i >= 0)
			{
				// Unlock the texture
				unlock_texture(fn->tn[i]);

				// Delete it
				delete_texture(fn->tn[i]);
			}

			// Free the font node
			free(fn);

			// Return fail
			return((font_node_t *)0);
		}
#endif
if(fn->tn[i])
		// Lock the texture
		lock_texture(fn->tn[i]);
	}

	// Finish initializing the font node fields
	fn->font_info = fi;
	fn->id = id;
	fn->num_tex = tex_count;

	// Link the new node to the font list
	fn->prev = (font_node_t *)0;
	fn->next = font_list;
	if(font_list)
	{
		font_list->prev = fn;
	}
	font_list = fn;

	// Return OK
	return(fn);
}


font_node_t *create_font_from_memory(void *font_data, font_info_t *fi, int id)
{
	font_node_t	*fn;
	int			i;
	int			j;
	int			tex_count = 0;
	char			*tex_names[16];

	// Walk the font list and see if the font being created already exists
	// and has the same id
	fn = font_list;
	while(fn)
	{
		// Does the font info pointers and id's match the font we are creating ?
		if(fn->font_info == fi && fn->id == id)
		{
			// YES - Return the pointer to the existing font
			return(fn);
		}

		// Check the next font
		fn = fn->next;
	}

	// Walk the the image info structures for the font and find out how
	// many unique textures there are
	for(i = 0; i < fi->num_characters; i++)
	{
		// Is this a valid character ?
		if(fi->characters[i])
		{
			// YES - Search found texture names to see if this texture name
			// is unique
			for(j = 0; j < tex_count; j++)
			{
				if((int)fi->characters[i]->texture_name == (int)tex_names[j])
				{
					break;
				}
			}

			// Was it a unique texture name
			if(j == tex_count)
			{
				// YES - Add it to the list of textures
				tex_names[tex_count] = fi->characters[i]->texture_name;
				tex_count++;
				if(tex_count >= (sizeof(tex_names)/sizeof(void *)))
				{
#if defined(DEBUG)
					fprintf(stderr, "create_font(): Too many textures for font\r\n");
					lockup();
#endif
					return((font_node_t *)0);
				}
			}
		}
	}

	// Are there any textures ?
	if(!tex_count)
	{
#if defined(DEBUG)
		fprintf(stderr, "create_font(): No textures for font\r\n");
		lockup();
#endif
		return((font_node_t *)0);
	}

	// Allocate memory for the font node
	fn = (font_node_t *)malloc(sizeof(font_node_t));
	if(!fn)
	{
#if defined(DEBUG)
		fprintf(stderr, "create_font(): Can not allocate memory for font node\r\n");
		lockup();
#endif
		return(fn);
	}

	// Loop to create all of the textures for the font
	for(i = 0; i < tex_count; i++)
	{
		/* Create the texture for this font */
		fn->tn[i] = create_texture_from_memory(tex_names[i],
			font_data,
			0,
			0,
			CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP,
			GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_POINT_SAMPLED,
			GR_TEXTUREFILTER_POINT_SAMPLED);
		if(!fn->tn[i])
		{
#if defined(DEBUG)
			fprintf(stderr, "create_font(): Could not create texture: %s\r\n", tex_names[i]);
			lockup();
#endif
			// Delete the texture that were created (if any)
			while(--i >= 0)
			{
				// Unlock the texture
				unlock_texture(fn->tn[i]);

				// Delete it
				delete_texture(fn->tn[i]);
			}

			// Free the font node
			free(fn);

			// Return fail
			return((font_node_t *)0);
		}

		// Lock the texture
		lock_texture(fn->tn[i]);
	}

	// Finish initializing the font node fields
	fn->font_info = fi;
	fn->id = id;
	fn->num_tex = tex_count;

	// Link the new node to the font list
	fn->prev = (font_node_t *)0;
	fn->next = font_list;
	if(font_list)
	{
		font_list->prev = fn;
	}
	font_list = fn;

	// Return OK
	return(fn);
}

static void free_font(font_node_t *fn)
{
	int	i;

	// Loop and delete all of the textures for the font
	for(i = 0; i < fn->num_tex; i++)
	{
		// Unlock the texture
		unlock_texture(fn->tn[i]);

		// Delete the texture
		delete_texture(fn->tn[i]);
	}

	// Free the font node
	free(fn);
}
	

void delete_font(font_node_t *fn)
{
	font_node_t	*flist;

	// Get pointer to font list
	flist = font_list;

	// Search for font and delete it
	while(flist)
	{
		// Is this the font we are looking for ?
		if(fn == flist)
		{
			// YES - Is the node at the beginning of the list ?
			if(fn == font_list)
			{
				// YES - Reset list to point at the next node
				font_list = fn->next;

				// Are there nodes left in the list
				if(font_list)
				{
					// YES - Set list previous to terminate
					font_list->prev = (font_node_t *)0;
				}
			}

			// Is node at end of list ?
			else if(!fn->next)
			{
				// Does a node exist before this one ?
				if(fn->prev)
				{
					// YES - Terminate the list at the previous node
					fn->prev->next = (font_node_t *)0;
				}
			}

			// Node must be in middle of list somewhere
			else
			{
				// Unlink it from the list
				fn->prev->next = fn->next;
				fn->next->prev = fn->prev;
			}

			// Free up the nodes resources
			free_font(fn);

			// All done
			return;
		}

		// Check next node
		flist = flist->next;
	}
#if defined(DEBUG)
	fprintf(stderr, "delete_font(): Attempt to delete non-existant font\r\n");
#endif
}

void delete_font_id(int id)
{
	font_node_t	*flist;
	font_node_t	*fnext;

	// Get list pointer
	flist = font_list;

	// Search list for font with id that matches id passed to function
	while(flist)
	{
		// Save next node pointer
		fnext = flist->next;

		// Id match ?
		if(flist->id == id)
		{
			// YES - delete the node
			delete_font(flist);

			// Done
			return;
		}

		// Check next node
		flist = fnext;
	}
#if defined(DEBUG)
	fprintf(stderr, "delete_font_id(): Attempt to delete non-existant font id: %d\r\n", id);
#endif
}

font_node_t *get_font_handle(int id)
{
	font_node_t	*flist;

	// Get font list pointer
	flist = font_list;

	// Walk list looking for font id that matches id passed
	while(flist)
	{
		// Id match ?
		if(flist->id == id)
		{
			// YES - Return node pointer
			return(flist);
		}

		// Check next font node
		flist = flist->next;
	}

#ifdef DEBUG
	fprintf(stderr, "get_font_handle(): font id: %d not found\n", id);
#endif

	// NOT FOUND
	return((font_node_t *)0);
}

image_info_t *get_char_image_info(int id, char val)
{
	font_node_t	*fn;

	// Get the font node pointer
	fn = get_font_handle(id);

	// Found ?
	if(!fn)
	{
		// NO - return error
		return((image_info_t *)0);
	}

	// Valid character for this font ?
	if(val < fn->font_info->start_character || val > fn->font_info->end_character)
	{
		// NO - return error
		return((image_info_t *)0);
	}

	// Return image info for the character
	val -= fn->font_info->start_character;
	return(fn->font_info->characters[val]);
}

int get_font_space_width(int id)
{
	font_node_t	*fn;

	// Get font node pointer
	fn = get_font_handle(id);

	// Error ?
	if(!fn)
	{
		// YES - Return Error
		return(-1);
	}

	// Return fonts space character width
	return(fn->font_info->space_width);
}

int get_font_height(int id)
{
	font_node_t	*fn;

	// Get the font node pointer
	fn = get_font_handle(id);

	// Error ?
	if(!fn)
	{
		// YES - return error
		return(-1);
	}

	// Return font height
	return(fn->font_info->height);
}

void delete_fonts(void)
{
	font_node_t	*fn;

	// Is the font list empty ?
	if(!font_list)
	{
		// YES - return
		return;
	}
	fn = font_list;
	while(fn->next)
	{
		fn = fn->next;
		free(fn->prev);
	}
	if(fn)
	{
		free(fn);
	}

	font_list = NULL;
}
