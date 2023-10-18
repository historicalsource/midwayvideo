;**************************************************************************
;
; gt64010.h - Header file for GT-64010 stuff
;
; Written by:  Jason Skiles, Michael J. Lynch
; Version:     1.00
; Date:        10/27/95
;
; Copyright (c) 1995 by Williams Electronics Games Inc.
; All Rights Reserved
;
; Use, duplication, or disclosure is strictly forbidden unless approved
; in writing by Williams Electronics Games Inc.
;
;**************************************************************************

DMA_ACTIVE			equ	1
DMA_NOT_ACTIVE			equ	0
DMA_FORCE_FETCH_NEXT		equ	1
DMA_TRANSFER_MODE_DEMAND	equ	0
DMA_TRANSFER_MODE_BLOCK		equ	1
DMA_INT_TERMINAL_COUNT		equ	0
DMA_INT_CHAIN_END		equ	1
DMA_CHAINED_MODE		equ	0
DMA_NON_CHAINED_MODE		equ	1
DMA_TRANSFER_LIMIT_4		equ	0
DMA_TRANSFER_LIMIT_8		equ	1
DMA_TRANSFER_LIMIT_16		equ	3
DMA_TRANSFER_LIMIT_32		equ	7
DMA_DEST_INCREMENT		equ	0
DMA_DEST_DECREMENT		equ	1
DMA_DEST_HOLD			equ	2
DMA_SRC_INCREMENT		equ	0
DMA_SRC_DECREMENT		equ	1
DMA_SRC_HOLD			equ	2

ROUND_ROBIN			equ	0
CHANNEL_1_PRIORITY		equ	1
CHANNEL_0_PRIORITY		equ	2
CHANNEL_3_PRIORITY		equ	CHANNEL_1_PRIORITY
CHANNEL_2_PRIORITY		equ	CHANNEL_0_PRIORITY
GROUP_23_PRIORITY		equ	1
GROUP_01_PRIORITY		equ	2
RELINQUISH			equ	0
GRANT				equ	1

TIMER_TYPE_COUNTER		equ	0
TIMER_TYPE_TIMER		equ	1


;**************************************************************************
; ASSORTED DEFINITIONS
;**************************************************************************

ENABLE		equ	1
DISABLE		equ	0
ALLOW_ACCESS	equ	1
RESET		equ	0
ACTIVE		equ	1
NOT_ACTIVE	equ	0


;**************************************************************************
; ASSORTED GT64010 REGISTERS
;**************************************************************************

GT_PCI_INTERNAL_COMMAND_REG		equ	GT_64010_BASE+$0c00
GT_PCI_INTERNAL_CONFIG_ADDRESS_REG	equ	GT_64010_BASE+$0cf8
GT_PCI_INTERNAL_CONFIG_DATA_REG		equ	GT_64010_BASE+$0cfc
GT_TIMER_COUNTER_CONTROL_REG		equ	GT_64010_BASE+$0864
GT_TIMER_COUNTER_0_REG			equ	GT_64010_BASE+$0850
GT_TIMER_COUNTER_1_REG			equ	GT_64010_BASE+$0854
GT_TIMER_COUNTER_2_REG			equ	GT_64010_BASE+$0858
GT_TIMER_COUNTER_3_REG			equ	GT_64010_BASE+$085c

GT_DMA_ARBITER_REG			equ	GT_64010_BASE+$0860
GT_DMA_CHANNEL_0_CONTROL_REG		equ	GT_64010_BASE+$0840
GT_DMA_CHANNEL_1_CONTROL_REG		equ	GT_64010_BASE+$0844
GT_DMA_CHANNEL_2_CONTROL_REG		equ	GT_64010_BASE+$0848
GT_DMA_CHANNEL_3_CONTROL_REG		equ	GT_64010_BASE+$084c
GT_DMA_CHANNEL_0_BYTE_COUNT_REG		equ	GT_64010_BASE+$0800
GT_DMA_CHANNEL_1_BYTE_COUNT_REG		equ	GT_64010_BASE+$0804
GT_DMA_CHANNEL_2_BYTE_COUNT_REG		equ	GT_64010_BASE+$0808
GT_DMA_CHANNEL_3_BYTE_COUNT_REG		equ	GT_64010_BASE+$080c
GT_DMA_CHANNEL_0_SRC_ADDRESS_REG	equ	GT_64010_BASE+$0810
GT_DMA_CHANNEL_1_SRC_ADDRESS_REG	equ	GT_64010_BASE+$0814
GT_DMA_CHANNEL_2_SRC_ADDRESS_REG	equ	GT_64010_BASE+$0818
GT_DMA_CHANNEL_3_SRC_ADDRESS_REG	equ	GT_64010_BASE+$081c
GT_DMA_CHANNEL_0_DEST_ADDRESS_REG	equ	GT_64010_BASE+$0820
GT_DMA_CHANNEL_1_DEST_ADDRESS_REG	equ	GT_64010_BASE+$0824
GT_DMA_CHANNEL_2_DEST_ADDRESS_REG	equ	GT_64010_BASE+$0828
GT_DMA_CHANNEL_3_DEST_ADDRESS_REG	equ	GT_64010_BASE+$082c
GT_DMA_CHANNEL_0_NEXT_RECORD_REG	equ	GT_64010_BASE+$0830
GT_DMA_CHANNEL_1_NEXT_RECORD_REG	equ	GT_64010_BASE+$0834
GT_DMA_CHANNEL_2_NEXT_RECORD_REG	equ	GT_64010_BASE+$0838
GT_DMA_CHANNEL_3_NEXT_RECORD_REG	equ	GT_64010_BASE+$083c

;**************************************************************************
; DEVICE NUMBERS
;**************************************************************************

GT64010_DEVICE_NUMBER		equ	0
PC87415_DEVICE_NUMBER		equ	9
PCI_SLOT_2_DEVICE_NUMBER	equ	8
PCI_SLOT_1_DEVICE_NUMBER	equ	7
PCI_SLOT_0_DEVICE_NUMBER	equ	6


;**************************************************************************
; GT64010 VERSION NUMBER
;**************************************************************************

GT64010_REV		equ	1


;**************************************************************************
; PCI CONFIGURATION REGISTER DEFINITIONS
;**************************************************************************

PCI_CONFIG_REG0		equ	0
PCI_CONFIG_REG1		equ	1
PCI_CONFIG_REG2		equ	2
PCI_CONFIG_REG3		equ	3
PCI_CONFIG_REG4		equ	4
PCI_CONFIG_REG5		equ	5
PCI_CONFIG_REG6		equ	6
PCI_CONFIG_REG7		equ	7
PCI_CONFIG_REG8		equ	8
PCI_CONFIG_REG9		equ	9
PCI_CONFIG_REG10	equ	10
PCI_CONFIG_REG11	equ	11
PCI_CONFIG_REG12	equ	12
PCI_CONFIG_REG13	equ	13
PCI_CONFIG_REG14	equ	14
PCI_CONFIG_REG15	equ	15


; register definitions for the galileo GT-64010


GAL_CPU_INT_CFG	equ	$0000
GAL_RAS10_LOW	equ	$0008
GAL_RAS10_HIGH	equ	$0010
GAL_RAS32_LOW	equ	$0018
GAL_RAS32_HIGH	equ	$0020
GAL_CS210_LOW	equ	$0028
GAL_CS210_HIGH	equ	$0030
GAL_CS3B_LOW	equ	$0038
GAL_CS3B_HIGH	equ	$0040
GAL_PCIIO_LOW	equ	$0048
GAL_PCIIO_HIGH	equ	$0050
GAL_PCIM_LOW	equ	$0058
GAL_PCIM_HIGH	equ	$0060
GAL_REG_LOW	equ	$0068

GAL_RAS0_LOW	equ	$0400
GAL_RAS0_HIGH	equ	$0404
GAL_RAS1_LOW	equ	$0408
GAL_RAS1_HIGH	equ	$040c
GAL_RAS2_LOW	equ	$0410
GAL_RAS2_HIGH	equ	$0414
GAL_RAS3_LOW	equ	$0418
GAL_RAS3_HIGH	equ	$041c
GAL_CS0_LOW	equ	$0420
GAL_CS0_HIGH	equ	$0424
GAL_CS1_LOW	equ	$0428
GAL_CS1_HIGH	equ	$042c
GAL_CS2_LOW	equ	$0430
GAL_CS2_HIGH	equ	$0434
GAL_CS3_LOW	equ	$0438
GAL_CS3_HIGH	equ	$043c
GAL_BOOT_LOW	equ	$0440
GAL_BOOT_HIGH	equ	$0444

GAL_DRAM_CFG	equ	$0448

GAL_DRAM0_CFG	equ	$044c
GAL_DRAM1_CFG	equ	$0450
GAL_DRAM2_CFG	equ	$0454
GAL_DRAM3_CFG	equ	$0458

GAL_CS0_CFG	equ	$045c
GAL_CS1_CFG	equ	$0460
GAL_CS2_CFG	equ	$0464
GAL_CS3_CFG	equ	$0468
GAL_BOOT_CFG	equ	$046c

