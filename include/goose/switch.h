#ifndef	__SWITCH_H__
#define	__SWITCH_H__

#ifdef VERSIONS
char	goose_switch_h_version[] = {"$Revision: 10 $"};
#endif

#define	COIN_DOOR_CLOSED	0
#define	COIN_DOOR_OPEN		1

#define	SWITCH_CLOSE		1
#define	SWITCH_OPEN			0

// Definitions of bit positions for player switches
// If the bit positions of the player switches changes, this is what must be
// changed to reflect those changes.
#define	P_UP_SHIFT					0
#define	P_DOWN_SHIFT				1
#define	P_LEFT_SHIFT				2
#define	P_RIGHT_SHIFT				3
#define	P_A_SHIFT					4
#define	P_B_SHIFT					5
#define	P_C_SHIFT					6
#define	P_D_SHIFT					7

#define	PLAYER_BITS					8

// Byte position of player inputs
// If the byte positions of the player switches changes, this is what must be
// changed to reflect those changes
#define	P1_BYTE						0
#define	P2_BYTE						1
#define	P3_BYTE						2
#define	P4_BYTE						3

// Definitions of Bits for player12 and player34 data registers
#define	P_UP							(1 << (P_UP_SHIFT))
#define	P_DOWN						(1 << (P_DOWN_SHIFT))
#define	P_LEFT						(1 << (P_LEFT_SHIFT))
#define	P_RIGHT						(1 << (P_RIGHT_SHIFT))
#define	P_A							(1 << (P_A_SHIFT))
#define	P_B							(1 << (P_B_SHIFT))
#define	P_C							(1 << (P_C_SHIFT))
#define	P_D							(1 << (P_D_SHIFT))

// Definitions of player shifts
#define	P1_SHIFT						(P1_BYTE * PLAYER_BITS)
#define	P2_SHIFT						(P2_BYTE * PLAYER_BITS)
#define	P3_SHIFT						(P3_BYTE * PLAYER_BITS)
#define	P4_SHIFT						(P4_BYTE * PLAYER_BITS)

// Definitions of shift amounts for player switches
#define	P1_UP_SWID					(P_UP_SHIFT    + P1_SHIFT)
#define	P1_DOWN_SWID				(P_DOWN_SHIFT  + P1_SHIFT)
#define	P1_LEFT_SWID				(P_LEFT_SHIFT  + P1_SHIFT)
#define	P1_RIGHT_SWID				(P_RIGHT_SHIFT + P1_SHIFT)
#define	P1_A_SWID					(P_A_SHIFT     + P1_SHIFT)
#define	P1_B_SWID					(P_B_SHIFT     + P1_SHIFT)
#define	P1_C_SWID					(P_C_SHIFT     + P1_SHIFT)
#define	P1_D_SWID					(P_D_SHIFT     + P1_SHIFT)

#define	P2_UP_SWID					(P_UP_SHIFT    + P2_SHIFT)
#define	P2_DOWN_SWID				(P_DOWN_SHIFT  + P2_SHIFT)
#define	P2_LEFT_SWID				(P_LEFT_SHIFT  + P2_SHIFT)
#define	P2_RIGHT_SWID				(P_RIGHT_SHIFT + P2_SHIFT)
#define	P2_A_SWID					(P_A_SHIFT     + P2_SHIFT)
#define	P2_B_SWID					(P_B_SHIFT     + P2_SHIFT)
#define	P2_C_SWID					(P_C_SHIFT     + P2_SHIFT)
#define	P2_D_SWID					(P_D_SHIFT     + P2_SHIFT)

#define	P3_UP_SWID					(P_UP_SHIFT    + P3_SHIFT)
#define	P3_DOWN_SWID				(P_DOWN_SHIFT  + P3_SHIFT)
#define	P3_LEFT_SWID				(P_LEFT_SHIFT  + P3_SHIFT)
#define	P3_RIGHT_SWID				(P_RIGHT_SHIFT + P3_SHIFT)
#define	P3_A_SWID					(P_A_SHIFT     + P3_SHIFT)
#define	P3_B_SWID					(P_B_SHIFT     + P3_SHIFT)
#define	P3_C_SWID					(P_C_SHIFT     + P3_SHIFT)
#define	P3_D_SWID					(P_D_SHIFT     + P3_SHIFT)

#define	P4_UP_SWID					(P_UP_SHIFT    + P4_SHIFT)
#define	P4_DOWN_SWID				(P_DOWN_SHIFT  + P4_SHIFT)
#define	P4_LEFT_SWID				(P_LEFT_SHIFT  + P4_SHIFT)
#define	P4_RIGHT_SWID				(P_RIGHT_SHIFT + P4_SHIFT)
#define	P4_A_SWID					(P_A_SHIFT     + P4_SHIFT)
#define	P4_B_SWID					(P_B_SHIFT     + P4_SHIFT)
#define	P4_C_SWID					(P_C_SHIFT     + P4_SHIFT)
#define	P4_D_SWID					(P_D_SHIFT     + P4_SHIFT)

// Definitions of the player switches as they are returned
#define	P1_UP							(1 << P1_UP_SWID)
#define	P1_DOWN						(1 << P1_DOWN_SWID)
#define	P1_LEFT						(1 << P1_LEFT_SWID)
#define	P1_RIGHT						(1 << P1_RIGHT_SWID)
#define	P1_A							(1 << P1_A_SWID)
#define	P1_B							(1 << P1_B_SWID)
#define	P1_C							(1 << P1_C_SWID)
#define	P1_D							(1 << P1_D_SWID)

#define	P2_UP							(1 << P2_UP_SWID)
#define	P2_DOWN						(1 << P2_DOWN_SWID)
#define	P2_LEFT						(1 << P2_LEFT_SWID)
#define	P2_RIGHT						(1 << P2_RIGHT_SWID)
#define	P2_A							(1 << P2_A_SWID)
#define	P2_B							(1 << P2_B_SWID)
#define	P2_C							(1 << P2_C_SWID)
#define	P2_D							(1 << P2_D_SWID)

#define	P3_UP							(1 << P3_UP_SWID)
#define	P3_DOWN						(1 << P3_DOWN_SWID)
#define	P3_LEFT						(1 << P3_LEFT_SWID)
#define	P3_RIGHT						(1 << P3_RIGHT_SWID)
#define	P3_A							(1 << P3_A_SWID)
#define	P3_B							(1 << P3_B_SWID)
#define	P3_C							(1 << P3_C_SWID)
#define	P3_D							(1 << P3_D_SWID)

#define	P4_UP							(1 << P4_UP_SWID)
#define	P4_DOWN						(1 << P4_DOWN_SWID)
#define	P4_LEFT						(1 << P4_LEFT_SWID)
#define	P4_RIGHT						(1 << P4_RIGHT_SWID)
#define	P4_A							(1 << P4_A_SWID)
#define	P4_B							(1 << P4_B_SWID)
#define	P4_C							(1 << P4_C_SWID)
#define	P4_D							(1 << P4_D_SWID)

#define	P_SWITCH_MASK				(P_UP|P_DOWN|P_RIGHT|P_LEFT|P_A|P_B|P_C|P_D)
#define	P1_SWITCH_MASK				(P_SWITCH_MASK << (P1_SHIFT))
#define	P2_SWITCH_MASK				(P_SWITCH_MASK << (P2_SHIFT))
#define	P3_SWITCH_MASK				(P_SWITCH_MASK << (P3_SHIFT))
#define	P4_SWITCH_MASK				(P_SWITCH_MASK << (P4_SHIFT))

#define	P_RLDU_MASK					(P_UP|P_DOWN|P_RIGHT|P_LEFT)
#define	P1_RLDU_MASK				(P_RLDU_MASK << (P1_SHIFT))
#define	P2_RLDU_MASK				(P_RLDU_MASK << (P2_SHIFT))
#define	P3_RLDU_MASK				(P_RLDU_MASK << (P3_SHIFT))
#define	P4_RLDU_MASK				(P_RLDU_MASK << (P4_SHIFT))

#define	P_RL_MASK					(P_RIGHT | P_LEFT)
#define	P1_RL_MASK					(P_RL_MASK << (P1_SHIFT))
#define	P2_RL_MASK					(P_RL_MASK << (P2_SHIFT))
#define	P3_RL_MASK					(P_RL_MASK << (P3_SHIFT))
#define	P4_RL_MASK					(P_RL_MASK << (P4_SHIFT))

#define	P_DU_MASK					(P_DOWN | P_UP)
#define	P1_DU_MASK					(P_DU_MASK << (P1_SHIFT))
#define	P2_DU_MASK					(P_DU_MASK << (P2_SHIFT))
#define	P3_DU_MASK					(P_DU_MASK << (P3_SHIFT))
#define	P4_DU_MASK					(P_DU_MASK << (P4_SHIFT))

#define	P_ABCD_MASK					(P_A|P_B|P_C|P_D)
#define	P1_ABCD_MASK				(P_ABCD_MASK << (P1_SHIFT))
#define	P2_ABCD_MASK				(P_ABCD_MASK << (P2_SHIFT))
#define	P3_ABCD_MASK				(P_ABCD_MASK << (P3_SHIFT))
#define	P4_ABCD_MASK				(P_ABCD_MASK << (P4_SHIFT))

// Definitions of the coin switch bit positions
#define	LEFT_COIN_SHIFT				0
#define	RIGHT_COIN_SHIFT				1
#define	P1_START_SHIFT					2
#define	SLAM_SHIFT						3
#define	TEST_SHIFT						4
#define	P2_START_SHIFT					5
#define	SERVICE_CREDIT_SHIFT			6
#define	CENTER_COIN_SHIFT				7
#define	EXTRA_COIN_SHIFT				8
#define	P3_START_SHIFT					9
#define	P4_START_SHIFT					10
#define	VOLUME_DOWN_SHIFT				11
#define	VOLUME_UP_SHIFT				12
#define	COINDOOR_INTERLOCK_SHIFT	14
#define	BILL_VALIDATOR_SHIFT			15

// Definitions of the the word positions of coins and dipswitches
#define	COIN_SHIFT						0
#define	DIP_SHIFT						16

// Defintions of the coin switch ids
#define	LEFT_COIN_SWID					(LEFT_COIN_SHIFT + COIN_SHIFT)
#define	RIGHT_COIN_SWID				(RIGHT_COIN_SHIFT + COIN_SHIFT)
#define	P1_START_SWID					(P1_START_SHIFT + COIN_SHIFT)
#define	SLAM_SWID						(SLAM_SHIFT + COIN_SHIFT)
#define	TEST_SWID						(TEST_SHIFT + COIN_SHIFT)
#define	P2_START_SWID					(P2_START_SHIFT + COIN_SHIFT)
#define	SERVICE_CREDIT_SWID			(SERVICE_CREDIT_SHIFT + COIN_SHIFT)
#define	CENTER_COIN_SWID				(CENTER_COIN_SHIFT + COIN_SHIFT)
#define	EXTRA_COIN_SWID				(EXTRA_COIN_SHIFT + COIN_SHIFT)
#define	P3_START_SWID					(P3_START_SHIFT + COIN_SHIFT)
#define	P4_START_SWID					(P4_START_SHIFT + COIN_SHIFT)
#define	VOLUME_DOWN_SWID				(VOLUME_DOWN_SHIFT + COIN_SHIFT)
#define	VOLUME_UP_SWID					(VOLUME_UP_SHIFT + COIN_SHIFT)
#define	COINDOOR_INTERLOCK_SWID		(COINDOOR_INTERLOCK_SHIFT + COIN_SHIFT)
#define	BILL_VALIDATOR_SWID			(BILL_VALIDATOR_SHIFT + COIN_SHIFT)

// Definitions of the bits on the miscellaneous input port
#define	LEFT_COIN_SW				(1 << (LEFT_COIN_SWID))
#define	RIGHT_COIN_SW				(1 << (RIGHT_COIN_SWID))
#define	P1_START_SW					(1 << (P1_START_SWID))
#define	SLAM_SW						(1 << (SLAM_SWID))
#define	TEST_SW						(1 << (TEST_SWID))
#define	P2_START_SW					(1 << (P2_START_SWID))
#define	SERVICE_CREDIT_SW			(1 << (SERVICE_CREDIT_SWID))
#define	CENTER_COIN_SW				(1 << (CENTER_COIN_SWID))
#define	EXTRA_COIN_SW				(1 << (EXTRA_COIN_SWID))
#define	P3_START_SW					(1 << (P3_START_SWID))
#define	P4_START_SW					(1 << (P4_START_SWID))
#define	VOLUME_DOWN_SW				(1 << (VOLUME_DOWN_SWID))
#define	VOLUME_UP_SW				(1 << (VOLUME_UP_SWID))
#define	COINDOOR_INTERLOCK_SW	(1 << (COINDOOR_INTERLOCK_SWID))
#define	BILL_VALIDATOR_SW			(1 << (BILL_VALIDATOR_SWID))
#define	MENU_UP_SW					VOLUME_UP_SW
#define	MENU_DOWN_SW				VOLUME_DOWN_SW
#define	MENU_ESCAPE_SW				SERVICE_CREDIT_SW
#define	MENU_SELECT_SW				TEST_SW
#define	TILT_SW						SLAM_SW


#define	PLAYER_STARTS_MASK		(P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)
#define	COIN_INPUTS_MASK			(LEFT_COIN_SW|RIGHT_COIN_SW|CENTER_COIN_SW|EXTRA_COIN_SW)
#define	VOLUME_SW_MASK				(VOLUME_UP_SW|VOLUME_DOWN_SW)
#define	MENU_CONTROL_MASK			(MENU_UP_SW|MENU_DOWN_SW|MENU_ESCAPE_SW|MENU_SELECT_SW)


// Definitions of dipswitch bit shifts
#define	DIP_BIT_0_SHIFT			0
#define	DIP_BIT_1_SHIFT			1
#define	DIP_BIT_2_SHIFT			2
#define	DIP_BIT_3_SHIFT			3
#define	DIP_BIT_4_SHIFT			4
#define	DIP_BIT_5_SHIFT			5
#define	DIP_BIT_6_SHIFT			6
#define	DIP_BIT_7_SHIFT			7
#define	DIP_BIT_8_SHIFT			8
#define	DIP_BIT_9_SHIFT			9
#define	DIP_BIT_10_SHIFT			10
#define	DIP_BIT_11_SHIFT			11
#define	DIP_BIT_12_SHIFT			12
#define	DIP_BIT_13_SHIFT			13
#define	DIP_BIT_14_SHIFT			14
#define	DIP_BIT_15_SHIFT			15

// Definitions of dipswitch ids
#define	DIP_BIT_0_SWID				(DIP_BIT_0_SHIFT + DIP_SHIFT)
#define	DIP_BIT_1_SWID				(DIP_BIT_1_SHIFT + DIP_SHIFT)
#define	DIP_BIT_2_SWID				(DIP_BIT_2_SHIFT + DIP_SHIFT)
#define	DIP_BIT_3_SWID				(DIP_BIT_3_SHIFT + DIP_SHIFT)
#define	DIP_BIT_4_SWID				(DIP_BIT_4_SHIFT + DIP_SHIFT)
#define	DIP_BIT_5_SWID				(DIP_BIT_5_SHIFT + DIP_SHIFT)
#define	DIP_BIT_6_SWID				(DIP_BIT_6_SHIFT + DIP_SHIFT)
#define	DIP_BIT_7_SWID				(DIP_BIT_7_SHIFT + DIP_SHIFT)
#define	DIP_BIT_8_SWID				(DIP_BIT_8_SHIFT + DIP_SHIFT)
#define	DIP_BIT_9_SWID				(DIP_BIT_9_SHIFT + DIP_SHIFT)
#define	DIP_BIT_10_SWID			(DIP_BIT_10_SHIFT + DIP_SHIFT)
#define	DIP_BIT_11_SWID			(DIP_BIT_11_SHIFT + DIP_SHIFT)
#define	DIP_BIT_12_SWID			(DIP_BIT_12_SHIFT + DIP_SHIFT)
#define	DIP_BIT_13_SWID			(DIP_BIT_13_SHIFT + DIP_SHIFT)
#define	DIP_BIT_14_SWID			(DIP_BIT_14_SHIFT + DIP_SHIFT)
#define	DIP_BIT_15_SWID			(DIP_BIT_15_SHIFT + DIP_SHIFT)

// Definitions of Dipswitch Inputs
#define	DIP_BIT_0					(1 << DIP_BIT_0_SWID)
#define	DIP_BIT_1					(1 << DIP_BIT_1_SWID)
#define	DIP_BIT_2					(1 << DIP_BIT_2_SWID)
#define	DIP_BIT_3					(1 << DIP_BIT_3_SWID)
#define	DIP_BIT_4					(1 << DIP_BIT_4_SWID)
#define	DIP_BIT_5					(1 << DIP_BIT_5_SWID)
#define	DIP_BIT_6					(1 << DIP_BIT_6_SWID)
#define	DIP_BIT_7					(1 << DIP_BIT_7_SWID)
#define	DIP_BIT_8					(1 << DIP_BIT_8_SWID)
#define	DIP_BIT_9					(1 << DIP_BIT_9_SWID)
#define	DIP_BIT_10					(1 << DIP_BIT_10_SWID)
#define	DIP_BIT_11					(1 << DIP_BIT_11_SWID)
#define	DIP_BIT_12					(1 << DIP_BIT_12_SWID)
#define	DIP_BIT_13					(1 << DIP_BIT_13_SWID)
#define	DIP_BIT_14					(1 << DIP_BIT_14_SWID)
#define	DIP_BIT_15					(1 << DIP_BIT_15_SWID)

// Definition of where bits end up in long word
#define	DSW1_BIT0					DIP_BIT_0
#define	DSW1_BIT1					DIP_BIT_1
#define	DSW1_BIT2					DIP_BIT_2
#define	DSW1_BIT3					DIP_BIT_3
#define	DSW1_BIT4					DIP_BIT_4
#define	DSW1_BIT5					DIP_BIT_5
#define	DSW1_BIT6					DIP_BIT_6
#define	DSW1_BIT7					DIP_BIT_7

#define	DSW2_BIT0					DIP_BIT_0
#define	DSW2_BIT1					DIP_BIT_1
#define	DSW2_BIT2					DIP_BIT_2
#define	DSW2_BIT3					DIP_BIT_3
#define	DSW2_BIT4					DIP_BIT_4
#define	DSW2_BIT5					DIP_BIT_5
#define	DSW2_BIT6					DIP_BIT_6
#define	DSW2_BIT7					DIP_BIT_7

#define	DIP_SW1_BITS				(DSW1_BIT0|DSW1_BIT1|DSW1_BIT2|DSW1_BIT3|DSW1_BIT4|DSW1_BIT5|DSW1_BIT6|DSW1_BIT7)
#define	DIP_SW2_BITS				(DSW2_BIT0|DSW2_BIT1|DSW2_BIT2|DSW2_BIT3|DSW2_BIT4|DSW2_BIT5|DSW2_BIT6|DSW2_BIT7)
#define	DIP_BITS						(DIP_SW1_BITS|DIP_SW2_BITS)

// Definition of what dip switches mean
// NOTE - The must match the tables in the dip test portion of diagnostics
typedef struct dip_inputs
{
	unsigned int	unused         : 16;
	unsigned int	coinage_mode   : 1;	// Coinage from dip or CMOS
	unsigned int	dip_coinage    : 5;	// Dip coinage
	unsigned int	totalizer_mode : 2;	// Coin counter mode
	unsigned int	reserved1      : 1;	// User defineable
	unsigned int	resolution     : 2;	// monitor resolution
	unsigned int	reserved       : 2;	// User defineable
	unsigned int	reserved2		: 1;	// User defineable
	unsigned int	putest_bypass  : 1;	// Power up test bypass
	unsigned int	test_sw        : 1;	// Diagnostics start switch
} dip_inputs_t;

// Definitions of values for dip inputs
#define	DIP_COINAGE_MODE_DIP		0
#define	DIP_COINAGE_MODE_CMOS	1

#define	DIP_COINAGE_USA_1			0
#define	DIP_COINAGE_USA_2			1
#define	DIP_COINAGE_USA_3			2
#define	DIP_COINAGE_USA_4			3
#define	DIP_COINAGE_USA_5			4
#define	DIP_COINAGE_USA_6			5
#define	DIP_COINAGE_USA_7			6
#define	DIP_COINAGE_USA_ECA		7

#define	DIP_COINAGE_FRENCH_1		8
#define	DIP_COINAGE_FRENCH_2		9
#define	DIP_COINAGE_FRENCH_3		10
#define	DIP_COINAGE_FRENCH_4		11
#define	DIP_COINAGE_FRENCH_5		12
#define	DIP_COINAGE_FRENCH_6		13
#define	DIP_COINAGE_FRENCH_7		14
#define	DIP_COINAGE_FRENCH_ECA	15

#define	DIP_COINAGE_GERMAN_1		16
#define	DIP_COINAGE_GERMAN_2		17
#define	DIP_COINAGE_GERMAN_3		18
#define	DIP_COINAGE_GERMAN_4		19
#define	DIP_COINAGE_GERMAN_5		20
#define	DIP_COINAGE_GERMAN_6		21
#define	DIP_COINAGE_GERMAN_7		22
#define	DIP_COINAGE_GERMAN_ECA	23

#define	DIP_COINAGE_UK_1			24
#define	DIP_COINAGE_UK_2			25
#define	DIP_COINAGE_UK_3			26
#define	DIP_COINAGE_UK_4			27
#define	DIP_COINAGE_UK_5			28
#define	DIP_COINAGE_UK_6			29
#define	DIP_COINAGE_UK_ECA		30
#define	DIP_COINAGE_FREEPLAY		31

#define	DIP_COUNTER_1_COUNTER_1_CLICK		0
#define	DIP_COUNTER_2_COUNTERS_1_CLICK	1
#define	DIP_COUNTER_1_COUNTER_TOTALIZER	2
#define	DIP_COUNTER_2_COUNTERS_TOTALIZER	3

#define	DIP_NO_BILL_VALIDATOR	0
#define	DIP_HAS_BILL_VALIDATOR	1

#define	DIP_GAME_MODE				0
#define	DIP_TEST_MODE				1

#define	DIP_RUN_POWER_TEST		0
#define	DIP_BYPASS_POWER_TEST	1

#define	DIP_STICK_TYPE_8_WAY		0
#define	DIP_STICK_TYPE_49_WAY	1

#define DIP_2PLAYER_PANEL		0
#define DIP_4PLAYER_PANEL		1

#define	DIP_RESOLUTION_400X256	0
#define	DIP_RESOLUTION_512X256	1
#define	DIP_RESOLUTION_512X384	2
#define	DIP_RESOLUTION_VGA		3

#define	DIP_PUTEST_RUN				0
#define	DIP_PUTEST_BYPASS			1

#define	DIP_TEST_FALSE				0
#define	DIP_TEST_TRUE				1

// Function prototypes
void switch_init(void);
void scan_switches(void);
void init_sw_handlers(void);
void *set_dcsw_handler(int, void (*)(int, int));
void *set_psw_handler(int, void (*)(int, int));
int get_player_49way_current(void);
int get_player_sw_current(void);
int get_p5_sw_current(void);
int get_p5_sw_open(void);
int get_p5_sw_close(void);
int get_dip_coin_current(void);
int get_player_sw_close(void);
int get_player_sw_open(void);
int get_dip_coin_close(void);
int get_dip_coin_open(void);
int read_dip_direct(void);
int read_coin_direct(void);
int read_player12_direct(void);
int read_player34_direct(void);
void set_49way(int);
int clear_stuck_psw(void);

#endif
