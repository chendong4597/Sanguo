#pragma once

#include "SkillRoleValue.h"

namespace skill
{
    enum ENUM_MAGICOBJECT_TYPE
    {
        ENUM_MAGICOBJECT_NULL = 0,
        ENUM_MAGICOBJECT_BALL = 1,	// 直線法球
        ENUM_MAGICOBJECT_CURVEBALL = 2,	// 曲線法球
        ENUM_MAGICOBJECT_AREA = 3,	// 區域
        ENUM_MAGICOBJECT_LINE = 4,	// 直線
        ENUM_MAGICOBJECT_BALL_LINEARCYCLOTRON = 5, //直线回旋
        ENUM_MAGICOBJECT_MANUAL_BALL = 6, //客户端控制轨迹的法球
		ENUM_MAGICOBJECT_FOLLOW = 7, //跟随施法者
		ENUM_MAGICOBJECT_PARABOLA = 8, //抛物线法球
		ENUM_MAGICOBJECT_AREA_VARIABLE_WID = 9, // 可变宽区域陷阱
		ENUM_MAGICOBJECT_GROUND_BOUND = 10, // 贴地飞行法球(遇到障碍或悬空消失)
    };

    enum ENUM_THROUGH_TYPE
    {
        ENUM_THROUGH_NULL = 0,		// 都不穿
        ENUM_THROUGH_OBSTACLE = 1,	// 穿墙
        ENUM_THROUGH_TARGET = 2,	// 穿目标
        ENUM_THROUGH_ALL = 3,		// 穿墙+穿目标
    };

    // 技能作用目標 
    enum SKILL_TARGET_TYPE
    {
        SKILL_TARGET_TYPE_SELF = 1L << 0,	//自已
        SKILL_TARGET_TYPE_EMENY = 1L << 1,	//敵對陣營
        SKILL_TARGET_TYPE_FRIEND = 1L << 2,	//友方陣營

		//按策划最新需求,以下枚举不再使用 部分bit转SKILL_TARGET_CATEGORY_TYPE中定义
        //SKILL_TARGET_TYPE_GUILD = 1L << 3,	//公會成員陣營
        //SKILL_TARGET_TYPE_TEAM = 1L << 4,	//隊友
        //SKILL_TARGET_TYPE_FRIENDLIST = 1L << 5,	//好友
        //SKILL_TARGET_TYPE_RESOURCE = 1L << 6,	//資源塊
        //SKILL_TARGET_TYPE_TWOERBUILD = 1L << 7,	//塔防装置
        //SKILL_TARGET_TYPE_ANYGROUP = 1L << 8,	//任何陣營
        //SKILL_TARGET_TYPE_DOOR_KIND = 1L << 9,	//门类家俱
        //SKILL_TARGET_TYPE_PET = 1L << 10,	// 出战宠物
        //SKILL_TARGET_TYPE_NPC = 1L << 11,	// NPC
    };

	#define SKILL_TARGET_TYPE_ANYGROUP (SKILL_TARGET_TYPE_SELF | SKILL_TARGET_TYPE_EMENY | SKILL_TARGET_TYPE_FRIEND)

	// 技能作用的对象类型
	enum SKILL_TARGET_CATEGORY_TYPE
	{
		SKILL_TARGET_CATEGORY_TYPE_PLAYER      = 1,    // 玩家
		SKILL_TARGET_CATEGORY_TYPE_MOB         = 2,    // 怪物
		SKILL_TARGET_CATEGORY_TYPE_NPC         = 4,    // NPC
		SKILL_TARGET_CATEGORY_TYPE_PET         = 8,    // 宠物
		SKILL_TARGET_CATEGORY_TYPE_TOWERBUILD  = 16,   // 塔防
		SKILL_TARGET_CATEGORY_TYPE_ACTIVEBUILD = 32,   // 互动家具
		SKILL_TARGET_CATEGORY_TYPE_BLOCK       = 64,   // 资源块
		SKILL_TARGET_CATEGORY_TYPE_BUILD       = 128,  // 普通家具
		SKILL_TARGET_CATEGORY_TYPE_PLANT       = 256,  // 植物
		SKILL_TARGET_CATEGORY_TYPE_BACK_BLOCK  = 512,  // 背景块
		SKILL_TARGET_CATEGORY_TYPE_DOOR        = 1024, // 门		
	};


    // 技能作用時參考的數值
    enum SKILL_ATTR_REF
    {
        SKILL_ATTR_REF_STATIC = 0, // 0 固定值


    };

}