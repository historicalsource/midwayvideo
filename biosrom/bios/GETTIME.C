//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 2 $
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

int get_pic_time(struct dostime_t *);

void _gettime(struct dostime_t *time)
{
	get_pic_time(time);
}
