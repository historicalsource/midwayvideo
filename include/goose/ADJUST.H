//
// This file contains the adjustment ids for all adjustments that are NOT
// game specific adjustments.  Game specific adjustment ids should be put
// into a game specific header file.
//
#ifndef	__ADJUST_H__
#define	__ADJUST_H__

#ifdef VERSIONS
char	goose_adjust_h_version[] = {"$Revision: 9 $"};
#endif

typedef struct system_adjustment_info
{
	int	adj_num;
	int	min;
	int	max;
	int	default_val;
} system_adjustment_info_t;

#define	adjustment_info_t	system_adjustment_info_t

typedef enum
{
FREEPLAY_ADJ,						// Game free play mode
TOTALIZER_ADJ,						// Coin counter mode
ATTRACT_SOUND_ADJ,		  		// Sounds during attract mode
VOLUME_LEVEL_ADJ,					// Volume level
LEFT_SLOT_UNITS_ADJ,				// Left coin slot units
RIGHT_SLOT_UNITS_ADJ,	  		// Right coin slot units
CENTER_SLOT_UNITS_ADJ,			// Center coin slot units
EXTRA_SLOT_UNITS_ADJ,	  		// Extra coin slot units
BILL_VALIDATOR_UNITS_ADJ,		// Bill validator units
UNITS_PER_CREDIT_ADJ,	  		// Units per credit
UNITS_PER_BONUS_ADJ,				// Units per bonus
MINIMUM_UNITS_ADJ,		  		// Maximum units
CREDITS_TO_START_ADJ,	  		// Credits to start
CREDITS_TO_CONTINUE_ADJ,  		// Credits to continue
MAXIMUM_CREDITS_ADJ,				// Maximum credits
COINS_PER_BILL_ADJ,				// Coins per bill
SHOW_FRACTIONS_ADJ,				// Show fractions
LEFT_CHUTE_COUNT_ADJ,	  		// Left chute count
RIGHT_CHUTE_COUNT_ADJ,			// Right chute count
CENTER_CHUTE_COUNT_ADJ,			// Center chute count
EXTRA_CHUTE_COUNT_ADJ,			// Extra chute count
BILL_VALIDATOR_COUNT_ADJ,		// Bill validator count
CURRENCY_ADJ,						// Currency type
PRICING_INFO_STRING_ADJ,  		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ2, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ3, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ4, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ5, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ6, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ7, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ8, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ9, 		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ10,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ11,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ12,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ13,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ14,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ15,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ16,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ17,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ18,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ19,		// Pricing information string (20 entries)
PRICING_INFO_STRING_ADJ20,		// Pricing information string (20 entries)
CONTINENT_ADJ,						// Pricing continent
COUNTRY_ADJ,						// Pricing country
PRICE_ADJ,							// Pricing for country
FACTORY_RESTORE_ADJ,				// This adjustment is how the game knows to
										// restore game specific stuff.  After
										// doing the restore it sets this to 0
HIGH_SCORE_RESET_ADJ,			// FLAG HIGH SCORE RESET
FIRST_GAME_ADJ						// First adjustment available for a game
} adjustment_number_t;

int check_system_adjustments(int, adjustment_info_t *);
int restore_factory_adjustments(int, adjustment_info_t *);

// Definitions of control types for game installed adjustments
#define	ADJ_CONTROL_TYPE_DECIMAL			0		// Decimal value
#define	ADJ_CONTROL_TYPE_LIST				1		// Menu List
#define	ADJ_CONTROL_TYPE_ENABLE_DISABLE	2		// Dialog box
#define	ADJ_CONTROL_TYPE_STRING				3		// Not implemented
#define	ADJ_CONTROL_TYPE_YES_NO				4		// Dialog box (y/n)


// Macros used to add adjustments

#define	ADD_DECIMAL_ADJUST(a, b, c, d)	fprintf((a), "startadjust %d \"%s\" min %d max %d default %d adjustdone\r\n\r\n", (b), (c), (d)[(b)-FIRST_GAME_ADJ].min, (d)[(b)-FIRST_GAME_ADJ].max, (d)[(b)-FIRST_GAME_ADJ].default_val);

#define	ADD_LIST_ADJUST(a, b, c, d)	fprintf((a), "startadjust %d \"%s\" default %d control %d\r\n", (b), (c), (d)[(b)-FIRST_GAME_ADJ].default_val, ADJ_CONTROL_TYPE_LIST)

#define	ADJUST_LIST_NAME(a, b)	fprintf((a), "listtitle \"%s\"\r\n", (b))

#define	ADJUST_LIST_ITEM(a, b)	fprintf((a), "listitem \"%s\"\r\n", (b))

#define	LIST_ADJUST_DONE(a)	fprintf((a), "adjustdone\r\n\r\n")

#define	ADD_ENABLE_DISABLE_ADJUST(a, b, c, d)	fprintf((a), "startadjust %d \"%s\" default %d control %d adjustdone\r\n\r\n", (b), (c), (d)[(b)-FIRST_GAME_ADJ].default_val, ADJ_CONTROL_TYPE_ENABLE_DISABLE)

#define	ADD_YES_NO_ADJUST(a, b, c, d)	fprintf((a), "startadjust %d \"%s\" default %d control %d adjustdone\r\n\r\n", (b), (c), (d)[(b)-FIRST_GAME_ADJ].default_val, ADJ_CONTROL_TYPE_YES_NO)

#endif
