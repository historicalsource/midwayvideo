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
OBJS =	carncsu.o gt64010.o


#
# Include Files
#
INCS =	include/mon.inc include/cpu.inc include/p4000.inc include/gt64010.inc \
	include/phoenix.inc

#
# The main target we are going to build
#
all:	$(BUILDDIR)\carnbios.rom
	@echo CARNEVILBIOS.ROM for $(TARGET_SYS) DONE


#
# SRecord format target
#
$(BUILDDIR)\carnbios.rom:	$(OBJS)
	@echo Linking CARNEVILBIOS.ROM for $(TARGET_SYS)
	$(LINK1) /p @carnrom.lnk,$(BUILDDIR)/carnbios.rom
	@romsum $(BUILDDIR)/carnbios.rom

\video\biosrom\bios\carnbios.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f carnbios.mak rebuild

\video\biosrom\bios\carntest.bin:
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f carntest.mak rebuild


#
# Object modules
#
carncsu.o:	carncsu.s csu.s $(INCS) \video\biosrom\bios\carnbios.bin \video\biosrom\bios\carntest.bin
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) carncsu.s,carncsu.o,,carncsu.lst

gt64010.o:	gt64010.s $(INCS)
	@echo Assembling $< for $(TARGET_SYS)
	$(AS2) gt64010.s,gt64010.o,,gt64010.lst


#
# Completely clean out for clean rebuild
#
clean:	clobber
	@echo CARNEVILROM CLEAN
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
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f carnbios.mak clean
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f carntest.mak clean
	@echo CARNEVILROM CLEAN DONE


#
# Clobber the target for relink
#
clobber:
	@echo CARNEVILROM CLOBBER
	@if EXIST $(BUILDDIR)\carnbios.rom del $(BUILDDIR)\carnbios.rom
	@echo CARNEVILROM CLOBBER DONE

#
# What has to be done to do a complete rebuild
#
rebuild:	clean
	@echo CARNEVILROM REBUILD for $(TARGET_SYS)
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f carntest.mak rebuild
	@$(MAKE) --no-print-directory -C /video/biosrom/bios -f carnbios.mak
	@$(MAKE) --no-print-directory -f carnrom.mak
	@echo CARNEVILROM REBUILD for $(TARGET_SYS) DONE
