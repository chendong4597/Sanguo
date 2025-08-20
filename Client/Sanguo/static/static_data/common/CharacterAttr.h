#pragma once

#include <vector>

#include "msg/MsgPlayer.pb.h"

#include "EffectEnum.h"

//角色种族类型
enum CHARACTER_TYPE
{
    CHARACTER_TYPE_NONE = 0,
    CHARACTER_TYPE_PLAYER = 1,    //玩家
    CHARACTER_TYPE_COMMON_MOB = 2,	//普通怪
    CHARACTER_TYPE_BASE_MOB = 3,	//基础怪
    CHARACTER_TYPE_LIGHT_MOB = 4,	//轻型怪
    CHARACTER_TYPE_HEAVY_MOB = 5,	//重型怪
    CHARACTER_TYPE_STORM_MOB = 6,	//攻城怪

    //以下为玩家塔防家具，后面的枚举对应塔防家具表中的类型
    CHARACTER_TYPE_TOWREBUILD_ATTACK = 7,    // 0：输出
    CHARACTER_TYPE_TOWREBUILD_PIN_DOWN = 8,	// 1：牵制
    CHARACTER_TYPE_TOWREBUILD_ASSIST = 9,    // 2：辅助
    CHARACTER_TYPE_TOWREBUILD_EMERGENCY = 10,	// 3：紧急
    CHARACTER_TYPE_TOWREBUILD_SUMMON = 11,   // 4：召唤
    CHARACTER_TYPE_TOWREBUILD_CRYSTAL = 12,   // 6: 水晶

    //
    CHARACTER_TYPE_NPC = 13,	//NPC(我方陣營Mob)
    CHARACTER_TYPE_PET = 14,	// 玩家宠物

    CHARACTER_TYPE_ALL = 15,

    CHARACTER_TYPE_MAX
};

struct CharacterAttrVectorBase
{
    enum ATTR_DEF
    {
        ATTR_NULL = 0,
        ATTR_HPM,			//最大生命
        ATTR_MPM,			//最大魔力
        ATTR_AD,			//物理攻擊
        ATTR_AP,			//魔法攻擊
        ATTR_ARM,			//物理防禦
        ATTR_MARM,			//魔法防禦
        ATTR_AR,			//物理減免
        ATTR_MR_FIRE,		//魔法減免 火
        ATTR_MR_FORST,		//魔法減免 冰
        ATTR_MR_THUNDER,	//魔法減免 雷
        ATTR_MR_EARTH,		//魔法減免 土
        ATTR_MR_POSION,		//魔法減免 毒
        ATTR_MR_LIGHT,		//魔法減免 光
        ATTR_MR_DARK,		//魔法減免 暗
        ATTR_ARP,			//物裡穿透
        ATTR_MRP,			//魔法穿透
        ATTR_CRI,			//物理爆擊率
        ATTR_MCRI,			//魔法爆擊率
        ATTR_CRID,			//物理爆擊增傷
        ATTR_MCRID,			//魔法爆擊增傷
        ATTR_RES,			//物裡韌性
        ATTR_MRES,			//魔法韌性
        ATTR_CC_HIT,		//控制命中
        ATTR_CC_RES,		//控制抵抗
        ATTR_ATK_SP,		//攻擊速度
        ATTR_MV_SP,			//移動速度
        ATTR_HB,			//擊退
        ATTR_HPR,			//生命回復
        ATTR_HPRS,			//生命回復速度
        ATTR_MPR,			//魔法回復
        ATTR_MPRS,			//魔法回復速度
        ATTR_GATHER_ATK,	//採集攻擊力
        ATTR_JUMP,			//跳
        ATTR_SWIM,			//游泳
        ATTR_ATK_RANGE,		//攻擊距離 float *1000
        ATTR_GATHER_SP,		//採集速度
        ATTR_CHASE,			//追击速度
        ATTR_BOSS_HURT,		//boss伤害加成
        ATTR_BOSS_DMGREDUC,	//受到boss伤害减免
        ATTR_UNLOCK_FOG,    //解锁地型迷雾范围 (2N+1)*(2N+1)
        ATTR_UNLOCK_FOG_EMPTY, //解锁非地块迷雾范围 (2N+1)*(2N+1)

        ATTR_WIDTH,			//寬
        ATTR_HEIGHT,		//高
        ATTR_CATCH,			//拾取物品距離
        ATTR_FOV,			//視野
        ATTR_CHARACTER_TYPE,//角色类型
        ATTR_OXY,			//最大含氧量
        ATTR_JUMP_INIT_SPEED, //跳跃初速度
		ATTR_WATER_BREATH_DEEP,// 水下可呼吸深度（头顶开始算）

		ATTR_TOTAL_DAMAGE_INC, //全伤害增伤加成
		ATTR_DEBUFF_DAMAGE_INC,	// 目标Debuff增伤

        ATTR_NUM
    }; // End of ATTR_DEF
};

template< typename AttrType, typename AttrVector >
class TCharacterAttrVector : public CharacterAttrVectorBase
{
public:
    TCharacterAttrVector()
    {
		m_attrVector.reserve(ATTR_NUM);
        m_attrVector.resize(ATTR_NUM);
    };

    virtual ~TCharacterAttrVector()
    {
        m_attrVector.clear();
    };

    AttrType& operator [] (ATTR_DEF eAttr)
    {
        return m_attrVector[eAttr];
    }

    AttrType operator [] (ATTR_DEF eAttr) const
    {
        return m_attrVector[eAttr];
    }

    void CleanUp()
    {
        for (auto& attr : m_attrVector)
        {
            attr = 0;
        }
    }

    const AttrVector& GetAttrVector() const
    {
        return m_attrVector;
    }

    void SetAttrVector(const AttrVector& attrVector)
    {
        m_attrVector.assign(attrVector.begin(), attrVector.end());
    };

protected:
    AttrVector m_attrVector;
};
