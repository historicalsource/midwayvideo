/*
 *		$Archive: /video/tools/buildrom/lex.h $
 *		$Revision: 1 $
 *		$Date: 9/30/97 10:50a $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __LEX_H__
#define __LEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *		GLOBAL PROTOTYPES
 */

void init_lex(void);
int yylex(void);

/*
 *		GLOBAL VARIABLES
 */

extern int line_number;

#ifdef __cplusplus
}
#endif

/*
 *		$History: lex.h $
 * 
 * *****************  Version 1  *****************
 * User: Mlynch       Date: 9/30/97    Time: 10:50a
 * Created in $/video/tools/buildrom
 * Include files for the buildrom tool.
 */

#endif
