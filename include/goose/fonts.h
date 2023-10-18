/* $Revision: 5 $ */
#ifndef	__FONTS_H__
#define	__FONTS_H__

#ifdef VERSIONS
char	goose_fonts_h_version[] = {"$Revision: 5 $"};
#endif

#ifndef	__TEXTURE_H__
#include	<goose/texture.h>
#endif

/* This is the information stored in a font file */
typedef struct font_info
{
	int				num_characters; 	// # chars in font
	int				start_character;	// start char value
	int				end_character;		// end char value
	int				height;				// pixel height
	int				space_width;		// width of space
	int				char_space;			// inter-character spacing (pixels)
	char				*texture_name; 	// pointer to texture name
	image_info_t	**characters;		// pointer to array of char image info
	int				scale_size;			// Scale font size (for low res)
} font_info_t;

/* This is a font node */
typedef struct font_node
{
	struct font_node		*next;		// Next font in list
	struct font_node		*prev;		// Previous font in list
	int						id;			// Id of the font
	int						num_tex;		// Number of textures for font
	struct texture_node	*tn[16];		// Array of texture node pointers
	struct font_info		*font_info;	// Font information for node
} font_node_t;

/* Font function prototypes */
font_node_t *create_font(font_info_t *fi, int);
void delete_font(font_node_t *);
void delete_font_id(int);
font_node_t *get_font_handle(int);
image_info_t *get_char_image_info(int, char);
int get_font_space_width(int);
int get_font_height(int);
void delete_fonts(void);

#endif
