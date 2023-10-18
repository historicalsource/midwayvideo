#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	float x;
	float y;
	float z;
} VERTEX;

typedef struct {
	float s;
	float t;
} ST;

typedef struct {
	unsigned short v1, v2, v3;
	unsigned short t1, t2, t3;
	short texture;
} TRI;

typedef struct {
	int nvtx;
	int ntris;
	int nst;
	VERTEX *pvtx;
	ST *pst;
	TRI *ptri;
#ifdef JONHEY	
	VERTEX *bbox;
#endif
} LIMB;

typedef struct {
	int count;
	short orig[4];
	short new[4];
} texture_remap;

static texture_remap xlate_08 = {1, {0, -1, -1, -1}, {8, -1, -1, -1}};
static texture_remap xlate_48 = {1, {4, -1, -1, -1}, {8, -1, -1, -1}};
static texture_remap xlate_08_40 = {2, {0, 4, -1, -1}, {8, 0, -1, -1}};

#define MODEL_DATA
#define MODEL_DATA_ST_COUNT

/* BIG PLAYER */
#include "/video/nba/models/waist_1.h"
#include "/video/nba/models/torso_1.h"
#include "/video/nba/models/arm_ul1.h"
#include "/video/nba/models/arm_ur1.h"
#include "/video/nba/models/arm_ll1.h"
#include "/video/nba/models/arm_lr1.h"
#include "/video/nba/models/hand_r1.h"
#include "/video/nba/models/hand_l1.h"
#include "/video/nba/models/leg_ul1.h"
#include "/video/nba/models/leg_ur1.h"
#include "/video/nba/models/leg_ll1.h"
#include "/video/nba/models/leg_lr1.h"
#include "/video/nba/models/foot_r1.h"
#include "/video/nba/models/foot_l1.h"
static LIMB *plyr_limbs_1[] = {
	&limb_waist_1,
	&limb_torso_1,
	&limb_arm_ur1,
	&limb_arm_lr1,
	&limb_hand_r1,
	&limb_arm_ul1,
	&limb_arm_ll1,
	&limb_hand_l1,
	NULL,
	&limb_leg_ur1,
	&limb_leg_lr1,
	&limb_foot_r1,
	&limb_leg_ul1,
	&limb_leg_ll1,
	&limb_foot_l1
};
static texture_remap *plyr_limbs_1_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* NORMAL PLAYER */
#include "/video/nba/models/waist_2.h"
#include "/video/nba/models/torso_2.h"
#include "/video/nba/models/arm_ul2.h"
#include "/video/nba/models/arm_ur2.h"
#include "/video/nba/models/arm_ll2.h"
#include "/video/nba/models/arm_lr2.h"
#include "/video/nba/models/hand_r2.h"
#include "/video/nba/models/hand_l2.h"
#include "/video/nba/models/leg_ul2.h"
#include "/video/nba/models/leg_ur2.h"
#include "/video/nba/models/leg_ll2.h"
#include "/video/nba/models/leg_lr2.h"
#include "/video/nba/models/foot_r2.h"
#include "/video/nba/models/foot_l2.h"
static LIMB *plyr_limbs_2[] = {
	&limb_waist_2,
	&limb_torso_2,
	&limb_arm_ur2,
	&limb_arm_lr2,
	&limb_hand_r2,
	&limb_arm_ul2,
	&limb_arm_ll2,
	&limb_hand_l2,
	NULL,
	&limb_leg_ur2,
	&limb_leg_lr2,
	&limb_foot_r2,
	&limb_leg_ul2,
	&limb_leg_ll2,
	&limb_foot_l2
};
static texture_remap *plyr_limbs_2_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* HORNETS MASCOT */
#include "/video/nba/models/waist_h.h"
#include "/video/nba/models/torso_h.h"
#include "/video/nba/models/arm_ulh.h"
#include "/video/nba/models/arm_urh.h"
#include "/video/nba/models/arm_llh.h"
#include "/video/nba/models/arm_lrh.h"
#include "/video/nba/models/hand_rh.h"
#include "/video/nba/models/hand_lh.h"
#include "/video/nba/models/leg_ulh.h"
#include "/video/nba/models/leg_urh.h"
#include "/video/nba/models/leg_llh.h"
#include "/video/nba/models/leg_lrh.h"
#include "/video/nba/models/foot_rh.h"
#include "/video/nba/models/foot_lh.h"
static LIMB *hornet_limbs[] = {
	&limb_waist_h,
	&limb_torso_h,
	&limb_arm_urh,
	&limb_arm_lrh,
	&limb_hand_rh,
	&limb_arm_ulh,
	&limb_arm_llh,
	&limb_hand_lh,
	NULL,
	&limb_leg_urh,
	&limb_leg_lrh,
	&limb_foot_rh,
	&limb_leg_ulh,
	&limb_leg_llh,
	&limb_foot_lh
};
static texture_remap *hornet_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08,				/* leg UR */
	&xlate_08,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08,				/* leg UL */
	&xlate_08,				/* leg LL */
	&xlate_08				/* foot L */
};

/* BULLS MASCOT */
#include "/video/nba/models/waist_cb.h"
#include "/video/nba/models/torso_cb.h"
#include "/video/nba/models/arm_ulcb.h"
#include "/video/nba/models/arm_urcb.h"
#include "/video/nba/models/arm_llcb.h"
#include "/video/nba/models/arm_lrcb.h"
#include "/video/nba/models/hand_rcb.h"
#include "/video/nba/models/hand_lcb.h"
#include "/video/nba/models/leg_ulcb.h"
#include "/video/nba/models/leg_urcb.h"
#include "/video/nba/models/leg_llcb.h"
#include "/video/nba/models/leg_lrcb.h"
#include "/video/nba/models/foot_rcb.h"
#include "/video/nba/models/foot_lcb.h"
static LIMB *bulls_limbs[] = {
	&limb_waist_cb,
	&limb_torso_cb,
	&limb_arm_urcb,
	&limb_arm_lrcb,
	&limb_hand_rcb,
	&limb_arm_ulcb,
	&limb_arm_llcb,
	&limb_hand_lcb,
	NULL,
	&limb_leg_urcb,
	&limb_leg_lrcb,
	&limb_foot_rcb,
	&limb_leg_ulcb,
	&limb_leg_llcb,
	&limb_foot_lcb
};
static texture_remap *bulls_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	&xlate_08,				/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	&xlate_08,				/* hand L */
	NULL,					/* head */
	NULL,					/* leg UR */
	NULL,					/* leg LR */
	&xlate_08,				/* foot R */
	NULL,					/* leg UL */
	NULL,					/* leg LL */
	&xlate_08				/* foot L */
};

/* PACERS MASCOT */
#include "/video/nba/models/waist_b.h"
#include "/video/nba/models/torso_b.h"
#include "/video/nba/models/arm_ulb.h"
#include "/video/nba/models/arm_urb.h"
#include "/video/nba/models/arm_llb.h"
#include "/video/nba/models/arm_lrb.h"
#include "/video/nba/models/hand_rb.h"
#include "/video/nba/models/hand_lb.h"
#include "/video/nba/models/leg_ulb.h"
#include "/video/nba/models/leg_urb.h"
#include "/video/nba/models/leg_llb.h"
#include "/video/nba/models/leg_lrb.h"
#include "/video/nba/models/foot_rb.h"
#include "/video/nba/models/foot_lb.h"
static LIMB *pacers_limbs[] = {
	&limb_waist_b,
	&limb_torso_b,
	&limb_arm_urb,
	&limb_arm_lrb,
	&limb_hand_rb,
	&limb_arm_ulb,
	&limb_arm_llb,
	&limb_hand_lb,
	NULL,
	&limb_leg_urb,
	&limb_leg_lrb,
	&limb_foot_rb,
	&limb_leg_ulb,
	&limb_leg_llb,
	&limb_foot_lb
};
static texture_remap *pacers_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* TIMBERWOLVES MASCOT */
#include "/video/nba/models/waist_c.h"
#include "/video/nba/models/torso_c.h"
#include "/video/nba/models/arm_ulc.h"
#include "/video/nba/models/arm_urc.h"
#include "/video/nba/models/arm_llc.h"
#include "/video/nba/models/arm_lrc.h"
#include "/video/nba/models/hand_rc.h"
#include "/video/nba/models/hand_lc.h"
#include "/video/nba/models/leg_ulc.h"
#include "/video/nba/models/leg_urc.h"
#include "/video/nba/models/leg_llc.h"
#include "/video/nba/models/leg_lrc.h"
#include "/video/nba/models/foot_rc.h"
#include "/video/nba/models/foot_lc.h"
static LIMB *timberwolves_limbs[] = {
	&limb_waist_c,
	&limb_torso_c,
	&limb_arm_urc,
	&limb_arm_lrc,
	&limb_hand_rc,
	&limb_arm_ulc,
	&limb_arm_llc,
	&limb_hand_lc,
	NULL,
	&limb_leg_urc,
	&limb_leg_lrc,
	&limb_foot_rc,
	&limb_leg_ulc,
	&limb_leg_llc,
	&limb_foot_lc
};
static texture_remap *timberwolves_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* RAPTORS MASCOT */
#include "/video/nba/models/waist_r.h"
#include "/video/nba/models/torso_r.h"
#include "/video/nba/models/arm_ulr.h"
#include "/video/nba/models/arm_urr.h"
#include "/video/nba/models/arm_llr.h"
#include "/video/nba/models/arm_lrr.h"
#include "/video/nba/models/hand_rr.h"
#include "/video/nba/models/hand_lr.h"
#include "/video/nba/models/leg_ulr.h"
#include "/video/nba/models/leg_urr.h"
#include "/video/nba/models/leg_llr.h"
#include "/video/nba/models/leg_lrr.h"
#include "/video/nba/models/foot_rr.h"
#include "/video/nba/models/foot_lr.h"
static LIMB *raptors_limbs[] = {
	&limb_waist_r,
	&limb_torso_r,
	&limb_arm_urr,
	&limb_arm_lrr,
	&limb_hand_rr,
	&limb_arm_ulr,
	&limb_arm_llr,
	&limb_hand_lr,
	NULL,
	&limb_leg_urr,
	&limb_leg_lrr,
	&limb_foot_rr,
	&limb_leg_ulr,
	&limb_leg_llr,
	&limb_foot_lr
};
static texture_remap *raptors_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* ROCKETS MASCOT */
#include "/video/nba/models/waist_hb.h"
#include "/video/nba/models/torso_hb.h"
#include "/video/nba/models/arm_ulhb.h"
#include "/video/nba/models/arm_urhb.h"
#include "/video/nba/models/arm_llhb.h"
#include "/video/nba/models/arm_lrhb.h"
#include "/video/nba/models/hand_rhb.h"
#include "/video/nba/models/hand_lhb.h"
#include "/video/nba/models/leg_ulhb.h"
#include "/video/nba/models/leg_urhb.h"
#include "/video/nba/models/leg_llhb.h"
#include "/video/nba/models/leg_lrhb.h"
#include "/video/nba/models/foot_rhb.h"
#include "/video/nba/models/foot_lhb.h"
static LIMB *rockets_limbs[] = {
	&limb_waist_hb,
	&limb_torso_hb,
	&limb_arm_urhb,
	&limb_arm_lrhb,
	&limb_hand_rhb,
	&limb_arm_ulhb,
	&limb_arm_llhb,
	&limb_hand_lhb,
	NULL,
	&limb_leg_urhb,
	&limb_leg_lrhb,
	&limb_foot_rhb,
	&limb_leg_ulhb,
	&limb_leg_llhb,
	&limb_foot_lhb
};
static texture_remap *rockets_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	NULL,					/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	NULL,					/* leg LL */
	&xlate_08				/* foot L */
};

/* HAWKS MASCOT */
#include "/video/nba/models/waist_ah.h"
#include "/video/nba/models/torso_ah.h"
#include "/video/nba/models/arm_ulah.h"
#include "/video/nba/models/arm_urah.h"
#include "/video/nba/models/arm_llah.h"
#include "/video/nba/models/arm_lrah.h"
#include "/video/nba/models/hand_rah.h"
#include "/video/nba/models/hand_lah.h"
#include "/video/nba/models/leg_ulah.h"
#include "/video/nba/models/leg_urah.h"
#include "/video/nba/models/leg_llah.h"
#include "/video/nba/models/leg_lrah.h"
#include "/video/nba/models/foot_rah.h"
#include "/video/nba/models/foot_lah.h"
static LIMB *hawks_limbs[] = {
	&limb_waist_ah,
	&limb_torso_ah,
	&limb_arm_urah,
	&limb_arm_lrah,
	&limb_hand_rah,
	&limb_arm_ulah,
	&limb_arm_llah,
	&limb_hand_lah,
	NULL,
	&limb_leg_urah,
	&limb_leg_lrah,
	&limb_foot_rah,
	&limb_leg_ulah,
	&limb_leg_llah,
	&limb_foot_lah
};
static texture_remap *hawks_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08,				/* torso */
	&xlate_48,				/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	&xlate_48,				/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* SUNS MASCOT */
#include "/video/nba/models/waist_pg.h"
#include "/video/nba/models/torso_pg.h"
#include "/video/nba/models/arm_ulpg.h"
#include "/video/nba/models/arm_urpg.h"
#include "/video/nba/models/arm_llpg.h"
#include "/video/nba/models/arm_lrpg.h"
#include "/video/nba/models/hand_rpg.h"
#include "/video/nba/models/hand_lpg.h"
#include "/video/nba/models/leg_ulpg.h"
#include "/video/nba/models/leg_urpg.h"
#include "/video/nba/models/leg_llpg.h"
#include "/video/nba/models/leg_lrpg.h"
#include "/video/nba/models/foot_rpg.h"
#include "/video/nba/models/foot_lpg.h"
static LIMB *suns_limbs[] = {
	&limb_waist_pg,
	&limb_torso_pg,
	&limb_arm_urpg,
	&limb_arm_lrpg,
	&limb_hand_rpg,
	&limb_arm_ulpg,
	&limb_arm_llpg,
	&limb_hand_lpg,
	NULL,
	&limb_leg_urpg,
	&limb_leg_lrpg,
	&limb_foot_rpg,
	&limb_leg_ulpg,
	&limb_leg_llpg,
	&limb_foot_lpg
};
static texture_remap *suns_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08,				/* torso */
	&xlate_48,				/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	&xlate_48,				/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* NETS MASCOT */
#include "/video/nba/models/waist_s.h"
#include "/video/nba/models/torso_s.h"
#include "/video/nba/models/arm_uls.h"
#include "/video/nba/models/arm_urs.h"
#include "/video/nba/models/arm_lls.h"
#include "/video/nba/models/arm_lrs.h"
#include "/video/nba/models/hand_rs.h"
#include "/video/nba/models/hand_ls.h"
#include "/video/nba/models/leg_uls.h"
#include "/video/nba/models/leg_urs.h"
#include "/video/nba/models/leg_lls.h"
#include "/video/nba/models/leg_lrs.h"
#include "/video/nba/models/foot_rs.h"
#include "/video/nba/models/foot_ls.h"
static LIMB *nets_limbs[] = {
	&limb_waist_s,
	&limb_torso_s,
	&limb_arm_urs,
	&limb_arm_lrs,
	&limb_hand_rs,
	&limb_arm_uls,
	&limb_arm_lls,
	&limb_hand_ls,
	NULL,
	&limb_leg_urs,
	&limb_leg_lrs,
	&limb_foot_rs,
	&limb_leg_uls,
	&limb_leg_lls,
	&limb_foot_ls
};
static texture_remap *nets_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* DENVER MASCOT */
#include "/video/nba/models/waist_dr.h"
#include "/video/nba/models/torso_dr.h"
#include "/video/nba/models/arm_uldr.h"
#include "/video/nba/models/arm_urdr.h"
#include "/video/nba/models/arm_lldr.h"
#include "/video/nba/models/arm_lrdr.h"
#include "/video/nba/models/hand_rdr.h"
#include "/video/nba/models/hand_ldr.h"
#include "/video/nba/models/leg_uldr.h"
#include "/video/nba/models/leg_urdr.h"
#include "/video/nba/models/leg_lldr.h"
#include "/video/nba/models/leg_lrdr.h"
#include "/video/nba/models/foot_rdr.h"
#include "/video/nba/models/foot_ldr.h"
static LIMB *rocky_limbs[] = {
	&limb_waist_dr,
	&limb_torso_dr,
	&limb_arm_urdr,
	&limb_arm_lrdr,
	&limb_hand_rdr,
	&limb_arm_uldr,
	&limb_arm_lldr,
	&limb_hand_ldr,
	NULL,
	&limb_leg_urdr,
	&limb_leg_lrdr,
	&limb_foot_rdr,
	&limb_leg_uldr,
	&limb_leg_lldr,
	&limb_foot_ldr
};
static texture_remap *rocky_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* SONICS MASCOT */
#include "/video/nba/models/waist_ss.h"
#include "/video/nba/models/torso_ss.h"
#include "/video/nba/models/arm_ulss.h"
#include "/video/nba/models/arm_urss.h"
#include "/video/nba/models/arm_llss.h"
#include "/video/nba/models/arm_lrss.h"
#include "/video/nba/models/hand_rss.h"
#include "/video/nba/models/hand_lss.h"
#include "/video/nba/models/leg_ulss.h"
#include "/video/nba/models/leg_urss.h"
#include "/video/nba/models/leg_llss.h"
#include "/video/nba/models/leg_lrss.h"
#include "/video/nba/models/foot_rss.h"
#include "/video/nba/models/foot_lss.h"
static LIMB *sonics_limbs[] = {
	&limb_waist_ss,
	&limb_torso_ss,
	&limb_arm_urss,
	&limb_arm_lrss,
	&limb_hand_rss,
	&limb_arm_ulss,
	&limb_arm_llss,
	&limb_hand_lss,
	NULL,
	&limb_leg_urss,
	&limb_leg_lrss,
	&limb_foot_rss,
	&limb_leg_ulss,
	&limb_leg_llss,
	&limb_foot_lss
};
static texture_remap *sonics_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* UTAH MASCOT */
#include "/video/nba/models/waist_ub.h"
#include "/video/nba/models/torso_ub.h"
#include "/video/nba/models/arm_ulub.h"
#include "/video/nba/models/arm_urub.h"
#include "/video/nba/models/arm_llub.h"
#include "/video/nba/models/arm_lrub.h"
#include "/video/nba/models/hand_rub.h"
#include "/video/nba/models/hand_lub.h"
#include "/video/nba/models/leg_ulub.h"
#include "/video/nba/models/leg_urub.h"
#include "/video/nba/models/leg_llub.h"
#include "/video/nba/models/leg_lrub.h"
#include "/video/nba/models/foot_rub.h"
#include "/video/nba/models/foot_lub.h"
static LIMB *utah_limbs[] = {
	&limb_waist_ub,
	&limb_torso_ub,
	&limb_arm_urub,
	&limb_arm_lrub,
	&limb_hand_rub,
	&limb_arm_ulub,
	&limb_arm_llub,
	&limb_hand_lub,
	NULL,
	&limb_leg_urub,
	&limb_leg_lrub,
	&limb_foot_rub,
	&limb_leg_ulub,
	&limb_leg_llub,
	&limb_foot_lub
};
static texture_remap *utah_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* ALIEN CHARACTER */
#include "/video/nba/models/waist_a.h"
#include "/video/nba/models/torso_a.h"
#include "/video/nba/models/arm_ula.h"
#include "/video/nba/models/arm_ura.h"
#include "/video/nba/models/arm_lla.h"
#include "/video/nba/models/arm_lra.h"
#include "/video/nba/models/hand_ra.h"
#include "/video/nba/models/hand_la.h"
#include "/video/nba/models/leg_ula.h"
#include "/video/nba/models/leg_ura.h"
#include "/video/nba/models/leg_lla.h"
#include "/video/nba/models/leg_lra.h"
#include "/video/nba/models/foot_ra.h"
#include "/video/nba/models/foot_la.h"
static LIMB *alien_limbs[] = {
	&limb_waist_a,
	&limb_torso_a,
	&limb_arm_ura,
	&limb_arm_lra,
	&limb_hand_ra,
	&limb_arm_ula,
	&limb_arm_lla,
	&limb_hand_la,
	NULL,
	&limb_leg_ura,
	&limb_leg_lra,
	&limb_foot_ra,
	&limb_leg_ula,
	&limb_leg_lla,
	&limb_foot_la
};
static texture_remap *alien_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* SPACE JAM CHARACTER */
#include "/video/nba/models/waist_j.h"
#include "/video/nba/models/torso_j.h"
#include "/video/nba/models/arm_ulj.h"
#include "/video/nba/models/arm_urj.h"
#include "/video/nba/models/arm_llj.h"
#include "/video/nba/models/arm_lrj.h"
#include "/video/nba/models/hand_rj.h"
#include "/video/nba/models/hand_lj.h"
#include "/video/nba/models/leg_ulj.h"
#include "/video/nba/models/leg_urj.h"
#include "/video/nba/models/leg_llj.h"
#include "/video/nba/models/leg_lrj.h"
#include "/video/nba/models/foot_rj.h"
#include "/video/nba/models/foot_lj.h"
static LIMB *biggy_smalls_limbs[] = {
	&limb_waist_j,
	&limb_torso_j,
	&limb_arm_urj,
	&limb_arm_lrj,
	&limb_hand_rj,
	&limb_arm_ulj,
	&limb_arm_llj,
	&limb_hand_lj,
	NULL,
	&limb_leg_urj,
	&limb_leg_lrj,
	&limb_foot_rj,
	&limb_leg_ulj,
	&limb_leg_llj,
	&limb_foot_lj
};
static texture_remap *biggy_smalls_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* NIKKO DOG CHARACTER */
#include "/video/nba/models/waist_n.h"
#include "/video/nba/models/torso_n.h"
#include "/video/nba/models/arm_uln.h"
#include "/video/nba/models/arm_urn.h"
#include "/video/nba/models/arm_lln.h"
#include "/video/nba/models/arm_lrn.h"
#include "/video/nba/models/hand_rn.h"
#include "/video/nba/models/hand_ln.h"
#include "/video/nba/models/leg_uln.h"
#include "/video/nba/models/leg_urn.h"
#include "/video/nba/models/leg_lln.h"
#include "/video/nba/models/leg_lrn.h"
#include "/video/nba/models/foot_rn.h"
#include "/video/nba/models/foot_ln.h"
static LIMB *nikko_limbs[] = {
	&limb_waist_n,
	&limb_torso_n,
	&limb_arm_urn,
	&limb_arm_lrn,
	&limb_hand_rn,
	&limb_arm_uln,
	&limb_arm_lln,
	&limb_hand_ln,
	NULL,
	&limb_leg_urn,
	&limb_leg_lrn,
	&limb_foot_rn,
	&limb_leg_uln,
	&limb_leg_lln,
	&limb_foot_ln
};
static texture_remap *nikko_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08,				/* leg UR */
	NULL,					/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08,				/* leg UL */
	NULL,					/* leg LL */
	&xlate_08				/* foot L */
};

/* NANA CHARACTER */
#include "/video/nba/models/waist_f.h"
#include "/video/nba/models/torso_f.h"
#include "/video/nba/models/arm_ulf.h"
#include "/video/nba/models/arm_urf.h"
#include "/video/nba/models/arm_llf.h"
#include "/video/nba/models/arm_lrf.h"
#include "/video/nba/models/hand_rf.h"
#include "/video/nba/models/hand_lf.h"
#include "/video/nba/models/leg_ulf.h"
#include "/video/nba/models/leg_urf.h"
#include "/video/nba/models/leg_llf.h"
#include "/video/nba/models/leg_lrf.h"
#include "/video/nba/models/foot_rf.h"
#include "/video/nba/models/foot_lf.h"
static LIMB *nana_limbs[] = {
	&limb_waist_f,
	&limb_torso_f,
	&limb_arm_urf,
	&limb_arm_lrf,
	&limb_hand_rf,
	&limb_arm_ulf,
	&limb_arm_llf,
	&limb_hand_lf,
	NULL,
	&limb_leg_urf,
	&limb_leg_lrf,
	&limb_foot_rf,
	&limb_leg_ulf,
	&limb_leg_llf,
	&limb_foot_lf
};
static texture_remap *nana_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* RETRO CHARACTER */
#include "/video/nba/models/waist_rt.h"
#include "/video/nba/models/torso_rt.h"
#include "/video/nba/models/arm_ulrt.h"
#include "/video/nba/models/arm_urrt.h"
#include "/video/nba/models/arm_llrt.h"
#include "/video/nba/models/arm_lrrt.h"
#include "/video/nba/models/hand_rrt.h"
#include "/video/nba/models/hand_lrt.h"
#include "/video/nba/models/leg_ulrt.h"
#include "/video/nba/models/leg_urrt.h"
#include "/video/nba/models/leg_llrt.h"
#include "/video/nba/models/leg_lrrt.h"
#include "/video/nba/models/foot_rrt.h"
#include "/video/nba/models/foot_lrt.h"
static LIMB *retro_limbs[] = {
	&limb_waist_rt,
	&limb_torso_rt,
	&limb_arm_urrt,
	&limb_arm_lrrt,
	&limb_hand_rrt,
	&limb_arm_ulrt,
	&limb_arm_llrt,
	&limb_hand_lrt,
	NULL,
	&limb_leg_urrt,
	&limb_leg_lrrt,
	&limb_foot_rrt,
	&limb_leg_ulrt,
	&limb_leg_llrt,
	&limb_foot_lrt
};
static texture_remap *retro_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_48,				/* leg UR */
	NULL,					/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_48,				/* leg UL */
	NULL,					/* leg LL */
	&xlate_08				/* foot L */
};

/* WIZARD CHARACTER */
#include "/video/nba/models/waist_w.h"
#include "/video/nba/models/torso_w.h"
#include "/video/nba/models/arm_ulw.h"
#include "/video/nba/models/arm_urw.h"
#include "/video/nba/models/arm_llw.h"
#include "/video/nba/models/arm_lrw.h"
#include "/video/nba/models/hand_rw.h"
#include "/video/nba/models/hand_lw.h"
#include "/video/nba/models/leg_ulw.h"
#include "/video/nba/models/leg_urw.h"
#include "/video/nba/models/leg_llw.h"
#include "/video/nba/models/leg_lrw.h"
#include "/video/nba/models/foot_rw.h"
#include "/video/nba/models/foot_lw.h"
static LIMB *wizard_limbs[] = {
	&limb_waist_w,
	&limb_torso_w,
	&limb_arm_urw,
	&limb_arm_lrw,
	&limb_hand_rw,
	&limb_arm_ulw,
	&limb_arm_llw,
	&limb_hand_lw,
	NULL,
	&limb_leg_urw,
	&limb_leg_lrw,
	&limb_foot_rw,
	&limb_leg_ulw,
	&limb_leg_llw,
	&limb_foot_lw
};
static texture_remap *wizard_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08,				/* leg UR */
	&xlate_08,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08,				/* leg UL */
	&xlate_08,				/* leg LL */
	&xlate_08				/* foot L */
};

/* OLDMAN CHARACTER */
#include "/video/nba/models/waist_om.h"
#include "/video/nba/models/torso_om.h"
#include "/video/nba/models/arm_ulom.h"
#include "/video/nba/models/arm_urom.h"
#include "/video/nba/models/arm_llom.h"
#include "/video/nba/models/arm_lrom.h"
#include "/video/nba/models/hand_rom.h"
#include "/video/nba/models/hand_lom.h"
#include "/video/nba/models/leg_ulom.h"
#include "/video/nba/models/leg_urom.h"
#include "/video/nba/models/leg_llom.h"
#include "/video/nba/models/leg_lrom.h"
#include "/video/nba/models/foot_rom.h"
#include "/video/nba/models/foot_lom.h"
static LIMB *oldman_limbs[] = {
	&limb_waist_om,
	&limb_torso_om,
	&limb_arm_urom,
	&limb_arm_lrom,
	&limb_hand_rom,
	&limb_arm_ulom,
	&limb_arm_llom,
	&limb_hand_lom,
	NULL,
	&limb_leg_urom,
	&limb_leg_lrom,
	&limb_foot_rom,
	&limb_leg_ulom,
	&limb_leg_llom,
	&limb_foot_lom
};
static texture_remap *oldman_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* CLOWN CHARACTER */
#include "/video/nba/models/waist_cc.h"
#include "/video/nba/models/torso_cc.h"
#include "/video/nba/models/arm_ulcc.h"
#include "/video/nba/models/arm_urcc.h"
#include "/video/nba/models/arm_llcc.h"
#include "/video/nba/models/arm_lrcc.h"
#include "/video/nba/models/hand_rcc.h"
#include "/video/nba/models/hand_lcc.h"
#include "/video/nba/models/leg_ulcc.h"
#include "/video/nba/models/leg_urcc.h"
#include "/video/nba/models/leg_llcc.h"
#include "/video/nba/models/leg_lrcc.h"
#include "/video/nba/models/foot_rcc.h"
#include "/video/nba/models/foot_lcc.h"
static LIMB *clown_limbs[] = {
	&limb_waist_cc,
	&limb_torso_cc,
	&limb_arm_urcc,
	&limb_arm_lrcc,
	&limb_hand_rcc,
	&limb_arm_ulcc,
	&limb_arm_llcc,
	&limb_hand_lcc,
	NULL,
	&limb_leg_urcc,
	&limb_leg_lrcc,
	&limb_foot_rcc,
	&limb_leg_ulcc,
	&limb_leg_llcc,
	&limb_foot_lcc
};
static texture_remap *clown_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* PUMPKIN CHARACTER */
#include "/video/nba/models/waist_p.h"
#include "/video/nba/models/torso_p.h"
#include "/video/nba/models/arm_ulp.h"
#include "/video/nba/models/arm_urp.h"
#include "/video/nba/models/arm_llp.h"
#include "/video/nba/models/arm_lrp.h"
#include "/video/nba/models/hand_rp.h"
#include "/video/nba/models/hand_lp.h"
#include "/video/nba/models/leg_ulp.h"
#include "/video/nba/models/leg_urp.h"
#include "/video/nba/models/leg_llp.h"
#include "/video/nba/models/leg_lrp.h"
#include "/video/nba/models/foot_rp.h"
#include "/video/nba/models/foot_lp.h"
static LIMB *pumpkin_limbs[] = {
	&limb_waist_p,
	&limb_torso_p,
	&limb_arm_urp,
	&limb_arm_lrp,
	&limb_hand_rp,
	&limb_arm_ulp,
	&limb_arm_llp,
	&limb_hand_lp,
	NULL,
	&limb_leg_urp,
	&limb_leg_lrp,
	&limb_foot_rp,
	&limb_leg_ulp,
	&limb_leg_llp,
	&limb_foot_lp
};
static texture_remap *pumpkin_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* HORSE CHARACTER */
#include "/video/nba/models/waist_ho.h"
#include "/video/nba/models/torso_ho.h"
#include "/video/nba/models/arm_ulho.h"
#include "/video/nba/models/arm_urho.h"
#include "/video/nba/models/arm_llho.h"
#include "/video/nba/models/arm_lrho.h"
#include "/video/nba/models/hand_rho.h"
#include "/video/nba/models/hand_lho.h"
#include "/video/nba/models/leg_ulho.h"
#include "/video/nba/models/leg_urho.h"
#include "/video/nba/models/leg_llho.h"
#include "/video/nba/models/leg_lrho.h"
#include "/video/nba/models/foot_rho.h"
#include "/video/nba/models/foot_lho.h"
static LIMB *horse_limbs[] = {
	&limb_waist_ho,
	&limb_torso_ho,
	&limb_arm_urho,
	&limb_arm_lrho,
	&limb_hand_rho,
	&limb_arm_ulho,
	&limb_arm_llho,
	&limb_hand_lho,
	NULL,
	&limb_leg_urho,
	&limb_leg_lrho,
	&limb_foot_rho,
	&limb_leg_ulho,
	&limb_leg_llho,
	&limb_foot_lho
};
static texture_remap *horse_limbs_map[] = {
	&xlate_48,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	NULL,					/* leg LR */
	NULL,					/* foot R */
	&xlate_08_40,			/* leg UL */
	NULL,					/* leg LL */
	NULL					/* foot L */
};

/* FRANK CHARACTER */
#include "/video/nba/models/waist_uf.h"
#include "/video/nba/models/torso_uf.h"
#include "/video/nba/models/arm_uluf.h"
#include "/video/nba/models/arm_uruf.h"
#include "/video/nba/models/arm_lluf.h"
#include "/video/nba/models/arm_lruf.h"
#include "/video/nba/models/hand_ruf.h"
#include "/video/nba/models/hand_luf.h"
#include "/video/nba/models/leg_uluf.h"
#include "/video/nba/models/leg_uruf.h"
#include "/video/nba/models/leg_lluf.h"
#include "/video/nba/models/leg_lruf.h"
#include "/video/nba/models/foot_ruf.h"
#include "/video/nba/models/foot_luf.h"
static LIMB *frank_limbs[] = {
	&limb_waist_uf,
	&limb_torso_uf,
	&limb_arm_uruf,
	&limb_arm_lruf,
	&limb_hand_ruf,
	&limb_arm_uluf,
	&limb_arm_lluf,
	&limb_hand_luf,
	NULL,
	&limb_leg_uruf,
	&limb_leg_lruf,
	&limb_foot_ruf,
	&limb_leg_uluf,
	&limb_leg_lluf,
	&limb_foot_luf
};
static texture_remap *frank_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_08,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_48,				/* leg UL */
	&xlate_08_40,			/* leg LL */
	&xlate_08				/* foot L */
};

/* MUMMY CHARACTER */
#include "/video/nba/models/waist_um.h"
#include "/video/nba/models/torso_um.h"
#include "/video/nba/models/arm_ulum.h"
#include "/video/nba/models/arm_urum.h"
#include "/video/nba/models/arm_llum.h"
#include "/video/nba/models/arm_lrum.h"
#include "/video/nba/models/hand_rum.h"
#include "/video/nba/models/hand_lum.h"
#include "/video/nba/models/leg_ulum.h"
#include "/video/nba/models/leg_urum.h"
#include "/video/nba/models/leg_llum.h"
#include "/video/nba/models/leg_lrum.h"
#include "/video/nba/models/foot_rum.h"
#include "/video/nba/models/foot_lum.h"
static LIMB *mummy_limbs[] = {
	&limb_waist_um,
	&limb_torso_um,
	&limb_arm_urum,
	&limb_arm_lrum,
	&limb_hand_rum,
	&limb_arm_ulum,
	&limb_arm_llum,
	&limb_hand_lum,
	NULL,
	&limb_leg_urum,
	&limb_leg_lrum,
	&limb_foot_rum,
	&limb_leg_ulum,
	&limb_leg_llum,
	&limb_foot_lum
};
static texture_remap *mummy_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_48,				/* leg UR */
	NULL,					/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_48,				/* leg UL */
	NULL,					/* leg LL */
	&xlate_08				/* foot L */
};

/* BRIDE CHARACTER */
#include "/video/nba/models/waist_tb.h"
#include "/video/nba/models/torso_tb.h"
#include "/video/nba/models/arm_ultb.h"
#include "/video/nba/models/arm_urtb.h"
#include "/video/nba/models/arm_lltb.h"
#include "/video/nba/models/arm_lrtb.h"
#include "/video/nba/models/hand_rtb.h"
#include "/video/nba/models/hand_ltb.h"
#include "/video/nba/models/leg_ultb.h"
#include "/video/nba/models/leg_urtb.h"
#include "/video/nba/models/leg_lltb.h"
#include "/video/nba/models/leg_lrtb.h"
#include "/video/nba/models/foot_rtb.h"
#include "/video/nba/models/foot_ltb.h"
static LIMB *bride_limbs[] = {
	&limb_waist_tb,
	&limb_torso_tb,
	&limb_arm_urtb,
	&limb_arm_lrtb,
	&limb_hand_rtb,
	&limb_arm_ultb,
	&limb_arm_lltb,
	&limb_hand_ltb,
	NULL,
	&limb_leg_urtb,
	&limb_leg_lrtb,
	&limb_foot_rtb,
	&limb_leg_ultb,
	&limb_leg_lltb,
	&limb_foot_ltb
};
static texture_remap *bride_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* WOLFMAN CHARACTER */
#include "/video/nba/models/waist_wm.h"
#include "/video/nba/models/torso_wm.h"
#include "/video/nba/models/arm_ulwm.h"
#include "/video/nba/models/arm_urwm.h"
#include "/video/nba/models/arm_llwm.h"
#include "/video/nba/models/arm_lrwm.h"
#include "/video/nba/models/hand_rwm.h"
#include "/video/nba/models/hand_lwm.h"
#include "/video/nba/models/leg_ulwm.h"
#include "/video/nba/models/leg_urwm.h"
#include "/video/nba/models/leg_llwm.h"
#include "/video/nba/models/leg_lrwm.h"
#include "/video/nba/models/foot_rwm.h"
#include "/video/nba/models/foot_lwm.h"
static LIMB *wolfm_limbs[] = {
	&limb_waist_wm,
	&limb_torso_wm,
	&limb_arm_urwm,
	&limb_arm_lrwm,
	&limb_hand_rwm,
	&limb_arm_ulwm,
	&limb_arm_llwm,
	&limb_hand_lwm,
	NULL,
	&limb_leg_urwm,
	&limb_leg_lrwm,
	&limb_foot_rwm,
	&limb_leg_ulwm,
	&limb_leg_llwm,
	&limb_foot_lwm
};
static texture_remap *wolfm_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08_40,			/* leg UR */
	&xlate_48,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08_40,			/* leg UL */
	&xlate_48,				/* leg LL */
	&xlate_08				/* foot L */
};

/* REDUCED PLAYER */
#include "/video/nba/models/waist_rm.h"
#include "/video/nba/models/torso_rm.h"
#include "/video/nba/models/arm_ulrm.h"
#include "/video/nba/models/arm_urrm.h"
#include "/video/nba/models/arm_llrm.h"
#include "/video/nba/models/arm_lrrm.h"
#include "/video/nba/models/hand_rrm.h"
#include "/video/nba/models/hand_lrm.h"
#include "/video/nba/models/leg_ulrm.h"
#include "/video/nba/models/leg_urrm.h"
#include "/video/nba/models/leg_llrm.h"
#include "/video/nba/models/leg_lrrm.h"
#include "/video/nba/models/foot_rrm.h"
#include "/video/nba/models/foot_lrm.h"
static LIMB *plyr_limbs_rm[] = {
	&limb_waist_rm,
	&limb_torso_rm,
	&limb_arm_urrm,
	&limb_arm_lrrm,
	&limb_hand_rrm,
	&limb_arm_ulrm,
	&limb_arm_llrm,
	&limb_hand_lrm,
	NULL,
	&limb_leg_urrm,
	&limb_leg_lrrm,
	&limb_foot_rrm,
	&limb_leg_ulrm,
	&limb_leg_llrm,
	&limb_foot_lrm
};
static texture_remap *plyr_limbs_rm_map[] = {
	&xlate_08,				/* waist */
	&xlate_48,				/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_48,				/* leg UR */
	&xlate_08_40,			/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_48,				/* leg UL */
	&xlate_08_40,			/* leg LL */
	&xlate_08				/* foot L */
};

/* REF CHARACTER */
#include "/video/nba/models/waist_rf.h"
#include "/video/nba/models/torso_rf.h"
#include "/video/nba/models/arm_ulrf.h"
#include "/video/nba/models/arm_urrf.h"
#include "/video/nba/models/arm_llrf.h"
#include "/video/nba/models/arm_lrrf.h"
#include "/video/nba/models/hand_rrf.h"
#include "/video/nba/models/hand_lrf.h"
#include "/video/nba/models/leg_ulrf.h"
#include "/video/nba/models/leg_urrf.h"
#include "/video/nba/models/leg_llrf.h"
#include "/video/nba/models/leg_lrrf.h"
#include "/video/nba/models/foot_rrf.h"
#include "/video/nba/models/foot_lrf.h"
static LIMB *creff_limbs[] = {
	&limb_waist_rf,
	&limb_torso_rf,
	&limb_arm_urrf,
	&limb_arm_lrrf,
	&limb_hand_rrf,
	&limb_arm_ulrf,
	&limb_arm_llrf,
	&limb_hand_lrf,
	NULL,
	&limb_leg_urrf,
	&limb_leg_lrrf,
	&limb_foot_rrf,
	&limb_leg_ulrf,
	&limb_leg_llrf,
	&limb_foot_lrf
};
static texture_remap *creff_limbs_map[] = {
	&xlate_08,				/* waist */
	&xlate_48,				/* torso */
	&xlate_48,				/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	&xlate_48,				/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	&xlate_08,				/* leg UR */
	&xlate_08,				/* leg LR */
	&xlate_08,				/* foot R */
	&xlate_08,				/* leg UL */
	&xlate_08,				/* leg LL */
	&xlate_08				/* foot L */
};
/* CHEERLEADER CHARACTER */
#include "/video/nba/models/waist_cl.h"
#include "/video/nba/models/torso_cl.h"
#include "/video/nba/models/arm_ulcl.h"
#include "/video/nba/models/arm_urcl.h"
#include "/video/nba/models/arm_llcl.h"
#include "/video/nba/models/arm_lrcl.h"
#include "/video/nba/models/hand_rcl.h"
#include "/video/nba/models/hand_lcl.h"
#include "/video/nba/models/leg_ulcl.h"
#include "/video/nba/models/leg_urcl.h"
#include "/video/nba/models/leg_llcl.h"
#include "/video/nba/models/leg_lrcl.h"
#include "/video/nba/models/foot_rcl.h"
#include "/video/nba/models/foot_lcl.h"
static LIMB *cheerleader_limbs[] = {
	&limb_waist_cl,
	&limb_torso_cl,
	&limb_arm_urcl,
	&limb_arm_lrcl,
	&limb_hand_rcl,
	&limb_arm_ulcl,
	&limb_arm_llcl,
	&limb_hand_lcl,
	NULL,
	&limb_leg_urcl,
	&limb_leg_lrcl,
	&limb_foot_rcl,
	&limb_leg_ulcl,
	&limb_leg_llcl,
	&limb_foot_lcl
};
static texture_remap *cheerleader_limbs_map[] = {
	&xlate_08_40,			/* waist */
	&xlate_08_40,			/* torso */
	NULL,					/* arm UR */
	NULL,					/* arm LR */
	NULL,					/* hand R */
	NULL,					/* arm UL */
	NULL,					/* arm LL */
	NULL,					/* hand L */
	NULL,					/* head */
	NULL,					/* leg UR */
	&xlate_08_40,			/* leg LR */
	&xlate_08,				/* foot R */
	NULL,					/* leg UL */
	&xlate_08_40,			/* leg LL */
	&xlate_08				/* foot L */
};

#define JOINT_PELVIS	0
#define JOINT_TORSO		1
#define JOINT_RSHOULDER	2
#define JOINT_RELBOW	3
#define JOINT_RWRIST	4
#define JOINT_LSHOULDER	5
#define JOINT_LELBOW	6
#define JOINT_LWRIST	7
#define JOINT_NECK		8
#define JOINT_RHIP		9
#define JOINT_RKNEE		10
#define JOINT_RANKLE	11
#define JOINT_LHIP		12
#define JOINT_LKNEE		13
#define JOINT_LANKLE	14

typedef struct {
	char *name;
	char *bod_filename;
	LIMB **model;
	texture_remap **tremap;
} mascot_map;

static mascot_map name_list[] = {{"BIG_PLAYER", "big_plyr", plyr_limbs_1, plyr_limbs_1_map},
								 {"NORMAL_PLAYER", "normplyr", plyr_limbs_2, plyr_limbs_2_map},
								 {"REDUCED_BIG_PLAYER", "rb_plyr", plyr_limbs_rm, plyr_limbs_rm_map},
								 {"CHA_MASCOT", "horn", hornet_limbs, hornet_limbs_map},
								 {"CHI_MASCOT", "benny", bulls_limbs, bulls_limbs_map},
								 {"IND_MASCOT", "boome", pacers_limbs, pacers_limbs_map},
								 {"MIN_MASCOT", "crunc", timberwolves_limbs, timberwolves_limbs_map},
								 {"TOR_MASCOT", "raptr", raptors_limbs, raptors_limbs_map},
								 {"HOU_MASCOT", "hbear", rockets_limbs, rockets_limbs_map},
								 {"ATL_MASCOT", "hawk", hawks_limbs, hawks_limbs_map},
								 {"PHO_MASCOT", "goril", suns_limbs, suns_limbs_map},
								 {"NJN_MASCOT", "sly", nets_limbs, nets_limbs_map},
								 {"DEN_MASCOT", "rocky", rocky_limbs, rocky_limbs_map},
								 {"SEA_MASCOT", "sasqu", sonics_limbs, sonics_limbs_map},
								 {"UTA_MASCOT", "ubear", utah_limbs, utah_limbs_map},
								 {"ALIEN_CHARACTER", "alian", alien_limbs, alien_limbs_map},
								 {"JAM_CHARACTER", "jam", biggy_smalls_limbs, biggy_smalls_limbs_map},
								 {"NIKKO_CHARACTER", "nikko", nikko_limbs, nikko_limbs_map},
								 {"NANA_CHARACTER", "femal", nana_limbs, nana_limbs_map},
								 {"RETRO_CHARACTER", "retro", retro_limbs, retro_limbs_map},
								 {"WIZARD_CHARACTER", "wizrd", wizard_limbs, wizard_limbs_map},
								 {"OLDMAN_CHARACTER", "oman", oldman_limbs, oldman_limbs_map},
								 {"CLOWN_CHARACTER", "clown", clown_limbs, clown_limbs_map},
								 {"PUMPKIN_CHARACTER", "pumpk", pumpkin_limbs, pumpkin_limbs_map},
								 {"HORSE_CHARACTER", "horse", horse_limbs, horse_limbs_map},
								 {"FRANK_CHARACTER", "frank", frank_limbs, frank_limbs_map},
								 {"MUMMY_CHARACTER", "mummy", mummy_limbs, mummy_limbs_map},
								 {"BRIDE_CHARACTER", "bride", bride_limbs, bride_limbs_map},
								 {"WOLFMAN_CHARACTER", "wolfm", wolfm_limbs, wolfm_limbs_map},
								 {"REF_CHARACTER", "creff", creff_limbs, creff_limbs_map},
								 {"CHEERLEADER_CHARACTER", "cheer", cheerleader_limbs, cheerleader_limbs_map}
};
static int mascot_count = sizeof(name_list) / sizeof(name_list[0]);

static void usage(char *prog_name);
static int str_similiar(char *a, char *b);
static short xlate_texture(int mascot_index, int joint_index, short orig_texture);

int main(int argc, char *argv[])
{
	LIMB **mascot_model;
	LIMB *the_limb;
	char bod_filename[256];
	int mascot_index;
	int i, j;
	FILE *outfile;

	if (argc != 2)
		usage(argv[0]);
	
	mascot_index = -1;
	for (i = 0; i < mascot_count; i++)
		if (str_similiar(name_list[i].name, argv[1])) {
			mascot_index = i;
			break;
		}
	if (mascot_index == -1)
		usage(argv[0]);
	
	strcpy(bod_filename, name_list[mascot_index].bod_filename);
	strcat(bod_filename, ".bod");
	outfile = fopen(bod_filename, "wb");
	if (outfile == NULL) {
		fprintf(stderr, "could not create file \"%s\"\n", bod_filename);
		exit(1);
	}
	
	/* find the proper model to dump */
	mascot_model = name_list[mascot_index].model;
	
	for (i = JOINT_PELVIS; i <= JOINT_LANKLE; i++) {
		if (i != JOINT_NECK) {
			/* get the limb pointer */
			the_limb = mascot_model[i];
			/* write the VERTEX list */
			fwrite(&the_limb->nvtx, sizeof(the_limb->nvtx), 1, outfile);
			fwrite(the_limb->pvtx, sizeof(VERTEX), the_limb->nvtx, outfile);
			
			/* tweak the TRI list texture indices */
			/* adjust for the added ending sentil */
			the_limb->ntris++;
			for (j = 0; j < the_limb->ntris; j++)
				if (the_limb->ptri[j].texture >= 0)
					the_limb->ptri[j].texture = xlate_texture(mascot_index, i, the_limb->ptri[j].texture);
			
			/* write the TRI list */
			fwrite(&the_limb->ntris, sizeof(the_limb->ntris), 1, outfile);
			fwrite(the_limb->ptri, sizeof(TRI), the_limb->ntris, outfile);
			
			/* write the ST list */
			fwrite(&the_limb->nst, sizeof(the_limb->nst), 1, outfile);
			fwrite(the_limb->pst, sizeof(ST), the_limb->nst, outfile);
		}
	}
	fclose(outfile);
	return 0;
}	/* main */

static void usage(char *prog_name)
{
	int i;
	
	fprintf(stderr, "usage %s: [", prog_name);
	for (i = 0; i < mascot_count; i++)
		fprintf(stderr, "%s%s",  name_list[i].name, i == (mascot_count - 1) ? "" : " | ");
	fprintf(stderr, "]\n");
	exit(1);
}	/* usage */

static int str_similiar(char *a, char *b)
{
	while (*a != '\0') {
		if (toupper(*a) != toupper(*b))
			return 0;
		a++;
		b++;
	}
	return *b == '\0';
}	/* str_similiar */

static short xlate_texture(int mascot_index, int joint_index, short orig_texture)
{
	texture_remap *the_map;
	int i;
	short new_texture;
	
	new_texture = orig_texture;
	if (name_list[mascot_index].tremap != NULL) {
		the_map = name_list[mascot_index].tremap[joint_index];
		if (the_map != NULL) {
			for (i = 0; i < the_map->count; i++)
				if (the_map->orig[i] == orig_texture) {
					new_texture = the_map->new[i];
					break;
				}
		}
	}
	return new_texture;
}	/* xlate_texture */
