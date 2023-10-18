;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 10 $
;
; $Author: Mlynch $
;

	section	.text

	opt	w-


	; Global defined in this module
	xdef	install_debug_hook
	xdef	storeregs
	xdef	restoreregs
	xdef	ex_stack
	xdef	memcpy_fast
        xdef    unhook_vectors
        xdef    exception
        xdef    reg_pc
	xdef	reg_r31
	xdef	reg_fcr31
	xdef	reg_cp0_status
	xdef	filebuff
	xdef	vtemp1
	xdef	reg_runflag
	xdef	reg_extype
	xdef	active
	xdef	restart

	xdef	ints2handle
	xdef	user_int0_handler
	xdef	user_into_handler

	; Externals referenced in this module
	xref	process_scsi				; from psyq.c
	xref	fast_general_exception_handler		; from except.s
	xref	flush_cache				; from cache.s
	xref	writeback_cache				; from cache.s
	xref	exception_vectors			; from handlers.c
	xref	sys_exception_handler			; from handlers.c
	xref	interrupt_handler			; from handlers.c
	xref	fpu_handler				; from handlers.c
	xref	int_route				; from handlers.c
	xref	proc					; from idedrv.c
	xref	suspend					; from idedrv.c
	xref	resume					; from idedrv.c
	xref	callback				; from idedrv.c
	xref	watchdog_enabled			; from handlers.c
	xref	__bios_version				; from main.c

	if	PHOENIX_SYS=1
INT_ICAUSE_REG          equ     $b5080020
DEBUG_SWITCH_INT_PENDING    equ     $0001
PCI_A_INT_PENDING       equ     $0100
	else if	PHOENIX_SYS=2
INT_ICAUSE_REG		equ	$b7500000
DEBUG_SWITCH_INT_PENDING	equ	$40
PCI_A_INT_PENDING	equ	$8
	else if PHOENIX_SYS=3
	else if PHOENIX_SYS=4
	else
	endif

SR_CU1                  equ     $20000000
SR_FR                   equ     $04000000
IP_7                    equ     $8000



C0_CAUSE        equr    13
C0_EPC		equr	14
C0_SR		equr	12
C0_ERRPC	equr	30

;================================================
; copy a block of memory, possibly not-aligned
; a0 = dest, a1 = src, a2 = len

memcpy_fast
	or	t0,a0,a1
	or	t0,a2
	and	t0,7
	beqz	t0,@d
	and	t0,3
	beqz	t0,@w
	and	t0,1
	beqz	t0,@h
@b:
	lb	t0,0(a1)
	addu	a1,a1,1
	sb	t0,0(a0)
	subu	a2,a2,1
	bnez	a2,@b
	addu	a0,a0,1

	jr	ra
	nop
@h:
	lh	t0,0(a1)
	addu	a1,a1,2
	sh	t0,0(a0)
	subu	a2,a2,2
	bnez	a2,@h
	addu	a0,a0,2

	jr	ra
	nop
@w:
	lw	t0,0(a1)
	addu	a1,a1,4
	sw	t0,0(a0)
	subu	a2,a2,4
	bnez	a2,@w
	addu	a0,a0,4

	jr	ra
	nop
@d:
	ld	t0,0(a1)
	addu	a1,a1,8
	sd	t0,0(a0)
	subu	a2,a2,8
	bnez	a2,@d
	addu	a0,a0,8

	jr	ra
	nop

	; This function should only be called to initialize the interrupts
	; that are to be handled by the debugger stub.  This is called
	; by main and by process_scsi when a reboot request is received
install_debug_hook
	; Generate interrupts to handle mask
	li	t0,$ffff0200

	; Get address of data storage area
	la	t1,exregs

	; Store the mask
	sw	t0,ints2handle-exregs(t1)

	; Store the saved version of the mask
	sw	t0,ints2handle_save-exregs(t1)

	; Done
        jr      ra
	nop


intdiv0
	lw	r2,user_int0_handler-exregs(k0)
	beq	r2,r0,no_user_int0
	nop

	; Restore the two registers we temporarily saved
	ld	r1,vtemp1-exregs(k0)
	ld	r2,vtemp2-exregs(k0)

	; Go to the fast exception handler
	opt	at-
	j	fast_general_exception_handler
	opt	at+
	nop

intover
	lw	r2,user_into_handler-exregs(k0)
	beq	r2,r0,no_user_into
	nop

	; Restore the two registers we temporarily saved
	ld	r1,vtemp1-exregs(k0)
	ld	r2,vtemp2-exregs(k0)

	; Go to the fast exception handler
	opt	at-
	j	fast_general_exception_handler
	opt	at+
	nop


;================================================
;
; Exception handler
;
	opt	at-,m-

exception
	; Get address of register storage area
	la	k0,exregs

	; Save of 2 registers for now
	sd	r1,vtemp1-exregs(k0)
	sd	r2,vtemp2-exregs(k0)

	; Get the cause register
	mfc0	r1,C0_CAUSE

	; Get bits that tell what execptions to deal with
	lw	r2,ints2handle-exregs(k0)

	; Shift exception code down to low bits
	srl	r1,2

	; Only interested in exception code
        and     r1,$1F

	; Is this a hardware interrupt ? - br = yes
	beqz	r1,@hardint

	; Shift execeptions we are to handle down by exception code (BDSLOT)
	srlv	r2,r1

	; Is the bit set ?
	and	r2,1

	; YES - Then we are to handle this exception
	bnez	r2,@doit
	nop

	; We are not handling this exception
@chain
	; Restore the two registers we temporarily saved
	ld	r1,vtemp1-exregs(k0)
	ld	r2,vtemp2-exregs(k0)

	; Go to the fast exception handler
	opt	at-
	j	fast_general_exception_handler
	opt	at+
	nop


	; All hardware generated interrupts end up here
@hardint
	; Get the cause register
	mfc0	r1,C0_CAUSE
	nop

	; Check to see if SCSI or Debug Switch interrupt
	and	r1,IP_7

	; Branch if it was NOT a SCSI or Debug Switch interrupt
	beqz	r1,@chain
	nop

	; Determine what actually caused the interrupt
	li	r1,INT_ICAUSE_REG
	lw	r1,0(r1)
	nop

	; Was it a debug switch interrupt ?
	and	r2,r1,DEBUG_SWITCH_INT_PENDING

	; YES - Treat it as a break point
	bnez	r2,breakex

	; Was it a SCSI generated interrupt ?
	and	r2,r1,PCI_A_INT_PENDING

	; IDE - NEED TO FURTHER DETERMINE IF SCSI OR IDE INTERRUPT

	; Branch if it was NOT a SCSI or debug switch interrupt
	beqz	r2,@chain
	nop

	; Are we already in the SCSI interrupt handler ?
	lb	r1,active-exregs(k0)
	nop

	; YES - Then go to fast interrupt exception handler
	; I beleive this is a BUG - we should NOT go to the
	; fast interrupt handler here.  We should probably indicate
	; that an exception occured within the exception handler
	bnez	r1,@chain
	nop

	; If not already in SCSI interrupt handler - then do this stuff
	; Get the SCSI interrupt status register
	li	r1,$a9000000
	lw	r1,$14(r1)
	nop

	; Check for any interrupts being generated
	and	r1,7

	; If not any - go to the fast exception handler
	beqz	r1,@chain
	nop

	; Otherwise go the the debugging service poll entry point
	b	pollentry1
	nop

;----------------------
; r1 = ex type 0 -31
; r2 = scratch

	; If ints2handle says the debugger service is supposed to handle
	; an exception that is NOT a hardware interrupt - this is were
	; we end up
@doit
	; Is the exception a break exception ?
	li	r2,9

	; Branch - NOPE
	bne	r1,r2,normalex
	nop

	; It is a break exception - read the opcode to find out what kind
	; Get the address at which the exception occurred
	mfc0	r2,C0_EPC
	nop

	; Read the opcode
	lw	r2,0(r2)
	nop

	; Shift the code field down to the low bits
	srl	r2,6

	; Is it an integer divide by 0 break ?
	subu	r1,r2,$1c00
	beq	r1,r0,intdiv0
	nop
no_user_int0

	; Is it an integer overflow (0x80000000 / 0xffffffff)
	subu	r1,r2,$1800
	beq	r1,r0,intover
	nop
no_user_into

	; Subtract of the $400 bios op code
	subu	r1,r2,$400

	; If the code is >= 0 then it is some sort of bios request from
	; the application level
	bgez	r1,bios_op
	nop

	; If the debug switch was pressed this is where we end up
	; We now simulate a breakpoint
breakex
	; Tell system exception was due to break instruction
	li	r1,9


	; If the exception was NOT a break exception or
	; we are simulating a break exception we end up here
normalex
	; Save the exception type
	sb	r1,reg_extype-exregs(k0)

	; Restore the 2 registers we have been using to this point
	ld	r1,vtemp1-exregs(k0)
	ld	r2,vtemp2-exregs(k0)

	; Save the link register
	sd	r31,reg_r31-exregs(k0)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Clear the reg_runflag
	sb	zero,reg_runflag-exregs(k0)

	; Tell system we are in the debugging service code
	li	r1,1
	sb	r1,active-exregs(k0)

	; Set a stack pointer to use
	la	sp,ex_stack

	; Write back the caches
;	jal	writeback_cache
	jal	flush_cache
	nop

	; Tell process scsi that NO interrups are pending (BDSLOT)
	move	a0,zero


	; Go process requests from the SCSI link
	jal	process_scsi
	nop


	; When process_scsi finally exits this is where it comes
restart
	; Get address of interrupts to handle save area
	la	r1,ints2handle_save

	; Get the value
	lw	r1,0(r1)

	; Get address of interrupts to handle
	la	r2,ints2handle

	; Restore the application level installed interrupt hooks
	sw	r1,0(r2)

	; Flush the caches in case new stuff got downloaded
	jal	flush_cache
	nop

	; Get the address of where the registers are stored
	la	k0,exregs

restoreregs
	li	r1,1
	sb	r1,reg_extype-exregs(k0)

	; Restore the MTHI register
	ld	t0,reg_hi-exregs(k0)
	nop
	mthi	t0

	; Restore the MTLO register
	ld	t0,reg_lo-exregs(k0)
	nop
	mtlo	t0

	; Restore the status register
	lw	t0,reg_cp0_status-exregs(k0)
	nop
	mtc0	t0,C0_SR

	; If bit 2 of the status register is set it means an exception
	; occurred while we were in the process_scsi function.  This
	; means the pc must be restored from the ERRPC register
	and	t0,1<<2
	bnez	t0,@error
	nop

	; Restore the EPC
        lw      t0,reg_pc-exregs(k0)
        nop
        mtc0    t0,C0_EPC

	; Go set the PC
	j	@setpc
	nop

	; If an exception occurred while we were in the process_scsi
	; function, the PC must be restored to the ERRPC register
@error
	lw	t0,reg_pc-exregs(k0)
	nop
	mtc0	t0,C0_ERRPC

	
	; Now restore the rest of the registers and return from whence we
	; came
@setpc
	; Get the status register and see if the FPU is enabled
	lw	t0,reg_cp0_status-exregs(k0)
	li	t1,SR_CU1
	and	t1,t0

	; If the FPU is disabled - dont bother restoring the FPU registers
	beqz	t1,@skip_fp_load
	nop

	; Restore all of the even numbered FPU registers
	ldc1	f0,reg_fgr0-exregs(k0)
	ldc1	f2,reg_fgr2-exregs(k0)
	ldc1	f4,reg_fgr4-exregs(k0)
	ldc1	f6,reg_fgr6-exregs(k0)
	ldc1	f8,reg_fgr8-exregs(k0)
	ldc1	f10,reg_fgr10-exregs(k0)
	ldc1	f12,reg_fgr12-exregs(k0)
	ldc1	f14,reg_fgr14-exregs(k0)
	ldc1	f16,reg_fgr16-exregs(k0)
	ldc1	f18,reg_fgr18-exregs(k0)
	ldc1	f20,reg_fgr20-exregs(k0)
	ldc1	f22,reg_fgr22-exregs(k0)
	ldc1	f24,reg_fgr24-exregs(k0)
	ldc1	f26,reg_fgr26-exregs(k0)
	ldc1	f28,reg_fgr28-exregs(k0)
	ldc1	f30,reg_fgr30-exregs(k0)

	; Check to see if all 32 FPU registers are in use
	li	t1,SR_FR
	and	t1,t0

	; If only 16 FPU registers dont bother restoring the odd FPU registers
	beqz	t1,@skip_fp_load
	nop

	; Restore the odd FPU registers
	ldc1	f1,reg_fgr1-exregs(k0)
	ldc1	f3,reg_fgr3-exregs(k0)
	ldc1	f5,reg_fgr5-exregs(k0)
	ldc1	f7,reg_fgr7-exregs(k0)
	ldc1	f9,reg_fgr9-exregs(k0)
	ldc1	f11,reg_fgr11-exregs(k0)
	ldc1	f13,reg_fgr13-exregs(k0)
	ldc1	f15,reg_fgr15-exregs(k0)
	ldc1	f17,reg_fgr17-exregs(k0)
	ldc1	f19,reg_fgr19-exregs(k0)
	ldc1	f21,reg_fgr21-exregs(k0)
	ldc1	f23,reg_fgr23-exregs(k0)
	ldc1	f25,reg_fgr25-exregs(k0)
	ldc1	f27,reg_fgr27-exregs(k0)
	ldc1	f29,reg_fgr29-exregs(k0)
	ldc1	f31,reg_fgr31-exregs(k0)

@skip_fp_load
	; Restore the FPU Implemtation and Revision register
	lw	t0,reg_fcr0-exregs(k0)
	nop
	ctc1	t0,fcr0

	; Restore the FPU Control and status register
	lw	t0,reg_fcr31-exregs(k0)
	nop
	ctc1	t0,fcr31

	; Restore all of the general purpose registers
	; There is no need to restore register 0 because it is a read
	; only register that always contains a value of 0.  This is also
	; no need to restore register 26 (k0) because it is the register used
	; by the exception handlers and is NEVER used anywhere else.
	ld	r1,reg_r1-exregs(k0)
	ld	r2,reg_r2-exregs(k0)
	ld	r3,reg_r3-exregs(k0)
	ld	r4,reg_r4-exregs(k0)
	ld	r5,reg_r5-exregs(k0)
	ld	r6,reg_r6-exregs(k0)
	ld	r7,reg_r7-exregs(k0)
	ld	r8,reg_r8-exregs(k0)
	ld	r9,reg_r9-exregs(k0)
	ld	r10,reg_r10-exregs(k0)
	ld	r11,reg_r11-exregs(k0)
	ld	r12,reg_r12-exregs(k0)
	ld	r13,reg_r13-exregs(k0)
	ld	r14,reg_r14-exregs(k0)
	ld	r15,reg_r15-exregs(k0)
	ld	r16,reg_r16-exregs(k0)
	ld	r17,reg_r17-exregs(k0)
	ld	r18,reg_r18-exregs(k0)
	ld	r19,reg_r19-exregs(k0)
	ld	r20,reg_r20-exregs(k0)
	ld	r21,reg_r21-exregs(k0)
	ld	r22,reg_r22-exregs(k0)
	ld	r23,reg_r23-exregs(k0)
	ld	r24,reg_r24-exregs(k0)
	ld	r25,reg_r25-exregs(k0)
	ld	r27,reg_r27-exregs(k0)
	ld	r28,reg_r28-exregs(k0)
	ld	r29,reg_r29-exregs(k0)
	ld	r30,reg_r30-exregs(k0)
	ld	r31,reg_r31-exregs(k0)

	; Set flag saying we are no longer in exception handling code
	sb	zero,active-exregs(k0)

	; Return to whence we came
	eret


	; If the exception was caused by a break instruction with a code
	; between 0x400 and 0x408, then the application is requesting some
	; service from the debugging stub and this is where we end up
bios_op
	; Check to see if the code portion of the break instruction is
	; greater than 0x408
	subu	r1,r2,$400+num_biosops+1

	; Code is greater than 0x408, so its not a bios request
	bgez	r1,breakex
	nop

;
; Parse out other BIOS ops
;
; r2=$04XX Bios Opcode
;
; $400	Poll PC
; $401	Cold Start
; $402	Warm Start
; $403	Set Int Status
; $404	Set cache
; $405	Unhook vectors from downloader stub
; $406	Set Int Vectors to be hooked by debugger
; $407	Pause
; $408	GetReinstall
; $409	HookVector

	; Get the address of the jump table
	la	r1,biosjmp

	; Subract off the 0x400 from the code
	subu	r2,$400

	; Multiply it by 4
	sll	r2,2

	; Add it to the jump table address
	addu	r2,r1

	; Read the address to jump to from the table
	lw	r2,0(r2)
	nop

	; Jump to the desired function
	jr	r2
	nop


	; BIOS services jump table
biosjmp
	; See if there are any requests from the PC pending
	dw	pollentry

	; Cold start the bios
	dw	bios_cold

	; Warm start the bios
	dw	bios_warm

	; Set interrupt status
	dw	SetIntStatus

	; Set cache
	dw	SetCache

	; Unhook vectors from downloader stub
	dw	UnHook

	; Set interrupts to service
	dw	TrapInts

	; Pause
	dw	Pause

	; Get the address of the reinstall function
	dw	GetReinstall

	; Hook in an exception vector
	dw	HookVector

	; How many BIOS services exist
num_biosops	equ	(*-biosjmp)/4


	; When an application wants to poll to see if the PC has any
	; requests pending it uses this BIOS service
pollentry
	; See if we are already in the process_function
	lb	r1,active-exregs(k0)
	nop

	; If we are - just exit
	bnez	r1,bios_exit
	nop

	; Get the interrupt status from the SCSI controller
	li	r1,$a9000000
	lw	r1,$14(r1)
	nop

	; Are there any pending interrupts from the scsi controller ?
	and	r1,7

	; NOPE - then just exit
	beqz	r1,bios_exit
	nop


	; We get here whenever an interrupt from the SCSI controller is
	; received or the poll BIOS request is made and interrupts from
	; the SCSI controller are pending
pollentry1
	; Tell system we are entering process_scsi
	li	r2,1
	sb	r2,active-exregs(k0)

	; Save the SCSI interrupt status register
	sb	r1,istat-exregs(k0)

	; Restore the registers we have trashed to this point
	ld	r1,vtemp1-exregs(k0)
	ld	r2,vtemp2-exregs(k0)

	; Save the link register
	sd	r31,reg_r31-exregs(k0)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Clear the reg_runflag
	sb	zero,reg_runflag-exregs(k0)

	; Load a stack pointer to use
	la	sp,ex_stack

	; Writback the caches
;	jal	writeback_cache
	jal	flush_cache
	nop

	; Send the SCSI interrupt status register to the process_scsi function
	lb	a0,istat-exregs(k0)

	; Go to the process_scsi function and process PC requests
	jal	process_scsi
	nop

	; When process_scsi return - go restore everything an let it go
	j	restart
	nop


	; Cold Start - not implemented yet
bios_cold
	; Warm Start - not implemented yet
bios_warm
	li	r1,-1
	j	normalex
	nop



	; Set cache status - not implemented for Phoenix
SetCache
	j	breakex
	nop


	; Hook in an application level exception vector and tell debugger
	; not to service these interrupts anymore
HookVector
	; Get address of vector table
	la	r1,exception_vectors

	; If vector number < 0 just exit
	blt	r4,r0,bios_exit

	; If vector number > 15 just exit
	srl	r2,r4,4
	bne	r2,r0,bios_exit
	move	r2,r4

	; Multiply vector number by 4 (BDSLOT)
	sll	r4,2

	; Add it to table address
	addu	r4,r1,r4

	; Put 1 in r1
	li	r1,1

	; Shift it by vector number
	sllv	r1,r1,r2

	; Get the data
	lw	r2,ints2handle-exregs(k0)

	; Make the mask
	nor	r1,r1,r0

	; Mask off the bit associated with the vector number being installed
	and	r2,r2,r1

	; Write the new mask back
	sw	r2,ints2handle-exregs(k0)

	; Put function pointer into table (BDSLOT)
	sw	r5,0(r4)

	; Done
	b	bios_exit
	nop


sys_enable:
	; Enable the interrupt
	li	r1,1		; Generate enable bit
	sll	r1,r6		
	lui	r2,$ac00	; Base address of gt64010
	lw	r4,$0c1c(r2)	; Get interrupt mask register
	or	r1,r1,r4	; Or in the new enable bit
	sw	r1,$0c1c(r2)	; Write to interrupt mask register

	; Done
	b	bios_exit
	nop

	; This is how the application level can unhook its interrupt vectors
UnHook
	; Save the link register
	sw	r31,reg_r31-exregs(k0)

	; Unhook the vectors
	jal	unhook_vectors
	nop

	; Done
	b	bios_exit

	; Restore the link register (BDSLOT)
	lw	r31,reg_r31-exregs(k0)


	; This gets called by the reboot function in process_scsi
	; It is supposed to unhook any user installed exception vectors
	; from the system.  For the moment it is not implemented
unhook_vectors
	; Get interrupts to handle mask
	lw	r2,ints2handle-exregs(k0)

	; Generate new mask for interrupts to handle
	li	r1,$ffff0200

	; Save current value of interrupts to handle
	sw	r2,ints2handle_save-exregs(k0)

	; Done
	jr	ra

	; Write it to interrupts to handle (BDSLOT)
	sw	r1,ints2handle-exregs(k0)



	; This is how the application indicates to the debugging stub
	; which interrupts it wants the debugger to NOT service.  This is
	; NOT really used.  It has been replaced with the HookVector BIOS
	; service call.
SetIntStatus
TrapInts
	j	bios_exit
	sw	r4,ints2handle-exregs(k0)


	; This is in effect lockup
Pause
	; Move past the break opcode
	mfc0	r1,C0_EPC
	nop
	addu	r1,4
	mtc0	r1,C0_EPC
	li	r1,9
	j	normalex
	nop


	; Get the address of the routine to re-install the debug hooks.
	; This is NOT used!!!
GetReinstall
	j	bios_exit
	nop

	; Exit from a bios service request
bios_exit
	; Move past the break opcode
	mfc0	r1,C0_EPC
	nop
	addu	r1,4
	mtc0	r1,C0_EPC

	; Restore the registers we have trashed to this point
	ld	r1,vtemp1-exregs(k0)
	ld	r2,vtemp2-exregs(k0)

	; Return to whence we came
	eret


	; Store all of the regisers
storeregs
	; Save the general purpose registers
	sd	r1,reg_r1-exregs(k0)
	sd	r2,reg_r2-exregs(k0)
	sd	r3,reg_r3-exregs(k0)
	sd	r4,reg_r4-exregs(k0)
	sd	r5,reg_r5-exregs(k0)
	sd	r6,reg_r6-exregs(k0)
	sd	r7,reg_r7-exregs(k0)
	sd	r8,reg_r8-exregs(k0)
	sd	r9,reg_r9-exregs(k0)
	sd	r10,reg_r10-exregs(k0)
	sd	r11,reg_r11-exregs(k0)
	sd	r12,reg_r12-exregs(k0)
	sd	r13,reg_r13-exregs(k0)
	sd	r14,reg_r14-exregs(k0)
	sd	r15,reg_r15-exregs(k0)
	sd	r16,reg_r16-exregs(k0)
	sd	r17,reg_r17-exregs(k0)
	sd	r18,reg_r18-exregs(k0)
	sd	r19,reg_r19-exregs(k0)
	sd	r20,reg_r20-exregs(k0)
	sd	r21,reg_r21-exregs(k0)
	sd	r22,reg_r22-exregs(k0)
	sd	r23,reg_r23-exregs(k0)
	sd	r24,reg_r24-exregs(k0)
	sd	r25,reg_r25-exregs(k0)
	sd	r27,reg_r27-exregs(k0)
	sd	r28,reg_r28-exregs(k0)
	sd	r29,reg_r29-exregs(k0)
	sd	r30,reg_r30-exregs(k0)

	; Save the MFLO register
	mflo	t0
	sd	t0,reg_lo-exregs(k0)

	; Save the MFHI register
	mfhi	t0
	sd	t0,reg_hi-exregs(k0)

	; Save all of the CP0 registers
i	=	0
	rept	32
	mfc0	t0,r\#i
	nop
	sd	t0,i*8+(excp0regs-exregs)(k0)
i	=	i+1
	endr


	; Check to see if the FPU is enabled and if NOT dont bother saving
	; the floating pointe registers
	lw	t0,reg_cp0_status-exregs(k0)
	li	t1,SR_CU1
	and	t1,t0
	beqz	t1,@skip_fp_store
	nop

	; Save the even FPU registers
	sdc1	f0,reg_fgr0-exregs(k0)
	sdc1	f2,reg_fgr2-exregs(k0)
	sdc1	f4,reg_fgr4-exregs(k0)
	sdc1	f6,reg_fgr6-exregs(k0)
	sdc1	f8,reg_fgr8-exregs(k0)
	sdc1	f10,reg_fgr10-exregs(k0)
	sdc1	f12,reg_fgr12-exregs(k0)
	sdc1	f14,reg_fgr14-exregs(k0)
	sdc1	f16,reg_fgr16-exregs(k0)
	sdc1	f18,reg_fgr18-exregs(k0)
	sdc1	f20,reg_fgr20-exregs(k0)
	sdc1	f22,reg_fgr22-exregs(k0)
	sdc1	f24,reg_fgr24-exregs(k0)
	sdc1	f26,reg_fgr26-exregs(k0)
	sdc1	f28,reg_fgr28-exregs(k0)
	sdc1	f30,reg_fgr30-exregs(k0)

	; Check to see if the FPU is in 32 register mode and if not
	; dont bother saving the odd FPU registers
	li	t1,SR_FR
	and	t1,t0
	beqz	t1,@skip_fp_store
	nop

	; Save the odd FPU registers
	sdc1	f1,reg_fgr1-exregs(k0)
	sdc1	f3,reg_fgr3-exregs(k0)
	sdc1	f5,reg_fgr5-exregs(k0)
	sdc1	f7,reg_fgr7-exregs(k0)
	sdc1	f9,reg_fgr9-exregs(k0)
	sdc1	f11,reg_fgr11-exregs(k0)
	sdc1	f13,reg_fgr13-exregs(k0)
	sdc1	f15,reg_fgr15-exregs(k0)
	sdc1	f17,reg_fgr17-exregs(k0)
	sdc1	f19,reg_fgr19-exregs(k0)
	sdc1	f21,reg_fgr21-exregs(k0)
	sdc1	f23,reg_fgr23-exregs(k0)
	sdc1	f25,reg_fgr25-exregs(k0)
	sdc1	f27,reg_fgr27-exregs(k0)
	sdc1	f29,reg_fgr29-exregs(k0)
	sdc1	f31,reg_fgr31-exregs(k0)

@skip_fp_store
	; Save the FPU Implementation and Revision register
	cfc1	t0,fcr0
	nop
	sw	t0,reg_fcr0-exregs(k0)

	; Save the FPU Control and status register
	cfc1	t0,fcr31
	nop
	sw	t0,reg_fcr31-exregs(k0)

	; Check to see if we got here because we took an exception while
	; we were already in the exception handlers.
	lw	t0,reg_cp0_status-exregs(k0)
	nop
	and	t0,1<<2
	bnez	t0,@error
	nop

	; NORMAL - Get the PC from the EPC
	lw	t0,reg_cp0_epc-exregs(k0)
	j	@got_pc
	nop

	; ERROR - Get the PC from the ERRPC
@error
	lw	t0,reg_cp0_errorpc-exregs(k0)
	nop

@got_pc
	; Save the PC
	sw	t0,reg_pc-exregs(k0)

	; Return to whoever called me
	jr	ra
	nop

	opt	at+,m+




	; This is a section of uncached memory used to store all sorts of
	; stuff needed by the debugger stub.  This stuff MUST be stored in
	; uncached memory because the SCSI controller DMA must be able to
	; get at the information stored here.
	;
	; NOTE - This area is also used by the fast general exception handler
	; but is accessed through a cached segment.

	section	uncached.bss

	xdef	regs_buf
	xdef	memory_buf
	xdef	command_buf
	xdef	sense_buf
	xdef	sense_badlun_buf
	xdef	message_out_buf1
	xdef	message_out_buf2
	xdef	inquiry_buf
	xdef	inquiry_badlun_buf
	xdef	buffer_table
	xdef	runstate_buf
	xdef	exregs
	xdef	reg_cp0_status
	xdef	reg_extype
	xdef	reg_runflag
	xdef	reg_cp0_cause

	; Copyright string
copy_notice	dsb	40

	; This is where all of the registers get stored when an exception
	; occurs.

	; General Purpose Registers
exregs
reg_r0		dsd	1
reg_r1		dsd	1
reg_r2		dsd	1
reg_r3		dsd	1
reg_r4		dsd	1
reg_r5		dsd	1
reg_r6		dsd	1
reg_r7		dsd	1
reg_r8		dsd	1
reg_r9		dsd	1
reg_r10		dsd	1
reg_r11		dsd	1
reg_r12		dsd	1
reg_r13		dsd	1
reg_r14		dsd	1
reg_r15		dsd	1
reg_r16		dsd	1
reg_r17		dsd	1
reg_r18		dsd	1
reg_r19		dsd	1
reg_r20		dsd	1
reg_r21		dsd	1
reg_r22		dsd	1
reg_r23		dsd	1
reg_r24		dsd	1
reg_r25		dsd	1
reg_r26		dsd	1
reg_r27		dsd	1
reg_r28		dsd	1
reg_r29		dsd	1
reg_r30		dsd	1
reg_r31		dsd	1

	; MFLO, MFHI, and PC
reg_lo		dsd	1
reg_hi		dsd	1
reg_pc		dsd	1

	; CP0 Registers
excp0regs
reg_cp0_index	dsd	1
reg_cp0_random	dsd	1
reg_cp0_entrylo0 dsd	1
reg_cp0_entrylo1 dsd	1
reg_cp0_context	dsd	1
reg_cp0_pagemask dsd	1
reg_cp0_wired	dsd	1
reg_cp0_7	dsd	1
reg_cp0_badvaddr dsd	1
reg_cp0_count	dsd	1
reg_cp0_entryhi	dsd	1
reg_cp0_compare	dsd	1
reg_cp0_status	dsd	1
reg_cp0_cause	dsd	1
reg_cp0_epc	dsd	1
reg_cp0_prid	dsd	1
reg_cp0_config	dsd	1
reg_cp0_lladdr	dsd	1
reg_cp0_watchlo	dsd	1
reg_cp0_watchhi	dsd	1
reg_cp0_20	dsd	1
reg_cp0_21	dsd	1
reg_cp0_22	dsd	1
reg_cp0_23	dsd	1
reg_cp0_24	dsd	1
reg_cp0_25	dsd	1
reg_cp0_ecc	dsd	1
reg_cp0_cacheerr dsd	1
reg_cp0_taglo	dsd	1
reg_cp0_taghi	dsd	1
reg_cp0_errorpc	dsd	1
reg_cp0_31	dsd	1

	; FPU General purpose registers
reg_fgr0	dsd	1
reg_fgr1	dsd	1
reg_fgr2	dsd	1
reg_fgr3	dsd	1
reg_fgr4	dsd	1
reg_fgr5	dsd	1
reg_fgr6	dsd	1
reg_fgr7	dsd	1
reg_fgr8	dsd	1
reg_fgr9	dsd	1
reg_fgr10	dsd	1
reg_fgr11	dsd	1
reg_fgr12	dsd	1
reg_fgr13	dsd	1
reg_fgr14	dsd	1
reg_fgr15	dsd	1
reg_fgr16	dsd	1
reg_fgr17	dsd	1
reg_fgr18	dsd	1
reg_fgr19	dsd	1
reg_fgr20	dsd	1
reg_fgr21	dsd	1
reg_fgr22	dsd	1
reg_fgr23	dsd	1
reg_fgr24	dsd	1
reg_fgr25	dsd	1
reg_fgr26	dsd	1
reg_fgr27	dsd	1
reg_fgr28	dsd	1
reg_fgr29	dsd	1
reg_fgr30	dsd	1
reg_fgr31	dsd	1

	; FPU FCR Registers
reg_fcr0	dsw	1
reg_fcr31	dsw	1

	; Assorted debugger stub control goop
reg_extype	dsb	1
		dsb	1
reg_runflag	dsb	1
		dsb	1

	; Temporary storage use during preliminary exception handling
vtemp1		dsd	1
vtemp2		dsd	1

	; Exception control Goop
ints2handle	dsw	1
ints2handle_save	dsw	1

	; Buffers used by the SCSI Controller to send/receive messages
buffer_table	dsw	32*2
regs_buf	dsw	1
memory_buf	dsb	2048
command_buf	dsb	12
sense_buf	dsb	20
sense_badlun_buf dsb	20
message_out_buf1 dsb	1
message_out_buf2 dsb	3
inquiry_buf	dsb	96
inquiry_badlun_buf dsb	96
runstate_buf	dsb	4

active		dsb	1
istat		dsb	1

user_int0_handler	dsw	1
user_into_handler	dsw	1

		dsd	1

	; Buffer used to transfer large amounts of data and top is used
	; for stack during exception handling
filebuff	dsb	32768

	; This is the top of the exception handling stack
	xdef	ex_stack
ex_stack	dsw	4	; 4 words above stack to store params

