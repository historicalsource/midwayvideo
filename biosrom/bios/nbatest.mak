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
GAME=NBA

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

CSRCS= 	nbatmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
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
all:	$(BUILDDIR)\nbatest.bin
	@echo NBATEST for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\nbatest.bin:	$(OBJS) nbatest.lnk nbatest.mak \video\lib\glide244.a
	@$(LINK) /p @nbatest.lnk,$(BUILDDIR)/nbatest.bin,nbatest.sym,nbatest.map > nbatest.log
	@echo NBATEST.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

nbatest.lnk:	biosbin.in bflist.in nbatest.mak
	@if EXIST nbatest.lnk del nbatest.lnk
	@copy biosbin.in nbatest.lnk > NUL:
	@echo 	include	"crt0.o" >> nbatest.lnk
	@echo 	include	"cache.o" >> nbatest.lnk
	@echo 	include	"tlb.o" >> nbatest.lnk
	@echo 	include	"jtovrlay.o" >> nbatest.lnk
	@echo 	include	"psyqdbg.o" >> nbatest.lnk
	@echo 	include	"memcpy.o" >> nbatest.lnk
	@echo 	include	"cntreg.o" >> nbatest.lnk
	@echo 	include	"except.o" >> nbatest.lnk
	@echo 	include	"handlers.o" >> nbatest.lnk
	@echo 	include	"nbatmain.o" >> nbatest.lnk
	@echo 	include	"__main.o" >> nbatest.lnk
	@echo 	include	"conf.o" >> nbatest.lnk
	@echo 	include	"io.o" >> nbatest.lnk
	@echo 	include	"open.o" >> nbatest.lnk
	@echo 	include	"close.o" >> nbatest.lnk
	@echo 	include	"read.o" >> nbatest.lnk
	@echo 	include	"write.o" >> nbatest.lnk
	@echo 	include	"ioctl.o" >> nbatest.lnk
	@echo 	include	"geterrno.o" >> nbatest.lnk
	@echo 	include	"leddrv.o" >> nbatest.lnk
	@echo 	include	"ioasic.o" >> nbatest.lnk
	@echo 	include	"picdrv.o" >> nbatest.lnk
	@echo 	include	"wdogdrv.o" >> nbatest.lnk
	@echo 	include	"cmosdrv.o" >> nbatest.lnk
	@echo 	include	"filedrv.o" >> nbatest.lnk
	@echo 	include	"timerdrv.o" >> nbatest.lnk
	@echo 	include	"mdiagdrv.o" >> nbatest.lnk
	@echo 	include	"settime.o" >> nbatest.lnk
	@echo 	include	"gettime.o" >> nbatest.lnk
	@echo 	include	"setdate.o" >> nbatest.lnk
	@echo 	include	"getdate.o" >> nbatest.lnk
	@echo 	include	"strchr.o" >> nbatest.lnk
	@echo 	include	"setdrive.o" >> nbatest.lnk
	@echo 	include	"getdrive.o" >> nbatest.lnk
	@echo 	include	"unlock.o" >> nbatest.lnk
	@echo 	include	"lock.o" >> nbatest.lnk
	@echo 	include	"memtest.o" >> nbatest.lnk
	@echo 	include	"mtest.o" >> nbatest.lnk
	@echo 	include	"memset.o" >> nbatest.lnk
	@echo 	include	"_exit.o" >> nbatest.lnk
	@echo 	include	"pciio.o" >> nbatest.lnk
	@echo 	include	"dskcache.o" >> nbatest.lnk
	@echo 	include	"filesys.o" >> nbatest.lnk
	@echo 	include	"commit.o" >> nbatest.lnk
	@echo 	include	"creat.o" >> nbatest.lnk
	@echo 	include	"creatnew.o" >> nbatest.lnk
	@echo 	include	"ffirst.o" >> nbatest.lnk
	@echo 	include	"fnext.o" >> nbatest.lnk
	@echo 	include	"getdfree.o" >> nbatest.lnk
	@echo 	include	"getfattr.o" >> nbatest.lnk
	@echo 	include	"getftime.o" >> nbatest.lnk
	@echo 	include	"setfattr.o" >> nbatest.lnk
	@echo 	include	"setftime.o" >> nbatest.lnk
	@echo 	include	"psyq.o" >> nbatest.lnk
	@echo 	include	"exec.o" >> nbatest.lnk
	@echo 	include	"doprnt.o" >> nbatest.lnk
	@echo 	include	"strcpy.o" >> nbatest.lnk
	@echo 	include	"strlen.o" >> nbatest.lnk
	@echo 	include	"memchr.o" >> nbatest.lnk
	@echo 	include	"modfl.o" >> nbatest.lnk
	@echo 	include	"lconv.o" >> nbatest.lnk
	@echo 	include	"printf.o" >> nbatest.lnk
	@echo 	include	"ct_flags.o" >> nbatest.lnk
	@echo 	include	"disasm.o" >> nbatest.lnk
	@echo 	include	"ginit.o" >> nbatest.lnk
	@echo 	include	"font8x8.o" >> nbatest.lnk
	@echo 	include	"div0hand.o" >> nbatest.lnk
	@echo 	include	"memsize.o" >> nbatest.lnk
ifeq	($(TESTROM),YES)
	@echo 	include	"stest/qatest01.o" >> nbatest.lnk
	@echo 	include	"stest/cputest.o" >> nbatest.lnk
	@echo 	include	"stest/siotest.o" >> nbatest.lnk
	@echo 	include	"stest/fmemtest.o" >> nbatest.lnk
	@echo 	include	"stest/tmemtest.o" >> nbatest.lnk
endif
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> nbatest.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> nbatest.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> nbatest.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> nbatest.lnk
	@echo 	inclib	"\video\lib\glide244.a" >> nbatest.lnk

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

nbatmain.c:	nbamain.c
	@if EXIST nbatmain.c del nbatmain.c
	@copy nbamain.c nbatmain.c > NUL:

tgt64010.c:	gt64010.c
	@if EXIST tgt64010.c del tgt64010.c
	@copy gt64010.c tgt64010.c > NUL:

#
# Files that must be compiled due to TEST compile time switch
#

.PHONY:	conf.o exec.o ginit.o memtest.o idedrv.o tgt64010.o nbatmain.o

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

nbatmain.o:
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
	@echo NBATEST CLEAN
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
	@echo NBATEST CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo NBATEST CLOBBER
	@if EXIST $(BUILDDIR)\nbatest.bin del $(BUILDDIR)\nbatest.bin
	@echo NBATEST CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo NBATEST REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f nbatest.mak
	@echo NBATEST REBUILD for $(TARGET_SYS) DONE
	
