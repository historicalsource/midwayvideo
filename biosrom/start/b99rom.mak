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
OBJS =	b99csu.o gt64010.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\b99bios.rom
	@echo B99BIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\b99bios.rom:	$(OBJS)
	@echo Linking B99BIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @b99rom.lnk,$(BUILDDIR)/b99bios.rom
	@romsum $(BUILDDIR)/b99bios.rom

\video\biosrom\bios\b99bios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f b99bios.mak rebuild


\video\biosrom\bios\b99test.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f b99test.mak rebuild


#
# Object modules
#
b99csu.o:	b99csu.s csu.s $(INCS) \video\biosrom\bios\b99bios.bin \video\biosrom\bios\b99test.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) b99csu.s,b99csu.o,,b99csu.lst

gt64010.o:	gt64010.s $(INCS)
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) gt64010.s,gt64010.o,,gt64010.lst


#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo B99ROM CLEAN
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
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f b99bios.mak clean
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f b99test.mak clean
	@echo B99ROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo B99ROM CLOBBER
	@if EXIST $(BUILDDIR)\b99bios.rom del $(BUILDDIR)\b99bios.rom
	@echo B99ROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo B99ROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f b99test.mak rebuild
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f b99bios.mak
	@$(MAKE) --no-print-directory -f b99rom.mak
	@echo B99ROM REBUILD for $(TARGET_SYS) DONE
