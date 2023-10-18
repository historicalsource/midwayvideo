;****************************************************************************/
;*                                                                          */
;* except.mip - Low level routines for exception handling.                  */
;*                                                                          */
;* Written by:  Michael J. Lynch                                            */
;* Version:     1.00                                                        */
;* Date:        10/11/95                                                    */
;*                                                                          */
;* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
;* All Rights Reserved                                                      */
;*                                                                          */
;* Use, duplication, or disclosure is strictly forbidden unless approved,   */
;* in writing by Williams Electronics Games Inc.                            */
;*                                                                          */
;****************************************************************************/

	opt	w-,at-,m-

	section	.text

	xref	storeregs
	xref	ex_stack
	xref	general_exception

	xdef	TLBExceptionHandler
	xdef	XTLBExceptionHandler
	xdef	CacheErrorExceptionHandler
	xdef	fast_general_exception_handler
	xdef	enable_interrupts
	xdef	disable_interrupts
	xdef	get_cause
	xdef	enable_ip
	xdef	disable_ip

C0_CAUSE        equr    13
C0_EPC		equr	14
C0_SR		equr	12
C0_ERRPC	equr	30

SR_CU1                  equ     $20000000
SR_FR                   equ     $04000000

TLBExceptionHandler:
	j	fast_general_exception_handler
	nop


XTLBExceptionHandler:
	j	fast_general_exception_handler
	nop
	

CacheErrorExceptionHandler:
	j	fast_general_exception_handler
	nop


fast_general_exception_handler
	; Get address of where to store the registers (cached)
	la	r26,cexregs

	; Save the link register
	sd	r31,creg_r31-cexregs(r26)

	; Save the rest of the registers
	jal	storeregs
	nop

	; Get the address of the regs too
	move	r5,r26

	; Set up a stack to use (cached)
	la	r29,(ex_stack&$dfffffff)

	; Get the cause to send to C handler
	mfc0	r4,r13

	; Go to the C level handler
	jal	general_exception
	nop

	; Get address of stored registers (cached)
	la	r26,cexregs

	; Restore the registers and retur from whence we came
	j	crestoreregs
	nop


enable_interrupts:
	mfc0	r8,r12
	nop
	ori	r8,r8,1
	mtc0	r8,r12
	nop
	jr	r31
	nop


disable_interrupts:
	mfc0	r8,r12
	li	r9,$fffffffe
	and	r8,r8,r9
	mtc0	r8,r12
	nop
	jr	r31
	nop

get_cause:
	mfc0	r2,r13
	nop
	jr	r31
	nop

enable_ip:
	mfc0	r8,r12
	nop
	or	r8,r8,r4
	ori	r8,1
	mtc0	r8,r12
	nop
	jr	r31
	nop

disable_ip:
	mfc0	r8,r12
	li	r9,$ffffffff
	xor	r9,r9,r4
	move	r2,r8
	and	r8,r8,r9
	mtc0	r8,r12
	nop
	jr	r31
	nop


crestoreregs
	; Restore the MTHI register
	ld	t0,creg_hi-cexregs(k0)
	nop
	mthi	t0

	; Restore the MTLO register
	ld	t0,creg_lo-cexregs(k0)
	nop
	mtlo	t0

	; Restore the status register
	lw	t0,creg_cp0_status-cexregs(k0)
	nop
	mtc0	t0,C0_SR

	; If bit 2 of the status register is set it means an exception
	; occurred while we were in the process_scsi function.  This
	; means the pc must be restored from the ERRPC register
	and	t0,1<<2
	bnez	t0,@error
	nop

	; Restore the EPC
        lw      t0,creg_pc-cexregs(k0)
        nop
        mtc0    t0,C0_EPC

	; Go set the PC
	j	@setpc
	nop

	; If an exception occurred while we were in the process_scsi
	; function, the PC must be restored to the ERRPC register
@error
	lw	t0,creg_pc-cexregs(k0)
	nop
	mtc0	t0,C0_ERRPC

	
	; Now restore the rest of the registers and return from whence we
	; came
@setpc
	; Get the status register and see if the FPU is enabled
	lw	t0,creg_cp0_status-cexregs(k0)
	li	t1,SR_CU1
	and	t1,t0

	; If the FPU is disabled - dont bother restoring the FPU registers
	beqz	t1,@skip_fp_load
	nop

	; Restore all of the even numbered FPU registers
	ldc1	f0,creg_fgr0-cexregs(k0)
	ldc1	f2,creg_fgr2-cexregs(k0)
	ldc1	f4,creg_fgr4-cexregs(k0)
	ldc1	f6,creg_fgr6-cexregs(k0)
	ldc1	f8,creg_fgr8-cexregs(k0)
	ldc1	f10,creg_fgr10-cexregs(k0)
	ldc1	f12,creg_fgr12-cexregs(k0)
	ldc1	f14,creg_fgr14-cexregs(k0)
	ldc1	f16,creg_fgr16-cexregs(k0)
	ldc1	f18,creg_fgr18-cexregs(k0)
	ldc1	f20,creg_fgr20-cexregs(k0)
	ldc1	f22,creg_fgr22-cexregs(k0)
	ldc1	f24,creg_fgr24-cexregs(k0)
	ldc1	f26,creg_fgr26-cexregs(k0)
	ldc1	f28,creg_fgr28-cexregs(k0)
	ldc1	f30,creg_fgr30-cexregs(k0)

	; Check to see if all 32 FPU registers are in use
	li	t1,SR_FR
	and	t1,t0

	; If only 16 FPU registers dont bother restoring the odd FPU registers
	beqz	t1,@skip_fp_load
	nop

	; Restore the odd FPU registers
	ldc1	f1,creg_fgr1-cexregs(k0)
	ldc1	f3,creg_fgr3-cexregs(k0)
	ldc1	f5,creg_fgr5-cexregs(k0)
	ldc1	f7,creg_fgr7-cexregs(k0)
	ldc1	f9,creg_fgr9-cexregs(k0)
	ldc1	f11,creg_fgr11-cexregs(k0)
	ldc1	f13,creg_fgr13-cexregs(k0)
	ldc1	f15,creg_fgr15-cexregs(k0)
	ldc1	f17,creg_fgr17-cexregs(k0)
	ldc1	f19,creg_fgr19-cexregs(k0)
	ldc1	f21,creg_fgr21-cexregs(k0)
	ldc1	f23,creg_fgr23-cexregs(k0)
	ldc1	f25,creg_fgr25-cexregs(k0)
	ldc1	f27,creg_fgr27-cexregs(k0)
	ldc1	f29,creg_fgr29-cexregs(k0)
	ldc1	f31,creg_fgr31-cexregs(k0)

@skip_fp_load
	; Restore the FPU Implemtation and Revision register
	lw	t0,creg_fcr0-cexregs(k0)
	nop
	ctc1	t0,fcr0

	; Restore the FPU Control and status register
	lw	t0,creg_fcr31-cexregs(k0)
	nop
	ctc1	t0,fcr31

	; Restore all of the general purpose registers
	; There is no need to restore register 0 because it is a read
	; only register that always contains a value of 0.  This is also
	; no need to restore register 26 (k0) because it is the register used
	; by the exception handlers and is NEVER used anywhere else.
	ld	r1,creg_r1-cexregs(k0)
	ld	r2,creg_r2-cexregs(k0)
	ld	r3,creg_r3-cexregs(k0)
	ld	r4,creg_r4-cexregs(k0)
	ld	r5,creg_r5-cexregs(k0)
	ld	r6,creg_r6-cexregs(k0)
	ld	r7,creg_r7-cexregs(k0)
	ld	r8,creg_r8-cexregs(k0)
	ld	r9,creg_r9-cexregs(k0)
	ld	r10,creg_r10-cexregs(k0)
	ld	r11,creg_r11-cexregs(k0)
	ld	r12,creg_r12-cexregs(k0)
	ld	r13,creg_r13-cexregs(k0)
	ld	r14,creg_r14-cexregs(k0)
	ld	r15,creg_r15-cexregs(k0)
	ld	r16,creg_r16-cexregs(k0)
	ld	r17,creg_r17-cexregs(k0)
	ld	r18,creg_r18-cexregs(k0)
	ld	r19,creg_r19-cexregs(k0)
	ld	r20,creg_r20-cexregs(k0)
	ld	r21,creg_r21-cexregs(k0)
	ld	r22,creg_r22-cexregs(k0)
	ld	r23,creg_r23-cexregs(k0)
	ld	r24,creg_r24-cexregs(k0)
	ld	r25,creg_r25-cexregs(k0)
	ld	r27,creg_r27-cexregs(k0)
	ld	r28,creg_r28-cexregs(k0)
	ld	r29,creg_r29-cexregs(k0)
	ld	r30,creg_r30-cexregs(k0)
	ld	r31,creg_r31-cexregs(k0)

	; Return to whence we came
	eret


	section	.data

	; General Purpose Registers
cexregs
creg_r0		dsd	1
creg_r1		dsd	1
creg_r2		dsd	1
creg_r3		dsd	1
creg_r4		dsd	1
creg_r5		dsd	1
creg_r6		dsd	1
creg_r7		dsd	1
creg_r8		dsd	1
creg_r9		dsd	1
creg_r10		dsd	1
creg_r11		dsd	1
creg_r12		dsd	1
creg_r13		dsd	1
creg_r14		dsd	1
creg_r15		dsd	1
creg_r16		dsd	1
creg_r17		dsd	1
creg_r18		dsd	1
creg_r19		dsd	1
creg_r20		dsd	1
creg_r21		dsd	1
creg_r22		dsd	1
creg_r23		dsd	1
creg_r24		dsd	1
creg_r25		dsd	1
creg_r26		dsd	1
creg_r27		dsd	1
creg_r28		dsd	1
creg_r29		dsd	1
creg_r30		dsd	1
creg_r31		dsd	1

	; MFLO, MFHI, and PC
creg_lo		dsd	1
creg_hi		dsd	1
creg_pc		dsd	1

	; CP0 Registers
excp0regs
creg_cp0_index	dsd	1
creg_cp0_random	dsd	1
creg_cp0_entrylo0 dsd	1
creg_cp0_entrylo1 dsd	1
creg_cp0_context	dsd	1
creg_cp0_pagemask dsd	1
creg_cp0_wired	dsd	1
creg_cp0_7	dsd	1
creg_cp0_badvaddr dsd	1
creg_cp0_count	dsd	1
creg_cp0_entryhi	dsd	1
creg_cp0_compare	dsd	1
creg_cp0_status	dsd	1
creg_cp0_cause	dsd	1
creg_cp0_epc	dsd	1
creg_cp0_prid	dsd	1
creg_cp0_config	dsd	1
creg_cp0_lladdr	dsd	1
creg_cp0_watchlo	dsd	1
creg_cp0_watchhi	dsd	1
creg_cp0_20	dsd	1
creg_cp0_21	dsd	1
creg_cp0_22	dsd	1
creg_cp0_23	dsd	1
creg_cp0_24	dsd	1
creg_cp0_25	dsd	1
creg_cp0_ecc	dsd	1
creg_cp0_cacheerr dsd	1
creg_cp0_taglo	dsd	1
creg_cp0_taghi	dsd	1
creg_cp0_errorpc	dsd	1
creg_cp0_31	dsd	1

	; FPU General purpose registers
creg_fgr0	dsd	1
creg_fgr1	dsd	1
creg_fgr2	dsd	1
creg_fgr3	dsd	1
creg_fgr4	dsd	1
creg_fgr5	dsd	1
creg_fgr6	dsd	1
creg_fgr7	dsd	1
creg_fgr8	dsd	1
creg_fgr9	dsd	1
creg_fgr10	dsd	1
creg_fgr11	dsd	1
creg_fgr12	dsd	1
creg_fgr13	dsd	1
creg_fgr14	dsd	1
creg_fgr15	dsd	1
creg_fgr16	dsd	1
creg_fgr17	dsd	1
creg_fgr18	dsd	1
creg_fgr19	dsd	1
creg_fgr20	dsd	1
creg_fgr21	dsd	1
creg_fgr22	dsd	1
creg_fgr23	dsd	1
creg_fgr24	dsd	1
creg_fgr25	dsd	1
creg_fgr26	dsd	1
creg_fgr27	dsd	1
creg_fgr28	dsd	1
creg_fgr29	dsd	1
creg_fgr30	dsd	1
creg_fgr31	dsd	1

	; FPU FCR Registers
creg_fcr0	dsw	1
creg_fcr31	dsw	1


