; $Revision: 2 $
	section	.data

	xdef	goose_pshell_s_version
goose_pshell_s_version
	db	"$Revision: 2 $",0

	xref	cur_proc
	xref	die

	section	.text

	; r4 = argument ptr
	; r5 = function ptr
	xdef	_proc_shell
_proc_shell:
	la	r8,cur_proc
	lw	r8,0(r8)		; Get current process pointer
	nop

	;
	; NOTE - if the process structure changes this offset will
	; also need to be changed
	;
;;;	lw	r9,2080(r8)		; Get entry point address
	lw	r9,36(r8)		; Get entry point address

;;;	addi	r4,r8,16		; Get pointer to arguments
	lw	r4,16(r8)
	jalr	r9
	nop
	j	die			; If it returns - die
	li	r4,1
