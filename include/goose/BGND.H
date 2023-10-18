/****************************************************************************/
/*                                                                          */
/* bgnd.h - Header file for background list handler functions               */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 2 $                                                             */
/*                                                                          */
/****************************************************************************/
#ifndef	__BGND_H__
#define	__BGND_H__

#ifdef VERSIONS
char	goose_bgnd_h_version[] = {"$Revision: 2 $"};
#endif

// Function prototypes
void generate_background_sprite_verts(register sprite_info_t *);
sprite_info_t *beginbobj(image_info_t *, float, float, float, int);
void delbobj(sprite_info_t *);
void delb1c(int, int);
void delete_all_background_sprites(void);
void draw_background(void);
void set_bgnd_color(int);

#endif
