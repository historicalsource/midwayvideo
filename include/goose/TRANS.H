//
// trans.h - Include file for transition stuff
//
// $Revision: 2 $
//
// $Author: Mlynch $
//
#ifndef	__TRANS_H__
#define	__TRANS_H__

#ifdef VERSIONS
char	goose_trans_h_version[] = {"$Revision: 2 $"};
#endif

#ifndef	__TEXTURE_H__
#include	<goose/texture.h>
#endif

// Definition of a transition piece
typedef struct transition_piece
{
	int			w;				// Width of the piece
	int			h;				// Height of the piece
	int			x;				// Current x position of BL corner
	int			y;				// Current y position of BL corner
	int			x_start;		// Starting x position of BL corner
	int			x_end;		// Ending x positioon of BL corner
	int			y_start;		// Starting y position of BL corner
	int			y_end;		// Ending y position of BL corner
	int			dx;			// X Velocity
	int			dy;			// Y Velocity
	int			make_sound;	// Make sound status
	int			snd_enable;	// Enable sounds for this piece
#if defined(SEATTLE)
	MidVertex	vert[4];		// Veritices for the the piece
#elif defined(VEGAS)
	SpriteVertex	vert[4];
#else
#error Environment variable TARGET_SYS not set
#endif
} transition_piece_t;


// Definition of the transition control structure
typedef struct transition_control
{
	transition_piece_t	*tpiece;				// List of pieces for transition
	void						*tbuffer;			// Buffer where texture is stored
	image_info_t			*image;				// Pointer to image to use
	texture_node_t			*texture;			// Pointer to texture node for
	char						*tex_name;			// Name of loaded texture
	int						num_pieces;			// Number of pieces
	void						(*snd)(void);		// Sound function
	int						direction;			// Transition direction
	int						state;				// Transition state
	int						staggered;			// Staggered transition mode
	int						stagger_count;		// Last to move for stagger
} transition_control_t;

// Transition function numbers
enum transition_fn
{
// Horizontal bars
TRANSITION_FUNCTION_RL_WIPE,				// Right -> Left wipe
TRANSITION_FUNCTION_RL_SLIDE,				// Right -> Left slide
TRANSITION_FUNCTION_LR_WIPE,				// Left -> Right wipe
TRANSITION_FUNCTION_LR_SLIDE,				// Left -> Right slide
TRANSITION_FUNCTION_H_INTERLEAVE,		// Horizontal bar interleave
TRANSITION_FUNCTION_HTB_SLIDE,			// Horizontal Bar Top -> Bottom Slide
TRANSITION_FUNCTION_HBT_SLIDE,			// Horizontal Bar Bottom -> Top Slide

// Vertical Bars
TRANSITION_FUNCTION_VRL_SLIDE,			// Vertical bar Right -> Left slide
TRANSITION_FUNCTION_VLR_SLIDE,			// Vertical bar Left -> Right slide
TRANSITION_FUNCTION_TB_WIPE,				// Top -> Bottom Wipe
TRANSITION_FUNCTION_TB_SLIDE,				// Top -> Bottom Slide
TRANSITION_FUNCTION_BT_WIPE,				// Bottom -> Top Wipe
TRANSITION_FUNCTION_BT_SLIDE,				// Bottom -> Top Slide
TRANSITION_FUNCTION_V_INTERLEAVE			// Vertical Bar Interleave
};

// Definitions of Directions for transition() function
#define	TRANSITION_ON	0					// Transition on
#define	TRANSITION_OFF	1					// Transition off

// Definitions of transition states
#define	TRANSITION_START	0
#define	TRANSITION_DONE	1

// User interface functions

// Image to use and number of bars (0 = auto-calculate)
void	config_transition(char *ii, int num);

// Ticks, transition on function number, and transition on sound function
void	transition_on(int ticks, int fn, void (*ons)(void));

// Ticks and transition off function
void	transition_off(int ticks, void (*offs)(void));

#endif
