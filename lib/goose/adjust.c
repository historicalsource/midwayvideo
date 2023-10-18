//
// adjust.c
//
// $Revision: 4 $
//
#include	<goose/goose.h>

char	goose_adjust_c_version[] = {"$Revision: 4 $"};

static system_adjustment_info_t	system_adjustment_table[] = {
{FREEPLAY_ADJ, 0, 1, 0},					// Default is NO freeplay
{TOTALIZER_ADJ, 0, 1, 0},					// Default is totalizer mode
{ATTRACT_SOUND_ADJ, 0, 1, 1},				// Default is Attract sound on
{VOLUME_LEVEL_ADJ, 0, 255, 127},			// Default is 1/2 volume
{CONTINENT_ADJ, 0, 6, 0},					// Default is North America
{COUNTRY_ADJ, 0, 8, 0},						// Default is USA
{PRICE_ADJ, 0, 16, 0},						// Default is USA 1
{LEFT_SLOT_UNITS_ADJ, 0, 100, 1},		// Default is 1 (USA 1 Pricing)
{RIGHT_SLOT_UNITS_ADJ, 0, 100, 1},		// Default is 1 (USA 1 Pricing)
{CENTER_SLOT_UNITS_ADJ, 0, 100, 0},		// Default is 0 (USA 1 Pricing)
{EXTRA_SLOT_UNITS_ADJ, 0, 100, 0},		// Default is 0 (USA 1 Pricing)
{BILL_VALIDATOR_UNITS_ADJ, 0, 100, 4},	// Default is 4 (USA 1 Pricing)
{UNITS_PER_CREDIT_ADJ, 0, 100, 1},		// Default is 1 (USA 1 Pricing)
{UNITS_PER_BONUS_ADJ, 0, 100, 0},		// Default is 0 (USA 1 Pricing)
{MINIMUM_UNITS_ADJ, 0, 100, 0},			// Default is 0 (USA 1 Pricing)
{CREDITS_TO_START_ADJ, 0, 100, 2},		// Default is 2 (USA 1 Pricing)
{CREDITS_TO_CONTINUE_ADJ, 0, 100, 2},	// Default is 2 (USA 1 Pricing)
{MAXIMUM_CREDITS_ADJ, 0, 100, 30},		// Default is 30 (USA 1 Pricing)
{COINS_PER_BILL_ADJ, 0, 100, 4},			// Default is 4 (USA 1 Pricing)
{SHOW_FRACTIONS_ADJ, 0, 1, 1},			// Default is 1 (USA 1 Pricing)
{LEFT_CHUTE_COUNT_ADJ, 0, 100, 1},		// Default is 1 (USA 1 Pricing)
{RIGHT_CHUTE_COUNT_ADJ, 0, 100, 1},		// Default is 1 (USA 1 Pricing)
{CENTER_CHUTE_COUNT_ADJ, 0, 100, 0},	// Default is 1 (USA 1 Pricing)
{EXTRA_CHUTE_COUNT_ADJ, 0, 100, 0}, 	// Default is 1 (USA 1 Pricing)
{BILL_VALIDATOR_COUNT_ADJ, 0, 100, 4},	// Default is 1 (USA 1 Pricing)
{CURRENCY_ADJ, 0, 12, 0},					// Default is 0 (USA 1 Pricing)
{FACTORY_RESTORE_ADJ, 0, 1, 1},			// Default is "RESTORE"
{HIGH_SCORE_RESET_ADJ, 0, 1, 1}			// Default is RESET
};

static int _check_system_adjustments(int num, adjustment_info_t *sa)
{
	int	status = 0;
	int	i;
	int	val;

	for(i = 0; i < num; i++)
	{
		if(get_adjustment(sa->adj_num, &val))
		{
			status = 1;
			// Adjustment is bogus - reset it to its default value
			if(!set_adjustment(sa->adj_num, sa->default_val))
			{
				if(!get_adjustment(sa->adj_num, &val))
				{
					// Check the value
					if(val != sa->default_val)
					{
						// Bogus - CMOS is probably dead
						break;
					}
				}
			}
		}
		else if(val < sa->min)
		{
			status = 1;
			// Adjustment is bogus - reset it to its default value
			if(!set_adjustment(sa->adj_num, sa->default_val))
			{
				if(!get_adjustment(sa->adj_num, &val))
				{
					// Check the value
					if(val != sa->default_val)
					{
						// Bogus - CMOS is probably dead
						break;
					}
				}
			}
		}
		else if(val > sa->max)
		{
			status = 1;
			// Adjustment is bogus - reset it to its default value
			if(!set_adjustment(sa->adj_num, sa->default_val))
			{
				if(!get_adjustment(sa->adj_num, &val))
				{
					// Check the value
					if(val != sa->default_val)
					{
						// Bogus - CMOS is probably dead
						break;
					}
				}
			}
		}
		sa++;
	}
	return(status);
}

int check_system_adjustments(int num, adjustment_info_t *ga)
{
	int	status = 0;

	status = _check_system_adjustments(sizeof(system_adjustment_table)/sizeof(system_adjustment_info_t),
		system_adjustment_table);
	if(num && ga)
	{
		status |= _check_system_adjustments(num, ga);
	}
	return(status);
}

static int _restore_factory_adjustments(int num, system_adjustment_info_t *sa)
{
	int	i;
	int	status = 0;
	int	val;

	for(i = 0; i < num; i++)
	{
		if(set_adjustment(sa->adj_num, sa->default_val))
		{
			if(set_adjustment(sa->adj_num, sa->default_val))
			{
				status |= 1;
			}
		}
		if(get_adjustment(sa->adj_num, &val))
		{
			status |= 2;
		}
		else if(val != sa->default_val)
		{
			status |= 4;
		}
		sa++;
	}
	return(status);
}

int restore_factory_adjustments(int num, adjustment_info_t *ga)
{
	int	status = 0;

	status = _restore_factory_adjustments(sizeof(system_adjustment_table)/sizeof(system_adjustment_info_t),
		system_adjustment_table);
	if(num && ga)
	{
		status |= _restore_factory_adjustments(num, ga);
	}
	return(status);
}
