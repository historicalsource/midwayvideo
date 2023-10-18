/****************************************************************************/
/*                                                                          */
/* texture.c - Texture managment                                            */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 16 $                                                             */
/*                                                                          */
/****************************************************************************/

/* define TEXTURE_LOAD_DEBUG to print out the texture name (on stderr) */
/* on each load request */
//#define TEXTURE_LOAD_DEBUG 


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<goose/goose.h>
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif

char	goose_texture_c_version[] = {"$Revision: 16 $"};

int	texture_debug = 0;
int	texture_load_in_progress = 0;
int 	texture_node_mode = add_texture_to_free;

#if defined(DEBUG)
int	__process_debug_level = DEBUG_OFF;
int	__sprite_debug_level = DEBUG_OFF;
int	__texture_debug_level = DEBUG_OFF;
int	__string_debug_level = DEBUG_OFF;
int	__font_debug_level = DEBUG_OFF;
int	__object_debug_level = DEBUG_OFF;
int	__texture_load_debug_level = DEBUG_OFF;
#endif

#define ALLOC_NUM	16

struct texture_node	*tlist = (struct texture_node *)0;
struct texture_node	*tfree = (struct texture_node *)0;

// JMS
// This function allows you to control the way that texture node space
// is freed up. Under the default system, a give texture node is placed on
// the free list when it is deleted. If you set the mode to "free_texture_space"
// instead, this will free the memory used buy the node instead of placing it on the
// free texture node list. This stops fragmetation of the heap, but makes allocating
// texture nodes a touch longer, since you have to malloc everytime you need a new one.
// good for those that load all textures in one go, bad for those who do it dynamically
void set_texture_free_mode(int mode)
{
	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", mode);
	texture_node_mode = mode;
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}
// JMS

/*
** strn2upr() - Coverts a string of the specified length to upper case
*/
static void strn2upr(char *s, int length)
{
	while(--length)
	{
		if(*s >= 'a' && *s <= 'z')
		{
			*s &= ~0x20;
		}
		++s;
	}
}

/*
** reset_texture_lists() - This function is used to reset the texture
** list pointers.  This function should only be called if the memory heap
** is being reinitialize.  This function is dangerous. DO NOT EXPORT.
*/
void reset_texture_lists(void)
{
	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	tlist = tfree = (struct texture_node *)0;
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done");
}

/*
** add_texture_block() - This function is used to add texture nodes to the
** free texture list when a request for texture creation is made and there are
** no more available texture nodes on the free texture node list.
*/
static void add_texture_block(void)
{
	struct texture_node	*tn;
	int			i;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	if (texture_node_mode == add_texture_to_free)
	{
		tfree = (struct texture_node *)malloc(sizeof(struct texture_node) * ALLOC_NUM);
		if(tfree)
		{
			tn = tfree;
			for(i = 0; i < ALLOC_NUM; i++)
			{
				if(!i)
				{
					tn->prev = (struct texture_node *)0;
					tn->next = (tn+1);
				}
				else if(i == (ALLOC_NUM-1))
				{
					tn->prev = (tn-1);
					tn->next = (struct texture_node *)0;
				}
				else
				{
					tn->prev = (tn-1);
					tn->next = (tn+1);
				}
				strcpy((char *)tn->texture_name, "NO TEXTURE");
				tn->texture_flags = 0;
				tn->texture_count = 0;
				tn->texture_id = 0;
				tn = tn->next;
			}
		}
	}
	else
	{
		tfree = (struct texture_node *)malloc(sizeof(struct texture_node));
		if(tfree)
		{
			tn = tfree;
		 	tn->prev = (struct texture_node *)0;
		 	tn->next = (struct texture_node *)0;
		 	strcpy((char *)tn->texture_name, "NO TEXTURE");
		 	tn->texture_flags = 0;
		 	tn->texture_count = 0;
		 	tn->texture_id = 0;
		}
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done");
}

/*
** find_texture() - This function is used to search the active texture list
** for a texture based on the texture name and texture id.
*/


struct texture_node *find_texture(char *name, short id)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%s, %hd)\n", name, id);
	tn = tlist;
	while(tn)
	{
		if(tn->texture_id == id)
		{
			if(!strnicmp((char *)tn->texture_name, name, sizeof(tn->texture_name)))
			{
				TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "texture found - node: %p\n", tn);
				return(tn);
			}
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "No texture found\n");
	return((struct texture_node *)0);
}


/*
** create_texture() - This function is used to create a texture node on the
** active texture list.
*/
struct texture_node *create_texture(char *name, int id, int flags, int cflags,
int sclamp, int tclamp, int minfilt, int maxfilt)
{
	struct texture_node	*tn;
	char						*n;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%s, %d, %x, %x, %d, %d, %d, %d)\n", name, id, flags, cflags, sclamp, tclamp, minfilt, maxfilt);
//JMS
	if (texture_node_mode == add_texture_to_free)
	{
		if(!tfree)
		{
			add_texture_block();
		}
		if(!tfree)
		{
			TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not allocate memory for block of texture nodes\n");
			return(tfree);
		}
	}
//JMS

	/* Are we requesting an new texture to be created */
	if(cflags == CREATE_NORMAL_TEXTURE)
	{
		n = name;
		while(*n)
		{
			if(*n >= 'a' && *n <= 'z')
			{
				*n &= ~0x20;
			}
			n++;
		}

		/* See if this texture already exists */
		tn = find_texture(name, (short)id);
		if(tn)
		{
			/* Just increment the texture nodes usage count if */
			/* the texture already exists in the active texture */
			/* list */
			if(!(tn->texture_flags & TEXTURE_LOCKED))
			{
				tn->texture_count++;
			}
			TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Returning node: %p\n", tn);
			return(tn);
		}
	}

//JMS
	if (texture_node_mode != add_texture_to_free)
	{
		if(!tfree)
		{
			add_texture_block();
		}
		if(!tfree)
		{
			TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not allocate memory for block of texture nodes\n");
			return(tfree);
		}
	}
//JMS

	// Is disk info installed and a texture already being loaded ?
	if(texture_load_in_progress)
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Multiple texture loads\n");
		return((struct texture_node *)0);
	}

	// Set flag saying texture load is in progress
	texture_load_in_progress = 1;

	tn = tfree;

	tn->texture_handle = guLoadTextureDirect(
		(const char *)name,
		&tn->texture_info,
		sclamp,
		tclamp,
		minfilt,
		maxfilt);
#ifndef VEGAS
	if(tn->texture_handle == GR_NULL_MIPMAP_HANDLE)
#else
	if(tn->texture_handle == NULL)
#endif
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 0, "Problem downloading texture to TMU\n");
		// Reset texture load in progress flag
		texture_load_in_progress = 0;
#ifdef TEXTURE_LOAD_DEBUG
		fprintf(stderr, "texture %s not loaded\n", name);
#endif
		return((struct texture_node *)0);
	}
#ifdef TEXTURE_LOAD_DEBUG
	fprintf(stderr, "texture %s loaded\n", name);
#endif


	/* Remove node from free list */
	tfree = tn->next;
	if(tfree)
	{
		tfree->prev = (struct texture_node *)0;
	}
	/* Add node to beginning of active list */
	tn->next = tlist;
	tn->prev = (struct texture_node *)0;
	if(tlist)
	{
		tlist->prev = tn;
	}
	tlist = tn;

	/* Initialize the data for the new texture node */
	tn->texture_id = id;
	tn->texture_flags = flags;
	strncpy((char *)tn->texture_name, name, sizeof(tn->texture_name));

	strn2upr(tn->texture_name, sizeof(tn->texture_name));

	tn->texture_count = 0;

	texture_load_in_progress = 0;

	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Returning node: %p\n", tn);
	return(tn);
}


/*
** create_dtexture() - This function is used to create a texture node on the
** active texture list.
*/
#if 0
struct texture_node *create_dtexture(char *name, int id, int flags, int cflags,
int sclamp, int tclamp, int minfilt, int maxfilt)
{
	struct texture_node	*tn;

	if(!tfree)
	{
		add_texture_block();
	}
	if(!tfree)
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not allocate memory for block of texture nodes\n");
		return(tfree);
	}

	/* Are we requesting an new texture to be created */
	if(cflags == CREATE_NORMAL_TEXTURE)
	{
		/* See if this texture already exists */
		tn = find_texture(name, (short)id);
		if(tn)
		{
			/* Just increment the texture nodes usage count if */
			/* the texture already exists in the active texture */
			/* list */
			tn->texture_count++;
			TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Returning node: %p\n", tn);
			return(tn);
		}
	}

	tn = tfree;

	if(gu3dfGetInfo(name, &tn->texture_info))
	{
		if(!gu3dfLoad(name, &tn->texture_info))
		{
			TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not load texture\n");
			return((struct texture_node *)0);
		}
		tn->texture_handle = guTexAllocateMemory(
			0,
			GR_MIPMAPLEVELMASK_BOTH,
			tn->texture_info.header.width,
			tn->texture_info.header.height,
			tn->texture_info.header.format,
			GR_MIPMAP_NEAREST_DITHER,
			tn->texture_info.header.small_lod,
			tn->texture_info.header.large_lod,
			tn->texture_info.header.aspect_ratio,
			sclamp,
			tclamp,
			minfilt,
			maxfilt,
			0.0F,
			FXFALSE);
		if(tn->texture_handle == GR_NULL_MIPMAP_HANDLE)
		{
			TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not allocate TMU memory for texture\n");
			return((struct texture_node *)0);
		}
		guTexDownloadMipMap(tn->texture_handle, tn->texture_info.data,
			&tn->texture_info.table.nccTable);
	}
	else
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not get information for texture\n");
		return((struct texture_node *)0);
	}

	/* Remove node from free list */
	tfree = tn->next;
	if(tfree)
	{
		tfree->prev = (struct texture_node *)0;
	}

	/* Add node to beginning of active list */
	tn->next = tlist;
	tn->prev = (struct texture_node *)0;
	if(tlist)
	{
		tlist->prev = tn;
	}
	tlist = tn;

	/* Initialize the data for the new texture node */
	tn->texture_id = id;
	tn->texture_flags = flags;
	strncpy((char *)tn->texture_name, name, sizeof(tn->texture_name));
	strn2upr(tn->texture_name, sizeof(tn->texture_name));
	tn->texture_count = 1;
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Returning node: %p\n", tn);
	return(tn);
}
#endif


/* JBJ */
/* JBJ  */
/*
** texture_to_mem() - This function is used to create a texture node on the
** active texture list.
*/
struct texture_node *texture_to_mem(char *name, int id, int flags, int cflags)
{
	struct texture_node	*tn;


	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%s, %d, %x, %x)\n", name, id, flags, cflags);
	/* Are we requesting an new texture to be created */
//	if(cflags == CREATE_NORMAL_TEXTURE)
//	{
//	/* See if this texture is already in memory */
//		tn = find_texture(name, id);
//		if(tn)
//		{
//			/* Just increment the texture nodes usage count if */
//			/* the texture already exists in the active texture */
//			/* list */
//			tn->texture_count++;
//			return(tn);
//		}
//	}

	tn = tfree;

	if(gu3dfGetInfo(name, &tn->texture_info))
	{
		// allocate memory for incoming texture
		tn->texture_info.data = (void *)malloc(tn->texture_info.mem_required);
		if(!tn->texture_info.data)
		{
			TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not allocate memory to load texture\n");
			return((struct texture_node *)0);
		}
	
		// load texture from hard disk into memory	
		if(!gu3dfLoad(name, &tn->texture_info))
		{
			TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not load texture\n");
			return((struct texture_node *)0);
		}
	}	
	else
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not get information for texture\n");
		return((struct texture_node *)0);
	}	
	
	/* Remove node from free list */
	tfree = tn->next;
	if(tfree)
	{
		tfree->prev = (struct texture_node *)0;
	}

	/* Add node to beginning of active list */
	tn->next = tlist;
	tn->prev = (struct texture_node *)0;
	if(tlist)
	{
		tlist->prev = tn;
	}
	tlist = tn;

	/* Initialize the data for the new texture node */
	tn->texture_id = id;
	tn->texture_flags = flags;
	strncpy((char *)tn->texture_name, name, sizeof(tn->texture_name));
	strn2upr(tn->texture_name, sizeof(tn->texture_name));
	tn->texture_count = 1;
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Returning node: %p\n", tn);
	return(tn);
}

/* JBJ */
/* JBJ */




/*
** delete_texture() - This function is used to delete textures from the active
** texture list that are NOT locked and whos usage count is 1.
*/
void delete_texture(struct texture_node *tn)
{
	int				is_valid;
	texture_node_t	*t_list;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tn);
	/* If someone is trying to delete a locked texture - don't let */
	/* him do it. */
	if(tn->texture_flags & TEXTURE_LOCKED)
	{
		TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Texture is locked\n");
		return;
	}

	// Check to make sure the texture is on the active texture list
	is_valid = 0;
	t_list = tlist;
	while(t_list)
	{
		if((int)t_list == (int)tn)
		{
			is_valid = 1;
			break;
		}
		t_list = t_list->next;
	}

	// Was it on the list of valid textures ?
	if(!is_valid)
	{				   
		// NOPE - Return
		TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "Attempt to delete non-existant texture node\n");
		return;
	}

	/* Decrement the usage count */
	tn->texture_count--;

	/* Check the texture usage count */
	if(tn->texture_count <= 0)
	{
		/* Remove this texture from the active texture list */
		if((int)tn == (int)tlist)
		{

			/* Remove first node from active texture list */
			tlist = tn->next;
			if(tlist)
			{
				tlist->prev = (struct texture_node *)0;
			}
		}
		else if(!tn->next)
		{
			/* Remove last node from active texture list */
			tn->prev->next = (struct texture_node *)0;
		}
		else
		{
			/* Remove a node from in the active list */
			tn->prev->next = tn->next;
			tn->next->prev = tn->prev;
		}

//JMS
		/* Finally - free up the texture handle */
		grFreeTexture(tn->texture_handle);

		if (texture_node_mode == add_texture_to_free)
		{
			/* Add the node to the front of the free list */
			tn->next = tfree;
			tn->prev = (struct texture_node *)0;
			if(tfree)
			{
				tfree->prev = tn;
			}
			tfree = tn;
		}
		else
		{
			free(tn);
		}

//JMS
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done");
}

/*
** delete_texture_id() - This function is used to delete a texture from the
** active texture list based on its texture id.  Only textures that are NOT
** locked and whos usage count is 1 are actually taken off of the active
** texture list.  This function deletes only the first matching texture it
** finds.
*/
void delete_texture_id(int id)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", id);
	tn = tlist;
	while(tn)
	{
		if(id == tn->texture_id)
		{
			delete_texture(tn);
			TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Texture %p deleted\n", tn);
			return;
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Could not find texture\n");
}

/*
** force_delete_multiple_textures() - This function is used to delete multiple
** textures from the active texture list based on a masked version of the
** texture node id.
*/
void force_delete_multiple_textures(int id, int mask)
{
	struct texture_node	*tn;
	struct texture_node	*ts;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %d)\n", id, mask);
	tn = tlist;
	while(tn)
	{
		ts = tn->next;
		if((tn->texture_id & mask) == id)
		{
			tn->texture_count = 1;
			delete_texture(tn);
		}
		tn = ts;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}


/*
** delete_multiple_textures() - This function is used to delete multiple
** textures from the active texture list based on a masked version of the
** texture node id.  Only textures that are NOT locked and whos usage count
** is 1 are actually delete from the list.
*/
void delete_multiple_textures(int id, int mask)
{
	struct texture_node	*tn;
	struct texture_node	*ts;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %d)\n", id, mask);
	tn = tlist;
	while(tn)
	{
		ts = tn->next;
		if((tn->texture_id & mask) == id)
		{
			delete_texture(tn);
		}
		tn = ts;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** clear_all_textures() - This function uncontionally clears ALL textures from
** the active texture list and moves them to the free texture list.
*/
static void clear_all_textures(void)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	/* If no nodes on active list - just return */
	if(!tlist)
	{
		TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "No textures on active texture list\n");
		return;
	}

	/* Walk the active texture list and set all usage counts to 0 */
	tn = tlist;
	while(tn)
	{
		tn->texture_count = 0;
		tn = tn->next;
	}

	/* If there are texture nodes on the free list we link the */
	/* end of the active list to the beginning of the free list */
	if(tfree)
	{
		/* Go to last node on active list */
		tn = tlist;
		while(tn->next)
		{
			tn = tn->next;
		}
		/* Link free list to end of active list */
		tn->next = tfree;
		tfree->prev = tn;
	}

	/* Free list now points at active list */
	tfree = tlist;

	/* Set no nodes on active texture list */
	tlist = (struct texture_node *)0;

	// Clear out the textures from the the TMU too
	guTexMemReset();
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** clear_unlocked_textures() - This function clears all textures from the
** active texture list that are NOT locked.  This function deletes the
** texture regardless of the textures usage count.
*/
static void clear_unlocked_textures(void)
{
	struct texture_node	*tn;
	struct texture_node	*ts;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	tn = tlist;
	while(tn)
	{
		ts = tn->next;
		if(!(tn->texture_flags & TEXTURE_LOCKED))
		{
			tn->texture_count = 1;
			delete_texture(tn);
		}
		tn = ts;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** clear_locked_textures() - This functions clears all LOCKED textures from
** the active texture list.  This function deletes the texture regardless
** of the textures usage count.
*/
static void clear_locked_textures(void)
{
	struct texture_node	*tn;
	struct texture_node	*ts;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	tn = tlist;
	while(tn)
	{
		ts = tn->next;
		if((tn->texture_flags & TEXTURE_LOCKED))
		{
			tn->texture_count = 1;
			tn->texture_flags &= ~TEXTURE_LOCKED;
			delete_texture(tn);
		}
		tn = ts;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** clear_texture_list() - This function is used to clear the texture lists.
*/
void clear_texture_list(int flag)
{
	void	(*c_func)(void);

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", flag);
	switch(flag)
	{
		case TEXTURES_ALL:
		{
			c_func = clear_all_textures;
			break;
		}
		case TEXTURES_LOCKED:
		{
			c_func = clear_locked_textures;
			break;
		}
		default:
		{
			c_func = clear_unlocked_textures;
			break;
		}
	}
	c_func();
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** lock_texture() - This function is used to lock a texture into the active
** texture list.
*/
void lock_texture(struct texture_node *tn)
{
	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tn);
	tn->texture_flags |= TEXTURE_LOCKED;
	TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture locked\n");
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** lock_texture_id() - This function is used to lock a texture into the active
** texture list using the texture node id.  This function locks the first
** first texture in the list matching the id passed.
*/
void lock_texture_id(int id)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", id);
	tn = tlist;
	while(tn)
	{
		if(tn->texture_id == id)
		{
			tn->texture_flags |= TEXTURE_LOCKED;
			TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture locked\n");
			return;
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** lock_multiple_textures() - This function is used to lock a series of textures
** into the active texture list using a masked version of the texture node id
*/
void lock_multiple_textures(int id, int mask)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %d)\n", id, mask);
	tn = tlist;
	while(tn)
	{
		if((tn->texture_id & mask) == id)
		{
			TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture id: %d locked\n", id);
			tn->texture_flags |= TEXTURE_LOCKED;
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** lock_textures_all() - This function is used to lock ALL textures into the
** active texture list.
*/
void lock_textures_all(void)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	tn = tlist;
	while(tn)
	{
		TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture %p locked\n", tn);
		tn->texture_flags |= TEXTURE_LOCKED;
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** unlock_texture() - This function is used to unlock a texture from the active
** texture list.
*/
void unlock_texture(struct texture_node *tn)
{
	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tn);
	tn->texture_flags &= ~TEXTURE_LOCKED;
	TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture %p unlocked\n", tn);
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** unlock_texture_id() - This function is used to unlock a texture from the
** active texture list based on the texture node id.  This function unlocks
** the first texture in the list that matches the id and is locked.
*/
void unlock_texture_id(int id)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", id);
	tn = tlist;
	while(tn)
	{
		if(tn->texture_id == id)
		{
			if(tn->texture_flags & TEXTURE_LOCKED)
			{
				TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture %d unlocked\n", id);
				tn->texture_flags &= ~TEXTURE_LOCKED;
				TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
				return;
			}
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** unlock_multiple_textures() - This function is used to unlock multiple
** textures from the active texture list based on a masked version of the
** texture node id.
*/
void unlock_multiple_textures(int id, int mask)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %d)\n", id, mask);
	tn = tlist;
	while(tn)
	{
		if((tn->texture_id & mask) == id)
		{
			if(tn->texture_flags & TEXTURE_LOCKED)
			{
				TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture %d unlocked\n", tn->texture_id);
				tn->texture_flags &= ~TEXTURE_LOCKED;
			}
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** unlock_all_textures() - This function is used to unlock ALL textures from
** the active texture list.
*/
void unlock_all_textures(void)
{
	struct texture_node	*tn;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	tn = tlist;
	while(tn)
	{
		if(tn->texture_flags & TEXTURE_LOCKED)
		{
			TEXTURE_DBG_MSG(DEBUG_MSGS, 0, "texture %p unlocked\n", tn);
			tn->texture_flags &= ~TEXTURE_LOCKED;
		}
		tn = tn->next;
	}
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

/*
** create_texture_from_memory() - This function is used to create a texture
** node on the active texture list from a texture already loaded in memory
*/
struct texture_node *create_texture_from_memory(char *name, void *buf, int id, int flags, int cflags,
int sclamp, int tclamp, int minfilt, int maxfilt)
{
	struct texture_node	*tn;
	char						*n;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%s, %p, %d, %d, %d, %d, %d, %d, %d)\n", name, buf, id, flags, cflags, sclamp, tclamp, minfilt, maxfilt);
	if(!tfree)
	{
		add_texture_block();
	}
	if(!tfree)
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Could not allocate memory for block of texture nodes\n");
		return(tfree);
	}

	/* Are we requesting an new texture to be created */
	if(cflags == CREATE_NORMAL_TEXTURE)
	{
		n = name;
		while(*n)
		{
			if(*n >= 'a' && *n <= 'z')
			{
				*n &= ~0x20;
			}
			n++;
		}

		/* See if this texture already exists */
		tn = find_texture(name, (short)id);
		if(tn)
		{
			/* Just increment the texture nodes usage count if */
			/* the texture already exists in the active texture */
			/* list */
			if(!(tn->texture_flags & TEXTURE_LOCKED))
			{
				tn->texture_count++;
			}
			TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "Returning: %p\n", tn);
			return(tn);
		}
	}

	tn = tfree;

	tn->texture_handle = guLoadTextureDirectFromMemory(
		buf,
		&tn->texture_info,
		sclamp,
		tclamp,
		minfilt,
		maxfilt);
#ifndef VEGAS
	if(tn->texture_handle == GR_NULL_MIPMAP_HANDLE)
#else
	if(tn->texture_handle == NULL)
#endif
	{
		TEXTURE_DBG_MSG(DEBUG_FAILS, 1, "Problem downloading memory texture to TMU\n");
		return((struct texture_node *)0);
	}

	/* Remove node from free list */
	tfree = tn->next;
	if(tfree)
	{
		tfree->prev = (struct texture_node *)0;
	}

	/* Add node to beginning of active list */
	tn->next = tlist;
	tn->prev = (struct texture_node *)0;
	if(tlist)
	{
		tlist->prev = tn;
	}
	tlist = tn;

	/* Initialize the data for the new texture node */
	tn->texture_id = id;
	tn->texture_flags = flags;
	strncpy((char *)tn->texture_name, name, sizeof(tn->texture_name));
	strn2upr(tn->texture_name, sizeof(tn->texture_name));
	tn->texture_count = 0;
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "returning: %p\n", tn);
	return(tn);
}


//
// clear_free_texture_list() - Function used to clear out the free texture list
//
void clear_free_texture_list(void)
{
	register struct texture_node	*t;
	register struct texture_node	*next;

	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	// Get pointer to free list
	t = tfree;

	while(t)
	{
		// Save pointer to next node
		next = t->next;

		// Free memory used by the node
		free(t);

		// Get next node
		t = next;
	}

	// Set the new free list pointer
	tfree = (struct texture_node *)0;
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

//
// clear_free_lists() - Function used to clear all of the free lists
//
void clear_free_lists(void)
{
	TEXTURE_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	clear_free_process_list();
	clear_free_sprite_list();
	clear_free_object_list();
	clear_free_texture_list();
	TEXTURE_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}
