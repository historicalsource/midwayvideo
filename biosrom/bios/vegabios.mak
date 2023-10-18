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
# Test ROM
#
TESTROM=NO
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

CSRCS= 	vbmain.c __main.c conf.c leddrv.c open.c io.c close.c ioctl.c ioasic.c \
	$(TTY_DRIVER) picdrv.c wdogdrv.c $(SND_DRIVER) read.c write.c \
	geterrno.c vcmosdrv.c settime.c gettime.c setdate.c getdate.c \
	strchr.c setdrive.c getdrive.c unlock.c lock.c memtest.c \
	$(SYS_CONTROL) pciio.c $(IDE_DRIVER) dskcache.c filesys.c commit.c \
	creat.c creatnew.c ffirst.c fnext.c getdfree.c getfattr.c getftime.c \
	setfattr.c setftime.c filedrv.c vtmrdrv.c vhand.c psyq.c exec.c \
	memsize.c stubs.c coindrv.c \
	mdiagdrv.c doprnt.c strcpy.c strlen.c memchr.c modfl.c lconv.c printf.c \
	ct_flags.c disasm.c fpgaload.c vmm.c syscall.c exit.c switch.c
		

#
# Asm Source Modules
#
#ASRCS= 	
#	psyqdbg.s

ASRCS= 	crt0.s cache.s jtovrlay.s tlb.s memcpy.s cntreg.s memset.s \
	vexcept.s

#
# Object modules
#
OBJS = 	$(ASRCS:.s=.o) $(CSRCS:.c=.o)


#
# What we are going to build
#
all:	$(BUILDDIR)\vegabios.bin
	@echo VEGASBIOS for $(GAME) PIC on $(TARGET_SYS)
	@echo DONE


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\vegabios.bin:	$(OBJS) vegabios.lnk vegabios.mak \video\lib\glide244.a
	@$(LINK) /p @vegabios.lnk,$(BUILDDIR)/vegabios.bin,vegabios.sym,vegabios.map > vegabios.log
	@echo VEGASBIOS.BIN for $(GAME) PIC on $(TARGET_SYS) DONE

vegabios.lnk:	biosbin.in bflist.in vegabios.mak
	@if EXIST vegabios.lnk del vegabios.lnk
	@copy biosbin.in vegabios.lnk > NUL:
	@echo 	include	"crt0.o" >> vegabios.lnk
	@echo 	include	"cache.o" >> vegabios.lnk
	@echo 	include	"tlb.o" >> vegabios.lnk
	@echo 	include	"jtovrlay.o" >> vegabios.lnk
	@echo 	include	"memcpy.o" >> vegabios.lnk
	@echo 	include	"cntreg.o" >> vegabios.lnk
	@echo 	include	"vexcept.o" >> vegabios.lnk
	@echo 	include	"vhand.o" >> vegabios.lnk
	@echo 	include	"syscall.o" >> vegabios.lnk
	@echo 	include	"vbmain.o" >> vegabios.lnk
	@echo 	include	"__main.o" >> vegabios.lnk
	@echo 	include	"conf.o" >> vegabios.lnk
	@echo 	include	"io.o" >> vegabios.lnk
	@echo 	include	"open.o" >> vegabios.lnk
	@echo 	include	"close.o" >> vegabios.lnk
	@echo 	include	"read.o" >> vegabios.lnk
	@echo 	include	"write.o" >> vegabios.lnk
	@echo 	include	"ioctl.o" >> vegabios.lnk
	@echo 	include	"geterrno.o" >> vegabios.lnk
	@echo 	include	"leddrv.o" >> vegabios.lnk
	@echo 	include	"coindrv.o" >> vegabios.lnk
	@echo 	include	"ioasic.o" >> vegabios.lnk
	@echo 	include	"picdrv.o" >> vegabios.lnk
	@echo 	include	"wdogdrv.o" >> vegabios.lnk
	@echo 	include	"vcmosdrv.o" >> vegabios.lnk
	@echo 	include	"filedrv.o" >> vegabios.lnk
	@echo 	include	"vtmrdrv.o" >> vegabios.lnk
	@echo 	include	"mdiagdrv.o" >> vegabios.lnk
	@echo 	include	"vmm.o" >> vegabios.lnk
	@echo 	include	"settime.o" >> vegabios.lnk
	@echo 	include	"gettime.o" >> vegabios.lnk
	@echo 	include	"setdate.o" >> vegabios.lnk
	@echo 	include	"getdate.o" >> vegabios.lnk
	@echo 	include	"strchr.o" >> vegabios.lnk
	@echo 	include	"setdrive.o" >> vegabios.lnk
	@echo 	include	"getdrive.o" >> vegabios.lnk
	@echo 	include	"unlock.o" >> vegabios.lnk
	@echo 	include	"lock.o" >> vegabios.lnk
	@echo 	include	"memtest.o" >> vegabios.lnk
	@echo 	include	"memset.o" >> vegabios.lnk
	@echo 	include	"exit.o" >> vegabios.lnk
	@echo 	include	"pciio.o" >> vegabios.lnk
	@echo 	include	"dskcache.o" >> vegabios.lnk
	@echo 	include	"filesys.o" >> vegabios.lnk
	@echo 	include	"commit.o" >> vegabios.lnk
	@echo 	include	"creat.o" >> vegabios.lnk
	@echo 	include	"creatnew.o" >> vegabios.lnk
	@echo 	include	"ffirst.o" >> vegabios.lnk
	@echo 	include	"fnext.o" >> vegabios.lnk
	@echo 	include	"getdfree.o" >> vegabios.lnk
	@echo 	include	"getfattr.o" >> vegabios.lnk
	@echo 	include	"getftime.o" >> vegabios.lnk
	@echo 	include	"setfattr.o" >> vegabios.lnk
	@echo 	include	"setftime.o" >> vegabios.lnk
	@echo 	include	"psyq.o" >> vegabios.lnk
	@echo 	include	"exec.o" >> vegabios.lnk
	@echo 	include	"fpgaload.o" >> vegabios.lnk
	@echo 	include	"memsize.o" >> vegabios.lnk
	@echo 	include	"doprnt.o" >> vegabios.lnk
	@echo 	include	"strcpy.o" >> vegabios.lnk
	@echo 	include	"strlen.o" >> vegabios.lnk
	@echo 	include	"memchr.o" >> vegabios.lnk
	@echo 	include	"modfl.o" >> vegabios.lnk
	@echo 	include	"lconv.o" >> vegabios.lnk
	@echo 	include	"printf.o" >> vegabios.lnk
	@echo 	include	"ct_flags.o" >> vegabios.lnk
	@echo 	include	"disasm.o" >> vegabios.lnk
	@echo 	include	"stubs.o" >> vegabios.lnk
	@echo 	include	"switch.o" >> vegabios.lnk
	@echo 	include	"$(TTY_DRIVER:.c=.o)" >> vegabios.lnk
	@echo 	include	"$(SND_DRIVER:.c=.o)" >> vegabios.lnk
	@echo 	include	"$(IDE_DRIVER:.c=.o)" >> vegabios.lnk
	@echo 	include	"$(SYS_CONTROL:.c=.o)" >> vegabios.lnk

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

vbmain.c:	vmain.c
	@if EXIST vbmain.c del vbmain.c
	@copy vmain.c vbmain.c > NUL:

#
# Files that are always built
#
.PHONY:	vbmain.o vhand.o niletty.o nile4.o memtest.o exec.o conf.o syscall.o \
	switch.o

vbmain.o:	vbmain.c
	@echo $(basename $@).c to $@ and $(basename $@).d for $(GAME) PIC on $(TARGET_SYS)
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

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo VEGASBIOS CLEAN
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
	@echo VEGASBIOS CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo VEGASBIOS CLOBBER
	@if EXIST $(BUILDDIR)\vegabios.bin del $(BUILDDIR)\vegabios.bin
	@echo VEGASBIOS CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo VEGASBIOS REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -f vegabios.mak
	@echo VEGASBIOS REBUILD for $(TARGET_SYS) DONE
	
