/*
** Copyright 1989 Integrated Device Technology, Inc.
** All rights reserved.
*/

/*
** istdio.c -- stdio routines
*/
#include <varargs.h>
#include "idtio.h"

static int  default_base = 10;
static int  spprintfmode;
static int  column;         /* current output column for tab processing */


void putchar(), puts(), putc(), showchar();
int printf();

static void  prtn();
static void  printn();
int  _putstr();


/*
** putchar -- put a single character to enabled console devices
** handles simple character mappings
*/
void
putchar(c)
char c;
{
//    put_char(stdout,c);
}


/*
** putc -- put a character only to a single device
*/
void
putc(c,fd)
char c;
int fd;
{
//    write(fd, &c, 1);
}


void
puts(s)
register char *s;
{
	register c;

	if (s == NULL)
		s = "<NULL>";
	while (c = *s++)
		putchar(c);
}

vsprintf(char *str, char *fmt, char *ap)
{
    int count;

    spprintfmode = 1;
    count = _printf(str,_putstr,fmt,ap);
    spprintfmode = 0;

    return (count);
}


sprintf(str, fmt, va_alist)
char *str;
char *fmt;
va_dcl
{
    int count;
	va_list ap;

    spprintfmode = 1;

	va_start(ap);
    count = _printf(str,_putstr,fmt, ap);
    va_end(ap);

    spprintfmode = 0;

    return (count);
}


printf(fmt, va_alist)
char *fmt;
va_dcl
{
	int count;
	va_list ap;


    spprintfmode = 1;

    va_start(ap);
//    count = _printf(stdout,put_char,fmt, ap);
    va_end(ap);

    spprintfmode = 0;

    return (count);
}



/************************************************************************/
/*  _printf.c                                                           */
/*                                                                      */
/*          _ P R I N T F  M O D U L E                                  */
/*          --------------------------                                  */
/*  -------------------------------------------------------             */
/*  Copyright 1987 by Integrated Device Technology Inc.                 */
/*          All rights reserved                                         */
/*                                                                      */
/*  -------------------------------------------------------             */
/*                                                                      */
/*  This module is called through the single entry point "_printf"      */
/*  to perform the conversions and output for the library functions:    */
/*                                                                      */
/*        printf  - Formatted print to standard output                  */
/*        fprintf - Formatted print to stream file                      */
/*        sprintf - Formatted print to string                           */
/*                                                                      */
/*  The calling routines are logically a part of this module, but       */
/*  when are compiled separately to save space in the user's            */
/*  program only one of the library routines is used.                   */
/*                                                                      */
/*  The following routines are present:                                 */
/*                                                                      */
/*        _printf       Internal printf conversion / output             */
/*        _prnt8        Octal conversion routine                        */
/*        _prntx        Hex conversion routine                          */
/*        __conv        Decimal ASCII to binary routine                 */
/*        _putstr       Output character to string routine              */
/*        _prtl     Decimal conversion routine                          */
/*                                                                      */
/*  The following routines are called:                                  */
/*                                                                      */
/*        strlen        Compute length of a string                      */
/*        putc      Stream output routine                               */
/*        ftoa      Floating point output conversion routine            */
/*                                                                      */
/*                                                                      */
/*  This routine depends on the fact that the argument list is          */
/*  always composed of LONG data items.                                 */
/*                                                                      */
/*  Configured for Whitesmith's C on VAX.  "putc" arguments are         */
/*  reversed from UNIX.                                                 */
/*                                                                      */
/************************************************************************/
/*              Modifications                                           */
/************************************************************************/
/*                                                                      */
/*  MEP - Wed Sep 9, 1987 12:18:56 - Created file                       */
/*  MJM - Wed Sep 9, 1987 12:19:20 - Corrected typos                    */
/*  MJM - Wed Sep 9, 1987 12:25:08 - Commented out floating point code	*/
/*  MJM - Wed Sep 9, 1987 14:59:00 - Added LOCAL & GLOBAL definitions	*/
/*  MJM - Sun Sep 13, 1987 21:00:15 - Change stdio.h to portab.h        */
/*  MJM - Tue Sep 15, 1987 17:08:11 - Fix prtl, local a[] can't be REG	*/
/*  MJM - Tue Sep 15, 1987 21:54:02 - Add 'l' modifier to % case        */
/*  MJM - Fri Jul  8, 1988 11:25:02 - Changed to "varargs.h" method.	*/
/*  DLC - Fri Sep 21, 1990 11:45:00 - Added floating point              */
/*                                                                      */
/************************************************************************/

/*
**	Local DEFINEs
*/
 
/*
**	Local static data
*/
static char *_ptrbf;        /********************************/
static char *_ptrst;        /*  Buffer Pointer              */
static char *__fmt;         /*  -> File/string (if any)     */
                            /********************************/

/*****************************************************************************
*
*		P R I N T F  I N T E R N A L  R O U T I N E
*		-------------------------------------------
*
*	Routine "_printf" is used to handle all "printf" functions, including
*	"sprintf", and "fprintf".
*
*	Calling Sequence:
*
*		_printf(fd,func,fmt,argl);
*
*	Where:
*
*		fd		Is the file or string pointer.
*		func		Is the function to handle output.
*		fmt		Is the address of the format string.
*		argl		Is the address of the first arg.
*
*
*	Returns:
*
*		Number of characters output
*
*	Bugs:
*
*	It is assumed that args are contiguous starting at "arg1", and that
*	all are the same size (LONG), except for floating point.
*****************************************************************************/
_printf(fd,f,fmt,args)                  /************************************/
    int     fd;                         /* Not really, but . . .            */
    int     (*f)();                     /* Function pointer                 */
    char    *fmt;                       /* -> Format string                 */
    va_list args;                       /* -> Arg list                      */
{                                       /************************************/
    char  c;                            /* Format Character temp            */
    char  *s;                           /* Output string pointer            */
    char  adj;                          /* Right/left adjust flag           */
    char  len;                          /* short or long flag               */
    char  buf[30];                      /* Temporary buffer                 */
                                        /************************************/
    static int  argcount;               /* count of arguments               */
    volatile int    x;                  /* Arg Value   temporary            */
    volatile int    y;                  /* Arg Value   temporary            */
    int   n;                            /* String Length Temp               */
    int   m;                            /* Field  Length Temporary          */
    int   width;                        /* Field width                      */
    int   prec;                         /* Precision for "%x.yf"            */
    char  padchar;                      /* '0' or ' ' (padding)             */
/*  double    fx;                       /* Floating temporary               */
/*  double    *dblptr;                  /* Floating temp. address           */
    int   ccount;                       /* Character count                  */
    int   b;                            /* base                             */
    int   charset;                      /* caps for hex or not 1=caps       */
    char  numbuf[16];                   /* tempoary number buf              */
                                        /************************************/
					
					
                                        /****************************/
    charset = 0;                        /* select lower case as default */
    ccount = 0;                         /* Initially no characters  */
	argcount = 0;
    _ptrbf = buf;                       /* Set buffer pointer       */
    _ptrst = (char *)fd;                /* Copy file descriptor     */
    __fmt = fmt;                        /* Copy format address      */
                                        /****************************/
                                        /*              */
/*************************************************              */
/* This is the main format conversion loop,  Load a character from the      */
/* format string.  If the character is '%', perform the appropriate         */
/* conversion.  Otherwise, just output the character.                       */
/*************************************************			    */
                                        /*              */
    while( c = *__fmt++ )               /* Pick up nest format char */
    {                                   /*              */
      if(c != '%')                      /****************************/
        {                               /*              */
        (*f)(fd,c);                     /* If not '%', just output  */
        ccount++;                       /* Bump character count     */
        }                               /****************************/
      else                              /* It is a '%',             */
        {                               /*      convert     */
        len = 's';                      /* Set length flag      */
                                        /****************************/
        if( *__fmt == '-' )             /* Check for left adjust    */
          {                             /****************************/
          adj = 'l';                    /* Is left, sed flag        */
          __fmt++;                      /* Bump format pointer      */
          }                             /*              */
        else                            /* Right adjust         */
          adj = 'r';                    /****************************/
                                        /*              */
	    padchar=(*__fmt=='0') ? '0' : ' ';	/* Select Pad character     */
        if (*__fmt == '*')
	      {
		  ++__fmt;
          width = va_arg(args, int);
	      }
	    else
          width = __conv();             /* Convert width (if any)   */
                                        /****************************/
        if( *__fmt == '.')              /* '.' means precision spec */
          {                             /*              */
          ++__fmt;                      /* Bump past '.'            */
	      if (*__fmt == '*')
	        {
		    ++__fmt;
            prec = va_arg(args, int);
		}
	      else
            prec = __conv();            /* Convert precision spec   */
          }                             /*              */
        else                            /* None specified           */
          prec = 0;                     /****************************/
        if(*__fmt == 'L' || *__fmt == 'l')  /* Long output              */
          {                             /*              */
          len = 'l';                    /*              */
          __fmt++;                      /*  conversions         */
          }                             /*              */
        s = 0;                          /* Assume no output string  */
        switch ( c = *__fmt++ )         /* Next char is conversion  */
          {                             /*              */
        case 'D':                       /* Decimal          */
        case 'd':                       /*              */
        case 'u':                       /*              */
          b = 10;                       /*  set base            */
        break;                          /* Go do output         */
                                        /****************************/
        case 'o':                       /* Octal            */
        case 'O':                       /*    Print         */
          b = 8;                        /*  set base            */
          break;                        /* Go do output         */
                                        /****************************/
        case 'X':                       /*                  */
          charset = 1;                  /* falls through    */
        case 'x':                       /* Hex              */
          b = 16;                       /* set base             */
          break;                        /* Go do output         */
                                        /****************************/
        case 'S':                       /* String           */
        case 's':                       /*     Output?      */
              s =(char*) va_arg(args, int ); /* x = next argument       */
              if (s == NULL) s = "<NULL>";
          argcount++;
          break;                        /* Go finish up         */
                                        /****************************/
        case 'C':                       /* Character            */
        case 'c':                       /*  Output?         */
          x = va_arg(args, int );       /* x = next argument        */
		  argcount++;
          *_ptrbf++ = x&0377;           /* Just load buffer     */
		  *_ptrbf = 0;
		  s = buf;
          break;                        /* Go output            */
                                        /****************************/
        case 'v':
		   if(width == 0)
			width = prec;
		   b = default_base;
		   if( default_base == 8 )
		      width = (((4 * width)/3) + 1 ); 
		   else if ( default_base == 10 )
		      width = (((56 * width)/45) + 1 );
		   else if (default_base == 0)
		      default_base = 16;
		   break;
		case 'e':
		case 'f':
		case 'g':
		case 'E':
        case 'G':
           break;
        default:                        /* None of the above    */
		  *_ptrbf++ = c;
		  *_ptrbf = 0;
		  s = buf;
       }                                /* End switch           */
                                        /****************************/
       if (s == 0)                      /* If s = 0, string is in   */
        {                               /* "buf",           */
         x = va_arg(args, int );        /* x = next argument        */
	     argcount++;
	     prtn(prec,b,x,charset);
         *_ptrbf = 0;                   /*   Insure termination     */
         s = buf;                       /*   Load address       */
         charset = 0;                   /* reset back to lower case */
        }                               /****************************/
       n = strlen (s);                  /* Compute converted length */
	   if( prec == 0 )
	     prec = n;	
       m = width-n;                     /* m is # of pad characcters*/
                                        /****************************/
       if (adj == 'r')                  /* For right adjust,        */
          while (m-- > 0)               /* Pad in front         */
          {                             /*              */
            (*f)(fd,padchar);           /* Thusly           */
        ccount++;                       /* Count it         */
          }                             /*              */
                                        /****************************/
	   while ((n--) && ( prec-- > 0) )	/* Output Converted	    */
         {                              /*              */
          (*f)(fd,*s++);                /*        Data      */
           ccount++;                    /* Count it         */
          }                             /****************************/
       while (m-- > 0)                  /* If left adjust,      */
         {                              /*              */
          (*f)(fd,padchar);             /*        Pad       */
          ccount++;                     /* Count padded characters  */
         }                              /****************************/
        _ptrbf = buf;                   /* Reset buffer pointer     */
        }                               /* End else clause      */
     }                                  /* End while            */
     if((*f) == _putstr)                /* If string output,        */
           (*f)(fd,0);                  /* Drop in terminator char  */
                                        /****************************/
    return(ccount);                     /* Return appropriate value */
                                        /* End _printf          */
                                        /****************************/
}


/*
** prtn() - converts an interger to an ascii string.
**	entry:
**		prec - an integer containing the max number of chars
**			to be returned in the str.
**		b - base ( 8=octal 10=decimal 16=hex - surprise! 
**		x - the integer that is the object of this exercise
**	return:
**		yup!
*/
static void prtn(prec,b,x,cset)
int prec;
int b;
unsigned int x;
int cset;
{
    char tmpbuf[32];
    char *cptr;
    char *cs;

    cptr = tmpbuf;
    cs = (cset != 0) ? "0123456789ABCDEF" : "0123456789abcdef";

	do {
        *cptr++ = cs[x%b];
        x /= b;
    } while (x);

    if (((cptr-tmpbuf) > prec) && (prec != 0)) {
       cptr = tmpbuf + prec;
    }

	do {
	    *_ptrbf++ = *--cptr;
    } while (cptr > tmpbuf);
}

/****************************************************************************/
/*									    */
/*			_ _ C o n v  F u n c t i o n			    */
/*			----------------------------			    */
/*									    */
/*	Function "__conv" is used to convert a decimal ASCII string in	    */
/*	the format to binary.						    */
/*									    */
/*	Calling Sequence:						    */
/*									    */
/*	      val = __conv();						    */
/*									    */
/*	Returns:							    */
/*									    */
/*	      "val" is the converted value				    */
/*	      Zero is returned if no value				    */
/*									    */
/****************************************************************************/
int  __conv()                            /*                          */
{                                        /****************************/
    register char   c;                   /* Character temporary      */
    register int    n;                   /* Accumulator              */
                                         /****************************/
      n = 0;                             /* Zero found so far        */
      while (((c = *__fmt++) >= '0') &&  /* While c is a digit       */
             (c <= '9'))                 /*                          */
        n = (n*10) + (c-'0');            /* Add c to accumulator     */
      __fmt--;                           /* Back up format pointer to*/
                                         /* character skipped above  */
      return(n);                         /* See, wasn't that simple? */
}                                        /****************************/


/****************************************************************************/
/*									    */
/*			_ P u t s t r  F u n c t i o n			    */
/*			------------------------------			    */
/*									    */
/*	Function "_putstr" is used by "sprintf" as the output function	    */
/*	argument to "_printf".  A single character is copied to the buffer  */
/*	at "_ptrst".							    */
/*									    */
/*	Calling Sequence:						    */
/*									    */
/*		   _putstr(str,chr);					    */
/*									    */
/*	where "str" is a dummy argument necessary because the other output  */
/*	functions have two arguments.					    */
/*									    */
/*	Returns:							    */
/*									    */
/*	      (none)							    */
/*									    */
/****************************************************************************/
int _putstr(str, chr)				/*			    */
char *str;				/* Dummy argument	    */
char chr;				/* The output character	    */
{						/****************************/
      *_ptrst++ =chr;				/* Output the character	    */
      return(0);				/* Go back		    */
}						/****************************/


/*
** sprintn converts a number n in base b and puts the results in
**	a string pointed to by str.
*/
char * sprintn(n, b,precis,str)
register unsigned n;
register int b;
int precis;
char *str;
{
	char prbuf[11];
	register char *cp;
	int xx;

	if (b < 0) {
		b = -b;
		if ((int)n < 0) {
			*str++ = '-';
			n = (unsigned)(-(int)n);
		}
	}

	cp = prbuf;
	do {
		*cp++ = "0123456789abcdef"[n%b];
		n /= b;
	} while (n);
	if ( ((cp - prbuf) > precis) && (precis != 0) )
	   cp = prbuf + precis;
	xx = (int)(cp - prbuf);
	while ( xx++ < precis )
	   *str++ = '0';
	do
		*str++ = (*--cp);
	while (cp > prbuf);
	return (str);
}

/*
 * showchar -- print character in visible manner
 */
void
showchar(c)
int c;
{
	c &= 0xff;
	if (isprint(c))
		putchar(c);
	else switch (c) {
	case '\b':
		puts("\\b");
		break;
	case '\f':
		puts("\\f");
		break;
	case '\n':
		puts("\\n");
		break;
	case '\r':
		puts("\\r");
		break;
	case '\t':
		puts("\\t");
		break;
	default:
		putchar('\\');
		putchar(((c&0300) >> 6) + '0');
		putchar(((c&070) >> 3) + '0');
		putchar((c&07) + '0');
		break;
	}
}
