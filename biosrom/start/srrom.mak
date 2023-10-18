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
OBJS =	srcsu.o gt64010.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\srbios.rom
	@echo SRBIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\srbios.rom:	$(OBJS)
	@echo Linking SRBIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @srrom.lnk,$(BUILDDIR)/srbios.rom,,srrom.map
	@romsum $(BUILDDIR)/srbios.rom

\video\biosrom\bios\srbios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f srbios.mak rebuild

\video\biosrom\bios\srtest.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f srtest.mak rebuild


#
# Object modules
#
srcsu.o:	srcsu.s csu.s $(INCS) \video\biosrom\bios\srbios.bin \video\biosrom\bios\srtest.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) srcsu.s,srcsu.o,,srcsu.lst

gt64010.o:	gt64010.s $(INCS)
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) gt64010.s,gt64010.o,,gt64010.lst


#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo SRROM CLEAN
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
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f srbios.mak clean
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f srtest.mak clean
	@echo SRROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo SRROM CLOBBER
	@if EXIST $(BUILDDIR)\srbios.rom del $(BUILDDIR)\srbios.rom
	@echo SRROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo SRROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f srtest.mak rebuild
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f srbios.mak
	@$(MAKE) --no-print-directory -f srrom.mak
	@echo SRROM REBUILD for $(TARGET_SYS) DONE
