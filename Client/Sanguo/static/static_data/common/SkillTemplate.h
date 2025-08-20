#pragma once

#include <string>

#include "EffectEnum.h"

using namespace NS;

// 技能作用在角色身上的類型
enum SKILL_EFFECT_TYPE
{
	SKILL_EFFECT_DMG_NULL = 0,
	SKILL_EFFECT_DMG_REAL = 1,// 真實傷害
	SKILL_EFFECT_DMG_WEAPON = 2,// 直接傷害(物理)	
	SKILL_EFFECT_DMG_FIRE = 3,// 直接傷害(火)
	SKILL_EFFECT_DMG_FORST = 4,// 直接傷害(冰)
	SKILL_EFFECT_DMG_THUNDER = 5,// 直接傷害(雷)
	SKILL_EFFECT_DMG_EARTH = 6,// 直接傷害(土)
	SKILL_EFFECT_DMG_POSION = 7,// 直接傷害(毒)
	SKILL_EFFECT_DMG_LIGHT = 8,// 直接傷害(光)
	SKILL_EFFECT_DMG_DARK = 9,// 直接傷害(暗)	
	SKILL_EFFECT_BUFF = 10,// 增加Buff
	SKILL_EFFECT_SUMMON = 11,// 召喚
	SKILL_EFFECT_HP_RESTORE = 12,// HP恢復
	SKILL_EFFECT_MP_RESTORE = 13,// MP恢復
	SKILL_EFFECT_RESURRECTION = 14,// 復活	
	SKILL_EFFECT_HITBACK = 15,// 击退	
	SKILL_EFFECT_CALLSKILL = 16,// 调用技能
	SKILL_EFFECT_DMG_OXY = 17,// 扣氧气
	SKILL_EFFECT_HURTSOIL = 18,// 炸地块 
	SKILL_EFFECT_CALLSKILLDIRECT = 19,// 调用技能 跟16的区别在于 对准目标直接发射 不做随机和落地
	SKILL_EFFECT_CALLSKILLSPACE = 20,// 调用技能 跟16的区别在于 根据法球半径产生间隔 坐标不重复
	SKILL_EFFECT_WEAPONVAMPIREADD2CALLER = 21,// 物理吸血并反馈给召唤者
	SKILL_EFFECT_ELEMTVAMPIREADD2CALLER = 22,// 技能吸血并反馈给召唤者
	SKILL_EFFECT_SUMMONBEHURTIMAGE = 23,// 对目标召唤受击镜像
	SKILL_EFFECT_SUMMON_ON_TARGET_POS = 24, // 在目标位置召唤
	SKILL_EFFECT_SUCKBLOOD = 25, //主动技能吸血
	SKILL_EFFECT_MP_DEDUCE = 26,// MP扣除
	SKILL_EFFECT_COPY = 27,//复制
	SKILL_EFFECT_TRANSMIT_MAGIC_BALL = 28, // 位移法球
	SKILL_EFFECT_SUMMON_ON_FIXED_POS = 29, // 固定坐标召唤
	SKILL_EFFECT_HURTSOIL_NODROP = 30,		// 对地块伤害，无掉落 弱联网专用效果
	SKILL_EFFECT_HURT_PLANT = 31,			// 对植物伤害         弱联网专用效果
	SKILL_EFFECT_HURT_PLANT_NODROP = 32,	// 对植物伤害，无掉落 弱联网专用效果
	SKILL_EFFECT_CALLSKILL_LEFTRIGHT = 33, // 朝目标左右方向调用技能
	
	SKILL_EFFECT_PutBlock = 34, // 放置地块 弱联网专用效果
	SKILL_EFFECT_PutBiild = 35, // 放置家具 弱联网专用效果

	SKILL_EFFECT_CALLSKILL_LEFTRIGHT_POST = 36, //技能结束后,朝目标左右方向调用技能
	SKILL_EFFECT_CALLSKILL_POST = 37, //技能结束后,再施放一个法球类技能

	SKILL_EFFECT_HURTSOIL_IGNORE_STICK = 38,			// 对地块伤害，地块有掉落，无视附作物，附作物无掉落	弱联网专用效果
	SKILL_EFFECT_HURTSOIL_NODROP_IGNORE_STICK = 39,		// 对地块伤害，地块无掉落，无视附作物，附作物无掉落	弱联网专用效果

	SKILL_EFFECT_DEL_BUFF = 40, // 根据buff id删除buff
	SKILL_EFFECT_DIRECT_VAMPIRIC = 41, //直接吸血，不管伤害是多少
	SKILL_EFFECT_DEL_ALL_DEBUFF = 42, //删除所有debuff

	SKILL_EFFECT_MAX,
};

// 技能串接事件
enum SKILL_CALLSKILL_EVENT_TYPE
{
    SKILL_CALLSKILL_EVENT_NULL = 0,
    SKILL_CALLSKILL_EVENT_HIT = 1, // 擊中時觸發
};

// 技能串接 施法的位置
enum SKILL_CALLSKILL_CASTPOS_TYPE
{
    SKILL_CALLSKILL_CASTPOS_NULL = 0,
    SKILL_CALLSKILL_CASTPOS_CASTER = 1, // 原施法者	
    SKILL_CALLSKILL_CASTPOS_TRIGGERPOS = 2, // 觸發的位置
};


// 技能串接 施法的方向
enum SKILL_CALLSKILL_CASTDIR_TYPE
{
    SKILL_CALLSKILL_CASTDIR_NULL = 0,
    SKILL_CALLSKILL_CASTDIR_ORG = 1, // 原方向
    SKILL_CALLSKILL_CASTDIR_CASTER = 2, // 往施法者方向
};

enum ENUM_SKILL_TYPE
{
    SKILL_TYPE_NULL = 0,
    SKILL_TYPE_DIRECTATK = 1,	// 直擊型
    SKILL_TYPE_MAGICBALL,		// 直線法球
    SKILL_TYPE_BOOMERANG,		// 曲線法球
    SKILL_TYPE_AREA,			// 區域
    SKILL_TYPE_LINE,			// 直線
    SKILL_TYPE_CALLSKILL,		// 组合型技能
    SKILL_TYPE_ROLLMINES,		// 滚地雷 爬坡 直击
    SKILL_TYPE_TRANSVERSE,		// 横向技能 法球
    SKILL_TYPE_PARABOLA,		// 抛物线 算坐标 直击
    SKILL_TYPE_ROLLMINES_BALL,	// 滚地雷 爬坡 法球
    SKILL_TYPE_TELEPORT,		// 瞬移
    SKILL_TYPE_TRANSMITBALL,	// 位移法球 施法者会跟着法球移动 最终出现在法球消失的地方
    SKILL_TYPE_PREVIEW,			// 技能预演 用于客户端处理 效果同6 SKILL_TYPE_CALLSKILL 
    SKILL_TYPE_RANDOM_FALL,		// 随机高度坠落法球
    SKILL_TYPE_VERTICAL_BALL,	// 竖直向上 法球
    SKILL_TYPE_FORCE_TELEPORT,	// 强制瞬移
    SKILL_TYPE_HORIZON_BALL,    // 水平法球
    SKILL_TYPE_HORIZON_FRONT_FIXED_OFFSET_BALL, // 施法者前方固定偏移法球
    SKILL_TYPE_CHAIN_PREVIEW, // 连锁技能预演
    SKILL_TYPE_LINEAR_CYCLOTRON, // 直线回旋
    SKILL_TYPE_MANUAL_BALL,    // 客户端控制轨迹的法球
    SKILL_TYPE_PREVIEW_ON_POS, // 技能预演 用于客户端处理 效果同13 SKILL_TYPE_PREVIEW 
    SKILL_TYPE_SINGLE_COMBINE, // 单体组合技
	SKILL_TYPE_BALL_FOLLOW,    // 跟随法球

	SKILL_TYPE_NONE_25,        // 直接伤害 + 吸血
	SKILL_TYPE_NONE_26,        // MP减少
	SKILL_TYPE_NONE_27,        // 复制
	SKILL_TYPE_NONE_28,        // 位移
	SKILL_TYPE_NONE_29,        // 固定坐标召唤
	SKILL_TYPE_NONE_30,        // 对地块伤害(不掉落)
	SKILL_TYPE_NONE_31,        // 对植物伤害
	SKILL_TYPE_NONE_32,        // 对植物伤害（不掉落）

	SKILL_TYPE_BALL_PARABOLA = 33,  // 抛物线法球
	SKILL_TYPE_AREA_VARIABLE_WIDTH = 34, // 宽度可变区域陷阱
	SKILL_TYPE_GROUND_BOUND = 35, // 贴地飞行法球(遇到障碍或悬空消失)
	SKILL_TYPE_RADIATION = 36, //辐射状发动技能
};

//enum ENUM_TRIGGER_TYPE
//{
//    ENUM_TRIGGER_NONE = 0,
//    ENUM_TRIGGER_HPLESS = 1,	// 血量低于多少时触发
//    ENUM_TRIGGER_BEHIT = 2,		// 被命中时（计算伤害前）
//
//    ENUM_TRIGGER_BENORMALATK_DELBUFF = 4,	//受到普通攻击 移除buff 
//    ENUM_TRIGGER_BENORMALATK_CASTSKILL = 8,	//受到普通攻击 释放技能 
//    ENUM_TRIGGER_BENORMALATK_HITBACK = 16,	//受到普通攻击 直接反击
//
//    ENUM_TRIGGER_BESKILLATK_DELBUFF = 32,	//受到技能攻击 移除buff 
//    ENUM_TRIGGER_BESKILLATK_CASTSKILL = 64,	//受到技能攻击 释放技能 
//    ENUM_TRIGGER_BESKILLATK_HITBACK = 128,	//受到技能攻击 直接反击
//
//    ENUM_TRIGGER_NORMALATK_SUCKBLOOD = 256,	// 普通攻击时 吸血
//    ENUM_TRIGGER_NORMALATK_CASTSKILL = 512,	// 普通攻击时 放技能
//    ENUM_TRIGGER_SKILLATK_SUCKBLOOD = 1024,	// 技能攻击时 吸血
//    ENUM_TRIGGER_SKILLATK_CASTSKILL = 2048,	// 技能攻击时 放技能
//
//    ENUM_TRIGGER_BENORMALATK_HURTHOST = 4096,	// 受到普通攻击 伤害宿主
//    ENUM_TRIGGER_BESKILLATK_HURTHOST = 8192,	// 受到技能攻击 伤害宿主
//
//    ENUM_TRIGGER_FRIEND_DEAD = 16384, // 友方死亡触发
//    ENUM_TRIGGER_PERMANENT_INC_ATTR = 32768, // 常驻型被动(属性计算)
//};

enum ENUM_TRIGGER_TYPE
{
    ENUM_TRIGGER_NONE = 0,
    ENUM_TRIGGER_HPLESS = 1L << 0,	// 血量低于多少时触发
    ENUM_TRIGGER_BEHIT = 1L << 1,	// 被命中时（计算伤害前）

    ENUM_TRIGGER_BENORMALATK_DELBUFF = 1L << 2,	//受到普通攻击 移除buff 
    ENUM_TRIGGER_BENORMALATK_CASTSKILL = 1L << 3,	//受到普通攻击 释放技能 
    ENUM_TRIGGER_BENORMALATK_HITBACK = 1L << 4,	//受到普通攻击 直接反击

    ENUM_TRIGGER_BESKILLATK_DELBUFF = 1L << 5,	//受到技能攻击 移除buff 
    ENUM_TRIGGER_BESKILLATK_CASTSKILL = 1L << 6,	//受到技能攻击 释放技能 
    ENUM_TRIGGER_BESKILLATK_HITBACK = 1L << 7,	//受到技能攻击 直接反击

    ENUM_TRIGGER_NORMALATK_SUCKBLOOD = 1L << 8,	// 普通攻击时 吸血
    ENUM_TRIGGER_NORMALATK_CASTSKILL = 1L << 9,	// 普通攻击时 放技能
    ENUM_TRIGGER_SKILLATK_SUCKBLOOD = 1L << 10,	// 技能攻击时 吸血
    ENUM_TRIGGER_SKILLATK_CASTSKILL = 1L << 11,	// 技能攻击时 放技能

    ENUM_TRIGGER_BENORMALATK_HURTHOST = 1L << 12,	// 受到普通攻击 伤害宿主
    ENUM_TRIGGER_BESKILLATK_HURTHOST = 1L << 13,	// 受到技能攻击 伤害宿主

    ENUM_TRIGGER_FRIEND_DEAD = 1L << 14,	// 友方死亡触发
    ENUM_TRIGGER_PERMANENT_INC_ATTR = 1L << 15,	// 常驻型被动(属性计算)

    ENUM_TRIGGER_MOB_DIE = 1L << 16,	// 指定怪物死亡触发
    ENUM_TRIGGER_TARGET_BY_MOB_TYPE = 1L << 17,	// 指定怪物在身边时候
    ENUM_TRIGGER_BY_TIME = 1L << 18,	// 指定时间段触发
    ENUM_TRIGGER_BY_FALLING_DIST = 1L << 19,	// 坠落距离触发
    ENUM_TRIGGER_BY_WATER = 1L << 20,	// 液体中触发
    ENUM_TRIGGER_BY_MOVING = 1L << 21,	// 移动时触发
    ENUM_TRIGGER_BY_SPRINT = 1L << 22,	// 冲刺时触发

    ENUM_TRIGGER_NPC_PET = 1L << 23,	// 对出战的宠物和NPC使用

	ENUM_TRIGGER_16777216 = 1L << 24, // 16777216:Y轴一定深度触发（世界坐标）   client only
	ENUM_TRIGGER_33554432 = 1L << 25, // 33554432：每损失x%生命触发技能(不走CD) client only
	ENUM_TRIGGER_KILL_TARGET = 1L << 26, // 67108864 : 当击杀敌人时释放         client only
};

enum SKILL_CAST_TYPE
{
    SKILL_CAST_TYPE_NONE = 0,
    SKILL_CAST_TYPE_ACTIVE = 1,		// 主动技能
    SKILL_CAST_TYPE_PASSIVE = 2,	// 被动技能
};

// 技能作用效果
struct S_SkillTemplate_effect
{
    int32 SkillID = 0;				// 效果所屬的技能id
    SKILL_EFFECT_TYPE type = SKILL_EFFECT_DMG_NULL;		// 類型
    int32 target = 0;				// 攻击目标 1我/2敌/4友 bit位组合 对应skill_config  n_target_type             SKILL_TARGET_TYPE
	NS::I64 category = 0;             // 目标类型 (怪/npc/玩家/宠物等bit组合)        对应 n_target_category         SKILL_TARGET_CATEGORY_TYPE
    std::set<int32> setmobid{};		// 具体目标mobid 0不筛选
    int32 range = 0;				// 目標範圍	
    uint32 targetcount = 0;			// 目標數量(預設1)
    int32 hitpercent = 1000;// 触发效果概率  千分比
    std::vector<AttrEffectValue> values;	// 同時作用組別

    friend bool operator==(const S_SkillTemplate_effect& var1, const S_SkillTemplate_effect& var2)
    {
        return (var1.type == var2.type &&
            var1.target == var2.target &&
            var1.values == var2.values);
    }
    S_SkillTemplate_effect() {}
    S_SkillTemplate_effect(SKILL_EFFECT_TYPE theType, int32 theTarget) :type(theType), target(theTarget) {}
};

// 技能串接
struct S_SkillTemplate_callskill
{
    SKILL_CALLSKILL_EVENT_TYPE event;
    int32 id = 0;
    SKILL_CALLSKILL_CASTPOS_TYPE pos;
    SKILL_CALLSKILL_CASTDIR_TYPE dir;
};
