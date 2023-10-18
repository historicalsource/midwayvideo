;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 2 $
;
; $Author: Mlynch $
;
	section	.text

	xdef	memcpy

memcpy
	beq	r0,r6,@1
	lb	r8,0(r5)
	addi	r5,1
	sb	r8,0(r4)
	addi	r4,1
	b	memcpy
	subi	r6,1
@1
	jr	r31
	nop
	
	
