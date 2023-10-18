/****************************************************************************/
/*                                                                          */
/* mthread.c - Source for process system for Phoenix system.                */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* Version:     1.00                                                        */
/* Date:        6/28/95                                                     */
/*                                                                          */
/* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* Use, duplication, or disclosure is strictly forbidden unless approved    */
/* in writing by Williams Electronics Games Inc.                            */
/*                                                                          */
/* $Revision: 34 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<string.h>
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#include	<goose/process.h>
#include	<goose/gfader.h>
#include	<goose/object.h>
#include    <ioctl.h>
#include	<goose/goose.h>

#ifdef VEGAS
void wait_vsync(int);
#endif

char	goose_mthread_c_version[] = {"$Revision: 34 $"};

#if defined(PROFILE) || defined(VEGAS)

__asm__("
	.set	noreorder
	.globl	___clr_count
___clr_count:
	jr	$31
	mtc0	$0,$9

	.globl	___get_count
___get_count:
	mfc0	$2,$9
	nop
	nop
	jr	$31
	nop
	.set	reorder
");

void ___clr_count(void);
int ___get_count(void);

int	profile_enable = 1;

#define	START_TIMER	if(profile_enable) ___clr_count()

#ifndef VEGAS
#define MAX_NANOS	17600000
#define	GET_TIME(A)	if(profile_enable) (A) = (___get_count() * 13)
#else
// NOTE - This is for 250 Mhz processor
#define	GET_TIME(A)	if(profile_enable) (A) = (___get_count() * 8)
#endif
#endif


// Function prototypes for global functions used in this module
void	draw_sprites(void);
void	draw_background(void);
void	draw_transition(void);
void	dma_start(void);
void	kill(struct process_node *proc, int cause);
void	do_pre_dma_proc(void);
void	do_pre_proc(void);
void	do_post_proc(void);


// Static variables use only in this module
static int				__draw = 1;
static int				frame_factor = 1;
static int frame_time = 1;
static int enable_realtime = FALSE;

#ifndef VEGAS
static void	(*pstall_func)(void) = (void (*)(void))0xbfc00000;
static int	pstall_count = 0;
#endif

// External variables accessed from this module
#ifndef VEGAS
extern volatile int			got_interrupt;
extern int						*dma_buffer1, *dma_buffer2;
#endif
extern int						background_color;
extern struct process_node	*plist;
extern struct process_node	*pfree;
extern struct process_node	*cur_proc;

#if defined(PROFILE) || defined(VEGAS)
int	psleep_time[256];
int	pwake_time[256];
int	pdone_time[256];
int	pkill_time[256];
int	cp_next[256];
int	kill_time, kill_start, kill_end;
int	pnum;
int	pkcount = 0;
char	pdie_name[256][8];
struct process_node	__proc[256];
#if defined(VEGAS)
unsigned int	max_nanos = 0;
extern int		tsec;
#endif
#endif


// Assorted definitions
#define	FALSE	0

//****************************************************************************
//
// This is the sleep function.  When processes sleep they call this function
// This function only exits when the process wakes up.
//
//****************************************************************************
#if defined(PROFILE) || defined(VEGAS)
__asm__("
	.text
	.align	2
	.globl	sleep
	.ent		sleep
	.frame	$sp,0,$31
	.mask		0x00000000,0
	.fmask	0x00000000,0
	.def	ticks;	.val	4;	.scl	17;	.type	0x4;	.endef
	.set		noreorder
sleep:
	# Get address of current process pointer
	la		$8,cur_proc

	# Get the current process pointer
	lw		$9,0($8)
	bne	$9,$0,1f
	nop
	jr		$31
	nop
1:
	lh		$10,12($9)
	or		$4,$10,$4
	sh		$4,12($9)

	# Disable interrupts while doing stack pointer swap
	mfc0	$11,$12
	li		$12,1
	not	$12
	and	$11,$11,$12
	mtc0	$11,$12
	nop
	nop

	# Drop into system mode by swapping r27 & r29.
	xor	$27,$27,$29
	xor	$29,$27,$29
	xor	$27,$27,$29

	# Re-enable interrupts after stack pointer swap
	mfc0	$11,$12
	ori	$11,1
	mtc0	$11,$12
	nop
	nop
											  
	# Save all of the registers that are guarenteed to be preserved across
	# function calls.
	sw		$16,-4($27)
	sw		$17,-8($27)
	sw		$18,-12($27)
	sw		$19,-16($27)
	sw		$20,-20($27)
	sw		$21,-24($27)
	sw		$22,-28($27)
	sw		$23,-32($27)
	sw		$30,-36($27)
	sw		$31,-40($27)
	swc1	$f20,-44($27)
	swc1	$f22,-48($27)
	swc1	$f24,-52($27)
	swc1	$f26,-56($27)
	swc1	$f28,-60($27)
	swc1	$f30,-64($27)
	swc1	$f21,-68($27)
	swc1	$f23,-72($27)
	swc1	$f25,-76($27)
	swc1	$f27,-80($27)
	swc1	$f29,-84($27)
	swc1	$f31,-88($27)

	# Update the process stack pointer field of the process node structure
	sw		$27,8($9)

	# Grab the sleep time stamp
	mfc0	$10,$9
	la		$9,pnum
	lw		$8,0($9)
	sll	$8,2
	la		$9,psleep_time
	addu	$8,$8,$9
	li		$11,0x80000000
	or		$10,$10,$11
	j		___sys_wake
	sw		$10,0($8)

	.set	reorder
	.end	sleep
");
#else
__asm__("
	.text
	.align	2
	.globl	sleep
	.ent		sleep
	.frame	$sp,0,$31
	.mask		0x00000000,0
	.fmask	0x00000000,0
	.def	ticks;	.val	4;	.scl	17;	.type	0x4;	.endef
	.set		noreorder
sleep:
	# Get address of current process pointer
	la		$8,cur_proc

	# Get current process pointer
	lw		$9,0($8)
	bne	$9,$0,1f
	nop
	jr		$31
	nop
1:
	lh		$10,12($9)
	or		$4,$10,$4
	sh		$4,12($9)

	# Disable interrupts while doing stack pointer swap
	mfc0	$11,$12
	li		$12,1
	not	$12
	and	$11,$11,$12
	mtc0	$11,$12
	nop
	nop

	# Drop into system mode by swapping r27 & r29.
	xor	$27,$27,$29
	xor	$29,$27,$29
	xor	$27,$27,$29
											  
	# Re-enable interrupts after stack pointer swap
	mfc0	$11,$12
	ori	$11,1
	mtc0	$11,$12
	nop
	nop
											  
	# Save all of the registers that are guarenteed to be preserved across
	# function calls.
	sw		$16,-4($27)
	sw		$17,-8($27)
	sw		$18,-12($27)
	sw		$19,-16($27)
	sw		$20,-20($27)
	sw		$21,-24($27)
	sw		$22,-28($27)
	sw		$23,-32($27)
	sw		$30,-36($27)
	sw		$31,-40($27)
	swc1	$f20,-44($27)
	swc1	$f22,-48($27)
	swc1	$f24,-52($27)
	swc1	$f26,-56($27)
	swc1	$f28,-60($27)
	swc1	$f30,-64($27)
	swc1	$f21,-68($27)
	swc1	$f23,-72($27)
	swc1	$f25,-76($27)
	swc1	$f27,-80($27)
	swc1	$f29,-84($27)
	swc1	$f31,-88($27)

	# Update the process stack pointer field of the process node structure
	j		___sys_wake
	sw		$27,8($9)
	.set	reorder
	.end	sleep
");
#endif


/*****************************************************************************/
/*                                                                           */
/* process kills itself                                                      */
/*                                                                           */
/*****************************************************************************/
void kill_current(int);

void die( int cause )
{
#if defined(PROFILE) || defined(VEGAS)
	strncpy(pdie_name[pkcount], cur_proc->process_name, 8);
	GET_TIME(psleep_time[pnum]);
#endif

#if defined(PROFILE) || defined(VEGAS)
	GET_TIME(kill_start);
#endif

	// Kill myself
//	kill(cur_proc, cause);
	kill_current(cause);

#if defined(PROFILE) || defined(VEGAS)
	GET_TIME(kill_end);
	pkill_time[pkcount] = kill_start;
	pkcount++;
	kill_time += (kill_end-kill_start);
#endif

	// return to system mode by swapping the system and process stack pointers
	__asm__("
	.set	noreorder
	# Disable interrupts while doing stack swap
	mfc0	$8,$12
	li		$9,1
	not	$9
	and	$8,$8,$9
	mtc0	$8,$12
	nop
	nop

   xor	$27,$27,$29
   xor	$29,$27,$29
   xor	$27,$27,$29

	# Re-enable interrupts
	mfc0	$8,$12
	ori	$8,1
	mtc0	$8,$12
	nop
	nop

   j		___sys_wake
	nop
	.set	reorder
	");
}

/*****************************************************************************/
/*                                                                           */
/* process_dispatch() never returns.  Once called, it becomes the top-level  */
/* loop.  Every tick it calls the functions on the system jobs list, and     */
/* decrements all the sleep times on the processes, running the ones that    */
/* reach zero.  It's a good idea to create at least one process before       */
/* this, unless one of the system jobs creates one.                          */
/*                                                                           */
/*****************************************************************************/

void clear_screen(void)
{
	grBufferClear(background_color, 0, GR_WDEPTHVALUE_FARTHEST);
}


// This is the process dispatcher.  This function gets called after at least
// one process has been created.  This loop NEVER ever returns.  Once started
// it simply dispatches processes from the process list.  This loop is
// synchronized to the vertical display interrupt.  If the vertical display
// interrupt is NOT occurring the system WILL appear hung.
int	drawing = 0;

#if defined(PROFILE) || defined(VEGAS)
unsigned int	tot_time;
unsigned int	proc_time;
#if defined(SEATTLE)
unsigned int	dma_start_time;
unsigned int	pre_dma_proc_time;
#endif
unsigned int	pre_proc_start_time;
unsigned int	post_proc_start_time;
unsigned int	draw_time;
unsigned int	proc_sort_start_time;
#if defined(SEATTLE)
unsigned int	dma_int_time;
unsigned int	dma_int_exit_time;
#endif
unsigned int	ptime[256];
unsigned int	ploop_start_time;
unsigned int	sprite_draw_start_time;
unsigned int	object_draw_start_time;
unsigned int	transition_draw_start_time;
unsigned int	unim_time;
unsigned int	fp0_time;
#if defined(SEATTLE)
extern volatile int	dma_int;
#endif
void dump_3d_object_times(void);
void reset_dfunc_time(void);
void display_dfunc_time(void);
#endif

void process_dispatch(void)
{
#if defined(PROFILE) || defined(VEGAS)
	int	pcount;
#endif

#ifndef VEGAS
	got_interrupt = 0;
#endif

#if defined(VEGAS)
#if defined(DEBUG)
	init_text_overlay(1);
	to_printf("[2J");
#endif
	wait_vsync(1);
	wait_vsync(1);
	START_TIMER;
	wait_vsync(10);
	GET_TIME(max_nanos);
	max_nanos /= 10;
	tsec = (int)((1.0f / ((float)max_nanos / 1000000000.0f)) + 0.5f);
#endif

	// Beginning of the main process dispatch loop (loops forever)
	while(1)
	{
#if defined(PROFILE) || defined(VEGAS)
		pnum = 0;
		pkcount = 0;
#endif
		// Is drawing enabled ?
		if(__draw)
		{
			// YES - Load a new gamma table if needed
			load_gamma_table();
		}

#ifndef VEGAS
		pstall_count = 0;

		// Wait here for the vertical retrace interrupt count to be equal
		// to the framing factor
		do
		{
			++pstall_count;
			if(pstall_count > 100000000)
			{
				pstall_func();
			}
		} while(got_interrupt < frame_factor);
#if defined(PROFILE) || defined(VEGAS)
		unim_time = 0;
		fp0_time = 0;
		kill_time = 0;
		START_TIMER;
#endif

#else
		if(__draw)
		{
			grBufferSwap(1);
			draw_background();
		}
		wait_vsync(frame_factor);
#if defined(PROFILE) || defined(VEGAS)
		kill_time = 0;
#if defined(VEGAS)
		reset_dfunc_time();
#endif
		START_TIMER;
		scan_switches();
#endif
#endif

#ifndef VEGAS
		// Reset the vertical retrace interrupt count
		got_interrupt = 0;
        
		/* FRAME RATE */    
		if( enable_realtime == TRUE)
		{
			/* start the timer */
			_ioctl(5, FIOCSTARTTIMER3, 0);
		}

		// Do any application installed pre-DMA start functions
#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(pre_dma_proc_time);
#endif
		do_pre_dma_proc();

		// Is drawing enabled ?
		if(__draw)
		{
			// Tell the DMA engine to send the last frames worth of rendering data
#if defined(PROFILE) || defined(VEGAS)
			dma_int_time = 0;
			GET_TIME(dma_start_time);
#endif
			dma_start();
		}
#endif
	
		// Do any application install pre-process loop functions
#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(pre_proc_start_time);
#endif
		do_pre_proc();

		// Set the current process pointer
		cur_proc = plist;

#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(ploop_start_time);
#endif

		// Loop through all of the processes
		while(cur_proc)
		{
#if defined(PROFILE) || defined(VEGAS)
			memcpy(&__proc[pnum], cur_proc, sizeof(struct process_node));
			GET_TIME(ptime[pnum]);
#endif
			// If the process sleep time is <= 0 the process is suspended
			if(cur_proc->process_sleep_time > 0)
			{
				// Process NOT suspended - decrement it's sleep time
				cur_proc->process_sleep_time--;
	
				// Is this process awake this tick ?
				if(!cur_proc->process_sleep_time)
				{
					// Save the system registers onto the system stack
					__asm__("	.set	noreorder");
					__asm__("	addi	$29,$29,-204");
					__asm__("	.set	noat");
					__asm__("	sw	$1,204-4($29)");
					__asm__("	.set	at");
					__asm__("	sw	$2,204-8($29)");
					__asm__("	sw	$3,204-12($29)");
					__asm__("	sw	$4,204-16($29)");
					__asm__("	sw	$5,204-20($29)");
					__asm__("	sw	$6,204-24($29)");
					__asm__("	sw	$7,204-28($29)");
					__asm__("	sw	$8,204-32($29)");
					__asm__("	sw	$9,204-36($29)");
					__asm__("	sw	$10,204-40($29)");
					__asm__("	sw	$11,204-44($29)");
					__asm__("	sw	$12,204-48($29)");
					__asm__("	sw	$13,204-52($29)");
					__asm__("	sw	$14,204-56($29)");
					__asm__("	sw	$15,204-60($29)");
					__asm__("	sw	$16,204-64($29)");
					__asm__("	sw	$17,204-68($29)");
					__asm__("	sw	$18,204-72($29)");
					__asm__("	sw	$19,204-76($29)");
					__asm__("	sw	$20,204-80($29)");
					__asm__("	sw	$21,204-84($29)");
					__asm__("	sw	$22,204-88($29)");
					__asm__("	sw	$23,204-92($29)");
					__asm__("	sw	$24,204-96($29)");
					__asm__("	sw	$25,204-100($29)");
					__asm__("	sw	$30,204-108($29)");
					__asm__("	sw	$31,204-112($29)");
					__asm__("	swc1	$f0,204-116($29)");
					__asm__("	swc1	$f2,204-120($29)");
					__asm__("	swc1	$f4,204-124($29)");
					__asm__("	swc1	$f6,204-128($29)");
					__asm__("	swc1	$f8,204-132($29)");
					__asm__("	swc1	$f10,204-136($29)");
					__asm__("	swc1	$f12,204-140($29)");
					__asm__("	swc1	$f14,204-144($29)");
					__asm__("	swc1	$f16,204-148($29)");
					__asm__("	swc1	$f18,204-152($29)");
					__asm__("	swc1	$f20,204-156($29)");
					__asm__("	swc1	$f22,204-160($29)");
					__asm__("	swc1	$f24,204-164($29)");
					__asm__("	swc1	$f26,204-168($29)");
					__asm__("	swc1	$f28,204-172($29)");
					__asm__("	swc1	$f30,204-176($29)");
					__asm__("	swc1	$f21,204-180($29)");
					__asm__("	swc1	$f23,204-184($29)");
					__asm__("	swc1	$f25,204-188($29)");
					__asm__("	swc1	$f27,204-192($29)");
					__asm__("	swc1	$f29,204-196($29)");
					__asm__("	swc1	$f31,204-200($29)");
	
					// Now restore all of the register from the process that is
					// being woken up.  NOTE - This function does NOT return until
					// it releases it's time slice by sleeping, dieing, or suspending

					// Get the processes stack pointer
					__asm__("	la		$8,cur_proc");
					__asm__("	lw		$9,0($8)");
					__asm__("   lw		$27,8($9)");
					__asm__("   nop");
#if defined(PROFILE) || defined(VEGAS)
					__asm__("	.set	reorder");
					GET_TIME(pwake_time[pnum]);
					__asm__("	.set	noreorder");
#endif
					// restore all but registers 27 and 29.
					__asm__("   lw		$16,-4($27)");
					__asm__("   lw		$17,-8($27)");
					__asm__("   lw		$18,-12($27)");
					__asm__("   lw		$19,-16($27)");
					__asm__("   lw		$20,-20($27)");
					__asm__("   lw		$21,-24($27)");
					__asm__("   lw		$22,-28($27)");
					__asm__("   lw		$23,-32($27)");
					__asm__("   lw		$30,-36($27)");
				
					// register 31 contains the address at which the process resumes execution.
					// In most cases this will be the instruction immediately following a call
					// to sleep().
					__asm__("   lw		$31,-40($27)");
					__asm__("   lwc1	$f20,-44($27)");
					__asm__("   lwc1	$f22,-48($27)");
					__asm__("   lwc1	$f24,-52($27)");
					__asm__("   lwc1	$f26,-56($27)");
					__asm__("   lwc1	$f28,-60($27)");
					__asm__("   lwc1	$f30,-64($27)");
					__asm__("   lwc1	$f21,-68($27)");
					__asm__("   lwc1	$f23,-72($27)");
					__asm__("   lwc1	$f25,-76($27)");
					__asm__("   lwc1	$f27,-80($27)");
					__asm__("   lwc1	$f29,-84($27)");
					__asm__("   lwc1	$f31,-88($27)");

					// Disable interrupts while swapping stack pointer
					__asm__("	mfc0	$8,$12");
					__asm__("	li		$9,1");
					__asm__("	not	$9");
					__asm__("	and	$8,$8,$9");
					__asm__("	mtc0	$8,$12");
					__asm__("	nop");
					__asm__("	nop");
				
					// switch from system to process mode
					__asm__("   xor	$27,$27,$29");
					__asm__("   xor	$29,$27,$29");
					__asm__("   xor	$27,$27,$29");

					// Re-enable interrupts
					__asm__("	mfc0	$8,$12");
					__asm__("	ori	$8,1");
					__asm__("	mtc0	$8,$12");
					__asm__("	nop");
					__asm__("	nop");

					// Go pick process back up where it left off
					__asm__("	jr		$31");
	
					// Now that the process has given up it's time slice, restore all
					// of the system registers
					__asm__("___sys_wake:" );
#if defined(PROFILE) || defined(VEGAS)
					__asm__("	.set	reorder");
					__asm__("	nop");
					GET_TIME(pdone_time[pnum]);
					__asm__("	.set	noreorder");
#endif
					__asm__("	.set	noat");
					__asm__("	lw	$1,204-4($29)");
					__asm__("	.set	at");
					__asm__("	lw	$2,204-8($29)");
					__asm__("	lw	$3,204-12($29)");
					__asm__("	lw	$4,204-16($29)");
					__asm__("	lw	$5,204-20($29)");
					__asm__("	lw	$6,204-24($29)");
					__asm__("	lw	$7,204-28($29)");
					__asm__("	lw	$8,204-32($29)");
					__asm__("	lw	$9,204-36($29)");
					__asm__("	lw	$10,204-40($29)");
					__asm__("	lw	$11,204-44($29)");
					__asm__("	lw	$12,204-48($29)");
					__asm__("	lw	$13,204-52($29)");
					__asm__("	lw	$14,204-56($29)");
					__asm__("	lw	$15,204-60($29)");
					__asm__("	lw	$16,204-64($29)");
					__asm__("	lw	$17,204-68($29)");
					__asm__("	lw	$18,204-72($29)");
					__asm__("	lw	$19,204-76($29)");
					__asm__("	lw	$20,204-80($29)");
					__asm__("	lw	$21,204-84($29)");
					__asm__("	lw	$22,204-88($29)");
					__asm__("	lw	$23,204-92($29)");
					__asm__("	lw	$24,204-96($29)");
					__asm__("	lw	$25,204-100($29)");
					__asm__("	lw	$30,204-108($29)");
					__asm__("	lw	$31,204-112($29)");
					__asm__("	lwc1	$f0,204-116($29)");
	  				__asm__("	lwc1	$f2,204-120($29)");
					__asm__("	lwc1	$f4,204-124($29)");
					__asm__("	lwc1	$f6,204-128($29)");
					__asm__("	lwc1	$f8,204-132($29)");
					__asm__("	lwc1	$f10,204-136($29)");
					__asm__("	lwc1	$f12,204-140($29)");
					__asm__("	lwc1	$f14,204-144($29)");
					__asm__("	lwc1	$f16,204-148($29)");
					__asm__("	lwc1	$f18,204-152($29)");
					__asm__("	lwc1	$f20,204-156($29)");
					__asm__("	lwc1	$f22,204-160($29)");
					__asm__("	lwc1	$f24,204-164($29)");
					__asm__("	lwc1	$f26,204-168($29)");
					__asm__("	lwc1	$f28,204-172($29)");
					__asm__("	lwc1	$f30,204-176($29)");
	
					__asm__("	lwc1	$f21,204-180($29)");
					__asm__("	lwc1	$f23,204-184($29)");
					__asm__("	lwc1	$f25,204-188($29)");
					__asm__("	lwc1	$f27,204-192($29)");
					__asm__("	lwc1	$f29,204-196($29)");
					__asm__("	lwc1	$f31,204-200($29)");
					__asm__("	addi	$29,$29,204");
					__asm__("	.set	reorder");

				}
#if defined(PROFILE) || defined(VEGAS)
				else
				{
					GET_TIME(pwake_time[pnum]);
					pdone_time[pnum] = psleep_time[pnum] = pwake_time[pnum];
				}
#endif
			}
#if defined(PROFILE) || defined(VEGAS)
			else
			{
				GET_TIME(pwake_time[pnum]);
				pdone_time[pnum] = psleep_time[pnum] = pwake_time[pnum];
			}
#endif
	
			// Get the next process to check for running this tick
			// If the executing process was the first on the process list AND
			// it died, then the cur_proc pointer will be 0, so set the cur_proc
			// pointer to the beginning of the process list (which now contains
			// the pointer to the process after the process that died, if one
			// exists).
			if(cur_proc)
			{
#if defined(PROFILE) || defined(VEGAS)
				GET_TIME(cp_next[pnum]);
#endif
				cur_proc = cur_proc->next;
			}
	
			// POSSIBLE BUG - This appears to cause the process list to be
			// walked 2 times if the last process in the list dies!!!
			else
			{
				cur_proc = plist;
			}
#if defined(PROFILE) || defined(VEGAS)
			pnum++;
#endif
		}
	
#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(post_proc_start_time);
#endif

		// Do any application install post-process loop functions
		do_post_proc();

		// Is drawing enabled ?
		if(__draw)
		{
			// YES - Draw the 3d objects from the object list
#if defined(PROFILE) || defined(VEGAS)
			GET_TIME(object_draw_start_time);
#endif
			draw_3d_objects();
	
#if defined(PROFILE) || defined(VEGAS)
			GET_TIME(sprite_draw_start_time);
#endif
			// Draw the sprites
			draw_sprites();

#if defined(PROFILE) || defined(VEGAS)
			GET_TIME(transition_draw_start_time);
#endif
			// Draw the overlays
			draw_transition();
		}
        
#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(proc_sort_start_time);
#endif

		// Check to see if the process list needs to be sorted and do so
		do_process_sort();

#if defined(PROFILE) || defined(VEGAS)
		GET_TIME(tot_time);
#endif

#if defined(VEGAS) && defined(DEBUG)
		if((get_dip_coin_current() & 0x2000))
		{
			to_printf("[37m[35;1H[KLimit:  %4.2f (ms)", (float)max_nanos/1000000.0f);
			if(tot_time > max_nanos)
			{
				to_printf("[37m[36;1H[KTime:  [31m%6.2f[37m (ms)", (float)tot_time/1000000.0f);
			}
			else
			{
				to_printf("[37m[36;1H[KTime:  [32m%6.2f[37m (ms)", (float)tot_time/1000000.0f);
			}											   
		}
		else
		{
			to_printf("[2J");
		}
		draw_text_overlay();	
#endif

#if defined(TIME_STATS)
		{
			#define STAT_CNT	(60 * 57)

			static unsigned int times[STAT_CNT];
			static int idx = 0;
			static int flg = 0;
			int dc_cls = get_dip_coin_close();
			int dc_cur = get_dip_coin_current();
			int cnt = 0;
			times[idx++] = tot_time;
			if (idx == STAT_CNT) idx = 0, flg = 1;
			if (dc_cls & 0x1000 && dc_cur & 0x0800) cnt = 10 * 57;	// vol + & vol -
			if (dc_cls & 0x0800 && dc_cur & 0x1000) cnt = STAT_CNT;	// vol - & vol +
			if (cnt)
			{
				float bogper, avg;
				double total = 0;
				unsigned int best = 0;
				unsigned int worst = 0;
				int bogcnt = 0;
				int i, j;
				if (!flg && cnt > idx) cnt = idx;
				fprintf(stderr, "\n\nProcess Timing Stats: %6d frames\n", cnt);
				fprintf(stderr,     "  at full frame-rate: %6.2f seconds\n", (float)cnt/57.0f);
				fprintf(stderr,     "--------------------\n");
				i = idx;
				j = cnt;
				while(j--)
				{
					if (!i) i = STAT_CNT;
					i--;
#if defined(VEGAS)
					if (times[i] > max_nanos) bogcnt++;
#else
					if (times[i] > MAX_NANOS) bogcnt++;
#endif
					if (times[i] < best || !best) best = times[i];
					if (times[i] > worst) worst = times[i];
					total += times[i];
				}
				bogper = (float)bogcnt/(float)cnt;
				avg = total/(double)cnt;
				fprintf(stderr, "Bog count    : %6d  (%5.1f%%)\n", bogcnt, bogper*100.0f);
				fprintf(stderr, "Quickest time: %12.2f (us) %9.3f (ms)\n", (float)best/1000.0f, (float)best/1000000.0f);
				fprintf(stderr, "Slowest time : %12.2f (us) %9.3f (ms)\n", (float)worst/1000.0f,(float)worst/1000000.0f);
				fprintf(stderr, "Averge time  : %12.2f (us) %9.3f (ms)\n", avg/1000.0f, avg/1000000.0f);
				fprintf(stderr, "\n");
			}
		}
#endif

#if defined(PROFILE) || defined(VEGAS)
#if defined(VEGAS)
		if ((tot_time > max_nanos) &&
#else
		if ((tot_time > MAX_NANOS) &&
#endif
			(get_dip_coin_current() & 0x2000))
		{
			struct process_node	*p = __proc;
		
			fprintf(stderr, "\n\nProcess ");
	
#if defined(SEATTLE)
			// Wait for the dma interrupt
			while(!dma_int) ;

			if(dma_int_exit_time > MAX_NANOS)
			{
				fprintf(stderr, "and DMA ");
			}
			fprintf(stderr, "bog\n");

			fprintf(stderr, "pre_dma_proc_time:  %d\n", pre_dma_proc_time);
			fprintf(stderr, "dma_start_time: %d\n", dma_start_time);
#endif
#if 0
			fprintf(stderr, "pre_proc_start_time:  %d\n", pre_proc_start_time);
			fprintf(stderr, "\n%-12s%-12s%-12s%-12s%-13s%-8s\n",
				"PLOOPTIME",
				"WAKETIME",
				"SLEEP/DIE",
				"SYSRE-ENT",
				"PLOOPBOT",
				"NAME");
			for(pcount = 0; pcount < pnum; pcount++)
			{
				if(psleep_time[pcount] < 0)
				{
					psleep_time[pcount] &= ~0x80000000;
					psleep_time[pcount] *= 13;
				}
				fprintf(stderr, "%09d - %09d - %09d - %09d - %09d -> %-8.8s\n",
					ptime[pcount],
					pwake_time[pcount],
					psleep_time[pcount],
					pdone_time[pcount],
					cp_next[pcount],
					p->process_name);
				p++;
			}
#else
			fprintf(stderr, "Times for each process\n");
			for(pcount = 0; pcount < pnum; pcount++)
			{
				if(psleep_time[pcount] < 0)
				{
					psleep_time[pcount] &= ~0x80000000;
					psleep_time[pcount] *= 8;
				}
				fprintf(stderr, "%-8.8s: ", p->process_name);
				if(psleep_time[pcount] - pwake_time[pcount])
				{
					fprintf(stderr, "%8.2f (us)\n", (float)(psleep_time[pcount] - pwake_time[pcount])/1000.0f);
				}
				else
				{
					fprintf(stderr, "IDLE\n");
				}
				p++;
			}
			fprintf(stderr, "\n");
#endif
			dump_3d_object_times();
//			fprintf(stderr, "post_proc_time: %d\n", post_proc_time);
//			fprintf(stderr, "object_draw:    %d\n", object_draw_time);
//			fprintf(stderr, "sprite_draw:    %d\n", sprite_draw_time);
//			fprintf(stderr, "transistion:    %d\n", transition_draw_time);
//			fprintf(stderr, "proc_sort_time: %d\n", proc_sort_time);
			fprintf(stderr, "Time in pre proc functions: %6.3f (ms)\n", (float)(ploop_start_time - pre_proc_start_time)/1000000.0f);
			fprintf(stderr, "Time in processes:          %6.3f (ms)\n", (float)(post_proc_start_time - ploop_start_time)/1000000.0f);
			fprintf(stderr, "Time in post proc function: %6.3f (ms)\n", (float)(object_draw_start_time - post_proc_start_time)/1000000.0f);
			fprintf(stderr, "Time in 3d objects:         %6.3f (ms)\n", (float)(sprite_draw_start_time - object_draw_start_time)/1000000.0f);
			fprintf(stderr, "Time in sprites:            %6.3f (ms)\n", (float)(transition_draw_start_time - sprite_draw_start_time)/1000000.0f);
			fprintf(stderr, "Time in transitions:        %6.3f (ms)\n", (float)(proc_sort_start_time - transition_draw_start_time)/1000000.0f);
			fprintf(stderr, "Time sorting processes:     %6.3f (ms)\n", (float)(tot_time - proc_sort_start_time)/1000000.0f);
			fprintf(stderr, "Total Time:                 %6.3f (ms)\n", (float)tot_time/1000000.0f);
#if defined(SEATTLE)
			fprintf(stderr, "dma_int_time:   %d\n", dma_int_time);
			fprintf(stderr, "dma_exit_time:  %d\n", dma_int_exit_time);
			fprintf(stderr, "unim_time:      %d\n", unim_time);
			fprintf(stderr, "fp0_time:       %d\n", fp0_time);
#endif
			fprintf(stderr, "Time spent killing procs:   %6.3f (ms)\n", (float)kill_time/1000000.0f);
#if defined(VEGAS)
			display_dfunc_time();
#endif
			if(pkcount)
			{
				fprintf(stderr, "\nProcesses that died this tick\n");
				fprintf(stderr, "%-12s%s\n", "NAME", "TIME");
			}
			for(pcount = 0; pcount < pkcount; pcount++)
			{
				fprintf(stderr, "%-8.8s -> %09d\n", pdie_name[pcount], pkill_time[pcount]);
			}
#ifndef VEGAS
			got_interrupt = 0;
#endif
		}
#if defined(SEATTLE)
		else if ((!dma_int) && 
				(get_dip_coin_current() & 0x2000))
		{
			struct process_node	*p = __proc;
		
			// Wait for the dma interrupt
			while(!dma_int) ;
		
			if(dma_int_exit_time > 17600000)
			{
				fprintf(stderr, "DMA bog\n");
		
				fprintf(stderr, "pre_dma_proc_time:  %d\n", pre_dma_proc_time);
				fprintf(stderr, "dma_start_time: %d\n", dma_start_time);
				fprintf(stderr, "pre_proc_start_time:  %d\n", pre_proc_start_time);
				fprintf(stderr, "\n%-12s%-12s%-12s%-12s%-13s%-8s\n",
					"PLOOPTIME",
					"WAKETIME",
					"SLEEP/DIE",
					"SYSRE-ENT",
					"PLOOPBOT",
					"NAME");
				for(pcount = 0; pcount < pnum; pcount++)
				{
					if(psleep_time[pcount] < 0)
					{
						psleep_time[pcount] &= ~0x80000000;
						psleep_time[pcount] *= 13;
					}
					fprintf(stderr, "%09d - %09d - %09d - %09d - %09d -> %-8.8s\n",
						ptime[pcount],
						pwake_time[pcount],
						psleep_time[pcount],
						pdone_time[pcount],
						cp_next[pcount],
						p->process_name);
					p++;
				}
				fprintf(stderr, "post_proc_time: %d\n", post_proc_time);
				fprintf(stderr, "object_draw:    %d\n", object_draw_time);
				fprintf(stderr, "sprite_draw:    %d\n", sprite_draw_time);
				fprintf(stderr, "transistion:    %d\n", transition_draw_time);
				fprintf(stderr, "proc_sort_time: %d\n", proc_sort_time);
				fprintf(stderr, "tot_time:       %d\n", tot_time);
				fprintf(stderr, "dma_int_time:   %d\n", dma_int_time);
				fprintf(stderr, "dma_exit_time:  %d\n", dma_int_exit_time);
				fprintf(stderr, "unim_time:      %d\n", unim_time);
				fprintf(stderr, "fp0_time:       %d\n", fp0_time);
				fprintf(stderr, "kill_time:      %d\n", kill_time);
				if(pkcount)
				{
					fprintf(stderr, "\nProcesses that died this tick\n");
					fprintf(stderr, "%-12s%s\n", "NAME", "TIME");
				}
				for(pcount = 0; pcount < pkcount; pcount++)
				{
					fprintf(stderr, "%-8.8s -> %09d\n", pdie_name[pcount], pkill_time[pcount]);
				}
#ifndef VEGAS
				got_interrupt = 0;
#endif
			}
		}
#endif

#ifndef VEGAS
		/* FRAME RATE */    
		if( enable_realtime == TRUE)
		{
			/* get the time in nano seconds */
			_ioctl (5, FIOCGETTIMER3, (int)&frame_time);
		}
#endif
#endif
	}
}







/* NOTE - THIS FUNCTION IS HERE BECAUSE IT GETS FUCKED IF THE OPTIMIZER IS   */
/* USED ON IT!!!  IN OTHER WORDS - DON'T USE THE OPTIMIZER ON THIS FILE      */
/*****************************************************************************/
/*                                                                           */
/* qcreate() is a wrapper for create_process() that takes all the neccesary  */
/* information as arguments instead of in a struct.  If you're creating      */
/* multiple similar processes, it's more efficient to use create_process().  */
/*                                                                           */
/*****************************************************************************/
struct process_node *qcreate(char *name, int process_id, void (*entry_func)(), int arg1, int arg2, int arg3, int arg4)
{
	int	args[4];

	// Fill in the argument array
	args[0] = arg1;
	args[1] = arg2;
	args[2] = arg3;
	args[3] = arg4;

	// Go create the process on the FRONT of the process list
	return(create_process(name, process_id, args, entry_func, 0));
}

/* NOTE - THIS FUNCTION IS HERE BECAUSE IT GETS FUCKED IF THE OPTIMIZER IS   */
/* USED ON IT!!!  IN OTHER WORDS - DON'T USE THE OPTIMIZER ON THIS FILE      */
/*****************************************************************************/
/*                                                                           */
/* iqcreate() is a wrapper for create_process() that takes all the neccesary */
/* information as arguments instead of in a struct.  If you're creating      */
/* multiple similar processes, it's more efficient to use create_process().  */
/*                                                                           */
/*****************************************************************************/
struct process_node *iqcreate(char *name, int process_id, void (*entry_func)(), int arg1, int arg2, int arg3, int arg4)
{
	int	args[4];

	// Fill in the argument array
	args[0] = arg1;
	args[1] = arg2;
	args[2] = arg3;
	args[3] = arg4;

	// Go create the process in the process list immediately after the
	// process that is calling this function.
	return(create_process_immediate(name, process_id, args, entry_func, 0));
}

void set_frame_factor(int f)
{
	frame_factor = f;
}


void draw_enable(int val)
{
	// If we are enabling drawing and it WAS disabled - add
	// the background draw to the dma list
	if(val == 1 && !__draw)
	{
		draw_background();
	}
	__draw = val;
}






/*****************************************************************************/
/*                                                                           */
/* FUNCTION: mthread_enable_framerate()                                      */
/*                                                                           */
/* This turns on a local flag to enable the timer for the framerate          */
/* calculations.                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 27 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void mthread_enable_framerate(void)

{ /* mthread_enable_framerate() */

	enable_realtime = TRUE;

} /* mthread_enable_framerate() */

/***** End of mthread_enable_framerate() *************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: mthread_disable_framerate()                                     */
/*                                                                           */
/* This turns off a local flag to disable the timer for the framerate        */
/* calculations.                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 27 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void mthread_disable_framerate(void)

{ /* mthread_disable_framerate() */

	enable_realtime = FALSE;
    
} /* mthread_disable_framerate() */

/***** End of mthread_disable_framerate() ************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: mthread_get_framerate()                                         */
/*                                                                           */
/* This function returns the current framerate in nano seconds.              */
/* This is only valid if the if mthread_enable_framerate() is called         */
/* prior to this funciton.                                                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 27 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

int mthread_get_framerate(void)

{ /* mthread_get_framerate() */

	return(frame_time);

} /* mthread_get_framerate() */

/***** End of mthread_get_framerate() ****************************************/


#ifndef VEGAS
void install_process_stall_func(void (*func)(void))
{
	pstall_func = func;
}
#endif

#if defined(PROFILE) || defined(VEGAS)
void profile(int enable)
{
	profile_enable = enable;
}
#endif

/***** END of MTHREAD.C ******************************************************/
