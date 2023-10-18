
; idtmon.h - General header file for the IDT Prom Monitor
;
; Copyright 1989 Integrated Device Technology, Inc.
; All Rights Reserved.
;
; June 1989 - D.Cahoon



; P_STACKSIZE is the size of the Prom Stack. 
; the prom stack grows downward 

P_STACKSIZE	equ	$2000	; sets stack size to 8k 

; M_BUSWIDTH
; Memory bus width (including bank interleaving) in bytes
; used when doing memory sizing to prevent bus capacitance
; reporting ghost memory locations

M_BUSWIDTH	equ	16	; 64 bit memory bank interleaved 

; this is the default value for the number of bytes to add in calculating
; the checksums in the checksum command

CHK_SUM_CNT	equ	$20000	; number of bytes to calc chksum for 

; Monitor modes

MODE_MONITOR	equ	5	; IDT Prom Monitor is executing 
MODE_USER	equ	$a	; USER is executing 

; memory reference widths

SW_BYTE		equ	1
SW_HALFWORD	equ	2
SW_WORD		equ	4
SW_TRIBYTEL	equ	12
SW_TRIBYTER	equ	20

; definitions for select_cache call

DCACHE		equ	0
ICACHE		equ	1
SCACHE		equ	2

; definitions for CPU types

R4600		equ	1
R4400		equ	2
R4700		equ	3
R4650		equ	4

; general equates for diagnostics and boolean functions

PASS		equ	0
FAIL		equ	1

TRUE		equ	1
NULL		equ	0
FALSE		equ	0

