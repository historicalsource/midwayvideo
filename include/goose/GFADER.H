/****************************************************************************/
/*                                                                          */
/* gfader.h - Gamma Table Fader Include File                                */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 5 $                                                             */
/*                                                                          */
/****************************************************************************/
#ifndef	__GFADER_H__
#define	__GFADER_H__

#ifdef VERSIONS
char	goose_gfader_h_version[] = {"$Revision: 5 $"};
#endif

// Function prototypes
void load_gamma_table(void);

void fade(float, int, void (*)(void));
int  fade_at_full(void);		/* returns 0 if intensity at full ; otherwise, 1 */
int fade_complete(void);		/* Returns TRUE (1) if the fade is complete, or FALSE (0) if it's still in progress */

void black_screen(void);
void normal_screen(void);
void specific_gamma_load(float,float);

#endif
