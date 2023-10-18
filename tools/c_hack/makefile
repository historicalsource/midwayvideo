#
# Copyright (c) 1998 by Midway Games
#
# Makefile for c3d tool
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
CSRCS=	bagger.c c_hack.c pack.c tga.c util.c

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\C_HACK TOP LEVEL
	@$(MAKE) --no-print-directory $(BUILDDIR)\\c_hack.exe
	@echo \VIDEO\TOOLS\C_HACK DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -O2 -MD -Wall -c -o $@ $<

#
# Rules for exe modules
#
$(BUILDDIR)\c_hack.exe:	$(CSRCS:.c=.o)
	@gcc -O2 -Wall -o $(BUILDDIR)/c_hack.exe $(CSRCS:.c=.o)


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\C_HACK CLEAN
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\TOOLS\C_HACK DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\C_HACK CLOBBER
	@if EXIST $(BUILDDIR)\c_hack.exe del $(BUILDDIR)\c_hack.exe
	@echo \VIDEO\TOOLS\C_HACK CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\C_HACK REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\C_HACK REBUILD DONE
