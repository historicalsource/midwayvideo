;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 1 $
;
; $Author: Mlynch $
;
	section	.text

	xdef	reset_count
	xdef	get_count_reg

reset_count
	jr	r31
	mtc0	r0,r9

get_count_reg
	jr	r31
	mfc0	r2,r9
