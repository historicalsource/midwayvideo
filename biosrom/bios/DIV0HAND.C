//
// div0hand.c
//
// $Revision: 5 $
//
#include	<stdio.h>
#include	<ioctl.h>

#if ((PHOENIX_SYS & VEGAS))
extern int	(*user_handler[])(int, unsigned int *);
#endif

#define	SINGLE_PREC_FLOAT	16
#define	DOUBLE_PREC_FLOAT	17

#define	PC		68
#define	FGR0	134

#if (PHOENIX_SYS & VEGAS)
void unhandled_exception(char *);
#endif
#if (PHOENIX_SYS & SEATTLE)
void show_exc_info(char *, int, unsigned int *);
#endif

void fpu_div0_handler(int cause, unsigned int *r_save)
{
	int	instruction;
	int	ft_reg;
	int	reg_val;
	int	fmt;

	// At this point what should happen is we should decode the divide
	// instruction that caused the exception and replace the 0 value in
	// the ft register with the smallest non-zero value that can be used
	// making sure to perserve the sign of the 0 value on the new value.
	// Then we reset the EPC to the instruction that caused the exception
	// and let 'er rip.  This should be a graceful recovery from the nasty
	// divide by zero situation that can (and does) happen in 3D projections

	if(cause & 0x80000000)
	{
		instruction = *((int *)(r_save[PC] + 4));
	}
	else
	{
		instruction = *((int *)r_save[PC]);
	}
	if((instruction >> 26) != 0x11)
	{
#if ((PHOENIX_SYS & VEGAS))
		if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM])
		{
			if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM](cause, r_save))
			{
				unhandled_exception("FPE - DIV0 - Unrecognized instruction");
			}
			return;
		}
		unhandled_exception("FPE - DIV0 - Unrecognized instruction");
#endif
#if (PHOENIX_SYS & SEATTLE)
		show_exc_inf("FPE - DIV0 - Unrecognized instruction", cause, r_save);
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
#if ((PHOENIX_SYS & VEGAS))
			if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM])
			{
				if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM](cause, r_save))
				{
					unhandled_exception("FPE - DIV0 - Unrecognized format on DIV instruction");
				}
				return;
			}
			unhandled_exception("FPE - DIV0 - Unrecognized format on DIV instruction");
#endif
#if (PHOENIX_SYS & SEATTLE)
			show_exc_inf("FPE - DIV0 - Unrecognized format on DIV instruction", cause, r_save);
#endif
			return;
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
#if ((PHOENIX_SYS & VEGAS))
			if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM])
			{
				if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM](cause, r_save))
				{
					unhandled_exception("FPE - DIV0 - Unrecognized format on RECIP instruction");
				}
				return;
			}
			unhandled_exception("FPE - DIV0 - Unrecognized format on RECIP instruction");
#endif
#if (PHOENIX_SYS & SEATTLE)
			show_exc_inf("FPE - DIV0 - Unrecognized format on RECIP instruction", cause, r_save);
#endif
			return;
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
#if ((PHOENIX_SYS & VEGAS))
			if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM])
			{
				if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM](cause, r_save))
				{
					unhandled_exception("FPE - DIV0 - Unrecognized format on RSQRT instruction");
				}
				return;
			}
			unhandled_exception("FPE - DIV0 - Unrecognized format on RSQRT instruction");
#endif
#if (PHOENIX_SYS & SEATTLE)
			show_exc_inf("FPE - DIV0 - Unrecognized format on RSQRT instruction", cause, r_save);
#endif
			return;
		}
	}
	else
	{
#if ((PHOENIX_SYS & VEGAS))
		if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM])
		{
			if(user_handler[COP1_DIVIDE_BY_ZERO_HANDLER_NUM](cause, r_save))
			{
				unhandled_exception("FPE - DIV0 - Unrecognized instruction");
			}
			return;
		}
		unhandled_exception("FPE - DIV0 - Unrecognized instruction");
#endif
#if (PHOENIX_SYS & SEATTLE)
		show_exc_inf("FPE - DIV0 - Unrecognized instruction", cause, r_save);
#endif
	}
}
