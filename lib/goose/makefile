ifneq	($(TARGET_SYS),VEGAS)
ifneq	($(TARGET_SYS),SEATTLE)
.PHONY:	all

all:
	@$(MAKE) --no-print-directory TARGET_SYS=SEATTLE
	@$(MAKE) --no-print-directory TARGET_SYS=VEGAS
endif
endif


#
# Systems for which game goose library can be built
#
SYSTEMS=	VEGAS.SYS SEATTLE.SYS

#
# Modes for which game goose library can be built
#
MODES=		RELEASE.MOD DEBUG.MOD

#
# If the TARGET_SYS envioronment variable is set, build only the goose library
# for that system.
#
ifeq	($(TARGET_SYS),VEGAS)
SYSTEMS=	VEGAS.SYS
GRAPHICS=	VOODOO2.GRX BANSHEE.GRX
ifeq	($(GRX_HARDWARE),VOODOO2)
GRAPHICS=	VOODOO2.GRX
endif
ifeq	($(GRX_HARDWARE),BANSHEE)
GRAPHICS=	BANSHEE.GRX
endif
endif

ifeq	($(TARGET_SYS),SEATTLE)
SYSTEMS=	SEATTLE.SYS
GRAPHICS=	VOODOO1.GRX
endif


#
# IF the BUILDMODE environment variable is set, build only the goose library
# in the specified mode
#
ifeq	($(BUILDMODE),DEBUG)
MODES=	DEBUG.MOD
endif

ifeq	($(BUILDMODE),RELEASE)
MODES=	RELEASE.MOD
endif


#
# Default target
#
all:	$(GRAPHICS)

%.GRX:
	@$(MAKE) --no-print-directory GRX_HARDWARE=$(basename $@) systems

#
# System targets
#
systems:	$(SYSTEMS)


#
# Rule for building goose library for different systems
#
%.SYS:
	@$(MAKE) --no-print-directory TARGET_SYS=$(basename $@) GRX_HARDWARE=$(GRX_HARDWARE) modes


#
# Mode target
#
modes:	$(MODES)

#
# Rule for building goose library for different modes
#
%.MOD:
	@echo $(basename $@) VERSION OF GOOSE LIBRARY FOR $(GRX_HARDWARE) ON $(TARGET_SYS)
	@$(MAKE) --no-print-directory TARGET_SYS=$(TARGET_SYS) GRX_HARDWARE=$(GRX_HARDWARE) BUILDMODE=$(basename $@) -f goose.mak
	@echo $(basename $@) VERSION OF GOOSE LIBRARY FOR $(GRX_HARDWARE) ON $(TARGET_SYS) DONE


#
# How to clean up for a relink
#
clobber:
	@echo \VIDEO\LIB\GOOSE CLOBBER
	@-del \video\lib\*goose*.a
	@echo \VIDEO\LIB\GOOSE CLOBBER DONE


#
# How to clean up for a recompile and relink
#
clean:	clobber
	@echo \VIDEO\LIB\GOOSE CLEAN
	@-deltree /y sd > NUL:
	@-deltree /y sr > NUL:
	@-deltree /y vd > NUL:
	@-deltree /y vr > NUL:
	@-deltree /y bd > NUL:
	@-deltree /y br > NUL:
	@echo \VIDEO\LIB\GOOSE CLEAN DONE

#
# How to rebuild the goose library
#
rebuild:	clean
	@echo \VIDEO\LIB\GOOSE REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\LIB\GOOSE REBUILD DONE
