//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 3 $
//
// $Author: Mlynch $
//
struct dosdate_t
{
	unsigned char	day __attribute__((packed));
	unsigned char	month __attribute__((packed));
	unsigned short	year __attribute__((packed));
	unsigned char	dayofweek __attribute__((packed));
};

int set_pic_date(struct dosdate_t *);

unsigned int _setdate(struct dosdate_t *date)
{
	return(set_pic_date(date));
}
