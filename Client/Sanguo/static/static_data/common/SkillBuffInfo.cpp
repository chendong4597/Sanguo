#include "stdafx.h"
#include "ClientSpec.h"
#include "SkillBuffInfo.h"
//using namespace skill;

SkillBuffInfo::SkillBuffInfo()
{
    Reset();
}

void SkillBuffInfo::Reset()
{
    CasterMapObjectID = -1;
    OwnerMapObjectID = -1;
    //pBuffCaster			= nullptr;
    //pBuffOwner			= nullptr;    
    BuffDataID = -1;
    AliveTimer = 0;
    HeapCounter = 0;
    Param = 0;
    m_UpdateTimer = 0;
    nEffectRate = 1000;
    nDurability = 0;
    BuffEffects.clear();
	locked = false;
}

bool SkillBuffInfo::IsEmpty() const
{
    if (BuffEffects.size() <= 0) {
        return true;
    }
    return (BuffDataID <= 0);
}

bool SkillBuffInfo::IsExpired() const
{
    if ((BUFF_DEL_TIME & nBuffDel) == 0)	// 不跟随时间消失
        return false;
    return (AliveTimer <= 0);
}

bool SkillBuffInfo::IsNotDurable() const
{
    if ((BUFF_DEL_DRUABILITY & nBuffDel) == 0)	// 不跟随耐久消失
        return false;
    return nDurability <= 0;
}

bool SkillBuffInfo::IntervalUpdate(unsigned int time_escaple)
{
    AliveTimer -= time_escaple;
    m_UpdateTimer += time_escaple;
    //if (m_UpdateTimer == time_escaple) {	//首次触发
    //	return true;
    //}
    if (m_UpdateTimer < IntervalTimer)
        return false;
    m_UpdateTimer -= IntervalTimer;
    return true;
}

/*
// 取得傳送給Client的訊息
void SkillBuffInfo::GetSendClientMsg(MsgPB::ServerBuffInfo* pBuffInfo) const
{
if (pBuffInfo == nullptr)
return;
pBuffInfo->set_owner_mapobjectid(OwnerMapObjectID);
pBuffInfo->set_index(Index);
pBuffInfo->set_buff_dataid(BuffDataID);
pBuffInfo->set_alive_timer(AliveTimer);
pBuffInfo->set_heap_counter(HeapCounter);
pBuffInfo->set_caster_mapobjectid(CasterMapObjectID);
pBuffInfo->set_effect_rate(nEffectRate);
}
*/