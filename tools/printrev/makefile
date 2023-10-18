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
CSRCS=	printrev.c upio.c

#
# Libraries
#
LIBS=		../zlib/zlib.lib

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\PRINTREV TOP LEVEL
	@$(MAKE) -C ../zlib --no-print-directory

	@echo \VIDEO\TOOLS\PRINTREV
	@$(MAKE) --no-print-directory $(BUILDDIR)\\printrev.exe
	@echo \VIDEO\TOOLS\PRINTREV DONE

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
$(BUILDDIR)\printrev.exe:	$(CSRCS:.c=.o) $(LIBS)
	@gcc -I../buildrom -I../zlib -Wall -o $(BUILDDIR)/printrev.exe $(CSRCS:.c=.o) $(LIBS)


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\PRINTREV CLEAN
	@$(MAKE) --no-print-directory -C ../zlib clean
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@if EXIST upio.c del upio.c
	@echo \VIDEO\TOOLS\PRINTREV DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\PRINTREV CLOBBER
	@$(MAKE) --no-print-directory -C ../zlib clobber
	@if EXIST $(BUILDDIR)\printrev.exe del $(BUILDDIR)\printrev.exe
	@echo \VIDEO\TOOLS\PRINTREV CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\PRINTREV REBUILD
	@$(MAKE) --no-print-directory -C ../zlib
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\PRINTREV REBUILD DONE
