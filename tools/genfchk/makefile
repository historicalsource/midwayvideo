#
# Assorted directories
#
BUILDDIR=	..

#
# Include files everyone is dependant on
#
INCS=	$(INCDIR)/glide/glide.h

#
# C Source Modules
#
CSRCS=	genfchk.c

#
# Object Modules
#
OBJS=	$(CSRCS:.c=.o)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/genfchk.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET)
	@echo GENFCHK DONE

#
# Module rules and dependencies
#
$(BUILDDIR)/genfchk.exe:	$(OBJS)
	@gcc -O3 -o $(BUILDDIR)/genfchk.exe $(OBJS)

#
# Rules and dependencies
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -MD -O3 -c -o $@ $<

ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo Cleaning genfchk
	@if EXIST genfchk.o del genfchk.o
	@if EXIST genfchk.d del genfchk.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering $(BUILDDIR)\genfchk.exe
	@if EXIST $(BUILDDIR)\genfchk.exe del $(BUILDDIR)\genfchk.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@echo Rebuilding $(BUILDDIR)\genfchk.exe
	@$(MAKE) --no-print-directory

	
