#
# Copyright (c) 1997 by Midway Video Inc.
# All rights reserved
#
# Makefile for GOOSE Operating system library for Phoneix systems
#
# $Revision: 14 $
#
ifeq	($(TARGET_SYS),VEGAS)
CF_FILE=	vgoose.cf
NAME_PREFIX=	v
ifeq	($(GRX_HARDWARE),BANSHEE)
CF_FILE=	bgoose.cf
NAME_PREFIX=	b
else
endif
endif
ifeq	($(TARGET_SYS),SEATTLE)
CF_FILE=	goose.cf
NAME_PREFIX=	s
endif

ifeq	($(BUILDMODE),RELEASE)
NAME_SUFFIX=	r
endif
ifeq	($(BUILDMODE),DEBUG)
NAME_SUFFIX=	d
endif

ifneq	($(TIMING),)
TIME_STATS=		-DTIME_STATS
endif

#
# Assembler command line
#
ifeq	($(DEV_ENV),CYGNUS)
GASP=	mips64vr5000-elf-gasp
AS=	mips64vr5000-elf-as
else
AS=	@asmmips /q /of.rdata /zd /l /oc+
endif


#
# Command line options for archiver
#
ifeq	($(DEV_ENV),CYGNUS)
ARFLAGS= -ru
else
ARFLAGS= /u
endif

#
# Archiver
#
ifeq	($(DEV_ENV),CYGNUS)
AR=	mips64vr5000-elf-ar
else
AR=	redir -o /dev/null psylib
endif


#
# Command line options for C compiler
ifeq	($(DEV_ENV),CYGNUS)
ifeq	($(TARGET_SYS),SEATTLE)
CFLAGS=	$(TIME_STATS) -D$(DEV_ENV) -D$(BUILDMODE) -D$(TARGET_SYS) -mhard-float -D__DJGPP__ -fno-builtin -mips4 -fomit-frame-pointer -g -c -G 0 -mcpu=r5000 -nostdinc -I\video\include -Wall -MD -O
endif
ifeq	($(TARGET_SYS),VEGAS)
ifeq	($(GRX_HARDWARE),VOODOO2)
CFLAGS=	$(TIME_STATS) -D$(DEV_ENV) -D$(BUILDMODE) -D$(TARGET_SYS) -mhard-float -DINIT_DOS -D__DJGPP__ -D__GOOSE__ -DGLIDE_HARDWARE -DGLIDE_LIB -DGLIDE_USE_C_TRISETUP -DCVG -DGLIDE_HW_TRI_SETUP -DUSE_PACKET_FIFO -DGLIDE3 -DGLIDE3_ALPHA -fno-builtin -mips4 -fomit-frame-pointer -g -c -G 0 -mpcu=r5000 -nostdinc -I\video\include -I\video\include\glide300 -Wall -O
endif
ifeq	($(GRX_HARDWARE),BANSHEE)
CFLAGS=	$(TIME_STATS) -D$(DEV_ENV) -D$(BUILDMODE) -D$(TARGET_SYS) -mhard-float -DINIT_DOS -D__DOS32__ -D__DJGPP__ -D__GOOSE__ -DGLIDE_HARDWARE -DGLIDE_LIB -DGLIDE_USE_C_TRISETUP -DH3 -DH3_B0 -DGLIDE_HW_TRI_SETUP -DGLIDE3 -DGLIDE3_ALPHA -DGLIDE_INIT_HWC -DGLIDE_PACKET3_TRI_SETUP -DHAL_HW -DMAPPL_DPMI -DPORTIO_DIRECT -DUSE_PACKET_FIFO -fno-builtin -mips4 -fomit-frame-pointer -g -c -G 0 -mcpu=r5000 -nostdinc -I\video\include -I\video\include\bglide -Wall -O
endif
endif
else
CFLAGS=	$(TIME_STATS) -D$(GRX_HARDWARE) -D$(BUILDMODE) -D$(TARGET_SYS) @$(CF_FILE) -O
endif


#
# C compiler command lines
#
ifeq	($(DEV_ENV),CYGNUS)
CC3 = 	mips64vr5000-elf-gcc $(CFLAGS)3
CC2 = 	mips64vr5000-elf-gcc $(CFLAGS)2
CC1 = 	mips64vr5000-elf-gcc $(CFLAGS)1
CC0 = 	mips64vr5000-elf-gcc $(CFLAGS)0
else
CC3 = 	@ccmips $(CFLAGS)3
CC2 = 	@ccmips $(CFLAGS)2
CC1 = 	@ccmips $(CFLAGS)1
CC0 = 	@ccmips $(CFLAGS)0
endif

#
# C Sources for VEGAS Version
#
ifeq	($(TARGET_SYS),VEGAS)
CSRCS =	bgnd.c draw.c object.c ostrings.c process.c \
	sincos.c sprites.c texture.c mthread.c fonts.c gfader.c \
	switch.c randper.c prochook.c \
	anim2d.c adjust.c trans.c lockup.c setcmos.c versions.c \
	gext.c textovly.c initg.c

#
# ASM Sources for VEGAS Version
#
ifeq	($(DEV_ENV),CYGNUS)
ASRCS =	cgetcxsp.S cpshell.S
else
ASRCS =	getcxsp.s pshell.s
endif
else
#
# C Sources for SEATTLE Version
#
CSRCS =	bgnd.c crc.c draw.c object.c ostrings.c process.c sema.c \
	sincos.c sprites.c texture.c mthread.c fonts.c gfader.c cmos.c \
	switch.c inthand.c galhand.c randper.c prochook.c div0hand.c \
	anim2d.c adjust.c unimhand.c trans.c lockup.c setcmos.c versions.c \
	initg.c

#
# ASM Sources for SEATTLE Version
#
#ASRCS =	getcxsp.s pshell.s qhook.s intsup.s insthand.s
ifeq	($(DEV_ENV),CYGNUS)
ASRCS =	cgetcxsp.S cpshell.S cintsup.S cinsthan.S
else
ASRCS =	getcxsp.s pshell.s intsup.s insthand.s
endif
endif

#
# Object modules
#
ifeq	($(DEV_ENV),CYGNUS)
OBJS = 	$(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(ASRCS:.S=.o)) $(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(CSRCS:.c=.o))
else
OBJS = 	$(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(ASRCS:.s=.o)) $(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(CSRCS:.c=.o))
endif

DEPS =	$(addprefix $(NAME_PREFIX)$(NAME_SUFFIX)/, $(CSRCS:.c=.d))

#
# Phony target for prompt
#
.PHONY:	$(NAME_PREFIX)$(NAME_SUFFIX) dep

#
# Tell make NOT to delete .o files when done
#
.PRECIOUS:	$(OBJS)

	
$(NAME_PREFIX)$(NAME_SUFFIX):
	@cmdir $(NAME_PREFIX)$(NAME_SUFFIX)
	@$(MAKE) --no-print-directory -f goose.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) dep

dep:	$(DEPS)
	@$(MAKE) --no-print-directory -f goose.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE) /video/lib/$(NAME_PREFIX)goose$(NAME_SUFFIX).a

define NEWLINE


endef
#
# What we are going to build
#
/video/lib/$(NAME_PREFIX)goose$(NAME_SUFFIX).a:	$(OBJS)
	@if EXIST update.bat del update.bat
	$(foreach OBJ_FILE, $?,@echo>>update.bat @$(AR) $(ARFLAGS) $(@) $(OBJ_FILE)$(NEWLINE))
	@if EXIST update.bat update.bat
	@if EXIST update.bat del update.bat



#
# Dependancies and rules for object modules
#

ifeq	($(DEV_ENV),CYGNUS)
$(NAME_PREFIX)$(NAME_SUFFIX)/%.o:	%.S
	@echo $< to $@
	$(CC3) -o $@ $<
else
$(NAME_PREFIX)$(NAME_SUFFIX)/%.o:	%.s
	@echo $< to $@
	$(AS) $<,$@
endif

$(NAME_PREFIX)$(NAME_SUFFIX)/%.o:	%.c
	@echo $< to $@
	$(CC3) -o $@ $<

$(NAME_PREFIX)$(NAME_SUFFIX)/%.d:	%.c
	@echo $< to $@
	@mkdephdr $(NAME_PREFIX)$(NAME_SUFFIX)/ > $(NAME_PREFIX)$(NAME_SUFFIX)\$(NAME_PREFIX)$(NAME_SUFFIX).dh
	$(CC3) -M -o $(NAME_PREFIX)$(NAME_SUFFIX)/tmp.d $<
	@copy $(NAME_PREFIX)$(NAME_SUFFIX)\$(NAME_PREFIX)$(NAME_SUFFIX).dh+$(NAME_PREFIX)$(NAME_SUFFIX)\tmp.d $(NAME_PREFIX)$(NAME_SUFFIX)\$(notdir $@) > NUL:
	@del $(NAME_PREFIX)$(NAME_SUFFIX)\*.dh
	@del $(NAME_PREFIX)$(NAME_SUFFIX)\tmp.d


#
# Special rules for some modules
#
$(NAME_PREFIX)$(NAME_SUFFIX)/mthread.o:	mthread.c
	@echo $< to $@
	$(CC0) -o $@ $<

$(NAME_PREFIX)$(NAME_SUFFIX)/process.o:	process.c
	@echo $< to $@
	$(CC0) -o $@ $<


$(NAME_PREFIX)$(NAME_SUFFIX)/initg.o:	initg.c
	@echo $< to $@
	$(CC1) -o $@ $<


clobber:
	@if EXIST \video\lib\$(NAME_PREFIX)goose$(NAME_SUFFIX).a del \video\lib\$(NAME_PREFIX)goose$(NAME_SUFFIX).a


clean:	clobber
	@-deltree /y $(NAME_PREFIX)$(NAME_SUFFIX)


rebuild:	clean
	@$(MAKE) --no-print-directory -f goose.mak TARGET_SYS=$(TARGET_SYS) BUILDMODE=$(BUILDMODE) GRX_HARDWARE=$(GRX_HARDWARE)


#
# Dependency files
#
ifneq	($(wildcard $(NAME_PREFIX)$(NAME_SUFFIX)/*.d),)
include	$(wildcard $(NAME_PREFIX)$(NAME_SUFFIX)/*.d)
endif
