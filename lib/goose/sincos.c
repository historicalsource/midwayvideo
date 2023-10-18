#include	<math.h>
#include	<goose/pmath.h>

char	goose_sincos_c_version[] = {"$Revision: 4 $"};

/****************************************************************************/

/* sine coefficients */
#define K3f		(-1.66666666666666666666e-01f)
#define K5f		( 8.33333333333333333333e-03f)
#define K7f		(-1.98412698412698412698e-04f)
#define K9f		( 2.75573192239858906525e-06f)
#define K11f	(-2.50521083854417187750e-08f)


float fsin(float t)
{
	register float t2;

	/* get t in range */
	while (t >= M_TWOPI) t -= M_TWOPI;
	while (t < 0.0F)     t += M_TWOPI;

	if (t < M_PI) {
		if (t >= M_PI_2)
			t = M_PI - t;
	}
	else {
		if (t >= M_3PI_2)
			t = M_TWOPI - t;
		else
			t = t - M_PI;
		t = -t;
	}
	t2 = t * t;
	t2 = t2 * (K3f + t2 * (K5f + t2 * (K7f + t2 * (K9f + t2 * K11f))));
	return(t + t * t2);
}


float fcos(float t)
{
	return(fsin(t + M_PI_2));
}
