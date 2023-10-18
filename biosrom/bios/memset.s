;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 1 $
;
; $Author: Mlynch $
;
	section	.text

	xdef	memset

memset
	beq	r0,r6,@1
	addi	r4,1
	sb	r5,-1(r4)
	b	memset
	subi	r6,1
@1
	jr	r31
	nop
	
	
