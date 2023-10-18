/****************************************************************************/
/*                                                                          */
/* unimhand.c - Unimplementd Operation Exception Handler.                   */
/*                                                                          */
/* Copyright (c) 1996 by Midway Video Inc.                                  */
/* All rights reserved                                                      */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* $Revision: 10 $                                                           */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<math.h>
#include	<goose/goose.h>

char	goose_unimhand_c_version[] = {"$Revision: 10 $"};
extern int	unim_time;
#ifdef PROFILE
static int	ustart_time;
static int	uend_time;
#endif

#ifdef PROFILE
int ___get_count(void);
#define	GET_TIME(A)	(A) = (___get_count() * 13)
#endif

void unimplemented_handler(int *r_save)
{
	unsigned	instruction;
	int		reg;
	int		fr_val;
	int		ft_val;
	int		fs_val;
	int		fd_val;
	int		in_bd_slot;
	char *	psz;

#ifdef PROFILE
	GET_TIME(ustart_time);
#endif

	// At this point we determine whether or not the instruction is a
	// COP1 instruction and if so we look specifically for a madd or
	// msub instruction.  If either of these instructions caused the
	// exception we attempt to determine if the exception was possibly
	// caused by an intermediate de-normalized result.  (E.G.  The result
	// of the multiply portion of the instruction resulted in a denormalized
	// result).  If so we, simply move the fr register to the fd register
	// and set the PC to the next instruction.

	if(r_save[CP0_CAUSE] & 0x80000000)
	{
		//fprintf(stderr, "FPU exception - instruction in bd slot\r\n");
		//fprintf(stderr, "Program Counter: %x\r\n", r_save[PC] + 4);    
		instruction = *((unsigned *)(r_save[PC] + 4));
		in_bd_slot = 1;
	}
	else
	{
		instruction = *((unsigned *)r_save[PC]);
		in_bd_slot = 0;
	}

	// Point [psz] at an error string & <break> from a <case> to <fprintf> the
	// error message & address, typically when its unfixable; <return> out of
	// a <case> to skip the message, typically when its fixed & trying to
	// continue
	psz = "";

	switch(instruction >> 26)
	{
		case 0x00:									// Special
		{
			if((instruction & 0x3f) != 1)
			{
				psz = "NOT AN FPU INSTRUCTION";
			}
			else
			{
				psz = "Special Instruction";
			}
			break;
		}
		case 0x11:									// COP1
		{
#if 0		// Enable/disable reporting of exception data
			// Another exception may unfortunately occur in the report
			// as <fprintf> doesn't appear to handle more extreme normalized
			// numbers; greater than about 2^30, for example, which is still
			// within the legal IEEE range of 2^-126 to 2^127
			{
				static char * sz[] = {
					"",
					"(Denormalized)",
					"(Signaling NaN)",
					"(Quiet NaN)",
					"(- Infinity)",
					"(+ Infinity)"
				};
				enum fp_type {
					NORMALIZED,
					DENORMALIZED,
					NAN_SIGNALING,
					NAN_QUIET,
					NEG_INFINITY,
					POS_INFINITY
				};
				char * szft = sz[NORMALIZED];
				char * szfs = sz[NORMALIZED];
				float ft,fs;
				float sgn;
				int exp,man;

				// Process ft value
				ft_val = r_save[((instruction>>(16-1))&(31<<1))+FGR0];
				sgn = (ft_val & 0x80000000) ? -1.0f:1.0f;
				exp = (ft_val & 0x7f800000) >> 23;
				man = (ft_val & 0x007fffff);
				if (exp == 0)
				{
					if (man == 0)
					{
						ft = 0.0f;
					}
					else
					{
						szft = sz[DENORMALIZED];
						ft = sgn*(float)man/8388608.0f/(float)pow(2,126);
					}
				}
				else if (exp == 255)
				{
					if (man == 0)
					{
						szft = sz[(sgn < 0.0f) ? NEG_INFINITY:POS_INFINITY];
						ft = sgn;
					}
					else
					{
						szft = sz[(man & 0x400000) ? NAN_SIGNALING:NAN_QUIET];
						ft = sgn;
					}
				}
				else
				{
					ft = *((float *)&ft_val);
				}

				// Process fs value
				fs_val = r_save[((instruction>>(11-1))&(31<<1))+FGR0];
				sgn = (fs_val & 0x80000000) ? -1.0f:1.0f;
				exp = (fs_val & 0x7f800000) >> 23;
				man = (fs_val & 0x007fffff);
				if (exp == 0)
				{
					if (man == 0)
					{
						fs = 0.0f;
					}
					else
					{
						szfs = sz[DENORMALIZED];
						fs = sgn*(float)man/8388608.0f/pow(2,126);
					}
				}
				else if (exp == 255)
				{
					if (man == 0)
					{
						szfs = sz[(sgn < 0.0f) ? NEG_INFINITY:POS_INFINITY];
						fs = sgn;
					}
					else
					{
						szfs = sz[(man & 0x400000) ? NAN_SIGNALING:NAN_QUIET];
						fs = sgn;
					}
				}
				else
				{
					fs = *((float *)&fs_val);
				}

				fprintf(stderr,
					"COP1 exception:\n"
					"  of:%d\n"
					"  ft:%d = 0x%08X = %e  %s\n"
					"  fs:%d = 0x%08X = %e  %s\n"
					"  fd:%d\n"
					"  op:%d\n",
					((instruction>>21)&31),
					((instruction>>16)&31),ft_val,ft,szft,
					((instruction>>11)&31),fs_val,fs,szfs,
					((instruction>>6)&31),
					(instruction&31)
				);
			}
#endif // enable/disable reporting of exception data

			// Check operand format to determine how to handle this
			if(((instruction >> 21) & 0x1f) == 16)	// .S single precision?
			{
				reg = instruction & 0x3f;

				switch(reg)
				{
					case 0:							// ADD.S
					case 1:							// SUB.S
					case 2:							// MUL.S
					case 3:							// DIV.S
					{
						static char * sz[] = {
							"unable to fix ADD.S qNaN",
							"unable to fix SUB.S qNaN",
							"unable to fix MUL.S qNaN",
							"unable to fix DIV.S qNaN"
						};

						// Get ft and fs indecies, values
						fr_val = ((instruction>>(16-1))&(31<<1))+FGR0;
						fd_val = ((instruction>>(11-1))&(31<<1))+FGR0;
						ft_val = r_save[fr_val];
						fs_val = r_save[fd_val];

						// Continue only if ft and fs are not qNaN
						if (((ft_val >> 23) & 0xff) != 0xff &&
							((fs_val >> 23) & 0xff) != 0xff)
						{
							if (((ft_val >> 23) & 0xff) == 0)
							{
								// Value in ft is denormalized or zero
								// Make it a same-sign 0
								r_save[fr_val] = ft_val & 0x80000000;
							}
							if (((fs_val >> 23) & 0xff) == 0)
							{
								// Value in fs is denormalized or zero
								// Make it a same-sign 0
								r_save[fd_val] = fs_val & 0x80000000;
							}

							// Go try again
#ifdef PROFILE
							GET_TIME(uend_time);
							unim_time += (uend_time - ustart_time);
#endif
							return;
						}

						// Value in ft and/or fs is quiet NaN
						psz = sz[reg];
						break;
					}
					case 4:							// SQRT.S
					case 5:							// ABS.S
					case 7:							// NEG.S
					case 12:						// ROUND.W.S
					case 13:						// TRUNC.W.S
					case 14:						// CEIL.W.S
					case 15:						// FLOOR.W.S
					case 33:						// CVT.D.S
					case 36:						// CVT.W.S
					{
						fr_val = ((instruction>>(11-1))&(31<<1))+FGR0;
						fs_val = r_save[fr_val];

						if (((fs_val >> 23) & 0xff) == 0)
						{
							// Value in fs is denormalized
							// Make it a same-sign 0 & go try again
							r_save[fr_val] = fs_val & 0x80000000;
#ifdef PROFILE
							GET_TIME(uend_time);
							unim_time += (uend_time - ustart_time);
#endif
							return;
						}

						// Value in fs is quiet NaN
						if (reg ==  4) psz = "unable to fix SQRT.S qNaN";
						else
						if (reg ==  5) psz = "unable to fix ABS.S qNaN";
						else
						if (reg ==  7) psz = "unable to fix NEG.S qNaN";
						else
						if (reg == 12) psz = "unable to fix ROUND.W.S qNaN";
						else
						if (reg == 13) psz = "unable to fix TRUNC.W.S qNaN";
						else
						if (reg == 14) psz = "unable to fix CEIL.W.S qNaN";
						else
						if (reg == 15) psz = "unable to fix FLOOR.W.S qNaN";
						else
						if (reg == 33) psz = "unable to fix CVT.D.S qNaN";
						else
						if (reg == 36) psz = "unable to fix CVT.W.S qNaN";

						break;
					}
					case 32:						// CVT.S.S
					{
						if(!in_bd_slot)
						{
							// Not in branch delay slot
							// Copy fs to fd & skip the instruction
							r_save[((instruction>>(6-1))&(31<<1))+FGR0] =
								r_save[((instruction>>(11-1))&(31<<1))+FGR0];

							r_save[PC] += 4;
#ifdef PROFILE
							GET_TIME(uend_time);
							unim_time += (uend_time - ustart_time);
#endif
							return;
						}

						// In branch delay slot
						psz = "unable to fix CVT.S.S in a BDS";
						break;
					}
					default:
					{
						psz = "reserved value in COP1 .S function field";
						break;
					}
				}
			}
			else
			if(((instruction >> 21) & 0x1f) == 17)	// .D double precision?
			{
				psz = "unable to fix COP1 .D format";
			}
			else
			if(((instruction >> 21) & 0x1f) == 20)	// .W fixed-point?
			{
				psz = "unable to fix COP1 .W format";
			}
			else
			{
				psz = "reserved value in COP1 format field";
			}
			break;
		}
		case 0x13:									// COP1X
		{
			reg = instruction & 0x3f;

			if (reg == 0x20 ||						// MADD.S
				reg == 0x28 ||						// MSUB.S
				reg == 0x30 ||						// NMADD.S
				reg == 0x38)						// NMSUB.S
			{
				// Get ft value, exponent
				fr_val = r_save[((instruction>>(16-1))&(31<<1))+FGR0];
				ft_val = ((fr_val >> 23) & 0xff) - 127;

				// Get fs value, exponent
				fd_val = r_save[((instruction>>(11-1))&(31<<1))+FGR0];
				fs_val = ((fd_val >> 23) & 0xff) - 127;

				if(!in_bd_slot)
				{
					// Not in branch delay slot
					// If multiply product would be denormalized, copy to the
					// destination register the value that would have been
					// added/subtracted; otherwise abort with no clue
					ft_val += fs_val;
					if(ft_val <= -127)
					{
						// Get fr value
						fr_val = r_save[((instruction>>(21-1))&(31<<1))+FGR0];

						// Flip the sign if MSUB.S or  NMADD.S
						if (reg == 0x28 || reg == 0x30) fr_val ^= 0x80000000;

						// Set fd value
						r_save[((instruction>>(6-1))&(31<<1))+FGR0] = fr_val;

						r_save[PC] += 4;
#ifdef PROFILE
						GET_TIME(uend_time);
						unim_time += (uend_time - ustart_time);
#endif
						return;
					}

					// No clue
					if (reg == 0x20) psz = "unable to fix MADD.S";
					else
					if (reg == 0x28) psz = "unable to fix MSUB.S";
					else
					if (reg == 0x30) psz = "unable to fix NMADD.S";
					else
					if (reg == 0x38) psz = "unable to fix NMSUB.S";
				}
                else
                {
					// In branch delay slot
					// If fs exp <= ft exp & abs(fs) != 0 set fs to same-sign
					// 0 otherwise set ft to same-sign 0
					if(fs_val <= ft_val && (fd_val & 0x7fffffff))
					{
						// Set fs index
						reg = ((instruction>>(11-1))&(31<<1))+FGR0;
						fr_val = r_save[reg] & 0x80000000;
					}
					else
					{
						// Set ft index
						reg = ((instruction>>(16-1))&(31<<1))+FGR0;
						fr_val = r_save[reg] & 0x80000000;
					}
					r_save[reg] = fr_val;

					// NOW when the eret instruction is executed, the branch
					// instruction is returned to and the fp instruction in
					// the bd slot that caused this mess gets re-executed.  If
					// the exception gets thrown again, the other register
					// (s or t) will get zero'd and the whole mess will occur
					// again.  If both regs get set to 0, the intermediate
					// result can NOT cause a denormalized result and therefore
					// we should go on.
#ifdef PROFILE
					GET_TIME(uend_time);
					unim_time += (uend_time - ustart_time);
#endif
					return;
				}
			}
			else if (reg == 0x21)					// MADD.D
			{
				psz = "unable to fix MADD.D";
			}
			else if (reg == 0x29)					// MSUB.D
			{
				psz = "unable to fix MSUB.D";
			}
			else if (reg == 0x31)					// NMADD.D
			{
				psz = "unable to fix NMADD.D";
			}
			else if (reg == 0x39)					// NMSUB.D
			{
				psz = "unable to fix NMSUB.D";
			}
			else if (reg == 0x00)					// LWXC1
			{
				psz = "unable to fix LWXC1";
			}
			else if (reg == 0x01)					// LDXC1
			{
				psz = "unable to fix LDXC1";
			}
			else if (reg == 0x08)					// SWXC1
			{
				psz = "unable to fix SWXC1";
			}
			else if (reg == 0x09)					// SDXC1
			{
				psz = "unable to fix SDXC1";
			}
			else if (reg == 0x0f)					// PREFIX
			{
				psz = "unable to fix PREFIX";
			}
			break;
		}
		case 0x31:									// LWC1
		{
			psz = "LWC1 Instruction";
			break;
		}
		case 0x35:									// LDC1
		{
			psz = "LDC1 Instruction";
			break;
		}
		case 0x39:									// SWC1
		{
			psz = "SWC1 Instruction";
			break;
		}
		case 0x3d:									// SDC1
		{
			psz = "SDC1 Instruction";
			break;
		}
		default:
		{
			psz = "NOT FPU Instruction";
			break;
		}
	}
	fprintf(stderr, "unimplemented_handler() - %s; addr: %08X\r\n",
		psz,r_save[PC]+((in_bd_slot)?4:0));
#ifndef DEBUG
	// If not in debugging mode - wait for the watchdog to catch us
	while(1) ;
#endif
#ifdef PROFILE
	GET_TIME(uend_time);
	unim_time += (uend_time - ustart_time);
#endif
}
