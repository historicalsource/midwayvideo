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
GAME=B99

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

CSRCS= 	b99tmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
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
all:	$(BUILDDIR)\b99test.bin
	@echo B99TEST for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\b99test.bin:	$(OBJS) b99test.lnk b99test.mak \video\lib\glide244.a
	@$(LINK) /p @b99test.lnk,$(BUILDDIR)/b99test.bin,b99test.sym,b99test.map > b99test.log
	@echo B99TEST.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

b99test.lnk:	biosbin.in bflist.in b99test.mak
	@if EXIST b99test.lnk del b99test.lnk
	@copy biosbin.in b99test.lnk > NUL:
	@echo 	include	"crt0.o" >> b99test.lnk
	@echo 	include	"cache.o" >> b99test.lnk
	@echo 	include	"tlb.o" >> b99test.lnk
	@echo 	include	"jtovrlay.o" >> b99test.lnk
	@echo 	include	"psyqdbg.o" >> b99test.lnk
	@echo 	include	"memcpy.o" >> b99test.lnk
	@echo 	include	"cntreg.o" >> b99test.lnk
	@echo 	include	"except.o" >> b99test.lnk
	@echo 	include	"handlers.o" >> b99test.lnk
	@echo 	include	"b99tmain.o" >> b99test.lnk
	@echo 	include	"__main.o" >> b99test.lnk
	@echo 	include	"conf.o" >> b99test.lnk
	@echo 	include	"io.o" >> b99test.lnk
	@echo 	include	"open.o" >> b99test.lnk
	@echo 	include	"close.o" >> b99test.lnk
	@echo 	include	"read.o" >> b99test.lnk
	@echo 	include	"write.o" >> b99test.lnk
	@echo 	include	"ioctl.o" >> b99test.lnk
	@echo 	include	"geterrno.o" >> b99test.lnk
	@echo 	include	"leddrv.o" >> b99test.lnk
	@echo 	include	"ioasic.o" >> b99test.lnk
	@echo 	include	"picdrv.o" >> b99test.lnk
	@echo 	include	"wdogdrv.o" >> b99test.lnk
	@echo 	include	"cmosdrv.o" >> b99test.lnk
	@echo 	include	"filedrv.o" >> b99test.lnk
	@echo 	include	"timerdrv.o" >> b99test.lnk
	@echo 	include	"mdiagdrv.o" >> b99test.lnk
	@echo 	include	"settime.o" >> b99test.lnk
	@echo 	include	"gettime.o" >> b99test.lnk
	@echo 	include	"setdate.o" >> b99test.lnk
	@echo 	include	"getdate.o" >> b99test.lnk
	@echo 	include	"strchr.o" >> b99test.lnk
	@echo 	include	"setdrive.o" >> b99test.lnk
	@echo 	include	"getdrive.o" >> b99test.lnk
	@echo 	include	"unlock.o" >> b99test.lnk
	@echo 	include	"lock.o" >> b99test.lnk
	@echo 	include	"memtest.o" >> b99test.lnk
	@echo 	include	"mtest.o" >> b99test.lnk
	@echo 	include	"memset.o" >> b99test.lnk
	@echo 	include	"_exit.o" >> b99test.lnk
	@echo 	include	"pciio.o" >> b99test.lnk
	@echo 	include	"dskcache.o" >> b99test.lnk
	@echo 	include	"filesys.o" >> b99test.lnk
	@echo 	include	"commit.o" >> b99test.lnk
	@echo 	include	"creat.o" >> b99test.lnk
	@echo 	include	"creatnew.o" >> b99test.lnk
	@echo 	include	"ffirst.o" >> b99test.lnk
	@echo 	include	"fnext.o" >> b99test.lnk
	@echo 	include	"getdfree.o" >> b99test.lnk
	@echo 	include	"getfattr.o" >> b99test.lnk
	@echo 	include	"getftime.o" >> b99test.lnk
	@echo 	include	"setfattr.o" >> b99test.lnk
	@echo 	include	"setftime.o" >> b99test.lnk
	@echo 	include	"psyq.o" >> b99test.lnk
	@echo 	include	"exec.o" >> b99test.lnk
	@echo 	include	"doprnt.o" >> b99test.lnk
	@echo 	include	"strcpy.o" >> b99test.lnk
	@echo 	include	"strlen.o" >> b99test.lnk
	@echo 	include	"memchr.o" >> b99test.lnk
	@echo 	include	"modfl.o" >> b99test.lnk
	@echo 	include	"lconv.o" >> b99test.lnk
	@echo 	include	"printf.o" >> b99test.lnk
	@echo 	include	"ct_flags.o" >> b99test.lnk
	@echo 	include	"disasm.o" >> b99test.lnk
	@echo 	include	"ginit.o" >> b99test.lnk
	@echo 	include	"font8x8.o" >> b99test.lnk
	@echo 	include	"div0hand.o" >> b99test.lnk
	@echo 	include	"memsize.o" >> b99test.lnk
ifeq	($(TESTROM),YES)
	@echo 	include	"stest/qatest01.o" >> b99test.lnk
	@echo 	include	"stest/cputest.o" >> b99test.lnk
	@echo 	include	"stest/siotest.o" >> b99test.lnk
	@echo 	include	"stest/fmemtest.o" >> b99test.lnk
	@echo 	include	"stest/tmemtest.o" >> b99test.lnk
endif
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> b99test.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> b99test.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> b99test.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> b99test.lnk
	@echo 	inclib	"\video\lib\glide244.a" >> b99test.lnk

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

b99tmain.c:	b99main.c
	@if EXIST b99tmain.c del b99tmain.c
	@copy b99main.c b99tmain.c > NUL:

tgt64010.c:	gt64010.c
	@if EXIST tgt64010.c del tgt64010.c
	@copy gt64010.c tgt64010.c > NUL:

#
# Files that must be compiled due to TEST compile time switch
#

.PHONY:	conf.o exec.o ginit.o memtest.o idedrv.o tgt64010.o b99tmain.o

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

b99tmain.o:
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
	@echo B99TEST CLEAN
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
	@echo B99TEST CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo B99TEST CLOBBER
	@if EXIST $(BUILDDIR)\b99test.bin del $(BUILDDIR)\b99test.bin
	@echo B99TEST CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo B99TEST REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f b99test.mak
	@echo B99TEST REBUILD for $(TARGET_SYS) DONE
	
