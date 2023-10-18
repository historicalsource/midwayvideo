/* $Revision: 8 $ */
#ifndef	__OSTRINGS_H__
#define	__OSTRINGS_H__

#ifdef VERSIONS
char	goose_ostrings_h_version[] = {"$Revision: 8 $"};
#endif

#ifndef	__FONTS_H__
#include	<goose/fonts.h>
#endif

typedef struct text_state_info {
	float	x;
	float	y;
	float	z;
	float	h_scale;
	float	w_scale;
	int	color;
	int	bgnd_color;
	int	id;
	int	transparency_mode;
	int	justification_mode;
	int	alpha_mode;
	int	constant_alpha;
	int	drop_shadow;
	font_node_t	*font_node;
} text_state_info_t;

#define	HJUST_MODE_MASK	0x3
#define	VJUST_MODE_MASK	0xc

#define	HJUST_LEFT	0x0
#define	HJUST_RIGHT	0x1
#define	HJUST_CENTER	0x2

#define	VJUST_BOTTOM	(0x0<<2)
#define	VJUST_TOP	(0x1<<2)
#define	VJUST_CENTER	(0x2<<2)

#define	TRANSPARENCY_ENABLE	1
#define	TRANSPARENCY_DISABLE	0

#define	ALPHA_ENABLE	1
#define	ALPHA_DISABLE	0

#define  TEXT_SHAD_ON     1
#define  TEXT_SHAD_LEFT   2
#define  TEXT_SHAD_RIGHT  0
#define  TEXT_SHAD_UP  	  4
#define  TEXT_SHAD_DOWN   0

typedef struct ostring {
	struct ostring		*next;
	struct ostring		*prev;
	float					width;
	int					id;
	char					string[128];
	text_state_info_t	state;
} ostring_t;

/* Definitions of modes for set_string_shading */
#define	STR_SHADE_CHARACTERS	0
#define	STR_SHADE_LINE			1


/* Function prototypes */
void get_text_state(text_state_info_t *);
void set_text_state(text_state_info_t *);
void set_text_position(int, int, float);
void set_text_font(int);
void set_text_color(int);
void set_text_bgnd_color(int);
void set_text_transparency_mode(int);
void set_text_justification_mode(int);
void set_text_scales(float, float);
void set_text_id(int);

ostring_t *oputs(char *);
void set_string_id(int);
void delete_string(ostring_t *);
void delete_string_id(int);
void delete_multiple_strings(int, int);
void delete_all_strings(void);
void change_string_state(ostring_t *);
void set_string_shading(ostring_t *os, int mode, float *argb);
ostring_t *oprintf(char *, ...) __attribute__ ((format (printf, 1, 2)));
int get_string_width(char *);
void set_text_z_inc(float);
void fade_string(ostring_t *os, int alpha);
void set_text_drop_shadow(int drop_flag);
void change_string_cc( void *, int );

#endif
