// disabled by wfd 1/25/99
//#ifdef BANSHEE
//#ifdef DEBUG
//#define	GDBG_INFO_ON	1
//#endif
//#endif

#include <memory.h>
#include	<string.h>
#include	<stdio.h>
#include	<dir.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<sys/stat.h>

#include	<goose/goose.h>
//#include	<goose/texture.h>
//#include	<goose/sprites.h>
#include	<glideutl.h>

#include <3dfx.h>

#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>

#include "fxglide.h"
#ifdef BANSHEE
#include	<fxcmd.h>
#endif

#if GLIDE_DISPATCH_SETUP
#include "fxinline.h"
#endif
#ifdef GLIDE_DEBUG
#include <math.h>
#endif /* GLIDE_DEBUG */

#ifdef DEBUG
#define	DEBUG_INFO
#endif

unsigned int ___get_count(void);
#define	GET_TIME(A)	(A) = (___get_count() * 10)

extern texture_node_t		*tlist;

static unsigned int	draw_func_time = 0;
static unsigned int	dfunc_start;
static unsigned int	dfunc_end;

#ifndef	DEBUG_INFO
#undef	GDBG_INFO
#define	GDBG_INFO(a, b, c, d)
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef TRUE
#define	TRUE	1
#endif


#if defined(BANSHEE)
#define GRX		"BANSHEE"
#elif defined(VOODOO2)
#define GRX		"VOODOO2"
#else
#define GRX		"VOODOO1"
#endif

#if 0
static float cull_check(const float *va, const float *vb, const float *vc);
#endif

//
// Define this to make texture system use 3df files instead of WMS files
//
//#define	USE_3DF_FILES


void print_tmu_list (Tmu_node_t *list);
void dump_tmu_list (Tmu_node_t *node);
void dump_all(void);


/* init functions *****************************************/
static void initTextureInfo(TmuBoardInfo_t *info);
static void initMemoryInfo (TmuBoardInfo_t *info);
static void init_tmu_list (void);

static void TextureCombine( TextureAttrib_t *attrib );
static GrTexTable_t texTableType( GrTextureFormat_t format );
#ifdef USE_3DF_FILES
static int LoadTexture( const char *filename, GrTexInfo *info, Table_node_t **table, Gu3dfInfo *) ;
#endif

/* load related functions: ********************************/
static int texLoadTmuMemory (Texture_node_t *text);
static int texSingleTmu0 (Texture_node_t *text, int memory );
#if defined(VOODOO2)
static int texSingleTmu1 (Texture_node_t *text, int memory );
static int texTrilinearTmu0 (Texture_node_t *text, int memEven, int memOdd );
static int texTrilinearTmu1 (Texture_node_t *text, int memEven, int memOdd );
#endif
static int texMultiBaseTmu0 (Texture_node_t *text, int memBoth );
#if defined(VOODOO2)
static int texMultiBaseTmu1 (Texture_node_t *text, int memBoth );
#endif
static int getTotalMemTmu ( GrChipID_t tmu);
static int get_memory (int size, Tmu_node_t *node, Texture_node_t *text);
static int query_memory (int size, GrChipID_t tmu, Tmu_node_t **node, unsigned int *free);
static int findMemTmuList (Tmu_node_t *list, Tmu_node_t **node, int size);
static unsigned int *findTmuSection ( unsigned int addr, GrChipID_t tmu );

/* tableFreeList: *****************************************/
static void initTableFreeList (int num);
static Table_node_t *getTableFreeList (void);
static void returnTableFreeList (Table_node_t *tmp);
static void dump_TableFreeList (void);

/* textFreeList: ******************************************/
static void initTextFreeList (int num);
static Texture_node_t *getTextFreeList (void);
static void returnTextFreeList (Texture_node_t *tmp);
static void dump_TextFreeList (void);

/* tmuFreeList: *******************************************/
static void initTmuFreeList (int num);
static Tmu_node_t *getTmuFreeList (void);
static void returnTmuFreeList (Tmu_node_t *tmp);
static void dump_TmuFreeList (void);

/* table_list: ********************************************/
static void create_table_node ( Table_node_t **node );
static Table_node_t *insert_table_node ( void );
static void delete_table_node (Table_node_t **node);
static void remove_table_node (Table_node_t *table);

/* text_list: *********************************************/
static void create_texture_node ( Texture_node_t **node);
Texture_node_t *insert_texture_node( void);
static void delete_texture_node (Texture_node_t **text);
static void remove_texture_node (Texture_node_t *text);

/* tmu0_A,tmu0_B,tmu1_A,tmu1_B: ***************************/
static void create_tmu_node ( Tmu_node_t **node );
static Tmu_node_t *insert_tmu_node ( Tmu_node_t *node );
static void delete_tmu_node (Tmu_node_t **node);
static void remove_tmu_node (Tmu_node_t *node);

void delete_texture_handle(Texture_node_t *text);

static Texture_node_t *textFreeList = NULL;
static Tmu_node_t     *tmuFreeList = NULL;
static Table_node_t   *tableFreeList = NULL;

static Texture_node_t *text_list = NULL;
static Table_node_t   *table_list = NULL;

static Tmu_node_t *tmu0_A = NULL;
static Tmu_node_t *tmu0_B = NULL;
static Tmu_node_t *tmu1_A = NULL;
static Tmu_node_t *tmu1_B = NULL;


static TmuMemoryInfo_t MemoryInfo;
static TmuBoardInfo_t TmuInfo;

int id_number = 0;

int	callno = 0;
int	callno_delta = 0;

#ifndef USE_3DF_FILES
static unsigned char	*texture_buffer = NULL;
static unsigned char	*tex_ptr;
void						(*tex_crc_fail_audit_func)(void) = NULL;

void flush_disk_cache(void);
void flush_disk_queue(void);

//unsigned long	crc(void *, int);
int _read(int, void *, int);

#define	WMS_FILE_VERSION	(5|0x8000)
#endif

void display_dfunc_time(void)
{
//	fprintf(stderr, "Time in draw functions:     %6.3f (ms)\n", (float)draw_func_time / 1000000.0f);
	fprintf(stderr, "Time in draw functions:     NOT ENABLED\n");
}

void reset_dfunc_time(void)
{
	draw_func_time = 0;
}


GR_ENTRY(grDrawSprite, void, (const float *a))
{
	register int	vsize;
	register int	*pptr;

	GR_DCL_GC;

//	GET_TIME(dfunc_start);

	GR_FLUSH_STATE();

	vsize = gc->state.vData.vSize;

	GR_SET_EXPECTED_SIZE(4 * vsize, 1);

	TRI_STRIP_BEGIN(kSetupFan, 4, vsize, SSTCP_PKT3_BDDDDD);

	pptr = (int *)tPackPtr;

	if(vsize == 12)
	{
		*pptr++ = *((int *)a + 0);
		*pptr++ = *((int *)a + 1);
		*pptr++ = *((int *)a + 2);

		*pptr++ = *((int *)a + 5);
		*pptr++ = *((int *)a + 6);
		*pptr++ = *((int *)a + 7);

		*pptr++ = *((int *)a + 10);
		*pptr++ = *((int *)a + 11);
		*pptr++ = *((int *)a + 12);

		*pptr++ = *((int *)a + 15);
		*pptr++ = *((int *)a + 16);
		*pptr++ = *((int *)a + 17);
	}
	else
	{
		*pptr++ = *((int *)a + 0);
		*pptr++ = *((int *)a + 1);
		*pptr++ = *((int *)a + 2);
		*pptr++ = *((int *)a + 3);
		*pptr++ = *((int *)a + 4);

		*pptr++ = *((int *)a + 5);
		*pptr++ = *((int *)a + 6);
		*pptr++ = *((int *)a + 7);
		*pptr++ = *((int *)a + 8);
		*pptr++ = *((int *)a + 9);

		*pptr++ = *((int *)a + 10);
		*pptr++ = *((int *)a + 11);
		*pptr++ = *((int *)a + 12);
		*pptr++ = *((int *)a + 13);
		*pptr++ = *((int *)a + 14);

		*pptr++ = *((int *)a + 15);
		*pptr++ = *((int *)a + 16);
		*pptr++ = *((int *)a + 17);
		*pptr++ = *((int *)a + 18);
		*pptr++ = *((int *)a + 19);

	}

	tPackPtr = (void *)pptr;

	TRI_END;

//	GET_TIME(dfunc_end);
//	draw_func_time += (dfunc_end - dfunc_start);
}



GR_ENTRY(grDrawFan, void, (const float *a, int num_verts))
{
	register int	vsize;
	register float	*pptr;

	GR_DCL_GC;

//	GET_TIME(dfunc_start);

	GR_FLUSH_STATE();

	vsize = gc->state.vData.vSize;

	GR_SET_EXPECTED_SIZE(num_verts * vsize, 1);

	TRI_STRIP_BEGIN(kSetupFan, num_verts, vsize, SSTCP_PKT3_BDDDDD);

	pptr = (float *)tPackPtr;

	while(num_verts--)
	{
		*pptr++ = *a++;
		*pptr++ = *a++;
		*pptr++ = *a++;
		*pptr++ = *a++;
		*pptr++ = *a++;
		a++;
	}

	tPackPtr = (void *)pptr;

	TRI_END;

//	GET_TIME(dfunc_end);
//	draw_func_time += (dfunc_end - dfunc_start);
}

static void memcpy_words(unsigned long *to, unsigned long *from, int num)
{
	while(num--)
	{
		*to++ = *from++;
	}
}


GR_ENTRY(grDrawStrip, void, (const float *a, int num_verts))
{
	register int	vsize;
	int				num = num_verts > 15 ? 15 : num_verts;

	GR_DCL_GC;

	// Flush the state
	GR_FLUSH_STATE();

	// Get the size of vertex (in bytes)
	vsize = gc->state.vData.vSize;

	// Set the expected size for the first part of the strip
	GR_SET_EXPECTED_SIZE(num * vsize, 1);

#if 1
	// Set the strip begin header
	TRI_STRIP_BEGIN(kSetupStrip, num, vsize, SSTCP_PKT3_BDDDDD);
#endif

	// Loop and send verts
	while(num_verts)
	{
		// Decrement number of verts left to send
		num_verts -= num;

		// Adjust num for total words
		num *= (vsize >> 2);

		// Send the words
		memcpy_words((unsigned long *)tPackPtr, (unsigned long *)a, num);

		// Increment packet buffer pointer
		tPackPtr += num;

		// Done with this vertex set
		TRI_END;

		// More verts left ?
		if(num_verts)
		{
			// YES - Increment vertex pointer
			a += num;

			// Limit to 15 vertices
			num = num_verts > 15 ? 15 : num_verts;

			// Set the expected size
			GR_SET_EXPECTED_SIZE(num * vsize, 1);

#if 1
			*tPackPtr++ = ((kSetupStrip<<SSTCP_PKT3_SMODE_SHIFT) |
				(num<<SSTCP_PKT3_NUMVERTEX_SHIFT) |
				(SSTCP_PKT3_DDDDDD) |
				(gc->cmdTransportInfo.cullStripHdr));
#else
			// Send the continue header
			TRI_STRIP_BEGIN(kSetupStrip, num, vsize, SSTCP_PKT3_DDDDDD);
#endif
		}
	}
}

GR_ENTRY(grPreflush, void *, (void))
{
	GR_DCL_GC;

	GR_FLUSH_STATE();
	return(gc);
}

GR_ENTRY(grDrawSimpleTrianglePreflushed, void, (GrGC *gc, const void *a, const void *b, const void *c))
{
	register int	*pptr;

	GR_SET_EXPECTED_SIZE(3 * 20, 1);

	TRI_STRIP_BEGIN(kSetupStrip, 3, 20, SSTCP_PKT3_BDDBDD);

	pptr = (int *)tPackPtr;

	*pptr++ = *((int *)a + 0);
	*pptr++ = *((int *)a + 1);
	*pptr++ = *((int *)a + 2);
	*pptr++ = *((int *)a + 3);
	*pptr++ = *((int *)a + 4);

	*pptr++ = *((int *)b + 0);
	*pptr++ = *((int *)b + 1);
	*pptr++ = *((int *)b + 2);
	*pptr++ = *((int *)b + 3);
	*pptr++ = *((int *)b + 4);

	*pptr++ = *((int *)c + 0);
	*pptr++ = *((int *)c + 1);
	*pptr++ = *((int *)c + 2);
	*pptr++ = *((int *)c + 3);
	*pptr++ = *((int *)c + 4);

	tPackPtr = (void *)pptr;

	TRI_END;
}

GR_ENTRY(grDrawSimpleTriangle, void, (const void *a, const void *b, const void *c))
{
	register int	vsize;
	register int	*pptr;

	GR_DCL_GC;

//	GET_TIME(dfunc_start);

	GR_FLUSH_STATE();

	vsize = gc->state.vData.vSize;

	GR_SET_EXPECTED_SIZE(3 * vsize, 1);

	TRI_STRIP_BEGIN(kSetupStrip, 3, vsize, SSTCP_PKT3_BDDBDD);

	pptr = (int *)tPackPtr;

	*pptr++ = *((int *)a + 0);
	*pptr++ = *((int *)a + 1);
	*pptr++ = *((int *)a + 2);
	*pptr++ = *((int *)a + 3);
	*pptr++ = *((int *)a + 4);

	*pptr++ = *((int *)b + 0);
	*pptr++ = *((int *)b + 1);
	*pptr++ = *((int *)b + 2);
	*pptr++ = *((int *)b + 3);
	*pptr++ = *((int *)b + 4);

	*pptr++ = *((int *)c + 0);
	*pptr++ = *((int *)c + 1);
	*pptr++ = *((int *)c + 2);
	*pptr++ = *((int *)c + 3);
	*pptr++ = *((int *)c + 4);

	tPackPtr = (void *)pptr;

	TRI_END;

//	GET_TIME(dfunc_end);
//	draw_func_time += (dfunc_end - dfunc_start);
}

static int	last_cmode = -1;

unsigned long long get_timer_val(void);

GR_ENTRY(grDrawTriangleDma, void, (const void *a, const void *b, const void *c, int cull_enable))
{
	if((last_cmode && !cull_enable) || (!last_cmode && cull_enable))
	{
		if(cull_enable)
		{
			grCullMode(GR_CULL_NEGATIVE);
			last_cmode = 1;
		}
		else
		{
			grCullMode(GR_CULL_DISABLE);
			last_cmode = 0;
		}
	}

	grDrawSimpleTriangle(a, b, c);
}

#ifdef USE_3DF_FILES
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: Load_3dfx_texture()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Texture_node_t *Load_3dfx_texture(const char *filename,
											TexCombineMode_t combine,
											TexLod_t	lod,
											GrTextureFilterMode_t minfilter,
											GrTextureFilterMode_t maxfilter,
											GrMipMapMode_t mipmapmode,
											Gu3dfInfo *Info)
{
	Texture_node_t *text;


	text = insert_texture_node();

	if(!text)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, "Can not allocated texture node\n");
		return(NULL);
	}
	 
	text->attrib.combine_mode = combine;
	text->attrib.lod = lod;
	text->attrib.minFilterMode = minfilter;
	text->attrib.magFilterMode = maxfilter;
	text->attrib.mipmapmode = mipmapmode;
	      
	if(!LoadTexture(filename, &text->info, &text->table, Info))
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, "Could not load texture: %s\n", filename);
		delete_texture_handle(text);
		return(NULL);
	}


	status = texLoadTmuMemory (text);

	if(status == TRUE)
	{
		return(text);
	}
	else
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, "Could not load texture %s into TMU memory\n", filename);
		delete_texture_handle(text);
		return(NULL);
	}
}
#endif


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: findMemTmuList()                                                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int findMemTmuList (Tmu_node_t *list, Tmu_node_t **node, int size)
{
	unsigned int	addr1;
	unsigned int	addr2;
	Tmu_node_t		*tmp = NULL;
	Tmu_node_t		*tmp1;
	int				mem_size;
	
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p, %p, %d)\n", list, node, size);
	tmp1 = list;
	mem_size = 0;
    
	while(mem_size < size) 
	{
		tmp = tmp1;
		tmp1 = tmp->next;
        
		/* Check if walked off the end of the list */
		if(tmp1 == NULL)
		{
			node = NULL;
			return(FALSE);
		}
        
		addr1 = tmp->start + tmp->size;
		addr2 = tmp1->start;
		mem_size = addr2 - addr1;
	}
	
	*node = tmp;

	return (TRUE);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: init_tmu_list()                                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void init_tmu_list (void)
{
	Tmu_node_t	*tmp;
	Tmu_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	/***********************************************************/
	/* tmu0_A:                                                 */
	/***********************************************************/
	tmp = getTmuFreeList();
	tmp2 = getTmuFreeList();

	/* Header */
	tmu0_A = tmp;
	tmp->prev = NULL;
	tmp->next = tmp2;
	tmp->start = MemoryInfo.minAddrTmu0_A;
	tmp->size = 0x0;
	tmp->text = NULL;
    
	/* Footer */
	tmp2->prev = tmp;
	tmp2->next = NULL;
	tmp2->start = MemoryInfo.maxAddrTmu0_A;
	tmp2->size = 0x0;
	tmp2->text = NULL;
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "TMU0_A: %d bytes\n", MemoryInfo.maxAddrTmu0_A - MemoryInfo.minAddrTmu0_A);

	/***********************************************************/
	/* tmu0_B:                                                 */
	/***********************************************************/

	tmp = getTmuFreeList();
	tmp2 = getTmuFreeList();

	/* Header */
	tmu0_B = tmp;
	tmp->prev = NULL;
	tmp->next = tmp2;
	tmp->start = MemoryInfo.minAddrTmu0_B;
	tmp->size = 0x0;
	tmp->text = NULL;
    
	/* Footer */
	tmp2->prev = tmp;
	tmp2->next = NULL;
	tmp2->start = MemoryInfo.maxAddrTmu0_B;
	tmp2->size = 0x0;
	tmp2->text = NULL;
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "TMU0_B: %d bytes\n", MemoryInfo.maxAddrTmu0_B - MemoryInfo.minAddrTmu0_B);
 
	/***********************************************************/
	/* tmu1_A:                                                 */
	/***********************************************************/

	tmp = getTmuFreeList();
	tmp2 = getTmuFreeList();

	/* Header */
	tmu1_A = tmp;
	tmp->prev = NULL;
	tmp->next = tmp2;
	tmp->start = MemoryInfo.minAddrTmu1_A;
	tmp->size = 0x0;
	tmp->text = NULL;
    
	/* Footer */
	tmp2->prev = tmp;
	tmp2->next = NULL;
	tmp2->start = MemoryInfo.maxAddrTmu1_A;
	tmp2->size = 0x0;
	tmp2->text = NULL;
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "TMU1_A: %d bytes\n", MemoryInfo.maxAddrTmu1_A - MemoryInfo.minAddrTmu1_A);
    
	/***********************************************************/
	/* tmu1_B:                                                 */
	/***********************************************************/
 
	tmp = getTmuFreeList();
	tmp2 = getTmuFreeList();
    
	/* Header */
	tmu1_B = tmp;
	tmp->prev = NULL;
	tmp->next = tmp2;
	tmp->start = MemoryInfo.minAddrTmu1_B;
	tmp->size = 0x0;
	tmp->text = NULL;
    
	/* Footer */
	tmp2->prev = tmp;
	tmp2->next = NULL;
	tmp2->start = MemoryInfo.maxAddrTmu1_B;
	tmp2->size = 0x0;
	tmp2->text = NULL;
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "TMU1_B: %d bytes\n", MemoryInfo.maxAddrTmu1_B - MemoryInfo.minAddrTmu1_B);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: query_memory()                                                  */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int query_memory (int size, GrChipID_t tmu, Tmu_node_t **node, unsigned int *free)
{
	Tmu_node_t	*tmp;
	int			status;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %d, %p, %p)\n", size, (int)tmu, node, free);
	tmp = NULL;
	status = FALSE;

	if(tmu == GR_TMU0)
	{
		if(size < MemoryInfo.freeMemTmu0_A)
		{
			status = findMemTmuList (tmu0_A, node, size);
			*free = (unsigned int) &MemoryInfo.freeMemTmu0_A;
			if(status == FALSE)
			{
				if(size < MemoryInfo.freeMemTmu0_B)
				{
					status = findMemTmuList (tmu0_B, node, size);
					*free = (unsigned int) &MemoryInfo.freeMemTmu0_B;
				}
			}           
			return (status);                   
		}
		else if(size < MemoryInfo.freeMemTmu0_B)
		{
			status = findMemTmuList (tmu0_B, node, size);
			*free = (unsigned int) &MemoryInfo.freeMemTmu0_B;
			return (status);                   
		}
		else
		{
			return (FALSE);
		}                            
	}
#if defined(VOODOO2)
	else if(tmu == GR_TMU1)
	{
		if(size < MemoryInfo.freeMemTmu1_A)
		{
			status = findMemTmuList (tmu1_A, node, size);
			*free = (unsigned int) &MemoryInfo.freeMemTmu1_A;
            
			if(status == FALSE)
			{
				if(size < MemoryInfo.freeMemTmu1_B)
				{
					status = findMemTmuList (tmu1_B, node, size);
					*free = (unsigned int) &MemoryInfo.freeMemTmu1_B;
				}
			}           
			return (status);                   
		}
		else if(size < MemoryInfo.freeMemTmu1_B)
		{
			status = findMemTmuList (tmu1_B, node, size);
			*free = (unsigned int) &MemoryInfo.freeMemTmu1_B;
			return (status);                   
		}
		else
		{
			return (FALSE);
		}                            
	}            
#endif
	else
	{
		return (FALSE);
	}        
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: get_memory()                                                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int get_memory (int size, Tmu_node_t *node, Texture_node_t *text)
{
	int			start_addr;
	Tmu_node_t	*tmu;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %p, %p)\n", size, node, text);
	start_addr = node->start + node->size;
	tmu = insert_tmu_node ( node );
    
	text->tmu_list = tmu;
	tmu->text = text;
	tmu->start = start_addr;
	tmu->size = size;
    
	return(start_addr);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texLoadTmuMemory()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int texLoadTmuMemory (Texture_node_t *text)
{
	int	memEven;
	int	memOdd; 
	int	memBoth;
	int	memTMU0;
	int	memTMU1 = 0;
	int	status;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", text);
	id_number++;

	memEven = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_EVEN,&text->info);
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "memEven: %d\n", memEven);
	memOdd  = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_ODD,&text->info);
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "memOdd: %d\n", memOdd);
	memBoth = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH,&text->info);
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "memBoth: %d\n", memBoth);
	memTMU0 = getTotalMemTmu(GR_TMU0);
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "memTMU0: %d\n", memTMU0);
#if defined(VOODOO2)
	memTMU1 = getTotalMemTmu(GR_TMU1);
	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "memTMU1: %d\n", memTMU1);
#endif
    
	status = FALSE;
    
	/******************************************************************/
	/* For Single TMU support:                                        */
	/*   1) find the total memory required                            */
	/*   2) See if it fits continuously in TMU0                       */
	/*   3) See if it fits continuously in TMU1                       */
	/*   4) See if it fits with even TMU0 and odd TMU1 trilinear mode */
	/*   5) See if it fits with odd TMU0 and even TMU1 trilinear mode */
	/*   6) See if it fits into TMU0 with MultiBase                   */
	/*   7) See if it fits into TMU1 with MultiBase                   */
	/*   8) Else could not allocate                                   */
	/******************************************************************/

	if(text->attrib.combine_mode == TEXTURE_COMBINE_MODE_SINGLE_TMU)
	{ /* TEXTURE_COMBINE_MODE_SINGLE_TMU */
		/************************************************/
		/* Try to place continuously into TMU0 or TMU1: */
		/************************************************/
		if(memTMU0 >= memTMU1)
		{
			TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Attempting to load into TMU0\n");
			status = texSingleTmu0 (text, memBoth );
#if defined(VOODOO2)
			if(status == FALSE)
			{
				TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Failed to load into TMU0\n");
				status = texSingleTmu1 (text, memBoth );
			}
#endif
		}
#if defined(VOODOO2)
		else
		{
			TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Attempting to load into TMU1\n");
			status = texSingleTmu1 (text, memBoth );
			if(status == FALSE)
			{
				TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Failed to load into TMU1\n");
				status = texSingleTmu0 (text, memBoth );
			}
		}
#endif
            
		if(status == TRUE)
		{
			return(TRUE);            
		}

#if defined(VOODOO2)         
		TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Single TMU0 load failure\n");
#else
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, "Single TMU0 load failure\n");
#endif

#if defined(VOODOO2)
		/*****************************************************************/
		/* Try to Split the texture with trilinear across TMU0 and TMU1: */
		/*****************************************************************/
		if(memTMU0 >= memTMU1)
		{
			status = texTrilinearTmu0(text, memEven, memOdd);
			if(status == FALSE)
			{
				status = texTrilinearTmu1(text, memEven, memOdd);
			}
		}        
		else
		{
			status = texTrilinearTmu1(text, memEven, memOdd);
			if(status == FALSE)
			{
				status = texTrilinearTmu0(text, memEven, memOdd);
			}
		}            

		if(status == TRUE)
		{
			return(TRUE);            
		}
#endif
             
		/**************************************************************/
		/* Try to Split the texture into MultiBase into TMU0 or TMU1: */
		/**************************************************************/

		/* If it has same only one MipMap level then it should of */
		/* been stuffed above.                                    */

		/* If its has multiple MipMap levels but all are smaller  */
		/* than LOG2_32 then MultiBase Tmu mode will not work.    */
        
		if(text->info.smallLodLog2 != text->info.largeLodLog2 &&
			text->info.largeLodLog2 > GR_LOD_LOG2_32)
		{
			if(memTMU0 >= memTMU1)
			{
				status = texMultiBaseTmu0 (text, memBoth);
#if defined(VOODOO2)
				if(status == FALSE)
				{
					status = texMultiBaseTmu1 (text, memBoth );
				}
#endif
			}        
#if defined(VOODOO2)
			else
			{
				status = texMultiBaseTmu1 (text, memBoth );
				if(status == FALSE)
				{
					status = texMultiBaseTmu0 (text, memBoth );
				}
			}
#endif
		}
           
		if(status == TRUE)
		{
			return(TRUE);            
		}

		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Texture load failed\n");
		return(FALSE);

	} /* TEXTURE_COMBINE_MODE_SINGLE_TMU */
        
	/******************************************************************/
	/* For Multiple TMU support:                                      */
	/*   1) find the total memory required                            */
	/*   2) See if it fits with even TMU0 and odd TMU1 trilinear mode */
	/*   3) See if it fits with odd TMU0 and even TMU1 trilinear mode */
	/*   4) Try to stuff it into one TMU, send a warning.             */
	/*   	a) See if it fits continuously in TMU0                    */
	/*   	b) See if it fits continuously in TMU1                    */
	/*   	c) See if it fits into TMU0 with MultiBase                */
	/*   	d) See if it fits into TMU1 with MultiBase                */
	/*   5) Else could not allocate                                   */
	/******************************************************************/
         
        
	if(text->attrib.combine_mode == TEXTURE_COMBINE_MODE_TRILINEAR)
	{ /* TEXTURE_COMBINE_MODE_TRILINEAR */

#if defined(VOODOO2)        
		/*****************************************************************/
		/* Try to Split the texture with trilinear across TMU0 and TMU1: */
		/*****************************************************************/
		if(memTMU0 >= memTMU1)
		{
			status = texTrilinearTmu0 (text, memEven, memOdd );
			if(status == FALSE)
			{
				status = texTrilinearTmu1 (text, memEven, memOdd );
			}
		}        
		else
		{
			status = texTrilinearTmu1 (text, memEven, memOdd );
			if(status == FALSE)
			{
				status = texTrilinearTmu0 (text, memEven, memOdd );
			}
		}            
		if(status == TRUE)
		{
			return(TRUE);            
		}
#endif
		/************************************************/
		/* Try to place continuously into TMU0 or TMU1: */
		/************************************************/
		if(memTMU0 >= memTMU1)
		{
			status = texSingleTmu0 (text, memBoth );
#if defined(VOODOO2)
			if(status == FALSE)
			{
				status = texSingleTmu1 (text, memBoth );
			}
#endif
		}
#if defined(VOODOO2)
		else
		{
			status = texSingleTmu1 (text, memBoth );
			if(status == FALSE)
			{
				status = texSingleTmu0 (text, memBoth );
			}
		}
#endif
		if(status == TRUE)
		{
			return(TRUE);            
		}
         
		/**************************************************************/
		/* Try to Split the texture into MultiBase into TMU0 or TMU1: */
		/**************************************************************/

		/* If it has same only one MipMap level then it should of */
		/* been stuffed above.                                    */

		/* If its has multiple MipMap levels but all are smaller  */
		/* than LOG2_32 then MultiBase Tmu mode will not work.    */
        
		if(text->info.smallLodLog2 != text->info.largeLodLog2 &&
			text->info.largeLodLog2 > GR_LOD_LOG2_32 )
		{
			if(memTMU0 >= memTMU1)
			{
				status = texMultiBaseTmu0 (text, memBoth );
#if defined(VOODOO2)
				if(status == FALSE)
				{
					status = texMultiBaseTmu1 (text, memBoth );
				}
#endif
			}        
#if defined(VOODOO2)
			else
			{
				status = texMultiBaseTmu1 (text, memBoth );
				if(status == FALSE)
				{
					status = texMultiBaseTmu0 (text, memBoth );
				}
			}
#endif
		}                
                        
		if(status == TRUE)
		{
			return(TRUE);            
		}

		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Trilinear texture load failed\n");
		return(FALSE);

	} /* TEXTURE_COMBINE_MODE_TRILINEAR */

	TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Texture load failed - unknown combine mode\n");
	return(FALSE);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texSingleTmu0()                                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int texSingleTmu0 (Texture_node_t *text, int memory )
{
	Tmu_node_t		*tmp;
	unsigned int	freeMemory;
	unsigned int	*ptr;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p, %d)\n", text, memory);
	if(query_memory (memory, GR_TMU0, &tmp, &freeMemory))        
	{
		text->attrib.lodBlend = FXFALSE;
		text->attrib.tmu = GR_TMU0;
		text->attrib.evenOdd = GR_MIPMAPLEVELMASK_BOTH;
		text->multibase = NULL;
		text->trilinear = NULL;
		text->attrib.texBaseRange = NO_TEXBASERANGE;
		text->attrib.start_addr = get_memory (memory, tmp, text);
		text->attrib.size = memory;
		text->attrib.combine_mode = TEXTURE_COMBINE_MODE_SINGLE_TMU;

		TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Loading to TMU0 address: 0x%8.8X\n", text->attrib.start_addr);

		grTexDownloadMipMap(text->attrib.tmu,
									text->attrib.start_addr,
									text->attrib.evenOdd,
									&text->info );
    
		ptr = (unsigned int *)freeMemory;
		*ptr -= memory;

		text->id_number = id_number;

		return(TRUE);
	}
	else
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "TMU0 out of memory\n");
		return(FALSE);
	}            
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texSingleTmu1()                                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
#if defined(VOODOO2)
int texSingleTmu1 (Texture_node_t *text, int memory )
{
	Tmu_node_t		*tmp;
	unsigned int	freeMemory;
	unsigned int	*ptr;
		 
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p, %d)\n", text, memory);
	if(query_memory (memory, GR_TMU1, &tmp, &freeMemory))
	{
		text->attrib.lodBlend = FXFALSE;
		text->attrib.tmu = GR_TMU1;
		text->attrib.evenOdd = GR_MIPMAPLEVELMASK_BOTH;
		text->multibase = NULL;
		text->trilinear = NULL;
		text->attrib.texBaseRange = NO_TEXBASERANGE;
		text->attrib.combine_mode = TEXTURE_COMBINE_MODE_SINGLE_TMU;
		text->attrib.start_addr = get_memory (memory, tmp, text);
		text->attrib.size = memory;
            
		grTexDownloadMipMap(text->attrib.tmu,
									text->attrib.start_addr,
									text->attrib.evenOdd,
									&text->info );

		ptr = (unsigned int *) freeMemory;
		*ptr -= memory;

//		free(text->info.data);                          

		text->id_number = id_number;

		return(TRUE);
	}
	else
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "TMU1 out of memory\n");
		return(FALSE);
	}            
}
#endif


#if defined(VOODOO2)
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texTrilinearTmu0()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int texTrilinearTmu0 (Texture_node_t *text, int memEven, int memOdd )
{
	Texture_node_t	*text2;
	Texture_node_t	*text2prev;
	Texture_node_t	*text2next;
	Tmu_node_t		*tmp;
	Tmu_node_t		*tmp2;
	unsigned int	freeMemory;
	unsigned int	freeMemory2;
	unsigned int	*ptr;


	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p, %d, %d)\n", text, memEven, memOdd);
	if(text->info.smallLodLog2 == text->info.largeLodLog2)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Attempting trilinear with texture that has no mip-maps\n");
		return (FALSE);
	}

	if(query_memory (memEven, GR_TMU0, &tmp, &freeMemory) && 
		query_memory (memOdd, GR_TMU1, &tmp2, &freeMemory2))
	{
		//  yes even on TMU0 odd on TMU1
		text->attrib.lodBlend = FXTRUE;
		text->multibase = NULL;
		text->attrib.texBaseRange = NO_TEXBASERANGE;
		text->attrib.combine_mode = TEXTURE_COMBINE_MODE_TRILINEAR;

		text->attrib.evenOdd = GR_MIPMAPLEVELMASK_EVEN;
		text->attrib.tmu = GR_TMU0;
		text->attrib.start_addr  = get_memory (memEven, tmp, text);
		text->attrib.size = memEven;
		ptr = (unsigned int *) freeMemory;
		*ptr -= memEven;

        
		grTexDownloadMipMap(text->attrib.tmu,
									text->attrib.start_addr,
									text->attrib.evenOdd,
									&text->info );



		text2 = insert_texture_node ();
		text2next = text2->next;
		text2prev = text2->prev;
        
		memcpy (text2,text,sizeof(Texture_node_t));
		text->trilinear = text2;
		text2->next = text2next;
		text2->prev = text2prev;
                            
		text2->trilinear = NULL;
		text2->attrib.evenOdd = GR_MIPMAPLEVELMASK_ODD;
		text2->attrib.tmu = GR_TMU1;
		text2->attrib.start_addr = get_memory (memOdd, tmp2, text2);
		text2->attrib.size = memOdd;
                             
                            
		grTexDownloadMipMap(text2->attrib.tmu,
									text2->attrib.start_addr,
									text2->attrib.evenOdd,
									&text2->info );
                          
		ptr = (unsigned int *) freeMemory2;
		*ptr -= memOdd;

		text->id_number = id_number;
		text2->id_number = id_number;

//		free(text->info.data);                          
            
		return(TRUE);
	}
	else
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "TMU0 out of memory\n");
		return(FALSE);
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texTrilinearTmu1()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int texTrilinearTmu1 (Texture_node_t *text, int memEven, int memOdd )
{
	Texture_node_t	*text2;
	Texture_node_t	*text2next;
	Texture_node_t	*text2prev;
	Tmu_node_t		*tmp;
	Tmu_node_t		*tmp2;
	unsigned int	freeMemory;
	unsigned int	freeMemory2;
	unsigned int	*ptr;


	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p, %d, %d)\n", text, memEven, memOdd);
	if(text->info.smallLodLog2 == text->info.largeLodLog2)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Trilinear with texture that has no mip-maps\n");
		return (FALSE);
	}

	if(query_memory (memOdd, GR_TMU0, &tmp, &freeMemory) &&
		query_memory (memEven, GR_TMU1, &tmp2, &freeMemory2) )
	{
		//  yes even on TMU0 odd on TMU1
		text->attrib.evenOdd = GR_MIPMAPLEVELMASK_EVEN;
		text->attrib.tmu  = GR_TMU1;
		text->attrib.lodBlend = FXTRUE;
		text->multibase = NULL;
		text->attrib.texBaseRange = NO_TEXBASERANGE;
		text->attrib.combine_mode = TEXTURE_COMBINE_MODE_TRILINEAR;
		text->attrib.start_addr   = get_memory (memEven, tmp2, text);
		text->attrib.size = memEven;
        
		ptr = (unsigned int *)freeMemory;
		*ptr -= memOdd;        

		grTexDownloadMipMap(text->attrib.tmu,
									text->attrib.start_addr,
									text->attrib.evenOdd,
									&text->info );

		text2 = insert_texture_node ();
		text2next = text2->next;
		text2prev = text2->prev;
		memcpy (text2,text,sizeof(Texture_node_t));
		text->trilinear = text2;
		text2->next = text2next;
		text2->prev = text2prev;

		text2->trilinear = NULL;
		text2->attrib.evenOdd = GR_MIPMAPLEVELMASK_ODD;
		text2->attrib.tmu = GR_TMU0;
		text2->attrib.start_addr  = get_memory (memOdd, tmp, text2);
		text2->attrib.size = memOdd;

		grTexDownloadMipMap(text2->attrib.tmu,
									text2->attrib.start_addr,
									text2->attrib.evenOdd,
									&text2->info );
                          

		ptr = (unsigned int *)freeMemory2;
		*ptr -= memEven;

		text->id_number = id_number;
		text2->id_number = id_number;

//		free(text->info.data);                          
            
		return(TRUE);
	}
	else
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "TMU1 out of memory\n");
		return(FALSE);
	}
}
#endif



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texMultiBaseTmu0()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int texMultiBaseTmu0 (Texture_node_t *text, int memBoth )
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, __FUNCTION__"(%p, %d)\n", text, memBoth);
    return(FALSE);
}


#if defined(VOODOO2)
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texMultiBaseTmu1()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int texMultiBaseTmu1 (Texture_node_t *text,  int memBoth )
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, __FUNCTION__"(%p, %d)\n", text, memBoth);
	return(FALSE);
}
#endif


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: getTotalMemTmu()                                                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int getTotalMemTmu ( GrChipID_t tmu )
{
	int	size;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", (int)tmu);
	if(tmu == GR_TMU0)
	{
		size = MemoryInfo.freeMemTmu0_A + MemoryInfo.freeMemTmu0_B;
		return (size);
	}
	else if ( tmu == GR_TMU1 )
	{
		size = MemoryInfo.freeMemTmu1_A + MemoryInfo.freeMemTmu1_B;
		return (size);
	}
	if ( tmu == GR_TMU2 )
	{
		size = MemoryInfo.freeMemTmu2_A + MemoryInfo.freeMemTmu2_B;
		return (size);
	}                

	TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, "TMU %d has NO memory\n", (int)tmu);
	return(0);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: findTmuSection()                                                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
unsigned int *findTmuSection ( unsigned int addr, GrChipID_t tmu )
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d, %d)\n", addr, (int)tmu);
	if ( tmu == GR_TMU0 )
	{
		if ( addr >= MemoryInfo.minAddrTmu0_A && addr <= MemoryInfo.maxAddrTmu0_A )
		{
			return (&MemoryInfo.freeMemTmu0_A);
		}

		if ( addr >= MemoryInfo.minAddrTmu0_B && addr <= MemoryInfo.maxAddrTmu0_B )
		{
			return (&MemoryInfo.freeMemTmu0_B);
		}
		else
		{
			return (NULL);
		}
	}
	else if ( tmu == GR_TMU1 )
	{
		if ( addr >= MemoryInfo.minAddrTmu1_A && addr <= MemoryInfo.maxAddrTmu1_A )
		{
			return (&MemoryInfo.freeMemTmu1_A);
		}

		if ( addr >= MemoryInfo.minAddrTmu1_B && addr <= MemoryInfo.maxAddrTmu1_B )
		{
			return (&MemoryInfo.freeMemTmu1_B);
		}
		else
		{
			return (NULL);
		}
	}
	if ( tmu == GR_TMU2 )
	{
		if ( addr >= MemoryInfo.minAddrTmu2_A && addr <= MemoryInfo.maxAddrTmu2_A )
		{
			return (&MemoryInfo.freeMemTmu2_A);
		}

		if ( addr >= MemoryInfo.minAddrTmu2_B && addr <= MemoryInfo.maxAddrTmu2_B )
		{
			return (&MemoryInfo.freeMemTmu2_B);
		}
		else
		{
			return (NULL);
		}
 
	}                
	return(NULL);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: TextureSource()                                                 */
/*                                                                           */
/* This function sets all of the texture sources, mipmap levels, tables, and */
/* filter modes in the 3dfx/GLIDE registers for the specific input texture   */
/* handle.                                                                   */
/*                                                                           */
/* INPUT: (Texture_node_t *ptr) texture handle.                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void TextureSource( Texture_node_t *text )
{
	TextureAttrib_t	*attrib;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", text);
	attrib = &text->attrib;
    
	/* setup the texture combine modes */
	TextureCombine( attrib );
    
	/* Is there MultiBase texturing ? */
	if (text->attrib.texBaseRange != NO_TEXBASERANGE)
	{
		int i;  
		Texture_node_t *t;      

		/* Enable MultiBase Texturing */
		grTexMultibase (attrib->tmu, FXTRUE);
        
		/* Set up multibase addresses:                              */
		/* There is a maximum of 4 addresses to setup.  The largest */
		/* mip map does not need to be set since grTexSource() sets */
		/* it so there may at maximum 3 to setup.  The do loop is   */
		/* here as a safety net to not exceed 3 multibase address   */
		/* setups.                                                  */
		t = text;
		i = 2;
		do
		{        
			if (t->multibase != NULL)
			{
				t = t->multibase;
			}
			else
			{
				break;
			}                                
                
			if(t->attrib.tmu == NO_TEXBASERANGE)
			{
				break;
			}     
                           
			grTexMultibaseAddress(t->attrib.tmu,
											t->attrib.texBaseRange,
											t->attrib.start_addr,
											t->attrib.evenOdd,
											&t->info);
		} while (i--);
	}
	else
	{
		/* Disable MultiBase Texturing */
		grTexMultibase (GR_TMU0, FXFALSE);
#if defined(VOODOO2)
		grTexMultibase (GR_TMU1, FXFALSE);
#endif
	}        
	
	/* Set the lod bias value */
	grTexLodBiasValue(GR_TMU0, attrib->lod);

#if defined(VOODOO2)
	/* Set the lod bias value */
	grTexLodBiasValue(GR_TMU1, attrib->lod);
#endif

	/* Specify the texture Minification and magnification filters */
	grTexFilterMode( GR_TMU0,
							attrib->minFilterMode,
							attrib->magFilterMode );

#if defined(VOODOO2)
	/* Specify the texture Minification and magnification filters */
	grTexFilterMode( GR_TMU1,
							attrib->minFilterMode,
							attrib->magFilterMode );
#endif
     
	/* Set the Mip Mapping Mode */                     
	grTexMipMapMode( GR_TMU0,
							attrib->mipmapmode,
							attrib->lodBlend);
                     

#if defined(VOODOO2)
	/* Set the Mip Mapping Mode */                     
	grTexMipMapMode( GR_TMU1,
							attrib->mipmapmode,
							attrib->lodBlend);
#endif

	/* Set the Clamp Modes */
	grTexClampMode( GR_TMU0,
							attrib->s_clamp_mode,
							attrib->t_clamp_mode);

#if defined(VOODOO2)
	/* Set the Clamp Modes */
	grTexClampMode( GR_TMU1,
							attrib->s_clamp_mode,
							attrib->t_clamp_mode);
#endif

	/* download the table if there is one */                     
	if ( text->table != NULL ) 
	{
		grTexDownloadTable( text->table->tableType, &text->table->tableData );
	}

	/* Select Texture As Source of all texturing operations */
	grTexSource( attrib->tmu,
					attrib->start_addr,
					attrib->evenOdd,
					&text->info );

	if ( text->attrib.combine_mode == TEXTURE_COMBINE_MODE_TRILINEAR )
	{
		/* Select Texture As Source of all texturing operations */
		grTexSource( text->trilinear->attrib.tmu,
						text->trilinear->attrib.start_addr,
						text->trilinear->attrib.evenOdd,
						&text->trilinear->info );
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: TextureCombine()                                                */
/*                                                                           */
/* INPUT:                                                                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void TextureCombine( TextureAttrib_t *attrib )
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", attrib);
	if ( attrib->tmu == GR_TMU2 )
	{
		fprintf(stderr,"Cannot support TMU2 TextureCombine() \n\r");
	}
        
	if ( attrib->combine_mode == TEXTURE_COMBINE_MODE_SINGLE_TMU )
	{
		if ( attrib->tmu == GR_TMU0 )
		{
			grTexCombine( GR_TMU0,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_NONE,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_NONE,
							FXFALSE, FXFALSE );

#if defined(VOODOO2)
			grTexCombine( GR_TMU1,
							GR_COMBINE_FUNCTION_ZERO,
							GR_COMBINE_FACTOR_NONE,
							GR_COMBINE_FUNCTION_ZERO,
							GR_COMBINE_FACTOR_NONE,
							FXFALSE, FXFALSE );
#endif
	
		}
		else
		{
			grTexCombine( GR_TMU0,
							GR_COMBINE_FUNCTION_SCALE_OTHER,
							GR_COMBINE_FACTOR_ONE,
							GR_COMBINE_FUNCTION_SCALE_OTHER,
							GR_COMBINE_FACTOR_ONE,
							FXFALSE, FXFALSE );

#if defined(VOODOO2)
			grTexCombine( GR_TMU1,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_NONE,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_NONE,
							FXFALSE, FXFALSE );
#endif
		}            
	}
	else if ( attrib->combine_mode == TEXTURE_COMBINE_MODE_TRILINEAR )
	{
		/* This assumes that even LODs are sent to TMU0,    */
		/* odd LODs are sent to TMU1, and lodBlend is true. */
		if ( attrib->tmu == GR_TMU0 )
		{
			grTexCombine( GR_TMU0,
							GR_COMBINE_FUNCTION_BLEND,
							GR_COMBINE_FACTOR_LOD_FRACTION,
							GR_COMBINE_FUNCTION_BLEND,
							GR_COMBINE_FACTOR_LOD_FRACTION,
							FXFALSE, FXFALSE );

#if defined(VOODOO2)
			grTexCombine( GR_TMU1,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_ONE,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_ONE,
							FXFALSE, FXFALSE );
#endif
		}
		else
		{
			/* This assumes that odd LODs are sent to TMU0,    */
			/* even LODs are sent to TMU1, and lodBlend is true. */
			grTexCombine( GR_TMU0,
							GR_COMBINE_FUNCTION_BLEND,
							GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION,
							GR_COMBINE_FUNCTION_BLEND,
							GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION,
							FXFALSE, FXFALSE );

#if defined(VOODOO2)
			grTexCombine( GR_TMU1,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_ONE,
							GR_COMBINE_FUNCTION_LOCAL,
							GR_COMBINE_FACTOR_ONE,
							FXFALSE, FXFALSE );
#endif
		}           
	}
	else if ( attrib->combine_mode == TEXTURE_COMBINE_MODE_PROJECTED )
	{							  
		fprintf(stderr, "TextureCombine(): TEXTURE_COMBINE_MODE_PROJECTED not supported \n\r");
	}							 
	else if ( attrib->combine_mode == TEXTURE_COMBINE_MODE_COMPOSITE )
	{
		fprintf(stderr, "TextureCombine(): TEXTURE_COMBINE_MODE_COMPOSITE not supported \n\r");
	}        
	else
	{
		fprintf(stderr,"TextureCombine() combine_mode:%x not supported \n\r", (int)attrib->combine_mode);
	}        
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: resetTextureSystem()                                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void resetTextureSystem (void)
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	/* Init texture information */    
	initTextureInfo(&TmuInfo);
    
	/* Init the memory info structure */
	initMemoryInfo(&TmuInfo);

	/* check assumptions:  check that they are NULL.    */
	/*         tmu0_A,tmu0_B,tmu1_A,tmu1_B,text_list	*/
	/*         textFreeList,nodeFreeList,tableFreeList  */

	delete_table_node(&table_list);

	delete_texture_node(&text_list);

	delete_tmu_node(&tmu0_A);

	delete_tmu_node(&tmu0_B);

	delete_tmu_node(&tmu1_A);

	delete_tmu_node(&tmu1_B);

	delete_texture_node(&textFreeList);

	delete_tmu_node(&tmuFreeList);

	delete_table_node(&tableFreeList);

	initTmuFreeList(NUM_FREELIST_TMU_RESET);

	initTextFreeList(NUM_FREELIST_TEXTURE_RESET);

	initTableFreeList(NUM_FREELIST_TABLE_RESET);

	/* Init the headers and footers to the 4 tmu lists:     */
	/*    tmu0_A, tmu0_B, tmu1_A, tmu1_B                    */
	init_tmu_list();                      
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: initTableFreeList()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void initTableFreeList (int num)
{
	int	i;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", num);
	for (i=0;i<num;i++)
	{
		create_table_node ( &tableFreeList );
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: getTableFreeList()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Table_node_t *getTableFreeList (void)
{
	Table_node_t	*tmp;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	//
	// Are there any nodes on the free list ?
	//
	if(tableFreeList == NULL)
	{
		//
		// Nope - Add nodes to the free list
		//
		initTableFreeList(NUM_FREELIST_TEXTURE_DELTA);
	}

	//
	// Are there nodes on the free list now ?
	//
	if(!tableFreeList)
	{
		//
		// Nope - return NULL (error)
		//
		return(NULL);
	}

	//
	// Get first node from free list
	//
	tmp = tableFreeList;

	//
	// Reset free list to point at next node
	//
	tableFreeList = tableFreeList->next;

	//
	// Did we just take the last node off of the free list ?
	//
	if(tableFreeList)
	{
		//
		// Nope - Set first free list node back link to nowhere
		//
		tableFreeList->prev = NULL;
	}

	//
	// Return node
	//
	return(tmp);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: returnTableFreeList()                                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void returnTableFreeList (Table_node_t *tmp)
{
	Table_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tmp);
	if (tableFreeList == NULL)
	{
		/* create a new table struct with no linkage */
		tmp->next = NULL;
		tmp->prev = NULL;
		tableFreeList = tmp;
	}
	else 
	{
		/* insert table to the front of the list */
		tmp2 = tableFreeList;
		tmp2->prev = tmp;
		tmp->next = tmp2;
		tmp->prev = NULL;
		tableFreeList = tmp;
	}   
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: dump_TableFreeList()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void dump_TableFreeList (void)
{
	Table_node_t	*tmp = NULL;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	if (tableFreeList == NULL)
	{
		printf("tableFreeList = NULL\n");
	}
	else
	{
		tmp	= tableFreeList;
		printf("\n");
		printf("tableFreeList\n");
		while (tmp != NULL)
		{
			printf("node:%p next:%p prev:%p\n",tmp,tmp->next,tmp->prev);
			tmp = tmp->next;
		}
		printf("\n");

	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: initTextFreeList()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void initTextFreeList (int num)
{
	int	i;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", num);
	for (i=0;i<num;i++)
	{
		create_texture_node ( &textFreeList );
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: getTextFreeList()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Texture_node_t *getTextFreeList(void)
{
	Texture_node_t	*tmp;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	//
	// Are there any Texture nodes on the free list ?
	//
	if(textFreeList == NULL)
	{
		//
		// Nope - go add some
		//
		initTextFreeList(NUM_FREELIST_TEXTURE_DELTA);

		//
		// Did some get added ?
		//
		if(!textFreeList)
		{
			//
			// NOPE - Error
			//
#ifdef DEBUG
			fprintf(stderr, "getTextFreeList():  No Texture nodes available\n");
#endif
			return(NULL);
		}
	}

	//
	// Grab first node off of free list
	//
	tmp = textFreeList;

	//
	// Set new free list pointer
	//
	textFreeList = textFreeList->next;

	//
	// Is there a node on the free list ?
	//
	if(textFreeList)
	{
		//
		// YES - set it's backlink
		//
		textFreeList->prev = NULL;
	}	

	//
	// Return the node pointer
	//
	return(tmp);
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: returnTextFreeList()                                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void returnTextFreeList (Texture_node_t *tmp)
{
	Texture_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tmp);
	if (textFreeList == NULL)
	{
		/* create a new text struct with no linkage */
		tmp->next = NULL;
		tmp->prev = NULL;
		tmp->multibase = NULL;
		tmp->trilinear = NULL;
		tmp->tmu_list = NULL;
		tmp->table = NULL;
		textFreeList = tmp;
	}
	else 
	{
		/* insert text to the front of the list */
		tmp2 = textFreeList;
		tmp2->prev = tmp;
		tmp->next = tmp2;
		tmp->prev = NULL;
		tmp->trilinear = NULL;
		tmp->multibase = NULL;
		tmp->tmu_list = NULL;
		tmp->table = NULL;
		textFreeList = tmp;
	}   
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: dump_TextFreeList()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void dump_TextFreeList (void)
{
	Texture_node_t	*tmp = NULL;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	if (textFreeList == NULL)
	{
		printf("textFreeList = NULL\n");
	}
	else
	{
		tmp	= textFreeList;
		printf("\n");
		printf("textFreeList\n");
		while (tmp != NULL)
		{
			printf("node:%p next:%p prev:%p\n",tmp,tmp->next,tmp->prev);
			tmp = tmp->next;
		}
		printf("\n");
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: initTmuFreeList()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void initTmuFreeList (int num)
{
	int	i;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", num);
	for (i=0;i<num;i++)
	{
		create_tmu_node ( &tmuFreeList );
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: getTmuFreeList()                                                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Tmu_node_t *getTmuFreeList (void)
{
	Tmu_node_t	*tmp;
	Tmu_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	if (tmuFreeList == NULL)
	{
		initTmuFreeList (NUM_FREELIST_TMU_DELTA);
	}
	
	if (tmuFreeList->next == NULL)
	{
		tmp = tmuFreeList;
		tmuFreeList = NULL;
		return(tmp);
	}
	else
	{
		tmp = tmuFreeList;

		tmp2 = tmuFreeList->next;		
		tmp2->prev = NULL;

		tmuFreeList = tmp2;
		return(tmp);
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: returnTmuFreeList()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void returnTmuFreeList (Tmu_node_t *tmp)
{
	Tmu_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tmp);
	if (tmuFreeList == NULL)
	{
		/* create a new tmu struct with no linkage */
		tmp->next = NULL;
		tmp->prev = NULL;
		tmp->text = NULL;
		tmp->start = 0;
		tmp->size = 0;
		tmuFreeList = tmp;
	}
	else 
	{
		/* insert tmu to the front of the list */
		tmp2 = tmuFreeList;
		tmp2->prev = tmp;
		tmp->next = tmp2;
		tmp->prev = NULL;
		tmp->text = NULL;
		tmp->start = 0;
		tmp->size = 0;
		tmuFreeList = tmp;
	}   
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: dump_TmuFreeList()                                              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void dump_TmuFreeList (void)
{
	Tmu_node_t	*tmp = NULL;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	if(tmuFreeList == NULL)
	{
		printf("tmuFreeList = NULL\n");
	}
	else
	{
		tmp	= tmuFreeList;
		printf("\n");
		printf("tmuFreeList\n");
		while (tmp != NULL)
		{
			printf("node:%p next:%p prev:%p\n",tmp,tmp->next,tmp->prev);
			tmp = tmp->next;
		}
		printf("\n");
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: initTextureInfo()                                               */
/*                                                                           */
/* This function queries the 3dfx system to find the information about       */
/* the texture memory system.                                                */
/*                                                                           */
/* INPUT: (TmuBoardInfo_t *ptr) TMU info structure.                          */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void initTextureInfo (TmuBoardInfo_t *info)
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", info);
	if ( grGet(GR_REVISION_TMU,4,(signed long *)&info->revision_tmu) == 4 )
	{
	}
	else
	{
		fprintf(stderr,"ERROR: cannot find revision_tmu\n");
		info->revision_tmu = 0;
	}
        
	if ( grGet(GR_NUM_TMU,4,(signed long *)&info->num_tmu) == 4)
	{
	}
	else
	{
		fprintf(stderr,"ERROR: cannot find num_tmu\n");
		info->num_tmu = 0;
	}
        
	if ( grGet(GR_MEMORY_TMU,4,(signed long *)&info->memory_tmu) == 4 )
	{
	}
	else
	{
		fprintf(stderr,"ERROR: cannot find memory_tmu\n");
		info->memory_tmu = 0;
	}
	 
 
	if ( grGet(GR_TEXTURE_ALIGN,4,(signed long *)&info->texture_align) == 4 )
	{
	}
	else
	{
		fprintf(stderr,"ERROR: cannot find texture_align\n");
		info->texture_align = 0;
	}

	info->minAddrTmu0 = grTexMinAddress(GR_TMU0);
	info->minAddrTmu1 = grTexMinAddress(GR_TMU1);
	info->minAddrTmu2 = grTexMinAddress(GR_TMU2);

	info->maxAddrTmu0 = grTexMaxAddress(GR_TMU0);
	info->maxAddrTmu1 = grTexMaxAddress(GR_TMU1);
	info->maxAddrTmu2 = grTexMaxAddress(GR_TMU2);


	//printf("\n");
	//printf("debug texture memory\n");
	//printf("info->memory_tmu: %x\n",info->memory_tmu);
	//printf("info->minAddrTmu0: %x  \n ",info->minAddrTmu0  );
	//printf("info->minAddrTmu1: %x  \n ",info->minAddrTmu1  );
	//printf("info->minAddrTmu2: %x  \n ",info->minAddrTmu2  );
	//printf("info->maxAddrTmu0: %x  \n ",info->maxAddrTmu0  );
	//printf("info->maxAddrTmu1: %x  \n ",info->maxAddrTmu1  );
	//printf("info->maxAddrTmu2: %x  \n ",info->maxAddrTmu2  );
	//printf("\n");

}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: initMemoryInfo()                                                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void initMemoryInfo (TmuBoardInfo_t *info)
{
	int	align;
	int	total_mem;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", info);
	align = info->texture_align;

	if (info->num_tmu >= 1)
	{
		total_mem = info->maxAddrTmu0 - info->minAddrTmu0 + align;
		MemoryInfo.minAddrTmu0_A = info->minAddrTmu0;
		MemoryInfo.maxAddrTmu0_B = info->maxAddrTmu0 + align;
		MemoryInfo.minAddrTmu0_B = total_mem * 0.5f;
		MemoryInfo.maxAddrTmu0_A = total_mem * 0.5f;
		MemoryInfo.freeMemTmu0_A = MemoryInfo.maxAddrTmu0_A - MemoryInfo.minAddrTmu0_A;
		MemoryInfo.freeMemTmu0_B = MemoryInfo.maxAddrTmu0_B - MemoryInfo.minAddrTmu0_B;
	}

	if (info->num_tmu >= 2)
	{
		total_mem = info->maxAddrTmu1 - info->minAddrTmu1 + align;
		MemoryInfo.minAddrTmu1_A = info->minAddrTmu1;
		MemoryInfo.maxAddrTmu1_B = info->maxAddrTmu1 + align;
		MemoryInfo.minAddrTmu1_B = total_mem * 0.5f;
		MemoryInfo.maxAddrTmu1_A = total_mem * 0.5f;
		MemoryInfo.freeMemTmu1_A = MemoryInfo.maxAddrTmu1_A - MemoryInfo.minAddrTmu1_A;
		MemoryInfo.freeMemTmu1_B = MemoryInfo.maxAddrTmu1_B - MemoryInfo.minAddrTmu1_B;
	}
        
	if (info->num_tmu >= 3)
	{
		total_mem = info->maxAddrTmu2 - info->minAddrTmu2 + align;
		MemoryInfo.minAddrTmu2_A = info->minAddrTmu0;
		MemoryInfo.maxAddrTmu2_B = info->maxAddrTmu0 + align;
		MemoryInfo.minAddrTmu2_B = total_mem * 0.5f;
		MemoryInfo.maxAddrTmu2_A = total_mem * 0.5f;
		MemoryInfo.freeMemTmu2_A = MemoryInfo.maxAddrTmu2_A - MemoryInfo.minAddrTmu2_A;
		MemoryInfo.freeMemTmu2_B = MemoryInfo.maxAddrTmu2_B - MemoryInfo.minAddrTmu2_B;
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: texTableType()                                                  */
/*                                                                           */
/* Return the type of table needed for a given format.                       */
/*                                                                           */
/* INPUT: (GrTextureFormat_t format)                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
GrTexTable_t texTableType( GrTextureFormat_t format ) 
{
    GrTexTable_t	rv;
    
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%d)\n", (int)format);
	rv = (GrTexTable_t)NO_TABLE;
    
	switch( format ) 
	{
		case GR_TEXFMT_YIQ_422:
		case GR_TEXFMT_AYIQ_8422:
			rv = GR_TEXTABLE_NCC0;
			break;
		case GR_TEXFMT_P_8:
		case GR_TEXFMT_AP_88:
			rv = GR_TEXTABLE_PALETTE;
			break;
	}
        
	return rv;
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: create_table_node()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void create_table_node ( Table_node_t **node)
{
	Table_node_t	*tmp;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	//
	// Allocate a table node
	//
	tmp = (Table_node_t *)malloc(sizeof (Table_node_t));

	//
	// Did we get one ?
	//    
	if(tmp == NULL)
	{
		//
		// Nope - print error message
		//
		fprintf(stderr,"create_table_node():ERROR could not malloc Table_node_t\n");

		//
		// Return
		//
		return;
	}

	//
	// Is there anything on the list
	//
	if(*node)
	{
		//
		// YES - back link list to new node
		//
		(*node)->prev = tmp;
	}

	//
	// Forward link new node to existing list
	//
	tmp->next = *node;

	//
	// Initialize the remainder of the node fields
	//
	tmp->prev = NULL;

	//
	// Make list point start with new node
	//
	*node = tmp;
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: insert_table_node()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Table_node_t *insert_table_node ( void )
{
	Table_node_t	*tmp;
	Table_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	tmp = getTableFreeList();

	if(!tmp)
	{
	}

	/* nothing is in the list */
	if (table_list == NULL)
	{
		/* create a new table struct with no linkage */
		table_list = tmp;
		table_list->next = NULL;
		table_list->prev = NULL;
	}
	else 
	{
		/* insert table to the front of the list */
		tmp2 = table_list;
		tmp2->prev = tmp;
		tmp->next = tmp2;
		tmp->prev = NULL;
		table_list = tmp;
	}   
	return (tmp);        
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: delete_table_node()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void delete_table_node (Table_node_t **node)
{
	register Table_node_t	*next;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	while(*node)
	{
		next = (*node)->next;
		free(*node);
		*node = next;
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: remove_table_node()                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void remove_table_node (Table_node_t *table)
{
	Table_node_t	*tmp;
	Table_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", table);
	if (table == NULL)
		return;

	if  (table->prev == NULL && table->next == NULL )
	{
		returnTableFreeList(table);
		table_list = NULL;
	}
	else if  (table->prev == NULL && table->next != NULL)
	{
		table_list = table->next;
		table_list->prev = NULL;
		returnTableFreeList(table);
	}
	else if (table->prev != NULL && table->next == NULL)
	{
		tmp = table->prev;
		tmp->next = NULL;
		returnTableFreeList(table);
	}
	else //(table->prev != NULL && table->next != NULL)
	{
		tmp = table->prev;
		tmp2 = table->next;
		tmp->next =  tmp2;
		tmp2->prev = tmp;
		returnTableFreeList(table);
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: create_texture_node()                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void create_texture_node ( Texture_node_t **node)
{
	Texture_node_t	*tmp;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	//
	// Allocate a new texture node
	//
	tmp = (Texture_node_t *)malloc(sizeof(Texture_node_t));

	//
	// Did we get memory ?
	//    
	if(tmp == NULL)
	{
		//
		// Nope return error
		//
		fprintf(stderr,"create_texture_node():ERROR could not malloc Texture_node_t\n");

		return;
	}

	//
	// Are there any nodes on the list ?
	if(*node)
	{
		//
		// Yes - Back link the existing list to the new node
		//
		(*node)->prev = tmp;
	}

	//
	// Forward link new node to existing list
	//
	tmp->next = *node;

	//
	// Initialize the remainder of the new nodes fields
	//
	tmp->prev = NULL;
	tmp->trilinear = NULL;
	tmp->multibase = NULL;
	tmp->tmu_list = NULL;

	//
	// Set the new list pointer
	//
	*node = tmp;
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: insert_texture_node()                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Texture_node_t *insert_texture_node ( void )
{
	Texture_node_t	*tmp;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	//
	// Get the node from the free list
	//
	tmp = getTextFreeList();

	//
	// Did we get a node ?
	//
	if(!tmp)
	{
		//
		// NOPE - error
		//
		fprintf(stderr, "insert_texture_node():  No free texture nodes\n");
		return(NULL);
	}

	//
	// Is there anything on the texture list ?
	//
	if(text_list)
	{
		//
		// YES - Back link the the list to the new node
		//
		text_list->prev = tmp;
	}

	//
	// Forward link the new node to the existing list
	//
	tmp->next = text_list;

	//
	// Initialize the rest of the nodes fields
	//
	tmp->prev = NULL;
	tmp->multibase = NULL;
	tmp->trilinear = NULL;
	tmp->tmu_list = NULL;
	tmp->table = NULL;
	tmp->new_tex = 1;

	//
	// Set the new list pointer
	//
	text_list = tmp;

	//
	// Return the node pointer
	//
	return(tmp);        
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: delete_texture_node()                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void delete_texture_node (Texture_node_t **node)
{
	register Texture_node_t	*next;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	while(*node)
	{
		next = (*node)->next;
		free(*node);
		*node = next;
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: remove_texture_node()                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void remove_texture_node (Texture_node_t *text)
{
	Texture_node_t	*tmp;
	Texture_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", text);
	if(!text)
	{
		return;
	}
	if  (text->prev == NULL && text->next == NULL )
	{
		returnTextFreeList(text);
		text_list = NULL;
	}
	else if  (text->prev == NULL && text->next != NULL)
	{
		text_list = text->next;
		text_list->prev = NULL;
		returnTextFreeList(text);
	}
	else if (text->prev != NULL && text->next == NULL)
	{
		tmp = text->prev;
		tmp->next = NULL;
		returnTextFreeList(text);
	}
	else //(text->prev != NULL && text->next != NULL)
	{
		tmp = text->prev;
		tmp2 = text->next;
		tmp->next =  tmp2;
		tmp2->prev = tmp;
		returnTextFreeList(text);
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: delete_texture_handle()                                         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void delete_texture_handle(Texture_node_t *text)
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", text);
	if (text->trilinear != NULL)
	{
		remove_table_node(text->trilinear->table);
		remove_tmu_node(text->trilinear->tmu_list);
		remove_texture_node(text->trilinear);

		/* This happens above since both texture struct point to one table */
		/*		remove_table_node(text->table);	                       */
		remove_tmu_node(text->tmu_list);
		remove_texture_node(text);
	}
	else if (text->multibase != NULL)
	{
		/* FUTURE */
	}
	else
	{
		remove_table_node(text->table);	
		remove_tmu_node(text->tmu_list);
		remove_texture_node (text);
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: create_tmu_node()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void create_tmu_node ( Tmu_node_t **node )
{
	Tmu_node_t	*tmp;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	//
	// Allocate a TMU node
	//
	tmp = (Tmu_node_t *)malloc(sizeof(Tmu_node_t));

	//
	// Did we get one ?
	//    
	if(tmp == NULL)
	{
		//
		// NOPE
		//
#ifdef DEBUG
		fprintf(stderr,"create_tmu_node(): ERROR could not malloc Tmu_node_t\n");
#endif
		return;
	}

	//
	// Is the list pointer set ?
	//
	if(*node)
	{
		//
		// YES - Back link the current list beginning to the new node
		//
		(*node)->prev = tmp;
	}

	//
	// Forward link the new node to the existing list
	//
	tmp->next = *node;

	//
	// Initialize the rest of the node information
	//
	tmp->prev = NULL;
	tmp->text = NULL;
	tmp->size = 0;
	tmp->start = 0;

	//
	// Set the new list pointer
	//
	*node = tmp;
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: insert_tmu_node()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
Tmu_node_t *insert_tmu_node ( Tmu_node_t *node )
{
	Tmu_node_t	*tmp;
	Tmu_node_t	*tmp2;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	tmp = getTmuFreeList();

	/* Insert node after Key */
	tmp2 = node->next;
	tmp2->prev = tmp;
	tmp->next = tmp2;
	tmp->prev = node;
	node->next = tmp;

	return (tmp);        
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: delete_tmu_node()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void delete_tmu_node (Tmu_node_t **node)
{
	register Tmu_node_t	*next;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	while(*node)
	{
		next = (*node)->next;
		free(*node);
		*node = next;
	}
}



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: remove_tmu_node()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void remove_tmu_node (Tmu_node_t *node)
{
	Tmu_node_t		*tmp;
	Tmu_node_t		*tmp2;
	unsigned int	*freeMemory;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", node);
	if(!node)
	{
		return;
	}
	if (node->prev != NULL && node->next != NULL)
	{
		tmp = node->prev;
		tmp2 = node->next;
		tmp->next =  tmp2;
		tmp2->prev = tmp;

		freeMemory = findTmuSection ( node->start, node->text->attrib.tmu );
		*freeMemory += node->size;

		returnTmuFreeList (node);
	}
	else
	{
		fprintf(stderr,"remove_tmu_node(): ERROR unknown link list\n");
	}
}

#ifdef USE_3DF_FILES
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: LoadTexture()                                                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int LoadTexture( const char *filename, GrTexInfo *info, Table_node_t **table, Gu3dfInfo *tdfInfo ) 
{
	GrTexTable_t	tableType;
	int				rv = 0;

#ifdef DEBUG
	fprintf(stderr, "LoadTexture(%s, %p, %p, %p)\n", filename, info, table, tdfInfo);
#endif

	if(gu3dfGetInfo( filename, tdfInfo))
	{
		tdfInfo->data = malloc(tdfInfo->mem_required);
		if(!tdfInfo->data)
		{
#ifdef DEBUG
			fprintf(stderr, "LoadTexture():  Could not allocate memory for file: %s\n", filename);
#endif
			return(0);
		}
       
		if(gu3dfLoad(filename, tdfInfo))
		{
			info->smallLodLog2    = tdfInfo->header.small_lod;
			info->largeLodLog2    = tdfInfo->header.large_lod;
			info->aspectRatioLog2 = tdfInfo->header.aspect_ratio;

			info->format      = tdfInfo->header.format;
			info->data        = tdfInfo->data;
			tableType = texTableType(info->format);

			switch(tableType) 
			{
				case GR_TEXTABLE_NCC0:
				case GR_TEXTABLE_NCC1:
				case GR_TEXTABLE_PALETTE:
					*table = insert_table_node();
					(*table)->tableType = tableType;
					memcpy( &(*table)->tableData, &(tdfInfo->table), sizeof( TextureTable ) );
					break;
				default:
					break;
			}
			rv = 1;
		}
#ifdef DEBUG
		else
		{
			fprintf(stderr, "LoadTexture():  Could not load file %s\n", filename);
		}
#endif

		free(tdfInfo->data);
	}
#ifdef DEBUG
	else
	{
		fprintf(stderr, "LoadTexture():  Could not get info for file %s\n", filename);
	}
#endif
    
	return rv;
}
#endif

static Texture_node_t	*last_tex = 0;

void guTexSource(Texture_node_t *tex)
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tex);
	if(!tex)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Null texture node pointer\n");
		return;
	}
	if(tex->new_tex || last_tex != tex)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Setting texture: %p\n", tex);
		TextureSource(tex);
		tex->new_tex = 0;
		last_tex = tex;
	}
	TEXTURE_LOAD_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

void grFreeTexture(Texture_node_t *tex)
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p)\n", tex);
	if(!tex)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, 0, "Null texture node pointer\n");
		return;
	}
	delete_texture_handle(tex);
	TEXTURE_LOAD_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

void guTexMemReset(void)
{
	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(void)\n");
	resetTextureSystem();
	TEXTURE_LOAD_DBG_MSG(DEBUG_RETURNS, 0, "done\n");
}

#ifndef USE_3DF_FILES
Texture_node_t *guLoadTextureDirect(const char *FileName, Gu3dfInfo *Info, int sclamp, int tclamp, int minfilt, int maxfilt)
{
	int							img_fd;
	struct ffblk				ffblk;
	wms_header_t				*wms_header;
	unsigned long				gen_crc;
	unsigned long				stored_crc;
	unsigned long				fsize;
	int							retry = 4;
	Texture_node_t				*tex_node;

	TEXTURE_LOAD_DBG_MSG(DEBUG_MSGS, NULL, __FUNCTION__"(%s, %p, %d, %d, %d, %d)\n", FileName, Info, sclamp, tclamp, minfilt, maxfilt);
	//
	// Get the TMU texture list node
	//
	tex_node = insert_texture_node();

	//
	// Error or none left ?
	//
	if(!tex_node)
	{
		//
		// YES - Return null
		//
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 1, "Can not get texture node\n");
		return(NULL);
	}

try_again:
	if(retry < 4)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Retry attempt on file %s\n", FileName);
//		flush_disk_cache();
//		flush_disk_queue();
	}

	//
	// Get info about the file
	//
	if(findfirst(FileName, &ffblk, 0))
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Can not get info for file %s\n", FileName);
		if(retry)
		{
			retry--;
			goto try_again;
		}
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Set the file size
	//
	fsize = ffblk.ff_fsize;

	//
	// Allocate memory for the texture buffer
	//
	if((texture_buffer = realloc(texture_buffer, ffblk.ff_fsize)) == NULL)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Can not allocate memory for texture buffer: %s", FileName);
		delete_texture_handle(tex_node);
		return(NULL);
	}

	//
	// Open the file
	//
	if((img_fd = open(FileName, O_RDONLY|O_BINARY)) == -1)
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Cant open file %s - retrying\n", FileName);
		if(retry)
		{
			retry--;
			goto try_again;
		}
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Read the CRC from the front of the file
	//
	if(_read(img_fd, &stored_crc, 4) != 4)
	{
		close(img_fd);
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Cant read CRC: %s\n", FileName);
		if(retry)
		{
			retry--;
			goto try_again;
		}
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Adjust the file size
	//
	fsize -= 4;

	//
	// Read the entire file into buffer
	//
	if(_read(img_fd, texture_buffer, fsize) != fsize)
	{
		close(img_fd);
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Cant read file %s\n", FileName);
		if(retry)
		{
			retry--;
			goto try_again;
		}
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Close the file
	//
	close(img_fd);

	//
	// Generate a CRC on the data read (exclude the stored CRC)
	//
	gen_crc = crc((unsigned char *)texture_buffer, fsize);

	//
	// Check the CRC's
	//
	if(gen_crc != stored_crc)
	{
		//
		// First retry ?
		//
		if(retry == 4)
		{
			// YES - Show something on debug terminal
			TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "CRC error detected in data: %s\n", FileName);
		}

		//
		// Retry again ?
		//
		if(retry)
		{
			// YES
			retry--;
			goto try_again;
		}

		//
		// Could not get file in 4 retries - is there an audit function ?
		//
		if(tex_crc_fail_audit_func)
		{
			// YES - Call it
			tex_crc_fail_audit_func();
		}

		//
		// Show something on the debug terminal
		//
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "File Read Failure %s\n", FileName);

		//
		// Return mip map 0
		//
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Set the header pointer
	//
	wms_header = (wms_header_t *)texture_buffer;

	//
	// Set the texture data pointer
	//
	tex_ptr = (char *)(wms_header + 1);

	//
	// Check the version number from the file
	//
	if(wms_header->version != WMS_FILE_VERSION)
	{
		close(img_fd);
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Version mismatch on file %s\n", FileName);
		if(retry)
		{
			retry--;
			goto try_again;
		}
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Copy the 3df header to where it needs to be
	//
	memcpy(&Info->header, &wms_header->header, sizeof(Gu3dfHeader));

	//
	// Initialize the Texture node fields
	//
	tex_node->attrib.combine_mode = (wms_header->tri_mode==0?TEXTURE_COMBINE_MODE_SINGLE_TMU:TEXTURE_COMBINE_MODE_TRILINEAR);
	tex_node->attrib.lod = wms_header->bias;
	tex_node->attrib.minFilterMode = minfilt;
	tex_node->attrib.magFilterMode = maxfilt;
	tex_node->attrib.mipmapmode = wms_header->filter_mode;
	tex_node->attrib.s_clamp_mode = sclamp;
	tex_node->attrib.t_clamp_mode = tclamp;

	//
	// If necessary, read in the YIQ decompression table
	//
	if((Info->header.format == GR_TEXFMT_YIQ_422) || (Info->header.format == GR_TEXFMT_AYIQ_8422))
	{
		//
		// Copy the YIQ info to where it needs to be
		//
		tex_node->table = insert_table_node();
		if(tex_node->table)
		{
			tex_node->table->tableType = GR_TEXTABLE_NCC0;
			memcpy(&tex_node->table->tableData, tex_ptr, 48);
		}
		else
		{
			TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Cant create compression table node: %s\n", FileName);
		}

		//
		// Adjust the texture data pointer
		//
		tex_ptr += 48;
	}

	//
	// If necessary, read in the Palette
	//
	else if((Info->header.format == GR_TEXFMT_P_8) || (Info->header.format == GR_TEXFMT_AP_88))
	{
		//
		// Copy the palette data to where it needs to be
		//
		tex_node->table = insert_table_node();
		if(tex_node->table)
		{
			tex_node->table->tableType = GR_TEXTABLE_PALETTE;
			memcpy(&tex_node->table->tableData, tex_ptr, 1024);
		}
		else
		{
			TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Cant create compression table node: %s\n", FileName);
		}

		//
		// Adjust the texture data pointer
		//
		tex_ptr += 1024;
	}

	//
	// Initialize the texel data pointer
	//
	Info->data = tex_ptr;

	//
	// Initialize the GrTexInfo structure in the Texture node
	//
	tex_node->info.smallLodLog2 = GR_LOD_LOG2_256 - Info->header.small_lod;
	tex_node->info.largeLodLog2 = GR_LOD_LOG2_256 - Info->header.large_lod;
	tex_node->info.aspectRatioLog2 = GR_ASPECT_LOG2_8x1 - Info->header.aspect_ratio;
	tex_node->info.format = Info->header.format;
	tex_node->info.data = Info->data;

	//
	// Download the texture to a TMU
	//
	if(!texLoadTmuMemory(tex_node))
	{
		TEXTURE_LOAD_DBG_MSG(DEBUG_FAILS, 0, "Cant load texture to TMU: %s\n", FileName);
		delete_texture_handle(tex_node);
		return((void *)0);
	}

	//
	// Return the mipmap handle
	//
	return(tex_node);
}


Texture_node_t *guLoadTextureDirectFromMemory(char *buffer, Gu3dfInfo *Info, int sclamp, int tclamp, int minfilt, int maxfilt)
{
	wms_header_t	*wms_header;
	Texture_node_t	*tex_node;

	TEXTURE_LOAD_DBG_MSG(DEBUG_FENTRY, NULL, __FUNCTION__"(%p, %p, %d, %d, %d %d)\n", buffer, Info, sclamp, tclamp, minfilt, maxfilt);
	//
	// Get the TMU texture list node
	//
	tex_node = insert_texture_node();

	//
	// Error or none left ?
	//
	if(!tex_node)
	{
		//
		// YES - Return null
		//
#ifdef DEBUG
		fprintf(stderr, "guLoadTextureDirectFromMemory(): Can not get texture node\n");
#endif
		return(NULL);
	}

	//
	// Set the header pointer
	//
	wms_header = (wms_header_t *)buffer;

	//
	// Set the texture data pointer
	//
	tex_ptr = (char *)(wms_header + 1);

	//
	// Check the version number from the file
	//
	if(wms_header->version != WMS_FILE_VERSION)
	{
#ifdef DEBUG
		fprintf(stderr, "guLoadTextureDirectFromMemory(): Version mismatch on texture data\n");
		fprintf(stderr, "guLoadTextureDirectFromMemory(): Expected %d and got %d\n", (WMS_FILE_VERSION&~0x8000), (wms_header->version&~0x8000));
#endif
		delete_texture_handle(tex_node);
		return(0);
	}

	//
	// Copy the 3df header to where it needs to be
	//
	memcpy(&Info->header, &wms_header->header, sizeof(Gu3dfHeader));

	//
	// Initialize the Texture node fields
	//
	tex_node->attrib.combine_mode = (wms_header->tri_mode==0?TEXTURE_COMBINE_MODE_SINGLE_TMU:TEXTURE_COMBINE_MODE_TRILINEAR);
	tex_node->attrib.lod = wms_header->bias;
	tex_node->attrib.minFilterMode = minfilt;
	tex_node->attrib.magFilterMode = maxfilt;
	tex_node->attrib.mipmapmode = wms_header->filter_mode;
	tex_node->attrib.s_clamp_mode = sclamp;
	tex_node->attrib.t_clamp_mode = tclamp;

	//
	// If necessary, read in the YIQ decompression table
	//
	if((Info->header.format == GR_TEXFMT_YIQ_422) || (Info->header.format == GR_TEXFMT_AYIQ_8422))
	{
		//
		// Copy the YIQ info to where it needs to be
		//
		tex_node->table = insert_table_node();
		if(tex_node->table)
		{
			tex_node->table->tableType = GR_TEXTABLE_NCC0;
			memcpy(&tex_node->table->tableData, tex_ptr, 48);
		}
		else
		{
#ifdef DEBUG
			fprintf(stderr, "guLoadTextureDirectFromMemory(): Could not create compression table node\n");
#endif
		}

		//
		// Adjust the texture data pointer
		//
		tex_ptr += 48;
	}

	//
	// If necessary, read in the Palette
	//
	else if((Info->header.format == GR_TEXFMT_P_8) || (Info->header.format == GR_TEXFMT_AP_88))
	{
		//
		// Copy the palette data to where it needs to be
		//
		tex_node->table = insert_table_node();
		if(tex_node->table)
		{
			tex_node->table->tableType = GR_TEXTABLE_PALETTE;
			memcpy(&tex_node->table->tableData, tex_ptr, 1024);
		}
		else
		{
#ifdef DEBUG
			fprintf(stderr, "guLoadTextureDirectFromMemory(): Could not create compression table node\n");
#endif
		}

		//
		// Adjust the texture data pointer
		//
		tex_ptr += 1024;
	}

	//
	// Initialize the texel data pointer
	//
	Info->data = tex_ptr;

	//
	// Initialize the GrTexInfo structure in the Texture node
	//
	tex_node->info.smallLodLog2 = GR_LOD_LOG2_256 - Info->header.small_lod;
	tex_node->info.largeLodLog2 = GR_LOD_LOG2_256 - Info->header.large_lod;
	tex_node->info.aspectRatioLog2 = GR_ASPECT_LOG2_8x1 - Info->header.aspect_ratio;
	tex_node->info.format = Info->header.format;
	tex_node->info.data = Info->data;

	//
	// Download the texture to a TMU
	//
	texLoadTmuMemory(tex_node);

	//
	// Return the mipmap handle
	//
	return(tex_node);
}

#else	// USE_3DF_FILES

Texture_node_t *guLoadTextureDirect(const char *FileName, Gu3dfInfo *Info, int sclamp, int tclamp, int minfilt, int maxfilt)
{
	Texture_node_t	*tn;

#ifdef DEBUG
	fprintf(stderr, "Loading texture: %s\n", FileName);
#endif
	tn = Load_3dfx_texture(FileName,
							TEXTURE_COMBINE_MODE_SINGLE_TMU,
							0.0f,
							minfilt,
							maxfilt,
							GR_MIPMAP_NEAREST,
							Info);
#ifdef DEBUG
	if(!tn)
	{
		fprintf(stderr, "Could not load texture: %s\n", FileName);
	}
#endif
	return(tn);
}

Texture_node_t *guLoadTextureDirectFromMemory(char *buffer, Gu3dfInfo *Info, int sclamp, int tclamp, int minfilt, int maxfilt)
{
	return(NULL);
}
#endif


/*---------------------------------------------------------------------------
** guTexCombineFunction
**                              
** Sets the texture combine function.  For a dual TMU system this function
** will configure the TEXTUREMODE registers as appropriate.  For a
** single TMU system this function will configure TEXTUREMODE if
** possible, or defer operations until grDrawTriangle() is called.
*/
static GrChipID_t	last_tmu = -1;
static GrTextureCombineFnc_t	last_tc = -1;

GR_DIENTRY(guTexCombineFunction, void,
           (GrChipID_t tmu, GrTextureCombineFnc_t tc))
{
	if(last_tmu == tmu &&
		last_tc == tc)
	{
		return;
	}
	last_tmu = tmu;
	last_tc = tc;

  switch ( tc )  {
  case GR_TEXTURECOMBINE_ZERO:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_ZERO, GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_ZERO, GR_COMBINE_FACTOR_NONE, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_DECAL:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_ONE:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_ZERO, GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_ZERO, GR_COMBINE_FACTOR_NONE, FXTRUE, FXTRUE );
    break;

  case GR_TEXTURECOMBINE_ADD:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL, GR_COMBINE_FACTOR_ONE,
                  GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL, GR_COMBINE_FACTOR_ONE, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_MULTIPLY:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL,
                  GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_DETAIL:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_ONE_MINUS_DETAIL_FACTOR,
                  GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_ONE_MINUS_DETAIL_FACTOR, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_DETAIL_OTHER:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_DETAIL_FACTOR,
                  GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_DETAIL_FACTOR, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_TRILINEAR_ODD:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION,
                  GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_TRILINEAR_EVEN:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_LOD_FRACTION,
                  GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_LOD_FRACTION, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_SUBTRACT:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL, GR_COMBINE_FACTOR_ONE,
                  GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL, GR_COMBINE_FACTOR_ONE, FXFALSE, FXFALSE );
    break;

  case GR_TEXTURECOMBINE_OTHER:
    grTexCombine( tmu, GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE,
                  GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE, FXFALSE, FXFALSE );
    break;

  default:
    GrErrorCallback( "guTexCombineFunction:  Unsupported function", FXTRUE );
    break;
  }
} /* guTexCombineFunction */

/*---------------------------------------------------------------------------
** grTexCombineFunction - obsolete
**                              
*/
static GrChipID_t	last_gr_tmu = -1;
static GrTextureCombineFnc_t	last_gr_tc = -1;

GR_DIENTRY(grTexCombineFunction, void,
           (GrChipID_t tmu, GrTextureCombineFnc_t tc)) 
{
	if(last_gr_tmu == tmu &&
		last_gr_tc == tc)
	{
		return;
	}
	last_gr_tmu = tmu;
	last_gr_tc = tc;
  guTexCombineFunction( tmu, tc );
}

GR_ENTRY(guTexChangeLodBias, void, (Texture_node_t *tex, float lod_bias))
{
	tex->attrib.lod = lod_bias;
}

void delay_us(int count)
{
	count *= 25;

	while(--count) ;
}

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
									GrTextureFilterMode_t	magFilterMode)
{
	if(!tex)
	{
		return(0);
	}

	if(width != -1)
	{
	}

	if(height != -1)
	{
	}

	if(fmt != -1)
	{
		tex->info.format = fmt;
	}

	if(mm_mode != -1)
	{
		tex->attrib.mipmapmode = mm_mode;
	}

	if(smallest_lod != -1)
	{
		tex->info.smallLodLog2 = smallest_lod;
	}

	if(largest_lod != -1)
	{
		tex->info.largeLodLog2 = largest_lod;
	}

	if(aspect != -1)
	{
		tex->info.aspectRatioLog2 = aspect;
	}

	if(s_clamp_mode != -1)
	{
		tex->attrib.s_clamp_mode = s_clamp_mode;
	}

	if(t_clamp_mode != -1)
	{
		tex->attrib.t_clamp_mode = t_clamp_mode;
	}

	if(minFilterMode != -1)
	{
		tex->attrib.minFilterMode = minFilterMode;
	}

	if(magFilterMode != -1)
	{
		tex->attrib.magFilterMode = magFilterMode;
	}

	return(1);
}

#define	HRESf	512.0f
#define	VRESf	384.0f

#define CLIP_XMIN	0.0f
#define CLIP_XMAX	(HRESf)
#define CLIP_YMIN	0.0f
#define CLIP_YMAX	(VRESf)

#define TACCEPT_OVERANGE	50.0f

static void calcParams(const MidVertex *a, const MidVertex *b, MidVertex *isect, float d)
{
	isect->oow = a->oow + d * ( b->oow - a->oow );
	isect->sow = a->sow + d * ( b->sow - a->sow );
	isect->tow = a->tow + d * ( b->tow - a->tow );
} /* calcParams */



static void intersectTop( const MidVertex *a, const MidVertex *b, MidVertex *intersect )
{
	float	d = (CLIP_YMIN - a->y) / (b->y - a->y);
  
	intersect->x = a->x + d * ( b->x - a->x );
	intersect->y = CLIP_YMIN;

	intersect->x += (float)(1<<21);
	intersect->x -= (float)(1<<21);
  
	calcParams(a, b, intersect, d);

} /* intersectTop */

static void intersectBottom( const MidVertex *a, const MidVertex *b, MidVertex *intersect )
{
	float	d = ( CLIP_YMAX - a->y ) / ( b->y - a->y );
  
	intersect->x = a->x + d * ( b->x - a->x );
	intersect->y = CLIP_YMAX; 

	intersect->x += (float)(1<<21);
	intersect->x -= (float)(1<<21);

	calcParams(a, b, intersect, d);

} /* intersectBottom */

static void intersectRight( const MidVertex *a, const MidVertex *b, MidVertex *intersect )
{
	float d = ( CLIP_XMAX - a->x ) / ( b->x - a->x );
  
	intersect->x = CLIP_XMAX;
	intersect->y = a->y + d * ( b->y - a->y );

	intersect->y += (float)(1<<21);
	intersect->y -= (float)(1<<21);

	calcParams(a, b, intersect, d);

} /* intersectRight */

static void intersectLeft( const MidVertex *a, const MidVertex *b, MidVertex *intersect )
{
	float	d = ( CLIP_XMIN - a->x ) / ( b->x - a->x );
  
	intersect->x = CLIP_XMIN;
	intersect->y = a->y + d * ( b->y - a->y );

	intersect->y += (float)(1<<21);
	intersect->y -= (float)(1<<21);

	calcParams(a, b, intersect, d);

} /* intersectLeft */

static void intersectFront( const MidVertex *a, const MidVertex *b, MidVertex *intersect )
{
	float	d = ( 1.0F - a->oow ) / ( b->oow - a->oow );
  
	intersect->x = a->x + d * ( b->x - a->x );
	intersect->y = a->y + d * ( b->y - a->y );

	intersect->x += (float)(1<<21);
	intersect->x -= (float)(1<<21);

	intersect->y += (float)(1<<21);
	intersect->y -= (float)(1<<21);

	calcParams(a, b, intersect, d);

} /* intersectFront */


static FxBool aboveYMin(const MidVertex *p)
{
	return (( p->y > CLIP_YMIN ) ? FXTRUE : FXFALSE);
	
} /* aboveYMin */

static FxBool belowYMax(const MidVertex *p)
{
	return (( p->y < CLIP_YMAX ) ? FXTRUE : FXFALSE);
} /* belowYMax */

static FxBool aboveXMin(const MidVertex *p)
{
	return (( p->x > CLIP_XMIN ) ? FXTRUE : FXFALSE );
} /* aboveXMin */

static FxBool belowXMax(const MidVertex *p)
{
	return (( p->x < CLIP_XMAX ) ? FXTRUE : FXFALSE );
} /* belowXMax */

static FxBool aboveZMin(const MidVertex *p)
{
	return ((p->oow > 0.0F) && (p->oow <= 1.0F) ? FXTRUE : FXFALSE );
} /* aboveZMin */


/*
** shClipPolygon
*/
static void
shClipPolygon(
              const MidVertex invertexarray[],
              MidVertex outvertexarray[],
              int inlength, int *outlength,
              FxBool (*inside)(const MidVertex *p),
              void (*intersect)(
                                const MidVertex *a,
                                const MidVertex *b,
                                MidVertex *intersect )
              ) {
  MidVertex
    *s, *p;
  int
    j;
  
  *outlength = 0;
  
  s = (MidVertex *)(invertexarray + inlength - 1);
  for ( j = 0; j < inlength; j++ ) {
    p = (MidVertex *)(invertexarray + j);
    if ( inside( p ) ) {
      if ( inside( s ) ) {
        outvertexarray[*outlength] = *p;
        (*outlength)++;
      }else {
        intersect( s, p, outvertexarray + *outlength );
        (*outlength)++;
        outvertexarray[*outlength] = *p;
        (*outlength)++;
      }
    } else {
      if ( inside( s ) ) {
        intersect( s, p, outvertexarray + *outlength );
        (*outlength)++;
      }
    }
    s = p;
  }
} /* shClipPolygon */


static MidVertex	output_array[8];
static MidVertex	output_array2[8];
static int			outlength;

/*
** shClipFirstPolygon
*/
static void
shClipFirstPolygon(
			  const MidVertex *in1,
			  const MidVertex *in2,
			  const MidVertex *in3,
              MidVertex outvertexarray[],
              int *outlength,
              FxBool (*inside)(const MidVertex *p),
              void (*intersect)(
                                const MidVertex *a,
                                const MidVertex *b,
                                MidVertex *intersect )
              ) {
  MidVertex
    *s, *p, intersection;
  int
    j;
  
  *outlength = 0;
  
  s = (MidVertex *)in3;

  for ( j = 0; j < 3; j++ ) {
	p = j ? (j == 1 ? (MidVertex *)in2 : (MidVertex *)in3 ) : (MidVertex *)in1;
    if ( inside( p ) ) {
      if ( inside( s ) ) {
        outvertexarray[*outlength] = *p;
        (*outlength)++;
      }else {
        intersect( s, p, &intersection );
        outvertexarray[*outlength] = intersection;
        (*outlength)++;
        outvertexarray[*outlength] = *p;
        (*outlength)++;
      }
    } else {
      if ( inside( s ) ) {
        intersect( s, p, &intersection );
        outvertexarray[*outlength] = intersection;
        (*outlength)++;
      }
    }
    s = p;
  }
} /* shClipPolygon */


/*
** guDrawTriangleWithClip
**
** NOTE:  This routine snaps vertices by adding a large number then
** subtracting that same number again.  In order for this to work
** you MUST set up the FPU to work in single precision mode.  Code
** to perform this is listed in the Appendix to the Glide Programmer's
** Guide.
*/
#define SNAP_CONSTANT (float)(1L<<21);

int	snap_n_draw_time = 0, snap_n_draw_cnt = 0;
int clip_n_render_time = 0, clip_n_render_cnt = 0;

void show_tdata(MidVertex *a, MidVertex *b, MidVertex *c);

int	cull_enable = 1;


void guDrawTriangleWithClip(const MidVertex *a, const MidVertex *b, const MidVertex *c )
{
//	register int			i;

	// If the triangle is in front of the camera but too close - throw it out
	if(a->oow > 1.0F &&	b->oow > 1.0F &&	c->oow > 1.0F)
	{
		return;
	}

	// If the triangle is behind the camera - throw it out
	if(a->oow <= 0.0F && b->oow <= 0.0f && c->oow <= 0.0F)
	{
		return;
	}

	// If the triangle is off screen to the left - throw it out
	if((a->x * a->oow) <= 0.0F && (b->x * b->oow) <= 0.0F && (c->x * c->oow) <= 0.0F)
	{
		return;
	}

	// If the triangle if off the bottom of the screen - throw it out
	if((a->y * a->oow) <= 0.0F && (b->y * b->oow) <= 0.0F && (c->y * c->oow) <= 0.0F)
	{
		return;
	}

	// If the triangle is in front of the camera
	if(a->oow > 0.0f && b->oow >= 0.0f && c->oow > 0.0f)
	{
		// If the triangle is above the top of the screen - throw it out
		if(a->y >= 384.0F &&	b->y >= 384.0F &&	c->y >= 384.0F)
		{
			return;
		}

		// If the triangle is off the right side of the screen - throw it out
		if(a->x >= 512.0F &&	b->x >= 512.0F &&	c->x >= 512.0F)
		{
			return;
		}
	}

	/*
	** perform trivial accept
	*/
	// If the triangle is within x pixels of the edges of screen - just draw it
	if (
		( a->x >= (CLIP_XMIN - TACCEPT_OVERANGE)) &&
		( a->x <  (CLIP_XMAX + TACCEPT_OVERANGE)) &&
		( a->y >= (CLIP_YMIN - TACCEPT_OVERANGE)) &&
		( a->y <  (CLIP_YMAX + TACCEPT_OVERANGE)) &&
		( b->x >= (CLIP_XMIN - TACCEPT_OVERANGE)) &&
		( b->x <  (CLIP_XMAX + TACCEPT_OVERANGE)) &&
		( b->y >= (CLIP_YMIN - TACCEPT_OVERANGE)) &&
		( b->y <  (CLIP_YMAX + TACCEPT_OVERANGE)) &&
		( c->x >= (CLIP_XMIN - TACCEPT_OVERANGE)) &&
		( c->x <  (CLIP_XMAX + TACCEPT_OVERANGE)) &&
		( c->y >= (CLIP_YMIN - TACCEPT_OVERANGE)) &&
		( c->y <  (CLIP_YMAX + TACCEPT_OVERANGE)) &&
		( a-> oow > 0.0F ) && ( a->oow <= 1.0F ) &&
		( b-> oow > 0.0F ) && ( b->oow <= 1.0F ) &&
		( c-> oow > 0.0F ) && ( c->oow <= 1.0F ))
		{
	    grDrawTriangleDma((MidVertex *)a, (MidVertex *)b, (MidVertex *)c, cull_enable );
	    return;
	  }

	//
	// go ahead and clip and render
	//

	// Clip the triangle - NOTE - also does vertex snapping
	shClipFirstPolygon( a, b, c, output_array,  &outlength, aboveZMin, intersectFront );
	shClipPolygon( output_array,  output_array2, outlength, &outlength, belowXMax, intersectRight );
	shClipPolygon( output_array2, output_array,  outlength, &outlength, belowYMax, intersectBottom );
	shClipPolygon( output_array,  output_array2, outlength, &outlength, aboveXMin, intersectLeft );
	shClipPolygon( output_array2, output_array,  outlength, &outlength, aboveYMin, intersectTop );

	// Draw the triangles generated by the clipper
#if 0
	for(i = 1; i < outlength - 1; i++)
	{
		grDrawTriangleDma(&output_array[0], &output_array[i], &output_array[i+1], cull_enable);
	}
#else
	if(outlength < 3) return;
	grDrawFan((const float *)output_array, outlength);
#endif

} /* guDrawTriangleWithClip */


#if 0
static float cull_check(const float *va, const float *vb, const float *vc)
{
	register float			area = 1.0f;
	register float			dxAB, dxBC;

	if(va[1] < vb[1])
	{
		if(vb[1] > vc[1])
		{        /* acb */
			if(va[1] < vc[1])
			{
				area = -1.0f;
				dxAB = va[0] - vc[0];
				dxBC = vc[0] - vb[0];

				dxBC *= (va[1] - vc[1]);
				dxAB *= (vc[1] - vb[1]);
			}
			else
			{                  /* cab */
				/* Compute Area */
				dxAB = vc[0] - va[0];
				dxBC = va[0] - vb[0];

				dxBC *= (vc[1] - va[1]);
				dxAB *= (va[1] - vb[1]);
			}
		}
		else
		{
			dxAB = va[0] - vb[0];
			dxBC = vb[0] - vc[0];

			dxBC *= (va[1] - vb[1]);
			dxAB *= (vb[1] - vc[1]);
		}
	}
	else
	{
		if(vb[1] < vc[1])
		{        /* bac */
			if(va[1] < vc[1])
			{
				area = -1.0f;
				/* Compute Area */
				dxAB = vb[0] - va[0];
				dxBC = va[0] - vc[0];

				dxBC *= (vb[1] - va[1]);
				dxAB *= (va[1] - vc[1]);
			}
			else
			{                  /* bca */
				/* Compute Area */
				dxAB = vb[0] - vc[0];
				dxBC = vc[0] - va[0];

				dxBC *= (vb[1] - vc[1]);
				dxAB *= (vc[1] - va[1]);
			}
		}
		else
		{                    /* cba */
			area = -1.0f;
			/* Compute Area */
			dxAB = vc[0] - vb[0];
			dxBC = vb[0] - va[0];

			dxBC *= (vc[1] - vb[1]);
			dxAB *= (vb[1] - va[1]);
		}
	}

	area *= (dxAB - dxBC);

	return(area);
}
#endif




/*****************************************************************************/
/* PRINT: ********************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: dump_tmu_list()                                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void dump_tmu_list (Tmu_node_t *node)

{ /* dump_tmu_list() */

/*****************************************************************************/

	printf("\n");
	printf("Cntr Tmu Start  Size   Next   T O Mip  Lod  Fil Text     Multi    Tril     Table    TmuNode\n");

	while (node != NULL )
		{
		print_tmu_list (node);
		node = node->next;
		}

} /* dump_tmu_list() */

/***** End of dump_tmu_list() ************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: print_tmu_list ()                                               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void print_tmu_list (Tmu_node_t *list)

{ /* printf_tmu_list() */

Texture_node_t *text;
char odd_char;
char magfilt_char;
char minfilt_char;



/*****************************************************************************/

	if ( list->text == NULL && list->prev == NULL )
		{
		printf("      H  %06x %06x %06x %x %x %x:%x %+3.2f %x:%x %08x %08x %08x %08x %08x\n",
                //H,//text->attrib.tmu,
                list->start,//text->attrib.start_addr,
                list->size,//text->attrib.size,
				0,//text->attrib.size+text->attrib.start_addr,
                0,//text->attrib.combine_mode,
                0,//text->attrib.evenOdd,
                0,//text->info.largeLodLog2,
                0,//text->info.smallLodLog2,
                0.0f,//text->attrib.lod,
                0,//text->attrib.minFilterMode,
                0,//text->attrib.magFilterMode,
                (unsigned int)list->text,//text,
                0,//text->multibase,
                0,//text->trilinear
                0,
                (unsigned int)list);
								  		   
		}

	else if ( list->text == NULL && list->next == NULL )
		{
   		printf("      F  %06x %06x %06x %x %x %x:%x %+3.2f %x:%x %08x %08x %08x %08x %08x\n",
                //F,//text->attrib.tmu,
                list->start,//text->attrib.start_addr,
                list->size,//text->attrib.size,
				0,//text->attrib.size+text->attrib.start_addr,
                0,//text->attrib.combine_mode,
                0,//text->attrib.evenOdd,
                0,//text->info.largeLodLog2,
                0,//text->info.smallLodLog2,
                0.0f,//text->attrib.lod,
                0,//text->attrib.minFilterMode,
                0,//text->attrib.magFilterMode,
                (unsigned int)list->text,//text,
                0,//text->multibase,
                0, //text->trilinear
                0, // table
                (unsigned int)list);

		}

	else if ( list->text != NULL && list->prev != NULL && list->next != NULL )
		{
		text = list->text;
		if (text != NULL)
			{
			if (text->attrib.evenOdd == 1)
				odd_char = 'E';
			else if (text->attrib.evenOdd == 2)
				odd_char = 'O';
			else if (text->attrib.evenOdd == 3)
				odd_char = 'B';
			else
				odd_char = 'X';

			if (text->attrib.minFilterMode == GR_TEXTUREFILTER_POINT_SAMPLED)
				minfilt_char = 'P';
			else if (text->attrib.minFilterMode == GR_TEXTUREFILTER_BILINEAR)
				minfilt_char = 'B';
			else
				{
				minfilt_char = 'X';
				}


			if (text->attrib.magFilterMode == GR_TEXTUREFILTER_POINT_SAMPLED)
				magfilt_char = 'P';
			else if (text->attrib.magFilterMode == GR_TEXTUREFILTER_BILINEAR)
				magfilt_char = 'B';
			else
				{
				magfilt_char = 'X';
				}
		
			
   			printf("%04d  %d  %06x %06x %06x %x %c %x:%x %+3.2f %c:%c %08x %08x %08x %08x %08x\n",
				text->id_number,
                (int)text->attrib.tmu,
                text->attrib.start_addr,
                text->attrib.size,
				text->attrib.size+text->attrib.start_addr,
                (int)text->attrib.combine_mode,
                odd_char,
                (int)text->info.largeLodLog2,
                (int)text->info.smallLodLog2,
                text->attrib.lod,
                minfilt_char,
                magfilt_char,
                (int)text,//text,
                (int)text->multibase,//text->multibase,
                (int)text->trilinear,
				(int)text->table,
                (int)list);

			 }
		 else
		 	{
			 printf("text == NULL\n");
			 }

		}
	else
		{
		printf("did not recognize node\n");
		}


} /* printf_tmu_list() */

/***** End of print_tmu_list() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: dump_all_textures ()                                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void dump_all_textures(void)
{

int memTMU0;
int memTMU1;
int	total_mem; 
int	total_used;  
int	total_avail; 
float percent_used;
float percent_avail;


/*****************************************************************************/

	printf("\ntmu0_A:");
	dump_tmu_list (tmu0_A);  
	printf("\ntmu0_B:");
	dump_tmu_list (tmu0_B);  
	printf("\ntmu1_A:");
	dump_tmu_list (tmu1_A);  
	printf("\ntmu1_B:");
	dump_tmu_list (tmu1_B);  

    memTMU0 = getTotalMemTmu ( GR_TMU0 );
    memTMU1 = getTotalMemTmu ( GR_TMU1 );

	printf("so far so good\n");

	printf("\n");
	total_mem   = TmuInfo.num_tmu * TmuInfo.memory_tmu;
	total_avail  = memTMU0 + memTMU1;												    
	total_used = total_mem - total_avail;
	percent_used = ((float)total_used/(float)total_mem)*100.0f;
	percent_avail = ((float)total_avail/(float)total_mem)*100.0f;

	printf("Total TMU memory    : %010d Bytes %3.2f MB\n",total_mem,(float)(total_mem)/(1024.0f*1024.0f));
	printf("TMU memory Used     : %010d Bytes %3.2f MB %3.2f%%\n",total_used,(float)(total_used)/(1024.0f*1024.0f),percent_used);
	printf("TMU memory Available: %010d Bytes %3.2f MB %3.2f%%\n",total_avail,(float)(total_avail)/(1024.0f*1024.0f),percent_avail);
	printf("\n");

    printf("TMU 0: %010d Bytes %3.2f MB free\n",memTMU0,(float)(memTMU0)/(1024.0f*1024.0f));
    printf("TMU 1: %010d Bytes %3.2f MB free\n",memTMU1,(float)(memTMU1)/(1024.0f*1024.0f));

	printf("\n");
	printf("\n");
	printf("\n");

}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: show_texture_list1 ()                                           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 03 Sep 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void show_texture_list1(void)
{
	texture_node_t	*tn;
	unsigned int	total_tmu_used = 0;
	unsigned int	tmu_mem;
	unsigned int	width;
	unsigned int	height;

/*****************************************************************************/

	int memTMU0;
	int memTMU1;
	int	total_mem; 
	int	total_used;  
	int	total_avail; 
	float percent_used;
	float percent_avail;

	tn = tlist;

	total_mem   = TmuInfo.num_tmu * TmuInfo.memory_tmu * 1024 * 1024;


	if(tn)
	{
		fprintf(stderr, "\r\n");
		fprintf(stderr, "Graphics hardware:" GRX "\r\n");
		fprintf(stderr, "Currently loaded textures:\r\n");
		fprintf(stderr, "%-13s", "Name");
		fprintf(stderr, "%-7s",  "TextID");
		fprintf(stderr, "%-6s",  "Count");
		fprintf(stderr, "%-8s",  "Size");
		fprintf(stderr, "%-8s",  "Locked");
		fprintf(stderr, "%-10s", "Tctl ptr");
		fprintf(stderr, "%-8s",  "Format");
		fprintf(stderr, "%-9s",  "Mipmap");
		fprintf(stderr, "\r\n");
		while(tn)
		{
			width = tn->texture_info.header.width;
			height = tn->texture_info.header.height;

			/******************************************************************/
			/*                                                                */
			/* NOTE: This has to be updated for Trilinear and Multibase       */
			/*                                                                */
			/******************************************************************/

			tmu_mem	= tn->texture_handle->attrib.size;

			fprintf(stderr, "%-14.12s", tn->texture_name);
			fprintf(stderr, "%-8hx", tn->texture_id);
			fprintf(stderr, "%-4hd", tn->texture_count);
			fprintf(stderr, "%-8u", tmu_mem);
			fprintf(stderr, "%-8s", (tn->texture_flags & TEXTURE_LOCKED?"YES":"NO"));
			fprintf(stderr, "%-10.8X", (int)tn->texture_handle);
			fprintf(stderr, "%-8s", (tn->texture_info.header.format >= GR_TEXFMT_16BIT ? "16 BIT" : "8 BIT"));
			fprintf(stderr, "%-9d", tn->texture_info.header.small_lod - tn->texture_info.header.large_lod + 1);
			fprintf(stderr, "\r\n");
			total_tmu_used += tmu_mem;
			tn = tn->next;
		}

	    memTMU0 = getTotalMemTmu ( GR_TMU0 );
	    memTMU1 = getTotalMemTmu ( GR_TMU1 );

		printf("\n");
		total_mem   = TmuInfo.num_tmu * TmuInfo.memory_tmu;
		total_avail  = memTMU0 + memTMU1;
		total_used = total_mem - total_avail;
		percent_used = ((float)total_used/(float)total_mem)*100.0f;
		percent_avail = ((float)total_avail/(float)total_mem)*100.0f;

		printf("Total TMU memory    : %010d Bytes %3.2f MB\n",total_mem,(float)(total_mem)/(1024.0f*1024.0f));
		printf("TMU memory Used     : %010d Bytes %3.2f MB %3.2f%%\n",total_used,(float)(total_used)/(1024.0f*1024.0f),percent_used);
		printf("TMU memory Available: %010d Bytes %3.2f MB %3.2f%%\n",total_avail,(float)(total_avail)/(1024.0f*1024.0f),percent_avail);
		printf("\n");

	    printf("TMU 0: %010d Bytes %3.2f MB free\n",memTMU0,(float)(memTMU0)/(1024.0f*1024.0f));
    	printf("TMU 1: %010d Bytes %3.2f MB free\n",memTMU1,(float)(memTMU1)/(1024.0f*1024.0f));

		printf("\n");
		printf("\n");
		printf("\n");

	}
	else
	{
		fprintf(stderr, "\r\nThere are NO Textures Currently loaded\r\n\r\n");
	}
}
