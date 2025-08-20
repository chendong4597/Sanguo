#pragma once

#include <cmath>

enum MAP_OBJECT_TYPE
{
    OBJECT_NONE = 0,
    OBJECT_PLAYER,
    OBJECT_MOB,
    OBJECT_NPC,
    OBJECT_MAGIC,
    OBJECT_BUILDTOWER,	// 塔防家俱
    OBJECT_PVP_CHEST,	// pvp 宝箱
    OBJECT_PVP_GRASS,	// pvp草丛
    OBJECT_BUILDACTIVE,	// 互動傢俱
    OBJECT_VEHICLE,		// 載具
    OBJECT_PET,			// 宠物
    OBJECT_MAX
};

#define IS_MAP_CHARACTER(type) ((type) == OBJECT_PLAYER || (type) == OBJECT_MOB || (type) == OBJECT_BUILDTOWER || (type) == OBJECT_PET)

#define CeilMulDiv(Number, Numerator, Denominator) (std::ceil((double)((Number) * (Numerator)) / (double)(Denominator)))
#define CeilMulDivI(Number, Numerator, Denominator) (int)(CeilMulDiv(Number, Numerator, Denominator))

#define FloorMulDiv(Number, Numerator, Denominator) (std::floor((double)((Number) * (Numerator)) / (double)(Denominator)))
#define FloorMulDiv_X(Number, Numerator, Denominator, x) (x)(FloorMulDiv(Number, Numerator, Denominator))

#define ANTIQUE_NOT_TREASURES_PRICE(price, star, integrity, quality) (4 * price * star * (integrity + quality) / 3.9f)
#define ANTIQUE_TREASURES_PRICE(price, star, integrity, quality) (4 * price * star / 10 * std::pow((integrity + quality) / 7.8f, 3))

const int AirShipMapId = 501001;
const int PriveMapId = 101001;

// 获取鉴定装备所需银币
inline int IdentifyEquipConsumeSilver(int level)
{
    // 装备鉴定费用公式 =（装备等级* 2）^ 1.85 取整 
    // 装备等级：item表n_use_lv字段
    return int(std::pow(double(level * 2), 1.85f));
}

// 获取鉴定古玩所需银币
inline int IdentifyAntiqueConsumeSilver(int grade, int recyclePrice)
{
    // 古玩鉴定费用=4*星级*产地对应的基础价格参数
    return 4 * grade * recyclePrice;
}

// 计算NPC挂机农场收益
inline int AutoIncomeFarmingReward(int rewardItemNum, double rewardRatio, int spendItemNum)
{
	// 挂机收益公式 = 奖励物品数目 * 调整比例 * 花费的种子或饲料数
	return static_cast<int>(rewardItemNum * rewardRatio * spendItemNum);
}


//根据辉章系数算出间隔时间
inline int AutoIncomeCalcInterTime(int matureTimeCfg, float protectorRatio)
{
	return (int)(matureTimeCfg / protectorRatio);
}

// 计算根据时间消耗的物品数量
inline int AutoIncomeCalcCostItemNum(int nRewardTime, int nMatureTimeCfg, int nMaxNum, float fAdjParam)
{
	return nRewardTime / AutoIncomeCalcInterTime(nMatureTimeCfg, fAdjParam) * nMaxNum;
}

//计算耗尽种子的时间;
inline int AutoIncomeCalCastTime(int leftNum,int realMatureTime,int maxNum)
{
	return (int)(ceil(leftNum * 1.0 / maxNum) * realMatureTime);
}

// 计算NPC挂机农场收益(用于效率统计界面);
inline float AutoIncomeFarmingRewardOfUsePreView(int rewardItemNum, double rewardRatio, float spendItemNum)
{
	// 挂机收益公式 = 奖励物品数目 * 调整比例  * 花费的种子或饲料数
	return (float)(rewardItemNum * rewardRatio * spendItemNum);
}

