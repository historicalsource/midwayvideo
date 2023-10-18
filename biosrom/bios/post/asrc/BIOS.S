#include <machine/asmhdr.h>
#include <machine/idtcpu.h>
#include <machine/iregdef.h>


.text


FRAME(installhandler,sp,0,ra)

    .set    noreorder

    .word   (0x40D<<6)|0x0D     ;# break   0x40D

    jr      ra
    nop

    .set    reorder

ENDFRAME(installhandler)


FRAME(enableint,sp,0,ra)

    .set    noreorder

    mfc0    v0, C0_SR
    nop
    nop
    or      v0, a0, zero        ;# enable interrupt
    mtc0    v0, C0_SR

    jr      ra
    nop

    .set    reorder

ENDFRAME(enableint)


FRAME(disableint,sp,0,ra)

    .set    noreorder

    mfc0    v0, C0_SR
    nop
    nop
    nor     v1, a0, zero
    and     v0, v0, v1          ;# disable interrupt
    mtc0    v0, C0_SR

    jr      ra
    nop

    .set    reorder

ENDFRAME(disableint)
