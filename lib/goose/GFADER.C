/****************************************************************************/
/*                                                                          */
/* gfader.c - Gamma Table Fader Source File                                 */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 9 $                                                             */
/*                                                                          */
/****************************************************************************/
#ifndef VEGAS
#include	<glide/glide.h>
#else
#include	<glide.h>
#endif
#include	<goose/gfader.h>

char	goose_gfader_c_version[] = {"$Revision: 9 $"};

#define	NULL	0

#if defined(BANSHEE)
#define	GAMMA_ENTRIES		256
#define	GAMMA_TABLE_SIZE	256
#else
#define	GAMMA_ENTRIES		32
#define	GAMMA_TABLE_SIZE	33
#endif

static float					current_intensity = 1.0f;
static float					target_intensity = 1.0f;
static float					intensity_delta = 0.0f;
static int						intensity_ticks = 0;
static void						(*callback_func)(void) = (void (*)(void))0;
#ifdef VEGAS
static unsigned long			g_gamma[GAMMA_TABLE_SIZE];
#endif

void load_gamma_table(void)
{
	int	i;
#ifndef VEGAS
	int	g;
	int	val;
#endif
	float	gamma;
	float	gamma_delta;

	// Are we at the target intensity ?
	if(current_intensity != target_intensity)
	{
		// Have the ticks expired ?
		// This is here to clamp the intensity at the target intensity
		// if floating point rounding error occurs and the target intensity
		// is not reached in the number of ticks specified.
		if(!intensity_ticks)
		{
			// Yes - set to target intensity
			current_intensity = target_intensity;
		}
		else
		{
			// Nope - Adjust the current intensity
			current_intensity += intensity_delta;
		}

		// Calculate the delta for the gamma table
		gamma_delta = current_intensity / (float)GAMMA_ENTRIES;

		// Generate the gamma table
		gamma = 0.0f;
		for(i = 0; i < GAMMA_TABLE_SIZE; i++)
		{
#ifndef VEGAS
			g = (int)(gamma * 255.0f);
			val = (i << 24);
			val |= (((unsigned)g << 16) |	((unsigned)g << 8) |	(unsigned)g);
			*((volatile int *)0xa8000228) = val;
#else
			g_gamma[i] = (unsigned long)(gamma * 255.0f);
#endif
			gamma += gamma_delta;
		}

#ifdef VEGAS
		grLoadGammaTable(GAMMA_TABLE_SIZE, g_gamma, g_gamma, g_gamma);
#endif

		// Do we have ticks left ?
		if(intensity_ticks)
		{
			// Decrement the tick count
			--intensity_ticks;
 
			// Is there a callback function and intensity is done ?
			if(callback_func && !intensity_ticks)
			{
				// Yes - call the function
				callback_func();
			}
		}
	}
}

void fade(float target, int ticks, void (*callback)(void))
{
	if(target_intensity != target)
	{
		target_intensity = target;
		intensity_delta = (target_intensity - current_intensity) / (float)ticks;
		intensity_ticks = ticks;
		callback_func = callback;
	}

} /* fade */




/* April 14th, 98 P.Giokaris
   This function will pass back 0 if fade value is at full ; otherwise,
   it will return a 1 */
int fade_at_full ( void )
{

	if ( target_intensity == 1.0f )
		return 0;
	else
		return 1;

} /* fade_at_full */


/*
	JAB - 1/6/99
	Returns TRUE (1) if the fade is complete, or FALSE (0) if it's still in progress
*/
int fade_complete(void)
{
	return(current_intensity == target_intensity);
}	


void normal_screen(void)
{
	fade(1.0f, 1, NULL);
}

void black_screen(void)
{
	fade(0.0f, 1, NULL);
}

// loads the gamma table with values between FROM and TO (note FROM & TO must be between 0.0 and 1.0, 1.0 being pure white)
// Also NOTE, this routine is not associated with the system above, keep track of your use of it youself, the system above wont.
#ifdef VEGAS
static unsigned long	sg[GAMMA_TABLE_SIZE];
#endif

void specific_gamma_load(float from, float to)
{
	int 	i;
#ifndef VEGAS
	int	g;
	int	val;
#endif
	float	gamma;
	float	gamma_delta = 0.0f;

	// Calculate the delta for the gamma table
	if(to != from)
	{
		gamma_delta = (to-from) / (float)GAMMA_ENTRIES;
	}

	// Generate the gamma table
	gamma = from;
	for(i = 0; i < GAMMA_TABLE_SIZE; i++)
	{
#ifndef VEGAS
	  	g = (int)(gamma * 255.0f);
		val = (i << 24);
		val |= (((unsigned)g << 16) |	((unsigned)g << 8) |	(unsigned)g);
		*((volatile int *)0xa8000228) = val;
#else
		sg[i] = (unsigned long)(gamma * 255.0f);
#endif
		gamma += gamma_delta;
	}
#ifdef VEGAS
	grLoadGammaTable(GAMMA_TABLE_SIZE, sg, sg, sg);
#endif
}
