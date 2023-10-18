#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "iffr.h"
#include "uvr.h"

static int find_uv( float, float );

char		writebuf[1024];

extern grface	fdb[FACE_MAX];		/* face list */
extern char		textures[MAX_TEXTURES][TEXTURE_LEN];
extern vertex_t	uvpairs[MAX_TRIANGLES*3];

extern int		nfiles,nuvpairs,ntriangles;

int read_uv( char *filename )
{
	FILE	*infile;
	int		i,j,x,n;
	char	*ctmp1, *ctmp2;
	float	tu,tv,foo;

	infile = fopen( filename, "rt" );

	if( infile == NULL )
	{
		fprintf( stderr, "Error opening file %s for reading.\n",
			filename );
		return -1;
	}

	// get the texture file count
	fscanf( infile, "%d %d", &x, &nfiles );
	assert( MAX_TEXTURES >= nfiles );
	
	if(x > 1)
	{
		for( i = 0; i < nfiles; i++ )
			fscanf( infile, "%s", writebuf );
	}

	// get the path-stripped texture filenames
	for( i = 0; i < nfiles; i++ )
	{
		fscanf( infile, "%s", writebuf );

		ctmp1 = writebuf;
		do
		{
			ctmp2 = ctmp1+1;
			ctmp1 = strchr( ctmp2, '\\' );
		} while (ctmp1);

		if(( ctmp1 = strchr( ctmp2, '.' )))
		{
			*(ctmp1+1) = 'w';
			*(ctmp1+2) = 'm';
			*(ctmp1+3) = 's';
			*(ctmp1+4) = '\0';
		}
		
		strncpy( textures[i], ctmp2, TEXTURE_LEN );
		textures[i][TEXTURE_LEN-1] = '\0';
	}

	// get the vertex count
	fscanf( infile, "%d", &ntriangles );

	// read triangles
	for( i = 0; i < ntriangles; i++ )
	{
		fscanf( infile, "%d", &x );
		if( x != i )
			fprintf( stderr, "Weird.  Count on vertex %d doesn't match.  N"
				"ot fatal.  Continuing.\n", i );

		fscanf( infile, "%d", &x );
		if( x != 3 )
		{
			fprintf( stderr, "FATAL ERROR: Poly %d isn't a triangle.\n", i );
			exit( 1 );
		}

		for( j = 0; j < 3; j++ )
		{
			fscanf( infile, "%f %f %f", &tu, &tv, &foo );
			uvpairs[nuvpairs].u = tu;
			uvpairs[nuvpairs].v = tv;

			n = find_uv( tu, tv );

			if( n == -1 )
			{
				fdb[i].tvtx[j] = nuvpairs;
				nuvpairs++;
			}
			else
				fdb[i].tvtx[j] = n;
		}
	}

	return 0;
}

static int find_uv( float u, float v )
{
	int		i;

	for( i = 0; i < nuvpairs; i++ )
	{
		if ((fabs(u-uvpairs[i].u) <= MATCH) &&
			(fabs(v-uvpairs[i].v) <= MATCH))
			return i;
	}
	
	return -1;
}
