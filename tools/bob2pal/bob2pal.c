#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} rgb;

int main(int argc, char *argv[])
{
	FILE *fp;
	short width, height;
	rgb pal[256];
	char bob_name[FILENAME_MAX];
	char base_name[FILENAME_MAX];
	char pal_name[FILENAME_MAX];
	char *c;
	int i;
	
	if (argc != 2)
		printf("usage:%s bob_file\n", argv[0]);
	else {
		strcpy(bob_name, argv[1]);
		fp = fopen(bob_name, "rb");
		if (fp == NULL) {
			c = strrchr(bob_name, '.');
			if (c == NULL)
				strcat(bob_name, ".bob");
			fp = fopen(bob_name, "rb");
		}
		if (fp != NULL) {
			fread(&width, sizeof(width), 1, fp);
			fread(&height, sizeof(height), 1, fp);
			fread(pal, sizeof(pal), 1, fp);
			fclose(fp);
			
			strcpy(base_name, argv[1]);
			c = strrchr(base_name, '.');
			if (c != NULL)
				*c = '\0';
			strcpy(pal_name, base_name);
			strcat(pal_name, ".pal");
			
			fp = fopen(pal_name, "w");
			if (fp != NULL) {
				fprintf(fp, "unsigned char %s[768] = {", base_name);
				for (i = 0 ; i < 256; i++)
					fprintf(fp, "%s0x%02X, 0x%02X, 0x%02X%s\n", i != 0 ? "\t\t\t\t\t\t\t" : "", pal[i].red, pal[i].green, pal[i].blue, i != 255 ? "," : "");
				fprintf(fp, "};\n");
				fclose(fp);
			} else
				printf("%s:error creating file %s\n", pal_name, argv[1]);
		} else
			printf("%s:error opening file %s\n", argv[0], bob_name);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}	/* main */
