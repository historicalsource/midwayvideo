#
# Copyright (c) 1996 by Williams Electronics Games Inc.
#
# Makefile for Operating System Kernel for Space Race
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
CSRCS= 	__main.c srbmain.c conf.c io.c $(TTY_DRIVER) ioasic.c $(SYS_CONTROL) \
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
all:	$(BUILDDIR)\srbios.bin
	@echo SRBIOS for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\srbios.bin:	$(OBJS) srbios.lnk
	@$(LINK) /p @srbios.lnk,$(BUILDDIR)/srbios.bin,srbios.sym,srbios.map > srbios.log
	@echo SRBIOS.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

srbios.lnk:	biosbin.in bflist.in
	@if EXIST srbios.lnk del srbios.lnk
	@copy biosbin.in+bflist.in srbios.lnk > NUL:
	@echo 	include	"srbmain.o" >> srbios.lnk
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> srbios.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> srbios.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> srbios.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> srbios.lnk

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

srbmain.c:	srmain.c
	@if EXIST srbmain.c del srbmain.c
	@copy srmain.c srbmain.c > NUL:

bgt64010.c:	gt64010.c
	@if EXIST bgt64010.c del bgt64010.c
	@copy gt64010.c bgt64010.c > NUL:

#
# Files that must be compiled due to TEST compile time switch
#

.PHONY:	conf.o exec.o ginit.o memtest.o idedrv.o bgt64010.o srbmain.o

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

bgt64010.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c

srbmain.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $(basename $@).c


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo SRBIOS CLEAN
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
	@echo SRBIOS CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo SRBIOS CLOBBER
	@if EXIST $(BUILDDIR)\srbios.bin del $(BUILDDIR)\srbios.bin
	@echo SRBIOS CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo SRBIOS REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f srbios.mak
	@echo SRBIOS REBUILD for $(TARGET_SYS) DONE
	
