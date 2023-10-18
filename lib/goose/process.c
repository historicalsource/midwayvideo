/******************************************************************************/
/*                                                                            */
/* process.c - Object Management                                              */
/*                                                                            */
/* Written by:  Michael J. Lynch                                              */
/* Version:     1.00                                                          */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Righos Reserved                                                        */
/*                                                                            */
/******************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<goose/goose.h>

char	goose_process_c_version[] = {"$Revision: 19 $"};

void	_proc_shell(int *);

#define ALLOC_NUM	16

struct process_node	*plist = (struct process_node *)0;
struct process_node	*pfree = (struct process_node *)0;
struct process_node	*cur_proc = (struct process_node *)0;
static int				sort_processes = 0;

int						resource_mode = NO_FREE_MEMORY;

void set_resource_control(int mode)
{
	resource_mode = mode;
	if(resource_mode == NO_FREE_MEMORY)
	{
		set_texture_free_mode(0);
	}
	else
	{
		set_texture_free_mode(1);
	}
}

void reset_process_lists(void)
{
	plist = pfree = cur_proc = (struct process_node *)0;
}

/*
** add_process_block() - This function is used to add process nodes to the
** free process list when a request for process creation is made and there are
** no more available process nodes on the free process node list.
*/
static void add_process_block(void)
{
	struct process_node	*pn;
	struct process_node	*last;
	int			i;

	if(resource_mode == NO_FREE_MEMORY)
	{
		for(i = 0; i < ALLOC_NUM; i++)
		{
			// Allocate memory for a process node
			pn = (struct process_node *)malloc(sizeof(struct process_node));

			// Did we get memory ?
			if(!pn)
			{
				// NOPE - Done
				return;
			}

			// First node ?
			if(!i)
			{
				// YES - set list pointer
				pfree = pn;

				// Set node next link
				pn->next = (struct process_node *)0;

				// Set node last link
				pn->prev = (struct process_node *)0;
			}

			// Last node ?
			else if(i == (ALLOC_NUM-1))
			{
				// Terminate list
				pn->next = (struct process_node *)0;

				// Back link to last node
				pn->prev = last;

				// Forward link last node to this node
				pn->prev->next = pn;
			}

			// Node in middle
			else
			{
				// Back link to last node
				pn->prev = last;

				// Terminate list
				pn->next = (struct process_node *)0;

				// Forward link last node to this node
				pn->prev->next = pn;
			}

			// Initialize nodes fields
			strcpy((char *)pn->process_name, "FREEPROC");
			pn->process_flags = 0;
			pn->process_id = 0;
			pn->process_data = (void *)0;

			// Set last node pointer
			last = pn;
		}
	}
	else
	{
		pfree = (struct process_node *)malloc(sizeof(struct process_node));
		if(pfree)
		{
			pn = pfree;
			pn->prev = (struct process_node *)0;
			pn->next = (struct process_node *)0;;
			strcpy((char *)pn->process_name, "FREEPROC");
			pn->process_flags = 0;
			pn->process_id = 0;
			pn = pn->next;
		}
	}
}

/*
** create_process() - This function is used to create a process node on the
** active process list.
*/
struct process_node *create_process(char *name, int id, int *args, void (*entry_func)(), int astack)
{
	struct process_node	*pn;
	int			*pdata;

	if(!pfree)
	{
		add_process_block();
	}
	if(!pfree)
	{
		return(pfree);
	}

	pn = pfree;

#ifdef DEBUG
	if(astack & 7)
	{
		fprintf(stderr, "FATAL ERROR - Process stack size is NOT an even multiple of 8 bytes\r\n");
		lockup();
	}
#endif

//	if(!pn->process_data)
//	{
//		pn->process_data = malloc(DEFAULT_PDATA_SIZE + astack);
//	}
	pn->process_data = realloc(pn->process_data, DEFAULT_PDATA_SIZE + astack);
	if(!pn->process_data)
	{
#if defined(DEBUG)
		fprintf(stderr, "Could not allocate memory for process stack\r\n");
		lockup();
#endif
		return((process_node_t *)0);
	}

	/* Remove node from free list */
	pfree = pn->next;
	if(pfree)
	{
		pfree->prev = (struct process_node *)0;
	}

	/* Add node to beginning of active list */
	pn->next = plist;
	pn->prev = (struct process_node *)0;
	if(plist)
	{
		plist->prev = pn;
	}
	plist = pn;

	/* Initialize the data for the new process node */
	pn->process_sleep_time = 1;
	pn->process_flags = 0;
	pn->process_stack_ptr = &pn->process_data[DEFAULT_PDATA_SIZE+astack-8];
	pn->process_id = id;
	pdata = (int *)pn->process_stack_ptr;
	pdata -= 10;
	*pdata = (int)_proc_shell;
	pn->entry_func = entry_func;
	if(args)
	{
		pdata = (int *)pn->process_data;
		*pdata++ = *args++;
		*pdata++ = *args++;
		*pdata++ = *args++;
		*pdata++ = *args++;
	}
	(int)pn->process_exit = 0;
	strncpy((char *)pn->process_name, name, sizeof(pn->process_name));

	// Set the default process run level
	pn->run_level = 0;

	return(pn);
	
}

/*
** create_process_immediate() - This function is used to create a process
** node on the active process list immediately following the current process.
*/
struct process_node *create_process_immediate(char *name, int id, int *args, void (*entry_func)(), int astack)
{
	struct process_node	*pn;
	int			*pdata;

	if(!pfree)
	{
		add_process_block();
	}
	if(!pfree)
	{
		return(pfree);
	}

	pn = pfree;

//	if(!pn->process_data)
//	{
//		pn->process_data = malloc(DEFAULT_PDATA_SIZE + astack);
//	}
	pn->process_data = realloc(pn->process_data, DEFAULT_PDATA_SIZE + astack);
	if(!pn->process_data)
	{
#if defined(DEBUG)
		fprintf(stderr, "Could not allocate memory for process stack\r\n");
		lockup();
#endif
		return((process_node_t *)0);
	}

	/* Remove node from free list */
	pfree = pn->next;
	if(pfree)
	{
		pfree->prev = (struct process_node *)0;
	}

	/* Insert node into list after the current process */
	/* Link the new node previous to the current process */
	pn->prev = cur_proc;
	/* Link the new node next to the current next */
	pn->next = cur_proc->next;
	/* If there is a next node link it's previous to the new node */
	if(pn->next)
	{
		pn->next->prev = pn;
	}
	/* If the is a prev node link it's next to the new node */
	if(pn->prev)
	{
		pn->prev->next = pn;
	}

	/* Initialize the data for the new process node */
	pn->process_sleep_time = 1;
	pn->process_flags = 0;
	pn->process_stack_ptr = &pn->process_data[DEFAULT_PDATA_SIZE+astack-8];
	pn->process_id = id;
	pdata = (int *)pn->process_stack_ptr;
	pdata -= 10;
	*pdata = (int)_proc_shell;
	pn->entry_func = entry_func;
	if(args)
	{
		pdata = (int *)pn->process_data;
		*pdata++ = *args++;
		*pdata++ = *args++;
		*pdata++ = *args++;
		*pdata++ = *args++;
	}
	(int)pn->process_exit = 0;
	strncpy((char *)pn->process_name, name, sizeof(pn->process_name));

	// Set the default process run level
	pn->run_level = 0;

	return(pn);
	
}

/*
** delete_process() - This function is used to delete processs from the active
** process list that are NOT locked and whos usage count is 1.
*/
void delete_process(struct process_node *pn)
{
	if(pn == cur_proc)
	{
		cur_proc = cur_proc->prev;
	}

	/* Remove this process from the active process list */
	if((int)pn == (int)plist)
	{
		/* Remove first node from active process list */
		plist = pn->next;
		if(plist)
		{
			plist->prev = (struct process_node *)0;
		}
	}
	else if(!pn->next)
	{
		/* Remove last node from active process list */
		pn->prev->next = (struct process_node *)0;
	}
	else
	{
		/* Remove a node from in the active list */
		pn->prev->next = pn->next;
		pn->next->prev = pn->prev;
	}

	/* Add the node to the front of the free list */
	if(resource_mode == NO_FREE_MEMORY)
	{
		pn->next = pfree;
		pn->prev = (struct process_node *)0;
		if(pfree)
		{
			pfree->prev = pn;
		}
		pfree = pn;
//		free(pn->process_data);
//		pn->process_data = (void *)0;
	}
	else
	{
		// Free the process data and stack memory area
		free(pn->process_data);
		pn->process_data = (void *)0;

		// Free the memory used for the process node
		free(pn);
	}
}

/*
** delete_process_id() - This function is used to delete a process from the
** active process list based on its process id.
*/
void delete_process_id(int id)
{
	struct process_node	*pn;

	pn = plist;
	while(pn)
	{
		if(id == pn->process_id)
		{
			delete_process(pn);
			return;
		}
		pn = pn->next;
	}
}

/*
** print_processes() - This function is used to print out the process name and address of each active process
*/
void print_processes(void)
{
	struct process_node	*pn;

	pn = plist;
	while(pn)
	{
		printf("Process name %s, start address %x\n",pn->process_name,(int)pn->entry_func);
		pn = pn->next;
	}
}


/*
** delete_multiple_processs() - This function is used to delete multiple
** processs from the active process list base on a masked version of the
** process node id.
*/
void delete_multiple_processs(int id, int mask)
{
	struct process_node	*pn;
	struct process_node	*ps;

	pn = plist;
	while(pn)
	{
		ps = pn->next;
		if((pn->process_id & mask) == id)
		{
			delete_process(pn);
		}
		pn = ps;
	}
}

/*
** clear_all_processs() - This function uncontionally clears ALL processs from
** the active process list and moves them to the free process list.
*/
void clear_process_list(void)
{
	struct process_node	*pn;

	/* If no nodes on active list - just return */
	if(!plist)
	{
		return;
	}

	// Walk the process list and free up all of the process stacks
	pn = plist;
	while(pn)
	{
		free(pn->process_data);
		pn->process_data = (void *)0;
		pn = pn->next;
	}

	/* If there are process nodes on the free list we add the link the */
	/* end of the active list to the beginning of the free list */
	if(pfree)
	{
		/* Go to last node on active list */
		pn = plist;
		while(pn->next)
		{
			pn = pn->next;
		}
		/* Link free list to end of active list */
		pn->next = pfree;
		pfree->prev = pn;
	}

	/* Free list now poinos at active list */
	pfree = plist;

	/* Set no nodes on active process list */
	plist = (struct process_node *)0;
}


/*
** add_sleep_process_id(id, ticks) adds ticks to the sleep time of all 
** processes with pid of id
*/
void add_sleep_process_id(int id, short ticks)
{
	struct process_node	*pn;

	pn = plist;
	while(pn)
	{
		if(id == pn->process_id)
		{
			/* this will not be correct if the process is suspended, the high order bit */
			/* is set to signify that the process is suspended and this will be interpreted */
			/* as a negative sleep time in this context */
         pn->process_sleep_time += ticks;
		}
		pn = pn->next;
	}
}


/*****************************************************************************/
/*                                                                           */
/* This function is by a process to put another process to sleep for a total */
/* of ticks number of ticks.                                                 */
/*                                                                           */
/*****************************************************************************/
void sleep_process(struct process_node *p, short ticks)
{
      register struct process_node *lst;

      lst = plist;
      while(lst)
      {
          if((long)lst == (long)p)
          {
              lst->process_sleep_time |= (ticks - (lst->process_sleep_time & 0x7fff));
              if(lst->process_sleep_time <= 0)
              {
                  lst->process_sleep_time |= 1;
              }
              break;
          }
          lst = lst->next;
      }
}

/*
** same as process below, only uses proc id rather than direct pointer
*/
void wakeup_process_id(int id)
{
	struct process_node	*pn;

	pn = plist;
	while(pn)
	{
		if(id == pn->process_id)
		{
         pn->process_sleep_time = 1;
		}
		pn = pn->next;
	}
}

/*****************************************************************************/
/*                                                                           */
/* This function is used by a process to force another process to wakeup on  */
/* the next tick.                                                            */
/*                                                                           */
/*****************************************************************************/
void wakeup_process(struct process_node *p)
{
	register struct process_node *lst;

	lst = plist;
	while(lst)
	{
		if((long)lst == (long)p)
		{
			lst->process_sleep_time = 1;
			break;
		}
		lst = lst->next;
	}
}

/*****************************************************************************/
/*                                                                           */
/* This function is used by a process to suspend itself.                     */
/*                                                                           */
/*****************************************************************************/
void suspend_self(void)
{
	if(!cur_proc)
	{
#ifdef DEBUG
		fprintf(stderr, "suspend_self(): Attempt to suspend a non-process\r\n");
		lockup();
#endif
		return;
	}
	cur_proc->process_sleep_time |= 0x8000;
	sleep(1);
}

/*****************************************************************************/
/*                                                                           */
/* This function is used by a process to suspend another process.            */
/*                                                                           */
/*****************************************************************************/
void suspend_process(struct process_node *p)
{
      register struct process_node *lst;

      lst = plist;
      while(lst)
      {
          if((long)p == (long)lst)
          {
		lst->process_sleep_time |= 0x8000;
              break;
          }
          lst = lst->next;
      }
}

/*****************************************************************************/
/*                                                                           */
/* This function is used to suspend ALL processes EXCEPT the calling process */
/*                                                                           */
/*****************************************************************************/
void suspend_all(void)
{
      register struct process_node *lst;

      lst = plist;
      while(lst)
      {
          if((long)lst != (long)cur_proc)
          {
		lst->process_sleep_time |= 0x8000;
          }
          lst = lst->next;
      }
}

/*****************************************************************************/
/*                                                                           */
/* This function is used to resume a suspended process.                      */
/*                                                                           */
/*****************************************************************************/
void resume_process(struct process_node *p)
{
      register struct process_node *lst;

      lst = plist;
      while(lst)
      {
          if((long)lst == (long)p)
          {
		lst->process_sleep_time &= ~0x8000;
              break;
          }
          lst = lst->next;
      }
}

/*****************************************************************************/
/*                                                                           */
/* This function is used to resume ALL suspended processes.                  */
/*                                                                           */
/*****************************************************************************/
void resume_all(void)
{
      register struct process_node *lst;

      lst = plist;
      while(lst)
      {
          lst->process_sleep_time &= ~0x8000;
          lst = lst->next;
      }
}

/*****************************************************************************/
/*                                                                           */
/* existp() searches the process list for a process matching 'process_id' and 'mask'*/
/* and returns that pointer if successful, or a NULL pointer on no match.    */
/*                                                                           */
/*****************************************************************************/
struct process_node *existp( int process_id, int mask )
{
      struct process_node *ptr;

      ptr = plist;

      while( ptr )
      {
          if(( ptr->process_id & mask ) == ( process_id & mask ))
              return ptr;
          ptr = ptr->next;
      }
      return ptr;
}

/*****************************************************************************/
/*                                                                           */
/* kill 'proc'                                                               */
/*                                                                           */
/*****************************************************************************/
void kill( struct process_node *proc, int cause )
{
	process_node_t	*p_list;
	int				is_on_list = 0;

	// First check to see that the node is really on the
	// active process list
	p_list = plist;
	while(p_list)
	{
		if((int)proc == (int)p_list)
		{
			is_on_list = 1;
			break;
		}
		p_list = p_list->next;
	}

	// Is it on the list
	if(!is_on_list)
	{
#if defined(DEBUG)
		fprintf(stderr, "Attempt to kill a non-existant process\r\n");
#endif
		return;
	}

	/* if the process has an exit function, do it. */
	if(proc->process_exit)
	{
		proc->process_exit(proc, cause);
	}

	delete_process(proc);
}

void kill_current(int cause)
{
	if(cur_proc->process_exit)
	{
		cur_proc->process_exit(cur_proc, cause);
	}
	delete_process(cur_proc);
}

/*****************************************************************************/
/*                                                                           */
/* kill all processes except self that match 'process_id' and 'mask'.               */
/*                                                                           */
/*****************************************************************************/
void killall( int process_id, int mask )
{
      struct process_node *ptr, *nxt;

      /* don't allow mask to override the indestructible bit */
      mask |= NODESTRUCT;
      ptr = plist;

      while( ptr )
      {
          nxt = ptr->next;
          if((( ptr->process_id & mask ) == ( process_id & mask )) && ( ptr != cur_proc ))
              kill( ptr, PD_WIPEOUT );
          ptr = nxt;
      }
}

void kill_everything(void)
{
	struct process_node	*ptr;
	struct process_node	*nxt;

	ptr = plist;
	while(ptr)
	{
		nxt = ptr->next;
		if(ptr != cur_proc)
		{
			kill(ptr, PD_WIPEOUT);
		}
		ptr = nxt;
	}
}

/*****************************************************************************/
/*                                                                           */
/* set_process_id() sets the process_id of the current process.                     */
/*                                                                           */
/*****************************************************************************/
void set_pid( int new_process_id )
{
	if(!cur_proc)
	{
#ifdef DEBUG
		fprintf(stderr, "set_pid(): Attempt to change process id of non-process\r\n");
		lockup();
#endif
		return;
	}
	cur_proc->process_id = new_process_id;
}

void *get_pdata(void)
{
	if(!cur_proc)
	{
#ifdef DEBUG
		fprintf(stderr, "get_pdata(): Attempt to get process data of non-process\r\n");
		lockup();
#endif
		return((void *)0);
	}
	return(cur_proc->process_data);
}

/*
** suspend_multiple_processes() - This function is used to suspend multiple
** processes from the active process list base on a masked version of the
** process node id.
*/
void suspend_multiple_processes(int id, int mask)
{
	struct process_node	*pn;
	struct process_node	*ps;

	pn = plist;
	while(pn)
	{
		ps = pn->next;
		if((pn->process_id & mask) == id && pn != cur_proc)
		{
			suspend_process(pn);
		}
		pn = ps;
	}
}

/*
** resume_multiple_processes() - This function is used to resume multiple
** processes from the active process list base on a masked version of the
** process node id.
*/
void resume_multiple_processes(int id, int mask)
{
	struct process_node	*pn;
	struct process_node	*ps;

	pn = plist;
	while(pn)
	{
		ps = pn->next;
		if((pn->process_id & mask) == id)
		{
			resume_process(pn);
		}
		pn = ps;
	}
}


/*
** set_process_run_level() - This function is used to the process run level
** of the process that called the function.
*/
void set_run_level(int level)
{
	cur_proc->run_level = level;
	sort_processes = 1;
}

/*
** set_process_run_level() - This function is used to set a processes run
** level.
*/
void set_process_run_level(process_node_t *proc, int level)
{
	process_node_t	*plst;

	plst = plist;
	while(plst)
	{
		if((int)plst == (int)proc)
		{
			proc->run_level = level;
			sort_processes = 1;
			return;
		}
		plst = plst->next;
	}
#if defined(DEBUG)
	fprintf(stderr, "set_process_run_level(): Attempt to set run level for non-existant process\r\n");
#endif
}



/*
** _proc_sort() - Function called be proc sort to do comparisons
*/
static int _proc_sort(const void *a, const void *b)
{
	process_node_t	*pa;
	process_node_t	*pb;

	pa = (process_node_t *)*((process_node_t **)a);
	pb = (process_node_t *)*((process_node_t **)b);

	return(pa->run_level - pb->run_level);
}

/*
** do_process_sort() - This function actually sorts the process list into
** ascending run order.  NOTE - This function gets called from the process
** dispatching loop.
*/
void do_process_sort(void)
{
	int				count;
	int				i;
	process_node_t	*plst;
	process_node_t	**pptr;

	// Do we need to sort the processes ?
	if(!sort_processes)
	{
		return;
	}

	// Reset process sort flag
	sort_processes = 0;

	// Are ther any processes on the process list ?
	if(!plist)
	{
		// NO - Don't bother
		return;
	}

	// Is there more that 1 process on the process list ?
	if(!plist->next)
	{
		// NO - Don't bother
		return;
	}

	// Figure out how many processes there are
	plst = plist;
	count = 0;
	while(plst)
	{
		++count;
		plst = plst->next;
	}

	// Allocate memory for the array of process pointers
	if((pptr = (process_node_t **)malloc(sizeof(void *) * count)) == (process_node_t **)0)
	{
#if defined(DEBUG)
		fprintf(stderr, "do_process_sort(): Can not allocate memory for process pointer array\r\n");
		lockup();
#endif
		return;
	}

	// Set the process pointer array
	plst = plist;
	count = 0;
	while(plst)
	{
		pptr[count++] = plst;
		plst = plst->next;
	}

	// Sort the processes
	qsort((void *)&pptr[0], count, sizeof(void *), _proc_sort);

	// Rebuild the links for the process node list
	for(i = 0; i < count; i++)
	{
		if(!i)
		{
			pptr[i]->prev = (process_node_t *)0;
			pptr[i]->next = pptr[i+1];
		}
		else if(i == (count - 1))
		{
			pptr[i]->prev = pptr[i-1];
			pptr[i]->next = (process_node_t *)0;
		}
		else
		{
			pptr[i]->prev = pptr[i-1];
			pptr[i]->next = pptr[i+1];
		}
	}

	// Set the new process list pointer
	plist = pptr[0];
		
	// Free the memory used for the process pointer array
	free(pptr);
}


//
// clear_free_process_list() - Function used to clear out the the free list
// for process nodes.
//
void clear_free_process_list(void)
{
	register process_node_t	*p;
	register process_node_t	*next;

	// Get pointer to beginning of list
	p = pfree;

	while(p)
	{
		// Save next node pointer
		next = p->next;

		// Has process data memory been allocated ?
		if(p->process_data)
		{
			// YES - Free memory used for process data/stack
			free(p->process_data);
		}

		// Free memory used for process node
		free(p);

		// Next node on free list
		p = next;
	}

	// Set free list pointer
	pfree = (process_node_t *)0;
}
