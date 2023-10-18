#include <stdio.h>
#include <math.h>

struct entry {
	float	tgt;
	int		count;
};

int main( void )
{
	float	array[256];
	int		i,j,k;
	float	cur,inc,target;

	struct entry profile[] =
	{
	{-0.150000,	13},
	{-0.100000,	16},
	{-0.042500,	24},
	{-0.013900,	26},
	{-0.01000,	8},
	{-0.00600,	12},
	{-0.002000,	17},
	{ 0.002000,	20},
	{ 0.006000,	17},
	{ 0.010000,	12},
	{ 0.040000,	36},
	{ 0.050000,	10},
	{ 0.070000,	10},
	{ 0.100000,	10},
	{ 0.150000,	10},
	{ 0.6,		14},
	};

	for( i = 0; i < 256; i++ )
		array[i] = 0.0;

	i = 1;

	cur = -1.0f;
	array[0] = cur;

	for( j = 0; j < sizeof( profile ) / sizeof( struct entry ); j++ )
	{
		target = profile[j].tgt;

		inc = (target - cur) / (float)(profile[j].count);

		for( k = 0; k < profile[j].count; k++ )
		{
			cur += inc;
			array[i++] = cur;
		}
	}

	//
	// print results
	//
	printf( "float lookup[] =\n{\n" );
	for( i = 0; i < 64; i++ )
		printf( "\t%9ff, %9ff, %9ff, %9ff,\n", array[i*4+0],
				array[i*4+1], array[i*4+2], array[i*4+3] );

	printf( "};\n" );

	return( 0 );
}
