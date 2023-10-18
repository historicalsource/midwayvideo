#ifndef __PMATH_H__
#define __PMATH_H__

#ifdef VERSIONS
char	goose_pmath_h_version[] = {"$Revision: 3 $"};
#endif


#define DG2RD(a)	((a)*((float)M_PI)/180.0f)			// Convert degrees to rads
#define GR2RD(a)	((a)*((float)M_PI)/512.0f)			// Convert Skiles grads to rads
#define RD2GR(a)	((a)/((float)M_PI)*512.0f)			// Convert rads to Skiles grads

float fcos(float);
float fsin(float);

#endif
