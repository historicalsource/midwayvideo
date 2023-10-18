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
OBJS =	nbacsu.o gt64010.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\nbabios.rom
	@echo NBABIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\nbabios.rom:	$(OBJS)
	@echo Linking NBABIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @nbarom.lnk,$(BUILDDIR)/nbabios.rom
	@romsum $(BUILDDIR)/nbabios.rom

\video\biosrom\bios\nbabios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nbabios.mak rebuild

\video\biosrom\bios\nbatest.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nbatest.mak rebuild


#
# Object modules
#
nbacsu.o:	nbacsu.s csu.s $(INCS) \video\biosrom\bios\nbabios.bin \video\biosrom\bios\nbatest.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) nbacsu.s,nbacsu.o,,nbacsu.lst

gt64010.o:	gt64010.s $(INCS)
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) gt64010.s,gt64010.o,,gt64010.lst


#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo NBAROM CLEAN
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
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nbabios.mak clean
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nbatest.mak clean
	@echo NBAROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo NBAROM CLOBBER
	@if EXIST $(BUILDDIR)\nbabios.rom del $(BUILDDIR)\nbabios.rom
	@echo NBAROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo NBAROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nbatest.mak rebuild
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f nbabios.mak
	@$(MAKE) --no-print-directory -f nbarom.mak
	@echo NBAROM REBUILD for $(TARGET_SYS) DONE
