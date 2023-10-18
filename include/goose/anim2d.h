#ifndef __ANIM2D_H__
#define __ANIM2D_H__

#ifdef VERSIONS
char	goose_anim2d_h_version[] = {"$Revision: 7 $"};
#endif

typedef struct ani_2d_info {
	int	frame_delay;
	int	loop_count;
	int	*a_2d_script;
	int	*a_loop_script;
	void	(*func_call)(sprite_info_t*);
	float	aux1;
	float	aux2;
	int	aux3;
	int	aux4;
}	ani_2d_info_t;

/* Animation commands */
#define		A_2D_LOOP		0x80000001
#define		A_2D_LOOP_BACK	0x80000002
#define		A_2D_CALL		0x80000003
#define		A_KILL_OBJ		0x80000004
#define		A_2D_GOTO		0x80000005

/* Function prototypes */
void animate_2d_proc(int*);
void make_sprite_anim(sprite_info_t *sprite, int* script);
void free_anim_space(sprite_node_t *snode);
void pause_sprite_anim(sprite_info_t *sprite);
void unpause_sprite_anim(sprite_info_t *sprite);
void unhook_sprite_anim(sprite_info_t *sprite, image_info_t *frame, int frame_tid);
void add_func_to_sprite(sprite_info_t *sprite, int func_addr );
void check_2danim_list(void);

#endif
