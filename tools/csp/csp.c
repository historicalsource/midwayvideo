/* $Revision: 11 $                                                            */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define VMAX	10000

#define PFX_COORDATA	"cd_"
#define PFX_FLOATSPLINE	"fs_"
#define PFX_NORMDATA	"nd_"
#define PFX_RAILSPLINE	"rs_"

enum {
	M_COORDATA, M_FLOATSPLINE, M_NORMDATA, M_RAILSPLINE
};

//////////////////////////////////////////////////////////////////////////////
/* simple 4-vector */
//typedef struct _vec3d
//{
//	float	x,y,z,w;
//} vec3d;

/* 4-point spline struct */
typedef struct _SPLINE
{
	double	x1,y1,z1;		// starting pt (typically 0,0,0)
	double	x2,y2,z2;		// control pt 1
	double	x3,y3,z3;		// control pt 2
	double	x4,y4,z4;		// ending pt
} SPLINE;

typedef struct _VERTEX
{
	double	x,y,z;
} VERTEX;

/*
//////////////////////////////////////////////////////////////////////////////
// compute magnitude of vector v
//////////////////////////////////////////////////////////////////////////////
float magv( vec3d *v )
{
	float f;

	f = v->x * v->x;
	f += v->y * v->y;
	f += v->z * v->z;

	f /= v->w;

	f = sqrt( f );

	return f;
}

//////////////////////////////////////////////////////////////////////////////
// compute cross product of vectors v1 and v2
//////////////////////////////////////////////////////////////////////////////
void vxv( vec3d *v1, vec3d *v2, vec3d *v3 )
{	
	v3->x = v1->y * v2->z - v1->z * v2->y;
	v3->y = v1->z * v2->x - v1->x * v2->z;
	v3->z = v1->x * v2->y - v1->y * v2->x;
	v3->w = v1->w * v2->w;
}

//////////////////////////////////////////////////////////////////////////////
// normalize vector v
//////////////////////////////////////////////////////////////////////////////
void norm( vec3d *v )
{
	float m = magv( v );

	v->x /= m;
	v->y /= m;
	v->z /= m;
}
*/
//////////////////////////////////////////////////////////////////////////////
int invalid_option( char * sz )
{
	fprintf(stderr,"\r\nError -- invalid command-line option `%s'\r\n\r\n",sz);
	return(1);
}

//////////////////////////////////////////////////////////////////////////////
int invalid_filename( char * sz )
{
	fprintf(stderr,"\r\nError -- unable to access file `%s'\r\n\r\n",sz);
	return(1);
}

//////////////////////////////////////////////////////////////////////////////
int error_fileread( char * sz )
{
	fprintf(stderr,"\r\nError -- problem reading file `%s'\r\n\r\n",sz);
	return(1);
}

//////////////////////////////////////////////////////////////////////////////
int error_numpoints( char * sz )
{
	fprintf(stderr,"\r\nError -- not enough points in file `%s'\r\n\r\n",sz);
	return(1);
}

//////////////////////////////////////////////////////////////////////////////
int error_numentered( void )
{
	fprintf(stderr,"\r\nError -- not enough points entered\r\n\r\n");
	return(1);
}

//////////////////////////////////////////////////////////////////////////////
int error_generic( char * sz )
{
	fprintf(stderr,"\r\nError -- %s\r\n\r\n",sz);
	return(1);
}

//////////////////////////////////////////////////////////////////////////////
int main( int argc, char *args[] )
{
	// translation of full 4 pt to normalized 2 pt

	FILE * pfile = NULL;
	SPLINE sp;
	VERTEX vtx[VMAX];
	double * fp = (double *)&sp.x1;
	SPLINE sptmp;
//	VERTEX vtxtmp[3];
//	vec3d p1p2;
//	vec3d p1p3;
//	vec3d p13xp12;
//	vec3d ry;
//	float xform[12];
	double x = 1.0f;
	int i, j, k, l, m = 12;
	char * pszname = NULL;
	char * psz;
	char * fn = "\0";
	char sz[200];
	char szout[200];
	char cmod[] = "XYZ";
	char noverify = 0;
	char in_alias = 0;
	char mode     = M_FLOATSPLINE;
	register char c;

	for (i = 1; i < argc; i++) {
		switch (args[i][0]) {
			case '-':
			case '/':
//				psz = args[i];
//				while (*psz)
//					*psz++ = (char)tolower(*psz);
//				switch (args[i][1]) {
				switch (tolower(args[i][1])) {
					case 'h':
					case '?':
						psz = args[0];
						while (*psz)
							*psz++ = (char)toupper(*psz);
						fprintf(stderr,"\r\n%s [-a <fn>] [-c] [-f] [-n <x>] [-r] [-v] [<name>]\r\n\r\n"
								"    -a <fn>  Expect Alias point-list text file `fn' for input\r\n"
								"    -c       Take numbers just as entered (`coordinate data') & organize\r\n"
								"              them sequentially to a zero-terminated float matrix\r\n"
								"    -f       Convert an arbitrary 4 pt spline to 2 normalized control pts\r\n"
								"              (`float spline'), making the 1st pt the origin & the 4th pt\r\n"
								"              a full unit (1.0f) in measure; pts are rotated to the Z axis\r\n"
								"              (default mode)\r\n"
								"    -n <x>   Normalize numbers entered to the Z axis (`normalized data'),\r\n"
								"              making the 1st pt the origin & the last pt a full unit (x)\r\n"
								"              in measure; organize them sequentially to a zero-terminated\r\n"
								"              float matrix; <x> default is 1.0f if not specified\r\n"
								"    -r       Take an arbitrary 4 pt spline just as entered (`rail spline') &\r\n"
								"              organize the pts sequentially in a SPLINE structure\r\n"
								"    -v       Disable value verification query\r\n"
								"    <name>   Name for the matrix in the text output; if none specified,\r\n"
								"              will default to `"PFX_COORDATA"', `"PFX_FLOATSPLINE"', `"PFX_NORMDATA"' or `"PFX_RAILSPLINE"' prefix per\r\n"
								"              the selected mode\r\n",
								args[0]);
						return(0);

					case 'a':
						in_alias = 1;
						if ((i+1) < argc)
							fn = args[++i];
						break;

					case 'c':
						mode = M_COORDATA;
						fp = (double *)vtx;
						m = VMAX;
						break;

					case 'f':
						mode = M_FLOATSPLINE;
						fp = (double *)&sp.x1;
						m = 12;
						break;

					case 'n':
						mode = M_NORMDATA;
						fp = (double *)vtx;
						m = VMAX;
						//fprintf(stderr,"\r\ni=%d argc=%d\r\n",i,argc);
						if ((i+1) < argc) {
							j = 0;
							while ((c = args[i+1][j++])) {
								//fprintf(stderr,"\r\n`%c'\r\n",c);
								if (c < '0' || c > '9') {
									if (c != '-' && c != '.')
										break;
								}
							}
							if (!c) {
								sscanf(args[++i],"%lf",&x);
								//fprintf(stderr,"\r\nx=%lf\r\n",x);
							}
						}
						break;

					case 'r':
						mode = M_RAILSPLINE;
						fp = (double *)&sp.x1;
						m = 12;
						break;

					case 'v':
						noverify = 1;
						break;

					default:
						return(invalid_option(args[i]));
				}
				break;

			default:
				if (pszname)
					return(invalid_option(args[i]));
				pszname = args[i];
				break;
		}
	}
	if (in_alias) {
		pfile = fopen(fn, "r");
		if (!pfile)
			return(invalid_filename( fn ));
	}

	if (!pszname) {
		switch (mode) {
			case M_COORDATA:
				pszname = PFX_COORDATA;
				break;
			case M_FLOATSPLINE:
				pszname = PFX_FLOATSPLINE;
				break;
			case M_NORMDATA:
				pszname = PFX_NORMDATA;
				break;
			case M_RAILSPLINE:
				pszname = PFX_RAILSPLINE;
				break;
		}
	}

	i = 0;
	while (i < m) {
		j = i / 3 + 1;
		k = i % 3;
		if (!in_alias) {
			if (!k)
				fprintf(stderr,"Enter X%d, Y%d, Z%d> ", j, j, j);
			else
				fprintf(stderr,"Enter %c%d> ", cmod[k], j);

			psz = gets(sz);
		}
		else {
			if (!(psz = fgets(sz,sizeof(sz),pfile))) {
				if (ferror(pfile)) {
					fclose(pfile);
					return(error_fileread(fn));
				}
				break;
			}
			if (*psz == '\n' || *psz == '\r')
				*psz = '\0';
		}

		if (psz) {
			if (!(*psz)) {
				if (in_alias)
					continue;
				if (mode == M_FLOATSPLINE || mode == M_RAILSPLINE)
					continue;
				if (j == 1)
					return(1);

				fprintf(stderr,"\r\nCompleted (Y/N)");
				do {
					c = (char)getche();
				} while (c != 'Y' && c != 'N' && c != 'y' && c != 'n');
				fprintf(stderr,"\r\n");

				if (c == 'N' || c == 'n')
					continue;

				if (j < 3)
					return(error_numentered());

				break;
			}
			while (c = *psz) {
				if (c == '\n' && in_alias) {
					c = *psz = '\0';
					break;
				}
				if (c < '0' || c > '9') {
					if (c != '-' && c != '.' && c != ' ' && c != ',') {
						if (!in_alias)
							fprintf(stderr,"\r\nError -- invalid character `%c'\r\n\r\n",c);
						else
							fprintf(stderr,"\r\n%s\r\n",sz);
						break;
					}
				}
				psz++;
			}
			if (c)
				continue;

			fprintf(stderr,"\r\n");
			l = i;
			psz = sz;
			szout[0] = '\0';
			while (*psz) {
//				fprintf(stderr,"\r\n\r\n`%s'\r\n\r\n",psz);
				sscanf(psz,"%lf",&fp[i]);
				sprintf(szout,"%s%c%d= %lf  ", szout, cmod[k], j, fp[i]);

				while (*psz == ' ') psz++;
				do {
					c = *psz++;
					if (c == ' ' || c == ',')
						break;
				} while (c);

				if (!(k = ++i % 3) || !c)
					break;
			}

			if (in_alias && k) {
				c = 'n';
//				szout[0] = 'P';
//				psz = szout;
//				while (*psz++ != '=');
//				*(psz-1) = '\0';
//				fprintf(stderr,"%s\r\n", szout);
			}
			else {
				fprintf(stderr,"%s", szout);

				if (!noverify && !in_alias) {
					fprintf(stderr,"Correct (Y/N)");
					do {
						c = (char)getche();
					} while (c != 'Y' && c != 'N' && c != 'y' && c != 'n');
				}
				else
					c = 'y';

				fprintf(stderr,"\r\n");
				if (!in_alias)
					fprintf(stderr,"\r\n");
			}

//			fprintf(stderr,"%f %f %f\r\n", fp[i-3], fp[i-2], fp[i-1]);
			if (c == 'N' || c == 'n')
				i = l;
//			else if (!k) {
//				if (!fp[i-3] && !fp[i-2] && !fp[i-1])
//					break;
//			}
		}
		else {
			fprintf(stderr,"Really big error!\r\n\r\n");
			return(1);
		}
	}

	if (pfile)
		fclose(pfile);

	switch (mode) {
		case M_COORDATA:
			fp = (double *)vtx;
			printf("float %s[] = {\n",pszname);

			for (k = 1; k < j; fp += 3, k++)  
				printf("\t%f,\t%f,\t%f,\n",(float)fp[0],(float)fp[1],(float)fp[2]);

			printf("\t0.0f,\t0.0f,\t0.0f\n};\n");
			break;

		case M_FLOATSPLINE:
			if (i < m)
				return(error_numpoints(fn));

			//translate
			sptmp.x4 = sp.x4  - sp.x1;
			sptmp.x3 = sp.x3  - sp.x1;
			sptmp.x2 = sp.x2  - sp.x1;
			sptmp.x1 = (double)0.0f;
			sptmp.y4 = sp.y4  - sp.y1;
			sptmp.y3 = sp.y3  - sp.y1;
			sptmp.y2 = sp.y2  - sp.y1;
			sptmp.y1 = (double)0.0f;
			sptmp.z4 = sp.z4  - sp.z1;
			sptmp.z3 = sp.z3  - sp.z1;
			sptmp.z2 = sp.z2  - sp.z1;
			sptmp.z1 = (double)0.0f;
/*
			printf("float %s[] = {\n"
					"\t%f,\t%f,\t%f,\n"
					"\t%f,\t%f,\t%f,\n"
					"\t%f,\t%f,\t%f\n"
					"\t%f,\t%f,\t%f\n"
					"};\n",
					pszname,
					sptmp.x1, sptmp.y1, sptmp.z1,
					sptmp.x2, sptmp.y2, sptmp.z2,
					sptmp.x3, sptmp.y3, sptmp.z3,
					sptmp.x4, sptmp.y4, sptmp.z4);
*/
{
	double tsin, tcos, psin, pcos, m2d, m3d, dx2, dy2, dz2, dx3, dy3, dz3, dx4, dy4, dz4, dt;

	dx4 = sptmp.x4 - sptmp.x1;
	dy4 = sptmp.y4 - sptmp.y1;
	dz4 = sptmp.z4 - sptmp.z1;

	dx3 = sptmp.x3;
	dy3 = sptmp.y3;
	dz3 = sptmp.z3;

	dx2 = sptmp.x2;
	dy2 = sptmp.y2;
	dz2 = sptmp.z2;


	m2d = dx4 * dx4 + dz4 * dz4;
	m3d = dy4 * dy4 + m2d;

	m2d = sqrt(m2d);
	m3d = sqrt(m3d);

	psin = dy4 / m3d;
	pcos = m2d / m3d;
	tsin = dx4 / m2d;
	tcos = dz4 / m2d;


	dt  = dx2;
	dx2 = dt * tcos - dz2 * tsin;
	dz2 = dt * tsin + dz2 * tcos;

	dt  = dy2;
	dy2 = dt * pcos - dz2 * psin;
	dz2 = dt * psin + dz2 * pcos;


	dt  = dx3;
	dx3 = dt * tcos - dz3 * tsin;
	dz3 = dt * tsin + dz3 * tcos;

	dt  = dy3;
	dy3 = dt * pcos - dz3 * psin;
	dz3 = dt * psin + dz3 * pcos;


	dt  = dx4;
	dx4 = dt * tcos - dz4 * tsin;
	dz4 = dt * tsin + dz4 * tcos;

	dt  = dy4;
	dy4 = dt * pcos - dz4 * psin;
	dz4 = dt * psin + dz4 * pcos;
/*
	printf("\nTesting\t%f,\t%f,\t%f\n"
	       "\t%f,\t%f,\t%f\n"
	       "\t%f,\t%f,\t%f\n",
	       dx2,dy2,dz2,dx3,dy3,dz3,dx4,dy4,dz4);
*/
	dx2 /= dz4;
	dy2 /= dz4;
	dz2 /= dz4;

	dx3 /= dz4;
	dy3 /= dz4;
	dz3 /= dz4;

	dx4 /= dz4;
	dy4 /= dz4;
	dz4 /= dz4;

	printf("float %s[] = {\n"
			"\t%f,\t%f,\t%f,\n"
			"\t%f,\t%f,\t%f\n"
			"};\n",
			pszname, (float)dx2, (float)dy2, (float)dz2, (float)dx3, (float)dy3, (float)dz3);
}
			break;

		case M_NORMDATA:
			{
				double tsin, tcos, psin, pcos, m2d, m3d, dx2, dy2, dz2, dx3, dy3, dz3, dx4, dy4, dz4, dt;

				dx3 = vtx[0].x;
				dy3 = vtx[0].y;
				dz3 = vtx[0].z;

//	printf("\nPoints\t%f,\t%f,\t%f\n\t%f,\t%f,\t%f\n",dx3,dy3,dz3,vtx[j-2].x,vtx[j-2].y,vtx[j-2].z);

				dx4 = vtx[j-2].x - dx3;
				dy4 = vtx[j-2].y - dy3;
				dz4 = vtx[j-2].z - dz3;

				m2d = dx4 * dx4 + dz4 * dz4;
				m3d = dy4 * dy4 + m2d;

				if (fabs(m2d) < 0.000001f)
					return(error_generic("first & last points can not overlap"));

				m2d = sqrt(m2d);
				m3d = sqrt(m3d);

				psin = dy4 / m3d;
				pcos = m2d / m3d;
				tsin = dx4 / m2d;
				tcos = dz4 / m2d;

				dt  = dx4;
				dx4 = dt * tcos - dz4 * tsin;
				dz4 = dt * tsin + dz4 * tcos;

				dt  = dy4;
				dy4 = dt * pcos - dz4 * psin;

				dz4 = dt * psin + dz4 * pcos;

//	printf("\nUnit\t%f,\t%f,\t%f\n",dx4,dy4,dz4);

				fp = (double *)vtx;

				for (k = 1, l = 0; k < j; fp += 3, k++) {
					dx2 = fp[0] - dx3;
					dy2 = fp[1] - dy3;
					dz2 = fp[2] - dz3;

					dt  = dx2;
					dx2 = dt * tcos - dz2 * tsin;
					dz2 = dt * tsin + dz2 * tcos;

					dt  = dy2;
					dy2 = dt * pcos - dz2 * psin;
					dz2 = dt * psin + dz2 * pcos;

					dx2 = x * dx2 / dz4;
					dy2 = x * dy2 / dz4;
					dz2 = x * dz2 / dz4;

					if ((float)dx2 || (float)dy2 || (float)dz2) {
						if (!l)
							printf("float %s[] = {\n",pszname);
						l = 1;
					}
					else if (l) {
						dz2 = (double)0.000001f;
					}
					if (l)
						printf("\t%f,\t%f,\t%f,\n",(float)dx2,(float)dy2,(float)dz2);
				}
				if (l)
					printf("\t0.0f,\t0.0f,\t0.0f\n};\n");
			}
			break;

		case M_RAILSPLINE:
			if (i < m)
				return(error_numpoints(fn));

			printf("SPLINE %s = {\n"
					"\t%f,\t%f,\t%f,\n"
					"\t%f,\t%f,\t%f,\n"
					"\t%f,\t%f,\t%f,\n"
					"\t%f,\t%f,\t%f\n"
					"};\n",
					pszname,
					(float)sp.x1, (float)sp.y1, (float)sp.z1,
					(float)sp.x2, (float)sp.y2, (float)sp.z2,
					(float)sp.x3, (float)sp.y3, (float)sp.z3,
					(float)sp.x4, (float)sp.y4, (float)sp.z4);
			break;
	}

	return(0);
}