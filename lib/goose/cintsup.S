	.section	data

	.global	goose_intsup_s_version
goose_intsup_s_version:
	.asciz	"$Revision: 1 $"

	.if	0
	.section	text
	.global	enable_ip
enable_ip:
#	Get the current value of the status register
	mfc0	$8,$12

#	Or in the value we passed in
	or	$8,$8,$4

#	Or in the interrupt enable bit
	ori	$8,1

#	Write back the status register
	mtc0	$8,$12

#	return
	jr	$31
	nop
	.endif

