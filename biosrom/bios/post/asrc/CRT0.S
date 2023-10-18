/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    CRT0.S                                                            บ
บ Author:  Jack Miller                                                       บ
บ Created: 16-Sep-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   'C' Run-time startup code.                                               บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  16Sep96 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <machine/asmhdr.h>
#include <machine/iregdef.h>
#include <machine/idtcpu.h>
#include <machine/seattle.h>
#include "post.h"


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                       Structure & Literal Defintions                       ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                               External Data                                ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
    .data

.extern _fbss,4           /* this is defined by the linker */
.extern _end,4            /* this is defined by the linker */


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                    Code                                    ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
    .text

FRAME(start,sp,0,ra) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/

    .set    noreorder

  ;# Setup the WATCHHI AND WATCHLO registers
    nor     a0, zero, zero      ;# -1
    mtc0    a0, C0_WATCHLO
    mtc0    a0, C0_WATCHHI
    nop

  ;# Initialize bss
    la      v0, _fbss           ;# clear bss
    la      v1, _end            ;# end of bss

    .set    noreorder

LABEL(1)
    sd      zero, 0(v0)         ;# clear bss
    bltu    v0, v1, 1b
    add     v0, 8

#if BIOS == JACK_BIOS
    subu    v0, 8

  ;# Initialize stack
    li      v1, 0xFFFFFFF8
    and     v1, v0, v1
    add     sp, v1, BIOS_STACK_SZ-8 ;# program stack top

    li      v1, 0x4B415453      ;# "STAK"
LABEL(1)
    sw      v1, 0(v0)
    bltu    v0, sp, 1b
    add     v0, 4
#endif

    subu    sp, 8
    sw      ra, 8(sp)           ;# save the return address

    li      gp, 0xA8000000

#if BIOS == MIKE_BIOS
    mfc0    v0, C0_SR
    nop
    nop
    li      v1, SR_FR
    or      v0, v0, v1          ;# enable 32 64-bit Floating-point registers
    mtc0    v0, C0_SR
#endif

    move    a3, zero            ;# zero the arg registers
    move    a2, zero
    move    a1, zero
    jal     main                ;# Call the main entry point
    move    a0, zero

#if BIOS == MIKE_BIOS
    mfc0    v0, C0_SR
    nop
    nop
    li      v1, ~SR_FR
    and     v0, v0, v1          ;# disable 32 64-bit Floating-point registers
    mtc0    v0, C0_SR
#endif

    lw      ra, 8(sp)           ;# restore the return address
    add     sp, sp, 8
    jr      ra
    nop

ENDFRAME(start)
