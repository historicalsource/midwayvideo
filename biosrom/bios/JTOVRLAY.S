;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 2 $
;
; $Author: Mlynch $
;
	section	.text

	xref	sys_jtable
	xref	sys_jt_end
	xref	flush_cache

	xdef	overlay_jtable
overlay_jtable
	addiu	r29,-4
	sw	r31,0(r29)
	la	r8,sys_jtable
	la	r9,sys_jt_end
	la	r10,$a0000000
@1
	beq	r8,r9,@2
	lw	r11,0(r8)
	addi	r8,4
	sw	r11,0(r10)
	b	@1
	addi	r10,4
@2
	jal	flush_cache
	nop

	lw	r31,0(r29)
	addiu	r29,4
	jr	r31
	nop
