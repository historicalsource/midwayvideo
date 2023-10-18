/*
**  Various routines which needed to be defined
**  to make a stand-alone application.
*/
#include <stdio.h>
#include "post.h"


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                               External Data                                ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
extern volatile _ASIC_REGS * const _ioasic;


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                    Code                                    ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
isprint(char c)
{
    return(c>=32 && c<127);
}


size_t strlen(const char *s)
{
    int     n = 0;

    while (*s++) n++;

	return(n);
}

void *memset(void *s, int c, int n)
{
	void	*d = s;
	
    while (n--) *((char *)s)++ = (char)c;
}


void putmsg(char *msg)
{
    void (*write)(int fd, char *buf, int cnt);


#if BIOS == MIKE_BIOS
    write = (void *)0x800000C0;
#elif BIOS == JACK_BIOS
    write = (void *)0x800001C8;
#else
#error Must define BIOS to either MIKE_BIOS or JACK_BIOS
#endif

    write(0, msg, strlen(msg));
}


mxBool getinput(mxU32 bmask, mxU32 smask) /*
ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
    Read the joystick and button inputs allowing only those whose mask is
    speicified by 'bmask' or 'smask'.
ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ*/
{
    register mxU32  jb, mb, timer;


    jb = mb = 0;

    jb = ((~_ioasic->asicP43 << 16) | ~_ioasic->asicP21) & bmask;
    mb = ~_ioasic->asicMisc & smask;

    if (jb || mb) {

        timer = getSysTicks() + 1;

        while (jb || mb) {
            jb = ((~_ioasic->asicP43 << 16) | ~_ioasic->asicP21) & bmask;
            mb = ~_ioasic->asicMisc & smask;

            if (getSysTicks() > timer) break;
        }

        return (TRUE);
    } else {
        return (FALSE);
    }
}
