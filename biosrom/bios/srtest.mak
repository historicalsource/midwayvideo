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
GAME=SPACE

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

CSRCS= 	srtmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
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
all:	$(BUILDDIR)\srtest.bin
	@echo SRTEST for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\srtest.bin:	$(OBJS) srtest.lnk srtest.mak \video\lib\glide244.a
	@$(LINK) /p @srtest.lnk,$(BUILDDIR)/srtest.bin,srtest.sym,srtest.map > srtest.log
	@echo SRTEST.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

srtest.lnk:	biosbin.in bflist.in srtest.mak
	@if EXIST srtest.lnk del srtest.lnk
	@copy biosbin.in srtest.lnk > NUL:
	@echo 	include	"crt0.o" >> srtest.lnk
	@echo 	include	"cache.o" >> srtest.lnk
	@echo 	include	"tlb.o" >> srtest.lnk
	@echo 	include	"jtovrlay.o" >> srtest.lnk
	@echo 	include	"psyqdbg.o" >> srtest.lnk
	@echo 	include	"memcpy.o" >> srtest.lnk
	@echo 	include	"cntreg.o" >> srtest.lnk
	@echo 	include	"except.o" >> srtest.lnk
	@echo 	include	"handlers.o" >> srtest.lnk
	@echo 	include	"srtmain.o" >> srtest.lnk
	@echo 	include	"__main.o" >> srtest.lnk
	@echo 	include	"conf.o" >> srtest.lnk
	@echo 	include	"io.o" >> srtest.lnk
	@echo 	include	"open.o" >> srtest.lnk
	@echo 	include	"close.o" >> srtest.lnk
	@echo 	include	"read.o" >> srtest.lnk
	@echo 	include	"write.o" >> srtest.lnk
	@echo 	include	"ioctl.o" >> srtest.lnk
	@echo 	include	"geterrno.o" >> srtest.lnk
	@echo 	include	"leddrv.o" >> srtest.lnk
	@echo 	include	"ioasic.o" >> srtest.lnk
	@echo 	include	"picdrv.o" >> srtest.lnk
	@echo 	include	"wdogdrv.o" >> srtest.lnk
	@echo 	include	"cmosdrv.o" >> srtest.lnk
	@echo 	include	"filedrv.o" >> srtest.lnk
	@echo 	include	"timerdrv.o" >> srtest.lnk
	@echo 	include	"mdiagdrv.o" >> srtest.lnk
	@echo 	include	"settime.o" >> srtest.lnk
	@echo 	include	"gettime.o" >> srtest.lnk
	@echo 	include	"setdate.o" >> srtest.lnk
	@echo 	include	"getdate.o" >> srtest.lnk
	@echo 	include	"strchr.o" >> srtest.lnk
	@echo 	include	"setdrive.o" >> srtest.lnk
	@echo 	include	"getdrive.o" >> srtest.lnk
	@echo 	include	"unlock.o" >> srtest.lnk
	@echo 	include	"lock.o" >> srtest.lnk
	@echo 	include	"memtest.o" >> srtest.lnk
	@echo 	include	"mtest.o" >> srtest.lnk
	@echo 	include	"memset.o" >> srtest.lnk
	@echo 	include	"_exit.o" >> srtest.lnk
	@echo 	include	"pciio.o" >> srtest.lnk
	@echo 	include	"dskcache.o" >> srtest.lnk
	@echo 	include	"filesys.o" >> srtest.lnk
	@echo 	include	"commit.o" >> srtest.lnk
	@echo 	include	"creat.o" >> srtest.lnk
	@echo 	include	"creatnew.o" >> srtest.lnk
	@echo 	include	"ffirst.o" >> srtest.lnk
	@echo 	include	"fnext.o" >> srtest.lnk
	@echo 	include	"getdfree.o" >> srtest.lnk
	@echo 	include	"getfattr.o" >> srtest.lnk
	@echo 	include	"getftime.o" >> srtest.lnk
	@echo 	include	"setfattr.o" >> srtest.lnk
	@echo 	include	"setftime.o" >> srtest.lnk
	@echo 	include	"psyq.o" >> srtest.lnk
	@echo 	include	"exec.o" >> srtest.lnk
	@echo 	include	"doprnt.o" >> srtest.lnk
	@echo 	include	"strcpy.o" >> srtest.lnk
	@echo 	include	"strlen.o" >> srtest.lnk
	@echo 	include	"memchr.o" >> srtest.lnk
	@echo 	include	"modfl.o" >> srtest.lnk
	@echo 	include	"lconv.o" >> srtest.lnk
	@echo 	include	"printf.o" >> srtest.lnk
	@echo 	include	"ct_flags.o" >> srtest.lnk
	@echo 	include	"disasm.o" >> srtest.lnk
	@echo 	include	"ginit.o" >> srtest.lnk
	@echo 	include	"font8x8.o" >> srtest.lnk
	@echo 	include	"div0hand.o" >> srtest.lnk
	@echo 	include	"memsize.o" >> srtest.lnk
ifeq	($(TESTROM),YES)
	@echo 	include	"stest/qatest01.o" >> srtest.lnk
	@echo 	include	"stest/cputest.o" >> srtest.lnk
	@echo 	include	"stest/siotest.o" >> srtest.lnk
	@echo 	include	"stest/fmemtest.o" >> srtest.lnk
	@echo 	include	"stest/tmemtest.o" >> srtest.lnk
endif
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> srtest.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> srtest.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> srtest.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> srtest.lnk
	@echo 	inclib	"\video\lib\glide244.a" >> srtest.lnk

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

srtmain.c:	srmain.c
	@if EXIST srtmain.c del srtmain.c
	@copy srmain.c srtmain.c > NUL:

tgt64010.c:	gt64010.c
	@if EXIST tgt64010.c del tgt64010.c
	@copy gt64010.c tgt64010.c > NUL:

#
# Files that must be compiled due to TEST compile time switch
#

.PHONY:	conf.o exec.o ginit.o memtest.o idedrv.o tgt64010.o srtmain.o

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

srtmain.o:
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
	@echo SRTEST CLEAN
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
	@echo SRTEST CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo SRTEST CLOBBER
	@if EXIST $(BUILDDIR)\srtest.bin del $(BUILDDIR)\srtest.bin
	@echo SRTEST CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo SRTEST REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f srtest.mak
	@echo SRTEST REBUILD for $(TARGET_SYS) DONE
	
