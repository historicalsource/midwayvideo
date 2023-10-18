/******************************************************************************/
/*                                                                            */
/* object.h - Object header file                                             */
/*                                                                            */
/* Written by:  Michael J. Lynch                                              */
/* Version:     1.00                                                          */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
#ifndef	__OBJECT_H__
#define	__OBJECT_H__

#ifdef VERSIONS
char	goose_object_h_version[] = {"$Revision: 5 $"};
#endif

#ifndef OBJECT_DEFINED

struct object_node {
	struct object_node	*next;
	struct object_node	*prev;
	char			object_name[12];
	short			object_id;
	short			object_flags;
	int				draw_order;
	void			(*object_draw_func)(void *);
	void			*object_data;
#if defined(PROFILE) || defined(VEGAS)
	int			time;
#endif
};


struct object_node *create_object(char *, int, int, int, void *, void (*)(void *));
void delete_object(struct object_node *);
void delete_object_id(int);
void delete_multiple_objects(int, int);
void hide_multiple_objects( int, int );
void unhide_multiple_objects( int, int );
void clear_object_list(void);
void draw_3d_objects(void);
void hide_all_3d_objects(void);
void unhide_all_3d_objects(void);
int object_exists( struct object_node * );

// 3D object flags
#define OF_NONE				0
#define OF_HIDDEN			0x1

#define OBJECT_DEFINED
#endif
#endif
