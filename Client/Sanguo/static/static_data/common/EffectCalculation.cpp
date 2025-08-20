#include "stdafx.h"
#include "ClientSpec.h"
#include "EffectCalculation.h"

bool CEffectCalculation::CalEffect(SkillFormulaParam& Param, int& dmg, bool& bCritical)
{
    // 是否爆擊 : A法术/物理暴击率 – B法术/物理韧性率
    int criticalRate = Param.CasterCrit - Param.TargetTenacity;
    int roll = rand() % 1000;
    bCritical = (roll <= criticalRate);

    // 計算傷害值
    if (!bCritical)
    {
        dmg = NormalEffect(Param);
    }
    else
    {
        dmg = CriticalEffect(Param);
    }

    // 傷害浮動
    roll = 100 - (rand() % 10);
    dmg = static_cast<int>(double(dmg * roll) / 100.0f);

    // 最終傷害 : 攻击力5%作比较，取最大值作为最终伤害
    int atk = Param.CasterAtk;
    int checkDmg = (std::max)(static_cast<int>(ceil(double(atk) * 5.0f / 100.0f)), 1);
    dmg = (std::max)(checkDmg, dmg);

    return true;
}

// 一般計算
int CEffectCalculation::NormalEffect(SkillFormulaParam& param)
{
    //公式: ((A[法术/物理]攻击力 – B[法术/物理]防御力 * (1 – A[法术/物理]穿透率)) * (1-B对应[法術元素/物理]免伤率)	
    // A[法术/物理]攻击力 = A角色[法術/物理]攻擊力 + 配置指定	PS：A的[法术/物理]攻击必然属于（冰火雷等）元素中的一种			

    //公式：((A物理攻击力 – B物理防御力 * (1 - A物理穿透率)) * 技能伤害倍数 + 技能附加值) * (1 - B物理免伤率)
    //公式：((A法术攻击力 – B法术防御力 * (1 – A法术穿透率)) * 技能伤害倍数 + 技能附加值) * (1 - B对应元素免伤率)

    // A攻击
    double atk = static_cast<double>(param.CasterAtk);

    // B防御 = 防御力 * (1 – 穿透率)
    double def = double(param.TargetDef) * (double(1000 - param.CasterPenetrate) / 1000.0f);

    // 傷害值
    double dmg = atk - def;

    // 技能伤害倍数
    double rate = 0;
    // 技能伤害附加值
    double bonusDmg = 0;
    NS::int32 size = (std::min)((int)param.vecEffect.size(), (int)param.EffectValues.size());
    for (int i = 0; i < size; ++i)
    {
        const auto& effect = param.vecEffect[i];
        if (effect.attrref == ATTR_REF_SELF_AD_NOW || effect.attrref == ATTR_REF_SELF_AP_NOW)
        {
            rate += static_cast<double>(effect.value);
        }
        else
        {
            bonusDmg += static_cast<double>(param.EffectValues[i]);
        }
    }
    if (rate > 0)
    {
        dmg = dmg * rate / 1000.0f;
    }
    dmg += bonusDmg;

    // 免傷 : 傷害值 * (1-B对应元素免伤率)
    dmg = dmg * (1000.0f - double(param.TargetDmgAvoid)) / 1000.0f;

    // boss 增伤减伤
    dmg = dmg * double(param.BossHurt) / 1000.0f;
    dmg = dmg * double(param.BossDmgRed) / 1000.0f;

    return static_cast<int>(dmg);
}

// 暴击計算
int CEffectCalculation::CriticalEffect(SkillFormulaParam& Param)
{
    // 公式前半等於一般功擊
    double dmg = static_cast<double>(NormalEffect(Param));
    // 免傷 & 暴擊增傷 : 傷害值 * (1-B对应元素免伤率) * (1 + A法术暴击增伤率)
    dmg = dmg * double(1000 + Param.CasterCritAdd) / 1000.0f;
    return static_cast<int>(dmg);
}
