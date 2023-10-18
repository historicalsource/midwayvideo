typedef union point_data
{
	float	fdata;
	int	idata;
} point_data_t;

typedef struct	chip_draw_info
{
	int	num_points;
	int	*points;
	int	status;
} chip_draw_info_t;

#define	CHIP_STATUS_UNTESTED		0
#define	CHIP_STATUS_NOT_STUFFED	1
#define	CHIP_STATUS_GOOD			2
#define	CHIP_STATUS_FAILED		3
#define	CHIP_STATUS_TESTING		4

void draw_chip(chip_draw_info_t *);

