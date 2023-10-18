//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//

char *strchr(char *str, int val)
{
	while(*str)
	{
		if(*str == val)
		{
			return(str);
		}
		++str;
	}
	return((char *)0);
}


