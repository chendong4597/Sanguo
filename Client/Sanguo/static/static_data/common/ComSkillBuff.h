#pragma once

#include <map>
#include "SkillBuffInfo.h"
#include "SkillBuffContainer.h"
#include "BaseTypeDefine.h"


enum BUFF_EVENT
{
    BUFF_EVENT_ADD = 0,
    BUFF_EVENT_UPDATE = 1,
    BUFF_EVENT_DEL = 2,
    BUFF_EVENT_LOCK = 3,
    BUFF_EVENT_UNLOCK = 4,
};

class ComSkillBuff
{
public:
    ComSkillBuff();
    ~ComSkillBuff();
    // 注入相關系統	
    void Update(unsigned int time_escaple);

    // 操作	
	struct ParamInAddBuff
	{
		ParamInAddBuff(const std::vector<S_BuffTemplate_effect>& refv):BuffEffects(refv)
		{}

		int objCaster;
		int objTarget;
		int Caster_CC_HIT;
		int Target_CC_RES;
		int BuffDataID;
		int AliveTimer;
		int IntervalTimer;
		int nGroupId;
		int nMaxHeap;
		const std::vector<S_BuffTemplate_effect>& BuffEffects;
		int nBuffDel;
		long long SerialID;
		int AttrRuntimeEffectRate = 1000;
		int nAddHeap = 1;
	};
	bool AddBuff(const ParamInAddBuff& ParamIn);

#if 1
	bool AddBuff(int objCaster, int objTarget, int BuffDataID, int AliveTimer, int IntervalTimer, int nGroupId, int nMaxHeap,
		const std::vector<S_BuffTemplate_effect>& BuffEffects, int nBuffDel, long long SerialID, int AttrRuntimeEffectRate = 1000, int nAddHeap = 1)
	{
		ParamInAddBuff param(BuffEffects);
		param.objCaster = objCaster;
		param.objTarget = objTarget;
		param.Caster_CC_HIT = 0;
		param.Target_CC_RES = 0;
		param.BuffDataID = BuffDataID;
		param.AliveTimer = AliveTimer;
		param.IntervalTimer = IntervalTimer;
		param.nGroupId = nGroupId;
		param.nMaxHeap = nMaxHeap;

		param.nBuffDel = nBuffDel;
		param.SerialID = SerialID;
		param.AttrRuntimeEffectRate = AttrRuntimeEffectRate;
		param.nAddHeap = nAddHeap;

		return AddBuff(param);
	}
#endif

    bool DelBuff_ByBuffID(int objOwner, int BuffDataID, const NS::I64 SerialID = 0);
    bool DelBuff(int objOwner, int BuffIndex, NS::I64 SerialID, bool bUpdate = true);
	bool DelBuff_BySetFlag(int objOwner, int BuffIndex);
    bool DelAllBuff(int objOwner, bool bDebuff = false);
    bool DelOneDeBuff(int objOwner);

	void DecDurability(const NS::int32 BuffOnwerID, const NS::int32 BuffIdx, const NS::int32 decVal, const bool bSendToClient = false);


    bool GetBuffContainer(int idObj, SkillBuffContainer& buffContainer);
	inline SkillBuffContainer* GetBuffContainerPtr(int idObj) {
		return FindBuffContainer(idObj);
	}
	SkillBuffContainer* FindBuffContainer(int idObj);
    void RemoveBuffContainer(int objID);
    int  SearchReplaceBuffIndex(int BuffDataID, int nGroupId, int nMaxHeap, SkillBuffContainer& buffContainer);

    // 以下是要实现的
    virtual void UpdateAttr(int objOwner, bool bUpdate) {}
    virtual bool IsDebuff(int BuffDataID) { return true; }
    virtual void UpdateBuffToClient(int objOwner, SkillBuffInfo* pBuff, long long SerialID, BUFF_EVENT nEvent) {}
    virtual void BuffEventBegin(SkillBuffInfo& pBuff, int EffectIndex) {}
    virtual void BuffEventUpdate(SkillBuffInfo& pBuff, int EffectIndex) {}
    virtual void BuffEventEnd(SkillBuffInfo& pBuff, int EffectIndex) {}
    virtual void BuffEventLock(SkillBuffInfo& buff, int effectIndex) {};
    virtual void BuffEventUnlock(SkillBuffInfo& buff, int effectIndex) {};
    virtual	SkillBuffInfo* CheckReplaceBuffExist(int nGroupID, SkillBuffContainer& buffContainer) { return nullptr; }
    virtual SkillBuffInfo* CheckExclusionBuffExist(int nGroupID, SkillBuffContainer& buffContainer) { return nullptr; }
    virtual void InitBuffDurability(SkillBuffInfo& pBuff) {};
    // 是否有同组优先级更高的buff运行中
    virtual bool IsSameGroupHighRankBuffActive(NS::int32& BuffIdx_LockBy, const SkillBuffContainer& buffContainer, const SkillBuffInfo& buff) { return false; };
	//// 获取效果命中
	//virtual NS::int32 CB_GetObjBuffControlRate(const NS::int32 MapObj) { return 0; }
	//// 获取效果抵抗
	//virtual NS::int32 CB_GetObjBuffControlAntiRate(const NS::int32 MapObj) { return 0; }

    int SearchBuffIndex(int CasterMapObjectID, int BuffDataID);
	// 统计Debuff个数
	int CountDebuff(int nBuffOnwerID);

protected:
    void CheckBuffState(SkillBuffInfo* pBuff, int nMaxHeap, bool& SetUpdateEvent, int nAddHeap);

	void CheckBuffLock(SkillBuffContainer& buffContainer);

	void CheckBuffControlAnti(const ParamInAddBuff& ParamIn, SkillBuffInfo& pBuff);

	// 添加buff之前调用
	virtual bool CB_AddBuffPreCheck(SkillBuffContainer& buffContainer, const ParamInAddBuff& ParamIn) {
		return true;
	}

protected:
    // Buff的更新事件
    enum BUFF_TRIGGER_EVENT
    {
        BUFF_TRIGGER_INIT,
        BUFF_TRIGGER_UPDATE,
        BUFF_TRIGGER_END,
        BUFF_TRIGGER_LOCK,
        BUFF_TRIGGER_UNLOCK,
    };


    // Buff初始,更新,結束
    void DoBuffEvent(BUFF_TRIGGER_EVENT emEvent, SkillBuffInfo& pBuffInfo, unsigned int time_escaple = 0);

    // 更新的Timer
    unsigned int m_UpdateTimer = 0;

    std::map<int, SkillBuffContainer> mapBuffContainer;

};






