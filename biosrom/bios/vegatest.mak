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
GAME=DEVELOPMENT

#
# Target systems
#
TARGET_SYS=VEGAS

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
CC3 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @vbios.cf -O3
CC2 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @vbios.cf -O2
CC1 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @vbios.cf -O1
CC0 =		ccmips -DPHOENIX_SYS=$(SYSTEM) -D$(GAME) $(TEST) @vbios.cf -O0


#
# Drivers
#
TTY_DRIVER=	niletty.c

SYS_CONTROL=	nile4.c

SND_DRIVER=	dcs2drv.c

IDE_DRIVER=	pci0646.c

#
# C Source Modules
#
#CSRCS= 	
#	mdiagdrv.c

CSRCS= 	vtmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
	$(TTY_DRIVER) picdrv.c wdogdrv.c $(SND_DRIVER) read.c write.c exec.c \
	geterrno.c vcmosdrv.c settime.c gettime.c setdate.c getdate.c \
	strchr.c setdrive.c getdrive.c unlock.c lock.c memtest.c \
	$(SYS_CONTROL) pciio.c $(IDE_DRIVER) dskcache.c filesys.c commit.c \
	creat.c creatnew.c ffirst.c fnext.c getdfree.c getfattr.c getftime.c \
	setfattr.c setftime.c filedrv.c vtmrdrv.c vhand.c \
	mdiagdrv.c doprnt.c strcpy.c strlen.c memchr.c modfl.c lconv.c printf.c \
	ct_flags.c disasm.c ginit.c font8x8.c fpgaload.c memsize.c \
	vtest/cputest.c vtest/siotest.c \
	vmm.c coindrv.c syscall.c exit.c switch.c
#	vtest/tmemtest.c vtest/fmemtest.c vtest/qatest01.c
		

#
# Asm Source Modules
#
ASRCS= 	crt0.s cache.s jtovrlay.s tlb.s memcpy.s cntreg.s mtest.s memset.s \
	vexcept.s

#
# Object modules
#
OBJS = 	$(ASRCS:.s=.o) $(CSRCS:.c=.o)


#
# What we are going to build
#
all:	$(BUILDDIR)\vegatest.bin
	@echo VEGASTEST for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\vegatest.bin:	$(OBJS) vegatest.lnk vegatest.mak \video\lib\gliderom.a
	@$(LINK) /p @vegatest.lnk,$(BUILDDIR)/vegatest.bin,vegatest.sym,vegatest.map > vegatest.log
	@echo VEGASTEST.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

vegatest.lnk:	biosbin.in bflist.in vegatest.mak
	@if EXIST vegatest.lnk del vegatest.lnk
	@copy biosbin.in vegatest.lnk > NUL:
	@echo 	include	"crt0.o" >> vegatest.lnk
	@echo 	include	"cache.o" >> vegatest.lnk
	@echo 	include	"tlb.o" >> vegatest.lnk
	@echo 	include	"jtovrlay.o" >> vegatest.lnk
	@echo 	include	"memcpy.o" >> vegatest.lnk
	@echo 	include	"cntreg.o" >> vegatest.lnk
	@echo 	include	"vexcept.o" >> vegatest.lnk
	@echo 	include	"vhand.o" >> vegatest.lnk
	@echo 	include	"syscall.o" >> vegatest.lnk
	@echo 	include	"vtmain.o" >> vegatest.lnk
	@echo 	include	"__main.o" >> vegatest.lnk
	@echo 	include	"exec.o" >> vegatest.lnk
	@echo 	include	"conf.o" >> vegatest.lnk
	@echo 	include	"io.o" >> vegatest.lnk
	@echo 	include	"open.o" >> vegatest.lnk
	@echo 	include	"close.o" >> vegatest.lnk
	@echo 	include	"read.o" >> vegatest.lnk
	@echo 	include	"write.o" >> vegatest.lnk
	@echo 	include	"ioctl.o" >> vegatest.lnk
	@echo 	include	"geterrno.o" >> vegatest.lnk
	@echo 	include	"leddrv.o" >> vegatest.lnk
	@echo 	include	"coindrv.o" >> vegatest.lnk
	@echo 	include	"ioasic.o" >> vegatest.lnk
	@echo 	include	"picdrv.o" >> vegatest.lnk
	@echo 	include	"wdogdrv.o" >> vegatest.lnk
	@echo 	include	"vcmosdrv.o" >> vegatest.lnk
	@echo 	include	"filedrv.o" >> vegatest.lnk
	@echo 	include	"vtmrdrv.o" >> vegatest.lnk
	@echo 	include	"mdiagdrv.o" >> vegatest.lnk
	@echo 	include	"vmm.o" >> vegatest.lnk
	@echo 	include	"settime.o" >> vegatest.lnk
	@echo 	include	"gettime.o" >> vegatest.lnk
	@echo 	include	"setdate.o" >> vegatest.lnk
	@echo 	include	"getdate.o" >> vegatest.lnk
	@echo 	include	"strchr.o" >> vegatest.lnk
	@echo 	include	"setdrive.o" >> vegatest.lnk
	@echo 	include	"getdrive.o" >> vegatest.lnk
	@echo 	include	"unlock.o" >> vegatest.lnk
	@echo 	include	"lock.o" >> vegatest.lnk
	@echo 	include	"memtest.o" >> vegatest.lnk
	@echo 	include	"mtest.o" >> vegatest.lnk
	@echo 	include	"memset.o" >> vegatest.lnk
	@echo 	include	"exit.o" >> vegatest.lnk
	@echo 	include	"pciio.o" >> vegatest.lnk
	@echo 	include	"dskcache.o" >> vegatest.lnk
	@echo 	include	"filesys.o" >> vegatest.lnk
	@echo 	include	"commit.o" >> vegatest.lnk
	@echo 	include	"creat.o" >> vegatest.lnk
	@echo 	include	"creatnew.o" >> vegatest.lnk
	@echo 	include	"ffirst.o" >> vegatest.lnk
	@echo 	include	"fnext.o" >> vegatest.lnk
	@echo 	include	"getdfree.o" >> vegatest.lnk
	@echo 	include	"getfattr.o" >> vegatest.lnk
	@echo 	include	"getftime.o" >> vegatest.lnk
	@echo 	include	"setfattr.o" >> vegatest.lnk
	@echo 	include	"setftime.o" >> vegatest.lnk
	@echo 	include	"doprnt.o" >> vegatest.lnk
	@echo 	include	"strcpy.o" >> vegatest.lnk
	@echo 	include	"strlen.o" >> vegatest.lnk
	@echo 	include	"memchr.o" >> vegatest.lnk
	@echo 	include	"modfl.o" >> vegatest.lnk
	@echo 	include	"lconv.o" >> vegatest.lnk
	@echo 	include	"printf.o" >> vegatest.lnk
	@echo 	include	"ct_flags.o" >> vegatest.lnk
	@echo 	include	"disasm.o" >> vegatest.lnk
	@echo 	include	"ginit.o" >> vegatest.lnk
	@echo 	include	"font8x8.o" >> vegatest.lnk
	@echo 	include	"fpgaload.o" >> vegatest.lnk
	@echo 	include	"memsize.o" >> vegatest.lnk
ifeq	($(TESTROM),YES)
	@echo 	include	"vtest/cputest.o" >> vegatest.lnk
	@echo 	include	"vtest/siotest.o" >> vegatest.lnk
endif
	@echo 	include	"switch.o" >> vegatest.lnk
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> vegatest.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> vegatest.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> vegatest.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> vegatest.lnk
	@echo 	inclib	"\video\lib\gliderom.a" >> vegatest.lnk
#	@echo 	include	"vtest/qatest01.o" >> vegatest.lnk
#	@echo 	include	"vtest/fmemtest.o" >> vegatest.lnk
#	@echo 	include	"vtest/tmemtest.o" >> vegatest.lnk

#
# Dependancies and rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC3) -o $@ $<

%.o:	%.s
	@echo $< to $@ for $(GAME) PIC on $(TARGET_SYS)
	@$(AS2) $<,$@

%.S:	%.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -S -o $@ $<

\video\lib\gliderom.a:
	@$(MAKE) --no-print-directory -C /video/lib/gliderom

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

vtmain.c:	vmain.c
	@if EXIST vtmain.c del vtmain.c
	@copy vmain.c vtmain.c > NUL:

vtest/cputest.o:	vtest/cputest.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $<

#
# Files that are always built
#
.PHONY:	vtmain.o vhand.o niletty.o nile4.o memtest.o exec.o conf.o syscall.o \
	switch.o

vtmain.o:	vtmain.c
	@echo $< to $@ and $(basename $<).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $<

vhand.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

niletty.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

nile4.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

memtest.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

exec.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

conf.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

syscall.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

switch.o:
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
	@$(CC2) -o $@ $(basename $@).c

ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif
ifeq	($(TESTROM),YES)
ifneq	($(wildcard vtest/*.d),)
include	$(wildcard vtest/*.d)
endif
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo VEGASTEST CLEAN
ifneq	($(wildcard *.o),)
	@del *.o
endif
ifneq	($(wildcard vtest/*.o),)
	@del vtest\*.o
endif
ifneq	($(wildcard *.d),)
	@del *.d
endif
ifneq	($(wildcard vtest/*.d),)
	@del vtest\*.d
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
	@echo VEGASTEST CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo VEGASTEST CLOBBER
	@if EXIST $(BUILDDIR)\vegatest.bin del $(BUILDDIR)\vegatest.bin
	@echo VEGASTEST CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo VEGASTEST REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f vegatest.mak
	@echo VEGASTEST REBUILD for $(TARGET_SYS) DONE
	
