#include "stdafx.h"
#include "ClientSpec.h"
#include "ComSkillBuff.h"
#include <algorithm>


ComSkillBuff::ComSkillBuff()
{
    mapBuffContainer.clear();
}

ComSkillBuff::~ComSkillBuff()
{
}

void ComSkillBuff::Update(unsigned int time_escaple)
{
    // 100ms更新一次
    static unsigned int FixTimeInterval = 100;
    m_UpdateTimer += time_escaple;
    if (m_UpdateTimer < FixTimeInterval)
        return;
    unsigned int UpdateTimer = m_UpdateTimer;
    m_UpdateTimer = 0;

    for (auto& info : mapBuffContainer)
    {
        SkillBuffContainer& buffContainer = info.second;

        // 利用 Lambda走訪
        auto DoBuffFunc = [this, UpdateTimer, &buffContainer](SkillBuffInfo& buff) {
            if (buff.IsEmpty())
            {
                return;
            }

#if 1
			if (buff.locked)
				return;
#else
            if (!buff.locked)
            {
                if (this->IsSameGroupHighRankBuffActive(buffContainer, buff))
                {
                    // 有同组内更高优先级的buff在生效，则冻结buff
                    buff.locked = true;
                    DoBuffEvent(BUFF_TRIGGER_LOCK, buff, UpdateTimer);
                    return;
                }
            }
            else
            {
                if (!this->IsSameGroupHighRankBuffActive(buffContainer, buff))
                {
                    // 没有同组更高优先级的buff在生效，则激活buff
                    buff.locked = false;
                    DoBuffEvent(BUFF_TRIGGER_UNLOCK, buff, UpdateTimer);
                }
                return;
            }
#endif

            // 更新
            DoBuffEvent(BUFF_TRIGGER_UPDATE, buff, UpdateTimer); // 執行Buff策略

            // 是否過期
            if (buff.IsExpired())
                DelBuff(buff.OwnerMapObjectID, buff.Index, 0);

            // 耐久是否耗光
            if (buff.IsNotDurable())
            {
                DelBuff(buff.OwnerMapObjectID, buff.Index, 0);
            }
        };

        // 執行
        buffContainer.ForEachBuff(DoBuffFunc);
    }
}

void ComSkillBuff::CheckBuffLock(SkillBuffContainer& buffContainer)
{
	std::map<NS::int32, NS::int32> mapLock;
	auto DoBuffFunc = [this, &mapLock, &buffContainer](SkillBuffInfo& buff) {
		if (buff.IsEmpty())
		{
			return;
		}

		NS::int32 BuffIdx_LockBy = -1;
		bool NeedLock = false;
		if (this->IsSameGroupHighRankBuffActive(BuffIdx_LockBy, buffContainer, buff))
		{
			NeedLock = true;
			auto it = mapLock.find(BuffIdx_LockBy);
			if (mapLock.end() != it)
			{
				if (it->second == buff.Index)
					NeedLock = false;
			}

			if (NeedLock)
				mapLock[buff.Index] = BuffIdx_LockBy;
		}

		if (NeedLock)
		{
			if (!buff.locked)
				DoBuffEvent(BUFF_TRIGGER_LOCK, buff);

			buff.locked = true;
		}
		else
		{
			if(buff.locked)
				DoBuffEvent(BUFF_TRIGGER_UNLOCK, buff);

			buff.locked = false;
		}
	};

	buffContainer.ForEachBuff(DoBuffFunc);
}


//bool ComSkillBuff::AddBuff(int objCaster, int objTarget, int BuffDataID, int AliveTimer, int IntervalTimer, int nGroupId, int nMaxHeap,
//    const std::vector<S_BuffTemplate_effect>& BuffEffects, int nBuffDel, long long SerialID, int AttrRuntimeEffectRate /*= 1000*/, int nAddHeap)
bool ComSkillBuff::AddBuff(const ParamInAddBuff& ParamIn)
{
    SkillBuffContainer& buffContainer = mapBuffContainer[ParamIn.objTarget];

	bool bCheckOK = CB_AddBuffPreCheck(buffContainer, ParamIn);
	if (!bCheckOK)
		return false;

    // 確認是否有互斥的Buff存在	
    const auto* oldBuff = CheckExclusionBuffExist(ParamIn.nGroupId, buffContainer);
    if (oldBuff && oldBuff->AliveTimer > 0)
        return false;

    // 依Buff堆疊限制來取得BuffIndex
    int BuffIndex = -1;
    BUFF_EVENT emBuffEvent = BUFF_EVENT_ADD;
    // 表示是可以堆的buff,或是有群組規則,角色身上是否已有對應的Buff
    if (ParamIn.nMaxHeap > 1 || ParamIn.nGroupId > 0) {
        BuffIndex = SearchReplaceBuffIndex(ParamIn.BuffDataID, ParamIn.nGroupId, ParamIn.nMaxHeap, buffContainer);
        //if (-1 == BuffIndex) {
        //	BuffIndex = buffContainer.SearchBuffIndex(objCaster, BuffDataID);
        //}
        emBuffEvent = BUFF_EVENT_UPDATE;
    }

    if (BuffIndex < 0)// 取得一個空的.  
    {
        BuffIndex = buffContainer.GetEmptyBuffIndex();
        emBuffEvent = BUFF_EVENT_ADD;
    }

    // 取得Buff
    SkillBuffInfo* pBuff = buffContainer.GetBuff(BuffIndex);
    if (pBuff == nullptr)
        return false;

    // 設定Buff資料
    pBuff->BuffDataID = ParamIn.BuffDataID;
    pBuff->AliveTimer = ParamIn.AliveTimer;
    pBuff->IntervalTimer = ParamIn.IntervalTimer;
    pBuff->CasterMapObjectID = ParamIn.objCaster;
    pBuff->OwnerMapObjectID = ParamIn.objTarget;
	
	{
		pBuff->BuffEffects.clear();
		for (auto& itEffect : ParamIn.BuffEffects)
			pBuff->BuffEffects.push_back(itEffect);
	}

#if 1
	CheckBuffControlAnti(ParamIn, *pBuff);
#else
	//NS::int32 CasterControlRate = ParamIn.Caster_CC_HIT;
	//NS::int32 TargerControlAnti = ParamIn.Target_CC_RES;

	//if (this->IsDebuff(pBuff->BuffDataID) &&
	//	ParamIn.objCaster != ParamIn.objTarget)
	//{
	//	if (TargerControlAnti > CasterControlRate)
	//	{
	//		if (TargerControlAnti < (CasterControlRate + 1000)) //有概率出现抵抗效果控制 效果类型是301的,判断控制命中与抵抗,决定是否生效
	//		{
	//			NS::int32 HitRate = (1000 + CasterControlRate - TargerControlAnti);
	//			for (auto& itEffect : pBuff->BuffEffects)
	//			{
	//				if (ROLE_EFFECT_SKILL_STATUS != itEffect.type)
	//					continue;

	//				NS::int32 rRate = abs(rand()) % 1000;
	//				if (rRate <= HitRate)
	//					continue;

	//				itEffect.m_bActive = false;
	//			}
	//		}
	//		else //不可能命中
	//		{
	//			for (auto& itEffect : pBuff->BuffEffects)
	//			{
	//				if (ROLE_EFFECT_SKILL_STATUS != itEffect.type)
	//					continue;

	//				itEffect.m_bActive = false;
	//			}
	//		}
	//	}
	//}
#endif

    pBuff->nEffectRate = ParamIn.AttrRuntimeEffectRate;
    pBuff->nGroupId = ParamIn.nGroupId;
    pBuff->nBuffDel = ParamIn.nBuffDel;
    pBuff->nEffectCount = 1;
    pBuff->nDmg = 0;
    pBuff->SerialID = ParamIn.SerialID;
    if (ParamIn.IntervalTimer > 0 && ParamIn.AliveTimer > 0) {
        pBuff->nEffectCount = ParamIn.AliveTimer / ParamIn.IntervalTimer;
    }

    // 初始化buff耐久（护盾buff=护盾值，其他buff=0）
    InitBuffDurability(*pBuff);

    // 確認Buff狀態
    bool SetUpdateEvent = false;	// 是否更新角色數值	
    CheckBuffState(pBuff, ParamIn.nMaxHeap, SetUpdateEvent, ParamIn.nAddHeap);

    // 更新給Client
    this->UpdateBuffToClient(ParamIn.objTarget, pBuff, ParamIn.SerialID, emBuffEvent);

    UpdateAttr(ParamIn.objTarget, true);

    // 需要初始
    if (SetUpdateEvent)
        DoBuffEvent(BUFF_TRIGGER_INIT, *pBuff);

	CheckBuffLock(buffContainer);

    return true;
}

// 確認Buff狀態
void ComSkillBuff::CheckBuffState(SkillBuffInfo* pBuff, int nMaxHeap, bool& SetUpdateEvent, int nAddHeap)
{
    if (pBuff == nullptr)
        return;

    // 可堆的Buff
    SetUpdateEvent = true;
    if (nMaxHeap > 0)
    {
        // 是否還可以疊
		int nHeap = (std::min)(pBuff->HeapCounter + nAddHeap, nMaxHeap);
		pBuff->HeapCounter = nHeap;
    }
    else// 不可堆的Buff
    {
        pBuff->HeapCounter = 1;
    }
}

// 以BuffDataID刪除
bool ComSkillBuff::DelBuff_ByBuffID(int objOwner, int BuffDataID, const NS::I64 SerialID)
{
    auto itr = mapBuffContainer.find(objOwner);
    if (itr == mapBuffContainer.end())
        return false;

    SkillBuffContainer& buffContainer = itr->second;

    // 走訪所有Buff
    ComSkillBuff* pSkillBuff = this;
    bool bChange = false;
	bool bDel = false;
    auto pLambdu = [&bDel, &bChange, &pSkillBuff, BuffDataID, SerialID](SkillBuffInfo& theBuff)
    {
		if (bDel)
			return;

        //if (theBuff.IsEmpty())
        //    return;
        if (theBuff.BuffDataID != BuffDataID)
            return;

		if (0 != SerialID)
		{
			if (SerialID != theBuff.SerialID)
				return;
		}

		bDel = true;

        // 結束Buff
        bChange = true;
        pSkillBuff->DelBuff(theBuff.OwnerMapObjectID, theBuff.Index, 0, false);//此时解锁buff
    };
    buffContainer.ForEachBuff(pLambdu);

    return true;
}

// 刪除一個Buff
bool ComSkillBuff::DelBuff(int objOwner, int BuffIndex, NS::I64 SerialID, bool bUpdate)
{
    auto itr = mapBuffContainer.find(objOwner);
    if (itr == mapBuffContainer.end())
        return false;

    SkillBuffContainer& buffContainer = itr->second;

    // 取得Buff
    SkillBuffInfo* pBuff = buffContainer.GetBuff(BuffIndex);
    if (pBuff == nullptr)
        return false;

    // 結束策略	
    DoBuffEvent(BUFF_TRIGGER_END, *pBuff);

    // 更新給Client
    this->UpdateBuffToClient(objOwner, pBuff, SerialID, BUFF_EVENT_DEL);

    pBuff->Reset(); // 清空

	CheckBuffLock(buffContainer);

    this->UpdateAttr(objOwner, bUpdate);

    return true;
}

bool ComSkillBuff::DelBuff_BySetFlag(int objOwner, int BuffIndex)
{
	auto itr = mapBuffContainer.find(objOwner);
	if (itr == mapBuffContainer.end())
		return false;

	SkillBuffContainer& buffContainer = itr->second;

	// 取得Buff
	SkillBuffInfo* pBuff = buffContainer.GetBuff(BuffIndex);
	if (pBuff == nullptr)
		return false;

	pBuff->AliveTimer = 0; //todo

	return true;
}

// 清空某人的BUFF
bool ComSkillBuff::DelAllBuff(int objOwner, bool bDebuff)
{
    auto itr = mapBuffContainer.find(objOwner);
    if (itr == mapBuffContainer.end())
        return false;

    SkillBuffContainer& buffContainer = itr->second;

    // 走訪所有Buff
    ComSkillBuff* pSkillBuff = this;
    bool bChange = false;
    for (auto& theBuff : buffContainer.GetBuffInfos()) {
        if (theBuff.IsEmpty()) continue;

        if (bDebuff) {
            if (!IsDebuff(theBuff.BuffDataID)) continue;
        }

        // 結束Buff
        bChange = true;
        pSkillBuff->DelBuff(theBuff.OwnerMapObjectID, theBuff.Index, 0, false);
    }

    this->UpdateAttr(objOwner, bChange);

    return true;
}

// 移除一条DEBUFF
bool ComSkillBuff::DelOneDeBuff(int objOwner)
{
    auto itr = mapBuffContainer.find(objOwner);
    if (itr == mapBuffContainer.end())
        return false;

    SkillBuffContainer& buffContainer = itr->second;

    // 走訪所有Buff
    ComSkillBuff* pSkillBuff = this;
    bool bChange = false;
    for (auto& theBuff : buffContainer.GetBuffInfos()) {
        if (bChange) continue;
        if (theBuff.IsEmpty()) continue;
        if (!IsDebuff(theBuff.BuffDataID)) continue;

        // 結束Buff
        pSkillBuff->DelBuff(theBuff.OwnerMapObjectID, theBuff.Index, 0, false);//此处解锁buff
        bChange = true;
        break;
    }

    this->UpdateAttr(objOwner, bChange);

    return true;
}

void ComSkillBuff::DecDurability(const NS::int32 BuffOnwerID, const NS::int32 BuffIdx, const NS::int32 decVal, const bool bSendToClient)
{
	if (decVal <= 0)
		return;

	auto itr = mapBuffContainer.find(BuffOnwerID);
	if (itr == mapBuffContainer.end())
		return;

	SkillBuffContainer& buffContainer = itr->second;

	SkillBuffInfo* pBuff = buffContainer.GetBuff(BuffIdx);
	if (!pBuff)
		return;

	pBuff->nDurability -= decVal;

	//if (pBuff->nDurability > 0)//耐久小于0会成下一次循环发送buff删除消息,此处不用再发多作的更新消息
	{
		if (bSendToClient)
			this->UpdateBuffToClient(pBuff->OwnerMapObjectID, pBuff, pBuff->SerialID, BUFF_EVENT_UPDATE);
	}
}

void ComSkillBuff::DoBuffEvent(BUFF_TRIGGER_EVENT emEvent, SkillBuffInfo& buff, unsigned int time_escaple)
{
    // 是否需要更新
    bool needUpdate = buff.IntervalUpdate(time_escaple);

    // 走訪Buff中的三個作用效果
    int nCount = int(buff.BuffEffects.size());
    for (int EffectIndex = 0; EffectIndex < nCount; ++EffectIndex)
    {
        if (buff.IsEmpty())
            break;
        // 依類型呼叫對應的Function
        switch (emEvent)
        {
        case BUFF_TRIGGER_INIT:
            this->BuffEventBegin(buff, EffectIndex);
            break;
        case BUFF_TRIGGER_UPDATE:
            if (needUpdate)
            {
                this->BuffEventUpdate(buff, EffectIndex);
            }
            break;
        case BUFF_TRIGGER_END:
            this->BuffEventEnd(buff, EffectIndex);
            break;
        case BUFF_TRIGGER_LOCK:
            this->BuffEventLock(buff, EffectIndex);
            break;
        case BUFF_TRIGGER_UNLOCK:
            this->BuffEventUnlock(buff, EffectIndex);
            break;
        }
    }
}

bool ComSkillBuff::GetBuffContainer(int idObj, SkillBuffContainer& buffContainer)
{
    auto itr = mapBuffContainer.find(idObj);
    if (itr == mapBuffContainer.end())
        return false;
    buffContainer = itr->second;
    return true;
}
SkillBuffContainer* ComSkillBuff::FindBuffContainer(int idObj)
{
	auto itr = mapBuffContainer.find(idObj);
	if (itr == mapBuffContainer.end())
		return nullptr;
	return &(itr->second);
}

void ComSkillBuff::RemoveBuffContainer(int objID)
{
    auto itr = mapBuffContainer.find(objID);
    if (itr == mapBuffContainer.end())
    {
        return;
    }
    mapBuffContainer.erase(itr);
}

int ComSkillBuff::SearchReplaceBuffIndex(int BuffDataID, int nGroupId, int nMaxHeap, SkillBuffContainer& buffContainer)
{
    SkillBuffInfo* pReplaceBuff = CheckReplaceBuffExist(nGroupId, buffContainer);
    if (pReplaceBuff != nullptr)
    {
        // 不是同一個Buff
        if (pReplaceBuff->BuffDataID != BuffDataID)
        {
            pReplaceBuff->HeapCounter = 0;
            return pReplaceBuff->Index;
        }

        // 是否為同一個Buff,且不能疊,清空疊的數量
        if (pReplaceBuff->BuffDataID == BuffDataID && nMaxHeap <= 1)
            pReplaceBuff->HeapCounter = 0;

        return pReplaceBuff->Index;
    }
    return -1;
}

int ComSkillBuff::SearchBuffIndex(int CasterMapObjectID, int BuffDataID)
{
    auto it = mapBuffContainer.find(CasterMapObjectID);
    if (it == mapBuffContainer.end())
        return -1;

    return it->second.SearchBuffIndex(CasterMapObjectID, BuffDataID);
}

void ComSkillBuff::CheckBuffControlAnti(const ParamInAddBuff& ParamIn, SkillBuffInfo& refBuff)
{
	NS::int32 CasterControlRate = ParamIn.Caster_CC_HIT;
	NS::int32 TargerControlAnti = ParamIn.Target_CC_RES;

	if (this->IsDebuff(refBuff.BuffDataID) &&
		ParamIn.objCaster != ParamIn.objTarget)
	{
		if (TargerControlAnti > CasterControlRate)
		{
			if (TargerControlAnti < (CasterControlRate + 1000)) //有概率出现抵抗效果控制 效果类型是301的,判断控制命中与抵抗,决定是否生效
			{
				NS::int32 HitRate = (1000 + CasterControlRate - TargerControlAnti);
				for (auto& itEffect : refBuff.BuffEffects)
				{
					if (ROLE_EFFECT_SKILL_STATUS != itEffect.type)
						continue;

					NS::int32 rRate = abs(rand()) % 1000;
					if (rRate <= HitRate)
						continue;

					itEffect.m_bActive = false;
				}
			}
			else //不可能命中
			{
				for (auto& itEffect : refBuff.BuffEffects)
				{
					if (ROLE_EFFECT_SKILL_STATUS != itEffect.type)
						continue;

					itEffect.m_bActive = false;
				}
			}
		}
	}
}

int ComSkillBuff::CountDebuff(int nBuffOnwerID)
{
	auto it = mapBuffContainer.find(nBuffOnwerID);
	if (it == mapBuffContainer.end())
	{
		return 0;
	}

	int nDebuffCount = 0;
	SkillBuffContainer skillBuffContainer = it->second;
	for (SkillBuffInfo buffInfo : skillBuffContainer.GetBuffInfos())
	{
		if (buffInfo.IsEmpty()) { continue; }
		if (!this->IsDebuff(buffInfo.BuffDataID)) { continue; }

		++nDebuffCount;
	}

	return nDebuffCount;
}
