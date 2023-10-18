/* $Revision: 1 $ */

#include	<stdlib.h>
#include	<stdio.h>
#include	<sys\stat.h>

int main(int argc, char *argv[])
{
	struct stat	buf;

	while(--argc)
	{
		// Check for existance of file
		if(!(stat(argv[argc], &buf)))
		{
			// Delete file if it exists
			if(unlink(argv[argc]))
			{
				// Error deleting file
				printf("Could not delete file: %s\n", argv[argc]);
				exit(1);
			}
		}
	}
	exit(0);
}
