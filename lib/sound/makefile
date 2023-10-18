#
# Systems for which game sound library can be built
#
SYSTEMS=	VEGAS.SYS SEATTLE.SYS

#
# Modes for which game sound library can be built
#
MODES=		RELEASE.MOD DEBUG.MOD

#
# If the TARGET_SYS envioronment variable is set, build only the sound library
# for that system.
#
ifeq	($(TARGET_SYS),VEGAS)
SYSTEMS=	VEGAS.SYS
endif

ifeq	($(TARGET_SYS),SEATTLE)
SYSTEMS=	SEATTLE.SYS
endif


#
# IF the BUILDMODE environment variable is set, build only the sound library
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
all:	$(SYSTEMS)

#
# Rule for building sound library for different systems
#
%.SYS:
	@$(MAKE) --no-print-directory TARGET_SYS=$(basename $@) modes


#
# Mode target
#
modes:	$(MODES)

#
# Rule for building sound library for different modes
#
%.MOD:
	@echo $(basename $@) VERSION OF SOUND LIBRARY FOR $(TARGET_SYS)
	@$(MAKE) --no-print-directory TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(basename $@) -f sound.mak
	@echo $(basename $@) VERSION OF SOUND LIBRARY FOR $(TARGET_SYS) DONE

#
# How to clean up for a relink
#
clobber:
	@echo \VIDEO\LIB\SOUND CLOBBER
ifneq	($(wildcard /video/lib/*sound*.a),)
	@-del \video\lib\*sound*.a
endif
	@echo \VIDEO\LIB\SOUND CLOBBER DONE


#
# How to clean up for a recompile and relink
#
clean:	clobber
	@echo \VIDEO\LIB\SOUND CLEAN
	@-deltree /y sd > NUL:
	@-deltree /y sr > NUL:
	@-deltree /y vd > NUL:
	@-deltree /y vr > NUL:
	@echo \VIDEO\LIB\SOUND CLEAN DONE

#
# How to rebuild the sound library
#
rebuild:	clean
	@echo \VIDEO\LIB\SOUND REBUILD
	@$(MAKE) --no-print-directory
	@echo \VIDEO\LIB\SOUND REBUILD DONE
