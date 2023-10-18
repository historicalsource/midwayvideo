#
# Copyright (c) 1998 by Midways Games Inc
#
# Makefile for tweakd utility
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
CSRCS=	tweakd.c

#
# Libraries
#
LIBS=

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\TWEAKD
	@$(MAKE) --no-print-directory $(BUILDDIR)\\tweakd.exe
	@echo \VIDEO\TOOLS\TWEAKD DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -MD -Wall -c -o $@ $<

#
# Rules for exe modules
#
$(BUILDDIR)\tweakd.exe:	$(CSRCS:.c=.o) $(LIBS)
	@gcc -Wall -o $(BUILDDIR)/tweakd.exe $(CSRCS:.c=.o) $(LIBS)

ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\TWEAKD CLEAN
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\TOOLS\TWEAKD DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\TWEAKD CLOBBER
	@if EXIST $(BUILDDIR)\tweakd.exe del $(BUILDDIR)\tweakd.exe
	@echo \VIDEO\TOOLS\TWEAKD CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\TWEAKD REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\TWEAKD REBUILD DONE
