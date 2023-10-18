/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    MAIN.C                                                            บ
บ Author:  Jack Miller                                                       บ
บ Created: 11-Sep-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ     P.O.S.T. main module.                                                  บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  11Sep97 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <compiler.h>
#include <stdio.h>
#include <glide.h>
#include <machine/seattle.h>
#include <machine/gt64010.h>
#include <machine/gprintf.h>
#include <machine/colors.h>
#include "post.h"
#include "version.h"


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                       Structure & Literal Defintions                       ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
typedef struct {
  int   x1;
  int   y1;
  int   x2;
  int   y2;
} BOARD;

typedef struct {
  int   x;
  int   y;
  int   w;
  int   h;
  int   grp;
  int   grpid;
  int   failcnt;
  int   state;
  char  *desc;
} CHIP_DISP;

enum {
  TS_NORM = 0,
  TS_TEST,
  TS_PASS,
  TS_FAIL
};

enum {
  CHIP_NONE = 0,
  CHIP_FBI,
  CHIP_TMU,
  CHIP_DRAM,
  CHIP_BOOT,
  CHIP_EXP,
  CHIP_IO,
  CHIP_PIC,
  CHIP_HD,
  CHIP_SCPU,
  CHIP_SROM,
  CHIP_SDRAM,
  CHIP_SSRAM,
  CHIP_LAST,
};


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                               External Data                                ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
extern mxU16    *_screenPtr;

extern int      sysMemErrors[4];
extern int      fbiMemErrors[2][4];
extern int      tmuMemErrors[2][2][4];

extern _POST_VERSION  _ver;


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                             External Functions                             ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
extern void     installhandler(int, mxU32 *func);
extern void     disableint(mxU32 imask);
extern mxBool   getinput(mxU32 bmask, mxU32 smask);
extern void     putmsg(char *str);


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                Global Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
volatile _ASIC_REGS * const _ioasic = (_ASIC_REGS *)PHYS_TO_K1(IO_ASIC_BASE);
mxVU32  __systicks;
mxBool  loopmode;
mxU32   loopcount = 1;


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                 Local Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
static GrVertex pta, ptb;

static BOARD board [] = {
{ 220,   0,   0,   0},  // Top
{   0,   0,   0, 320},  // Left
{ 220, 320,   0, 320},  // Bottom
{ 220,   0, 220, 169},  // Right 1

{ 220, 169, 214, 169},  //
{ 214, 168, 214, 179},  // Jamma indentation
{ 220, 179, 214, 179},  //

{ 220, 179, 220, 270},  // Right 2

{ 220, 270, 214, 270},  //
{ 214, 269, 214, 280},  // Jamma indentation
{ 220, 280, 214, 280},  //

{ 220, 280, 220, 320}   // Right 3
};

#define BOARD_SZ    (sizeof(board)/sizeof(board[0]))


static BOARD heatsink [] = {
{ 132, 295, 121, 295},
{ 155, 295, 144, 295},
{ 132, 315, 122, 315},
{ 155, 315, 145, 315},
{ 145, 304, 132, 304},
{ 145, 306, 131, 306},
{ 122, 295, 122, 315},
{ 132, 295, 132, 304},
{ 132, 306, 132, 315},
{ 145, 295, 145, 304},
{ 145, 306, 145, 315},
{ 155, 295, 155, 315},

{ 171, 295, 160, 295},
{ 194, 295, 183, 295},
{ 171, 315, 161, 315},
{ 194, 315, 184, 315},
{ 184, 304, 171, 304},
{ 184, 306, 170, 306},
{ 161, 295, 161, 315},
{ 171, 295, 171, 304},
{ 171, 306, 171, 315},
{ 184, 295, 184, 304},
{ 184, 306, 184, 315},
{ 194, 295, 194, 315}
};

#define HEATSINK_SZ    (sizeof(heatsink)/sizeof(heatsink[0]))


static CHIP_DISP chip_disp [] = {
{  87, 235,  25,  25, CHIP_FBI,   0, 0, TS_NORM, "U17  3DFX FBI"   },
{  36, 234,  20,   8, CHIP_FBI,   1, 0, TS_NORM, "U75  FBI RAM"    },
{  36, 248,  20,   8, CHIP_FBI,   2, 0, TS_NORM, "U76  FBI RAM"    },
{  11, 234,  20,   8, CHIP_FBI,   3, 0, TS_NORM, "U77  FBI RAM"    },
{  11, 248,  20,   8, CHIP_FBI,   4, 0, TS_NORM, "U74  FBI RAM"    },
{  91, 276,  21,  21, CHIP_TMU,   0, 0, TS_NORM, "U87  3DFX TMU"   },
{  11, 262,  20,   8, CHIP_TMU,   1, 0, TS_NORM, "U80  TMU RAM"    },
{  36, 262,  20,   8, CHIP_TMU,   2, 0, TS_NORM, "U81  TMU RAM"    },
{  36, 304,  20,   8, CHIP_TMU,   3, 0, TS_NORM, "U79  TMU RAM"    },
{  11, 304,  20,   8, CHIP_TMU,   4, 0, TS_NORM, "U78  TMU RAM"    },
{  11, 276,  20,   8, CHIP_TMU,   5, 0, TS_NORM, "U84  TMU RAM"    },
{  36, 276,  20,   8, CHIP_TMU,   6, 0, TS_NORM, "U83  TMU RAM"    },
{  36, 290,  20,   8, CHIP_TMU,   7, 0, TS_NORM, "U82  TMU RAM"    },
{  11, 290,  20,   8, CHIP_TMU,   8, 0, TS_NORM, "U85  TMU RAM"    },
{  31,  28,   9,  21, CHIP_DRAM,  0, 0, TS_NORM, "U43  System RAM" },
{  17,  28,   9,  21, CHIP_DRAM,  1, 0, TS_NORM, "U44  System RAM" },
{  45,  28,   9,  21, CHIP_DRAM,  2, 0, TS_NORM, "U46  System RAM" },
{  59,  28,   9,  21, CHIP_DRAM,  3, 0, TS_NORM, "U45  System RAM" },
{ 180,  29,  14,  31, CHIP_BOOT,  0, 0, TS_NORM, "U32  EPROM 1"    },
{ 180,  65,  14,  31, CHIP_EXP,   0, 0, TS_NORM, "U33  EPROM 2"    },
{ 151, 180,  22,  22, CHIP_IO,    0, 0, TS_NORM, "U20  I/O ASIC"   },
{ 180, 100,  14,  27, CHIP_PIC,   0, 0, TS_NORM, "U96  PIC"        },
{  35, 172,  10,  15, CHIP_HD,    0, 0, TS_NORM, "U6   IDE H.D."   },
{ 146,  70,  10,  15, CHIP_SCPU,  0, 0, TS_NORM, "U65  Sound CPU"  },
{ 145, 136,  19,  19, CHIP_SCPU,  1, 0, TS_NORM, "U94  Sound DSP"  },
{ 147,  94,  14,  27, CHIP_SROM,  0, 0, TS_NORM, "U95  Sound EPROM"},
{ 164,  82,   8,  21, CHIP_SDRAM, 0, 0, TS_NORM, "U101 Sound DRAM" },
{ 124, 138,  14,   6, CHIP_SSRAM, 0, 0, TS_NORM, "U62  Sound SRAM" },
{ 124, 151,  14,   6, CHIP_SSRAM, 1, 0, TS_NORM, "U63  Sound SRAM" },
{ 124, 164,  14,   6, CHIP_SSRAM, 2, 0, TS_NORM, "U64  Sound SRAM" },
{  22,  80,  37,  37, CHIP_NONE,  0, 0, TS_NORM, "U100 System CPU" },
{  87, 175,  22,  22, CHIP_NONE,  1, 0, TS_NORM, "U86  Galileo"    },
{ 125, 112,  12,  12, CHIP_NONE,  2, 0, TS_NORM, "U50  Reset PLD"  },
{ 139,  32,  10,  15, CHIP_NONE,  3, 0, TS_NORM, "U89  Ctrl PLD 1" },
{ 125,  85,  12,  12, CHIP_NONE,  4, 0, TS_NORM, "U60  Ctrl PLD 2" },
{ 159, 236,  19,  19, CHIP_NONE,  5, 0, TS_NORM, "U18  RAM DAC"    },
};

#define CHIP_DISP_SZ    (sizeof(chip_disp)/sizeof(chip_disp[0]))


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                              Local Functions                               ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
mxBool          initVideoCard(void);
void            testdelay(int delay);
void            setChipState(int group, int groupid, int state);
void            drawPostScreen(int group);
void            drawBoard(void);
void            drawRectangle(mxU32 x, mxU32 y, mxU32 w, mxU32 h);
void            drawFilledRectangle(mxU32 x, mxU32 y, mxU32 w, mxU32 h, mxU32 color);
mxU32           doSysTicks(void);
mxU32           doDiskInterrupt(void);


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                    Code                                    ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/

void __main(void) {}

void main(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    int     ix, jx, kx;

    static  mxBool firstTime;


    putmsg("\r\n");
    putmsg("Starting the P.O.S.T.\r\n");

    loopmode = ((~_ioasic->asicDipSw & 0x0080) == 0x0080) ? TRUE : FALSE;

#if BIOS == JACK_BIOS
    *((mxVU32 *)PHYS_TO_K1(RESET_REG)) &= ~(ENABLE_3DFX);
    *((mxVU32 *)PHYS_TO_K1(RESET_REG)) |= (ENABLE_3DFX);
#endif

    do {
        firstTime = TRUE;

      /* Initialize the chip state */
        for (ix = 0; ix < CHIP_DISP_SZ; ix++) chip_disp[ix].state = TS_NORM;

        for (ix = CHIP_FBI; ix < CHIP_LAST; ix++) {

            setChipState(ix, -1, TS_TEST);

          /* Only draw the board after the 3dfx tests are run */
            if (ix >= CHIP_DRAM) {
              /* One time initialization stuff */
                if (firstTime) {
                    *((mxVU32 *)PHYS_TO_K1(CONTROL_LED_IO_REG)) = (CTRL_LED_GRN|CTRL_LED_YEL|CTRL_LED_RED);

                    firstTime = FALSE;

                    *((mxVU32 *)PHYS_TO_K1(RESET_REG)) &= ~(ENABLE_3DFX|ENABLE_PCI_IDE);
                    *((mxVU32 *)PHYS_TO_K1(RESET_REG)) |= (ENABLE_3DFX|ENABLE_PCI_IDE);

                    if (!initVideoCard()) return;


#if BIOS == MIKE_BIOS
                    installhandler(0, (mxU32 *)&doSysTicks);
                    installhandler(2, (mxU32 *)&doDiskInterrupt);
                    disableint(DISK_INT);
#endif
                }

                grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);

                drawPostScreen(ix);

                grBufferSwap(1);
                while (grBufferNumPending());
            }

            if (getinput(0, 0x0064)) {  // P1/P2 START or SERVICE
                setChipState(ix, -1, TS_PASS);
                goto bypass;
            }

            switch (ix) {
              case CHIP_FBI:
                if (!testFBIMem()) {
                    setChipState(ix, 0, TS_FAIL);

                    for (jx = 0; jx < 4; jx++) {
                        for (kx = 0; kx < 2; kx++) {
                            setChipState(ix, jx+1, (fbiMemErrors[kx][jx] ? TS_FAIL : TS_PASS));
                        }
                    }
                } else {
                    setChipState(ix, -1, TS_PASS);
                }
                break;

              case CHIP_TMU:
                if (!testTMUMem()) {
                    setChipState(ix, 0, TS_FAIL);

                    for (jx = 0; jx < 4; jx++) {
                        for (kx = 0; kx < 2; kx++) {
                            setChipState(ix, (jx*4)+kx+1, (tmuMemErrors[0][kx][jx] ? TS_FAIL : TS_PASS));
                        }
                    }
                } else {
                    setChipState(ix, -1, TS_PASS);
                }
                break;

              case CHIP_DRAM:
                if (!testSysMem()) {
                    for (kx = 0; kx < 4; kx++) {
                        setChipState(ix, kx, (sysMemErrors[kx] ? TS_FAIL : TS_PASS));
                    }
                } else {
                    setChipState(ix, -1, TS_PASS);
                }
                break;
              case CHIP_BOOT:
                setChipState(ix, -1, (bootEpromTest() ? TS_PASS : TS_FAIL));
                break;

              case CHIP_EXP:
                setChipState(ix, -1, (expEpromTest() ? TS_PASS : TS_FAIL));
                break;

              case CHIP_IO:
                setChipState(ix, -1, TS_PASS);
                break;

              case CHIP_PIC:
                setChipState(ix, -1, (sendPicNOP() ? TS_PASS : TS_FAIL));
                break;

              case CHIP_HD:
                setChipState(ix, -1, (ide_init() ? TS_PASS : TS_FAIL));
                break;

              case CHIP_SCPU:
                setChipState(ix, -1, dcs2CPUDSPTest() ? TS_PASS : TS_FAIL);
                break;

              case CHIP_SROM:
                setChipState(ix, -1, dcs2BootROMTest() ? TS_PASS : TS_FAIL);
                break;

              case CHIP_SDRAM:
                setChipState(ix, -1, dcs2DRAMTest() ? TS_PASS : TS_FAIL);
                break;

              case CHIP_SSRAM:
                setChipState(ix, -1, dcs2SRAMTest() ? TS_PASS : TS_FAIL);
                break;

              default:
                break;
            }

bypass:
            if (ix >= CHIP_DRAM) {
                grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);

                drawPostScreen(ix);

                grBufferSwap(1);
                while (grBufferNumPending());
            }
        }


        ++loopcount;
        testdelay(2*60);    // let screen be visible for 2 seconds

#if BIOS == MIKE_BIOS
        installhandler(0, (mxU32 *)0);
        disableint(VBLANK_INT);
#endif
    } while (loopmode);

    putmsg("Finished the P.O.S.T.\r\n");
}


mxBool initVideoCard() /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
    Initialize video card and set video mode
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/

{
	GrHwConfiguration	hwconfig;


  /*** Initialize 3Dfx system ***/
    if (!grSstQueryHardware(&hwconfig)) return (FALSE);

  /*** Select SST 0 ***/
    grSstSelect(0);

    if (!grSstOpen(GR_RESOLUTION_512x400,  GR_REFRESH_60Hz,
                   GR_COLORFORMAT_ARGB,    GR_ORIGIN_UPPER_LEFT,
                   GR_SMOOTHING_ENABLE,    2 )) {

        return (FALSE);
    }

  /** Configure Glide to test depth buffering ***/
    grConstantColorValue(~0);
    grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
    grDepthBufferFunction(GR_CMP_LESS);
    grDepthMask(FXTRUE);

    grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_ZERO);
    guColorCombineFunction(GR_COLORCOMBINE_ITRGB);

    grLfbWriteMode(GR_LFBWRITEMODE_565);
}


void setChipState(int group, int groupid, int state) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    int     ix;


    for (ix = 0; ix < CHIP_DISP_SZ; ix++) {
        if ((chip_disp[ix].grp == group) &&
            ((chip_disp[ix].grpid == groupid) || (groupid == -1))) {
            chip_disp[ix].state = state;
            if (state == TS_FAIL) chip_disp[ix].failcnt++;
        }
    }
}

void drawPostScreen(int group) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    int     ix;
    mxU32   fcol;


    drawBoard();

    _screenPtr = grLfbGetWritePtr(GR_BUFFER_BACKBUFFER);

    gprintf(18, 1, DK_WHT, DK_BLK, "P.O.S.T. Version %d.%d%c",  _ver.maj, _ver.min, _ver.rev);
    gprintf(2, 2, DK_WHT, DK_BLK, "Legend:");
    gprintf(5, 3, DK_YEL, DK_BLK, "Û");
    gprintf(7, 3, LT_BLK, DK_BLK, "- Testing");
    gprintf(5, 4, DK_GRN, DK_BLK, "Û");
    gprintf(7, 4, LT_BLK, DK_BLK, "- Pass");
    gprintf(5, 5, DK_RED, DK_BLK, "Û");
    gprintf(7, 5, LT_BLK, DK_BLK, "- Fail");
    gprintf(2, 40, DK_WHT, DK_BLK, "Pass: %ld", loopcount);

    for (ix = 0; ix < CHIP_DISP_SZ; ix++) {
        switch (chip_disp[ix].state) {
          case TS_NORM:
            fcol = LT_BLK;  break;
          case TS_TEST:
            fcol = DK_YEL;  break;
          case TS_PASS:
            fcol = DK_GRN;  break;
          case TS_FAIL:
            fcol = DK_RED;  break;
        }

        if (chip_disp[ix].grp != CHIP_NONE) {
            gprintf(2, ix+8, fcol, DK_BLK, "%s", chip_disp[ix].desc);
        }

        if (chip_disp[ix].failcnt && loopmode) {
            gprintf(20, ix+8, DK_RED, DK_BLK, "%ld", chip_disp[ix].failcnt);
        }

        drawRectangle(chip_disp[ix].x+240, chip_disp[ix].y+40,
                      chip_disp[ix].w, chip_disp[ix].h);

        drawFilledRectangle(chip_disp[ix].x+240, chip_disp[ix].y+40,
                            chip_disp[ix].w, chip_disp[ix].h,
                            chip_disp[ix].state);
    }
}


void drawBoard(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Draw the circuit board outline.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    int     ix;


  /* Initialize the GrVertex structure common elements for each point */
    pta.tmuvtx[0].oow = ptb.tmuvtx[0].oow = 1.0f;
    pta.r   = ptb.r   = 128.0f;
    pta.g   = ptb.g   = 128.0f;
    pta.b   = ptb.b   = 128.0f;

    for (ix = 0; ix < BOARD_SZ; ix++) {
        pta.x = (float)(board[ix].x1+240);
        pta.y = (float)(board[ix].y1+40);
        ptb.x = (float)(board[ix].x2+240);
        ptb.y = (float)(board[ix].y2+40);

        grDrawLine(&pta, &ptb);
    }

    for (ix = 0; ix < HEATSINK_SZ; ix++) {
        pta.x = (float)(heatsink[ix].x1+240);
        pta.y = (float)(heatsink[ix].y1+40);
        ptb.x = (float)(heatsink[ix].x2+240);
        ptb.y = (float)(heatsink[ix].y2+40);

        grDrawLine(&pta, &ptb);
    }
}


void drawRectangle(mxU32 x, mxU32 y, mxU32 w, mxU32 h) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Draw a rectangle with starting coordinates of (x, y)
  with a width and height specified by (w, h).
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{

  /* Initialize the GrVertex structure common elements for each point */
    pta.tmuvtx[0].oow = ptb.tmuvtx[0].oow = 1.0f;
    pta.r   = ptb.r   = 128.0f;
    pta.g   = ptb.g   = 128.0f;
    pta.b   = ptb.b   = 128.0f;

/* Top */
  /* pta = x1, y1 */
    pta.x = (float)x;
    pta.y = (float)y;

  /* ptb = x2, y1 */
    ptb.x = (float)(x + w - 1);
    ptb.y = (float)y;

    grDrawLine(&ptb, &pta);

/* Left */
    pta.y = (float)(y-1);

  /* ptb = x1, y2 */
    ptb.x = (float)x;
    ptb.y = (float)(y + h - 1);

    grDrawLine(&pta, &ptb);

/* Bottom */
  /* pta = x2, y2 */
    pta.x = (float)(x + w - 1);
    pta.y = (float)(y + h - 1);

    grDrawLine(&pta, &ptb);

/* Right */
  /* ptb = x2, y1 */
    ptb.x = (float)(x + w - 1);
    ptb.y = (float)y;

    grDrawLine(&pta, &ptb);

}



void drawFilledRectangle(mxU32 x, mxU32 y, mxU32 w, mxU32 h, mxU32 color) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Draw a rectangle with starting coordinates of (x, y)
  with a width and height specified by (w, h).
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxU32   xt, yt, wt, ht, yt2;

    xt = x;
    yt = y + 1;
    wt = w - 1;
    ht = y + h - 1;

  /* Initialize the GrVertex structure common elements for each point */
    pta.tmuvtx[0].oow = ptb.tmuvtx[0].oow = 1.0f;
    pta.b = ptb.b = 0.0f;

    switch (color) {
      case TS_NORM:
        pta.r = ptb.r = 0.0f;
        pta.g = ptb.g = 0.0f;
        break;
      case TS_TEST:
        pta.r = ptb.r = 192.0f;
        pta.g = ptb.g = 192.0f;
        break;
      case TS_PASS:
        pta.r = ptb.r = 0.0f;
        pta.g = ptb.g = 128.0f;
        break;
      case TS_FAIL:
        pta.r = ptb.r = 128.0f;
        pta.g = ptb.g = 0.0f;
        break;
    }

    for (yt2 = yt; yt2 < ht; yt2++) {
      /* pta = x1, y */
        pta.x = (float)xt;
        pta.y = (float)yt2;

      /* ptb = x2, y */
        ptb.x = (float)(xt + wt - 1);
        ptb.y = (float)yt2;

        grDrawLine(&ptb, &pta);
    }
}


mxU32 getSysTicks(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
#if BIOS == JACK_BIOS
    mxU32   (*getticks)(void) = (mxU32 (*)(void))0x80000220;

    return (getticks());

#elif BIOS == MIKE_BIOS
    return(__systicks);
#endif
}


#if BIOS == MIKE_BIOS
#pragma interrupt

mxU32 doSysTicks(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    ++__systicks;

    return (1);
}


#pragma interrupt

mxU32 doDiskInterrupt(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    gprintf(2, 40, DK_RED, DK_BLK, "Got the disk interrupt !");

    return (0);
}
#endif


void testdelay(int delay) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxU32   ticks;


    ticks = getSysTicks() + delay;
    while (getSysTicks() < ticks);
}


void delay(mxU32 vblanks) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxU32   timer;

    if (vblanks) {
        timer = getSysTicks() + vblanks;

        while (getSysTicks() < timer);
    }
}
