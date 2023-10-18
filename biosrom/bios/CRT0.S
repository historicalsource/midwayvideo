;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 15 $
;
; $Author: Mlynch $
;
SA1_SYSTEM		equ	1
SEATTLE_SYSTEM		equ	2
FLAGSTAFF_SYSTEM	equ	4
VEGAS_SYSTEM		equ	8

bss	group	bss
ubss	group	bss

	if	PHOENIX_SYS=SA1_SYSTEM
LED_ADDR	equ	$b5100010
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
LED_ADDR	equ	$b7900000
	elseif	PHOENIX_SYS=VEGAS_SYSTEM
LED_ADDR	equ	$a1600000
	endif

	section	.text

	xdef	start		; Only needed by the linker

	xref	init_io		; This is in conf.c
        xref    exception       ; This is in psyqdbg.s
        xref    flush_cache	; This is in cache.s
	xref	overlay_jtable	; This is in jtovrlay.s
	xref	main		; This is in main.c
	xref	setup_tlb	; This is in tlb.s
	xref	post_test	; This in in cputest.c

	xref	exc_stack_end

	; Include the system jump table
	include	"jtable.inc"

	; This is the start of the actual program BIOS code
	; IF THIS CHANGES - The address that is jumped to after the code
	; in csu.s copies the BIOS code to ram must be changed.  The address
	; is calculated as follows:
	; The first value in the dcb below is added to $80000000.

	dcb	$4000-(*-sys_jtable),0
start
        li      r8,$30010001    ; Set the status register
	mtc0	r8,r12		; Write the status register
	nop
	nop

	ctc1	r0,r31		; Initialize the FP CSR
	li	r2,$01000400	; Flush denormalized results to zero, enable div0
	ctc1	r2,r31

	; Put a code out on the LEDs
	la	r8,LED_ADDR
	if	PHOENIX_SYS=SA1_SYSTEM
	li	r9,$f
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	li	r9,3
	elseif	PHOENIX_SYS=VEGAS_SYSTEM
	li	r4,$a
	jal	led_digit
	nop
	endif
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	sw	r9,0(r8)
	endif

	; Clear out the bss space
	la	r8,group(bss)
	la	r9,groupend(bss)
        beq     r8,r9,no_bss
        nop
clear_loop
	sb	r0,0(r8)
	addiu	r8,1
	bne	r8,r9,clear_loop
	nop
no_bss

	; Put a code out on the LEDs
	la	r8,LED_ADDR
	if	PHOENIX_SYS=SA1_SYSTEM
	li	r9,$1f
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	li	r9,2
	elseif	PHOENIX_SYS=VEGAS_SYSTEM
	li	r4,$b
	jal	led_digit
	nop
	endif
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	sw	r9,0(r8)
	endif

	; Clear out the uncached bss space
	la	r8,group(ubss)
	la	r9,groupend(ubss)
	beq	r8,r9,no_ubss
	nop
uclear_loop
	sb	r0,0(r8)
	addiu	r8,1
	bne	r8,r9,uclear_loop
	nop
no_ubss

	; Check to see if this code is running at $800c0000 and if it is
	; overlay the jump table at $80000000 with the jump table from this
	; code.
	la	r8,start
	li	r9,$c0000
	and	r8,r8,r9
	beq	r8,r0,romd_code
	nop
	jal	overlay_jtable
	nop

	; Flush the caches
romd_code
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	li	r4,$c
	jal	led_digit
	nop
	endif

        jal     flush_cache
        nop

	if	PHOENIX_SYS=VEGAS_SYSTEM
	; Release the resets
	la	r8,$a1600003
	li	r2,$ff
	sb	r2,0(r8)

	la	r26,exc_stack_end

	endif

	jal	post_test
	nop

	; Initialize the drivers
	jal	init_io
	nop

	; Initialize the TLB's
	jal	setup_tlb
	nop

	; Call the main function
	jal     main
	nop

not_implem
	jr	r31		; Return 0
	move	r2,r0


	; Turn off the 7 Segment display
	xdef	led_off
led_off
	opt	at-
	la	r2,LED_ADDR
	li	r1,$ff
	jr	r31
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	sb	r1,0(r2)
	else
	sw	r1,0(r2)
	endif
	opt	at+

	; Turn on the 7 Segment display
	xdef	led_on
led_on
	la	r2,LED_ADDR
	jr	r31
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	sb	r0,0(r2)
	else
	sw	r0,0(r2)
	endif


	; Put a digit out on the 7 segment display
	xdef	led_digit
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
	xdef	led_flash
led_flash
	la	r2,LED_ADDR
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	lb	r5,0(r2)
	else
	lw	r5,0(r2)
	endif
led_flp
	jal	led_off
	nop
	li	r1,10000000
loff_dly
	bne	r1,r0,loff_dly
	subi	r1,1
	la	r2,LED_ADDR
	if	(PHOENIX_SYS=VEGAS_SYSTEM)
	sb	r5,0(r2)
	else
	sw	r5,0(r2)
	endif
	li	r1,10000000
lon_dly
	bne	r1,r0,lon_dly
	subi	r1,1
	b	led_flash
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
