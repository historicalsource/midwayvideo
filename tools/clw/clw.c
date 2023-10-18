#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "iffr.h"
#include "uvr.h"

/* model source data structures */
typedef struct _VERTEX {
	float	x,y,z;
} VERTEX;

typedef struct _ST {
	float	s,t;
} ST;

typedef struct _TRI {
	unsigned short	v1,v2,v3;
	unsigned short	t1,t2,t3;
	short			texture;
} TRI;

typedef enum {
	PS_NONE,
	PS_NAME,
	PS_SCALE,
	PS_SCALEX,
	PS_SCALEY,
	PS_SCALEZ,
	PS_APPEND,
	PS_MAP,
	PS_MAPW,
	PS_MAPH
} pstate;

enum {
	MODE_GENERIC_MODEL,
	MODE_HEAD_MODEL,
	MODE_LIMB_MODEL
};

VERTEX	bvtx[VTX_MAX];
TRI 		btri[FACE_MAX];
ST			bst[MAX_TRIANGLES * 3];

int	nvtx = 0, ntri = 0, nst = 0;
int	nface = 0, nsurf = 0;
vec3d		vdb[VTX_MAX];		/* vertex list */
grface		fdb[FACE_MAX];		/* face list */
lwSurface	sdb[SURF_MAX];		/* surface list */
flags_t		flags;
char		textures[MAX_TEXTURES][TEXTURE_LEN];
vertex_t	uvpairs[MAX_TRIANGLES*3];
int			to_unique[MAX_TEXTURES];
int			from_unique[MAX_TEXTURES];
int			num_unique, is_unique;
int			nfiles,nuvpairs,ntriangles;
int			zsort_mode;
int			no_limbs;
int			no_textures;
float		sfx = 1.0f;
float		sfy = 1.0f;
float		sfz = 1.0f;

static char writebuf[256], datfile[256];

static void print_help(void);

/**********************************************************************/
int main(int argc, char *args[])
{
	int		i,j,os;
	pstate pmode = PS_NONE;
	int cf_square = FALSE;
	int cf_1x2 = FALSE;
	int cf_w = 0;
	int cf_h = 0;
	char empty_string[] = "";
	char xxx_string[] = "xxx";
	char *np = xxx_string, *papp = empty_string;
	float	sf,tf;
	char *filespec = NULL;
	int operating_mode;
	FILE *dat;

#ifdef JONHEY
	float max_x,max_y,max_z,min_x,min_y,min_z;
	float vx,vy,vz;
#endif

	operating_mode = MODE_GENERIC_MODEL;
	/* parse command line */
	for (i = 1; i < argc; i++) {
		switch (args[i][0]) {
		case '-':
			switch (args[i][1]) {
			case 'b':				/* for dump a head model into a binary .DAT file */
				operating_mode = MODE_HEAD_MODEL;
				break;
			case 'e':				/* for generating a limb header file */
				operating_mode = MODE_LIMB_MODEL;
				break;
			case 'm':	/* scale factor */
				if (cf_square || cf_1x2)
					fprintf(stderr,"WARNING: %s will be overridden by a previous option\n", args[i]);
				switch (args[i][2]) {
				case '\0':
					pmode = PS_MAP;
					break;
				case 'w':
					pmode = PS_MAPW;
					break;
				case 'h':
					pmode = PS_MAPH;
					break;
				default:
					fprintf(stderr,"WARNING: Unrecognized command line option: %s\n", args[i]);
					break;
				}
				break;
			case 'n':	/* name */
				pmode = PS_NAME;
				break;
			case 's':	/* assume square textures */
				cf_square = TRUE;
				break;
			case 'f':	/* scale factor */
				switch (args[i][2]) {
				case '\0':
					pmode = PS_SCALE;
					break;
				case 'x':
					pmode = PS_SCALEX;
					break;
				case 'y':
					pmode = PS_SCALEY;
					break;
				case 'z':
					pmode = PS_SCALEZ;
					break;
				default:
					fprintf(stderr,"WARNING: Unrecognized command line option: %s\n", args[i]);
					break;
				}
				break;
			case 'h':	/* help */
				print_help();
				exit(1);
				break;
			case 'a':	/* append string to pset names */
				pmode = PS_APPEND;
				break;
			case 'z':	/* z-sort triangles */
				zsort_mode = TRUE;
				break;
			case 'x':	/* assume 2x1 textures */
				cf_1x2 = TRUE;
				break;
			case 'l':	/* no limb list */
				no_limbs = TRUE;
				break;
			case 't':	/* no texture list */
				no_textures = TRUE;
				break;
			default:
				fprintf(stderr,"WARNING: Unrecognized command line option: %s\n", args[i]);
				break;
			}
			break;
		default:
			switch (pmode) {
			case PS_NAME:
				np = args[i];
				break;
			case PS_SCALE:
				sfx = sfy = sfz = atof(args[i]);
				break;
			case PS_SCALEX:
				sfx = atof(args[i]);
				break;
			case PS_SCALEY:
				sfy = atof(args[i]);
				break;
			case PS_SCALEZ:
				sfz = atof(args[i]);
				break;
			case PS_APPEND:
				papp = args[i];
				break;
			case PS_NONE:
				filespec = args[i];
				break;
			case PS_MAP:
				cf_w = cf_h = atoi(args[i]);
				break;
			case PS_MAPW:
				cf_w = atoi(args[i]);
				break;
			case PS_MAPH:
				cf_h = atoi(args[i]);
				break;
			default:
				break;
			}
			pmode = PS_NONE;
			break;
		}
	}
	
	if (!filespec) {
		fprintf(stderr, "ERROR: File not specified.  clw -h for info.\n");
		exit(1);
	}
	
	sprintf(writebuf, "%s.lwo", filespec);
	read_lwo(writebuf);
	
	sprintf(writebuf, "%s.uv", filespec);
	read_uv(writebuf);
	
	/*
	 * to_unique maps original texture list into list of unique indices.
	 * from_unique maps the other way, to first instance of each texture
	 */
	num_unique = 0;
	for(i = 0; i < nsurf; i++) {
		is_unique = TRUE;
		for(j = 0; (j < i) && is_unique; j++) {
			if (!strcmp(textures[i], textures[j])) {
				is_unique = FALSE;
				j--;
			}
		}
		
		if (is_unique) {
			num_unique++;
			from_unique[num_unique-1] = i;
		}
		to_unique[i] = j;
	}
	
	/* update surface indices */
	for (i = 0; i < nface; i++)
		fdb[i].surface = to_unique[fdb[i].surface];
	
	/* mangle the surface indices */
	os = fdb[0].surface;
	for (i = 1; i < nface; i++) {
		if (fdb[i].surface == os)
			fdb[i].surface = -1;
		else
			os = fdb[i].surface;
	}
	
	/* if dumping a head models to a .DAT file */
	if (operating_mode == MODE_HEAD_MODEL) {
		sprintf(datfile, "%s%s.dat", np, papp);
		dat = fopen(datfile, "wb");
		if (dat == NULL) {
			fprintf(stderr,"could not create file %s\n", datfile);
			exit(1);
		}
	} else
		dat = NULL;
	
	/* output the VERTEX list */
	if (operating_mode == MODE_HEAD_MODEL) {
		for (i = 0; i < nvtx; i++) {
			bvtx[i].x = vdb[i].x * sfx;
			bvtx[i].y = vdb[i].y * sfy;
			bvtx[i].z = vdb[i].z * sfz * -1.0f;
		}
		fwrite(bvtx, sizeof(bvtx[0]), nvtx, dat);
	} else {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("VERTEX %s%s_vtx[] =\n{\n", np, papp);
		for (i = 0; i < nvtx; i++) {
			printf("\t{%ff,%ff,%ff}%s",
					vdb[i].x * sfx,
					vdb[i].y * sfy,
					vdb[i].z * sfz * -1.0f,
					(i == (nvtx-1) ? "\n};\n" : ",\n"));
		}
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
		printf("\n");
	}
	
	/* output the TRI list */
	if (operating_mode == MODE_HEAD_MODEL) {
		for (i = 0; i < nface; i++) {

#ifndef JONHEY
			btri[i].v1 = fdb[i].vtxi[0] * 3;
			btri[i].v2 = fdb[i].vtxi[1] * 3;
			btri[i].v3 = fdb[i].vtxi[2] * 3;
#else
			btri[i].v1 = fdb[i].vtxi[0];
			btri[i].v2 = fdb[i].vtxi[1];
			btri[i].v3 = fdb[i].vtxi[2];
#endif
			btri[i].t1 = fdb[i].tvtx[0];
			btri[i].t2 = fdb[i].tvtx[1];
			btri[i].t3 = fdb[i].tvtx[2];
			btri[i].texture = fdb[i].surface * 4;
		}
		btri[i].v1 = btri[i].v2 = btri[i].v3 = 0;
		btri[i].t1 = btri[i].t2 = btri[i].t3 = 0;
		btri[i].texture = -20;
		fwrite(btri, sizeof(btri[0]), nface + 1, dat);
	} else {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("TRI %s%s_tris[] =\n{\n", np, papp);
		for (i = 0; i < nface; i++) {

#ifndef JONHEY
			printf("\t{%3d,%3d,%3d, %2d,%2d,%2d, %d},\n",
				   fdb[i].vtxi[0] * 3,
				   fdb[i].vtxi[1] * 3,
				   fdb[i].vtxi[2] * 3,
				   fdb[i].tvtx[0],
				   fdb[i].tvtx[1],
				   fdb[i].tvtx[2],
				   fdb[i].surface * 4);
#else
			printf("\t{%3d,%3d,%3d, %2d,%2d,%2d, %d},\n",
				   fdb[i].vtxi[0] ,
				   fdb[i].vtxi[1] ,
				   fdb[i].vtxi[2] ,
				   fdb[i].tvtx[0],
				   fdb[i].tvtx[1],
				   fdb[i].tvtx[2],
				   fdb[i].surface * 4);
#endif

		}
		printf("\t{0, 0, 0, 0, 0, 0, -20}\n};\n");
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
		printf("\n");
	}
	
	/* get the S and T scaling factor */
	if (cf_square) {
		sf = 256.0f;
		tf = 256.0f;
	} else if (cf_1x2) {
		sf = 256.0f;
		tf = 128.0f;
	} else {
		sf = pow2(cf_w);
		tf = pow2(cf_h);
	}
	
	/* output the S and T list */
	if (operating_mode == MODE_HEAD_MODEL) {
		for (i = 0; i < nuvpairs; i++) {
			bst[i].s = uvpairs[i].u * sf;
			bst[i].t = uvpairs[i].v * tf;
		}
		fwrite(&nuvpairs, sizeof(nuvpairs), 1, dat);
		fwrite(bst, sizeof(bst[0]), nuvpairs, dat);
		fclose(dat);
	} else {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("ST %s%s_st[] =\n{\n", np, papp);
		for (i = 0; i < nuvpairs; i++) {
			printf("\t{%ff,%ff}%s",
					uvpairs[i].u * sf,
					uvpairs[i].v * tf,
					(i == (nuvpairs-1) ? "\n};\n" : ",\n"));
		}
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
		printf("\n");
	}


	#ifdef JONHEY
	/* output the bounding boxes */

		printf("VERTEX %s%s_bbox[] =\n{\n", np, papp);

		max_x = vdb[0].x * sfx;
		max_y = vdb[0].y * sfy;
		max_z = vdb[0].z * sfz * -1.0f;
		min_x = vdb[0].x * sfx;
		min_y = vdb[0].y * sfy;
		min_z = vdb[0].z * sfz * -1.0f;
			
		for (i = 0; i < nvtx; i++) 
			{

			vx = vdb[i].x * sfx;
			vy = vdb[i].y * sfy;
			vz = vdb[i].z * sfz * -1.0f;


			if (vx < min_x)
				min_x = vx;

			if (vy < min_y)
				min_y = vy;

			if (vz < min_z)
				min_z = vz;

			if (vx > max_x)
				max_x = vx;

			if (vy > max_y)
				max_y = vy;

			if (vz > max_z)
				max_z = vz;

			}
	
			printf("\t{%ff,%ff,%ff},\n",min_x,min_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,min_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,max_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",min_x,max_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,min_y,min_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,min_y,max_z);
			printf("\t{%ff,%ff,%ff},\n",max_x,max_y,min_z);
			printf("\t{%ff,%ff,%ff}\n};\n",max_x,max_y,max_z);
		printf("\n");
	#endif
	
	/* output the LIMB data */
	printf("LIMB limb_%s%s =\n{\n", np, papp);
	printf("\t%d,\n", nvtx);
	printf("\t%d,\n", nface);
	printf("#ifdef MODEL_DATA_ST_COUNT\n");
	printf("\t%d,\n", nuvpairs);
	printf("#else\n");
	printf("\tNULL,\n");
	printf("#endif\n");
	if (operating_mode == MODE_GENERIC_MODEL) {
		printf("\t%s%s_vtx,\n", np, papp);
		printf("\t%s%s_st,\n", np, papp);
		printf("\t%s%s_tris,\n", np, papp);
	#ifdef JONHEY
		if (nvtx > 8)
			{
			printf("\t%s%s_bbox,\n", np, papp);
			}
		else
			{
			printf("\tNULL,\n");
			}
	#endif
	} else {
		printf("#ifdef MODEL_DATA\n");
		printf("\t%s%s_vtx,\n", np, papp);
		printf("\t%s%s_st,\n", np, papp);
		printf("\t%s%s_tris,\n", np, papp);
	#ifdef JONHEY
		if (nvtx > 8)
			{
			printf("\t%s%s_bbox,\n", np, papp);
			}
		#else
			{
			printf("\tNULL,\n");
			}
	#endif
		printf("#else\n");
		printf("\tNULL,\n");
		printf("\tNULL,\n");
		printf("\tNULL,\n");
	#ifdef JONHEY
		if (nvtx > 8)
			{
			printf("\t%s%s_bbox,\n", np, papp);
			}
		else
			{
			printf("\tNULL,\n");
			}
	#endif
		printf("#endif\n");
	}
	printf("};\n\n");
	
	/* output the TEXTURE data */
	if ((operating_mode != MODE_HEAD_MODEL) && !no_textures) {
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#ifdef MODEL_DATA\n");
		
		printf("char *%s_textures[] =\n{\n", np);
		for (i = 0; i < num_unique; i++)
			printf("\t\"%s\",\n", textures[from_unique[i]]);
		printf("\tNULL\n};\n");
		
		if (operating_mode == MODE_LIMB_MODEL)
			printf("#endif\n");
	}
	return 0;
}

void print_help(void)
{
	printf("clw [options] <filename>\n");
	printf("  -n xxx      name polyset\n");
	printf("  -s          square textures\n");
	printf("  -x          2x1 textures\n");
	printf("  -f[xyz]     scale factor\n");
}
