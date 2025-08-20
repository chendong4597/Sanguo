#pragma once
//#include "SkillType.h"
#include "BuffTemplate.h"

#ifdef _CLIENT_MSG_
#	include <msg/MsgBuff.pb.h>
using NS::I64;
#else
#	include "msg/MsgBuff.pb.h"
#endif

#include "BaseTypeDefine.h"


//class ISkillBuffStrategy;

struct S_BuffTemplate_effect_RT : public S_BuffTemplate_effect
{
	S_BuffTemplate_effect_RT(const S_BuffTemplate_effect& ref) {
		this->type = ref.type;
		this->values = ref.values;
	}

	bool m_bActive = true;//该效果是否生效
};

// Buff結構
#define MAX_SKILL_BUFF_SIZE 32
class SkillBuffInfo
{
public:
    int Index = -1;
    int BuffDataID;			// ID
    int AliveTimer;			// 剩餘時間(ms)	
    int HeapCounter;		// 疊了多少個
    int Param;				// 依Buff的類型而有不同的使用方式
    int nBuffDel;			// buff消失类型
    int nEffectCount = 1;	// 效果次数 总时间/间隔时间
    int nDmg = 0;
    int nGroupId = -1;
    int nDurability = 0;    // buff耐久，耐久扣光，buff消失。如果是护盾buff，表示护盾值。其他buff暂时未用到
    NS::I64 SerialID = 0; // 触发buff的技能序列id
    bool locked = false; // 是否冻结

    // 施法者與擁有者資訊
    int						  CasterMapObjectID;	// 施法者
    //	PtrCMapObject			  pBuffCaster;
    int						  OwnerMapObjectID;		// 擁有者
    //	shared_ptr<CMapCharacter> pBuffOwner;	
    //	SkillRoleValue CasterRoleValue; // 施放時的角色屬性
    int						  nEffectRate;		// buff效果系数

    // Buff的作用效果策
    int								IntervalTimer;
    std::vector<S_BuffTemplate_effect_RT>	BuffEffects;

    SkillBuffInfo();
    void Reset();
    bool IsEmpty() const;
    void Update();
    bool IsExpired() const;
    bool IsNotDurable() const;
    bool IntervalUpdate(unsigned int time_escaple);
    //void GetSendClientMsg(MsgPB::ServerBuffInfo* pBuffInfo) const;

private:
    int m_UpdateTimer = 0;

};



