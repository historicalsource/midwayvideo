//
// disasm.c - Source for the disassembler used in the exception processing
//
// Original from IDT
//
// $Author: Mlynch $
//
// $Revision: 1 $
//
#include <idtinst.h>

int	printf(const char *_format, ...);

struct asm_tab {
  int                   op_val;         /* op-code value */
  const struct asm_tab  *asm_tab_ptr;   /* pointer to next decode table */
  int                   opt_decode;     /* optional decode flag or flags */
  char                  *const mnem;          /* assembler mnemonic */
  short                 inst_type;      /* type of instruction */
  short                 br_jmp_type;    /* if branch - tells what type */
  int                   arg_order;      /* arguments and order for this inst */
};

extern	unsigned long	*except_regs;


const struct asm_tab    asm_tab_spec[] = {
  {0x00, NULL,          N_MASK|N_DEC|S_ENC|MAY_BE_NOP,         "sll",     R_TYPE, NULL,           0x4f2f3},
  {0x00, NULL,          N_MASK|N_DEC|S_ENC,                    "nop",     R_TYPE, NULL,           0x0},
  {0x01, NULL,          N_MASK|N_DEC|S_ENC|CI_TF,              "mov",     CI_TYPE, NULL,          0x4df1f3},
  {0x02, NULL,          N_MASK|N_DEC|S_ENC,                    "srl",     R_TYPE, NULL,           0x4f2f3},
  {0x03, NULL,          N_MASK|N_DEC|S_ENC,                    "sra",     R_TYPE, NULL,           0x4f2f3},
  {0x04, NULL,          N_MASK|N_DEC|S_ENC,                    "sllv",    R_TYPE, NULL,           0x1f2f3},

  {0x06, NULL,          N_MASK|N_DEC|S_ENC,                    "srlv",    R_TYPE, NULL,           0x1f2f3},
  {0x07, NULL,          N_MASK|N_DEC|S_ENC,                    "srav",    R_TYPE, NULL,           0x1f2f3},
  {0x08, NULL,          N_MASK|N_DEC|S_ENC|DIS_NEXT,           "jr",      R_TYPE, JUMP|RETN,      0x1},
  {0x09, NULL,          N_MASK|N_DEC|S_ENC|DIS_NEXT,           "jalr",    R_TYPE, JUMP|LINK,      0x1f3},
  {0x0a, NULL,          N_MASK|N_DEC|S_ENC,                    "movz",    R_TYPE, NULL,           0x2f1f3},
  {0x0b, NULL,          N_MASK|N_DEC|S_ENC,                    "movn",    R_TYPE, NULL,           0x2f1f3},
  {0x0c, NULL,          N_MASK|N_DEC|S_ENC,                    "syscall", R_TYPE, NULL,           0xc},
/* technically the following is incorrect, but it does follow Mips conventions */
  {0x0d, NULL,          N_MASK|N_DEC|S_ENC,                    "break",   R_TYPE, NULL,           0xc},

  {0x0f, NULL,          N_MASK|N_DEC|S_ENC,                    "sync",    R_TYPE, NULL,           0x0},
  {0x10, NULL,          N_MASK|N_DEC|S_ENC,                    "mfhi",    R_TYPE, NULL,           0x3},
  {0x11, NULL,          N_MASK|N_DEC|S_ENC,                    "mthi",    R_TYPE, NULL,           0x1},
  {0x12, NULL,          N_MASK|N_DEC|S_ENC,                    "mflo",    R_TYPE, NULL,           0x3},
  {0x13, NULL,          N_MASK|N_DEC|S_ENC,                    "mtlo",    R_TYPE, NULL,           0x1},
  {0x14, NULL,          N_MASK|N_DEC|S_ENC,                    "dsllv",   R_TYPE, NULL,           0x1f2f3},

  {0x16, NULL,          N_MASK|N_DEC|S_ENC,                    "dsrlv",   R_TYPE, NULL,           0x1f2f3},
  {0x17, NULL,          N_MASK|N_DEC|S_ENC,                    "dsrav",   R_TYPE, NULL,           0x1f2f3},
  {0x18, NULL,          N_MASK|N_DEC|S_ENC,                    "mult",    R_TYPE, NULL,           0x2f1},
  {0x19, NULL,          N_MASK|N_DEC|S_ENC,                    "multu",   R_TYPE, NULL,           0x2f1},
  {0x1a, NULL,          N_MASK|N_DEC|S_ENC,                    "div",     R_TYPE, NULL,           0x2f1},
  {0x1b, NULL,          N_MASK|N_DEC|S_ENC,                    "divu",    R_TYPE, NULL,           0x2f1},
  {0x1c, NULL,          N_MASK|N_DEC|S_ENC,                    "dmult",   R_TYPE, NULL,           0x2f1},
  {0x1d, NULL,          N_MASK|N_DEC|S_ENC,                    "dmultu",  R_TYPE, NULL,           0x2f1},
  {0x1e, NULL,          N_MASK|N_DEC|S_ENC,                    "ddiv",    R_TYPE, NULL,           0x2f1},
  {0x1f, NULL,          N_MASK|N_DEC|S_ENC,                    "ddivu",   R_TYPE, NULL,           0x2f1},

  {0x20, NULL,          N_MASK|N_DEC|S_ENC,                    "add",     R_TYPE, NULL,           0x2f1f3},
/* prefer "or" over "addu" for move, since it is 64-bit safe */
  {0x25, NULL,          N_MASK|N_DEC|S_ENC|MAY_BE_MOVE,        "or",      R_TYPE, NULL,           0x2f1f3},
  {0x25, NULL,          N_MASK|N_DEC|S_ENC,                    "move",    R_TYPE, NULL,           0x1f3},
  {0x21, NULL,          N_MASK|N_DEC|S_ENC|MAY_BE_MOVE,        "addu",    R_TYPE, NULL,           0x2f1f3},
  {0x21, NULL,          N_MASK|N_DEC|S_ENC,                    "move",    R_TYPE, NULL,           0x1f3},
  {0x22, NULL,          N_MASK|N_DEC|S_ENC,                    "sub",     R_TYPE, NULL,           0x2f1f3},
  {0x23, NULL,          N_MASK|N_DEC|S_ENC,                    "subu",    R_TYPE, NULL,           0x2f1f3},
  {0x24, NULL,          N_MASK|N_DEC|S_ENC,                    "and",     R_TYPE, NULL,           0x2f1f3},
  {0x26, NULL,          N_MASK|N_DEC|S_ENC,                    "xor",     R_TYPE, NULL,           0x2f1f3},
  {0x27, NULL,          N_MASK|N_DEC|S_ENC,                    "nor",     R_TYPE, NULL,           0x2f1f3},

  {0x2a, NULL,          N_MASK|N_DEC|S_ENC,                    "slt",     R_TYPE, NULL,           0x2f1f3},
  {0x2b, NULL,          N_MASK|N_DEC|S_ENC,                    "sltu",    R_TYPE, NULL,           0x2f1f3},
  {0x2c, NULL,          N_MASK|N_DEC|S_ENC,                    "dadd",    R_TYPE, NULL,           0x2f1f3},
  {0x2d, NULL,          N_MASK|N_DEC|S_ENC|MAY_BE_MOVE,        "daddu",   R_TYPE, NULL,           0x2f1f3},
  {0x2d, NULL,          N_MASK|N_DEC|S_ENC,                    "dmove",   R_TYPE, NULL,           0x1f3},
  {0x2e, NULL,          N_MASK|N_DEC|S_ENC,                    "dsub",    R_TYPE, NULL,           0x2f1f3},
  {0x2f, NULL,          N_MASK|N_DEC|S_ENC,                    "dsubu",   R_TYPE, NULL,           0x2f1f3},
  {0x30, NULL,          N_MASK|N_DEC|S_ENC,                    "tge",     R_TYPE, NULL,           0x2f1},
  {0x31, NULL,          N_MASK|N_DEC|S_ENC,                    "tgeu",    R_TYPE, NULL,           0x2f1},
  {0x32, NULL,          N_MASK|N_DEC|S_ENC,                    "tlt",     R_TYPE, NULL,           0x2f1},
  {0x33, NULL,          N_MASK|N_DEC|S_ENC,                    "tltu",    R_TYPE, NULL,           0x2f1},
  {0x34, NULL,          N_MASK|N_DEC|S_ENC,                    "teq",     R_TYPE, NULL,           0x2f1},

  {0x36, NULL,          N_MASK|N_DEC|S_ENC,                    "tne",     R_TYPE, NULL,           0x2f1},

  {0x38, NULL,          N_MASK|N_DEC|S_ENC,                    "dsll",    R_TYPE, NULL,           0x4f2f3},

  {0x3a, NULL,          N_MASK|N_DEC|S_ENC,                    "dsrl",    R_TYPE, NULL,           0x4f2f3},
  {0x3b, NULL,          N_MASK|N_DEC|S_ENC,                    "dsra",    R_TYPE, NULL,           0x4f2f3},
  {0x3c, NULL,          N_MASK|N_DEC|S_ENC,                    "dsll32",  R_TYPE, NULL,           0x4f2f3},

  {0x3e, NULL,          N_MASK|N_DEC|S_ENC,                    "dsrl32",  R_TYPE, NULL,           0x4f2f3},
  {0x3f, NULL,          N_MASK|N_DEC|S_ENC,                    "dsra32",  R_TYPE, NULL,           0x4f2f3},

  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_rimm[] = {
  {0x00, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bltz",    I_TYPE, JUMP|COND,      0x7f1},
  {0x01, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bgez",    I_TYPE, JUMP|COND,      0x7f1},
  {0x02, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bltzl",   I_TYPE, JUMP|COND,      0x7f1},
  {0x03, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bgezl",   I_TYPE, JUMP|COND,      0x7f1},

  {0x08, NULL,          R_MASK|N_DEC|R_ENC,                    "tgei",    I_TYPE, NULL,           0x5f1},
  {0x09, NULL,          R_MASK|N_DEC|R_ENC,                    "tgeiu",   I_TYPE, NULL,           0x5f1},
  {0x0a, NULL,          R_MASK|N_DEC|R_ENC,                    "tlti",    I_TYPE, NULL,           0x5f1},
  {0x0b, NULL,          R_MASK|N_DEC|R_ENC,                    "tltiu",   I_TYPE, NULL,           0x5f1},
  {0x0c, NULL,          R_MASK|N_DEC|R_ENC,                    "teqi",    I_TYPE, NULL,           0x5f1},

  {0x0e, NULL,          R_MASK|N_DEC|R_ENC,                    "tnei",    I_TYPE, NULL,           0x5f1},

  {0x10, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bltzal",  I_TYPE, JUMP|LINK|COND, 0x7f1},
  {0x11, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bgezal",  I_TYPE, JUMP|LINK|COND, 0x7f1},
  {0x12, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bltzall", I_TYPE, JUMP|LINK|COND, 0x7f1},
  {0x13, NULL,          R_MASK|N_DEC|R_ENC|DIS_NEXT,           "bgezall", I_TYPE, JUMP|LINK|COND, 0x7f1},

  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_cp0[] = {
  {0x01, NULL,          N_MASK|N_DEC|CO_ENC,                   "tlbr",    R_TYPE, NULL,           0x0},
  {0x02, NULL,          N_MASK|N_DEC|CO_ENC,                   "tlbwi",   R_TYPE, NULL,           0x0},
  {0x06, NULL,          N_MASK|N_DEC|CO_ENC,                   "tlbwr",   R_TYPE, NULL,           0x0},
  {0x08, NULL,          N_MASK|N_DEC|CO_ENC,                   "tlbp",    R_TYPE, NULL,           0x0},
  {0x10, NULL,          N_MASK|N_DEC|CO_ENC,                   "rfe",     R_TYPE, NULL,           0x0},
  {0x18, NULL,          N_MASK|N_DEC|CO_ENC,                   "eret",    R_TYPE, NULL,           0x0},
  {0x20, NULL,          N_MASK|N_DEC|CO_ENC,                   "wait",    R_TYPE, NULL,           0x0},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_bc0[] = {
  {0x00, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc0f",    I_TYPE, JUMP|COND,      0x7},
  {0x01, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc0t",    I_TYPE, JUMP|COND,      0x7},
  {0x02, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc0fl",   I_TYPE, JUMP|COND,      0x7},
  {0x03, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc0tl",   I_TYPE, JUMP|COND,      0x7},
  {NULL, NULL,          NULL,                                   NULL,     E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_cop0[] = {
  {0x00, NULL,          C_MASK|N_DEC|C_ENC,                    "mfc0",    R_TYPE, NULL,           0xef2},
  {0x01, NULL,          C_MASK|N_DEC|C_ENC,                    "dmfc0",   R_TYPE, NULL,           0xef2},
  {0x02, NULL,          C_MASK|N_DEC|C_ENC,                    "cfc0",    R_TYPE, NULL,           0xef2},
  {0x04, NULL,          C_MASK|N_DEC|C_ENC,                    "mtc0",    R_TYPE, NULL,           0xef2},
  {0x05, NULL,          C_MASK|N_DEC|C_ENC,                    "dmtc0",   R_TYPE, NULL,           0xef2},
  {0x06, NULL,          C_MASK|N_DEC|C_ENC,                    "ctc0",    R_TYPE, NULL,           0xef2},

  {0x08, asm_tab_bc0,   C_MASK|CB_DEC|C_ENC,                   NULL,      NULL,   NULL,           NULL},
  {0x10, asm_tab_cp0,   CO_MASK|CO_DEC|C_ENC,                  NULL,      NULL,   NULL,           NULL},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_bc1[] = {
  {0x00, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc1f",    I_TYPE, JUMP|COND,      0x7},
  {0x01, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc1t",    I_TYPE, JUMP|COND,      0x7},
  {0x02, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc1fl",   I_TYPE, JUMP|COND,      0x7},
  {0x03, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc1tl",   I_TYPE, JUMP|COND,      0x7},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_cp1[] = {
  {0x00, NULL,          N_MASK|N_DEC|S_ENC,                    "add.",    F_TYPE, NULL,           0x9fafb},
  {0x01, NULL,          N_MASK|N_DEC|S_ENC,                    "sub.",    F_TYPE, NULL,           0x9fafb},
  {0x02, NULL,          N_MASK|N_DEC|S_ENC,                    "mul.",    F_TYPE, NULL,           0x9fafb},
  {0x03, NULL,          N_MASK|N_DEC|S_ENC,                    "div.",    F_TYPE, NULL,           0x9fafb},
  {0x04, NULL,          N_MASK|N_DEC|S_ENC,                    "sqrt.",   F_TYPE, NULL,           0x9fafb},
  {0x05, NULL,          N_MASK|N_DEC|S_ENC,                    "abs.",    F_TYPE, NULL,           0xafb},
  {0x06, NULL,          N_MASK|N_DEC|S_ENC,                    "mov.",    F_TYPE, NULL,           0xafb},
  {0x07, NULL,          N_MASK|N_DEC|S_ENC,                    "neg.",    F_TYPE, NULL,           0xafb},
  {0x08, NULL,          N_MASK|N_DEC|S_ENC,                    "round.l.",F_TYPE, NULL,           0xafb},
  {0x09, NULL,          N_MASK|N_DEC|S_ENC,                    "trunc.l.",F_TYPE, NULL,           0xafb},
  {0x0a, NULL,          N_MASK|N_DEC|S_ENC,                    "ceil.l.", F_TYPE, NULL,           0xafb},
  {0x0b, NULL,          N_MASK|N_DEC|S_ENC,                    "floor.l.",F_TYPE, NULL,           0xafb},
  {0x0c, NULL,          N_MASK|N_DEC|S_ENC,                    "round.w.",F_TYPE, NULL,           0xafb},
  {0x0d, NULL,          N_MASK|N_DEC|S_ENC,                    "trunc.w.",F_TYPE, NULL,           0xafb},
  {0x0e, NULL,          N_MASK|N_DEC|S_ENC,                    "ceil.w.", F_TYPE, NULL,           0xafb},
  {0x0f, NULL,          N_MASK|N_DEC|S_ENC,                    "floor.w.",F_TYPE, NULL,           0xafb},

  {0x11, NULL,          N_MASK|N_DEC|S_ENC|CF_TF,              "mov",     CF_TYPE, NULL,          0x4dfafb},
  {0x12, NULL,          N_MASK|N_DEC|S_ENC,                    "movz.",   F_TYPE, NULL,           0x2fafb},
  {0x13, NULL,          N_MASK|N_DEC|S_ENC,                    "movn.",   F_TYPE, NULL,           0x2fafb},

  {0x15, NULL,          N_MASK|N_DEC|S_ENC,                    "recip.",  F_TYPE, NULL,           0xafb},
  {0x16, NULL,          N_MASK|N_DEC|S_ENC,                    "rsqrt.",  F_TYPE, NULL,           0xafb},

  {0x20, NULL,          N_MASK|N_DEC|S_ENC,                    "cvt.s.",  F_TYPE, NULL,           0xafb},
  {0x21, NULL,          N_MASK|N_DEC|S_ENC,                    "cvt.d.",  F_TYPE, NULL,           0xafb},
  {0x22, NULL,          N_MASK|N_DEC|S_ENC,                    "cvt.e.",  F_TYPE, NULL,           0xafb},
  {0x23, NULL,          N_MASK|N_DEC|S_ENC,                    "cvt.q.",  F_TYPE, NULL,           0xafb},
  {0x24, NULL,          N_MASK|N_DEC|S_ENC,                    "cvt.w.",  F_TYPE, NULL,           0xafb},
  {0x25, NULL,          N_MASK|N_DEC|S_ENC,                    "cvt.l.",  F_TYPE, NULL,           0xafb},

  {0x30, NULL,          N_MASK|N_DEC|S_ENC,                    "c.f.",    F_TYPE, NULL,           0x9fa},
  {0x31, NULL,          N_MASK|N_DEC|S_ENC,                    "c.un.",   F_TYPE, NULL,           0x9fa},
  {0x32, NULL,          N_MASK|N_DEC|S_ENC,                    "c.eq.",   F_TYPE, NULL,           0x9fa},
  {0x33, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ueq.",  F_TYPE, NULL,           0x9fa},
  {0x34, NULL,          N_MASK|N_DEC|S_ENC,                    "c.olt.",  F_TYPE, NULL,           0x9fa},
  {0x35, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ult.",  F_TYPE, NULL,           0x9fa},
  {0x36, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ole.",  F_TYPE, NULL,           0x9fa},
  {0x37, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ule.",  F_TYPE, NULL,           0x9fa},
  {0x38, NULL,          N_MASK|N_DEC|S_ENC,                    "c.sf.",   F_TYPE, NULL,           0x9fa},
  {0x39, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ngle.", F_TYPE, NULL,           0x9fa},
  {0x3a, NULL,          N_MASK|N_DEC|S_ENC,                    "c.seq.",  F_TYPE, NULL,           0x9fa},
  {0x3b, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ngl.",  F_TYPE, NULL,           0x9fa},
  {0x3c, NULL,          N_MASK|N_DEC|S_ENC,                    "c.lt.",   F_TYPE, NULL,           0x9fa},
  {0x3d, NULL,          N_MASK|N_DEC|S_ENC,                    "c.nge.",  F_TYPE, NULL,           0x9fa},
  {0x3e, NULL,          N_MASK|N_DEC|S_ENC,                    "c.le.",   F_TYPE, NULL,           0x9fa},
  {0x3f, NULL,          N_MASK|N_DEC|S_ENC,                    "c.ngt.",  F_TYPE, NULL,           0x9fa},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_cop1[] = {
  {0x00, NULL,          C_MASK|N_DEC|C_ENC,                    "mfc1",    R_TYPE, NULL,           0xaf2},
  {0x01, NULL,          C_MASK|N_DEC|C_ENC,                    "dmfc1",   R_TYPE, NULL,           0xaf2},
  {0x02, NULL,          C_MASK|N_DEC|C_ENC,                    "cfc1",    R_TYPE, NULL,           0xaf2},
  {0x04, NULL,          C_MASK|N_DEC|C_ENC,                    "mtc1",    R_TYPE, NULL,           0xaf2},
  {0x05, NULL,          C_MASK|N_DEC|C_ENC,                    "dmtc1",   R_TYPE, NULL,           0xaf2},
  {0x06, NULL,          C_MASK|N_DEC|C_ENC,                    "ctc1",    R_TYPE, NULL,           0xaf2},
  {0x08, asm_tab_bc1,   C_MASK|CB_DEC|C_ENC,                   NULL,      NULL,   NULL,           NULL},
  {0x10, asm_tab_cp1,   CO_MASK|CO_DEC|C_ENC,                  NULL,      NULL,   NULL,           NULL},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_bc2[] = {
  {0x00, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc2f",    I_TYPE, JUMP|COND,      0x7},
  {0x01, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc2t",    I_TYPE, JUMP|COND,      0x7},
  {0x02, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc2fl",   I_TYPE, JUMP|COND,      0x7},
  {0x03, NULL,          CB_MASK|N_DEC|CB_ENC|DIS_NEXT,         "bc2tl",   I_TYPE, JUMP|COND,      0x7},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};

const struct asm_tab    asm_tab_cop2[] = {
  {0x00, NULL,          C_MASK|N_DEC|C_ENC,                    "mfc2",    R_TYPE, NULL,           0xef2},
  {0x01, NULL,          C_MASK|N_DEC|C_ENC,                    "dmfc2",   R_TYPE, NULL,           0xef2},
  {0x02, NULL,          C_MASK|N_DEC|C_ENC,                    "cfc2",    R_TYPE, NULL,           0xef2},
  {0x04, NULL,          C_MASK|N_DEC|C_ENC,                    "mtc2",    R_TYPE, NULL,           0xef2},
  {0x05, NULL,          C_MASK|N_DEC|C_ENC,                    "dmtc2",   R_TYPE, NULL,           0xef2},
  {0x06, NULL,          C_MASK|N_DEC|C_ENC,                    "ctc2",    R_TYPE, NULL,           0xef2},
  {0x08, asm_tab_bc2,   C_MASK|CB_DEC|C_ENC,                   NULL,      NULL,   NULL,           NULL},
  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};


const struct asm_tab    asm_tab_cop1x[] = {
  {0x00, NULL,          N_MASK|N_DEC|CO_ENC,                   "lwxc1",   X_TYPE, NULL,           0x82fb},
  {0x01, NULL,          N_MASK|N_DEC|CO_ENC,                   "ldxc1",   X_TYPE, NULL,           0x82fb},

  {0x08, NULL,          N_MASK|N_DEC|CO_ENC,                   "swxc1",   X_TYPE, NULL,           0x82fb},
  {0x09, NULL,          N_MASK|N_DEC|CO_ENC,                   "sdxc1",   X_TYPE, NULL,           0x82fb},

  {0x0F, NULL,          N_MASK|N_DEC|CO_ENC,                   "prefx",   X_TYPE, NULL,           0x82f3d},

  {0x20, NULL,          CX_MASK|N_DEC|C_ENC,                   "madd.",   X_TYPE, NULL,           0x9faf2dfb},

  {0x28, NULL,          CX_MASK|N_DEC|C_ENC,                   "msub.",   X_TYPE, NULL,           0x9faf2dfb},

  {0x30, NULL,          CX_MASK|N_DEC|C_ENC,                   "nmadd.",  X_TYPE, NULL,           0x9faf2dfb},

  {0x38, NULL,          CX_MASK|N_DEC|C_ENC,                   "nmsub.",  X_TYPE, NULL,           0x9faf2dfb},

  {NULL, NULL,          NULL,                                  NULL,      E_TYPE, NULL,           NULL}
};


const struct asm_tab    asm_tab_op[] = {
  {0x00, asm_tab_spec,  N_MASK|S_DEC|N_ENC,                    NULL,     NULL,    NULL,           NULL},
  {0x01, asm_tab_rimm,  N_MASK|R_DEC|N_ENC,                    NULL,     NULL,    NULL,           NULL},
  {0x02, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "j",      J_TYPE,  JUMP,           0x7},
  {0x03, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "jal",    J_TYPE,  JUMP|LINK,      0x7},
  {0x04, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT|MAY_BE_BR, "beq",    I_TYPE,  JUMP|COND,      0x7f2f1},
  {0x04, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "b",      I_TYPE,  JUMP,           0x7},
  {0x05, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "bne",    I_TYPE,  JUMP|COND,      0x7f2f1},
  {0x06, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "blez",   I_TYPE,  JUMP|COND,      0x7f1},
  {0x07, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "bgtz",   I_TYPE,  JUMP|COND,      0x7f1},
  {0x08, NULL,          N_MASK|N_DEC|N_ENC,                    "addi",   I_TYPE,  NULL,           0x5f1f2},
  {0x09, NULL,          N_MASK|N_DEC|N_ENC|MAY_BE_LI,          "addiu",  I_TYPE,  NULL,           0x5f1f2},
  {0x09, NULL,          N_MASK|N_DEC|N_ENC,                    "li",     I_TYPE,  NULL,           0x5f2},
  {0x0a, NULL,          N_MASK|N_DEC|N_ENC,                    "slti",   I_TYPE,  NULL,           0x5f1f2},
  {0x0b, NULL,          N_MASK|N_DEC|N_ENC,                    "sltiu",  I_TYPE,  NULL,           0x5f1f2},
  {0x0c, NULL,          N_MASK|N_DEC|N_ENC,                    "andi",   I_TYPE,  NULL,           0x6f1f2},
  {0x0d, NULL,          N_MASK|N_DEC|N_ENC,                    "ori",    I_TYPE,  NULL,           0x6f1f2},
  {0x0e, NULL,          N_MASK|N_DEC|N_ENC,                    "xori",   I_TYPE,  NULL,           0x6f1f2},
  {0x0f, NULL,          N_MASK|N_DEC|N_ENC,                    "lui",    I_TYPE,  NULL,           0x6f2},
  {0x10, asm_tab_cop0,  N_MASK|C_DEC|N_ENC,                    NULL,     NULL,    NULL,           NULL},
  {0x11, asm_tab_cop1,  N_MASK|C_DEC|N_ENC,                    NULL,     NULL,    NULL,           NULL},
  {0x12, asm_tab_cop2,  N_MASK|C_DEC|N_ENC,                    NULL,     NULL,    NULL,           NULL},
  {0x13, asm_tab_cop1x, N_MASK|CO_DEC|N_ENC,                   NULL,     NULL,    NULL,           NULL},
  {0x14, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "beql",   I_TYPE,  JUMP|COND,      0x7f2f1},
  {0x15, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "bnel",   I_TYPE,  JUMP|COND,      0x7f2f1},
  {0x16, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "blezl",  I_TYPE,  JUMP|COND,      0x7f1},
  {0x17, NULL,          N_MASK|N_DEC|N_ENC|DIS_NEXT,           "bgtzl",  I_TYPE,  JUMP|COND,      0x7f1},
  {0x18, NULL,          N_MASK|N_DEC|N_ENC,                    "daddi",  I_TYPE,  NULL,           0x5f1f2},
  {0x19, NULL,          N_MASK|N_DEC|N_ENC,                    "daddiu", I_TYPE,  NULL,           0x5f1f2},
  {0x1a, NULL,          N_MASK|N_DEC|N_ENC,                    "ldl",    I_TYPE,  NULL,           0x85f2},
  {0x1b, NULL,          N_MASK|N_DEC|N_ENC,                    "ldr",    I_TYPE,  NULL,           0x85f2},

  {0x20, NULL,          N_MASK|N_DEC|N_ENC,                    "lb",     I_TYPE,  LOAD,           0x85f2},
  {0x21, NULL,          N_MASK|N_DEC|N_ENC,                    "lh",     I_TYPE,  LOAD,           0x85f2},
  {0x22, NULL,          N_MASK|N_DEC|N_ENC,                    "lwl",    I_TYPE,  LOAD,           0x85f2},
  {0x23, NULL,          N_MASK|N_DEC|N_ENC,                    "lw",     I_TYPE,  LOAD,           0x85f2},
  {0x24, NULL,          N_MASK|N_DEC|N_ENC,                    "lbu",    I_TYPE,  LOAD,           0x85f2},
  {0x25, NULL,          N_MASK|N_DEC|N_ENC,                    "lhu",    I_TYPE,  LOAD,           0x85f2},
  {0x26, NULL,          N_MASK|N_DEC|N_ENC,                    "lwr",    I_TYPE,  LOAD,           0x85f2},
  {0x27, NULL,          N_MASK|N_DEC|N_ENC,                    "lwu",    I_TYPE,  LOAD,           0x85f2},
  {0x28, NULL,          N_MASK|N_DEC|N_ENC,                    "sb",     I_TYPE,  STORE,          0x85f2},
  {0x29, NULL,          N_MASK|N_DEC|N_ENC,                    "sh",     I_TYPE,  STORE,          0x85f2},
  {0x2a, NULL,          N_MASK|N_DEC|N_ENC,                    "swl",    I_TYPE,  STORE,          0x85f2},
  {0x2b, NULL,          N_MASK|N_DEC|N_ENC,                    "sw",     I_TYPE,  STORE,          0x85f2},
  {0x2c, NULL,          N_MASK|N_DEC|N_ENC,                    "sdl",    I_TYPE,  STORE,          0x85f2},
  {0x2d, NULL,          N_MASK|N_DEC|N_ENC,                    "sdr",    I_TYPE,  STORE,          0x85f2},
  {0x2e, NULL,          N_MASK|N_DEC|N_ENC,                    "swr",    I_TYPE,  STORE,          0x85f2},
  {0x2f, NULL,          N_MASK|N_DEC|N_ENC,                    "cache",  I_TYPE,  STORE,          0x85f1d},
  {0x30, NULL,          N_MASK|N_DEC|N_ENC,                    "ll",     I_TYPE,  LOAD,           0x85f2},
  {0x31, NULL,          N_MASK|N_DEC|N_ENC,                    "lwc1",   I_TYPE,  LOAD,           0x85f9},
  {0x32, NULL,          N_MASK|N_DEC|N_ENC,                    "lwc2",   I_TYPE,  LOAD,           0x85f2},
  {0x33, NULL,          N_MASK|N_DEC|N_ENC,                    "pref",   R_TYPE,  LOAD,           0x85f2},
  {0x34, NULL,          N_MASK|N_DEC|N_ENC,                    "lld",    I_TYPE,  LOAD,           0x85f2},
  {0x35, NULL,          N_MASK|N_DEC|N_ENC,                    "ldc1",   I_TYPE,  LOAD,           0x85f9},
  {0x36, NULL,          N_MASK|N_DEC|N_ENC,                    "ldc2",   I_TYPE,  LOAD,           0x85f2},
  {0x37, NULL,          N_MASK|N_DEC|N_ENC,                    "ld",     I_TYPE,  LOAD,           0x85f2},
  {0x38, NULL,          N_MASK|N_DEC|N_ENC,                    "sc",     I_TYPE,  STORE,          0x85f2},
  {0x39, NULL,          N_MASK|N_DEC|N_ENC,                    "swc1",   I_TYPE,  STORE,          0x85f9},
  {0x3a, NULL,          N_MASK|N_DEC|N_ENC,                    "swc2",   I_TYPE,  STORE,          0x85f2},
  {0x3b, NULL,          N_MASK|N_DEC|N_ENC,                    "swc3",   I_TYPE,  STORE,          0x85f2},
  {0x3c, NULL,          N_MASK|N_DEC|N_ENC,                    "scd",    I_TYPE,  LOAD,           0x85f2},
  {0x3d, NULL,          N_MASK|N_DEC|N_ENC,                    "sdc1",   I_TYPE,  LOAD,           0x85f9},
  {0x3e, NULL,          N_MASK|N_DEC|N_ENC,                    "sdc2",   I_TYPE,  LOAD,           0x85f2},
  {0x3f, NULL,          N_MASK|N_DEC|N_ENC,                    "sd",     I_TYPE,  LOAD,           0x85f2},
  {NULL, NULL,          NULL,                                  NULL,     E_TYPE,  NULL,           NULL}
};


/*
**  CPU registers
*/
char    *const rregs[2][32] = {
  {
   "zero","at","v0","v1","a0","a1","a2","a3",
   "t0","t1","t2","t3","t4","t5","t6","t7",
   "s0","s1","s2","s3","s4","s5","s6","s7",
   "t8","t9","k0","k1","gp","sp","fp","ra"
  },
  {
   "r00","r01","r02","r03","r04","r05","r06","r07",
   "r08","r09","r10","r11","r12","r13","r14","r15",
   "r16","r17","r18","r19","r20","r21","r22","r23",
   "r24","r25","r26","r27","r28","r29","r30","r31"
  }
};


/*
** Coprocessor (system) 0 registers
*/
static  char    *const c0regs[32] = {
  "index","random","tlblo0","tlblo1","context","pagemask","wired","7",
  "badvaddr","count","tlbhi","compare","sr","cr","epc","prid",
  "config","lladdr","watchlo","watchhi","xcontext","21","22","23",
  "24","25","ecc","cacheerr","taglo","taghi","errpc","31"
};


/*
** Coprocessor (FPU) 1 registers
*/
static  char    *const c1regs[32] = {
  "0","1","2","3","4","5","6","7",
  "8","9","10","11","12","13","14","15",
  "16","17","18","19","20","21","22","23",
  "24","25","26","27","28","29","feir","fcsr"
};

char    *const fregs[32] = {
  "f00","f01","f02","f03","f04","f05","f06","f07",
  "f08","f09","f10","f11","f12","f13","f14","f15",
  "f16","f17","f18","f19","f20","f21","f22","f23",
  "f24","f25","f26","f27","f28","f29","f30","f31"
};


/*
**	static definitions used by the idt disassembler 
*/
        int         op;             /* instruction opcode */
static  int         rs;             /* rs register number */
static  int         rt;             /* rt register number */
static  int         rd;             /* rd register number */
static  int         shamt;          /* shift amount */
static  int         funct;          /* function code for special */
static  short       s_imd;          /* signed immediate value */
static  unsigned    u_imd;          /* unsigned immediate value */
static  int         target;         /* target address of br and jump */
static  int         format;         /* floating point format */
static  int         base;           /* the index reg in loads and stores */
/* 'fr', 'ft', 'fs' and 'fd' are GLOBAL so the exception handler can see them */
        int         fr;             /* floating pt fr reg # */
        int         ft;             /* floating pt ft reg # */
        int         fs;             /* floating pt fs reg # */
        int         fd;             /* floating pt fd reg # */
static  int         tf;             /* move conditional t/f flag */
static  int         cc;             /* condition code for move conditional */

static  int         bad_arg;
//static  int         char_cnt;
static  int         delay_slot;     /* a flag to signal next instr in delay slot */
static  int         reg_set;        /* what set of regs to use for display */
static  int         der_regs[4];    /* contains list of regs used in this inst */
static  int         is_fp_reg[4];   /* if regs were floating point */
static  int         num_regs;       /* the num. of regs in the arrays above */
static  union       r5kInst a_inst;
        union       r5kInst d_inst;


static void find_op(const struct asm_tab *asmtab, union r5kInst inst, int mode, unsigned *addr);
static int  do_opt_decode(int code, union r5kInst inst);
static void do_decode(int type, union r5kInst inst, unsigned *addr);
static int  print_mnem(const struct asm_tab *asmtab, int i);
static int  print_arg(int code);


int disasm(unsigned *addr, int mode, int regset, int first) /*
  entry:
    addr   - address of instruction to be disassembled
    mode   - flag to tell if the registers can be shown
             also a bit 'DO_DELAY' controls if branch delay
             slots are treated autominously
    regset - 0 = use compiler notation for reg display
             1 = use raw hardware notation

  return:
    Returns the number of bytes disassembled. This is
    normally 4 for one instruction. If the delay slot
    was disassembled also, the number will be 8.
    retuns 0 if error.

  notes:
    This routine may recurse if the instruction in the delay
    slot needs to be disassembled. Branches and jumps and
    their delay slots are treated autominously (controlled by
    bit 1 in the mode flag - needs to be a one (1) if autominous).
*/
{
    union r5kInst   inst;
    int             i, num_bytes;


	reg_set = regset;
    num_regs = 0;

//    if (first) char_cnt = 0;

    for (i = 0; i < 4; i++) is_fp_reg[i] = 0;
    num_bytes = 4;

    if (!(mode & NO_ADDR)) printf("0x%08X:\t", addr);

    if ((int)addr & 3 != 0) {
        printf("(disasm): Unaligned Address Specified\n");
        return(0);
    }

    d_inst.asm_code = inst.asm_code = *addr;
    op = inst.r_type.op;

    //if (!(mode & SHOW_REGS)) printf("0x%08X\t", inst.asm_code);
    if (!(mode & NO_INST)) printf("%08X\t", inst.asm_code);

    find_op(asm_tab_op, inst, mode, addr);

    if ((mode & DO_DELAY) && (delay_slot == 1)) {
        printf("\n");
        disasm(addr+1, mode, regset, FALSE);
        num_bytes += 4;
    }

    return (num_bytes);
}


static void find_op(const struct asm_tab *asmtab, union r5kInst inst, int mode, unsigned *addr) /*
  Searches the asm_tabs for opcode and follows the path
  until the instruction is completely disassembled.

  entry:
    asmtab - pointer to the asm_tab
    inst   - instruction to disassemble
    mode   - flag to tell if registers and instructions can be shown
    addr   - the address of the instruction

  return:
    none
*/
{
    int     i, j, argkey, tmp_op;
    int     escaped;


    for (i = 0; asmtab[i].inst_type != E_TYPE; i++) {

        tmp_op = op & ((asmtab[i].opt_decode & OPT_OP_MASK) >> 8);
        if (tmp_op == asmtab[i].op_val) {
            if ((asmtab[i].opt_decode & OPT_DEC_MASK) != 0) {
                do_opt_decode((asmtab[i].opt_decode & OPT_DEC_MASK), inst);
                find_op(asmtab[i].asm_tab_ptr, inst, mode, addr);
                return;
            } else {
                do_decode(asmtab[i].inst_type, inst, addr);
                if (!(mode & NO_INST)) {
                    i = print_mnem(asmtab, i);
                    argkey = asmtab[i].arg_order;
                    j = escaped = 0;
                    while ((argkey & 0xF) && (j < 8)) {
                        if ((argkey & 0xF) == ESC_ARG) {
                            escaped = ESC_ESC;
                        } else {
                            print_arg((argkey & 0xF) | escaped);
                            escaped = 0;
                        }

                        j++;
                        argkey = argkey >> 4;
                    }
                }
                break;
            }
        }
    }

    if ((asmtab[i].opt_decode & DELAY_MASK) == DIS_NEXT) {
        delay_slot = 1;
    } else {
        delay_slot = 0;
    }

    if ((mode & SHOW_REGS) && (num_regs > 0)) {
        for (i = 0; i < num_regs; i++) {
				if(!(i & 3))
					printf("\n");
				else
					printf(", ");

            if (mode & USE_EXCEPT) {
                if (is_fp_reg[i] == 0) {
                    printf("%s=0x%08X", rregs[reg_set][der_regs[i]], except_regs[der_regs[i] << 1]);
                } else {
                    printf("%s=0x%08X", fregs[der_regs[i]], except_regs[(is_fp_reg[i]<<1)+70]);
                }
            } else {
                if (is_fp_reg[i] == 0) {
                    printf("%s=0x%08X", rregs[reg_set][der_regs[i]], except_regs[der_regs[i] << 1]);
                } else {
                    printf("%s=0x%08X", fregs[der_regs[i]], except_regs[(is_fp_reg[i]<<1)+70]);
                }
		    }	
        }

	    printf(">");
    }

    if (asmtab[i].inst_type == E_TYPE) {
        if (mode & SHOW_REGS) printf("0x%08X\t", inst.asm_code);
        if (!(mode & NO_INST)) {
            printf("%08X\t", inst.asm_code);
            printf("reserved");
        }
    }
}


static int do_opt_decode(int code, union r5kInst inst) /*
  entry:
    code - type of decode to do :
      N_DEC  - normal decode (no-op)
      S_DEC  - extract op for special class
      R_DEC  - register immediate instructions
      C_DEC  - Coprocessor instruction
      CO_DEC - Coprocessor operation instruction
      CB_DEC - Coprocessor branch instruction

  return:
    none
*/
{
    switch (code) {
      case N_DEC:
        break;
      case S_DEC:
		op = inst.r_type.funct;
		break;
      case R_DEC:
		op = inst.r_type.rt;
		break;
      case C_DEC:
		op = inst.r_type.rs;
		break;
      case CO_DEC:
		op = inst.r_type.funct;
		break;
      case CB_DEC:
		op = inst.r_type.rt;
        break;
    }
}


static void do_decode(int type, union r5kInst inst, unsigned *addr) /*
  Extracts the operands for the particular instruction type.

  entry:
    type - the type of instruction
    inst - the instruction to decode
    addr - the address of the instr. ( used for calc. offsets and targets )

  return:
    none
*/
{

    switch (type) {
      case J_TYPE:
        target = ((unsigned)addr & 0xF0000000) | inst.j_type.target << 2;
		break;
      case R_TYPE:
		rs = inst.r_type.rs;
		rt = inst.r_type.rt;
		rd = inst.r_type.rd;
		fs = inst.f_type.fs;
		shamt = inst.r_type.shamt;
		funct = inst.r_type.funct;
        break;
      case I_TYPE:
		base = inst.i_type.rs;
		rs = base;
		rt = inst.i_type.rt;
		ft = inst.f_type.ft;
		rd = rt;
        s_imd = inst.i_type.s_imd;
        target = (unsigned)addr + (s_imd << 2) + 4;
        u_imd = inst.u_type.u_imd;
		break;
      case F_TYPE:
		format = inst.f_type.fmt;
		ft = inst.f_type.ft;
		fs = inst.f_type.fs;
		fd = inst.f_type.fd;
		rt = inst.r_type.rt;
		funct = inst.f_type.funct;
        break;
      case X_TYPE:
        base = inst.i_type.rs;
        fr = inst.x_type.fr;
        ft = inst.x_type.ft;
        fs = inst.x_type.fs;
        fd = inst.x_type.fd;
        format = inst.x_type.funct & 0x7;
        break;
      case CI_TYPE:
        rs = inst.r_type.rs;
        cc = inst.r_type.rt >> 2;
        tf = inst.r_type.rt & 1;
        rd = inst.r_type.rd;
        break;
      case CF_TYPE:
        format = inst.f_type.fmt;
        cc = inst.f_type.ft >> 2;
        tf = inst.f_type.ft & 1;
        fs = inst.x_type.fs;
        fd = inst.x_type.fd;
        break;

    }
}


static int print_mnem(const struct asm_tab *asmtab, int i) /*
  Prints the instruction mnemonic

  entry:
    asm_tab - pointer to an asmtab array of structures.
    i       - index into the array.

  return:
    returns the updated asm_tab index.

  notes:
    This routine may recurse if there is a possible alternate
    mnemonic for this instruction (i.e addu/move). Depending
    on the table, recursion could take place multiple times.
*/
{
    int     omn_code, tf_code;
    char    *ch;

    omn_code = asmtab[i].opt_decode & OPT_MN_MASK;

    switch (omn_code) {
      case MAY_BE_MOVE:
        if ( rt == 0 ) {
            i = print_mnem(asmtab, ++i);
            return (i);
        }
		break;
      case MAY_BE_NOP:
        if ((rt == 0) && (rs == 0) && (rd == 0) && (shamt == 0)) {
            i = print_mnem(asmtab, ++i);
            return (i);
        }
		break;
      case MAY_BE_BR:
        if ((rt == 0) && (rs == 0)) {
            i = print_mnem(asmtab, ++i);
            return (i);
        }
        break;
      case MAY_BE_LI:
        if (rs == 0) {
            i = print_mnem(asmtab, ++i);
            return (i);
        }
      default:
        break;
    }

    tf_code = asmtab[i].opt_decode & OPT_TF_MASK;
    switch (tf_code) {
      case CI_TF:
        ch = (tf == 0) ? "f" : "t";
        break;
      case CF_TF:
        ch = (tf == 0) ? "f." : "t.";
        break;
      default:
        break;
    }

    if (tf_code) {
        printf("%s%s", asmtab[i].mnem, ch);
    } else {
        printf("%s", asmtab[i].mnem);
        ch = asmtab[i].mnem;
    }

    while (*ch++ != NULL);
    ch -= 2;
    if (*ch == '.') printf("%c", "sdeqwl??????????"[format]);
    printf("\t");

    return (i);
}


static int print_arg(int code) /*
  Prints the instruction's arguments

  entry:
    code - argument code

  return:
    number of characters printed.
*/
{
    int     argcnt;

    switch (code) {
      case RS_ARG:
        argcnt = printf("%s", rregs[reg_set][rs]);
		der_regs[num_regs++] = rs;
		break;
      case RT_ARG:
        argcnt = printf("%s", rregs[reg_set][rt]);
		der_regs[num_regs++] = rt;
		break;
      case RD_ARG:
        argcnt = printf("%s", rregs[reg_set][rd]);
		der_regs[num_regs++] = rd;
		break;
      case SHAMT_ARG:
        argcnt = printf("%d", shamt);
		break;
      case SIGNED_IMD_ARG:
        argcnt = printf("0x%X", s_imd);
		break;
      case UNSIGNED_IMD_ARG:
        argcnt = printf("0x%X", u_imd);
		break;
      case TARGET_ARG:
        argcnt = printf("0x%X", target);
		break;
      case BASE_ARG:
        argcnt = printf("(%s)", rregs[reg_set][base]);
		der_regs[num_regs++] = base;
        break;
      case FT_ARG:
        argcnt = printf("%s", fregs[ft]);
		der_regs[num_regs] = ft;
		is_fp_reg[num_regs++] = ft+32;
		break;
      case FS_ARG:
        argcnt = printf("%s", fregs[fs]);
		der_regs[num_regs] = fs;
		is_fp_reg[num_regs++] = fs+32;
		break;
      case FD_ARG:
        argcnt = printf("%s", fregs[fd]);
		der_regs[num_regs] = fd;
		is_fp_reg[num_regs++] = fd+32;
		break;
      case CODE_ARG:
        argcnt = printf("0x%X", ((rs << 10) + (rt << 5) + rd));
		break;
      case C0_ARG:
        argcnt = printf("%s",c0regs[rd]);
		break;
      case COMMA_ARG:
        argcnt = printf(",");
		break;
      case ESC_CACHE_ARG:
        argcnt = printf("%d", rt);
		break;
      case ESC_FR_ARG:
        argcnt = printf("%s", fregs[fr]);
        der_regs[num_regs] = fr;
        is_fp_reg[num_regs++] = fr+32;
		break;
      case ESC_PREFX_ARG:
        argcnt = printf("%d", rd);
        break;
      case ESC_CC_ARG:
        argcnt = printf("%d", cc);
        break;
    }

    return (argcnt);
}


#if 0
int disasm_printf(fmt, va_alist)
char *fmt;
va_dcl
{
    int     count;
	va_list ap;

	va_start(ap);
    count = vsprintf(&disasm_buf[char_cnt], fmt, ap);
    va_end(ap);

    char_cnt += count;

    return (count);
}




/*
** get_arg(code,reg) - returns argument value based on code
*/
static reg_t
get_arg(int code, int *reg)
{
    switch (code) {
      case RS_ARG:
		*reg = rs;
        return (except_regs[rs]);
		break;
      case RT_ARG:
		*reg = rt;
        return (except_regs[rt]);
        break;
      case RD_ARG:
		*reg = rd;
        return (except_regs[rd]);
		break;
      case SHAMT_ARG:
        *reg = 0xF4;
        return (shamt);
		break;
      case SIGNED_IMD_ARG:
        *reg = 0xF1;
        return (s_imd);
		break;
      case UNSIGNED_IMD_ARG:
        *reg = 0xF2;
        return (u_imd);
		break;
      case TARGET_ARG:
        *reg = 0xF0;
        return (target);
		break;
      case BASE_ARG:
		*reg = base;
        return (except_regs[base]);
		break;
      case FT_ARG:
		*reg = ft + 32;
        return (except_regs[ft+32]);
		break;
      case FS_ARG:
		*reg = fs + 32;
        return (except_regs[fs+32]);
		break;
      case FD_ARG:
		*reg = fd + 32;
        return (except_regs[fd+32]);
		break;
      case CODE_ARG:
        *reg = 0xF5;
        return ((rs << 10) + (rt << 5) + rd);
		break;
      case C0_ARG:
        *reg = 0xF6;
        return (0);
		break;
      case ESC_CACHE_ARG:
        *reg = 0xFD1;
        return (0);
		break;
    }

	return(0);
}


/*
** fill_args(tr_ptr,asmtab) - saves args in trace buffer - 
**
**	entry - pointer to trace buffer entry
**		pointer to asmtab
**
*/
fill_args(tr_tab *tr_ptr, struct asm_tab *asmtab)
{
    int     i, argkey, reg;
    int     escaped;

	escaped = 0;

	argkey = asmtab->arg_order;
    i = 0;

    while (((argkey & 0xF) != 0) && (i < 4)) {
        if ((argkey & 0xF) == ESC_ARG) {
            escaped = ESC_ESC;
        } else if ((argkey & 0xF) != COMMA_ARG) {
            tr_ptr->arg[i] = get_arg((argkey & 0xF) | escaped, &reg);
            escaped = 0;
            tr_ptr->argnum[i++] = reg;
        }

        argkey = argkey >> 4;
    }

    while (i < 4) tr_ptr->argnum[i++] = 0xFF;
}


/*
** do_encode(asmptr) - builds instruction
*/
do_encode(struct asm_tab asmptr, int code)
{
    switch (code) {
     case N_ENC:
		a_inst.r_type.op = asmptr->op_val;
		break;
     case S_ENC:
		a_inst.r_type.funct = asmptr->op_val;
		break;
     case R_ENC:
		a_inst.r_type.rt = asmptr->op_val;
        break;
     case C_ENC:
		a_inst.r_type.rs = asmptr->op_val;
		break;
     case CO_ENC:
		a_inst.r_type.funct = asmptr->op_val;
		break;
     case CB_ENC:
		a_inst.r_type.rt = asmptr->op_val;
		break;
     case S2_ENC:
		a_inst.r_type.funct = asmptr->op_val;
        break;
    }
}


/*
**	find_mnem(asmtable,mnem) returns a pointer to the asm_tab entry
**		for mnemonic passed - 
**	returns null pointer if couldn't find
*/
find_mnem(struct asm_tab *asmtable, struct asm_tab **asmtab_found, char *mnem)
{
    struct asm_tab  *asmptr;


    for (asmptr = asmtable; (asmptr->inst_type!=E_TYPE) &&
                            (*asmtab_found == NULL); asmptr++) {

        if (asmptr->mnem == NULL) {
            find_mnem(asmptr->asm_tab_ptr, asmtab_found, mnem);

            if (*asmtab_found != NULL) {
                do_encode(asmptr, (asmptr->opt_decode & OPT_ENC_MASK));
                return(TRUE);
            }
        } else if (streq(asmptr->mnem, mnem)) {
            *asmtab_found = asmptr;
            do_encode(asmptr, (asmptr->opt_decode & OPT_ENC_MASK));
            return(TRUE);
        }
    }

    return (FALSE);
}


/*
** get_reg_value(arg) find the value for the register pointed to by arg
*/
int get_reg_value(char *arg)
{
    int     i;


    for(i = 0; i < 32; i++) {
        if (streq(rregs[0][i], arg)) return (i);

        if (streq(rregs[1][i], arg)) return (i);
    }

    bad_arg = 1;

    return (0);
}


/*
** get_freg_value(arg) find the value for the register pointed to by arg
*/
int get_freg_value(char *arg)
{
    int     i;


    for (i = 0; i < 32; i++) if (streq(fregs[i], arg)) return (i);

    bad_arg = 1;

	return(0);
}


/*
** get_c0reg_value(arg) find the value for the register pointed to by arg
*/
int get_c0reg_value(char *arg)
{
    int     i;


    for (i = 0; i < 32; i++) if (streq(c0regs[i], arg)) return (i);

    bad_arg = 1;

	return(0);
}


/*
** arg_encode(aptr,ode,arg) - get value for arg and or into instruction
**	code = argument code( i.e. rs, rt, immediate ... )
*/
void arg_encode(struct asm_tab *aptr, int code, char *arg)
{
    int     value;

    switch (code) {
      case RS_ARG:
      case BASE_ARG:
		value = get_reg_value(arg);
		a_inst.r_type.rs = value;
		break;
      case RT_ARG:
		value = get_reg_value(arg);
		a_inst.r_type.rt = value;
		break;
      case RD_ARG:
		value = get_reg_value(arg);
		a_inst.r_type.rd = value;
		break;
      case SHAMT_ARG:
        if (*atob(arg, &value, 10, 0) != '\0') bad_arg = 1;
        a_inst.r_type.shamt = value;
		break;
      case SIGNED_IMD_ARG:
        if (*atob(arg, &value, 10, 0) != '\0') bad_arg = 1;
        a_inst.i_type.s_imd = value;
		break;
      case UNSIGNED_IMD_ARG:
        if (*atob(arg, &value, 10, 0) != '\0') bad_arg = 1;
        a_inst.u_type.u_imd = value;
		break;
      case TARGET_ARG:
        if (*atob(arg, &value, 16, 0) != '\0') bad_arg = 1;
        if ((aptr->inst_type & I_TYPE) && (aptr->br_jmp_type & JUMP)) {
           a_inst.i_type.s_imd = value - 1;
        } else {
           a_inst.j_type.target = (value >> 2) & 0x3FFFFFF;
        }
		break;
      case FT_ARG:
		value = get_freg_value(arg);
		a_inst.f_type.ft = value;
		break;
      case FS_ARG:
		value = get_freg_value(arg);
		a_inst.f_type.fs = value;
		break;
      case FD_ARG:
		value = get_freg_value(arg);
		a_inst.f_type.fd = value;
		break;
      case CODE_ARG:
        if (*atob(arg, &value, 10, 0) != '\0') bad_arg = 1;
		a_inst.r_type.rs = value;
		break;
      case C0_ARG:
		value = get_c0reg_value(arg);
		a_inst.r_type.rd = value;
		break;
    }
}


/*
**	asmbler(str,ptrval) - assemble instruction passed in str and put in 
**			ptrval. 
**	str points to the instruction string
**	ptrval - is the location to store the assembled instruction
**	
**  returns: 1  - if instruction could be assembled
**           -1 - if it could not be assembled
*/
int asmbler(char *str, int *ptrval)
{
    struct argv_array   argv;
    struct asm_tab      *aptr = NULL;
    int                 argc, argkey, arg_count;
    char                **av, *mnem;
    int                 len, escaped;


	a_inst.asm_code = 0;
	argc = tokenize(str,&argv);
	av = argv.argv_ptrs;
	if ( argc <= 0 )
	   return(-1);
	len = strlen(*av);
	len--;
	mnem = *av;
	if( mnem[len-1] == '.')
	  { if(mnem[len] == 's' )
		format = 0;
	    else if (mnem[len] == 'd')
		format = 1;
	    else if (mnem[len] == 'e')
		format = 2;
	    else if (mnem[len] == 'q')
		format = 3;
	    else if (mnem[len] == 'w')
		format = 4;
	    else if (mnem[len] == 'l')
		format = 5;
	    mnem[len] = 0; }
	if(!find_mnem(asm_tab_op,&aptr,*av++))
	   return(-1);
	if ( aptr->inst_type == F_TYPE)
	   a_inst.f_type.fmt = format;
	bad_arg = 0;
	arg_count = 0;
	argkey = aptr->arg_order;
	escaped = 0;
	while ( (argkey & 0xf) != 0 )
	   {
	     if ((argkey&0xf) == ESC_ARG)
	       escaped = ESC_ESC;
	     else if((argkey & 0xf) != 0xf)
		{ arg_encode(aptr, (argkey & 0xf)|escaped, *av++);
		  escaped = 0;
		  arg_count++; }
	     argkey = argkey >> 4;  }
	if ( (arg_count != --argc) || (bad_arg == 1) )
	   return(-2);
	*ptrval = a_inst.asm_code;
	return(1);
}

	
		

/*
**  bp_query(asmtab,inst,addr,bp_info,b_target)
**	this routine returns information about the instruction for
**	the breakpoint routines. it returns flags for branch, jump,
**	conditional and linking. It also retuns the branch target
**	address.
**
**	entry:
**		asmtab - pointer to disassembly table
**		inst - the instruction in question
**		addr - the address of the instruction
**		bp_info pointer to where to put the bp information
**		b_target - pointer to branch target address
**	return:
**		TRUE if found a valid instruction
**		FALSE if didnt find a valid one
**
*/
static struct asm_tab *bp_query(asmtab,inst,addr,bp_info,b_target)
struct asm_tab *asmtab;
union r3k_inst inst;
unsigned *addr;
int *bp_info;
int *b_target;
{
	int i,tmp_op;

    for( i = 0; asmtab[i].inst_type != E_TYPE; i++ )
	 { tmp_op = op & ((asmtab[i].opt_decode&OPT_OP_MASK)>>8);
	   if (tmp_op == asmtab[i].op_val )
		{if ( (asmtab[i].opt_decode&OPT_DEC_MASK) != 0 )
		  { do_opt_decode((asmtab[i].opt_decode&OPT_DEC_MASK),inst);
		    bp_query(asmtab[i].asm_tab_ptr,inst,addr,bp_info,b_target);
		    return; }
	         else
		  { do_decode(asmtab[i].inst_type,inst,addr);
		    *bp_info = asmtab[i].br_jmp_type;
		    if((*bp_info & JUMP) && (asmtab[i].inst_type == R_TYPE) )
            *b_target = (int) except_regs[rs];
		    else if ( (*bp_info == STORE) || ( *bp_info == LOAD ) )
            *b_target = (int) except_regs[rs] + s_imd;
		    else
		        *b_target = target;
		    break; }
		}
	  }
	return(&asmtab[i]);
}
/*
**  brk_query() - called by breakpoint routines to get info on instruction
**
**	entry:
**		addr - address of instruction in question
**		bp_info - pointer to location for results of query
**		b_target - pointer to loc for branch target address
**
*/
struct asm_tab *brk_query(addr,bp_info,b_target)
unsigned *addr;
int *bp_info;
int *b_target;
{
	union r3k_inst inst;
	struct asm_tab *asmtab;

	*bp_info = 0;
	*b_target = -1;

	inst.asm_code = *addr;
	op = inst.r_type.op;
	asmtab = bp_query(asm_tab_op,inst,addr,bp_info,b_target);
	return(asmtab);
}

#endif
