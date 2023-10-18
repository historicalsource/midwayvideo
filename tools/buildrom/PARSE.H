typedef union {
	int param[4];
	char *str;
	int ic;
} YYSTYPE;
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


extern YYSTYPE yylval;
