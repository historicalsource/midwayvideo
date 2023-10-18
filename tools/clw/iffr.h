/****************************************************************************/
/* miscellaneous definitions ************************************************/

#define FALSE		0
#define TRUE		!FALSE

#define VTX_MAX		2048		/* max vertices */
#define FACE_MAX	2048		/* max faces */
#define SURF_MAX	64			/* max surfaces */

#define FILE_MAX	16			/* max input files */

#define GrMipMapId_t	int

/****************************************************************************/
/* structures & typedefs ****************************************************/

typedef short			int16;	/* 16 bits signed				*/
typedef unsigned short	uns16;	/* 16 bits unsigned				*/
typedef long			int32;	/* 32 bits signed				*/
typedef unsigned long	uns32;	/* 32 bits unsigned				*/

enum gtypes	{ GT_WATCOM, GT_GLIDE, GT_NONE };
enum gmodes	{ GM_WIREFRAME, GM_SURFACES };

typedef struct _flags_t
{
	int		help;		/* TRUE / FALSE 					*/
	int		graphics;	/* GT_WATCOM, GT_GLIDE, or GT_NONE	*/
	int		mode;		/* GM_WIREFRAME or GM_SURFACES		*/
	int		dump_iff;	/* TRUE / FALSE						*/
} flags_t;

typedef struct _vec3d
{
	float	x,y,z,w;
} vec3d;

//typedef struct _tvertex
//{
//	float	s, t;
//} tvertex;

typedef struct _viewcoord
{
	float 	x,y,z;		/* camera x,y,and z */
	float 	a,b,c;		/* focus x,y,and z */
	float 	e,f,g;		/* point above focus in y on viewplane */
	float 	length;		/* dist from cambot to viewplane */
} viewcoord;

typedef struct _grface
{
	int		vtxi[3];	/* vertex indices */
	int		tvtx[3];	/* texture indices */
	int16	surface;	/* surface index */
	vec3d	norm;		/* surface normal */
	int16	visible;	/* TRUE/FALSE */
} grface;

typedef struct _lwPoint
{
	float	x;
	float	y;
	float	z;
} lwPoint;

enum ttypes { TT_UNKNOWN, TT_PLANAR, TT_CYLINDRICAL, TT_SPHERICAL };

typedef struct _lwSurface
{
	char	imgfile[13];
	GrMipMapId_t imap;
/*	Gu3dfInfo info; */
	int		ttype;
	int16	tflags;
	vec3d	tsize;
	vec3d	tcenter;
	float	sman;
} lwSurface;


/****************************************************************************/
/* lightwave constants ******************************************************/

/* TFLG bits */

#define TF_XAXIS	0x0001
#define TF_YAXIS	0x0002
#define TF_ZAXIS	0x0004


/****************************************************************************/
/* prototypes ***************************************************************/

int read_lwo( char * );


/****************************************************************************/

