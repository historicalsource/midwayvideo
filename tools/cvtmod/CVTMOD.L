%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
	 
#define FALSE	0
#define TRUE	!FALSE

#define PCOUNT	1024		/* max polysets */
#define NUM_SHADERS	128	/* max shaders (textures) per pset */

void init_pset( char * );
void end_pset( void );
void add_vertex( char * );
void add_st( char * );
void add_tri( char * );
void add_tri2( char * );
void add_shader( char * );
void begin_shader( char * );
void begin_texture( char * );
void get_tex_name( char * );
void get_shd_name( char * );
void get_tex_type( char * );
void get_tex_parm( char *, float * );
void print_help( void );
int tricmp( const void *, const void * );
int tpcmp( const void *, const void * );

int nvtx = 0, nst = 0, ntri = 0, nsets = 0;
int ntex = 0, nshd = 0;

enum ttypes { TT_FILE, TT_PROJECTION, TT_SFRACTAL, TT_SKY };
enum stypes { ST_TEXTURE, ST_OTHER };

#ifdef JONHEY
char dir_name[128] = {0};

typedef struct wms_header 
{
	char reserved[16];
	int  w;
	int  h;
	int  small;
	int  large;
}	wms_header_t;
#endif

typedef struct _TEXTURE
{
	char	name[48];
	int		ttype;
	char	file[48];
	float	rotate;
	float	urepeat, vrepeat;
	float	uoffset, voffset;
	int		refs;			/* # of triangles that use it */
	int		index;			/* index after unref's textures are dropped */
} TEXTURE;

typedef struct _SHADER
{
	char	name[48];
	int		stype;
	char	texture[48];
} SHADER;

typedef struct _VERTEX
{
	float x,y,z;
} VERTEX;

typedef struct _ST
{
	float s,t;
} ST;

typedef struct _TRI
{
	int v1,v2,v3;
	int t1,t2,t3;
	int texture;
} TRI;

typedef struct _QUAD
{
	int v1,v2,v3,v4;
} QUAD;

typedef struct _POLYSET
{
	char	name[32];
	int		nvtx;
	int		ntri;
	int		nst;
	VERTEX	*pvtx;
	VERTEX	*pvn;
	ST		*pst;
	TRI		*ptri;
	int		shaders[NUM_SHADERS];
	int		nshaders;
} POLYSET;

#define MAX_VERTEX	(10 * 1024)
#define MAX_ST		(MAX_VERTEX * 3)
#define MAX_TRIS	(MAX_VERTEX * 4)
#define MAX_SHADER	(1024)
#define MAX_TEXTURE	(1024)

VERTEX	vtx[MAX_VERTEX];
ST		st[MAX_ST];
ST		new_st[MAX_ST];
TRI		tris[MAX_TRIS];
POLYSET psets[PCOUNT];
SHADER	shaders[MAX_SHADER];
TEXTURE	textures[MAX_TEXTURE];
TEXTURE *tptrs[MAX_TEXTURE];
VERTEX	*sort_vtx;
char	cur_tex[128];
char	cur_shd[128];
int		last_st[MAX_TRIS];
float	sfx = 1.0f;
float	sfy = 1.0f;
float	sfz = 1.0f;
int		debug_mode;
int		zsort_mode;
int		no_limbs;
int		no_textures;
int		net_anim;
int		reverse_tsort;			// reverse-sort by texture index
int		do_tsort;
%}

%x		PSET
%s		STEX
%s		SSHD

ws		[ \t]+
num 	-?(([0-9]+)|([0-9]*\.[0-9]+)([eE][+-]?[0-9]+)?)
name	[a-zA-Z0-9_#]+

%%

^polyset{ws}{name}						{ BEGIN PSET; init_pset( yytext ); };
<PSET>");"								{ BEGIN 0; end_pset(); };
<PSET>"polygon (("{num},{ws}{num},{ws}{num}"),("{num},{ws}{num},{ws}{num}"),"{ws}{num} add_tri( yytext );
<PSET>"polygon (("{num},{ws}{num},{ws}{num}"),("{num},{ws}{num},{ws}{num}"),("{num},{ws}{num},{ws}{num}"),"{ws}{num} add_tri2( yytext );
<PSET>"cv(("{num},{ws}{num},{ws}{num}	add_vertex( yytext );
<PSET>"st("{ws}{num},{ws}{num}			add_st( yytext );
<PSET>"shader = ("{name}				add_shader( yytext );
<PSET>.|\n								;

^texture{ws}{name}						{begin_texture( yytext );BEGIN STEX;}
^shader{ws}{name}						{begin_shader( yytext );BEGIN SSHD;}
<STEX>"procedure = "[a-zA-Z]+/,			get_tex_type( yytext );
<STEX>"image =".*,						get_tex_name( yytext );
<STEX>"rotate ="{ws}{num}				get_tex_parm( yytext, &textures[ntex].rotate );
<STEX>"urepeat ="{ws}{num}				get_tex_parm( yytext, &textures[ntex].urepeat );
<STEX>"vrepeat ="{ws}{num}				get_tex_parm( yytext, &textures[ntex].vrepeat );
<STEX>"uoffset ="{ws}{num}				get_tex_parm( yytext, &textures[ntex].uoffset );
<STEX>"voffset ="{ws}{num}				get_tex_parm( yytext, &textures[ntex].voffset );
<STEX>^");"								{ntex++;BEGIN 0;}
<SSHD>"phong"							{get_shd_name( NULL );BEGIN 0;}
<SSHD>"color = "{name}/,				{get_shd_name( yytext );BEGIN 0;}
<SSHD>"color = ("						{get_shd_name( NULL );BEGIN 0;}

.|\n									;

%%

typedef enum { PS_NONE, PS_NAME, PS_SCALE, PS_SCALEX, PS_SCALEY,
	PS_SCALEZ, PS_APPEND, PS_MAP, PS_MAPW, PS_MAPH, PS_NETANIM } pstate;

/**********************************************************************/
int main( int argc, char *args[] )
{
	int i, j, k, m, n;
	int cf_square = FALSE;
	int cf_1x2 = FALSE;
	int cf_w = 0;
	int cf_h = 0;
#ifdef JONHEY
	int ar = 0;
	float sf;
	float tf;
#endif
	pstate pmode = PS_NONE;
	POLYSET *cp;
	char empty_string[] = "";
	char *np = NULL, *papp = empty_string, *nan = NULL;

#ifdef JONHEY
	float max_x,max_y,max_z,min_x,min_y,min_z;
	float vx,vy,vz;
#endif


	/* parse command flags */
	debug_mode = FALSE;
	zsort_mode = FALSE;
	no_limbs = FALSE;
	no_textures = FALSE;
	net_anim = FALSE;
	reverse_tsort = FALSE;
	do_tsort = TRUE;

	for( i = 1; i < argc; i++ )
	{
		switch( args[i][0] )
		{
			case '-':
				switch( args[i][1] )
				{
					case 'a':	/* append string to pset names */
						pmode = PS_APPEND;
						break;
					case 'd':	/* debug mode */
						debug_mode = TRUE;
						break;
					case 'f':	/* scale factor */
						switch( args[i][2] )
						{
							case '\0':
								pmode = PS_SCALE;
								break;
							case 'x':
								pmode = PS_SCALEX;
								break;
							case 'y':
								pmode = PS_SCALEY;
								break;
							case 'z':
								pmode = PS_SCALEZ;
								break;
							default:
								fprintf(stderr,"WARNING: Unrecognized command "
										"line option: %s\n", args[i] );
								break;
						}
						break;
					case 'h':	/* help */
						print_help();
						exit( 1 );
						break;
				#ifdef JONHEY
					case 'i':
						strcpy(dir_name,&args[i][2]);
						break;
				#endif
					case 'l':	/* no limb list */
						no_limbs = TRUE;
						break;
					case 'm':	/* scale factor */
						if (cf_square || cf_1x2)
						{
							fprintf(stderr,"WARNING: %s will be overridden "
									"by a previous option\n", args[i] );
						}
						switch( args[i][2] )
						{
							case '\0':
								pmode = PS_MAP;
								break;
							case 'w':
								pmode = PS_MAPW;
								break;
							case 'h':
								pmode = PS_MAPH;
								break;
							default:
								fprintf(stderr,"WARNING: Unrecognized command "
										"line option: %s\n", args[i] );
								break;
						}
						break;
					case 'n':	/* name */
						pmode = PS_NAME;
						break;
					case 'q':	/* no texture sort */
						do_tsort = FALSE;
						break;
					case 'r':	/* reverse texture sort */
						reverse_tsort = TRUE;
						break;
					case 's':	/* assume square textures */
						cf_square = TRUE;
						break;
					case 't':	/* no texture list */
						no_textures = TRUE;
						break;
					case 'v':	/* just output the vertex list */
						net_anim = TRUE;
						pmode = PS_NETANIM;
						break;
					case 'x':	/* assume 2x1 textures */
						if (cf_square)
						{
							fprintf(stderr,"WARNING: %s will be overridden "
									"by a previous option\n", args[i] );
						}
						cf_1x2 = TRUE;
						break;
					case 'z':	/* z-sort triangles */
						zsort_mode = TRUE;
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
					case PS_NETANIM:
						nan = args[i];
						break;
					case PS_NAME:
						np = args[i];
						break;
					case PS_SCALE:
						sfx = sfy = sfz = atof( args[i] );
						break;
					case PS_SCALEX:
						sfx = atof( args[i] );
						break;
					case PS_SCALEY:
						sfy = atof( args[i] );
						break;
					case PS_SCALEZ:
						sfz = atof( args[i] );
						break;
					case PS_APPEND:
						papp = args[i];
						break;
					case PS_MAP:
						cf_w = cf_h = atoi( args[i] );
						break;
					case PS_MAPW:
						cf_w = atoi( args[i] );
						break;
					case PS_MAPH:
						cf_h = atoi( args[i] );
						break;
					default:
						break;
				}
				break;
		}
	}


#ifdef JONHEY
	dir_name[0] = '\\';
	dir_name[1] = 'v';
	dir_name[2] = 'i';
	dir_name[3] = 'd';
	dir_name[4] = 'e';
	dir_name[5] = 'o';
	dir_name[6] = '\\';
	dir_name[7] = 'n';
	dir_name[8] = 'b';
	dir_name[9] = 'a';
	dir_name[10] = '\\';
	dir_name[11] = 'i';
	dir_name[12] = 'm';
	dir_name[13] = 'g';
	dir_name[14] = '3';
	dir_name[15] = 'd';
#endif


	/* read data */
	yylex();

	/* texture coverage check */
	for( i = 0; i < nsets; i++ )
	{
		cp = psets + i;

		for( j = 0; j < cp->ntri; j++ )
		{
			int prev_tex = -1;

			if( cp->ptri[j].texture != -1 )
				prev_tex = psets[i].shaders[cp->ptri[j].texture];

			if( prev_tex == -1 )
				printf( "Problem in polyset %s.\n", cp->name );

			assert( prev_tex != -1 );

			textures[prev_tex].refs++;
		}
	}

	/* make an array of pointers to the texture list, then sort it by name */
	for( j = 0; j < ntex; j++ )
	{
		tptrs[j] = textures + j;
	}
	qsort( tptrs, ntex, sizeof( TEXTURE * ), tpcmp );

	/* assign texture indexes */
	for( j = k = 0; j < ntex; j++ )
	{
		if( tptrs[j]->refs )
			tptrs[j]->index = k++;
		else
			tptrs[j]->index = -1;
	}


	for( i = 0; i < nsets; i++ )
	{
		cp = psets + i;
		printf( "VERTEX %s%s_vtx[] =\n{\n", psets[i].name, papp );

		for( j = 0; j < cp->nvtx; j++ )
		{
			printf( "\t{%ff,%ff,%ff}",
					cp->pvtx[j].x,
					cp->pvtx[j].y,
					cp->pvtx[j].z );
			if( j < cp->nvtx - 1 )
				printf( ",\n" );
		}
		
		printf( "\n};\n\n" );
	}






#ifdef JONHEY

	for( i = 0; i < nsets; i++ )
	{
		cp = psets + i;
		printf( "VERTEX %s%s_bbox[] =\n{\n", psets[i].name, papp );

		max_x = cp->pvtx[0].x;
		max_y = cp->pvtx[0].y;
		max_z = cp->pvtx[0].z;
		min_x = cp->pvtx[0].x;
		min_y = cp->pvtx[0].y;
		min_z = cp->pvtx[0].z;
			
		for( j = 0; j < cp->nvtx; j++ )
			{

			vx = cp->pvtx[j].x;
			vy = cp->pvtx[j].y;
			vz = cp->pvtx[j].z;

			if (vx < min_x)
				min_x = vx;

			if (vy < min_y)
				min_y = vy;

			if (vz < min_z)
				min_z = vz;

			if (vx > max_x)
				max_x = vx;

			if (vy > max_y)
				max_y = vy;

			if (vz > max_z)
				max_z = vz;

			}
	
			printf("\t{%ff,%ff,%ff},\n",min_x,min_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,min_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,max_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,max_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,min_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,min_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,max_y,min_z);
			printf("\t{%ff,%ff,%ff}\n};\n\n",max_x,max_y,max_z);

	}

#endif


	if(net_anim)
	{
		printf( "VERTEX *%s_verts[] =\n{\n", np ? np : "xxx" );
		for( i = 0; i < nsets; i++ )
			printf( "\t%s%s_vtx,\n", psets[i].name, papp );

//printf( "\t&limb_%s%s,\n", psets[i].name, papp );

		printf( "\tNULL\n};\n\n" );

#ifdef JONHEY
		printf( "VERTEX *%s_bbox[] =\n{\n", np ? np : "xxx" );
		for( i = 0; i < nsets; i++ )
			{
			if (psets[i].nvtx > 8)
				{
				printf( "\t%s%s_bbox,\n", psets[i].name, papp );
				}
			else
				{
				printf("\tNULL,\t/* bounding box */\n" );
				}
			}
		printf( "\tNULL\n};\n\n" );
#endif

	}

 if(!net_anim)
 {
	/* mangle data */

	for( i = 0; i < nsets; i++ )
	{
		cp = psets + i;

		/* sort the triangles by texture index and mean z */
		sort_vtx = cp->pvtx;
		qsort( cp->ptri, cp->ntri, sizeof( TRI ), tricmp );

		/* create unique u-v pairs for each instance of a vertex */
		for( j = 0; j < cp->ntri; j++ )
		{
			TEXTURE *ptex = textures + cp->shaders[cp->ptri[j].texture];
	
		#ifdef JONHEY
			ar = get_aspect_ratio (ptex->file);

			sf = tf = 256.0f;
			switch( ar )
			{
				case 0:	/* 8W x 1H */
					tf = 32.0f;
					break;
				case 1:	/* 4W x 1H */
					tf = 64.0f;
					break;
				case 2:	/* 2W x 1H */
					tf = 128.0f;
					break;
				case 3:	/* 1W x 1H */
					break;
				case 4:	/* 1W x 2H */
					sf = 128.0f;
					break;
				case 5:	/* 1W x 4H */
					sf = 64.0f;
					break;
				case 6:	/* 1W x 8H */
					sf = 32.0f;
					break;
				default:
					fprintf(stderr,"unrecognized aspect ratio: %d\n\r",ar);
					exit (0);
			}	

			new_st[j*3].s = sf * cp->pst[cp->ptri[j].t1].s;
			new_st[j*3].t = tf * (1.0f - cp->pst[cp->ptri[j].t1].t);
			new_st[j*3+1].s = sf * cp->pst[cp->ptri[j].t2].s;
			new_st[j*3+1].t = tf * (1.0f - cp->pst[cp->ptri[j].t2].t);
			new_st[j*3+2].s = sf * cp->pst[cp->ptri[j].t3].s;
			new_st[j*3+2].t = tf * (1.0f - cp->pst[cp->ptri[j].t3].t);

		#else

			new_st[j*3].s = cp->pst[cp->ptri[j].t1].s;
			new_st[j*3].t = cp->pst[cp->ptri[j].t1].t;
			new_st[j*3+1].s = cp->pst[cp->ptri[j].t2].s;
			new_st[j*3+1].t = cp->pst[cp->ptri[j].t2].t;
			new_st[j*3+2].s = cp->pst[cp->ptri[j].t3].s;
			new_st[j*3+2].t = cp->pst[cp->ptri[j].t3].t;

		#endif			

			new_st[j*3].s *= ptex->urepeat;
			new_st[j*3].t *= ptex->vrepeat;
			new_st[j*3+1].s *= ptex->urepeat;
			new_st[j*3+1].t *= ptex->vrepeat;
			new_st[j*3+2].s *= ptex->urepeat;
			new_st[j*3+2].t *= ptex->vrepeat;

			new_st[j*3].s += ptex->uoffset;
			new_st[j*3].t += ptex->voffset;
			new_st[j*3+1].s += ptex->uoffset;
			new_st[j*3+1].t += ptex->voffset;
			new_st[j*3+2].s += ptex->uoffset;
			new_st[j*3+2].t += ptex->voffset;

			cp->ptri[j].t1 = j*3;
			cp->ptri[j].t2 = j*3+1;
			cp->ptri[j].t3 = j*3+2;
		}
		cp->nst = j * 3;


		// crunch the list, eliminating duplicates
		// A: Start at the front of the list and scan forward for matches.
		// When a match is found, go through the triangle list and change
		// any references to the later match to point to the lower match,
		// then shuffle the remaining list entries down a notch to remove
		// the match, and keep going.
		for( k = 0; k < cp->nst; k++ )
		{
			for( m = k+1; m < cp->nst; m++ )
			{

				if(( new_st[k].s == new_st[m].s ) &&
					(new_st[k].t == new_st[m].t ))
				{
					/* update the triangle list */
					for( n = 0; n < cp->ntri; n++ )
					{
						if(cp->ptri[n].t1 == m)
							cp->ptri[n].t1 = k;
						if(cp->ptri[n].t2 == m)
							cp->ptri[n].t2 = k;
						if(cp->ptri[n].t3 == m)
							cp->ptri[n].t3 = k;
						if(cp->ptri[n].t1 > m)
							cp->ptri[n].t1 -= 1;
						if(cp->ptri[n].t2 > m)
							cp->ptri[n].t2 -= 1;
						if(cp->ptri[n].t3 > m)
							cp->ptri[n].t3 -= 1;
					}

					/* shuffle the remaning ST entries each down a notch */
					for( n = m+1; n < cp->nst; n++ )
					{
						new_st[n-1].s = new_st[n].s;
						new_st[n-1].t = new_st[n].t;
					}
	
					cp->nst -= 1;
					m -= 1;
				}
			}
		}

		/* check to see if we will collide with the 255 sentinel */
		if (cp->nst > 254) {
			for (n = cp->nst; n >= 255; n--) {
				new_st[n + 1].s = new_st[n].s;
				new_st[n + 1].t = new_st[n].t;
			}
			new_st[255].s = 0;
			new_st[255].t = 0;
			cp->nst++;

			/* update the triangle list */
			for (n = 0; n < cp->ntri; n++) {
				if(cp->ptri[n].t1 >= 255)
					cp->ptri[n].t1++;
				if(cp->ptri[n].t2 >= 255)
					cp->ptri[n].t2++;
				if(cp->ptri[n].t3 >= 255)
					cp->ptri[n].t3++;
			}
		}			

		// Assign texture codes.  A nonnegative integer is a texture
		// index, and all three vertex uv pairs will be pointers.  a
		// negative integer means that we use the same texture as the
		// previous triangle.  Value will be -1, minus the number of
		// uv indices that are -1.  Just set it to -1 for now, and
		// let the rotate loop subtract off the number of -1s among
		// the uv indices.
		for( j = cp->ntri -1; j; j-- )
		{
			if( cp->ptri[j-1].texture == cp->ptri[j].texture )
				cp->ptri[j].texture = -1;
		}

		// walk the triangle list.  Any time a vertex is matched with
		// the same ST index as the last time it was referenced, change
		// the index to a 255.  This signals the geometry code to skip
		// the vtx->oow * st multiplies.  DONT, however, do this in
		// triangles that begin a new texture.  This shouldn't ever
		// happen anyway, though.
		for( j = 0; j < cp->ntri*3; j++ )
			last_st[j] = 255;
		for( j = 0; j < cp->ntri; j++ )
		{
			// check for new texture
			if( cp->ptri[j].texture >= 0 )
			{
				last_st[cp->ptri[j].v1] = cp->ptri[j].t1;
				last_st[cp->ptri[j].v2] = cp->ptri[j].t2;
				last_st[cp->ptri[j].v3] = cp->ptri[j].t3;
				continue;
			}

#ifndef JONHEY	
			if( cp->ptri[j].t1 == last_st[cp->ptri[j].v1] )
				cp->ptri[j].t1 = 255;
			else
#endif
				last_st[cp->ptri[j].v1] = cp->ptri[j].t1;

#ifndef JONHEY	
			if( cp->ptri[j].t2 == last_st[cp->ptri[j].v2] )
				cp->ptri[j].t2 = 255;
			else
#endif
				last_st[cp->ptri[j].v2] = cp->ptri[j].t2;

#ifndef JONHEY	
			if( cp->ptri[j].t3 == last_st[cp->ptri[j].v3] )
				cp->ptri[j].t3 = 255;
			else
#endif
				last_st[cp->ptri[j].v3] = cp->ptri[j].t3;
		}

		// Now go through the triangle list and rotate vs and ts such
		// that all the 255's in the ts are packed to the right.
		for( j = 0; j < cp->ntri; j++ )
		{
			k = 0;

			if( cp->ptri[j].t3 == 255 )
			{
				k |= 0x01;
				cp->ptri[j].texture--;
			}
			if( cp->ptri[j].t2 == 255 )
			{
				k |= 0x02;
				cp->ptri[j].texture--;
			}
			if( cp->ptri[j].t1 == 255 )
			{
				k |= 0x04;
				cp->ptri[j].texture--;
			}

			if(( k == 2 ) || ( k == 6 ))
			{
				// rotate right
				m = cp->ptri[j].v1;
				cp->ptri[j].v1 = cp->ptri[j].v3;
				cp->ptri[j].v3 = cp->ptri[j].v2;
				cp->ptri[j].v2 = m;
				m = cp->ptri[j].t1;
				cp->ptri[j].t1 = cp->ptri[j].t3;
				cp->ptri[j].t3 = cp->ptri[j].t2;
				cp->ptri[j].t2 = m;

			}

			if(( k == 4 ) || ( k == 5 ))
			{
				// rotate left
				m = cp->ptri[j].v3;
				cp->ptri[j].v3 = cp->ptri[j].v1;
				cp->ptri[j].v1 = cp->ptri[j].v2;
				cp->ptri[j].v2 = m;
				m = cp->ptri[j].t3;
				cp->ptri[j].t3 = cp->ptri[j].t1;
				cp->ptri[j].t1 = cp->ptri[j].t2;
				cp->ptri[j].t2 = m;
			}
		}

#if 0
		printf( "VERTEX %s%s_vn[] =\n{\n", psets[i].name, papp );
		for( j = 0; j < cp->nvtx; j++ )
		{
			printf( "\t{%ff,%ff,%ff}",
					cp->pvn[j].x,
					cp->pvn[j].y,
					cp->pvn[j].z );
			if( j < cp->nvtx - 1 )
				printf( ",\n" );
		}
#endif

		printf( "TRI %s%s_tris[] =\n{\n", psets[i].name, papp );
		for( j = 0; j < cp->ntri; j++ )
		{

	#ifndef JONHEY											 
			printf( "\t{%d,%d,%d, %d,%d,%d, %d},\n",
					3*cp->ptri[j].v1,
					3*cp->ptri[j].v2,
					3*cp->ptri[j].v3,
					cp->ptri[j].t1,
					cp->ptri[j].t2,
					cp->ptri[j].t3,
					cp->ptri[j].texture < 0 ? (cp->ptri[j].texture * 4) :
					(4*textures[psets[i].shaders[cp->ptri[j].texture]].index));

	#else

			printf( "\t{%d,%d,%d, %d,%d,%d, %d},\n",
					cp->ptri[j].v1,
					cp->ptri[j].v2,
					cp->ptri[j].v3,
					cp->ptri[j].t1,
					cp->ptri[j].t2,
					cp->ptri[j].t3,
					cp->ptri[j].texture < 0 ? (cp->ptri[j].texture * 4) :
					(4*textures[psets[i].shaders[cp->ptri[j].texture]].index));
	
	#endif

		}
		printf( "\t{0, 0, 0, 0, 0, 0, -20}\n};\n\n" );

		printf( "ST %s%s_st[] =\n{\n", psets[i].name, papp );
		for( j = 0; j < cp->nst; j++ )
		{

	#ifndef JONHEY											 
			float	sf,tf;

			if (cf_square)
			{
				sf = 256.0f;	tf = 256.0f;
			}
			else if (cf_1x2)
			{
				sf = 256.0f;	tf = 128.0f;
			}
			else
			{
				sf = pow2(cf_w);		tf = pow2(cf_h);
			}


			printf( "\t{%ff,%ff}",
					sf * new_st[j].s,
					tf * (1.0f - new_st[j].t ));
			if( j < cp->nst - 1 )
				printf( ",\n" );
	#else

			printf( "\t{%ff,%ff}",
					new_st[j].s,
					new_st[j].t);

			if( j < cp->nst - 1 )
				printf( ",\n" );
  	#endif
		}

		printf( "\n};\n\n" );


	}


	for( i = 0; i < nsets; i++ )
	{
		printf("LIMB limb_%s%s = \n{\n", psets[i].name, papp);
		printf("\t%d,\t\t\t\t/* vertex count */\n", psets[i].nvtx);
		printf("\t%d,\t\t\t\t/* triangle count */\n", psets[i].ntri);
#if 0
		printf("\t%s%s_vn,\t/* vertex normals */\n", psets[i].name, papp);
#else
		printf("\tNULL,\t/* vertex normals */\n" );
#endif
		printf("\t%s%s_vtx,\t/* vertices */\n", psets[i].name, papp);
		printf("\t%s%s_st,\t/* texture coordinates */\n", psets[i].name, papp);
		printf("\t%s%s_tris,\t/* triangles */\n", psets[i].name, papp);
#ifdef JONHEY
		if (psets[i].nvtx > 8)
			{
			printf("\t%s%s_bbox,\t/* bounding box */\n", psets[i].name, papp);
			}
		else
			{
			printf("\tNULL,\t/* bounding box */\n" );
			}
#endif
		printf("};\n\n");
	}

	if (!no_limbs)
	{
		printf( "LIMB *%s_limbs[] =\n{\n", np ? np : "xxx" );
		for( i = 0; i < nsets; i++ )
			printf( "\t&limb_%s%s,\n", psets[i].name, papp );
		printf( "\tNULL\n};\n\n" );
	}

	if (!no_textures)
	{
		printf( "char *%s_textures[] =\n{\n", np ? np : "xxx" );
		for( i = 0; i < ntex; i++ )
		{
			if( tptrs[i]->index != -1 )
			{
				if( tptrs[i]->file[strlen(tptrs[i]->file)-4] == '.' )
				{
					tptrs[i]->file[strlen(tptrs[i]->file)-3] = 'w';
					tptrs[i]->file[strlen(tptrs[i]->file)-2] = 'm';
					tptrs[i]->file[strlen(tptrs[i]->file)-1] = 's';
				}
				printf( "\t\"%s\",\n", tptrs[i]->file );
			}
		}
		printf( "\tNULL\n};\n" );
	}
 }
	return 0;
}


/**********************************************************************/
void init_pset( char *text )
{
	int i;

	sscanf( text, "polyset %s (", psets[nsets].name );
	nvtx = 0;
	ntri = 0;
	nst = 0;
	for( i = 0; i < NUM_SHADERS; i++ )
		psets[nsets].shaders[i] = 0;
	psets[nsets].nshaders = 0;

	if( debug_mode )
	{
		printf( "init_pset( %s )\n", psets[nsets].name );
	}
}


/**********************************************************************/
void end_pset( void )
{
	VERTEX	*pvtx, *vn;
	TRI		*ptri;
	ST		*pst;
	int		i;

	/* allocate space for vertex normals */
	vn = (VERTEX *)malloc( sizeof( VERTEX ) * nvtx );
	if (vn == NULL) {
		fprintf(stderr, "could not malloc vertex normal list\n");
		abort();
	}

	/* compute vertex normals */
	for( i = 0; i < nvtx; i++ )
	{
		vn[i].x = 0.0f;
		vn[i].y = 0.0f;
		vn[i].z = 0.0f;
	}

	for( i = 0; i < ntri; i++ )
	{
		VERTEX v12, v13, vsn;

		/* compute surface normal */
		v12.x = vtx[tris[i].v2].x - vtx[tris[i].v1].x;
		v12.y = vtx[tris[i].v2].y - vtx[tris[i].v1].y;
		v12.z = vtx[tris[i].v2].z - vtx[tris[i].v1].z;

		v13.x = vtx[tris[i].v3].x - vtx[tris[i].v1].x;
		v13.y = vtx[tris[i].v3].y - vtx[tris[i].v1].y;
		v13.z = vtx[tris[i].v3].z - vtx[tris[i].v1].z;

		vsn.x = v13.y * v12.z - v13.z * v12.y;
		vsn.y = v13.z * v12.x - v13.x * v12.z;
		vsn.z = v13.x * v12.y - v13.y * v12.x;

		/* add to component vertex normals */
		vn[tris[i].v1].x += vsn.x;
		vn[tris[i].v1].y += vsn.y;
		vn[tris[i].v1].z += vsn.z;

		vn[tris[i].v2].x += vsn.x;
		vn[tris[i].v2].y += vsn.y;
		vn[tris[i].v2].z += vsn.z;

		vn[tris[i].v3].x += vsn.x;
		vn[tris[i].v3].y += vsn.y;
		vn[tris[i].v3].z += vsn.z;
	}

	/* normalize vertex normals */
	for( i = 0; i < nvtx; i++ )
	{
		float magv;

		magv = (float)sqrt((double)(vn[i].x*vn[i].x+vn[i].y*vn[i].y+vn[i].z*vn[i].z));
		if (magv > 0.000001f) {
			vn[i].x /= magv;
			vn[i].y /= magv;
			vn[i].z /= magv;
		}
	}

	/* make a copy of the vertex scratch pad */
	pvtx = (VERTEX *)malloc( sizeof( VERTEX ) * nvtx );

	if( pvtx == NULL )
	{
		printf( "Error allocating space for vertex list.  Aborting.\n" );
		exit( 0 );
	}

	for( i = 0; i < nvtx; i++ )
	{
		pvtx[i].x = vtx[i].x;
		pvtx[i].y = vtx[i].y;
		pvtx[i].z = vtx[i].z;
	}


	/* make a copy of the triangle scratch pad */
	ptri = (TRI *)malloc( sizeof( TRI ) * ntri );

	if( ptri == NULL )
	{
		printf( "Error allocating space for triangle list.  Aborting.\n" );
		exit( 0 );
	}

	for( i = 0; i < ntri; i++ )
	{
		ptri[i].v1 = tris[i].v1;
		ptri[i].v2 = tris[i].v2;
		ptri[i].v3 = tris[i].v3;
		ptri[i].t1 = tris[i].t1;
		ptri[i].t2 = tris[i].t2;
		ptri[i].t3 = tris[i].t3;
		ptri[i].texture = tris[i].texture;
	}

	/* make a copy of the s&t scratch pad */
	pst = (ST *)malloc( sizeof( ST ) * nst );

	if( pst == NULL )
	{
		printf( "Error allocating space for s&t list.  Aborting.\n" );
		exit( 0 );
	}

	for( i = 0; i < nst; i++ )
	{
		pst[i].s = st[i].s;
		pst[i].t = st[i].t;
	}

	psets[nsets].pvtx = pvtx;
	psets[nsets].ptri = ptri;
	psets[nsets].pst = pst;
	psets[nsets].nvtx = nvtx;
	psets[nsets].ntri = ntri;
	psets[nsets].nst = nst;
	psets[nsets].pvn = vn;

	if( debug_mode )
	{
		printf( "end_pset( %s )\n", psets[nsets].name );
		printf( " nvtx: %d ntri: %d\n", nvtx, ntri );
	}

	nsets++;
}


/**********************************************************************/
void add_vertex( char *text )
{
	float x,y,z;

	if( nvtx == MAX_VERTEX )
	{
		fprintf( stderr, "Error: Too many vertices.\n" );
		exit( 0 );
	}

	sscanf( text, "cv((%f, %f, %f", &x, &y, &z );
	vtx[nvtx].x = x * sfx;
	vtx[nvtx].y = y * sfy;
	vtx[nvtx].z = z * sfz;
	nvtx++;
}


/**********************************************************************/
void add_st( char *text )
{
	float s,t;

	if( nst == MAX_ST )
	{
		fprintf( stderr, "Error: Too many st pairs.\n" );
		exit( 0 );
	}

	sscanf( text, "st( %f, %f", &s, &t );
	st[nst].s = s;
	st[nst].t = t;
	nst++;
}


/**********************************************************************/
void add_tri( char *text )
{
	int	v1,v2,v3,t1,t2,t3,tex;

	if( ntri == MAX_TRIS )
	{
		fprintf( stderr, "Error: Too many triangles.\n" );
		exit( 0 );
	}

	sscanf( text, "polygon ((%d, %d, %d),(%d, %d, %d), %d",
			&v1, &v2, &v3, &t1, &t2, &t3, &tex );
	tris[ntri].v1 = v1;
	tris[ntri].v2 = v2;
	tris[ntri].v3 = v3;
	tris[ntri].t1 = t1;
	tris[ntri].t2 = t2;
	tris[ntri].t3 = t3;
	tris[ntri].texture = tex;
	ntri++;
}


/**********************************************************************/
void add_tri2( char *text )
{
	int	v1,v2,v3,t1,t2,t3,n1,n2,n3,tex;

	if( ntri == MAX_TRIS )
	{
		fprintf( stderr, "Error: Too many triangles.\n" );
		exit( 0 );
	}

	sscanf( text, "polygon ((%d, %d, %d),(%d, %d, %d), (%d, %d, %d), %d",
			&v1, &v2, &v3, &t1, &t2, &t3, &n1, &n2, &n3, &tex );
	tris[ntri].v1 = v1;
	tris[ntri].v2 = v2;
	tris[ntri].v3 = v3;
	tris[ntri].t1 = t1;
	tris[ntri].t2 = t2;
	tris[ntri].t3 = t3;
	tris[ntri].texture = tex;
	ntri++;
}


/**********************************************************************/
void add_shader( char *text )
{
	char *c;
	int i,j;

	/* extract the name */
	c = text+10;

	/* look up this shader */
	for( i = 0; (i < nshd) && (strcmp( c, shaders[i].name )); i++ );

	if( i == nshd )
	{
		fprintf( stderr, "Error: unmatched shader %s\n", c );
		exit( 0 );
	}

	/* found the shader.  now look up the color */
	if( shaders[i].stype == ST_TEXTURE )
	{
		for( j = 0; (j < ntex) && (strcmp( shaders[i].texture,
				textures[j].name )); j++ );

		if( j == ntex )
		{
			fprintf( stderr, "Error: shader %s has unmatched texture %s\n",
				shaders[i].name, shaders[i].texture );
			exit( 0 );
		}
		psets[nsets].shaders[psets[nsets].nshaders] = j;

		if( debug_mode )
		{
			printf( " ref shader: %s, uses texture %s\n", c, textures[j].name );
		}
	}
	else
	{
		psets[nsets].shaders[psets[nsets].nshaders] = -1;
	}

	psets[nsets].nshaders++;
}


/**********************************************************************/
int yywrap( void )
{
	return 1;
}

/**********************************************************************/
void begin_texture( char *text )
{
	if( strlen( text ) > 55 )
	{
		printf( "Error.  Texture %s name too long.\n", text+8 );
	}
	strcpy( cur_tex, text+8 );

	if( debug_mode )
	{
		printf( "Texture: \"%s\"\n", text+8 );
	}
}

/**********************************************************************/
void begin_shader( char *text )
{
	if( strlen( text ) > 54 )
	{
		printf( "Error.  Shader` %s name too long.\n", text+7 );
	}
	strcpy( cur_shd, text+7 );

	if( debug_mode )
	{
		printf( "Shader: \"%s\"\n", text+7 );
	}
}

/**********************************************************************/
void get_tex_name( char *text )
{
	char *c;

	/*	image = "/abc/def/ghi/filename.TIF",	*/
	/*	image = File#213,						*/

	/* trim off trailing commas or quotes */
	for( c = text + strlen( text ) - 1; (*c == ',') || (*c == '"' ); c-- )
		*c = '\0';

	/* now back up until we find either a frontslash or a whitespace char */
	for( ; (*c != ' ') && (*c != '\t') && (*c != '/'); c-- );

	strcpy( textures[ntex].name, cur_tex );
	strcpy( textures[ntex].file, c+1 );
	textures[ntex].refs = 0;
}

/**********************************************************************/
void get_shd_name( char *text )
{
	char *c;

	if( text )
	{
		/* find the beginning */
		for( c = text + strlen( text ) -1; *c != ' '; c-- );

		strcpy( shaders[nshd].texture, c+1 );
		shaders[nshd].stype = ST_TEXTURE;
	}
	else
	{
		shaders[nshd].stype = ST_OTHER;
	}

	strcpy( shaders[nshd].name, cur_shd );
	nshd++;
}

/**********************************************************************/
void get_tex_type( char *text )
{
	char *c;

	c = text+12;

	if( !strcmp( c, "Sky" ))
	{
		textures[ntex].ttype = TT_SKY;
		ntex++;
		BEGIN 0;
	}
	else if( !strcmp( c, "file" ))
	{
		textures[ntex].ttype = TT_FILE;
	}
	else if( !strcmp( c, "Projection" ))
	{
		textures[ntex].ttype = TT_PROJECTION;
	}
	else if( !strcmp( c, "sFractal" ))
	{
		textures[ntex].ttype = TT_SFRACTAL;
		ntex++;
		BEGIN 0;
	}
	else
	{
		fprintf( stderr, "Error!  Unknown texture type: %s\n", c );
		exit( 0 );
	}
}

/**********************************************************************/
void get_tex_parm( char *text, float *val )
{
	char *c;

	/* look for the first digit */
	c = text;
	while( !isdigit( *c ))
		c++;

	*val = atof( c );

	return;
}

/**********************************************************************/
void print_help( void )
{
	printf( "cvtmod [options] infile" );
}

/**********************************************************************/
// NOTE:
// For whatever reason, this doesn't seem to be working
// reliably.  The sort by texture indices is somehow fucked
// up, but I (JMS) dont have time to deal with it just now.
// Be aware that if you think you're getting bogus sorts,
// you're probably right.

int tricmp( const void *p1, const void *p2 )
{
	TRI		*t1, *t2;
	float	z1,z2;

	t1 = (TRI *)p1;
	t2 = (TRI *)p2;

	if( do_tsort )
	{
		if(( t1->texture - t2->texture ) || !zsort_mode )
			return(( t1->texture - t2->texture ) * (reverse_tsort ? -1 : 1));
	}

	z1 = sort_vtx[t1->v1].z + sort_vtx[t1->v2].z + sort_vtx[t1->v3].z;
	z2 = sort_vtx[t2->v1].z + sort_vtx[t2->v2].z + sort_vtx[t2->v3].z;

	return( z1 - z2 );
}

/**********************************************************************/
int tpcmp( const void *p1, const void *p2 )
{
	TEXTURE	**t1, **t2;

	t1 = (TEXTURE **)p1;
	t2 = (TEXTURE **)p2;

	return( strcmp( (*t1)->file, (*t2)->file ) );
}

/**********************************************************************/


#ifdef JONHEY
int get_aspect_ratio (char *name)
{
	char work_name[128];
	char file_spec[128];
	FILE *fp;
	char *tmp;
	int ar;
	wms_header_t wms_header;

	strcpy(work_name,name);
	tmp = strstr(work_name, ".TIF");
	if (!tmp)
	{
		tmp = strstr(work_name, ".tif");
		if (!tmp)
		{
			fprintf(stderr,"unrecognized texture extension: %s\n\r",name);
			return(3);
//			exit(0);
		}
	}
	strcpy(tmp,".wms");

	strcpy(file_spec,dir_name);

	tmp = strrchr(dir_name,'\\');
	if (tmp)
	{
		++tmp;
		if( isalnum(*tmp) )
		{
			strcat(dir_name,"\\");
		}
	}
	else
	{
	dir_name[0] = 0;
	}

	sprintf(file_spec,"%s%s",dir_name,work_name);


	if((fp=fopen(file_spec,"rb")) == (FILE *)0)
	{
		fprintf(stderr,"can't open file: %s\n\r",file_spec);
		exit(0);
	}

	fread(&wms_header,sizeof(wms_header_t),1,fp);
//	printf("wms.w %d %d %d %d\n",wms_header.w,wms_header.h,wms_header.small,wms_header.large);
	fread(&ar,sizeof(int),1,fp);
	fclose(fp);
//	printf("ar: %x\n",ar);
	
	return (ar);
}
#endif