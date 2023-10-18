//
// lockup.h - Header file for system lockup stuff
//
// $Revision: 4 $
//
// $Author: Markg $
//
#ifndef __LOCKUP_H__
#define __LOCKUP_H__

#ifdef VERSIONS
char	goose_lockup_h_version[] = {"$Revision: 4 $"};
#endif

// Function prototypes
void install_vector(int, int (*)(int, int *));
void show_process_list(void);
void dump_used_handle( int mmid );
void enable_lockup(void);
void disable_lockup(void);
void show_texture_list(void);
void enable_tlb_dump(void);
void do_list_dump(void);


// Definition of the lockup macro
#ifdef DEBUG
#define	lockup()	__asm__("	teqi	$0,0");
#else
#define	lockup()
#endif

#endif
