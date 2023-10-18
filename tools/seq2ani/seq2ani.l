%{
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/*****************************************************************************/
#define MAX_SEQUENCES	512

struct seq_header
{
	char	name[16];
	int		nframes;
	int		nlimbs;
	int		flags;
};

struct asb_seq_header
{
	int		nframes;
	int		nlimbs;
	int		filepos;
	int		flags;
};

#define F_COMPRESSED	0x1

int file_size( struct seq_header * );
void process_infile( char * );
int nseq;
struct seq_header	seq_headers[MAX_SEQUENCES];
char infile_names[MAX_SEQUENCES][16];

%}

%x		SEQS

%%

^"SEQS"[ \t]*"="[ \t]*				BEGIN SEQS;
<SEQS>"\\"\n							;
<SEQS>\n							BEGIN 0;
<SEQS>[a-z0-9A-Z._]+					process_infile( yytext );
[ \n\t]+							;
.									;
<SEQS>[ \t]+						;

%%

/*****************************************************************************/
int main(int argc, char *args[])
{
	struct asb_seq_header out_header;
	FILE		*outfile, *cfile, *fp;
	char		writebuf[128];
	long		filepos;
	int			*dp;
	int			vstamp, i;

	nseq = 0;

	/* get vstamp (exec time) */
	vstamp = (int)time( NULL );

	if(argc != 3)
	{
		fprintf(stderr, "seq2ani <outfile base> <config file>\n");
		exit(1);
	}

	/* open the config file */
	cfile = fopen( args[2], "rt" );
	if( cfile == NULL )
	{
		fprintf(stderr, "Error opening file %s for reading.\n", args[2] );
		exit(1);
	}

	yyin = cfile;
	yylex();

	fclose( cfile );

	/* write the (asm) header file */
	sprintf(writebuf, "%s.equ", args[1] );
	outfile = fopen(writebuf, "wt");
	if(outfile == NULL)
	{
		fprintf(stderr, "Error opening file %s for writing.\n", writebuf);
		exit(1);
	}

	for( i = 0; i < strlen( args[1] ); i++ )
		args[1][i] = toupper( args[1][i] );

	fprintf( outfile, "\t\t.equ\t%s_VERSION,\t0x%X\n\n", args[1], vstamp );

	for(i=0; i< nseq; i++)
	{
		fprintf(outfile, "\t\t.equ\tSEQ_%s,\t\tASB_%s|%d\t\t# %d/%d \n",
				seq_headers[i].name, args[1], i, seq_headers[i].nframes,
				seq_headers[i].nlimbs );
	}

	fclose( outfile );

	/* write the (C) header file */
	sprintf(writebuf, "%s.h", args[1] );
	outfile = fopen(writebuf, "wt");
	if(outfile == NULL)
	{
		fprintf(stderr, "Error opening file %s for writing.\n", writebuf);
		exit(1);
	}

	fprintf( outfile, "#define %s_VERSION\t0x%X\n\n", args[1], vstamp );

	for(i=0; i< nseq; i++)
	{
		fprintf(outfile, "#define\tSEQ_%s\t\t(ASB_%s|%d)\n",
				seq_headers[i].name, args[1], i );
	}

	fclose( outfile );

	/* write the (C) header debug file */
	sprintf(writebuf, "%s.dbg", args[1] );
	outfile = fopen(writebuf, "wt");
	if(outfile == NULL)
	{
		fprintf(stderr, "Error opening file %s for writing.\n", writebuf);
		exit(1);
	}

	fprintf( outfile, "char *seq_names[] = {\n" );

	for(i=0; i< nseq; i++)
	{
		fprintf( outfile, "\t\"%s\",\n", seq_headers[i].name );
	}

	fprintf( outfile, "};\n" );

	fclose( outfile );

	/* write the binary file */
	sprintf(writebuf, "%s.ani", args[1] );
	outfile = fopen(writebuf, "wb");
	if(outfile == NULL)
	{
		fprintf(stderr, "Error opening file %s for writing.\n", writebuf);
		exit(1);
	}

	/* write the version stamp */
	fwrite( &vstamp, sizeof( int ), 1, outfile );

	/* write the headers */
	filepos = sizeof( struct asb_seq_header ) * nseq;
	for(i=0; i<nseq; i++)
	{
		/* write file pointer, frame count, limb count */
		out_header.nframes = seq_headers[i].nframes;
		out_header.nlimbs = seq_headers[i].nlimbs;
		out_header.filepos = filepos;
		out_header.flags = seq_headers[i].flags;

		fwrite( &out_header, sizeof( struct asb_seq_header ), 1, outfile );
		printf( "sequence %d: %s  nf: %ld nl: %ld fp: %ld,",
				i, seq_headers[i].name, out_header.nframes, out_header.nlimbs,
				out_header.filepos );
		printf( out_header.flags & F_COMPRESSED ? "  COMPRESSED\n" : "\n" );

		/* compute new file offset */
		filepos += file_size( seq_headers + i );
	}

	/* now loop through the files and copy their data into the output file */
	for(i = 0; i < nseq; i++)
	{
		fp = fopen( infile_names[i], "rb");
		if(fp == NULL)
		{
			fprintf(stderr, "Error opening file %s for reading.\n", args[i-2]);
			exit(1);
		}

		/* read the header (again) */
		fread(seq_headers+i, sizeof( struct seq_header ), 1, fp);

		dp = (int *)malloc( file_size( seq_headers + i ));

		if( dp == NULL )
		{
			fprintf(stderr, "Error allocating memory for file %s.\n",
					seq_headers[i].name);
			exit(1);
		}

		fread( dp, sizeof( char ), file_size( seq_headers + i ), fp );
		fwrite( dp, sizeof( char ), file_size( seq_headers + i ), outfile );

		free( dp );

		fclose(fp);
	}

	return 0;
}

/*****************************************************************************/
int file_size( struct seq_header *ph )
{
	int i;

	i = sizeof( float ) * 3 * ph->nframes;				    /* translation */
	i += sizeof( float ) * 4 * ph->nlimbs;				    /* frame 0     */
	if( ph->flags & F_COMPRESSED )
		i += sizeof( char ) * 4 * ph->nlimbs * (ph->nframes-1);  /* frames */
	else
		i += sizeof( float ) * 4 * ph->nlimbs * (ph->nframes-1); /* frames */

	return i;
}

/*****************************************************************************/
int yywrap( void )
{
	return 1;
}

/*****************************************************************************/
void process_infile( char *infile )
{
	FILE *fp;

	fp = fopen( infile, "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file %s for reading.\n", infile );
		exit(1);
	}
	fread( seq_headers+nseq, sizeof( struct seq_header ), 1, fp);
	fclose(fp);
	strcpy( infile_names[nseq], infile );
	nseq++;
}

/*****************************************************************************/
