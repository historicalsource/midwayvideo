#
# Assorted directories
#
BUILDDIR=	..

#
# C Sources Modules
#
CSRCS=	dumpbod.c

#
# Objet Modules
#
OBJS=	$(CSRCS:.c=.obj)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/dumpbod.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET) dummy

#
# Dummy target
#
dummy:
	@echo DUMPBOD DONE


#
# Module rules and dependencies
#
$(BUILDDIR)/dumpbod.exe:	$(OBJS)
	@echo Linking $@
	@gcc -O3 -Wall -o $@ $(OBJS)
	@echo $@ built

%.d:	%.c
	@echo Generating dependencies for $<
	@gcc -M $< | sed 's/$*.o/&bj $@/g' > $@

%.obj:	%.c
	@echo Compiling $<
#	@gcc -c -Wall -O3 -o $@ $<
	@gcc -c -Wall -O3 -DJONHEY -o $@ $<


include	$(CSRCS:.c=.d)

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo Cleaning cvtmod
	@if EXIST dumpbod.obj del dumpbod.obj
	@if EXIST dumpbod.d del dumpbod.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering $(BUILDDIR)\dumpbod.exe
	@if EXIST $(BUILDDIR)\dumpbod.exe del $(BUILDDIR)\dumpbod.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@$(MAKE) --no-print-directory
