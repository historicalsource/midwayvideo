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
CSRCS=	parse.c lex.c buildrom.c upio.c

#
# Libraries
#
LIBS=		../zlib/zlib.lib

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\BUILDROM TOP LEVEL
	@$(MAKE) -C ../zlib --no-print-directory

	@echo \VIDEO\TOOLS\BUILDROM
	@$(MAKE) --no-print-directory $(BUILDDIR)\\buildrom.exe
	@echo \VIDEO\TOOLS\BUILDROM DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -I../zlib -MD -Wall -c -o $@ $<

parse.c:	parse.y
	@echo $< to $@
	@bison --defines -o $@ $<

#
# Rules for exe modules
#
$(BUILDDIR)\buildrom.exe:	$(CSRCS:.c=.o) $(LIBS)
	@gcc -I../zlib -Wall -o $(BUILDDIR)/buildrom.exe $(CSRCS:.c=.o) $(LIBS)


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\BUILDROM CLEAN
	@$(MAKE) --no-print-directory -C ../zlib clean
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@if EXIST parse.h del parse.h
	@if EXIST parse.c del parse.c
	@echo \VIDEO\TOOLS\BUILDROM DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\BUILDROM CLOBBER
	@$(MAKE) --no-print-directory -C ../zlib clobber
	@if EXIST $(BUILDDIR)\buildrom.exe del $(BUILDDIR)\buildrom.exe
	@echo \VIDEO\TOOLS\BUILDROM CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\BUILDROM REBUILD
	@$(MAKE) --no-print-directory -C ../zlib
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\BUILDROM REBUILD DONE
	
