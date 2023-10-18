;****************************************************************************/
;*                                                                          */
;* vexcept.s - Low level exception handling.                                */
;*                                                                          */
;* $Author: Mlynch $                                                               */
;*                                                                          */
;* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
;* All Rights Reserved                                                      */
;*                                                                          */
;* Use, duplication, or disclosure is strictly forbidden unless approved,   */
;* in writing by Williams Electronics Games Inc.                            */
;*                                                                          */
;* $Revision: 4 $                                                             */
;*                                                                          */
;****************************************************************************/

	opt	w-,at-,m-

	section	.text

	xref	general_exception
	xref	tlb_exception
	xref	xtlb_exception
	xref	cache_error_exception
	xref	debug_service
	xref	syscall_table

	xref	show_epc

	xdef	exregs
	xdef	ex_stack
	xdef	memcpy_fast
	xdef	TLBExceptionHandler
	xdef	XTLBExceptionHandler
	xdef	CacheErrorExceptionHandler
	xdef	exception
	xdef	enable_interrupts
	xdef	disable_interrupts
	xdef	get_cause
	xdef	enable_ip
	xdef	disable_ip

	xdef	reg_cp0_epc

C0_CAUSE        equr    13
C0_EPC		equr	14
C0_SR		equr	12
C0_ERRPC	equr	30

SR_CU1                  equ     $20000000
SR_FR                   equ     $04000000

TLBExceptionHandler:
	; Save the link register
	sd	r31,reg_r31-exregs(r26)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Call the C level handler
	jal	tlb_exception

	; Send the C level handler the cause register (bdslot)
	lw	r4,reg_cp0_cause-exregs(r26)

	; TLB exception handler returns pointer to where to restore
	; registers from
	; Restore the registers and return from exception
	j	crestoreregs
	move	r26,r2


XTLBExceptionHandler:
	; Save the link register
	sd	r31,reg_r31-exregs(r26)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Call the C level handler
	jal	xtlb_exception

	; Send the C level handler the cause register (bdslot)
	lw	r4,reg_cp0_cause-exregs(r26)

	; General exception handler returns pointer to where to restore
	; registers from
	; Restore the registers and return from exception
	j	crestoreregs
	move	r26,r2
	

CacheErrorExceptionHandler:
	; Save the link register
	sd	r31,reg_r31-exregs(r26)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Call the C level handler
	jal	cache_error_exception

	; Send the C level handler the cause register (bdslot)
	lw	r4,reg_cp0_cause-exregs(r26)

	; General exception handler returns pointer to where to restore
	; registers from
	; Restore the registers and return from exception
	j	crestoreregs
	move	r26,r2


	xdef	setup_debug_service
setup_debug_service
	; Get the FPCSR and turn off any FP causes
	cfc1	r2,r31
	li	r2,$3f000
	not	r2
	ctc1	r2,r31

	; Grab the status register
	mfc0	r2,C0_SR

	; Turn off ERL and EXL bits
	; Turn on the IE bit
	li	r3,$6
	not	r3
	and	r2,r2,r3
	ori	r2,1

	; Return to caller
	jr	r31

	; Write back status register (bdslot)
	mtc0	r2,C0_SR

exception
	mfc0	r26,r12
	srl	r26,1
	sll	r26,1
	mtc0	r26,r12

	la	r26,exc_stack			; Get address to store regs
	sd	r8,reg_r8-exregs(r26)		; Save r8
	mfc0	r8,r13				; Get cause
	srl	r8,2				; Shift to low bits
	andi	r8,$1f				; Mask un-needed bits
	addi	r8,-8				; Subtract syscall cause code
	bne	r8,r0,not_syscall		; br = not syscall
	ld	r8,reg_r8-exregs(r26)		; Restore r8 (bdslot)

	mfc0	r2,r14				; Grab EPC

	lw	r2,0(r2)			; Get instruction
	sra	r2,6				; Shift code to low bits
	blt	r2,r0,syscall_error		; br = code is negative

	subi	r9,r2,53			; Is this set_handler ?

	sll	r2,2				; Generate table offset
	la	r3,syscall_table		; Get address of table
	addu	r3,r2,r3			; Add in offset
	lw	r24,0(r3)			; Get function pointer
	beq	r24,r0,syscall_error		; br = NULL function pointer
	nop

	beq	r9,r0,no_int_enbl		; br = set_handler

	mfc0	r2,r12				; Get status reg
	li	r3,$6				; Turn off EXL and ERL
	not	r3
	and	r2,r2,r3
	ori	r2,1				; Enable interrupts
	mtc0	r2,r12				; Write back status register

no_int_enbl
	jr	r24				; Go do system call
	nop

	; If the syscall handler returns error
	; we end up here and treat the syscall as an exception
	; Restore r8
syscall_error
	lw	r2,reg_r2-exregs(r26)
	lw	r3,reg_r3-exregs(r26)
	lw	r8,reg_r8-exregs(r26)
	lw	r9,reg_r9-exregs(r26)

not_syscall
	; Save the link register
	sd	r31,reg_r31-exregs(r26)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Call the C level handler
	jal	general_exception

	; Send the C level handler the cause register (bdslot)
	ld	r4,reg_cp0_cause-exregs(r26)

	; General exception handler returns pointer to where to restore
	; registers from
	; Restore the registers and return from exception
	move	r26,r2
	j	crestoreregs
	nop


enable_interrupts:
	mfc0	r8,r12		; Get the status register
	ori	r8,r8,1		; Turn on interrupt enable bit
	jr	r31		; Return to caller
	mtc0	r8,r12		; Write back the status register


disable_interrupts:
	mfc0	r8,r12		; Get the status register
	li	r9,$fffffffe	; Turn off the interrupt enable bit
	and	r8,r8,r9
	jr	r31		; Return to caller
	mtc0	r8,r12		; Write back the status register

get_cause:
	jr	r31		; Return to caller
	mfc0	r2,r13		; Get the cause register

enable_ip:
	mfc0	r8,r12		; Get the status register
	or	r8,r8,r4	; Enable requested interrupts
	ori	r8,1		; Make sure interrupt enable is on
	jr	r31		; Return to caller
	mtc0	r8,r12		; Write back status register

disable_ip:
	mfc0	r8,r12		; Get status register
	li	r9,$ffffffff
	xor	r9,r9,r4	; Turn off all requested interrupt bits
	move	r2,r8		; Send back current interrupt enables
	and	r8,r8,r9	; Turn off the requested interrupts
	jr	r31		; Return to caller
	mtc0	r8,r12		; Write back the status register


	xdef	crestoreregs
crestoreregs
	; Restore the MTHI register
	ld	t0,reg_hi-exregs(k0)
	nop
	mthi	t0

	; Restore the MTLO register
	ld	t0,reg_lo-exregs(k0)
	nop
	mtlo	t0

	; Restore the status register
	ld	t0,reg_cp0_status-exregs(k0)
	nop
	
	; NEW
	ori	t0,1
	; NEW END

	mtc0	t0,C0_SR

	; If bit 2 of the status register is set it means the exception was
	; caused by either a cache error exception, a reset exception
	; (power up, cold, or warm), or a NMI exception.  In this case, the
	; PC gets put into the ErrorEPC.
	and	t0,1<<2
	bnez	t0,@use_error
	nop

	; Restore the EPC
        ld      t0,reg_pc-exregs(k0)
        nop
        mtc0    t0,C0_EPC

	; PC set - done
	j	@pcset
	nop

@use_error
	; Restore the ErrorEPC
	ld	t0,reg_pc-exregs(k0)
	nop
	mtc0	t0,C0_ERRPC

	
	; Now restore the rest of the registers and return from whence we
	; came
@pcset
	; Get the status register and see if the FPU is enabled
	ld	t0,reg_cp0_status-exregs(k0)
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

	sync
	sync

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
	sd	r26,reg_r26-exregs(k0)
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
	mfc0	t0,C0_SR
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

	; If bit 2 of the status register is set it means that we got here
	; because of either a cache error exception, reset exception
	; (power up, cold, or warm), or a NMI.  If this is the case, the
	; PC is taken from the ErrorEPC register, otherwise, it is taken
	; from the EPC.
	mfc0	t0,C0_SR
	nop
	and	t0,1<<2
	bnez	t0,@error
	nop

	; NORMAL - Get the PC from the EPC
	ld	t0,reg_cp0_epc-exregs(k0)
	j	@got_pc
	nop

	; ERROR - Get the PC from the ERRPC
@error
	ld	t0,reg_cp0_errorpc-exregs(k0)
	nop

@got_pc
	; Save the PC
	sd	t0,reg_pc-exregs(k0)

	; Restore t0 and t1
	ld	t0,reg_r8-exregs(k0)
	ld	t1,reg_r9-exregs(k0)

	sync
	sync

	; Return to whoever called me
	jr	ra
	nop


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
	xdef	filebuff
	xdef	reg_fcr31
	xdef	active
	xdef	istat

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



	section	.bss
	xdef	exc_stack
	xdef	exc_stack_end
exc_stack	dsb	1024
exc_stack_end	dsb	8
