#
# Assorted directories
#
BUILDDIR=	..

#
# C Sources Modules
#
CSRCS=	seq2ani.c

#
# Objet Modules
#
OBJS=	$(CSRCS:.c=.obj)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/seq2ani.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET) dummy

#
# Dummy target
#
dummy:
	@echo SEQ2ANI DONE


#
# Module rules and dependencies
#
$(BUILDDIR)/seq2ani.exe:	$(OBJS)
	@echo Linking $@
	@gcc -O3 -Wall -o $@ $(OBJS)
	@echo $@ built

%.d:	%.c
	@echo Generating dependencies for $<
	@gcc -M $< | sed 's/$*.o/&bj $@/g' > $@

%.c:	%.l
	@echo Generating source file
	@flex -oseq2ani.c seq2ani.l

%.obj:	%.c
	@echo Compiling $<
	@gcc -c -O3 -o $@ $<

include	$(CSRCS:.c=.d)

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo Cleaning seq2ani
	@if EXIST seq2ani.obj del seq2ani.obj
	@if EXIST seq2ani.c del seq2ani.c
	@if EXIST seq2ani.d del seq2ani.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering $(BUILDDIR)\seq2ani.exe
	@if EXIST $(BUILDDIR)\seq2ani.exe del $(BUILDDIR)\seq2ani.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@$(MAKE) --no-print-directory
