#
# Copyright (c) 1997 by Midway Video Inc.
# All rights reserved
#
# Makefile for SOUND Operating system library for Phoneix systems
#
# $Revision: 7 $
#
CF_FILE=	sound.cf

ifeq	($(TARGET_SYS),VEGAS)
NAME_PREFIX=	v
endif
ifeq	($(TARGET_SYS),SEATTLE)
NAME_PREFIX=	s
endif

ifeq	($(BUILDMODE),RELEASE)
NAME_SUFFIX=	r
endif
ifeq	($(BUILDMODE),DEBUG)
NAME_SUFFIX=	d
endif

#
# Assembler command line
#
AS=	@asmmips /q /of.rdata /zd /l /oc+

#
# Command line options for archiver
#
ARFLAGS= /u

#
# Archiver
#
AR=	@redir -o /dev/null psylib


#
# Command line options for C compiler
#
CFLAGS=	-D$(BUILDMODE) -D$(TARGET_SYS) @$(CF_FILE) -O

#
# C compiler command lines
#
CC3 = 	@ccmips $(CFLAGS)3
CC2 = 	@ccmips $(CFLAGS)2
CC1 = 	@ccmips $(CFLAGS)1
CC0 = 	@ccmips $(CFLAGS)0


#
# C Sources for VEGAS Version
#
ifeq	($(BUILDMODE),DEBUG)
CSRCS =	sound.c versions.c dcs_shel.c
endif

ifeq	($(BUILDMODE),RELEASE)
CSRCS =	sound.c versions.c 
endif

#
# ASM Sources for VEGAS Version
#
ASRCS =

#
# Object modules
#
OBJS = 	$(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(CSRCS:.c=.o)) $(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(ASRCS:.c=.o))

#
# Dependency files
#
DEPS =	$(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(CSRCS:.c=.d))

#
# Phoney targets
#
.PHONY:	$(NAME_PREFIX)$(NAME_SUFFIX) dep

#
# Keep the .o's around
#
.PRECIOUS:	$(OBJS)


#
# Default target
#
$(NAME_PREFIX)$(NAME_SUFFIX):
	@cmdir $(NAME_PREFIX)$(NAME_SUFFIX)
	@$(MAKE) --no-print-directory -f sound.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) dep

#
# Dendencies target
#
dep:	$(DEPS)
	@$(MAKE) --no-print-directory -f sound.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) /video/lib/$(NAME_PREFIX)sound$(NAME_SUFFIX).a

define NEWLINE


endef
#
# Archive target
#
/video/lib/$(NAME_PREFIX)sound$(NAME_SUFFIX).a:	$(OBJS)
	@if EXIST update.bat del update.bat
	$(foreach OBJ_FILE, $?,@echo>>update.bat $(AR) $(ARFLAGS) $(@) $(OBJ_FILE)$(NEWLINE))
	@if EXIST update.bat update.bat
	@if EXIST update.bat del update.bat


#
# Dependancies and rules for object modules
#
$(NAME_PREFIX)$(NAME_SUFFIX)/%.o:	%.s
	@echo $< to $@
	$(AS) $<,$@

$(NAME_PREFIX)$(NAME_SUFFIX)/%.o:	%.c
	@echo $< to $@
	$(CC3) -o $@ $<

$(NAME_PREFIX)$(NAME_SUFFIX)/%.d:	%.c
	@echo $< to $@
	@mkdephdr $(NAME_PREFIX)$(NAME_SUFFIX)/ > $(NAME_PREFIX)$(NAME_SUFFIX)\$(NAME_PREFIX)$(NAME_SUFFIX).dh
	@$(CC3) -M -o $(NAME_PREFIX)$(NAME_SUFFIX)/tmp.d $<
	@copy $(NAME_PREFIX)$(NAME_SUFFIX)\$(NAME_PREFIX)$(NAME_SUFFIX).dh+$(NAME_PREFIX)$(NAME_SUFFIX)\tmp.d $(NAME_PREFIX)$(NAME_SUFFIX)\$(notdir $@) > NUL:
	@del $(NAME_PREFIX)$(NAME_SUFFIX)\*.dh
	@del $(NAME_PREFIX)$(NAME_SUFFIX)\tmp.d

#
# Special rules for some modules
#
$(NAME_PREFIX)$(NAME_SUFFIX)/sound.o:	sound.c
	@echo $< to $@
	$(CC2) -o $@ $<
	
#
# Dependency files
#
ifneq	($(wildcard $(NAME_PREFIX)$(NAME_SUFFIX)/*.d),)
include	$(wildcard $(NAME_PREFIX)$(NAME_SUFFIX)/*.d)
endif
