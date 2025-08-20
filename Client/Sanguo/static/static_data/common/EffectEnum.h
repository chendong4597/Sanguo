#include "msg/MsgGameDef.pb.h"
#include "BaseTypeDefine.h"


#pragma once
typedef std::map<MsgPB::ROLE_EFFECT_TYPE, float> AttrMap;
// 角色數值的參考對像
enum ATTR_REF
{
    ATTR_REF_NULL = 0,
    ATTR_REF_STATIC = 1,//固定值
    ATTR_REF_SELF_HP = 2,//自身目前血量千分比
    ATTR_REF_SELF_MP = 3,//自身目前魔力千分比
    ATTR_REF_TARGET_HP = 4,//目標目前血量千分比
    ATTR_REF_TARGET_MP = 5,//目標目前魔力千分比
    ATTR_REF_CASTER_HP = 6,//施法者目前血量千分比
    ATTR_REF_CASTER_MP = 7,//施法者目前魔力千分比

    ATTR_REF_SELF_HPM_BASE = 101,//自身基礎血量千分比
    ATTR_REF_SELF_MPM_BASE = 102,//自身基礎魔力千分比
    ATTR_REF_SELF_AD_BASE = 103,//自身基礎物理攻擊千分比
    ATTR_REF_SELF_AP_BASE = 104,//自身基礎魔法攻擊千分比
    ATTR_REF_SELF_ARM_BASE = 105,//自身基礎物理防禦千分比
    ATTR_REF_SELF_MARM_BASE = 106,//自身基礎魔法防禦千分比
    ATTR_REF_SELF_AR_BASE = 107,//自身基礎物理減免千分比
    ATTR_REF_SELF_MR_FIRE_BASE = 108,//自身基礎魔法減免火千分比
    ATTR_REF_SELF_MR_FORST_BASE = 109,//自身基礎魔法減免冰千分比
    ATTR_REF_SELF_MR_THUNDER_BASE = 110,//自身基礎魔法減免雷千分比
    ATTR_REF_SELF_MR_EARTH_BASE = 111,//自身基礎魔法減免土千分比
    ATTR_REF_SELF_MR_POSION_BASE = 112,//自身基礎魔法減免毒千分比
    ATTR_REF_SELF_MR_LIGHT_BASE = 113,//自身基礎魔法減免光千分比
    ATTR_REF_SELF_MR_DARK_BASE = 114,//自身基礎魔法減免暗千分比
    ATTR_REF_SELF_ARP_BASE = 115,//自身基礎物裡穿透千分比
    ATTR_REF_SELF_MRP_BASE = 116,//自身基礎魔法穿透千分比
    ATTR_REF_SELF_CRI_BASE = 117,//自身基礎物理爆擊率千分比
    ATTR_REF_SELF_MCRI_BASE = 118,//自身基礎魔法爆擊率千分比
    ATTR_REF_SELF_CRID_BASE = 119,//自身基礎物理爆擊增傷千分比
    ATTR_REF_SELF_MCRID_BASE = 120,//自身基礎魔法爆擊增傷千分比
    ATTR_REF_SELF_RES_BASE = 121,//自身基礎物裡韌性千分比
    ATTR_REF_SELF_MRES_BASE = 122,//自身基礎魔法韌性千分比
    ATTR_REF_SELF_CC_HIT_BASE = 123,//自身基礎控制命中千分比
    ATTR_REF_SELF_CC_RES_BASE = 124,//自身基礎控制抵抗千分比
    ATTR_REF_SELF_ATK_SP_BASE = 125,//自身基礎攻速千分比
    ATTR_REF_SELF_MV_SP_BASE = 126,//自身基礎移動速度	千分比
    ATTR_REF_SELF_HB_BASE = 127,//自身基礎擊退千分比	
    ATTR_REF_SELF_HPR_BASE = 128,//自身基礎擊生命回復
    ATTR_REF_SELF_HPRS_BASE = 129,//自身基礎擊生命回復速度
    ATTR_REF_SELF_MPR_BASE = 130,//自身基礎擊魔法回復
    ATTR_REF_SELF_MPRS_BASE = 131,//自身基礎擊魔法回復速度
    ATTR_REF_SELF_GATHER_ATK_BASE = 132,//自身基礎擊採集攻擊力
    ATTR_REF_SELF_JUMP_BASE = 133,//自身基礎跳跃力
    ATTR_REF_SELF_SWIM_BASE = 134,//自身基礎游泳速度
    ATTR_REF_SELF_ATK_RANGE_BASE = 135,//自身基礎攻击距离
    ATTR_REF_SELF_GATHER_SP_BASE = 136,//自身基礎採集速度
    ATTR_REF_SELF_CHASE_BASE = 137,//自身基礎追击速度
    ATTR_REF_SELF_BOSS_HURT_BASE = 138,//自身基礎boss伤害加成
    ATTR_REF_SELF_BOSS_DMGREDUC_BASE = 139,//自身基礎受到boss伤害减免
    ATTR_REF_SELF_JUMP_INIT_SPEED_BASE = 140,//自身基础跳跃初速度
	ATTR_REF_SELF_TOTAL_DAMAGE_INC_BASE = 141, //全伤害增伤
	ATTR_REF_SELF_DEBUFF_DAMAGE_INC = 142, //目标debuff数增伤
    ATTR_REF_SELF_BASE_MAX,

    ATTR_REF_SELF_HPM_NOW = 201,//自身(最大)血量千分比
    ATTR_REF_SELF_MPM_NOW = 202,//自身(最大)魔力千分比
    ATTR_REF_SELF_AD_NOW = 203,//自身當前物理攻擊	千分比
    ATTR_REF_SELF_AP_NOW = 204,//自身當前魔法攻擊	千分比
    ATTR_REF_SELF_ARM_NOW = 205,//自身當前物理防禦	千分比
    ATTR_REF_SELF_MARM_NOW = 206,//自身當前魔法防禦	千分比
    ATTR_REF_SELF_AR_NOW = 207,//自身當前物理減免	千分比
    ATTR_REF_SELF_MR_FIRE_NOW = 208,//自身當前魔法減免火千分比
    ATTR_REF_SELF_MR_FORST_NOW = 209,//自身當前魔法減免冰千分比
    ATTR_REF_SELF_MR_THUNDER_NOW = 210,//自身當前魔法減免雷千分比
    ATTR_REF_SELF_MR_EARTH_NOW = 211,//自身當前魔法減免土千分比
    ATTR_REF_SELF_MR_POSION_NOW = 212,//自身當前魔法減免毒千分比	
    ATTR_REF_SELF_MR_LIGHT_NOW = 213,//自身當前魔法減免光千分比	
    ATTR_REF_SELF_MR_DARK_NOW = 214,//自身當前魔法減免暗千分比	
    ATTR_REF_SELF_ARP_NOW = 215,//自身當前物裡穿透	千分比
    ATTR_REF_SELF_MRP_NOW = 216,//自身當前魔法穿透	千分比
    ATTR_REF_SELF_CRI_NOW = 217,//自身當前物理爆擊率千分比	
    ATTR_REF_SELF_MCRI_NOW = 218,//自身當前魔法爆擊率千分比	
    ATTR_REF_SELF_CRID_NOW = 219,//自身當前物理爆擊增傷千分比	
    ATTR_REF_SELF_MCRID_NOW = 220,//自身當前魔法爆擊增傷千分比	
    ATTR_REF_SELF_RES_NOW = 221,//自身當前物裡韌性千分比
    ATTR_REF_SELF_MRES_NOW = 222,//自身當前魔法韌性千分比	
    ATTR_REF_SELF_CC_HIT_NOW = 223,//自身當前控制命中	千分比
    ATTR_REF_SELF_CC_RES_NOW = 224,//自身當前控制抵抗千分比	
    ATTR_REF_SELF_ATK_SP_NOW = 225,//自身當前攻速千分比	
    ATTR_REF_SELF_MV_SP_NOW = 226,//自身當前移動速度	千分比
    ATTR_REF_SELF_HB_NOW = 227,//自身當前擊退千分比
    ATTR_REF_SELF_HPR_NOW = 228,//自身當前擊生命回復
    ATTR_REF_SELF_HPRS_NOW = 229,//自身當前擊生命回復速度
    ATTR_REF_SELF_MPR_NOW = 230,//自身當前擊魔法回復
    ATTR_REF_SELF_MPRS_NOW = 231,//自身當前擊魔法回復速度
    ATTR_REF_SELF_GATHER_ATK_NOW = 232,//自身當前擊採集攻擊力
    ATTR_REF_SELF_JUMP_NOW = 233,//自身當前跳跃力
    ATTR_REF_SELF_SWIM_NOW = 234,//自身當前游泳速度
    ATTR_REF_SELF_ATK_RANGE_NOW = 235,//自身當前攻击距离
    ATTR_REF_SELF_GATHER_SP_NOW = 236,//自身當前採集速度
    ATTR_REF_SELF_CHASE_NOW = 237,//自身當前追击速度
    ATTR_REF_SELF_BOSS_HURT_NOW = 238,//自身當前boss伤害加成
    ATTR_REF_SELF_BOSS_DMGREDUC_NOW = 239,//自身當前受到boss伤害减免
    ATTR_REF_SELF_JUMP_INIT_SPEED_NOW = 240,//自身当前跳跃初速度
	ATTR_REF_SELF_TOTAL_DAMAGE_INC_NOW = 241, //全伤害增伤
	ATTR_REF_SELF_DEBUFF_DAMAGE_INC_NOW = 242, //目标debuff数增伤
    ATTR_REF_SELF_NOW_MAX,

    ATTR_REF_TARGET_HPM_BASE = 301,//目標基礎血量千分比
    ATTR_REF_TARGET_MPM_BASE = 302,//目標基礎魔力千分比
    ATTR_REF_TARGET_AD_BASE = 303,//目標基礎物理攻擊千分比
    ATTR_REF_TARGET_AP_BASE = 304,//目標基礎魔法攻擊千分比
    ATTR_REF_TARGET_ARM_BASE = 305,//目標基礎物理防禦千分比
    ATTR_REF_TARGET_MARM_BASE = 306,//目標基礎魔法防禦千分比
    ATTR_REF_TARGET_AR_BASE = 307,//目標基礎物理減免千分比
    ATTR_REF_TARGET_MR_FIRE_BASE = 308,//目標基礎魔法減免火千分比
    ATTR_REF_TARGET_MR_FORST_BASE = 309,//目標基礎魔法減免冰千分比
    ATTR_REF_TARGET_MR_THUNDER_BASE = 310,//目標基礎魔法減免雷千分比
    ATTR_REF_TARGET_MR_EARTH_BASE = 311,//目標基礎魔法減免土千分比
    ATTR_REF_TARGET_MR_POSION_BASE = 312,//目標基礎魔法減免毒千分比
    ATTR_REF_TARGET_MR_LIGHT_BASE = 313,//目標基礎魔法減免光千分比
    ATTR_REF_TARGET_MR_DARK_BASE = 314,//目標基礎魔法減免暗千分比
    ATTR_REF_TARGET_ARP_BASE = 315,//目標基礎物裡穿透千分比
    ATTR_REF_TARGET_MRP_BASE = 316,//目標基礎魔法穿透千分比
    ATTR_REF_TARGET_CRI_BASE = 317,//目標基礎物理爆擊率千分比
    ATTR_REF_TARGET_MCRI_BASE = 318,//目標基礎魔法爆擊率千分比
    ATTR_REF_TARGET_CRID_BASE = 319,//目標基礎物理爆擊增傷千分比
    ATTR_REF_TARGET_MCRID_BASE = 320,//目標基礎魔法爆擊增傷千分比
    ATTR_REF_TARGET_RES_BASE = 321,//目標基礎物裡韌性千分比
    ATTR_REF_TARGET_MRES_BASE = 322,//目標基礎魔法韌性千分比
    ATTR_REF_TARGET_CC_HIT_BASE = 323,//目標基礎控制命中千分比
    ATTR_REF_TARGET_CC_RES_BASE = 324,//目標基礎控制抵抗千分比
    ATTR_REF_TARGET_ATK_SP_BASE = 325,//目標基礎攻速千分比	
    ATTR_REF_TARGET_MV_SP_BASE = 326,//目標基礎移動速度	千分比
    ATTR_REF_TARGET_HB_BASE = 327,//目標基礎擊退千分比	
    ATTR_REF_TARGET_HPR_BASE = 328,//目標基礎擊生命回復
    ATTR_REF_TARGET_HPRS_BASE = 329,//目標基礎擊生命回復速度
    ATTR_REF_TARGET_MPR_BASE = 330,//目標基礎擊魔法回復
    ATTR_REF_TARGET_MPRS_BASE = 331,//目標基礎擊魔法回復速度
    ATTR_REF_TARGET_GATHER_ATK_BASE = 332,//目標基礎擊採集攻擊力
    ATTR_REF_TARGET_JUMP_BASE = 333,//目標基礎跳跃力
    ATTR_REF_TARGET_SWIM_BASE = 334,//目標基礎游泳速度
    ATTR_REF_TARGET_ATK_RANGE_BASE = 335,//目標基礎攻击距离
    ATTR_REF_TARGET_GATHER_SP_BASE = 336,//目標基礎採集速度
    ATTR_REF_TARGET_CHASE_BASE = 337,//目標基礎追击速度
    ATTR_REF_TARGET_BOSS_HURT_BASE = 338,//目標基礎boss伤害加成
    ATTR_REF_TARGET_BOSS_DMGREDUC_BASE = 339,//目標基礎受到boss伤害减免
    ATTR_REF_TARGET_JUMP_INIT_SPEED_BASE = 340,//目標基礎跳跃初速度
	ATTR_REF_TARGET_TOTAL_DAMAGE_INC_BASE = 341, //全伤害增伤
	ATTR_REF_TARGET_DEBUFF_DAMAGE_INC_BASE = 342, //目标debuff数增伤
    ATTR_REF_TARGET_BASE_MAX,

    ATTR_REF_TARGET_HPM_NOW = 401,//目標(最大)血量千分比
    ATTR_REF_TARGET_MPM_NOW = 402,//目標(最大)魔力千分比
    ATTR_REF_TARGET_AD_NOW = 403,//目標物理攻擊千分比
    ATTR_REF_TARGET_AP_NOW = 404,//目標魔法攻擊千分比
    ATTR_REF_TARGET_ARM_NOW = 405,//目標物理防禦千分比
    ATTR_REF_TARGET_MARM_NOW = 406,//目標魔法防禦千分比
    ATTR_REF_TARGET_AR_NOW = 407,//目標物理減免千分比
    ATTR_REF_TARGET_MR_FIRE_NOW = 408,//目標魔法減免火千分比	
    ATTR_REF_TARGET_MR_FORST_NOW = 409,//目標魔法減免冰千分比	
    ATTR_REF_TARGET_MR_THUNDER_NOW = 410,//目標魔法減免雷千分比	
    ATTR_REF_TARGET_MR_EARTH_NOW = 411,//目標魔法減免土千分比
    ATTR_REF_TARGET_MR_POSION_NOW = 412,//目標魔法減免毒千分比	
    ATTR_REF_TARGET_MR_LIGHT_NOW = 413,//目標魔法減免光千分比	
    ATTR_REF_TARGET_MR_DARK_NOW = 414,//目標魔法減免暗千分比	
    ATTR_REF_TARGET_ARP_NOW = 415,//目標物裡穿透千分比
    ATTR_REF_TARGET_MRP_NOW = 416,//目標魔法穿透千分比
    ATTR_REF_TARGET_CRI_NOW = 417,//目標物理爆擊率千分比	
    ATTR_REF_TARGET_MCRI_NOW = 418,//目標魔法爆擊率千分比	
    ATTR_REF_TARGET_CRID_NOW = 419,//目標物理爆擊增傷千分比	
    ATTR_REF_TARGET_MCRID_NOW = 420,//目標魔法爆擊增傷千分比	
    ATTR_REF_TARGET_RES_NOW = 421,//目標物裡韌性千分比	
    ATTR_REF_TARGET_MRES_NOW = 422,//目標魔法韌性千分比	
    ATTR_REF_TARGET_CC_HIT_NOW = 423,//目標控制命中千分比	
    ATTR_REF_TARGET_CC_RES_NOW = 424,//目標控制抵抗千分比		
    ATTR_REF_TARGET_ATK_SP_NOW = 425,//目標攻速千分比	
    ATTR_REF_TARGET_MV_SP_NOW = 426,//目標移動速度千分比	
    ATTR_REF_TARGET_HB_NOW = 427,//目標擊退千分比
    ATTR_REF_TARGET_HPR_NOW = 428,//目標當前生命回復
    ATTR_REF_TARGET_HPRS_NOW = 429,//目標當前生命回復速度
    ATTR_REF_TARGET_MPR_NOW = 430,//目標當前魔法回復
    ATTR_REF_TARGET_MPRS_NOW = 431,//目標當前魔法回復速度
    ATTR_REF_TARGET_GATHER_ATK_NOW = 432,//目標當前採集攻擊力
    ATTR_REF_TARGET_JUMP_NOW = 433,//目標當前跳跃力
    ATTR_REF_TARGET_SWIM_NOW = 434,//目標當前游泳速度
    ATTR_REF_TARGET_ATK_RANGE_NOW = 435,//目標當前攻击距离
    ATTR_REF_TARGET_GATHER_SP_NOW = 436,//目標當前採集速度
    ATTR_REF_TARGET_CHASE_NOW = 437,//目標當前追击速度
    ATTR_REF_TARGET_BOSS_HURT_NOW = 438,//目標當前boss伤害加成
    ATTR_REF_TARGET_BOSS_DMGREDUC_NOW = 439,//目標當前受到boss伤害减免
    ATTR_REF_TARGET_JUMP_INIT_SPEED_NOW = 440,//目標当前跳跃初速度
	ATTR_REF_TARGET_TOTAL_DAMAGE_INC_NOW = 441, //全伤害增伤
	ATTR_REF_TARGET_DEBUFF_DAMAGE_INC_NOW = 442, //目标debuff数增伤
    ATTR_REF_TARGET_NOW_MAX,

    ATTR_REF_CASTER_HP_BASE = 501,//施法者基礎血量千分比
    ATTR_REF_CASTER_MP_BASE = 502,//施法者基礎魔力千分比
    ATTR_REF_CASTER_AP_BASE = 504,//施法者基礎魔法攻擊千分比
    ATTR_REF_CASTER_AD_BASE = 503,//施法者基礎物理攻擊千分比
    ATTR_REF_CASTER_ARM_BASE = 505,//施法者基礎物理防禦千分比
    ATTR_REF_CASTER_MARM_BASE = 506,//施法者基礎魔法防禦千分比
    ATTR_REF_CASTER_AR_BASE = 507,//施法者基礎物理減免千分比
    ATTR_REF_CASTER_MR_FIRE_BASE = 508,//施法者基礎魔法減免火千分比
    ATTR_REF_CASTER_MR_FORST_BASE = 509,//施法者基礎魔法減免冰千分比
    ATTR_REF_CASTER_MR_THUNDER_BASE = 510,//施法者基礎魔法減免雷千分比
    ATTR_REF_CASTER_MR_EARTH_BASE = 511,//施法者基礎魔法減免土千分比
    ATTR_REF_CASTER_MR_POSION_BASE = 512,//施法者基礎魔法減免毒千分比
    ATTR_REF_CASTER_MR_LIGHT_BASE = 513,//施法者基礎魔法減免光千分比
    ATTR_REF_CASTER_MR_DARK_BASE = 514,//施法者基礎魔法減免暗千分比
    ATTR_REF_CASTER_ARP_BASE = 515,//施法者基礎物裡穿透千分比
    ATTR_REF_CASTER_MRP_BASE = 516,//施法者基礎魔法穿透千分比
    ATTR_REF_CASTER_CRI_BASE = 517,//施法者基礎物理爆擊率千分比
    ATTR_REF_CASTER_MCRI_BASE = 518,//施法者基礎魔法爆擊率千分比
    ATTR_REF_CASTER_CRID_BASE = 519,//施法者基礎物理爆擊增傷千分比
    ATTR_REF_CASTER_MCRID_BASE = 520,//施法者基礎魔法爆擊增傷千分比
    ATTR_REF_CASTER_RES_BASE = 521,//施法者基礎物裡韌性千分比
    ATTR_REF_CASTER_MRES_BASE = 522,//施法者基礎魔法韌性千分比
    ATTR_REF_CASTER_CC_HIT_BASE = 523,//施法者基礎控制命中千分比
    ATTR_REF_CASTER_CC_RES_BASE = 524,//施法者基礎控制抵抗千分比
    ATTR_REF_CASTER_ATK_SP_BASE = 525,//施法者基礎攻速千分比
    ATTR_REF_CASTER_MV_SP_BASE = 526,//施法者基礎移動速度千分比	
    ATTR_REF_CASTER_HB_BASE = 527,//施法者基礎擊退千分比	
    ATTR_REF_CASTER_HPR_BASE = 528,//施法者基礎擊生命回復
    ATTR_REF_CASTER_HPRS_BASE = 529,//施法者基礎擊生命回復速度
    ATTR_REF_CASTER_MPR_BASE = 530,//施法者基礎擊魔法回復
    ATTR_REF_CASTER_MPRS_BASE = 531,//施法者基礎擊魔法回復速度
    ATTR_REF_CASTER_GATHER_ATK_BASE = 532,//施法者基礎採集攻擊力
    ATTR_REF_CASTER_JUMP_BASE = 533,//施法者基礎跳跃力
    ATTR_REF_CASTER_SWIM_BASE = 534,//施法者基礎游泳速度
    ATTR_REF_CASTER_ATK_RANGE_BASE = 535,//施法者基礎攻击距离
    ATTR_REF_CASTER_GATHER_SP_BASE = 536,//施法者基礎採集速度
    ATTR_REF_CASTER_CHASE_BASE = 537,//施法者基礎追击速度
    ATTR_REF_CASTER_BOSS_HURT_BASE = 538,//施法者基礎boss伤害加成
    ATTR_REF_CASTER_BOSS_DMGREDUC_BASE = 539,//施法者基礎受到boss伤害减免
    ATTR_REF_CASTER_JUMP_INIT_SPEED_BASE = 540,//施法者基礎跳跃初速度
	ATTR_REF_CASTER_TOTAL_DAMAGE_INC_BASE = 541, //全伤害增伤
	ATTR_REF_CASTER_DEBUFF_DAMAGE_INC_BASE = 542, //目标debuff数增伤
    ATTR_REF_CASTER_BASE_MAX,

    ATTR_REF_CASTER_HP_NOW = 601,//施法者(最大)血量千分比
    ATTR_REF_CASTER_MP_NOW = 602,//施法者(最大)魔力千分比
    ATTR_REF_CASTER_AD_NOW = 603,//施法者當前物理攻擊千分比
    ATTR_REF_CASTER_AP_NOW = 604,//施法者當前魔法攻擊千分比
    ATTR_REF_CASTER_ARM_NOW = 605,//施法者當前物理防禦千分比
    ATTR_REF_CASTER_MARM_NOW = 606,//施法者當前魔法防禦千分比
    ATTR_REF_CASTER_AR_NOW = 607,//施法者當前物理減免千分比
    ATTR_REF_CASTER_MR_FIRE_NOW = 608,//施法者當前魔法減免火千分比
    ATTR_REF_CASTER_MR_FORST_NOW = 609,//施法者當前魔法減免冰千分比
    ATTR_REF_CASTER_MR_THUNDER_NOW = 610,//施法者當前魔法減免雷千分比	
    ATTR_REF_CASTER_MR_EARTH_NOW = 611,//施法者當前魔法減免土千分比	
    ATTR_REF_CASTER_MR_POSION_NOW = 612,//施法者當前魔法減免毒千分比	
    ATTR_REF_CASTER_MR_LIGHT = 613,//施法者當前魔法減免光千分比	
    ATTR_REF_CASTER_MR_DARK_NOW = 614,//施法者當前魔法減免暗千分比	
    ATTR_REF_CASTER_ARP_NOW = 615,//施法者當前物裡穿透千分比	
    ATTR_REF_CASTER_MRP_NOW = 616,//施法者當前魔法穿透千分比	
    ATTR_REF_CASTER_CRI_NOW = 617,//施法者當前物理爆擊率千分比	
    ATTR_REF_CASTER_MCRI_NOW = 618,//施法者當前魔法爆擊率千分比	
    ATTR_REF_CASTER_CRID_NOW = 619,//施法者當前物理爆擊增傷千分比	
    ATTR_REF_CASTER_MCRID_NOW = 620,//施法者當前魔法爆擊增傷千分比	
    ATTR_REF_CASTER_RES_NOW = 621,//施法者當前物裡韌性千分比	
    ATTR_REF_CASTER_MRES_NOW = 622,//施法者當前魔法韌性千分比	
    ATTR_REF_CASTER_CC_HIT_NOW = 623,//施法者當前控制命中千分比
    ATTR_REF_CASTER_CC_RES_NOW = 624,//施法者當前控制抵抗千分比		
    ATTR_REF_CASTER_ATK_SP_NOW = 625,//施法者當前攻速千分比	
    ATTR_REF_CASTER_MV_SP_NOW = 626,//施法者當前移動速度千分比	
    ATTR_REF_CASTER_HB_NOW = 627,//施法者當前擊退千分比
    ATTR_REF_CASTER_HPR_NOW = 628,//施法者當前擊生命回復
    ATTR_REF_CASTER_HPRS_NOW = 629,//施法者當前擊生命回復速度
    ATTR_REF_CASTER_MPR_NOW = 630,//施法者當前擊魔法回復
    ATTR_REF_CASTER_MPRS_NOW = 631,//施法者當前擊魔法回復速度
    ATTR_REF_CASTER_GATHER_ATK_NOW = 632,//施法者當前擊採集攻擊力
    ATTR_REF_CASTER_JUMP_NOW = 633,//施法者當前跳跃力
    ATTR_REF_CASTER_SWIM_NOW = 634,//施法者當前游泳速度
    ATTR_REF_CASTER_ATK_RANGE_NOW = 635,//施法者當前攻击距离
    ATTR_REF_CASTER_GATHER_SP_NOW = 636,//施法者當前採集速度 
    ATTR_REF_CASTER_CHASE_NOW = 637,//施法者當前追击速度
    ATTR_REF_CASTER_BOSS_HURT_NOW = 638,//施法者當前boss伤害加成
    ATTR_REF_CASTER_BOSS_DMGREDUC_NOW = 639,//施法者當前受到boss伤害减免
    ATTR_REF_CASTER_JUMP_INIT_SPEED_NOW = 640,//施法者当前跳跃初速度
	ATTR_REF_CASTER_TOTAL_DAMAGE_INC_NOW = 641, //全伤害增伤
	ATTR_REF_CASTER_DEBUFF_DAMAGE_INC_NOW = 642, //目标debuff数增伤
    ATTR_REF_CASTER_NOW_MAX,

    ATTR_REF_MAX,
};

// Buff效果
enum BUFF_EFFECT_TYPE
{
	BUFF_EFFECT_NULL = 1000,
	BUFF_EFFECT_REAL_DOT = 1001,	// 真實傷害Dot
	BUFF_EFFECT_FIRE_DOT = 1002,	// 火傷害Dot
	BUFF_EFFECT_FORST_DOT = 1003, // 冰傷害Dot
	BUFF_EFFECT_THUNDER_DOT = 1004,	// 雷傷害Dot
	BUFF_EFFECT_EARTH_DOT = 1005,	// 土傷害Dot
	BUFF_EFFECT_POSION_DOT = 1006,	// 毒傷害Dot
	BUFF_EFFECT_LIGHT_DOT = 1007,	// 光傷害Dot
	BUFF_EFFECT_OXY_DOT = 1008,	// 扣氧气Dot
	BUFF_EFFECT_WEAPON_DOT = 1009,	// 物理傷害Dot
	BUFF_EFFECT_DARK_DOT = 1010,	// 暗傷害Dot
	BUFF_EFFECT_HP_RESTORE = 1011,	// 回恢HP
	BUFF_EFFECT_MP_RESTORE = 1012,	// 回恢MP
	BUFF_EFFECT_ROLE_STATE = 1013, // 角色狀態改變
	BUFF_EFFECT_SHIELD = 1014,	// 全伤害护盾
	BUFF_EFFECT_REAL_SHIELD = 1015,	// 真实伤害护盾
	BUFF_EFFECT_FIRE_SHIELD = 1016,	// 火傷害护盾
	BUFF_EFFECT_FORST_SHIELD = 1017,	// 冰傷害护盾
	BUFF_EFFECT_THUNDER_SHIELD = 1018,	// 雷傷害护盾
	BUFF_EFFECT_EARTH_SHIELD = 1019,	// 土傷害护盾
	BUFF_EFFECT_POSION_SHIELD = 1020,	// 毒傷害护盾
	BUFF_EFFECT_LIGHT_SHIELD = 1021,	// 光傷害护盾
	BUFF_EFFECT_WEAPON_SHIELD = 1022,	// 物理傷害护盾
	BUFF_EFFECT_DARK_SHIELD = 1023,		// 暗傷害护盾
	BUFF_EFFECT_MP_DEDUCT = 1024,		// 扣除MP
	BUFF_EFFECT_CAST_SKILL_ON_DEAD = 1025, // 死亡时在原地发动技能
	BUFF_EFFECT_HEAL_MEMBER_ON_UNDER_ATTACK = 1026, // 被攻击时治疗友军
	BUFF_EFFECT_CAST_SKILL_IN_OWNER_POS = 1027, // 在buff拥有者的位置上释放技能
	BUFF_EFFECT_FACE_REDUCE_HURT = 1028,// 正面朝向减伤
	BUFF_EFFECT_PHY_HIT_BACK = 1029, // 物理反伤
	BUFF_EFFECT_ELEM_HIT_BACK = 1030, // 元素反伤
	BUFF_EFFECT_TOTAL_HIT_BACK = 1031, // 全面反伤
	BUFF_EFFECT_REDUCE_NEGATIVE_EFFECT_BY_RATE = 1032, // 按比例（千分比）降低负面效应影响
	BUFF_EFFECT_REDUCE_HURT_BY_SKILL = 1033, //按比例降低指定技能伤害
	BUFF_EFFECT_TOTAL_DAMAGE_INC_RATE = 1034, //全伤害增伤加成(千分比)
	BUFF_EFFECT_CAST_SKILL_WHEN_OBJ_IN_VIEW = 1035, //地图对象进入视野时,对该对象发动技能
	BUFF_EFFECT_CAST_SKILL_WHEN_OBJ_OUT_VIEW = 1036, //地图对象离开视野时,对该对象发动技能
	BUFF_EFFECT_MARKING = 1037, //印记[buff堆叠X层后触发技能(技能施法者为Buff拥有者)，触发后自动移除buff，buff持续时间结束移除全部层数buff]
	BUFF_EFFECT_ADD_PASSIVE_SKILL = 1038,//添加被动技能
	BUFF_EFFECT_BUFF_IMMUNE = 1039, //免疫某个buff
	BUFF_EFFECT_BUFF_KILL_MOB_DROP = 1040, // 精英怪以下怪物掉落增加掉落物品
    BUFF_EFFECT_MAX,
};

using namespace MsgPB;

//////////////////////////////////////////////////////////////////////////////
// 角色屬性效果值
struct AttrEffectValue
{
    ATTR_REF attrref; // 要參考的角色屬性
    NS::int32	 value;
    friend bool operator==(const AttrEffectValue& var1, const AttrEffectValue& var2)
    {
        return (var1.attrref == var2.attrref &&
            var1.value == var2.value);
    }
};

class CEffectSum
{
public:
    typedef std::map<ATTR_REF, NS::int32> MAP_ATTRREF;
    typedef std::map<MsgPB::ROLE_EFFECT_TYPE, MAP_ATTRREF> MAP_EFFECT;

public:
	CEffectSum() = default;
	~CEffectSum() = default;

public:
    void CleanUp() { m_mapEffect.clear(); }

    void AddEffect(MsgPB::ROLE_EFFECT_TYPE eType, const std::vector<AttrEffectValue>& values, NS::int32 EffectRate, NS::int32 reduceNegativeEffectRate = 0)
    {
        if (eType == MsgPB::ROLE_EFFECT_NULL)
            return;

        MAP_ATTRREF& mapAttrRef = m_mapEffect[eType];

        for (const auto& pEffectValue : values)
        {
            auto it = mapAttrRef.find(pEffectValue.attrref);
			NS::int32& nValue = mapAttrRef[pEffectValue.attrref];
            if (it == mapAttrRef.end())
                nValue = 0;
            int EffectValue = pEffectValue.value;
            if (eType == MsgPB::ROLE_EFFECT_SKILL_STATUS)
            {
                nValue |= EffectValue;
            }
            else
            {
                if (EffectRate != 0)
                {
                    EffectValue = NS::int32(EffectValue * (double(EffectRate) / 1000.0f));
                }
                if (EffectValue < 0 && reduceNegativeEffectRate > 0)
                {
                    // 降低负面影响
                    EffectValue = NS::int32(EffectValue * (double(1000 - reduceNegativeEffectRate) / 1000.0f));
                }
                nValue += EffectValue;
            }

        } // End for
    }

    void AddEffect(MsgPB::ROLE_EFFECT_TYPE eType, const AttrEffectValue& value, NS::int32 EffectRate)
    {
        if (eType == MsgPB::ROLE_EFFECT_NULL)
            return;

        MAP_ATTRREF& mapAttrRef = m_mapEffect[eType];
        auto it = mapAttrRef.find(value.attrref);

		NS::int32& nValue = mapAttrRef[value.attrref];
        if (it == mapAttrRef.end())
            nValue = 0;

        int EffectValue = value.value;
        if (eType == MsgPB::ROLE_EFFECT_SKILL_STATUS)
        {
            nValue |= EffectValue;
        }
        else
        {
            if (EffectRate != 0)
                EffectValue = NS::int32(EffectValue * (double(EffectRate) / 1000.0f));
            nValue += EffectValue;
        }
    }

	//从施法者身上加属性
	void AddEffectByCaster(MsgPB::ROLE_EFFECT_TYPE eType, const std::vector<AttrEffectValue>& values, NS::int32 EffectRate, const AttrMap& casterAttr, NS::int32 reduceNegativeEffectRate = 0)
	{
		if (eType == MsgPB::ROLE_EFFECT_NULL)
			return;

		MAP_ATTRREF& mapAttrRef = m_mapEffect[eType];

		for (const auto& pEffectValue : values)
		{
			if (pEffectValue.attrref >= ATTR_REF_CASTER_HP_BASE && pEffectValue.attrref < ATTR_REF_CASTER_BASE_MAX && casterAttr.size() > 0)
			{
				float addValue{ 0 };
				addValue = calculateAddValue(pEffectValue.attrref, getEffectTypeValue(eType, const_cast<AttrMap&>(casterAttr)), static_cast<float>(pEffectValue.value));
				auto it = mapAttrRef.find(ATTR_REF_STATIC);
				NS::int32& nValue = mapAttrRef[ATTR_REF_STATIC];
				if (it == mapAttrRef.end())
					nValue = 0;
				nValue += (int)addValue;
			}
		} // End for
	}

	CEffectSum& operator+=(const CEffectSum& rhs)
	{
		for (auto& kv_rhs : rhs.m_mapEffect)
		{
			const MsgPB::ROLE_EFFECT_TYPE eType = kv_rhs.first;
			const MAP_ATTRREF& mapAttrRef_rhs = kv_rhs.second;
			
			MAP_ATTRREF& mapAttrRef = m_mapEffect[eType];

			for (auto& kv2_rhs : mapAttrRef_rhs)
			{
				const ATTR_REF attr = kv2_rhs.first;

				NS::int32& nValue = mapAttrRef[attr];

				if (eType == MsgPB::ROLE_EFFECT_SKILL_STATUS)
					nValue |= kv2_rhs.second;
				else
					nValue += kv2_rhs.second;
			}
		}

		return *this;
	}

    const MAP_EFFECT& GetEffects() const { return m_mapEffect; }
    MAP_EFFECT& GetModifyEffects() { return m_mapEffect; }

private:
	MAP_EFFECT m_mapEffect{};
	static float calculateAddValue(ATTR_REF eAttrRef, float nRefTarget, float nRefValue = 0)
	{
		return (eAttrRef == ATTR_REF_STATIC) ? nRefValue : nRefTarget * nRefValue / 1000;
	};
	static float getEffectTypeValue(MsgPB::ROLE_EFFECT_TYPE eType, const AttrMap& attrs)
	{
		auto findId = attrs.find((eType > 100) ? static_cast<MsgPB::ROLE_EFFECT_TYPE>(eType % 100) : eType);
		return (findId == attrs.end()) ? 0.0f : findId->second;
	};
};