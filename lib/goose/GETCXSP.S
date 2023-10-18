	section	.data

	xdef	goose_getcxsp_s_version
goose_getcxsp_s_version
	db	"$Revision: 2 $",0

	section	.text

	xdef	get_count_xor_sp
get_count_xor_sp:
	mfc0	r2,r29
	jr	r31
	xor	r2,r2,r29
