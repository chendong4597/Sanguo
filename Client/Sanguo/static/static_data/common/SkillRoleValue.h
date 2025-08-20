#pragma once

#include "GameDefine.h"
#include "CharacterAttr.h"

// 技能計算時使用的結構
template <
    typename MapObject
    , typename MapCharacter /*必须派生自MapObject，参考GameDefine.h的IS_MAP_CHARACTER*/
    , typename AttrVector /*必须派生自CharacterAttrVectorBase，且实现[]操作符，参考TCharacterAttrVector*/
>
struct TSkillRoleValue
{
    bool bSet = false;

    int32 HP = 0;				//目前血量
    int32 MP = 0;				//目前魔力
    int32 BaseHP = 0;			//基礎血量
    int32 BaseMP = 0;			//基礎魔力
    int32 BaseHPR = 0;			//基礎生命回復
    int32 BaseHPRS = 0;			//基礎生命回復速度
    int32 BaseMPR = 0;			//基礎魔法回復
    int32 BaseMPRS = 0;			//基礎魔法回復速度
    int32 BaseAD = 0;			//基礎物理攻擊
    int32 BaseAP = 0;			//基礎魔法攻擊
    int32 BaseARM = 0;			//基礎物理防禦
    int32 BaseMARM = 0;			//基礎魔法防禦
    int32 BaseAR = 0;			//基礎物理減免
    int32 BaseMR_FIRE = 0;		//基礎魔法減免 火
    int32 BaseMR_FORST = 0;		//基礎魔法減免 冰
    int32 BaseMR_THUNDER = 0;	//基礎魔法減免 雷
    int32 BaseMR_EARTH = 0;		//基礎魔法減免 土
    int32 BaseMR_POSION = 0;	//基礎魔法減免 毒
    int32 BaseMR_LIGHT = 0;		//基礎魔法減免 光
    int32 BaseMR_DARK = 0;		//基礎魔法減免 暗
    int32 BaseARP = 0;			//基礎物裡穿透
    int32 BaseMRP = 0;			//基礎魔法穿透
    int32 BaseCRI = 0;			//基礎物理爆擊率
    int32 BaseMCRI = 0;			//基礎魔法爆擊率
    int32 BaseCRID = 0;			//基礎物理爆擊增傷
    int32 BaseMCRID = 0;		//基礎魔法爆擊增傷
    int32 BaseRES = 0;			//基礎物裡韌性
    int32 BaseMRES = 0;			//基礎魔法韌性
    int32 BaseCC_HIT = 0;		//基礎控制命中
    int32 BaseCC_RES = 0;		//基礎控制抵抗
    int32 BaseATK_SP = 0;		//基礎攻速	
    int32 BaseMV = 0;			//基礎移動速度		
    int32 BaseHB = 0;			//基礎擊退
    int32 BaseDigAtk = 0;		//基礎採集攻擊力
    int32 BaseDigAtkSP = 0;		//基礎採集速度
    int32 BaseJump = 0;			//基礎跳跃力
    int32 BaseSwin = 0;			//基礎游泳速度
    int32 BaseBossHurt = 0;		//基礎boss伤害
    int32 BaseBossDmgRed = 0;	//基礎受到boss伤害减免
	int32 BaseJumpInitSpeed = 0; // 基础跳跃初速度
	int32 BaseTotalDamageInc = 0; // 基础全伤害增伤(千分比)

    int32 NowHP = 0;			//最大血量
    int32 NowMP = 0;			//最大魔力
    int32 NowHPR = 0;			//目前生命回復
    int32 NowHPRS = 0;			//目前生命回復速度
    int32 NowMPR = 0;			//目前魔法回復
    int32 NowMPRS = 0;			//目前魔法回復速度
    int32 NowAD = 0;			//目前物理攻擊
    int32 NowAP = 0;			//目前魔法攻擊
    int32 NowARM = 0;			//目前物理防禦
    int32 NowMARM = 0;			//目前魔法防禦
    int32 NowAR = 0;			//目前物理減免
    int32 NowMR_FIRE = 0;		//目前目前魔法減免 火
    int32 NowMR_FORST = 0;		//目前魔法減免 冰
    int32 NowMR_THUNDER = 0;	//目前魔法減免 雷
    int32 NowMR_EARTH = 0;		//目前魔法減免 土
    int32 NowMR_POSION = 0;		//目前魔法減免 毒
    int32 NowMR_LIGHT = 0;		//目前魔法減免 光
    int32 NowMR_DARK = 0;		//目前魔法減免 暗
    int32 NowARP = 0;			//目前物裡穿透
    int32 NowMRP = 0;			//目前魔法穿透
    int32 NowCRI = 0;			//目前物理爆擊率
    int32 NowMCRI = 0;			//目前魔法爆擊率
    int32 NowCRID = 0;			//目前物理爆擊增傷
    int32 NowMCRID = 0;			//目前魔法爆擊增傷
    int32 NowRES = 0;			//目前物裡韌性
    int32 NowMRES = 0;			//目前魔法韌性
    int32 NowCC_HIT = 0;		//目前控制命中
    int32 NowCC_RES = 0;		//目前控制抵抗
    int32 NowATK_SP = 0;		//目前攻速	
    int32 NowMV = 0;			//目前移動速度
    int32 NowHB = 0;			//目前擊退
    int32 NowGatherAtk = 0;		//目前採集攻擊力
    int32 NowGatherSpeed = 0;	//目前採集速度
    int32 NowJump = 0;			//目前跳跃力
    int32 NowSwin = 0;			//目前游泳速度
    int32 NowBossHurt = 0;		//目前boss伤害
    int32 NowBossDmgRed = 0;	//目前受到boss伤害减免
    int32 NowJumpInitSpeed = 0; //目前跳跃初速度
	int32 NowTotalDamageInc = 0; //目前全伤害增伤(千分比)

    TSkillRoleValue()
    {
        Reset();
    };

    void Reset()
    {
        bSet = false;
    };

    using PtrMapObject = std::shared_ptr<MapObject>;

    bool GetSkillRoleValue(PtrMapObject pMapObject)
    {
        if (bSet)
        {
            Reset();
        }

        if (pMapObject == nullptr)
        {
            return false;
        }

        MAP_OBJECT_TYPE emType = pMapObject->GetObjectType();

        if (IS_MAP_CHARACTER(emType))
        {
            return this->GetSkillRoleValue_ByCharacter(std::dynamic_pointer_cast<MapCharacter>(pMapObject));
        }

        return false;
    };


    using PtrMapCharacter = std::shared_ptr<MapCharacter>;

    bool GetSkillRoleValue_ByCharacter(const PtrMapCharacter& pMapCharacter)
    {
        if (bSet)
        {
            Reset();
        }

        if (pMapCharacter == nullptr)
        {
            return false;
        }

        // 自MapCharacter執行取得角色技能資訊 [Design Patterns: Memento]
        pMapCharacter->GetSkillRoleValue(*this);
        if (bSet)
        {
            return true;
        }

        // 再自行取得
        const auto& theAttr = pMapCharacter->GetCharacterAttr();
        const auto& theNowAttr = theAttr.GetAttr();
        const auto& theBaseAttr = theAttr.GetBase();

        bSet = true;
        HP = theAttr.GetHP();	//目前血量
        MP = theAttr.GetMP();	//目前魔力
        BaseHP = theBaseAttr[AttrVector::ATTR_HPM];	//基礎血量
        BaseMP = theBaseAttr[AttrVector::ATTR_MPM];	//基礎魔力
        BaseHPR = theBaseAttr[AttrVector::ATTR_HPR];	//基礎生命回復
        BaseHPRS = theBaseAttr[AttrVector::ATTR_HPRS];	//基礎生命回復速度
        BaseMPR = theBaseAttr[AttrVector::ATTR_MPR];	//基礎魔法回復
        BaseMPRS = theBaseAttr[AttrVector::ATTR_MPRS];	//基礎魔法回復速度
        BaseAD = theBaseAttr[AttrVector::ATTR_AD];	//基礎物理攻擊
        BaseAP = theBaseAttr[AttrVector::ATTR_AP];	//基礎魔法攻擊
        BaseARM = theBaseAttr[AttrVector::ATTR_ARM];	//基礎物理防禦
        BaseMARM = theBaseAttr[AttrVector::ATTR_MARM];	//基礎魔法防禦
        BaseAR = theBaseAttr[AttrVector::ATTR_AR];	//基礎物理減免
        BaseMR_FIRE = theBaseAttr[AttrVector::ATTR_MR_FIRE];	//基礎魔法減免 火
        BaseMR_FORST = theBaseAttr[AttrVector::ATTR_MR_FORST];	//基礎魔法減免 冰
        BaseMR_THUNDER = theBaseAttr[AttrVector::ATTR_MR_THUNDER];	//基礎魔法減免 雷
        BaseMR_EARTH = theBaseAttr[AttrVector::ATTR_MR_EARTH];	//基礎魔法減免 土
        BaseMR_POSION = theBaseAttr[AttrVector::ATTR_MR_POSION];	//基礎魔法減免 毒
        BaseMR_LIGHT = theBaseAttr[AttrVector::ATTR_MR_LIGHT];	//基礎魔法減免 光
        BaseMR_DARK = theBaseAttr[AttrVector::ATTR_MR_DARK];	//基礎魔法減免 暗
        BaseARP = theBaseAttr[AttrVector::ATTR_ARP];	//基礎物裡穿透
        BaseMRP = theBaseAttr[AttrVector::ATTR_MRP];	//基礎魔法穿透
        BaseCRI = theBaseAttr[AttrVector::ATTR_CRI];	//基礎物理爆擊率
        BaseMCRI = theBaseAttr[AttrVector::ATTR_MCRI];	//基礎魔法爆擊率
        BaseCRID = theBaseAttr[AttrVector::ATTR_CRID];	//基礎物理爆擊增傷
        BaseMCRID = theBaseAttr[AttrVector::ATTR_MCRID];	//基礎魔法爆擊增傷
        BaseRES = theBaseAttr[AttrVector::ATTR_RES];	//基礎物裡韌性
        BaseMRES = theBaseAttr[AttrVector::ATTR_MRES];	//基礎魔法韌性
        BaseCC_HIT = theBaseAttr[AttrVector::ATTR_CC_HIT];	//基礎控制命中
        BaseCC_RES = theBaseAttr[AttrVector::ATTR_CC_RES];	//基礎控制抵抗
        BaseATK_SP = theBaseAttr[AttrVector::ATTR_ATK_SP];	//基礎攻速	
        BaseMV = theBaseAttr[AttrVector::ATTR_MV_SP];	//基礎移動速度		
        BaseHB = theBaseAttr[AttrVector::ATTR_HB];	//基礎擊退
        BaseDigAtk = theBaseAttr[AttrVector::ATTR_GATHER_ATK];	//基礎採集攻擊力
        BaseDigAtkSP = theBaseAttr[AttrVector::ATTR_GATHER_SP];	//基礎採集速度
        BaseJump = theBaseAttr[AttrVector::ATTR_JUMP];	//基礎跳跃力
        BaseSwin = theBaseAttr[AttrVector::ATTR_SWIM];	//基礎游泳速度	
        BaseBossHurt = theBaseAttr[AttrVector::ATTR_BOSS_HURT];	//基礎boss伤害加成	
        BaseBossDmgRed = theBaseAttr[AttrVector::ATTR_BOSS_DMGREDUC];	//基礎受到boss伤害减免
		BaseJumpInitSpeed = theBaseAttr[AttrVector::ATTR_JUMP_INIT_SPEED]; //基础跳跃初速度
		BaseTotalDamageInc = theBaseAttr[AttrVector::ATTR_TOTAL_DAMAGE_INC]; //基础跳跃初速度

        NowHP = theNowAttr[AttrVector::ATTR_HPM];		//最大血量
        NowMP = theNowAttr[AttrVector::ATTR_MPM];		//最大魔力
        NowHPR = theNowAttr[AttrVector::ATTR_HPR];		//目前生命回復
        NowHPRS = theNowAttr[AttrVector::ATTR_HPRS];		//目前生命回復速度
        NowMPR = theNowAttr[AttrVector::ATTR_MPR];		//目前魔法回復
        NowMPRS = theNowAttr[AttrVector::ATTR_MPRS];		//目前魔法回復速度
        NowAD = theNowAttr[AttrVector::ATTR_AD];		//目前物理攻擊
        NowAP = theNowAttr[AttrVector::ATTR_AP];		//目前魔法攻擊
        NowARM = theNowAttr[AttrVector::ATTR_ARM];		//目前物理防禦
        NowMARM = theNowAttr[AttrVector::ATTR_MARM];		//目前魔法防禦
        NowAR = theNowAttr[AttrVector::ATTR_AR];		//目前物理減免
        NowMR_FIRE = theNowAttr[AttrVector::ATTR_MR_FIRE];	//目前目前魔法減免 火
        NowMR_FORST = theNowAttr[AttrVector::ATTR_MR_FORST];	//目前魔法減免 冰
        NowMR_THUNDER = theNowAttr[AttrVector::ATTR_MR_THUNDER];//目前魔法減免 雷
        NowMR_EARTH = theNowAttr[AttrVector::ATTR_MR_EARTH]; //目前魔法減免 土
        NowMR_POSION = theNowAttr[AttrVector::ATTR_MR_POSION];//目前魔法減免 毒
        NowMR_LIGHT = theNowAttr[AttrVector::ATTR_MR_LIGHT];	//目前魔法減免 光
        NowMR_DARK = theNowAttr[AttrVector::ATTR_MR_DARK];	//目前魔法減免 暗
        NowARP = theNowAttr[AttrVector::ATTR_ARP];		//目前物裡穿透
        NowMRP = theNowAttr[AttrVector::ATTR_MRP];		//目前魔法穿透
        NowCRI = theNowAttr[AttrVector::ATTR_CRI];		//目前物理爆擊率
        NowMCRI = theNowAttr[AttrVector::ATTR_MCRI];		//目前魔法爆擊率
        NowCRID = theNowAttr[AttrVector::ATTR_CRID];		//目前物理爆擊增傷
        NowMCRID = theNowAttr[AttrVector::ATTR_MCRID];	//目前魔法爆擊增傷
        NowRES = theNowAttr[AttrVector::ATTR_RES];		//目前物裡韌性
        NowMRES = theNowAttr[AttrVector::ATTR_MRES];		//目前魔法韌性
        NowCC_HIT = theNowAttr[AttrVector::ATTR_CC_HIT];	//目前控制命中
        NowCC_RES = theNowAttr[AttrVector::ATTR_CC_RES];	//目前控制抵抗
        NowATK_SP = theNowAttr[AttrVector::ATTR_ATK_SP];	//目前攻速	
        NowMV = theNowAttr[AttrVector::ATTR_MV_SP];	//自身基礎移動速度		
        NowHB = theNowAttr[AttrVector::ATTR_HB];		//自身當前擊退
        NowGatherAtk = theNowAttr[AttrVector::ATTR_GATHER_ATK];//目前採集攻擊力
        NowGatherSpeed = theNowAttr[AttrVector::ATTR_GATHER_SP];//目前採集速度
        NowJump = theNowAttr[AttrVector::ATTR_JUMP];		//目前跳跃力
        NowSwin = theNowAttr[AttrVector::ATTR_SWIM];		//目前游泳速度
        NowBossHurt = theNowAttr[AttrVector::ATTR_BOSS_HURT];//目前boss伤害加成
        NowBossDmgRed = theNowAttr[AttrVector::ATTR_BOSS_DMGREDUC];//目前受到boss伤害减免
        NowJumpInitSpeed = theNowAttr[AttrVector::ATTR_JUMP_INIT_SPEED]; // 目前跳跃初速度
		NowTotalDamageInc = theNowAttr[AttrVector::ATTR_TOTAL_DAMAGE_INC]; //基础跳跃初速度

        return true;
    };

    friend bool operator==(const TSkillRoleValue& var1, const TSkillRoleValue& var2)
    {
        return (var1.bSet == var2.bSet);// &&
        //var1.DamageHP == var2.DamageHP);
    }
};
