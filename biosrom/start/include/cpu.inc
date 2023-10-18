; idtcpu.h -- cpu related defines

; memory configuration and mapping

K0BASE		equ	$80000000
K0SIZE		equ	$20000000
K1BASE		equ	$a0000000
K1SIZE		equ	$20000000
K2BASE		equ	$c0000000
K2SIZE		equ	$20000000
KSBASE		equ	$e0000000
KSSIZE		equ	$20000000

KUBASE		equ	0
KUSIZE		equ	$80000000

; Exception Vectors

T_VEC		equ	K0BASE+$000		; tlbmiss vector
X_VEC		equ	K0BASE+$080		; xtlbmiss vector
C_VEC		equ	K0BASE+$100		; cache error vector
E_VEC		equ	K0BASE+$180		; exception vector
R_VEC		equ	K1BASE+$1fc00000	; reset vector

; Address conversion macros
;
;K0_TO_K1(x)	equ	((x)|$A0000000)	; kseg0 to kseg1
;K1_TO_K0(x)	equ	((x)&$9FFFFFFF)	; kseg1 to kseg0
;K0_TO_PHYS(x)	equ	((x)&$1FFFFFFF)	; kseg0 to physical
;K1_TO_PHYS(x)	equ	((x)&$1FFFFFFF)	; kseg1 to physical
;PHYS_TO_K0(x)	equ	((x)|$80000000)	; physical to kseg0
;PHYS_TO_K1(x)	equ	((x)|$A0000000)	; physical to kseg1

; Cache size constants

MINCACHE	equ	$800	; 2*1024 2k
MAXCACHE	equ	$40000	; 256*1024 256k

; R4000 configuration register definitions

CFG_CM		equ	$80000000	; Master-Checker mode
CFG_ECMASK	equ	$70000000	; System Clock Ratio
CFG_ECBY2	equ	$00000000	; divide by 2
CFG_ECBY3	equ	$10000000	; divide by 3
CFG_ECBY4	equ	$20000000	; divide by 4
CFG_EPMASK	equ	$0f000000	; Transmit data pattern
CFG_EPD		equ	$00000000	; D
CFG_EPDDX	equ	$01000000	; DDX
CFG_EPDDXX	equ	$02000000	; DDXX
CFG_EPDXDX	equ	$03000000	; DXDX
CFG_EPDDXXX	equ	$04000000	; DDXXX
CFG_EPDDXXXX	equ	$05000000	; DDXXXX
CFG_EPDXXDXX	equ	$06000000	; DXXDXX
CFG_EPDDXXXXX	equ	$07000000	; DDXXXXX
CFG_EPDXXXDXXX	equ	$08000000	; DXXXDXXX
CFG_SBMASK	equ	$00c00000	; Secondary cache block size
CFG_SBSHIFT	equ	22
CFG_SB4		equ	$00000000	; 4 words
CFG_SB8		equ	$00400000	; 8 words
CFG_SB16	equ	$00800000	; 16 words
CFG_SB32	equ	$00c00000	; 32 words
CFG_SS		equ	$00200000	; Split secondary cache
CFG_SW		equ	$00100000	; Secondary cache port width
CFG_EWMASK	equ	$000c0000	; System port width
CFG_EWSHIFT	equ	18
CFG_EW64	equ	$00000000	; 64 bit
CFG_EW32	equ	$00010000	; 32 bit
CFG_SC		equ	$00020000	; Secondary cache absent
CFG_SM		equ	$00010000	; Dirty Shared mode disabled
CFG_BE		equ	$00008000	; Big Endian
CFG_EM		equ	$00004000	; ECC mode enable
CFG_EB		equ	$00002000	; Block ordering
CFG_ICMASK	equ	$00000e00	; Instruction cache size
CFG_ICSHIFT	equ	9
CFG_DCMASK	equ	$000001c0	; Data cache size
CFG_DCSHIFT	equ	6
CFG_IB		equ	$00000020	; Instruction cache block size
CFG_DB		equ	$00000010	; Data cache block size
CFG_CU		equ	$00000008	; Update on Store Conditional
CFG_K0MASK	equ	$00000007	; KSEG0 coherency algorithm

; R4000 primary cache mode

CFG_C_UNCACHED		equ	2
CFG_C_NONCOHERENT	equ	3
CFG_C_COHERENTXCL	equ	4
CFG_C_COHERENTXCLW	equ	5
CFG_C_COHERENTUPD	equ	6

; R4000 cache operations (should be in assembler...?)

Index_Invalidate_I	equ	$0	; 0 0
Index_Writeback_Inv_D	equ	$1	; 0 1
Index_Invalidate_SI	equ	$2	; 0 2
Index_Writeback_Inv_SD	equ	$3	; 0 3
Index_Load_Tag_I	equ	$4	; 1 0
Index_Load_Tag_D	equ	$5	; 1 1
Index_Load_Tag_SI	equ	$6	; 1 2
Index_Load_Tag_SD	equ	$7	; 1 3
Index_Store_Tag_I	equ	$8	; 2 0
Index_Store_Tag_D	equ	$9	; 2 1
Index_Store_Tag_SI	equ	$A	; 2 2
Index_Store_Tag_SD	equ	$B	; 2 3
Create_Dirty_Exc_D	equ	$D	; 3 1
Create_Dirty_Exc_SD	equ	$F	; 3 3
Hit_Invalidate_I	equ	$10	; 4 0
Hit_Invalidate_D	equ	$11	; 4 1
Hit_Invalidate_SI	equ	$12	; 4 2
Hit_Invalidate_SD	equ	$13	; 4 3
Hit_Writeback_Inv_D	equ	$15	; 5 1
Hit_Writeback_Inv_SD	equ	$17	; 5 3
Fill_I			equ	$14	; 5 0
Hit_Writeback_D		equ	$19	; 6 1
Hit_Writeback_SD	equ	$1B	; 6 3
Hit_Writeback_I		equ	$18	; 6 0
Hit_Set_Virtual_SI	equ	$1E	; 7 2
Hit_Set_Virtual_SD	equ	$1F	; 7 3


; TLB resource defines

N_TLB_ENTRIES		equ	48

TLBHI_VPN2MASK		equ	$ffffe000
TLBHI_PIDMASK		equ	$000000ff
TLBHI_NPID		equ	256

TLBLO_PFNMASK		equ	$3fffffc0
TLBLO_PFNSHIFT		equ	6
TLBLO_D			equ	$00000004	; writeable
TLBLO_V			equ	$00000002	; valid bit
TLBLO_G			equ	$00000001	; global access bit
TLBLO_CMASK		equ	$00000038	; cache algorithm mask
TLBLO_CSHIFT		equ	3

TLBLO_UNCACHED		equ	CFG_C_UNCACHED<<TLBLO_CSHIFT
TLBLO_NONCOHERENT	equ	CFG_C_NONCOHERENT<<TLBLO_CSHIFT
TLBLO_COHERENTXCL	equ	CFG_C_COHERENTXCL<<TLBLO_CSHIFT
TLBLO_COHERENTXCLW	equ	CFG_C_COHERENTXCLW<<TLBLO_CSHIFT
TLBLO_COHERENTUPD	equ	CFG_C_COHERENTUPD<<TLBLO_CSHIFT

TLBINX_PROBE		equ	$80000000
TLBINX_INXMASK		equ	$0000003f

TLBRAND_RANDMASK	equ	$0000003f

TLBCTXT_BASEMASK	equ	$ff800000
TLBCTXT_BASESHIFT	equ	23

TLBCTXT_VPN2MASK	equ	$007ffff0
TLBCTXT_VPN2SHIFT	equ	4

TLBPGMASK_MASK		equ	$01ffe000


SR_CUMASK	equ	$f0000000	; coproc usable bits
SR_CU3		equ	$80000000	; Coprocessor 3 usable
SR_CU2		equ	$40000000	; Coprocessor 2 usable
SR_CU1		equ	$20000000	; Coprocessor 1 usable
SR_CU0		equ	$10000000	; Coprocessor 0 usable

SR_RP		equ	$08000000	; Reduced power operation
SR_FR		equ	$04000000	; Additional floating point registers
SR_RE		equ	$02000000	; Reverse endian in user mode

SR_BEV		equ	$00400000	; Use boot exception vectors
SR_TS		equ	$00200000	; TLB shutdown
SR_SR		equ	$00100000	; Soft reset
SR_CH		equ	$00040000	; Cache hit
SR_CE		equ	$00020000	; Use cache ECC
SR_DE		equ	$00010000	; Disable cache exceptions

; status register interrupt masks and bits


SR_IMASK	equ	$0000ff00	; Interrupt mask
SR_IMASK8	equ	$00000000	; mask level 8
SR_IMASK7	equ	$00008000	; mask level 7
SR_IMASK6	equ	$0000c000	; mask level 6
SR_IMASK5	equ	$0000e000	; mask level 5
SR_IMASK4	equ	$0000f000	; mask level 4
SR_IMASK3	equ	$0000f800	; mask level 3
SR_IMASK2	equ	$0000fc00	; mask level 2
SR_IMASK1	equ	$0000fe00	; mask level 1
SR_IMASK0	equ	$0000ff00	; mask level 0

SR_IMASKSHIFT	equ	8

SR_IBIT8	equ	$00008000	; bit level 8
SR_IBIT7	equ	$00004000	; bit level 7
SR_IBIT6	equ	$00002000	; bit level 6
SR_IBIT5	equ	$00001000	; bit level 5
SR_IBIT4	equ	$00000800	; bit level 4
SR_IBIT3	equ	$00000400	; bit level 3
SR_IBIT2	equ	$00000200	; bit level 2
SR_IBIT1	equ	$00000100	; bit level 1

SR_KSMASK	equ	$00000018	; Kernel mode mask
SR_KSUSER	equ	$00000010	; User mode
SR_KSSUPER	equ	$00000008	; Supervisor mode
SR_KSKERNEL	equ	$00000000	; Kernel mode
SR_ERL		equ	$00000004	; Error level
SR_EXL		equ	$00000002	; Exception level
SR_IE		equ	$00000001	; Interrupts enabled



; Cause Register

CAUSE_BD	equ	$80000000	; Branch delay slot
CAUSE_CEMASK	equ	$30000000	; coprocessor error
CAUSE_CESHIFT	equ	28


CAUSE_IPMASK	equ	$0000FF00	; Pending interrupt mask
CAUSE_IPSHIFT	equ	8

CAUSE_EXCMASK	equ	$0000003C	; Cause code bits
CAUSE_EXCSHIFT	equ	2

; Coprocessor 0 registers

C0_INX		equr	0	; tlb index
C0_RAND		equr	1	; tlb random
C0_TLBLO0	equr	2	; tlb entry low 0
C0_TLBLO1	equr	3	; tlb entry low 1

C0_CTXT		equr	4	; tlb context

C0_PAGEMASK	equr	5	; tlb page mask
C0_WIRED	equr	6	; number of wired tlb entries

C0_BADVADDR	equr	8	; bad virtual address

C0_COUNT	equr	9	; cycle count

C0_TLBHI	equr	10	; tlb entry hi

C0_COMPARE	equr	11	; cyccle count comparator

C0_SR		equr	12	; status register
C0_CAUSE	equr	13	; exception cause
C0_EPC		equr	14	; exception pc
C0_PRID		equr	15	; revision identifier

C0_CONFIG	equr	16	; configuration register
C0_LLADDR	equr	17	; linked load address
C0_WATCHLO	equr	18	; watchpoint trap register
C0_WATCHHI	equr	19	; watchpoint trap register
C0_ECC		equr	26	; secondary cache ECC control
C0_CACHEERR	equr	27	; cache error status
C0_TAGLO	equr	28	; cache tag lo
C0_TAGHI	equr	29	; cache tag hi
C0_ERRPC	equr	30	; cache error pc


