
/*  A Bison parser, made from parse.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_FOUR_MB	258
#define	T_EIGHT_MB	259
#define	T_OLD_REVISION_DIRECTORY	260
#define	T_NEW_REVISION_DIRECTORY	261
#define	T_EPROM_SIZE	262
#define	T_REVISION_NAME	263
#define	T_OLD_REVISION_LEVEL	264
#define	T_NEW_REVISION_LEVEL	265
#define	T_GAME_ID	266
#define	T_NEW_FILE	267
#define	T_DELETE_FILE	268
#define	T_RENAME_FILE	269
#define	T_EXEC_FILE	270
#define	T_PATCH_FILE	271
#define	T_COPY_FILE	272
#define	T_END_SCRIPT	273
#define	T_EOF	274
#define	T_UNKNOWN_IDENT	275
#define	T_STR	276
#define	T_IC	277

#line 1 "parse.y"

/*
 *		$Archive: $
 *		$Revision: $
 *		$Date: $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

/* Due to the crummy filesystem we got stuck with all writes and seeks must be a multiple of 4 */

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "lex.h"
#include "update.h"
#include "upio.h"
#include "buildrom.h"

/*
 *		DEFINES
 */

#define GZIP_SCRIPT_FILENAME	"UPD.DAT"
#define ROUND_DOWN_TO_4(x)		((x / 4) * 4)

/*
 *		STATIC PROTOTYPES
 */

static void yyerror(char *err_string);
static int gzip_write_fname(gzFile gfd, char *fname);
static int gzip_write_opcode(gzFile gfd, uchar opcode);
static void build_path_filename(char *path, char *file_name, char *path_filename);
static bool check_error(int err, char *doing, char *func);
static bool check_params(void);
static void set_header(rom_header *h, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev, ushort rom_count, ushort rom_number);
static void set_trailer(rom_trailer *t);
static void set_checksum(rom_header *h, rom_trailer *t, void *data_block, int block_size);
static int build_rom_set(char *gzip_script_filename, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev);
static int build_patch_script(gzFile gfd, char *old_file_name, char *new_file_name);
static int patch_output_copy(gzFile gfd, int file_offset, int bytes_to_copy);
static int patch_output_new(gzFile gfd, uchar *new, int index, int num_bytes);
static int patch_output_end(gzFile gfd);

/*
 *		STATIC VARIABLES
 */

static ushort game_id;
static bool game_id_set;
static int eprom_size;
static ushort old_revision_level;
static bool old_revision_level_set;
static ushort new_revision_level;
static bool new_revision_level_set;
static char old_revision_directory[256];
static char new_revision_directory[256];
static char revision_name[256];
static bool exec_file_issued;
static gzFile gzfd;

#line 81 "parse.y"
typedef union {
	int param[4];
	char *str;
	int ic;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		52
#define	YYFLAG		-32768
#define	YYNTBASE	25

#define YYTRANSLATE(x) ((unsigned)(x) <= 277 ? yytranslate[x] : 31)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    24,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    23,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     4,     6,     7,    10,    14,    17,    20,    23,    26,
    29,    32,    35,    38,    41,    45,    49,    53,    57,    59,
    61,    63,    65,    66,    68,    72,    78
};

static const short yyrhs[] = {    26,
    27,    19,     0,    19,     0,     0,    28,    23,     0,    27,
    28,    23,     0,     5,    21,     0,     6,    21,     0,     7,
    29,     0,     8,    21,     0,     9,    22,     0,    10,    22,
     0,    11,    22,     0,    12,    21,     0,    13,    21,     0,
    14,    21,    21,     0,    15,    21,    30,     0,    17,    21,
    21,     0,    16,    21,    21,     0,    18,     0,     1,     0,
     3,     0,     4,     0,     0,    22,     0,    22,    24,    22,
     0,    22,    24,    22,    24,    22,     0,    22,    24,    22,
    24,    22,    24,    22,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   109,   131,   138,   162,   164,   167,   174,   180,   185,   191,
   197,   203,   209,   280,   312,   349,   385,   424,   485,   494,
   500,   505,   512,   519,   526,   533,   540
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_FOUR_MB",
"T_EIGHT_MB","T_OLD_REVISION_DIRECTORY","T_NEW_REVISION_DIRECTORY","T_EPROM_SIZE",
"T_REVISION_NAME","T_OLD_REVISION_LEVEL","T_NEW_REVISION_LEVEL","T_GAME_ID",
"T_NEW_FILE","T_DELETE_FILE","T_RENAME_FILE","T_EXEC_FILE","T_PATCH_FILE","T_COPY_FILE",
"T_END_SCRIPT","T_EOF","T_UNKNOWN_IDENT","T_STR","T_IC","';'","','","script",
"empty1","script_file","script_statement","rom_size","params", NULL
};
#endif

static const short yyr1[] = {     0,
    25,    25,    26,    27,    27,    28,    28,    28,    28,    28,
    28,    28,    28,    28,    28,    28,    28,    28,    28,    28,
    29,    29,    30,    30,    30,    30,    30
};

static const short yyr2[] = {     0,
     3,     1,     0,     2,     3,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     3,     3,     3,     3,     1,     1,
     1,     1,     0,     1,     3,     5,     7
};

static const short yydefact[] = {     3,
     2,     0,    20,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    19,     0,     0,     6,
     7,    21,    22,     8,     9,    10,    11,    12,    13,    14,
     0,    23,     0,     0,     1,     0,     4,    15,    24,    16,
    18,    17,     5,     0,    25,     0,    26,     0,    27,     0,
     0,     0
};

static const short yydefgoto[] = {    50,
     2,    18,    19,    24,    40
};

static const short yypact[] = {   -16,
-32768,    18,-32768,     0,     1,    -2,    16,    17,    19,    20,
    22,    23,    24,    25,    26,    27,-32768,    -1,    -3,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    28,    29,    31,    32,-32768,    15,-32768,-32768,    30,-32768,
-32768,-32768,-32768,    33,    34,    35,    36,    37,-32768,    40,
    50,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    38,-32768,-32768
};


#define	YYLAST		60


static const short yytable[] = {     3,
    22,    23,     1,     4,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    35,     3,    37,
    20,    21,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    25,    43,    26,    51,
    27,    28,    29,    30,    31,    32,    33,    34,    38,    52,
    39,    41,    42,    44,    45,    36,    47,    46,    49,    48
};

static const short yycheck[] = {     1,
     3,     4,    19,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,     1,    23,
    21,    21,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    21,    23,    22,     0,
    22,    22,    21,    21,    21,    21,    21,    21,    21,     0,
    22,    21,    21,    24,    22,    18,    22,    24,    22,    24
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 110 "parse.y"
{
				int err;
				
				/* close the output file */
				err = gzip_close(gzfd);
				if (check_error(err, "closing gzip file", "stript reduction"))
					YYABORT;
				
				err = build_rom_set(GZIP_SCRIPT_FILENAME, eprom_size, revision_name, game_id, old_revision_level, new_revision_level);
				if (check_error(err, "building EPROM set", "stript reduction"))
					YYABORT;
				
#if 0
				/* delete the temp script file */
				err = file_delete(GZIP_SCRIPT_FILENAME);
				if (check_error(err, "deleting temp script file", "stript reduction"))
					YYABORT;
#endif
				
				YYACCEPT;
			;
    break;}
case 2:
#line 132 "parse.y"
{
				fprintf(stderr, "The file `%s\' contains no script commands", in_file_name);
				YYACCEPT;
			;
    break;}
case 3:
#line 139 "parse.y"
{
				int err;
				
				/* init the script parameters */
				game_id = 0;
				game_id_set = FALSE;
				eprom_size = 0;
				old_revision_level = 0;
				old_revision_level_set = FALSE;
				new_revision_level = 0;
				new_revision_level_set = FALSE;
				*old_revision_directory = '\0';
				*new_revision_directory = '\0';
				*revision_name = '\0';
				exec_file_issued = FALSE;
				
				/* create the output gzip file */
				err = gzip_create(GZIP_SCRIPT_FILENAME, &gzfd);
				if (check_error(err, "creating a gzip file", "empty1 reduction"))
					YYABORT;
			;
    break;}
case 6:
#line 169 "parse.y"
{
				strcpy(old_revision_directory, yyvsp[0].str);
				fprintf(stderr, "setting old_revision_directory = %s\n", old_revision_directory);
				free(yyvsp[0].str);
			;
    break;}
case 7:
#line 175 "parse.y"
{
				strcpy(new_revision_directory, yyvsp[0].str);
				fprintf(stderr, "setting new_revision_directory = %s\n", new_revision_directory);
				free(yyvsp[0].str);
			;
    break;}
case 8:
#line 181 "parse.y"
{
				eprom_size = yyvsp[0].ic;
				fprintf(stderr, "setting rom_size = %dK\n", eprom_size / 1024);
			;
    break;}
case 9:
#line 186 "parse.y"
{
				strcpy(revision_name, yyvsp[0].str);
				fprintf(stderr, "setting revision_name = %28s\n", revision_name);
				free(yyvsp[0].str);
			;
    break;}
case 10:
#line 192 "parse.y"
{
				old_revision_level = yyvsp[0].ic;
				old_revision_level_set = TRUE;
				fprintf(stderr, "setting old_revision_level = %hu\n", old_revision_level);
			;
    break;}
case 11:
#line 198 "parse.y"
{
				new_revision_level = yyvsp[0].ic;
				new_revision_level_set = TRUE;
				fprintf(stderr, "setting new_revision_level = %hu\n", new_revision_level);
			;
    break;}
case 12:
#line 204 "parse.y"
{
				game_id = yyvsp[0].ic;
				game_id_set = TRUE;
				fprintf(stderr, "setting game_id = %hu\n", game_id);
			;
    break;}
case 13:
#line 210 "parse.y"
{
				uchar buffer[1024];
				char path_filename[256];
				int f_size, pad_size;
				int num_bytes;
				int err, fd, pad;
				
				/* NEW_FILE opcode
				 * filename file_name						name of new file
				 * int file_length							number of bytes to copy
				 * uchar file_bytes[file_length]			file data */
				
				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, NEW_FILE);
				if (check_error(err, "writing an opcode", "new_file reduction"))
					YYABORT;
				
				/* write out the new filename */
				err = gzip_write_fname(gzfd, yyvsp[0].str);
				if (check_error(err, "writing a filename", "new_file reduction"))
					YYABORT;
				
				/* open the new file for copying into the script file */
				build_path_filename(new_revision_directory, yyvsp[0].str, path_filename);
				err = file_open(path_filename, &fd);
				if (check_error(err, "opening a file", "new_file reduction"))
					YYABORT;
				
				/* get the size of the file */
				err = file_size(fd, NULL, &f_size);
				if (check_error(err, "getting a file size", "new_file reduction"))
					YYABORT;
				pad_size = ROUND_UP_TO_4(f_size) - f_size;

				/* write out the size to the script file */
				err = gzip_write(gzfd, &f_size, sizeof(f_size));
				if (check_error(err, "writing a file size", "new_file reduction"))
					YYABORT;
				
				while (f_size > 0) {
					/* determine the number of bytes to read */
					num_bytes = f_size > sizeof(buffer) ? sizeof(buffer) : f_size;
					/* update the number of bytes left to process */
					f_size -= num_bytes;
					err = file_read(fd, buffer, num_bytes);
					if (check_error(err, "reading a file buffer", "new_file reduction"))
						YYABORT;
					
					err = gzip_write(gzfd, buffer, num_bytes);
					if (check_error(err, "writing a file buffer", "new_file reduction"))
						YYABORT;					
				}
				
				/* pad the new file data out to four bytes */
				pad = 0;
				err = gzip_write(gzfd, &pad, pad_size);
				if (check_error(err, "writing a file buffer", "new_file reduction"))
					YYABORT;					

				err = file_close(fd);
				if (check_error(err, "closing a file buffer", "new_file reduction"))
					YYABORT;
				
				/* free the string storage */
				free(yyvsp[0].str);
			;
    break;}
case 14:
#line 281 "parse.y"
{
				char path_filename[256];
				int err;
				
				/* DELETE_FILE opcode
				 * filename file_name						name of file to delete from disk */
				
				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, DELETE_FILE);
				if (check_error(err, "writing an opcode", "delete_file reduction"))
					YYABORT;
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, yyvsp[0].str, path_filename);
				
				/* check to see if the file actually exists */
				if (file_exist(path_filename)) {
					err = gzip_write_fname(gzfd, yyvsp[0].str);
					if (check_error(err, "writing a filename", "delete_file reduction"))
						YYABORT;
				} else {
					fprintf(stderr, "Error %s not a file in the %s directory\n", yyvsp[0].str, old_revision_directory);
				}

				/* free the string storage */
				free(yyvsp[0].str);
			;
    break;}
case 15:
#line 313 "parse.y"
{
				char path_filename[256];
				int err;
				
				/* RENAME_FILE opcode
				 * filename old_name						name of file to rename
				 * filename new_name						new file name */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, RENAME_FILE);
				if (check_error(err, "writing an opcode", "rename_file reduction"))
					YYABORT;
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, yyvsp[-1].str, path_filename);
				
				/* check to see if the file actually exists */
				if (file_exist(path_filename)) {
					err = gzip_write_fname(gzfd, yyvsp[-1].str);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
					err = gzip_write_fname(gzfd, yyvsp[0].str);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
				} else {
					fprintf(stderr, "Error %s not a file in the %s directory\n", yyvsp[-1].str, old_revision_directory);
				}
				
				/* free the string storage */
				free(yyvsp[-1].str);
				free(yyvsp[0].str);				
			;
    break;}
case 16:
#line 350 "parse.y"
{
				int i;
				int err;
				
				/* EXEC_FILE opcode
				 * filename file_name						name of file to exec
				 * int params[4] 							params to pass to program */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, EXEC_FILE);
				if (check_error(err, "writing an opcode", "exec_file reduction"))
					YYABORT;
				
				/* write out the filename to exec */
				err = gzip_write_fname(gzfd, yyvsp[-1].str);
				if (check_error(err, "writing a filename", "exec_file reduction"))
					YYABORT;

				/* write out the four int params */
				for (i = 0; i < 4; i++) {
					err = gzip_write(gzfd, &yyvsp[0].param[i], sizeof(int));
					if (check_error(err, "writing an exec parameter", "exec_file reduction"))
						YYABORT;
				}
				
				/* free the string storage */
				free(yyvsp[-1].str);
				
				/* warn if any commands follow this */
				exec_file_issued = TRUE;
			;
    break;}
case 17:
#line 386 "parse.y"
{
				char old_path_filename[256];
				char new_path_filename[256];
				int err;
				
				/* COPY_FILE opcode
				 * filename old_name						name of file to copy
				 * filename new_name						new file name */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, COPY_FILE);
				if (check_error(err, "writing an opcode", "copy_file reduction"))
					YYABORT;
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, yyvsp[-1].str, old_path_filename);
				build_path_filename(new_revision_directory, yyvsp[-1].str, new_path_filename);
				
				/* check to see if the file actually exists */
				if (file_exist(old_path_filename) || file_exist(new_path_filename)) {
					err = gzip_write_fname(gzfd, yyvsp[-1].str);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
					err = gzip_write_fname(gzfd, yyvsp[0].str);
					if (check_error(err, "writing a filename", "rename_file reduction"))
						YYABORT;
				} else {
					fprintf(stderr, "Error %s not a file in the %s or %s directory\n", yyvsp[-1].str, old_revision_directory, new_revision_directory);
				}
				
				/* free the string storage */
				free(yyvsp[-1].str);
				free(yyvsp[0].str);				
			;
    break;}
case 18:
#line 425 "parse.y"
{
				char path_old_filename[256];
				char path_new_filename[256];
				int err;
				
				/* PATCH_FILE opcode
				 * filename file_name						file on disk to patch
				 * filename rename_file						name to rename the original to
				 *
				 * op codes
				 *		COPY_CHUNK							from the original file
				 *			int file_offset					offset in original file
				 *			int num_byte_to_copy			number of bytes to copy to new file
				 *		NEW_CHUNK							from the patch stream
				 *			int num_new_byte				number of bytes that follow
				 *			uchar chunk_bytes[num_new_byte]	new data to copy in file
				 *		END_PATCH							end of patch op code setinel */

				/* make sure the header parameters have been set */
				if (check_params())
					YYABORT;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, PATCH_FILE);
				if (check_error(err, "writing an opcode", "patch_file reduction"))
					YYABORT;

				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(old_revision_directory, yyvsp[-1].str, path_old_filename);
				
				/* build the pathfilename to the file in the old_revision_directory */
				build_path_filename(new_revision_directory, yyvsp[-1].str, path_new_filename);
				
				/* check to see if the files actually exists */
				if (!file_exist(path_old_filename)) {
					fprintf(stderr, "Error %s not a file in the %s directory\n", yyvsp[-1].str, old_revision_directory);
					YYABORT;
				}
				if (!file_exist(path_new_filename)) {
					fprintf(stderr, "Error %s not a file in the %s directory\n", yyvsp[-1].str, new_revision_directory);
					YYABORT;
				}
				
				/* write out the orginal file name */
				err = gzip_write_fname(gzfd, yyvsp[-1].str);
				if (check_error(err, "writing a filename", "patch_file reduction"))
					YYABORT;
				/* write out the rename filename */
				err = gzip_write_fname(gzfd, yyvsp[0].str);
				if (check_error(err, "writing a filename", "patch_file reduction"))
					YYABORT;

				err = build_patch_script(gzfd, path_old_filename, path_new_filename);
				if (check_error(err, "writing a patch script", "patch_file reduction"))
					YYABORT;
				
				/* free the string storage */
				free(yyvsp[-1].str);
				free(yyvsp[0].str);
			;
    break;}
case 19:
#line 486 "parse.y"
{
				int err;
				
				/* write out the op code */
				err = gzip_write_opcode(gzfd, END_SCRIPT);
				if (check_error(err, "writing an opcode", "end_script reduction"))
					YYABORT;
			;
    break;}
case 20:
#line 495 "parse.y"
{
				YYABORT;
			;
    break;}
case 21:
#line 501 "parse.y"
{
				/* 4Mb == 512K */
				yyval.ic = (1024 * 1024 * 4) / 8;
			;
    break;}
case 22:
#line 506 "parse.y"
{
				/* 8Mb == 1024K */
				yyval.ic = (1024 * 1024 * 8) / 8;
			;
    break;}
case 23:
#line 513 "parse.y"
{
				yyval.param[0] = 0;
				yyval.param[1] = 0;
				yyval.param[2] = 0;
				yyval.param[3] = 0;
			;
    break;}
case 24:
#line 520 "parse.y"
{
				yyval.param[0] = yyvsp[0].ic;
				yyval.param[1] = 0;
				yyval.param[2] = 0;
				yyval.param[3] = 0;
			;
    break;}
case 25:
#line 527 "parse.y"
{
				yyval.param[0] = yyvsp[-2].ic;
				yyval.param[1] = yyvsp[0].ic;
				yyval.param[2] = 0;
				yyval.param[3] = 0;
			;
    break;}
case 26:
#line 534 "parse.y"
{
				yyval.param[0] = yyvsp[-4].ic;
				yyval.param[1] = yyvsp[-2].ic;
				yyval.param[2] = yyvsp[0].ic;
				yyval.param[3] = 0;
			;
    break;}
case 27:
#line 541 "parse.y"
{
				yyval.param[0] = yyvsp[-6].ic;
				yyval.param[1] = yyvsp[-4].ic;
				yyval.param[2] = yyvsp[-2].ic;
				yyval.param[3] = yyvsp[0].ic;
			;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 548 "parse.y"

/*
 *		STATIC FUNCTIONS
 */

static void yyerror(char *err_string)
{
	fprintf (stderr, "%s at line %d\n", err_string, line_number);
}  /* yyerror */

/* write out a filename to the gzip script file, padded to 4 bytes */
static int gzip_write_fname(gzFile gfd, char *fname)
{
	int err;
	int len, pad;

	len = strlen(fname);
	err = gzip_write(gfd, &len, sizeof(len));
	if (err == UPDATE_NO_ERROR) {
		err = gzip_write(gfd, fname, len);
		if (err == UPDATE_NO_ERROR) {
			pad = 0;
			/* pad to a multiple of four! */
			err = gzip_write(gfd, &pad, ROUND_UP_TO_4(len) - len);
		}
	}
	return err;
}  /* gzip_write_fname */

/* write out an opcode to the gzip script file, padded to 4 bytes */
static int gzip_write_opcode(gzFile gfd, uchar opcode)
{
	int pad_opcode;
	
	/* pad the opcode to four bytes */
	pad_opcode = opcode;
	return gzip_write(gfd, &pad_opcode, sizeof(pad_opcode));
}  /* gzip_write_opcode */

static void build_path_filename(char *path, char *file_name, char *path_filename)
{
	strcpy(path_filename, path);
	strcat(path_filename, "\\");
	strcat(path_filename, file_name);
}  /* build_path_filename */
				
static bool check_error(int err, char *doing, char *func)
{
	char *str;
	
	if (err != UPDATE_NO_ERROR) {
		switch (err) {
		case UPDATE_NO_ERROR:
			str = "UPDATE_NO_ERROR";
			break;
		case UPDATE_FILE_OPEN_ERROR:
			str = "UPDATE_FILE_OPEN_ERROR";
			break;
		case UPDATE_FILE_CREATE_ERROR:
			str = "UPDATE_FILE_CREATE_ERROR";
			break;
		case UPDATE_FILE_CLOSE_ERROR:
			str = "UPDATE_FILE_CLOSE_ERROR";
			break;
		case UPDATE_FILE_DELETE_ERROR:
			str = "UPDATE_FILE_DELETE_ERROR";
			break;
		case UPDATE_FILE_RENAME_ERROR:
			str = "UPDATE_FILE_RENAME_ERROR";
			break;
		case UPDATE_FILE_READ_EOF_ERROR:
			str = "UPDATE_FILE_READ_EOF_ERROR";
			break;
		case UPDATE_FILE_READ_ERROR:
			str = "UPDATE_FILE_READ_ERROR";
			break;
		case UPDATE_FILE_WRITE_EOF_ERROR:
			str = "UPDATE_FILE_WRITE_EOF_ERROR";
			break;
		case UPDATE_FILE_WRITE_ERROR:
			str = "UPDATE_FILE_WRITE_ERROR";
			break;
		case UPDATE_FILE_SEEK_ERROR:
			str = "UPDATE_FILE_SEEK_ERROR";
			break;
		case UPDATE_EPROM_NOT_PRESENT_ERROR:
			str = "UPDATE_EPROM_NOT_PRESENT_ERROR";
			break;
		case UPDATE_EPROM_READ_ERROR:
			str = "UPDATE_EPROM_READ_ERROR";
			break;
		case UPDATE_EPROM_CHECKSUM_ERROR:
			str = "UPDATE_EPROM_CHECKSUM_ERROR";
			break;
		case UPDATE_WRONG_GAME_ERROR:
			str = "UPDATE_WRONG_GAME_ERROR";
			break;
		case UPDATE_WRONG_REVISON_LEVEL_ERROR:
			str = "UPDATE_WRONG_REVISON_LEVEL_ERROR";
			break;
		case UPDATE_UP_TO_DATE:
			str = "UPDATE_UP_TO_DATE";
			break;
		case UPDATE_ROM_ALREADY_PROCESSED:
			str = "UPDATE_ROM_ALREADY_PROCESSED";
			break;
		case UPDATE_SCRIPTFILE_CREATE_ERROR:
			str = "UPDATE_SCRIPTFILE_CREATE_ERROR";
			break;
		case UPDATE_MORE_ROMS_NEEDED:
			str = "UPDATE_MORE_ROMS_NEEDED";
			break;
		case UPDATE_HAVE_ALL_PIECES:
			str = "UPDATE_HAVE_ALL_PIECES";
			break;
		case UPDATE_GZIP_OPEN_ERROR:
			str = "UPDATE_GZIP_OPEN_ERROR";
			break;
		case UPDATE_GZIP_CREATE_ERROR:
			str = "UPDATE_GZIP_CREATE_ERROR";
			break;
		case UPDATE_GZIP_CLOSE_ERROR:
			str = "UPDATE_GZIP_CLOSE_ERROR";
			break;
		case UPDATE_GZIP_READ_ERROR:
			str = "UPDATE_GZIP_READ_ERROR";
			break;
		case UPDATE_GZIP_WRITE_ERROR:
			str = "UPDATE_GZIP_WRITE_ERROR";
			break;
		case UPDATE_GZIP_EOF_ERROR:
			str = "UPDATE_GZIP_EOF_ERROR";
			break;
		case UPDATE_GZIP_BUFFER_SMALL_ERROR:
			str = "UPDATE_GZIP_BUFFER_SMALL_ERROR";
			break;
		case UPDATE_GZIP_UNKNOWN_OPCODE:
			str = "UPDATE_GZIP_UNKNOWN_OPCODE";
			break;
		case UPDATE_PATCH_UNKNOWN_OPCODE:
			str = "UPDATE_PATCH_UNKNOWN_OPCODE";
			break;
		case UPDATE_MALLOC_ERROR:
			str = "UPDATE_MALLOC_ERROR";
			break;
		case UPDATE_DISKIO_PARAM_ERROR:
			str = "UPDATE_DISKIO_PARAM_ERROR";
			break;
		default:
			str = "UNKNOWN ERROR";
			break;
		}
		fprintf(stderr, "%s while %s in %s\n", str, doing, func);
	}
	return err != UPDATE_NO_ERROR;
}  /* check_error */

static bool check_params(void)
{
	int cnt;
	
	cnt = 0;
	if (!game_id_set)
		cnt = fprintf(stderr, "Use \'GAME_ID num;\' to set the games id\n");
	if (eprom_size == 0)
		cnt = fprintf(stderr, "Use \'EPROM_SIZE (FOUR_MB | EIGHT_MB);\' to set the EPROM size\n");
	if (!old_revision_level_set)
		cnt = fprintf(stderr, "Use \'OLD_REVISION_LEVEL num;\' to set the previous revision level\n");
	if (!new_revision_level_set)
		cnt = fprintf(stderr, "Use \'NEW_REVISION_LEVEL num;\' to set the new revision level\n");
	if (*old_revision_directory == '\0')
		cnt = fprintf(stderr, "Use \'OLD_REVISION_DIRECTORY str;\' to set the old revision directory\n");
	if (*new_revision_directory == '\0')
		cnt = fprintf(stderr, "Use \'NEW_REVISION_DIRECTORY str;\' to set the new revision directory\n");
	if (*revision_name == '\0')
		cnt = fprintf(stderr, "Use \'REVISION_NAME str;\' to set the revision name\n");
	if (old_revision_level >= new_revision_level)
		cnt = fprintf(stderr, "old revision level is >= new revision level!\n");
	if (exec_file_issued)
		cnt = fprintf(stderr, "commands following exec_file will never get executed!\n");
	return cnt > 0;
}  /* check_params */

static void set_header(rom_header *h, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev, ushort rom_count, ushort rom_number)
{
	int i;
	
	/* set the header signature */
	h->signature = HEADER_SIG;
	h->rom_size = rom_size;
	/* the checksum and inv_checksum will be calculated later */
	h->checksum_16 = 0;
	h->inv_checksum_16 = 0;
	
	/* copy over the revision name, may not be nul terminated */
	for (i = 0; i < sizeof(revision_desc) && rev_name[i] != '\0'; i++)
		h->rev_name[i] = rev_name[i];
	while (i < sizeof(revision_desc))
		h->rev_name[i++] = '\0';
	
	/* set the required level for update */
	h->required_level.game_id = game_id;
	h->required_level.rev_level = from_rev;
	
	/* set the new revision level */
	h->new_level.game_id = game_id;
	h->new_level.rev_level = to_rev;
	
	/* set the number of EPROMs in the set */
	h->rom_count = rom_count;
	
	/* set which EPROM number this is */
	h->rom_number = rom_number;
	
	/* unused/future expansion fields */
	h->flags = 0;
	h->unused1 = 0xDEADBEEF;
	h->unused2 = 0xDEADBEEF;
}  /* set_header */

static void set_trailer(rom_trailer *t)
{
	/* set up the trialer signature */
	t->i = TRAILER_SIG;
	
	/* calculate the adjustment later */
	t->s.checksum_adjust = 0;
}  /* set_trailer */

static void set_checksum(rom_header *h, rom_trailer *t, void *data_block, int block_size)
{
	ushort sum, temp_checksum, final_checksum;
	uchar lo_byte, rev_level, rom_number;
	
	/* get the low order nibble of the rom number */
	rom_number = h->rom_number & 0x000F;
	
	/* get the low order nibble of the revision level */
	rev_level = h->new_level.rev_level & 0x000F;
	
	/* encode the revision level and rom number in the low order byte of the checksum */
	lo_byte = (rev_level << 4) | rom_number;
		
	/* init the low order byte of the checksum and inv_checksum to final value, the revision level */
	h->checksum_16 = lo_byte;
	h->inv_checksum_16 = (~lo_byte) & 0x00FF;
	
	/* zero out the checksum tweak byte */
	t->s.checksum_adjust = 0;
	
	/* since checksum and inv_checksum are bitwise inverts of each other, and the checksum is an */
	/* additive calculation, the sum of the checksums low and high order bytes are 0xFF + 0xFF */
	/* the low order bytes are already set to final value, init a high order byte to 0xFF for */
	/* calculating the real checksum */
	h->checksum_16 |= 0xFF00;

	/* calculate the pretweaked checksum */
	sum = calculate_checksum(h, sizeof(rom_header));
	sum += calculate_checksum(data_block, block_size);
	sum += calculate_checksum(t, sizeof(rom_trailer));
	
	/* force the revision level and rom number on the lower byte of the checksum */
	temp_checksum = (sum & 0xFF00) | lo_byte;
	
	if (temp_checksum > sum) {
		/* the new checksum with the revision level forced on is bigger */
		/* add the difference to the tweak byte */
		/* to properly adjust the contents to make the checksum */

		/* determine the final checksum */
		final_checksum = temp_checksum;
		
		/* set the checksum adjustment byte */
		t->s.checksum_adjust = temp_checksum - sum;
	} else /* temp_checksum <= sum */{
		/* the new checksum is smaller */
		/* we need to keep the lower order byte as is, so bump up the high order byte by one */
		
		/* determine the final checksum */
		final_checksum = temp_checksum + (temp_checksum == sum ? 0x0000 : 0x0100);
		
		/* set the checksum adjustment byte */
		t->s.checksum_adjust = final_checksum - sum;
	}
	
	/* set the header fields */
	h->checksum_16 = final_checksum;
	h->inv_checksum_16 = ~final_checksum;
}  /* set_checksum */

/* given the filename of a gzipped script file on disk, split it into the EPROM images */
/* rom_size is 512 * 1024 for 27C040 or 1024 * 1024 for 27C080 */
/* rev_name is the text string descibing the update */
/* game_id is the id of the game the patch is for */
/* from_rev is the required revision that be present to do the patch */
/* to_rev is the new revision level after the patch */
static int build_rom_set(char *gzip_script_filename, int rom_size, char *rev_name, ushort game_id, ushort from_rev, ushort to_rev)
{
	rom_header h;
	rom_trailer t;
	int script_file_size;
	uchar *block, *rom_name;
	int gfd, rfd;
	int num_rom, read_chunk;
	int block_size;
	int err;
	int i, j;
	
	block = malloc(rom_size);
	err = block == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	if (check_error(err, "mallocing the rom block", "build_rom_set"))
		return err;
	
	/* open the script file */
	err = file_open(gzip_script_filename, &gfd);
	if (check_error(err, "opening compressed script file", "build_rom_set"))
		return err;
	
	/* get the size of the gzipped script file in bytes */
	err = file_size(gfd, NULL, &script_file_size);
	if (check_error(err, "getting script file size", "build_rom_set"))
		return err;
	
	/* determine the number of EPROMs needed for the set */
	num_rom = script_file_size / rom_size;
	if (script_file_size % rom_size != 0)
		num_rom++;
	
	/* EPROMs are numbered one based */
	for (i = 1; i <= num_rom; i++) {
		/* clear out the EPROM bank */
		for (j = 0; j < rom_size; j++)
			block[j] = 0;
		
		block_size = rom_size - (sizeof(rom_header) + sizeof(rom_trailer));
		read_chunk = script_file_size > block_size ? block_size : script_file_size;
		script_file_size -= read_chunk;
		err = file_read(gfd, block, read_chunk);
		if (check_error(err, "reading from script file", "build_rom_set"))
			return err;
		
		/* fill in the header, trailer, and calculate the checksum */
		set_header(&h, rom_size, rev_name, game_id, from_rev, to_rev, num_rom, i);
		set_trailer(&t);
		set_checksum(&h, &t, block, read_chunk);
		
		/* create the EPROM image file */
		rom_name = build_piece_filename(to_rev, i);
		err = file_create(rom_name, &rfd);
		if (check_error(err, "creating ROM piece image file", "build_rom_set"))
			return err;
		
		err = file_write(rfd, &h, sizeof(rom_header));
		if (check_error(err, "writing ROM piece header", "build_rom_set"))
			return err;
		
		/* use block_size instead of read_chunk to pad out the last EPROM */
		err = file_write(rfd, block, block_size);
		if (check_error(err, "writing ROM piece data", "build_rom_set"))
			return err;
		
		err = file_write(rfd, &t, sizeof(rom_trailer));
		if (check_error(err, "writing ROM piece trailer", "build_rom_set"))
			return err;
		
		err = file_close(rfd);
		if (check_error(err, "closing ROM piece", "build_rom_set"))
			return err;
	}
	/* close the script file */
	err = file_close(gfd);
	if (check_error(err, "closing script file", "build_rom_set"))
		return err;
	
	/* free the data block */
	free(block);
	return err;
}  /* build_rom_set */

/* COPY_CHUNK						from the original file
 *	int file_offset					offset in original file
 *		file_offset must be a mult of four!
 *	int byte_to_copy				number of bytes to copy to new file
 *		byte_to_copy must be a mult of four!
 * NEW_CHUNK						from the patch stream
 *	int new_bytes					number of bytes that follow
 *	uchar chunk_bytes[new_bytes]	new data to copy in file
 *		new_bytes must be a mult of 4!
 * END_PATCH							end of patch op code setinel */
static int build_patch_script(gzFile gfd, char *old_file_name, char *new_file_name)
{
	uchar *old, *new;
	int fd;
	int old_size, new_size;
	int min_size;
	int top;
	int err;
	int i;
	
	/* open the old file */
	err = file_open(old_file_name, &fd);
	if (check_error(err, "opening old file", "build_patch_script"))
		return err;
	/* get the old files size */
	err = file_size(fd, NULL, &old_size);
	if (check_error(err, "getting file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}
	/* malloc space to read old file */
	old = malloc(ROUND_UP_TO_4(old_size));
	err = old == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	if (check_error(err, "mallocing old file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}
	/* read the old file in */
	err = file_read(fd, old, old_size);
	if (check_error(err, "reading old file buffer", "build_patch_script")) {
		file_close(fd);
		free(old);
		return err;
	}
	/* blank out the trailing pad */
	for (i = old_size; i < ROUND_UP_TO_4(old_size); i++)
		old[i] = 0;
	/* close the old file */
	err = file_close(fd);
	if (check_error(err, "closing old file", "build_patch_script")) {
		free(old);
		return err;
	}

	/* open the new file */
	err = file_open(new_file_name, &fd);
	if (check_error(err, "opening new file", "build_patch_script"))
		return err;
	/* get the new files size */
	err = file_size(fd, NULL, &new_size);
	if (check_error(err, "getting file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}
	/* malloc space to read new file */
	new = malloc(ROUND_UP_TO_4(new_size));
	err = new == NULL ? UPDATE_MALLOC_ERROR : UPDATE_NO_ERROR;
	if (check_error(err, "mallocing new file size", "build_patch_script")) {
		file_close(fd);
		return err;
	}	
	/* read the new file */
	err = file_read(fd, new, new_size);
	if (check_error(err, "reading old file buffer", "build_patch_script")) {
		file_close(fd);
		free(new);
		return err;
	}	
	/* blank out the trailing pad */
	for (i = new_size; i < ROUND_UP_TO_4(new_size); i++)
		new[i] = 0;
	/* close the new file */
	err = file_close(fd);
	if (check_error(err, "closing old file", "build_patch_script")) {
		free(new);
		return err;
	}
	
	/* find the smaller size */
	min_size = old_size < new_size ? old_size : new_size;
	
	/* compare two files to find the top part that matches */
	for (top = 0; top < min_size; top++)
		if (old[top] != new[top])
			break;
	/* scale top back to a mult of four */
	top = ROUND_DOWN_TO_4(top);
	/* check to see if the number of bytes is worth using */
	if (top > 16) {
		/* tell to copy the matching top block */
		err = patch_output_copy(gfd, 0, top);
	} else {
		/* output the entire new file */
		top = 0;
	}
	/* output the rest of the new file */
	if (err == UPDATE_NO_ERROR)
		err = patch_output_new(gfd, new, top, ROUND_UP_TO_4(new_size - top));
	if (err == UPDATE_NO_ERROR)
		err = patch_output_end(gfd);
	/* free the file data */
	free(old);
	free(new);
	return err;
}  /* build_patch_script */

static int patch_output_copy(gzFile gfd, int file_offset, int bytes_to_copy)
{
	int err;
	
	if (file_offset % 4 != 0)
		printf("patch_output_copy:file_offset not a mult of four!\n");
	if (bytes_to_copy % 4 != 0)
		printf("patch_output_copy:bytes_to_copy not a mult of four!\n");
	err = gzip_write_opcode(gfd, COPY_CHUNK);
	if (err == UPDATE_NO_ERROR) {
		err = gzip_write(gfd, &file_offset, sizeof(file_offset));
		if (err == UPDATE_NO_ERROR)
			err = gzip_write(gfd, &bytes_to_copy, sizeof(bytes_to_copy));
	}
	return err;
}  /* patch_output_copy */

static int patch_output_new(gzFile gfd, uchar *new, int index, int num_bytes)
{
	int err;
	
	if (num_bytes % 4 != 0)
		printf("patch_output_new:num_bytes not a mult of four!\n");
	err = gzip_write_opcode(gfd, NEW_CHUNK);
	if (err == UPDATE_NO_ERROR) {
		err = gzip_write(gfd, &num_bytes, sizeof(num_bytes));
		if (err == UPDATE_NO_ERROR)
			err = gzip_write(gfd, &new[index], num_bytes);
	}
	return err;
}  /* patch_output_new */

static int patch_output_end(gzFile gfd)
{
	return gzip_write_opcode(gfd, END_PATCH);
}  /* patch_output_end */

/*
 *		$History: $
 */
