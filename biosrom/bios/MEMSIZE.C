int __memory_size = 0;


__asm__("
	.set	noreorder
	.globl	get_mem_size
get_mem_size:
	la		$2,__memory_size
	lw		$3,0($2)
	nop
	bne	$3,$0,noset
	mfc1	$4,$f0
	andi	$4,0x1fffffff
	sw		$4,0($2)
noset:
	jr		$31
	nop
	.set	reorder
");


