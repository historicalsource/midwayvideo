#
# Assorted directories
#
BUILDDIR=	..

#
# C Sources Modules
#
CSRCS=	csp.c

#
# Objet Modules
#
OBJS=	$(CSRCS:.c=.obj)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/csp.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET) dummy

#
# Dummy target
#
dummy:
	@echo CSP DONE


#
# Module rules and dependencies
#
$(BUILDDIR)/csp.exe:	$(OBJS)
	@echo Linking $@
	@gcc -O3 -Wall -o $@ $(OBJS)
	@echo $@ built

%.d:	%.c
	@echo Generating dependencies for $<
	@gcc -M $< | sed 's/$*.o/&bj $@/g' > $@

%.obj:	%.c
	@echo Compiling $<
	@gcc -c -O3 -o $@ $<

include	$(CSRCS:.c=.d)

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo Cleaning csp
	@if EXIST csp.obj del csp.obj
	@if EXIST csp.d del csp.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering $(BUILDDIR)\csp.exe
	@if EXIST $(BUILDDIR)\csp.exe del $(BUILDDIR)\csp.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@$(MAKE) --no-print-directory
