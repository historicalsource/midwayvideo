/*
**	debugger macros for assembly language routines. Allows the 
**	programmer to easily set up the necessary stack frame info
**	required by debuggers to do stack traces.
*/
#define FRAME(name,frm_reg,offset,ret_reg)  \
    .align  3;                              \
    .globl  name;                           \
    .ent    name;                           \
name:;                                      \
    .frame  frm_reg,offset,ret_reg


#define ENDFRAME(name)                      \
	.end name


#define LEAF(label) FRAME(label,sp,0,ra)
#define XLEAF(label)\
    .align  3;      \
    .globl  label;  \
label:

#define END(name)   ENDFRAME(name)


/*
** Macros for assembly language routines. Used for
** creating branch labels and subroutine callse
** for the MIPS R5000 CPU/ GALILEO GT-64010 combination.
*/

#define LABEL(name) \
    .align  3;      \
name:;

#define jsr(name)   \
    .align  3;      \
    jal     name;   \
    nop;
