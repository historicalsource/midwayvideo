	section	.data

	xdef	goose_qhook_s_version
goose_qhook_s_version:
	db	"$Revision: 3 $",0

	section	.text

	xref	cur_proc
	xref	suspend_self
	xref	resume_process

	xdef	disk_queue_hook
disk_queue_hook
	la	r4,cur_proc
	la	r5,suspend_self
	la	r6,resume_process
	break	$40a
	jr	r31
	nop


;	xdef	install_disk_callback
;install_disk_callback
;	break	$40e
;	jr	r31
;	nop
