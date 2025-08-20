#pragma once

#include <unordered_map>
#include "SkillBuffInfo.h"
#include <functional>

#ifndef _CLIENT_MSG_
//#include "GlobalMacro.h"
#endif
#include "BaseTypeDefine.h"

#include <vector>
#include <algorithm>

class SkillBuffContainer
{
public:
    SkillBuffContainer();

    // 設定Buff
    bool SetBuff(unsigned int Index, SkillBuffInfo& theBuff);
    SkillBuffInfo* GetBuff(unsigned int Index);

	inline std::vector<SkillBuffInfo>& GetBuffInfos() {
		return m_BuffInfos;
	}

	void ClearBuffContainer();

    template<typename OP>
    void ForEachBuff(OP theOP)
    {
		std::for_each(m_BuffInfos.begin(), m_BuffInfos.end(), theOP);
    }

    void ConstForEachBuff(std::function<bool(const SkillBuffInfo& buff)>&& op) const
    {
        for (const SkillBuffInfo& buff : m_BuffInfos)
        {
            if (!op(buff))
            {
                break;
            }
        }
    }

	void ForEachNoEmptyBuff(std::function<void(SkillBuffInfo& buff)>&& op)
	{
		for (auto& buff : m_BuffInfos)
		{
			if (buff.IsEmpty())
				continue;

			op(buff);
		}
	}

    int	GetEmptyBuffIndex();
    int	SearchBuffIndex(int CasterMapObjectID, int BuffDataID);
    void SumEffect(CEffectSum& effectSum);


    SkillBuffInfo* GetBuffByGroupRule(std::vector<NS::int32>& Rules);
#if 0
    std::vector<SkillBuffInfo> m_BuffInfos;
#endif

    void AddReduceNegativeEffectByRate(NS::int32 buffDataID, NS::int32 effect, NS::int32 rate);
    void RemoveReduceNegativeEffectByRate(NS::int32 buffDataID);

protected:
    int getReduceNegativeEffectRate(NS::int32 effectType);

protected:
    // 记录下降低负面效应的buff信息
    // buff data id -> ROLE_EFFECT_TYPE/BUFF_EFFECT_TYPE -> rate
    std::unordered_map<NS::int32, std::unordered_map<NS::int32, NS::int32>> m_reduceNegativeEffectByRateBuffs;

	std::vector<SkillBuffInfo> m_BuffInfos;
};