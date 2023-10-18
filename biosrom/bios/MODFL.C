#include	<math.h>

double modfl(double x, double *pint)
{
	int	integer;

	integer = (int)x;
	x -= (double)integer;
	*pint = (double)integer;
	return(x);
}
