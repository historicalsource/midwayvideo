/*****************************************************************************/
/*                                                                           */
/* process.h - Header file for process system.                               */
/*                                                                           */
/* Written by:  Michael J. Lynch                                             */
/* Version:     1.00                                                         */
/* Date:        6/28/95                                                      */
/*                                                                           */
/* Copyright (c) 1995 by Williams Electronics Games Inc.                     */
/* All Rights Reserved                                                       */
/*                                                                           */
/* Use, duplication, or disclosure is strictly forbidden unless approved     */
/* in writing by Williams Electronics Games Inc.                             */
/*                                                                           */
/*****************************************************************************/
#ifndef	__PROCESS_H__
#define	__PROCESS_H__

#ifdef VERSIONS
char	goose_process_h_version[] = {"$Revision: 6 $"};
#endif

/* Values for pflags */
#define PF_EXIT_AND_DIE         0x0001      /* Call exit function and die */
#define PF_SUSPENDED            0x0002      /* Process is suspended */
#define PF_HAS_EXIT_FUNCTION    0x0004      /* Process has exit function */

/* compile options */
#define SAVE_FPREG

/* die causes */
#define PD_DIE          0x0000
#define PD_RETURN       0x0001
#define PD_WIPEOUT      0x0002

#define DEFAULT_PDATA_SIZE  2048

typedef struct process_node
{
	struct process_node	*next;
	struct process_node	*prev;
	void						*process_stack_ptr;
	short						process_sleep_time;
	short						process_flags;
	char						*process_data;
	void						(*process_exit)(struct process_node *, int);
	int						process_id;
	char						process_name[8];
	void                (*entry_func)();
#if defined(PROFILE) || defined(VEGAS)
	int						time;
#else
	int						pad;
#endif
	int						run_level;
} process_node_t;

struct process_node *create_process(char *, int, int *, void (*)(), int);
struct process_node *create_process_immediate(char *, int, int *, void (*)(), int);
void delete_process(struct process_node *);
void delete_process_id(int);
void delete_multiple_processs(int, int);
void clear_process_list(void);
struct process_node *qcreate(char *, int, void (*)(), int, int, int, int);
struct process_node *iqcreate(char *, int, void (*)(), int, int, int, int);
void sleep_process(struct process_node *, short);
void wakeup_process(struct process_node *);
void suspend_self(void);
void suspend_process(struct process_node *);
void suspend_all(void);
void resume_process(struct process_node *);
void resume_all(void);
void suspend_multiple_processes(int id, int mask);
void resume_multiple_processes(int id, int mask);
struct process_node *existp(int, int);
void kill(struct process_node *, int);
void killall(int, int);
void set_pid(int);
void *get_pdata(void);
void set_run_level(int);
void set_process_run_level(process_node_t *, int);
void do_process_sort(void);
void sleep(int);
void die(int);
void disk_queue_hook(void);
void add_sleep_process_id(int, short);
void wakeup_process_id(int);
void print_processes(void);

//#define	lockup()	__asm__("	break	0x407")

// Or this into a processes id to make it indestructable by killall()
#define NODESTRUCT      0x8000

/* END mproc.h ***************************************************************/

#endif
