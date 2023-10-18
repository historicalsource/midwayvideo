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
TARGET_SYS=VEGAS


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
OBJS =	vegacsu.o nile4.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\vegabios.rom
	@echo VEGASBIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\vegabios.rom:	$(OBJS)
	@echo Linking VEGASBIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @vegarom.lnk,$(BUILDDIR)/vegabios.rom,,vegarom.map
	@romsum $(BUILDDIR)/vegabios.rom

\video\biosrom\bios\vegabios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f vegabios.mak rebuild

\video\biosrom\bios\vegatest.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f vegatest.mak rebuild


#
# Object modules
#
vegacsu.o:	vegacsu.s csu.s $(INCS) \video\biosrom\bios\vegabios.bin \video\biosrom\bios\vegatest.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) vegacsu.s,vegacsu.o,,vegacsu.lst

nile4.o:	nile4.s
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) nile4.s,nile4.o,,nile4.lst

#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo VEGASROM CLEAN
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
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f vegabios.mak clean
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f vegatest.mak clean
	@echo VEGASROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo VEGASROM CLOBBER
	@if EXIST $(BUILDDIR)\vegabios.rom del $(BUILDDIR)\vegabios.rom
	@echo VEGASROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo VEGASROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f vegatest.mak rebuild
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f vegabios.mak
	@$(MAKE) --no-print-directory -f vegasrom.mak
	@echo VEGASROM REBUILD for $(TARGET_SYS) DONE
