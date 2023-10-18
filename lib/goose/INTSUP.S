	section	.data

	xdef	goose_intsup_s_version
goose_intsup_s_version
	db	"$Revision: 2 $",0

	if	0
	section	.text
	xdef	enable_ip
enable_ip
	; Get the current value of the status register
	mfc0	r8,r12

	; Or in the value we passed in
	or	r8,r8,r4

	; Or in the interrupt enable bit
	ori	r8,1

	; Write back the status register
	mtc0	r8,r12

	; return
	jr	r31
	nop
	endif
