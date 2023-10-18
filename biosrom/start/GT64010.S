;
; Copyright (c) 1997 by Midway Video Inc.
;
; $Revision: 1 $
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

	sectionw	.text

	xdef	initGt64010
initGt64010
	; GALILEO PCI CONTROLLER INITIALIZATION
	; set up galileo base register
	la	r2,GAL_RESET_REG_BASE
	li	r3,$60
	sw	r3,$68(r2)

	; load new base address
	la	r2,(GAL_REG_BASE)|$A0000000

	; Set up DRAM banks 0 and 1 base address at 0
	sw	r0,GAL_RAS10_LOW(r2)

	; Set up DRAM banks 0 and 1 for 64 Meg total
	li	r3,$1f
	sw	r3,GAL_RAS10_HIGH(r2)

	; Set up DRAM bank 0 at address 0
	sw	r0,GAL_RAS0_LOW(r2)

	; Set up DRAM bank 0 size to 12 Meg
	li	r3,$0b
	sw	r3,GAL_RAS0_HIGH(r2)

	; Set up DRAM bank 1 at address $1000000
	li	r3,$10
	sw	r3,GAL_RAS1_LOW(r2)

	; Set up DRAM bank 1 size to 48 Meg
	li	r3,$3f
	sw	r3,GAL_RAS1_HIGH(r2)

	; Set up DRAM banks 2 and 3 at base address $4000000
	li	r3,$20
	sw	r3,GAL_RAS32_LOW(r2)

	; Set up DRAM banks 2 and 3 for 64 Meg total
	li	r3,$3f
	sw	r3,GAL_RAS32_HIGH(r2)

	; Set up DRAM bank 2 at address $4000000
	li	r3,$40
	sw	r3,GAL_RAS2_LOW(r2)

	; Set up DRAM bank 2 size to 32 Meg
	li	r3,$5f
	sw	r3,GAL_RAS2_HIGH(r2)

	; Set up DRAM bank 3 at address $6000000
	li	r3,$60
	sw	r3,GAL_RAS3_LOW(r2)

	; Set up DRAM bank 3 size to 32 Meg
	li	r3,$7f
	sw	r3,GAL_RAS3_HIGH(r2)

	; set up PCI I/O base address at $a000000
	li	r3,$50
	sw	r3,GAL_PCIIO_LOW(r2)

	; set up PCI I/O size to 32 Meg
	li	r3,$5f
	sw	r3,GAL_PCIIO_HIGH(r2)

	; set up PCI memory base address at $8000000
	li	r3,$40
	sw	r3,GAL_PCIM_LOW(r2)

	; set up PCI memory size to 32 Meg
	li	r3,$4f
	sw	r3,GAL_PCIM_HIGH(r2)

	; set up CS[2:0] base address at $10000000
	li	r3,$80
	sw	r3,GAL_CS210_LOW(r2)

	if	PHOENIX_SYS=SA1_SYSTEM
	; set up CS[2:0] top address at $17ffffff
	li	r3,$3f
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	; set up CS[2:0] top address at $15ffffff
	li	r3,$2f
	endif
	sw	r3,GAL_CS210_HIGH(r2)

	if	PHOENIX_SYS=SA1_SYSTEM
	; set up CS3 & BootCS base address at $18000000
	li	r3,$c0
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	; set up CS3 & BootCS base address at $16000000
	li	r3,$b0
	endif
	sw	r3,GAL_CS3B_LOW(r2)

	; set up CS3 & BootCS top address to $1fffffff
	li	r3,$7f
	sw	r3,GAL_CS3B_HIGH(r2)

	; set up CS0 base to $10000000
	sw	r0,GAL_CS0_LOW(r2)

	; set up CS0 top to $11ffffff
	li	r3,$1f
	sw	r3,GAL_CS0_HIGH(r2)

	; set up CS1 base to $12000000
	li	r3,$20
	sw	r3,GAL_CS1_LOW(r2)

	; set up CS1 top to $13ffffff
	li	r3,$3f
	sw	r3,GAL_CS1_HIGH(r2)

	; set up CS2 base to $14000000
	li	r3,$40
	sw	r3,GAL_CS2_LOW(r2)

	; set up CS2 top to $15ffffff
	li	r3,$5f
	sw	r3,GAL_CS2_HIGH(r2)

	if	PHOENIX_SYS=SA1_SYSTEM
	; set up CS3 base to $18000000
	li	r3,$80
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	; set up CS3 base to $16000000
	li	r3,$60
	endif
	sw	r3,GAL_CS3_LOW(r2)

	if	PHOENIX_SYS=SA1_SYSTEM
	; set up CS3 top to $1fbfffff
	li	r3,$fb
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	; set up CS3 top to $17ffffff
	li	r3,$7f
	endif
	sw	r3,GAL_CS3_HIGH(r2)

	; set up CS0 and CS2 and for:
	; turnOff = 1,AccToFirst = 4,AccToNext = 2,ADStoWr = 2,WrActive = 2
	; WrHigh = 1,DevWidth = 2 (32 bits),DevLoc = 0,LatchFunct = 0
	; Parity = 0 (off)

	li	r3,0<<30+0<<25+0<<23+2<<20+1<<17+2<<14+2<<11+2<<7+4<<3+1
	sw	r3,GAL_CS0_CFG(r2)
	sw	r3,GAL_CS2_CFG(r2)

	; set up CS1 and CS3 and for:
	; turnOff = 1,AccToFirst = 2,AccToNext = 2,ADStoWr = 1,WrActive = 1
	; WrHigh = 1,DevWidth = 2 (32 bits),DevLoc = 1 (odd bank),
	; LatchFunct = 0,Parity = 0 (off)
	if	PHOENIX_SYS=SA1_SYSTEM
	li	r3,$3ce24911
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
	li	r3,0<<30+0<<25+0<<23+2<<20+1<<17+2<<14+2<<11+2<<7+4<<3+1
	endif
	sw	r3,GAL_CS1_CFG(r2)

	if	PHOENIX_SYS=SEATTLE_SYSTEM
;	li	r3,0<<30+0<<25+0<<23+2<<20+2<<17+3<<14+3<<11+3<<7+7<<3+2
	li	r3,0<<30+0<<25+0<<23+2<<20+4<<17+5<<14+5<<11+5<<7+9<<3+4
	endif
	sw	r3,GAL_CS3_CFG(r2)

	; END OF GALILEO PCI CONTROLLER INITIALIZATION

	jr	r31
	nop
