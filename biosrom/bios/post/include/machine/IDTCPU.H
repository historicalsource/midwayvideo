#ifndef _IDTCPU_H_
#define _IDTCPU_H_

/*
** idtcpu.h -- cpu related defines
*/



/*
** memory configuration and mapping
*/
#define K0BASE	0x80000000
#define K0SIZE	0x20000000
#define K1BASE  0xA0000000
#define K1SIZE	0x20000000
#define K2BASE  0xC0000000
#define K2SIZE	0x20000000
#define KSBASE  0xE0000000
#define KSSIZE	0x20000000

#define KUBASE	0
#define KUSIZE	0x80000000

/*
** Exception Vectors
*/
#define	T_VEC	(K0BASE+0x000)		/* tlbmiss vector */
#define X_VEC	(K0BASE+0x080)		/* xtlbmiss vector */
#define C_VEC	(K0BASE+0x100)		/* cache error vector */
#define E_VEC	(K0BASE+0x180)		/* exception vector */
#define R_VEC   (K1BASE+0x1FC00000) /* reset vector */

/*
** Address conversion macros
*/
#ifdef CLANGUAGE
#define CAST(as) (as)
#else
#define CAST(as)
#endif
#define K0_TO_K1(x)     (CAST(unsigned)(x)|0xA0000000)  /* kseg0 to kseg1 */
#define K1_TO_K0(x)     (CAST(unsigned)(x)&0x9FFFFFFF)  /* kseg1 to kseg0 */
#define	K0_TO_PHYS(x)	(CAST(unsigned)(x)&0x1FFFFFFF)	/* kseg0 to physical */
#define	K1_TO_PHYS(x)	(CAST(unsigned)(x)&0x1FFFFFFF)	/* kseg1 to physical */
#define	PHYS_TO_K0(x)	(CAST(unsigned)(x)|0x80000000)	/* physical to kseg0 */
#define	PHYS_TO_K1(x)	(CAST(unsigned)(x)|0xA0000000)	/* physical to kseg1 */

/*
**	Cache size constants
*/
#define MINCACHE        0x800       /* 2*1024     2k   */
#define MAXCACHE        0x40000     /* 256*1024   256k */

/* R4000 configuration register definitions */
#define CFG_CM          0x80000000  /* Master-Checker mode */

#define CFG_ECMASK      0x70000000  /* System Clock Ratio */
#define CFG_ECBY2       0x00000000  /* divide by 2 */
#define CFG_ECBY3       0x10000000  /* divide by 3 */
#define CFG_ECBY4       0x20000000  /* divide by 4 */

#define CFG_EPMASK      0x0F000000  /* Transmit data pattern */
#define CFG_EPD         0x00000000  /* D */
#define CFG_EPDDX       0x01000000  /* DDX */
#define CFG_EPDDXX      0x02000000  /* DDXX */
#define CFG_EPDXDX      0x03000000  /* DXDX */
#define CFG_EPDDXXX     0x04000000  /* DDXXX */
#define CFG_EPDDXXXX	0x05000000	/* DDXXXX */
#define CFG_EPDXXDXX	0x06000000	/* DXXDXX */
#define CFG_EPDDXXXXX	0x07000000	/* DDXXXXX */
#define CFG_EPDXXXDXXX  0x08000000  /* DXXXDXXX */

#define CFG_SBMASK      0x00C00000  /* Secondary cache block size */
#define CFG_SBSHIFT     22
#define CFG_SB4         0x00000000  /* 4 words */
#define CFG_SB8         0x00400000  /* 8 words */
#define CFG_SB16        0x00800000  /* 16 words */
#define CFG_SB32        0x00C00000  /* 32 words */

/* BITS 21 and 20 have different meanings on the R4700 and R5000 */
/* R4700 */
#define CFG_SS          0x00200000  /* Split secondary cache */
#define CFG_SW          0x00100000  /* Secondary cache port width */

/* R5000 */
#define CFG_SSMASK      0x00300000  /* Secondary cache size */
#define CFG_SSSHIFT     20
#define CFG_SS512K      0x00000000  /* 512K bytes */
#define CFG_SS1M        0x00100000  /* 1M   bytes */
#define CFG_SS2M        0x00200000  /* 2M   bytes */
#define CFG_SSNONE      0x00300000  /* none       */


#define CFG_EWMASK      0x000C0000  /* System port width */
#define CFG_EWSHIFT     18
#define CFG_EW64        0x00000000  /* 64 bit */
#define CFG_EW32        0x00010000  /* 32 bit */

#define CFG_SC          0x00020000  /* Secondary cache absent */
#define CFG_SM          0x00010000  /* Dirty Shared mode disabled */
#define CFG_BE          0x00008000  /* Big Endian */
#define CFG_EM          0x00004000  /* ECC mode enable */
#define CFG_EB          0x00002000  /* Block ordering */
#define CFG_SE          0x00001000  /* Secondary cache enable (R5000) */

#define CFG_ICMASK      0x00000E00  /* Instruction cache size */
#define CFG_ICSHIFT     9

#define CFG_DCMASK      0x000001C0  /* Data cache size */
#define CFG_DCSHIFT     6

#define CFG_IB          0x00000020  /* Instruction cache block size */
#define CFG_DB          0x00000010  /* Data cache block size */

#define CFG_CU          0x00000008  /* Update on Store Conditional */

#define CFG_K0MASK      0x00000007  /* KSEG0 coherency algorithm */

/*
 * R4000 primary cache mode
 */
#define CFG_C_UNCACHED		2
#define CFG_C_NONCOHERENT	3
#define CFG_C_COHERENTXCL	4
#define CFG_C_COHERENTXCLW	5
#define CFG_C_COHERENTUPD	6

/*
 * R4000 cache operations (should be in assembler...?)
 */
#define Index_Invalidate_I               0x0         /* 0       0 */
#define Index_Writeback_Inv_D            0x1         /* 0       1 */
#define Index_Invalidate_SI              0x2         /* 0       2 */
#define Index_Writeback_Inv_SD           0x3         /* 0       3 */
#define Index_Load_Tag_I                 0x4         /* 1       0 */
#define Index_Load_Tag_D                 0x5         /* 1       1 */
#define Index_Load_Tag_SI                0x6         /* 1       2 */
#define Index_Load_Tag_SD                0x7         /* 1       3 */
#define Index_Store_Tag_I                0x8         /* 2       0 */
#define Index_Store_Tag_D                0x9         /* 2       1 */
#define Index_Store_Tag_SI               0xA         /* 2       2 */
#define Index_Store_Tag_SD               0xB         /* 2       3 */
#define Create_Dirty_Exc_D               0xD         /* 3       1 */
#define Create_Dirty_Exc_SD              0xF         /* 3       3 */
#define Hit_Invalidate_I                 0x10        /* 4       0 */
#define Hit_Invalidate_D                 0x11        /* 4       1 */
#define Hit_Invalidate_SI                0x12        /* 4       2 */
#define Hit_Invalidate_SD                0x13        /* 4       3 */
#define Hit_Writeback_Inv_D              0x15        /* 5       1 */
#define Hit_Writeback_Inv_SD             0x17        /* 5       3 */
#define Fill_I                           0x14        /* 5       0 */
#define Hit_Writeback_D                  0x19        /* 6       1 */
#define Hit_Writeback_SD                 0x1B        /* 6       3 */
#define Hit_Writeback_I                  0x18        /* 6       0 */
#define Hit_Set_Virtual_SI               0x1E        /* 7       2 */
#define Hit_Set_Virtual_SD               0x1F        /* 7       3 */


/*
** TLB resource defines
*/
#define N_TLB_ENTRIES       48

#define TLBHI_VPN2MASK      0xFFFFE000
#define TLBHI_PIDMASK       0x000000FF
#define TLBHI_NPID          256

#define TLBLO_PFNMASK       0x3FFFFFC0
#define TLBLO_PFNSHIFT      6
#define TLBLO_D             0x00000004  /* writeable */
#define TLBLO_V             0x00000002  /* valid bit */
#define TLBLO_G             0x00000001  /* global access bit */
#define TLBLO_CMASK         0x00000038  /* cache algorithm mask */
#define TLBLO_CSHIFT        3

#define TLBLO_UNCACHED		(CFG_C_UNCACHED<<TLBLO_CSHIFT)
#define TLBLO_NONCOHERENT	(CFG_C_NONCOHERENT<<TLBLO_CSHIFT)
#define TLBLO_COHERENTXCL	(CFG_C_COHERENTXCL<<TLBLO_CSHIFT)
#define TLBLO_COHERENTXCLW	(CFG_C_COHERENTXCLW<<TLBLO_CSHIFT)
#define TLBLO_COHERENTUPD	(CFG_C_COHERENTUPD<<TLBLO_CSHIFT)

#define TLBINX_PROBE        0x80000000
#define TLBINX_INXMASK      0x0000003F

#define TLBRAND_RANDMASK    0x0000003F

#define TLBCTXT_BASEMASK    0xFF800000
#define	TLBCTXT_BASESHIFT	23

#define TLBCTXT_VPN2MASK    0x007FFFF0
#define	TLBCTXT_VPN2SHIFT	4

#define TLBPGMASK_MASK      0x01FFE000


/*
** Status Register
*/
#define SR_CUMASK   0xF0000000  /* Coprocessor usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */

#define SR_RP       0x08000000  /* Reduced power operation */
#define SR_FR		0x04000000	/* Additional floating point registers */
#define SR_RE		0x02000000	/* Reverse endian in user mode */

#define SR_BEV		0x00400000	/* Use boot exception vectors */
#define SR_TS		0x00200000	/* TLB shutdown */
#define SR_SR		0x00100000	/* Soft reset */
#define SR_CH		0x00040000	/* Cache hit */
#define SR_CE		0x00020000	/* Use cache ECC  */
#define SR_DE		0x00010000	/* Disable cache exceptions */

/*
**  Status register interrupt masks and bits
*/
#define SR_IMMASK   0x0000FF00  /* Interrupt mask */
#define SR_IMSHIFT  8

#define SR_KSMASK	0x00000018	/* Kernel mode mask */
#define SR_KSUSER	0x00000010	/* User mode */
#define SR_KSSUPER	0x00000008	/* Supervisor mode */
#define SR_KSKERNEL 0x00000000  /* Kernel mode */

#define SR_ERL		0x00000004	/* Error level */
#define SR_EXL		0x00000002	/* Exception level */
#define SR_IE		0x00000001	/* Interrupts enabled */


/*
** Cause Register
*/
#define CAUSE_BD        0x80000000  /* Branch delay slot */
#define	CAUSE_CEMASK	0x30000000	/* coprocessor error */
#define	CAUSE_CESHIFT	28

#define	CAUSE_IPMASK	0x0000FF00	/* Pending interrupt mask */
#define CAUSE_IPSHIFT   8

#define	CAUSE_EXCMASK	0x0000003C	/* Cause code bits */
#define CAUSE_EXCSHIFT  2


/*
** Cause Register Exception Codes
*/
#define CAUSE_INT       0       /* Interrupt */
#define CAUSE_TLBM      1       /* TLB modification */
#define CAUSE_TLBL      2       /* TLB (load or instruction fetch) */
#define CAUSE_TLBS      3       /* TLB (store) */
#define CAUSE_ADEL      4       /* Address error (load or instruction fetch) */
#define CAUSE_ADES      5       /* Address error (store) */
#define CAUSE_IBE       6       /* Bus error (instruction fetch) */
#define CAUSE_DBE       7       /* Bus error (data reference: load or store)
#define CAUSE_SYS       8       /* Syscall */
#define CAUSE_BP        9       /* Breakpoint */
#define CAUSE_RI        10      /* Reserved instruction */
#define CAUSE_CU        11      /* Coprocessor Unusable */
#define CAUSE_AO        12      /* Arithmetic Overflow */
#define CAUSE_TR        13      /* Trap */
#define CAUSE_VCI       14      /* Virtual Coherency (instruction) */
#define CAUSE_FP        15      /* Floating-Point */
#define CAUSE_RES16     16      /* Reserved */
#define CAUSE_RES17     17      /* Reserved */
#define CAUSE_RES18     18      /* Reserved */
#define CAUSE_RES19     19      /* Reserved */
#define CAUSE_RES20     20      /* Reserved */
#define CAUSE_RES21     21      /* Reserved */
#define CAUSE_RES22     22      /* Reserved */
#define CAUSE_WATCH     23      /* Watch Point */
#define CAUSE_RES24     24      /* Reserved */
#define CAUSE_RES25     25      /* Reserved */
#define CAUSE_RES26     26      /* Reserved */
#define CAUSE_RES27     27      /* Reserved */
#define CAUSE_RES28     28      /* Reserved */
#define CAUSE_RES29     29      /* Reserved */
#define CAUSE_RES30     30      /* Reserved */
#define CAUSE_VCD       31      /* Virtual Coherency (data) */

/*
** Status Register IM Bits and Cause Register IP Bits
*/
#define HW_INT5         0x80    /* hardware interrupt 5 */
#define HW_INT4         0x40    /* hardware interrupt 4 */
#define HW_INT3         0x20    /* hardware interrupt 3 */
#define HW_INT2         0x10    /* hardware interrupt 2 */
#define HW_INT1         0x08    /* hardware interrupt 1 */
#define HW_INT0         0x04    /* hardware interrupt 0 */
#define SW_INT1         0x02    /* software interrupt 1 */
#define SW_INT0         0x01    /* software interrupt 0 */


/*
**  Coprocessor 0 registers
*/
#define	C0_INX		$0		/* tlb index */
#define	C0_RAND		$1		/* tlb random */
#define	C0_TLBLO0	$2		/* tlb entry low 0 */
#define	C0_TLBLO1	$3		/* tlb entry low 1 */

#define	C0_CTXT		$4		/* tlb context */

#define C0_PAGEMASK	$5		/* tlb page mask */
#define C0_WIRED	$6		/* number of wired tlb entries */

#define	C0_BADVADDR	$8		/* bad virtual address */

#define C0_COUNT	$9		/* cycle count */

#define	C0_TLBHI	$10		/* tlb entry hi */

#define C0_COMPARE	$11		/* cyccle count comparator  */

#define	C0_SR		$12		/* status register */
#define	C0_CAUSE	$13		/* exception cause */
#define	C0_EPC		$14		/* exception pc */
#define	C0_PRID		$15		/* revision identifier */

#define C0_CONFIG	$16		/* configuration register */
#define C0_LLADDR	$17		/* linked load address */
#define C0_WATCHLO	$18		/* watchpoint trap register */
#define C0_WATCHHI	$19		/* watchpoint trap register */
#define C0_ECC		$26		/* secondary cache ECC control */
#define C0_CACHEERR	$27		/* cache error status */
#define C0_TAGLO	$28		/* cache tag lo */
#define C0_TAGHI	$29		/* cache tag hi */
#define C0_ERRPC    $30     /* cache error pc */


/*
** Coprocessor 1 (Floating-Point) Status Register masks and bits
*/
#define FCR_FS          0x01000000  /* Flush denormalized results = 1 */
#define FCR_CONDITON    0x00800000  /* Condition bit */


#define FCR_CAUSE_MASK  0x0003F000  /* Exception Cause mask */
#define FCR_CAUSE_SHIFT 12

#define FCR_CAUSE_E     0x00020000  /* Unimplemented Operation */
#define FCR_CAUSE_V     0x00010000  /* Invalid Operation */
#define FCR_CAUSE_Z     0x00008000  /* Divide by Zero */
#define FCR_CAUSE_O     0x00004000  /* Overflow */
#define FCR_CAUSE_U     0x00002000  /* Underflow */
#define FCR_CAUSE_I     0x00001000  /* Inexact Operation */


#define FCR_ENA_MASK    0x00000F80  /* Exception Enable mask */
#define FCR_ENA_SHIFT   7

#define FCR_ENA_V       0x00000800  /* Invalid Operation */
#define FCR_ENA_Z       0x00000400  /* Divide by Zero */
#define FCR_ENA_O       0x00000200  /* Overflow */
#define FCR_ENA_U       0x00000100  /* Underflow */
#define FCR_ENA_I       0x00000080  /* Inexact Operation */


#define FCR_FLAGS_MASK  0x0000007C  /* Exception Status Flags mask */
#define FCR_FLAGS_SHIFT 2

#define FCR_FLAGS_V     0x00000040  /* Invalid Operation */
#define FCR_FLAGS_Z     0x00000020  /* Divide by Zero */
#define FCR_FLAGS_O     0x00000010  /* Overflow */
#define FCR_FLAGS_U     0x00000008  /* Underflow */
#define FCR_FLAGS_I     0x00000004  /* Inexact Operation */


#define FCR_RM_MASK     0x00000003  /* Rounding Mode mask */
#define FCR_RM_RM       3           /* Round toward -Inf */
#define FCR_RM_RP       2           /* Round toward +Inf */
#define FCR_RM_RZ       1           /* Round toward 0 */
#define FCR_RM_RN       0           /* Round to nearest */

#endif
