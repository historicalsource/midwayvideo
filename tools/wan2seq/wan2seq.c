///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "lookup.h"


///////////////////////////////////////////////////////////////////////////////
// definitions
#define	MAXFILES	16
#define MAXSEQ		128
#define MAXFRAMES	1024
#define MAXLIMBS	15
#define X	0
#define Y	1
#define Z	2
#define W	3
#define BYTESWAP(x)	( (((x)>>24)&0xff)|(((x)>>8)&0xff00)|(((x)&0xff00)<<8)|((x)<<24))
#define FALSE		0
#define TRUE		!FALSE
typedef enum { PS_NONE, PS_SCALE } pstate;
typedef float matrix[3][3];
typedef float quat[4];
typedef char cquat[4];

typedef struct _wiffhdr
{
	long	id;
	long	size;
} wiffhdr_t;

typedef struct _chunkhdr
{
	long	id;
	long	size;
	long	num;
	long	refnum;
} chunkhdr_t;

typedef struct _achdr
{
	char	name[20];
	long	flags;
	long	pcnt;
	long	spare[10];
} achdr_t;

// achdr flags
#define A_SELECTED		0x4000
#define A_COMPRESSED	0x0001

typedef struct seq_header
{
	char	name[16];
	int		nframes;
	int		nlimbs;
	int		flags;
} sqhdr_t;

char	seq_names[MAXSEQ][16];
int		seq_index = 0;

// seq_header flags
#define S_COMPRESSED	0x1

///////////////////////////////////////////////////////////////////////////////
// global variables
float	xlate_sf;
float	xlate[MAXFRAMES][3];
quat	frames[MAXFRAMES][MAXLIMBS];
cquat	cframe[MAXLIMBS];
quat	prev_frame[MAXLIMBS];
int		fflags[MAXFRAMES];
int		depend = FALSE;
int		nocompress = FALSE;
int		profile = FALSE;
int		debugfile = FALSE;
int		profile256[256], pcount;
double	eprofile[256];
char	*gseq;
int		gframe;
double	total_error;
int		error_count;

///////////////////////////////////////////////////////////////////////////////
// prototypes
int process_wanfile( char * );
void process_anichunk( FILE * );
void mat2quat( matrix, quat );
void adjust_quat( quat, quat );
void lcasestr( char * );
void compress_frame( cquat *, quat *, quat *, int );
int best_lookup( float );
void check_quat( quat, long, char *, long, matrix );

///////////////////////////////////////////////////////////////////////////////
int main( int argc, char *args[] )
{
	int		f, i, nfiles=0, retval=0;
	char	*infiles[MAXFILES];
	pstate	pmode=PS_NONE;
	int		acc;

	// initialize globals
	xlate_sf = 1.0f;
	total_error = 0.0;
	error_count = 0;
	for( i = 0; i < 256; i++ )
	{
		profile256[i] = 0;
		eprofile[i] = 0.0;
	}

	// parse command flags
	for( i = 1; i < argc; i++ )
	{
		switch( args[i][0] )
		{
			case '-':
				switch( args[i][1] )
				{
					case 's':	// set translation scale factor
						if( args[i][2] )
							xlate_sf = atof( &args[i][2] );
						else
							pmode = PS_SCALE;
						break;
					case 'd':	// write dependency file
						depend = TRUE;
						break;
					case 'n':	// compress none
						nocompress = TRUE;
						break;
					case 'p':	// print compression profile
						profile = TRUE;
						break;
					case 't':	// write .txt debug file
						debugfile = TRUE;
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
					case PS_SCALE:
						xlate_sf = atof( args[i] );
						pmode = PS_NONE;
						break;
					default:
						if( nfiles == MAXFILES )
						{
							fprintf( stderr, "Error: Too many input files.  Increase "
									"MAXFILES.\n" );
							exit( 1 );
						}
						infiles[nfiles++] = args[i];
				}
				break;
		}
	}

	if( nfiles == 0 )
	{
		fprintf( stderr, "Error: Must specify at least one input file.\n" );
		exit( 1 );
	}

	for( f = 0; f < nfiles; f++ )
		retval += process_wanfile( infiles[f] );

	if( profile )
	{
		if( error_count )
			printf( "Mean error: %6f\n", (float)(total_error / (double)error_count ));
		acc = 0;
		for( f = 0; f < 256; f++ )
		{
			printf( "%3d: %5d (%8.6f)\t%f", f, profile256[f], profile256[f] ? (float)(eprofile[f] / (double)(profile256[f])) : 0.0f, lookup[f] );

			acc += profile256[f];

			if( acc > (pcount*10)/256 )
			{
				acc = acc % ((pcount*10)/256);
				printf( "-MARK\n" );
			}
			else
				printf( "\n" );
		}
	}

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
int process_wanfile( char *filename )
{
	FILE		*infile, *outfile;
	wiffhdr_t	wiffhdr;
	chunkhdr_t	chunkhdr;
	long		len,cur;
	char		*cp;
	char		buf[128], buf2[16], buf3[16];
	int			i;

	printf( "Reading file %s\n", filename );

	if((infile = fopen( filename, "rb" )) == NULL )
	{
		printf( " Error opening file %s for reading.\n", filename );
		return 1;
	}

	// check the file header
	fread( &wiffhdr, sizeof( wiffhdr_t ), 1, infile );
	wiffhdr.id = BYTESWAP(wiffhdr.id);
	wiffhdr.size = BYTESWAP(wiffhdr.size);
	if( wiffhdr.id != 'WIFF' )
	{
		printf( " Error: File %s is not a WIFF file.\n", filename );
		return 1;
	}

	// make sure actual file length and header file length check out
	cur = ftell( infile );
	fseek( infile, 0L, SEEK_END );
	len = ftell( infile );

	if( len != wiffhdr.size + sizeof( wiffhdr_t ))
	{
		printf( "Error: file size %ld doesn't match header size %ld\n",
			len, wiffhdr.size );
		return 1;
	}
	fseek( infile, cur, SEEK_SET );

	do {
		fread( &chunkhdr, sizeof( chunkhdr_t ), 1, infile );

		chunkhdr.id = BYTESWAP(chunkhdr.id);
		chunkhdr.size = BYTESWAP(chunkhdr.size);
		chunkhdr.num = BYTESWAP(chunkhdr.num);
		chunkhdr.refnum = BYTESWAP(chunkhdr.refnum);

		cur = ftell( infile ) + chunkhdr.size - 8;

		switch( chunkhdr.id )
		{
			case 'ORIG':
				fread( buf, chunkhdr.size-8L, 1L, infile );
				buf[chunkhdr.size-8L] = '\0';
				printf( "  %s", buf );
				break;
			case '3AN ':
				process_anichunk( infile );
				break;
			default:
				cp = (char *)&(chunkhdr.id);
				printf( "  skipping unknown chunk: %c%c%c%c, size %ld, num %ld,"
					" ref %ld\n", cp[3], cp[2], cp[1], cp[0], chunkhdr.size,
					chunkhdr.num, chunkhdr.refnum );
				break;
		}

		if( cur & 1 )
			cur += 1;

		fseek( infile, cur, SEEK_SET );

	} while( len != cur );

	printf( "\n" );

	fclose( infile );

	for( i = 0; (i < 15) && (filename[i]) && (filename[i] != '.'); i++ )
		buf2[i] = tolower( filename[i] );
	buf2[i] = '\0';

	sprintf( buf3, "%s.d", buf2 );

	if( depend )
	{
		if(( outfile = fopen( buf3, "wt" )) == NULL )
		{
			printf( "Error opening file %s for writing.\n", buf3 );
			return 0;
		}
	
		for( i = 0; i < seq_index; i++ )
		{
			sprintf( buf, "%s", seq_names[i] );
			lcasestr( buf );
			fprintf( outfile, "%s", buf );

			if( (i+1) % 5 )
				fprintf( outfile, " " );
			else
				fprintf( outfile, "\\\n" );
		}

		fprintf( outfile, ":\t%s\n", filename );
		fprintf( outfile, "\t@wan2seq -s 0.12898 -d $<\n" );

		fclose( outfile );
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void process_anichunk( FILE *infile )
{
	achdr_t	anihdr;
	long	id, count, mcount=-1;
	long	done = FALSE, i;
	matrix	mat;
	quat	q1;
	sqhdr_t	seqhdr;
	FILE	*outfile = NULL, *outfile2 = NULL;
	char	filespec[20], filespec2[20];
	int		written;

	fread( &anihdr, sizeof( achdr_t ), 1L, infile );
	
	if( seq_index+1 == MAXSEQ )
	{
		fprintf( stderr, "Too many sequences.  Increase MAXSEQ.\n" );
		exit( 0 );
	}

	gseq = anihdr.name;
	gframe = 0;

	if( nocompress )
		anihdr.flags &= ~A_COMPRESSED;

	if( !(anihdr.flags & A_SELECTED ))
		return;

	if( anihdr.flags & A_COMPRESSED )
		printf( "  c: %s", anihdr.name );

	if( anihdr.pcnt > MAXLIMBS )
	{
		printf( "  Too many limbs.  Increase MAXLIMBS.\n" );
		return;
	}

	while( !done )
	{
		fread( &id, sizeof( long ), 1L, infile );

		switch( id )
		{
			case -1:
				fread( &count, sizeof( long ), 1L, infile );
				if( mcount == -1 )
					mcount = count;
				else
				{
					if( count != mcount )
					{
						printf( "Error: %ld flags in %ld frame sequence %s.\n",
								count, mcount, anihdr.name );
						return;
					}
				}
				fread( &fflags, sizeof( long ), count, infile );
				break;
			case -2:
				fread( &count, sizeof( long ), 1L, infile );
				if( mcount == -1 )
					mcount = count;
				else
				{
					if( count != mcount )
					{
						printf( "Error: %ld translation points in %ld frame seque"
						"nce %s.\n", count, mcount, anihdr.name );
						return;
					}
				}
				fread( &xlate, sizeof( float ), 3L * count, infile );

				for( i = 0; i < mcount; i++ )
				{
					xlate[i][0] *= xlate_sf;
					xlate[i][1] *= xlate_sf;
					xlate[i][2] *= xlate_sf;
				}

				break;
				
			case 0:
				for( i = 0; (i < 15) && (anihdr.name[i]) && (anihdr.name[i] != '.'); i++ )
					seqhdr.name[i] = toupper( anihdr.name[i] );
				do
				{
					seqhdr.name[i++] = '\0';
				} while( i < 16 );

				seqhdr.nlimbs = anihdr.pcnt;
				seqhdr.flags = 0;
				if( anihdr.flags & A_COMPRESSED )
					seqhdr.flags |= S_COMPRESSED;

				// subtract deselected frames from header frame count
				seqhdr.nframes = 0;
				for( i = 0; i < mcount; i++ )
				{
					if( fflags[i] & A_SELECTED )
						seqhdr.nframes += 1;
				}

				// open output file
				sprintf( filespec, "%s.seq", seqhdr.name );
				sprintf( filespec2, "%s.txt", seqhdr.name );

				// save the file name
				strcpy( seq_names[seq_index++], filespec );

				if((outfile = fopen( filespec, "wb" )) == NULL )
				{
					printf( "Error opening file %s for writing.\n", filespec );
					return;
				}

				if( debugfile )
				{
					if((outfile2 = fopen( filespec2, "wt" )) == NULL )
					{
						printf( "Error opening file %s for writing.\n", filespec2 );
						return;
					}
				}

				// write seq header
				fwrite( &seqhdr, sizeof( sqhdr_t ), 1L, outfile );

				// write translation data
				for( i = 0; i < mcount; i++ )
					if( fflags[i] & A_SELECTED )
						fwrite( xlate[i], sizeof( float ), 3L, outfile );

				// write frame data
				written = 0;
				for( i = 0; i < mcount; i++ )
				{
					int	j;

					gframe = i;
					if( debugfile )
						fprintf( outfile2, "frame %ld\n", i );
					if(!( fflags[i] & A_SELECTED ))
						continue;

					if( anihdr.flags & A_COMPRESSED )
					{
						if( written )
						{
							compress_frame( cframe, frames[i], prev_frame, seqhdr.nlimbs );
							fwrite( cframe, sizeof( cquat ), seqhdr.nlimbs, outfile );
							for( j = 0; j < seqhdr.nlimbs; j++ )
							{
								float	*pq, n;

								pq = prev_frame[j];
								n = sqrt( pq[0]*pq[0]+pq[1]*pq[1]+pq[2]*pq[2]+pq[3]*pq[3] );

								if( debugfile )
									fprintf( outfile2, "%9f %9f %9f %9f - %9f %9f %9f %9f - %3u %3u %3u %3u - %9f %s\n",
										frames[i][j][0], frames[i][j][1], frames[i][j][2], frames[i][j][3],
										prev_frame[j][0],
										prev_frame[j][1],
										prev_frame[j][2],
										prev_frame[j][3],
										(0xff & cframe[j][0]),
										(0xff & cframe[j][1]),
										(0xff & cframe[j][2]),
										(0xff & cframe[j][3]),
										n,
										(fabs(n-1.0f) > 0.1 ) ? "BAD" : "" );
							}
						}
						else
						{
							fwrite( frames[i], sizeof( quat ), seqhdr.nlimbs, outfile );
							memcpy( prev_frame, frames[i], sizeof( quat ) * seqhdr.nlimbs );
							if( debugfile )
								for( j = 0; j < seqhdr.nlimbs; j++ )
									fprintf( outfile2, "%9f %9f %9f %9f\n",
										frames[i][j][0], frames[i][j][1], frames[i][j][2], frames[i][j][3] );
						}
					}
					else
					{
						fwrite( frames[i], sizeof( quat ), seqhdr.nlimbs, outfile );
						if( debugfile )
							for( j = 0; j < seqhdr.nlimbs; j++ )
								fprintf( outfile2, "%9f %9f %9f %9f\n",
									frames[i][j][0], frames[i][j][1], frames[i][j][2], frames[i][j][3] );
					}
					written += 1;
				}

				fclose( outfile );
				if( debugfile )
					fclose( outfile2 );

				done = TRUE;
				break;
			default:
				fread( &count, sizeof( long ), 1L, infile );
				if( mcount == -1 )
					mcount = count;
				else
				{
					if( count != mcount )
					{
						printf( "Error: %ld frames in %ld frame sequence %s, li"
								"mb %ld.\n", count, mcount, anihdr.name, id );
						return;
					}
				}
				for( i = 0; i < count; i++ )
				{
					// skip limb offset
					fread( &mat, sizeof( float ), 3L, infile );

					// read matrix
					fread( &mat, sizeof( float ), 9L, infile );

					// convert to quat
					mat2quat( mat, q1 );
					
					check_quat( q1, i, anihdr.name, id, mat );
					
					if( i )
						adjust_quat( q1, frames[i-1][id-1] );

					// write
					frames[i][id-1][0] = q1[0];
					frames[i][id-1][1] = q1[1];
					frames[i][id-1][2] = q1[2];
					frames[i][id-1][3] = q1[3];
				}
				break;
		}

	}
}

///////////////////////////////////////////////////////////////////////////////
void check_quat( quat q1, long frame, char *seq, long limb, matrix mat )
{
	if(( q1[0] < -1.0f ) || ( q1[0] > 1.0f ) ||
		( q1[1] < -1.0f ) || ( q1[1] > 1.0f ) ||
		( q1[2] < -1.0f ) || ( q1[2] > 1.0f ) ||
		( q1[3] < -1.0f ) || ( q1[3] > 1.0f ))
	{
		fprintf( stderr, "bad quat %f %f %f %f in frame %ld seq %s limb %ld\n",
			q1[0], q1[1], q1[2], q1[3], frame, seq, limb );
		fprintf( stderr, "src matrix:\n" );
		fprintf( stderr, "%f %f %f\n",
			mat[0][0], mat[0][1], mat[0][2] );
		fprintf( stderr, "%f %f %f\n",
			mat[1][0], mat[1][1], mat[1][2] );
		fprintf( stderr, "%f %f %f\n",
			mat[2][0], mat[2][1], mat[2][2] );
	}
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
void lcasestr( char *str )
{
	int i;

	for( i = 0; i < strlen( str ) - 1; i++ )
		str[i] = tolower( str[i] );
}

///////////////////////////////////////////////////////////////////////////////
void compress_frame( cquat *dest, quat *src, quat *prev, int limbs )
{
	float	matches[4];
	int		fits[4];
	int		i,j;

	for( i = 0; i < limbs; i++ )
	{
		adjust_quat( src[i], prev[i] );

		fits[0] = best_lookup( src[i][0] - prev[i][0] );
		fits[1] = best_lookup( src[i][1] - prev[i][1] );
		fits[2] = best_lookup( src[i][2] - prev[i][2] );
		fits[3] = best_lookup( src[i][3] - prev[i][3] );

		matches[0] = lookup[fits[0]];
		matches[1] = lookup[fits[1]];
		matches[2] = lookup[fits[2]];
		matches[3] = lookup[fits[3]];

		prev[i][0] += matches[0];
		prev[i][1] += matches[1];
		prev[i][2] += matches[2];
		prev[i][3] += matches[3];

		dest[i][0] = (unsigned char)(fits[0]);
		dest[i][1] = (unsigned char)(fits[1]);
		dest[i][2] = (unsigned char)(fits[2]);
		dest[i][3] = (unsigned char)(fits[3]);

		for( j = 0; j < 4; j++ )
		{
			profile256[fits[j]] += 1;
			total_error += (double)fabs(src[i][j] - prev[i][j]);
			eprofile[fits[j]] += (double)fabs(src[i][j] - prev[i][j]);
			error_count += 1;
		}
		pcount += 4;

	}
}

///////////////////////////////////////////////////////////////////////////////
int best_lookup( float val )
{
	int		i;

	// take the highest value that doesn't exceed the target
	for( i = 1; i < 256; i++ )
	{
		if( lookup[i]  > val )
			return i-1;
	}

	return 255;
}

///////////////////////////////////////////////////////////////////////////////
