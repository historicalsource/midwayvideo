#ifndef __JMALLOC_H__
#define __JMALLOC_H__

#ifdef VERSIONS
char	goose_jmalloc_h_version[] = {"$Revision: 2 $"};
#endif

#undef MALLOC_DEBUG
#ifdef MALLOC_DEBUG
 #define JMALLOC(x)	jmalloc( x, __FILE__, __LINE__ )
 #define JREALLOC(x,y)	jrealloc( x, y, __FILE__, __LINE__ )
 #define JFREE(x)	jfree( x, __FILE__, __LINE__ )
#else
 #define JMALLOC(x)	malloc(x)
 #define JREALLOC(x,y)	realloc(x,y)
 #define JFREE(x)	free(x)
#endif

void *jmalloc( size_t, char *, int );
void *jrealloc( void *, size_t, char *, int );
void jfree( void *, char *, int );
void dump_alloclist( void );
void check_fences( void );
void dump_block( unsigned int );

#endif
