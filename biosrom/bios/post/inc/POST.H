#ifndef _POST_H_
#define _POST_H_

#define MIKE_BIOS   1
#define JACK_BIOS   2

#define BIOS        MIKE_BIOS

#if BIOS == MIKE_BIOS

#include "ioasic.h"
#define BIOS_STACK_SZ   0x4000

#elif BIOS == JACK_BIOS

#include <machine/ioasic.h>
#define BIOS_STACK_SZ   0x2000

#else
#error Must define BIOS in POST.H to be either MIKE_BIOS or JACK_BIOS
#endif

#define DISK_INT    0x1000
#define VBLANK_INT  0x4000

#endif
