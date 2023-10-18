#include	<stdio.h>
#include	<dirent.h>
#include	<sys/stat.h>

//#define	DEBUG

int main(int argc, char *argv[])
{
	DIR	*d;

	//
	// Did we get expected arguments ?
	//
	if(argc != 2)
	{
		//
		// NOPE - say so and exit
		//
		fprintf(stderr, "USAGE: cmdir <dir_name>\n");
		exit(1);
	}

	//
	// Does the specified directory already exist ?
	//
	d = opendir(argv[1]);
	if(d)
	{
		//
		// YES - Done
		//
#ifdef DEBUG
		fprintf(stderr, "Directory %s already exists\n", argv[1]);
#endif
		closedir(d);
		exit(0);
	}

	//
	// Create the directory
	//
	if(mkdir(argv[1], 0))
	{
		//
		// ERROR
		//
		fprintf(stderr, "Problem creating directory: %s\n", argv[1]);
		exit(1);
	}

	//
	// Done
	//
	exit(0);
}
	
