#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iffr.h"


/****************************************************************************/
/*** macros *****************************************************************/

#define SWAP16(x) (((x&0xff00)>>8)|((x&0x00ff)<<8))
#define SWAP32(x) ((SWAP16((x&0xffff0000)>>16))|(SWAP16((x&0xffff))<<16))
#define MakeID(a,b,c,d) (((long)a)<<24|((long)b)<<16|((long)c)<<8|((long)d))


/****************************************************************************/
/*** chunk IDs **************************************************************/

#define ID_FORM		((long)MakeID('F','O','R','M'))
#define ID_LIST		((long)MakeID('L','I','S','T'))
#define ID_PROP		((long)MakeID('P','R','O','P'))
#define ID_CAT		((long)MakeID('C','A','T',' '))
#define ID_FILLER	((long)MakeID(' ',' ',' ',' '))

/**************************************/
/* LWOB and related chunks ************/

#define ID_LWOB		((long)MakeID('L','W','O','B'))

#define ID_PNTS		((long)MakeID('P','N','T','S'))
#define ID_SRFS		((long)MakeID('S','R','F','S'))
#define ID_POLS		((long)MakeID('P','O','L','S'))
#define ID_CRVS		((long)MakeID('C','R','V','S'))
#define ID_SURF		((long)MakeID('S','U','R','F'))

/**************************************/
/* SURF sub-chunks ********************/

#define ID_COLR		((long)MakeID('C','O','L','R'))
#define ID_FLAG		((long)MakeID('F','L','A','G'))
#define ID_LUMI		((long)MakeID('L','U','M','I'))
#define ID_DIFF		((long)MakeID('D','I','F','F'))
#define ID_SPEC		((long)MakeID('S','P','E','C'))
#define ID_REFL		((long)MakeID('R','E','F','L'))
#define ID_TRAN		((long)MakeID('T','R','A','N'))
#define ID_GLOS		((long)MakeID('G','L','O','S'))
#define ID_RIMG		((long)MakeID('R','I','M','G'))
#define ID_RSAN		((long)MakeID('R','S','A','N'))
#define ID_RIND		((long)MakeID('R','I','N','D'))
#define ID_EDGE		((long)MakeID('E','D','G','E'))
#define ID_SMAN		((long)MakeID('S','M','A','N'))
#define ID_CTEX		((long)MakeID('C','T','E','X'))
#define ID_DTEX		((long)MakeID('D','T','E','X'))
#define ID_STEX		((long)MakeID('S','T','E','X'))
#define ID_RTEX		((long)MakeID('R','T','E','X'))
#define ID_TTEX		((long)MakeID('T','T','E','X'))
#define ID_BTEX		((long)MakeID('B','T','E','X'))
#define ID_TIMG		((long)MakeID('T','I','M','G'))
#define ID_TFLG		((long)MakeID('T','F','L','G'))
#define ID_TSIZ		((long)MakeID('T','S','I','Z'))
#define ID_TCTR		((long)MakeID('T','C','T','R'))
#define ID_TFAL		((long)MakeID('T','F','A','L'))
#define ID_TVEL		((long)MakeID('T','V','E','L'))
#define ID_TCLR		((long)MakeID('T','C','L','R'))
#define ID_TVAL		((long)MakeID('T','V','A','L'))
#define ID_TAMP		((long)MakeID('T','A','M','P'))
#define ID_TFRQ		((long)MakeID('T','F','R','Q'))
#define ID_TSP0		((long)MakeID('T','S','P','0'))
#define ID_TSP1		((long)MakeID('T','S','P','1'))
#define ID_TSP2		((long)MakeID('T','S','P','2'))

/**************************************/
/* ILBM and related chunks ************/

#define ID_ILBM		((long)MakeID('I','L','B','M'))


/****************************************************************************/
/*** structs & typedefs *****************************************************/

typedef int32 ID;				/* 4 chars in ' ' through '~'	*/


/****************************************************************************/
/*** local function prototypes **********************************************/

static void parseIFF( FILE * );

static void parseFORM( FILE * );

static void parsePNTS( FILE * );
static void parseSRFS( FILE * );
static void parsePOLS( FILE * );
static void parseSURF( FILE * );

static void parseCOLR( FILE * );
static void parseFLAG( FILE * );
static void parseDIFF( FILE * );
static void parseCTEX( FILE * );
static void parseTIMG( FILE * );
static void parseTFLG( FILE * );
static void parseTSIZ( FILE * );
static void parseTCTR( FILE * );
static void parseTCLR( FILE * );
static void parseSMAN( FILE * );


/****************************************************************************/
/*** local data *************************************************************/

int		vtx_base, surf_base;			/* nvtx & nsurf at start of parsing */


/****************************************************************************/
/*** external data references ***********************************************/

extern int 			nvtx;				/* vertex counts */
extern int			nface, nsurf;		/* face & surface counts */
extern vec3d		vdb[VTX_MAX];		/* vertex list */
extern grface		fdb[FACE_MAX];		/* face list */
extern lwSurface	sdb[SURF_MAX];		/* surface list */
extern flags_t		flags;

/****************************************************************************/

int read_lwo( char *filename )
{
	FILE *fp;

	fp = fopen( filename, "rb" );

	if( fp == NULL )
	{
		printf( "Error opening file: %s\n", filename );
		exit( 0 );
	}

	parseIFF( fp );

	fclose( fp );

	return 0;
}


/****************************************************************************/

static void parseIFF( FILE *fp )
{
	ID		file_id;

	vtx_base = nvtx;
	surf_base = nsurf;

	fread( &file_id, sizeof( ID ), 1, fp );
	file_id = SWAP32( file_id );

	switch( file_id )
	{
		case ID_FORM:
			parseFORM( fp );
			break;
		case ID_LIST:
			printf( "I can't read IFF LIST files.  Stop.\n" );
			exit( 0 );
			break;
		case ID_CAT:
			printf( "I can't read IFF CAT files.  Stop.\n" );
			exit( 0 );
			break;
		default:
			printf( "This isn't a valid IFF file.  Stop.\n" );
			exit( 0 );
			break;
	}
}


/****************************************************************************/

static void parseFORM( FILE *fp )
{
	int32	form_size, chunk_size, file_pos;
	ID		form_type, chunk_id;
	int		nPNTS = 0, nSRFS = 0, nPOLS = 0;

	/* read FORM size */
	fread( &form_size, sizeof( int32 ), 1, fp );
	form_size = SWAP32( form_size );

	/* get FORM type */
	fread( &form_type, sizeof( ID ), 1, fp );
	form_type = SWAP32( form_type );

	if( form_type == ID_LWOB )
	{
		if( flags.dump_iff )
		{
			printf( "\nFORM type LWOB found, size %ld.  Processing.\n",
					form_size );
		}
	}
	else
	{
		printf( "Unknown FORM type: %c%c%c%c, size %ld  Stop.\n",
				*(((char *)(&form_type)) + 3),
				*(((char *)(&form_type)) + 2),
				*(((char *)(&form_type)) + 1),
				*((char *)(&form_type)),
				form_size );
		exit( 0 );
	}

	/* parse all the sub-chunks */
	do
	{
		/* read chunk ID and size */
		fread( &chunk_id, sizeof( ID ), 1, fp );
		chunk_id = SWAP32( chunk_id );

		/* process the chunk */
		switch( chunk_id )
		{
			case ID_PNTS:
				nPNTS++;
				parsePNTS( fp );
				break;
			case ID_SRFS:
				nSRFS++;
				parseSRFS( fp );
				break;
			case ID_POLS:
				nPOLS++;
				parsePOLS( fp );
				break;
			case ID_SURF:
				parseSURF( fp );
				break;
			default:
				/* don't know this chunk.  skip over it */
				if( flags.dump_iff )
				{
					printf( "Unrecognized chunk type: %c%c%c%c, size %ld\n",
							*(((char *)(&chunk_id)) + 3),
							*(((char *)(&chunk_id)) + 2),
							*(((char *)(&chunk_id)) + 1),
							*((char *)(&chunk_id)),
							chunk_size );
				}
				fread( &chunk_size, sizeof( int32 ), 1, fp );
				chunk_size = SWAP32( chunk_size );
				fseek( fp, chunk_size, SEEK_CUR );
				break;
		}

		fgetpos( fp, &file_pos );

	} while( file_pos < (form_size + 8 ));
}


/****************************************************************************/

static void parsePNTS( FILE *fp )
{
	int32	chunk_size;
	int		i, npoints;
	int32	*lp;
	lwPoint	pnt;

	fread( &chunk_size, sizeof( int32 ), 1, fp );
	chunk_size = SWAP32( chunk_size );

	npoints = chunk_size / sizeof( lwPoint );

	if( flags.dump_iff )
		printf( "PNTS chunk, %d vertices\n", npoints );

	for( i = 0; i < npoints; i++ )
	{
		fread( &pnt, sizeof( lwPoint ), 1, fp );
		lp = (int32 *)(&pnt);
		lp[0] = SWAP32( lp[0] );
		lp[1] = SWAP32( lp[1] );
		lp[2] = SWAP32( lp[2] );
		vdb[nvtx].x = pnt.x;
		vdb[nvtx].y = pnt.y;
		vdb[nvtx].z = pnt.z;
		nvtx++;
	}

	return;
}


/****************************************************************************/

static void parseSRFS( FILE *fp )
{
	int32	chunk_size;
	char	str[512];
	int		i, sp;

	fread( &chunk_size, sizeof( int32 ), 1, fp );
	chunk_size = SWAP32( chunk_size );

	if( flags.dump_iff )
		printf( "SRFS chunk, size %ld\n", chunk_size );

	sp = 0;
	for( i = 0; i < chunk_size; i+=2 )
	{
		fread( str + sp, sizeof( char ), 2, fp );
		if(!(str[sp+1]))
		{
			/* str is a name of a surface.  yawn. */
			sp = 0;
		}
		else
		{
			sp += 2;
		}
	}
}


/****************************************************************************/

static void parsePOLS( FILE *fp )
{
	int32	chunk_size;
	int16	nvert, cnt, s, npols = 0;
	int16	vertices[3];

	fread( &chunk_size, sizeof( int32 ), 1, fp );
	chunk_size = SWAP32( chunk_size );

	cnt = chunk_size / sizeof( int16 );

	/* read entire block */
	while( cnt )
	{
		/* read the vertex count */
		fread( &nvert, sizeof( int16 ), 1, fp );
		nvert = SWAP16( nvert );

		/* if the face has three vertices, add it to the list */
		if( nvert == 3 )
		{
			fread( vertices, sizeof( int16 ), nvert, fp );

			fdb[nface].vtxi[0] = vtx_base + SWAP16( vertices[0] );
			fdb[nface].vtxi[1] = vtx_base + SWAP16( vertices[1] );
			fdb[nface].vtxi[2] = vtx_base + SWAP16( vertices[2] );

			/* read the surface index */
			fread( &s, sizeof( int16 ), 1, fp );
			fdb[nface].surface = surf_base - 1 + SWAP16( s );
			nface++;
		}
		/* otherwise just skip it */
		else
		{
			fseek( fp, sizeof( int16 ) * (1 + nvert), SEEK_CUR );
			fprintf( stderr, "Warning! %d-sided face detected.\n", nvert );
		}

		cnt -= (2 + nvert );
		npols++;
	}

	if( flags.dump_iff )
		printf( "POLS chunk, %d polygons\n", npols );
}


/****************************************************************************/

static void parseSURF( FILE *fp )
{
	int32	chunk_size, i;
	int16	subchunk_size;
	char	name[128];
	ID		subchunk_id;
	fpos_t	endpoint, current;

	fread( &chunk_size, sizeof( int32 ), 1, fp );
	chunk_size = SWAP32( chunk_size );

	fgetpos( fp, &endpoint );
	endpoint += chunk_size;

	/* read the name */
	i = 0;
	do
	{
		fread( name + i, sizeof( char ), 2, fp );
		i += 2;
	} while( name[i-1] );

	if( flags.dump_iff )
		printf( "SURF chunk, name: %s\n", name );

	/* parse the sub-chunks */
	do
	{
		fread( &subchunk_id, sizeof( ID ), 1, fp );
		subchunk_id = SWAP32( subchunk_id );

		switch( subchunk_id )
		{
			case ID_COLR:
				parseCOLR( fp );
				break;
			case ID_FLAG:
				parseFLAG( fp );
				break;
			case ID_DIFF:
				parseDIFF( fp );
				break;
			case ID_CTEX:
				parseCTEX( fp );
				break;
			case ID_TIMG:
				parseTIMG( fp );
				break;
			case ID_TFLG:
				parseTFLG( fp );
				break;
			case ID_TSIZ:
				parseTSIZ( fp );
				break;
			case ID_TCTR:
				parseTCTR( fp );
				break;
			case ID_TCLR:
				parseTCLR( fp );
				break;
			case ID_SMAN:
				parseSMAN( fp );
				break;
			default:
				fread( &subchunk_size, sizeof( int16 ), 1, fp );
				subchunk_size = SWAP16( subchunk_size );
				if( flags.dump_iff )
				{
					printf( "unrecognized sub-chunk type: %c%c%c%c, "
							"size: %d\n",
							*(((char *)(&subchunk_id)) + 3),
							*(((char *)(&subchunk_id)) + 2),
							*(((char *)(&subchunk_id)) + 1),
							*((char *)(&subchunk_id)),
							subchunk_size );
				}
				fseek( fp, subchunk_size, SEEK_CUR );
				break;
		}

		fgetpos( fp, &current );

	} while( current != endpoint );

	nsurf++;
}


/****************************************************************************/

static void parseCOLR( FILE *fp )
{
	int16	chunk_size;
	char	r,g,b,x;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	/* read r, g, and b.  discard fourth byte */
	fread( &r, sizeof( char ), 1, fp );
	fread( &g, sizeof( char ), 1, fp );
	fread( &b, sizeof( char ), 1, fp );
	fread( &x, sizeof( char ), 1, fp );

	if( flags.dump_iff )
		printf( "-COLR sub-chunk, rgb: %d %d %d (%d)\n", (int)r, (int)g,
				(int)b, (int)x );
}


/****************************************************************************/

static void parseFLAG( FILE *fp )
{
	int16	chunk_size;
	int16	flag;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	fread( &flag, sizeof( int16 ), 1, fp );
	flag = SWAP16( flag );

	if( flags.dump_iff )
		printf( "-FLAG sub-chunk, flags: 0x%X\n", flag );
}


/****************************************************************************/

static void parseDIFF( FILE *fp )
{
	int16	chunk_size;
	int16	diff;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	fread( &diff, sizeof( int16 ), 1, fp );
	diff = SWAP16( diff );

	if( flags.dump_iff )
		printf( "-DIFF sub-chunk, diffuse: %d\n", diff );
}


/****************************************************************************/

static void parseCTEX( FILE *fp )
{
	int16	chunk_size;
	char	*name;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	name = (char *)malloc( chunk_size );

	fread( name, sizeof( char ), chunk_size, fp );

	if( flags.dump_iff )
		printf( "-CTEX sub-chunk, name: %s\n", name );

	if( !strcmp( name, "Planar Image Map" ))
	{
		sdb[nsurf+1].ttype = TT_PLANAR;
	}
	else
	{
		sdb[nsurf+1].ttype = TT_UNKNOWN;
	}

	free( name );
}


/****************************************************************************/

static void parseTIMG( FILE *fp )
{
	int16	chunk_size;
	char	*name, *p, *q;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	name = (char *)malloc( chunk_size );

	fread( name, sizeof( char ), chunk_size, fp );

	p = name;
	while( strchr( p, '\\' ))
		p = 1 + strchr( p, '\\' );

	if(( q = strchr( p, '.' )))
	{
		*(q+1) = 'w';
		*(q+2) = 'm';
		*(q+3) = 's';
		*(q+4) = '\0';
	}

	strncpy( sdb[nsurf+1].imgfile, p, 13 );
	sdb[nsurf+1].imgfile[12] = '\0';

	if( flags.dump_iff )
		printf( "-TIMG sub-chunk, name: %s\n", p );

	free( name );
}


/****************************************************************************/

static void parseTFLG( FILE *fp )
{
	int16	chunk_size;
	int16	flag;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	fread( &flag, sizeof( int16 ), 1, fp );
	flag = SWAP16( flag );

	if( flags.dump_iff )
		printf( "-TFLG sub-chunk, flags: 0x%X\n", flag );

	sdb[nsurf+1].tflags = flag;
}


/****************************************************************************/

static void parseTSIZ( FILE *fp )
{
	int16	chunk_size;
	float	f[3];
	int32	*p;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	fread( f, sizeof( int32 ), 3, fp );
	p = (int32 *)f;
	p[0] = SWAP32( p[0] );
	p[1] = SWAP32( p[1] );
	p[2] = SWAP32( p[2] );

	if( flags.dump_iff )
		printf( "-TSIZ sub-chunk, x,y,z: %f %f %f\n", f[0], f[1], f[2] );

	sdb[nsurf+1].tsize.x = f[0];
	sdb[nsurf+1].tsize.y = f[1];
	sdb[nsurf+1].tsize.z = f[2];
}


/****************************************************************************/

static void parseTCTR( FILE *fp )
{
	int16	chunk_size;
	float	f[3];
	int32	*p;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	fread( f, sizeof( int32 ), 3, fp );
	p = (int32 *)f;
	p[0] = SWAP32( p[0] );
	p[1] = SWAP32( p[1] );
	p[2] = SWAP32( p[2] );

	if( flags.dump_iff )
		printf( "-TCTR sub-chunk, x,y,z: %f %f %f\n", f[0], f[1], f[2] );

	sdb[nsurf+1].tcenter.x = f[0];
	sdb[nsurf+1].tcenter.y = f[1];
	sdb[nsurf+1].tcenter.z = f[2];
}


/****************************************************************************/

static void parseTCLR( FILE *fp )
{
	int16	chunk_size;
	char	r,g,b,x;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	/* read r, g, and b.  discard fourth byte */
	fread( &r, sizeof( char ), 1, fp );
	fread( &g, sizeof( char ), 1, fp );
	fread( &b, sizeof( char ), 1, fp );
	fread( &x, sizeof( char ), 1, fp );

	if( flags.dump_iff )
		printf( "-TCLR sub-chunk, rgb: %d %d %d (%d)\n", (int)r, (int)g,
			(int)b, (int)x );
}


/****************************************************************************/

static void parseSMAN( FILE *fp )
{
	int16	chunk_size;
	float	*angle;
	int32	in;

	fread( &chunk_size, sizeof( int16 ), 1, fp );
	chunk_size = SWAP16( chunk_size );

	/* read r, g, and b.  discard fourth byte */
	fread( &in, sizeof( int32 ), 1, fp );

	in = SWAP32( in );

	angle = (float *)(&in);

	sdb[nsurf+1].sman = *angle;

	if( flags.dump_iff )
		printf( "-SMAN sub-chunk, angle: %f\n", *angle );
}


/****************************************************************************/
