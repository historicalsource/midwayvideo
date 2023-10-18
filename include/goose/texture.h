/******************************************************************************/
/*                                                                            */
/* texture.h - Texture header file                                            */
/*                                                                            */
/* Written by:  Michael J. Lynch                                              */
/* Version:     1.00                                                          */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
#ifndef	__TEXTURE_H__
#define	__TEXTURE_H__

#ifdef VERSIONS
char	goose_texture_h_version[] = {"$Revision: 6 $"};
#endif

#ifndef __GLIDE_H__
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#endif

// JMS
#define add_texture_to_free 0
#define free_texture_space  1
// JMS

/* Flag bits for [texture_flags] in [texture_node} struct */
#define	TEXTURE_LOCKED		(1 << 0)

/* Opcodes for [flag] parm of <clear_texture_list>
    function in Texture.C */
enum {
	TEXTURES_ALL,
	TEXTURES_NORMAL,
	TEXTURES_LOCKED
};

/* Opcodes for [cflags] parm of <create_texture>, etc,
    functions in Texture.C */
enum {
	CREATE_NORMAL_TEXTURE,
	FORCE_NEW_TEXTURE
};

/* Texture ID's */
//#define TXID_PLAYER			1
//#define TXID_FIELD			2
//#define TXID_STADIUM		3


#ifdef VEGAS
#define TEXTURE_COMBINE_MODE_SINGLE_TMU 0x0
#define TEXTURE_COMBINE_MODE_TRILINEAR  0x1
#define TEXTURE_COMBINE_MODE_PROJECTED  0x2
#define TEXTURE_COMBINE_MODE_COMPOSITE  0x3

#define NO_TEXBASERANGE ((GrTexBaseRange_t) (~0))
#define NO_TABLE ((GrTexTable_t)(~0))


#define NUM_FREELIST_TMU_RESET	     256
#define NUM_FREELIST_TEXTURE_RESET	 256
#define NUM_FREELIST_TABLE_RESET	 32

#define NUM_FREELIST_TMU_DELTA		 32
#define NUM_FREELIST_TEXTURE_DELTA	 32
#define NUM_FREELIST_TABLE_DELTA	 8

typedef FxFloat TexLod_t;
typedef FxU32 TexCombineMode_t;


/*****************************************************************************/
/* Structures:                                                               */
/*****************************************************************************/

typedef struct TmuBoardInfo{
	int minAddrTmu0;
	int maxAddrTmu0;
	int minAddrTmu1;
	int maxAddrTmu1;
	int minAddrTmu2;
	int maxAddrTmu2;
	int memory_tmu;
	int num_tmu;
	int revision_tmu;
	int texture_align;
	} TmuBoardInfo_t;

typedef struct TmuMemoryInfo{
	unsigned int minAddrTmu0_A;
	unsigned int maxAddrTmu0_A;
	unsigned int freeMemTmu0_A;
    
    unsigned int minAddrTmu0_B;
	unsigned int maxAddrTmu0_B;
	unsigned int freeMemTmu0_B;
	
    unsigned int minAddrTmu1_A;
	unsigned int maxAddrTmu1_A;
	unsigned int freeMemTmu1_A;
    
	unsigned int minAddrTmu1_B;
	unsigned int maxAddrTmu1_B;
	unsigned int freeMemTmu1_B;
    
	
    unsigned int minAddrTmu2_A;
	unsigned int maxAddrTmu2_A;
	unsigned int freeMemTmu2_A;
    
	unsigned int minAddrTmu2_B;
	unsigned int maxAddrTmu2_B;
	unsigned int freeMemTmu2_B;
	        
	} TmuMemoryInfo_t;

    
    
//typedef struct {
//    GrLOD_t           smallLodLog2;
//    GrLOD_t           largeLodLog2;
//    GrAspectRatio_t   aspectRatioLog2;
//	  GrTextureFormat_t format;
//    void              *data;
//	  } GrTexInfo;





typedef FxU32 Palette[256];

typedef struct {
  FxU8  yRGB[16];
  FxI16 iRGB[4][3];
  FxI16 qRGB[4][3];
  FxU32 packed_data[12];
} NCCTable;

typedef union {
    Palette  palette;
    NCCTable nccTable;
} TextureTable;


typedef struct table_node{
	struct table_node *next;
	struct table_node *prev;
    GrTexTable_t    tableType;
    TextureTable    tableData;
	} Table_node_t;

typedef struct tmu_node {
    struct tmu_node	*next;
    struct tmu_node	*prev;
    struct Texture_Node	*text;
    unsigned int start;
    unsigned int size;
	} Tmu_node_t;


typedef struct {
	GrChipID_t tmu;
    TexLod_t lod;
    unsigned int start_addr;
    unsigned int size;
    unsigned int evenOdd;
	GrTexBaseRange_t texBaseRange;
    GrTextureFilterMode_t minFilterMode;
    GrTextureFilterMode_t magFilterMode;
    GrTextureClampMode_t s_clamp_mode;
    GrTextureClampMode_t t_clamp_mode;
    GrMipMapMode_t mipmapmode;
    FxBool	lodBlend;
    TexCombineMode_t combine_mode;
	} TextureAttrib_t;

    
typedef struct Texture_Node {
    struct Texture_Node	*next;
    struct Texture_Node	*prev;
    struct tmu_node *tmu_list;
    struct Texture_Node *trilinear;
    struct Texture_Node *multibase;
	struct table_node *table;
    GrTexInfo       info;
    TextureAttrib_t attrib;
	int id_number;
   int new_tex;
	} Texture_node_t;

void guTexSource(Texture_node_t *tex);
void grFreeTexture(Texture_node_t *tex);
void guTexMemReset(void);

typedef struct wms_header
{
	int				version;
	float				bias;
	GrMipMapMode_t	filter_mode;
	FxBool			tri_mode;
	Gu3dfHeader		header;
} wms_header_t;

Texture_node_t *guLoadTextureDirect(const char *FileName, Gu3dfInfo *Info, int sclamp, int tclamp, int minfilt, int maxfilt);
Texture_node_t *guLoadTextureDirectFromMemory(char *buffer, Gu3dfInfo *Info, int sclamp, int tclamp, int minfilt, int maxfilt);

#endif

typedef struct texture_node {
	struct texture_node	*next;
	struct texture_node	*prev;
	short			texture_id;
	short			texture_flags;
	short			texture_count;
	char			texture_name[12];
#ifndef VEGAS
	GrMipMapId_t		texture_handle;
#else
	Texture_node_t	*texture_handle;
#endif
	Gu3dfInfo		texture_info;
} texture_node_t;

struct texture_node *find_texture(char *, short);
struct texture_node *create_texture(char *, int, int, int, int, int, int, int);
struct texture_node *create_texture_from_memory(char *, void *, int, int, int, int, int, int, int);
struct texture_node *texture_to_mem(char *, int, int, int);
void delete_texture(struct texture_node *);
void delete_texture_id(int);
void delete_multiple_textures(int, int);
void clear_texture_list(int);
void lock_texture(struct texture_node *);
void lock_texture_id(int);
void lock_multiple_textures(int, int);
void lock_textures_all(void);
void unlock_texture(struct texture_node *);
void unlock_texture_id(int);
void unlock_multiple_textures(int, int);
void unlock_all_textures(void);
void force_delete_multiple_textures(int, int);
// JMS
void set_texture_free_mode(int);
// JMS


#endif
