	.section	data

	.global		goose_getcxsp_s_version
goose_getcxsp_s_version:
	.asciz	"$Revision: 1 $"

	.section	text
		
	.global		get_count_xor_sp
get_count_xor_sp:
	mfc0	$2,$29
	jr	$31
	xor	$2,$2,$29
