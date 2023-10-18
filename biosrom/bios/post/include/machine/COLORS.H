#ifndef _COLOR_H
#define _COLOR_H 1

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                            ³
³                             Literal Definitions                            ³
³                                                                            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

/* Define indeces into the EGA color table */
enum EGA_COLORS {
  _BLACK,               /* dark colors  */
  _BLUE,
  _GREEN,
  _CYAN,
  _RED,
  _MAGENTA,
  _DKYELLOW,
  _DKGRAY,
  _LTGRAY,              /* light colors */
  _LTBLUE,
  _LTGREEN,
  _LTCYAN,
  _LTRED,
  _LTMAGENTA,
  _LTYELLOW,
  _LTWHITE,
};

/* The following define "EGA" gun values for 5:6:5 bits/pixel color mode */

                      /*  RED GUN     GREEN GUN   BLUE GUN  */
#define DK_BLK          ((00 << 11) | (00 << 5) | (00 << 0))
#define DK_BLU          ((00 << 11) | (00 << 5) | (21 << 0))
#define DK_GRN          ((00 << 11) | (42 << 5) | (00 << 0))
#define DK_CYN          ((00 << 11) | (42 << 5) | (21 << 0))

#define DK_RED          ((21 << 11) | (00 << 5) | (00 << 0))
#define DK_MAG          ((21 << 11) | (00 << 5) | (21 << 0))
#define DK_YEL          ((21 << 11) | (42 << 5) | (00 << 0))
#define DK_WHT          ((21 << 11) | (42 << 5) | (21 << 0))

#define LT_BLK          ((11 << 11) | (21 << 5) | (11 << 0))
#define LT_BLU          ((11 << 11) | (21 << 5) | (31 << 0))
#define LT_GRN          ((11 << 11) | (63 << 5) | (11 << 0))
#define LT_CYN          ((11 << 11) | (63 << 5) | (31 << 0))

#define LT_RED          ((31 << 11) | (21 << 5) | (11 << 0))
#define LT_MAG          ((31 << 11) | (21 << 5) | (31 << 0))
#define LT_YEL          ((31 << 11) | (63 << 5) | (11 << 0))
#define LT_WHT          ((31 << 11) | (63 << 5) | (31 << 0))

#define LT_ORG          ((31 << 11) | (31 << 5) | (5 << 0))

/* Define the maximum gun value color combinations */

#define MAXBLK          ((00 << 11) | (00 << 5) | (00 << 0))
#define MAXBLU          ((00 << 11) | (00 << 5) | (31 << 0))
#define MAXGRN          ((00 << 11) | (63 << 5) | (00 << 0))
#define MAXCYN          ((00 << 11) | (63 << 5) | (31 << 0))
#define MAXRED          ((31 << 11) | (00 << 5) | (00 << 0))
#define MAXYEL          ((31 << 11) | (63 << 5) | (00 << 0))
#define MAXWHT          ((31 << 11) | (63 << 5) | (31 << 0))

#endif
