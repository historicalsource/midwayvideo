;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 18 $
;
; $Author: Mlynch $
;
	include	"include/mon.inc"
	include	"include/cpu.inc"
	include	"include/p4000.inc"
	include	"include/gt64010.inc"
	include	"include/phoenix.inc"

SA1_SYSTEM		equ	1
SEATTLE_SYSTEM		equ	2
FLAGSTAFF_SYSTEM	equ	4
VEGAS_SYSTEM		equ	8

	if	PHOENIX_SYS=VEGAS_SYSTEM
	xref	initNile4
	else
	xref	initGt64010
	endif

        xdef    bios_start
        xdef    bios_end

ROM_SIZE	equ	$80000	; 4 MBit ROM

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
ALIGN8	macro
	opt	w-
	cnop	0,8
	opt	w+
	endm
	else
ALIGN8	macro
	endm
	endif

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
JALALIGN	macro
count	=	(*-start)
;	inform	0,"PC: %h", count
	endm
	else
JALALIGN	macro
	endm
	endif

	sectionw	.bss
ra_save	dsw	1

	sectionw	.text

;--------------------------------------------------------------
; prom entry point table
;-------------------------------------------------------------

start
	j	wmsstart	; |000| begin monitor from start
	nop
	if	~(PHOENIX_SYS=SA1_SYSTEM)
	j	post_exit	; |008| Exit from the POST (power up tests)
	else
	j	not_implem	; |010|
	endif
	nop
	j	not_implem	; |010|
	nop
	j	not_implem	; |018|
	nop
	j	not_implem	; |020|
	nop
	j	not_implem	; |028|
	nop
	j	not_implem	; |030|
	nop
	j	not_implem	; |038|
	nop
	j	not_implem	; |040|
	nop
	j	not_implem	; |048|
	nop
	j	not_implem	; |050|
	nop
	j	not_implem	; |058|
	nop
	j	not_implem	; |060|
	nop
	j	not_implem	; |068|
	nop
	j	not_implem	; |070|
	nop
	j	not_implem	; |078|
	nop
	j	not_implem	; |080|
	nop
	j	not_implem	; |088|
	nop
	j	not_implem	; |090|
	nop
	j	not_implem	; |098|
	nop
	j	not_implem	; |0A0|
	nop
	j	not_implem	; |0A8|
	nop
	j	not_implem	; |0B0|
	nop
	j	not_implem	; |0B8|
	nop
	j	not_implem	; |0C0|
	nop
	j	not_implem	; |0C8|
	nop
	j	not_implem	; |0D0|
	nop
	j	not_implem	; |0D8|
	nop
	j	not_implem	; |0E0|
	nop
	j	not_implem	; |0E8|
	nop
	j	not_implem	; |0F0|
	nop
	j	not_implem	; |0F8|
	nop
	j	not_implem	; |100|
	nop
	j	not_implem	; |108|
	nop
	j	not_implem	; |110|
	nop
	j	not_implem	; |118|
	nop
	j	not_implem	; |120|
	nop
	j	not_implem	; |128|
	nop
	j	not_implem	; |130|
	nop
	j	not_implem	; |138|
	nop
	j	not_implem	; |140|
	nop
	j	not_implem	; |148|
	nop
	j	not_implem	; |150|
	nop
	j	not_implem	; |158|
	nop
	j	not_implem	; |160|
	nop
	j	not_implem	; |168|
	nop
	j	not_implem	; |170|
	nop
	j	not_implem	; |178|
	nop
	j	not_implem	; |180|
	nop
	j	not_implem	; |188|
	nop
	j	not_implem	; |190|
	nop
	j	not_implem	; |198|
	nop
	j	not_implem	; |1A0|
	nop
	j	not_implem	; |1A8|
	nop
	j	not_implem	; |1B0|
	nop
	j	not_implem	; |1B8|
	nop
	j	not_implem	; |1C0|
	nop
	j	not_implem	; |1C8|
	nop
	j	not_implem	; |1D0|
	nop
	j	not_implem	; |1D8|
	nop
	j	not_implem	; |1E0|
	nop
	j	not_implem	; |1E8|
	nop
	j	not_implem	; |1F0|
	nop
	j	not_implem	; |1F8|
	nop
	j	tlberr
	nop
	j	not_implem	; |208|
	nop
	j	not_implem	; |210|
	nop
	j	not_implem	; |218|
	nop
	j	not_implem	; |220|
	nop
	j	not_implem	; |228|
	nop
	j	not_implem	; |230|
	nop
	j	not_implem	; |238|
	nop
	j	not_implem	; |240|
	nop
	j	not_implem	; |248|
	nop
	j	not_implem	; |250|
	nop
	j	not_implem	; |258|
	nop
	j	not_implem	; |260|
	nop
	j	not_implem	; |268|
	nop
	j	not_implem	; |270|
	nop
	j	not_implem	; |278|
	nop
	j	xtlberr
	nop
	j	not_implem	; |288|
	nop
	j	not_implem	; |290|
	nop
	j	not_implem	; |298|
	nop
	j	not_implem	; |2A0|
	nop
	j	not_implem	; |2A8|
	nop
	j	not_implem	; |2B0|
	nop
	j	not_implem	; |2B8|
	nop
	j	not_implem	; |2C0|
	nop
	j	not_implem	; |2C8|
	nop
	j	not_implem	; |2D0|
	nop
	j	not_implem	; |2D8|
	nop
	j	not_implem	; |2E0|
	nop
	j	not_implem	; |2E8|
	nop
	j	not_implem	; |2F0|
	nop
	j	not_implem	; |2F8|
	nop
	j	cerr
	nop
	j	not_implem	; |308|
	nop
	j	not_implem	; |310|
	nop
	j	not_implem	; |318|
	nop
	j	not_implem	; |320|
	nop
	j	not_implem	; |328|
	nop
	j	not_implem	; |330|
	nop
	j	not_implem	; |338|
	nop
	j	not_implem	; |340|
	nop
	j	not_implem	; |348|
	nop
	j	not_implem	; |350|
	nop
	j	not_implem	; |358|
	nop
	j	not_implem	; |360|
	nop
	j	not_implem	; |368|
	nop
	j	not_implem	; |370|
	nop
	j	not_implem	; |378|
	nop
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	j	not_implem	; |378|
	else
	j	oerr
	endif
	nop

	ALIGN8
wmsstart
	mtc0	r0,C0_LLADDR		; Clear lladdr reg
	mfc0	v0,C0_SR		; Get Status reg
	srl	v0,20			; Shift SR bit down
	mtc0	v0,C0_LLADDR		; Store in lladdr reg

	li	v0,SR_CU1|SR_BEV|SR_DE
	mtc0	v0,C0_SR		; state unknown on reset
	mtc0	zero,C0_CAUSE		; clear software interrupts

	li	v0,CFG_C_NONCOHERENT	; initialise default cache mode
	mtc0	v0,C0_CONFIG

	ctc1	zero,fcr31		; inialize the FPU CSR

	; Give the DRAM 120us to get started
	li	v0,128	; ~256us

	ALIGN8
@1
	bne	v0,zero,@1
	subu	v0,1	; BDSLOT

	if	PHOENIX_SYS=VEGAS_SYSTEM

	inform	0,"Nile IV Initialization"
	jal	initNile4
	nop

	else

	inform	0,"GT64010 Initializtion"
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	initGt64010
	nop
	endif

	; Vegas systems - LED's come up in unknown state so turn em on then
	; off
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_off
	nop
	li	r4,0
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop


	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	; NOW ACTUALLY DETERMINE THE DRAM CONFIGURATION
	li	r3,$300
	sw	r3,GAL_DRAM_CFG(r2)
	or	r9,r0,r0

	la	r8,(GAL_REG_BASE+GAL_DRAM0_CFG)|$A0000000
	la	r4,$a0000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	dramtest
	nop
	or	r9,r0,r2

	la	r8,(GAL_REG_BASE+GAL_DRAM1_CFG)|$A0000000
	la	r4,$a1000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	dramtest
	nop
	sll	r2,2
	or	r9,r2,r2

	la	r8,(GAL_REG_BASE+GAL_DRAM2_CFG)|$A0000000
	la	r4,$a4000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	dramtest
	nop
	sll	r2,4
	or	r9,r2,r2

	la	r8,(GAL_REG_BASE+GAL_DRAM3_CFG)|$A0000000
	la	r4,$a6000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif


	JALALIGN
	jal	dramtest
	nop
	sll	r2,6
	or	r27,r2,r9
	endif
	; END OF MEMORY CONFIGURATION DETERMINATION


	; Verify that memory can be accessed
	li	v0,K1BASE		; Address
	li	t0,$aaaa5555		; Data pattern
	sw	t0,0(v0)		; Write it
	sw	zero,M_BUSWIDTH(v0)	; scratch the bus
	lw	t1,0(v0)		; Read back
	nop
	beq	t1,t0,@3		; is memory accessable ? br = yes
	nop


	; FATAL ERROR - MEMORY IS NOT ACCESSIBLE
	ALIGN8
@2
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_flash
	nop
	b	@2
	nop

	; MEMORY IS OK SO FAR - CONTINUE CHECK
	ALIGN8
@3
	li	t0,-1			; Data pattern
	sw	t0,M_BUSWIDTH*2(v0)	; Write it
	sw	zero,M_BUSWIDTH(v0)	; scratch the bus
	lw	t1,M_BUSWIDTH*2(v0)	; Read back
	nop
	bne	t0,t1,@2		; is memory accessible ? br = NO
	nop

	; This is were very low level diagnostics the determine fatal
	; hardware failues should occur.
	li	r4,1
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	rom_checksum
	nop

	; Do low level board initialization

	li	r4,2
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	sbdinit
	nop

	; Put out something on the LEDS
	li	r4,3
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop

	; Initialize the memory
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	initmem
	nop

	; Put out something on the LEDS
	li	r4,4
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	test_memory
	nop

	li	r4,5
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop

	if	~(PHOENIX_SYS=SA1_SYSTEM)
	la	r9,post_start
	andi	r9,~$20000000
	la	r10,post_end
	andi	r10,~$20000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	bios_checksum
	nop
	mtc1	r2,f8
	jal	move_post
	nop

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	flush_cache
	nop
	la	r9,post_start
	la	r10,post_end
	subu	r10,r10,r9
	la	r9,$80000000
	ori	r10,$80000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	bios_checksum
	nop
	mfc1	r4,f8
	nop
	nop
	beq	r2,r4,post_ok
	nop
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_flash
	nop
	ALIGN8
post_ok
	; Set the stack pointer
	mfc1	r29,f0
	andi	r29,~$20000000

	; Jump to the POST
	la	r27,0x80004000		# Start address of POST
	jr	r27			# Jump to post
	nop
	ALIGN8
post_exit
	mfc0	r2,r12			# Disable the interrupts
	andi	r2,~1
	mtc0	r2,r12
	endif

	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	la	r8,$a1600003
	sb	r0,0(r8)
	li	r9,10000
rst_wait
	bne	r9,r0,rst_wait
	subi	r9,1
	li	r2,$ff
	sb	r2,0(r8)
	endif	

	; Copy the BIOS to its DRAM location
	la	r9,bios_start
	if	(PHOENIX_SYS=VEGAS_SYSTEM)	
	andi	r9,~$20000000
	endif
	la	r10,bios_end
	if	(PHOENIX_SYS=VEGAS_SYSTEM)	
	andi	r10,~$20000000
	endif

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	bios_checksum
	nop
	mtc1	r2,f8

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	move_bios
	nop

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	flush_cache
	nop

	la	r9,bios_start
	la	r10,bios_end
	subu	r10,r10,r9
	la	r9,$80000000
	ori	r10,$80000000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	bios_checksum
	nop
	mfc1	r4,f8
	nop
	nop
	beq	r2,r4,bios_ok
	nop
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	jal	led_flash
	nop
	endif

	ALIGN8
bios_ok
	; Put out something on the LEDS
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_off
	nop

	; Set the stack pointer
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	la	r29,$80800000
	endif

	; Jump to the BIOS
	la	r27,0x80004000		# Start address of BIOS
	jr	r27			# Goto it
	nop

;
; not_implem - just does a return with FALSE
	xdef	not_implem

	ALIGN8
not_implem

	j	ra
	li	v0,0	; return zero

;
; initmem -- config and init cache,clear prom bss
; clears memory between PROM_STACK-$2000 and PROM_STACK-4 inclusive

	ALIGN8
initmem

	la	sp,ra_save
	sw	ra,0(sp)

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	flush_cache
	nop

	lw	ra,0(sp)
	nop
	j	ra
	nop

	ALIGN8
dramtest

	or	r2,r0,r0		; Set DRAM type to 64 bit

	; test 32/64 bit DRAM
	; set 64 wide,even bank?,1K refresh
	li	r3,$50
	sw	r3,0(r8)
	sync

	; write a consecutive $55555555 $aaaaaaaa and check it
	li	r5,$55555555
	sw	r5,0(r4)		; should go in the even bank
	li	r7,$aaaaaaaa
	sw	r7,4(r4)		; should go in the odd bank

	sw	r0,8(r4)
	sw	r0,12(r4)
	sync

	lw	r6,0(r4)
	nop
	bne	r5,r6,Dram32bit	; is the even data there ??
	nop				; branch delay
	lw	r6,4(r4)
	nop
	beq	r7,r6,Dramdone	; is the odd data there ??
	nop

	ALIGN8
Dram32bit:

	ori	r2,r0,1		; Set DRAM type to 32 bit (even)
	li	r3,$10
	sw	r3,0(r8)		; 1k,even,32 bit
	sync
	sw	r5,0(r4)
	li	r3,$c0edbabe		; scratch the bus!
	sw	r3,4(r4)
	lw	r6,0(r4)
	nop
	beq	r5,r6,Dramdone	; is even ok? if no,check odd
	nop

	ALIGN8
Dram32odd:

	ori	r2,r0,2		; Set DRAM type to 32 bit (odd)
	li	r3,$90
	sw	r3,0(r8)		; 1k,odd,32 bit
	nop
	sw	r5,0(r4)
	li	r3,$c0edbabe		; scratch the bus!
	sw	r3,4(r4)
	lw	r6,0(r4)
	nop
	beq	r5,r6,Dramdone	; odd ok? br = yes
	nop
	ori	r2,r0,3			; Set DRAM type 3 - none (BD)

	ALIGN8
Dramdone:

	jr	r31
	nop

; lowlevel initialisation called at ROM startup and reinitialisation
; All registers can be used but no memory!


	ALIGN8
sbdinit

	; slight amount of kludgery here to stop RAM resident
	; program from overwriting itself...

	li	v1,$1fc00000		; check return address is in ROM
	and	v0,ra,v1
	bne	v0,v1,@9
	nop

	; explicitly clear parity in bottom 64K for cache initialisation

	la	t0,$a0000000
	la	t5,$a0010000

	ALIGN8
@4
	sd	zero,0(t0)
	addu	t0,16
	bne	t0,t5,@4
	sd	zero,-8(t0)

	; now we can safely initialises the R4000 caches

	move	t9,ra			; we know t9 isn't used!!

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	_init_cache
	nop
	move	ra,t9

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	j	@mpdone
	nop
	endif

	; we *must* now size and clear all of memory,so that it
	; has good parity

	la	t0,$a0000000
	li	t2,$aa55aa55
	not	t3,t2

	ALIGN8
@memprobe
	move	t4,t0

	; store pattern in bank 0,line 0
	sw	t2,0(t4)
	addu	t4,4

	; fill remainder of line with zeros
	li	t5,M_BUSWIDTH-4

	ALIGN8
@1

	sw	zero,0(t4)
	subu	t5,4
	addu	t4,4
	bnez	t5,@1
	nop

	; store inverse pattern in bank 0,line 1
	sw	t3,0(t4)
	addu	t4,4

	; fill remainder of line with zeros
	li	t5,M_BUSWIDTH-4

	ALIGN8
@2
	sw	zero,0(t4)
	subu	t5,4
	addu	t4,4
	bnez	t5,@2
	nop

	; defeat write buffering
	lw	zero,-4(t4)

	; retrieve pattern and check
	lw	t4,0(t0)
	addu	t0,$100000		; check in Megabyte chunks
	beq	t4,t2,@memprobe
	nop

	; back up pointer
	subu	t5,t0,$100000

	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	ALIGN8
@mpdone
	la	t0,$a0000000
	la	t5,$a0800000
	endif

	; point to start of dram,and check for zero memory size!
	la	t0,$a0000000
	beq	t0,t5,@9

	; Save the memory size
	mtc1	t5,f0
	
	; force memory clear loop to run cached
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	la	t1,@3
	li	t2,$20000000
	xor	t1,t1,t2
	jr	t1
	nop
	endif

	; XXX could use "sd" on R4000 for even faster clear

	ALIGN8
@3
	sd	zero,0(t0)
	sd	zero,8(t0)
	sd	zero,16(t0)
	sd	zero,24(t0)
	sd	zero,32(t0)
	sd	zero,40(t0)
	sd	zero,48(t0)
	addu	t0,64
	bne	t0,t5,@3
	sd	zero,-8(t0)		; BDSLOT

	ALIGN8
@9
	nop

	jr	ra
	nop

CACHE_LINE_SIZE	equ	32
CACHE_SIZE	equ	32768

addr		equr	t0
maxaddr		equr	t1
mask		equr	t2

cacheop	macro	kva,n,linesize,op
	local	l10,l11

	; check for bad size

	blez	\n,l11
	nop

	addu	maxaddr,\kva,\n

	; align to line boundaries

	subu	mask,linesize,1
	not	mask
	and	addr,\kva,mask
	addu	maxaddr,-1
	and	maxaddr,mask

	; the cacheop loop

	ALIGN8
l10

	cache	\op,0(addr)
	bne	addr,maxaddr,l10
	addu	addr,\linesize

	ALIGN8
l11

	endm


; static void _size_cache()
;
; Internal routine to determine cache sizes by looking at R4000 config
; register. Sizes are returned in registers,as follows:


icachesize	equr	t2
dcachesize	equr	t3
scachesize	equr	t4
ilinesize	equr	t5
dlinesize	equr	t6
slinesize	equr	t7
pcache2way	equr	t8

SCACHE_MIN_SIZE	equ	$0020000	; minimum scache: 128Kb
SCACHE_MAX_SIZE	equ	$1000000	; maximum scache: 16Mb

; void _init_cache()

	ALIGN8
_init_cache

	; The caches may be in an indeterminate state,
	; so we force good parity into them by doing an
	; invalidate,load/fill,invalidate for each line.


	mfc0	v0,C0_SR
	li	v1,SR_BEV
	and	v1,v0
	or	v1,SR_DE

	mtc0	v1,C0_SR
	nop
	nop
	mtc0	zero,C0_TAGLO			; initial cache tag
	nop
	nop

	; Assume bottom of RAM will generate good parity for the
	; primary caches (max 32K)

	; Initialise primary instruction cache.

	li	a0,K0BASE
	addu	a1,a0,CACHE_SIZE		; limit = base+icachesize

	ALIGN8
@1

	addu	a0,CACHE_LINE_SIZE
	cache	Index_Store_Tag_I,-4(a0)	; clear tag
	nop
	cache	Fill_I,-4(a0)			; fill line
	nop
	bne	a0,a1,@1
	cache	Index_Store_Tag_I,-4(a0)	; BDSLOT: clear tag

	; Initialise primary data cache.
	; (for R4600 2-way set caches,we do it in 3 passes).

	; 1: initialise dcache tags

	li	a0,K0BASE
	addu	a1,a0,CACHE_SIZE	; limit = base+dcachesize

	ALIGN8
@2

	addu	a0,CACHE_LINE_SIZE
	bne	a0,a1,@2
	cache	Index_Store_Tag_D,-4(a0)	; BDSLOT: clear tag

	; 2: fill dcache

	li	a0,K0BASE
	addu	a1,a0,CACHE_SIZE		; limit = base+dcachesize

	ALIGN8
@3

	addu	a0,CACHE_LINE_SIZE
	bne	a0,a1,@3
	lw	zero,-4(a0)			; BDSLOT: fill line

	; 3: clear dcache tags

	li	a0,K0BASE
	addu	a1,a0,CACHE_SIZE	; limit = base+dcachesize

	ALIGN8
@4

	addu	a0,CACHE_LINE_SIZE
	bne	a0,a1,@4
	cache	Index_Store_Tag_D,-4(a0)	; BDSLOT: clear tag

	mtc0	v0,C0_SR
	j	ra
	nop


;
; void flush_cache (void)
;
; Flush and invalidate all caches

	ALIGN8
flush_cache

	; secondary cacheops do all the work if present

	li	a0,K0BASE

	; flush primary caches individually

	li	a1,CACHE_SIZE
	li	a2,CACHE_LINE_SIZE
	cacheop	a0,a1,a2,Index_Writeback_Inv_D

	li	a1,CACHE_SIZE
	li	a2,CACHE_LINE_SIZE
	cacheop	a0,a1,a2,Index_Invalidate_I

	j	ra
	nop


	; Move the BIOS from the ROM to RAM
	ALIGN8
move_bios
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	la	r8,move_bios1
	li	r9,$20000000
	xor	r8,r8,r9
	jr	r8
	nop
	ALIGN8
move_bios1
	endif

	la	r8,bios_start	; Source address of OS in EPROM
	la	r10,bios_end	; End address of OS in EPROM

	beq	r8,r10,@2

	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	; Cached accesses from the ROM
	nop
	xor	r8,r8,r9
	xor	r10,r10,r9
	endif

	la	r9,$80000000

	ALIGN8
@1
	lw	r11,0(r8)	; Read a word
	addi	r8,4		; Increment source address
	sw	r11,0(r9)	; Write it
	addi	r9,4		; Increment destination address
	bne	r8,r10,@1	; Done ? - br = NO
	nop
	ALIGN8
@2
	j	ra		; Retrun to however call me
	nop


	if	~(PHOENIX_SYS=SA1_SYSTEM)
move_post
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	la	r8,move_post1
	li	r9,$20000000
	xor	r8,r8,r9
	jr	r8
	nop
move_post1
	endif
	la	r8,post_start	; Source address of OS in EPROM
	la	r10,post_end	; End address of OS in EPROM

	beq	r8,r10,@2

	; Cached accesses from the ROM
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	nop
	xor	r8,r8,r9
	xor	r10,r10,r9
	endif

	la	r9,$80000000

	ALIGN8
@1
	lw	r11,0(r8)	; Read a word
	addi	r8,4		; Increment source address
	sw	r11,0(r9)	; Write it
	addi	r9,4		; Increment destination address
	bne	r8,r10,@1	; Done ? - br = NO
	nop

	ALIGN8
@2
	j	ra		; Retrun to however call me
	nop
	endif


	; r9 = start
	; r10 = end
	; r2 = sum
	ALIGN8
	xdef	bios_checksum
bios_checksum
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	la	r8,bios_checksum1
	andi	r8,~$20000000
	jr	r8
	nop
	ALIGN8
	xdef	bios_checksum1
bios_checksum1
	endif
	move	r2,r0
	ALIGN8
	xdef	bios_checkloop
bios_checkloop
	lbu	r3,0(r9)
	addiu	r9,1
	addu	r2,r2,r3
	bne	r9,r10,bios_checkloop
	nop
	jr	ra
	nop
	

	ALIGN8
test_memory
	la	r3,test_memory1
	li	r2,~$20000000
	and	r3,r3,r2
	jr	r3
	nop
	ALIGN8
test_memory1
	li	r10,1			; Loop counter
	li	r2,1			; Starting data
	ALIGN8
write_loop_start
	bne	r10,r0,wl1		; Done looping ?
	subi	r10,1			; Decrement loop count (BDSLOT)
	jr	r31			; Return if done
	nop
	ALIGN8
wl1
	move	r4,r2			; Save starting data for read loop
	la	r3,$80000000		; Starting address
	mfc1	r5,f0			; Get ending address
	andi	r5,~$20000000		; Make cached
	ALIGN8
write_loop
	sb	r2,0(r3)		; Write the data
	nop
	addi	r3,1			; Increment the address
	beq	r3,r5,read_loop_start	; Go do read pass
	nop
	addi	r2,1			; Increment the data
	andi	r2,$ff			; Mask data
	bne	r2,r0,write_loop	; BR = data != 0
	nop
	li	r2,1			; Reset data to 0
	b	write_loop		; Go write next byte
	nop
	ALIGN8
read_loop_start
	move	r2,r4			; Get starting data for compare
	la	r3,$80000000		; Starting address
	mfc1	r5,f0			; Get ending address
	andi	r5,~$20000000		; Make cached
	ALIGN8
read_loop
	lbu	r6,0(r3)		; Read the data
	nop
	bne	r6,r2,tm_error		; BR = ERROR
	nop
	addi	r3,1			; Increment address
	beq	r3,r5,write_loop_start	; Go do another write pass
	nop
	addi	r2,1			; Increment the data
	andi	r2,$ff			; Mask data
	bne	r2,r0,read_loop		; BR = data != 0
	nop
	li	r2,1			; Reset data to 0
	b	read_loop
	nop

	; Test memory error
	ALIGN8
tm_error
	la	r2,led_flash		; Get address of led_flash
	j	r2			; Jump to it (uncached)
	nop
	

	ALIGN8
cerr
	li	r4,8
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop
	j	led_flash
	nop

	ALIGN8
tlberr
	li	r4,6
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop
	j	led_flash
	nop

	ALIGN8
xtlberr
	li	r4,7
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop
	j	led_flash
	nop

	ALIGN8
oerr
	li	r4,9
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_digit
	nop
	j	led_flash
	nop


	; Turn off the 7 Segment display
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
led_off
	opt	at-
	la	r2,LED_ADDR
	li	r1,$ff
	jr	r31
	sb	r1,0(r2)
	opt	at+
	else
	ALIGN8
led_off
	opt	at-
	la	r2,LED_ADDR
	li	r1,$7
	jr	r31
	sw	r1,0(r2)
	opt	at+
	endif

	; Turn on the 7 Segment display
	ALIGN8
led_on
	la	r2,LED_ADDR
	jr	r31
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	sb	r0,0(r2)
	else
	sw	r0,0(r2)
	endif


	; Put a digit out on the 7 segment display
	ALIGN8
led_digit
	opt	at-
	la	r2,LED_ADDR
	la	r1,led_data
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	andi	r4,$f
	else
	andi	r4,$7
	endif
	sll	r4,2
	addu	r1,r1,r4
	lw	r4,0(r1)
	jr	r31
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	sb	r4,0(r2)
	else
	sw	r4,0(r2)
	endif
	opt	at+

	; Flash the 7 segment display with whatever it is current displaying
	opt	at-
led_flash
	la	r2,LED_ADDR
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	lb	r5,0(r2)
	else
	lw	r5,0(r2)
	endif
	ALIGN8
led_flp
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)	
	opt	w-
	cnop	0,8
	opt	w+
	endif

	JALALIGN
	jal	led_off
	nop
	li	r1,40000
	ALIGN8
loff_dly
	bne	r1,r0,loff_dly
	subi	r1,1
	la	r2,LED_ADDR
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	sb	r5,0(r2)
	else
	sw	r5,0(r2)
	endif
	li	r1,40000
	ALIGN8
lon_dly
	bne	r1,r0,lon_dly
	subi	r1,1
	b	led_flash
	nop 
	opt	at+


	ALIGN8
rom_checksum
	la	r2,$9fc00000		; Base address of rom (Cached)
	la	r3,($9fc80000-4)
	andi	r3,~$20000000		; Make it cached
	addiu	r3,3			; Make sure it ends on word boundary
	andi	r3,~3
	move	r5,r0			; Current sum
	la	r4,rom_chk_lp		; Get address of routine
	andi	r4,~$20000000		; Make it cached
	jr	r4			; Jump to it
	nop
	ALIGN8
rom_chk_lp
	beq	r2,r3,rom_chk		; End of check ? BR = YES
	nop
	lbu	r4,0(r2)		; Get byte from ROM
	addiu	r2,1			; Incrment the address (BDSLOT)
	b	rom_chk_lp		; Get next byte
	addu	r5,r5,r4		; Accumulate the sum
	ALIGN8
rom_chk
	lw	r3,0(r2)		; Grab stored checksum
	nop
	bne	r3,r5,rom_error		; Check sum failure
	nop
	jr	r31
	nop
	ALIGN8
rom_error
	opt	at-
	la	r1,led_flash		; Get address of led_flash
	jr	r1			; Jump to it
	nop
	opt	at+
	

	; Array of data for 7 segment display
	section	.data
led_data
	if	(PHOENIX_SYS=VEGAS_SYSTEM)	
	dw	~$3f
	dw	~$06
	dw	~$5b
	dw	~$4f
	dw	~$66
	dw	~$6d
	dw	~$7d
	dw	~$27
	dw	~$7f
	dw	~$67
	dw	~$77
	dw	~$7c
	dw	~$39
	dw	~$5e
	dw	~$79
	dw	~$71
	else
	dw	~$0
	dw	~$1
	dw	~$2
	dw	~$3
	dw	~$4
	dw	~$5
	dw	~$6
	dw	~$7
	endif
