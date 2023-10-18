#ifndef	__GOOSE_H__
#define	__GOOSE_H__

#ifdef VERSIONS
char	goose_goose_h_version[] = {"$Revision: 23 $"};
#endif

#include	<goose/sound.h>
#include	<goose/process.h>
#include	<goose/texture.h>
#include	<goose/object.h>
#include	<goose/sprites.h>
#include	<goose/bgnd.h>
#include	<goose/colors.h>
#include	<goose/pmath.h>
#include	<goose/fonts.h>
#include	<goose/ostrings.h>
#include	<goose/sema.h>
#include	<goose/gfader.h>
#include	<goose/cmos.h>
#include	<goose/switch.h>
#include	<goose/jmalloc.h>
#include <goose/anim2d.h>
#include	<goose/adjust.h>
#include	<goose/trans.h>
#include	<goose/lockup.h>

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

int randper(int);
//void enable_ip(int);
void	install_handler(int, int (*)(int, int *));
void	install_sys_handler(int, void (*)(void));
void	install_interrupt_handler(int, int (*)(void));
void	install_fpu_handler(int, void (*)(int *));
void	install_disk_info(void **, void (*)(void), void (*)(process_node_t *));
void	install_disk_callback(void (*)(int));
void	div0_handler(int *);
void unimplemented_handler(int *);
int	pre_dma_proc_func(void (*)(void));
int	pre_proc_func(void (*)(void));
int	post_proc_func(void (*)(void));
int	randrng(int);
int	exec(char *, int, int *);
char	*get_platform_id(void);
void	draw_enable(int);
void mthread_enable_framerate(void);
void mthread_disable_framerate(void);
int  mthread_get_framerate(void);
int	is_revision_a(void);
unsigned long crc(unsigned char *data, int len);

typedef enum {
RESOLUTION_400x256,
RESOLUTION_512x256,
RESOLUTION_512x384,
RESOLUTION_NONE,
RESOLUTION_DISABLE = RESOLUTION_NONE
} resolution_t;

typedef enum {
REFRESH_55HZ,
REFRESH_56HZ,
REFRESH_57HZ,
REFRESH_58HZ,
REFRESH_59HZ,
REFRESH_60HZ,
} refresh_t;

typedef struct goose_init_info
{
	resolution_t			resolution[4];		// Resolutions 0 is default
#if ! defined(SEATTLE)
	refresh_t				refresh;				// Refresh rate
#endif
	char						*name;		 		// Name of the game
	char						*version;	 		// Version of the game
	char						*build_date; 		// Build date of the game
	char						*build_time; 		// Build time of the game
	int						num_game_ids;		// Number of game ids
	int						*game_id;			// Array of game ids
	setup_cmos_struct2_t	*sc;					// CMOS setup info (NULL = None)
	int						snd_test_bypass;	// Bypass the sound tests
	int						allow_49way;		// Setup for 49 way joysticks
	void						(*mproc)(int *);	// Pointer to main proc
	void						(*minit)(void);	// Pointer to user init function
#if defined(SEATTLE)
	int						dma_buffer_size;	// Seattle only
#endif
	int						low_res_scale;		// Scale for low resolutions
} goose_init_info_t;

void init_text_overlay(int mode);
void draw_text_overlay(void);
void to_printf(char *fmt, ...);
int get_to_rows(void);
int get_to_columns(void);
void goose_init(goose_init_info_t *);

#ifdef VEGAS
int get_tsec(void);
void grTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t tc);
void guColorCombineFunction(GrColorCombineFnc_t fnc );
void guAlphaSource(GrAlphaSource_t mode);
//void grDrawSimpleTriangle(MidVertex *, MidVertex *, MidVertex *);
//void grDrawTriangleDma(MidVertex *, MidVertex *, MidVertex *, int);

void grDrawSimpleTriangle(const void *, const void *, const void *);
void grDrawTriangleDma(const void *, const void *, const void *, int);

void guTexChangeLodBias(Texture_node_t *, float lod_bias);
int guTexChangeAttributes(Texture_node_t *tex,
									int width,
									int height,
									GrTextureFormat_t	fmt,
									GrMipMapMode_t		mm_mode,
									GrLOD_t	smallest_lod,
									GrLOD_t	largest_lod,
									GrAspectRatio_t	aspect,
									GrTextureClampMode_t	s_clamp_mode,
									GrTextureClampMode_t	t_clamp_mode,
									GrTextureFilterMode_t	minFilterMode,
									GrTextureFilterMode_t	magFilterMode);
#endif

/* defined in sprite.c */
extern int is_low_res;
/* defined in glide\getenv.c */
extern float hres, vres;
extern int sst1InitGrxClk_Called;
/* defined in process.c */
extern struct process_node *cur_proc;
/* defined in galhand.c */
void galileo_dma0_handler(void);
void galileo_timer3_handler(void);
void start_timeout_timer(void);
void stop_timeout_timer(void);
int get_timeout_time(void);
/* defined in inthand.c */
int interrupt_handler(void);
/* defined in mthread.c */
void process_dispatch(void);

// Offsets to the various registers int the register save area

// CPU general purpose registers
#define	GP0	0
#define	GP1	2
#define	GP2	4
#define	GP3	6
#define	GP4	8
#define	GP5	10
#define	GP6	12
#define	GP7	14
#define	GP8	16
#define	GP9	18
#define	GP10	20
#define	GP11	22
#define	GP12	24
#define	GP13	26
#define	GP14	28
#define	GP15	30
#define	GP16	32
#define	GP17	34
#define	GP18	36
#define	GP19	38
#define	GP20	40
#define	GP21	42
#define	GP22	44
#define	GP23	46
#define	GP24	48
#define	GP25	50
#define	GP26	52
#define	GP27	54
#define	GP28	56
#define	GP29	58
#define	GP30	60
#define	GP31	62


// Integer multiply/divide registers
#define	MFLO	64
#define	MFHI	66

// The program counter
#define	PC		68

// Coprocessor 0 registers
#define	CPO_INDEX		70
#define	CP0_RANDOM		72
#define	CP0_ENTRYLO0	74
#define	CP0_ENTRYHI1	76
#define	CP0_CONTEXT		78
#define	CPO_PAGEMASK	80
#define	CP0_WIRED		82
#define	CP0_7				84
#define	CP0_BADVADDR	86
#define	CPO_COUNT		88
#define	CP0_ENTRYHI		90
#define	CPO_COMPARE		92
#define	CP0_STATUS		94
#define	CP0_CAUSE		96
#define	CP0_EPC			98
#define	CP0_PRID			100
#define	CP0_CONFIG		102
#define	CP0_LLADDR		104
#define	CP0_WATCHLO		106
#define	CP0_WATCHHI		108
#define	CPO_20			110
#define	CP0_21			112
#define	CP0_22			114
#define	CP0_23			116
#define	CPO_24			118
#define	CP0_25			120
#define	CP0_ECC			122
#define	CP0_CACHEERR	124
#define	CP0_TAGLO		126
#define	CP0_TAGHI		128
#define	CP0_ERRORPC		130
#define	CP0_31			132


// Coprocessor 1 (FPU) general purpose register
#define	FGR0				134
#define	FGR1				136
#define	FGR2				138
#define	FGR3				140
#define	FGR4				142
#define	FGR5				144
#define	FGR6				146
#define	FGR7				148
#define	FGR8				150
#define	FGR9				152
#define	FGR10				154
#define	FGR11				156
#define	FGR12				158
#define	FGR13				160
#define	FGR14				162
#define	FGR15				164
#define	FGR16				166
#define	FGR17				168
#define	FGR18				170
#define	FGR19				172
#define	FGR20				174
#define	FGR21				176
#define	FGR22				178
#define	FGR23				180
#define	FGR24				182
#define	FGR25				184
#define	FGR26				186
#define	FGR27				188
#define	FGR28				190
#define	FGR29				192
#define	FGR30				194
#define	FGR31				196

// FPU Control and Status Registers
#define	FCR0				198
#define	FCR31				199


// FPU Exception handler numbers
#define	FPU_EXC_INEXACT	0
#define	FPU_EXC_UNDERFLOW	1
#define	FPU_EXC_OVERFLOW	2
#define	FPU_EXC_DIV0		3
#define	FPU_EXC_INVALID	4
#define	FPU_EXC_UNIMP		5

#define	VERTICAL_RETRACE_HANDLER	0
#define	SYSTEM_CONTROL_HANDLER		1
#define	IDE_DISK_HANDLER				2
#define	IOASIC_HANDLER					3
#define	NSS_HANDLER						4
#define	HILINK_HANDLER					NSS_HANDLER
#define	WIDGET_HANDLER					NSS_HANDLER

typedef struct versions_info
{
	char	*file_name;
	char	*version_str;
} versions_info_t;

versions_info_t *get_goose_module_versions(void);
char *get_goose_library_mode(void);
versions_info_t *get_glide_module_versions(void);
char	*get_glide_library_mode(void);
versions_info_t *get_sound_module_versions(void);
char	*get_sound_library_mode(void);
versions_info_t *get_rom_module_versions(void);
char	*get_rom_library_mode(void);


// Resource control Modes
#define	NO_FREE_MEMORY	0			// Default
#define	FREE_RESOURCES	1			// Free memory resources

void set_resource_control(int mode);

void clear_free_process_list(void);
void clear_free_sprite_list(void);
void clear_free_object_list(void);
void clear_free_texture_list(void);

void clear_free_lists(void);

void install_process_stall_func(void (*)(void));

#if defined(DEBUG)
#define	DEBUG_LEVEL0	0
#define	DEBUG_LEVEL1	1
#define	DEBUG_LEVEL2	2
#define	DEBUG_LEVEL3	3
#define	DEBUG_LEVEL4	4
#define	DEBUG_LEVEL5	5
#define	DEBUG_LEVEL6	6
#define	DEBUG_LEVEL7	7
#define	DEBUG_LEVEL8	8
#define	DEBUG_LEVEL9	9
#define	DEBUG_LEVEL10	10

#define	DEBUG_OFF		-1
#define	DEBUG_MIN		DEBUG_LEVEL0
#define	DEBUG_MAX		DEBUG_LEVEL10

#define	DEBUG_FAILS		DEBUG_LEVEL0
#define	DEBUG_MSGS		DEBUG_LEVEL1
#define	DEBUG_FENTRY	DEBUG_LEVEL2
#define	DEBUG_RETURNS	DEBUG_LEVEL3

#define	PROCESS_DEBUG_LEVEL(a) {extern int __process_debug_level; __process_debug_level = (a); }
#define	SPRITE_DEBUG_LEVEL(a) {extern int __sprite_debug_level; __sprite_debug_level = (a); }
#define	TEXTURE_DEBUG_LEVEL(a) {extern int __texture_debug_level; __texture_debug_level = (a); }
#define	STRING_DEBUG_LEVEL(a) {extern int __string_debug_level; __string_debug_level = (a); }
#define	FONT_DEBUG_LEVEL(a) {extern int __font_debug_level; __font_debug_level = (a); }
#define	OBJECT_DEBUG_LEVEL(a) {extern int __object_debug_level; __object_debug_level = (a); }
#define	TEXTURE_LOAD_DEBUG_LEVEL(a) {extern int __texture_load_debug_level; __texture_load_debug_level = (a); }

#define	DBG_MSG_COMMON(file, line, func, fatal, format, args...) \
	{ \
		fprintf(stderr, "File: %s - Line: %d - Function: %s()\n", (char *)(file), (int)(line), (char *)(func)); \
		fprintf(stderr, format, ##args); \
		if(fatal) \
		{ \
			__asm__("	teqi	$0,0");\
		}\
	}

#define	_TEXTURE_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__texture_debug_level;\
		if(level <= __texture_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_TEXTURE_LOAD_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__texture_load_debug_level;\
		if(level <= __texture_load_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_PROCESS_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__process_debug_level;\
		if(level <= __process_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_SPRITE_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__sprite_debug_level;\
		if(level <= __sprite_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_STRING_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__string_debug_level;\
		if(level <= __string_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_FONT_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__font_debug_level;\
		if(level <= __font_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_OBJECT_DBG_MSG(file, line, func, level, fatal, args...) \
	{ \
		extern int	__object_debug_level;\
		if(level <= __object_debug_level)\
		{\
			DBG_MSG_COMMON(file, line, func, fatal, ##args);\
		}\
	}

#define	_DBG_MSG(file, line, func, format, args...) \
	{ \
		fprintf(stderr, "File: %s - Line: %d - Function: %s()\n", (char *)(file), (int)(line), (char *)(func));\
		fprintf(stderr, format, ##args); \
	}

#else

#define	DEBUG_LEVEL0
#define	DEBUG_LEVEL1
#define	DEBUG_LEVEL2
#define	DEBUG_LEVEL3
#define	DEBUG_LEVEL4
#define	DEBUG_LEVEL5
#define	DEBUG_LEVEL6
#define	DEBUG_LEVEL7
#define	DEBUG_LEVEL8
#define	DEBUG_LEVEL9
#define	DEBUG_LEVEL10

#define	DEBUG_OFF
#define	DEBUG_MIN
#define	DEBUG_MAX

#define	DEBUG_FAILS
#define	DEBUG_MSGS
#define	DEBUG_FENTRY
#define	DEBUG_RETURNS

#define	PROCESS_DEBUG_LEVEL(a)
#define	SPRITE_DEBUG_LEVEL(a)
#define	TEXTURE_DEBUG_LEVEL(a)
#define	STRING_DEBUG_LEVEL(a)
#define	FONT_DEBUG_LEVEL(a)
#define	OBJECT_DEBUG_LEVEL(a)
#define	TEXTURE_LOAD_DEBUG_LEVEL(a)

#define	_TEXTURE_DBG_MSG(file, line, func, level, fatal, args...)
#define	_TEXTURE_LOAD_DBG_MSG(file, line, func, level, fatal, format, args...)
#define	_PROCESS_DBG_MSG(file, line, func, level, fatal, format, args...)
#define	_SPRITE_DBG_MSG(file, line, func, level, fatal, format, args...)
#define	_STRING_DBG_MSG(file, line, func, level, fatal, format, args...)
#define	_FONT_DBG_MSG(file, line, func, level, fatal, format, args...)
#define	_OBJECT_DBG_MSG(file, line, func, level, fatal, format, args...)
#define	_DBG_MSG(file, line, func, format, args...)

#endif

#define	TEXTURE_DBG_MSG(level, fatal, args...) \
	_TEXTURE_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	TEXTURE_LOAD_DBG_MSG(level, fatal, args...) \
	_TEXTURE_LOAD_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	PROCESS_DBG_MSG(level, fatal, args...) \
	_PROCESS_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	SPRITE_DBG_MSG(level, fatal, args...) \
	_SPRITE_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	STRING_DBG_MSG(level, fatal, args...) \
	_STRING_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	FONT_DBG_MSG(level, fatal, args...) \
	_FONT_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	OBJECT_DBG_MSG(level, fatal, args...) \
	_OBJECT_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, (level), (fatal), ##args)

#define	DBG_MSG(args...) \
	_DBG_MSG(__FILE__, __LINE__, __FUNCTION__, ##args)

#define	DEBUG_MSG	DBG_MSG
#define	DEBUG_PRINT	DBG_MSG


void get_platform_name(char *, int);
void get_proc_name(char *, int);
int  get_cpu_speed(void);


#endif
