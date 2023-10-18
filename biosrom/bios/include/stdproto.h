/****************************************************************************/
/*                                                                          */
/* stdproto.h - prototypes and defs that are more or less universal.        */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Right Reserved                                                       */
/*                                                                          */
/* $Revision: 1 $                                                             */
/*                                                                          */
/****************************************************************************/

#ifndef TRUE
#define FALSE	0
#define TRUE	!FALSE
#endif

void lockup( void );

/*
** from memory.c
*/
void *kmalloc( int );
void *malloc( int );
void *calloc( int, int );
void *realloc( void *, int );
void kfree( void * );
void free( void * );
unsigned int get_heap_available(void);

/*
** from mthread.c
*/
void sleep( int );
void die( int );

/*
** from except.s
*/
void lockup(void);

/*
** from handlers.c
*/
int get_tsec(void);

/*
** from draw.c
*/
void set_bgnd_color(int);
int get_bgnd_color(void);


/*
** from __main.c
*/
void exit(void);

/*
** from printf.c
*/
void *oprintf(char *, ...);
void dbg_printf(char *, ...);
int sprintf(char *, char *, ...);

/*
** from string.c
*/
char *strcat(char *s, char *t);
int strcmp(char *s, char *t);
char *strcpy(char *s, char *t);
int strlen(char *s);
int strlen_num(register char *s);
int isalpha(char c);
int isupper(char c);
int islower(char c);
int isdigit(char c);
int ishex(char c);
int isspace(char c);
int isalnum(char c);
int isprint(char c);
int iscntrl(char c);
int ispunct(char c);
int isascii(char c);
int toupper(char c);
int tolower(char c);
int toascii(char c);
char *atoui(char *str, unsigned int *ptrnum, unsigned int base);
char *atob(char *str, unsigned int *ptrnum, unsigned int base, int seg);
char *strncpy(char *dest, char *src, int length);
int strncmp(char *s1, char *s2, int length);

/*
** from mthread.c
*/
void sleep(int ticks);
void die( int cause );
void set_frame_factor(int f);

/*
** from switch.c
*/
void *set_psw_handler(int sw_id, void (*h_func)(int, int));
void *set_dcsw_handler(int sw_id, void (*h_func)(int, int));
int get_player_sw_current(void);
int get_dip_coin_current(void);
int get_player_sw_close(void);
int get_player_sw_open(void);
int get_dip_coin_close(void);
int get_dip_coin_open(void);


/*
** from qsort.c
*/
void qsort(const void *, int, int, int (*cmp)(const void *, const void *));

