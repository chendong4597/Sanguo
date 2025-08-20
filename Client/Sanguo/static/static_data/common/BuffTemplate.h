#pragma once
#include <string>
//#include "ConfigerDB.h"
//#include "SkillTemplate.h"
#include "EffectEnum.h"

#include <vector>

struct S_BuffTemplate_effect
{
	MsgPB::ROLE_EFFECT_TYPE type; // + BUFF_EFFECT_TYPE		
	std::vector<AttrEffectValue> values;	// 同時作用組別		
};

enum ENUM_BUFF_DEL
{
	BUFF_DEL_NONE = 0,// 不会随着时间消除
	BUFF_DEL_TIME = 1,// 持续时间

	BUFF_DEL_BEHURT = 2,//受到伤害, 状态持有者失去HP时进行移除
	BUFF_DEL_ATK = 4,//发起攻击, 状态持有者对某目标发动普攻或攻击技能时进行移除
	BUFF_DEL_CHGMAP = 8,//更换所在场景, 状态持有者改变所在场景时进行移除
	BUFF_DEL_DRUABILITY = 16,//buff耐久扣完后移除，比如：护盾buff
};

////Buff的樣版
//class S_BuffTemplate
//{
//public:
//	int32 n_ID = 0;
//	std::string s_name = "";	//Buff名稱
//	int32 n_groupid = 0;		//Buff群組
//	int32 n_group_rank = 0;		//Buff群組中的等級
//	int32 n_alive_time = 0;		//持續時間(ms)
//	int32 n_interval_time = 0;	//作用間隔(ms)
//	int32 n_max_heap = 0;		//最大堆疊數
//	int32 n_auto_heap = 0;		//自動疊加
//	int32 n_removebyskill = 0;	//是否可以被技能清除
//	int32 n_bindwithcaster = 0;	//是否綁定施法者
//	int32 n_buff_gob = 0;		//增益1 负面debuff 2 
//
//	std::vector<S_BuffTemplate_effect> n_effects; // Buff效果
//
//	S_BuffTemplate(){}
//	static void BeginLoad(const CXmlTable& table){};
//	static void FinishLoad(){};
//	void Deserialization(const CXmlRecord& record);	
//	LuaObject GetLuaObject();	
//
//
//	CEffectSum m_mapEffect;
//	int32 GetValueByTypeAttr(ROLE_EFFECT_TYPE eType, ATTR_REF eAttr);
//
//	int32 nBuffDel = 0;
//	bool IsCanDel(ENUM_BUFF_DEL eDel);
//};
//
//using BuffConfigDB = TConfigerDB<int32,S_BuffTemplate>;
//using PtrBuffTemplate = shared_ptr<S_BuffTemplate>;