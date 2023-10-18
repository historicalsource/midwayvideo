//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 3 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>

static void led_delay(int amount)
{
	amount *= TICKS_100HZ;
	reset_count();
	do
	{
		;
	} while(get_count_reg() < amount);
}

#if (PHOENIX_SYS & SA1)
static int	led_seg_table[] = {
0x3f,	// 0
0x06,	// 1
0x5b,	// 2
0x4f,	// 3
0x66,	// 4
0x6d,	// 5
0x7d,	// 6
0x07,	// 7
0x7f,	// 8
0x67,	// 9
0x77,	// A
0x7c,	// b
0x39,	// c
0x5e,	// d
0x79,	// e
0x71	// f
};

void led_write(int val, int fatal)
{
	int	i;

	do
	{
		// First blank the display and flash the DP a few times
		*((volatile int *)LED_ADDR) = 0;
		for(i = 0; i < 10; i++)
		{
			*((volatile int *)LED_ADDR) ^= 0x80;
			led_delay(25);
		}
		for(i = 28; i >= 0; i -= 4)
		{
			*((volatile int *)LED_ADDR) = 0;
			led_delay(25);
			*((volatile int *)LED_ADDR) = led_seg_table[(val >> i) & 0xf];
			led_delay(100);
		}
	} while(fatal);
}

int ledwrite(register struct iocntb *io, char *buf, int count)
{
	int	save_count = count;

	while(count--)
	{
		*((volatile int *)LED_ADDR) = 0;
		led_delay(10);
		*((volatile int *)LED_ADDR) = led_seg_table[(*buf >> 4) & 0xf];
		led_delay(25);
		*((volatile int *)LED_ADDR) = 0;
		led_delay(10);
		*((volatile int *)LED_ADDR) = led_seg_table[*buf & 0xf];
		led_delay(75);
		++buf;
	}
	return(save_count);
}
#elif (PHOENIX_SYS & VEGAS)
static int	led_seg_table[] = {
0xfc,	// 0
0x60,	// 1
0xda,	// 2
0xf2,	// 3
0x66,	// 4
0xb6,	// 5
0xbe,	// 6
0xe0,	// 7
0xfe,	// 8
0xe6,	// 9
0xee,	// A
0x3e,	// b
0x9c,	// c
0x7a,	// d
0x9e,	// e
0x8e	// f
};

void led_write(int val, int fatal)
{
	int	i;

	do
	{
		// First blank the display and flash the DP a few times
		*((volatile char *)LED_ADDR) = ~0;
		for(i = 0; i < 10; i++)
		{
			*((volatile char *)LED_ADDR) = ~1;
			led_delay(13);
			*((volatile char *)LED_ADDR) = ~0;
			led_delay(12);
		}
		for(i = 28; i >= 0; i -= 4)
		{
			*((volatile char *)LED_ADDR) = ~0;
			led_delay(15);
			*((volatile char *)LED_ADDR) = ~led_seg_table[(val >> i) & 0xf];
			led_delay(60);
		}
	} while(fatal);
}

int ledwrite(register struct iocntb *io, char *buf, int count)
{
	int	save_count = count;

	while(count--)
	{
		*((volatile char *)LED_ADDR) = ~0;
		led_delay(10);
		*((volatile char *)LED_ADDR) = ~led_seg_table[(*buf >> 4) & 0xf];
		led_delay(25);
		*((volatile char *)LED_ADDR) = ~0;
		led_delay(10);
		*((volatile char *)LED_ADDR) = ~led_seg_table[*buf & 0xf];
		led_delay(75);
		++buf;
	}
	return(save_count);
}
#elif (PHOENIX_SYS & SEATTLE)
int led_write(int val, int fatal)
{
	int	i;

	// Turn off all of the LEDs
	*((volatile int *)LED_ADDR) = 0x7;
	do
	{
		for(i = 0; i < val; i++)
		{
			// Turn on LED 1 (red)
			*((volatile int *)LED_ADDR) &= 0x6;

			// Wait 250 ms
			led_delay(25);

			// Turn off LED (red)
			*((volatile int *)LED_ADDR) |= 0x1;

			// Wait 250 ms
			led_delay(25);
		}

		// Resend code after 1 second
		led_delay(100);

	// If fatal error - just keep doing it forever
	} while(fatal);
}

int ledwrite(register struct iocntb *io, char *buf, int count)
{
	int	save_count;

	while(count--)
	{
		led_write((int)*buf, 0);
		++buf;
	}
	return(save_count);
}
#elif (PHOENIX_SYS & FLAGSTAFF)
#else
#endif

int ledinit(void)
{
	int	i;

#if (PHOENIX_SYS & SA1)
	// Turn off all of the LEDs
	*((volatile int *)LED_ADDR) = 0;

	// Wait 1/10th second
	led_delay(10);

	// Turn on all of the LEDs
	*((volatile int *)LED_ADDR) = 0xff;

	// Wait 1/10th second
	led_delay(10);

	// Turn off all of the LEDs
	*((volatile int *)LED_ADDR) = 0;

	// Wait 1/10th second
	led_delay(10);

	// Turn on each LED 1 at a time
	for(i = 1; i < 0x100; i <<= 1)
	{
		// Turn on an LED
		*((volatile int *)LED_ADDR) = i;

		// Wait 1/20th second
		led_delay(5);
	}

	// Turn off all of the LEDs
	*((volatile int *)LED_ADDR) = 0;
#elif (PHOENIX_SYS & VEGAS)
	// Turn off all of the LEDs
	*((volatile char *)LED_ADDR) = ~0;

	// Wait 1/10th second
	led_delay(1);

	// Turn on all of the LEDs
	*((volatile char *)LED_ADDR) = (char)~0xff;

	// Wait 1/10th second
	led_delay(1);

	// Turn off all of the LEDs
	*((volatile char *)LED_ADDR) = ~0;

	// Wait 1/10th second
	led_delay(1);

	// Turn on each LED 1 at a time
	for(i = 1; i < 0x100; i <<= 1)
	{
		// Turn on an LED
		*((volatile char *)LED_ADDR) = ~i;

		// Wait 1/20th second
		led_delay(1);
	}

	// Turn off all of the LEDs
	*((volatile char *)LED_ADDR) = ~0;
#elif (PHOENIX_SYS & SEATTLE)
	// Turn all LEDs off
	*((volatile int *)LED_ADDR) = 0x7;

	// Wait 1/4 second
	led_delay(25);

	// Turn all LEDs on
	*((volatile int *)LED_ADDR) = 0;

	// Wait 1/4 second
	led_delay(25);

	// Turn all LEDs off
	*((volatile int *)LED_ADDR) = 0x7;

	// Wait 1/4 second
	led_delay(25);

	// Individually Turn on each LED starting at LED2
	for(i = 0; i < 3; i++)
	{
		// Turn on 1 LED - others off
		*((volatile int *)LED_ADDR) = (0x1b >> i);

		// Wait 1/4 second
		led_delay(25);
	}

	// Turn all LEDs off
	*((volatile int *)LED_ADDR) = 0x7;

	// Wait 1/4 second
	led_delay(25);
#elif (PHOENIX_SYS & FLAGSTAFF)
#error led.c - FLAGSTAFF NOT SUPPORTED YET
#endif

	// Return OK
	return(1);
}

		
int ledopen(register struct iocntb *io)
{
#if (PHOENIX_SYS & SA1)
	*((volatile int *)LED_ADDR) = 0;
#elif (PHOENIX_SYS & VEGAS)
	*((volatile char *)LED_ADDR) = 0xff;
#elif (PHOENIX_SYS & SEATTLE)
	*((volatile int *)LED_ADDR) = 0x7;
#endif
	return(0);
}

int ledclose(register struct iocntb *io)
{
#if (PHOENIX_SYS & SA1)
	*((volatile int *)LED_ADDR) = 0;
#elif (PHOENIX_SYS & VEGAS)
	*((volatile char *)LED_ADDR) = 0xff;
#elif (PHOENIX_SYS & SEATTLE)
	*((volatile int *)LED_ADDR) = 0x7;
#endif
	return(0);
}


int ledread(register struct iocntb *io, char *buf, int count)
{
	*buf = (char)*((volatile int *)LED_ADDR);
	return(1);
}
