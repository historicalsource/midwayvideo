#ifndef 	__MATH_H__
#define	__MATH_H__

#ifndef	__FLOAT_H__
#include	<float.h>
#endif

typedef union
{
	float value;
	struct 
	{
		unsigned int fraction0: 7;
		unsigned int fraction1: 16;
		unsigned int exponent: 8;
		unsigned int sign : 1;
	} number;
	struct 
	{
		unsigned int function1:16;
		unsigned int function0:6;
		unsigned int quiet:1;
		unsigned int exponent:8;
		unsigned int sign:1;
	} nan;
	long p1;
} __ieee_float_shape_type;

typedef struct exception
{
	char	*name;
	int	type;
	float	arg1;
	float	arg2;
	int	err;
	float	retval;
} exception_t;


#define __IEEE_FLT_EXPBIAS		127
#define __IEEE_FLT_EXPLEN		8
#define __IEEE_FLT_FRACLEN		(32 - (__IEEE_FLT_EXPLEN + 1))
#define __IEEE_FLT_MAXPOWTWO	((float)(1L << (32 - 8) - 1))
#define __IEEE_FLT_NAN_EXP		0xff

#define IEEE_BIAS		__IEEE_FLT_EXPBIAS
#define BITSTYPE		__ieee_float_shape_type
#define MAXPOW			__IEEE_FLT_MAXPOWTWO
#define TYPE_MAX		FLT_MAX
#define TYPE_MIN		FLT_MIN
#define TYPE_MAX_EXP	FLT_MAX_EXP
#define TYPE_MIN_EXP	FLT_MIN_EXP
#define finite(x)		(((__ieee_float_shape_type *)&(x))->number.exponent != __IEEE_FLT_NAN_EXP)

float _arcsine(float x, int flag);
float acos(float x);
float acosh(float xa);
float asin(float x);
float asinh(float x);
float satan(float arg);
float atan(float arg);
float atan2(float arg_1, float arg_2);
float atanh(float x);
float cbrt(float xa);
float cosh(float x);
float __matherror(char *name, float arg1, float arg2, int type, float ret);
float fabs(float x);
float floor(float x);
float ceil(float x);
float fmod(float xa, float ya);
float frexp(float val, int *exp);
float hypot(float aa, float ba);
float log(float xa);
float log10(float x);
float log1p(float xa);
float log2(float x);
float log__L(float z);
float modf(float aval, float *ipart_ptr);
float pow(float x, float y);
float sin(float x);
float cos(float x);
float sinh(float x);
float sqrt(float);
float tanh(float x);
float exp(float xa);
float ldexp(float rval, int );
int matherr(struct exception * );
float infinity(void);
float nan(void);
float maxpowtwo(void);
float scalbn(float xa, int y);
double modfl(double x, double *pint);


#define DOMAIN 1
#define SING 2
#define OVERFLOW 3
#define UNDERFLOW 4
#define TLOSS 5
#define PLOSS 6

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402

#define M_PI_4		0.78539816339744830962
#define M_PI_2		1.57079632679489661923
#define M_3PI_4		2.35619449019234492884
#define M_PI		3.14159265358979323846
#define M_5PI_4		3.92699081698724154808
#define M_3PI_2		4.71238898038468985769
#define M_7PI_4		5.49778714378213816731
#define M_TWOPI         6.28318530717958647692

#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_SQRTPI        1.77245385090551602792981
#define M_2_SQRTPI	1.12837916709551257390

#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3   	1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        0.693147180559945309417
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

#define	EDOM		1
#define	ERANGE	2

#if defined(DEBUG)
#define	fsqrt(x) \
({ float	__value, __arg = (x); \
	if((x) < 0.0f) \
	{\
		fprintf(stderr, "YO!! dipshit you cant sent fsqrt a negative value\n");\
		__asm__("	teqi	$0,0");\
	}\
	asm("sqrt.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#define	rsqrt(x) \
({ float	__value, __arg = (x); \
	if((x) < 0.0f) \
	{\
		fprintf(stderr, "YO!! dipshit you cant sent rsqrt a negative value\n");\
		__asm__("	teqi	$0,0");\
	}\
	asm("rsqrt.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#define	fabs(x) \
({ float	__value, __arg = (x); \
	asm("abs.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#else

#define	fsqrt(x) \
({ float	__value, __arg = (x); \
	asm("sqrt.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#define	rsqrt(x) \
({ float	__value, __arg = (x); \
	asm("rsqrt.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#define	fabs(x) \
({ float	__value, __arg = (x); \
	asm("abs.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#endif


#endif
