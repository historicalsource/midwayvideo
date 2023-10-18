;** constants for the WMS phoenix board (5579-14661-00)
;**
;** AMD 12/17/95
;
;* note that all of this can move around if the chips select registers
;* get hosed, either in startup, or running
SA1_SYSTEM		equ	1
SEATTLE_SYSTEM		equ	2
FLAGSTAFF_SYSTEM	equ	4
VEGAS_SYSTEM		equ	8

LOCAL_MEM		equ	$00000000	; Local on-board/private memory
LOCAL_MEM_SIZE		equ	$10000000	; Local memory size 128Mb

PCI_IO			equ	$08000000
PCI_IO_SIZE		equ	$02000000

PCI_MEM			equ	$0a000000
PCI_MEM_SIZE		equ	$02000000

GAL_RESET_REG_BASE	equ	$b4000000
GAL_REG_BASE		equ	$0c000000
GAL_REG_SIZE		equ	$01000000

EXPCS0			equ	$10000000	; Daughterboard expansion area
EXPCS0_SIZE		equ	$02000000	; Daughterboard expansion area

EXPCS1			equ	$12000000	; Daughterboard expansion area
EXPCS1_SIZE		equ	$02000000	; Daughterboard expansion area

EXPCS2			equ	$14000000	; Daughterboard expansion area
EXPCS2_SIZE		equ	$02000000	; Daughterboard expansion area

EXPCS3			equ	$18000000	; Daughterboard expansion area
EXPCS3_SIZE		equ	$07c00000	; Daughterboard expansion area

BOOT_PROM		equ	$1fc00000
BOOT_PROM_SIZE		equ	$00400000

IOASIC_BASE		equ	$15000000
INTPLD_BASE		equ	$15080000
MISCPLD_BASE		equ	$15100000
WATCHDOG		equ	$15180000
UART0_BASE		equ	$15200000
UART1_BASE		equ	$15280000
PRT_BASE		equ	$15300000
ADC_BASE		equ	$15380000
SNDFIFO_BASE		equ	$15400000
NVRAM_BASE		equ	$15480000
IOASIC_RESET		equ	$15800000
PCI_RESET		equ	$15880000
LED_BASE		equ	$15900000


INT_CONTROLLER_ADDRESS	equ	$b5080000

INT_ENABLE_REG		equ	INT_CONTROLLER_ADDRESS+0
INT_MAPA_REG		equ	INT_CONTROLLER_ADDRESS+8
INT_MAPB_REG		equ	INT_CONTROLLER_ADDRESS+16
INT_ICAUSE_REG		equ	INT_CONTROLLER_ADDRESS+24
INT_ISTATUS_REG		equ	INT_CONTROLLER_ADDRESS+32
INT_GPSTAT_REG		equ	INT_CONTROLLER_ADDRESS+40

; Enable Register Bits

EXP_CON0_INT_ENABLE	equ	$0001
EXP_CON1_INT_ENABLE	equ	$0002
EXP_CON2_INT_ENABLE	equ	$0004
EXP_CON3_INT_ENABLE	equ	$0008
MISC_IO_CNT_INT_ENABLE	equ	$0010
UART_16552_1_INT_ENABLE	equ	$0020
UART_16552_2_INT_ENABLE	equ	$0040
PAR_16552_INT_ENABLE	equ	$0080
PCI_A_INT_ENABLE	equ	$0100
PCI_B_INT_ENABLE	equ	$0200
PCI_C_INT_ENABLE	equ	$0400
PCI_D_INT_ENABLE	equ	$0800
IDE_INT_ENABLE		equ	$1000
IO_ASIC_INT_ENABLE	equ	$2000
A2D_INT_ENABLE		equ	$4000

; Map Register Controls
; Galileo CPU Int is hard routed to CPU Int 0
; Galileo PCI Int is hard routed to CPU Int 1

ROUTE_INT_TO_2		equ	0
ROUTE_INT_TO_3		equ	1
ROUTE_INT_TO_4		equ	2
ROUTE_INT_TO_5		equ	3

; Map register A routing shifts
EXP_CON0_ROUTE_SHIFT		equ	0
EXP_CON1_ROUTE_SHIFT		equ	2
EXP_CON2_ROUTE_SHIFT		equ	4
EXP_CON3_ROUTE_SHIFT		equ	6
MISC_IO_CNT_ROUTE_SHIFT		equ	8
UART_16552_1_ROUTE_SHIFT	equ	10
UART_16552_2_ROUTE_SHIFT	equ	12
PAR_16552_ROUTE_SHIFT		equ	14

; Map register B routing shifts
PCI_A_ROUTE_SHIFT	equ	0
PCI_B_ROUTE_SHIFT	equ	2
PCI_C_ROUTE_SHIFT	equ	4
PCI_D_ROUTE_SHIFT	equ	6
IDE_ROUTE_SHIFT		equ	8
IO_ASIC_ROUTE_SHIFT	equ	10
A2D_ROUTE_SHIFT		equ	12

; Cause & Status Register bits
; The difference is the cause register shows the interrupt if it is active
; and the interrupt is enabled. The status register shows the interrupt
; source regardless of whether or not the interrupt is enabled.

EXP_CON0_INT_PENDING		equ	$0001
EXP_CON1_INT_PENDING		equ	$0002
EXP_CON2_INT_PENDING		equ	$0004
EXP_CON3_INT_PENDING		equ	$0008
MISC_IO_CNT_INT_PENDING		equ	$0010
UART_16552_1_INT_PENDING	equ	$0020
UART_16552_2_INT_PENDING	equ	$0040
PAR_16552_INT_PENDING		equ	$0080
PCI_A_INT_PENDING		equ	$0100
PCI_B_INT_PENDING		equ	$0200
PCI_C_INT_PENDING		equ	$0400
PCI_D_INT_PENDING		equ	$0800
IDE_INT_PENDING			equ	$1000
IO_ASIC_INT_PENDING		equ	$2000
A2D_INT_PENDING			equ	$4000

; General Status Register Bits

AUX_OUT_0_11_RD		equ	$0fff
WATCHDOG_NOT_STATUS	equ	$1000
NMI_NOT_STATUS		equ	$2000
BAT_NOT_LOW		equ	$4000
SPARE			equ	$8000


	if	PHOENIX_SYS=SA1_SYSTEM
LED_ADDR	equ	$b5100010
	elseif	PHOENIX_SYS=SEATTLE_SYSTEM
LED_ADDR	equ	$b7900000
	elseif	PHOENIX_SYS=VEGAS_SYSTEM
LED_ADDR	equ	$a1600000
	endif
