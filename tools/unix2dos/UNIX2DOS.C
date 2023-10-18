#include	<stdio.h>

int main(int argc, char *argv[])
{
	int c;
	FILE	*fp;
	FILE	*fp1;
	int	convert;

	if(argc < 2)
	{
		printf("\nUSAGE: uxix2dos <filename> ... <filename>\n");
		return(1);
	}
	while(--argc)
	{
		convert = 1;
		if((fp = fopen(argv[argc], "rb")) == (FILE *)0)
		{
			printf("\nCan not open file %s\n", argv[argc]);
			return(1);
		}
		if((fp1 = fopen("tmp", "wb")) == (FILE *)0)
		{
			printf("\nCan not open temporary file\n");
			return(1);
		}
		while((c = fgetc(fp)) != EOF)
		{
			if(c == '\n')
			{
				fputc('\r', fp1);
			}
			else if(c == '\r')
			{
				printf("\nFile %s does not appear to need conversion\n", argv[argc]);
				fclose(fp);
				fclose(fp1);
				unlink("tmp");
				convert = 0;
				break;
			}
			fputc(c, fp1);
		}
		fflush(fp1);
		fclose(fp);
		fclose(fp1);
		if(convert)
		{
			unlink(argv[argc]);
			rename("tmp", argv[argc]);
			printf("File %s converted\n", argv[argc]);
		}
	}
}
