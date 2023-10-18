#
# Assorted directories
#
BUILDDIR=	..

#
# C Sources Modules
#
CSRCS=	clw.c iffr.c uvr.c

#
# Objet Modules
#
OBJS=	$(CSRCS:.c=.obj)

#
# What we are going to build
#
TARGET=	$(BUILDDIR)/clw.exe

#
# This gets built if nothing is specified on the command line
#
all:	$(TARGET) dummy

#
# Dummy target
#
dummy:
	@echo CLW DONE


#
# Module rules and dependencies
#
$(BUILDDIR)/clw.exe:	$(OBJS)
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
	@if EXIST clw.obj del clw.obj
	@if EXIST clw.d del clw.d
	@if EXIST iffr.obj del iffr.obj
	@if EXIST iffr.d del iffr.d
	@if EXIST uvr.obj del uvr.obj
	@if EXIST uvr.d del uvr.d

#
# How to clean up for a relink only
#
clobber:
	@echo Clobbering $(BUILDDIR)\cvtmod.exe
	@if EXIST $(BUILDDIR)\cvtmod.exe del $(BUILDDIR)\cvtmod.exe

#
# How to rebuild the whole works from scratch
#
rebuild:	clean
	@$(MAKE) --no-print-directory
