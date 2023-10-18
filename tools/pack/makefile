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
BUILDDIR=	..

#
# C Source files
#
CSRCS=	as.c bagger.c getopt.c util.c globals.c pack.c test.c

#
# Libraries
#
LIBS=

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\PACK
	@$(MAKE) --no-print-directory dosenv.h
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pack.exe
	@echo \VIDEO\TOOLS\PACK DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -D__DOS__ -MD -Wall -c -o $@ $<

dosenv.h:	genenv.exe
	@echo generating $@
	@genenv $@

#
# Rules for exe modules
#
$(BUILDDIR)\pack.exe:	$(CSRCS:.c=.o) $(LIBS)
	@gcc -D__DOS__ -Wall -o $(BUILDDIR)/pack.exe $(CSRCS:.c=.o) $(LIBS)

genenv.exe:
	@gcc -Wall -o genenv.exe genenv.c

ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\PACK CLEAN
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\TOOLS\PACK DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\PACK CLOBBER
	@if EXIST dosenv.h del dosenv.h
	@if EXIST genenv.exe del genenv.exe
	@if EXIST $(BUILDDIR)\pack.exe del $(BUILDDIR)\pack.exe
	@echo \VIDEO\TOOLS\PACK CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\PACK REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\PACK REBUILD DONE
