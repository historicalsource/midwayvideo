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
CSRCS=	buildscr.c upio.c

#
# Libraries
#
LIBS=		../zlib/zlib.lib

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\BUILDSCR TOP LEVEL
	@$(MAKE) -C ../zlib --no-print-directory

	@echo \VIDEO\TOOLS\BUILDSCR
	@$(MAKE) --no-print-directory $(BUILDDIR)\\buildscr.exe
	@echo \VIDEO\TOOLS\BUILDSCR DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -I../buildrom -I../zlib -MD -Wall -c -o $@ $<

upio.c:	..\buildrom\upio.c
	@copy ..\buildrom\upio.c . > NUL:

#
# Rules for exe modules
#
$(BUILDDIR)\buildscr.exe:	$(CSRCS:.c=.o) $(LIBS)
	@gcc -I../buildrom -I../zlib -Wall -o $(BUILDDIR)/buildscr.exe $(CSRCS:.c=.o) $(LIBS)


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\BUILDSCR CLEAN
	@$(MAKE) --no-print-directory -C ../zlib clean
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@if EXIST upio.c del upio.c
	@echo \VIDEO\TOOLS\BUILDSCR DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\BUILDSCR CLOBBER
	@$(MAKE) --no-print-directory -C ../zlib clobber
	@if EXIST $(BUILDDIR)\buildscr.exe del $(BUILDDIR)\buildscr.exe
	@echo \VIDEO\TOOLS\BUILDSCR CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\BUILDSCR REBUILD
	@$(MAKE) --no-print-directory -C ../zlib
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\BUILDSCR REBUILD DONE
