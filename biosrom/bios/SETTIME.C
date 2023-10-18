//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 3 $
//
// $Author: Mlynch $
//
struct dostime_t
{
	unsigned char	hour;
	unsigned char	minute;
	unsigned char	second;
	unsigned char	hsecond;
};

int set_pic_time(struct dostime_t *);

unsigned int _settime(struct dostime_t *time)
{
	return(set_pic_time(time));
}
