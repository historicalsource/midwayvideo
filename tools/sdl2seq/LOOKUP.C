#include <stdio.h>
#include <math.h>

int main( void )
{
	float array[256];
	int i;

#if 0
	/* even distribution.  awful */
	for( i = 0; i < 256; i++ )
		array[i] = (float)i / 127.5f - 1.0f;
#endif

	for( i = 0; i < 127; i++ )
		array[i] = -1.0f / (float)(4*i+1);

	array[127] = 0.0f;

	for( i = 128; i < 256; i++ )
		array[i] = 1.0f / (float)(1025-4*(i+1));

	printf( "float lookup[] =\n{\n" );
	for( i = 0; i < 64; i++ )
		printf( "\t%9ff, %9ff, %9ff, %9ff,\n", array[i*4+0],
				array[i*4+1], array[i*4+2], array[i*4+3] );

	printf( "};" );

	return( 0 );
}
