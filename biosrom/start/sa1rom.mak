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
TARGET_SYS=SA1

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
OBJS =	sa1csu.o gt64010.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\sa1bios.rom
	@echo SA1BIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\sa1bios.rom:	$(OBJS)
	@echo Linking SA1BIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @sa1rom.lnk,$(BUILDDIR)/sa1bios.rom
	@romsum $(BUILDDIR)/sa1bios.rom

\video\biosrom\bios\sa1bios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f sa1bios.mak rebuild


#
# Object modules
#
sa1csu.o:	sa1csu.s csu.s $(INCS) \video\biosrom\bios\sa1bios.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) sa1csu.s,sa1csu.o,,sa1csu.lst

gt64010.o:	gt64010.s $(INCS)
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) gt64010.s,gt64010.o,,gt64010.lst


#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo SA1ROM CLEAN
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
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f sa1bios.mak clean
	@echo SA1ROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo SA1ROM CLOBBER
	@if EXIST $(BUILDDIR)\sa1bios.rom del $(BUILDDIR)\sa1bios.rom
	@echo SA1ROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo SA1ROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f sa1bios.mak rebuild
	@$(MAKE) --no-print-directory -f sa1rom.mak
	@echo SA1ROM REBUILD for $(TARGET_SYS) DONE
