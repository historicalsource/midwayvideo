//
// randper.c
//
// $Revision: 4 $
//
#include	<stdlib.h>

char	goose_randper_c_version[] = {"$Revision: 4 $"};

int randrng(int val)
{
	if (val == 0)
		return 0;

	return((random() % val));
}
