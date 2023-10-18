/*
**	idtinst.h - header file for the IDT disassembler/assembler
**
**	Copyright Integrated Device Technology, Inc.
**	All Rights Reserved
**
**  Aug. 16, 1989 - D. Cahoon
**  20 Jul 87 - Jack Miller, modified for R5K instruction set.
**
** $Author: Mlynch $
**
** $Revision: 1 $
*/

#ifndef	MIPSEL
#define	MIPSEL
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	NULL
#define	NULL	0
#endif

typedef	long				reg_t;
typedef	unsigned long	ureg_t;
typedef	unsigned char	u_char;

/*
**	The following defines are used in the asm_tab structure to
**  specify the type of instruction (format) - (i,j,r) The 'E_TYPE'
**	is used to mark the end of the table. 'F' type is for floating
**	point instructions 
*/

#define J_TYPE		1
#define I_TYPE		2
#define R_TYPE		3
#define F_TYPE      4       /* Floating-point */
#define X_TYPE      5       /* Floating-point eXtented */
#define CI_TYPE     6
#define CF_TYPE     7
#define E_TYPE      0x20

/*
**	The following defines are used in the asm_tab structure to 
**	allow for easy determination of branch and jump instructions.
**	these flags are used by the breakpoint code.
*/
#define JUMP        1       /* this instruction is a jump or branch */
#define LINK        2       /* this instruction is a jump or br and link */
#define COND        4       /* this instruction is conditional */
#define LOAD        8       /* This instr is a load from mem */
#define STORE		0x10	/* This instr is a store to mem */
#define RETN		0x20	/* This instr is typ a rtn from subr */

/*
**  The following are codes that indicate the type of operand to be
**	printed out for the instruction being decoded. Example:
**	Suppose we were disassembling the instruction - lw  a0,24(sp)
**	The operands for this instruction would be rt, signed immediate,
**	and a base register. So the arguments would be RT_ARG, 
**	SIGNED_IMD_ARG and BASE_ARG. In the asm_tab structure the member
**	'arg_order' contains these codes as 4 bit quantities ordered from
**	right to left. The value in 'arg_order' would be - 0x000085f2.
**	The print routines uses this value by fetching the code in the
**	bottom four bits and printing the operand pointed too. It then 
**	shifts right 4 bits and continues until a zero code is found.
**  This scheme will allow a max of seven operands. The comma is treated
**	like an operand ( hence the 'f' code ).
*/ 
#define RS_ARG              0x1
#define RT_ARG              0x2
#define RD_ARG              0x3
#define SHAMT_ARG           0x4
#define SIGNED_IMD_ARG      0x5
#define UNSIGNED_IMD_ARG    0x6
#define TARGET_ARG          0x7
#define BASE_ARG            0x8
#define FT_ARG              0x9
#define FS_ARG              0xA
#define FD_ARG              0xB
#define CODE_ARG            0xC
#define ESC_ARG             0xD
#define C0_ARG              0xE
#define COMMA_ARG           0xF

#define ESC_ESC             0x10
#define ESC_CACHE_ARG       (ESC_ESC+0x1)
#define ESC_FR_ARG          (ESC_ESC+0x2)
#define ESC_PREFX_ARG       (ESC_ESC+0x3)
#define ESC_CC_ARG          (ESC_ESC+0x4)

/*
**	these flags indicate that additional decoding is required
**	to determine the type of instruction. The pointer to asm_tab
**	in the asm_tab structure points to the next decode table.
*/	
#define N_DEC		0	/* regular ole decode */
#define	S_DEC		1	/* this is in the class of 'special' inst */
#define	R_DEC		2	/* this is in the class of register immediate inst */
#define	C_DEC		3	/* this is a coprocesser instruction */
#define CO_DEC		4	/* this is a coprocessor op instruction */
#define CB_DEC      5   /* this is a coprocessor branch instruction */

#define N_ENC		0x00
#define S_ENC		0x10
#define R_ENC		0x20
#define C_ENC		0x30
#define CO_ENC		0x40
#define CB_ENC      0x50

/*
**	masks for op field qualification
*/
#define N_MASK		0x3f00		/* Normal instructions */
#define R_MASK		0x1f00		/* Register immediate */
#define C_MASK		0x1f00		/* Coprocessor instruction */
#define CO_MASK		0x1000		/* Coprocessor op instruction */
#define CB_MASK     0x1f00      /* Coprocessor branch instruction */
#define CX_MASK     0x3800      /* Coprocessor eXtended instruction */

/*
** These flags are for those special mnemonics that don't fall out of the
** normal decoding - (ie. addiu  r3,zero,0x1234 is called a li r3,0x1234)
*/
#define MAY_BE_MOVE	0x00010000
#define MAY_BE_NOP	0x00020000
#define MAY_BE_LI	0x00030000
#define MAY_BE_BR	0x00040000

#define DIS_NEXT    0x01000000  /* delay slot in next instruction */

#define CI_TF       0x10000000
#define CF_TF       0x20000000


/*
**	masks for getting the various fields out of the opt_decode 
**	member in asm_tab
**	
*/
#define OPT_DEC_MASK	0x0000000f	/* optional decode field */
#define OPT_ENC_MASK	0x000000f0	/* optional enccode field */
#define OPT_OP_MASK     0x0000ff00  /* op field mask */
#define OPT_MN_MASK     0x00ff0000  /* the 'MAY_BE...' field */
#define DELAY_MASK      0x0f000000  /* next instr in a delay slot */
#define OPT_TF_MASK     0xf0000000  /* condition code t/f field */


/*
** Mode arguments to the disassembler
*/
#define DONT_SHOW_REGS	0	/* don't display register contents */
#define SHOW_REGS       1   /* flag to disassembler - display registers */
#define DO_DELAY        2   /* disassemble the delay slot inst too */
#define NO_ADDR         4   /* don't print out address */
#define NO_INST         8   /* don't print instruction */
#define USE_EXCEPT      16  /* Use except saved regs instead of client */


/*
** These defines are used by the breakpoint routines. The values
** for the various types of breakpoint are actually the machine code
** for a break instruction. The type of breakpoint is contained in the
** code field(bits 8-11) of the break instruction. The breakpoint 
** number of the sticky type of breakpoint is in the code field 
** (bits 16-19) also.
*/
#define BP_STICKY       0xa0d       /* created by break command */
#define BP_NON_STICKY	0xb0d		/* temporary to impliment single step */
#define BP_RIPPLE       0xc0d       /* continuing (running) from a sticky */
                                    /* or non_sticky breakpoint */
#define BP_TRACE        0xd0d       /* trace type breakpoint */
#define COND_STRT_TRC	0x00100000	/* start tracing on bp */
#define COND_STOP_TRC	0x00200000	/* stop tracing on bp */

#define BP_NUM_MASK     0x000f0000
#define BP_CT_MASK      0x00f00000
#define BP_NUM_SHIFT	16
#define BP_TYPE_MASK	0xfff

#define NUMBRKPT        16
#define EMPTYFLAG       (unsigned*)-1

#define	BRK_ON_BREAK	1 
#define	BRK_ON_STORES	2
#define	BRK_ON_LOADS	4
#define BRK_ON_FULL     0x8
#define	BRK_ON_INSTR	0x10

#define TRACE_ALL       0x100
#define	TRACE_CALLS 	0x200
#define	TRACE_LOADS 	0x400
#define	TRACE_STORES 	0x800
#define	TRACE_INSTR 	0x1000

#define STOP_MASK       0x00ff
#define TRACE_MASK      0xff00

#define TRBUF_SIZE      512

typedef struct {
	unsigned	*addr;	   /* the address executed */
	unsigned	instr;	   /* the instruction  executed */
	u_char		argnum[4]; /* arg. code - either reg num or code */
	unsigned	arg[4];    /* arg contents */
	unsigned	type;	   /* decode type */
	unsigned	argkey;	   /* key to decoding arguments */
} tr_tab ;

/*
** Floating-Point number bit masks
*/
#define FL32_SIGN_MASK  0x80000000
#define FL32_EXP_MASK   0x7F800000
#define FL32_FRAC_MASK  0x007FFFFF

#define FL64_SIGN_MASK  0x8000000000000000
#define FL64_EXP_MASK   0x7FF0000000000000
#define FL64_FRAC_MASK  0x000FFFFFFFFFFFFF


#ifdef MIPSEB
union r5kInst {
  unsigned      asm_code;
  struct {
    unsigned    op      : 6;
    unsigned    target  : 26;
  } j_type;
  struct {
    unsigned    op      : 6;
    unsigned    rs      : 5;
    unsigned    rt      : 5;
    signed      s_imd   : 16;
  } i_type;
  struct {
    unsigned    op      : 6;
    unsigned    rs      : 5;
    unsigned    rt      : 5;
    unsigned    u_imd   : 16;
  } u_type;
  struct {
    unsigned    op      : 6;
    unsigned    rs      : 5;
    unsigned    rt      : 5;
    unsigned    rd      : 5;
    unsigned    shamt   : 5;
    unsigned    funct   : 6;
  } r_type;
  struct {
    unsigned    op      : 6;
    unsigned    fmtx    : 1;
    unsigned    fmt     : 4;
    unsigned    ft      : 5;
    unsigned    fs      : 5;
    unsigned    fd      : 5;
    unsigned    funct   : 6;
  } f_type;
  struct {
    unsigned    op      : 6;
    unsigned    fr      : 5;
    unsigned    ft      : 5;
    unsigned    fs      : 5;
    unsigned    fd      : 5;
    unsigned    funct   : 6;
  } x_type;
};
#endif

#ifdef MIPSEL
union r5kInst {
  unsigned      asm_code;
  struct {
    unsigned target     : 26;
    unsigned op         : 6;
  } j_type;
  struct {
    signed      s_imd   : 16;
    unsigned    rt      : 5;
    unsigned    rs      : 5;
    unsigned    op      : 6;
  } i_type;
  struct {
    unsigned    u_imd   : 16;
    unsigned    rt      : 5;
    unsigned    rs      : 5;
    unsigned    op      : 6;
  } u_type;
  struct {
    unsigned    funct   : 6;
    unsigned    shamt   : 5;
    unsigned    rd      : 5;
    unsigned    rt      : 5;
    unsigned    rs      : 5;
    unsigned    op      : 6;
  } r_type;
  struct {
    unsigned    funct   : 6;
    unsigned    fd      : 5;
    unsigned    fs      : 5;
    unsigned    ft      : 5;
    unsigned    fmt     : 4;
    unsigned    fmtx    : 1;
    unsigned    op      : 6;
  } f_type;
  struct {
    unsigned    funct   : 6;
    unsigned    fd      : 5;
    unsigned    fs      : 5;
    unsigned    ft      : 5;
    unsigned    fr      : 5;
    unsigned    op      : 6;
  } x_type;
};
#endif
