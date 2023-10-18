#
# Copyright (c) 1996 by Williams Electronics Games Inc.
#
# Makefile for Operating System Kernel for Development PIC on VEGAS
#
# board specific defines
#

#
# Target system types
#
SA1=		1
SEATTLE=	2
FLAGSTAFF=	4
VEGAS=		8

#
# Game
#
GAME=NFL

#
# Target systems
#
TARGET_SYS=SEATTLE

#
# Buildmode
#
BUILDMODE=DEBUG

#
# Watchdog enable
#
DBG=

#
# Test ROM
#
TESTROM=YES
ifeq	($(TESTROM),YES)
TEST=-DTEST
else
TEST=
endif


SYSTEM=$($(TARGET_SYS))

SND_SYSTEM=	DCS2

#
# Directories of where stuff is
#
BUILDDIR=	.

PROCESSOR=	R5000


#
# Assemble command line
#
AS2 =           asmmips /q /of.rdata /zd /l /oc+ /e $(PROCESSOR)=1 /e PHOENIX_SYS=$(SYSTEM) /e LIBC=0

#
# Linker Command line
#
LINK =	  	psylink /s /c /m /wl /wm

#
# Compiler command lines
#
CC3 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @tbios.cf -O3
CC2 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @tbios.cf -O2
CC1 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @tbios.cf -O1
CC0 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @tbios.cf -O0


#
# Drivers
#
TTY_DRIVER=	asictty.c

SYS_CONTROL=	tgt64010.c

SND_DRIVER=	dcs2drv.c

IDE_DRIVER=	idedrv.c

#
# C Source Modules
#
#CSRCS= 	
#	mdiagdrv.c

CSRCS= 	nfltmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
	$(TTY_DRIVER) picdrv.c wdogdrv.c $(SND_DRIVER) read.c write.c \
	geterrno.c cmosdrv.c settime.c gettime.c setdate.c getdate.c \
	strchr.c setdrive.c getdrive.c unlock.c lock.c memtest.c \
	$(SYS_CONTROL) pciio.c $(IDE_DRIVER) dskcache.c filesys.c commit.c \
	creat.c creatnew.c ffirst.c fnext.c getdfree.c getfattr.c getftime.c \
	setfattr.c setftime.c filedrv.c timerdrv.c handlers.c psyq.c exec.c \
	mdiagdrv.c doprnt.c strcpy.c strlen.c memchr.c modfl.c lconv.c printf.c \
	ct_flags.c disasm.c ginit.c font8x8.c memsize.c \
	stest/cputest.c stest/siotest.c stest/qatest01.c stest/fmemtest.c\
	stest/tmemtest.c div0hand.c
		

#
# Asm Source Modules
#
#ASRCS= 	
#	psyqdbg.s

ASRCS= 	crt0.s cache.s jtovrlay.s tlb.s memcpy.s cntreg.s mtest.s memset.s \
	_exit.s except.s psyqdbg.s

#
# Object modules
#
OBJS = 	$(ASRCS:.s=.o) $(CSRCS:.c=.o)


#
# What we are going to build
#
all:	$(BUILDDIR)\nfltest.bin
	@echo NFLTEST for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\nfltest.bin:	$(OBJS) nfltest.lnk nfltest.mak \video\lib\glide244.a
	@$(LINK) /p @nfltest.lnk,$(BUILDDIR)/nfltest.bin,nfltest.sym,nfltest.map > nfltest.log
	@echo NFLTEST.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

nfltest.lnk:	biosbin.in bflist.in nfltest.mak
	@if EXIST nfltest.lnk del nfltest.lnk
	@copy biosbin.in nfltest.lnk > NUL:
	@echo 	include	"crt0.o" >> nfltest.lnk
	@echo 	include	"cache.o" >> nfltest.lnk
	@echo 	include	"tlb.o" >> nfltest.lnk
	@echo 	include	"jtovrlay.o" >> nfltest.lnk
	@echo 	include	"psyqdbg.o" >> nfltest.lnk
	@echo 	include	"memcpy.o" >> nfltest.lnk
	@echo 	include	"cntreg.o" >> nfltest.lnk
	@echo 	include	"except.o" >> nfltest.lnk
	@echo 	include	"handlers.o" >> nfltest.lnk
	@echo 	include	"nfltmain.o" >> nfltest.lnk
	@echo 	include	"__main.o" >> nfltest.lnk
	@echo 	include	"conf.o" >> nfltest.lnk
	@echo 	include	"io.o" >> nfltest.lnk
	@echo 	include	"open.o" >> nfltest.lnk
	@echo 	include	"close.o" >> nfltest.lnk
	@echo 	include	"read.o" >> nfltest.lnk
	@echo 	include	"write.o" >> nfltest.lnk
	@echo 	include	"ioctl.o" >> nfltest.lnk
	@echo 	include	"geterrno.o" >> nfltest.lnk
	@echo 	include	"leddrv.o" >> nfltest.lnk
	@echo 	include	"ioasic.o" >> nfltest.lnk
	@echo 	include	"picdrv.o" >> nfltest.lnk
	@echo 	include	"wdogdrv.o" >> nfltest.lnk
	@echo 	include	"cmosdrv.o" >> nfltest.lnk
	@echo 	include	"filedrv.o" >> nfltest.lnk
	@echo 	include	"timerdrv.o" >> nfltest.lnk
	@echo 	include	"mdiagdrv.o" >> nfltest.lnk
	@echo 	include	"settime.o" >> nfltest.lnk
	@echo 	include	"gettime.o" >> nfltest.lnk
	@echo 	include	"setdate.o" >> nfltest.lnk
	@echo 	include	"getdate.o" >> nfltest.lnk
	@echo 	include	"strchr.o" >> nfltest.lnk
	@echo 	include	"setdrive.o" >> nfltest.lnk
	@echo 	include	"getdrive.o" >> nfltest.lnk
	@echo 	include	"unlock.o" >> nfltest.lnk
	@echo 	include	"lock.o" >> nfltest.lnk
	@echo 	include	"memtest.o" >> nfltest.lnk
	@echo 	include	"mtest.o" >> nfltest.lnk
	@echo 	include	"memset.o" >> nfltest.lnk
	@echo 	include	"_exit.o" >> nfltest.lnk
	@echo 	include	"pciio.o" >> nfltest.lnk
	@echo 	include	"dskcache.o" >> nfltest.lnk
	@echo 	include	"filesys.o" >> nfltest.lnk
	@echo 	include	"commit.o" >> nfltest.lnk
	@echo 	include	"creat.o" >> nfltest.lnk
	@echo 	include	"creatnew.o" >> nfltest.lnk
	@echo 	include	"ffirst.o" >> nfltest.lnk
	@echo 	include	"fnext.o" >> nfltest.lnk
	@echo 	include	"getdfree.o" >> nfltest.lnk
	@echo 	include	"getfattr.o" >> nfltest.lnk
	@echo 	include	"getftime.o" >> nfltest.lnk
	@echo 	include	"setfattr.o" >> nfltest.lnk
	@echo 	include	"setftime.o" >> nfltest.lnk
	@echo 	include	"psyq.o" >> nfltest.lnk
	@echo 	include	"exec.o" >> nfltest.lnk
	@echo 	include	"doprnt.o" >> nfltest.lnk
	@echo 	include	"strcpy.o" >> nfltest.lnk
	@echo 	include	"strlen.o" >> nfltest.lnk
	@echo 	include	"memchr.o" >> nfltest.lnk
	@echo 	include	"modfl.o" >> nfltest.lnk
	@echo 	include	"lconv.o" >> nfltest.lnk
	@echo 	include	"printf.o" >> nfltest.lnk
	@echo 	include	"ct_flags.o" >> nfltest.lnk
	@echo 	include	"disasm.o" >> nfltest.lnk
	@echo 	include	"ginit.o" >> nfltest.lnk
	@echo 	include	"font8x8.o" >> nfltest.lnk
	@echo 	include	"div0hand.o" >> nfltest.lnk
	@echo 	include	"memsize.o" >> nfltest.lnk
ifeq	($(TESTROM),YES)
	@echo 	include	"stest/qatest01.o" >> nfltest.lnk
	@echo 	include	"stest/cputest.o" >> nfltest.lnk
	@echo 	include	"stest/siotest.o" >> nfltest.lnk
	@echo 	include	"stest/fmemtest.o" >> nfltest.lnk
	@echo 	include	"stest/tmemtest.o" >> nfltest.lnk
endif
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> nfltest.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> nfltest.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> nfltest.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> nfltest.lnk
	@echo 	inclib	"\video\lib\glide244.a" >> nfltest.lnk

#
# Dependancies and rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $<

%.o:	%.s
	@echo $< to $@ for $(GAME) PIC on $(TARGET_SYS)
	@$(AS2) $<,$@

%.S:	%.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -S -o $@ $<

\video\lib\glide244.a:
	@$(MAKE) --no-print-directory -C /video/lib/glide244

#
# File that must be compiled at lower optimization levels
#
ioasic.o:	ioasic.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC1) -o $@ $<

psyq.o:	psyq.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $<

crt0.o:	crt0.s jtable.inc
	@echo $< to $@ for $(GAME) PIC on $(TARGET_SYS)
	@$(AS2) $<,$@

fpgaload.o:	fpgaload.c vsio_v30.ttf
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $<

nfltmain.c:	nflmain.c
	@if EXIST nfltmain.c del nfltmain.c
	@copy nflmain.c nfltmain.c > NUL:

tgt64010.c:	gt64010.c
	@if EXIST tgt64010.c del tgt64010.c
	@copy gt64010.c tgt64010.c > NUL:

#
# Files that must be compiled due to TEST compile time switch
#

.PHONY:	conf.o exec.o ginit.o memtest.o idedrv.o tgt64010.o nfltmain.o

conf.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

exec.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

ginit.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

memtest.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

idedrv.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

tgt64010.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

nfltmain.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif
ifeq	($(TESTROM),YES)
ifneq	($(wildcard stest/*.d),)
include	$(wildcard stest/*.d)
endif
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo NFLTEST CLEAN
ifneq	($(wildcard *.o),)
	@del *.o
endif
ifneq	($(wildcard stest/*.o),)
	@del stest\*.o
endif
ifneq	($(wildcard *.d),)
	@del *.d
endif
ifneq	($(wildcard stest/*.d),)
	@del stest\*.d
endif
ifneq	($(wildcard *.lnk),)
	@del *.lnk
endif
ifneq	($(wildcard *.sym),)
	@del *.sym
endif
ifneq	($(wildcard *.log),)
	@del *.log
endif
	@echo NFLTEST CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo NFLTEST CLOBBER
	@if EXIST $(BUILDDIR)\nfltest.bin del $(BUILDDIR)\nfltest.bin
	@echo NFLTEST CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo NFLTEST REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f nfltest.mak
	@echo NFLTEST REBUILD for $(TARGET_SYS) DONE
	
