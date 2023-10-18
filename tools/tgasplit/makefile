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
CSRCS=	tgasplit.c

#
# Object Modules
#
OBJS=	$(CSRCS:.c=.o)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/tgasplit.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET)
	@echo tgasplit DONE

#
# Module rules and dependencies
#
$(BUILDDIR)/tgasplit.exe:	$(OBJS)
	@gcc -g -o $(BUILDDIR)/tgasplit.exe $(OBJS)

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
	@echo Cleaning tgasplit
	@if EXIST tgasplit.o del tgasplit.o
	@if EXIST tgasplit.d del tgasplit.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering $(BUILDDIR)\tgasplit.exe
	@if EXIST $(BUILDDIR)\tgasplit.exe del $(BUILDDIR)\tgasplit.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@echo Rebuilding $(BUILDDIR)\tgasplit.exe
	@$(MAKE) --no-print-directory

	
