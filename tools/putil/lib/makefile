#
# Copyright (c) 1996 by Williams Electronics Games Inc.
#
# Makefile for pd library for host to target tools
#
# board specific defines
#
# $Revision: $
#

#
# Directories of where stuff is
#
BUILDDIR=	.

#
# C Sources
#
CSRCS =	pdcreat.c pdexterr.c pdfindn.c pdgetdf.c pdgetfa.c pdgettim.c \
	pdread.c pdsetdrv.c pdsetftm.c pdwrite.c pdclose.c pdcommit.c \
	pdcreatn.c pdfindf.c pdgetdat.c pdgetdrv.c pdgetftm.c pdopen.c \
	pdsetdat.c pdsetfa.c pdsettim.c pd.c fnsplit.c wrsector.c rdsector.c \
	getptbl.c memwrite.c memread.c gettarid.c getinfo.c getver.c reset.c \
	initserv.c getraddr.c halt.c resume.c bmwrite.c enable.c bmread.c \
	unlock.c pdremove.c sndwrite.c


#
# Object modules
#
OBJS =	  	$(CSRCS:.c=.o)

.PHONY:	dummy

#
# What we are going to build
#
all:	dummy $(BUILDDIR)\pd.lib
	@echo \VIDEO\TOOLS\PUTIL\LIB DONE

dummy:
	@echo \VIDEO\TOOLS\PUTIL\LIB


#
# Dependencies and rules for primary target
#
$(BUILDDIR)\pd.lib:	$(OBJS)
	@if EXIST $(BUILDDIR)\pd.lib del $(BUILDDIR)\pd.lib
	@if EXIST pd.a del pd.a
	@ar cr pd.a $(OBJS)
	@copy pd.a $(BUILDDIR)\pd.lib > NUL:
	@del pd.a


#
# Dependancies and rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -MD -Wall -c -o $@ $<

ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\PUTIL\LIB CLEAN
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\TOOLS\PUTIL\LIB CLEAN DONE

#
# How to clean up for a relink
#
clobber:
	@echo \VIDEO\TOOLS\PUTIL\LIB CLOBBER
	@if EXIST pd.a del pd.a
	@if EXIST $(BUILDDIR)\pd.lib del $(BUILDDIR)\pd.lib
	@echo \VIDEO\TOOLS\PUTIL\LIB CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\PUTIL\LIB REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\PUTIL\LIB REBUILD DONE
	
