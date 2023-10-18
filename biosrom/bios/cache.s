;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 4 $
;
; $Author: Mlynch $
;
;/*
; * R4000 cache operations
; */


Index_Invalidate_I	equ	$0         ;/* 0       0 */
Index_Writeback_Inv_D	equ	$1         ;/* 0       1 */
Index_Invalidate_SI	equ	$2         ;/* 0       2 */
Index_Writeback_Inv_SD	equ	$3         ;/* 0       3 */
Index_Load_Tag_I	equ	$4         ;/* 1       0 */
Index_Load_Tag_D	equ	$5         ;/* 1       1 */
Index_Load_Tag_SI	equ	$6         ;/* 1       2 */
Index_Load_Tag_SD	equ	$7         ;/* 1       3 */
Index_Store_Tag_I	equ	$8         ;/* 2       0 */
Index_Store_Tag_D	equ	$9         ;/* 2       1 */
Index_Store_Tag_SI	equ	$A         ;/* 2       2 */
Index_Store_Tag_SD	equ	$B         ;/* 2       3 */
Create_Dirty_Exc_D	equ	$D         ;/* 3       1 */
Create_Dirty_Exc_SD	equ	$F         ;/* 3       3 */
Hit_Invalidate_I	equ	$10        ;/* 4       0 */
Hit_Invalidate_D	equ	$11        ;/* 4       1 */
Hit_Invalidate_SI	equ	$12        ;/* 4       2 */
Hit_Invalidate_SD	equ	$13        ;/* 4       3 */
Hit_Writeback_Inv_D	equ	$15        ;/* 5       1 */
Hit_Writeback_Inv_SD	equ	$17        ;/* 5       3 */
Fill_I			equ	$14        ;/* 5       0 */
Hit_Writeback_D		equ	$19        ;/* 6       1 */
Hit_Writeback_SD	equ	$1B        ;/* 6       3 */
Hit_Writeback_I		equ	$18        ;/* 6       0 */
Hit_Set_Virtual_SI	equ	$1E        ;/* 7       2 */
Hit_Set_Virtual_SD	equ	$1F        ;/* 7       3 */



DCACHE_LINESIZE	equ	32
ICACHE_LINESIZE	equ	DCACHE_LINESIZE
SCACHE_LINESIZE	equ	DCACHE_LINESIZE

	if	def(R5000)
ICACHE_SIZE	equ	32768
	else
ICACHE_SIZE	equ	16384
	endif
	inform	0,"Cache Size: %d", ICACHE_SIZE
DCACHE_SIZE	equ	ICACHE_SIZE

addr		equs	"t0"
maxaddr		equs	"t1"
mask		equs	"t2"

cacheop	macro	kva, n, linesize, op, lab
	blez	\n,@11_\lab
	addu	\maxaddr,\kva,\n
	subu	mask,\linesize,1
	not	mask
	and	addr,\kva,mask
	addu	maxaddr,-1
	and	maxaddr,mask
@10_\lab
	cache	\op,0(addr)
	bne	addr,maxaddr,@10_\lab
	addu	addr,\linesize
@11_\lab
	endm
	
;/* virtual cache op: no limit on size of region */
vcacheop	macro	kva, n, linesize, op, lab
	cacheop	\kva, \n, \linesize, \op, \lab
	endm

;/* indexed cache op: region limited to cache size */
icacheop	macro	kva, n, linesize, op, lab
	move	t3,\n
	bltu	\n,\size,@12_\lab
	move	t3,\size
@12_\lab
	cacheop	\kva, t3, \linesize, \op, \lab
	endm



	section	.text

;/*
; * void clean_cache (unsigned kva, size_t n)
; *
; * Writeback and invalidate address range in all caches
; */
	xdef	clear_cache
clear_cache:
	li	a2,DCACHE_LINESIZE
	vcacheop	a0,a1,a2,Hit_Writeback_Inv_D, 1

	li	a2,ICACHE_LINESIZE
	vcacheop	a0,a1,a2,Hit_Invalidate_I, 2

	j	ra
	nop

;/*
; * void flush_cache (void)
; *
; * Flush and invalidate all caches
; */
	xdef	flush_cache
flush_cache:
	;/* secondary cacheops do all the work if present */
	la	a0,0x80000000

	;/* flush primary caches individually */
	li	a1,DCACHE_SIZE
	li	a2,DCACHE_LINESIZE
	cacheop	a0,a1,a2,Index_Writeback_Inv_D, 3

	li	a1,ICACHE_SIZE
	li	a2,ICACHE_LINESIZE
	cacheop	a0,a1,a2,Index_Invalidate_I, 4

	j	ra
	nop


	xdef	flush_data_cache
flush_data_cache:
	;/* secondary cacheops do all the work if present */
	la	a0,0x80000000

	;/* flush primary caches individually */
	li	a1,DCACHE_SIZE
	li	a2,DCACHE_LINESIZE
	cacheop	a0,a1,a2,Index_Writeback_Inv_D, 5

	j	ra
	nop


; Writeback contents of cache
	xdef	writeback_cache
writeback_cache

	; secondary cacheops do all the work if present

	li	a0,$80000000

	; flush primary caches individually

	li	a1,DCACHE_SIZE
	li	a2,DCACHE_LINESIZE
	cacheop	a0,a1,a2,Index_Writeback_Inv_D, 6

	j	ra
	nop


; Invalidate Instruction cache
	xdef	invalidate_icache
invalidate_icache
	li	a0,$80000000

	li	a1,ICACHE_SIZE
	li	a2,ICACHE_LINESIZE

	cacheop	a0,a1,a2,Index_Invalidate_I, 7

	jr	ra
	nop

; Invalidate data cache
	xdef	invalidate_dcache
invalidate_dcache
	li	a0,$80000000

	li	a1,DCACHE_SIZE
	li	a2,DCACHE_LINESIZE

	cacheop	a0,a1,a2,Index_Writeback_Inv_D, 8

	jr	ra
	nop


; Hit Invalidate data cache
	; void hit_invalidate_dcache(void *addr, int bytes);
	xdef	hit_invalidate_dcache
hit_invalidate_dcache
	li	a2,DCACHE_LINESIZE

	cacheop	a0,a1,a2,Hit_Invalidate_D, 9

	jr	ra
	nop


; Hit writeback & invalidate data cache
	; void hit_writeback_invalidate_dcache(void *addr, int bytes);
	xdef	hit_writeback_invalidate_dcache
hit_writeback_invalidate_dcache
	li	a2,DCACHE_LINESIZE

	cacheop	a0,a1,a2,Hit_Writeback_Inv_D, 10

	jr	ra
	nop

; Hit writeback data cache
	; void hit_writeback_dcache(void *addr, int bytes);
	xdef	hit_writeback_dcache
hit_writeback_dcache
	li	a2,DCACHE_LINESIZE

	cacheop	a0,a1,a2,Hit_Writeback_D, 11

	jr	ra
	nop
