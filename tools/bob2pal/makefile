#
# Copyright (c) 1998 by Midway Games
#
# Makefile for bob2pal tool
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
CSRCS=	bob2pal.c

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\BOB2PAL TOP LEVEL
	@$(MAKE) --no-print-directory $(BUILDDIR)\\bob2pal.exe
	@echo \VIDEO\TOOLS\BOB2PAL DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -O2 -MD -Wall -c -o $@ $<

#
# Rules for exe modules
#
$(BUILDDIR)\bob2pal.exe:	$(CSRCS:.c=.o)
	@gcc -O2 -Wall -o $(BUILDDIR)/bob2pal.exe $(CSRCS:.c=.o)


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\BOB2PAL CLEAN
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\TOOLS\BOB2PAL DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\BOB2PAL CLOBBER
	@if EXIST $(BUILDDIR)\bob2pal.exe del $(BUILDDIR)\bob2pal.exe
	@echo \VIDEO\TOOLS\BOB2PAL CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\BOB2PAL REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\BOB2PAL REBUILD DONE
