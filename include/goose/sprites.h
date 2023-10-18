/* $Revision: 5 $ */

#ifndef	__SPRITES_H__
#define	__SPRITES_H__

#ifdef VERSIONS
char	goose_sprites_h_version[] = {"$Revision: 5 $"};
#endif

#ifndef __GLIDE_H__
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#endif

#ifdef VEGAS
typedef struct
{
	float	x;
	float	y;
	float	oow;
	float	sow;
	float	tow;
	int	pad;
} MidVertex;

typedef struct
{
	float	x;
	float	y;
	float	oow;
	float	sow;
	float	tow;
} SpriteVertex;

void grDrawSprite(const float *);
#endif


typedef struct state_info {
	unsigned char			color_combiner_function;
	unsigned char			texture_combiner_function;
	unsigned char			texture_filter_mode;
	unsigned char			texture_clamp_mode;
	unsigned char			chroma_key_mode;
#if (!(MULTIPART_IMAGES & 1))
	int			chroma_color;
#endif
	unsigned char			alpha_source;
	unsigned char			alpha_rgb_src_function;
	unsigned char			alpha_rgb_dst_function;
	unsigned char			alpha_a_src_function;
	unsigned char			alpha_a_dst_function;
	unsigned char			cull_mode;
	int			constant_color;
} state_info_t;

typedef struct image_info {
	char	*texture_name;
	float	w;
	float	h;
	float	ax;
	float	ay;
	float	s_start;
	float	s_end;
	float	t_start;
	float	t_end;
#if (MULTIPART_IMAGES & 1)
	struct image_info	*next;
	int	chroma_color;
#endif
} image_info_t;

typedef struct sprite_info {
	void						*parent;			/* Pointer of object that created this */
	int						id;				/* User specified id */
	int						mode;				/* Mode for the sprite */
	struct texture_node	*tn;				/* Texture node for sprite */
	void						*node_ptr;		/* Pointer to this sprites list node */
	void						*vert_mem;		/* Allocated memory for vertices */
#if defined(SEATTLE)
	MidVertex				*verts;			/* Cache aligned vertex array */
#elif defined(VEGAS)
	SpriteVertex			*verts;
#else
#error Environment variable TARGET_SYS not set
#endif
	float						tile_x;
	float						tile_y;
	float						x;					/* Screen X Position */
	float						y;					/* Screen Y Position */
	float						z;					/* Screen Z Position */
	float						x_end;			/* Ending X position */
	float						y_end;			/* Ending Y position */
	float						z_end;			/* Ending Z posotion */
	float						x_angle;			/* X rotation angle */
	float						y_angle;			/* Y rotation angle */
	float						z_angle;			/* Z rotation angle */
	float						x_angle_end;	/* Ending X rotation angle */
	float						y_angle_end;	/* Ending Y rotation angle */
	float						z_angle_end;	/* Ending Z rotation angle */
	float						w_scale;			/* Width Scale */
	float						h_scale;			/* Height Scale */
	float						w_scale_end;	/* Ending width scale */
	float						h_scale_end;	/* Ending height scale */
	float						x_vel;			/* X velocity */
	float						y_vel;			/* Y velocity */
	float						z_vel;			/* Z velocity */
	float						x_ang_vel;		/* X rotation velocity */
	float						y_ang_vel;		/* Y rotation velocity */
	float						z_ang_vel;		/* Z rotation velocity */
	float						w_scale_vel;	/* Width Scale velocity */
	float						h_scale_vel;	/* Height Scale veloctiy */
	int						notify_modes;	/* Notification modes */
	void						(*notify_func)(struct sprite_info *, int);
#if (MULTIPART_IMAGES & 1)
	struct sprite_info	*mp_next;		/* Next part of multipart sprite */
#else
	int						chroma_color;	/* Color 0 */
#endif
	struct image_info		*ii;				/* Image Header */
	state_info_t			state;			/* Graphics state for this sprite */
} sprite_info_t;

typedef struct sprite_node {
	struct sprite_node	*next;		/* Next node in list */
	struct sprite_node	*prev;		/* Previous node in list */
	sprite_info_t			*si;			/* Sprite info for this node */
	int						mode;			/* Mode for node (see below) */
} sprite_node_t;


/* Sprite Node Mode definitions */
#define	DO_VEL_ADD	1		// Do the velocity add for this sprite
#define	HIDE_SPRITE	2		// Don't draw the sprite

/* Sprite Modes */
#define	IT_ALPHA		1		// Sprite uses iterated alpha
#define	SHADED		2		// Sprite uses iterated rgb
#define	TEXTURED		4		// Sprite is textured
#define	FLIP_TEX_H	8		// Flip texture horizontally on geometry
#define	FLIP_TEX_V	16		// Flip texture vertically on geometry
#define	ALPHA_MODE	32		// Sprite is on alpha list
#define	PER_PROJECT	64		// Perform perspective projection
#define	ROTATED		128	// Sprite is rotated
#define	ANIM_2D		256	// Sprite is in the 2d animation list, PARENT_NODE points to anim info

/* Notification Modes */
#define		NOTIFY_X_POS	1
#define		NOTIFY_Y_POS	2
#define		NOTIFY_Z_POS	4
#define		NOTIFY_X_ROT	8
#define		NOTIFY_Y_ROT	16
#define		NOTIFY_Z_ROT	32
#define		NOTIFY_W_SCALE	64
#define		NOTIFY_H_SCALE	128

/* Function prototypes */
void generate_sprite_verts(sprite_info_t *);
sprite_info_t *_beginobj(void *, image_info_t *, float, float, float, int);
sprite_info_t *beginobj(image_info_t *, float, float, float, int tid);
void delobj(sprite_info_t *);
void del1c(int, int);
void delete_all_sprites(void);
void change_img(sprite_info_t *, image_info_t *, int);
void del_child_obj(void *);
void hide_sprite(sprite_info_t *sprite);
void hide_sprites(void);
void unhide_sprite(sprite_info_t *sprite);
void unhide_sprites(void);
void set_veladd_mode(sprite_info_t *sprite, int mode);
void _generate_sprite_st(register sprite_info_t *sprite);
void init_sprites(void);
void change_sprite_alpha_state(sprite_info_t *sprite, int mode);
int sprite_exists( sprite_info_t * );
#endif
