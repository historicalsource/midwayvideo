#
# Copyright (c) 1996 by Williams Electronics Games Inc.
#
# Makefile for Operating System Kernel for SA1 Bios
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
GAME=DEVELOPMENT

#
# Target systems
#
TARGET_SYS=SA1

#
# Buildmode
#
BUILDMODE=DEBUG

#
# Watchdog enable
#
DOG=


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
CC3 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(PROCESSOR) -D$(GAME) $(DOG) @bios.cf -O3
CC2 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(PROCESSOR) -D$(GAME) $(DOG) @bios.cf -O2
CC1 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(PROCESSOR) -D$(GAME) $(DOG) @bios.cf -O1
CC0 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(PROCESSOR) -D$(GAME) $(DOG) @bios.cf -O0


#
# Drivers
#
ifeq	($(TARGET_SYS),SA1)
TTY_DRIVER=	ns450con.c
else
TTY_DRIVER=	asictty.c
endif

ifeq	($(TARGET_SYS),VEGAS)
SYS_CONTROL=	nile.c
else
SYS_CONTROL=	bgt64010.c
endif

ifeq	($(SND_SYSTEM),DCS2)
SND_DRIVER=	dcs2drv.c
else
SND_DRIVER=	dcsdrv.c
endif

IDE_DRIVER=	idedrv.c

#
# C Source Modules
#
CSRCS= 	__main.c sa1bmain.c conf.c io.c $(TTY_DRIVER) ioasic.c $(SYS_CONTROL) \
	memtest.c $(IDE_DRIVER) dskcache.c filesys.c filedrv.c leddrv.c wdogdrv.c \
	cmosdrv.c strchr.c psyq.c close.c commit.c creat.c creatnew.c \
	ffirst.c fnext.c getdate.c getdfree.c getdrive.c getfattr.c \
	getftime.c gettime.c lock.c open.c read.c setdate.c setdrive.c \
	setfattr.c setftime.c settime.c unlock.c write.c ioctl.c handlers.c \
	geterrno.c psyq.c timerdrv.c pciio.c picdrv.c $(SND_DRIVER) exec.c \
	mdiagdrv.c stubs.c
		

#
# Asm Source Modules
#
ASRCS= 	crt0.s except.s cache.s mtest.s cntreg.s memset.s memcpy.s \
	psyqdbg.s jtovrlay.s _exit.s tlb.s

#
# Object modules
#
OBJS = 	$(ASRCS:.s=.o) $(CSRCS:.c=.o)


#
# What we are going to build
#
all:	$(BUILDDIR)\sa1bios.bin
	@echo SA1BIOS for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\sa1bios.bin:	$(OBJS) sa1bios.lnk
	@$(LINK) /p @sa1bios.lnk,$(BUILDDIR)/sa1bios.bin,sa1bios.sym,sa1bios.map > sa1bios.log
	@echo SA1BIOS.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

sa1bios.lnk:	biosbin.in bflist.in
	@if EXIST sa1bios.lnk del sa1bios.lnk
	@copy biosbin.in+bflist.in sa1bios.lnk > NUL:
	@echo 	include	"sa1bmain.o" >> sa1bios.lnk
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> sa1bios.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> sa1bios.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> sa1bios.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> sa1bios.lnk

#
# Dependancies and rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $<

%.o:	%.s
	@echo $< to $@ for $(GAME) PIC on $(TARGET_SYS)
	@$(AS2) $<,$@

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

sa1bmain.c:	main.c
	@if EXIST sa1bmain.c del sa1bmain.c
	@copy main.c sa1bmain.c > NUL:

bgt64010.c:	gt64010.c
	@if EXIST bgt64010.c del bgt64010.c
	@copy gt64010.c bgt64010.c > NUL:

ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo SA1BIOS CLEAN
ifneq	($(wildcard *.o),)
	@del *.o
endif
ifneq	($(wildcard *.d),)
	@del *.d
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
	@echo SA1BIOS CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo SA1BIOS CLOBBER
	@if EXIST $(BUILDDIR)\sa1bios.bin del $(BUILDDIR)\sa1bios.bin
	@echo SA1BIOS CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo SA1BIOS REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f sa1bios.mak
	@echo SA1BIOS REBUILD for $(TARGET_SYS) DONE
	
