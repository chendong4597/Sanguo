#include "stdafx.h"

#include <algorithm>

#include "ClientSpec.h"
#include "SkillBuffContainer.h"

SkillBuffContainer::SkillBuffContainer()
{
    SkillBuffInfo tempBuf;
    m_BuffInfos.reserve(MAX_SKILL_BUFF_SIZE);
    for (int i = 0; i < MAX_SKILL_BUFF_SIZE; ++i)
    {
        tempBuf.Index = i;
        m_BuffInfos.push_back(tempBuf);
    }
}

// 設定一個Buff
bool SkillBuffContainer::SetBuff(unsigned int Index, SkillBuffInfo& theBuff)
{
    if (/*Index < 0 ||*/ Index >= m_BuffInfos.size())
        return false;
    theBuff.Index = Index;
    m_BuffInfos[Index] = theBuff;
    return true;
}

SkillBuffInfo* SkillBuffContainer::GetBuff(unsigned int Index)
{
    if (/*Index < 0 ||*/ Index >= m_BuffInfos.size())
        return nullptr;
    return &(m_BuffInfos[Index]);
}

// 取得一個空的Buff位置
int	SkillBuffContainer::GetEmptyBuffIndex()
{
    for (auto& pBuff : m_BuffInfos)
        if (pBuff.IsEmpty())
            return pBuff.Index;
    return -1;
}

// 搜尋
int	SkillBuffContainer::SearchBuffIndex(int CasterMapObjectID, int BuffDataID)
{
    for (auto& pBuff : m_BuffInfos)
    {
        if (pBuff.IsEmpty())
            continue;
        if (pBuff.CasterMapObjectID == CasterMapObjectID && pBuff.BuffDataID == BuffDataID)
            return pBuff.Index;
    }
    return -1;
}

SkillBuffInfo* SkillBuffContainer::GetBuffByGroupRule(std::vector<NS::int32>& Rules)
{
    if (Rules.size() == 0)
        return nullptr;

    for (auto& BuffInfo : m_BuffInfos)
    {
        if (BuffInfo.IsEmpty())
            continue;

        int CheckGroupID = BuffInfo.nGroupId;

        // 是否存在替代規則中
        auto op = std::find(Rules.begin(), Rules.end(), CheckGroupID);
        if (op == Rules.end())
            continue;
        return &BuffInfo;
    };
    return nullptr;
}

void SkillBuffContainer::AddReduceNegativeEffectByRate(NS::int32 buffDataID, NS::int32 effect, NS::int32 rate)
{
    auto& rates = m_reduceNegativeEffectByRateBuffs[buffDataID];
    rates.emplace(effect, rate);
}

void SkillBuffContainer::RemoveReduceNegativeEffectByRate(NS::int32 buffDataID)
{
    m_reduceNegativeEffectByRateBuffs.erase(buffDataID);
}

int SkillBuffContainer::getReduceNegativeEffectRate(NS::int32 effectType)
{
    int rate = 0;
    for (const auto& rs : m_reduceNegativeEffectByRateBuffs)
    {
        for (const auto& r : rs.second)
        {
            if (r.first == effectType)
            {
                rate += r.second;
            }
        }
    }
    return (std::min)(rate, 1000);
}

void SkillBuffContainer::SumEffect(CEffectSum& effectSum)
{
    for (auto& buff : m_BuffInfos)
    {
        if (buff.IsEmpty())
        {
            continue;
        }
        for (auto& effect : buff.BuffEffects)
        {
			if (!effect.m_bActive)
				continue;

            NS::int32 reduceNegativeEffectRate = this->getReduceNegativeEffectRate(effect.type);
            effectSum.AddEffect(effect.type, effect.values, (std::max)(buff.HeapCounter, 1) * buff.nEffectRate, reduceNegativeEffectRate);
        }
    }
}

void SkillBuffContainer::ClearBuffContainer()
{
	m_reduceNegativeEffectByRateBuffs.clear();

	for (size_t i = 0; i < m_BuffInfos.size(); i++) {
		m_BuffInfos[i].Reset();
	}
}
