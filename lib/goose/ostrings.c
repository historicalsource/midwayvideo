/* $Revision: 24 $ */
#include	<stdio.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<goose/process.h>
#include	<goose/sprites.h>
#include	<goose/texture.h>
#include	<goose/fonts.h>
#include	<goose/ostrings.h>
#include	<goose/colors.h>
#include	<goose/lockup.h>
#include	<goose/jmalloc.h>

char	goose_ostrings_c_version[] = {"$Revision: 24 $"};

char	*atoui(char *, unsigned int *, unsigned int);
static char	*decode_escape(char *);
static int	old_is_low_res;
int			scale_font_size = 0;

extern	float	hres, vres;
extern	sprite_node_t	*sprite_node_list;
extern	sprite_node_t	*alpha_sprite_node_list;
extern	int				is_low_res;

ostring_t	*ostring_list = (ostring_t *)0;

static sprite_info_t	*char_sprites[200];

static float	zinc = 0.0f;

static text_state_info_t	text_state = {
0.0F, 0.0F, 0.0F, 1.0F, 1.0F, WHITE, BLACK, 0, 1, (HJUST_LEFT|VJUST_BOTTOM), 0, 0, 0, (font_node_t *)0};

static int	string_id = 0;

void get_text_state(text_state_info_t *tsi)
{
	memcpy((char *)tsi, (char *)&text_state, sizeof(text_state_info_t));
}

void set_text_state(text_state_info_t *tsi)
{
	memcpy((char *)&text_state, (char *)tsi, sizeof(text_state_info_t));
}

void set_text_position(int x, int y, float z)
{
	text_state.x = (float)x;
	text_state.y = (float)y;
	text_state.z = z;
}

void set_text_font(int font_id)
{
	text_state.font_node = get_font_handle(font_id);
	if(!text_state.font_node)
	{
		scale_font_size = 10;
		return;
	}
	scale_font_size = text_state.font_node->font_info->scale_size;
}

void set_text_drop_shadow(int drop_flag)
{
	text_state.drop_shadow = drop_flag;
}

void set_text_color(int color)
{
	text_state.color = color;
}

void set_text_bgnd_color(int color)
{
	text_state.bgnd_color = color;
}

void set_text_transparency_mode(int mode)
{
	text_state.transparency_mode = mode;
}

void set_text_justification_mode(int mode)
{
	text_state.justification_mode = mode;
}

void set_text_scales(float w_scale, float h_scale)
{
	text_state.w_scale = w_scale;
	text_state.h_scale = h_scale;
}

void set_text_z_inc(float val)
{
	zinc = val;
}

void set_text_id(int id)
{
	text_state.id = id;
}

void set_string_id(int id)
{
	string_id = id;
}


static void apply_justification(int num, float width, float height)
{
	float		x_adj;
	float		y_adj;
	int		val;

	x_adj = text_state.x;
	switch((text_state.justification_mode & HJUST_MODE_MASK))
	{
		case HJUST_RIGHT:
		{
			x_adj -= width;
			break;
		}
		case HJUST_CENTER:
		{
			val = width / 2;
			x_adj -= (float)val;
			break;
		}
	}
	y_adj = text_state.y;
	switch((text_state.justification_mode & VJUST_MODE_MASK))
	{
		case VJUST_BOTTOM:
		{
			y_adj += height;
			break;
		}
		case VJUST_CENTER:
		{
			val = (int)height / 2;
			y_adj += (float)val;
			break;
		}
	}
	while((--num) >= 0)
	{
		char_sprites[num]->x += x_adj;
		char_sprites[num]->y += y_adj;
		if (text_state.drop_shadow && (num & 1))
			{
			if (text_state.drop_shadow & TEXT_SHAD_LEFT)
				{
				char_sprites[num]->x -= 2;
				}
			else
				{
				char_sprites[num]->x += 2;
				}
			if (text_state.drop_shadow & TEXT_SHAD_UP)
				{
				char_sprites[num]->y += 2;
				}
			else
				{
				char_sprites[num]->y -= 2;
				}
			}
		old_is_low_res = is_low_res;
		if(is_low_res)
		{
			is_low_res = 3;
		}
		generate_sprite_verts(char_sprites[num]);
		is_low_res = old_is_low_res;
	}
}

static void set_char_sprite_info(sprite_info_t *si)
{
	si->h_scale = text_state.h_scale;
	si->w_scale = text_state.w_scale;
	si->state.constant_color = text_state.color;
	si->state.chroma_color = text_state.bgnd_color;
	if(!si->tn)
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
	else if(si->tn->texture_info.header.format == GR_TEXFMT_ALPHA_8 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ALPHA_INTENSITY_44 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ARGB_8332 ||
		si->tn->texture_info.header.format == GR_TEXFMT_AYIQ_8422 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ARGB_1555 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ARGB_4444 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ALPHA_INTENSITY_88 ||
		si->tn->texture_info.header.format == GR_TEXFMT_AP_88)
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
	else if(text_state.transparency_mode)
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	}
	else
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
	if(si->tn)
	{
		if(text_state.color & 0xffffff)
		{
			si->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
		}
		else
		{
			si->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;
		}
	}
	else
	{
		si->state.color_combiner_function = GR_COLORCOMBINE_CCRGB;
	}
	si->id = string_id;
}

static void set_char_sprite_info_drop(sprite_info_t *si)
{
	si->h_scale = text_state.h_scale;
	si->w_scale = text_state.w_scale;
	si->state.constant_color = 0xff000000;
	si->state.chroma_color = text_state.bgnd_color;
	if(!si->tn)
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
	else if(si->tn->texture_info.header.format == GR_TEXFMT_ALPHA_8 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ALPHA_INTENSITY_44 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ARGB_8332 ||
		si->tn->texture_info.header.format == GR_TEXFMT_AYIQ_8422 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ARGB_1555 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ARGB_4444 ||
		si->tn->texture_info.header.format == GR_TEXFMT_ALPHA_INTENSITY_88 ||
		si->tn->texture_info.header.format == GR_TEXFMT_AP_88)
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
	else if(text_state.transparency_mode)
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	}
	else
	{
		si->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
	if(text_state.color )
	{
		si->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	}
	else
	{
		si->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;
	}
	si->id = string_id;
}


static char *decode_escape(char *seq)
{
	int	count;
	int	arg_count;
	char	*arg_ptr[10];
	char	buf[40];

	/* Read up until a termination character is detected */
	count = 0;
	arg_count = 0;
	arg_ptr[arg_count] = buf;
	while(isdigit(*seq) || *seq ==';' || *seq == '-')
	{
		if(*seq == ';')
		{
			seq++;
			arg_count++;
			buf[count++] = 0;
			arg_ptr[arg_count] = &buf[count];
		}
		else
		{
			buf[count++] = *seq++;
		}
	}
	buf[count] = 0;
	arg_count++;

	/* Figure out and apply the escape sequence */
	switch(*seq)
	{
		case 'f':	/* Font change */
		{
			if(arg_count == 1)
			{
				set_text_font(atoi(arg_ptr[0]));
			}
			break;
		}
#if 0
		case 'p':	/* Position change */
		{
			if(arg_count < 3)
			{
			}
			break;
		}
#endif
		case 'c':	/* Foreground color change */
		{
			if(arg_count == 1)
			{
				set_text_color(atoi(arg_ptr[0]));
			}
			break;
		}
		case 'b':	/* Background color change */
		{
			if(arg_count == 1)
			{
				set_text_bgnd_color(atoi(arg_ptr[0]));
			}
			break;
		}
		case 'i':	/* Id change */
		{
			if(arg_count == 1)
			{
				set_text_id(atoi(arg_ptr[0]));
			}
			break;
		}
		case 't':	/* Transparency mode change */
		{
			if(arg_count == 1)
			{
				set_text_transparency_mode(atoi(arg_ptr[0]));
			}
			break;
		}
		case 'j':	/* Justification mode change */
		{
			if(arg_count == 1)
			{
				set_text_justification_mode(atoi(arg_ptr[0]));
			}
			break;
		}
	}

	return(seq);
}

static ostring_t *add_string_object(char *str)
{
	ostring_t	*string;
	int			i;

	/* Allocate a string node */
	string = (ostring_t *)JMALLOC(sizeof(ostring_t));
	if(!string)
	{
#if defined(DEBUG)
		fprintf(stderr, "\r\nCould not allocate memory for string object\r\n");
		lockup();
#endif
		return((ostring_t *)0);
	}

	/* Initialize the data fields of the string object structure */
	memset((char *)string, 0, sizeof(ostring_t));
	
	/* Copy the string */
	i = 0;
	while(*str && i < (sizeof(string->string) - 1))
	{
		string->string[i++] = *str++;
	}
	string->string[i] = 0;

	/* Set the string id */
	string->id = string_id;

	/* Link the new sprite to the front of the sprite list */
	string->next = ostring_list;
	if(ostring_list)
	{
		ostring_list->prev = string;
	}
	ostring_list = string;

	return(string);
}


static void _oputs(char *str, ostring_t *string)
{
	float		width;
	float		height;
	int		num_sprites;
	int		val;
	image_info_t	*char_ii;
	float		z;

	width = 0.0F;
	height = 0.0F;
	num_sprites = 0;
	z = text_state.z;
	while(*str)
	{
		if(*str == '')	/* Escape sequence */
		{
			++str;
			str = decode_escape(str);
		}
		else if(isprint(*str))
		{
			if(!isspace(*str))
			{
				/* Here we create the sprite for the char */
				/* Using the values of the globals */
				/* Find the image info for the character */
				val = *str;
				val &= 0xff;
				val -= text_state.font_node->font_info->start_character;
				if(val < 0 ||
					val > text_state.font_node->font_info->num_characters)
				{
					++str;
					continue;
				}
				char_ii = text_state.font_node->font_info->characters[val];
				if(!char_ii)
				{
					++str;
					continue;
				}

				old_is_low_res = is_low_res;
				if(is_low_res)
				{
					is_low_res = 3;
				}
				char_sprites[num_sprites] = _beginobj((void *)string, char_ii, width, 0.0F, z, 0);
				char_sprites[num_sprites]->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
				is_low_res = old_is_low_res;
				if(!char_sprites[num_sprites])
				{
#if defined(DEBUG)
					fprintf(stderr, "Could not create sprite for character: %c\r\n", *str);
					lockup();
#endif
					break;
				}
				z += zinc;
				if(z < 1.0f)
				{
					z = 1.0f;
				}
				set_char_sprite_info(char_sprites[num_sprites]);

				if(char_sprites[num_sprites]->ii->h * text_state.h_scale > height)
				{
					height = char_sprites[num_sprites]->ii->h * text_state.h_scale;
				}

				if (text_state.drop_shadow)
				{
					++num_sprites;
					if(num_sprites >= sizeof(char_sprites)/sizeof(void *))
					{
						break;
					}

					char_sprites[num_sprites] = _beginobj((void *)string, char_ii, width, 0.0F, z+0.0001f, 0);
					char_sprites[num_sprites]->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
					is_low_res = old_is_low_res;
					if(!char_sprites[num_sprites])
					{
#if defined(DEBUG)
						fprintf(stderr, "Could not create sprite for drop shadow character: %c\r\n", *str);
						lockup();
#endif
						break;
					}
					set_char_sprite_info_drop(char_sprites[num_sprites]);
					if(char_sprites[num_sprites]->ii->h * text_state.h_scale > height)
					{
						height = char_sprites[num_sprites]->ii->h * text_state.h_scale;
					}

					width += (char_sprites[num_sprites]->ii->w * text_state.w_scale);
					width += (text_state.font_node->font_info->char_space * text_state.w_scale);

					++num_sprites;
					if(num_sprites >= sizeof(char_sprites)/sizeof(void *))
					{
						break;
					}
				}
				else
					{
					width += (char_sprites[num_sprites]->ii->w * text_state.w_scale);
					width += (text_state.font_node->font_info->char_space * text_state.w_scale);
					++num_sprites;
					if(num_sprites >= sizeof(char_sprites)/sizeof(void *))
					{
						break;
					}
				}

			}
			else
			{
				width += (text_state.font_node->font_info->space_width * text_state.w_scale);
//				if(text_state.font_node->font_info->space_width * text_state.h_scale > height)
//				{
//					height = text_state.font_node->font_info->space_width * text_state.h_scale;
//				}
			}
		}
		str++;
	}
	string->width = width;
	apply_justification(num_sprites, width, height);

}


int get_string_width(char *str)
{
	float				width;
	int				val;
	image_info_t	*char_ii;

	width = 0.0F;
	while(*str)
	{
		if(*str == '')	/* Escape sequence */
		{
			++str;
			str = decode_escape(str);
		}
		else if(isprint(*str))
		{
			if(!isspace(*str))
			{
				/* Here we create the sprite for the char */
				/* Using the values of the globals */
				/* Find the image info for the character */
				val = *str;
				val &= 0xff;
				val -= text_state.font_node->font_info->start_character;
				if(val < 0 ||
					val > text_state.font_node->font_info->num_characters)
				{
					++str;
					continue;
				}
				char_ii = text_state.font_node->font_info->characters[val];
				if(!char_ii)
				{
					++str;
					continue;
				}

				width += (char_ii->w * text_state.w_scale);
				width += (text_state.font_node->font_info->char_space * text_state.w_scale);
			}
			else
			{
				width += (text_state.font_node->font_info->space_width * text_state.w_scale);
			}
		}
		str++;
	}
	return((int)width);
}


ostring_t *oputs(char *str)
{
	ostring_t		*string;
	char				*to;
	char				*from;
	int				i;

	string = add_string_object(str);
	if(!string)
	{
		return(string);
	}
	_oputs(str, string);

	/* Copy the state information */
	to = (char *)&string->state;
	from = (char *)&text_state;
	for(i = 0; i < sizeof(text_state_info_t); i++)
	{
		*to++ = *from++;
	}

	return(string);
}


static void free_string(ostring_t *string)
{
	/* Free up the memory used by the the string object */
	JFREE(string);

	/* Now go through and delete all sprites whos parent pointer */
	/* is the same as the string pointer */
	del_child_obj((void *)string);
}

void delete_string(ostring_t *string)
{
	ostring_t	*strlist;
	int			tptr = (int)string;
	
	if ((tptr < 0x80000000) ||
		(tptr & 0x3))
	{
#ifdef DEBUG
		fprintf( stderr, "Bogus delete_string %p.\n", string );
		lockup();
#endif
		return;
	}

	strlist = ostring_list;
	while(strlist)
	{
		if(strlist == string)
		{
			/* First node on list */
			if(string == ostring_list)
			{
				ostring_list = string->next;
				if(ostring_list)
				{
					ostring_list->prev = (ostring_t *)0;
				}
			}
			/* Last node on list */
			else if(!string->next)
			{
				string->prev->next = (ostring_t *)0;
			}
			/* Node in middle of list */
			else
			{
				string->next->prev = string->prev;
				string->prev->next = string->next;
			}
			free_string(string);
			return;
		}
		strlist = strlist->next;
	}
}

void delete_string_id(int id)
{
	ostring_t	*strlist;

	strlist = ostring_list;
	while(strlist)
	{
		if(strlist->id == id)
		{
			/* First node on list */
			if(strlist == ostring_list)
			{
				ostring_list = strlist->next;
				if(ostring_list)
				{
					ostring_list->prev = (ostring_t *)0;
				}
			}
			/* Last node on list */
			else if(!strlist->next)
			{
				strlist->prev->next = (ostring_t *)0;
			}
			/* Node in middle of list */
			else
			{
				strlist->next->prev = strlist->prev;
				strlist->prev->next = strlist->next;
			}
			free_string(strlist);
			return;
		}
		strlist = strlist->next;
	}
}

void delete_multiple_strings(int id, int mask)
{
	ostring_t	*strlist;
	ostring_t	*strsave;

	strlist = ostring_list;
	while(strlist)
	{
		strsave = strlist->next;
		if((strlist->id & mask) == id)
		{
			delete_string(strlist);
		}
		strlist = strsave;
	}
}

void delete_all_strings(void)
{
	ostring_t	*strlist;
	ostring_t	*strsave;

	strlist = ostring_list;
	while(strlist)
	{
		strsave = strlist->next;
		free_string(strlist);
		strlist = strsave;
	}
	ostring_list = (ostring_t *)0;
}

void change_string_state(ostring_t *string)
{
	text_state_info_t	cur_ts;
	char					*to;
	char					*from;
	int					i;
	char					save_string[128];
	int					old_scale_font_size;

	/* First we delete all of the sprites associated with this string */
	del_child_obj((void *)string);

	/* Save the current text state */
	to = (char *)&cur_ts;
	from = (char *)&text_state;
	for(i = 0; i < sizeof(text_state_info_t); i++)
	{
		*to++ = *from++;
	}

	/* Set the text state for this string */
	to = (char *)&text_state;
	from = (char *)&string->state;
	for(i = 0; i < sizeof(text_state_info_t); i++)
	{
		*to++ = *from++;
	}

	/* Copy the string from the object */
	to = save_string;
	from = string->string;
	while(*from)
	{
		*to++ = *from++;
	}
	*to = 0;

	/* Need to use scale flag from the old string so */
	/* it won't suddenly change size */
	old_scale_font_size = scale_font_size;
	scale_font_size = text_state.font_node->font_info->scale_size;


	/* Create the new string with the new parameters */
	_oputs(save_string, string);


	/* Put back the scale flag */
	scale_font_size = old_scale_font_size;

	/* Restore the current text state */
	to = (char *)&text_state;
	from = (char *)&cur_ts;
	for(i = 0; i < sizeof(text_state_info_t); i++)
	{
		*to++ = *from++;
	}
}

static void shade_characters(ostring_t *os, float *argb)
{
	sprite_node_t	*slist;
	int				i;

	/* Walk the list and apply the shading values to each character */
	slist = sprite_node_list;
	while(slist)
	{
		if(slist->si->parent == (void *)os)
		{
			slist->si->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB;
			for(i = 0; i < 4; i++)
			{
#if (!(USE_MID_VERTEX & 1))
#ifndef VEGAS
				slist->si->verts[i].a = *(argb+(i<<2)+0);
				slist->si->verts[i].r = *(argb+(i<<2)+1);
#endif
#endif /* USE_MID_VERTEX */
#if ((SPRITE_RGB & 1) || !(USE_MID_VERTEX & 1))
#ifndef VEGAS
				slist->si->verts[i].g = *(argb+(i<<2)+2);
				slist->si->verts[i].b = *(argb+(i<<2)+3);
#endif
#endif
			}
		}
		slist = slist->next;
	}
}

static void shade_line(ostring_t *os, float *argb)
{
	sprite_node_t	*slist;
	float				x_min = hres;
	float				x_max = 0.0f;
	float				y_min = vres;
	float				y_max = 0.0f;
	float				x_diff;
	float				y_diff;
	int				vert_num;

	/* Walk the list and determine the x and y extents */
	slist = sprite_node_list;
	while(slist)
	{
		if(slist->si->parent == (void *)os)
		{
			if(slist->si->verts[0].x < x_min)
			{
				x_min = slist->si->verts[0].x;
			}
			if(slist->si->verts[1].x > x_max)
			{
				x_max = slist->si->verts[1].x;
			}
			if(slist->si->verts[0].y < y_min)
			{
				y_min = slist->si->verts[0].y;
			}
			if(slist->si->verts[2].y > y_max)
			{
				y_max = slist->si->verts[2].y;
			}
		}
		slist = slist->next;
	}

	/* Calculate the x and y ranges */
	x_diff = x_max - x_min;
	y_diff = y_max - y_min;

	/* Walk the list and apply the shading values to each character */
	slist = sprite_node_list;
	while(slist)
	{
		if(slist->si->parent == (void *)os)
		{
			/* Set the color combiner mode */
			slist->si->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB;

			/* Calculate the x based percentages for the vertices */
			for(vert_num = 0; vert_num < 4; vert_num++)
			{
#ifndef VEGAS
#if (!(USE_MID_VERTEX & 1))
				slist->si->verts[vert_num].a = \
					slist->si->verts[vert_num].r = \
					slist->si->verts[vert_num].g = \
					slist->si->verts[vert_num].b = \
						(slist->si->verts[vert_num].x - x_min)/x_diff;
#else
#if (SPRITE_RGB & 1)
				slist->si->verts[vert_num].a_or_r = \
					slist->si->verts[vert_num].g = \
					slist->si->verts[vert_num].b = \
						(slist->si->verts[vert_num].x - x_min)/x_diff;
#else
#endif /* SPRITE_RGB */
#endif /* USE_MID_VERTEX */
#endif
			}

#if 0
			/* Factor in the y base percentages for the vertices */
			for(vert_num = 0; vert_num < 4; vert_num++)
			{
				slist->verts[vert_num].a += (slist->verts[vert_num].y - y_min)/y_diff;
				slist->verts[vert_num].r = \
					slist->verts[vert_num].g = \
					slist->verts[vert_num].b = \
						slist->verts[vert_num].a;
			}
#endif

			/* Calculate the bottom vertex colors */
#ifndef VEGAS
			for(vert_num = 0; vert_num < 4; vert_num++)
			{
#if (!(USE_MID_VERTEX & 1))
				slist->si->verts[vert_num].a *= (argb[4] - argb[0]);
				slist->si->verts[vert_num].r *= (argb[5] - argb[1]);
#else
#endif /* USE_MID_VERTEX */
#if ((SPRITE_RGB & 1) || !(USE_MID_VERTEX & 1))
				slist->si->verts[vert_num].g *= (argb[6] - argb[2]);
				slist->si->verts[vert_num].b *= (argb[7] - argb[3]);
#endif

#if (!(USE_MID_VERTEX & 1))
				slist->si->verts[vert_num].a += argb[0];
				slist->si->verts[vert_num].r += argb[1];
#else
#endif /* USE_MID_VERTEX */
#if ((SPRITE_RGB & 1) || !(USE_MID_VERTEX & 1))
				slist->si->verts[vert_num].g += argb[2];
				slist->si->verts[vert_num].b += argb[3];
#endif
			}
#endif
		}
		slist = slist->next;
	}
}

void set_string_shading(ostring_t *os, int mode, float *argb)
{
	if(mode == STR_SHADE_CHARACTERS)
	{
		shade_characters(os, argb);
	}
	else
	{
		shade_line(os, argb);
	}
}

void change_sprite_children_alpha(ostring_t *, int, int);

void change_string_alpha_state(ostring_t *str, int mode, int alpha)
{
	str->state.constant_alpha = (alpha << 24);
	if(mode)
	{
		change_sprite_children_alpha(str, ALPHA_MODE, str->state.constant_alpha);
		str->state.alpha_mode = ALPHA_ENABLE;
	}
	else
	{
		change_sprite_children_alpha(str, 0, str->state.constant_alpha);
		str->state.alpha_mode = 0;
	}
}

static char	str_buffer[256];

ostring_t *oprintf(char *fmt, ...)
{
	vsprintf(str_buffer, fmt, (&fmt)+1);
	return(oputs(str_buffer));
}

void fade_string(ostring_t *os, int alpha)
{
	sprite_node_t	*slist = sprite_node_list;
	sprite_node_t	*b_list = sprite_node_list;

	/* Walk the normal list and apply the shading values to each character */
	/* Walk the normal list and apply the shading values to each character */
	while(slist)
	{
		b_list = slist;
		slist = slist->next;
		if(b_list->si->parent == (void *)os)
			{
			change_sprite_alpha_state(b_list->si, 1);
			}
	}

	/* Walk the alpha list and apply the shading values to each character */
	slist = alpha_sprite_node_list;
	while(slist)
	{
		if(slist->si->parent == (void *)os)
		{
			slist->si->state.constant_color = ((slist->si->state.constant_color & 0xffffff) | (alpha <<24)); 
		}
		slist = slist->next;
	}

}

//////////////////////////////////////////////////////////////////////////////
// change the constant_color field in the status of all sprites
// matching a given parent.
void change_string_cc( void *parent, int cc )
{
	register sprite_node_t	*snode = sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
		{
			snode->si->state.constant_color = cc;
		}

		// Save the next node pointer
		snode = snode->next;
	}
}

//////////////////////////////////////////////////////////////////////////////
// END ostrings.c
