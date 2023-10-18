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
# Libraries
#
LIBS=		lib/pd.lib

#
# What we are going to build
#
all:
	@echo \VIDEO\TOOLS\PUTIL TOP LEVEL
	@$(MAKE) -C lib --no-print-directory

	@echo \VIDEO\TOOLS\PCOPY
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pcopy.exe
	@echo \VIDEO\TOOLS\PCOPY DONE

	@echo \VIDEO\TOOLS\PDIR
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pdir.exe
	@echo \VIDEO\TOOLS\PDIR DONE

	@echo \VIDEO\TOOLS\DTEST
	@$(MAKE) --no-print-directory $(BUILDDIR)\\dtest.exe
	@echo \VIDEO\TOOLS\DTEST DONE

	@echo \VIDEO\TOOLS\PWAKE
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pwake.exe
	@echo \VIDEO\TOOLS\PWAKE DONE

	@echo \VIDEO\TOOLS\PCOMM
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pcomm.exe
	@echo \VIDEO\TOOLS\PCOMM DONE

	@echo \VIDEO\TOOLS\POSLOAD
	@$(MAKE) --no-print-directory $(BUILDDIR)\\posload.exe
	@echo \VIDEO\TOOLS\POSLOAD DONE

	@echo \VIDEO\TOOLS\PBLOAD
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pbload.exe
	@echo \VIDEO\TOOLS\PBLOAD DONE

	@echo \VIDEO\TOOLS\PGAS
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pgas.exe
	@echo \VIDEO\TOOLS\PGAS DONE

	@echo \VIDEO\TOOLS\PHALT
	@$(MAKE) --no-print-directory $(BUILDDIR)\\phalt.exe
	@echo \VIDEO\TOOLS\PHALT DONE

	@echo \VIDEO\TOOLS\PRESUME
	@$(MAKE) --no-print-directory $(BUILDDIR)\\presume.exe
	@echo \VIDEO\TOOLS\PRESUME DONE

	@echo \VIDEO\TOOLS\PRUN
	@$(MAKE) --no-print-directory $(BUILDDIR)\\prun.exe
	@echo \VIDEO\TOOLS\PRUN DONE

	@echo \VIDEO\TOOLS\PTEST
	@$(MAKE) --no-print-directory $(BUILDDIR)\\ptest.exe
	@echo \VIDEO\TOOLS\PTEST DONE

	@echo \VIDEO\TOOLS\PDTEST
	@$(MAKE) --no-print-directory $(BUILDDIR)\\pdtest.exe
	@echo \VIDEO\TOOLS\PDTEST DONE

	@echo \VIDEO\TOOLS\PUTIL TOP LEVEL DONE

#
# Rules for object modules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	@gcc -Ilib -MD -Wall -c -o $@ $<

#
# Rules for exe modules
#
$(BUILDDIR)\pcopy.exe:	pcopy.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pcopy.exe pcopy.o $(LIBS)

$(BUILDDIR)\pdir.exe:	pdir.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pdir.exe pdir.o $(LIBS)

$(BUILDDIR)\dtest.exe:	dtest.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/dtest.exe dtest.o $(LIBS)

$(BUILDDIR)\pwake.exe:	pwake.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pwake.exe pwake.o $(LIBS)

$(BUILDDIR)\pcomm.exe:	pcomm.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pcomm.exe pcomm.o $(LIBS)

$(BUILDDIR)\posload.exe:	posload.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/posload.exe posload.o $(LIBS)

$(BUILDDIR)\pbload.exe:	pbload.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pbload.exe pbload.o $(LIBS)

$(BUILDDIR)\pgas.exe:	pgas.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pgas.exe pgas.o $(LIBS)

$(BUILDDIR)\prun.exe:	prun.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/prun.exe prun.o $(LIBS)

$(BUILDDIR)\presume.exe:	presume.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/presume.exe presume.o $(LIBS)

$(BUILDDIR)\phalt.exe:	phalt.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/phalt.exe phalt.o $(LIBS)

$(BUILDDIR)\ptest.exe:	ptest.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/ptest.exe ptest.o $(LIBS)

$(BUILDDIR)\pdtest.exe:	pdtest.o $(LIBS)
	@gcc -Ilib -Wall -o $(BUILDDIR)/pdtest.exe pdtest.o $(LIBS)


ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif


#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\TOOLS\PUTIL CLEAN
	@$(MAKE) --no-print-directory -C lib clean
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\TOOLS\PUTIL DONE

#
# How to clean up for a relink
#
clobber:			     
	@echo \VIDEO\TOOLS\PUTIL CLOBBER
	@$(MAKE) --no-print-directory -C lib clobber
	@if EXIST $(BUILDDIR)\pcopy.exe del $(BUILDDIR)\pcopy.exe
	@if EXIST $(BUILDDIR)\pdir.exe del $(BUILDDIR)\pdir.exe
	@if EXIST $(BUILDDIR)\dtest.exe del $(BUILDDIR)\dtest.exe
	@if EXIST $(BUILDDIR)\pwake.exe del $(BUILDDIR)\pwake.exe
	@if EXIST $(BUILDDIR)\pcomm.exe del $(BUILDDIR)\pcomm.exe
	@if EXIST $(BUILDDIR)\posload.exe del $(BUILDDIR)\posload.exe
	@if EXIST $(BUILDDIR)\pbload.exe del $(BUILDDIR)\pbload.exe
	@if EXIST $(BUILDDIR)\pgas.exe del $(BUILDDIR)\pgas.exe
	@if EXIST $(BUILDDIR)\ptest.exe del $(BUILDDIR)\ptest.exe
	@echo \VIDEO\TOOLS\PUTIL CLOBBER DONE
#
# What to do for a complete rebuild
#
rebuild:	clean
	@echo \VIDEO\TOOLS\PUTIL REBUILD
	@$(MAKE) --no-print-directory -C lib
	@$(MAKE) --no-print-directory
	@echo \VIDEO\TOOLS\PUTIL REBUILD DONE
	
