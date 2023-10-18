//
// div0hand.c
//
// $Revision: 6 $
//
#include	<stdio.h>
#include	<goose/goose.h>

#ifdef PROFILE
int ___get_count(void);
#define	GET_TIME(A)	(A) = ___get_count() * 13
#endif

extern int	fp0_time;

char	goose_div0hand_c_version[] = {"$Revision: 6 $"};

#define	SINGLE_PREC_FLOAT	16
#define	DOUBLE_PREC_FLOAT	17

#ifndef DEBUG
void	fp_div0_reboot(void);
#else
void	fp_div0_reboot(void)
{
	fprintf(stderr, "This will cause reboot in non-debug mode\r\n");
}
#endif

#ifdef PROFILE
static int dstart_time;
static int dend_time;
#endif

void div0_handler(int *r_save)
{
	int	instruction;
	int	ft_reg;
	int	reg_val;
	int	fmt;

#ifdef PROFILE
	GET_TIME(dstart_time);
#endif

	// At this point what should happen is we should decode the divide
	// instruction that caused the exception and replace the 0 value in
	// the ft register with the smallest non-zero value that can be used
	// making sure to perserve the sign of the 0 value on the new value.
	// Then we reset the EPC to the instruction that caused the exception
	// and let 'er rip.  This should be a graceful recovery from the nasty
	// divide by zero situation that can (and does) happen in 3D projections


	// Get the correct instruction if in delayed branch     
    if(r_save[CP0_CAUSE] & 0x80000000)
    {
		//fprintf(stderr, "FPU exception - instruction in bd slot\r\n");
		//fprintf(stderr, "Program Counter: %x\r\n", r_save[PC] + 4);    
		instruction = *((unsigned *)(r_save[PC] + 4));
    }
    else
    {
		instruction = *((unsigned *)r_save[PC]);
	}

    
    
	if((instruction >> 26) != 0x11)
	{
		fprintf(stderr, "div0_handler() - Not cop1 instruction\r\n");
		fp_div0_reboot();
#ifdef PROFILE
		GET_TIME(dend_time);
		fp0_time += (dend_time - dstart_time);
#endif
		return;
	}
	if((instruction & 0x3f) == 3)		// div.fmt
	{
		fmt = instruction >> 21;
		fmt &= 0x1f;

		ft_reg = instruction >> 16;
		ft_reg &= 0x1f;
		ft_reg <<= 1;
		ft_reg += FGR0;
		reg_val = r_save[ft_reg];

		if(fmt == SINGLE_PREC_FLOAT)
		{
			r_save[ft_reg] = 0x3727c5ac;	// 0.000001
			if(reg_val < 0)
			{
				r_save[ft_reg] |= 0x80000000;	// Negative
			}
		}
		else if(fmt == DOUBLE_PREC_FLOAT)
		{
			reg_val = r_save[ft_reg+1];
			r_save[ft_reg] = 0xa0b5ed8d;
			r_save[ft_reg+1] = 0x3eb0c6f7;
			if(reg_val < 0)
			{
				r_save[ft_reg+1] |= 0x80000000;
			}
		}
		else
		{
			fprintf(stderr, "div0_handler() - Unrecognized format for div instruction\r\n");
			fp_div0_reboot();
		}
	}
	else if((instruction & 0x3f) == 0x15)	// recip.fmt
	{
		fmt = instruction >> 21;
		fmt &= 0x1f;

		ft_reg = instruction >> 11;
		ft_reg &= 0x1f;
		ft_reg <<= 1;
		ft_reg += FGR0;
		reg_val = r_save[ft_reg];

		if(fmt == SINGLE_PREC_FLOAT)
		{
			r_save[ft_reg] = 0x3727c5ac;	// 0.000001
			if(reg_val < 0)
			{
				r_save[ft_reg] |= 0x80000000;	// Negative
			}
		}
		else if(fmt == DOUBLE_PREC_FLOAT)
		{
			reg_val = r_save[ft_reg+1];
			r_save[ft_reg] = 0xa0b5ed8d;
			r_save[ft_reg+1] = 0x3eb0c6f7;
			if(reg_val < 0)
			{
				r_save[ft_reg+1] |= 0x80000000;
			}
		}
		else
		{
			fprintf(stderr, "div0_handler() - Unrecognized format for recip instruction\r\n");
			fp_div0_reboot();
		}
	}
	else if((instruction & 0x3f) == 0x16)	// rsqrt.fmt
	{
		fmt = instruction >> 21;
		fmt &= 0x1f;

		ft_reg = instruction >> 11;
		ft_reg &= 0x1f;
		ft_reg <<= 1;
		ft_reg += FGR0;
		reg_val = r_save[ft_reg];

		if(fmt == SINGLE_PREC_FLOAT)
		{
			r_save[ft_reg] = 0x3727c5ac;	// 0.000001
			if(reg_val < 0)
			{
				r_save[ft_reg] |= 0x80000000;	// Negative
			}
		}
		else if(fmt == DOUBLE_PREC_FLOAT)
		{
			reg_val = r_save[ft_reg+1];
			r_save[ft_reg] = 0xa0b5ed8d;
			r_save[ft_reg+1] = 0x3eb0c6f7;
			if(reg_val < 0)
			{
				r_save[ft_reg+1] |= 0x80000000;
			}
		}
		else
		{
			fprintf(stderr, "divO_handler() - Unrecognized format for rsqrt instruction\r\n");
			fp_div0_reboot();
		}
	}
	else
	{
		fprintf(stderr, "div0_handler() - Unrecognized instruction: %x\r\n", instruction);
		fp_div0_reboot();
#ifdef PROFILE
		GET_TIME(dend_time);
		fp0_time += (dend_time - dstart_time);
#endif
		return;
	}
#ifdef PROFILE
	GET_TIME(dend_time);
	fp0_time += (dend_time - dstart_time);
#endif
}


