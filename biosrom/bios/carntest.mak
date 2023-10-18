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
GAME=CARN

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

CSRCS= 	ctmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
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
all:	$(BUILDDIR)\carntest.bin
	@echo CARNTEST for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\carntest.bin:	$(OBJS) carntest.lnk carntest.mak \video\lib\glide244.a
	@$(LINK) /p @carntest.lnk,$(BUILDDIR)/carntest.bin,carntest.sym,carntest.map > carntest.log
	@echo CARNTEST.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

carntest.lnk:	biosbin.in bflist.in carntest.mak
	@if EXIST carntest.lnk del carntest.lnk
	@copy biosbin.in carntest.lnk > NUL:
	@echo 	include	"crt0.o" >> carntest.lnk
	@echo 	include	"cache.o" >> carntest.lnk
	@echo 	include	"tlb.o" >> carntest.lnk
	@echo 	include	"jtovrlay.o" >> carntest.lnk
	@echo 	include	"psyqdbg.o" >> carntest.lnk
	@echo 	include	"memcpy.o" >> carntest.lnk
	@echo 	include	"cntreg.o" >> carntest.lnk
	@echo 	include	"except.o" >> carntest.lnk
	@echo 	include	"handlers.o" >> carntest.lnk
	@echo 	include	"ctmain.o" >> carntest.lnk
	@echo 	include	"__main.o" >> carntest.lnk
	@echo 	include	"conf.o" >> carntest.lnk
	@echo 	include	"io.o" >> carntest.lnk
	@echo 	include	"open.o" >> carntest.lnk
	@echo 	include	"close.o" >> carntest.lnk
	@echo 	include	"read.o" >> carntest.lnk
	@echo 	include	"write.o" >> carntest.lnk
	@echo 	include	"ioctl.o" >> carntest.lnk
	@echo 	include	"geterrno.o" >> carntest.lnk
	@echo 	include	"leddrv.o" >> carntest.lnk
	@echo 	include	"ioasic.o" >> carntest.lnk
	@echo 	include	"picdrv.o" >> carntest.lnk
	@echo 	include	"wdogdrv.o" >> carntest.lnk
	@echo 	include	"cmosdrv.o" >> carntest.lnk
	@echo 	include	"filedrv.o" >> carntest.lnk
	@echo 	include	"timerdrv.o" >> carntest.lnk
	@echo 	include	"mdiagdrv.o" >> carntest.lnk
	@echo 	include	"settime.o" >> carntest.lnk
	@echo 	include	"gettime.o" >> carntest.lnk
	@echo 	include	"setdate.o" >> carntest.lnk
	@echo 	include	"getdate.o" >> carntest.lnk
	@echo 	include	"strchr.o" >> carntest.lnk
	@echo 	include	"setdrive.o" >> carntest.lnk
	@echo 	include	"getdrive.o" >> carntest.lnk
	@echo 	include	"unlock.o" >> carntest.lnk
	@echo 	include	"lock.o" >> carntest.lnk
	@echo 	include	"memtest.o" >> carntest.lnk
	@echo 	include	"mtest.o" >> carntest.lnk
	@echo 	include	"memset.o" >> carntest.lnk
	@echo 	include	"_exit.o" >> carntest.lnk
	@echo 	include	"pciio.o" >> carntest.lnk
	@echo 	include	"dskcache.o" >> carntest.lnk
	@echo 	include	"filesys.o" >> carntest.lnk
	@echo 	include	"commit.o" >> carntest.lnk
	@echo 	include	"creat.o" >> carntest.lnk
	@echo 	include	"creatnew.o" >> carntest.lnk
	@echo 	include	"ffirst.o" >> carntest.lnk
	@echo 	include	"fnext.o" >> carntest.lnk
	@echo 	include	"getdfree.o" >> carntest.lnk
	@echo 	include	"getfattr.o" >> carntest.lnk
	@echo 	include	"getftime.o" >> carntest.lnk
	@echo 	include	"setfattr.o" >> carntest.lnk
	@echo 	include	"setftime.o" >> carntest.lnk
	@echo 	include	"psyq.o" >> carntest.lnk
	@echo 	include	"exec.o" >> carntest.lnk
	@echo 	include	"doprnt.o" >> carntest.lnk
	@echo 	include	"strcpy.o" >> carntest.lnk
	@echo 	include	"strlen.o" >> carntest.lnk
	@echo 	include	"memchr.o" >> carntest.lnk
	@echo 	include	"modfl.o" >> carntest.lnk
	@echo 	include	"lconv.o" >> carntest.lnk
	@echo 	include	"printf.o" >> carntest.lnk
	@echo 	include	"ct_flags.o" >> carntest.lnk
	@echo 	include	"disasm.o" >> carntest.lnk
	@echo 	include	"ginit.o" >> carntest.lnk
	@echo 	include	"font8x8.o" >> carntest.lnk
	@echo 	include	"div0hand.o" >> carntest.lnk
	@echo 	include	"memsize.o" >> carntest.lnk
ifeq	($(TESTROM),YES)
	@echo 	include	"stest/qatest01.o" >> carntest.lnk
	@echo 	include	"stest/cputest.o" >> carntest.lnk
	@echo 	include	"stest/siotest.o" >> carntest.lnk
	@echo 	include	"stest/fmemtest.o" >> carntest.lnk
	@echo 	include	"stest/tmemtest.o" >> carntest.lnk
endif
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> carntest.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> carntest.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> carntest.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> carntest.lnk
	@echo 	inclib	"\video\lib\glide244.a" >> carntest.lnk

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

ctmain.c:	cmain.c
	@if EXIST ctmain.c del ctmain.c
	@copy cmain.c ctmain.c > NUL:

tgt64010.c:	gt64010.c
	@if EXIST tgt64010.c del tgt64010.c
	@copy gt64010.c tgt64010.c > NUL:

#
# Files that must be compiled due to TEST compile time switch
#

.PHONY:	conf.o exec.o ginit.o memtest.o idedrv.o tgt64010.o ctmain.o

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

ctmain.o:
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
	@echo CARNTEST CLEAN
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
	@echo CARNTEST CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo CARNTEST CLOBBER
	@if EXIST $(BUILDDIR)\carntest.bin del $(BUILDDIR)\carntest.bin
	@echo CARNTEST CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo CARNTEST REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f carntest.mak
	@echo CARNTEST REBUILD for $(TARGET_SYS) DONE
	
