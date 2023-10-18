#
# Makefile for phoenix system startup ROM
#
#
# Copyright (c) 1996 by Williams Electronics Games Inc.
#
# Makefile for Operating System Kernel for Phoenix System
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
# Target systems
#
TARGET_SYS=SEATTLE


SYSTEM=$($(TARGET_SYS))

#
# Directories
#
BUILDDIR=	..

PROCESSOR=	R5000


#
# Assembler Command Line
#
AS2 =	@asmmips /q /of.rdata /zd /l /oc+ /e KERNEL=1 /e $(PROCESSOR)=1 /e PHOENIX_SYS=$(SYSTEM)


#
# Linker Command Line
#
LINK =	@psylink /c /m /ps

LINK1=	@psylink /s /c /m /wl /wm


#
# Object modules
#
OBJS =	nflcsu.o gt64010.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\nflbios.rom
	@echo NFLBIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\nflbios.rom:	$(OBJS)
	@echo Linking NFLBIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @nflrom.lnk,$(BUILDDIR)/nflbios.rom
	@romsum $(BUILDDIR)/nflbios.rom

\video\biosrom\bios\nflbios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nflbios.mak rebuild


\video\biosrom\bios\nfltest.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nfltest.mak rebuild


#
# Object modules
#
nflcsu.o:	nflcsu.s csu.s $(INCS) \video\biosrom\bios\nflbios.bin \video\biosrom\bios\nfltest.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) nflcsu.s,nflcsu.o,,nflcsu.lst

gt64010.o:	gt64010.s $(INCS)
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) gt64010.s,gt64010.o,,gt64010.lst


#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo NFLROM CLEAN
ifneq	($(wildcard *.o),)
	@del *.o
endif
ifneq	($(wildcard *.sym),)
	@del *.sym
endif
ifneq	($(wildcard *.lst),)
	@del *.lst
endif
ifneq	($(wildcard *.map),)
	@del *.map
endif
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nflbios.mak clean
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nfltest.mak clean
	@echo NFLROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo NFLROM CLOBBER
	@if EXIST $(BUILDDIR)\nflbios.rom del $(BUILDDIR)\nflbios.rom
	@echo NFLROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo NFLROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nfltest.mak rebuild
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nflbios.mak
	@$(MAKE) --no-print-directory -f nflrom.mak
	@echo NFLROM REBUILD for $(TARGET_SYS) DONE
