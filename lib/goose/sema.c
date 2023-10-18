/****************************************************************************/
/*                                                                          */
/* sema.c - Source code for semaphore support functions.                    */
/*                                                                          */
/* Written by:  Michael J. Lynch (Midway Video Inc.)                        */
/*                                                                          */
/* Copyright (c) 1997 by Midway Video Inc.                                  */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 2 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<goose/process.h>
#include	<goose/sema.h>

char	goose_sema_c_version[] = {"$Revision: 2 $"};

extern process_node_t	*cur_proc;

//
// init_semaphore() - This function is used to initialize a semaphore
//
void init_semaphore(semaphore_t *sema)
{
	sema->proc = cur_proc;
}

//
// wait_semaphore() - This function is used to wait on a semaphore
//
void wait_semaphore(void)
{
	suspend_self();
}

//
// send_semaphore() - This function is used to send a signal to a semaphore
//
void send_semaphore(semaphore_t *sema)
{
	resume_process(sema->proc);
}
