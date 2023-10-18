%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "lookup.h"

#define FALSE	0
#define TRUE	!FALSE


typedef int boolean;
typedef float matrix[3][3];
typedef float quat[4];

void init_curve( char * );
void end_curve( void );
void add_point( char * );

void adjust_quat( quat, quat );
void mxm( matrix, matrix, matrix );
void rotx( float, matrix );
void roty( float, matrix );
void rotz( float, matrix );
int best_lookup( float );
int new_node( char * );
void assign_channel( int, char * );
void mat2quat( matrix, quat );
void quat2mat( quat, matrix );
void print_help( void );
unsigned long crc(unsigned char *data, int len);
void print_mat( matrix );
void print_quat( quat );

int cur_node;
boolean flip = FALSE;
boolean	compress = FALSE;
float	yrot = 0.0f;
float	highy = 0.0f;

%}

%x		CV

ws		[ \t]+
cws		[ \t,]+
num 	-?(([0-9]+)|([0-9]*\.[0-9]+)([eE][+-]?[0-9]+)?)

%%

^"COMPRESS\n"							compress = TRUE;
^"NODE Y="{num}							yrot = atof( yytext + 7 );
^"node y="{num}							yrot = atof( yytext + 7 );
^"HIGH Y="{num}							highy = atof( yytext + 7 );
^"high y="{num}							highy = atof( yytext + 7 );
^"FLIP"									flip = TRUE;
"parameter_curve"{ws}[a-zA-Z_.#0-9]+	{ BEGIN CV; init_curve( yytext ); };
<CV>");"								{ BEGIN 0; end_curve(); };
"type \"Dag Node\" ( "[a-zA-Z_]+		cur_node = new_node( yytext+18 );
"channel \"X Rotate\" ( "[a-zA-Z_.#0-9]+	assign_channel( 0, yytext+21 );
"channel \"Y Rotate\" ( "[a-zA-Z_.#0-9]+	assign_channel( 1, yytext+21 );
"channel \"Z Rotate\" ( "[a-zA-Z_.#0-9]+	assign_channel( 2, yytext+21 );
"channel \"X Translate\" ( "[a-zA-Z_.#0-9]+	assign_channel( -3, yytext+24 );
"channel \"Y Translate\" ( "[a-zA-Z_.#0-9]+	assign_channel( -2, yytext+24 );
"channel \"Z Translate\" ( "[a-zA-Z_.#0-9]+	assign_channel( -1, yytext+24 );
<CV>"vertex("{ws}{num}{cws}{num}		add_point( yytext );
<CV>.|\n								;
.|\n									;

%%
typedef struct _VECTOR
{
	float	x,y,z;
} VECTOR;

typedef struct _CURVE
{
	char	name[32];
	int		count;
	float	*pdata;
} CURVE;

struct seq_header
{
	char	name[16];
	int		nframes;
	int		nlimbs;
	int		flags;
};

#define A_COMPRESSED	0x1


#define X	0
#define Y	1
#define Z	2
#define W	3

#define LIMB_COUNT	15
#define MAX_FRAMES	512
#define MAX_CURVES  128

int flip_limbs[LIMB_COUNT] = { 0,0,1,1,1,1,1,1,0,1,1,1,1,1,1 };

char *limb_names[LIMB_COUNT] =
{
	"BIPED", "BACK", "RT_SHOULDER", "RT_ELBOW", "RT_WRIST",
	"LF_SHOULDER", "LF_ELBOW", "LF_WRIST", "NECK",
	"RT_HIP", "RT_KNEE", "RT_ANKLE", "LF_HIP", "LF_KNEE", "LF_ANKLE",
};

char *limb_names_flip[LIMB_COUNT] =
{
	"BIPED", "BACK", "LF_SHOULDER", "LF_ELBOW", "LF_WRIST",
	"RT_SHOULDER", "RT_ELBOW", "RT_WRIST", "NECK",
	"LF_HIP", "LF_KNEE", "LF_ANKLE", "RT_HIP", "RT_KNEE", "RT_ANKLE",
};

int limb_map[LIMB_COUNT][3];
int	translate[6];
CURVE curves[MAX_CURVES];
float points[1024];
int npoints, ncurves = 0;
int hits[256];

float xlate[MAX_FRAMES][3];
float frame0[LIMB_COUNT][4];
char frames[MAX_FRAMES*LIMB_COUNT][4];			/* compressed quats */
float uframes[MAX_FRAMES*LIMB_COUNT][4];		/* raw (precompressed) quats */
float ucframes[MAX_FRAMES*LIMB_COUNT][4];		/* reconstituted quats */
int frame_count;
boolean	write_crcs = FALSE;
unsigned long crcs[MAX_FRAMES];

typedef enum { PS_NONE, PS_OUTFILE, PS_NAME, PS_SCALE, PS_CRC,
				PS_YROT } pstate;
typedef enum { OM_TEXT, OM_BINARY } outmode;


/**********************************************************************/
int main( int argc, char *args[] )
{
	int		i, j;
	matrix	m1,m2,m3,m4,m5,m6,m7;
	quat	q1;
	FILE	*outfile, *infile, *crcfile;
	struct seq_header header;
	char	*outfspec = NULL, *seqname = NULL, *infspec = NULL;
	char	*crcfspec = NULL;
	char	root[16], *cp;
	char	tbuf[128];
	pstate	pmode = PS_NONE;
	outmode	output_mode = OM_BINARY;
	float	xlate_sf = 1.0f;

	float	prev[15][4];

	/* parse command flags */
	for( i = 1; i < argc; i++ )
	{
		switch( args[i][0] )
		{
			case '-':
				switch( args[i][1] )
				{
					case 'o':	/* output filename */
						if( args[i][2] )
							outfspec = &args[i][2];
						else
							pmode = PS_OUTFILE;
						break;
					case 'n':	/* sequence name */
						if( args[i][2] )
							seqname = &args[i][2];
						else
							pmode = PS_NAME;
						break;
					case 's':	/* set translation scale factor */
						if( args[i][2] )
							xlate_sf = atof( &args[i][2] );
						else
							pmode = PS_SCALE;
						break;
					case 't':
						output_mode = OM_TEXT;
						break;
					case 'c':
						write_crcs = TRUE;
						if( args[i][2] )
							crcfspec = &args[i][2];
						else
							pmode = PS_CRC;
						break;
					case 'k':	/* kompress */
						compress = TRUE;
						break;
					case 'f':	/* flip (in x) */
						flip = TRUE;
						break;
					case 'h':	/* help */
						print_help();
						exit( 1 );
						break;
					case 'y':	/* top-level y rotate */
						if( args[i][2] )
							yrot = atof( &args[i][2] );
						else
							pmode = PS_YROT;
						break;
					default:
						fprintf(stderr,"WARNING: Unrecognized command li"
								"ne option: %s\n", args[i] );
						break;
				}
				break;
			default:
				switch( pmode )
				{
					case PS_OUTFILE:
						outfspec = args[i];
						pmode = PS_NONE;
						break;
					case PS_NAME:
						seqname = args[i];
						pmode = PS_NONE;
						break;
					case PS_SCALE:
						xlate_sf = atof( args[i] );
						pmode = PS_NONE;
						break;
					case PS_CRC:
						crcfspec = args[i];
						pmode = PS_NONE;
						break;
					case PS_YROT:
						yrot = atof( args[i] );
						pmode = PS_NONE;
						break;
					default:
						if( infspec )
						{
							fprintf(stderr,"ERROR: Too many input files speci"
									"fied.\n" );
							exit( 1 );
						}
						else
							infspec = args[i];
						break;
				}
				break;
		}
	}

	if( !infspec )
	{
		fprintf( stderr, "ERROR: Must specify one input file.\n" );
		exit( 1 );
	}

	strncpy( root, infspec, 16 );
	root[15] = '\0';
	if(( cp = strchr( root, '.' )))
		*cp = '\0';

	if( !seqname )
		seqname = root;

	if( !outfspec )
	{
		sprintf( tbuf, "%s.seq", root );
		outfspec = tbuf;
	}

	infile = fopen( infspec, "rt" );
	if( infile == NULL )
	{
		fprintf( stderr, "Error opening file %s for reading.\n", infspec );
		exit( 1 );
	}

	outfile = fopen( outfspec, ( output_mode == OM_BINARY) ? "wb" : "wt" );
	if( outfile == NULL )
	{
		fprintf( stderr, "Error opening file %s for writing.\n", outfspec );
		exit( 1 );
	}

	if( write_crcs )
	{
		crcfile = fopen( crcfspec, "wt" );
		if( crcfile == NULL )
		{
			fprintf( stderr, "Error opening file %s for writing.\n", crcfspec );
			exit( 1 );
		}
	}

	/* initialize limb_map */
	for( i = 0; i < LIMB_COUNT; i++ )
	{
		limb_map[i][0] = -1;
		limb_map[i][1] = -1;
		limb_map[i][2] = -1;
	}

	translate[0] = -1;
	translate[1] = -1;
	translate[2] = -1;
	translate[3] = -1;
	translate[4] = -1;
	translate[5] = -1;

	/* read curves from stdin */
	yyin = infile;
	yylex();

	/* holler if any channels are missing */
	for( i = 0; i < LIMB_COUNT; i++ )
	{
		char xyz[] = { 'X', 'Y', 'Z' };

		for( j = 0; j < 3; j++ )
		{
			if( limb_map[i][j] == -1 )
			{
				fprintf( stderr, "Warning! Channel %s has no %c rotate cha"
						"nnel.\n", limb_names[i], xyz[j] );
				limb_map[i][j] = ncurves;
			}
		}
	}

	/* make sure all cuves have the same count */
	for( i = 1; i < ncurves; i++ )
	{
		if( curves[i].count != curves[i-1].count )
		{
			fprintf( stderr, "Error!  Curves %s and %s don't have the same "
					"number of data points. (%d and %d)\n", curves[i-1].name,
					curves[i].name, curves[i-1].count, curves[i].count );
			exit( 1 );
		}
	}

	frame_count = curves[0].count;

	/* put a zero curve at the end of the list */
	curves[ncurves].pdata = (float *)malloc( curves[0].count * sizeof( float ));
	for( i = 0; i < curves[0].count; i++ )
		curves[ncurves].pdata[i] = 0.0f;
	sprintf( curves[ncurves].name, "zero" );
	curves[ncurves].count = curves[0].count;

	/* initialize prev */
	for( i = 0; i < 15; i++ )
	{
		prev[i][0] = 0.0f;
		prev[i][1] = 0.0f;
		prev[i][2] = 0.0f;
		prev[i][3] = 0.0f;
	}

	for( i = 0; i < frame_count; i++ )
	{
		float x,y,z;
		float theta = yrot /360.0f * 2.0f * 3.14159f;

		x = curves[translate[0]].pdata[i] * xlate_sf;
		y = curves[translate[1]].pdata[i] * xlate_sf;
		z = curves[translate[2]].pdata[i] * xlate_sf;

		xlate[i][0] = (z*sin(theta) + x*cos(theta));
		xlate[i][1] = y;
		xlate[i][2] = z*cos(theta) - x*sin(theta);

		x = 0.0f;
		y = 0.0f;
		z = 0.0f;

		theta = highy /360.0f * 2.0f * 3.14159f;

		if( translate[3] != -1 )
			x = curves[translate[3]].pdata[i] * xlate_sf;

		if( translate[4] != -1 )
			y = curves[translate[4]].pdata[i] * xlate_sf;

		if( translate[5] != -1 )
			z = curves[translate[5]].pdata[i] * xlate_sf;

		xlate[i][0] += (z*sin(theta) + x*cos(theta));
		xlate[i][1] += y;
		xlate[i][2] += z*cos(theta) - x*sin(theta);

		xlate[i][0] *= (flip ? -1.0f : 1.0f);
	}

	for( i = 0; i < frame_count; i++ )	/* frame loop */
	{
		for( j = 0; j < LIMB_COUNT; j++ )			/* limb loop */
		{
			quat	fq;
			float	matches[4];
			int		fits[4];
			float	r3,r4,r5,r6;

			/* build a composite matrix for this rotation */
			rotx( r3 = curves[limb_map[j][0]].pdata[i], m3 );
			roty( r4 = curves[limb_map[j][1]].pdata[i] * (flip ? -1.0f : 1.0f), m4 );
			rotz( r5 = curves[limb_map[j][2]].pdata[i] * (flip ? -1.0f : 1.0f), m5 );
			if( j )
				roty( r6 = 0.0f, m6 );
			else
				roty( r6 = (yrot * (flip ? -1.0f : 1.0f)), m6 );

			roty( 0.0f, m2 );

			mxm( m6, m5, m7 );
			mxm( m7, m4, m1 );
			mxm( m1, m3, m7 );
			mxm( m7, m2, m1 );

			mat2quat( m1, fq );

			if( i == 0 )
			{
				prev[j][0] = fq[0];
				prev[j][1] = fq[1];
				prev[j][2] = fq[2];
				prev[j][3] = fq[3];

				frame0[j][0] = fq[0];
				frame0[j][1] = fq[1];
				frame0[j][2] = fq[2];
				frame0[j][3] = fq[3];

				uframes[j][0] = fq[0];
				uframes[j][1] = fq[1];
				uframes[j][2] = fq[2];
				uframes[j][3] = fq[3];

				ucframes[j][0] = fq[0];
				ucframes[j][1] = fq[1];
				ucframes[j][2] = fq[2];
				ucframes[j][3] = fq[3];
			}
			else
			{

				// if -fq[i] is a better match to fq[i-1] than fq[i], use it
				adjust_quat( fq, prev[j] );

				uframes[(i*15)+j][0] = fq[0];
				uframes[(i*15)+j][1] = fq[1];
				uframes[(i*15)+j][2] = fq[2];
				uframes[(i*15)+j][3] = fq[3];

				fits[0] = best_lookup( fq[0] - prev[j][0] );
				fits[1] = best_lookup( fq[1] - prev[j][1] );
				fits[2] = best_lookup( fq[2] - prev[j][2] );
				fits[3] = best_lookup( fq[3] - prev[j][3] );

				matches[0] = lookup[fits[0]];
				matches[1] = lookup[fits[1]];
				matches[2] = lookup[fits[2]];
				matches[3] = lookup[fits[3]];

				prev[j][0] += matches[0];
				prev[j][1] += matches[1];
				prev[j][2] += matches[2];
				prev[j][3] += matches[3];

				ucframes[(i*15)+j][0] = prev[j][0];
				ucframes[(i*15)+j][1] = prev[j][1];
				ucframes[(i*15)+j][2] = prev[j][2];
				ucframes[(i*15)+j][3] = prev[j][3];

				frames[((i-1)*15)+j][0] = fits[0];
				frames[((i-1)*15)+j][1] = fits[1];
				frames[((i-1)*15)+j][2] = fits[2];
				frames[((i-1)*15)+j][3] = fits[3];

#if 0
				/* coverage check */
				hits[fits[0]] ++;
				hits[fits[1]] ++;
				hits[fits[2]] ++;
				hits[fits[3]] ++;
#endif
				q1[0] = ucframes[(i*15)+j][0];
				q1[1] = ucframes[(i*15)+j][1];
				q1[2] = ucframes[(i*15)+j][2];
				q1[3] = ucframes[(i*15)+j][3];
				quat2mat( q1, m2 );

#if 0
				if((j==3) && (i<=25) && (i>=16))
				{
					printf( "f%d  l%d:  rotx(%f) roty(%f) rotz(%f) roty(%f)\n", i, j, r3, r4, r5, r6 );

					printf( "resulting matrix:  %9f %9f %9f  %9f %9f %9f   %9f %9f %9f - %9f\n",
						m1[0][0], m1[0][1], m1[0][2],
						m1[1][0], m1[1][1], m1[1][2],
						m1[2][0], m1[2][1], m1[2][2],
						m1[0][0] + m1[1][1] + m1[2][2] );
					printf( "quaternion:        %9f %9f %9f %9f\n", fq[0], fq[1], fq[2], fq[3] );
					printf( "decompressed quat: %9f %9f %9f %9f\n", q1[0], q1[1], q1[2], q1[3] );
					printf( "decompressed mat:  %9f %9f %9f  %9f %9f %9f   %9f %9f %9f\n",
						m2[0][0], m2[0][1], m2[0][2],
						m2[1][0], m2[1][1], m2[1][2],
						m2[2][0], m2[2][1], m2[2][2] );
				}
#endif
			}
		}

		/* compute crc */
		crcs[i] = crc( (char *)(uframes[15*i]), 240 );
	}

	/* output */
	strcpy( header.name, seqname );

	for( i = 0; i < strlen( header.name ); i++ )
		header.name[i] = toupper( header.name[i] );
	while( i < 16 )
		header.name[i++] = '\0';

	header.nframes = frame_count;
	header.nlimbs = 15;
	header.flags = 0;
	if( compress )
		header.flags |= A_COMPRESSED;

	if( output_mode == OM_BINARY )
	{
		/* header */
		fwrite( &header, sizeof( struct seq_header ), 1, outfile );

		/* translation */
		fwrite( xlate, sizeof( float ), header.nframes * 3, outfile );

		/* frame 0 */
		fwrite( frame0, sizeof( float ), header.nlimbs * 4, outfile );

		/* other frames */
		if( compress )
		{
			fwrite( frames, sizeof( char ), header.nlimbs * (header.nframes-1) * 4,
					outfile );
		}
		else
		{
			fwrite( uframes + header.nlimbs, sizeof( float ), header.nlimbs * (header.nframes-1) * 4,
					outfile );
		}
	}
	else
	{
		/* header */
		fprintf( outfile, "name: %s  frames: %d  limbs: %d\n", header.name,
				header.nframes, header.nlimbs );

		/* translation */

		/* frame 0 */
		fprintf( outfile, "\nframe 0: crc: 0x%lX\t\txlate: %9f %9f %9f\n", crcs[0],
				xlate[0][0], xlate[0][1], xlate[0][2] );
		for( i = 0; i < header.nlimbs; i++ )
			fprintf( outfile, " n/a  n/a  n/a  n/a | %9f %9f %9f %9f\n",
					frame0[i][0], frame0[i][1], frame0[i][2], frame0[i][3] );

		/* other frames */
		for( i = 0; i < header.nframes-1; i++ )
		{
			fprintf( outfile, "\nframe %d: crc: 0x%lX\t\t xlate: %9f %9f %9f\n", i+1, crcs[i+1],
					xlate[i+1][0], xlate[i+1][1], xlate[i+1][2] );
			if( compress )
			{
				for( j = 0; j < header.nlimbs; j++ )
				{
					unsigned char n0,n1,n2,n3;	/* compress qt indices */
					float	v0,v1,v2,v3;	/* pre-compressed result */
					float	t0,t1,t2,t3;	/* compressed + uncompressed result */
					float	d0,d1,d2,d3;	/* difference between tX and vX (loss ) */
					float	n;

					n0 = frames[i * header.nlimbs + j][0];
					n1 = frames[i * header.nlimbs + j][1];
					n2 = frames[i * header.nlimbs + j][2];
					n3 = frames[i * header.nlimbs + j][3];

	 				v0 = uframes[(i+1) * header.nlimbs + j][0];
	 				v1 = uframes[(i+1) * header.nlimbs + j][1];
	 				v2 = uframes[(i+1) * header.nlimbs + j][2];
	 				v3 = uframes[(i+1) * header.nlimbs + j][3];

	 				t0 = ucframes[(i+1) * header.nlimbs + j][0];
	 				t1 = ucframes[(i+1) * header.nlimbs + j][1];
	 				t2 = ucframes[(i+1) * header.nlimbs + j][2];
	 				t3 = ucframes[(i+1) * header.nlimbs + j][3];

					d0 = t0 - v0;
					d1 = t1 - v1;
					d2 = t2 - v2;
					d3 = t3 - v3;

					n = sqrt( t0 * t0 + t1 * t1 + t2 * t2 + t3 * t3 );

					fprintf( outfile, "%4u %4u %4u %4u | %9f %9f %9f %9f | %9f %9f %9f %9f | %9f %9f %9f %9f | %9f%s",
						0xff & n0, 0xff & n1, 0xff & n2, 0xff & n3,
						v0, v1, v2, v3,
						t0, t1, t2, t3,
						d0, d1, d2, d3,
						n,
						((( n >= 1.01 ) || ( n <= 0.99 )) ? " BAD\n" : "\n" )
						);
				}
			}
			else
			{
				for( j = 0; j < header.nlimbs; j++ )
				{
					float	v0,v1,v2,v3;	/* pre-compressed result */

	 				v0 = uframes[(i+1) * header.nlimbs + j][0];
	 				v1 = uframes[(i+1) * header.nlimbs + j][1];
	 				v2 = uframes[(i+1) * header.nlimbs + j][2];
	 				v3 = uframes[(i+1) * header.nlimbs + j][3];

					fprintf( outfile, "%4s %4s %4s %4s | %9f %9f %9f %9f | %9f %9f %9f %9f | %9f %9f %9f %9f | %9f\n",
						"n/a", "n/a", "n/a", "n/a",
						v0, v1, v2, v3,
						v0, v1, v2, v3,
						0.0f, 0.0f, 0.0f, 0.0f,
						sqrt( v0 * v0 + v1 * v1 + v2 * v2 + v3 * v3 )
						);
				}
			}
		}
	}

	fclose( outfile );

	/* write the crc file */
	if( write_crcs )
	{
		/* header */
		fprintf( crcfile, "name: %s  frames: %d  limbs: %d\n\n", header.name,
				header.nframes, header.nlimbs );

		/* frames */
		for( i = 0; i < frame_count; i++ )
			fprintf( crcfile ,"%3d %9f\t%9f\t%9f\t0x%0lX\n", i, xlate[i][0],
				xlate[i][1], xlate[i][2], crcs[i] );

		fclose( crcfile );
	}

	return 0;
}

/****************************************************************************/

void init_curve( char *text )
{
	sscanf( text, "parameter_curve %s (", curves[ncurves].name );
	npoints = 0;
}

/****************************************************************************/

void end_curve( void )
{
	int		i;
	float	*pd;

	/* allocate space for curve data */
	pd = (float *)malloc( sizeof( float ) * npoints );

	for( i = 0; i < npoints; i++ )
	{
		pd[i] = points[i];
	}

	curves[ncurves].pdata = pd;
	curves[ncurves].count = npoints;

	ncurves++;
}

/****************************************************************************/

void add_point( char *text )
{
	char *p;

	/* find start of second number in string */
	p = text;
	while( !isdigit( *p ))
		p++;

	while( *p != ' ' && *p != '\t' )
		p++;

	while( !isdigit( *p ) && *p != '-' )
		p++;

	points[npoints] = atof( p );
	npoints++;
}

/****************************************************************************/

int new_node( char *text )
{
	int i;

	/* figure out which node this is */
	for( i = 0; i < LIMB_COUNT; i++ )
	{
		if( !strcmp( (flip ? limb_names_flip[i] : limb_names[i]), text ))
			return i;
	}

	return -1;
}

/****************************************************************************/

void assign_channel( int channel, char *skey )
{
	int i;

	for( i = 0; (i < ncurves) && (strcmp( skey, curves[i].name)); i++ );

	if( channel >= 0 )
		limb_map[cur_node][channel] = i;
	else
	{
		switch( cur_node )
		{
			case 0:
				translate[channel+3] = i;
				break;
			case -1:
				translate[channel+6] = i;
				break;
			default:
				printf( "Warning: curve %s is translation data for non-root "
						"node.\n", skey );
				break;
		}
	}
}

/****************************************************************************/

void mxm( matrix m1, matrix m2, matrix d )
{
	int i, j;

	for( i = 0; i < 3; i++ )
	{
		for( j = 0; j < 3; j++ )
		{
			d[i][j] = m1[i][0] * m2[0][j] +
					  m1[i][1] * m2[1][j] +
					  m1[i][2] * m2[2][j];
		}
	}
}

/****************************************************************************/

void rotx( float deg, matrix m )
{
	float rad;

	rad = deg * 2.0f * 3.14159f / 360.0f;

	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = cos( rad );
	m[1][2] = -1.0f * sin( rad );

	m[2][0] = 0.0f;
	m[2][1] = sin( rad );
	m[2][2] = cos( rad );
}

/****************************************************************************/

void roty( float deg, matrix m )
{
	float rad;

	rad = deg * 2.0f * 3.14159f / 360.0f;

	m[0][0] = cos( rad );
	m[0][1] = 0.0f;
	m[0][2] = sin( rad );

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;

	m[2][0] = -1.0f * sin( rad );
	m[2][1] = 0.0f;
	m[2][2] = cos( rad );
}

/****************************************************************************/

void rotz( float deg, matrix m )
{
	float rad;

	rad = deg * 2.0f * 3.14159f / 360.0f;

	m[0][0] = cos( rad );
	m[0][1] = -1.0f * sin( rad );
	m[0][2] = 0.0f;

	m[1][0] = sin( rad );
	m[1][1] = cos( rad );
	m[1][2] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
}

/****************************************************************************/

int yywrap( void )
{
	return 1;
}

/****************************************************************************/
void mat2quat( matrix mat, quat q )
{
	float	tr,s;
	int		i,j,k;
	int		nxt[3] = {Y,Z,X};
	float	mag;

	tr = mat[0][0] + mat[1][1] + mat[2][2];
	if( tr > 0.0f )
	{
		s = sqrt( tr + 1.0f );
		q[W] = s * 0.5f;
		s = 0.5f / s;
		q[X] = -(mat[1][2] - mat[2][1]) * s;
		q[Y] = -(mat[2][0] - mat[0][2]) * s;
		q[Z] = -(mat[0][1] - mat[1][0]) * s;
	}
	else
	{
		i = X;
		if( mat[Y][Y] > mat[X][X] )
			i = Y;
		if( mat[Z][Z] > mat[i][i] )
			i = Z;

		j = nxt[i];
		k = nxt[j];

		s = sqrt((mat[i][i] - (mat[j][j]+mat[k][k])) + 1.0f);
		q[i] = s*0.5f;
		s = 0.5f / s;
		q[W] =-(mat[j][k] - mat[k][j]) * s;
		q[j] =(mat[i][j] + mat[j][i]) * s;
		q[k] =(mat[i][k] + mat[k][i]) * s;
	}

	// normalize
	mag = sqrt( q[X]*q[X] + q[Y]*q[Y] + q[Z]*q[Z] + q[W]*q[W] );

	q[X] /= mag;
	q[Y] /= mag;
	q[Z] /= mag;
	q[W] /= mag;
}

/****************************************************************************/
void quat2mat( quat q, matrix m )
{
	float	s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz;

	s = 2.0f;

	xs = q[X]*s;	ys = q[Y]*s;	zs = q[Z]*s;
	wx = q[W]*xs;	wy = q[W]*ys;	wz = q[W]*zs;
	xx = q[X]*xs;	xy = q[X]*ys;	xz = q[X]*zs;
	yy = q[Y]*ys;	yz = q[Y]*zs;	zz = q[Z]*zs;

	m[0][0] = 1.0f-(yy+zz);	m[0][1] = xy-wz;		m[0][2] = xz+wy;
	m[1][0] = xy+wz;		m[1][1] = 1.0f-(xx+zz);	m[1][2] = yz-wx;
	m[2][0] = xz-wy;		m[2][1] = yz+wx;		m[2][2] = 1.0f-(xx+yy);
}

/****************************************************************************/
int best_lookup( float val )
{
	int		i;

	/* take the highest value that doesn't exceed the target */

	for( i = 1; i < 256; i++ )
	{
		if( lookup[i]  > val )
			return i-1;
	}

	return 255;
}

/****************************************************************************/
void print_help( void )
{
	printf( "sdl2seq [options] infile\n" );
	printf( " -o <file>     write output to <file>\n" );
	printf( " -n <name>     sequence name\n" );
	printf( " -s <factor>   scale translation data\n" );
	printf( " -t            text output (debug only - not C source\n" );
	printf( " -c <file>     write frame crc's to <file>\n" );
}

/****************************************************************************/
void print_mat( matrix m )
{
	printf( "\n%9f %9f %9f - %9f\n", m[0][0], m[0][1], m[0][2],
			sqrt( m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2] ));
	printf( "%9f %9f %9f - %9f\n", m[1][0], m[1][1], m[1][2],
			sqrt( m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2] ));
	printf( "%9f %9f %9f - %9f\n", m[2][0], m[2][1], m[2][2],
			sqrt( m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2] ));
}

/****************************************************************************/
void print_quat( quat q )
{
	printf( "\n%9f %9f %9f %9f - %9f\n", q[0], q[1], q[2], q[3],
			sqrt( q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3] ));
}

/****************************************************************************/
// if -p is a better match to q, set p to -p.
void adjust_quat( quat p, quat q )
{
	float	pdq,npdq;

	pdq = (p[0]-q[0])*(p[0]-q[0]) +
			(p[1]-q[1])*(p[1]-q[1]) +
			(p[2]-q[2])*(p[2]-q[2]) +
			(p[3]-q[3])*(p[3]-q[3]);

	npdq = (p[0]+q[0])*(p[0]+q[0]) +
			(p[1]+q[1])*(p[1]+q[1]) +
			(p[2]+q[2])*(p[2]+q[2]) +
			(p[3]+q[3])*(p[3]+q[3]);

	if( npdq < pdq )
	{
		p[0] *= -1.0f;
		p[1] *= -1.0f;
		p[2] *= -1.0f;
		p[3] *= -1.0f;
	}
}

/****************************************************************************/
