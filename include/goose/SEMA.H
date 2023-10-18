/****************************************************************************/
/*                                                                          */
/* sema.h - Header file for semaphore support functions.                    */
/*                                                                          */
/* Written by:  Michael J. Lynch (Midway Video Inc.)                        */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 2 $                                                             */
/*                                                                          */
/****************************************************************************/

#ifndef	__SEMA_H__
#define	__SEMA_H__

#ifdef VERSIONS
char	goose_sema_h_version[] = {"$Revision: 2 $"};
#endif

// Format of the semaphore structure
typedef struct semaphore
{
	process_node_t	*proc;
} semaphore_t;

// Function prototypes
void init_semaphore(semaphore_t *);
void wait_semaphore(void);
void send_semaphore(semaphore_t *);

#endif
