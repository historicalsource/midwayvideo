#include	<stdio.h>
#include	<string.h>

static char	filename[256];

int main(int argc, char *argv[])
{
	unsigned int	exe_addr;
	FILE				*fp;
	FILE				*fp1;
	char				*tmp;
	int				c;

	if(argc != 3)
	{
		printf("\n\nUSAGE: bin2exe <file.bin> <address>\n\n");
		exit(1);
	}

	if(!sscanf(argv[2], "%x", &exe_addr))
	{
		printf("\n\nAddress: %s - not understood\n\n");
		exit(1);
	}

	strcpy(filename, argv[1]);
	strlwr(filename);

	if((tmp = strstr(filename, ".bin")) == (void *)0)
	{
		printf("\n\nFile %s does not have .bin extension\n\n", argv[1]);
		exit(1);
	}
	*tmp = 0;
	strcat(filename, ".exe");

	if((fp = fopen(argv[1], "rb")) == (FILE *)0)
	{
		printf("\n\nCan not open file: %s\n\n", argv[1]);
		exit(1);
	}

	if((fp1 = fopen(filename, "wb")) == (FILE *)0)
	{
		fclose(fp);
		printf("\n\nCan not open file: %s\n\n", filename);
		exit(1);
	}

	fwrite(&exe_addr, sizeof(exe_addr), 1, fp1);

	while((c = fgetc(fp)) != EOF)
	{
		fputc(c, fp1);
	}

	fflush(fp1);
	fclose(fp);
	fclose(fp1);

	printf("\nFile %s create from file %s\n", filename, argv[1]);

	exit(0);

	return(0);
}


