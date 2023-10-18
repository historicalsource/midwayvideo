# $Revision: 1 $
	.section	data

#	xdef	goose_pshell_s_version
goose_pshell_s_version:
	.asciz	"$Revision: 1 $"

#	xref	cur_proc
#	xref	die

	.section	text

#	r4 = argument ptr
#	r5 = function ptr
#	xdef	_proc_shell
_proc_shell:
	la	$8,cur_proc
	lw	$8,0($8)		# Get current process pointer
	nop

#	
#	NOTE - if the process structure changes this offset will
#	also need to be changed
#	
#	lw	$9,2080($8)		# Get entry point address
	lw	$9,36($8)		# Get entry point address

#	addi	$4,$8,16		# Get pointer to arguments
1:	lw	$4,16($8)
	jalr	$9
	nop
	j	die			# If it returns - die
	li	$4,1

