/*
����������������������������������������������������������������������������ͻ
�                                                                            �
� File:    GPRINTF.C                                                         �
� Author:  Jack Miller                                                       �
� Created: 03-Jun-1996                                                       �
�                                                                            �
����������������������������������������������������������������������������͹
�                                                                            �
�     Graphics 'printf' function for the 3DFX Voodoo Graphics System         �
�                                                                            �
�  This module references '_screenPtr' which is a global variable that must  �
�  be set up before calling 'gprintf()'.  It is set up with a call to        �
�  'grLfbGetWritePtr(buffer)'.                                               �
�                                                                            �
����������������������������������������������������������������������������͹
� HISTORY:                                                                   �
�                                                                            �
�  03Jun96 JVM  Created.                                                     �
�                                                                            �
����������������������������������������������������������������������������ͼ
*/
#include <compiler.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glide.h>


/*
����������������������������������������������������������������������������Ŀ
�                       Structure & Literal Defintions                       �
������������������������������������������������������������������������������
*/
#define _CR         '\r'
#define _NL         '\n'
#define _TAB        '\t'
#define _SPC        ' '

#define FONT_WIDTH  8
#define FONT_HEIGHT 8
#define FONT_SZ     ((FONT_WIDTH*FONT_HEIGHT)/8)

#define TAB_SZ      8
#define TAB_PIXELS  (TAB_SZ*FONT_WIDTH)

#define X_START_OFFSET  16
#define Y_START_OFFSET  12

#define T_SCREEN_WIDTH  480
#define T_SCREEN_HEIGHT 388

/* min and max macros */
#if !defined(max)
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif


/*
����������������������������������������������������������������������������Ŀ
�                             External Functions                             �
������������������������������������������������������������������������������
*/



/*
����������������������������������������������������������������������������Ŀ
�                               External Data                                �
������������������������������������������������������������������������������
*/
extern mxU8         font8x8[];


/*
����������������������������������������������������������������������������Ŀ
�                              Local Functions                               �
������������������������������������������������������������������������������
*/
static void writeString(int, int, mxU32, mxU32, mxBool, mxU8 *);
static void writeChar8(int, int, mxU32, mxU32, mxBool, mxU8);
static void putPixel(int, int, mxU32);


/*
����������������������������������������������������������������������������Ŀ
�                                Global Data                                 �
������������������������������������������������������������������������������
*/
mxU16               *_screenPtr;
mxU16               _fxBackColor;


/*
����������������������������������������������������������������������������Ŀ
�                                 Local Data                                 �
������������������������������������������������������������������������������
*/
static mxU8         stringBuf[4608];


/*
����������������������������������������������������������������������������Ŀ
�                                    Code                                    �
������������������������������������������������������������������������������
*/
int gprintf(int x, int y, mxU32 fColor, mxU32 bColor, char *fmt, ...) /*
����������������������������������������������������������������������������
    Used just like 'printf' with additional arguments for x and y location
    location and foreground and background colors.

    x, y   - Character position to begin drawing string.
    fColor - Foreground pixel color.
    bColor - Background pixel color.
    *fmt   - Ordinary characters, escape sequences, and format
             specifications.
    (...)  - any additional string arguments

    returns # of characters printed
����������������������������������������������������������������������������*/
{
    va_list     marker;
    int         retVal;
    mxBool      drawbg;
    GrState     state;


    va_start(marker, fmt);
    retVal = vsprintf(stringBuf, fmt, (int *)marker);
    va_end(marker);

    drawbg = (fColor == bColor) ? FALSE : TRUE;

    x = (x * FONT_HEIGHT) + X_START_OFFSET;
    y = (y * FONT_HEIGHT) + Y_START_OFFSET;

    grGlideGetState(&state);
    grLfbWriteMode(GR_LFBWRITEMODE_565);
    grLfbBypassMode(GR_LFBBYPASS_ENABLE);

    grLfbBegin();
    writeString(x, y, fColor, bColor, drawbg, &stringBuf[0]);
    grLfbEnd();

    grGlideSetState(&state);

    return (retVal);

}


static void writeString(int x, int y, mxU32 fColor, mxU32 bColor, mxBool bg, mxU8 *s) /*
����������������������������������������������������������������������������
    display 's' at (x,y) formatted by processText ().
����������������������������������������������������������������������������*/
{
    char    ch;
    int     textX, count;

    textX = x;

    while ((ch = *s)) {

        switch (ch) {
          case _TAB:
            count = (TAB_PIXELS - (x  % TAB_PIXELS)) / FONT_WIDTH;
            while (count) {
                writeChar8(x, y, bColor, bColor, bg, _SPC);
                x+=FONT_WIDTH;
                count--;
            }
            break;

          case _NL:
            y += FONT_HEIGHT;
            x = textX;
            break;

          case _CR:
            x = textX;
            break;

          default:
            writeChar8(x, y, fColor, bColor, bg, ch);
            x+=FONT_WIDTH;
        }

        s++;
    }
}


static void writeChar8(int x, int y, mxU32 fColor, mxU32 bColor, mxBool bg, mxU8 ch) /*
����������������������������������������������������������������������������
    Draw a character using the 8x8 character font by plotting the individual
    pixels of each character. Pixels corresponding to character information
    are plotted in 'fColor', background pixels are plotted in 'bColor'.

    x, y   - Position to begin drawing string.
    fColor - Foreground pixel color.
    bColor - Background pixel color.
    ch     - Character to draw
����������������������������������������������������������������������������*/
{
    mxU8            byte;
    int             r, c;
    mxU8            *font;

    font = font8x8;

    for (r = 0; r < FONT_HEIGHT; r++) {

        byte = *(font + (ch * FONT_SZ) + r);

        for (c = 0; c < FONT_WIDTH; c++) {

            if (byte & 0x80) {
               putPixel (x+c, y+r, fColor);
            } else {
              /* test for background pixels draw */
                if (bg) putPixel (x+c, y+r, bColor);
            }
            byte <<= 1;

        }
    }
}


static void putPixel(int x, int y, mxU32 pixel) /*
����������������������������������������������������������������������������
    Light the 'pixel' at (x, y) in screen buffer.
    NOTE: _screenPtr must be set up prior to calling this routine.
����������������������������������������������������������������������������*/
{

    _screenPtr[x + y * 1024] = (mxU16)pixel;

}