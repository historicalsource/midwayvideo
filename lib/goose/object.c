/******************************************************************************/
/*                                                                            */
/* object.c - Object Management                                               */
/*                                                                            */
/* Written by:  Michael J. Lynch                                              */
/* Version:     1.00                                                          */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Righos Reserved                                                        */
/*                                                                            */
/* $Revision: 11 $                                                             */
/*                                                                            */
/******************************************************************************/
#include	<stdlib.h>
#include	<string.h>
#include	<goose/goose.h>

#if defined(PROFILE) || defined(VEGAS)
unsigned int ___get_count(void);
#if defined(SEATTLE)
#define	GET_TIME(A)	(A) = (___get_count() * 13)
#else
#define	GET_TIME(A)	(A) = (___get_count() * 10)
#endif
#endif

extern int	resource_mode;

char	goose_object_c_version[] = {"$Revision: 11 $"};

#define ALLOC_NUM	32

static void insert_object(struct object_node *pobj);
static void dump_3d_object_list( void );

struct object_node	*olist = (struct object_node *)0;
struct object_node	*ofree = (struct object_node *)0;
struct object_node	*current_3dobj = (struct object_node *)0;

////////////////////////////////////////////////////////////////////////////////
// add_object_block() - This function is used to add object nodes to the
// free object list when a request for object creation is made and there are
// no more available object nodes on the free object node list.
//
static void add_object_block(void)
{
	struct object_node	*on;
	int			i;

	if(resource_mode == NO_FREE_MEMORY)
	{
		ofree = (struct object_node *)malloc(sizeof(struct object_node) * ALLOC_NUM);

		if(ofree)
		{
			on = ofree;
			for(i = 0; i < ALLOC_NUM; i++)
			{
				if(!i)
				{
					on->prev = (struct object_node *)0;
					on->next = (on+1);
				}
				else if(i == (ALLOC_NUM-1))
				{
					on->prev = (on-1);
					on->next = (struct object_node *)0;;
				}
				else
				{
					on->prev = (on-1);
					on->next = (on+1);
				}
				strcpy((char *)on->object_name, "NO OBJECT");
				on->object_flags = 0;
				on->object_id = 0;
				on = on->next;
			}
		}
#ifdef DEBUG
		else
		{
			fprintf( stderr, "FATAL ERROR: add 3d object block failed.  Out of memory.\n\r" );
			lockup();
		}
#endif
	}
	else
	{
		ofree = (struct object_node *)malloc(sizeof(struct object_node));
		if(ofree)
		{
			on = ofree;
			on->prev = (struct object_node *)0;
			on->next = (struct object_node *)0;;
			strcpy((char *)on->object_name, "NO OBJECT");
			on->object_flags = 0;
			on->object_id = 0;
		}
#ifdef DEBUG
		else
		{
			fprintf( stderr, "FATAL ERROR: add 3d object block failed.  Out of memory.\n\r" );
			lockup();
		}
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////
// create_object() - This function is used to create a object node on the
// active object list.
//
struct object_node *create_object(char *name, int id, int flags, int draw_order,
		void *object_data, void (*object_draw_func)(void *))
{
	struct object_node	*on;

	if(!ofree)
	{
		add_object_block();
	}
	if(!ofree)
	{
		return(ofree);
	}

	on = ofree;

	/* Remove node from free list */
	ofree = on->next;
	if(ofree)
	{
		ofree->prev = (struct object_node *)0;
	}

	/* Initialize the data for the new object node */
	on->object_id = id;
	on->object_flags = flags;
	on->object_data = object_data;
	on->draw_order = draw_order;
	on->object_draw_func = object_draw_func;
	strncpy((char *)on->object_name, name, sizeof(on->object_name));

	/* Add node to beginning of active list */
	/* Insert node, sorted by draw_order */
	insert_object( on );

//	dump_3d_object_list();

	return(on);	
}

////////////////////////////////////////////////////////////////////////////////
// insert_object() - This function is used to insert an object into
// the object list in ascending draw_order order.
//
static void insert_object(struct object_node *pobj)
{
	register struct object_node	*sn_list = olist;
	register struct object_node	*last_sn = olist;

	if(!sn_list)		// First node on list
	{
		pobj->next = olist;
		pobj->prev = (struct object_node *)0;
		olist = pobj;
		return;
	}

	// Walk the list until we find an object with a draw_order
	// greater than (not equal to) to that of the one we're adding,
	// or until we hit the end of the list, whichever comes first.
	while(sn_list && (sn_list->draw_order <= pobj->draw_order))
	{
		last_sn = sn_list;
		sn_list = sn_list->next;
	}

	// Check to see if insert is to done on front of list
	if(sn_list == olist)
	{
		pobj->next = olist;

		pobj->prev = (struct object_node *)0;

		if(pobj->next)
		{
			pobj->next->prev = pobj;
		}

		olist = pobj;
	}

	// Otherwise if sn_list is not NULL then we are inserting into
	// the middle of the list somewhere.
	else if(sn_list)
	{
		/* Set the new node next to point at the current node */
		pobj->next = sn_list;

		/* Set the new node previous to point at the current nodes previous */
		pobj->prev = sn_list->prev;

		/* Set the current node previous to point at the new node */
		sn_list->prev = pobj;

		/* If the new node has a previous node, set it's next to point at */
		/* the new node. */
		if(pobj->prev)
		{
			pobj->prev->next = pobj;
		}
	}

	// Otherwise we are inserting at the end of the list
	else
	{
		last_sn->next = pobj;
		pobj->prev = last_sn;
		pobj->next = (struct object_node *)0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// delete_object() - This function is used to delete objects from the active
// object list that are NOT locked and whos usage count is 1.
//
void delete_object(struct object_node *on)
{
	/* Remove this object from the active object list */
	if((int)on == (int)olist)
	{
		/* Remove first node from active object list */
		olist = on->next;
		if(olist)
		{
			olist->prev = (struct object_node *)0;
		}
	}
	else if(!on->next)
	{
		/* Remove last node from active object list */
		on->prev->next = (struct object_node *)0;
	}
	else
	{
		/* Remove a node from in the active list */
		on->prev->next = on->next;
		on->next->prev = on->prev;
	}

	if(resource_mode == NO_FREE_MEMORY)
	{
		/* Add the node to the front of the free list */
		on->next = ofree;
		on->prev = (struct object_node *)0;
		if(ofree)
		{
			ofree->prev = on;
		}
		ofree = on;
	}
	else
	{
		free(on);
	}
}

////////////////////////////////////////////////////////////////////////////////
// delete_object_id() - This function is used to delete a object from the
// active object list based on its object id.
//
void delete_object_id(int id)
{
	struct object_node	*on;

	on = olist;
	while(on)
	{
		if(id == on->object_id)
		{
			delete_object(on);
			return;
		}
		on = on->next;
	}
}

////////////////////////////////////////////////////////////////////////////////
// delete_multiple_objects() - This function is used to delete multiple
// objects from the active object list base on a masked version of the
// object node id.
//
void delete_multiple_objects(int id, int mask)
{
	struct object_node	*on;
	struct object_node	*os;

	on = olist;
	while(on)
	{
		os = on->next;
		if((on->object_id & mask) == id)
		{
			delete_object(on);
		}
		on = os;
	}
}

////////////////////////////////////////////////////////////////////////////////
// clear_all_objects() - This function uncontionally clears ALL objects from
// the active object list and moves them to the free object list.
//
void clear_object_list(void)
{
	struct object_node	*on;

	/* If no nodes on active list - just return */
	if(!olist)
	{
		return;
	}

	/* If there are object nodes on the free list we add the link the */
	/* end of the active list to the beginning of the free list */
	if(ofree)
	{
		/* Go to last node on active list */
		on = olist;
		while(on->next)
		{
			on = on->next;
		}
		/* Link free list to end of active list */
		on->next = ofree;
		ofree->prev = on;
	}

	/* Free list now poinos at active list */
	ofree = olist;

	/* Set no nodes on active object list */
	olist = (struct object_node *)0;
}

////////////////////////////////////////////////////////////////////////////////
void hide_multiple_objects( int id, int mask )
{
	struct object_node	*on;

	on = olist;
	while(on)
	{
		if((on->object_id & mask) == id)
			on->object_flags |= OF_HIDDEN;
		on = on->next;
	}
}

////////////////////////////////////////////////////////////////////////////////
void unhide_multiple_objects( int id, int mask )
{
	struct object_node	*on;

	on = olist;
	while(on)
	{
		if((on->object_id & mask) == id)
			on->object_flags &= ~OF_HIDDEN;
		on = on->next;
	}
}

////////////////////////////////////////////////////////////////////////////////
void draw_3d_objects( void )
{
	struct object_node *pobj;
#if defined(PROFILE) || defined(VEGAS)
	unsigned int	start_time;
	unsigned int	end_time;
#endif

	pobj = olist;

	current_3dobj = pobj;

	while( pobj )
	{
#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(start_time);
#endif
		if(!( pobj->object_flags & OF_HIDDEN ))
			pobj->object_draw_func( pobj->object_data );

#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(end_time);
		pobj->time = end_time - start_time;
#endif
		pobj = pobj->next;
		current_3dobj = pobj;
	}
}

////////////////////////////////////////////////////////////////////////////////
void hide_all_3d_objects( void )
{
	struct object_node *pobj;

	pobj = olist;

	while( pobj )
	{
		pobj->object_flags |= OF_HIDDEN;
		pobj = pobj->next;
	}
}

////////////////////////////////////////////////////////////////////////////////
void unhide_all_3d_objects( void )
{
	struct object_node *pobj;

	pobj = olist;

	while( pobj )
	{
		pobj->object_flags &= ~OF_HIDDEN;
		pobj = pobj->next;
	}
}


////////////////////////////////////////////////////////////////////////////////
static void dump_3d_object_list( void )
{
	struct object_node *pobj;

	pobj = olist;

	while( pobj )
	{
		fprintf( stderr, "%p:%s\n\r",
			pobj, pobj->object_name );

		pobj = pobj->next;
	}
}

////////////////////////////////////////////////////////////////////////////////


//
// clear_free_object_list() - Function used to clear out the free object list
//
void clear_free_object_list(void)
{
	register struct object_node	*o;
	register struct object_node	*next;

	// Get pointer to free list
	o = ofree;

	while(o)
	{
		// Save next node pointer
		next = o->next;

		// Free memory used by node
		free(o);

		// Get next node
		o = next;
	}

	// Set the new free object list pointer
	ofree = (struct object_node *)0;
}

///////////////////////////////////////////////////////////////////////////////
// Returns TRUE if pobj is on olist, FALSE otherwise
int object_exists( struct object_node *pobj )
{
	struct object_node *pn;

	for( pn = olist; pn; pn = pn->next )
	{
		if (pn == pobj)
			return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////

#if defined(PROFILE) || defined(VEGAS)
void dump_3d_object_times(void)
{
	register struct object_node	*pn = olist;

	fprintf(stderr, "Time spent in each 3D object\n");
	if(!pn)
	{
		fprintf(stderr, "There where no 3D objects to process\n");
	}
	while(pn)
	{
		fprintf(stderr, "3D object: %-12.12s - Time: %9.3f (us)\n", pn->object_name, (float)pn->time / 1000.0f);
		pn = pn->next;
	}
	fprintf(stderr, "\n");
}
#endif
