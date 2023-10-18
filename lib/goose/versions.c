//
// versions.c
//
// $Revision: 1 $
//
#define	VERSIONS
#include	<goose/goose.h>

static char	goose_versions_c_version[] = {"$Revision: 1 $"};

extern char goose_adjust_c_version[];
extern char goose_anim2d_c_version[];
extern char goose_bgnd_c_version[];
extern char goose_cmos_c_version[];
extern char goose_crc_c_version[];
extern char goose_div0hand_c_version[];
extern char goose_draw_c_version[];
extern char goose_fonts_c_version[];
extern char goose_galhand_c_version[];
extern char goose_gfader_c_version[];
extern char goose_inthand_c_version[];
extern char goose_lockup_c_version[];
extern char goose_mthread_c_version[];
extern char goose_object_c_version[];
extern char goose_ostrings_c_version[];
extern char goose_process_c_version[];
extern char goose_prochook_c_version[];
extern char goose_randper_c_version[];
extern char goose_sema_c_version[];
extern char goose_setcmos_c_version[];
extern char goose_sincos_c_version[];
extern char goose_sprites_c_version[];
extern char goose_switch_c_version[];
extern char goose_texture_c_version[];
extern char goose_trans_c_version[];
extern char goose_unimhand_c_version[];

extern char	goose_getcxsp_s_version[];
extern char	goose_insthand_s_version[];
extern char	goose_intsup_s_version[];
extern char	goose_pshell_s_version[];
extern char	goose_qhook_s_version[];

extern char	goose_goose_h_version[];
extern char	goose_sound_h_version[];
extern char	goose_process_h_version[];
extern char	goose_texture_h_version[];
extern char	goose_object_h_version[];
extern char	goose_sprites_h_version[];
extern char	goose_bgnd_h_version[];
extern char	goose_colors_h_version[];
extern char	goose_pmath_h_version[];
extern char	goose_fonts_h_version[];
extern char	goose_ostrings_h_version[];
extern char	goose_sema_h_version[];
extern char	goose_gfader_h_version[];
extern char	goose_cmos_h_version[];
extern char	goose_switch_h_version[];
extern char	goose_jmalloc_h_version[];
extern char	goose_anim2d_h_version[];
extern char	goose_adjust_h_version[];
extern char	goose_trans_h_version[];
extern char	goose_lockup_h_version[];

static versions_info_t	goose_module_versions[] = {
{"ADJUST.C", goose_adjust_c_version},
{"ANIM2D.C", goose_anim2d_c_version},
{"BGND.C", goose_bgnd_c_version},
{"CMOS.C", goose_cmos_c_version},
{"CRC.C", goose_crc_c_version},
{"DIV0HAND.C", goose_div0hand_c_version},
{"DRAW.C", goose_draw_c_version},
{"FONTS.C", goose_fonts_c_version},
{"GALHAND.C", goose_galhand_c_version},
{"GFADER.C", goose_gfader_c_version},
{"INTHAND.C", goose_inthand_c_version},
{"LOCKUP.C", goose_lockup_c_version},
{"MTHREAD.C", goose_mthread_c_version},
{"OBJECT.C", goose_object_c_version},
{"OSTRINGS.C", goose_ostrings_c_version},
{"PROCESS.C", goose_process_c_version},
{"PROCHOOK.C", goose_prochook_c_version},
{"RANDPER.C", goose_randper_c_version},
{"SEMA.C", goose_sema_c_version},
{"SETCMOS.C", goose_setcmos_c_version},
{"SINCOS.C", goose_sincos_c_version},
{"SPRITES.C", goose_sprites_c_version},
{"SWITCH.C", goose_switch_c_version},
{"TEXTURE.C", goose_texture_c_version},
{"TRANS.C", goose_trans_c_version},
{"UNIMHAND.C", goose_unimhand_c_version},
{"VERSIONS.C", goose_versions_c_version},

{"GETCXSP.S", goose_getcxsp_s_version},
{"INSTHAND.S", goose_insthand_s_version},
{"INTSUP.S", goose_intsup_s_version},
{"PSHELL.S", goose_pshell_s_version},
{"QHOOK.S", goose_qhook_s_version},

{"ADJUST.H", goose_adjust_h_version},
{"ANIM2D.H", goose_anim2d_h_version},
{"BGND.H", goose_bgnd_h_version},
{"CMOS.H", goose_cmos_h_version},
{"COLORS.H", goose_colors_h_version},
{"FONTS.H", goose_fonts_h_version},
{"GFADER.H", goose_gfader_h_version},
{"GOOSE.H", goose_goose_h_version},
{"JMALLOC.H", goose_jmalloc_h_version},
{"LOCKUP.H", goose_lockup_h_version},
{"OBJECT.H", goose_object_h_version},
{"OSTRINGS.H", goose_ostrings_h_version},
{"PMATH.H", goose_pmath_h_version},
{"PROCESS.H", goose_process_h_version},
{"SEMA.H", goose_sema_h_version},
{"SOUND.H", goose_sound_h_version},
{"SPRITES.H", goose_sprites_h_version},
{"SWITCH.H", goose_switch_h_version},
{"TEXTURE.H", goose_texture_h_version},
{"TRANS.H", goose_trans_h_version},

{NULL, NULL}
};

versions_info_t *get_goose_module_versions(void)
{
	return(goose_module_versions);
}

char *get_goose_library_mode(void)
{
#ifdef DEBUG
	return("DEBUG");
#else
	return("RELEASE");
#endif
}

