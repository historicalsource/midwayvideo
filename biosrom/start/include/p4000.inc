
; ROM/RAM and fixed memory address spaces etc.

;
;
; Copyright (c) 1992 ALGORITHMICS LIMITED
; ALL RIGHTS RESERVED
;
; THIS SOFTWARE PRODUCT CONTAINS THE UNPUBLISHED SOURCE
; CODE OF ALGORITHMICS LIMITED
;
; The copyright notices above do not evidence any actual
; or intended publication of such source code.
;


; p4000.h: P4000 board information


; ROM/RAM and fixed memory address spaces etc.


LOCAL_MEM	equ	$00000000	; Local on-board/private memory
LOCAL_MEM_SIZE	equ	$10000000	; Local memory size (256Mb max)

EXP1		equ	$10000000	; Daughterboard expansion area
EXP1_SIZE	equ	$0f000000	; Daughterboard expansion area

LOCAL_PROM	equ	$1fc00000	; PROM decode address
LOCAL_PROM_SIZE	equ	$00080000	; PROM decode size

BOOTPROM	equ	$1fc00000	; Boot Prom address
BOOTPROM_SIZE	equ	$00080000	; Boot Prom size (512k max)

EXP2		equ	$20000000	; Daughterboard expansion area
EXP2_SIZE	equ	$df000000	; Daughterboard expansion area

; Device Address Spaces

RESET_BASE	equ	$1f100000	; Reset/Configuration
ALPHN_BASE	equ	$1f200000	; Alphanumeric display
MPSC_BASE	equ	$1f300000	; NEC uPD72001
RTCLOCK_BASE	equ	$1f400000	; Mk48T02 NVRAM/RTC
SONIC_BASE	equ	$1f600000	; Ethernet controller
INTREG_BASE	equ	$1f700000	; Interrupt registers

ALPHN_CLR_	equ	RESET_BASE+0	; display clear (active low)
ALPHN_CSR	equ	RESET_BASE+4	; display cursor enable
ALPHN_BLNK_	equ	RESET_BASE+8	; display blank (active low)
SIO_RESET_	equ	RESET_BASE+12	; uPD72001 reset (active low)
NET_RESET_	equ	RESET_BASE+16	; SONIC reset (active low)

RESET_ZERO	equ	0
RESET_ONE	equ	~0

; Alphanumeric display

ALPHA_BASE	equ	ALPHN_BASE+16
CURSOR_BASE	equ	ALPHN_BASE
;ALPHN_ENTRY(n)	equ	((3 - (n)) << 2)
;ALPHN_CURS(n)	equ	(CURSOR_BASE + ALPHN_ENTRY(n))
;ALPHN_CHAR(n)	equ	(ALPHA_BASE + ALPHN_ENTRY(n))

; interrupt registers

INT_IRR0	equ	INTREG_BASE+$00	; interrupt request lo (read)
INT_IRR1	equ	INTREG_BASE+$20	; interrupt request hi (read)
INT_IMR0	equ	INTREG_BASE+$00	; interrupt mask (write)
INT_IMR1	equ	INTREG_BASE+$04	; interrupt mask (write)
INT_IMR2	equ	INTREG_BASE+$08	; interrupt mask (write)
INT_IMR3	equ	INTREG_BASE+$0c	; interrupt mask (write)
INT_ICG0	equ	INTREG_BASE+$20	; interrupt group (write)
INT_ICG1	equ	INTREG_BASE+$24	; interrupt group (write)
INT_ICG2	equ	INTREG_BASE+$28	; interrupt group (write)
INT_ICG3	equ	INTREG_BASE+$2c	; interrupt group (write)
INT_ACKPANIC	equ	INTREG_BASE+$40	; interrupt acknowledge (read)

IRR0_IOIRQ1	equ	$80000000	; Daughterboard IRQ1
IRR0_IOIRQ0	equ	$40000000	; Daughterboard IRQ0
IRR0_SIO	equ	$20000000	; uPD72001
IRR0_NET	equ	$10000000	; SONIC
IRR0_BUS	equ	$08000000	; bus timeout
IRR0_ETHPAR	equ	$04000000	; ethernet parity error
IRR0_ACFAIL	equ	$02000000	; AC fail
IRR0_DEBUG	equ	$01000000	; Debug button
IRR1_IOIRQ3	equ	$02000000	; Daughterboard IRQ3
IRR1_IOIRQ2	equ	$01000000	; Daughterboard IRQ2

IMR_ENABLE	equ	0		; enable interrupt
IMR_DISABLE	equ	~0		; disable interrupt

ICG_CLR		equ	0		; clear ICG bit
ICG_SET		equ	~0		; set ICG bit

