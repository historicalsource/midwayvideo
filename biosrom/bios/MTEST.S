;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 1 $
;
; $Author: Mlynch $
;
	section	.text

	xdef	test_memory
	xdef	mt_stack
	xref	memory_test

test_memory:
	; Save off the current stack pointer
	move	r8,r29

	; Set the stack somewhere where it won't get overwritten
	la	r29,mt_stack

	; Save the link register
	move	r9,r31

	; Go run the memory test
	jal	memory_test
	nop

	; Put the stack back
	move	r29,r8

	; Restore the link register
	move	r31,r9

	; Return to the fool that called us
	jr	r31
	nop

	section	.bss
	dsb	1024
mt_stack
