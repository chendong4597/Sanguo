#pragma once

#include "EffectEnum.h"


// 公式用的結構
struct SkillFormulaParam
{
    NS::int32 CasterCrit;		//暴击率
    NS::int32 TargetTenacity;	//韧性率	
    NS::int32 CasterAtk;		//攻击力
    NS::int32 TargetDef;		//防御力
    NS::int32 CasterPenetrate;	//穿透率
    NS::int32 TargetDmgAvoid;	//免伤率
    NS::int32 CasterCritAdd;	//暴击增伤率
    NS::int32 EffectValue;		//效果值
    NS::int32 BossHurt;			//boss伤害加成
    NS::int32 BossDmgRed;		//受到boss伤害减免
    std::vector<AttrEffectValue> vecEffect;	// 技能配置表的效果分组
    std::vector<NS::int32> EffectValues;		//效果值 分组
};



// 技能效果:公式形傷害
class CEffectCalculation
{
public:
    CEffectCalculation() {};

    bool CalEffect(SkillFormulaParam& Param, int& DmgValue, bool& bCritical);
    int NormalEffect(SkillFormulaParam& Param);
    int CriticalEffect(SkillFormulaParam& Param);

};
